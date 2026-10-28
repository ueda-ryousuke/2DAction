#pragma once
#include "FadeEffect.h"
#include "SceneTransition.h"
#include "IScene.h"
#include <memory>

// シーンの生成・切り替え。
// 切り替えの流れ: シーンが終了を宣言 → 壁が落ちて画面を覆う → 次シーンを生成 → 壁が引き上がる
class SceneManager {
public:
	void Initialize();
	void Update();
	void Draw();

private:
	std::unique_ptr<IScene> Create(SceneId id) const;
	void ChangeTo(SceneId id);

	std::unique_ptr<IScene> scene_;
	SceneContext context_;
	SceneTransition transition_;
	FadeEffect fade_; // 起動時の白フェード
	SceneId pending_ = SceneId::kTitle;
	bool changing_ = false;
};
