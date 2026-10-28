#include "TitleScene.h"
#include "BgmPlayer.h"
#include "DrawScope.h"
#include "GameConfig.h"
#include "MathUtil.h"
#include "ModelLibrary.h"
#include "Matri4x4.h"
#include <cmath>

using namespace KamataEngine;

void TitleScene::Initialize(SceneContext& context) {
	context_ = &context;
	finished_ = false;
	time_ = 0.0f;
	backdrop_.Initialize();
	BgmPlayer::Get().Play(BgmId::kTitle);

	ModelLibrary& lib = ModelLibrary::Get();
	playerModel_ = lib.Find("player");
	flyerModel_ = lib.Find("enemy_flyer");
	slimeModel_ = lib.Find("enemy_split");
	playerWt_.Initialize();
	flyerWt_.Initialize();
	slimeWt_.Initialize();

	logo_.Initialize("title_logo.png", {1119.0f, 232.0f}, {0.5f, 0.5f});
	pressStart_.Initialize("press_start.png", {495.0f, 80.0f}, {0.5f, 0.5f});
}

void TitleScene::Update() {
	time_ += GameConfig::kDeltaTime;
	backdrop_.Update();
	UpdateActors();

	if (Input::GetInstance()->TriggerKey(DIK_SPACE) || Input::GetInstance()->TriggerKey(DIK_RETURN)) {
		finished_ = true;
		next_ = SceneId::kStageSelect;
	}
}

// 背景がスクロールしている間、キャラは画面内の同じ位置で動き続ける（走っているように見える）
void TitleScene::UpdateActors() {
	const float camX = backdrop_.CameraX();
	const float ground = ScrollingBackdrop::GroundTopY();

	// プレイヤー: 小ジャンプ → 小ジャンプ → 大ジャンプ(二段ジャンプ) を繰り返す
	const float period = 1.4f;
	const int step = static_cast<int>(time_ / period) % 3;
	const float u = std::fmod(time_, period) / period;
	const float height = (step == 2) ? 7.5f : 3.0f;
	const float hop = 4.0f * u * (1.0f - u) * height;
	const float squash = (u < 0.12f) ? -(0.12f - u) * 2.0f : (u > 0.88f ? -(u - 0.88f) * 2.0f : 0.0f); // 着地前後で潰れる
	playerWt_.scale_ = {2.0f * (1.0f - squash * 0.5f), 2.0f * (1.0f + squash), 2.0f};
	playerWt_.translation_ = {camX - 15.0f, ground + 1.04f + hop, 0.0f};
	playerWt_.rotation_.y = MathUtil::kPi * 0.5f;
	playerWt_.matWorld_ = MakeAffineMatrix(playerWt_.scale_, playerWt_.rotation_, playerWt_.translation_);
	playerWt_.TransferMatrix();

	// 飛ぶ敵: ふわふわ上下しながら追いかけてくる
	flyerWt_.scale_ = {1.8f, 1.8f, 1.8f};
	flyerWt_.translation_ = {camX - 4.0f + std::sin(time_ * 0.9f) * 3.0f, 1.0f + std::sin(time_ * 2.2f) * 1.6f, 0.0f};
	flyerWt_.rotation_ = {0.0f, MathUtil::kPi * 0.5f, std::sin(time_ * 4.5f) * 0.15f};
	flyerWt_.matWorld_ = MakeAffineMatrix(flyerWt_.scale_, flyerWt_.rotation_, flyerWt_.translation_);
	flyerWt_.TransferMatrix();

	// スライム: 先頭でぴょんぴょん
	const float su = std::fmod(time_ * 1.1f, 1.0f);
	slimeWt_.scale_ = {2.2f, 2.2f * (0.85f + 0.3f * std::sin(su * MathUtil::kPi)), 2.2f};
	slimeWt_.translation_ = {camX + 8.0f, ground + 1.0f + 4.0f * su * (1.0f - su) * 4.0f, 0.0f};
	slimeWt_.rotation_.y = MathUtil::kPi * 1.5f;
	slimeWt_.matWorld_ = MakeAffineMatrix(slimeWt_.scale_, slimeWt_.rotation_, slimeWt_.translation_);
	slimeWt_.TransferMatrix();
}

void TitleScene::Draw() {
	{
		ModelDrawScope scope;
		backdrop_.Draw3D();
		playerModel_->Draw(playerWt_, backdrop_.GetCamera());
		flyerModel_->Draw(flyerWt_, backdrop_.GetCamera());
		slimeModel_->Draw(slimeWt_, backdrop_.GetCamera());
	}
	SpriteDrawScope scope;
	logo_.SetPosition({GameConfig::kScreenWidth * 0.5f, 190.0f + std::sin(time_ * 2.0f) * 10.0f});
	logo_.SetScale(0.9f + 0.02f * std::sin(time_ * 3.0f));
	logo_.Draw();
	pressStart_.SetPosition({GameConfig::kScreenWidth * 0.5f, 540.0f});
	pressStart_.SetAlpha(0.55f + 0.45f * std::sin(time_ * 4.0f));
	pressStart_.Draw();
}
