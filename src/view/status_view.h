#ifndef APP_VIEW_STATUSVIEW_H_
#define APP_VIEW_STATUSVIEW_H_

#include <kt/service/msg_service.h>
#include <kt/time/seconds.h>
#include <kt/view/label_view.h>
#include "app/status.h"

namespace cs {
class StatusMsg;

/**
 * @class cs::StatusView
 * @brief Display the status message.
 */
class StatusView : public kt::view::View {
public:
	StatusView() = delete;
	StatusView(const StatusView&) = delete;
	StatusView(kt::Cns&);

protected:
	void						onUpdate(const kt::UpdateParams&) override;
//	void						onDraw(const kt::view::DrawParams&) override;

private:
	void						onStatusMsg(const StatusMsg&);
	void						statusChanged();
	void						removeStatusId(const uint32_t);

	using base = kt::view::View;
	kt::msg::Client				mMsgClient;

	kt::view::Label&			mLabelView;

	// Keep a stack of status
	struct TimedStatus {
		TimedStatus() { }
		TimedStatus(const Status &s, const double d) : mStatus(s), mDuration(d) { }
		Status mStatus; double mDuration = 0.0;
	};
	std::vector<TimedStatus>	mStatus;

	kt::time::Seconds			mTimer;
	bool						mTimerActive = false;
};

} // namespace cs

#endif
