#include "EnemyManager.h"
#include "BossEnemy.h"
#include "../Application.h"
#include "Player.h"
#include "../Manager/SceneManager.h"
#include "Enemy.h"

// コンストラクタ
EnemyManager::EnemyManager(void)
{
	deadCount_ = 0;			// 通常の敵の死亡カウント
	bossSpawned_ = false;	// ボス出現
	bossDead_ = false;      // ボスの死亡フラグ
	isPhaseClear_ = false;
}

// デストラクタ
EnemyManager::~EnemyManager(void)
{
}

// 初期化
void EnemyManager::Init(void)
{
	enemies_.clear();

	// 敵のモデルの読み込み
	enemyModels["normal"] = MV1LoadModel((Application::PATH_MODEL + "EnemyModel.mv1").c_str());
	enemyModels["boss"] = MV1LoadModel((Application::PATH_MODEL + "BossEnemyModel.mv1").c_str());
	// 現在のフェーズの初期化
	currentPhase_ = 1;
	// 通常の敵をの死亡数の初期化
	deadCount_ = 0;
	// ボスの死亡フラグの初期化
	bossDead_ = false;
	// ボス出現の初期化
	bossSpawned_ = false;
	// スキルの選択も待ち時間
	isWaitingForSkillSelect_ = false;
	//フェーズのフラグ
	isPhaseClear_ = false;
	// スポーンフェーズの敵の処理の初期化
	SpawnPhaseEnemies();
}

// 更新
void EnemyManager::Update()
{
	// スキル選択中は処理を止める
	if (isWaitingForSkillSelect_)
	{
		ShowSkillSelect();  // キー入力による選択を処理
		return;
	}
		
	//複数の敵の更新
	for (auto it = enemies_.begin(); it != enemies_.end(); )
	{
		auto& enemy = *it;

		// === ボスが死んでいたらフラグを立てる（削除前に！） ===
		if (!bossDead_ && bossSpawned_)
		{
			if (dynamic_cast<BossEnemy*>(enemy.get()) && enemy->IsDead())
			{
				bossDead_ = true;
			}
		}

		// === 死亡している敵は削除 ===
		if (enemy->IsDead())
		{
			// 死亡数カウント
			++deadCount_;
			it = enemies_.erase(it);
		}
		else
		{
			enemy->Update();
			++it;
		}
	}

	// 全敵撃破後、次フェーズやボス出現処理へ
	if (enemies_.empty() && !isWaitingForSkillSelect_ && !isPhaseClear_)
	{
		isPhaseClear_ = true;		// フェーズのフラグ

		Player::GetInstance().RecoverHp();  // プレイヤーHP全快

		// 現在のフェーズが最大フェーズがどうか
		if (currentPhase_ < MAX_PHASE || currentPhase_ == MAX_PHASE)
		{
			// スキルの選択がされた
			isWaitingForSkillSelect_ = true;
		}
		else if(!bossSpawned_)// すべてのフェーズの敵を倒したのでボスの出現させる
		{
			SpawnBoss();
		}
		
	}
}

// 描画
void EnemyManager::Draw(void)
{
	//複数の敵の描画
	for (const auto& enemy : enemies_)
	{
		if (enemy != nullptr)
		{
			if (enemy) enemy->Draw();
		}
	}
	// スキル選択時だけメッセージ表示
	if (isWaitingForSkillSelect_)
	{
		// 左上あたりに表示
		DrawFormatString(0, 200, GetColor(255, 255, 255), "▼ スキルを選択してください ▼");
		DrawFormatString(0, 240, GetColor(255, 255, 0), "1: 攻撃力+30 ＆ HP最大値+30");
		DrawFormatString(0, 280, GetColor(0, 255, 0), "2: 攻撃力+40");
		DrawFormatString(0, 320, GetColor(0, 255, 255), "3: 攻撃力＆HP最大値(-50or+100)");
	}
}

