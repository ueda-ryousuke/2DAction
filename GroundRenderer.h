#pragma once
#include "KamataEngine.h"
#include <memory>
#include <vector>

class MapChipField;
class FrustumCuller;

// 地面（マップチップのブロック）の描画。元 GameScene の GenerateBlocks / 描画ループを独立させ、
// フラスタムカリングで画面外のブロックを描かないようにした。ブロックは動かないので行列は初期化時に1回だけ計算する。
class GroundRenderer {
public:
	void Initialize(MapChipField* field, KamataEngine::Model* blockModel);
	void Draw(const KamataEngine::Camera& camera, const FrustumCuller& culler);

private:
	KamataEngine::Model* model_ = nullptr;
	std::vector<std::vector<std::unique_ptr<KamataEngine::WorldTransform>>> blocks_; // [y][x]、ブロックがない所は nullptr
};
