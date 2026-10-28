#pragma once
#include "MapCollider.h"
#include "Aabb.h"
#include "KamataEngine.h"
#include "JumpController.h"
#include "PlayerAnimator.h"
#include "PlayerHealth.h"
#include "PlayerInput.h"
#include "PlayerMotor.h"
#include "Weapon.h"
#include <vector>

class MapChipField;
class BulletManager;

// プレイヤー本体。各担当クラスを束ねる「司令塔」で、ロジックは持たない。
//   入力 PlayerInput / 速度 PlayerMotor / ジャンプ判断 JumpController / 衝突 MapCollider
//   体力 PlayerHealth / 見た目 PlayerAnimator / 武器 Weapon
class Player {
public:
	// 演出・SEのフック。GameScene が毎フレーム ConsumeEvents() で受け取る
	struct Events {
		bool jumped = false;
		bool wallKicked = false;
		bool landed = false;
		bool fired = false;
		bool damaged = false;
	};

	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void Update();
	void Draw();

	// 依存の注入
	void SetMapChipField(MapChipField* field);
	void SetSolids(const std::vector<SolidBox>* solids) { collider_.SetSolids(solids); }
	void SetBulletManager(BulletManager* bullets) { bullets_ = bullets; }

	// 戦闘
	bool TakeDamage(int amount, const KamataEngine::Vector3& sourcePosition); // ノックバック付き。受けたら true
	void Kill() { health_.Kill(); } // 無敵無視の即死
	void Bounce();                                                           // 敵を踏んだ反動

	// ゲッター（CameraController が使う元の3つはシグネチャ維持）
	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }
	const KamataEngine::Vector3& GetVelocity() const { return motor_.Velocity(); }
	KamataEngine::Vector3 GetWorldPosition() const { return worldTransform_.translation_; }
	Aabb GetAabb() const;
	bool IsOnGround() const { return onGround_; }
	bool IsDead() const { return health_.IsDead(); }
	const PlayerHealth& GetHealth() const { return health_; }
	const Weapon& GetWeapon() const { return weapon_; }
	Events ConsumeEvents();

private:
	void UpdateWeapon(const PlayerInputState& input);
	void UpdateGroundState(bool& landedNow);
	void UpdateMatrix();

	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;

	PlayerMotor motor_;
	JumpController jump_;
	MapCollider collider_;
	PlayerHealth health_;
	PlayerAnimator animator_;
	Weapon weapon_;
	BulletManager* bullets_ = nullptr;

	bool onGround_ = false;
	SurfaceType groundSurface_ = SurfaceType::kNormal;
	KamataEngine::Vector3 groundCarry_{}; // 乗っている足場の移動量
	Events events_;
};
