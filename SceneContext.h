#pragma once

enum class SceneId { kTitle, kStageSelect, kGame };

// シーン間で受け渡す情報
struct SceneContext {
	int selectedStage = 0; // ステージ選択 → ゲーム
	int lastScore = 0;     // ゲーム → タイトル（必要なら表示に使う）
};
