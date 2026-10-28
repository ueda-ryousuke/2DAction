#pragma once
#include <cstdint>

// 効果音の種類。ファイル名は SePlayer.cpp の表で対応づける（Resources/se/）。
enum class SeId {
	kJump,        // プレイヤーのジャンプ（二段・壁キック共通）
	kLand,        // 着地
	kCrumble,     // 崩れる床が落ちる
	kSplit,       // 分裂する敵が割れる
	kEnemyDead,   // 敵を倒した
	kPlayerDead,  // プレイヤーがやられた
	kLose,        // ゲームオーバーのジングル
	kClear,       // ステージクリアのジングル
	kSceneChange, // シーン転換（壁が落ちる）
	kCount
};

// 効果音の再生。どこからでも SePlayer::Get().Play(SeId::kJump) と呼べる。
class SePlayer {
public:
	static SePlayer& Get();

	void Initialize();                          // 起動時に1回: 全 SE を読み込む
	void Play(SeId id, float volume = 1.0f);    // 1回再生（同じ音が重なってもOK）
	void SetMasterVolume(float v) { master_ = v; }

private:
	static constexpr int kCount = static_cast<int>(SeId::kCount);

	uint32_t sound_[kCount] = {};
	bool loaded_ = false;
	float master_ = 1.0f;
};
