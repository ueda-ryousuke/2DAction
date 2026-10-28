#include "SplitEnemy.h"
#include "EnemyFactory.h"
#include "EnemyManager.h"
#include "SePlayer.h"
#include <cmath>

using namespace KamataEngine;

void SplitEnemy::OnInitialize() {
	const float k = std::pow(0.7f, static_cast<float>(generation_)); // 世代ごとに 70%
	SetBody(generation_ == 0 ? 2 : 1, 1.0f * k, 0.9f * k);
	SetVisual(2.0f * k, -0.45f);
	hopTimer_.Start(kHopInterval * 0.5f);
}

void SplitEnemy::Update(EnemyContext& ctx) {
	const float dx = ctx.playerPos.x - pos_.x;
	hopTimer_.Update();

	if (onGround_) {
		vel_.x *= 0.85f; // 着地したら横の勢いを落とす
	}
	if (onGround_ && hopTimer_.IsFinished()) {
		Face(dx);
		vel_.y = kHopPower;
		vel_.x = facing_ * (kWalkSpeed + 0.015f * static_cast<float>(generation_)) * 4.0f; // ぴょんと跳ねて近づく
		onGround_ = false;
		hopTimer_.Start(kHopInterval);
	}
	StepPhysics();
	UpdateTransform();
}

void SplitEnemy::OnDeath(EnemyContext& ctx) {
	if (generation_ < kMaxGeneration && ctx.manager) {
		SePlayer::Get().Play(SeId::kSplit); // 子が生まれるときだけ分裂音
	}
	if (generation_ >= kMaxGeneration || !ctx.manager) {
		return;
	}
	for (float side : {-1.0f, 1.0f}) {
		Vector3 p = {pos_.x + side * 0.8f, pos_.y + 0.3f, pos_.z};
		std::unique_ptr<EnemyBase> child = EnemyFactory::CreateSplitChild(params_, p, generation_ + 1);
		child->SetVelocity(side * 0.18f, 0.28f); // 左右に飛び散る
		ctx.manager->Add(std::move(child));
	}
}