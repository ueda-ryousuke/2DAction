#include "StageBuilder.h"
#include "GameConfig.h"
#include "EnemyManager.h"
#include "ChasingWall.h"
#include "CrumbleFloor.h"
#include "CrusherTrap.h"
#include "FallingCeiling.h"
#include "FloorSwitch.h"
#include "GimmickManager.h"
#include "GoalFlag.h"
#include "MovingPlatform.h"
#include "SlipperyFloor.h"
#include "SwitchDoor.h"
#include "MapChipField.h"

using namespace KamataEngine;

namespace {

constexpr int kGroundRow = 18; // 地面の最上段（CSV の下2行が地面）

struct Ctx {
	MapChipField& field;
	GimmickManager& gimmicks;
	EnemyManager& enemies;

	Vector3 Cell(int x, int y) { return field.GetMapChipPositionByIndex(static_cast<uint32_t>(x), static_cast<uint32_t>(y)); }
	float GroundTop() { return Cell(0, kGroundRow).y + GameConfig::kBlockSize * 0.5f; }
	Vector3 OnGround(int x) { return {Cell(x, kGroundRow).x, GroundTop(), 0.0f}; }

	template <class T, class... Args> void Add(Args&&... args) {
		auto g = std::make_unique<T>();
		g->Initialize(std::forward<Args>(args)...);
		gimmicks.Add(std::move(g));
	}
	void Goal(int x, float footY) { Add<GoalFlag>(Vector3{Cell(x, 0).x, footY, 0.0f}); }
};

// ---------------------------------------------------------------- ステージ1
Vector3 Stage1(Ctx& c) {
	// 穴(30-32)に崩れる床の橋
	for (int x = 30; x <= 32; ++x) {
		c.Add<CrumbleFloor>(c.Cell(x, kGroundRow));
	}
	// 穴(62-66)に往復する移動床
	{
		Vector3 a = c.Cell(63, kGroundRow), b = c.Cell(65, kGroundRow);
		a.y = b.y = c.GroundTop() - 0.3f;
		c.Add<MovingPlatform>(a, b, 2.0f, 0.05f);
	}
	// スイッチ(70)を踏むと、壁(80)の扉が開く。扉は CSV の壁(0-13行)の真下 14-17 行を埋める
	c.Add<FloorSwitch>(c.OnGround(70), 1);
	c.Add<SwitchDoor>(Vector3{c.Cell(80, 0).x, c.GroundTop() + 4.0f, 0.0f}, 1, 4.0f);
	c.Goal(95, c.GroundTop());

	c.enemies.SpawnAt(EnemyType::kShooter, c.Cell(24, 17));
	c.enemies.SpawnAt(EnemyType::kFlyer, c.Cell(36, 9));
	c.enemies.SpawnAt(EnemyType::kSplit, c.Cell(52, 3));
	c.enemies.SpawnAt(EnemyType::kFlyer, c.Cell(70, 10));
	c.enemies.SpawnAt(EnemyType::kShield, c.Cell(74, 17));
	c.enemies.SpawnAt(EnemyType::kShooter, c.Cell(88, 17));
	c.enemies.SpawnAt(EnemyType::kShield, c.Cell(91, 17));
	return c.Cell(2, 16);
}

// ---------------------------------------------------------------- ステージ2
Vector3 Stage2(Ctx& c) {
	// 滑る床 A(8-27) と B(46-60)。CSV では空けてあり、ここで床を置く
	auto ice = [&](int x0, int x1) {
		float cx = (c.Cell(x0, kGroundRow).x + c.Cell(x1, kGroundRow).x) * 0.5f;
		float cy = (c.Cell(x0, kGroundRow).y + c.Cell(x0, kGroundRow + 1).y) * 0.5f;
		c.Add<SlipperyFloor>(Vector3{cx, cy, 0.0f}, static_cast<float>(x1 - x0 + 1), 2.0f);
	};
	ice(8, 27);
	ice(46, 60);

	// 落下する天井
	for (int x : {34, 38}) {
		c.Add<FallingCeiling>(c.Cell(x, 8), c.GroundTop() + 1.0f);
	}
	// 押しつぶしトラップ（時間差）
	float delay = 1.0f;
	for (int x : {70, 74, 78}) {
		c.Add<CrusherTrap>(c.Cell(x, 9), c.GroundTop() + 1.05f, delay);
		delay += 1.0f;
	}
	// 最後は壁キックの縦穴(86-89)を登った先がゴール
	c.Goal(94, c.Cell(94, 6).y + GameConfig::kBlockSize * 0.5f);

	c.enemies.SpawnAt(EnemyType::kFlyer, c.Cell(15, 10));
	c.enemies.SpawnAt(EnemyType::kFlyer, c.Cell(22, 8));
	c.enemies.SpawnAt(EnemyType::kShooter, c.Cell(31, 17));
	c.enemies.SpawnAt(EnemyType::kShield, c.Cell(42, 17));
	c.enemies.SpawnAt(EnemyType::kSplit, c.Cell(54, 17));
	c.enemies.SpawnAt(EnemyType::kShooter, c.Cell(66, 17));
	c.enemies.SpawnAt(EnemyType::kFlyer, c.Cell(82, 9));
	return c.Cell(2, 16);
}

// ---------------------------------------------------------------- ステージ3（強制スクロール）
Vector3 Stage3(Ctx& c) {
	// 左から迫る壁。プレイヤーが x=8 を越えたら動き出す
	c.Add<ChasingWall>(Vector3{c.Cell(0, 10).x - 16.0f, c.Cell(0, 10).y, 0.0f}, c.Cell(8, 0).x, 0.10f, 0.20f); // 初速 / 最高速（1フレームあたりの移動量。プレイヤーの最高速は 0.4）
	for (int x = 50; x <= 52; ++x) {
		c.Add<CrumbleFloor>(c.Cell(x, kGroundRow));
	}
	{
		Vector3 a = c.Cell(67, kGroundRow), b = c.Cell(68, kGroundRow);
		a.y = b.y = c.GroundTop() - 0.3f;
		c.Add<MovingPlatform>(a, b, 2.0f, 0.06f);
	}
	c.Goal(96, c.GroundTop());

	c.enemies.SpawnAt(EnemyType::kShooter, c.Cell(30, 17));
	c.enemies.SpawnAt(EnemyType::kFlyer, c.Cell(45, 10));
	c.enemies.SpawnAt(EnemyType::kSplit, c.Cell(60, 17));
	c.enemies.SpawnAt(EnemyType::kShield, c.Cell(76, 17));
	c.enemies.SpawnAt(EnemyType::kFlyer, c.Cell(88, 9));
	return c.Cell(4, 16);
}

} // namespace

Vector3 StageBuilder::Build(int stageIndex, MapChipField& field, GimmickManager& gimmicks, EnemyManager& enemies) {
	Ctx c{field, gimmicks, enemies};
	switch (stageIndex) {
	case 1: return Stage2(c);
	case 2: return Stage3(c);
	default: return Stage1(c);
	}
}
