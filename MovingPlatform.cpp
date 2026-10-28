#include "MovingPlatform.h"
#include "GameConfig.h"
#include "MathUtil.h"
#include "ModelLibrary.h"
#include <algorithm>

using namespace KamataEngine;

void MovingPlatform::Initialize(const Vector3& start, const Vector3& end, float widthInBlocks, float unitsPerFrame) {
	start_ = start;
	end_ = end;
	speed_ = unitsPerFrame;
	length_ = (std::max)(0.01f, MathUtil::Length(MathUtil::Sub(end, start)));
	const float width = widthInBlocks * GameConfig::kBlockSize;
	// モデルは 2.0 x 0.6 x 2.0。X方向だけ引き伸ばして幅を合わせる
	InitBase(ModelLibrary::Get().Find("platform_moving"), start, {width / 2.0f, 1.0f, 1.0f}, width * 0.5f, 0.3f);
}

void MovingPlatform::Update(GimmickContext&) {
	pause_.Update();
	Vector3 before = pos_;
	if (pause_.IsFinished()) {
		s_ += dir_ * speed_ / length_;
		if (s_ >= 1.0f || s_ <= 0.0f) {
			s_ = std::clamp(s_, 0.0f, 1.0f);
			dir_ = -dir_;
			pause_.Start(kEndPause);
		}
	}
	pos_ = MathUtil::Lerp(start_, end_, MathUtil::EaseInOut(s_));
	velocity_ = MathUtil::Sub(pos_, before);
	UpdateTransform();
}

bool MovingPlatform::GetSolid(SolidBox& out) const {
	out.box = Aabb::FromCenter(pos_, halfW_, halfH_);
	out.velocity = velocity_;
	return true;
}
