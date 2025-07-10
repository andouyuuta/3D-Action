#pragma once
#include <DxLib.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include "../Manager/AnimationManager.h"
#include <set>

class Player;
class Enemy;
class BossEnemy;
class AnimationManager;
class Buff;

class EnemyManager
{
public:
	static constexpr int ENEMY_COUNT = 1;				// 敵の数

	EnemyManager(void);		// コンストラクタ
	~EnemyManager(void);	// デストラクタ

	void Init(Player* player, AnimationManager* anim, const AnimationManager::AnimationInfo& animinfo, Buff* buff);    // 敵の初期化
	void Update(void);		// 敵の更新
	void Draw(void);		// 敵の描画
	void Release(void);		// 解放処理

	// ゲッター関数
	[[nodiscard]] std::vector<Enemy*> GetEnemyPtrs(void) const;
	[[nodiscard]] int GetEnemyCount(void) { return ENEMY_COUNT; }
	[[nodiscard]] int GetAliveEnemyCount(void)const;
	[[nodiscard]] bool GetBossDead(void)const { return bossDead_; }
	[[nodiscard]] void SetBossDead(bool dead) { bossDead_ = dead; }

	int currentPhase_ = 0;						// 現在のフェーズ
	const int MAX_PHASE = 5;					// フェーズの最大数
	bool isWaitingForSkillSelect_ = false;		// スキルの選択も待ち時間
	bool IsBoss_;

	bool GetBossSpawn(void) { return bossSpawned_; }
	bool GetIsBoss(void) { return IsBoss_; }

	// 状態取得関数
	bool IsWaitingForSkillSelect(void) const;

	// ゲッター
	int GetTotalHp(void)const { return totalHp_; }
	int GetTotalAttack(void)const { return totalAttack_; }

	void SetIsWaitingSlect(bool select) { isWaitingForSkillSelect_ = select; }
	void ShowSkillSelect(void);		// スキル選択
private:

	// ポインター
	Buff* buff_;
	Enemy* enemy_;
	BossEnemy* boss_;

	std::vector<Enemy*> result;
	std::vector<std::unique_ptr<Enemy>> enemies_;
	std::unordered_map<std::string, int> enemyModels;	//モデルを配列で管理
	int deadCount_;					// 通常の敵を倒した数
	bool bossSpawned_;				// ボス出現
	bool bossDead_;					// ボスの死亡
	bool isPhaseClear_;				// フェーズのフラグ
	int totalHp_;
	int totalAttack_;

	void SpawnBoss(void);

	void SpawnPhaseEnemies(void);	// スポーンフェーズの敵の処理

	Player* player_;
	AnimationManager* animation_;
	AnimationManager::AnimationInfo animInfo_;

	
};