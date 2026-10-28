#pragma once
#include "EnemyBase.h"
#include <memory>

enum class EnemyType { kShooter, kFlyer, kShield, kSplit };

// 敵の生成を1か所に集約（モデル名の対応づけもここだけ）。
class EnemyFactory {
public:
	static std::unique_ptr<EnemyBase> Create(EnemyType type, const KamataEngine::Vector3& position, MapChipField* field, const std::vector<SolidBox>* solids);
	static std::unique_ptr<EnemyBase> CreateSplitChild(const EnemyInitParams& parent, const KamataEngine::Vector3& position, int generation);
};