// 解放
void EnemyManager::Release(void)
{
	enemies_.clear();

	// モデルの解放
	for (auto& pair : enemyModels)
	{
		if (pair.second != -1)
		{
			MV1DeleteModel(pair.second);
			pair.second = -1;
		}
	}
}

// 生きている敵のポインターの取得
std::vector<Enemy*> EnemyManager::GetEnemyPtrs() const
{
	std::vector<Enemy*> result;
	for (const auto& e : enemies_) {
		result.push_back(e.get());
	}
	return result;
}

// 生きている敵の数のを取得
int EnemyManager::GetAliveEnemyCount() const
{
	int count = 0;
	for (const auto& enemy : enemies_)
	{
		if (enemy && !enemy->IsDead()) ++count;
	}
	return count;
}

// ボスの生成処理
void EnemyManager::SpawnBoss()
{
	if (bossSpawned_) return; // 2重防止
	bossSpawned_ = true;
	auto boss = std::make_unique<BossEnemy>();

	auto enemy = std::make_unique<Enemy>();

	int baseHp = enemy->ENEMY_MAX_HP;        // 基本HP
	int baseAttack = 350;              // 基本攻撃力

	boss->Init(enemyModels["boss"], baseHp, baseAttack);
	boss->SetEnemyPos({ 0.0f, 0.0f, 0.0f });
	enemies_.push_back(std::move(boss));
}

// フェーズごとの通常敵の生成処理（数・強さをフェーズに応じて増やす）
void EnemyManager::SpawnPhaseEnemies()
{
	enemies_.clear();
	int enemyCount = ENEMY_COUNT + (currentPhase_ - 1) * 5; // フェーズごとに5体ずつ増加

	auto enemy = std::make_unique<Enemy>();

	// === フェーズに応じた強化 ===
	int hpBoost = (currentPhase_ - 1) * 20;
	int atkBoost = (currentPhase_ - 1) * 5;
	int enemyHp = enemy->ENEMY_MAX_HP;        // 基本HP（例えば10）
	int enemyAttack = 50;              // 基本攻撃力（仮）

	//複数の敵の初期化
	for (int i = 0; i < enemyCount; i++)
	{
		auto enemy = std::make_unique<Enemy>();
		enemy->Init(enemyModels["normal"],enemyHp + hpBoost, enemyAttack + atkBoost);

		// ランダム座標に配置
		VECTOR pos = { GetRand(500) - 250.0f,0.0f,GetRand(500) - 250.0f };
		enemy->SetEnemyPos(pos);

		enemies_.push_back(std::move(enemy));
	}
}

// スキル選択
void EnemyManager::ShowSkillSelect()
{
	if (CheckHitKey(KEY_INPUT_1))
	{
		//　攻撃力とHPの上限アップ
		Player::GetInstance().AddAttack(30);
		Player::GetInstance().AddMaxHp(30);
	}
	else if (CheckHitKey(KEY_INPUT_2)) 
	{
		// 攻撃力のみUP
		Player::GetInstance().AddAttack(40);
	}
	else if (CheckHitKey(KEY_INPUT_3))
	{
		// 攻撃力とHPのアップ処理(確率)
		int boost = GetRand(1) == 0 ? -50 : 80 ;
		Player::GetInstance().AddAttack(boost);
		Player::GetInstance().AddMaxHp(boost);
	}
	else
	{
		// まだ選んでいない
		return;
	}

	// 共通処理
	isWaitingForSkillSelect_ = false;		// スキルの選択されたためフラグを折る
	isPhaseClear_ = false;					//フェーズのフラグを元に戻す

	// フェーズが最終だった場合ボスを出現させる
	if (currentPhase_ >= MAX_PHASE)
	{
		if (!bossSpawned_)
		{
			SpawnBoss();
		}
	}
	else
	{
		currentPhase_++;						// 現在のフェーズに足す
		deadCount_ = 0;							// 死亡数の初期化
		SpawnPhaseEnemies();					// スポーンフェーズの敵の処理
	}
}
