#pragma once
#include "Bullet.h"

// ホーミング弾。毎フレーム、最も近い敵の方向へ「最大旋回角つき」で向きを変える。
class HomingBullet : public Bullet {
public:
	void Update(const ITargetProvider* targets) override;

private:
	static inline const float kSeekRange = 28.0f;     // この距離内の敵を狙う
	static inline const float kMaxTurnPerFrame = 0.07f; // [rad/frame] 小さいほど曲がりが緩い
};
