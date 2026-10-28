#pragma once
#include "CountdownTimer.h"
#include "UiSprite.h"

// 単色フェード。画面全体を1色の矩形で覆い、アルファだけ動かす。
class FadeEffect {
public:
	enum class Mode { kNone, kFadeIn, kFadeOut }; // FadeIn: 色→透明(画面が見えてくる) / FadeOut: 透明→色(暗くなる)

	void Initialize();
	void Start(Mode mode, float seconds, float r = 0.0f, float g = 0.0f, float b = 0.0f);
	void Update();
	void Draw();

	bool IsActive() const { return timer_.IsActive(); }
	bool IsFinished() const { return mode_ != Mode::kNone && timer_.IsFinished(); }
	float Alpha() const;

private:
	UiSprite sprite_;
	CountdownTimer timer_;
	Mode mode_ = Mode::kNone;
	float r_ = 0, g_ = 0, b_ = 0;
};
