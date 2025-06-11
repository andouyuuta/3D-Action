#include "Enemy.h"
#include "Player.h"
#include "../Application.h"
#include "../Manager/InputManager.h"

namespace
{
	// 移動量
	const float MOVE_SPEED_WALK = 1.5f;				// 歩き
	const float MOVE_BOSS_SPEED_WALK = 5.0f;		// ボス走る
	const float MOVE_SPEED_RUN = 8.0f;				// 走る
	const float MOVE_SPEED_STOP = 0.0f;
	// 初期モデル補正角度
	const VECTOR INIT_MODEL_ROT_OFFSET = { 0.0f,DX_PI_F / 2.0f,0.0f };
	// 向いている方向
	const VECTOR INIT_DIR = { 0.0f,0.0f,-1.0f };
	// 視野角
	const float VIEW_ANGLE = 30.0f;
	// アニメーションの再生速度
	const float ANIM_SPEED = 0.5f;					// 生きている時
	const float ATTACK_ANIM_SPEED = 0.5f;			// 攻撃時
	const float DEAD_ANIM_SPEED = 0.2f;				// 死んでいる時
	
	// 攻撃のクールタイム(1.0f＝1秒)
	const float ATTACK_COOLDOWN_TIME = 1.0f * 60;
}


Enemy::Enemy(void)
{
	list.modelId_ = -1;
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

	list.animIndex_ = 0;
	list.animAttachNo_ = -1;
	list.animTotalTime_ = 0.0f;
	list.currentAnimTime_ = 0.0f;
	list.isAnimLock_ = false;

	list.isDead_ = false;
	list.isDraw_ = true;
	list.isAttack_ = false;
	list.isBoss_ = false;

	list.isAssign_ = false;

	list.attackRange_ = 0.0f;
}

Enemy::~Enemy(void)
{
}

// 初期化
void Enemy::Init(int org, int hp, int attack)
{
	//モデル
	list.modelId_ = MV1DuplicateModel(org);
	//座標
	list.pos_ = { 0.0f,0.0f,50.0f };
	list.nextPos_ = { 0.0f,0.0f,0.0f };
	MV1SetPosition(list.modelId_, list.pos_);
	//大きさ
	list.scale_ = { 0.8f,0.8f,0.8f };
	MV1SetScale(list.modelId_, list.scale_);
	//回転
	list.rot_ = { 0.0f,0.0f,0.0f };
	MV1SetRotationXYZ(list.modelId_, list.rot_);

	// 敵のHPの初期化
	list.hp_ = hp;
	//最大HPの保存
	list.maxHp_ = hp; 
	// 攻撃力の初期化
	list.attackPower_ = attack;

	//アニメーション関連
	list.animIndex_ = 0;
	list.animAttachNo_ = MV1AttachAnim(list.modelId_, list.animIndex_);
	list.animTotalTime_ = MV1GetAttachAnimTotalTime(list.modelId_, list.animAttachNo_);
	list.currentAnimTime_ = 0.0f;
	list.isAnimLock_ = false;
	MV1SetAttachAnimTime(list.modelId_, list.animAttachNo_, list.currentAnimTime_);

	list.isDead_ = false;

	list.isHitboxActive_ = false;
}

// 更新
void Enemy::Update()
{
	UpdateMove();
	PlayAnimation();
	MV1RefreshCollInfo(list.modelId_);
}

// 描画
void Enemy::Draw(void)
{
	if (!list.isDead_) 
	{
		MV1DrawModel(list.modelId_);
		VECTOR worldPos = VAdd(list.pos_, VGet(0.0f, 50.0f, 0.0f)); // 敵の上の位置
		worldPos.y += 100.0f;  // 頭の上
		float screenX, screenY;

		if (GetTransformPosition(&worldPos, &screenX, &screenY) == 0)
		{
			const int BAR_WIDTH = 40;
			const int BAR_HEIGHT = 5;

			// HPバー描画
			float hpRatio = (float)list.hp_ / list.maxHp_;
			if (hpRatio < 0.0f) hpRatio = 0.0f;
			if (hpRatio > 1.0f) hpRatio = 1.0f;

			int barX = static_cast<int>(screenX) - BAR_WIDTH / 2;
			int barY = static_cast<int>(screenY) - BAR_HEIGHT / 2;

			// 背景バー（グレー）
			DrawBox(barX, barY, barX + BAR_WIDTH, barY + BAR_HEIGHT, GetColor(80, 80, 80), TRUE);
			// HPバー（赤）
			DrawBox(barX, barY, barX + (int)(BAR_WIDTH * hpRatio), barY + BAR_HEIGHT, GetColor(255, 0, 0), TRUE);
			// デバッグ表示
			DrawFormatString(0, 600, GetColor(255, 255, 255), "敵HP：%d / %d", list.hp_, list.maxHp_);
			DrawFormatString(0, 630, GetColor(255, 255, 255), "敵攻撃力：%d", list.attackPower_);
		}
	}
}

