#pragma once
#include "CountdownTimer.h"
#include "KamataEngine.h"

// 画面揺れ。ダメージ演出などから Start() する。
class CameraShake {
public:
	void Start(float amplitude, float seconds);
	void Update();
	KamataEngine::Vector3 Offset() const; // カメラ位置に足すオフセット

private:
	float amplitude_ = 0.0f;
	float time_ = 0.0f;
	CountdownTimer timer_;
};
