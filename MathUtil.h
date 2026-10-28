#pragma once
#include "KamataEngine.h"
#include <algorithm>
#include <cmath>
#include <numbers>

// 小さな数学ヘルパ。Vector3 の演算子オーバーロードに依存しないよう関数で用意する。
namespace MathUtil {

inline constexpr float kPi = std::numbers::pi_v<float>;

inline float DegToRad(float deg) { return deg * (kPi / 180.0f); }
inline float Clamp01(float t) { return std::clamp(t, 0.0f, 1.0f); }
inline float Lerp(float a, float b, float t) { return a + (b - a) * t; }

inline KamataEngine::Vector3 Lerp(const KamataEngine::Vector3& a, const KamataEngine::Vector3& b, float t) {
	return {Lerp(a.x, b.x, t), Lerp(a.y, b.y, t), Lerp(a.z, b.z, t)};
}
inline KamataEngine::Vector3 Add(const KamataEngine::Vector3& a, const KamataEngine::Vector3& b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }
inline KamataEngine::Vector3 Sub(const KamataEngine::Vector3& a, const KamataEngine::Vector3& b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
inline KamataEngine::Vector3 Scale(const KamataEngine::Vector3& a, float s) { return {a.x * s, a.y * s, a.z * s}; }
inline float Length(const KamataEngine::Vector3& a) { return std::sqrt(a.x * a.x + a.y * a.y + a.z * a.z); }
inline float LengthXY(const KamataEngine::Vector3& a) { return std::sqrt(a.x * a.x + a.y * a.y); }
inline KamataEngine::Vector3 NormalizeXY(const KamataEngine::Vector3& a) {
	float l = LengthXY(a);
	return l > 1e-6f ? KamataEngine::Vector3{a.x / l, a.y / l, 0.0f} : KamataEngine::Vector3{0, 0, 0};
}

// ---- イージング（t は 0..1） ----
inline float EaseInOut(float t) { return t * t * (3.0f - 2.0f * t); }
inline float EaseOutCubic(float t) { float u = 1.0f - t; return 1.0f - u * u * u; }
inline float EaseInQuad(float t) { return t * t; }
inline float EaseOutBack(float t) {
	const float c1 = 1.70158f, c3 = c1 + 1.0f;
	float u = t - 1.0f;
	return 1.0f + c3 * u * u * u + c1 * u * u;
}
inline float EaseOutBounce(float t) {
	const float n1 = 7.5625f, d1 = 2.75f;
	if (t < 1.0f / d1) return n1 * t * t;
	if (t < 2.0f / d1) { t -= 1.5f / d1; return n1 * t * t + 0.75f; }
	if (t < 2.5f / d1) { t -= 2.25f / d1; return n1 * t * t + 0.9375f; }
	t -= 2.625f / d1;
	return n1 * t * t + 0.984375f;
}

// 2D横スクロール用: XY平面内の進行方向 vel に「モデルの正面(+Z)」を向ける。
// Y回転で ±X を向け、Z回転で上下の傾きを付ける（回転順 Z→X→Y を想定）。
inline void FaceVelocityXY(const KamataEngine::Vector3& vel, KamataEngine::Vector3& rotation) {
	if (LengthXY(vel) < 1e-6f) return;
	float phi = std::atan2(vel.y, vel.x);
	bool left = vel.x < 0.0f;
	rotation.y = left ? kPi * 1.5f : kPi * 0.5f;
	rotation.z = left ? phi - kPi : phi;
}

} // namespace MathUtil
