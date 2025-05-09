#pragma once
#include "Enemy.h"
#include <DxLib.h>
#include <vector>

class EnemyManager
{
public:
	EnemyManager(void);	// コンストラクタ
	~EnemyManager(void); // デストラクタ

	void Init();    // 敵の初期化
	void Update();  // 敵の更新
	void Draw();    // 敵の描画
	void Release(); // 解放処理

	// プレイヤー側で敵のリストを取得するための関数
	const std::vector<Enemy*>& GetEnemies() const;

	[[nodiscard]] int GetEnemyCount(void) { return ENEMY_COUNT; }
	[[nodiscard]] int GetAliveEnemyCount()const;
private:
	std::vector<Enemy*> enemyies_;			// 敵のリスト
	int enemymodel_;
	int bossenemymodel_;

	int ENEMY_COUNT = 10;	// 敵の数
};