#include "ScrollingBackdrop.h"
#include "DrawScope.h"
#include "GameConfig.h"
#include "ModelLibrary.h"
#include "Matri4x4.h"
#include <cmath>

using namespace KamataEngine;

void ScrollingBackdrop::Initialize() {
	camera_.Initialize();
	camera_.farZ = 1000.0f;
	scroll_ = 0.0f;
	background_.Initialize();
	clouds_.Initialize();
	blockModel_ = ModelLibrary::Get().Find("block");
	blocks_.clear();
	for (int i = 0; i < kColumns * kRows; ++i) {
		auto wt = std::make_unique<WorldTransform>();
		wt->Initialize();
		blocks_.push_back(std::move(wt));
	}
	Update();
}

void ScrollingBackdrop::Update() {
	scroll_ += kScrollSpeed;
	camera_.translation_ = {scroll_, 2.0f, -50.0f};
	camera_.UpdateMatrix();
	camera_.TransferMatrix();
	culler_.Update(camera_);
	clouds_.Update(camera_, culler_);

	// 草地: 画面の幅ぶんだけ並べ、カメラが2ユニット進むごとに1列ずらす
	const float bs = GameConfig::kBlockSize;
	const float baseX = std::floor(scroll_ / bs) * bs - bs * (kColumns / 2);
	for (int r = 0; r < kRows; ++r) {
		for (int c = 0; c < kColumns; ++c) {
			WorldTransform& wt = *blocks_[r * kColumns + c];
			wt.scale_ = {bs / 2.0f, bs / 2.0f, bs / 2.0f};
			wt.translation_ = {baseX + bs * static_cast<float>(c), GroundTopY() - bs * 0.5f - bs * static_cast<float>(r), 0.0f};
			wt.matWorld_ = MakeAffineMatrix(wt.scale_, wt.rotation_, wt.translation_);
			wt.TransferMatrix();
		}
	}
}

void ScrollingBackdrop::Draw3D() {
	background_.Draw(camera_, culler_);
	clouds_.Draw(camera_, culler_);
	for (auto& wt : blocks_) {
		if (culler_.IsVisible(wt->translation_, GameConfig::kBlockSize)) {
			blockModel_->Draw(*wt, camera_);
		}
	}
}
