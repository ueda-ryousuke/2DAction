#pragma once
#include "EnemyBase.h"

// 遠距離攻撃の敵: 普段は足場の端まで往復。プレイヤーが射程に入ると立ち止まって狙い撃ちする。
class ShooterEnemy : public EnemyBase {
public:
	void Update(EnemyContext& ctx) override;
	int ScoreValue() const override { return 200; }

protected:
	void OnInitialize() override;

private:
	static inline const float kPatrolSpeed = 0.03f;
	static inline const float kSightRangeX = 18.0f;
	static inline const float kSightRangeY = 5.0f;
	static inline const float kFireInterval = 2.0f;
	static inline const float kBulletSpeed = 0.28f;

	CountdownTimer fireTimer_;
};
