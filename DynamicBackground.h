#pragma once
#include "KamataEngine.h"
#include <memory>
#include <vector>

class FrustumCuller;

// 動く背景: ゆっくり流れて上下にゆれる雲。カメラの動きに合わせて画面内を回り続ける。
class DynamicBackground {
public:
	void Initialize();
	void Update(const KamataEngine::Camera& camera, const FrustumCuller& culler);
	void Draw(const KamataEngine::Camera& camera, const FrustumCuller& culler);

private:
	struct Cloud {
		float relX = 0.0f;      // カメラ中心からの横位置（左へ流れる）
		float yOffset = 0.0f;
		float z = 40.0f;
		float scale = 3.0f;
		float speed = 0.02f;
		float parallax = 0.1f;
		float phase = 0.0f;
		std::unique_ptr<KamataEngine::WorldTransform> wt;
	};

	KamataEngine::Model* model_ = nullptr;
	std::vector<Cloud> clouds_;
};
