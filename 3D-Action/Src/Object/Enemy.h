#pragma once
#include <DxLib.h>

class Enemy
{
public:
	// 敵の移動状態管理用
	enum EnemyAnim {
		ANIM_IDLE = 0,
		ANIM_WALK,
		ANIM_RUN,
		ANIM_ATTACK,
		ANIM_DEAD,
		ANIM_JUMP_ATTACK,
		ANIM_SWIP,
	};

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

		//アニメーション
		int animIndex_;			// アニメーションの種類
		int  animAttachNo_;		// 設定するアニメーションの番号
		float animTotalTime_;	// アニメーションの総再生時間
		float currentAnimTime_; // 直前のアニメーションの時間
		bool isAnimLock_;		// アニメーションロック

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

	virtual void Init(int org, int hp, int attack);	// 初期化
	virtual void Update();		// 更新
	virtual void Draw(void);	// 描画
	virtual void Release(void); // 解放

	// 追跡処理
	void UpdateMove(void);

	// 死亡フラグ
	bool IsDead()const;

	// 敵の現在位置の取得する（外部から読み取り専用）
	[[nodiscard]] int GetModel(void)const { return list.modelId_; }
	[[nodiscard]] VECTOR GetPosition(void)const { return list.pos_; }
	[[nodiscard]] float GetRotY(void)const { return list.rot_.y; }
	[[nodiscard]] int GetHP(void)const { return list.hp_; }
	[[nodiscard]] VECTOR GetRightHandPosition(void);
	[[nodiscard]] VECTOR GetLeftHandPosition(void);
	[[nodiscard]] bool GetAttackFlg(void) { return list.isAttack_; }
	struct Info GetInfo(void) { return list; }
	bool GetIsBoss(void)const { return list.isBoss_; }
	int GetAttackPower(void)const { return list.attackPower_; }	// 敵の攻撃力

	// セッター関数
	void SetEnemyPos(const VECTOR& pos);		// 敵の座標の設定
	void SetDamage(int dp);						// 敵のHPを減らす
	void SetIsBoss(bool boss) { list.isBoss_ = boss; }

	void SetIsAssign(bool flg) { list.isAssign_ = flg; }
	bool GetIsAssign(void) { return list.isAssign_; }
protected:
	// 構造体を格納する変数
	struct Info list;

	// 静的インスタンス
	static Enemy* instance_;

	virtual void PlayAnimation(void);		//アニメーション再生
	//void DebugAnimation(void);		//数字キーでアニメーション切り替え
	virtual void ChangeAnimation(int idx, bool lock = false);	//アニメーション切り替え

};

