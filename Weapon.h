#pragma once
#include "CountdownTimer.h"

enum class FireMode { kStraight, kHoming };

// プレイヤーの武器: 弾数制限 + リロード + 発射モード切り替え。
// 弾の生成そのものは BulletManager の仕事（ここは「撃てるか」の管理だけ）。
class Weapon {
public:
	void Initialize();
	void Update();

	bool TryFire();     // 撃てるなら弾数を消費して true
	void StartReload(); // 手動リロード（弾切れ時は自動で開始）
	void ToggleMode();

	FireMode Mode() const { return mode_; }
	int Ammo() const { return ammo_; }
	int MagazineSize() const { return kMagazineSize; }
	bool IsReloading() const { return reload_.IsActive(); }
	float ReloadProgress() const { return reload_.Progress(); } // 0..1

	static inline const int kMagazineSize = 8;

private:
	int Cost() const { return mode_ == FireMode::kHoming ? 2 : 1; }

	static inline const float kReloadTime = 1.5f;
	static inline const float kStraightCooldown = 0.15f;
	static inline const float kHomingCooldown = 0.4f;

	FireMode mode_ = FireMode::kStraight;
	int ammo_ = kMagazineSize;
	CountdownTimer reload_;
	CountdownTimer cooldown_;
};
