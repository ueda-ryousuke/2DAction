#pragma once
#include "CameraShake.h"
#include "ForcedScrollCameraMode.h"
#include "FollowCameraMode.h"
#include "KamataEngine.h"
#include <memory>

class Player;

// カメラ全体の窓口。元の CameraController の公開関数(Initialize/Update/SetTarget/SetMovableArea/Reset/GetCamera)を
// そのまま維持しつつ、動かし方を ICameraMode に分離した。
class CameraController {
public:
	struct Rect {
		float left = 0.0f;
		float right = 1.0f;
		float bottom = 0.0f;
		float top = 1.0f;
	};
	enum class Mode { kFollow, kForcedScroll };

	void Initialize(KamataEngine::Camera* camera);
	void Update();
	void Reset();

	void SetTarget(Player* target) { target_ = target; }
	void SetMovableArea(const Rect& area) { movableArea_ = area; }
	void SetMode(Mode mode);
	void SetForcedScrollSpeed(float unitsPerFrame) { forced_.SetSpeed(unitsPerFrame); }
	void Shake(float amplitude, float seconds) { shake_.Start(amplitude, seconds); }

	const KamataEngine::Camera& GetCamera() const { return camera_; }
	Mode GetMode() const { return mode_; }
	// 強制スクロール用: ターゲットが画面の左端より後ろへ置いていかれたか
	bool IsTargetLeftBehind(float margin) const;

private:
	ICameraMode& Current() { return mode_ == Mode::kFollow ? static_cast<ICameraMode&>(follow_) : static_cast<ICameraMode&>(forced_); }

	KamataEngine::Camera camera_;
	Player* target_ = nullptr;
	Rect movableArea_ = {0.0f, 100.0f, 0.0f, 100.0f};
	Mode mode_ = Mode::kFollow;
	FollowCameraMode follow_;
	ForcedScrollCameraMode forced_;
	CameraShake shake_;
	KamataEngine::Vector3 basePosition_{}; // シェイクを含まない本来のカメラ位置
};
