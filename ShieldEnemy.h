#pragma once
#include "EnemyBase.h"

// 盾持ちの敵: 正面からの攻撃（弾）を防ぐ。背後からの弾や、上から踏みつけるのは有効。
class ShieldEnemy : public EnemyBase {
public:
	void Update(EnemyContext& ctx) override;
	bool TakeDamage(int damage, float attackDirX) override;
	int ScoreValue() const override { return 300; }

protected:
	void OnInitialize() override;

private:
	static inline const float kWalkSpeed = 0.035f;
	static inline const float kChaseRange = 20.0f;
};
