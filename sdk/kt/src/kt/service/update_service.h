#ifndef KT_SERVICE_UPDATESERVICE_H_
#define KT_SERVICE_UPDATESERVICE_H_

#include <string>
#include <kt/app/service.h>

namespace kt {
class UpdateClient;
class UpdateList;

/**
 * @class kt::UpdateService
 * @brief A service with an UpdateList (a pass through to the main
 * app update). Clients would never use this class directly, access
 * is gained through an UpdateClient.
 */
class UpdateService : public Service {
public:
	static const std::string&	NAME();
	UpdateService(UpdateList&);

	void						addClient(UpdateClient*);
	void						removeClient(UpdateClient*);

private:
	UpdateList&					mAppUpdateList;
};

} // namespace kt

#endif
