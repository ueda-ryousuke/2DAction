#pragma once
#include "CountdownTimer.h"
#include "PlayerInput.h"
#include "PlayerMotor.h"

// 二段ジャンプ・壁キックジャンプ・壁ずりの判断をまとめたクラス。
//   地上ジャンプ → 空中ジャンプ(二段目) → 壁キック の優先順位で TryJump が1つだけ実行する。
//   壁キック後は「空中ジャンプ1回」が復活するので、壁キック→二段ジャンプの連携ができる。
class JumpController {
public:
	enum class Action { kNone, kGroundJump, kAirJump, kWallKick };

	// 今フレーム冒頭の接触状態（Player が MapCollider から集める）
	struct Contact {
		bool onGround = false;
		bool wallLeft = false;
		bool wallRight = false;
	};

	void Reset();
	void Update(const Contact& contact); // タイマー進行と状態更新。TryJump の前に毎フレーム呼ぶ
	Action TryJump(const PlayerInputState& input, const Contact& contact, PlayerMotor& motor);
	void ApplyJumpCut(const PlayerInputState& input, PlayerMotor& motor); // ボタンを離したら上昇を弱める

	bool IsWallSliding(const Contact& contact, float moveX, float velocityY) const;
	bool IsControlLocked() const { return controlLock_.IsActive(); }
	void LockControl(float seconds) { controlLock_.Start(seconds); }
	int LastKickDirection() const { return lastKickDirection_; } // 壁キックで飛んだ向き(+1右/-1左)
	int JumpCount() const { return jumpCount_; }

private:
	int jumpCount_ = 0;
	int lastWallSide_ = 0;      // 最後に触れた壁の側 (-1左 / +1右)
	int lastKickDirection_ = 0;
	bool cuttable_ = false;     // 小ジャンプ(ジャンプカット)を適用できる状態
	CountdownTimer groundCoyote_;
	CountdownTimer wallCoyote_;
	CountdownTimer jumpBuffer_;
	CountdownTimer controlLock_;
};
