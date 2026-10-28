#pragma once
#include "CountdownTimer.h"

// HP と無敵時間。
class PlayerHealth {
public:
	void Initialize(int maxHp);
	void Update();

	// 無敵中でなければダメージを受けて true。
	bool TakeDamage(int amount);
	void Heal(int amount);
	void Kill() { hp_ = 0; } // 無敵を無視して即死（奈落・押しつぶし用）

	int Hp() const { return hp_; }
	int MaxHp() const { return maxHp_; }
	bool IsDead() const { return hp_ <= 0; }
	bool IsInvincible() const { return invincible_.IsActive(); }
	bool BlinkVisible() const; // 無敵中の点滅用（描画するか）

private:
	int hp_ = 0;
	int maxHp_ = 0;
	CountdownTimer invincible_;
};
