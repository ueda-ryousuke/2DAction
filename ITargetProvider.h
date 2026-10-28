#pragma once
#include "KamataEngine.h"

// ホーミング弾が「狙う相手」を問い合わせるための窓口。
// 実装は EnemyManager。弾クラスが敵クラスを直接知らなくて済む。
class ITargetProvider {
public:
	virtual ~ITargetProvider() = default;
	// from の近くで一番近い狙える相手の位置を返す。いなければ false。
	virtual bool FindNearestTarget(const KamataEngine::Vector3& from, float maxRange, KamataEngine::Vector3& out) const = 0;
};
