#pragma once
#include "BannerEffect.h"

// クリア演出: 白いフラッシュと共に「STAGE CLEAR!」が上から落ちてきて弾み、そのまま居座る。
class ClearEffect : public BannerEffect {
public:
	void Initialize();
	void Start();
	void Update();
	void Draw();
	bool IsReadyForInput() const { return IsFinished(); } // 演出が終わったら「SPACEでタイトルへ」を受け付ける

protected:
	Pose Evaluate(float t) const override;

private:
	UiSprite flash_;
	UiSprite prompt_;
	float time_ = 0.0f;
};
