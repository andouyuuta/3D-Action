#include <cmath>
#include <DxLib.h>
#include "../Application.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "GameOver.h"


// �R���X�g���N�^
GameOver::GameOver(void) : SceneBase()
{

}

// �f�X�g���N�^
GameOver::~GameOver(void)
{
}

// ������
void GameOver::Init(void)
{
	// �摜�̓ǂݍ���
	hightLight_ = LoadGraph("Data/Image/hightLight.png");

	gameFlg_ = false;
	titleFlg_ = false;
}

// �X�V
void GameOver::Update(void)
{
	// �C���X�^���X�̎擾
	InputManager& ins = InputManager::GetInstance();

	// �}�E�X���W�̍X�V
	mousePos_ = ins.GetMousePos();

	if (mousePos_.x >= 600 && mousePos_.x <= 760 && mousePos_.y >= 300 && mousePos_.y <= 316)
	{
		gameFlg_ = true;
		if (ins.IsTrgMouseLeft()) 
		{
			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
		}
	}
	else
	{
		gameFlg_ = false;
	}
	
	if (mousePos_.x >= 600 && mousePos_.x <= 760 && mousePos_.y >= 330 && mousePos_.y <= 346)
	{
		titleFlg_ = true;
		if (ins.IsTrgMouseLeft())
		{
			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
		}
	}
	else
	{
		titleFlg_ = false;
	}
}

// �`��
void GameOver::Draw(void)
{
	// ���ߑf�ނ̕`��
	if (gameFlg_)
	{
		DrawGraph(600, 300, hightLight_, true);
	}

	if (titleFlg_)
	{
		DrawGraph(600, 330, hightLight_, true);
	}

	// �����̕`��
	DrawFormatString(20, 20, GetColor(0xff, 0xff, 0xff), "�Q�[���I�[�o�[���");
	DrawFormatString(300, 300, GetColor(0xff, 0xff, 0xff), "���N���b�N�Ō���");

	DrawFormatString(600, 300, GetColor(0xff, 0x00, 0x00), "���g���C");
	DrawFormatString(600, 330, GetColor(0xff, 0x00, 0x00), "�^�C�g���ɖ߂�");
}

// ���
void GameOver::Release(void)
{
	// �摜�̉��
	DeleteGraph(hightLight_);
}