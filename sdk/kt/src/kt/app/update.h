#ifndef KT_APP_UPDATE_H_
#define KT_APP_UPDATE_H_

#include <cstdint>
#include <vector>
#include "control_defs.h"
#include "update_defs.h"

namespace kt {
class Cns;
class UpdateService;

/**
 * @class kt::UpdateClient
 * @brief Convenience class for anything that wants to get update notices.
 */
class UpdateClient {
public:
	// Priority determines when this client runs during the update. The lower
	// the priority, the earlier in the update.
	UpdateClient(Cns&, const kt::priority = kt::STANDARD_PRIORITY);
	virtual ~UpdateClient();

	kt::priority				getPriority() const { return mPriority; }

	virtual void				update(const kt::UpdateParams&) = 0;

private:
	UpdateClient();
	UpdateService&				mService;
	const kt::priority			mPriority;
};

/**
 * @class kt::UpdateList
 * @brief Store all the update clients.
 */
class UpdateList {
public:
	UpdateList() { }

	void						addClient(UpdateClient*);
	void						removeClient(UpdateClient*);

	void						update(const UpdateParams&);

private:
	std::vector<UpdateClient*>	mClients;
};

} // namespace kt

#endif
