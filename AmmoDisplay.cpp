#include "AmmoDisplay.h"

using namespace KamataEngine;

namespace {
const Vector2 kOrigin = {32.0f, 84.0f};
constexpr float kIconStep = 26.0f;
} // namespace

void AmmoDisplay::Initialize() {
	for (auto& icon : icons_) {
		icon = std::make_unique<UiSprite>();
		icon->Initialize("bullet_icon.png", {24.0f, 48.0f});
	}
	labelStraight_.Initialize("weapon_straight.png", {153.0f, 41.0f});
	labelHoming_.Initialize("weapon_homing.png", {131.0f, 41.0f});
	labelReload_.Initialize("reload_label.png", {144.0f, 46.0f});
	barBack_.InitializeSolid({0.1f, 0.1f, 0.15f, 0.9f});
	barFront_.InitializeSolid({1.0f, 0.55f, 0.3f, 1.0f});
}

void AmmoDisplay::Draw(const Weapon& weapon) {
	const bool homing = weapon.Mode() == FireMode::kHoming;
	for (int i = 0; i < kMaxIcons; ++i) {
		// ホーミングは1発で2個消費するので、残りが少ない側のアイコンを先に暗くする
		icons_[i]->SetPosition({kOrigin.x + kIconStep * static_cast<float>(i), kOrigin.y});
		icons_[i]->SetColor(1.0f, homing ? 0.6f : 1.0f, homing ? 0.9f : 1.0f, i < weapon.Ammo() ? 1.0f : 0.18f);
		icons_[i]->Draw();
	}
	UiSprite& label = homing ? labelHoming_ : labelStraight_;
	label.SetPosition({kOrigin.x - 6.0f, kOrigin.y + 56.0f});
	label.Draw();

	if (weapon.IsReloading()) {
		labelReload_.SetPosition({kOrigin.x + 150.0f, kOrigin.y + 52.0f});
		labelReload_.Draw();
		const float w = 200.0f, h = 12.0f;
		barBack_.SetPosition({kOrigin.x, kOrigin.y + 108.0f});
		barBack_.SetSize({w, h});
		barBack_.Draw();
		barFront_.SetPosition({kOrigin.x, kOrigin.y + 108.0f});
		barFront_.SetSize({w * weapon.ReloadProgress(), h});
		barFront_.Draw();
	}
}
