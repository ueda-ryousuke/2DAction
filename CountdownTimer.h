#pragma once
#include "GameConfig.h"
#include <algorithm>

// 秒単位のカウントダウン。Update() を毎フレーム呼ぶ。
class CountdownTimer {
public:
	void Start(float seconds) { duration_ = seconds; remaining_ = seconds; }
	void Stop() { remaining_ = 0.0f; }
	void Update() { remaining_ = (std::max)(0.0f, remaining_ - GameConfig::kDeltaTime); }

	bool IsActive() const { return remaining_ > 0.0f; }
	bool IsFinished() const { return remaining_ <= 0.0f; }
	float Remaining() const { return remaining_; }
	float Duration() const { return duration_; }
	// 0(開始) -> 1(終了)
	float Progress() const { return duration_ > 0.0f ? 1.0f - remaining_ / duration_ : 1.0f; }

private:
	float duration_ = 0.0f;
	float remaining_ = 0.0f;
};
