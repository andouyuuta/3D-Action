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
#include "../UI/Aicon.h"
#include "../Object/Trail3D.h"
#include "../Manager/AnimationManager.h"
#include "../Manager/BuffDataManager.h"
#include "../Application.h"
#include "../Manager/Camera.h"
#include "../UI/HpBar.h"
#include "../UI/Buff.h"
#include "Game.h"

// �R���X�g���N�^
Game::Game(void)
{
	selectindex_ = 0;
	menuSizeX_ = 0;
	menuSizeY_ = 0;
	menuimg_ = -1;
	centerY_ = 0;
	centerX_ = 0;
	isMenu_ = false;
	player_ = nullptr;
	sword_ = nullptr;
	enemymng_ = nullptr;
	stage_ = nullptr;
	camera_ = nullptr;
	collision_ = nullptr;
	aicon_ = nullptr;
	hpbar_ = nullptr;
	animation_ = nullptr;
	buff_ = nullptr;
}

// �f�X�g���N�^
Game::~Game(void)
{
	
}

// ������
void Game::Init(void)
{
	isMenu_ = false;
	menuimg_ = LoadGraph("Data/Image/Menu.png");
	GetGraphSize(menuimg_, &menuSizeX_, &menuSizeY_);

	centerX_ = Application::HALF_SCREEN_SIZE_X;
	centerY_ = Application::HALF_SCREEN_SIZE_Y;

	selectindex_ = 0;
	// �J����������
	camera_ = new Camera();
	enemymng_ = new EnemyManager();
	animation_ = new AnimationManager();
	player_ = new Player();
	sword_ = new Sword();
	aicon_ = new Aicon();
	collision_ = new Collision();
	stage_ = new Stage();
	hpbar_ = new HpBar();
	buff_ = new Buff();

	// ����������
	stage_->Init();
	camera_->Init(player_);
	animation_->Init(player_,enemymng_);
	enemymng_->Init(player_, animation_, animation_->GetEnemyInfo(),buff_);
	player_->Init(enemymng_, animation_, camera_);
	sword_->Init(player_);
	aicon_->Init(player_);
	collision_->Init(player_, sword_, stage_,animation_);
	hpbar_->Init(player_);
	buff_->Init(player_,enemymng_);

	// �t�H�O�ݒ�i�����F�j
	SetFogEnable(TRUE);
	SetFogColor(60,80,60); // ���邢��
	SetFogStartEnd(1000.0f, 3000.0f);
}

// �X�V
void Game::Update(void)
{
	InputManager& ins = InputManager::GetInstance();
	// TAB�L�[���p�b�h�̃��j���[�{�^���Ń��j���[���
	if (ins.IsTrgDown(KEY_INPUT_TAB) ||
		ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::MENU))
	{
		isMenu_ = !isMenu_;
	}

	// �A�j���[�V����
	animation_->Update();
	// ��
	sword_->Update();

	// --- �o�t�X�L���I�𒆂̏��� ---
	if (enemymng_->IsWaitingForSkillSelect())
	{
		SetMouseDispFlag(true);
		enemymng_->ShowSkillSelect();

		// �X�L���I�𒆂̓Q�[���{�҂̏������X�L�b�v
		return;
	}

	// ���j���[��ʂ��o�Ă��Ȃ��Ȃ�X�V
	if (!isMenu_)
	{
		SetMouseDispFlag(false);
		// �v���C���[�̃q�b�g�X�g�b�v���ɂ͍X�V�~�߂�
		player_->HitStop();
		if (!player_->GetIsHitStop()) {
			// �J����
			camera_->GameUpdate();
			//�G
			enemymng_->Update();
			// �v���C���[
			player_->Update(3);
			//�����蔻��
			collision_->Update(enemymng_->GetEnemyPtrs());

			//�G���S���|���ꂽ��Q�[���N���A�V�[���ֈړ�
			if (enemymng_->GetAliveEnemyCount() <= 0 && enemymng_->GetBossDead())
			{
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAMECLEAR);
			}

			// �V�[���J��
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
	else
	{
		InputManager::JOYPAD_IN_STATE pad = ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);
		// �}�E�X���x �� �Q�[���p�b�h�p�̊��x�ɒu������
		float padSensi = 0.03f;  // ���x�����i�K�v�ɉ����ă`���[�j���O�j

		// �Q�[���p�b�h�̃X�e�B�b�N���͂𐳋K��
		float lx = pad.AKeyLX / 1000.0f;  // ���E

		if ((ins.IsTrgDown(KEY_INPUT_LEFT) || lx < 0) && selectindex_ > 0)
		{
			selectindex_--;
		}
		if ((ins.IsTrgDown(KEY_INPUT_RIGHT) || lx > 0) && selectindex_ < OPTION_COUNT - 1) {
			selectindex_++;
			// �I�����Ȃǂ���΍Đ�
		}
		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
		{
			if (selectindex_ == 0)
			{
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
			}
			else
			{
				isMenu_ = false;
			}
		}
	}
}

