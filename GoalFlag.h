#pragma once
#include "Gimmick.h"

// ゴール旗: 触れるとステージクリア。
class GoalFlag : public Gimmick {
public:
	void Initialize(const KamataEngine::Vector3& footPosition); // 旗の根元（モデル原点=足元）
	void Update(GimmickContext& ctx) override;

private:
	float phase_ = 0.0f;
};
