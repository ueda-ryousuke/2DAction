#pragma once

// ゲーム全体で共有する定数。
namespace GameConfig {
inline constexpr float kDeltaTime = 1.0f / 60.0f; // 固定60FPS前提
inline constexpr float kBlockSize = 2.0f;         // MapChipField のブロック1個の大きさに合わせる
inline constexpr float kScreenWidth = 1280.0f;
inline constexpr float kScreenHeight = 720.0f;
inline constexpr float kBackgroundZ = 40.0f; // 背景レイヤーの基準奥行き（カメラは z=-50 付近）
} // namespace GameConfig
