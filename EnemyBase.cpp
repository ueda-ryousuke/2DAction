#include "EnemyBase.h"
#include "MathUtil.h"
#include "Matri4x4.h"
#include <algorithm>
#include <cassert>
#include <cmath>

using namespace KamataEngine;

void EnemyBase::Initialize(const EnemyInitParams& params) {
	assert(params.model);
	params_ = params;
	model_ = params.model;
	pos_ = params.position;
	vel_ = {};
	worldTransform_.Initialize();
	collider_.Initialize(params.field, halfW_, halfH_);
	collider_.SetSolids(params.solids);
	OnInitialize(); // 派生クラスが SetBody/SetVisual を呼ぶ
	collider_.SetHalfSize(halfW_, halfH_);
	Face(-1.0f);
	UpdateTransform();
}

void EnemyBase::SetVisual(float scale, float modelBottomY) {
	visualScale_ = scale;
	visualOffsetY_ = -halfH_ - modelBottomY * scale;
	worldTransform_.scale_ = {scale, scale, scale};
}

void EnemyBase::SetBody(int hp, float halfW, float halfH) {
	hp_ = hp;
	halfW_ = halfW;
	halfH_ = halfH;
}

void EnemyBase::Face(float dirX) {
	if (dirX == 0.0f) {
		return;
	}
	facing_ = dirX > 0.0f ? 1.0f : -1.0f;
	worldTransform_.rotation_.y = facing_ > 0.0f ? MathUtil::kPi * 0.5f : MathUtil::kPi * 1.5f;
}

void EnemyBase::StepPhysics() {
	flash_.Update();
	vel_.y = onGround_ ? 0.0f : (std::max)(vel_.y - kGravity, -kMaxFall);

	CollisionResult r;
	r.moveAmount = {vel_.x, vel_.y, 0.0f};
	collider_.Resolve(pos_, r);
	pos_.x += r.moveAmount.x;
	pos_.y += r.moveAmount.y;
	wallHit_ = r.hitLeft || r.hitRight;
	if (r.hitTop) {
		vel_.y = 0.0f;
	}

	GroundInfo g = collider_.ProbeGround(pos_);
	onGround_ = g.onGround && vel_.y <= 0.0f;
	if (onGround_) {
		pos_.x += g.velocity.x; // 動く床に乗っていれば運ばれる
		pos_.y += g.velocity.y;
	}
}

bool EnemyBase::IsLedgeAhead(float dir) const {
	Vector3 probe = {pos_.x + dir * (halfW_ + 0.4f), pos_.y - halfH_ - 0.4f, pos_.z};
	return !collider_.IsSolidAt(probe);
}

bool EnemyBase::TakeDamage(int damage, float attackDirX) {
	(void)attackDirX;
	hp_ -= damage;
	flash_.Start(0.15f);
	return true;
}

void EnemyBase::UpdateTransform(float extraRotZ) {
	worldTransform_.translation_ = {pos_.x, pos_.y + visualOffsetY_, pos_.z};
	worldTransform_.rotation_.z = extraRotZ;
	// 被弾の瞬間だけ少し大きく見せる
	float pop = flash_.IsActive() ? 1.0f + 0.15f * flash_.Remaining() / 0.15f : 1.0f;
	worldTransform_.scale_ = {visualScale_ * pop, visualScale_ * pop, visualScale_ * pop};
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

void EnemyBase::Draw(const Camera& camera) { model_->Draw(worldTransform_, camera); }
