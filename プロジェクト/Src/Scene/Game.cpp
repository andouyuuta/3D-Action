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
#include "../UI/Aicon.h"
#include "../Object/Trail3D.h"
#include "../Manager/AnimationManager.h"
#include "../Manager/BuffDataManager.h"
#include "../Application.h"
#include "../Manager/Camera.h"
#include "../UI/HpBar.h"
#include "../UI/Buff.h"
#include "Game.h"

// コンストラクタ
Game::Game(void)
{
	selectindex_ = 0;
	menuSizeX_ = 0;
	menuSizeY_ = 0;
	menuimg_ = -1;
	centerY_ = 0;
	centerX_ = 0;
	isMenu_ = false;
	player_ = nullptr;
	sword_ = nullptr;
	enemymng_ = nullptr;
	stage_ = nullptr;
	camera_ = nullptr;
	collision_ = nullptr;
	aicon_ = nullptr;
	hpbar_ = nullptr;
	animation_ = nullptr;
	buff_ = nullptr;
}

// デストラクタ
Game::~Game(void)
{
	
}

// 初期化
void Game::Init(void)
{
	isMenu_ = false;
	menuimg_ = LoadGraph("Data/Image/Menu.png");
	GetGraphSize(menuimg_, &menuSizeX_, &menuSizeY_);

	centerX_ = Application::HALF_SCREEN_SIZE_X;
	centerY_ = Application::HALF_SCREEN_SIZE_Y;

	selectindex_ = 0;
	// カメラ初期化
	camera_ = new Camera();
	enemymng_ = new EnemyManager();
	animation_ = new AnimationManager();
	player_ = new Player();
	sword_ = new Sword();
	aicon_ = new Aicon();
	collision_ = new Collision();
	stage_ = new Stage();
	hpbar_ = new HpBar();
	buff_ = new Buff();

	// 初期化処理
	stage_->Init();
	camera_->Init(player_);
	animation_->Init(player_,enemymng_);
	enemymng_->Init(player_, animation_, animation_->GetEnemyInfo(),buff_);
	player_->Init(enemymng_, animation_, camera_);
	sword_->Init(player_);
	aicon_->Init(player_);
	collision_->Init(player_, sword_, stage_,animation_);
	hpbar_->Init(player_);
	buff_->Init(player_,enemymng_);

	// フォグ設定（薄紫色）
	SetFogEnable(TRUE);
	SetFogColor(60,80,60); // 明るい紫
	SetFogStartEnd(1000.0f, 3000.0f);
}

// 更新
void Game::Update(void)
{
	InputManager& ins = InputManager::GetInstance();
	// TABキーかパッドのメニューボタンでメニュー画面
	if (ins.IsTrgDown(KEY_INPUT_TAB) ||
		ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::MENU))
	{
		isMenu_ = !isMenu_;
	}

	// アニメーション
	animation_->Update();
	// 剣
	sword_->Update();

	// --- バフスキル選択中の処理 ---
	if (enemymng_->IsWaitingForSkillSelect())
	{
		SetMouseDispFlag(true);
		enemymng_->ShowSkillSelect();

		// スキル選択中はゲーム本編の処理をスキップ
		return;
	}

	// メニュー画面が出ていないなら更新
	if (!isMenu_)
	{
		SetMouseDispFlag(false);
		// プレイヤーのヒットストップ時には更新止める
		player_->HitStop();
		if (!player_->GetIsHitStop()) {
			// カメラ
			camera_->GameUpdate();
			//敵
			enemymng_->Update();
			// プレイヤー
			player_->Update(3);
			//当たり判定
			collision_->Update(enemymng_->GetEnemyPtrs());

			//敵が全員倒されたらゲームクリアシーンへ移動
			if (enemymng_->GetAliveEnemyCount() <= 0 && enemymng_->GetBossDead())
			{
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAMECLEAR);
			}

			// シーン遷移
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
	else
	{
		InputManager::JOYPAD_IN_STATE pad = ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);
		// マウス感度 → ゲームパッド用の感度に置き換え
		float padSensi = 0.03f;  // 感度調整（必要に応じてチューニング）

		// ゲームパッドのスティック入力を正規化
		float lx = pad.AKeyLX / 1000.0f;  // 左右

		if ((ins.IsTrgDown(KEY_INPUT_LEFT) || lx < 0) && selectindex_ > 0)
		{
			selectindex_--;
		}
		if ((ins.IsTrgDown(KEY_INPUT_RIGHT) || lx > 0) && selectindex_ < OPTION_COUNT - 1) {
			selectindex_++;
			// 選択音などあれば再生
		}
		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
		{
			if (selectindex_ == 0)
			{
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
			}
			else
			{
				isMenu_ = false;
			}
		}
	}
}

// 描画
void Game::Draw(void)
{
	ClearDrawScreen();  // 毎フレーム一番最初に！

	// カメラ設定
	camera_->SetBeforeDraw();

	//ステージ
	stage_->Draw();
	// プレイヤー
	player_->Draw();
	//剣
	sword_->Draw();
	// 敵
	enemymng_->Draw();
	// アイコン
	aicon_->Draw();
	// HPバー
	hpbar_->Draw();

	// デバッグ用
	// 当たり判定
	collision_->Draw(enemymng_->GetEnemyPtrs());
	// 文字の描画
	//DrawFormatString(20, 20, GetColor(0xff, 0xff, 0xff), "ゲーム画面");
	DrawFormatString(20, Application::SCREEN_SIZE_Y-40, GetColor(0xff, 0xff, 0xff), "Eキーでゲームオーバー");
	DrawFormatString(20, Application::SCREEN_SIZE_Y-20, GetColor(0xff, 0xff, 0xff), "Cキーでゲームクリア");

	// カメラデバックなど
	//camera_->Draw();

	if (isMenu_)
	{
		DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 0xffffff, true);

		DrawRectGraph(centerX_ - menuSizeX_ / 2,
			centerY_ - menuSizeY_ / 2,
			0, 0,
			menuSizeX_, menuSizeY_/2-50,
			menuimg_, true
		);

		if (selectindex_ == 0)
		{
			DrawRectGraph(0,
				menuSizeY_,
				0, menuSizeY_ / 2 - 50,
				menuSizeX_ / 2, menuSizeY_,
				menuimg_, true
			);
		}
		else
		{
			DrawRectGraph(centerX_,
				menuSizeY_,
				menuSizeX_ / 2, menuSizeY_ / 2 - 50,
				menuSizeX_, menuSizeY_,
				menuimg_, true
			);
		}
		SetDrawBright(255, 255, 255);
	}
	// --- バフスキル選択中の処理 ---
	if (enemymng_->IsWaitingForSkillSelect())
	{
		buff_->Draw();
	}
}

// 解放
void Game::Release(void)
{
	// カメラの解放
	camera_->Release();
	delete camera_;
	// HPバー
	hpbar_->Release();
	delete hpbar_;
	// アイコン
	aicon_->Release();
	delete aicon_;
	//剣の解放
	sword_->Release();
	delete sword_;
	// プレイヤーの解放
	player_->Release();
	delete player_;
	//敵の解放
	enemymng_->Release();
	delete enemymng_;
	//ステージの解放
	stage_->Release();
	delete stage_;
	// バフ
	buff_->Release();
	delete buff_;
}