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
	waitFrame_ = 0;
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

	waitFrame_ = 10;

	gameFlg_ = false;
	titleFlg_ = false;
}

// �X�V
void GameOver::Update(void)
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
	DrawFormatString(300, 300, GetColor(0xff, 0xff, 0xff), "���N���b�N��X�y�[�X�AA�{�^���Ń^�C�g����");
}

// ���
void GameOver::Release(void)
{
	// �摜�̉��
	DeleteGraph(hightLight_);
}