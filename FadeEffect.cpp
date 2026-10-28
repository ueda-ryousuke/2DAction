#include "FadeEffect.h"
#include "GameConfig.h"

void FadeEffect::Initialize() {
	sprite_.InitializeSolid({0, 0, 0, 1});
	sprite_.SetPosition({0.0f, 0.0f});
	sprite_.SetSize({GameConfig::kScreenWidth, GameConfig::kScreenHeight});
	mode_ = Mode::kNone;
}

void FadeEffect::Start(Mode mode, float seconds, float r, float g, float b) {
	mode_ = mode;
	r_ = r; g_ = g; b_ = b;
	timer_.Start(seconds);
}

float FadeEffect::Alpha() const {
	switch (mode_) {
	case Mode::kFadeIn: return 1.0f - timer_.Progress();
	case Mode::kFadeOut: return timer_.Progress();
	default: return 0.0f;
	}
}

void FadeEffect::Update() { timer_.Update(); }

void FadeEffect::Draw() {
	float a = Alpha();
	if (a <= 0.0f) {
		return;
	}
	sprite_.SetColor(r_, g_, b_, a);
	sprite_.Draw();
}
