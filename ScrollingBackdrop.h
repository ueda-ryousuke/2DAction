#pragma once
#include "DynamicBackground.h"
#include "RepeatBackground.h"
#include "FrustumCuller.h"
#include "KamataEngine.h"
#include <memory>
#include <vector>

// タイトル/ステージ選択で使う「自動で右へ流れる背景 + 草地」。
// 3D描画は Draw3D()。キャラなどはこのカメラ(GetCamera)を使って各シーンが自分で描く。
class ScrollingBackdrop {
public:
	void Initialize();
	void Update();
	void Draw3D(); // 呼び出し側が ModelDrawScope を開いておくこと

	const KamataEngine::Camera& GetCamera() const { return camera_; }
	float CameraX() const { return scroll_; }
	static float GroundTopY() { return -7.0f; }

private:
	static constexpr int kColumns = 46;
	static constexpr int kRows = 5;
	static inline const float kScrollSpeed = 0.12f;

	KamataEngine::Camera camera_;
	FrustumCuller culler_;
	RepeatBackground background_;
	DynamicBackground clouds_;
	KamataEngine::Model* blockModel_ = nullptr;
	std::vector<std::unique_ptr<KamataEngine::WorldTransform>> blocks_;
	float scroll_ = 0.0f;
};
