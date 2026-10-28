#include "CrumbleFloor.h"
#include "GameConfig.h"
#include "ModelLibrary.h"
#include "SePlayer.h"
#include <cmath>

using namespace KamataEngine;

void CrumbleFloor::Initialize(const Vector3& center) {
	const float half = GameConfig::kBlockSize * 0.5f;
	InitBase(ModelLibrary::Get().Find("block_crumble"), center, {1, 1, 1}, half, half);
}

void CrumbleFloor::OnPlayerStand() {
	if (state_ == State::kIdle) {
		state_ = State::kShaking;
		timer_.Start(kShakeTime);
	}
}

void CrumbleFloor::Update(GimmickContext&) {
	timer_.Update();
	switch (state_) {
	case State::kIdle:
		break;
	case State::kShaking:
		shakePhase_ += 1.0f;
		drawOffset_.x = std::sin(shakePhase_ * 1.8f) * 0.12f;
		if (timer_.IsFinished()) {
			state_ = State::kFalling;
			SePlayer::Get().Play(SeId::kCrumble); // 床が割れて落ちる瞬間
			fallSpeed_ = 0.0f;
			timer_.Start(kFallTime);
		}
		break;
	case State::kFalling:
		fallSpeed_ += 0.02f;
		drawOffset_.y -= fallSpeed_;
		if (timer_.IsFinished()) {
			state_ = State::kHidden;
			visible_ = false;
			timer_.Start(kRespawnTime);
		}
		break;
	case State::kHidden:
		if (timer_.IsFinished()) {
			state_ = State::kIdle;
			visible_ = true;
			drawOffset_ = {};
		}
		break;
	}
	UpdateTransform();
}

bool CrumbleFloor::GetSolid(SolidBox& out) const {
	if (state_ == State::kFalling || state_ == State::kHidden) {
		return false;
	}
	out.box = Aabb::FromCenter(pos_, halfW_, halfH_);
	return true;
}