#pragma once
#include <DxLib.h>
#include <vector>

class Enemy
{
public:
	// 移動量
	static constexpr float MOVE_SPEED_WALK = 2.5f;		// 歩き
	static constexpr float MOVE_SPEED_RUN = 8.0f;		// 走る
	static constexpr float MOVE_SPEED_STOP = 0.0f;

	// 初期モデル補正角度
	static constexpr VECTOR INIT_MODEL_ROT_OFFSET = { 0.0f,DX_PI_F / 2.0f,0.0f };

	// 向いている方向
	static constexpr VECTOR INIT_DIR = { 0.0f,0.0f,-1.0f };

	// 視野角
	static constexpr float VIEW_ANGLE = 30.0f;

	// アニメーションの再生速度
	static constexpr float ANIM_SPEED = 0.5f;		// 生きている時
	static constexpr float DEAD_ANIM_SPEED = 0.2f;	// 死んでいる時

	// 敵の移動状態管理用
	enum EnemyAnim {
		ANIM_IDLE = 0,
		ANIM_WALK,
		ANIM_RUN,
		ANIM_ATTACK,
		ANIM_DEAD
	};

	// 敵の最大HP
	static constexpr int ENEMY_MAX_HP = 10;

	// 敵の情報の構造体
	struct Info
	{
		int modelid_;			//モデルID

		VECTOR scale_;			//大きさ
		VECTOR pos_;			//座標
		VECTOR rot_;			//回転値
		float moveSpeed_;		//移動スピード
		int moveDir_;			//移動方向
		int moveKind_;			//プレイヤーの移動状態
		int hp_;				// HP

		// 移動ベクトル
		VECTOR moveVec_;		//移動ベクトル
		VECTOR oldMoveVec_;		//直前の移動ベクトル
		VECTOR moveVecRad_;		//移動ベクトルが生成する角度

		VECTOR localRot_;		//調整用初期角度

		//アニメーション
		int animindex_;			//アニメーションの種類
		int  animAttachNo_;		//設定するアニメーションの番号
		float animTotalTime_;	//アニメーションの総再生時間
		float currentAnimTime_; //直前のアニメーションの時間
		bool animlockflg_;		//アニメーションロック

		bool deadflg_;			//死んでいるか
		bool drawflg_;			//表示するか
		bool isHitboxActive_;	// 当たり判定が有効しているか
		bool hasHitPlayer_;		// 攻撃一回で一度しかヒットしないようにするフラグ
		bool attackflg_;		// 攻撃しているか
	};

	Enemy(void);		// コンストラクタ
	~Enemy(void);		// デストラクタ

	void Init(int org);	// 初期化
	void Update();	// 更新
	void Draw(void);	// 描画
	void Release(void); // 解放

	// 追跡処理
	void UpdateMove(void);

	// 死亡フラグ
	bool IsDead()const;

	// セッター関数
	void SetEnemyPos(const VECTOR& pos);		// 敵の座標の設定

	// ゲッター関数
	struct Info GetInfo(void) { return list; }

	// 敵の現在位置の取得する（外部から読み取り専用）
	[[nodiscard]] int GetModel()const { return list.modelid_; }
	[[nodiscard]] VECTOR GetPosition()const { return list.pos_; }
	[[nodiscard]] float GetRotY()const { return list.rot_.y; }
	[[nodiscard]] int GetHP()const { list.hp_; }
	[[nodiscard]] int GetRightHandIndex(void) { return MV1SearchFrame(list.modelid_, "mixamorig:RightHand"); }
	[[nodiscard]] VECTOR GetRightHandPosition(void) { return MV1GetFramePosition(list.modelid_, GetRightHandIndex()); }
	[[nodiscard]] bool GetAttackFlg(void) { return list.attackflg_; }
	// 敵のHPを減らす
	void SetDamage(int dp);
private:
	// 構造体を格納する変数
	struct Info list;

	void PlayAnimation(void);		//アニメーション再生
	void DebugAnimation(void);		//数字キーでアニメーション切り替え
	void ChangeAnimation(int idx, bool lock = false);	//アニメーション切り替え
};

