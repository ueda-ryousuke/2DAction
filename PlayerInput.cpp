#include "PlayerInput.h"
#include "KamataEngine.h"

using namespace KamataEngine;

PlayerInputState PlayerInput::Poll() {
	Input* in = Input::GetInstance();
	PlayerInputState s;
	if (in->PushKey(DIK_D) || in->PushKey(DIK_RIGHT)) {
		s.moveX += 1.0f;
	}
	if (in->PushKey(DIK_A) || in->PushKey(DIK_LEFT)) {
		s.moveX -= 1.0f;
	}
	s.jumpPressed = in->TriggerKey(DIK_SPACE);
	s.jumpHeld = in->PushKey(DIK_SPACE);
	s.shootPressed = in->TriggerKey(DIK_J);
	s.reloadPressed = in->TriggerKey(DIK_R);
	s.switchWeaponPressed = in->TriggerKey(DIK_K);
	return s;
}
