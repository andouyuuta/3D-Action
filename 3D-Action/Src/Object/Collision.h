#pragma once
#include <DxLib.h>
#include <vector>

class Enemy;

class Collision
{
public:
	static constexpr float COLLISION_RADIUS = 20.0f;	//“–‚½‚è”»’è‚Ì”¼Œa

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
	void EnemyToStageCollision(const std::vector<Enemy*>& enemies);
	void PlayerToEnemyCollision(const std::vector<Enemy*>& enemies);
	void EnemyToEnemyCollision(const std::vector<Enemy*> enemies);
	void SwordToEnemyCollision(const std::vector <Enemy*>& enemies);
	void DrawDebag(void);
	int stagemodel_;
};

