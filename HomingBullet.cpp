#include "HomingBullet.h"
#include "MathUtil.h"
#include <algorithm>
#include <cmath>

using namespace KamataEngine;

void HomingBullet::Update(const ITargetProvider* targets) {
	Vector3 target;
	if (targets && targets->FindNearestTarget(position_, kSeekRange, target)) {
		Vector3 want = MathUtil::NormalizeXY(MathUtil::Sub(target, position_));
		float speed = MathUtil::LengthXY(velocity_);
		float cur = std::atan2(velocity_.y, velocity_.x);
		float aim = std::atan2(want.y, want.x);
		float diff = aim - cur;
		while (diff > MathUtil::kPi) diff -= 2.0f * MathUtil::kPi; // -π..π に収める
		while (diff < -MathUtil::kPi) diff += 2.0f * MathUtil::kPi;
		diff = std::clamp(diff, -kMaxTurnPerFrame, kMaxTurnPerFrame);
		velocity_ = {std::cos(cur + diff) * speed, std::sin(cur + diff) * speed, 0.0f};
	}
	Integrate();
}
