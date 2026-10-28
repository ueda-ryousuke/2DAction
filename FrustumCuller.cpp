#include "FrustumCuller.h"
#include "GameConfig.h"
#include <cmath>

using namespace KamataEngine;

void FrustumCuller::Update(const Camera& camera) {
	cameraPos_ = camera.translation_;
	tanHalfFov_ = std::tan(camera.fovAngleY * 0.5f);
	aspect_ = camera.aspectRatio;
}

void FrustumCuller::Set(const Vector3& cameraPos, float fovAngleY, float aspectRatio) {
	cameraPos_ = cameraPos;
	tanHalfFov_ = std::tan(fovAngleY * 0.5f);
	aspect_ = aspectRatio;
}

void FrustumCuller::HalfExtentAt(float z, float& halfWidth, float& halfHeight) const {
	float depth = std::fabs(z - cameraPos_.z);
	halfHeight = tanHalfFov_ * depth;
	halfWidth = halfHeight * aspect_;
}

bool FrustumCuller::IsVisible(const Vector3& p, float radius) const {
	if (!enabled_) {
		return true;
	}
	float hw, hh;
	HalfExtentAt(p.z, hw, hh);
	return std::fabs(p.x - cameraPos_.x) <= hw + radius && std::fabs(p.y - cameraPos_.y) <= hh + radius;
}

Vector2 FrustumCuller::WorldToScreen(const Vector3& p) const {
	float hw, hh;
	HalfExtentAt(p.z, hw, hh);
	float nx = (p.x - cameraPos_.x) / hw; // -1..1
	float ny = (p.y - cameraPos_.y) / hh;
	return {(nx * 0.5f + 0.5f) * GameConfig::kScreenWidth, (0.5f - ny * 0.5f) * GameConfig::kScreenHeight};
}
