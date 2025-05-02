#include "Enemy.h"
#include "Player.h"
#include "../Application.h"
#include "../Manager/InputManager.h"

namespace
{
	// 移動量
	const float MOVE_SPEED_WALK = 2.5f;	// 歩き
	const float MOVE_SPEED_RUN = 8.0f;	// 走る
	const float MOVE_SPEED_STOP = 0.0f;

	// 初期モデル補正角度
	const VECTOR INIT_MODEL_ROT_OFFSET = { 0.0f,DX_PI_F / 2.0f,0.0f };

	// 向いている方向
	const VECTOR INIT_DIR = { 0.0f,0.0f,-1.0f };

	// 視野角
	const float VIEW_ANGLE = 30.0f;

	// アニメーションの再生速度
	const float ANIM_SPEED = 0.5f;		// 生きている時
	const float DEAD_ANIM_SPEED = 0.2f;	// 死んでいる時
}

Enemy::Enemy(void)
{
	list.modelid_ = -1;
	list.scale_ = VGet(1.0f, 1.0f, 1.0f);
	list.pos_ = VGet(0.0f, 0.0f, 0.0f);
	list.rot_ = VGet(0.0f, 0.0f, 0.0f);

	list.moveSpeed_ = MOVE_SPEED_STOP;
	list.moveDir_ = 0;
	list.moveKind_ = 0;

	list.moveVec_ = VGet(0.0f, 0.0f, 0.0f);
	list.oldMoveVec_ = VGet(0.0f, 0.0f, 0.0f);
	list.moveVecRad_ = VGet(0.0f, 0.0f, 0.0f);
	list.localRot_ = INIT_MODEL_ROT_OFFSET;

	list.animindex_ = 0;
	list.animAttachNo_ = -1;
	list.animTotalTime_ = 0.0f;
	list.currentAnimTime_ = 0.0f;
	list.animlockflg_ = false;

	list.deadflg_ = false;
}

Enemy::~Enemy(void)
{
}

// 初期化
void Enemy::Init(int org)
{
	//モデル
	list.modelid_ = MV1DuplicateModel(org);
	//座標
	list.pos_ = { 0.0f,0.0f,50.0f };
	MV1SetPosition(list.modelid_, list.pos_);
	//大きさ
	list.scale_ = { 1.0f,1.0f,1.0f };
	MV1SetScale(list.modelid_, list.scale_);
	//回転
	list.rot_ = { 0.0f,0.0f,0.0f };
	MV1SetRotationXYZ(list.modelid_, list.rot_);

	// 敵のHPの初期化
	list.hp_ = ENEMY_MAX_HP;


	//アニメーション関連
	list.animindex_ = 0;
	list.animAttachNo_ = MV1AttachAnim(list.modelid_, list.animindex_);
	list.animTotalTime_ = MV1GetAttachAnimTotalTime(list.modelid_, list.animAttachNo_);
	list.currentAnimTime_ = 0.0f;
	list.animlockflg_ = false;
	MV1SetAttachAnimTime(list.modelid_, list.animAttachNo_, list.currentAnimTime_);

	list.deadflg_ = false;
}

// 更新
void Enemy::Update()
{
	//UpdateMove();
	PlayAnimation();
	DebugAnimation();
}

// 描画
void Enemy::Draw(void)
{
	MV1DrawModel(list.modelid_);
}

// 解放
void Enemy::Release(void)
{
	MV1DeleteModel(list.modelid_);
}

