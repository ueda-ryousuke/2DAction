#pragma once
#include <cstdint>
#include <map>
#include <string>

// UI画像(Resources/ui/*.png。TextureManager::Load は先頭に Resources/ を自動で付ける)のハンドルをキャッシュする。
class TextureLibrary {
public:
	static uint32_t Ui(const std::string& fileName); // 例: Ui("title_logo.png")

private:
	static std::map<std::string, uint32_t>& Cache();
};
