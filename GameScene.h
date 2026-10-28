#pragma once
#include "AmmoDisplay.h"
#include "ClearEffect.h"
#include "ControlGuide.h"
#include "DamageEffect.h"
#include "FadeEffect.h"
#include "GameOverEffect.h"
#include "GameWorld.h"
#include "HpGauge.h"
#include "IScene.h"
#include "PauseMenu.h"
#include "ScoreDisplay.h"
#include "ScorePopupEffect.h"
#include "StartEffect.h"

// ゲーム本編のシーン。世界(GameWorld)の進行は任せ、ここは「フェーズ管理・演出・UI」だけを担当する。
//   Starting(READY→GO) → Playing ⇄ Paused → Cleared / GameOver
class GameScene : public IScene {
public:
	void Initialize(SceneContext& context) override;
	void Update() override;
	void Draw() override;

private:
	enum class Phase { kStarting, kPlaying, kPaused, kCleared, kGameOver };

	void UpdatePlaying();
	void ReactToEvents();
	void FinishTo(SceneId next);

	Phase phase_ = Phase::kStarting;
	GameWorld world_;
	ScoreManager score_;
	int hitStopFrames_ = 0;
	int loseDelay_ = 0; // ゲームオーバージングルまでの待ちフレーム

	// UI
	HpGauge hpGauge_;
	AmmoDisplay ammo_;
	ScoreDisplay scoreDisplay_;
	ControlGuide guide_;
	PauseMenu pause_;
	// 演出
	FadeEffect fade_;
	DamageEffect damage_;
	ScorePopupEffect popups_;
	StartEffect start_;
	ClearEffect clear_;
	GameOverEffect gameOver_;
};