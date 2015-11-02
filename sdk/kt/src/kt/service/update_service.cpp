#include "update_service.h"

#include "kt/app/update.h"

namespace kt {

/**
 * @class kt::UpdateService
 */
const std::string& UpdateService::NAME() {
	static const std::string	N("kt/update");
	return N;
}

UpdateService::UpdateService(UpdateList &list)
		: mAppUpdateList(list) {
}

void UpdateService::addClient(UpdateClient *c) {
	mAppUpdateList.addClient(c);
}

void UpdateService::removeClient(UpdateClient *c) {
	mAppUpdateList.removeClient(c);
}

} // namespace kt
