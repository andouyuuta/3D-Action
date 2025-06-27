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
	titleImg_ = -1;
	startImg_ = -1;
	camera_ = nullptr;
	waitFrame_ = 0;
}

// �f�X�g���N�^
Title::~Title(void)
{
}

// ������
void Title::Init(void)
{
	titleImg_ = LoadGraph("Data/Image/title.png");
	startImg_ = LoadGraph("Data/Image/Start.png");

	waitFrame_ = 10;

	// �}�E�X���W�̏�����
	mousePos_ = { 0, 0 };
}

// �X�V
void Title::Update(void)
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

	if(ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)||ins.IsTrgDown(KEY_INPUT_SPACE)||
		ins.IsTrgMouseLeft())
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
	}
}

// �`��
void Title::Draw(void)
{
	// �^�C�g���摜�̕`��
	DrawGraph(0, 0, titleImg_, true);
	DrawExtendGraph(50, Application::HALF_SCREEN_SIZE_Y + 100, Application::HALF_SCREEN_SIZE_X, Application::SCREEN_SIZE_Y -100, startImg_, true);
	
	// �����̕`��
	DrawFormatString(20, 20, GetColor(0xff, 0xff, 0xff), "�^�C�g�����");
	DrawFormatString(300, 300, GetColor(0xff, 0xff, 0xff), "���N���b�N��X�y�[�X�AA�{�^���ŃX�^�[�g");
}

// ���
void Title::Release(void)
{
	DeleteGraph(titleImg_);
	titleImg_ = -1;
}