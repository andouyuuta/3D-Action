#include <DxLib.h>
#include "../Application.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "../Object/Grid.h"
#include "../Object/Player.h"
#include "../Object/Sword.h"
#include "../Object/EnemyManager.h"
#include "../Object/Stage.h"
#include "../Object/Collision.h"
#include "../Object/Aicon.h"
#include "../Object/Trail3D.h"
#include "../Application.h"
#include "../Manager/Camera.h"
#include "Game.h"

// コンストラクタ
Game::Game(void)
{
	player_ = nullptr;
	sword_ = nullptr;
	enemymng_ = nullptr;
	stage_ = nullptr;
	camera_ = nullptr;
	collision_ = nullptr;
	aicon_ = nullptr;
}

// デストラクタ
Game::~Game(void)
{
	
}

// 初期化
void Game::Init(void)
{
	// カメラ初期化
	camera_ = new Camera();
	camera_->Init();

	Stage::CreateInstance();
	Stage::GetInstance().Init();

	enemymng_ = new EnemyManager();
	enemymng_->Init();

	// プレイヤー初期化
	Player::CreateInstance();
	Player::GetInstance().Init(enemymng_);

	//剣初期化
	Sword::CreateInstance();
	Sword::GetInstance().Init();


	aicon_ = new Aicon();
	aicon_->Init();

	collision_ = new Collision();
	collision_->Init();

	// カメラの追従対象を設定
	SceneManager& sceneManager = SceneManager::GetInstance();
	Camera* camera = sceneManager.GetCamera();
	camera->SetPlayer(player_);

	// フォグ設定（薄紫色）
	SetFogEnable(TRUE);
	SetFogColor(60,80,60); // 明るい紫
	SetFogStartEnd(1000.0f, 3000.0f);
}

// 更新
void Game::Update(void)
{
	Player::GetInstance().HitStop();
	if (!Player::GetInstance().GetIsHitStop()) {
		// カメラ更新
		camera_->Update();

		//敵更新
		enemymng_->Update();

		// プレイヤー更新
		Player::GetInstance().Update(3);

		//剣更新
		Sword::GetInstance().Update();

		//当たり判定
		collision_->Update(enemymng_->GetEnemyPtrs());

		//敵が全員倒されたらゲームクリアシーンへ移動
		if (enemymng_->GetAliveEnemyCount() <= 0 && enemymng_->GetBossDead())
		{
			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAMECLEAR);
		}

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
}

// 描画
void Game::Draw(void)
{
	ClearDrawScreen();  // 毎フレーム一番最初に！

	//ステージの描画
	Stage::GetInstance().Draw();

	// プレイヤー描画
	Player::GetInstance().Draw();

	//剣描画
	Sword::GetInstance().Draw();

	enemymng_->Draw();

	aicon_->Draw();

	// カメラ設定
	camera_->SetBeforeDraw();

	collision_->Draw(enemymng_->GetEnemyPtrs());

	// 文字の描画
	//DrawFormatString(20, 20, GetColor(0xff, 0xff, 0xff), "ゲーム画面");
	DrawFormatString(20, 80, GetColor(0xff, 0xff, 0xff), "Eキーでゲームオーバー");
	DrawFormatString(20, 100, GetColor(0xff, 0xff, 0xff), "Cキーでゲームクリア");

	// カメラデバックなど
	camera_->Draw();
}

// 解放
void Game::Release(void)
{
	// カメラの解放
	camera_->Release();
	delete camera_;

	aicon_->Release();
	delete aicon_;

	//剣の解放
	Sword::GetInstance().Release();

	// プレイヤーの解放
	Player::GetInstance().Release();

	//敵の解放
	enemymng_->Release();
	delete enemymng_;

	//ステージの解放
	Stage::GetInstance().Release();
}
