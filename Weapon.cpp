#include "Weapon.h"

void Weapon::Initialize() {
	mode_ = FireMode::kStraight;
	ammo_ = kMagazineSize;
	reload_.Stop();
	cooldown_.Stop();
}

void Weapon::Update() {
	cooldown_.Update();
	if (reload_.IsActive()) {
		reload_.Update();
		if (reload_.IsFinished()) {
			ammo_ = kMagazineSize;
		}
	}
}

bool Weapon::TryFire() {
	if (IsReloading() || cooldown_.IsActive()) {
		return false;
	}
	if (ammo_ < Cost()) {
		StartReload(); // 弾切れ → 自動リロード
		return false;
	}
	ammo_ -= Cost();
	cooldown_.Start(mode_ == FireMode::kHoming ? kHomingCooldown : kStraightCooldown);
	if (ammo_ <= 0) {
		StartReload();
	}
	return true;
}

void Weapon::StartReload() {
	if (IsReloading() || ammo_ >= kMagazineSize) {
		return;
	}
	reload_.Start(kReloadTime);
}

void Weapon::ToggleMode() {
	if (IsReloading()) {
		return;
	}
	mode_ = (mode_ == FireMode::kStraight) ? FireMode::kHoming : FireMode::kStraight;
}
