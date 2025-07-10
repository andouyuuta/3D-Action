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
#include "../Object/BossEnemy.h"
#include "../Manager/AnimationManager.h"
#include "../Manager/BuffDataManager.h"
#include "../Manager/MoviePlayer.h"
#include "../Manager/SoundManager.h"
#include "../Application.h"
#include "../Manager/Camera.h"
#include "../UI/HpBar_.h"
#include "../UI/Buff.h"
#include "../Object/EnemyManager.h"
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
	isBossBGMStated = false;

	isMenu_ = false;
	player_ = nullptr;
	sword_ = nullptr;
	enemymng_ = nullptr;
	stage_ = nullptr;
	camera_ = nullptr;
	collision_ = nullptr;
	aicon_ = nullptr;
	hpBar_ = nullptr;
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
	isBossBGMStated = false;
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
	hpBar_ = new HpBar();
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
	hpBar_->Init(player_, enemymng_);
	buff_->Init(player_,enemymng_);

	// �t�H�O�ݒ�i�����F�j
	SetFogEnable(TRUE);
	SetFogColor(60,80,60); // ���邢��
	SetFogStartEnd(1000.0f, 3000.0f);

	// �V���O���g���̌Ăяo��
	SoundManager* seMana_ = SoundManager::GetInstance();

	// BGM�̃��[�h
	seMana_->LoadBGM("GameBGM", (Application::PATH_SOUND + "BGM/Shadow_Moon.mp3").c_str(), false);
	// �{�X�p
	seMana_->LoadBGM("BossBGM", (Application::PATH_SOUND + "BGM/Boss.mp3").c_str(), false);
}

// �X�V
void Game::Update(void)
{
	// �C���X�^���X�̎擾
	InputManager& ins = InputManager::GetInstance();

	// �V���O���g���̌Ăяo��
	SoundManager* seMana_ = SoundManager::GetInstance();

	// BGM�̍Đ�
	seMana_->SetVolumeSE("GameBGM", 200);
	// �{�X�p
	seMana_->SetVolumeSE("BossBGM", 200);

	// �{�X���ǂ���
	if (!enemymng_->GetIsBoss())
	{
		seMana_->PlayBGM("GameBGM");
	}

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
			if (player_->GetIsUltPlay())
			{
				player_->MovieUpdate();
				return;
			}
			// �v���C���[
			player_->Update(3);
			// �J����
			camera_->GameUpdate();
			//�G
			enemymng_->Update();
			//�����蔻��
			collision_->Update(enemymng_->GetEnemyPtrs());

			if (enemymng_->GetIsBoss() == true&& !isBossBGMStated)
			{
				seMana_->StopBGM("GameBGM");
				seMana_->PlayBGM("BossBGM");
				isBossBGMStated = true;
			}

			//�G���S���|���ꂽ��Q�[���N���A�V�[���ֈړ�
			if (enemymng_->GetAliveEnemyCount() <= 0 && enemymng_->GetBossDead())
			{
				seMana_->StopBGM("GameBGM");
				seMana_->StopBGM("BossBGM");
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAMECLEAR);
			}

			// �V�[���J��
			if (ins.IsTrgDown(KEY_INPUT_C))
			{
				seMana_->StopBGM("GameBGM");
				seMana_->StopBGM("BossBGM");
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAMECLEAR);
			}
			if (ins.IsTrgDown(KEY_INPUT_E))
			{
				seMana_->StopBGM("GameBGM");
				seMana_->StopBGM("BossBGM");
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAMEOVER);
			}
		}
	}
	else
	{
		SetMouseDispFlag(true);
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
				seMana_->StopBGM("GameBGM");
				seMana_->StopBGM("BossBGM");
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
			}
			else
			{
				isMenu_ = false;
			}
			seMana_->StopBGM("GameBGM");
			seMana_->StopBGM("BossBGM");
		}
		
		// �}�E�X�ʒu�擾
		int mouseX, mouseY;
		GetMousePoint(&mouseX, &mouseY);

		// YES��NO�̃{�^���̈�i���摜�T�C�Y�ƈʒu�ɉ����Ē����j
		int yesX = centerX_ - menuSizeX_ / 2;
		int yesY = centerY_;
		int noX = centerX_;
		int noY = centerY_;
		int btnWidth = menuSizeX_ / 2;
		int btnHeight = menuSizeY_ / 2 + 50;

		// YES���z�o�[����Ă�����
		if (mouseX >= yesX && mouseX <= yesX + btnWidth &&
			mouseY >= yesY && mouseY <= yesY + btnHeight)
		{
			selectindex_ = 0;

			if (ins.IsTrgMouseLeft())
			{
				// YES���N���b�N���ꂽ
				seMana_->StopBGM("GameBGM");
				seMana_->StopBGM("BossBGM");
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
			}
		}
		// NO���z�o�[����Ă�����
		else if (mouseX >= noX && mouseX <= noX + btnWidth &&
			mouseY >= noY && mouseY <= noY + btnHeight)
		{
			selectindex_ = 1;

			if (ins.IsTrgMouseLeft())
			{
				// NO���N���b�N���ꂽ
				isMenu_ = false;
				seMana_->StopBGM("GameBGM");
				seMana_->StopBGM("BossBGM");
			}
		}
	}
}

