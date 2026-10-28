#include "EnemyFactory.h"
#include "ModelLibrary.h"
#include "FlyingEnemy.h"
#include "ShieldEnemy.h"
#include "ShooterEnemy.h"
#include "SplitEnemy.h"

using namespace KamataEngine;

std::unique_ptr<EnemyBase> EnemyFactory::Create(EnemyType type, const Vector3& position, MapChipField* field, const std::vector<SolidBox>* solids) {
	EnemyInitParams p;
	p.position = position;
	p.field = field;
	p.solids = solids;
	ModelLibrary& lib = ModelLibrary::Get();

	std::unique_ptr<EnemyBase> enemy;
	switch (type) {
	case EnemyType::kShooter:
		p.model = lib.Find("enemy_shooter");
		enemy = std::make_unique<ShooterEnemy>();
		break;
	case EnemyType::kFlyer:
		p.model = lib.Find("enemy_flyer");
		enemy = std::make_unique<FlyingEnemy>();
		break;
	case EnemyType::kShield:
		p.model = lib.Find("enemy_shield");
		enemy = std::make_unique<ShieldEnemy>();
		break;
	case EnemyType::kSplit:
		p.model = lib.Find("enemy_split");
		enemy = std::make_unique<SplitEnemy>();
		break;
	}
	enemy->Initialize(p);
	return enemy;
}

std::unique_ptr<EnemyBase> EnemyFactory::CreateSplitChild(const EnemyInitParams& parent, const Vector3& position, int generation) {
	EnemyInitParams p = parent;
	p.position = position;
	auto child = std::make_unique<SplitEnemy>();
	child->SetGeneration(generation);
	child->Initialize(p);
	return child;
}
