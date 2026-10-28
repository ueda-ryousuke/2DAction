#pragma once

enum class StageCamera { kFollow, kForcedScroll };

// ステージ1つぶんの定義。マップのCSVと、ギミック配置(StageBuilder)はこのインデックスで対応する。
struct StageInfo {
	const char* csvPath;
	StageCamera camera;
	float scrollSpeed;        // 強制スクロールの速さ[unit/frame]
	const char* previewModel; // ステージ選択画面で回すモデル
	float previewScale;
};

class StageData {
public:
	static int Count();
	static const StageInfo& Get(int index);
};
