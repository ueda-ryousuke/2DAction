#include "PauseMenu.h"
#include "GameConfig.h"
#include "KamataEngine.h"
#include <cmath>

using namespace KamataEngine;

namespace {
const float kCenterX = GameConfig::kScreenWidth * 0.5f;
const float kItemY[2] = {340.0f, 440.0f};
} // namespace

void PauseMenu::Initialize() {
	dim_.Initialize("pause_dim.png", {GameConfig::kScreenWidth, GameConfig::kScreenHeight});
	dim_.SetPosition({0.0f, 0.0f});
	dim_.SetSize({GameConfig::kScreenWidth, GameConfig::kScreenHeight});
	dim_.SetAlpha(0.65f);
	title_.Initialize("pause_title.png", {339.0f, 149.0f}, {0.5f, 0.5f});
	title_.SetPosition({kCenterX, 170.0f});
	items_[0].Initialize("pause_resume.png", {450.0f, 90.0f}, {0.5f, 0.5f});
	items_[1].Initialize("pause_quit.png", {709.0f, 91.0f}, {0.5f, 0.5f});
	cursor_.Initialize("cursor.png", {48.0f, 48.0f}, {0.5f, 0.5f});
	open_ = false;
}

void PauseMenu::Open() {
	open_ = true;
	selected_ = 0;
	time_ = 0.0f;
}

PauseMenu::Command PauseMenu::Update() {
	Input* in = Input::GetInstance();
	const bool pausePressed = in->TriggerKey(DIK_ESCAPE) || in->TriggerKey(DIK_P);

	if (!open_) {
		if (pausePressed) {
			Open();
		}
		return Command::kNone;
	}

	time_ += GameConfig::kDeltaTime;
	if (pausePressed) {
		open_ = false;
		return Command::kResume;
	}
	if (in->TriggerKey(DIK_W) || in->TriggerKey(DIK_UP)) {
		selected_ = (selected_ + 1) % 2;
	}
	if (in->TriggerKey(DIK_S) || in->TriggerKey(DIK_DOWN)) {
		selected_ = (selected_ + 1) % 2;
	}
	if (in->TriggerKey(DIK_SPACE) || in->TriggerKey(DIK_RETURN)) {
		open_ = false;
		return selected_ == 0 ? Command::kResume : Command::kQuitToTitle;
	}
	return Command::kNone;
}

void PauseMenu::Draw() {
	if (!open_) {
		return;
	}
	dim_.Draw();
	title_.Draw();
	for (int i = 0; i < 2; ++i) {
		const bool sel = (i == selected_);
		items_[i].SetPosition({kCenterX, kItemY[i]});
		items_[i].SetScale(sel ? 1.0f + 0.04f * std::sin(time_ * 8.0f) : 0.85f);
		items_[i].SetAlpha(sel ? 1.0f : 0.55f);
		items_[i].Draw();
	}
	cursor_.SetPosition({kCenterX - items_[selected_].BaseSize().x * 0.5f - 50.0f + 6.0f * std::sin(time_ * 8.0f), kItemY[selected_]});
	cursor_.Draw();
}
