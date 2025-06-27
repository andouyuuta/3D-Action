#include <string>
#include <DxLib.h>
#include "../Application.h"
#include "../Utility/Utility.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "Title.h"


// コンストラクタ
Title::Title(void) : SceneBase()
{
	titleImg_ = -1;
	startImg_ = -1;
	camera_ = nullptr;
	waitFrame_ = 0;
}

// デストラクタ
Title::~Title(void)
{
}

// 初期化
void Title::Init(void)
{
	titleImg_ = LoadGraph("Data/Image/title.png");
	startImg_ = LoadGraph("Data/Image/Start.png");

	waitFrame_ = 10;

	// マウス座標の初期化
	mousePos_ = { 0, 0 };
}

// 更新
void Title::Update(void)
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

	if(ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)||ins.IsTrgDown(KEY_INPUT_SPACE)||
		ins.IsTrgMouseLeft())
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
	}
}

// 描画
void Title::Draw(void)
{
	// タイトル画像の描画
	DrawGraph(0, 0, titleImg_, true);
	DrawExtendGraph(50, Application::HALF_SCREEN_SIZE_Y + 100, Application::HALF_SCREEN_SIZE_X, Application::SCREEN_SIZE_Y -100, startImg_, true);
	
	// 文字の描画
	DrawFormatString(20, 20, GetColor(0xff, 0xff, 0xff), "タイトル画面");
	DrawFormatString(300, 300, GetColor(0xff, 0xff, 0xff), "左クリックやスペース、Aボタンでスタート");
}

// 解放
void Title::Release(void)
{
	DeleteGraph(titleImg_);
	titleImg_ = -1;
}