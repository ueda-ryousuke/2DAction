#pragma once
#include "KamataEngine.h"

// 2Dゲーム用のAABB。判定は XY 平面のみ（Z は無視）。
struct Aabb {
	KamataEngine::Vector3 min{};
	KamataEngine::Vector3 max{};

	static Aabb FromCenter(const KamataEngine::Vector3& c, float halfW, float halfH) {
		return {{c.x - halfW, c.y - halfH, c.z}, {c.x + halfW, c.y + halfH, c.z}};
	}
	KamataEngine::Vector3 Center() const { return {(min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f, min.z}; }
};

inline bool Intersects(const Aabb& a, const Aabb& b) {
	return !(a.max.x < b.min.x || b.max.x < a.min.x || a.max.y < b.min.y || b.max.y < a.min.y);
}
