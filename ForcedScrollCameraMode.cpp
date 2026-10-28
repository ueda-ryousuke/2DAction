#include "ForcedScrollCameraMode.h"
#include "MathUtil.h"
#include <algorithm>

using namespace KamataEngine;

Vector3 ForcedScrollCameraMode::InitialPosition(const CameraTargetInfo& t) const { return MathUtil::Add(t.position, offset_); }

Vector3 ForcedScrollCameraMode::Compute(const Vector3& current, const CameraTargetInfo& t) {
	Vector3 p = current;
	p.x = (std::min)(current.x + speed_, stopX_);
	p.y = MathUtil::Lerp(current.y, t.position.y + offset_.y, kYFollowRate);
	p.z = t.position.z + offset_.z;
	return p;
}
