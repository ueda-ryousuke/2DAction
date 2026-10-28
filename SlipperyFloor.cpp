#include "SlipperyFloor.h"
#include "GameConfig.h"
#include "ModelLibrary.h"

using namespace KamataEngine;

void SlipperyFloor::Initialize(const Vector3& center, float widthInBlocks, float heightInBlocks) {
	const float bs = GameConfig::kBlockSize;
	InitBase(ModelLibrary::Get().Find("block_ice"), center, {widthInBlocks, heightInBlocks, 1.0f}, widthInBlocks * bs * 0.5f, heightInBlocks * bs * 0.5f);
}

void SlipperyFloor::Update(GimmickContext&) {}

bool SlipperyFloor::GetSolid(SolidBox& out) const {
	out.box = Aabb::FromCenter(pos_, halfW_, halfH_);
	out.surface = SurfaceType::kSlippery;
	return true;
}
