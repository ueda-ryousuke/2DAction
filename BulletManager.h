#pragma once
#include "Bullet.h"
#include "Weapon.h"
#include <memory>
#include <vector>

class MapChipField;

// 全ての弾を所有・更新・描画する。壁に当たった弾の消滅もここ。
class BulletManager {
public:
	void Initialize(MapChipField* field, const ITargetProvider* targets);

	void SpawnPlayerBullet(FireMode mode, const KamataEngine::Vector3& position, float directionX);
	void SpawnEnemyBullet(const KamataEngine::Vector3& position, const KamataEngine::Vector3& directionXY, float speed);

	void Update();
	void Draw(const KamataEngine::Camera& camera);
	void Clear() { bullets_.clear(); }

	std::vector<std::unique_ptr<Bullet>>& Bullets() { return bullets_; }

private:
	bool HitsWall(const KamataEngine::Vector3& p) const;

	MapChipField* field_ = nullptr;
	const ITargetProvider* targets_ = nullptr;
	std::vector<std::unique_ptr<Bullet>> bullets_;
};
