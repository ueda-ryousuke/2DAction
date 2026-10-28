#include "CrusherTrap.h"
#include "GameConfig.h"
#include "ModelLibrary.h"
#include <algorithm>

using namespace KamataEngine;

void CrusherTrap::Initialize(const Vector3& topCenter, float bottomY, float startDelay) {
	topY_ = topCenter.y;
	bottomY_ = bottomY;
	const float half = GameConfig::kBlockSize * 0.5f;
	InitBase(ModelLibrary::Get().Find("crusher"), topCenter, {1, 1, 1}, half, half + 0.05f);
	timer_.Start(startDelay);
}

void CrusherTrap::Update(GimmickContext&) {
	timer_.Update();
	switch (state_) {
	case State::kWaitTop:
		if (timer_.IsFinished()) {
			state_ = State::kSlam;
			fallSpeed_ = 0.0f;
		}
		break;
	case State::kSlam:
		fallSpeed_ = (std::min)(fallSpeed_ + 0.06f, 1.4f);
		pos_.y -= fallSpeed_;
		if (pos_.y <= bottomY_) {
			pos_.y = bottomY_;
			state_ = State::kHoldBottom;
			timer_.Start(kHoldTime);
		}
		break;
	case State::kHoldBottom:
		if (timer_.IsFinished()) {
			state_ = State::kRise;
		}
		break;
	case State::kRise:
		pos_.y += kRiseSpeed;
		if (pos_.y >= topY_) {
			pos_.y = topY_;
			state_ = State::kWaitTop;
			timer_.Start(kWaitTime);
		}
		break;
	}
	UpdateTransform();
}

bool CrusherTrap::GetHazard(Aabb& out, int& damage) const {
	if (state_ != State::kSlam && state_ != State::kHoldBottom) {
		return false; // 上がっている間は安全
	}
	out = Aabb::FromCenter(pos_, halfW_, halfH_);
	damage = kDamage;
	return true;
}
