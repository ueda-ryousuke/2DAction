#pragma once
#include <cstdint>

// プレイヤーの調整用パラメータをここに集約（元 Player.h の static inline const 群）。
// 数値は元コードの値をそのまま引き継いでいる。ゲームフィールに合わせてここだけ触ればOK。
namespace PlayerParams {

// ---- 当たり判定 / 見た目 ----
inline constexpr float kWidth = 1.4f;       // 元コードは 0.8。ブロックが 2.0 の場合はこのくらいが自然
inline constexpr float kHeight = 1.8f;
inline constexpr float kModelScale = 2.0f;  // 元コードの worldTransform_.scale_ と同じ

// ---- 水平移動 ----
inline constexpr float kAcceleration = 0.3f;
inline constexpr float kAttenuation = 0.02f;
inline constexpr float kLimitRunSpeed = 0.4f;
inline constexpr float kGroundStopRate = 0.2f;    // 入力なしのとき1フレームで残る速度の割合（地上）
inline constexpr float kAirStopRate = 0.9f;       // 同（空中。壁キックの勢いを残すため緩め）
inline constexpr float kSlipperyStopRate = 0.97f; // 同（滑る床）
inline constexpr float kSlipperyAccelScale = 0.15f;

// ---- 重力 / ジャンプ ----
inline constexpr float kGravityAcceleration = 0.02f;
inline constexpr float kLimitFallSpeed = 0.4f;
inline constexpr float kWallSlideSpeed = 0.1f;    // 壁に張り付いて落ちるときの最大落下速度
inline constexpr float kJumpAcceleration = 0.7f;
inline constexpr int32_t kMaxJumpCount = 2;       // 二段ジャンプ（地上ジャンプ含む回数）
inline constexpr float kJumpCutRate = 0.5f;       // ボタンを離したときの上昇速度の残り率（小ジャンプ）
inline constexpr float kJumpCutThreshold = 0.25f;

// ---- 壁キック ----
inline constexpr float kWallKickVelocityX = 0.45f; // 壁と反対方向への初速（元 kWallJumpVelocityX）
inline constexpr float kWallKickVelocityY = 0.7f;
inline constexpr float kWallKickLockTime = 0.18f;  // この間は左右入力を無視（蹴った勢いを殺さない）

// ---- 入力の猶予（操作感の改善） ----
inline constexpr float kGroundCoyoteTime = 0.08f;  // 崖から落ちた直後でも地上ジャンプできる猶予
inline constexpr float kWallCoyoteTime = 0.10f;    // 壁から離れた直後でも壁キックできる猶予
inline constexpr float kJumpBufferTime = 0.10f;    // 着地/壁接触の少し前に押したジャンプを覚えておく

// ---- 旋回 ----
inline constexpr float kTimeTurn = 0.3f;

// ---- 体力 ----
inline constexpr int32_t kMaxHp = 6;
inline constexpr float kInvincibleTime = 1.2f;
inline constexpr float kKnockbackX = 0.35f;
inline constexpr float kKnockbackY = 0.4f;
inline constexpr float kKnockbackLockTime = 0.25f;

// ---- 射撃 ----
inline constexpr float kMuzzleOffsetX = 1.0f;
inline constexpr float kMuzzleOffsetY = 0.1f;

} // namespace PlayerParams
