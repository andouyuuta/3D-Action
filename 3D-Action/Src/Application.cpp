#include <DxLib.h>
#include "Manager/InputManager.h"
#include "Manager/SceneManager.h"
#include "Application.h"

// �C���X�^���X�̐���
Application* Application::instance_ = nullptr;

// �f�[�^�p�X�֘A
const std::string Application::PATH_MODEL = "Data/Model/";
const std::string Application::PATH_IMAGE = "Data/Image/";
const std::string Application::PATH_SOUND = "Data/Sound/";

// �����I�ɃC���X�^���X�𐶐�����
void Application::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new Application();
	}
	instance_->Init();
}

// �ÓI�C���X�^���X�̎擾
Application& Application::GetInstance(void)
{
	return *instance_;
}

// ������
void Application::Init(void)
{
	// �A�v���P�[�V�����̏����ݒ�
	SetWindowText("School");

	// �E�B���h�E�T�C�Y
	SetGraphMode(SCREEN_SIZE_X, SCREEN_SIZE_Y, 32);
	ChangeWindowMode(true);

	// DxLib�̏�����
	SetUseDirect3DVersion(DX_DIRECT3D_11);
	isInitFail_ = false;
	if (DxLib_Init() == -1)
	{
		isInitFail_ = true;
		return;
	}

	SetDrawScreen(DX_SCREEN_BACK);

	// �L�[���䏉����
	SetUseDirectInputFlag(true);
	InputManager::CreateInstance();

	// �V�[���Ǘ�������
	SceneManager::CreateInstance();

	currentTime = 0;
	lastFrameTime = 0;
	frameCnt = 0;
	updateFrameRateTime = 0;
	frameRate = 0.f;
}

// �Q�[�����[�v�̊J�n
void Application::Run(void)
{
	auto& inputManager = InputManager::GetInstance();
	auto& sceneManager = SceneManager::GetInstance();

	// �Q�[�����[�v
	while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0)
	{
		Sleep(1);
		currentTime = GetNowCount();

		if (currentTime - lastFrameTime >= FRAME_RATE)
		{
			lastFrameTime = currentTime;
			frameCnt++;

			// ��ʂ�������
			ClearDrawScreen();

			inputManager.Update();
			sceneManager.Update();
			sceneManager.Draw();

			CalcFrameRate();
			DrawFrameRate();

			ScreenFlip();
		}
	}
}

// ���\�[�X�̉��
void Application::Release(void)
{
	// DxLib�I��
	if (DxLib_End() == -1)
	{
		isReleaseFail_ = true;
	}
}

// �����������^���s�̔���
bool Application::IsInitFail(void) const
{
	return isInitFail_;
}

// ��������^���s�̔���
bool Application::IsReleaseFail(void) const
{
	return isReleaseFail_;
}

// �f�t�H���g�R���X�g���N�^��private�ɂ��āA
// �O�����琶���ł��Ȃ��l�ɂ���
Application::Application(void)
{
	isInitFail_ = false;
	isReleaseFail_ = false;
}

// �f�X�g���N�^�����l
Application::~Application(void)
{
	delete instance_;
}

// �t���[�����[�g
void Application::CalcFrameRate()
{
	int nDifTime = currentTime - updateFrameRateTime;

	if (nDifTime > 1000)
	{
		float fFrameCnt = (float)(frameCnt * 1000);
		frameRate = fFrameCnt / nDifTime;
		frameCnt = 0;
		updateFrameRateTime = currentTime;
	}
}

// �t���[�����[�g
void Application::DrawFrameRate()
{
	DrawFormatString(SCREEN_SIZE_X - 90, 0, GetColor(255, 30, 30), "FPS[%.2f]", frameRate);
}
