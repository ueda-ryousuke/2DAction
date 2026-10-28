#include "ClearEffect.h"
#include "GameConfig.h"
#include "MathUtil.h"
#include <cmath>

using namespace KamataEngine;

void ClearEffect::Initialize() {
	BannerEffect::Initialize("clear.png", {976.0f, 163.0f}, 1.3f);
	flash_.InitializeSolid({1, 1, 1, 0});
	flash_.SetPosition({0, 0});
	flash_.SetSize({GameConfig::kScreenWidth, GameConfig::kScreenHeight});
	prompt_.Initialize("back_title.png", {412.0f, 75.0f}, {0.5f, 0.5f});
	prompt_.SetPosition({GameConfig::kScreenWidth * 0.5f, GameConfig::kScreenHeight * 0.68f});
	time_ = 0.0f;
}

void ClearEffect::Start() {
	BannerEffect::Start();
	time_ = 0.0f;
}

void ClearEffect::Update() {
	BannerEffect::Update();
	if (IsActive()) {
		time_ += GameConfig::kDeltaTime;
	}
}

BannerEffect::Pose ClearEffect::Evaluate(float t) const {
	Pose p;
	float y0 = -120.0f, y1 = GameConfig::kScreenHeight * 0.38f;
	p.center = {GameConfig::kScreenWidth * 0.5f, MathUtil::Lerp(y0, y1, MathUtil::EaseOutBounce(MathUtil::Clamp01(t / 0.8f)))};
	p.scale = 0.9f + 0.06f * std::sin(time_ * 3.0f); // 落ちたあともゆるく脈打つ
	p.alpha = 1.0f;
	return p;
}

void ClearEffect::Draw() {
	if (!IsActive()) {
		return;
	}
	if (Time() < 0.35f) { // 開始直後の白フラッシュ
		flash_.SetAlpha(0.85f * (1.0f - Time() / 0.35f));
		flash_.Draw();
	}
	BannerEffect::Draw();
	if (IsReadyForInput()) {
		prompt_.SetAlpha(0.6f + 0.4f * std::sin(time_ * 5.0f));
		prompt_.Draw();
	}
}
