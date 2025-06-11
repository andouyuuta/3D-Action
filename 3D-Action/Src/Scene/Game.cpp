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
#include "../Object/Aicon.h"
#include "../Object/Trail3D.h"
#include "../Application.h"
#include "../Manager/Camera.h"
#include "Game.h"

// �R���X�g���N�^
Game::Game(void)
{
	player_ = nullptr;
	sword_ = nullptr;
	enemymng_ = nullptr;
	stage_ = nullptr;
	camera_ = nullptr;
	collision_ = nullptr;
	aicon_ = nullptr;
}

// �f�X�g���N�^
Game::~Game(void)
{
	
}

// ������
void Game::Init(void)
{
	// �J����������
	camera_ = new Camera();
	camera_->Init();

	Stage::CreateInstance();
	Stage::GetInstance().Init();

	enemymng_ = new EnemyManager();
	enemymng_->Init();

	// �v���C���[������
	Player::CreateInstance();
	Player::GetInstance().Init(enemymng_);

	//��������
	Sword::CreateInstance();
	Sword::GetInstance().Init();


	aicon_ = new Aicon();
	aicon_->Init();

	collision_ = new Collision();
	collision_->Init();

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
	Player::GetInstance().HitStop();
	if (!Player::GetInstance().GetIsHitStop()) {
		// �J�����X�V
		camera_->Update();

		//�G�X�V
		enemymng_->Update();

		// �v���C���[�X�V
		Player::GetInstance().Update(3);

		//���X�V
		Sword::GetInstance().Update();

		//�����蔻��
		collision_->Update(enemymng_->GetEnemyPtrs());

		//�G���S���|���ꂽ��Q�[���N���A�V�[���ֈړ�
		if (enemymng_->GetAliveEnemyCount() <= 0 && enemymng_->GetBossDead())
		{
			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAMECLEAR);
		}

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
}

// �`��
void Game::Draw(void)
{
	ClearDrawScreen();  // ���t���[����ԍŏ��ɁI

	//�X�e�[�W�̕`��
	Stage::GetInstance().Draw();

	// �v���C���[�`��
	Player::GetInstance().Draw();

	//���`��
	Sword::GetInstance().Draw();

	enemymng_->Draw();

	aicon_->Draw();

	// �J�����ݒ�
	camera_->SetBeforeDraw();

	collision_->Draw(enemymng_->GetEnemyPtrs());

	// �����̕`��
	//DrawFormatString(20, 20, GetColor(0xff, 0xff, 0xff), "�Q�[�����");
	DrawFormatString(20, 80, GetColor(0xff, 0xff, 0xff), "E�L�[�ŃQ�[���I�[�o�[");
	DrawFormatString(20, 100, GetColor(0xff, 0xff, 0xff), "C�L�[�ŃQ�[���N���A");

	// �J�����f�o�b�N�Ȃ�
	camera_->Draw();
}

// ���
void Game::Release(void)
{
	// �J�����̉��
	camera_->Release();
	delete camera_;

	aicon_->Release();
	delete aicon_;

	//���̉��
	Sword::GetInstance().Release();

	// �v���C���[�̉��
	Player::GetInstance().Release();

	//�G�̉��
	enemymng_->Release();
	delete enemymng_;

	//�X�e�[�W�̉��
	Stage::GetInstance().Release();
}
