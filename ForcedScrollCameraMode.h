#pragma once
#include "ICameraMode.h"

// 強制スクロール: プレイヤーに関係なく X 方向へ一定速度で進む。Y だけ緩く追従。
class ForcedScrollCameraMode : public ICameraMode {
public:
	void SetSpeed(float unitsPerFrame) { speed_ = unitsPerFrame; }
	void SetStopX(float x) { stopX_ = x; } // ここより先へは進まない（ステージ終端）

	KamataEngine::Vector3 InitialPosition(const CameraTargetInfo& target) const override;
	KamataEngine::Vector3 Compute(const KamataEngine::Vector3& current, const CameraTargetInfo& target) override;

private:
	KamataEngine::Vector3 offset_ = {0.0f, 0.0f, -50.0f};
	float speed_ = 0.08f;
	float stopX_ = 1.0e9f;
	static inline const float kYFollowRate = 0.06f;
};
