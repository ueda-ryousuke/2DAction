#pragma once
#include "CountdownTimer.h"
#include "KamataEngine.h"
#include <vector>

// スコアとコンボの管理。敵を連続で倒すとコンボ倍率が上がる。
class ScoreManager {
public:
	struct Gain {
		int points = 0;
		KamataEngine::Vector3 worldPos{};
	};

	void Reset();
	void Update();
	void Add(int basePoints, const KamataEngine::Vector3& worldPos); // 敵撃破など。コンボ倍率が掛かる

	int Score() const { return score_; }
	int Combo() const { return combo_; }
	// 今フレーム新しく入った得点（ポップアップ演出用）。取り出すと空になる
	std::vector<Gain> ConsumeGains();

private:
	static inline const float kComboWindow = 2.5f; // この秒数以内に次を倒すとコンボ継続
	int score_ = 0;
	int combo_ = 0;
	CountdownTimer comboTimer_;
	std::vector<Gain> gains_;
};
