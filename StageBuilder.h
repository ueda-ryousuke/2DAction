#pragma once
#include "KamataEngine.h"

class MapChipField;
class GimmickManager;
class EnemyManager;

// ステージごとのギミック・敵・ゴールの配置。位置は「マップチップの(列,行)」で指定する。
// ※ 配置はコードに直書きしているが、数が増えたら CSV の別レイヤー(例: stage1_objects.csv)から読む形に置き換えやすいよう、
//   「Cell(列,行)」で座標を出す書き方に統一してある。
class StageBuilder {
public:
	// 配置を行い、プレイヤーの開始座標を返す
	static KamataEngine::Vector3 Build(int stageIndex, MapChipField& field, GimmickManager& gimmicks, EnemyManager& enemies);
};
