#include "status_view.h"

#include <cinder/gl/draw.h>
#include <cinder/gl/gl.h>
#include <cinder/gl/scoped.h>
#include "msg/app_msg.h"

namespace cs {

/**
 * @class cs::StatusView
 */
StatusView::StatusView(kt::Cns &cns)
		: base(cns)
		, mMsgClient(cns)
		, mLabelView(kt::view::Label::make(*this, cns, "app:status")) {
	mMsgClient.add<StatusMsg>([this](const StatusMsg &m){onStatusMsg(m);});
}

void StatusView::onUpdate(const kt::UpdateParams&) {
	if (mTimerActive) {
		if (mStatus.empty()) {
			mTimerActive = false;
		} else if (mTimer.elapsed() >= mStatus.back().mDuration) {
			mTimerActive = false;
			mStatus.pop_back();
			statusChanged();
		}
	}
}

void StatusView::onStatusMsg(const StatusMsg &m) {
	if (m.mStatus.mDuration == Status::Duration::kEnd && !mStatus.empty()) {
		// If we aren't popping off the current item, then don't disrupt the current timer.
		if (mStatus.back().mStatus.mId != m.mStatus.mId) {
			removeStatusId(m.mStatus.mId);
		} else {
			removeStatusId(m.mStatus.mId);
			statusChanged();
		}
	} else if (!m.mStatus.mMessage.empty()) {
		double		duration = std::numeric_limits<double>::max();
		if (m.mStatus.mDuration == Status::Duration::kOneShot) duration = 3.0;
		mStatus.push_back(TimedStatus(m.mStatus, duration));
		statusChanged();
	}
}

void StatusView::statusChanged() {
	if (mStatus.empty()) {
		setHidden(true);
		mTimerActive = false;
	} else {
		setHidden(false);
		mLabelView.setText(mStatus.back().mStatus.mMessage);
		if (mStatus.back().mStatus.mType == Status::Type::kError) {
			mLabelView.setColor(ci::ColorA(0.8f, 0.11f, 0.11f, 1.0f));
		} else {
			mLabelView.setColor(ci::ColorA(1.0f, 1.0f, 1.0f, 1.0f));
		}
		setSize(mLabelView.getSize());
		mTimerActive = true;
		mTimer.start();
	}
}

void StatusView::removeStatusId(const uint32_t id) {
	mStatus.erase(std::remove_if(mStatus.begin(), mStatus.end(), [id](const TimedStatus &ts)->bool{
		return ts.mStatus.mId == id;
	}), mStatus.end());
}

} // namespace cs
