#pragma once
#include "CountdownTimer.h"
#include "Gimmick.h"

// 押しつぶしトラップ: 上で待機 → 叩きつける → 下で一瞬止まる → ゆっくり戻る、を繰り返す。
class CrusherTrap : public Gimmick {
public:
	// topCenter: 上限位置の中心 / bottomY: 下限位置の中心Y / startDelay: 最初の待機（複数並べるとき時間差を付ける）
	void Initialize(const KamataEngine::Vector3& topCenter, float bottomY, float startDelay = 1.0f);
	void Update(GimmickContext& ctx) override;
	bool GetHazard(Aabb& out, int& damage) const override;

private:
	enum class State { kWaitTop, kSlam, kHoldBottom, kRise };
	static inline const float kWaitTime = 1.5f;
	static inline const float kHoldTime = 0.7f;
	static inline const float kRiseSpeed = 0.12f;
	static inline const int kDamage = 3;

	State state_ = State::kWaitTop;
	float topY_ = 0.0f;
	float bottomY_ = 0.0f;
	float fallSpeed_ = 0.0f;
	CountdownTimer timer_;
};
