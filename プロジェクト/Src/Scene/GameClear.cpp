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
	mouseFlg_ = false;
}

// �f�X�g���N�^
GameClear::~GameClear(void)
{
}

// ������
void GameClear::Init(void)
{
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
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
	}
}

// �`��
void GameClear::Draw(void)
{
	// �����̕`��
	DrawFormatString(20, 20, GetColor(0xff, 0xff, 0xff), "�Q�[���N���A���");
	DrawFormatString(300, 300, GetColor(0xff, 0xff, 0xff), "���N���b�N��X�y�[�X�AA�{�^���Ń^�C�g����");

}

// ���
void GameClear::Release(void)
{
	// �摜�̉��

}