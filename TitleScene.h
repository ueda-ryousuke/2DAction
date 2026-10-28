#pragma once
#include "UiSprite.h"
#include "IScene.h"
#include "ScrollingBackdrop.h"

// 動きのあるタイトル画面: 背景が流れ、プレイヤーが二段ジャンプで跳ね回り、敵が漂い、ロゴがゆれる。
class TitleScene : public IScene {
public:
	void Initialize(SceneContext& context) override;
	void Update() override;
	void Draw() override;

private:
	void UpdateActors();

	ScrollingBackdrop backdrop_;
	UiSprite logo_;
	UiSprite pressStart_;

	KamataEngine::WorldTransform playerWt_;
	KamataEngine::WorldTransform flyerWt_;
	KamataEngine::WorldTransform slimeWt_;
	KamataEngine::Model* playerModel_ = nullptr;
	KamataEngine::Model* flyerModel_ = nullptr;
	KamataEngine::Model* slimeModel_ = nullptr;
	float time_ = 0.0f;
};
