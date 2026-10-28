#pragma once

#include "KamataEngine.h"

using namespace KamataEngine;

	static KamataEngine::Matrix4x4 MakeIdentity4x4();
	
	// 拡大縮小行列
    static KamataEngine::Matrix4x4 MakeScaleMatrix(const KamataEngine::Vector3& scale);

	// 平行移動行列
    static KamataEngine::Matrix4x4 MakeTranslateMatrix(const KamataEngine::Vector3& translate);

	KamataEngine::Matrix4x4 Multiply(const KamataEngine::Matrix4x4& m1, const KamataEngine::Matrix4x4& m2);


	// Z軸回転
    KamataEngine::Matrix4x4 MakeRotateXMatrix(float radian);
    KamataEngine::Matrix4x4 MakeRotateYMatrix(float radian);
    KamataEngine::Matrix4x4 MakeRotateZMatrix(float radian);
	// XYZ回転まとめ
    KamataEngine::Matrix4x4 MakeRotateMatrix(const KamataEngine::Vector3& rotate);

	
	KamataEngine::Matrix4x4 MakeAffineMatrix(const KamataEngine::Vector3& scale, const KamataEngine::Vector3& rotate, const KamataEngine::Vector3& translate);
