#pragma once
#include "KamataEngine.h"
#include <string>

// KamataEngine::Sprite の薄いラッパ。所有(delete)と「サイズ/色/回転」の一括設定を担当。
class UiSprite {
public:
	UiSprite() = default;
	~UiSprite();
	UiSprite(const UiSprite&) = delete;
	UiSprite& operator=(const UiSprite&) = delete;

	// ui フォルダの画像から作る。size が {0,0} なら画像そのままのサイズ(nativeSize)。
	void Initialize(const std::string& uiFile, const KamataEngine::Vector2& nativeSize, const KamataEngine::Vector2& anchor = {0.0f, 0.0f});
	// 単色矩形（white1x1.png を着色）
	void InitializeSolid(const KamataEngine::Vector4& color);

	void SetPosition(const KamataEngine::Vector2& p);
	void SetSize(const KamataEngine::Vector2& s);
	void SetScale(float s) { SetSize({baseSize_.x * s, baseSize_.y * s}); }
	void SetColor(float r, float g, float b, float a);
	void SetAlpha(float a);
	void SetRotation(float rad);
	void SetTextureRect(const KamataEngine::Vector2& base, const KamataEngine::Vector2& size);
	void Draw();

	const KamataEngine::Vector2& BaseSize() const { return baseSize_; }

private:
	KamataEngine::Sprite* sprite_ = nullptr;
	KamataEngine::Vector2 baseSize_{1.0f, 1.0f};
	float r_ = 1, g_ = 1, b_ = 1, a_ = 1;
};
