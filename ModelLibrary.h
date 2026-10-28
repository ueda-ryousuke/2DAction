#pragma once
#include "KamataEngine.h"
#include <map>
#include <string>

// 3Dモデル(OBJ)の読み込みと所有を一手に引き受ける。 Resources/<名前>/<名前>.obj を読む。
class ModelLibrary {
public:
	static ModelLibrary& Get();

	void LoadAll();                                       // ゲーム起動時に1回
	KamataEngine::Model* Find(const std::string& name) const; // 見つからなければ nullptr
	void Release();                                       // 終了時

private:
	ModelLibrary() = default;
	std::map<std::string, KamataEngine::Model*> models_;
};
