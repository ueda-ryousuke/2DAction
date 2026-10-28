#include "GameWorld.h"
#include "DrawScope.h"
#include "GameConfig.h"
#include "ModelLibrary.h"
#include "ScoreManager.h"
#include "StageBuilder.h"
#include <algorithm>

using namespace KamataEngine;

GameWorld::~GameWorld() { delete debugCamera_; }

void GameWorld::Initialize(const StageInfo& stage, int stageIndex, ScoreManager* score) {
	ModelLibrary& models = ModelLibrary::Get();

	camera_.Initialize();
	camera_.farZ = 1000.0f;

	delete debugCamera_;
	debugCamera_ = new DebugCamera(1280, 720);
	debugCamera_->SetFarZ(camera_.farZ);
	debugCameraActive_ = false;

	// マップ
	field_ = std::make_unique<MapChipField>();
	field_->LoadMapChipCsv(stage.csvPath);
	ground_.Initialize(field_.get(), models.Find("block"));

	// 弾・敵・ギミック（互いに参照するので生成順に注意）
	gimmicks_.Clear();
	bullets_.Initialize(field_.get(), &enemies_);
	enemies_.Initialize(field_.get(), &bullets_, score, &gimmicks_.Solids());
	Vector3 start = StageBuilder::Build(stageIndex, *field_, gimmicks_, enemies_);

	// プレイヤー
	player_.Initialize(models.Find("player"), &camera_, start);
	player_.SetMapChipField(field_.get());
	player_.SetSolids(&gimmicks_.Solids());
	player_.SetBulletManager(&bullets_);

	// カメラ
	cameraKind_ = stage.camera;
	cameraController_.Initialize(&camera_);
	cameraController_.SetTarget(&player_);
	ConfigureCameraArea(stage);
	cameraController_.Reset();
	SyncCamera();

	background_.Initialize();
	clouds_.Initialize();
	events_ = {};
}

// カメラが映してよい範囲を「マップの外が見えない」ように決める
void GameWorld::ConfigureCameraArea(const StageInfo& stage) {
	FrustumCuller c;
	c.Set({0.0f, 0.0f, -50.0f}, camera_.fovAngleY, camera_.aspectRatio); // 追従カメラの標準位置(z=-50)から見える範囲
	float hw, hh;
	c.HalfExtentAt(0.0f, hw, hh);

	const float bs = GameConfig::kBlockSize;
	const float mapW = static_cast<float>(field_->GetNumBlockHorizontal()) * bs;
	const float mapH = static_cast<float>(field_->GetNumBlockVertical()) * bs;

	CameraController::Rect area;
	area.left = hw - bs * 0.5f;
	area.right = (std::max)(area.left, mapW - bs * 0.5f - hw);
	area.bottom = hh - bs * 0.5f;
	area.top = (std::max)(area.bottom, mapH - bs * 0.5f - hh);
	cameraController_.SetMovableArea(area);

	if (stage.camera == StageCamera::kForcedScroll) {
		cameraController_.SetMode(CameraController::Mode::kForcedScroll);
		cameraController_.SetForcedScrollSpeed(stage.scrollSpeed);
	} else {
		cameraController_.SetMode(CameraController::Mode::kFollow);
	}
}

void GameWorld::SyncCamera() {
	const Camera& c = cameraController_.GetCamera();
	camera_.translation_ = c.translation_;
	camera_.matView = c.matView;
	camera_.matProjection = c.matProjection;
	camera_.TransferMatrix();
	culler_.Update(camera_);
}

void GameWorld::Update() {
	// ① ギミック（床の移動 → SolidBox の再収集）
	GimmickContext gctx;
	gctx.playerPos = player_.GetWorldPosition();
	gctx.playerAabb = player_.GetAabb();
	gimmicks_.Update(gctx);

	// ② プレイヤー → 敵 → 弾
	player_.Update();
	const Vector3 playerPos = player_.GetWorldPosition();
	enemies_.Update(playerPos);
	bullets_.Update();

	// ③ 当たり判定
	events_ = CollisionManager::Run(player_, enemies_, bullets_, gimmicks_);

	// ④ 奈落 / 強制スクロールで置いていかれた
	if (playerPos.y < kKillY) {
		player_.Kill();
	}
	if (cameraKind_ == StageCamera::kForcedScroll && cameraController_.IsTargetLeftBehind(1.0f)) {
		player_.Kill();
	}

	// ⑤ カメラ
	if (!debugCameraActive_) {
		cameraController_.Update();
		SyncCamera();
	}
	debugCamera_->Update();
#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_Y)) {
		debugCameraActive_ = !debugCameraActive_;
		culler_.SetEnabled(!debugCameraActive_);
	}
#endif
	if (debugCameraActive_) {
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		camera_.TransferMatrix();
	}
	clouds_.Update(camera_, culler_);
}

void GameWorld::UpdateIdle() {
	clouds_.Update(camera_, culler_);
}

void GameWorld::Draw() {
	ModelDrawScope scope;
	background_.Draw(camera_, culler_);
	clouds_.Draw(camera_, culler_);
	ground_.Draw(camera_, culler_);
	gimmicks_.Draw(camera_, culler_);
	enemies_.Draw(camera_, culler_);
	bullets_.Draw(camera_);
	player_.Draw();
}
