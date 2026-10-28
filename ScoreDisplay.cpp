#include "ScoreDisplay.h"
#include "GameConfig.h"
#include <algorithm>

using namespace KamataEngine;

void ScoreDisplay::Initialize() {
	label_.Initialize("score_label.png", {144.0f, 51.0f});
	label_.SetPosition({GameConfig::kScreenWidth - 24.0f - 144.0f, 14.0f});
	number_.Initialize(8);
	shown_ = target_ = 0;
}

void ScoreDisplay::Update(int score) {
	target_ = score;
	if (shown_ < target_) {
		shown_ += (std::max)(1, (target_ - shown_) / 8); // 差が大きいほど速く数える
		shown_ = (std::min)(shown_, target_);
	}
}

void ScoreDisplay::Draw() {
	label_.Draw();
	number_.Draw(shown_, {GameConfig::kScreenWidth - 24.0f, 58.0f}, 0.5f);
}
