#pragma once
#include "CountdownTimer.h"
#include "Gimmick.h"

// 崩れる床: 乗ると揺れ始め → 落下して消える → しばらくして復活。
class CrumbleFloor : public Gimmick {
public:
	void Initialize(const KamataEngine::Vector3& center);
	void Update(GimmickContext& ctx) override;
	bool GetSolid(SolidBox& out) const override;
	void OnPlayerStand() override;

private:
	enum class State { kIdle, kShaking, kFalling, kHidden };
	static inline const float kShakeTime = 0.6f;
	static inline const float kFallTime = 1.0f;
	static inline const float kRespawnTime = 2.5f;

	State state_ = State::kIdle;
	CountdownTimer timer_;
	float fallSpeed_ = 0.0f;
	float shakePhase_ = 0.0f;
};
