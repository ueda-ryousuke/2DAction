#pragma once
#include "ICameraMode.h"

// 追従カメラ（元 CameraController::Update のロジックをそのまま移したもの）。
//   ・プレイヤー位置 + オフセット + 速度先読みを目標に、Lerp でゆったり追従
//   ・プレイヤーが画面外へ出ないよう margin で補正
class FollowCameraMode : public ICameraMode {
public:
	struct Margin {
		float left, right, bottom, top;
	};

	KamataEngine::Vector3 InitialPosition(const CameraTargetInfo& target) const override;
	KamataEngine::Vector3 Compute(const KamataEngine::Vector3& current, const CameraTargetInfo& target) override;

private:
	KamataEngine::Vector3 offset_ = {0.0f, 0.0f, -50.0f};
	static inline const float kInterpolationRate = 0.08f; // 1フレームで目標に近づく割合
	static inline const float kVelocityBias = 50.0f;      // 速度の先読み倍率
	static inline const Margin kMargin = {-12.0f, 12.0f, -6.0f, 6.0f};
};
