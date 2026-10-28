#pragma once
#include "CountdownTimer.h"
#include "UiSprite.h"
#include <string>

// 画面中央に出す文字バナー演出の土台。「時間 t(0..1) のときの位置・拡大率・透明度・回転」を
// Evaluate() で返すだけで、READY/GO/CLEAR/GAME OVER などの動きを派生クラスで作れる。
class BannerEffect {
public:
	struct Pose {
		KamataEngine::Vector2 center{0.0f, 0.0f};
		float scale = 1.0f;
		float alpha = 1.0f;
		float rotation = 0.0f;
	};

	virtual ~BannerEffect() = default;

	void Initialize(const std::string& uiFile, const KamataEngine::Vector2& nativeSize, float duration);
	void Start();
	void Update();
	void Draw();

	bool IsActive() const { return started_; }
	bool IsFinished() const { return started_ && timer_.IsFinished(); }
	float Time() const { return timer_.Progress(); }

protected:
	virtual Pose Evaluate(float t) const = 0;

private:
	UiSprite sprite_;
	CountdownTimer timer_;
	float duration_ = 1.0f;
	bool started_ = false;
};
