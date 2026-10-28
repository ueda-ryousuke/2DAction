#pragma once
#include "CountdownTimer.h"
#include "UiSprite.h"

// HPゲージ。減ったぶんは赤い「残像」として残り、少し遅れてスルスル縮む（減少演出）。
class HpGauge {
public:
	void Initialize();
	void Update(int hp, int maxHp); // 毎フレーム現在値を渡す
	void Draw();

private:
	static inline const float kBarWidth = 360.0f;
	static inline const float kBarHeight = 28.0f;
	static inline const KamataEngine::Vector2 kOrigin = {110.0f, 30.0f};

	UiSprite label_;
	UiSprite frame_;
	UiSprite back_;
	UiSprite trail_; // 減少演出（赤）
	UiSprite front_; // 現在値（緑→黄→赤）

	int lastHp_ = -1;
	float frontRatio_ = 1.0f;
	float trailRatio_ = 1.0f;
	float targetRatio_ = 1.0f;
	CountdownTimer trailDelay_;
	CountdownTimer shake_;
};
