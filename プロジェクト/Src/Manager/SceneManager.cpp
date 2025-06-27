#include <chrono>
#include <DxLib.h>
#include "../Common/Fader.h"
#include "../Scene/Title.h"
#include "../Scene/Game.h"
#include "../Scene/GameOver.h"
#include "../Scene/GameClear.h"
#include "SceneManager.h"
#include "Camera.h"


SceneManager* SceneManager::instance_ = nullptr;

void SceneManager::CreateInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new SceneManager();
	}
	instance_->Init();
}

SceneManager& SceneManager::GetInstance(void)
{
	return *instance_;
}

void SceneManager::Init(void)
{

	sceneId_ = SCENE_ID::TITLE;
	waitSceneId_ = SCENE_ID::NONE;

	fader_ = new Fader();
	fader_->Init();

	isSceneChanging_ = false;

	// デルタタイム
	preTime_ = std::chrono::system_clock::now();

	// 初期シーンの設定
	DoChangeScene(SCENE_ID::TITLE);

	Init3D();

}

void SceneManager::Init3D(void)
{
	//背景色設定
	SetBackgroundColor(60,80,60);

	// Zバッファを有効にする
	SetUseZBuffer3D(true);

	// Zバッファへの書き込みを有効にする
	SetWriteZBuffer3D(true);

	//バックカリングを有効にする
	SetUseBackCulling(true);

	//ライトを有効にする
	SetUseLighting(true);

	// ライトをやや斜めに
	ChangeLightTypeDir({ 0.3f, -0.7f, 0.5f });

	// 環境光を追加して陰影をなめらかに
	SetLightAmbColor(GetColorF(100.0f, 100.0f, 100.0f,1.0f));
}

void SceneManager::Update(void)
{

	if (scene_ == nullptr)
	{
		return;
	}

	// デルタタイム
	auto nowTime = std::chrono::system_clock::now();
	deltaTime_ = static_cast<float>(
		std::chrono::duration_cast<std::chrono::nanoseconds>(nowTime - preTime_).count() / 1000000000.0);
	preTime_ = nowTime;

	fader_->Update();
	if (isSceneChanging_)
	{
		Fade();
	}
	else
	{
		scene_->Update();
	}
}

void SceneManager::Draw(void)
{

	// 描画先グラフィック領域の指定
	// (３Ｄ描画で使用するカメラの設定などがリセットされる)
	SetDrawScreen(DX_SCREEN_BACK);

	// 画面を初期化
	ClearDrawScreen();

	// 描画
	scene_->Draw();

	// 暗転・明転
	fader_->Draw();

}

void SceneManager::Release(void)
{

	scene_->Release();
	delete scene_;

	delete fader_;

}

void SceneManager::ChangeScene(SCENE_ID nextId)
{

	// フェード処理が終わってからシーンを変える場合もあるため、
	// 遷移先シーンをメンバ変数に保持
	waitSceneId_ = nextId;

	// フェードアウト(暗転)を開始する
	fader_->SetFade(Fader::STATE::FADE_OUT);
	isSceneChanging_ = true;

}

SceneManager::SCENE_ID SceneManager::GetSceneID(void)
{
	return sceneId_;
}

float SceneManager::GetDeltaTime(void) const
{
	//return 1.0f / 60.0f;
	return deltaTime_;
}

SceneManager::SceneManager(void)
{

	sceneId_ = SCENE_ID::NONE;
	waitSceneId_ = SCENE_ID::NONE;

	scene_ = nullptr;
	fader_ = nullptr;

	isSceneChanging_ = false;

	// デルタタイム
	deltaTime_ = 1.0f / 60.0f;

}

SceneManager::~SceneManager(void)
{
	delete instance_;
}

void SceneManager::ResetDeltaTime(void)
{
	deltaTime_ = 0.016f;
	preTime_ = std::chrono::system_clock::now();
}

void SceneManager::DoChangeScene(SCENE_ID sceneId)
{

	// シーンを変更する
	sceneId_ = sceneId;

	// 現在のシーンを解放
	if (scene_ != nullptr)
	{
		scene_->Release();
		delete scene_;
	}

	switch (sceneId_)
	{
	case SCENE_ID::TITLE:
		scene_ = new Title();
		break;
	case SCENE_ID::GAME:
		scene_ = new Game();
		break;
	case SCENE_ID::GAMEOVER:
		scene_ = new GameOver();
		break;
	case SCENE_ID::GAMECLEAR:
		scene_ = new GameClear();
		break;
	}

	scene_->Init();

	ResetDeltaTime();

	waitSceneId_ = SCENE_ID::NONE;

}

void SceneManager::Fade(void)
{

	Fader::STATE fState = fader_->GetState();
	switch (fState)
	{
	case Fader::STATE::FADE_IN:
		// 明転中
		if (fader_->IsEnd())
		{
			// 明転が終了したら、フェード処理終了
			fader_->SetFade(Fader::STATE::NONE);
			isSceneChanging_ = false;
		}
		break;
	case Fader::STATE::FADE_OUT:
		// 暗転中
		if (fader_->IsEnd())
		{
			// 完全に暗転してからシーン遷移
			DoChangeScene(waitSceneId_);
			// 暗転から明転へ
			fader_->SetFade(Fader::STATE::FADE_IN);
		}
		break;
	}

}


