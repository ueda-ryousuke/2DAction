#include "FollowCameraMode.h"
#include "MathUtil.h"
#include <algorithm>

using namespace KamataEngine;

Vector3 FollowCameraMode::InitialPosition(const CameraTargetInfo& t) const { return MathUtil::Add(t.position, offset_); }

Vector3 FollowCameraMode::Compute(const Vector3& current, const CameraTargetInfo& t) {
	Vector3 goal = MathUtil::Add(t.position, offset_);
	goal.x += t.velocity.x * kVelocityBias;
	goal.y += t.velocity.y * kVelocityBias;

	Vector3 p = MathUtil::Lerp(current, goal, kInterpolationRate);

	// プレイヤーが画面外に出ないよう補正
	p.x = (std::max)(p.x, t.position.x + kMargin.left);
	p.x = (std::min)(p.x, t.position.x + kMargin.right);
	p.y = (std::max)(p.y, t.position.y + kMargin.bottom);
	p.y = (std::min)(p.y, t.position.y + kMargin.top);
	p.z = t.position.z + offset_.z;
	return p;
}
