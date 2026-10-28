#pragma once
#include "Gimmick.h"

// 左から迫ってくる壁。触れたら即ゲームオーバー級のダメージ。
// プレイヤーが triggerX を越えたら動き出し、少しずつ加速する。
class ChasingWall : public Gimmick {
public:
	void Initialize(const KamataEngine::Vector3& center, float triggerX, float startSpeed = 0.05f, float maxSpeed = 0.11f);
	void Update(GimmickContext& ctx) override;
	bool GetHazard(Aabb& out, int& damage) const override;
	float CullRadius() const override { return 30.0f; }

private:
	static inline const float kAcceleration = 0.00006f;
	static inline const int kDamage = 99;
	float triggerX_ = 0.0f;
	float speed_ = 0.05f;
	float maxSpeed_ = 0.11f;
	bool active_ = false;
};
