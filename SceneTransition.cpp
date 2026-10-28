#include "SceneTransition.h"
#include "GameConfig.h"
#include "MathUtil.h"
#include <algorithm>

using namespace KamataEngine;

void SceneTransition::Initialize() {
	const float w = GameConfig::kScreenWidth / kStripCount;
	for (int i = 0; i < kStripCount; ++i) {
		strips_[i] = std::make_unique<UiSprite>();
		strips_[i]->Initialize("wall_strip.png", {w, GameConfig::kScreenHeight});
		strips_[i]->SetPosition({w * static_cast<float>(i), -GameConfig::kScreenHeight});
	}
	state_ = State::kOpen;
}

void SceneTransition::StartClose() {
	state_ = State::kClosing;
	time_ = 0.0f;
}

void SceneTransition::StartOpen() {
	state_ = State::kOpening;
	time_ = 0.0f;
}

void SceneTransition::Update() {
	if (state_ != State::kClosing && state_ != State::kOpening) {
		return;
	}
	time_ += GameConfig::kDeltaTime;
	const float total = kStagger * (kStripCount - 1) + kStripTime;
	if (time_ >= total) {
		state_ = (state_ == State::kClosing) ? State::kClosed : State::kOpen;
	}
}

float SceneTransition::StripY(int i) const {
	const float H = GameConfig::kScreenHeight;
	switch (state_) {
	case State::kOpen: return -H - 8.0f; // 画面の外
	case State::kClosed: return 0.0f;
	case State::kClosing: {
		float t = MathUtil::Clamp01((time_ - kStagger * i) / kStripTime);
		return -H + H * MathUtil::EaseOutBounce(t); // ドスンと落ちて少し弾む
	}
	case State::kOpening: {
		float t = MathUtil::Clamp01((time_ - kStagger * i) / kStripTime);
		return -H * MathUtil::EaseInQuad(t); // 引き上がる
	}
	}
	return 0.0f;
}

void SceneTransition::Draw() {
	if (state_ == State::kOpen) {
		return;
	}
	const float w = GameConfig::kScreenWidth / kStripCount;
	for (int i = 0; i < kStripCount; ++i) {
		strips_[i]->SetPosition({w * static_cast<float>(i), StripY(i)});
		strips_[i]->Draw();
	}
}
