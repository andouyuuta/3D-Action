#include <cmath>
#include <DxLib.h>
#include "../Application.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "GameOver.h"


// コンストラクタ
GameOver::GameOver(void) : SceneBase()
{
	waitFrame_ = 0;
}

// デストラクタ
GameOver::~GameOver(void)
{
}

// 初期化
void GameOver::Init(void)
{
	// 画像の読み込み
	hightLight_ = LoadGraph("Data/Image/hightLight.png");

	waitFrame_ = 10;

	gameFlg_ = false;
	titleFlg_ = false;
}

// 更新
void GameOver::Update(void)
{
	// インスタンスの取得
	InputManager& ins = InputManager::GetInstance();

	// マウス座標の更新
	if (waitFrame_ > 0)
	{
		waitFrame_--;
		return;
	}

	GetMousePoint(&mousePos_.x, &mousePos_.y);

	if (ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN) || ins.IsTrgDown(KEY_INPUT_SPACE) ||
		ins.IsTrgMouseLeft())
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
	}
}

// 描画
void GameOver::Draw(void)
{
	// 透過素材の描画
	if (gameFlg_)
	{
		DrawGraph(600, 300, hightLight_, true);
	}

	if (titleFlg_)
	{
		DrawGraph(600, 330, hightLight_, true);
	}

	// 文字の描画
	DrawFormatString(20, 20, GetColor(0xff, 0xff, 0xff), "ゲームオーバー画面");
	DrawFormatString(300, 300, GetColor(0xff, 0xff, 0xff), "左クリックやスペース、Aボタンでタイトルに");
}

// 解放
void GameOver::Release(void)
{
	// 画像の解放
	DeleteGraph(hightLight_);
}