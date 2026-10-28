#include "DynamicBackground.h"
#include "FrustumCuller.h"
#include "ModelLibrary.h"
#include "Matri4x4.h"
#include <cmath>

using namespace KamataEngine;

void DynamicBackground::Initialize() {
	model_ = ModelLibrary::Get().Find("bg_cloud");
	clouds_.clear();
	// {横位置, 高さ, 奥行き, 大きさ, 速さ}
	const float defs[][5] = {
	    {-60.0f, 18.0f, 52.0f, 4.0f, 0.020f}, {-10.0f, 28.0f, 58.0f, 5.0f, 0.012f}, {40.0f, 14.0f, 48.0f, 3.5f, 0.028f}, {85.0f, 30.0f, 55.0f, 4.5f, 0.016f},
	    {-90.0f, 6.0f, 44.0f, 3.0f, 0.032f},  {20.0f, 36.0f, 60.0f, 6.0f, 0.010f}, {110.0f, 22.0f, 46.0f, 3.2f, 0.026f},
	};
	for (const auto& d : defs) {
		Cloud c;
		c.relX = d[0];
		c.yOffset = d[1];
		c.z = d[2];
		c.scale = d[3];
		c.speed = d[4];
		c.phase = d[0] * 0.1f;
		c.wt = std::make_unique<WorldTransform>();
		c.wt->Initialize();
		clouds_.push_back(std::move(c));
	}
}

void DynamicBackground::Update(const Camera& camera, const FrustumCuller& culler) {
	const Vector3 cam = camera.translation_;
	for (Cloud& c : clouds_) {
		float hw, hh;
		culler.HalfExtentAt(c.z, hw, hh);
		const float margin = c.scale * 3.0f;

		c.relX -= c.speed;                    // 左へ流れる
		if (c.relX < -hw - margin) {          // 画面の左に抜けたら右から再登場
			c.relX += 2.0f * (hw + margin);
		}
		c.phase += 0.02f;

		c.wt->scale_ = {c.scale, c.scale, c.scale};
		c.wt->translation_ = {cam.x * (1.0f - c.parallax) + c.relX, cam.y * 0.7f + c.yOffset + std::sin(c.phase) * 0.8f, c.z};
		c.wt->matWorld_ = MakeAffineMatrix(c.wt->scale_, c.wt->rotation_, c.wt->translation_);
		c.wt->TransferMatrix();
	}
}

void DynamicBackground::Draw(const Camera& camera, const FrustumCuller& culler) {
	for (Cloud& c : clouds_) {
		if (culler.IsVisible(c.wt->translation_, c.scale * 3.0f)) {
			model_->Draw(*c.wt, camera);
		}
	}
}
