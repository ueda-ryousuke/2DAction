#pragma once
#include "UiSprite.h"
#include <array>

// ポーズ画面。ESC/P で開閉、W/S か ↑/↓ で選び、SPACE/ENTER で決定。
//   「ゲームをつづける」 → kResume / 「ゲームを中断してタイトルへ」 → kQuitToTitle
class PauseMenu {
public:
	enum class Command { kNone, kResume, kQuitToTitle };

	void Initialize();
	Command Update(); // 閉じている間も呼ぶ（開くキーを監視するため）
	void Draw();

	bool IsOpen() const { return open_; }
	void Close() { open_ = false; }

private:
	void Open();

	UiSprite dim_;
	UiSprite title_;
	UiSprite items_[2];
	UiSprite cursor_;
	bool open_ = false;
	int selected_ = 0;
	float time_ = 0.0f;
};
