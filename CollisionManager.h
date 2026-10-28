#pragma once

class Player;
class EnemyManager;
class BulletManager;
class GimmickManager;

// そのフレームに起きた出来事（演出側が見る）
struct CollisionEvents {
	bool playerDamaged = false;
	bool enemyDefeated = false;
	bool shotBlocked = false; // 盾で防がれた
};

// ゲーム内の「当たったら何が起きるか」をまとめて処理する。
//   ① プレイヤーの弾 × 敵 / ② 敵の弾 × プレイヤー / ③ プレイヤー × 敵（踏みつけ or 接触ダメージ） / ④ プレイヤー × 危険ギミック
class CollisionManager {
public:
	static CollisionEvents Run(Player& player, EnemyManager& enemies, BulletManager& bullets, GimmickManager& gimmicks);
};
