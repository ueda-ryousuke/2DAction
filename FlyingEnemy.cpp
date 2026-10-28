#include "FlyingEnemy.h"
#include <cmath>

using namespace KamataEngine;

void FlyingEnemy::OnInitialize() {
	SetBody(1, 0.8f, 0.6f);
	SetVisual(1.2f, 0.0f);
	visualOffsetY_ = 0.0f; // 飛行は中心基準で描く
	baseY_ = pos_.y;
}

void FlyingEnemy::Update(EnemyContext& ctx) {
	flash_.Update();
	phase_ += 0.07f;

	const float dx = ctx.playerPos.x - pos_.x;
	if (std::fabs(dx) < kChaseRange) {
		Face(dx);
		pos_.x += (dx > 0.0f ? 1.0f : -1.0f) * kChaseSpeed;
		// プレイヤーの高さへゆるく寄せつつ上下に揺れる
		baseY_ += (ctx.playerPos.y + 1.5f - baseY_) * 0.01f;
	}
	pos_.y = baseY_ + std::sin(phase_) * kBobAmplitude;

	UpdateTransform(std::sin(phase_ * 2.0f) * 0.15f); // 羽ばたき風の傾き
}
