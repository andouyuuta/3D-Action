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
#include "../Object/BossEnemy.h"
#include "../Manager/AnimationManager.h"
#include "../Manager/BuffDataManager.h"
#include "../Manager/MoviePlayer.h"
#include "../Manager/SoundManager.h"
#include "../Application.h"
#include "../Manager/Camera.h"
#include "../UI/HpBar_.h"
#include "../UI/Buff.h"
#include "../Object/EnemyManager.h"
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
	isBossBGMStated = false;

	isMenu_ = false;
	player_ = nullptr;
	sword_ = nullptr;
	enemymng_ = nullptr;
	stage_ = nullptr;
	camera_ = nullptr;
	collision_ = nullptr;
	aicon_ = nullptr;
	hpBar_ = nullptr;
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
	isBossBGMStated = false;
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
	hpBar_ = new HpBar();
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
	hpBar_->Init(player_, enemymng_);
	buff_->Init(player_,enemymng_);

	// フォグ設定（薄紫色）
	SetFogEnable(TRUE);
	SetFogColor(60,80,60); // 明るい紫
	SetFogStartEnd(1000.0f, 3000.0f);

	// シングルトンの呼び出し
	SoundManager* seMana_ = SoundManager::GetInstance();

	// BGMのロード
	seMana_->LoadBGM("GameBGM", (Application::PATH_SOUND + "BGM/Shadow_Moon.mp3").c_str(), false);
	// ボス用
	seMana_->LoadBGM("BossBGM", (Application::PATH_SOUND + "BGM/Boss.mp3").c_str(), false);
}

// 更新
void Game::Update(void)
{
	// インスタンスの取得
	InputManager& ins = InputManager::GetInstance();

	// シングルトンの呼び出し
	SoundManager* seMana_ = SoundManager::GetInstance();

	// BGMの再生
	seMana_->SetVolumeSE("GameBGM", 200);
	// ボス用
	seMana_->SetVolumeSE("BossBGM", 200);

	// ボスかどうか
	if (!enemymng_->GetIsBoss())
	{
		seMana_->PlayBGM("GameBGM");
	}

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
			if (player_->GetIsUltPlay())
			{
				player_->MovieUpdate();
				return;
			}
			// プレイヤー
			player_->Update(3);
			// カメラ
			camera_->GameUpdate();
			//敵
			enemymng_->Update();
			//当たり判定
			collision_->Update(enemymng_->GetEnemyPtrs());

			if (enemymng_->GetIsBoss() == true&& !isBossBGMStated)
			{
				seMana_->StopBGM("GameBGM");
				seMana_->PlayBGM("BossBGM");
				isBossBGMStated = true;
			}

			//敵が全員倒されたらゲームクリアシーンへ移動
			if (enemymng_->GetAliveEnemyCount() <= 0 && enemymng_->GetBossDead())
			{
				seMana_->StopBGM("GameBGM");
				seMana_->StopBGM("BossBGM");
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAMECLEAR);
			}

			// シーン遷移
			if (ins.IsTrgDown(KEY_INPUT_C))
			{
				seMana_->StopBGM("GameBGM");
				seMana_->StopBGM("BossBGM");
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAMECLEAR);
			}
			if (ins.IsTrgDown(KEY_INPUT_E))
			{
				seMana_->StopBGM("GameBGM");
				seMana_->StopBGM("BossBGM");
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAMEOVER);
			}
		}
	}
	else
	{
		SetMouseDispFlag(true);
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
				seMana_->StopBGM("GameBGM");
				seMana_->StopBGM("BossBGM");
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
			}
			else
			{
				isMenu_ = false;
			}
			seMana_->StopBGM("GameBGM");
			seMana_->StopBGM("BossBGM");
		}
		
		// マウス位置取得
		int mouseX, mouseY;
		GetMousePoint(&mouseX, &mouseY);

		// YESとNOのボタン領域（※画像サイズと位置に応じて調整）
		int yesX = centerX_ - menuSizeX_ / 2;
		int yesY = centerY_;
		int noX = centerX_;
		int noY = centerY_;
		int btnWidth = menuSizeX_ / 2;
		int btnHeight = menuSizeY_ / 2 + 50;

		// YESがホバーされていたら
		if (mouseX >= yesX && mouseX <= yesX + btnWidth &&
			mouseY >= yesY && mouseY <= yesY + btnHeight)
		{
			selectindex_ = 0;

			if (ins.IsTrgMouseLeft())
			{
				// YESがクリックされた
				seMana_->StopBGM("GameBGM");
				seMana_->StopBGM("BossBGM");
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
			}
		}
		// NOがホバーされていたら
		else if (mouseX >= noX && mouseX <= noX + btnWidth &&
			mouseY >= noY && mouseY <= noY + btnHeight)
		{
			selectindex_ = 1;

			if (ins.IsTrgMouseLeft())
			{
				// NOがクリックされた
				isMenu_ = false;
				seMana_->StopBGM("GameBGM");
				seMana_->StopBGM("BossBGM");
			}
		}
	}
}

