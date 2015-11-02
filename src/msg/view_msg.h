#ifndef CS_MSG_VIEWMSG_H_
#define CS_MSG_VIEWMSG_H_

#include <kt/service/msg_service.h>

namespace cs {

/**
 * @class cs::MediaChangedMsg
 * @brief Notification that a media view has replaced its content.
 */
struct MediaChangedMsg : public kt::msg::RegisteredMsg<MediaChangedMsg> { };

} // namespace cs

#endif