// 追跡処理
void Enemy::UpdateMove(void)
{
	// 攻撃中は移動しない
	if (list.animlockflg_)return;

	// プレイヤーとのXZ距離を計算
	VECTOR toPlayer = VSub(Player::GetInstance().GetPlayerPos(), list.pos_);
	toPlayer.y = 0.0f;
	float dist = VSize(toPlayer);

	const float ATTACK_RANGE = 50.0f;

	if (dist < ATTACK_RANGE)
	{
		// 攻撃距離に入ったら攻撃アニメーション
		if (list.animindex_ != 3)		// 攻撃アニメーションでなければ
		{
			ChangeAnimation(3);			// 攻撃アニメーション
			list.animlockflg_ = true;	// 攻撃が終わるまでロック
		}
		// ダメージ処理(できるなら1回だけ当てれるようにする)
		return;
	}

	// プレイヤー追跡
	VECTOR dir = VNorm(toPlayer); // 向きの正規化
	list.moveVec_ = VScale(dir, MOVE_SPEED_WALK);	// 移動ベクトルをスピード付きで設定
	list.pos_ = VAdd(list.pos_, list.moveVec_);		// 座標の更新

	// 回転もプレイヤー方向に向ける
	list.moveVecRad_.y = atan2f(dir.x, dir.z);
	list.rot_.y = list.moveVecRad_.y;

	// モデル更新
	MV1SetPosition(list.modelid_, list.pos_);

	// 歩きアニメーションに変更
	if (!list.animlockflg_)ChangeAnimation(1);
}

// 死亡フラグ
bool Enemy::IsDead() const
{
	return list.deadflg_;
}

void Enemy::SetEnemyPos(const VECTOR& pos)
{
	list.pos_ = pos;
	MV1SetPosition(list.modelid_, list.pos_);
}


// 敵の現在位置の取得する（外部から読み取り専用）
VECTOR Enemy::GetPosition() const
{
	return list.pos_;
}

// 敵のHPの取得
int Enemy::GetHP() const
{
	return list.hp_;
}

// 敵のHPを減らす
void Enemy::SetDamage(int dp)
{
	list.hp_ -= dp;
	// HPが0以下なら死亡状態へ移行
	if (list.hp_ <= 0)
	{
		list.hp_ = 0;
		// 死んだフラグ
		list.deadflg_ = true;
	}
}

//アニメーション再生
void Enemy::PlayAnimation(void)
{
	DebugAnimation();			//数字キーでアニメーション切り替え

	switch (list.animindex_)
	{
		//ループさせる
	case 0:			//待機
	case 1:			//歩く
	case 2:			//走る
		list.currentAnimTime_ += ANIM_SPEED;
		if (list.currentAnimTime_ > list.animTotalTime_)
		{
			list.currentAnimTime_ = 0.0f;
		}
		break;
		//ループさせない
	case 3:			//攻撃
		list.currentAnimTime_ += ANIM_SPEED;
		if (list.currentAnimTime_ >= list.animTotalTime_)
		{
			list.currentAnimTime_ = list.animTotalTime_;
			list.animlockflg_ = false;
		}
		break;
	case 4:			//死亡
		list.currentAnimTime_ += DEAD_ANIM_SPEED;
		if (list.currentAnimTime_ >= list.animTotalTime_)
		{
			list.currentAnimTime_ = list.animTotalTime_;
			list.animlockflg_ = false;
		}
		break;
	default:
		break;
	}
	MV1SetAttachAnimTime(list.modelid_, list.animAttachNo_, list.currentAnimTime_);
	MV1SetPosition(list.modelid_, list.pos_);
}

//矢印キーでアニメーション切り替え
void Enemy::DebugAnimation(void)
{
	InputManager& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_0)) {
		ChangeAnimation(0);
	}
	if (ins.IsTrgDown(KEY_INPUT_1)) {
		ChangeAnimation(1);
	}
	if (ins.IsTrgDown(KEY_INPUT_2)) {
		ChangeAnimation(2);
	}
	if (ins.IsTrgDown(KEY_INPUT_3)) {
		ChangeAnimation(3);
	}
	if (ins.IsTrgDown(KEY_INPUT_4)) {
		ChangeAnimation(4);
	}
}

//アニメーション切り替え
void Enemy::ChangeAnimation(int idx)
{
	if (list.animindex_ != idx) {
		MV1DetachAnim(list.modelid_, list.animAttachNo_);
		list.animindex_ = idx;
		list.animAttachNo_ = MV1AttachAnim(list.modelid_, list.animindex_);
		list.animTotalTime_ = MV1GetAttachAnimTotalTime(list.modelid_, list.animAttachNo_);
		list.currentAnimTime_ = 0.0f;
		MV1SetAttachAnimTime(list.modelid_, list.animAttachNo_, list.currentAnimTime_);
	}
}
