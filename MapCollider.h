#pragma once
#include "SolidBox.h"
#include "KamataEngine.h"
#include <functional>
#include <vector>

class MapChipField;
class Gimmick;

// 1フレームの移動で起きた衝突の結果
struct CollisionResult {
	bool hitTop = false;
	bool landed = false;
	bool hitLeft = false;
	bool hitRight = false;
	KamataEngine::Vector3 moveAmount{}; // 入力: 欲しい移動量 / 出力: 衝突で補正された移動量
};

struct GroundInfo {
	bool onGround = false;
	SurfaceType surface = SurfaceType::kNormal;
	Gimmick* owner = nullptr;
	KamataEngine::Vector3 velocity{};
};

struct WallInfo {
	bool left = false;
	bool right = false;
};

// マップチップ + SolidBox に対する矩形(AABB)の衝突解決。Player と Enemy で共有する。
// X移動 → Y移動 の順に軸分離で解決するので、角に引っかかりにくい。
class MapCollider {
public:
	void Initialize(MapChipField* field, float halfWidth, float halfHeight);
	void SetSolids(const std::vector<SolidBox>* solids) { solids_ = solids; }
	void SetHalfSize(float halfWidth, float halfHeight) { halfW_ = halfWidth; halfH_ = halfHeight; }

	// pos(中心)から result.moveAmount だけ動いたときの衝突を解決する。
	void Resolve(const KamataEngine::Vector3& pos, CollisionResult& result) const;

	GroundInfo ProbeGround(const KamataEngine::Vector3& pos) const; // 足元の接地判定
	WallInfo ProbeWall(const KamataEngine::Vector3& pos) const;     // 左右の壁接触（壁キック/壁ずり用）
	bool IsSolidAt(const KamataEngine::Vector3& p) const;           // その点がタイルか SolidBox の中か

private:
	float MoveX(const KamataEngine::Vector3& pos, float dx, CollisionResult& r) const;
	float MoveY(const KamataEngine::Vector3& pos, float dy, CollisionResult& r) const;
	bool IsBlockedTile(const KamataEngine::Vector3& p) const;
	static void ForEachSample(float lo, float hi, const std::function<void(float)>& fn);

	MapChipField* field_ = nullptr;
	const std::vector<SolidBox>* solids_ = nullptr;
	float halfW_ = 0.5f;
	float halfH_ = 0.5f;
};
