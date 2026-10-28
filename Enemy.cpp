#include "Enemy.h"
#include "KamataEngine.h"
#include "Matri4x4.h"
#include "Player.h"
#include <cassert>
#include <cmath>
#include <numbers>

using namespace KamataEngine;

void Enemy::Initialize(Model* model, Camera* camera, const Vector3& position) {
	// 3Dモデルデータが正しく渡されているかチェック
	assert(model);
	model_ = model;
	camera_ = camera;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position; // 指定された初期位置を代入

	// 初期角度を左向きに設定
	worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;

	// アニメーション用の経過時間をリセット
	walkTimer_ = 0.0f;

	// 初期位置を反映したアフィン変換行列（ワールド行列）を作成し、GPUへ転送
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();

	// 初期速度を設定（最初は左に向かって歩く速度）
	velocity_ = {-kWalkSpeed, 0, 0};
}

void Enemy::Update(Player* player) {
	// ---------------------------------------------------------
	// === プレイヤーを追尾する処理（左右） ===
	// ---------------------------------------------------------
	if (player) {
		// プレイヤーの現在位置（座標）を取得
		Vector3 playerPos = player->GetWorldTransform().translation_;

		// プレイヤーへ向かう方向ベクトルを計算
		Vector3 direction = {
		    playerPos.x - worldTransform_.translation_.x, // X軸の差分（左右）
		    playerPos.z - worldTransform_.translation_.z  // Z軸の差分（奥行き）
		};

		// 方向ベクトルの長さ（距離）を計算
		float length = std::sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);

		// ゼロ除算（0での割り算）を防ぎつつ、ベクトルを正規化して速度を決定
		if (length > 0.0f) {
			// 方向ベクトルを長さで割ることで「長さ1の向き」にし、そこに歩行速度を掛ける
			velocity_.x = (direction.x / length) * kWalkSpeed;
			velocity_.y = 0.0f;
			velocity_.z = (direction.z / length) * kWalkSpeed;
		}

		// 移動方向（左右の速度成分）に合わせて、敵のモデルの向き（Y軸の回転）を自然に切り替える
		if (velocity_.x > 0.0f) {
			worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f; // 右を向く (PI / 2 = 90度)
		} else if (velocity_.x < 0.0f) {
			worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f; // 左を向く (PI * 1.5 = 270度)
		}
	}

	// 計算した移動量（velocity_）を敵の現在座標（translation_）に足しこむ
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;

	// ---------------------------------------------------------
	// === 回転アニメーション ===
	// ---------------------------------------------------------
	// 1フレームぶんの時間（1秒/60フレーム）を進める
	walkTimer_ += 1.0f / 60.0f;

	// サイン波を使って -1.0 ～ +1.0 の間でなめらかに変化する値を計算
	float param = std::sin(2.0f * std::numbers::pi_v<float> * walkTimer_ / kWalkMotionTime);

	// 計算したparam（-1～1）を使って、開始角度から終了角度までの間で角度を補間
	float degree = kWalkMotionAngleStart + (kWalkMotionAngleEnd - kWalkMotionAngleStart) * (param + 1.0f) / 2.0f;

	// X軸の回転（身体を前後に揺らす動き）に、度数法から弧度法（ラジアン）へ変換した角度を設定
	worldTransform_.rotation_.x = degree * (std::numbers::pi_v<float> / 180.0f);

	// ---------------------------------------------------------
	// === ワールド行列の更新 ===
	// ---------------------------------------------------------
	// 変更された「スケール」「回転」「位置」を元に最新のアフィン変換行列を合成
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	// 作成した行列データをGPU（描画側）に転送して反映させる
	worldTransform_.TransferMatrix();
}

void Enemy::Draw() {
	Model::PreDraw();
	model_->Draw(worldTransform_, *camera_);
	Model::PostDraw();
}

Vector3 Enemy::GetWorldPosition() {
	Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}

void Enemy::OnCollision(const Player* player) { (void)player; }

Enemy::AABB Enemy::GetAABB() {
	Enemy::AABB aabb;
	Vector3 center = GetWorldPosition();
	float radiusX = kWidth / 2.0f;
	float radiusY = kHeight / 2.0f;
	float radiusZ = 0.0f;
	aabb.min.x = center.x - radiusX;
	aabb.min.y = center.y - radiusY;
	aabb.min.z = center.z - radiusZ;
	aabb.max.x = center.x + radiusX;
	aabb.max.y = center.y + radiusY;
	aabb.max.z = center.z + radiusZ;
	return aabb;
}