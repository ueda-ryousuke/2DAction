#pragma once
#include "EnemyBase.h"

// 飛行する敵: 重力を無視してふわふわ漂い、近づくとプレイヤーへ寄ってくる。
class FlyingEnemy : public EnemyBase {
public:
	void Update(EnemyContext& ctx) override;
	int ScoreValue() const override { return 150; }

protected:
	void OnInitialize() override;

private:
	static inline const float kChaseRange = 24.0f;
	static inline const float kChaseSpeed = 0.06f;
	static inline const float kBobAmplitude = 1.4f;

	float baseY_ = 0.0f;
	float phase_ = 0.0f;
};
