#include "ChasingWall.h"
#include "ModelLibrary.h"
#include <algorithm>

using namespace KamataEngine;

void ChasingWall::Initialize(const Vector3& center, float triggerX, float startSpeed, float maxSpeed) {
	triggerX_ = triggerX;
	speed_ = startSpeed;
	maxSpeed_ = maxSpeed;
	// モデルは 2(X) x 40(Y) x 3(Z)。X の ±1 が当たり判定
	InitBase(ModelLibrary::Get().Find("chase_wall"), center, {1, 1, 1}, 1.0f, 20.0f);
}

void ChasingWall::Update(GimmickContext& ctx) {
	if (!active_ && ctx.playerPos.x > triggerX_) {
		active_ = true;
	}
	if (active_) {
		speed_ = (std::min)(maxSpeed_, speed_ + kAcceleration);
		pos_.x += speed_;
	}
	UpdateTransform();
}

bool ChasingWall::GetHazard(Aabb& out, int& damage) const {
	out = Aabb::FromCenter(pos_, halfW_, halfH_);
	damage = kDamage;
	return true;
}
