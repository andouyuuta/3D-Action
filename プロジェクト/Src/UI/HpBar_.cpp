#include "HpBar_.h"
#include "PlayerHpBar.h"
#include "EnemyHpBar.h"
#include "../Object/Player.h"
#include "../Object/EnemyManager.h"

HpBar::HpBar(void)
{
	hpBar_ = nullptr;
	enemyHp_ = nullptr;
}

HpBar::~HpBar(void)
{
	Release();
}

void HpBar::Init(Player* player, EnemyManager* enemyManager)
{
	// インスタンス生成
	hpBar_ = new PlayerHpBar();
	enemyHp_ = new EnemyHpBar();

	hpBar_->Init(player);
	enemyHp_->Init(enemyManager);
}

void HpBar::Update(void)
{
	if (hpBar_) hpBar_->Update();
	if (enemyHp_) enemyHp_->Update();
}

void HpBar::Draw(void)
{
	if (hpBar_) hpBar_->Draw();
	if (enemyHp_) enemyHp_->Draw();
}

void HpBar::Release(void)
{
	if (hpBar_)
	{
		hpBar_->Release();
		delete hpBar_;
		hpBar_ = nullptr;
	}

	if (enemyHp_)
	{
		enemyHp_->Release();
		delete enemyHp_;
		enemyHp_ = nullptr;
	}
}
