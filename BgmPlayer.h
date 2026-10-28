#pragma once
#include <cstdint>

// BGM の種類。ファイル名は BgmPlayer.cpp の表で対応づける。
enum class BgmId { kNone, kTitle, kStage1, kStage2, kStage3, kClear, kGameOver, kCount };

// BGM 専用のプレイヤー。シーンは Play(...) を呼ぶだけでよい。
//  - 同じ BGM が鳴っていれば何もしない（タイトル→ステージ選択でも途切れない）
//  - 切り替え時は前の曲をフェードアウトし、新しい曲をフェードインする
//  - ポーズ中は SetDuck(true) で音量を下げる
class BgmPlayer {
public:
	static BgmPlayer& Get();

	void Initialize();                                          // 起動時に1回: 全 BGM を読み込む
	void Play(BgmId id, bool loop = true, float fadeIn = 0.8f); // 再生。クリア/ゲームオーバーは loop=false
	void Stop(float fadeOut = 0.5f);                            // フェードアウトして停止
	void SetDuck(bool ducked) { duck_ = ducked; }               // ポーズ中など音量を下げる
	void SetMasterVolume(float v) { master_ = v; }              // 0.0〜1.0（設定画面用）
	void Update();                                              // 毎フレーム呼ぶ（フェード処理）

private:
	static constexpr int kCount = static_cast<int>(BgmId::kCount);

	float TargetVolume() const;

	uint32_t sound_[kCount] = {};
	bool hasSound_[kCount] = {}; // ハンドル 0 は有効な値なので「読み込めたか」は別に持つ
	bool loaded_ = false;

	BgmId current_ = BgmId::kNone;
	uint32_t voice_ = 0; // 再生中の voice ハンドル
	bool hasVoice_ = false;
	float fade_ = 1.0f; // 現在曲のフェード係数 0..1
	float fadeInStep_ = 0.0f;

	uint32_t outVoice_ = 0; // フェードアウト中の前の曲
	bool hasOutVoice_ = false;
	float outFade_ = 0.0f;
	float outStep_ = 0.0f;

	bool duck_ = false;
	float duckFactor_ = 1.0f;
	float lastVolume_ = -1.0f; // 直前に SetVolume した値（変わったときだけ呼ぶ）
	float master_ = 1.0f;
};