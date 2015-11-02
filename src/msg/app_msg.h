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

} // namespace cs

#endif
