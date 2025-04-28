#pragma once
#include <DxLib.h>
#include <vector>
#include "Enemy.h"

class EnemyManager
{
public:
	EnemyManager(void);
	~EnemyManager(void);

	void Init(void);
	void Update(void);
	void Draw(void);
	void Release(void);
private:
	std::vector<Enemy> Enemylist;
	int enemymodel_;
	int bossenemymodel_;
};