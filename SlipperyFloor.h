#pragma once
#include "Gimmick.h"

// 滑る床（氷）: 乗ると摩擦が小さくなる。見た目は block_ice を引き伸ばして使う。
class SlipperyFloor : public Gimmick {
public:
	void Initialize(const KamataEngine::Vector3& center, float widthInBlocks, float heightInBlocks = 1.0f);
	void Update(GimmickContext& ctx) override;
	bool GetSolid(SolidBox& out) const override;
};
