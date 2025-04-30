#include <DxLib.h>
#include "../Application.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "../Object/Grid.h"
#include "../Object/Player.h"
#include "../Object/Sword.h"
#include "../Object/Enemy.h"
#include "../Object/EnemyManager.h"
#include "../Application.h"
#include "../Manager/Camera.h"
#include "Game.h"

// �R���X�g���N�^
Game::Game(void)
{
	grid_ = nullptr;
	player_ = nullptr;
	sword_ = nullptr;
	enemy_ = nullptr;
	enemymng_ = nullptr;
	camera_ = nullptr;
}

// �f�X�g���N�^
Game::~Game(void)
{
	
}

// ������
void Game::Init(void)
{
	// �O���b�h������
	grid_ = new Grid();
	grid_->SystemInit();

	// �J����������
	camera_ = new Camera();
	camera_->Init();

	// �v���C���[������
	player_ = new Player();
	player_->SystemInit();

	//��������
	sword_ = new Sword();
	sword_->Init();

	enemymng_ = new EnemyManager();
	enemymng_->Init();

	// �O���b�h������
	grid_->GameInit();

	// �v���C���[������
	player_->GameInit();

	// �J�����̒Ǐ]�Ώۂ�ݒ�
	SceneManager& sceneManager = SceneManager::GetInstance();
	Camera* camera = sceneManager.GetCamera();
	camera->SetPlayer(player_);

}

// �X�V
void Game::Update(void)
{
	// �O���b�h�X�V
	grid_->Update();

	// �J�����X�V
	camera_->Update();

	// �v���C���[�X�V
	player_->Update();

	//���X�V
	sword_->Update();

	enemymng_->Update();

	// �V�[���J��
	InputManager& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_C))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAMECLEAR);
	}
	if (ins.IsTrgDown(KEY_INPUT_E))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAMEOVER);
	}
}

// �`��
void Game::Draw(void)
{
	// �O���b�h�̕`��
	grid_->Draw();

	// �v���C���[�`��
	player_->Draw();

	//���`��
	sword_->Draw();

	//enemy_->Draw();
	enemymng_->Draw();

	// �J�����ݒ�
	camera_->SetBeforeDraw();

	// �����̕`��
	DrawFormatString(20, 20, GetColor(0xff, 0xff, 0xff), "�Q�[�����");
	DrawFormatString(20, 80, GetColor(0xff, 0xff, 0xff), "E�L�[�ŃQ�[���I�[�o�[");
	DrawFormatString(20, 100, GetColor(0xff, 0xff, 0xff), "C�L�[�ŃQ�[���N���A");

	// �J�����f�o�b�N�Ȃ�
	camera_->Draw();
}

// ���
void Game::Release(void)
{
	// �O���b�h�̉��
	grid_->Release();
	delete grid_;

	// �J�����̉��
	camera_->Release();
	delete camera_;

	//���̉��
	sword_->Release();
	delete sword_;

	// �v���C���[�̉��
	player_->Release();
	delete player_;

	enemymng_->Release();
	delete enemymng_;
}
