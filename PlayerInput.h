#pragma once

// 1フレーム分のプレイヤー入力。キー配置を変えたいときは PlayerInput.cpp だけ直す。
struct PlayerInputState {
	float moveX = 0.0f; // -1(左) .. +1(右)
	bool jumpPressed = false;
	bool jumpHeld = false;
	bool shootPressed = false;
	bool reloadPressed = false;
	bool switchWeaponPressed = false;
};

class PlayerInput {
public:
	static PlayerInputState Poll();
};
