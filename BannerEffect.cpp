#include "BannerEffect.h"

using namespace KamataEngine;

void BannerEffect::Initialize(const std::string& uiFile, const Vector2& nativeSize, float duration) {
	sprite_.Initialize(uiFile, nativeSize, {0.5f, 0.5f}); // 中心基準
	duration_ = duration;
	started_ = false;
}

void BannerEffect::Start() {
	started_ = true;
	timer_.Start(duration_);
}

void BannerEffect::Update() {
	if (started_) {
		timer_.Update();
	}
}

void BannerEffect::Draw() {
	if (!started_) {
		return;
	}
	Pose p = Evaluate(timer_.Progress());
	if (p.alpha <= 0.0f) {
		return;
	}
	sprite_.SetPosition(p.center);
	sprite_.SetScale(p.scale);
	sprite_.SetAlpha(p.alpha);
	sprite_.SetRotation(p.rotation);
	sprite_.Draw();
}
