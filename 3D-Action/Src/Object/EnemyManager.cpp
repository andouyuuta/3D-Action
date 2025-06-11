#include "EnemyManager.h"
#include "BossEnemy.h"
#include "../Application.h"
#include "Player.h"
#include "../Manager/SceneManager.h"
#include "Enemy.h"

// �R���X�g���N�^
EnemyManager::EnemyManager(void)
{
	deadCount_ = 0;			// �ʏ�̓G�̎��S�J�E���g
	bossSpawned_ = false;	// �{�X�o��
	bossDead_ = false;      // �{�X�̎��S�t���O
	isPhaseClear_ = false;
}

// �f�X�g���N�^
EnemyManager::~EnemyManager(void)
{
}

// ������
void EnemyManager::Init(void)
{
	enemies_.clear();

	// �G�̃��f���̓ǂݍ���
	enemyModels["normal"] = MV1LoadModel((Application::PATH_MODEL + "EnemyModel.mv1").c_str());
	enemyModels["boss"] = MV1LoadModel((Application::PATH_MODEL + "BossEnemyModel.mv1").c_str());
	// ���݂̃t�F�[�Y�̏�����
	currentPhase_ = 1;
	// �ʏ�̓G���̎��S���̏�����
	deadCount_ = 0;
	// �{�X�̎��S�t���O�̏�����
	bossDead_ = false;
	// �{�X�o���̏�����
	bossSpawned_ = false;
	// �X�L���̑I�����҂�����
	isWaitingForSkillSelect_ = false;
	//�t�F�[�Y�̃t���O
	isPhaseClear_ = false;
	// �X�|�[���t�F�[�Y�̓G�̏����̏�����
	SpawnPhaseEnemies();
}

// �X�V
void EnemyManager::Update()
{
	// �X�L���I�𒆂͏������~�߂�
	if (isWaitingForSkillSelect_)
	{
		ShowSkillSelect();  // �L�[���͂ɂ��I��������
		return;
	}
		
	//�����̓G�̍X�V
	for (auto it = enemies_.begin(); it != enemies_.end(); )
	{
		auto& enemy = *it;

		// === �{�X������ł�����t���O�𗧂Ă�i�폜�O�ɁI�j ===
		if (!bossDead_ && bossSpawned_)
		{
			if (dynamic_cast<BossEnemy*>(enemy.get()) && enemy->IsDead())
			{
				bossDead_ = true;
			}
		}

		// === ���S���Ă���G�͍폜 ===
		if (enemy->IsDead())
		{
			// ���S���J�E���g
			++deadCount_;
			it = enemies_.erase(it);
		}
		else
		{
			enemy->Update();
			++it;
		}
	}

	// �S�G���j��A���t�F�[�Y��{�X�o��������
	if (enemies_.empty() && !isWaitingForSkillSelect_ && !isPhaseClear_)
	{
		isPhaseClear_ = true;		// �t�F�[�Y�̃t���O

		Player::GetInstance().RecoverHp();  // �v���C���[HP�S��

		// ���݂̃t�F�[�Y���ő�t�F�[�Y���ǂ���
		if (currentPhase_ < MAX_PHASE || currentPhase_ == MAX_PHASE)
		{
			// �X�L���̑I�������ꂽ
			isWaitingForSkillSelect_ = true;
		}
		else if(!bossSpawned_)// ���ׂẴt�F�[�Y�̓G��|�����̂Ń{�X�̏o��������
		{
			SpawnBoss();
		}
		
	}
}

// �`��
void EnemyManager::Draw(void)
{
	//�����̓G�̕`��
	for (const auto& enemy : enemies_)
	{
		if (enemy != nullptr)
		{
			if (enemy) enemy->Draw();
		}
	}
	// �X�L���I�����������b�Z�[�W�\��
	if (isWaitingForSkillSelect_)
	{
		// ���゠����ɕ\��
		DrawFormatString(0, 200, GetColor(255, 255, 255), "�� �X�L����I�����Ă������� ��");
		DrawFormatString(0, 240, GetColor(255, 255, 0), "1: �U����+30 �� HP�ő�l+30");
		DrawFormatString(0, 280, GetColor(0, 255, 0), "2: �U����+40");
		DrawFormatString(0, 320, GetColor(0, 255, 255), "3: �U���́�HP�ő�l(-50or+100)");
	}
}

