#include "SwitchDoor.h"
#include "GameConfig.h"
#include "ModelLibrary.h"
#include "GimmickManager.h"
#include <algorithm>

using namespace KamataEngine;

void SwitchDoor::Initialize(const Vector3& center, int channel, float heightInBlocks) {
	channel_ = channel;
	height_ = heightInBlocks * GameConfig::kBlockSize;
	// モデルは 2 x 4 x 1。高さを合わせて Y だけ伸縮、幅は 1ブロック分(2.0)
	InitBase(ModelLibrary::Get().Find("door"), center, {GameConfig::kBlockSize / 2.0f, height_ / 4.0f, 1.0f}, GameConfig::kBlockSize * 0.5f, height_ * 0.5f);
}

void SwitchDoor::Update(GimmickContext& ctx) {
	if (ctx.manager->IsSwitchOn(channel_)) {
		open_ = (std::min)(1.0f, open_ + kOpenSpeed);
	}
	drawOffset_.y = open_ * height_; // 上へ持ち上がる
	visible_ = open_ < 1.0f;
	UpdateTransform();
}

bool SwitchDoor::GetSolid(SolidBox& out) const {
	if (open_ >= 0.5f) {
		return false; // 半分上がったら通れる
	}
	out.box = Aabb::FromCenter(pos_, halfW_, halfH_);
	return true;
}