// 描画
void Game::Draw(void)
{
	ClearDrawScreen();  // 毎フレーム一番最初に！

	if (isMenu_)
	{
		DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 0xffffff, true);

		DrawRectGraph(centerX_ - menuSizeX_ / 2,
			centerY_ - menuSizeY_ / 2,
			0, 0,
			menuSizeX_, menuSizeY_ / 2 - 50,
			menuimg_, true
		);

		// 下部分：[YES] [NO] の画像表示（選択中だけ拡大）
		int optionWidth = menuSizeX_ / 2;
		int optionHeight = menuSizeY_ / 2 + 50;

		// YES（左）描画位置
		int yesX = centerX_ - menuSizeX_ / 2;
		int yesY = centerY_;

		// NO（右）描画位置
		int noX = centerX_;
		int noY = centerY_;

		int btnWidth = menuSizeX_ / 2;
		int btnHeight = menuSizeY_ / 2 + 50;

		if (selectindex_ == 0)
		{
			// YESを拡大
			DrawRectExtendGraph(
				yesX - (optionWidth * (scale - 1.0f) / 2),
				yesY - (optionHeight * (scale - 1.0f) / 2),
				yesX + optionWidth + (optionWidth * (scale - 1.0f) / 2),
				yesY + optionHeight + (optionHeight * (scale - 1.0f) / 2),
				0, menuSizeY_ / 2 - 50,
				optionWidth, optionHeight,
				menuimg_, true
			);

			// NOを通常サイズ
			DrawRectGraph(
				noX,
				noY,
				optionWidth, menuSizeY_ / 2 - 50,
				optionWidth, optionHeight,
				menuimg_, true
			);
		}
		else
		{
			// YESを通常サイズ
			DrawRectGraph(
				yesX,
				yesY,
				0, menuSizeY_ / 2 - 50,
				optionWidth, optionHeight,
				menuimg_, true
			);

			// NOを拡大
			DrawRectExtendGraph(
				noX - (optionWidth * (scale - 1.0f) / 2),
				noY - (optionHeight * (scale - 1.0f) / 2),
				noX + optionWidth + (optionWidth * (scale - 1.0f) / 2),
				noY + optionHeight + (optionHeight * (scale - 1.0f) / 2),
				optionWidth, menuSizeY_ / 2 - 50,
				optionWidth, optionHeight,
				menuimg_, true
			);
		}		SetDrawBright(255, 255, 255);

		DrawBox(noX, noY, noX + btnWidth, noY + btnHeight, 0x00ff00, false); // 緑枠で強調
		DrawBox(yesX, yesY, yesX + btnWidth, yesY + btnHeight, 0x00ff00, false); // 緑枠で強調

		DrawFormatString(0, 0, 0x000000, "攻撃力：%d", player_->GetAttackPower());
		DrawFormatString(0, 20, 0x000000, "体力：%d / %d", player_->GetHp(), player_->GetMaxHp());
		DrawFormatString(0, 40, 0x000000, "敵攻撃力：%d", enemymng_->GetTotalAttack());
		DrawFormatString(0, 60, 0x000000, "敵体力：%d", enemymng_->GetTotalHp());
		return;
	}

	if (player_->GetIsUltPlay())
	{
		player_->MovieDraw();
		return;
	}

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
	hpBar_->Draw();

	// デバッグ用
	// 当たり判定
	collision_->Draw(enemymng_->GetEnemyPtrs());

	// 文字の描画
	DrawFormatString(20, Application::SCREEN_SIZE_Y - 40, GetColor(0xff, 0xff, 0xff), "Eキーでゲームオーバー");
	DrawFormatString(20, Application::SCREEN_SIZE_Y - 20, GetColor(0xff, 0xff, 0xff), "Cキーでゲームクリア");

	// --- バフスキル選択中の処理 ---
	if (enemymng_->IsWaitingForSkillSelect())
	{
		buff_->Draw();
		return;
	}
}

// 解放
void Game::Release(void)
{
	// シングルトンの呼び出し
	SoundManager* seMana_ = SoundManager::GetInstance();

	// BGMの停止
	seMana_->StopBGM("GameBGM");
	seMana_->StopBGM("BossBGM");

	// 解放処理
	seMana_->ReleaseSound("GameBGM");
	seMana_->ReleaseSound("BossBGM");

	// アニメーションの解放
	animation_->Release();
	delete animation_;
	animation_ = nullptr;
	// カメラの解放
	camera_->Release();
	delete camera_;
	camera_ = nullptr;
	// HPバー
	hpBar_->Release();
	delete hpBar_;
	hpBar_ = nullptr;
	// アイコン
	aicon_->Release();
	delete aicon_;
	aicon_ = nullptr;
	//剣の解放
	sword_->Release();
	delete sword_;
	sword_ = nullptr;
	// プレイヤーの解放
	player_->Release();
	delete player_;
	player_ = nullptr;
	//敵の解放
	enemymng_->Release();
	delete enemymng_;
	enemymng_ = nullptr;
	//ステージの解放
	stage_->Release();
	delete stage_;
	stage_ = nullptr;
	// バフ
	buff_->Release();
	delete buff_;
	buff_ = nullptr;
	// 当たり判定
	collision_->Release();
	delete collision_;
	collision_ = nullptr;
}