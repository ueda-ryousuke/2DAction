#include "CollisionManager.h"
#include "EnemyManager.h"
#include "GimmickManager.h"
#include "Player.h"
#include "BulletManager.h"

using namespace KamataEngine;

namespace {
constexpr int kInstantKillDamage = 99; // これ以上のダメージは無敵時間を無視して即死扱い
}

CollisionEvents CollisionManager::Run(Player& player, EnemyManager& enemies, BulletManager& bullets, GimmickManager& gimmicks) {
	CollisionEvents ev;
	const Aabb playerBox = player.GetAabb();

	// ① 弾の判定
	for (auto& b : bullets.Bullets()) {
		if (!b->IsAlive()) {
			continue;
		}
		if (b->Owner() == BulletOwner::kPlayer) {
			for (auto& e : enemies.Enemies()) {
				if (e->IsDead() || !Intersects(b->GetAabb(), e->GetAabb())) {
					continue;
				}
				const float dirX = b->Velocity().x >= 0.0f ? 1.0f : -1.0f;
				if (!e->TakeDamage(b->Damage(), dirX)) {
					ev.shotBlocked = true;
				}
				if (e->IsDead()) {
					ev.enemyDefeated = true;
				}
				b->Kill();
				break;
			}
		} else if (Intersects(b->GetAabb(), playerBox)) {
			if (player.TakeDamage(b->Damage(), b->Position())) {
				ev.playerDamaged = true;
			}
			b->Kill();
		}
	}

	// ③ 敵との接触: 上から落ちながら当たれば踏みつけ、それ以外は接触ダメージ
	for (auto& e : enemies.Enemies()) {
		if (e->IsDead() || !Intersects(playerBox, e->GetAabb())) {
			continue;
		}
		const Aabb enemyBox = e->GetAabb();
		const bool stomp = player.GetVelocity().y < 0.0f && playerBox.min.y > enemyBox.Center().y - 0.2f;
		if (stomp) {
			e->TakeDamage(1, 0.0f); // attackDirX=0 → 盾でも防げない
			player.Bounce();
			if (e->IsDead()) {
				ev.enemyDefeated = true;
			}
		} else if (player.TakeDamage(e->ContactDamage(), e->Position())) {
			ev.playerDamaged = true;
		}
	}

	// ④ 危険ギミック（落下天井・押しつぶし・迫る壁）
	int damage = 0;
	Vector3 source{};
	if (gimmicks.CheckHazard(playerBox, damage, source)) {
		if (damage >= kInstantKillDamage) {
			player.Kill();
			ev.playerDamaged = true;
		} else if (player.TakeDamage(damage, source)) {
			ev.playerDamaged = true;
		}
	}
	return ev;
}
