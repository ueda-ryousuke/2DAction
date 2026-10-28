#pragma once
#include "UiSprite.h"
#include "IScene.h"
#include "ScrollingBackdrop.h"
#include <array>
#include <memory>

// ステージ選択: ← → で選び、SPACE で決定、ESC でタイトルへ。ステージごとのモデルがくるくる回る。
class StageSelectScene : public IScene {
public:
	void Initialize(SceneContext& context) override;
	void Update() override;
	void Draw() override;

private:
	static constexpr int kMaxStages = 3;

	ScrollingBackdrop backdrop_;
	UiSprite title_;
	UiSprite hint_;
	UiSprite arrowLeft_;
	UiSprite arrowRight_;
	std::array<std::unique_ptr<UiSprite>, kMaxStages> labels_;
	KamataEngine::WorldTransform previewWt_;
	int selected_ = 0;
	float time_ = 0.0f;
	float swing_ = 0.0f; // 切り替え時のぷるん演出
};
