
#include <cmath>
#include "Matri4x4.h"

using namespace KamataEngine;


// 単位行列
Matrix4x4 MakeIdentity4x4() {
	Matrix4x4 result{};

	for (int i = 0; i < 4; i++) {
		result.m[i][i] = 1.0f;
	}

	return result;
}


// X軸回転
Matrix4x4 MakeRotateXMatrix(float radian) {

	Matrix4x4 result = MakeIdentity4x4();

	result.m[1][1] = cosf(radian);
	result.m[1][2] = sinf(radian);

	result.m[2][1] = -sinf(radian);
	result.m[2][2] = cosf(radian);

	return result;
}

// Y軸回転
Matrix4x4 MakeRotateYMatrix(float radian) {

	Matrix4x4 result = MakeIdentity4x4();

	result.m[0][0] = cosf(radian);
	result.m[0][2] = -sinf(radian);

	result.m[2][0] = sinf(radian);
	result.m[2][2] = cosf(radian);

	return result;
}

// Z軸回転
Matrix4x4 MakeRotateZMatrix(float radian) {

	Matrix4x4 result = MakeIdentity4x4();

	result.m[0][0] = cosf(radian);
	result.m[0][1] = sinf(radian);

	result.m[1][0] = -sinf(radian);
	result.m[1][1] = cosf(radian);

	return result;
}

// XYZ回転まとめ
Matrix4x4 MakeRotateMatrix(const Vector3& rotate) {

	Matrix4x4 rotateX = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotateY = MakeRotateYMatrix(rotate.y);
	Matrix4x4 rotateZ = MakeRotateZMatrix(rotate.z);

	// X → Y → Z の順
	Matrix4x4 result = Multiply(rotateX, Multiply(rotateY, rotateZ));

	return result;
}

	// 拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale) {

	Matrix4x4 result = MakeIdentity4x4();

	result.m[0][0] = scale.x;
	result.m[1][1] = scale.y;
	result.m[2][2] = scale.z;

	return result;
}

// 平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {

	Matrix4x4 result = MakeIdentity4x4();

	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;

	return result;
}

// アフィン変換行列
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);
	Matrix4x4 rotateMatrix = MakeRotateMatrix(rotate);
	Matrix4x4 translateMatrix = MakeTranslateMatrix(translate);

	Matrix4x4 result = Multiply(scaleMatrix, Multiply(rotateMatrix, translateMatrix));

	return result;
}
// 乗算
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result{};

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {

			result.m[i][j] = m1.m[i][0] * m2.m[0][j] + m1.m[i][1] * m2.m[1][j] + m1.m[i][2] * m2.m[2][j] + m1.m[i][3] * m2.m[3][j];
		}
	}

	return result;
}


