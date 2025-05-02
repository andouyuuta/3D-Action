#pragma once
#include <DxLib.h>
#include <vector>

class Enemy;

class Collision
{
public:
	Collision();
	~Collision();

	void Init(void);
	void Update(const std::vector<Enemy*>& enemies);
	void Draw(void);
	void Release(void);

private:
	// “G
	Enemy* enemy_;

	void PlayerToStageCollision(void);
	void EnemyToStageCollision(void);
	void PlayerToEnemyCollision(const std::vector<Enemy*>& enemies);

	int stagemodel_;
};

