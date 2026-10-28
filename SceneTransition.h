#pragma once
#include "CountdownTimer.h"
#include "UiSprite.h"
#include <array>
#include <memory>

// ゲーム内容にちなんだ場面転換「壁がせり落ちてくる」。
// レンガの壁が左から順に落ちてきて画面を覆い（閉じる）、次のシーンの準備ができたら順に引き上がる（開く）。
class SceneTransition {
public:
	static constexpr int kStripCount = 8;

	void Initialize();
	void StartClose();
	void StartOpen();
	void Update();
	void Draw();

	bool IsClosed() const { return state_ == State::kClosed; }
	bool IsOpen() const { return state_ == State::kOpen; }

private:
	enum class State { kOpen, kClosing, kClosed, kOpening };
	static inline const float kStagger = 0.06f;  // 壁1枚ごとの時間差
	static inline const float kStripTime = 0.45f;

	float StripY(int index) const; // 画面上端からの y 座標

	std::array<std::unique_ptr<UiSprite>, kStripCount> strips_;
	State state_ = State::kOpen;
	float time_ = 0.0f;
};
