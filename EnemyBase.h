#pragma once
#include "MapCollider.h"
#include "Aabb.h"
#include "CountdownTimer.h"
#include "KamataEngine.h"
#include <vector>

class EnemyManager;
class BulletManager;
class MapChipField;

// 敵が毎フレーム参照する外部情報
struct EnemyContext {
	KamataEngine::Vector3 playerPos{};
	BulletManager* bullets = nullptr;
	EnemyManager* manager = nullptr;
};

// 敵の生成時に渡す情報（分裂の子を作るときにも使い回す）
struct EnemyInitParams {
	KamataEngine::Model* model = nullptr;
	KamataEngine::Vector3 position{};
	MapChipField* field = nullptr;
	const std::vector<SolidBox>* solids = nullptr;
};

// 敵の基底クラス。位置・体力・マップ衝突・描画の共通部分を持つ。
// 種類ごとの行動は Update() を override して書く。
class EnemyBase {
public:
	virtual ~EnemyBase() = default;

	void Initialize(const EnemyInitParams& params);
	virtual void Update(EnemyContext& ctx) = 0;
	virtual void Draw(const KamataEngine::Camera& camera);

	// attackDirX: 攻撃が進む向き(+1:右へ / -1:左へ / 0:上から踏まれた等)。ダメージが通ったら true。
	virtual bool TakeDamage(int damage, float attackDirX);
	virtual void OnDeath(EnemyContext& ctx) { (void)ctx; } // 倒された瞬間（分裂など）
	virtual int ScoreValue() const { return 100; }

	bool IsDead() const { return hp_ <= 0; }
	int ContactDamage() const { return 1; }
	const KamataEngine::Vector3& Position() const { return pos_; }
	Aabb GetAabb() const { return Aabb::FromCenter(pos_, halfW_, halfH_); }
	const EnemyInitParams& InitParams() const { return params_; }
	void SetVelocity(float x, float y) { vel_.x = x; vel_.y = y; }

protected:
	virtual void OnInitialize() {}
	// 見た目の設定: scale 倍した描画で、モデルの下端(bottomY)が当たり判定の足元に合うようにする
	void SetVisual(float scale, float modelBottomY);
	void SetBody(int hp, float halfW, float halfH);

	void StepPhysics();     // 重力 + 衝突解決 + 接地更新（地上の敵用）
	bool IsWallAhead() const { return wallHit_; }
	bool IsLedgeAhead(float dir) const;
	void Face(float dirX);  // 向き(+1右/-1左)を変える
	void UpdateTransform(float extraRotZ = 0.0f);

	KamataEngine::Model* model_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;
	EnemyInitParams params_;
	MapCollider collider_;

	KamataEngine::Vector3 pos_{};
	KamataEngine::Vector3 vel_{};
	float halfW_ = 0.8f;
	float halfH_ = 0.8f;
	int hp_ = 1;
	float facing_ = -1.0f; // +1:右 / -1:左
	bool onGround_ = false;
	bool wallHit_ = false;
	float visualScale_ = 1.0f;
	float visualOffsetY_ = 0.0f;
	CountdownTimer flash_; // 被弾/防御時の小さな演出
	static inline const float kGravity = 0.02f;
	static inline const float kMaxFall = 0.5f;
};
