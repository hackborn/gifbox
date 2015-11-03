#ifndef CS_MSG_APPMSG_H_
#define CS_MSG_APPMSG_H_

#include <kt/service/msg_service.h>
#include "app/status.h"

namespace cs {

/**
 * @class cs::StatusMsg
 * @brief An app status.
 */
class StatusMsg : public kt::msg::RegisteredMsg<StatusMsg> {
public:
	StatusMsg(const Status &s) : mStatus(s) { }
	const Status&			mStatus;
};

/**
 * @class cs::SetMediaPathMsg
 * @brief Command to set the current media path.
 * @description This is temporary, until navigation is worked out.
 */
class SetMediaPathMsg : public kt::msg::RegisteredMsg<SetMediaPathMsg> {
public:
	SetMediaPathMsg(const std::string &s) : mPath(s) { }
	const std::string&		mPath;
};

} // namespace cs

#endif
