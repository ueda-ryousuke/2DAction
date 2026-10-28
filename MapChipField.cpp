#include "MapChipField.h"
#include <fstream>
#include <map>
#include <sstream>
#include<cassert>
using namespace KamataEngine;

namespace {
std::map<std::string, MapChipType> mapChipTable = {
    {"0", MapChipType::kBlank},
    {"1", MapChipType::kBlock},
};
}

void MapChipField::ResetMapChipData() {
	mapChipData_.data.clear();

	mapChipData_.data.resize(kNumBlockVertical);

	for (std::vector<MapChipType>& mapChipDataLine : mapChipData_.data) {
		mapChipDataLine.resize(kNumBlockHorizontal);
	}
}

void MapChipField::LoadMapChipCsv(const std::string& filePath) {
	// マップチップデータをリセット
	ResetMapChipData();

	std::ifstream file;
	file.open(filePath);
	assert(file.is_open());

	std::stringstream mapChipCsv;

	mapChipCsv << file.rdbuf();
	file.close();

	for (uint32_t i = 0; i < kNumBlockVertical; ++i) {
		std::string line;
		getline(mapChipCsv, line);

		std::istringstream lineStream(line);

		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			std::string word;
			std::getline(lineStream, word, ',');

			if (mapChipTable.contains(word)) {
				mapChipData_.data[i][j] = mapChipTable[word];
			}
		}
	}
}

MapChipType MapChipField::GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex) {
	if (xIndex >= kNumBlockHorizontal) {
		return MapChipType::kBlank;
	}

	if (yIndex >= kNumBlockVertical) {
		return MapChipType::kBlank;
	}

	return mapChipData_.data[yIndex][xIndex];
}

Vector3 MapChipField::GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex) { return Vector3(kBlockWidth * xIndex, kBlockHeight * (kNumBlockVertical - 1 - yIndex), 0); }

Vector3 MapChipField::GetMapChipPosition(uint32_t xIndex, uint32_t yIndex) { return Vector3(kBlockWidth * xIndex, kBlockHeight * (kNumBlockVertical - 1 - yIndex), 0); }

uint32_t MapChipField::GetNumBlockVertical() const { return kNumBlockVertical; }

uint32_t MapChipField::GetNumBlockHorizontal() const { return kNumBlockHorizontal; }

MapChipField::IndexSet MapChipField::GetMapChipIndexSetByPosition(const KamataEngine::Vector3& position) {
	IndexSet indexSet = {};
	// 座標から正しくインデックスを算出（中心オフセットの計算順序を整理）
	indexSet.xIndex = static_cast<uint32_t>((position.x + (kBlockWidth / 2.0f)) / kBlockWidth);
	indexSet.yIndex = static_cast<uint32_t>((position.y + (kBlockHeight / 2.0f)) / kBlockHeight);

	// Y軸方向にマップを反転
	indexSet.yIndex = kNumBlockVertical - 1 - indexSet.yIndex;

	return indexSet; 
}

MapChipField::Rect MapChipField::GetRectByIndex(uint32_t xIndex, uint32_t yIndex) {
	Vector3 center = GetMapChipPositionByIndex(xIndex, yIndex);
	Rect rect;
	rect.left = center.x - kBlockWidth / 2.0f;
	rect.right = center.x + kBlockWidth / 2.0f; 
	rect.bottom = center.y - kBlockHeight / 2.0f;
	rect.top = center.y + kBlockHeight / 2.0f; 

	return rect; 
}