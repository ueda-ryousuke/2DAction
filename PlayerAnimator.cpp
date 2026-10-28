#include "PlayerAnimator.h"
#include "GameConfig.h"
#include "MathUtil.h"
#include "PlayerParams.h"

using namespace KamataEngine;

namespace {
float TargetYaw(LRDirection d) { return d == LRDirection::kRight ? MathUtil::kPi * 0.5f : MathUtil::kPi * 1.5f; }
} // namespace

void PlayerAnimator::Initialize(float baseScale) {
	baseScale_ = baseScale;
	facing_ = LRDirection::kRight;
	yaw_ = TargetYaw(facing_);
	turnTimer_ = 0.0f;
	squash_ = 0.0f;
}

void PlayerAnimator::FaceTo(LRDirection dir, float currentRotationY) {
	if (dir == facing_) {
		return;
	}
	facing_ = dir;
	turnFirstRotationY_ = currentRotationY;
	turnTimer_ = PlayerParams::kTimeTurn;
}

void PlayerAnimator::Update() {
	if (turnTimer_ > 0.0f) {
		turnTimer_ = (std::max)(0.0f, turnTimer_ - GameConfig::kDeltaTime);
		float t = MathUtil::EaseInOut(1.0f - turnTimer_ / PlayerParams::kTimeTurn);
		yaw_ = MathUtil::Lerp(turnFirstRotationY_, TargetYaw(facing_), t);
	} else {
		yaw_ = TargetYaw(facing_);
	}
	squash_ = MathUtil::Lerp(squash_, 0.0f, 0.2f); // ゆっくり元に戻る
}

void PlayerAnimator::Apply(WorldTransform& wt) const {
	wt.rotation_.y = yaw_;
	wt.scale_ = {baseScale_ * (1.0f - squash_ * 0.5f), baseScale_ * (1.0f + squash_), baseScale_ * (1.0f - squash_ * 0.5f)};
}
