#include "Bullet.h"
#include "GameConfig.h"
#include "MathUtil.h"
#include "Matri4x4.h"

using namespace KamataEngine;

void Bullet::Initialize(Model* model, const Vector3& position, const Vector3& velocity, BulletOwner owner, int damage, float halfSize, float lifeSeconds, float modelScale) {
	model_ = model;
	position_ = position;
	velocity_ = velocity;
	owner_ = owner;
	damage_ = damage;
	halfSize_ = halfSize;
	life_ = lifeSeconds;
	alive_ = true;
	worldTransform_.Initialize();
	worldTransform_.scale_ = {modelScale, modelScale, modelScale};
	worldTransform_.translation_ = position_;
	MathUtil::FaceVelocityXY(velocity_, worldTransform_.rotation_);
}

void Bullet::Integrate() {
	position_ = MathUtil::Add(position_, velocity_);
	life_ -= GameConfig::kDeltaTime;
	if (life_ <= 0.0f) {
		alive_ = false;
	}
	MathUtil::FaceVelocityXY(velocity_, worldTransform_.rotation_);
	worldTransform_.translation_ = position_;
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

void Bullet::Draw(const Camera& camera) {
	if (model_) {
		model_->Draw(worldTransform_, camera);
	}
}
