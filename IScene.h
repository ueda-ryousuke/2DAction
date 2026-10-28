#pragma once
#include "SceneContext.h"

// シーンの共通インターフェース。終了したいときは finished_ = true と next_ をセットするだけ。
// 切り替え演出(壁の転換)は SceneManager が行う。
class IScene {
public:
	virtual ~IScene() = default;
	virtual void Initialize(SceneContext& context) = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0; // 3Dパス → スプライトパス の順で自分で描く

	bool IsFinished() const { return finished_; }
	SceneId NextScene() const { return next_; }

protected:
	SceneContext* context_ = nullptr;
	bool finished_ = false;
	SceneId next_ = SceneId::kTitle;
};
