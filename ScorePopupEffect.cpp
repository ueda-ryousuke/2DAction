#include "ScorePopupEffect.h"
#include "FrustumCuller.h"
#include "MathUtil.h"

using namespace KamataEngine;

void ScorePopupEffect::Initialize() {
	pool_.clear();
	for (int i = 0; i < kPoolSize; ++i) {
		auto p = std::make_unique<Popup>();
		p->renderer.Initialize(6);
		pool_.push_back(std::move(p));
	}
}

void ScorePopupEffect::Spawn(const std::vector<ScoreManager::Gain>& gains) {
	for (const auto& g : gains) {
		for (auto& p : pool_) {
			if (!p->active) {
				p->active = true;
				p->value = g.points;
				p->worldPos = g.worldPos;
				p->timer.Start(kLifeTime);
				break;
			}
		}
	}
}

void ScorePopupEffect::Update() {
	for (auto& p : pool_) {
		if (p->active) {
			p->timer.Update();
			if (p->timer.IsFinished()) {
				p->active = false;
			}
		}
	}
}

void ScorePopupEffect::Draw(const FrustumCuller& culler) {
	for (auto& p : pool_) {
		if (!p->active) {
			continue;
		}
		float t = p->timer.Progress();
		Vector2 s = culler.WorldToScreen(p->worldPos);
		s.y -= 70.0f * MathUtil::EaseOutCubic(t) + 20.0f;   // ふわっと上へ
		float alpha = t < 0.6f ? 1.0f : 1.0f - (t - 0.6f) / 0.4f;
		float scale = 0.32f + 0.12f * MathUtil::EaseOutBack((std::min)(1.0f, t * 4.0f));
		p->renderer.Draw(p->value, {s.x + 40.0f, s.y}, scale, alpha);
	}
}
