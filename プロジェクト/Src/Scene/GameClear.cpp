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
	mouseFlg_ = false;
}

// デストラクタ
GameClear::~GameClear(void)
{
}

// 初期化
void GameClear::Init(void)
{
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
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
	}
}

// 描画
void GameClear::Draw(void)
{
	// 文字の描画
	DrawFormatString(20, 20, GetColor(0xff, 0xff, 0xff), "ゲームクリア画面");
	DrawFormatString(300, 300, GetColor(0xff, 0xff, 0xff), "左クリックやスペース、Aボタンでタイトルに");

}

// 解放
void GameClear::Release(void)
{
	// 画像の解放

}