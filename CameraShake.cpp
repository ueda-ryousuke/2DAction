#include "CameraShake.h"
#include <cmath>

void CameraShake::Start(float amplitude, float seconds) {
	if (amplitude < amplitude_ * timer_.Remaining() / (timer_.Duration() > 0.0f ? timer_.Duration() : 1.0f)) {
		return; // 今揺れている量のほうが大きいなら上書きしない
	}
	amplitude_ = amplitude;
	timer_.Start(seconds);
}

void CameraShake::Update() {
	timer_.Update();
	time_ += 1.0f;
}

KamataEngine::Vector3 CameraShake::Offset() const {
	if (timer_.IsFinished()) {
		return {};
	}
	float k = amplitude_ * (1.0f - timer_.Progress()); // だんだん弱く
	return {std::sin(time_ * 2.7f) * k, std::cos(time_ * 3.3f) * k, 0.0f};
}
