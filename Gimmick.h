#pragma once
#include "SolidBox.h"
#include "Aabb.h"
#include "KamataEngine.h"

class Player;
class GimmickManager;

// 毎フレーム Gimmick::Update に渡す共通情報
struct GimmickContext {
	GimmickManager* manager = nullptr;
	KamataEngine::Vector3 playerPos{};
	Aabb playerAabb{};
};

// フィールドギミック全ての基底クラス。
//   ・乗れる/ぶつかる物体 → GetSolid を実装（MapCollider がタイルと同じように扱う）
//   ・触れるとダメージ   → GetHazard を実装
class Gimmick {
public:
	virtual ~Gimmick() = default;

	virtual void Update(GimmickContext& ctx) = 0;
	virtual void Draw(const KamataEngine::Camera& camera);

	virtual bool GetSolid(SolidBox& out) const { (void)out; return false; }
	virtual bool GetHazard(Aabb& out, int& damage) const { (void)out; (void)damage; return false; }
	virtual void OnPlayerStand() {} // プレイヤーが上に乗っている間、毎フレーム呼ばれる

	const KamataEngine::Vector3& Position() const { return pos_; }
	virtual float CullRadius() const { return (halfW_ > halfH_ ? halfW_ : halfH_) + 2.0f; }

protected:
	void InitBase(KamataEngine::Model* model, const KamataEngine::Vector3& pos, const KamataEngine::Vector3& modelScale, float halfW, float halfH);
	void UpdateTransform(); // pos_ + drawOffset_ を行列へ反映（各 Update の最後で呼ぶ）

	KamataEngine::Model* model_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Vector3 pos_{};        // ギミックの中心（当たり判定の基準）
	KamataEngine::Vector3 drawOffset_{}; // 揺れ・落下などの見た目だけのずれ
	float halfW_ = 1.0f;
	float halfH_ = 1.0f;
	bool visible_ = true;
};
