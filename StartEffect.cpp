#include "StartEffect.h"
#include "GameConfig.h"
#include "MathUtil.h"

using namespace KamataEngine;

BannerEffect::Pose StartEffect::PopBanner::Evaluate(float t) const {
	Pose p;
	p.center = {GameConfig::kScreenWidth * 0.5f, GameConfig::kScreenHeight * 0.42f};
	float pop = MathUtil::EaseOutBack(MathUtil::Clamp01(t / 0.3f));
	p.scale = pop * MathUtil::Lerp(1.0f, growTo, t);
	p.alpha = t < 0.75f ? 1.0f : 1.0f - (t - 0.75f) / 0.25f;
	return p;
}

void StartEffect::Initialize() {
	ready_.Initialize("ready.png", {588.0f, 174.0f}, 0.9f);
	go_.Initialize("go.png", {439.0f, 227.0f}, 0.8f);
	go_.growTo = 1.5f;
	active_ = false;
	finished_ = false;
}

void StartEffect::Start() {
	active_ = true;
	finished_ = false;
	ready_.Start();
}

void StartEffect::Update() {
	if (!active_) {
		return;
	}
	ready_.Update();
	go_.Update();
	if (ready_.IsFinished() && !go_.IsActive()) {
		go_.Start();
	}
	if (go_.IsFinished()) {
		active_ = false;
		finished_ = true;
	}
}

void StartEffect::Draw() {
	if (!active_) {
		return;
	}
	if (!ready_.IsFinished()) {
		ready_.Draw();
	} else {
		go_.Draw();
	}
}
