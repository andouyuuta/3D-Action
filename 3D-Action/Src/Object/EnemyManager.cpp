#include "EnemyManager.h"
#include "../Application.h"

// �R���X�g���N�^
EnemyManager::EnemyManager(void)
	:enemymodel_(-1), bossenemymodel_(-1)
{
}

// �f�X�g���N�^
EnemyManager::~EnemyManager(void)
{
}

// ������
void EnemyManager::Init(void)
{
	enemyies_.clear();

	// �G�̃��f���̓ǂݍ���
	enemymodel_ = MV1LoadModel((Application::PATH_MODEL + "EnemyModel.mv1").c_str());
	bossenemymodel_ = MV1LoadModel((Application::PATH_MODEL + "BossEnemyModel.mv1").c_str());

	//�����̓G�̏�����(����10)
	for (int i = 0; i <= ENEMY_COUNT; i++)
	{
		Enemy* enemy = new Enemy();
		enemy->Init(enemymodel_);

		// �����_�����W�ɔz�u
		VECTOR pos = { GetRand(500) - 250.0f,0.0f,GetRand(500) - 250.0f };
		enemy->SetEnemyPos(pos);

		enemyies_.push_back(enemy);
	}
}

// �X�V
void EnemyManager::Update(void)
{
	//�����̓G�̍X�V
	for (Enemy* enemy : enemyies_)
	{
		if (enemy != nullptr)
		{
			enemy->Update();
		}
	}
}

// �`��
void EnemyManager::Draw(void)
{
	//�����̓G�̕`��
	for (Enemy* enemy : enemyies_)
	{
		if (enemy != nullptr)
		{
			enemy->Draw();
		}
	}

}

// ���
void EnemyManager::Release(void)
{
	//�����̓G�̉��
	for (Enemy* enemy : enemyies_)
	{

		if (enemy != nullptr)
		{
			enemy->Release();
			delete enemy;
		}
	}
	enemyies_.clear();

	// ���f�����̂̉��
	if (enemymodel_ != -1)
	{
		MV1DeleteModel(enemymodel_);
		enemymodel_ = -1;
	}

	// �{�X
	if (bossenemymodel_ != -1)
	{
		MV1DeleteModel(bossenemymodel_);
		bossenemymodel_ = -1;
	}
}

// �v���C���[���œG�̃��X�g���擾���邽�߂̊֐�
const std::vector<Enemy*>& EnemyManager::GetEnemies() const
{
	return enemyies_;
}
