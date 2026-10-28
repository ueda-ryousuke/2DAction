#include "RepeatBackground.h"
#include "FrustumCuller.h"
#include "ModelLibrary.h"
#include "Matri4x4.h"
#include <cmath>

using namespace KamataEngine;

void RepeatBackground::Initialize() {
	ModelLibrary& lib = ModelLibrary::Get();
	layers_.clear();

	auto add = [&](const char* model, float nativeWidth, float scale, float parallax, float parallaxY, float z, float baseY) {
		Layer l;
		l.model = lib.Find(model);
		l.scale = {scale, scale, 1.0f};
		l.tileWidth = nativeWidth * scale;
		l.parallax = parallax;
		l.parallaxY = parallaxY;
		l.z = z;
		l.baseY = baseY;
		for (int i = 0; i < 6; ++i) {
			auto wt = std::make_unique<WorldTransform>();
			wt->Initialize();
			l.pool.push_back(std::move(wt));
		}
		layers_.push_back(std::move(l));
	};
	// 数値は見た目の調整用（奥のものほど z が大きく、parallax が小さい）
	add("bg_sky", 40.0f, 5.0f, 0.00f, 0.0f, 60.0f, 0.0f);
	add("bg_hills_far", 40.0f, 3.0f, 0.15f, 0.2f, 45.0f, -20.0f);
	add("bg_hills_near", 40.0f, 2.2f, 0.35f, 0.2f, 25.0f, -24.0f);
}

void RepeatBackground::Draw(const Camera& camera, const FrustumCuller& culler, float extraOffsetX) {
	const Vector3 cam = camera.translation_;
	for (Layer& l : layers_) {
		float hw, hh;
		culler.HalfExtentAt(l.z, hw, hh);

		const float origin = cam.x * (1.0f - l.parallax) + extraOffsetX; // タイル0の中心
		const int kMin = static_cast<int>(std::ceil((cam.x - hw - origin) / l.tileWidth - 0.5f));
		const int kMax = static_cast<int>(std::floor((cam.x + hw - origin) / l.tileWidth + 0.5f));
		const float y = cam.y * (1.0f - l.parallaxY) + l.baseY;

		size_t used = 0;
		for (int k = kMin; k <= kMax && used < l.pool.size(); ++k, ++used) {
			WorldTransform& wt = *l.pool[used];
			wt.scale_ = l.scale;
			wt.translation_ = {origin + static_cast<float>(k) * l.tileWidth, y, l.z};
			wt.matWorld_ = MakeAffineMatrix(wt.scale_, wt.rotation_, wt.translation_);
			wt.TransferMatrix();
			l.model->Draw(wt, camera);
		}
	}
}
