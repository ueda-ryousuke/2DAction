#include "SceneManager.h"
#include "BgmPlayer.h"
#include "DrawScope.h"
#include "GameScene.h"
#include "SePlayer.h"
#include "StageSelectScene.h"
#include "TitleScene.h"

void SceneManager::Initialize() {
	transition_.Initialize();
	fade_.Initialize();
	scene_ = Create(SceneId::kTitle);
	scene_->Initialize(context_);
	fade_.Start(FadeEffect::Mode::kFadeIn, 1.0f, 1.0f, 1.0f, 1.0f); // 白から明ける
	changing_ = false;
}

std::unique_ptr<IScene> SceneManager::Create(SceneId id) const {
	switch (id) {
	case SceneId::kStageSelect:
		return std::make_unique<StageSelectScene>();
	case SceneId::kGame:
		return std::make_unique<GameScene>();
	case SceneId::kTitle:
	default:
		return std::make_unique<TitleScene>();
	}
}

void SceneManager::ChangeTo(SceneId id) {
	pending_ = id;
	changing_ = true;
	transition_.StartClose();
	SePlayer::Get().Play(SeId::kSceneChange);
}

void SceneManager::Update() {
	BgmPlayer::Get().Update();
	fade_.Update();
	transition_.Update();

	if (changing_) {
		// 壁が閉じきったら裏で差し替えて、すぐ開き始める
		if (transition_.IsClosed()) {
			scene_ = Create(pending_);
			scene_->Initialize(context_);
			transition_.StartOpen();
			changing_ = false;
		}
		return;
	}

	scene_->Update();
	if (scene_->IsFinished() && transition_.IsOpen()) {
		ChangeTo(scene_->NextScene());
	}
}

void SceneManager::Draw() {
	scene_->Draw();
	SpriteDrawScope scope; // 転換演出は常に最前面
	transition_.Draw();
	fade_.Draw();
}