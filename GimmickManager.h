#pragma once
#include "Gimmick.h"
#include <map>
#include <memory>
#include <vector>

class FrustumCuller;

// 全ギミックの所有・更新・描画と、ギミック同士で共有する状態（スイッチ/ゴール）の管理。
class GimmickManager {
public:
	void Clear();
	void Add(std::unique_ptr<Gimmick> gimmick);

	void Update(GimmickContext& ctx); // 更新 → SolidBox の再収集まで行う
	void Draw(const KamataEngine::Camera& camera, const FrustumCuller& culler);

	const std::vector<SolidBox>& Solids() const { return solids_; } // Player/Enemy の MapCollider に渡す
	bool CheckHazard(const Aabb& victim, int& damage, KamataEngine::Vector3& sourcePos) const; // 触れたらダメージの物体との判定

	// スイッチ（番号=チャンネルで FloorSwitch と SwitchDoor を結ぶ）
	void SetSwitch(int channel, bool on) { switches_[channel] = on; }
	bool IsSwitchOn(int channel) const;

	void NotifyGoal() { goalReached_ = true; }
	bool IsGoalReached() const { return goalReached_; }

private:
	std::vector<std::unique_ptr<Gimmick>> gimmicks_;
	std::vector<SolidBox> solids_;
	std::map<int, bool> switches_;
	bool goalReached_ = false;
};
