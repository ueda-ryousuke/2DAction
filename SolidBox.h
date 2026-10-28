#pragma once
#include "Aabb.h"
#include "KamataEngine.h"

class Gimmick;

// 床の種類（滑る床の判定に使う）
enum class SurfaceType { kNormal, kSlippery };

// マップチップ以外の「乗れる/ぶつかる」物体（動く足場・崩れる床・扉など）。
// GimmickManager が毎フレーム集めて MapCollider に渡す。
struct SolidBox {
	Aabb box;
	KamataEngine::Vector3 velocity{}; // このフレームの移動量（乗っている人を運ぶ）
	SurfaceType surface = SurfaceType::kNormal;
	Gimmick* owner = nullptr;
};
