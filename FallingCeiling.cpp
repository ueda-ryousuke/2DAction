#include "FallingCeiling.h"
#include "GameConfig.h"
#include "ModelLibrary.h"
#include <algorithm>
#include <cmath>

using namespace KamataEngine;

void FallingCeiling::Initialize(const Vector3& homeCenter, float restY) {
	home_ = homeCenter;
	restY_ = restY;
	const float half = GameConfig::kBlockSize * 0.5f;
	InitBase(ModelLibrary::Get().Find("ceiling_spike"), homeCenter, {1, 1, 1}, half * 0.9f, half);
}

void FallingCeiling::Update(GimmickContext& ctx) {
	timer_.Update();
	switch (state_) {
	case State::kIdle: {
		float dx = std::fabs(ctx.playerPos.x - pos_.x);
		float dy = pos_.y - ctx.playerPos.y;
		if (dx < kTriggerHalfWidth && dy > 0.0f && dy < kTriggerHeight) {
			state_ = State::kWarning;
			timer_.Start(kWarnTime);
		}
		break;
	}
	case State::kWarning:
		phase_ += 1.0f;
		drawOffset_.x = std::sin(phase_ * 2.2f) * 0.1f;
		if (timer_.IsFinished()) {
			drawOffset_ = {};
			state_ = State::kFalling;
			fallSpeed_ = 0.0f;
		}
		break;
	case State::kFalling:
		fallSpeed_ = (std::min)(fallSpeed_ + 0.04f, 1.2f);
		pos_.y -= fallSpeed_;
		if (pos_.y <= restY_) {
			pos_.y = restY_;
			state_ = State::kRest;
			timer_.Start(kRestTime);
		}
		break;
	case State::kRest:
		if (timer_.IsFinished()) {
			state_ = State::kRising;
		}
		break;
	case State::kRising:
		pos_.y += 0.08f;
		if (pos_.y >= home_.y) {
			pos_.y = home_.y;
			state_ = State::kIdle;
		}
		break;
	}
	UpdateTransform();
}

bool FallingCeiling::GetSolid(SolidBox& out) const {
	if (state_ != State::kRest) {
		return false;
	}
	out.box = Aabb::FromCenter(pos_, halfW_, halfH_);
	return true;
}

bool FallingCeiling::GetHazard(Aabb& out, int& damage) const {
	if (state_ != State::kFalling) {
		return false;
	}
	out = Aabb::FromCenter(pos_, halfW_, halfH_);
	damage = kDamage;
	return true;
}
