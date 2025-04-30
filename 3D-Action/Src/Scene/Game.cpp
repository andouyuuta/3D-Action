#include <DxLib.h>
#include "../Application.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "../Object/Grid.h"
#include "../Object/Player.h"
#include "../Object/Sword.h"
#include "../Object/Enemy.h"
#include "../Object/EnemyManager.h"
#include "../Application.h"
#include "../Manager/Camera.h"
#include "Game.h"

// コンストラクタ
Game::Game(void)
{
	grid_ = nullptr;
	player_ = nullptr;
	sword_ = nullptr;
	enemy_ = nullptr;
	enemymng_ = nullptr;
	camera_ = nullptr;
}

// デストラクタ
Game::~Game(void)
{
	
}

// 初期化
void Game::Init(void)
{
	// グリッド初期化
	grid_ = new Grid();
	grid_->SystemInit();

	// カメラ初期化
	camera_ = new Camera();
	camera_->Init();

	// プレイヤー初期化
	player_ = new Player();
	player_->SystemInit();

	//剣初期化
	sword_ = new Sword();
	sword_->Init();

	enemymng_ = new EnemyManager();
	enemymng_->Init();

	// グリッド初期化
	grid_->GameInit();

	// プレイヤー初期化
	player_->GameInit();

	// カメラの追従対象を設定
	SceneManager& sceneManager = SceneManager::GetInstance();
	Camera* camera = sceneManager.GetCamera();
	camera->SetPlayer(player_);

}

// 更新
void Game::Update(void)
{
	// グリッド更新
	grid_->Update();

	// カメラ更新
	camera_->Update();

	// プレイヤー更新
	player_->Update();

	//剣更新
	sword_->Update();

	enemymng_->Update();

	// シーン遷移
	InputManager& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_C))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAMECLEAR);
	}
	if (ins.IsTrgDown(KEY_INPUT_E))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAMEOVER);
	}
}

// 描画
void Game::Draw(void)
{
	// グリッドの描画
	grid_->Draw();

	// プレイヤー描画
	player_->Draw();

	//剣描画
	sword_->Draw();

	//enemy_->Draw();
	enemymng_->Draw();

	// カメラ設定
	camera_->SetBeforeDraw();

	// 文字の描画
	DrawFormatString(20, 20, GetColor(0xff, 0xff, 0xff), "ゲーム画面");
	DrawFormatString(20, 80, GetColor(0xff, 0xff, 0xff), "Eキーでゲームオーバー");
	DrawFormatString(20, 100, GetColor(0xff, 0xff, 0xff), "Cキーでゲームクリア");

	// カメラデバックなど
	camera_->Draw();
}

// 解放
void Game::Release(void)
{
	// グリッドの解放
	grid_->Release();
	delete grid_;

	// カメラの解放
	camera_->Release();
	delete camera_;

	//剣の解放
	sword_->Release();
	delete sword_;

	// プレイヤーの解放
	player_->Release();
	delete player_;

	enemymng_->Release();
	delete enemymng_;
}
