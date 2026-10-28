#include "ShooterEnemy.h"
#include "BulletManager.h"
#include <cmath>

using namespace KamataEngine;

void ShooterEnemy::OnInitialize() {
	SetBody(2, 0.8f, 0.95f);
	SetVisual(1.4f, -0.55f);
	fireTimer_.Start(kFireInterval);
}

void ShooterEnemy::Update(EnemyContext& ctx) {
	const float dx = ctx.playerPos.x - pos_.x;
	const float dy = ctx.playerPos.y - pos_.y;
	const bool inSight = std::fabs(dx) < kSightRangeX && std::fabs(dy) < kSightRangeY;

	if (inSight) {
		// 立ち止まってプレイヤーの方を向き、一定間隔で撃つ
		vel_.x = 0.0f;
		Face(dx);
		fireTimer_.Update();
		if (fireTimer_.IsFinished() && ctx.bullets) {
			Vector3 muzzle = {pos_.x + facing_ * 1.4f, pos_.y + 0.1f, pos_.z};
			ctx.bullets->SpawnEnemyBullet(muzzle, {ctx.playerPos.x - muzzle.x, ctx.playerPos.y - muzzle.y, 0.0f}, kBulletSpeed);
			fireTimer_.Start(kFireInterval);
		}
	} else {
		// 往復パトロール（壁か足場の端で折り返す）
		if (wallHit_ || (onGround_ && IsLedgeAhead(facing_))) {
			Face(-facing_);
		}
		vel_.x = facing_ * kPatrolSpeed;
	}
	StepPhysics();
	UpdateTransform();
}
