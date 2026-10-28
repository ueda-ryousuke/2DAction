#pragma once
#include "UiSprite.h"
#include "NumberRenderer.h"

// 画面右上のスコア表示。数字は目標値に向かってカウントアップする。
class ScoreDisplay {
public:
	void Initialize();
	void Update(int score);
	void Draw();

private:
	UiSprite label_;
	NumberRenderer number_;
	int shown_ = 0;
	int target_ = 0;
};