// ���
void EnemyManager::Release(void)
{
	enemies_.clear();

	// ���f���̉��
	for (auto& pair : enemyModels)
	{
		if (pair.second != -1)
		{
			MV1DeleteModel(pair.second);
			pair.second = -1;
		}
	}
}

// �����Ă���G�̃|�C���^�[�̎擾
std::vector<Enemy*> EnemyManager::GetEnemyPtrs() const
{
	std::vector<Enemy*> result;
	for (const auto& e : enemies_) {
		result.push_back(e.get());
	}
	return result;
}

// �����Ă���G�̐��̂��擾
int EnemyManager::GetAliveEnemyCount() const
{
	int count = 0;
	for (const auto& enemy : enemies_)
	{
		if (enemy && !enemy->IsDead()) ++count;
	}
	return count;
}

// �{�X�̐�������
void EnemyManager::SpawnBoss()
{
	if (bossSpawned_) return; // 2�d�h�~
	bossSpawned_ = true;
	auto boss = std::make_unique<BossEnemy>();

	auto enemy = std::make_unique<Enemy>();

	int baseHp = enemy->ENEMY_MAX_HP;        // ��{HP
	int baseAttack = 350;              // ��{�U����

	boss->Init(enemyModels["boss"], baseHp, baseAttack);
	boss->SetEnemyPos({ 0.0f, 0.0f, 0.0f });
	enemies_.push_back(std::move(boss));
}

// �t�F�[�Y���Ƃ̒ʏ�G�̐��������i���E�������t�F�[�Y�ɉ����đ��₷�j
void EnemyManager::SpawnPhaseEnemies()
{
	enemies_.clear();
	int enemyCount = ENEMY_COUNT + (currentPhase_ - 1) * 5; // �t�F�[�Y���Ƃ�5�̂�����

	auto enemy = std::make_unique<Enemy>();

	// === �t�F�[�Y�ɉ��������� ===
	int hpBoost = (currentPhase_ - 1) * 20;
	int atkBoost = (currentPhase_ - 1) * 5;
	int enemyHp = enemy->ENEMY_MAX_HP;        // ��{HP�i�Ⴆ��10�j
	int enemyAttack = 50;              // ��{�U���́i���j

	//�����̓G�̏�����
	for (int i = 0; i < enemyCount; i++)
	{
		auto enemy = std::make_unique<Enemy>();
		enemy->Init(enemyModels["normal"],enemyHp + hpBoost, enemyAttack + atkBoost);

		// �����_�����W�ɔz�u
		VECTOR pos = { GetRand(500) - 250.0f,0.0f,GetRand(500) - 250.0f };
		enemy->SetEnemyPos(pos);

		enemies_.push_back(std::move(enemy));
	}
}

// �X�L���I��
void EnemyManager::ShowSkillSelect()
{
	if (CheckHitKey(KEY_INPUT_1))
	{
		//�@�U���͂�HP�̏���A�b�v
		Player::GetInstance().AddAttack(30);
		Player::GetInstance().AddMaxHp(30);
	}
	else if (CheckHitKey(KEY_INPUT_2)) 
	{
		// �U���͂̂�UP
		Player::GetInstance().AddAttack(40);
	}
	else if (CheckHitKey(KEY_INPUT_3))
	{
		// �U���͂�HP�̃A�b�v����(�m��)
		int boost = GetRand(1) == 0 ? -50 : 80 ;
		Player::GetInstance().AddAttack(boost);
		Player::GetInstance().AddMaxHp(boost);
	}
	else
	{
		// �܂��I��ł��Ȃ�
		return;
	}

	// ���ʏ���
	isWaitingForSkillSelect_ = false;		// �X�L���̑I�����ꂽ���߃t���O��܂�
	isPhaseClear_ = false;					//�t�F�[�Y�̃t���O�����ɖ߂�

	// �t�F�[�Y���ŏI�������ꍇ�{�X���o��������
	if (currentPhase_ >= MAX_PHASE)
	{
		if (!bossSpawned_)
		{
			SpawnBoss();
		}
	}
	else
	{
		currentPhase_++;						// ���݂̃t�F�[�Y�ɑ���
		deadCount_ = 0;							// ���S���̏�����
		SpawnPhaseEnemies();					// �X�|�[���t�F�[�Y�̓G�̏���
	}
}
