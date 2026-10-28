#include "WorldTransformUpdate.h"
#include "Matri4x4.h"

using namespace KamataEngine;


void WorldTransformUpdate(WorldTransform& worldTransform) 
{
	worldTransform.matWorld_ = MakeAffineMatrix(worldTransform.scale_, worldTransform.rotation_, worldTransform.translation_);

	worldTransform.TransferMatrix();
}