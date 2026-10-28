#pragma once
#include "Gimmick.h"

// スイッチ扉: 同じ channel のスイッチが ON になると上へスライドして開く。
class SwitchDoor : public Gimmick {
public:
	void Initialize(const KamataEngine::Vector3& center, int channel, float heightInBlocks = 2.0f);
	void Update(GimmickContext& ctx) override;
	bool GetSolid(SolidBox& out) const override;

private:
	static inline const float kOpenSpeed = 0.02f; // 1フレームで開く割合
	int channel_ = 0;
	float open_ = 0.0f; // 0:閉 .. 1:開
	float height_ = 4.0f;
};
