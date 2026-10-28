#include "SePlayer.h"
#include "KamataEngine.h"
#include <Windows.h>
#include <string>

using namespace KamataEngine;

namespace {
// SE ファイル（Resources/se/ に置く）。音を差し替えるときはここだけ直す。
// ※ BGM と同じく、Audio::LoadWave は先頭に "Resources/" が付く想定
const char* kDir = "bgm/"; // 効果音もBGMと同じフォルダ（Resources/bgm/）に置いている
struct SeDef {
	const char* file;
	float volume; // 音ごとの基準音量（大きすぎる/小さすぎる素材の調整用）
};
const SeDef kDefs[] = {
    {"playerjump.mp3",         0.6f}, // kJump
    {"playerjumpchakuchi.mp3", 0.5f}, // kLand
    {"jimennkowareru.mp3",     0.7f}, // kCrumble
    {"bunretu.mp3",            0.7f}, // kSplit
    {"enemydead.mp3",          0.7f}, // kEnemyDead
    {"playerdead.mp3",         0.8f}, // kPlayerDead
    {"lose.mp3",               0.8f}, // kLose
    {"clear.mp3",              0.8f}, // kClear
    {"scenechange.mp3",        0.7f}, // kSceneChange
};
static_assert(sizeof(kDefs) / sizeof(kDefs[0]) == static_cast<size_t>(SeId::kCount), "SeId と kDefs の数を合わせてください");
} // namespace

SePlayer& SePlayer::Get() {
	static SePlayer instance;
	return instance;
}

void SePlayer::Initialize() {
	if (loaded_) {
		return;
	}
	Audio* audio = Audio::GetInstance();
	for (int i = 0; i < kCount; ++i) {
		sound_[i] = audio->LoadWave(std::string(kDir) + kDefs[i].file);
		OutputDebugStringA((std::string("[SE] LoadWave ") + kDir + kDefs[i].file + " -> handle " + std::to_string(sound_[i]) + "\n").c_str());
	}
	loaded_ = true;
}

void SePlayer::Play(SeId id, float volume) {
	if (!loaded_) {
		return;
	}
	const int i = static_cast<int>(id);
	const uint32_t voice = Audio::GetInstance()->PlayWave(sound_[i], false, kDefs[i].volume * volume * master_);
	OutputDebugStringA((std::string("[SE] Play ") + kDefs[i].file + " -> voice " + std::to_string(voice) + "\n").c_str());
}