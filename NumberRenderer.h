#pragma once
#include "UiSprite.h"
#include <memory>
#include <vector>

// digits.png(0-9 を横に並べた画像)で整数を描く。1桁につき1スプライトを持つ。
class NumberRenderer {
public:
	void Initialize(int maxDigits);
	// pos は「右上」の座標（右揃え）。scale=1 で 1桁 64x96。
	void Draw(int value, const KamataEngine::Vector2& pos, float scale = 0.5f, float alpha = 1.0f);

private:
	std::vector<std::unique_ptr<UiSprite>> digits_;
};
