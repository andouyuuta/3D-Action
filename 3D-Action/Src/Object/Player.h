#pragma once
#include <DxLib.h>
#include <vector>

class Sword;
class EnemyManager;

class Player
{
public:
	// 移動量
	static constexpr float MOVE_SPEED_STOP = 0.0f;		// しゃがみ&止まる
	static constexpr float MOVE_SPEED_WALK = 2.5f;		// 歩き
	static constexpr float MOVE_SPEED_RUN = 8.0f;		// 走る

	// 初期モデル補正角度
	static constexpr VECTOR INIT_MODEL_ROT_OFFSET = { 0.0f,DX_PI_F / 2.0f,0.0f };

	// 向いている方向
	static constexpr VECTOR INIT_DIR = { 0.0f,0.0f,-1.0f };

	// 視野角
	static constexpr float VIEW_ANGLE = 180.0f;

	// アニメションの再生速度
	static constexpr float ANIM_SPEED = 0.5f;			//生きている時
	static constexpr float CROUCH_ANIM_SPEED = 1.5f;	//しゃがんでいるとき
	static constexpr float CROUCH_ATTACK_SPEED = 1.0f;	//しゃがみ攻撃
	static constexpr float DEAD_ANIM_SPEED = 0.2f;		//死んでいる時
	static constexpr float ATTACK_ANIM_SPEED = 0.75f;	//攻撃している


	// プレイヤーの最大HP
    int PLAYER_MAX_HP = 200;

	// プレイヤーの最大攻撃力
    int AttackPower_ = 150;

	enum PlayerAnim {
		NoWeaponIdle = 0,		//武器なし待機
		NoWeaponWalk,			//武器なし歩き
		NoWeaponRun,			//武器なし走り
		WeaponOut,				//武器取り出し	
		Sheach,					//武器戻す
		WeaponIdle,				//武器あり待機
		WeaponWalk,				//武器あり歩き
		WeaponRun,				//武器あり走り
		Strafe,					//横歩き
		Jump,					//ジャンプ
		Crouch,					//しゃがみ
		Crouch_Cancel,			//しゃがみ解除
		Crouch_Idle,			//しゃがみ待機
		Crouch_Guard,			//しゃがみ防御
		Crouch_Attack,
		First_Attack,			//攻撃１
		Second_Attack,			//攻撃２
		Third_Attack,			//攻撃３
		Force_Attack,			//攻撃４
		Death,					//死亡
		Impact,					//攻撃受けたとき
	};

	// プレイヤーの情報の構造体
	struct Info
	{
		int modelId_;			// モデルID

		VECTOR pos_;			// 座標
		VECTOR rot_;			// 回転値
		VECTOR scale_;			// 大きさ
		float moveSpeed_;		// 移動スピード
		int moveDir_;			// 移動方向
		int moveKind_;			// プレイヤーの移動状態
		int hp_;				// HP
		int attackPower_;		// 攻撃力

		// 移動ベクトル
		VECTOR moveVec_;		// 移動ベクトル
		VECTOR oldMoveVec_;		// 直前の移動ベクトル
		VECTOR moveVecRad_;		// 移動ベクトルが生成する角度

		VECTOR localRot_;		// 調整用初期角度

		//アニメーション
		int animIndex_;			//アニメーションの種類
		int  animAttachNo_;		// 設定するアニメーションの番号
		float animTotalTime_;	// アニメーションの総再生時間
		float currentAnimTime_; // 直前のアニメーションの時間
		bool isAnimLock_;		//アニメーションロック

		bool isDead_;			//死亡したかどうか
		bool isCrouch_;			//しゃがんでいるか
		bool isWeapon_;			//武器出しているか
		bool isAttack_;			//攻撃しているか
		bool isCombo_;			//コンボしているか
		int comboStep_;			//コンボ段階

		float currentRatio_;	//現在のアニメーションの再生時間(割合)
		float remainingTime_;	//アニメーションがあとどれくらいで終わるか

		bool isGameOver_;		//死亡したかどうか
		bool isImpact_;			//攻撃受けたか
		bool isInvincible_;		//無敵かどうか
		bool hitPlayer_;		// 攻撃一回で一度しかヒットしないようにするフラグ

		float hitStopTime_;		// ヒットストップ用タイマー
		bool isHitStop_;		// ヒットストップ用フラグ
	
		MATRIX rotationMatrix_;	// 回転(マトリックス)
	};

	// インスタンスの生成
	static void CreateInstance(void);

	// インスタンスの取得
	static Player& GetInstance(void);


	Player(void);		// コンストラクタ
	~Player(void);		// デストラクタ

