#pragma once
#include "UiSprite.h"
#include "Weapon.h"
#include <array>
#include <memory>

// 残弾アイコン・現在の弾種・リロード進行バー。
class AmmoDisplay {
public:
	void Initialize();
	void Draw(const Weapon& weapon);

private:
	static constexpr int kMaxIcons = Weapon::kMagazineSize;

	std::array<std::unique_ptr<UiSprite>, kMaxIcons> icons_;
	UiSprite labelStraight_;
	UiSprite labelHoming_;
	UiSprite labelReload_;
	UiSprite barBack_;
	UiSprite barFront_;
};
