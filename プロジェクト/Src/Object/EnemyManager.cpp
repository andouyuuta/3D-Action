#include "EnemyManager.h"
#include "BossEnemy.h"
#include "Player.h"
#include "Enemy.h"
#include "../Application.h"
#include "../Manager/RandomManager.h"
#include "../Manager/SceneManager.h"
#include "../UI/Buff.h"

// コンストラクタ
EnemyManager::EnemyManager(void)
{
	deadCount_ = 0;			// 通常の敵の死亡カウント
	bossSpawned_ = false;	// ボス出現
	bossDead_ = false;      // ボスの死亡フラグ
	isPhaseClear_ = false;

	player_ = nullptr;
	buff_ = nullptr;
	animation_ = nullptr;
	animInfo_ = {};
}

// デストラクタ
EnemyManager::~EnemyManager(void)
{
}

// 初期化
void EnemyManager::Init(Player* player, AnimationManager* anim,const AnimationManager::AnimationInfo& animinfo, Buff* buff)
{
	player_ = player;
	animation_ = anim;
	animInfo_ = animinfo;
	buff_ = buff;

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

		player_->FullRecoveryHp();  // プレイヤーHP全快

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

	// アニメーション情報を map に登録し、その参照を取得
	animation_->enemyAnimInfos_.insert({ boss.get(), {} });
	AnimationManager::AnimationInfo& animInfo = animation_->enemyAnimInfos_[boss.get()];

	animInfo.animIndex_ = AnimationManager::EnemyAnim::ANIM_IDLE;
	animInfo.animAttachNo_ = MV1AttachAnim(enemyModels["boss"], animInfo.animIndex_);
	animInfo.animTotalTime_ = MV1GetAttachAnimTotalTime(enemyModels["boss"], animInfo.animAttachNo_);
	animInfo.currentAnimTime_ = 0.0f;
	animInfo.isAnimLock_ = false;
	int baseHp = 300;
	int baseAttack = 20;

	boss->Init(enemyModels["boss"], 1500, 40, player_, animation_, &animInfo);
	boss->SetEnemyPos({ 0.0f, 0.0f, 0.0f });

	enemies_.push_back(std::move(boss));
}

// フェーズごとの通常敵の生成処理（数・強さをフェーズに応じて増やす）
void EnemyManager::SpawnPhaseEnemies()
{
	enemies_.clear();
	int enemyCount = ENEMY_COUNT + (currentPhase_ - 1) * 5; // フェーズごとに5体ずつ増加

	// === フェーズに応じた強化 ===
	int hpBoost = (currentPhase_ - 1) * 20;
	int atkBoost = (currentPhase_ - 1) * 5;
	int enemyHp = 300;        // 基本HP（例えば10）
	int enemyAttack = 50;              // 基本攻撃力（仮）

	//複数の敵の初期化
	for (int i = 0; i < enemyCount; i++)
	{
		auto enemy = std::make_unique<Enemy>();

		AnimationManager::AnimationInfo animInfo{};
		animInfo.animIndex_ = AnimationManager::EnemyAnim::ANIM_IDLE;
		animInfo.animAttachNo_ = MV1AttachAnim(enemyModels["normal"], animInfo.animIndex_);
		animInfo.animTotalTime_ = MV1GetAttachAnimTotalTime(enemyModels["normal"], animInfo.animAttachNo_);
		animInfo.currentAnimTime_ = 0.0f;
		animInfo.isAnimLock_ = false;

		// AnimationManagerに登録
		animation_->enemyAnimInfos_[enemy.get()] = animInfo;

		// アニメーション情報のポインタを取得
		AnimationManager::AnimationInfo* animPtr = &animation_->enemyAnimInfos_[enemy.get()];

		// 敵にAnimationInfoのポインタを渡す（Init の引数にポインタを渡す）
		enemy->Init(enemyModels["normal"],
			enemyHp + hpBoost,
			enemyAttack + atkBoost,
			player_,
			animation_,
			animPtr);
		// 座標ランダム配置
		VECTOR pos = { GetRand(500) - 250.0f, 0.0f, GetRand(500) - 250.0f };
		enemy->SetEnemyPos(pos);

		enemies_.push_back(std::move(enemy));
	}
}

// スキル選択
void EnemyManager::ShowSkillSelect()
{
	buff_->Update();

	if (buff_->IsSelected())
	{
		// スキル選択完了後の初期化
		buff_->Reset();

		// 共通処理
		isWaitingForSkillSelect_ = false;		// スキルの選択されたためフラグを折る
		isPhaseClear_ = false;					// フェーズのフラグを元に戻す

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
}

// 状態取得関数
bool EnemyManager::IsWaitingForSkillSelect() const
{
	return isWaitingForSkillSelect_;
}
