#include "TextureLibrary.h"
#include "KamataEngine.h"

std::map<std::string, uint32_t>& TextureLibrary::Cache() {
	static std::map<std::string, uint32_t> cache;
	return cache;
}

uint32_t TextureLibrary::Ui(const std::string& fileName) {
	auto& cache = Cache();
	auto it = cache.find(fileName);
	if (it != cache.end()) {
		return it->second;
	}
	uint32_t handle = KamataEngine::TextureManager::Load("ui/" + fileName);
	cache[fileName] = handle;
	return handle;
}
