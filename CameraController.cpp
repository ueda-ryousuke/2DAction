#include "CameraController.h"
#include "FrustumCuller.h"
#include "MathUtil.h"
#include "Player.h"
#include <algorithm>

using namespace KamataEngine;

namespace {
CameraTargetInfo MakeTargetInfo(const Player* p) { return {p->GetWorldTransform().translation_, p->GetVelocity()}; }
} // namespace

void CameraController::Initialize(Camera* camera) {
	camera_.Initialize();
	camera_.farZ = camera->farZ;
	camera_.nearZ = camera->nearZ;
	camera_.fovAngleY = camera->fovAngleY;
	camera_.aspectRatio = camera->aspectRatio;
}

void CameraController::SetMode(Mode mode) { mode_ = mode; }

void CameraController::Update() {
	CameraTargetInfo info = MakeTargetInfo(target_);

	basePosition_ = Current().Compute(basePosition_, info);

	// 全体の移動範囲制限
	basePosition_.x = std::clamp(basePosition_.x, movableArea_.left, movableArea_.right);
	basePosition_.y = std::clamp(basePosition_.y, movableArea_.bottom, movableArea_.top);

	shake_.Update();
	camera_.translation_ = MathUtil::Add(basePosition_, shake_.Offset());
	camera_.UpdateMatrix();
}

void CameraController::Reset() {
	basePosition_ = Current().InitialPosition(MakeTargetInfo(target_));
	basePosition_.x = std::clamp(basePosition_.x, movableArea_.left, movableArea_.right);
	basePosition_.y = std::clamp(basePosition_.y, movableArea_.bottom, movableArea_.top);
	camera_.translation_ = basePosition_;
	camera_.UpdateMatrix();
}

bool CameraController::IsTargetLeftBehind(float margin) const {
	FrustumCuller culler;
	culler.Update(camera_);
	float hw, hh;
	culler.HalfExtentAt(target_->GetWorldTransform().translation_.z, hw, hh);
	return target_->GetWorldTransform().translation_.x < basePosition_.x - hw - margin;
}
