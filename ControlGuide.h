#pragma once
#include "CountdownTimer.h"
#include "UiSprite.h"

// 操作方法パネル。開始直後に表示し、数秒で自動的に隠れる。H キーで出し入れできる。
class ControlGuide {
public:
	void Initialize();
	void Update();
	void Draw();

private:
	static inline const float kAutoHideSeconds = 8.0f;
	UiSprite panel_;
	CountdownTimer autoHide_;
	bool autoHideArmed_ = true; // 手動で切り替えたら自動非表示は取りやめ
	bool visible_ = true;
	float alpha_ = 0.0f;
};
