#include "GroundRenderer.h"
#include "FrustumCuller.h"
#include "GameConfig.h"
#include "MapChipField.h"
#include "Matri4x4.h"

using namespace KamataEngine;

void GroundRenderer::Initialize(MapChipField* field, Model* blockModel) {
	model_ = blockModel;
	blocks_.clear();

	const uint32_t rows = field->GetNumBlockVertical();
	const uint32_t cols = field->GetNumBlockHorizontal();
	const float s = GameConfig::kBlockSize / 2.0f; // block モデルは 2x2x2
	blocks_.resize(rows);
	for (uint32_t y = 0; y < rows; ++y) {
		blocks_[y].resize(cols);
		for (uint32_t x = 0; x < cols; ++x) {
			if (field->GetMapChipTypeByIndex(x, y) != MapChipType::kBlock) {
				continue;
			}
			auto wt = std::make_unique<WorldTransform>();
			wt->Initialize();
			wt->scale_ = {s, s, s};
			wt->translation_ = field->GetMapChipPositionByIndex(x, y);
			wt->matWorld_ = MakeAffineMatrix(wt->scale_, wt->rotation_, wt->translation_);
			wt->TransferMatrix();
			blocks_[y][x] = std::move(wt);
		}
	}
}

void GroundRenderer::Draw(const Camera& camera, const FrustumCuller& culler) {
	for (auto& row : blocks_) {
		for (auto& wt : row) {
			if (wt && culler.IsVisible(wt->translation_, GameConfig::kBlockSize)) {
				model_->Draw(*wt, camera);
			}
		}
	}
}
