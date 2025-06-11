#pragma once
#include <DxLib.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>

class Enemy;
class BossEnemy;

class EnemyManager
{
public:
	static constexpr int ENEMY_COUNT = 1;				// 敵の数
	

	EnemyManager(void);	// コンストラクタ
	~EnemyManager(void); // デストラクタ

	void Init();    // 敵の初期化
	void Update();  // 敵の更新
	void Draw();    // 敵の描画
	void Release(); // 解放処理


	// プレイヤー側で敵のリストを取得するための関数
	std::vector<Enemy*> GetEnemyPtrs() const;
	[[nodiscard]] int GetEnemyCount(void) { return ENEMY_COUNT; }
	[[nodiscard]] int GetAliveEnemyCount()const;
	[[nodiscard]] bool GetBossDead()const { return bossDead_; }
	[[nodiscard]] void SetBossDead(bool dead) { bossDead_ = dead; }

	int currentPhase_ = 0;						// 現在のフェーズ（1~4）
	const int MAX_PHASE = 3;					// フェーズの最大数
	bool isWaitingForSkillSelect_ = false;		// スキルの選択も待ち時間

private:
	std::vector<Enemy*> result;
	std::vector<std::unique_ptr<Enemy>> enemies_;
	std::unordered_map<std::string, int> enemyModels;	//モデルを配列で管理
	
	int deadCount_;		// 通常の敵を倒した数
	bool bossSpawned_;	// ボス出現
	bool bossDead_;     // ボスの死亡
	bool isPhaseClear_;	// フェーズのフラグ

	void SpawnBoss();

	void SpawnPhaseEnemies();	// スポーンフェーズの敵の処理
	void ShowSkillSelect();		// スキル
};