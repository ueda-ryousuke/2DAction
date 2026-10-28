#pragma once
#include "CountdownTimer.h"
#include "UiSprite.h"

// ダメージ演出: 画面が赤く光って薄れる。カメラ揺れとヒットストップは GameScene が一緒に起動する。
class DamageEffect {
public:
	static inline const int kHitStopFrames = 4;
	static inline const float kShakeAmplitude = 0.7f;
	static inline const float kShakeSeconds = 0.35f;

	void Initialize();
	void Trigger();
	void Update();
	void Draw();

private:
	UiSprite sprite_;
	CountdownTimer timer_;
};
