#include "EnemyManager.h"
#include "BossEnemy.h"
#include "Player.h"
#include "Enemy.h"
#include "../Application.h"
#include "../Manager/RandomManager.h"
#include "../Manager/SceneManager.h"
#include "../UI/Buff.h"

// �R���X�g���N�^
EnemyManager::EnemyManager(void)
{
	deadCount_ = 0;			// �ʏ�̓G�̎��S�J�E���g
	bossSpawned_ = false;	// �{�X�o��
	bossDead_ = false;      // �{�X�̎��S�t���O
	isPhaseClear_ = false;

	player_ = nullptr;
	buff_ = nullptr;
	animation_ = nullptr;
	animInfo_ = {};
}

// �f�X�g���N�^
EnemyManager::~EnemyManager(void)
{
}

// ������
void EnemyManager::Init(Player* player, AnimationManager* anim,const AnimationManager::AnimationInfo& animinfo, Buff* buff)
{
	player_ = player;
	animation_ = anim;
	animInfo_ = animinfo;
	buff_ = buff;

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

		player_->FullRecoveryHp();  // �v���C���[HP�S��

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

	// �A�j���[�V�������� map �ɓo�^���A���̎Q�Ƃ��擾
	animation_->enemyAnimInfos_.insert({ boss.get(), {} });
	AnimationManager::AnimationInfo& animInfo = animation_->enemyAnimInfos_[boss.get()];

	animInfo.animIndex_ = AnimationManager::EnemyAnim::ANIM_IDLE;
	animInfo.animAttachNo_ = MV1AttachAnim(enemyModels["boss"], animInfo.animIndex_);
	animInfo.animTotalTime_ = MV1GetAttachAnimTotalTime(enemyModels["boss"], animInfo.animAttachNo_);
	animInfo.currentAnimTime_ = 0.0f;
	animInfo.isAnimLock_ = false;
	int baseHp = 300;
	int baseAttack = 20;

	boss->Init(enemyModels["boss"], 1500, 40, player_, animation_, &animInfo);
	boss->SetEnemyPos({ 0.0f, 0.0f, 0.0f });

	enemies_.push_back(std::move(boss));
}

// �t�F�[�Y���Ƃ̒ʏ�G�̐��������i���E�������t�F�[�Y�ɉ����đ��₷�j
void EnemyManager::SpawnPhaseEnemies()
{
	enemies_.clear();
	int enemyCount = ENEMY_COUNT + (currentPhase_ - 1) * 5; // �t�F�[�Y���Ƃ�5�̂�����

	// === �t�F�[�Y�ɉ��������� ===
	int hpBoost = (currentPhase_ - 1) * 20;
	int atkBoost = (currentPhase_ - 1) * 5;
	int enemyHp = 300;        // ��{HP�i�Ⴆ��10�j
	int enemyAttack = 50;              // ��{�U���́i���j

	//�����̓G�̏�����
	for (int i = 0; i < enemyCount; i++)
	{
		auto enemy = std::make_unique<Enemy>();

		AnimationManager::AnimationInfo animInfo{};
		animInfo.animIndex_ = AnimationManager::EnemyAnim::ANIM_IDLE;
		animInfo.animAttachNo_ = MV1AttachAnim(enemyModels["normal"], animInfo.animIndex_);
		animInfo.animTotalTime_ = MV1GetAttachAnimTotalTime(enemyModels["normal"], animInfo.animAttachNo_);
		animInfo.currentAnimTime_ = 0.0f;
		animInfo.isAnimLock_ = false;

		// AnimationManager�ɓo�^
		animation_->enemyAnimInfos_[enemy.get()] = animInfo;

		// �A�j���[�V�������̃|�C���^���擾
		AnimationManager::AnimationInfo* animPtr = &animation_->enemyAnimInfos_[enemy.get()];

		// �G��AnimationInfo�̃|�C���^��n���iInit �̈����Ƀ|�C���^��n���j
		enemy->Init(enemyModels["normal"],
			enemyHp + hpBoost,
			enemyAttack + atkBoost,
			player_,
			animation_,
			animPtr);
		// ���W�����_���z�u
		VECTOR pos = { GetRand(500) - 250.0f, 0.0f, GetRand(500) - 250.0f };
		enemy->SetEnemyPos(pos);

		enemies_.push_back(std::move(enemy));
	}
}

// �X�L���I��
void EnemyManager::ShowSkillSelect()
{
	buff_->Update();

	if (buff_->IsSelected())
	{
		// �X�L���I��������̏�����
		buff_->Reset();

		// ���ʏ���
		isWaitingForSkillSelect_ = false;		// �X�L���̑I�����ꂽ���߃t���O��܂�
		isPhaseClear_ = false;					// �t�F�[�Y�̃t���O�����ɖ߂�

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
}

// ��Ԏ擾�֐�
bool EnemyManager::IsWaitingForSkillSelect() const
{
	return isWaitingForSkillSelect_;
}
