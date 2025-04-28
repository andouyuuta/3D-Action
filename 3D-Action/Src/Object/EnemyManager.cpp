#include "EnemyManager.h"
#include "../Application.h"

EnemyManager::EnemyManager(void)
	:enemymodel_(-1),bossenemymodel_(-1)
{
}

EnemyManager::~EnemyManager(void)
{
}

void EnemyManager::Init(void)
{
	enemymodel_ = MV1LoadModel((Application::PATH_MODEL + "EnemyModel.mv1").c_str());
	bossenemymodel_ = MV1LoadModel((Application::PATH_MODEL + "BossEnemyModel.mv1").c_str());
	//�����̓G�̏�����
	for (int i = 0; i <= 10; i++)
	{
		Enemylist.emplace_back();
		Enemylist[i].Init(enemymodel_);
	}
}

void EnemyManager::Update(void)
{
	//�����̓G�̍X�V
	for (Enemy& enemy : Enemylist)
	{
		enemy.Update();
	}
}

void EnemyManager::Draw(void)
{
	//�����̓G�̕`��
	for (Enemy& enemy : Enemylist)
	{
		enemy.Draw();
	}
}

void EnemyManager::Release(void)
{
	//�����̓G�̉��
	for (Enemy& enemy : Enemylist)
	{
		enemy.Release();
		MV1DeleteModel(enemymodel_);
	}
}
