#include <cmath>
#include <DxLib.h>
#include "../Application.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "GameClear.h"


// コンストラクタ
GameClear::GameClear(void) : SceneBase()
{
	clearImg_ = -1;
	mouseFlg_ = false;
	waitFrame_ = 0;
}

// デストラクタ
GameClear::~GameClear(void)
{
}

// 初期化
void GameClear::Init(void)
{
	clearImg_ = LoadGraph("Data/Image/Gameclear.png");

	waitFrame_ = 10;
}

// 更新
void GameClear::Update(void)
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
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
	}
}

// 描画
void GameClear::Draw(void)
{
	// タイトル画像の描画
	DrawExtendGraph(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, clearImg_, true);

	// 文字の描画
	DrawFormatString(600, 500, GetColor(0xff, 0xff, 0xff), "左クリックやスペース、Aボタンでタイトルに");

}

// 解放
void GameClear::Release(void)
{
	// 画像の解放
	DeleteGraph(clearImg_);
	clearImg_ = -1;
}