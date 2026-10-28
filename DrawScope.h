#pragma once
#include "KamataEngine.h"

// 描画パスのRAII。3Dモデルとスプライトは別パスなので、1フレームにつき各1回だけ括る。
//   { ModelDrawScope s; /* model->Draw(...) */ }
//   { SpriteDrawScope s; /* sprite->Draw() */ }
struct ModelDrawScope {
	ModelDrawScope() { KamataEngine::Model::PreDraw(); }
	~ModelDrawScope() { KamataEngine::Model::PostDraw(); }
};

struct SpriteDrawScope {
	SpriteDrawScope() { KamataEngine::Sprite::PreDraw(KamataEngine::DirectXCommon::GetInstance()->GetCommandList()); }
	~SpriteDrawScope() { KamataEngine::Sprite::PostDraw(); }
};