	void Init(EnemyManager* mng);	// 初期化
	void Update(float deltaTime);	// 更新
	void Draw(void);	// 描画
	void Release(void); // 解放

	// ベクトルの移動判定
	bool IsMove(VECTOR _moveVec);

	void SetRotation(void);

	// ゲッター関数
	struct Info GetInfo(void) { return list; }

	// プレイヤーのスピードを設定
	void SetPlayerSpeed(const VECTOR& speed) { list.moveVec_ = speed; }	// 速度設定

	void ChangeAnimation(int idx, bool lock = false);	//アニメーション切り替え

	void HitStop(void);

	void RecoverHp();		// 全回復
	void AddAttack(int amount); // 攻撃力を上げる
	void AddMaxHp(int amount);  // HP,攻撃力を上げる

	void SetCriticalDisplay(bool enable);	// クリティカル表示処理

	bool IsPlayingImpactAnimation()const;	// アニメーション中かどうか
	bool AttackActive(void);				// 攻撃アニメーションかどうか

	bool IsDamagedThisFrame_;				// このフレームでダメージを受けたか

	// ゲッター
	[[nodiscard]] int GetPlayerModel(void) { return list.modelId_; }
	[[nodiscard]] VECTOR GetPlayerPos(void) { return list.pos_; }
	[[nodiscard]] VECTOR GetPlayerRot(void) { return list.rot_; }
	[[nodiscard]] float GetRotX(void) { return list.rot_.x; }
	[[nodiscard]] float GetRotY(void) { return list.rot_.y; }
	[[nodiscard]] bool GetIsWeapon(void) { return list.isWeapon_; }
	[[nodiscard]] bool GetIsDead(void) { return list.isDead_; }
	[[nodiscard]] int GetRightHandIndex(void) { return MV1SearchFrame(list.modelId_, "mixamorig:RightHand"); }
	[[nodiscard]] VECTOR GetRightHandPosition(void) { return MV1GetFramePosition(list.modelId_, GetRightHandIndex()); }
	[[nodiscard]] int GetDamageCooldown(void) { return damageCooldown_; }
	[[nodiscard]] int GetHp(void) { return list.hp_; }
	[[nodiscard]] bool GetIsAttack(void){return list.isAttack_;}
	[[nodiscard]] bool GetIsInvincible(void)const { return list.isInvincible_; }
	[[nodiscard]] bool GetIsHitStop(void)const { return list.isHitStop_; }
	[[nodiscard]] int GetComboStep(void)const { return list.comboStep_; }
	[[nodiscard]] MATRIX GetRotationMat(void)const { return list.rotationMatrix_; }
	int GetAttackPower()const { return list.attackPower_ ; }	// プレイヤーの攻撃力

	// セッター
	[[nodiscard]] void SetPlayerPos(VECTOR pos) { list.pos_ = pos; MV1SetPosition(list.modelId_, list.pos_); }
	[[nodiscard]] void SetDamageCooldown(int damage) { damageCooldown_ = damage; }
	[[nodiscard]] void SetMoveSpeed(float speed) { list.moveSpeed_ = speed; }
	[[nodiscard]] void DecreaseCoolDown(int damage) { damageCooldown_ -= damage; }
	[[nodiscard]] void TakeDamage(int damage);
	[[nodiscard]] void SetIsDead(bool dead) { list.isDead_ = dead; }
	[[nodiscard]] void SetInvincible(bool flg) { list.isInvincible_ = flg; }
	[[nodiscard]] void SetHitPlayer(bool hitflg) { list.hitPlayer_ = hitflg; }
	[[nodiscard]] void StartHitStop(float time) { list.hitStopTime_ = time; list.isHitStop_ = true; }
private:

	// 敵
	EnemyManager* enemymng_;

	// 静的インスタンス
	static Player* instance_;

	// 構造体を格納する変数
	struct Info list;

	int damageCooldown_; // ダメージ間隔制御用

	// プレイヤーがダメージ中かどうか(多段ヒット防止用)
	bool isDamaged_;
	// 無敵時間の残り時間(秒)
	float invincibleTime_;

	// クリティカル表示用
	bool showCriticalText_;
	float criticalDisplayTime_;


	void UpdateMove(void);								//移動更新
	void PlayerMove(int idle, int walk, int run);		//プレイヤーの動き
	bool CrouchUpdate(void);							//しゃがみ
	bool AttackUpdate(void);							//攻撃
	bool AttackCombo(int nowcombo, int nextanimidx, int nextstep, float reception, float remainingtime);
	void PlayAnimation(void);							//アニメーション再生
	void DebugAnimation(void);							//数字キーでアニメーション切り替え

};

