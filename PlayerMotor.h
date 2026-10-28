#pragma once
#include "SolidBox.h"
#include "KamataEngine.h"

// 速度(velocity)の更新だけを担当。位置の更新と衝突は Player / MapCollider 側。
class PlayerMotor {
public:
	void Reset() { velocity_ = {}; }

	// 左右の加減速。locked の間は入力を無視（壁キック直後・ノックバック中）。
	void ApplyHorizontal(float moveX, bool onGround, SurfaceType surface, bool locked);
	// 重力。wallSliding のときは落下速度の上限が小さくなる。
	void ApplyGravity(bool wallSliding);

	const KamataEngine::Vector3& Velocity() const { return velocity_; }
	void SetVelocity(float x, float y) { velocity_.x = x; velocity_.y = y; }
	void SetVelocityX(float x) { velocity_.x = x; }
	void SetVelocityY(float y) { velocity_.y = y; }
	void ScaleVelocityY(float k) { velocity_.y *= k; }

private:
	KamataEngine::Vector3 velocity_{};
};