// �`��
void Game::Draw(void)
{
	ClearDrawScreen();  // ���t���[����ԍŏ��ɁI

	// �J�����ݒ�
	camera_->SetBeforeDraw();

	//�X�e�[�W
	stage_->Draw();
	// �v���C���[
	player_->Draw();
	//��
	sword_->Draw();
	// �G
	enemymng_->Draw();
	// �A�C�R��
	aicon_->Draw();
	// HP�o�[
	hpbar_->Draw();

	// �f�o�b�O�p
	// �����蔻��
	collision_->Draw(enemymng_->GetEnemyPtrs());
	// �����̕`��
	//DrawFormatString(20, 20, GetColor(0xff, 0xff, 0xff), "�Q�[�����");
	DrawFormatString(20, Application::SCREEN_SIZE_Y-40, GetColor(0xff, 0xff, 0xff), "E�L�[�ŃQ�[���I�[�o�[");
	DrawFormatString(20, Application::SCREEN_SIZE_Y-20, GetColor(0xff, 0xff, 0xff), "C�L�[�ŃQ�[���N���A");

	// �J�����f�o�b�N�Ȃ�
	//camera_->Draw();

	if (isMenu_)
	{
		DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 0xffffff, true);

		DrawRectGraph(centerX_ - menuSizeX_ / 2,
			centerY_ - menuSizeY_ / 2,
			0, 0,
			menuSizeX_, menuSizeY_/2-50,
			menuimg_, true
		);

		if (selectindex_ == 0)
		{
			DrawRectGraph(0,
				menuSizeY_,
				0, menuSizeY_ / 2 - 50,
				menuSizeX_ / 2, menuSizeY_,
				menuimg_, true
			);
		}
		else
		{
			DrawRectGraph(centerX_,
				menuSizeY_,
				menuSizeX_ / 2, menuSizeY_ / 2 - 50,
				menuSizeX_, menuSizeY_,
				menuimg_, true
			);
		}
		SetDrawBright(255, 255, 255);
	}
	// --- �o�t�X�L���I�𒆂̏��� ---
	if (enemymng_->IsWaitingForSkillSelect())
	{
		buff_->Draw();
	}
}

// ���
void Game::Release(void)
{
	// �J�����̉��
	camera_->Release();
	delete camera_;
	// HP�o�[
	hpbar_->Release();
	delete hpbar_;
	// �A�C�R��
	aicon_->Release();
	delete aicon_;
	//���̉��
	sword_->Release();
	delete sword_;
	// �v���C���[�̉��
	player_->Release();
	delete player_;
	//�G�̉��
	enemymng_->Release();
	delete enemymng_;
	//�X�e�[�W�̉��
	stage_->Release();
	delete stage_;
	// �o�t
	buff_->Release();
	delete buff_;
}