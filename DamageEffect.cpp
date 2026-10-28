#include "DamageEffect.h"
#include "GameConfig.h"

void DamageEffect::Initialize() {
	sprite_.InitializeSolid({1.0f, 0.1f, 0.1f, 0.0f});
	sprite_.SetPosition({0.0f, 0.0f});
	sprite_.SetSize({GameConfig::kScreenWidth, GameConfig::kScreenHeight});
	timer_.Stop();
}

void DamageEffect::Trigger() { timer_.Start(0.45f); }
void DamageEffect::Update() { timer_.Update(); }

void DamageEffect::Draw() {
	if (timer_.IsFinished()) {
		return;
	}
	float k = 1.0f - timer_.Progress();
	sprite_.SetColor(1.0f, 0.1f, 0.1f, 0.45f * k * k);
	sprite_.Draw();
}
