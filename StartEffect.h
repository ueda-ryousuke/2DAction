#pragma once
#include "BannerEffect.h"
#include <memory>

// ゲーム開始演出: 「READY?」がポンと出て、「GO!」が大きく弾けて消える。終わったら操作可能。
class StartEffect {
public:
	void Initialize();
	void Start();
	void Update();
	void Draw();
	bool IsFinished() const { return finished_; }
	bool IsActive() const { return active_; }

private:
	// 登場で弾み(EaseOutBack)、終盤でフェードアウトするバナー。growTo > 1 なら時間とともに拡大する
	class PopBanner : public BannerEffect {
	public:
		float growTo = 1.0f;

	protected:
		Pose Evaluate(float t) const override;
	};

	PopBanner ready_;
	PopBanner go_;
	bool active_ = false;
	bool finished_ = false;
};
