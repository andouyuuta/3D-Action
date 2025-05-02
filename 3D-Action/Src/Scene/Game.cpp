#include <DxLib.h>
#include "../Application.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "../Object/Grid.h"
#include "../Object/Player.h"
#include "../Object/Sword.h"
#include "../Object/EnemyManager.h"
#include "../Object/Stage.h"
#include "../Object/Collision.h"
#include "../Application.h"
#include "../Manager/Camera.h"
#include "Game.h"

// �R���X�g���N�^
Game::Game(void)
{
	grid_ = nullptr;
	player_ = nullptr;
	sword_ = nullptr;
	enemymng_ = nullptr;
	stage_ = nullptr;
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

	Stage::CreateInstance();
	Stage::GetInstance().Init();

	// �v���C���[������
	Player::CreateInstance();
	Player::GetInstance().Init();

	//��������
	sword_ = new Sword();
	sword_->Init();

	enemymng_ = new EnemyManager();
	enemymng_->Init();

	collision_ = new Collision();
	collision_->Init();

	// �O���b�h������
	grid_->GameInit();

	// �J�����̒Ǐ]�Ώۂ�ݒ�
	SceneManager& sceneManager = SceneManager::GetInstance();
	Camera* camera = sceneManager.GetCamera();
	camera->SetPlayer(player_);

	// �t�H�O�ݒ�i�����F�j
	SetFogEnable(TRUE);
	SetFogColor(60,80,60); // ���邢��
	SetFogStartEnd(1000.0f, 3000.0f);
}

// �X�V
void Game::Update(void)
{
	// �O���b�h�X�V
	grid_->Update();

	// �J�����X�V
	camera_->Update();

	//�G�X�V
	enemymng_->Update();

	// �v���C���[�X�V
	Player::GetInstance().Update(enemymng_->GetEnemies());

	//���X�V
	sword_->Update();

	//�����蔻��
	collision_->Update(enemymng_->GetEnemies());

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
	//�X�e�[�W�̕`��
	Stage::GetInstance().Draw();

	collision_->Draw();

	// �O���b�h�̕`��
	//grid_->Draw();

	// �v���C���[�`��
	Player::GetInstance().Draw();
	//���`��
	sword_->Draw();

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
	Player::GetInstance().Release();

	//�G�̉��
	enemymng_->Release();
	delete enemymng_;

	//�X�e�[�W�̉��
	Stage::GetInstance().Release();
}
