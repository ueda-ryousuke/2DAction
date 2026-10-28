#pragma once
#include "KamataEngine.h"

// 2Dサイドビュー用の簡易フラスタムカリング。
// カメラの向きは +Z 固定・回転なしと仮定して、ある奥行き z の平面で「見えている矩形」を計算する。
class FrustumCuller {
public:
	void Update(const KamataEngine::Camera& camera);
	void Set(const KamataEngine::Vector3& cameraPos, float fovAngleY, float aspectRatio); // Camera を介さず直接指定
	void SetEnabled(bool enabled) { enabled_ = enabled; } // デバッグカメラ中は無効にする

	// worldPos を中心とする半径 radius のものが画面に入っているか（worldPos.z の平面で判定）
	bool IsVisible(const KamataEngine::Vector3& worldPos, float radius) const;
	// 奥行き z の平面で見えている半幅・半高
	void HalfExtentAt(float z, float& halfWidth, float& halfHeight) const;
	// ワールド座標 → スクリーン座標(ピクセル)
	KamataEngine::Vector2 WorldToScreen(const KamataEngine::Vector3& worldPos) const;

	const KamataEngine::Vector3& CameraPosition() const { return cameraPos_; }

private:
	KamataEngine::Vector3 cameraPos_{};
	float tanHalfFov_ = 0.414f;
	float aspect_ = 16.0f / 9.0f;
	bool enabled_ = true;
};
