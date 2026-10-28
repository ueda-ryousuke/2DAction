#include "EnemyManager.h"
#include "FrustumCuller.h"
#include "MathUtil.h"
#include "ScoreManager.h"
#include <algorithm>
#include <cmath>

using namespace KamataEngine;

void EnemyManager::Initialize(MapChipField* field, BulletManager* bullets, ScoreManager* score, const std::vector<SolidBox>* solids) {
	field_ = field;
	bullets_ = bullets;
	score_ = score;
	solids_ = solids;
	Clear();
}

void EnemyManager::Clear() {
	enemies_.clear();
	pending_.clear();
}

void EnemyManager::SpawnAt(EnemyType type, const Vector3& position) { Add(EnemyFactory::Create(type, position, field_, solids_)); }

void EnemyManager::Update(const Vector3& playerPos) {
	EnemyContext ctx;
	ctx.playerPos = playerPos;
	ctx.bullets = bullets_;
	ctx.manager = this;

	for (auto& e : enemies_) {
		if (std::fabs(e->Position().x - playerPos.x) < kActiveRange) {
			e->Update(ctx);
		}
	}

	// 倒れた敵の後始末（OnDeath で新しい敵が pending_ に積まれる）
	for (auto& e : enemies_) {
		if (e->IsDead()) {
			e->OnDeath(ctx);
			if (score_) {
				score_->Add(e->ScoreValue(), e->Position());
			}
		}
	}
	enemies_.erase(std::remove_if(enemies_.begin(), enemies_.end(), [](const std::unique_ptr<EnemyBase>& e) { return e->IsDead(); }), enemies_.end());

	for (auto& e : pending_) {
		enemies_.push_back(std::move(e));
	}
	pending_.clear();
}

void EnemyManager::Draw(const Camera& camera, const FrustumCuller& culler) {
	for (auto& e : enemies_) {
		if (culler.IsVisible(e->Position(), 3.0f)) {
			e->Draw(camera);
		}
	}
}

bool EnemyManager::FindNearestTarget(const Vector3& from, float maxRange, Vector3& out) const {
	float best = maxRange;
	bool found = false;
	for (const auto& e : enemies_) {
		float d = MathUtil::LengthXY(MathUtil::Sub(e->Position(), from));
		if (d < best) {
			best = d;
			out = e->Position();
			found = true;
		}
	}
	return found;
}
