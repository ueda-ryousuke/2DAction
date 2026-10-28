#include "ScoreManager.h"

void ScoreManager::Reset() {
	score_ = 0;
	combo_ = 0;
	comboTimer_.Stop();
	gains_.clear();
}

void ScoreManager::Update() {
	comboTimer_.Update();
	if (comboTimer_.IsFinished()) {
		combo_ = 0;
	}
}

void ScoreManager::Add(int basePoints, const KamataEngine::Vector3& worldPos) {
	combo_ = comboTimer_.IsActive() ? combo_ + 1 : 1;
	comboTimer_.Start(kComboWindow);
	int points = basePoints * (10 + (combo_ - 1)) / 10; // コンボ1つにつき +10%
	score_ += points;
	gains_.push_back({points, worldPos});
}

std::vector<ScoreManager::Gain> ScoreManager::ConsumeGains() {
	std::vector<Gain> out;
	out.swap(gains_);
	return out;
}
