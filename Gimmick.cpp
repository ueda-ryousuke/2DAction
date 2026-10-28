#include "Gimmick.h"
#include "MathUtil.h"
#include "Matri4x4.h"

using namespace KamataEngine;

void Gimmick::InitBase(Model* model, const Vector3& pos, const Vector3& modelScale, float halfW, float halfH) {
	model_ = model;
	pos_ = pos;
	halfW_ = halfW;
	halfH_ = halfH;
	worldTransform_.Initialize();
	worldTransform_.scale_ = modelScale;
	UpdateTransform();
}

void Gimmick::UpdateTransform() {
	worldTransform_.translation_ = MathUtil::Add(pos_, drawOffset_);
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

void Gimmick::Draw(const Camera& camera) {
	if (visible_ && model_) {
		model_->Draw(worldTransform_, camera);
	}
}
