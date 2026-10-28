#include "HpGauge.h"
#include "MathUtil.h"
#include <cmath>

using namespace KamataEngine;

void HpGauge::Initialize() {
	label_.Initialize("hp_label.png", {82.0f, 56.0f});
	label_.SetScale(0.8f);
	frame_.InitializeSolid({0.05f, 0.05f, 0.1f, 0.9f});
	back_.InitializeSolid({0.25f, 0.25f, 0.3f, 1.0f});
	trail_.InitializeSolid({1.0f, 0.35f, 0.2f, 1.0f});
	front_.InitializeSolid({0.3f, 0.9f, 0.4f, 1.0f});
	lastHp_ = -1;
	frontRatio_ = trailRatio_ = targetRatio_ = 1.0f;
}

void HpGauge::Update(int hp, int maxHp) {
	targetRatio_ = maxHp > 0 ? static_cast<float>(hp) / static_cast<float>(maxHp) : 0.0f;
	if (lastHp_ >= 0 && hp < lastHp_) {
		trailDelay_.Start(0.5f); // 減った → しばらく赤を残す
		shake_.Start(0.3f);
	}
	if (hp > lastHp_) {
		trailRatio_ = (std::max)(trailRatio_, targetRatio_); // 回復時は残像を追い越させない
	}
	lastHp_ = hp;

	trailDelay_.Update();
	shake_.Update();
	frontRatio_ = MathUtil::Lerp(frontRatio_, targetRatio_, 0.25f);
	if (trailDelay_.IsFinished()) {
		trailRatio_ = MathUtil::Lerp(trailRatio_, targetRatio_, 0.06f);
	}
	if (trailRatio_ < targetRatio_) {
		trailRatio_ = targetRatio_;
	}
}

void HpGauge::Draw() {
	Vector2 o = kOrigin;
	if (shake_.IsActive()) {
		o.x += std::sin(shake_.Remaining() * 90.0f) * 6.0f * shake_.Remaining() / 0.3f;
	}
	label_.SetPosition({o.x - 92.0f, o.y - 10.0f});
	label_.Draw();

	frame_.SetPosition({o.x - 4.0f, o.y - 4.0f});
	frame_.SetSize({kBarWidth + 8.0f, kBarHeight + 8.0f});
	frame_.Draw();
	back_.SetPosition(o);
	back_.SetSize({kBarWidth, kBarHeight});
	back_.Draw();
	trail_.SetPosition(o);
	trail_.SetSize({kBarWidth * trailRatio_, kBarHeight});
	trail_.Draw();

	// 残量に応じて 緑 → 黄 → 赤
	float r = frontRatio_ > 0.5f ? MathUtil::Lerp(1.0f, 0.3f, (frontRatio_ - 0.5f) * 2.0f) : 1.0f;
	float g = frontRatio_ > 0.5f ? 0.9f : MathUtil::Lerp(0.3f, 0.9f, frontRatio_ * 2.0f);
	front_.SetColor(r, g, 0.3f, 1.0f);
	front_.SetPosition(o);
	front_.SetSize({kBarWidth * frontRatio_, kBarHeight});
	front_.Draw();
}
