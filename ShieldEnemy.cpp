#include "ShieldEnemy.h"
#include <cmath>

using namespace KamataEngine;

void ShieldEnemy::OnInitialize() {
	SetBody(3, 0.75f, 1.0f);
	SetVisual(1.3f, -0.75f);
}

void ShieldEnemy::Update(EnemyContext& ctx) {
	const float dx = ctx.playerPos.x - pos_.x;
	if (std::fabs(dx) < kChaseRange) {
		Face(dx);
		if (wallHit_ || (onGround_ && IsLedgeAhead(facing_))) {
			vel_.x = 0.0f; // 崖や壁の前では立ち止まる
		} else {
			vel_.x = facing_ * kWalkSpeed;
		}
	} else {
		vel_.x = 0.0f;
	}
	StepPhysics();
	UpdateTransform();
}

bool ShieldEnemy::TakeDamage(int damage, float attackDirX) {
	// 攻撃が進む向きと敵の向きが逆 = 敵の正面から飛んできた → 防ぐ
	if (attackDirX * facing_ < 0.0f) {
		flash_.Start(0.15f); // 防いだ演出（少し膨らむ）
		return false;
	}
	return EnemyBase::TakeDamage(damage, attackDirX);
}
