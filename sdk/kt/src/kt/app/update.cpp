#include "update.h"

#include <algorithm>
#include <kt/app/cns.h>
#include <kt/service/update_service.h>

namespace kt {

/**
 * @class kt::UpdateClient
 */
UpdateClient::UpdateClient(Cns &cns, const kt::priority p)
		: mService(cns.getService<UpdateService>(UpdateService::NAME()))
		, mPriority(p) {
	mService.addClient(this);
}

UpdateClient::~UpdateClient() {
	mService.removeClient(this);
}

/**
 * @class kt::UpdateList
 */
void UpdateList::addClient(UpdateClient *c) {
	if (!c) return;

	try {
		mClients.push_back(c);
		auto sort_fn([](const UpdateClient *a, const UpdateClient *b) -> bool {
			return a->getPriority() < b->getPriority();
		});
		std::sort(mClients.begin(), mClients.end(), sort_fn);
	} catch (std::exception const&) {
	}
}

void UpdateList::removeClient(UpdateClient *c) {
	if (!c) return;

	try {
		mClients.erase(std::remove(mClients.begin(), mClients.end(), c), mClients.end());
	} catch (std::exception const&) {
	}
}

void UpdateList::update(const UpdateParams &p) {
	if (mClients.empty()) return;

	for (auto it : mClients) {
		it->update(p);
	}
}

} // namespace kt