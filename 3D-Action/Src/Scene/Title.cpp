#include <string>
#include <DxLib.h>
#include "../Application.h"
#include "../Utility/Utility.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "Title.h"


// �R���X�g���N�^
Title::Title(void) : SceneBase()
{

}

// �f�X�g���N�^
Title::~Title(void)
{
}

// ������
void Title::Init(void)
{
	// �t���O�̏�����
	mouseFlg_ = false;

	// �}�E�X���W�̏�����
	mousePos_ = { 0, 0 };
}

// �X�V
void Title::Update(void)
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
			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
		}
	}
	
}

// �`��
void Title::Draw(void)
{
	// �^�C�g���摜�̕`��


	// ���ߑf�ނ̕`��
	
	if (mouseFlg_)
	{
		DrawBox(500, 500, 600, 600, GetColor(128, 128, 128), true);
	}
	else
	{
		DrawBox(500, 500, 600, 600, GetColor(255, 255, 255), true);
	}
	

	// �����̕`��
	DrawFormatString(20, 20, GetColor(0xff, 0xff, 0xff), "�^�C�g�����");
	DrawFormatString(300, 300, GetColor(0xff, 0xff, 0xff), "���N���b�N�ŃX�^�[�g");
}

// ���
void Title::Release(void)
{
	// �摜�̉��

}