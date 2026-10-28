#pragma once
#include "EnemyBase.h"

// 分裂する敵(スライム): 倒すと小さい2体に分かれる。世代が進むほど小さく・速く・弱くなる。
class SplitEnemy : public EnemyBase {
public:
	void SetGeneration(int g) { generation_ = g; } // Initialize の前に呼ぶ
	void Update(EnemyContext& ctx) override;
	void OnDeath(EnemyContext& ctx) override;
	int ScoreValue() const override { return 100; }

	static inline const int kMaxGeneration = 2;

protected:
	void OnInitialize() override;

private:
	static inline const float kWalkSpeed = 0.035f;
	static inline const float kHopPower = 0.32f;
	static inline const float kHopInterval = 1.6f;

	int generation_ = 0;
	CountdownTimer hopTimer_;
};
