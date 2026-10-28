#pragma once
#include "CountdownTimer.h"
#include "Gimmick.h"

// 落下する天井: 真下にプレイヤーが来ると、揺れて警告 → 一気に落下 → しばらく居座って → 元の位置へ戻る。
// 落下中はダメージ判定。止まっている間は上に乗れる（SolidBox）。
class FallingCeiling : public Gimmick {
public:
	void Initialize(const KamataEngine::Vector3& homeCenter, float restY);
	void Update(GimmickContext& ctx) override;
	bool GetSolid(SolidBox& out) const override;
	bool GetHazard(Aabb& out, int& damage) const override;

private:
	enum class State { kIdle, kWarning, kFalling, kRest, kRising };
	static inline const float kWarnTime = 0.5f;
	static inline const float kRestTime = 2.0f;
	static inline const float kTriggerHalfWidth = 2.5f;
	static inline const float kTriggerHeight = 14.0f;
	static inline const int kDamage = 2;

	State state_ = State::kIdle;
	KamataEngine::Vector3 home_{};
	float restY_ = 0.0f;
	float fallSpeed_ = 0.0f;
	float phase_ = 0.0f;
	CountdownTimer timer_;
};
