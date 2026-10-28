#include "BgmPlayer.h"
#include "KamataEngine.h"
#include "ModelLibrary.h"
#include "SceneManager.h"
#include "SePlayer.h"
#include <Windows.h>

using namespace KamataEngine;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// エンジンの初期化
	Initialize(L"壁際ジャンパー");

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 3Dモデルは起動時にまとめて読み込む
	ModelLibrary::Get().LoadAll();
	SePlayer::Get().Initialize();                     // Resources/se/*.mp3 を読み込む
	BgmPlayer::Get().Initialize();                    // Resources/bgm/bgm.mp3 を読み込む
	BgmPlayer::Get().Play(BgmId::kTitle, true, 0.0f); // 起動した瞬間から（フェードインなしで）鳴らす

	SceneManager* sceneManager = new SceneManager();
	sceneManager->Initialize();

	// メインループ
	while (true) {
		if (Update()) {
			break;
		}

		sceneManager->Update();

		// 描画開始
		dxCommon->PreDraw();
		sceneManager->Draw();
		// 描画終了
		dxCommon->PostDraw();
	}

	delete sceneManager;
	sceneManager = nullptr;

	BgmPlayer::Get().Stop(0.0f);
	ModelLibrary::Get().Release();

	// エンジンの終了処理
	Finalize();

	return 0;
}