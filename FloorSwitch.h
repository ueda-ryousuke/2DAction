#pragma once
#include "Gimmick.h"

// 床スイッチ: 踏むと channel が ON になりっぱなしになる（SwitchDoor と同じ番号で連動）。
// 「鍵」にしたいときは、ここを「アイテムに触れたら SetSwitch」に置き換えるだけで同じ仕組みが使える。
class FloorSwitch : public Gimmick {
public:
	void Initialize(const KamataEngine::Vector3& floorTopCenter, int channel);
	void Update(GimmickContext& ctx) override;

private:
	int channel_ = 0;
	bool pressed_ = false;
};
