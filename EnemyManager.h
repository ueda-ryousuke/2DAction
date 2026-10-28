#pragma once
#include "EnemyBase.h"
#include "EnemyFactory.h"
#include "ITargetProvider.h"
#include <memory>
#include <vector>

class ScoreManager;
class FrustumCuller;

// 敵の所有・更新・描画。倒れた敵の後始末（OnDeath・スコア加算）もここ。
// ホーミング弾の標的探しも担当する（ITargetProvider）。
class EnemyManager : public ITargetProvider {
public:
	void Initialize(MapChipField* field, BulletManager* bullets, ScoreManager* score, const std::vector<SolidBox>* solids);
	void Clear();

	void SpawnAt(EnemyType type, const KamataEngine::Vector3& position);
	void Add(std::unique_ptr<EnemyBase> enemy) { pending_.push_back(std::move(enemy)); } // 更新中に呼んでも安全

	void Update(const KamataEngine::Vector3& playerPos);
	void Draw(const KamataEngine::Camera& camera, const FrustumCuller& culler);

	std::vector<std::unique_ptr<EnemyBase>>& Enemies() { return enemies_; }
	bool FindNearestTarget(const KamataEngine::Vector3& from, float maxRange, KamataEngine::Vector3& out) const override;

private:
	static inline const float kActiveRange = 50.0f; // プレイヤーからこの距離内の敵だけ動かす

	MapChipField* field_ = nullptr;
	BulletManager* bullets_ = nullptr;
	ScoreManager* score_ = nullptr;
	const std::vector<SolidBox>* solids_ = nullptr;
	std::vector<std::unique_ptr<EnemyBase>> enemies_;
	std::vector<std::unique_ptr<EnemyBase>> pending_;
};
