#pragma once
#include "KamataEngine.h"

struct CameraTargetInfo {
	KamataEngine::Vector3 position{};
	KamataEngine::Vector3 velocity{};
};

// カメラの動かし方の差し替えポイント（追従 / 強制スクロール …）。
class ICameraMode {
public:
	virtual ~ICameraMode() = default;
	// 開始時のカメラ位置
	virtual KamataEngine::Vector3 InitialPosition(const CameraTargetInfo& target) const = 0;
	// 現在位置 current から、このフレームの目標位置を計算して返す
	virtual KamataEngine::Vector3 Compute(const KamataEngine::Vector3& current, const CameraTargetInfo& target) = 0;
};
