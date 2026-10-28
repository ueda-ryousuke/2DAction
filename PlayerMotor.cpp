#include "PlayerMotor.h"
#include "PlayerParams.h"
#include <algorithm>
#include <cmath>

using namespace PlayerParams;

void PlayerMotor::ApplyHorizontal(float moveX, bool onGround, SurfaceType surface, bool locked) {
	if (locked) {
		return; // 壁キックの勢いなどをそのまま維持
	}
	const bool slippery = onGround && surface == SurfaceType::kSlippery;

	if (moveX != 0.0f) {
		float accel = slippery ? kAcceleration * kSlipperyAccelScale : kAcceleration;
		velocity_.x += moveX * accel;
		velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		if (!slippery) {
			velocity_.x *= (1.0f - kAttenuation);
		}
	} else {
		float rate = !onGround ? kAirStopRate : (slippery ? kSlipperyStopRate : kGroundStopRate);
		velocity_.x *= rate;
		if (std::fabs(velocity_.x) < 0.001f) {
			velocity_.x = 0.0f;
		}
	}
}

void PlayerMotor::ApplyGravity(bool wallSliding) {
	velocity_.y -= kGravityAcceleration;
	float limit = wallSliding ? kWallSlideSpeed : kLimitFallSpeed;
	velocity_.y = (std::max)(velocity_.y, -limit);
}
