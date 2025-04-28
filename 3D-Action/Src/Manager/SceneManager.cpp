#include <chrono>
#include <DxLib.h>
#include "../Common/Fader.h"
#include "../Scene/Title.h"
#include "../Scene/Game.h"
#include "../Scene/GameOver.h"
#include "../Scene/GameClear.h"
#include "SceneManager.h"
#include "Camera.h"


SceneManager* SceneManager::instance_ = nullptr;

void SceneManager::CreateInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new SceneManager();
	}
	instance_->Init();
}

SceneManager& SceneManager::GetInstance(void)
{
	return *instance_;
}

void SceneManager::Init(void)
{

	sceneId_ = SCENE_ID::TITLE;
	waitSceneId_ = SCENE_ID::NONE;

	fader_ = new Fader();
	fader_->Init();

	//�J����������
	camera_ = new Camera();
	camera_->Init();

	isSceneChanging_ = false;

	// �f���^�^�C��
	preTime_ = std::chrono::system_clock::now();

	// �����V�[���̐ݒ�
	DoChangeScene(SCENE_ID::TITLE);

	Init3D();

}

void SceneManager::Init3D(void)
{
	//�w�i�F�ݒ�
	SetBackgroundColor(105, 20, 100);

	// Z�o�b�t�@��L���ɂ���
	SetUseZBuffer3D(true);

	// Z�o�b�t�@�ւ̏������݂�L���ɂ���
	SetWriteZBuffer3D(true);

	//�o�b�N�J�����O��L���ɂ���
	SetUseBackCulling(true);

	//���C�g��L���ɂ���
	SetUseLighting(true);

	//���ʂ���΂߂ɉ��Ɍ����������C�g
	ChangeLightTypeDir({ 0.00f,-1.00f,1.00f });

	// �ł��邾�����₩�Ɍ��点��
	SetUseSpecular(false);  // �n�C���C�g�����S�I�t

	//SetLightAmbColor(GetColorF(0.5f, 0.5f, 0.5f, 1.0f)); // ����
	//SetLightDifColor(GetColorF(0.3f, 0.3f, 0.3f, 1.0f)); // �g�U��
	SetLightSpcColor(GetColorF(0.0f, 0.0f, 0.0f, 0.5f)); // �X�y�L���������[����
}

void SceneManager::Update(void)
{

	if (scene_ == nullptr)
	{
		return;
	}

	// �f���^�^�C��
	auto nowTime = std::chrono::system_clock::now();
	deltaTime_ = static_cast<float>(
		std::chrono::duration_cast<std::chrono::nanoseconds>(nowTime - preTime_).count() / 1000000000.0);
	preTime_ = nowTime;

	fader_->Update();
	if (isSceneChanging_)
	{
		Fade();
	}
	else
	{
		scene_->Update();
	}

	if (sceneId_ == SCENE_ID::GAME) 
	{
		SetMouseDispFlag(false);
		camera_->GameUpdate();
	}
	else
	{
		SetMouseDispFlag(true);
		camera_->Update();
	}
}

void SceneManager::Draw(void)
{

	// �`���O���t�B�b�N�̈�̎w��
	// (�R�c�`��Ŏg�p����J�����̐ݒ�Ȃǂ����Z�b�g�����)
	SetDrawScreen(DX_SCREEN_BACK);

	// ��ʂ�������
	ClearDrawScreen();

	//�J�����ݒ�
	camera_->SetBeforeDraw();

	// �`��
	scene_->Draw();

	//�J�����f�o�b�N��
	camera_->Draw();

	// �Ó]�E���]
	fader_->Draw();

}

void SceneManager::Release(void)
{

	scene_->Release();
	delete scene_;

	delete fader_;

}

void SceneManager::ChangeScene(SCENE_ID nextId)
{

	// �t�F�[�h�������I����Ă���V�[����ς���ꍇ�����邽�߁A
	// �J�ڐ�V�[���������o�ϐ��ɕێ�
	waitSceneId_ = nextId;

	// �t�F�[�h�A�E�g(�Ó])���J�n����
	fader_->SetFade(Fader::STATE::FADE_OUT);
	isSceneChanging_ = true;

}

SceneManager::SCENE_ID SceneManager::GetSceneID(void)
{
	return sceneId_;
}

float SceneManager::GetDeltaTime(void) const
{
	//return 1.0f / 60.0f;
	return deltaTime_;
}

Camera* SceneManager::GetCamera(void) const
{
	return camera_;
}

SceneManager::SceneManager(void)
{

	sceneId_ = SCENE_ID::NONE;
	waitSceneId_ = SCENE_ID::NONE;

	scene_ = nullptr;
	fader_ = nullptr;

	isSceneChanging_ = false;

	// �f���^�^�C��
	deltaTime_ = 1.0f / 60.0f;

}

SceneManager::~SceneManager(void)
{
	delete instance_;
}

void SceneManager::ResetDeltaTime(void)
{
	deltaTime_ = 0.016f;
	preTime_ = std::chrono::system_clock::now();
}

void SceneManager::DoChangeScene(SCENE_ID sceneId)
{

	// �V�[����ύX����
	sceneId_ = sceneId;

	// ���݂̃V�[�������
	if (scene_ != nullptr)
	{
		scene_->Release();
		delete scene_;
	}

	switch (sceneId_)
	{
	case SCENE_ID::TITLE:
		scene_ = new Title();
		break;
	case SCENE_ID::GAME:
		scene_ = new Game();
		camera_->ReSet();
		break;
	case SCENE_ID::GAMEOVER:
		scene_ = new GameOver();
		camera_->ReSet();
		break;
	case SCENE_ID::GAMECLEAR:
		scene_ = new GameClear();
		camera_->ReSet();
		break;
	}

	scene_->Init();

	ResetDeltaTime();

	waitSceneId_ = SCENE_ID::NONE;

}

void SceneManager::Fade(void)
{

	Fader::STATE fState = fader_->GetState();
	switch (fState)
	{
	case Fader::STATE::FADE_IN:
		// ���]��
		if (fader_->IsEnd())
		{
			// ���]���I��������A�t�F�[�h�����I��
			fader_->SetFade(Fader::STATE::NONE);
			isSceneChanging_ = false;
		}
		break;
	case Fader::STATE::FADE_OUT:
		// �Ó]��
		if (fader_->IsEnd())
		{
			// ���S�ɈÓ]���Ă���V�[���J��
			DoChangeScene(waitSceneId_);
			// �Ó]���疾�]��
			fader_->SetFade(Fader::STATE::FADE_IN);
		}
		break;
	}

}


