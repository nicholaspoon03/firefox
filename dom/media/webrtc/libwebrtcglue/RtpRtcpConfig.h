/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __RTPRTCP_CONFIG_H__
#define __RTPRTCP_CONFIG_H__
#include "api/rtp_headers.h"

namespace mozilla {
class RtpRtcpConfig {
 public:
  RtpRtcpConfig() = delete;
  explicit RtpRtcpConfig(const webrtc::RtcpMode aMode,
                         const bool aExtmapAllowMixed)
      : mRtcpMode(aMode), mExtmapAllowMixed(aExtmapAllowMixed) {}
  webrtc::RtcpMode GetRtcpMode() const { return mRtcpMode; }
  bool GetExtmapAllowMixed() const { return mExtmapAllowMixed; }

  bool operator==(const RtpRtcpConfig& aOther) const {
    return mRtcpMode == aOther.mRtcpMode &&
           mExtmapAllowMixed == aOther.mExtmapAllowMixed;
  }

 private:
  webrtc::RtcpMode mRtcpMode;
  bool mExtmapAllowMixed;
};
}  // namespace mozilla
#endif
