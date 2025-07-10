#pragma once

class Player;
class EnemyManager;
class PlayerHpBar;
class EnemyHpBar;

class HpBar
{
public:
	HpBar(void);
	~HpBar(void);

	void Init(Player* player, EnemyManager* enemyManager);
	void Update(void);
	void Draw(void);
	void Release(void);

private:
	PlayerHpBar* hpBar_;
	EnemyHpBar* enemyHp_;

	EnemyManager* enemymng_;
};
