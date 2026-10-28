#include "NumberRenderer.h"
#include <algorithm>

using namespace KamataEngine;

namespace {
constexpr float kDigitW = 64.0f;
constexpr float kDigitH = 96.0f;
} // namespace

void NumberRenderer::Initialize(int maxDigits) {
	digits_.clear();
	for (int i = 0; i < maxDigits; ++i) {
		auto s = std::make_unique<UiSprite>();
		s->Initialize("digits.png", {kDigitW, kDigitH});
		digits_.push_back(std::move(s));
	}
}

void NumberRenderer::Draw(int value, const Vector2& pos, float scale, float alpha) {
	value = (std::max)(0, value);
	const int maxDigits = static_cast<int>(digits_.size());
	int idx = 0;
	do {
		if (idx >= maxDigits) {
			break;
		}
		int d = value % 10;
		value /= 10;
		UiSprite& s = *digits_[idx];
		s.SetTextureRect({d * kDigitW, 0.0f}, {kDigitW, kDigitH});
		s.SetSize({kDigitW * scale, kDigitH * scale});
		s.SetPosition({pos.x - kDigitW * scale * static_cast<float>(idx + 1), pos.y});
		s.SetAlpha(alpha);
		s.Draw();
		++idx;
	} while (value > 0);
}
