#pragma once
#include "Bullet.h"

// まっすぐ飛ぶ弾。
class StraightBullet : public Bullet {
public:
	void Update(const ITargetProvider* targets) override;
};
