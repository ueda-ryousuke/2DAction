#pragma once
#include "KamataEngine.h"
class Player;

class Enemy {
public:
	struct AABB {
		KamataEngine::Vector3 min;
		KamataEngine::Vector3 max;
	};

	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void Update(Player* player);
	void Draw();

	void OnCollision(const Player* player);
	AABB GetAABB();
	KamataEngine::Vector3 GetWorldPosition();

private:
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Camera* camera_ = nullptr;
	KamataEngine::Model* model_ = nullptr;

	static inline const float kWalkSpeed = 0.05f;
	KamataEngine::Vector3 velocity_ = {};
	// 最初の角度[度]
	static inline const float kWalkMotionAngleStart = -10.0f;
	// 最後の角度[度]
	static inline const float kWalkMotionAngleEnd = 10.0f;
	// アニメーションの周期[秒]
	static inline const float kWalkMotionTime = 2.0f;
	// 経過時間タイマー
	float walkTimer_ = 0.0f;
	// 敵の当たり判定のサイズ
	static inline const float kWidth = 3.0f;
	static inline const float kHeight = 3.0f;
};