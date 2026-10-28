#include "FloorSwitch.h"
#include "ModelLibrary.h"
#include "GimmickManager.h"

using namespace KamataEngine;

void FloorSwitch::Initialize(const Vector3& floorTopCenter, int channel) {
	channel_ = channel;
	Vector3 center = floorTopCenter;
	center.y += 0.15f; // モデルの下端(-0.15)が床面に付くように
	InitBase(ModelLibrary::Get().Find("switch"), center, {1, 1, 1}, 0.8f, 0.4f);
}

void FloorSwitch::Update(GimmickContext& ctx) {
	if (!pressed_ && Intersects(ctx.playerAabb, Aabb::FromCenter(pos_, halfW_, halfH_))) {
		pressed_ = true;
		ctx.manager->SetSwitch(channel_, true);
	}
	worldTransform_.scale_.y = pressed_ ? 0.4f : 1.0f; // 押し込まれた見た目
	drawOffset_.y = pressed_ ? -0.06f : 0.0f;
	UpdateTransform();
}
