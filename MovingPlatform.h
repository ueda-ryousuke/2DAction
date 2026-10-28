#pragma once
#include "CountdownTimer.h"
#include "Gimmick.h"

// 移動床: start と end の間を往復する。乗っている人は velocity で運ばれる。
class MovingPlatform : public Gimmick {
public:
	void Initialize(const KamataEngine::Vector3& start, const KamataEngine::Vector3& end, float widthInBlocks, float unitsPerFrame);
	void Update(GimmickContext& ctx) override;
	bool GetSolid(SolidBox& out) const override;

private:
	static inline const float kEndPause = 0.5f; // 端で止まる秒数

	KamataEngine::Vector3 start_{};
	KamataEngine::Vector3 end_{};
	KamataEngine::Vector3 velocity_{};
	float speed_ = 0.05f;
	float length_ = 1.0f;
	float s_ = 0.0f;   // 0..1 の位置
	float dir_ = 1.0f; // +1: start→end / -1: end→start
	CountdownTimer pause_;
};