// �`��
void Game::Draw(void)
{
	ClearDrawScreen();  // ���t���[����ԍŏ��ɁI

	if (isMenu_)
	{
		DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 0xffffff, true);

		DrawRectGraph(centerX_ - menuSizeX_ / 2,
			centerY_ - menuSizeY_ / 2,
			0, 0,
			menuSizeX_, menuSizeY_ / 2 - 50,
			menuimg_, true
		);

		// �������F[YES] [NO] �̉摜�\���i�I�𒆂����g��j
		int optionWidth = menuSizeX_ / 2;
		int optionHeight = menuSizeY_ / 2 + 50;

		// YES�i���j�`��ʒu
		int yesX = centerX_ - menuSizeX_ / 2;
		int yesY = centerY_;

		// NO�i�E�j�`��ʒu
		int noX = centerX_;
		int noY = centerY_;

		int btnWidth = menuSizeX_ / 2;
		int btnHeight = menuSizeY_ / 2 + 50;

		if (selectindex_ == 0)
		{
			// YES���g��
			DrawRectExtendGraph(
				yesX - (optionWidth * (scale - 1.0f) / 2),
				yesY - (optionHeight * (scale - 1.0f) / 2),
				yesX + optionWidth + (optionWidth * (scale - 1.0f) / 2),
				yesY + optionHeight + (optionHeight * (scale - 1.0f) / 2),
				0, menuSizeY_ / 2 - 50,
				optionWidth, optionHeight,
				menuimg_, true
			);

			// NO��ʏ�T�C�Y
			DrawRectGraph(
				noX,
				noY,
				optionWidth, menuSizeY_ / 2 - 50,
				optionWidth, optionHeight,
				menuimg_, true
			);
		}
		else
		{
			// YES��ʏ�T�C�Y
			DrawRectGraph(
				yesX,
				yesY,
				0, menuSizeY_ / 2 - 50,
				optionWidth, optionHeight,
				menuimg_, true
			);

			// NO���g��
			DrawRectExtendGraph(
				noX - (optionWidth * (scale - 1.0f) / 2),
				noY - (optionHeight * (scale - 1.0f) / 2),
				noX + optionWidth + (optionWidth * (scale - 1.0f) / 2),
				noY + optionHeight + (optionHeight * (scale - 1.0f) / 2),
				optionWidth, menuSizeY_ / 2 - 50,
				optionWidth, optionHeight,
				menuimg_, true
			);
		}		SetDrawBright(255, 255, 255);

		DrawBox(noX, noY, noX + btnWidth, noY + btnHeight, 0x00ff00, false); // �Θg�ŋ���
		DrawBox(yesX, yesY, yesX + btnWidth, yesY + btnHeight, 0x00ff00, false); // �Θg�ŋ���

		DrawFormatString(0, 0, 0x000000, "�U���́F%d", player_->GetAttackPower());
		DrawFormatString(0, 20, 0x000000, "�̗́F%d / %d", player_->GetHp(), player_->GetMaxHp());
		DrawFormatString(0, 40, 0x000000, "�G�U���́F%d", enemymng_->GetTotalAttack());
		DrawFormatString(0, 60, 0x000000, "�G�̗́F%d", enemymng_->GetTotalHp());
		return;
	}

	if (player_->GetIsUltPlay())
	{
		player_->MovieDraw();
		return;
	}

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
	hpBar_->Draw();

	// �f�o�b�O�p
	// �����蔻��
	collision_->Draw(enemymng_->GetEnemyPtrs());

	// �����̕`��
	DrawFormatString(20, Application::SCREEN_SIZE_Y - 40, GetColor(0xff, 0xff, 0xff), "E�L�[�ŃQ�[���I�[�o�[");
	DrawFormatString(20, Application::SCREEN_SIZE_Y - 20, GetColor(0xff, 0xff, 0xff), "C�L�[�ŃQ�[���N���A");

	// --- �o�t�X�L���I�𒆂̏��� ---
	if (enemymng_->IsWaitingForSkillSelect())
	{
		buff_->Draw();
		return;
	}
}

// ���
void Game::Release(void)
{
	// �V���O���g���̌Ăяo��
	SoundManager* seMana_ = SoundManager::GetInstance();

	// BGM�̒�~
	seMana_->StopBGM("GameBGM");
	seMana_->StopBGM("BossBGM");

	// �������
	seMana_->ReleaseSound("GameBGM");
	seMana_->ReleaseSound("BossBGM");

	// �A�j���[�V�����̉��
	animation_->Release();
	delete animation_;
	animation_ = nullptr;
	// �J�����̉��
	camera_->Release();
	delete camera_;
	camera_ = nullptr;
	// HP�o�[
	hpBar_->Release();
	delete hpBar_;
	hpBar_ = nullptr;
	// �A�C�R��
	aicon_->Release();
	delete aicon_;
	aicon_ = nullptr;
	//���̉��
	sword_->Release();
	delete sword_;
	sword_ = nullptr;
	// �v���C���[�̉��
	player_->Release();
	delete player_;
	player_ = nullptr;
	//�G�̉��
	enemymng_->Release();
	delete enemymng_;
	enemymng_ = nullptr;
	//�X�e�[�W�̉��
	stage_->Release();
	delete stage_;
	stage_ = nullptr;
	// �o�t
	buff_->Release();
	delete buff_;
	buff_ = nullptr;
	// �����蔻��
	collision_->Release();
	delete collision_;
	collision_ = nullptr;
}