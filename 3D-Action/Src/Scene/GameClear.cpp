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

}

// �f�X�g���N�^
GameClear::~GameClear(void)
{
}

// ������
void GameClear::Init(void)
{
	// �摜�̓ǂݍ���

}

// �X�V
void GameClear::Update(void)
{
	// �C���X�^���X�̎擾
	InputManager& ins = InputManager::GetInstance();

	// �}�E�X���W�̍X�V
	mousePos_ = ins.GetMousePos();

	if (mousePos_.x >= 500 && mousePos_.x <= 600 && mousePos_.y >= 500 && mousePos_.y <= 600)
	{
		mouseFlg_ = true;
	}
	else
	{
		mouseFlg_ = false;
	}

	// �V�[���J��
	if (mouseFlg_)
	{
		if (ins.IsTrgMouseLeft())
		{
			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
		}
	}
}

// �`��
void GameClear::Draw(void)
{
	// �^�C�g���摜�̕`��


	// ���ߑf�ނ̕`��


	// �����̕`��
	DrawFormatString(20, 20, GetColor(0xff, 0xff, 0xff), "�Q�[���N���A���");
	DrawFormatString(300, 300, GetColor(0xff, 0xff, 0xff), "���N���b�N�ŃX�^�[�g");

	if (mouseFlg_)
	{
		DrawBox(500, 500, 600, 600, GetColor(128, 128, 128), true);
	}
	else
	{
		DrawBox(500, 500, 600, 600, GetColor(255, 255, 255), true);
	}
}

// ���
void GameClear::Release(void)
{
	// �摜�̉��

}