#include "Player.h"
#include "MathUtil.h"
#include "Gimmick.h"
#include "Matri4x4.h"
#include "PlayerParams.h"
#include "BulletManager.h"
#include <algorithm>
#include <cassert>

using namespace KamataEngine;
using namespace PlayerParams;

void Player::Initialize(Model* model, Camera* camera, const Vector3& position) {
	assert(model);
	model_ = model;
	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	motor_.Reset();
	jump_.Reset();
	health_.Initialize(kMaxHp);
	animator_.Initialize(kModelScale);
	weapon_.Initialize();
	collider_.SetHalfSize(kWidth * 0.5f, kHeight * 0.5f);
	onGround_ = false;
	groundSurface_ = SurfaceType::kNormal;
	groundCarry_ = {};
	events_ = {};
	UpdateMatrix();
}

void Player::SetMapChipField(MapChipField* field) { collider_.Initialize(field, kWidth * 0.5f, kHeight * 0.5f); }

// =========================================================
// 更新: 入力 → ジャンプ判断 → 速度 → 衝突解決 → 状態更新
// =========================================================
void Player::Update() {
	const PlayerInputState input = health_.IsDead() ? PlayerInputState{} : PlayerInput::Poll();

	// ① 今フレーム冒頭の接触状態（前フレーム終了時点の位置で調べる）
	JumpController::Contact contact;
	contact.onGround = onGround_;
	WallInfo wall = collider_.ProbeWall(worldTransform_.translation_);
	contact.wallLeft = wall.left;
	contact.wallRight = wall.right;
	jump_.Update(contact);
	const bool locked = jump_.IsControlLocked();

	// ② 向き（壁キック直後などの入力ロック中は変えない）
	if (!locked && input.moveX != 0.0f) {
		animator_.FaceTo(input.moveX > 0.0f ? LRDirection::kRight : LRDirection::kLeft, worldTransform_.rotation_.y);
	}

	// ③ ジャンプ / 二段ジャンプ / 壁キック
	JumpController::Action action = jump_.TryJump(input, contact, motor_);
	if (action == JumpController::Action::kWallKick) {
		animator_.FaceTo(jump_.LastKickDirection() > 0 ? LRDirection::kRight : LRDirection::kLeft, worldTransform_.rotation_.y);
		events_.wallKicked = true;
	}
	if (action != JumpController::Action::kNone) {
		animator_.OnJump();
		events_.jumped = true;
		onGround_ = false;
		contact.onGround = false;
	}
	jump_.ApplyJumpCut(input, motor_);

	// ④ 速度の更新
	motor_.ApplyHorizontal(input.moveX, contact.onGround, groundSurface_, locked);
	if (contact.onGround && motor_.Velocity().y <= 0.0f) {
		motor_.SetVelocityY(0.0f);
	} else {
		motor_.ApplyGravity(jump_.IsWallSliding(contact, input.moveX, motor_.Velocity().y));
	}

	// ⑤ 衝突解決して位置を更新（乗っている足場の動きも一緒に加える）
	CollisionResult info;
	info.moveAmount = {motor_.Velocity().x + groundCarry_.x, motor_.Velocity().y + groundCarry_.y, 0.0f};
	collider_.Resolve(worldTransform_.translation_, info);
	if (info.hitTop && motor_.Velocity().y > 0.0f) {
		motor_.SetVelocityY(0.0f); // 天井に頭をぶつけた
	}
	if (info.hitLeft || info.hitRight) {
		motor_.SetVelocityX(0.0f); // 壁にぶつかった（壁キックの判断は ProbeWall 側で行う）
	}
	worldTransform_.translation_.x += info.moveAmount.x;
	worldTransform_.translation_.y += info.moveAmount.y;

	// ⑥ 接地状態の更新
	bool landedNow = false;
	UpdateGroundState(landedNow);
	if (landedNow) {
		animator_.OnLand();
		events_.landed = true;
	}

	// ⑦ 武器・体力・見た目
	UpdateWeapon(input);
	health_.Update();
	animator_.Update();
	UpdateMatrix();
}

void Player::UpdateGroundState(bool& landedNow) {
	const bool was = onGround_;
	GroundInfo g = collider_.ProbeGround(worldTransform_.translation_);
	onGround_ = g.onGround && motor_.Velocity().y <= 0.0f;
	groundSurface_ = onGround_ ? g.surface : SurfaceType::kNormal;
	groundCarry_ = onGround_ ? g.velocity : Vector3{};
	if (onGround_ && g.owner) {
		g.owner->OnPlayerStand(); // 崩れる床など
	}
	landedNow = !was && onGround_;
	if (landedNow) {
		motor_.SetVelocityY(0.0f);
	}
}

void Player::UpdateWeapon(const PlayerInputState& input) {
	weapon_.Update();
	if (input.switchWeaponPressed) {
		weapon_.ToggleMode();
	}
	if (input.reloadPressed) {
		weapon_.StartReload();
	}
	if (input.shootPressed && bullets_ && weapon_.TryFire()) {
		const float dir = animator_.Facing() == LRDirection::kRight ? 1.0f : -1.0f;
		Vector3 muzzle = {worldTransform_.translation_.x + dir * kMuzzleOffsetX, worldTransform_.translation_.y + kMuzzleOffsetY, worldTransform_.translation_.z};
		bullets_->SpawnPlayerBullet(weapon_.Mode(), muzzle, dir);
		events_.fired = true;
	}
}

bool Player::TakeDamage(int amount, const Vector3& sourcePosition) {
	if (!health_.TakeDamage(amount)) {
		return false;
	}
	const float away = worldTransform_.translation_.x >= sourcePosition.x ? 1.0f : -1.0f;
	motor_.SetVelocity(away * kKnockbackX, kKnockbackY);
	jump_.LockControl(kKnockbackLockTime);
	onGround_ = false;
	events_.damaged = true;
	return true;
}

void Player::Bounce() {
	motor_.SetVelocityY(0.5f);
	onGround_ = false;
	animator_.OnJump();
}

Aabb Player::GetAabb() const { return Aabb::FromCenter(worldTransform_.translation_, kWidth * 0.5f, kHeight * 0.5f); }

Player::Events Player::ConsumeEvents() {
	Events e = events_;
	events_ = {};
	return e;
}

void Player::UpdateMatrix() {
	animator_.Apply(worldTransform_);
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

void Player::Draw() {
	if (health_.BlinkVisible()) {
		model_->Draw(worldTransform_, *camera_);
	}
}
