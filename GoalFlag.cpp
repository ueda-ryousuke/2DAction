#include "GoalFlag.h"
#include "ModelLibrary.h"
#include "GimmickManager.h"
#include <cmath>

using namespace KamataEngine;

void GoalFlag::Initialize(const Vector3& footPosition) {
	InitBase(ModelLibrary::Get().Find("goal_flag"), footPosition, {1.5f, 1.5f, 1.5f}, 1.2f, 2.5f);
}

void GoalFlag::Update(GimmickContext& ctx) {
	phase_ += 0.05f;
	worldTransform_.rotation_.y = std::sin(phase_) * 0.25f; // 旗がゆらゆら揺れる
	// 判定は根元から上へ伸ばした箱
	Aabb box{{pos_.x - halfW_, pos_.y, pos_.z}, {pos_.x + halfW_, pos_.y + halfH_ * 2.0f, pos_.z}};
	if (Intersects(ctx.playerAabb, box)) {
		ctx.manager->NotifyGoal();
	}
	UpdateTransform();
}
