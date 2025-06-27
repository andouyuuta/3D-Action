#pragma once
#include <DxLib.h>
#include "../Manager/AnimationManager.h"

class Player;

class Enemy
{
public:
	static constexpr float WALK_START = 0.05f;	// 動き始める判定
	static constexpr float WALK_STOP = 0.02f;	// 止まった判定
	// 敵の最大HP
	int ENEMY_MAX_HP = 300;
	// 敵の最大攻撃力
	int AttackPower_ = 40;

	// 敵の情報の構造体
	struct Info
	{
		int modelId_;			// モデルID

		VECTOR scale_;			// 大きさ
		VECTOR pos_;			// 座標
		VECTOR nextPos_;		// 次の座標
		VECTOR rot_;			// 回転値
		float moveSpeed_;		// 移動スピード
		int moveDir_;			// 移動方向
		int moveKind_;			// プレイヤーの移動状態
		int hp_;				// HP
		int maxHp_;				// 最大HP
		int attackPower_;		// 攻撃力

		// 移動ベクトル
		VECTOR moveVec_;		// 移動ベクトル
		VECTOR oldMoveVec_;		// 直前の移動ベクトル
		VECTOR moveVecRad_;		// 移動ベクトルが生成する角度
		VECTOR localRot_;		// 調整用初期角度

		bool isDead_;			// 死んでいるか
		bool isDraw_;			// 表示するか
		bool isHitboxActive_;	// 当たり判定が有効しているか
		bool isAttack_;			// 攻撃しているか

		//ボス関連
		bool isBoss_;			// ボスかどうか

		float attackRange_;		// 攻撃可能距離
		float attackCooldown_;	// 攻撃クールダウン

		bool isAssign_;			// 当てられたか
	};

	Enemy(void);				// コンストラクタ
	virtual ~Enemy(void);		// デストラクタ

	virtual void Init(int org, int hp, int attack, Player* player, AnimationManager* anim, AnimationManager::AnimationInfo* animinfo);	// 初期化
	virtual void Update(void);		// 更新
	virtual void Draw(void);	// 描画
	virtual void Release(void); // 解放

	// 追跡処理
	void UpdateMove(void);

	// 死亡フラグ
	bool IsDead(void)const;

	// 敵の現在位置の取得する（外部から読み取り専用）
	[[nodiscard]] int GetModel(void)const { return list.modelId_; }
	[[nodiscard]] VECTOR GetPosition(void)const { return list.pos_; }
	[[nodiscard]] float GetRotY(void)const { return list.rot_.y; }
	[[nodiscard]] int GetHP(void)const { return list.hp_; }
	[[nodiscard]] VECTOR GetRightHandPosition(void);
	[[nodiscard]] VECTOR GetLeftHandPosition(void);
	[[nodiscard]] bool GetAttackFlg(void) { return list.isAttack_; }
	//struct Info GetInfo(void) { return list; }
	[[nodiscard]] bool GetIsBoss(void)const { return list.isBoss_; }
	[[nodiscard]] int GetAttackPower(void)const { return list.attackPower_; }	// 敵の攻撃力
	[[nodiscard]] bool GetIsAssign(void) { return list.isAssign_; }

	// セッター関数
	void SetEnemyPos(const VECTOR& pos);		// 敵の座標の設定
	void SetDamage(int dp);						// 敵のHPを減らす
	void SetIsBoss(bool boss) { list.isBoss_ = boss; }
	void SetIsAttack(bool flg) { list.isAttack_ = flg; }

	void SetIsAssign(bool flg) { list.isAssign_ = flg; }

	AnimationManager::AnimationInfo* GetAnimInfo() { return animInfo_; }
	void SetAnimInfo(AnimationManager::AnimationInfo* info) { animInfo_ = info; }
protected:
	void DrawDebug(void);

	// 構造体を格納する変数
	struct Info list;

	// 静的インスタンス
	static Enemy* instance_;

	Player* player_;
	AnimationManager* animation_;
	AnimationManager::AnimationInfo* animInfo_;
};