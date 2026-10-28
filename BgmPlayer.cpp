#include "BgmPlayer.h"
#include "GameConfig.h"
#include "KamataEngine.h"
#include <algorithm>
#include <cmath>
#include <string>

using namespace KamataEngine;

namespace {
// BGM ファイル（Resources/bgm/ に置く .wav）。曲を差し替えるときはここだけ直す。
// ※ Audio::LoadWave は先頭に "Resources/" が付く想定。付かないエンジンなら kDir を "Resources/bgm/" に変える
const char* kDir = "bgm/";
const char* kFiles[] = {
    "",        // kNone
    "bgm.mp3", // kTitle / ステージ選択
    "bgm.mp3", // kStage1
    "bgm.mp3", // kStage2
    "bgm.mp3", // kStage3
    "",        // kClear     （空文字 = 専用曲なし。いま鳴っている曲をフェードアウトして止める）
    ""         // kGameOver  （同上）
};
constexpr float kBaseVolume = 0.5f;  // 全 BGM 共通の基準音量
constexpr float kDuckVolume = 0.35f; // ポーズ中の音量倍率
} // namespace

BgmPlayer& BgmPlayer::Get() {
	static BgmPlayer instance;
	return instance;
}

void BgmPlayer::Initialize() {
	if (loaded_) {
		return;
	}
	Audio* audio = Audio::GetInstance();
	for (int i = 1; i < kCount; ++i) {
		if (kFiles[i][0] == '\0') {
			continue; // 専用曲なし
		}
		// 同じファイルは1回だけ読み込んで使い回す（10分の曲を何度もメモリに載せない）
		bool shared = false;
		for (int j = 1; j < i; ++j) {
			if (std::string(kFiles[j]) == kFiles[i]) {
				sound_[i] = sound_[j];
				hasSound_[i] = true;
				shared = true;
				break;
			}
		}
		if (!shared) {
			sound_[i] = audio->LoadWave(std::string(kDir) + kFiles[i]);
			hasSound_[i] = true;
		}
	}
	loaded_ = true;
}

float BgmPlayer::TargetVolume() const { return kBaseVolume * master_ * duckFactor_; }

void BgmPlayer::Play(BgmId id, bool loop, float fadeIn) {
	if (!loaded_ || id == BgmId::kNone) {
		return;
	}
	if (!hasSound_[static_cast<int>(id)]) { // 専用曲なし: いまの曲を静かに止める
		Stop(0.8f);
		return;
	}
	if (hasVoice_ && current_ != BgmId::kNone && sound_[static_cast<int>(id)] == sound_[static_cast<int>(current_)]) {
		current_ = id;
		return; // 同じ曲がすでに鳴っている（シーンをまたいでも続ける）
	}
	Audio* audio = Audio::GetInstance();

	// 前の曲はフェードアウトへ（さらに前のフェードアウト中の曲は即停止）
	if (hasOutVoice_) {
		audio->StopWave(outVoice_);
		hasOutVoice_ = false;
	}
	if (hasVoice_) {
		outVoice_ = voice_;
		hasOutVoice_ = true;
		outFade_ = fade_;
		outStep_ = outFade_ / (0.5f / GameConfig::kDeltaTime);
	}

	current_ = id;
	fade_ = (fadeIn > 0.0f) ? 0.0f : 1.0f;
	fadeInStep_ = (fadeIn > 0.0f) ? GameConfig::kDeltaTime / fadeIn : 1.0f;
	lastVolume_ = TargetVolume() * fade_;
	voice_ = audio->PlayWave(sound_[static_cast<int>(id)], loop, lastVolume_);
	hasVoice_ = true;
}

void BgmPlayer::Stop(float fadeOut) {
	if (!hasVoice_) {
		return;
	}
	Audio* audio = Audio::GetInstance();
	if (hasOutVoice_) {
		audio->StopWave(outVoice_);
	}
	outVoice_ = voice_;
	hasOutVoice_ = true;
	outFade_ = fade_;
	outStep_ = (fadeOut > 0.0f) ? outFade_ * GameConfig::kDeltaTime / fadeOut : outFade_;
	hasVoice_ = false;
	current_ = BgmId::kNone;
}

void BgmPlayer::Update() {
	if (!loaded_) {
		return;
	}
	Audio* audio = Audio::GetInstance();

	// ポーズ中は音量をなめらかに下げる/戻す
	const float duckTarget = duck_ ? kDuckVolume : 1.0f;
	duckFactor_ += (duckTarget - duckFactor_) * 0.15f;

	if (hasVoice_) {
		fade_ = (std::min)(1.0f, fade_ + fadeInStep_);
		const float v = TargetVolume() * fade_;
		if (std::fabs(v - lastVolume_) > 0.002f) { // 毎フレーム SetVolume しない（変化したときだけ）
			audio->SetVolume(voice_, v);
			lastVolume_ = v;
		}
	}
	if (hasOutVoice_) {
		outFade_ -= outStep_;
		if (outFade_ <= 0.0f) {
			audio->StopWave(outVoice_);
			hasOutVoice_ = false;
		} else {
			audio->SetVolume(outVoice_, TargetVolume() * outFade_);
		}
	}
}