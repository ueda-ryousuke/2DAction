#include "GameScene.h"
#include "BgmPlayer.h"
#include "DrawScope.h"
#include "SePlayer.h"
#include "StageData.h"

using namespace KamataEngine;

void GameScene::Initialize(SceneContext& context) {
	context_ = &context;
	finished_ = false;

	const StageInfo& stage = StageData::Get(context.selectedStage);
	score_.Reset();
	world_.Initialize(stage, context.selectedStage, &score_);

	hpGauge_.Initialize();
	ammo_.Initialize();
	scoreDisplay_.Initialize();
	guide_.Initialize();
	pause_.Initialize();
	fade_.Initialize();
	damage_.Initialize();
	popups_.Initialize();
	start_.Initialize();
	clear_.Initialize();
	gameOver_.Initialize();

	fade_.Start(FadeEffect::Mode::kFadeIn, 0.6f, 0.0f, 0.0f, 0.0f); // 黒から明ける
	start_.Start();
	{
		const BgmId stageBgm[] = {BgmId::kStage1, BgmId::kStage2, BgmId::kStage3};
		BgmPlayer::Get().Play(stageBgm[context.selectedStage % 3]);
		BgmPlayer::Get().SetDuck(false);
	}
	phase_ = Phase::kStarting;
	loseDelay_ = 0;
	hitStopFrames_ = 0;
}

void GameScene::FinishTo(SceneId next) {
	finished_ = true;
	next_ = next;
}

void GameScene::Update() {
	fade_.Update();
	damage_.Update();
	popups_.Update();

	switch (phase_) {
	case Phase::kStarting:
		world_.UpdateIdle();
		start_.Update();
		if (start_.IsFinished()) {
			phase_ = Phase::kPlaying;
		}
		break;

	case Phase::kPlaying:
		UpdatePlaying();
		break;

	case Phase::kPaused: {
		PauseMenu::Command cmd = pause_.Update();
		if (cmd == PauseMenu::Command::kResume) {
			phase_ = Phase::kPlaying;
			BgmPlayer::Get().SetDuck(false);
		} else if (cmd == PauseMenu::Command::kQuitToTitle) {
			FinishTo(SceneId::kTitle); // ポーズからゲームを中断
		}
		break;
	}

	case Phase::kCleared:
		clear_.Update();
		if (clear_.IsReadyForInput() && (Input::GetInstance()->TriggerKey(DIK_SPACE) || Input::GetInstance()->TriggerKey(DIK_RETURN))) {
			FinishTo(SceneId::kTitle);
		}
		break;

	case Phase::kGameOver:
		gameOver_.Update();
		if (loseDelay_ > 0 && --loseDelay_ == 0) {
			SePlayer::Get().Play(SeId::kLose);
		}
		if (gameOver_.IsReadyForInput() && (Input::GetInstance()->TriggerKey(DIK_SPACE) || Input::GetInstance()->TriggerKey(DIK_RETURN))) {
			FinishTo(SceneId::kTitle);
		}
		break;
	}

	// UI は常に最新の値へ
	const Player& p = world_.GetPlayer();
	hpGauge_.Update(p.GetHealth().Hp(), p.GetHealth().MaxHp());
	scoreDisplay_.Update(score_.Score());
	if (phase_ == Phase::kPlaying || phase_ == Phase::kStarting) {
		guide_.Update();
	}
}

void GameScene::UpdatePlaying() {
	pause_.Update();
	if (pause_.IsOpen()) {
		phase_ = Phase::kPaused;
		BgmPlayer::Get().SetDuck(true); // ポーズ中は小さく
		return;
	}
	if (hitStopFrames_ > 0) { // ヒットストップ: ダメージの瞬間だけ世界を止めて重みを出す
		--hitStopFrames_;
		return;
	}

	world_.Update();
	score_.Update();
	ReactToEvents();

	if (world_.IsPlayerDead()) {
		phase_ = Phase::kGameOver;
		SePlayer::Get().Play(SeId::kPlayerDead);
		loseDelay_ = 30; // やられた音のあとに、少し遅れてジングル
		BgmPlayer::Get().Play(BgmId::kGameOver, false, 0.0f);
		gameOver_.Start();
		context_->lastScore = score_.Score();
	} else if (world_.IsGoalReached()) {
		phase_ = Phase::kCleared;
		SePlayer::Get().Play(SeId::kClear);
		BgmPlayer::Get().Play(BgmId::kClear, false, 0.0f);
		clear_.Start();
		context_->lastScore = score_.Score();
	}
}

void GameScene::ReactToEvents() {
	const Player::Events pe = world_.GetPlayer().ConsumeEvents();
	if (pe.damaged || world_.Events().playerDamaged) {
		damage_.Trigger();
		world_.ShakeCamera(DamageEffect::kShakeAmplitude, DamageEffect::kShakeSeconds);
		hitStopFrames_ = DamageEffect::kHitStopFrames;
	}
	if (pe.jumped || pe.wallKicked) {
		SePlayer::Get().Play(SeId::kJump);
	}
	if (pe.landed) {
		SePlayer::Get().Play(SeId::kLand);
	}
	if (world_.Events().enemyDefeated) {
		SePlayer::Get().Play(SeId::kEnemyDead);
		world_.ShakeCamera(0.25f, 0.15f);
	}
	popups_.Spawn(score_.ConsumeGains());
}

void GameScene::Draw() {
	world_.Draw();

	SpriteDrawScope scope;
	popups_.Draw(world_.Culler());
	hpGauge_.Draw();
	ammo_.Draw(world_.GetPlayer().GetWeapon());
	scoreDisplay_.Draw();
	guide_.Draw();
	damage_.Draw();
	start_.Draw();
	clear_.Draw();
	gameOver_.Draw();
	pause_.Draw();
	fade_.Draw();
}