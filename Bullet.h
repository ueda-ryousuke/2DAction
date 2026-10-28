#pragma once
#include "Aabb.h"
#include "KamataEngine.h"
#include "ITargetProvider.h"

enum class BulletOwner { kPlayer, kEnemy };

// 弾の基底クラス。直進弾/ホーミング弾はこれを継承して Update だけ変える。
class Bullet {
public:
	virtual ~Bullet() = default;

	void Initialize(KamataEngine::Model* model, const KamataEngine::Vector3& position, const KamataEngine::Vector3& velocity, BulletOwner owner, int damage, float halfSize,
	                float lifeSeconds, float modelScale = 1.0f);
	virtual void Update(const ITargetProvider* targets) = 0;
	void Draw(const KamataEngine::Camera& camera);

	void Kill() { alive_ = false; }
	bool IsAlive() const { return alive_; }
	BulletOwner Owner() const { return owner_; }
	int Damage() const { return damage_; }
	const KamataEngine::Vector3& Position() const { return position_; }
	const KamataEngine::Vector3& Velocity() const { return velocity_; }
	Aabb GetAabb() const { return Aabb::FromCenter(position_, halfSize_, halfSize_); }

protected:
	void Integrate(); // 位置更新・寿命・向き・行列

	KamataEngine::Model* model_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Vector3 position_{};
	KamataEngine::Vector3 velocity_{};
	BulletOwner owner_ = BulletOwner::kPlayer;
	int damage_ = 1;
	float halfSize_ = 0.3f;
	float life_ = 0.0f;
	bool alive_ = true;
};
