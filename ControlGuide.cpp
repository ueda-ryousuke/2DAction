#include "ControlGuide.h"
#include "GameConfig.h"
#include "MathUtil.h"
#include "KamataEngine.h"

using namespace KamataEngine;

void ControlGuide::Initialize() {
	panel_.Initialize("controls.png", {560.0f, 330.0f});
	panel_.SetPosition({24.0f, GameConfig::kScreenHeight - 330.0f - 24.0f});
	visible_ = true;
	autoHideArmed_ = true;
	alpha_ = 0.0f;
	autoHide_.Start(kAutoHideSeconds);
}

void ControlGuide::Update() {
	if (Input::GetInstance()->TriggerKey(DIK_H)) {
		visible_ = !visible_;
		autoHideArmed_ = false;
	}
	if (autoHideArmed_) {
		autoHide_.Update();
		if (autoHide_.IsFinished()) {
			visible_ = false;
			autoHideArmed_ = false;
		}
	}
	alpha_ = MathUtil::Lerp(alpha_, visible_ ? 0.92f : 0.0f, 0.15f);
}

void ControlGuide::Draw() {
	if (alpha_ < 0.01f) {
		return;
	}
	panel_.SetAlpha(alpha_);
	panel_.Draw();
}
