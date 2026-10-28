#include "BulletManager.h"
#include "MathUtil.h"
#include "ModelLibrary.h"
#include "MapChipField.h"
#include "HomingBullet.h"
#include "StraightBullet.h"
#include <algorithm>

using namespace KamataEngine;

void BulletManager::Initialize(MapChipField* field, const ITargetProvider* targets) {
	field_ = field;
	targets_ = targets;
	bullets_.clear();
}

void BulletManager::SpawnPlayerBullet(FireMode mode, const Vector3& position, float directionX) {
	const float dir = directionX >= 0.0f ? 1.0f : -1.0f;
	if (mode == FireMode::kHoming) {
		auto b = std::make_unique<HomingBullet>();
		b->Initialize(ModelLibrary::Get().Find("bullet_homing"), position, {dir * 0.5f, 0.0f, 0.0f}, BulletOwner::kPlayer, 2, 0.4f, 3.0f, 1.6f);
		bullets_.push_back(std::move(b));
	} else {
		auto b = std::make_unique<StraightBullet>();
		b->Initialize(ModelLibrary::Get().Find("bullet_straight"), position, {dir * 0.8f, 0.0f, 0.0f}, BulletOwner::kPlayer, 1, 0.3f, 1.5f, 1.4f);
		bullets_.push_back(std::move(b));
	}
}

void BulletManager::SpawnEnemyBullet(const Vector3& position, const Vector3& directionXY, float speed) {
	auto b = std::make_unique<StraightBullet>();
	Vector3 v = MathUtil::Scale(MathUtil::NormalizeXY(directionXY), speed);
	b->Initialize(ModelLibrary::Get().Find("bullet_straight"), position, v, BulletOwner::kEnemy, 1, 0.35f, 4.0f, 1.8f);
	bullets_.push_back(std::move(b));
}

bool BulletManager::HitsWall(const Vector3& p) const {
	if (!field_) {
		return false;
	}
	MapChipField::IndexSet idx = field_->GetMapChipIndexSetByPosition(p);
	return field_->GetMapChipTypeByIndex(idx.xIndex, idx.yIndex) == MapChipType::kBlock;
}

void BulletManager::Update() {
	for (auto& b : bullets_) {
		b->Update(targets_);
		if (HitsWall(b->Position())) {
			b->Kill();
		}
	}
	bullets_.erase(std::remove_if(bullets_.begin(), bullets_.end(), [](const std::unique_ptr<Bullet>& b) { return !b->IsAlive(); }), bullets_.end());
}

void BulletManager::Draw(const Camera& camera) {
	for (auto& b : bullets_) {
		b->Draw(camera);
	}
}
