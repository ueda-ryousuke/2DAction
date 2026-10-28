#include "UiSprite.h"
#include "TextureLibrary.h"

using namespace KamataEngine;

UiSprite::~UiSprite() { delete sprite_; }

void UiSprite::Initialize(const std::string& uiFile, const Vector2& nativeSize, const Vector2& anchor) {
	delete sprite_;
	sprite_ = Sprite::Create(TextureLibrary::Ui(uiFile), {0.0f, 0.0f}, {1, 1, 1, 1}, anchor);
	baseSize_ = nativeSize;
	sprite_->SetSize(baseSize_);
}

void UiSprite::InitializeSolid(const Vector4& color) {
	Initialize("white1x1.png", {1.0f, 1.0f});
	SetColor(color.x, color.y, color.z, color.w);
}

void UiSprite::SetPosition(const Vector2& p) { sprite_->SetPosition(p); }
void UiSprite::SetSize(const Vector2& s) { sprite_->SetSize(s); }
void UiSprite::SetColor(float r, float g, float b, float a) {
	r_ = r; g_ = g; b_ = b; a_ = a;
	sprite_->SetColor({r, g, b, a});
}
void UiSprite::SetAlpha(float a) { SetColor(r_, g_, b_, a); }
void UiSprite::SetRotation(float rad) { sprite_->SetRotation(rad); }
void UiSprite::SetTextureRect(const Vector2& base, const Vector2& size) { sprite_->SetTextureRect(base, size); }
void UiSprite::Draw() { sprite_->Draw(); }
