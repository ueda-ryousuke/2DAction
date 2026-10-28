#include "PlayerHealth.h"
#include "PlayerParams.h"
#include <algorithm>

void PlayerHealth::Initialize(int maxHp) {
	maxHp_ = maxHp;
	hp_ = maxHp;
	invincible_.Stop();
}

void PlayerHealth::Update() { invincible_.Update(); }

bool PlayerHealth::TakeDamage(int amount) {
	if (IsDead() || IsInvincible()) {
		return false;
	}
	hp_ = (std::max)(0, hp_ - amount);
	invincible_.Start(PlayerParams::kInvincibleTime);
	return true;
}

void PlayerHealth::Heal(int amount) { hp_ = (std::min)(maxHp_, hp_ + amount); }

bool PlayerHealth::BlinkVisible() const {
	if (!IsInvincible()) {
		return true;
	}
	return static_cast<int>(invincible_.Remaining() * 20.0f) % 2 == 0; // 約10Hzで点滅
}
