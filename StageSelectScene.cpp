#include "StageSelectScene.h"
#include "BgmPlayer.h"
#include "DrawScope.h"
#include "GameConfig.h"
#include "MathUtil.h"
#include "ModelLibrary.h"
#include "Matri4x4.h"
#include "StageData.h"
#include <cmath>

using namespace KamataEngine;

void StageSelectScene::Initialize(SceneContext& context) {
	context_ = &context;
	finished_ = false;
	selected_ = context.selectedStage;
	time_ = 0.0f;
	swing_ = 0.0f;
	backdrop_.Initialize();
	BgmPlayer::Get().Play(BgmId::kTitle); // タイトルと同じ曲（鳴っていれば途切れない）
	previewWt_.Initialize();

	title_.Initialize("select_title.png", {621.0f, 109.0f}, {0.5f, 0.5f});
	hint_.Initialize("select_hint.png", {559.0f, 71.0f}, {0.5f, 0.5f});
	arrowRight_.Initialize("cursor.png", {48.0f, 48.0f}, {0.5f, 0.5f});
	arrowLeft_.Initialize("cursor.png", {48.0f, 48.0f}, {0.5f, 0.5f});
	arrowLeft_.SetRotation(MathUtil::kPi); // 右向きの矢印を反転して左向きに

	const Vector2 sizes[kMaxStages] = {{683.0f, 97.0f}, {847.0f, 99.0f}, {570.0f, 96.0f}};
	for (int i = 0; i < kMaxStages; ++i) {
		labels_[i] = std::make_unique<UiSprite>();
		labels_[i]->Initialize("stage_" + std::to_string(i + 1) + ".png", sizes[i], {0.5f, 0.5f});
	}
}

void StageSelectScene::Update() {
	time_ += GameConfig::kDeltaTime;
	backdrop_.Update();
	swing_ = MathUtil::Lerp(swing_, 0.0f, 0.15f);

	Input* in = Input::GetInstance();
	const int count = StageData::Count();
	if (in->TriggerKey(DIK_RIGHT) || in->TriggerKey(DIK_D)) {
		selected_ = (selected_ + 1) % count;
		swing_ = 1.0f;
	}
	if (in->TriggerKey(DIK_LEFT) || in->TriggerKey(DIK_A)) {
		selected_ = (selected_ + count - 1) % count;
		swing_ = -1.0f;
	}
	if (in->TriggerKey(DIK_SPACE) || in->TriggerKey(DIK_RETURN)) {
		context_->selectedStage = selected_;
		finished_ = true;
		next_ = SceneId::kGame;
	} else if (in->TriggerKey(DIK_ESCAPE)) {
		finished_ = true;
		next_ = SceneId::kTitle;
	}

	// 選択中ステージのプレビューモデル（くるくる回りながらふわふわ浮く）
	const StageInfo& info = StageData::Get(selected_);
	const float s = info.previewScale * (1.0f + 0.25f * std::fabs(swing_));
	previewWt_.scale_ = {s, s, s};
	previewWt_.rotation_ = {0.35f, time_ * 0.9f + swing_ * 0.6f, 0.0f};
	previewWt_.translation_ = {backdrop_.CameraX(), 4.0f + std::sin(time_ * 2.0f) * 0.8f, 0.0f};
	previewWt_.matWorld_ = MakeAffineMatrix(previewWt_.scale_, previewWt_.rotation_, previewWt_.translation_);
	previewWt_.TransferMatrix();
}

void StageSelectScene::Draw() {
	{
		ModelDrawScope scope;
		backdrop_.Draw3D();
		Model* preview = ModelLibrary::Get().Find(StageData::Get(selected_).previewModel);
		if (preview) {
			preview->Draw(previewWt_, backdrop_.GetCamera());
		}
	}
	SpriteDrawScope scope;
	const float cx = GameConfig::kScreenWidth * 0.5f;
	title_.SetPosition({cx, 90.0f});
	title_.Draw();

	labels_[selected_]->SetPosition({cx, 560.0f});
	labels_[selected_]->Draw();
	hint_.SetPosition({cx, 660.0f});
	hint_.Draw();

	const float wiggle = std::sin(time_ * 6.0f) * 6.0f;
	arrowLeft_.SetPosition({cx - 360.0f - wiggle, 360.0f});
	arrowLeft_.Draw();
	arrowRight_.SetPosition({cx + 360.0f + wiggle, 360.0f});
	arrowRight_.Draw();
}
