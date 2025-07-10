#include <string>
#include <DxLib.h>
#include "../Application.h"
#include "../Utility/Utility.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "../Manager/SoundManager.h"
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

	// �V���O���g���̌Ăяo��
	SoundManager* seMana_ = SoundManager::GetInstance();

	//BGM�̃��[�h
	seMana_->LoadBGM("TitleBGM", (Application::PATH_SOUND + "BGM/Title.mp3").c_str(), false);
}

// �X�V
void Title::Update(void)
{
	// �C���X�^���X�̎擾
	InputManager& ins = InputManager::GetInstance();

	// �V���O���g���̌Ăяo��
	SoundManager* seMana_ = SoundManager::GetInstance();

	//BGM�̍Đ�
	seMana_->SetVolumeSE("TitleBGM", 200);
	seMana_->PlayBGM("TitleBGM");

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
		// BGM�̒�~
		seMana_->StopBGM("TitleBGM");
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
	DrawFormatString(300, 300, GetColor(0xff, 0xff, 0xff), "���N���b�N��X�y�[�X�AA�{�^���ŃX�^�[�g");
}

// ���
void Title::Release(void)
{
	// �V���O���g���̌Ăяo��
	SoundManager* seMana_ = SoundManager::GetInstance();
	seMana_->StopBGM("TitleBGM");
	seMana_->ReleaseSound("TitleBGM");

	DeleteGraph(titleImg_);
	titleImg_ = -1;
}