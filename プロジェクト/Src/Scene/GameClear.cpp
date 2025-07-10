#include <cmath>
#include <DxLib.h>
#include "../Application.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "GameClear.h"


// �R���X�g���N�^
GameClear::GameClear(void) : SceneBase()
{
	clearImg_ = -1;
	mouseFlg_ = false;
	waitFrame_ = 0;
}

// �f�X�g���N�^
GameClear::~GameClear(void)
{
}

// ������
void GameClear::Init(void)
{
	clearImg_ = LoadGraph("Data/Image/Gameclear.png");

	waitFrame_ = 10;
}

// �X�V
void GameClear::Update(void)
{
	// �C���X�^���X�̎擾
	InputManager& ins = InputManager::GetInstance();

	// �}�E�X���W�̍X�V
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

// �`��
void GameClear::Draw(void)
{
	// �^�C�g���摜�̕`��
	DrawExtendGraph(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, clearImg_, true);

	// �����̕`��
	DrawFormatString(600, 500, GetColor(0xff, 0xff, 0xff), "���N���b�N��X�y�[�X�AA�{�^���Ń^�C�g����");

}

// ���
void GameClear::Release(void)
{
	// �摜�̉��
	DeleteGraph(clearImg_);
	clearImg_ = -1;
}