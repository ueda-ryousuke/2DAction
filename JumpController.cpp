#include "JumpController.h"
#include "PlayerParams.h"

using namespace PlayerParams;

void JumpController::Reset() {
	jumpCount_ = 0;
	lastWallSide_ = 0;
	lastKickDirection_ = 0;
	cuttable_ = false;
	groundCoyote_.Stop();
	wallCoyote_.Stop();
	jumpBuffer_.Stop();
	controlLock_.Stop();
}

void JumpController::Update(const Contact& c) {
	groundCoyote_.Update();
	wallCoyote_.Update();
	jumpBuffer_.Update();
	controlLock_.Update();

	if (c.onGround) {
		jumpCount_ = 0;
		cuttable_ = false;
		groundCoyote_.Start(kGroundCoyoteTime);
	} else if (groundCoyote_.IsFinished() && jumpCount_ == 0) {
		jumpCount_ = 1; // 崖から歩き落ちた場合は「地上ジャンプ分」を消費したことにする
	}

	if (!c.onGround && (c.wallLeft || c.wallRight)) {
		if (c.wallLeft != c.wallRight) {
			lastWallSide_ = c.wallLeft ? -1 : +1;
		} else if (lastWallSide_ == 0) {
			lastWallSide_ = -1;
		}
		wallCoyote_.Start(kWallCoyoteTime);
	}
}

JumpController::Action JumpController::TryJump(const PlayerInputState& input, const Contact& c, PlayerMotor& motor) {
	if (input.jumpPressed) {
		jumpBuffer_.Start(kJumpBufferTime);
	}
	if (jumpBuffer_.IsFinished()) {
		return Action::kNone;
	}

	// ① 地上ジャンプ（コヨーテタイム込み）
	if (c.onGround || groundCoyote_.IsActive()) {
		motor.SetVelocityY(kJumpAcceleration);
		jumpCount_ = 1;
		cuttable_ = true;
		groundCoyote_.Stop();
		jumpBuffer_.Stop();
		return Action::kGroundJump;
	}

	// ② 壁キック: 壁と反対方向へ跳ぶ。以降しばらく左右入力を無視して勢いを保つ
	if (wallCoyote_.IsActive() && lastWallSide_ != 0) {
		lastKickDirection_ = -lastWallSide_;
		motor.SetVelocity(static_cast<float>(lastKickDirection_) * kWallKickVelocityX, kWallKickVelocityY);
		jumpCount_ = 1; // 空中ジャンプが1回残る
		cuttable_ = true;
		controlLock_.Start(kWallKickLockTime);
		wallCoyote_.Stop();
		jumpBuffer_.Stop();
		return Action::kWallKick;
	}

	// ③ 空中ジャンプ（二段目）
	if (jumpCount_ < kMaxJumpCount) {
		motor.SetVelocityY(kJumpAcceleration);
		++jumpCount_;
		cuttable_ = false; // 二段目は常にフル高さ
		jumpBuffer_.Stop();
		return Action::kAirJump;
	}
	return Action::kNone;
}

void JumpController::ApplyJumpCut(const PlayerInputState& input, PlayerMotor& motor) {
	if (!cuttable_) {
		return;
	}
	if (motor.Velocity().y <= 0.0f) {
		cuttable_ = false;
	} else if (!input.jumpHeld && motor.Velocity().y > kJumpCutThreshold) {
		motor.ScaleVelocityY(kJumpCutRate);
		cuttable_ = false;
	}
}

bool JumpController::IsWallSliding(const Contact& c, float moveX, float velocityY) const {
	if (c.onGround || velocityY >= 0.0f) {
		return false;
	}
	return (c.wallLeft && moveX < 0.0f) || (c.wallRight && moveX > 0.0f); // 壁に向かって押しているときだけ張り付く
}
