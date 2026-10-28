#include "MapCollider.h"
#include "GameConfig.h"
#include "MapChipField.h"
#include <algorithm>
#include <cmath>

using namespace KamataEngine;

namespace {
constexpr float kSkin = 0.001f;  // 壁との隙間（めり込み/ガタつき防止）
constexpr float kProbe = 0.08f;  // 足元・壁の接触判定で見る距離
} // namespace

void MapCollider::Initialize(MapChipField* field, float halfWidth, float halfHeight) {
	field_ = field;
	halfW_ = halfWidth;
	halfH_ = halfHeight;
}

// lo..hi を、ブロック1個より粗くならない間隔でサンプリング
void MapCollider::ForEachSample(float lo, float hi, const std::function<void(float)>& fn) {
	int n = (std::max)(2, static_cast<int>(std::ceil((hi - lo) / (GameConfig::kBlockSize * 0.9f))) + 1);
	for (int i = 0; i < n; ++i) {
		fn(lo + (hi - lo) * static_cast<float>(i) / static_cast<float>(n - 1));
	}
}

bool MapCollider::IsBlockedTile(const Vector3& p) const {
	if (!field_) {
		return false;
	}
	MapChipField::IndexSet idx = field_->GetMapChipIndexSetByPosition(p);
	return field_->GetMapChipTypeByIndex(idx.xIndex, idx.yIndex) == MapChipType::kBlock;
}

bool MapCollider::IsSolidAt(const Vector3& p) const {
	if (IsBlockedTile(p)) {
		return true;
	}
	if (solids_) {
		for (const SolidBox& s : *solids_) {
			if (p.x >= s.box.min.x && p.x <= s.box.max.x && p.y >= s.box.min.y && p.y <= s.box.max.y) {
				return true;
			}
		}
	}
	return false;
}

void MapCollider::Resolve(const Vector3& pos, CollisionResult& r) const {
	Vector3 want = r.moveAmount;
	Vector3 p = pos;
	float mx = MoveX(p, want.x, r);
	p.x += mx;
	float my = MoveY(p, want.y, r);
	r.moveAmount = {mx, my, want.z};
}

float MapCollider::MoveX(const Vector3& pos, float dx, CollisionResult& r) const {
	if (dx == 0.0f) {
		return 0.0f;
	}
	const float dir = dx > 0.0f ? 1.0f : -1.0f;
	const float edge = pos.x + dir * halfW_;
	const float lo = pos.y - halfH_ + kSkin;
	const float hi = pos.y + halfH_ - kSkin;
	float allowed = std::fabs(dx); // 進める距離（正の値）

	// --- マップチップ: 進行方向の前縁が入る先のセルを調べる ---
	if (field_) {
		ForEachSample(lo, hi, [&](float y) {
			Vector3 p{edge + dx, y, pos.z};
			if (!IsBlockedTile(p)) {
				return;
			}
			MapChipField::IndexSet idx = field_->GetMapChipIndexSetByPosition(p);
			MapChipField::Rect rect = field_->GetRectByIndex(idx.xIndex, idx.yIndex);
			float wall = dir > 0.0f ? rect.left : rect.right;
			allowed = (std::min)(allowed, (std::max)(0.0f, (wall - edge) * dir - kSkin));
		});
	}
	// --- SolidBox（足場・扉など） ---
	if (solids_) {
		for (const SolidBox& s : *solids_) {
			if (hi <= s.box.min.y || lo >= s.box.max.y) {
				continue;
			}
			float wall = dir > 0.0f ? s.box.min.x : s.box.max.x;
			float d = (wall - edge) * dir;
			if (d < -kSkin) {
				continue; // 背後 or すでにめり込み済み
			}
			allowed = (std::min)(allowed, (std::max)(0.0f, d - kSkin));
		}
	}

	if (allowed < std::fabs(dx) - 1e-6f) {
		(dir > 0.0f ? r.hitRight : r.hitLeft) = true;
	}
	return dir * allowed;
}

float MapCollider::MoveY(const Vector3& pos, float dy, CollisionResult& r) const {
	if (dy == 0.0f) {
		return 0.0f;
	}
	const float dir = dy > 0.0f ? 1.0f : -1.0f;
	const float edge = pos.y + dir * halfH_;
	const float lo = pos.x - halfW_ + kSkin;
	const float hi = pos.x + halfW_ - kSkin;
	float allowed = std::fabs(dy);

	if (field_) {
		ForEachSample(lo, hi, [&](float x) {
			Vector3 p{x, edge + dy, pos.z};
			if (!IsBlockedTile(p)) {
				return;
			}
			MapChipField::IndexSet idx = field_->GetMapChipIndexSetByPosition(p);
			MapChipField::Rect rect = field_->GetRectByIndex(idx.xIndex, idx.yIndex);
			float wall = dir > 0.0f ? rect.bottom : rect.top;
			allowed = (std::min)(allowed, (std::max)(0.0f, (wall - edge) * dir - kSkin));
		});
	}
	if (solids_) {
		for (const SolidBox& s : *solids_) {
			if (hi <= s.box.min.x || lo >= s.box.max.x) {
				continue;
			}
			float wall = dir > 0.0f ? s.box.min.y : s.box.max.y;
			float d = (wall - edge) * dir;
			if (d < -kSkin) {
				continue;
			}
			allowed = (std::min)(allowed, (std::max)(0.0f, d - kSkin));
		}
	}

	if (allowed < std::fabs(dy) - 1e-6f) {
		(dir > 0.0f ? r.hitTop : r.landed) = true;
	}
	return dir * allowed;
}

GroundInfo MapCollider::ProbeGround(const Vector3& pos) const {
	GroundInfo g;
	const float bottom = pos.y - halfH_;
	const float xl = pos.x - halfW_ + kSkin;
	const float xr = pos.x + halfW_ - kSkin;

	if (IsBlockedTile({xl, bottom - kProbe, pos.z}) || IsBlockedTile({xr, bottom - kProbe, pos.z})) {
		g.onGround = true;
	}
	if (solids_) {
		for (const SolidBox& s : *solids_) {
			if (xr <= s.box.min.x || xl >= s.box.max.x) {
				continue;
			}
			// 上昇中の足場に少し埋まっていても「乗っている」とみなす
			float embed = (std::max)(0.15f, std::fabs(s.velocity.y) + 0.05f);
			float gap = bottom - s.box.max.y;
			if (gap <= kProbe && gap >= -embed) {
				g.onGround = true;
				g.surface = s.surface;
				g.owner = s.owner;
				g.velocity = s.velocity;
				break;
			}
		}
	}
	return g;
}

WallInfo MapCollider::ProbeWall(const Vector3& pos) const {
	WallInfo w;
	const float reach = halfW_ + kProbe;
	const float ys[3] = {pos.y - halfH_ * 0.6f, pos.y, pos.y + halfH_ * 0.6f};
	for (float y : ys) {
		if (IsSolidAt({pos.x - reach, y, pos.z})) {
			w.left = true;
		}
		if (IsSolidAt({pos.x + reach, y, pos.z})) {
			w.right = true;
		}
	}
	return w;
}
