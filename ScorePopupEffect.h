#pragma once
#include "CountdownTimer.h"
#include "NumberRenderer.h"
#include "ScoreManager.h"
#include <memory>
#include <vector>

class FrustumCuller;

// 敵を倒した位置に「+200」のように得点が浮かんで消える演出。
class ScorePopupEffect {
public:
	void Initialize();
	void Spawn(const std::vector<ScoreManager::Gain>& gains);
	void Update();
	void Draw(const FrustumCuller& culler);

private:
	struct Popup {
		bool active = false;
		int value = 0;
		KamataEngine::Vector3 worldPos{};
		CountdownTimer timer;
		NumberRenderer renderer;
	};
	static constexpr int kPoolSize = 10;
	static inline const float kLifeTime = 0.9f;

	std::vector<std::unique_ptr<Popup>> pool_;
};
