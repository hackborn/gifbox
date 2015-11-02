#include "directory_watcher_service.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#endif
#include <iostream>
#include <mutex>
#include <Windows.h>
#include <kt/dbg/dbg_defs.h>
//#include <vr/dbg/logger.h>

static VOID CALLBACK empty_func(ULONG_PTR dwParam)	{ }

namespace kt {
namespace {
// OK It's horrible placing this as a global, but there can only ever be a single
// watcher thread going, and I don't want to clutter the API with platform references.
std::mutex			RUN_LOCK;
std::mutex			WAKEUP_LOCK;
static HANDLE		WAKEUP = INVALID_HANDLE_VALUE;

void				setWakeup(HANDLE h) {
	std::lock_guard<std::mutex> lock(WAKEUP_LOCK);
	WAKEUP = h;
}

void				signalWakeup() {
	std::lock_guard<std::mutex> lock(WAKEUP_LOCK);
	if (WAKEUP != INVALID_HANDLE_VALUE) SetEvent(WAKEUP);
}

}

/**
 * @class kt::DirectoryWatcherService
 */
void DirectoryWatcherService::wakeup() {
	signalWakeup();
}

/**
 * @class kt::DirectoryWatcherService::Waiter
 */
void DirectoryWatcherService::Waiter::run() {
	// Prevent clients from running multiple directory watchers.
	// Which wouldn't be necessary if I'd put that handler into local data.
	std::lock_guard<std::mutex> lock(RUN_LOCK);
//	XXX Well... so actually, I can't print anything here or else the printing to the
//	command prompt is broken and nothing in the app prints at all. So yeah, now I really
//	want a logger that gets that print info to another thread.
//	VR_LOG_INFO("Start directory watcher");
//	DBG_LINE(std::cout << "Start directory watcher" << std::endl);
	DWORD				ans;
	HANDLE*				handle;
	DWORD				notify = FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME
								 | FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE
								 | FILE_NOTIFY_CHANGE_LAST_WRITE;
	size_t				k, count = mPath.size();
	if (count < 1) return;
	// Insert my own handle I can wakeup
	++count;
	if ((handle = new HANDLE[count]) == 0) return;

	handle[0] = CreateEventW(0, FALSE, FALSE, 0);
	setWakeup(handle[0]);

	for (k = 1; k < count; k++) {
		handle[k] = FindFirstChangeNotificationA(mPath[k-1].c_str(), true, notify);
	}
	if (handle[0] == INVALID_HANDLE_VALUE) goto cleanup;
	for (k = 1; k < count; k++) {
		if (handle[k] == INVALID_HANDLE_VALUE) {
//			VR_LOG_INFO("ERROR DirectoryWatcherOp invalid handle on " << mPath[k-1]);
//			DBG_LINE(std::cout << "ERROR DirectoryWatcherOp invalid handle on " << mPath[k-1] << std::endl);
			goto cleanup;
		}
	}
//	VR_LOG_INFO("Start DirectoryWatcher loop");
//	DBG_LINE(std::cout << "Start DirectoryWatcher loop" << std::endl);
	while (TRUE) { 
		// Wait for notification.
		ans = WaitForMultipleObjectsEx(count, handle, FALSE, INFINITE, TRUE);

		if (isStopped()) goto cleanup;
		if (ans < WAIT_OBJECT_0 || ans >= WAIT_OBJECT_0 + count) goto cleanup;
		ans -= WAIT_OBJECT_0;

		if (ans >= 1 && ans < count) {
//			cout << "CHANGED=" << mPath[ans-1] << endl;
			if (!onChanged(mPath[ans-1])) goto cleanup;
		}

		if (FindNextChangeNotification(handle[ans]) == FALSE) goto cleanup;
	}

cleanup:
//	VR_LOG_INFO("End DirectoryWatcher");
//	DBG_LINE(std::cout << "End DirectoryWatcher" << std::endl);
	setWakeup(INVALID_HANDLE_VALUE);
	if (handle[0] != INVALID_HANDLE_VALUE) {
		CloseHandle(handle[0]);
	}
	for (k = 1; k < count; k++) {
		if (handle[k] != INVALID_HANDLE_VALUE)
			FindCloseChangeNotification(handle[k]);
	}
	delete[] handle;
}

} // namespace kt
