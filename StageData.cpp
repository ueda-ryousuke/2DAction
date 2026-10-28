#include "StageData.h"
#include <algorithm>

namespace {
const StageInfo kStages[] = {
    {"Resources/stage1.csv", StageCamera::kFollow, 0.0f, "block", 4.0f},         // はじまりの草原: 崩れる床/移動床/スイッチ扉/壁キックの縦穴
    {"Resources/stage2.csv", StageCamera::kFollow, 0.0f, "block_ice", 4.0f},     // すべる氷のダンジョン: 滑る床/落下天井/押しつぶし
    {"Resources/stage3.csv", StageCamera::kForcedScroll, 0.09f, "crusher", 3.2f}, // 迫りくる壁: 強制スクロール + 左から迫る壁
};
} // namespace

int StageData::Count() { return static_cast<int>(sizeof(kStages) / sizeof(kStages[0])); }

const StageInfo& StageData::Get(int index) { return kStages[std::clamp(index, 0, Count() - 1)]; }
