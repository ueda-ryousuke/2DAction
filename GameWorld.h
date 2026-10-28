#pragma once
#include "DynamicBackground.h"
#include "GroundRenderer.h"
#include "RepeatBackground.h"
#include "CameraController.h"
#include "FrustumCuller.h"
#include "EnemyManager.h"
#include "GimmickManager.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Player.h"
#include "CollisionManager.h"
#include "StageData.h"
#include "BulletManager.h"
#include <memory>

class ScoreManager;

// 1ステージぶんの「世界」（マップ・プレイヤー・敵・弾・ギミック・カメラ・背景）をまとめて持つ。
// 元 GameScene が持っていた処理のうち、ゲーム進行(演出/UI/ポーズ)以外はここへ移した。
class GameWorld {
public:
	~GameWorld();

	void Initialize(const StageInfo& stage, int stageIndex, ScoreManager* score);
	void Update();     // 1フレーム進める
	void UpdateIdle(); // 開始演出中: 世界は止めて雲だけ動かす
	void Draw();       // 3Dパス（内部で ModelDrawScope を使う）

	void ShakeCamera(float amplitude, float seconds) { cameraController_.Shake(amplitude, seconds); }

	Player& GetPlayer() { return player_; }
	const FrustumCuller& Culler() const { return culler_; }
	const CollisionEvents& Events() const { return events_; }
	bool IsGoalReached() const { return gimmicks_.IsGoalReached(); }
	bool IsPlayerDead() const { return player_.IsDead(); }

private:
	void ConfigureCameraArea(const StageInfo& stage);
	void SyncCamera();

	static inline const float kKillY = -6.0f; // これより下に落ちたら奈落

	KamataEngine::Camera camera_;
	KamataEngine::DebugCamera* debugCamera_ = nullptr;
	bool debugCameraActive_ = false;
	FrustumCuller culler_;

	std::unique_ptr<MapChipField> field_;
	Player player_;
	CameraController cameraController_;
	GroundRenderer ground_;
	RepeatBackground background_;
	DynamicBackground clouds_;
	BulletManager bullets_;
	EnemyManager enemies_;
	GimmickManager gimmicks_;

	StageCamera cameraKind_ = StageCamera::kFollow;
	CollisionEvents events_;
};
