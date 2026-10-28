#include "GameOverEffect.h"
#include "GameConfig.h"
#include "MathUtil.h"
#include <cmath>

using namespace KamataEngine;

void GameOverEffect::Initialize() {
	BannerEffect::Initialize("gameover.png", {816.0f, 162.0f}, 1.6f);
	dim_.Initialize("pause_dim.png", {GameConfig::kScreenWidth, GameConfig::kScreenHeight});
	dim_.SetPosition({0, 0});
	dim_.SetSize({GameConfig::kScreenWidth, GameConfig::kScreenHeight});
	prompt_.Initialize("back_title.png", {412.0f, 75.0f}, {0.5f, 0.5f});
	prompt_.SetPosition({GameConfig::kScreenWidth * 0.5f, GameConfig::kScreenHeight * 0.68f});
	time_ = 0.0f;
}

void GameOverEffect::Start() {
	BannerEffect::Start();
	time_ = 0.0f;
}

void GameOverEffect::Update() {
	BannerEffect::Update();
	if (IsActive()) {
		time_ += GameConfig::kDeltaTime;
	}
}

BannerEffect::Pose GameOverEffect::Evaluate(float t) const {
	Pose p;
	float y0 = -150.0f, y1 = GameConfig::kScreenHeight * 0.4f;
	p.center = {GameConfig::kScreenWidth * 0.5f, MathUtil::Lerp(y0, y1, MathUtil::EaseOutCubic(MathUtil::Clamp01(t / 0.7f)))};
	p.scale = 1.0f;
	p.alpha = MathUtil::Clamp01(t / 0.4f);
	p.rotation = 0.04f * std::sin(time_ * 2.0f) * MathUtil::Clamp01((t - 0.7f) / 0.3f); // 降りきったあと揺れる
	return p;
}

void GameOverEffect::Draw() {
	if (!IsActive()) {
		return;
	}
	dim_.SetAlpha(0.65f * MathUtil::Clamp01(Time() / 0.5f));
	dim_.Draw();
	BannerEffect::Draw();
	if (IsReadyForInput()) {
		prompt_.SetAlpha(0.6f + 0.4f * std::sin(time_ * 5.0f));
		prompt_.Draw();
	}
}
