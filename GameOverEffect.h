#pragma once
#include "BannerEffect.h"

// ゲームオーバー演出: 画面が暗くなり、「GAME OVER」が重たく降りてきて、ゆらゆら揺れる。
class GameOverEffect : public BannerEffect {
public:
	void Initialize();
	void Start();
	void Update();
	void Draw();
	bool IsReadyForInput() const { return IsFinished(); }

protected:
	Pose Evaluate(float t) const override;

private:
	UiSprite dim_;
	UiSprite prompt_;
	float time_ = 0.0f;
};