// 解放
void Enemy::Release(void)
{
	MV1DeleteModel(list.modelId_);
}

// 移動処理
void Enemy::UpdateMove(void)
{
	// 攻撃中は移動しない
	if (list.isAnimLock_) return;

	// 高さを強制的に地面に固定（不要なら削除可）
	if (list.pos_.y != 0.0f)
	{
		list.pos_.y = 0.0f;
	}

	if (!Player::GetInstance().GetIsDead())
	{
		// プレイヤーとのXZ方向ベクトル
		VECTOR toPlayer = VSub(Player::GetInstance().GetPlayerPos(), list.pos_);
		toPlayer.y = 0.0f;

		VECTOR dir = VNorm(toPlayer); // 正規化した向きベクトル

		// プレイヤーの方向に「指定距離」だけオフセットした座標を攻撃基準にする
		const float attackOffsetDistance = 20.0f;
		VECTOR targetAttackPos = VAdd(Player::GetInstance().GetPlayerPos(), VScale(dir, attackOffsetDistance));

		// 攻撃対象との距離を測る
		float dist = VSize(VSub(targetAttackPos, list.pos_));

		// 攻撃範囲設定（ボス or 通常敵）
		//list.attackRange_ = list.isBoss_ ? 100.0f : 50.0f;
		list.attackRange_ = 100.0f;
		// クールダウン減少
		if (list.attackCooldown_ > 0.0f)
		{
			list.attackCooldown_ -= 1.0f;
		}

		// 攻撃条件を満たす場合
		if (!list.isAnimLock_ && dist < list.attackRange_ && list.attackCooldown_ <= 0.0f)
		{
			list.isAttack_ = true;
			ChangeAnimation(EnemyAnim::ANIM_ATTACK, true);
			list.attackCooldown_ = ATTACK_COOLDOWN_TIME;
			return;
		}

		// 攻撃範囲内だがクールダウン中 → 動かない
		if (dist < list.attackRange_)
		{
			// 攻撃範囲内にいるので移動しない（向きだけ合わせる）
			float targetAngleY = atan2f(dir.x, dir.z);
			list.rot_.y = targetAngleY - INIT_MODEL_ROT_OFFSET.y * 2;

			MV1SetPosition(list.modelId_, list.pos_);
			MV1SetRotationXYZ(list.modelId_, list.rot_);

			// モーション切り替え（歩かない）
			if (list.animIndex_ != EnemyAnim::ANIM_IDLE && !list.isAnimLock_)
			{
				ChangeAnimation(ANIM_IDLE);
			}
			return;
		}

		float animRatio = list.currentAnimTime_ / list.animTotalTime_;

		if ((animRatio >= 0.8f && animRatio <= 1.0f) || (animRatio <= 0.1f) || (animRatio >= 0.3f && animRatio <= 0.6f))
		{

			// プレイヤーが攻撃範囲外だったら接近開始
			if (list.isBoss_)
			{
				list.moveVec_ = VScale(dir, MOVE_BOSS_SPEED_WALK);
			}
			else
			{
				list.moveVec_ = VScale(dir, MOVE_SPEED_WALK);
			}

			list.nextPos_ = VAdd(list.pos_, list.moveVec_);
			list.pos_ = list.nextPos_;

			// 向き調整
			float targetAngleY = atan2f(dir.x, dir.z);
			list.rot_.y = targetAngleY - INIT_MODEL_ROT_OFFSET.y * 2;
		}

		MV1SetPosition(list.modelId_, list.pos_);
		MV1SetRotationXYZ(list.modelId_, list.rot_);

		// モーション切り替え（移動中なら歩き）
		if (VSize(list.moveVec_) > 0.01f)
		{
			if (list.animIndex_ != EnemyAnim::ANIM_WALK && !list.isAnimLock_)
			{
				ChangeAnimation(ANIM_WALK);
			}
		}
		else
		{
			if (list.animIndex_ != EnemyAnim::ANIM_IDLE && !list.isAnimLock_)
			{
				ChangeAnimation(ANIM_IDLE);
			}
		}
	}
	else
	{
		// プレイヤー死亡時は待機
		ChangeAnimation(ANIM_IDLE);
	}
}

