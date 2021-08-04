/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "modules/audio_processing/agc2/rnn_vad/pitch_search.h"

#include <algorithm>
#include <vector>

#include "modules/audio_processing/agc2/rnn_vad/pitch_search_internal.h"
#include "modules/audio_processing/agc2/rnn_vad/test_utils.h"
// TODO(bugs.webrtc.org/8948): Add when the issue is fixed.
// #include "test/fpe_observer.h"
#include "test/gtest.h"

namespace webrtc {
namespace rnn_vad {

// Checks that the computed pitch period is bit-exact and that the computed
// pitch gain is within tolerance given test input data.
TEST(RnnVadTest, PitchSearchWithinTolerance) {
  auto lp_residual_reader = test::CreateLpResidualAndPitchPeriodGainReader();
  const int num_frames = std::min(lp_residual_reader.second, 300);  // Max 3 s.
  std::vector<float> lp_residual(kBufSize24kHz);
  float expected_pitch_period, expected_pitch_strength;
  PitchEstimator pitch_estimator;
  {
    // TODO(bugs.webrtc.org/8948): Add when the issue is fixed.
    // FloatingPointExceptionObserver fpe_observer;
    for (int i = 0; i < num_frames; ++i) {
      SCOPED_TRACE(i);
      lp_residual_reader.first->ReadChunk(lp_residual);
      lp_residual_reader.first->ReadValue(&expected_pitch_period);
      lp_residual_reader.first->ReadValue(&expected_pitch_strength);
      int pitch_period =
          pitch_estimator.Estimate({lp_residual.data(), kBufSize24kHz});
      EXPECT_EQ(expected_pitch_period, pitch_period);
      EXPECT_NEAR(expected_pitch_strength,
                  pitch_estimator.GetLastPitchStrengthForTesting(), 15e-6f);
    }
  }
}

}  // namespace rnn_vad
}  // namespace webrtc
