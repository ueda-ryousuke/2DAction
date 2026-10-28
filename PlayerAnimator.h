#pragma once
#include "KamataEngine.h"
#include "PlayerTypes.h"

// 見た目の演出担当: 旋回イージング + ジャンプ/着地のスクワッシュ&ストレッチ。
class PlayerAnimator {
public:
	void Initialize(float baseScale);
	void Update();
	void FaceTo(LRDirection dir, float currentRotationY); // 向き変更を開始（変わらないなら何もしない）
	void OnJump() { squash_ = 0.25f; }                    // 縦に伸びる
	void OnLand() { squash_ = -0.25f; }                   // 縦に潰れる
	void Apply(KamataEngine::WorldTransform& wt) const;   // rotation_.y と scale_ を反映

	LRDirection Facing() const { return facing_; }

private:
	LRDirection facing_ = LRDirection::kRight;
	float turnFirstRotationY_ = 0.0f;
	float turnTimer_ = 0.0f;
	float yaw_ = 0.0f;
	float squash_ = 0.0f;
	float baseScale_ = 1.0f;
};
