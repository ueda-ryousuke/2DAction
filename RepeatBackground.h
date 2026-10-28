#pragma once
#include "KamataEngine.h"
#include <memory>
#include <vector>

class FrustumCuller;

// 無限にリピートする多重スクロール背景（空 / 遠い丘 / 近い丘）。
// 各レイヤーは同じモデルを横に並べ、カメラ位置から「必要な枚数だけ」配置し直す＝画面外のタイルは描かない。
class RepeatBackground {
public:
	void Initialize();
	// extraOffsetX: タイトル画面のように自動でスクロールさせたいときに足す量
	void Draw(const KamataEngine::Camera& camera, const FrustumCuller& culler, float extraOffsetX = 0.0f);

private:
	struct Layer {
		KamataEngine::Model* model = nullptr;
		float tileWidth = 40.0f;       // ワールド上の1タイルの幅（モデル幅 × scale）
		float parallax = 0.0f;         // 0: 画面に張り付く(最遠) .. 1: ワールドと一緒に動く
		float parallaxY = 0.2f;        // 縦方向の追従の弱さ（0: 画面に張り付く）
		float z = 40.0f;
		float baseY = 0.0f;
		KamataEngine::Vector3 scale{1, 1, 1};
		std::vector<std::unique_ptr<KamataEngine::WorldTransform>> pool; // WorldTransform はコピー/ムーブ不可なので unique_ptr
	};

	std::vector<Layer> layers_;
};