// 死亡フラグ
bool Enemy::IsDead() const
{
	return list.isDead_;
}

void Enemy::SetEnemyPos(const VECTOR& pos)
{
	list.pos_ = pos;
	MV1SetPosition(list.modelId_, list.pos_);
}

VECTOR Enemy::GetRightHandPosition(void)
{
	int frameIndex = MV1SearchFrame(list.modelId_, "mixamorig:RightHand");
	if (frameIndex != -1)
	{
		// モデルのアニメーションや位置・回転を反映させた上でのワールド座標を取得
		return MV1GetFramePosition(list.modelId_, frameIndex);
	}
	else
	{
		// フレームが見つからなかった場合は適当に上方向の位置を返す
		return VAdd(list.pos_, VGet(0.0f, 100.0f, 0.0f));
	}
}
VECTOR Enemy::GetLeftHandPosition(void)
{
	int frameIndex = MV1SearchFrame(list.modelId_, "mixamorig:LeftHand");
	if (frameIndex != -1)
	{
		// モデルのアニメーションや位置・回転を反映させた上でのワールド座標を取得
		return MV1GetFramePosition(list.modelId_, frameIndex);
	}
	else
	{
		// フレームが見つからなかった場合は適当に上方向の位置を返す
		return VAdd(list.pos_, VGet(0.0f, 100.0f, 0.0f));
	}
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
		list.isDead_ = true;
	}
}

//アニメーション再生
void Enemy::PlayAnimation(void)
{
	switch (list.animIndex_)
	{
	//ループさせる
	case ANIM_IDLE:				//待機
	case ANIM_WALK:				//歩く
	case ANIM_RUN:				//走る
		list.currentAnimTime_ += ANIM_SPEED;
		if (list.currentAnimTime_ > list.animTotalTime_)
		{
			list.currentAnimTime_ = 0.0f;
		}
		break;
	//ループさせない
	case ANIM_ATTACK:			//攻撃
		list.currentAnimTime_ += ATTACK_ANIM_SPEED;
		if (list.currentAnimTime_ >= list.animTotalTime_)
		{
			list.currentAnimTime_ = list.animTotalTime_;
			list.isAnimLock_ = false;
			list.isAttack_ = false;
		}
		break;
	case ANIM_DEAD:				//死亡
		list.currentAnimTime_ += DEAD_ANIM_SPEED;
		if (list.currentAnimTime_ >= list.animTotalTime_)
		{
			list.currentAnimTime_ = list.animTotalTime_;
			list.isAnimLock_ = false;
		}
		break;
	default:
		break;
	}
	MV1SetAttachAnimTime(list.modelId_, list.animAttachNo_, list.currentAnimTime_);
	MV1SetPosition(list.modelId_, list.pos_);
}

//アニメーション切り替え
void Enemy::ChangeAnimation(int idx, bool lock)
{
	if (list.animIndex_ != idx) {
		MV1DetachAnim(list.modelId_, list.animAttachNo_);
		list.animIndex_ = idx;
		list.animAttachNo_ = MV1AttachAnim(list.modelId_, list.animIndex_);
		list.animTotalTime_ = MV1GetAttachAnimTotalTime(list.modelId_, list.animAttachNo_);
		list.currentAnimTime_ = 0.0f;
		MV1SetAttachAnimTime(list.modelId_, list.animAttachNo_, list.currentAnimTime_);
		//ロック指定されたらフラグON
		list.isAnimLock_ = lock;
	}
}
