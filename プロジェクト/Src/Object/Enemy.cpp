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

	list.isDead_ = false;
	list.isDraw_ = true;
	list.isAttack_ = false;
	list.isBoss_ = false;

	list.isAssign_ = false;

	list.attackRange_ = 0.0f;

	player_ = nullptr;
	animInfo_ = nullptr;
	animation_ = nullptr;
}

Enemy::~Enemy(void)
{
}

// 初期化
void Enemy::Init(int org, int hp, int attack, Player* player, AnimationManager* anim, AnimationManager::AnimationInfo* animinfo)
{
	player_ = player;
	animation_ = anim;
	animInfo_ = animinfo;

	SetAnimInfo(animInfo_);

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

	list.isDead_ = false;

	list.isHitboxActive_ = false;
}

// 更新
void Enemy::Update(void)
{
	UpdateMove();
	MV1RefreshCollInfo(list.modelId_);
}

// 描画
void Enemy::Draw(void)
{
	if (!list.isDead_) 
	{
		MV1DrawModel(list.modelId_);
		// 敵のHPバー
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
	const auto& anim = GetAnimInfo();

	// 攻撃中は移動しない
	if (anim->isAnimLock_) return;

	// 高さを強制的に地面に固定（不要なら削除可）
	if (list.pos_.y != 0.0f)
	{
		list.pos_.y = 0.0f;
	}

	if (!player_->GetIsDead())
	{
		// プレイヤーとのXZ方向ベクトル
		VECTOR toPlayer = VSub(player_->GetPlayerPos(), list.pos_);
		toPlayer.y = 0.0f;

		VECTOR dir = VNorm(toPlayer); // 正規化した向きベクトル

		// プレイヤーの方向に「指定距離」だけオフセットした座標を攻撃基準にする
		const float attackOffsetDistance = 20.0f;
		VECTOR targetAttackPos = VAdd(player_->GetPlayerPos(), VScale(dir, attackOffsetDistance));

		// 攻撃対象との距離を測る
		float dist = VSize(toPlayer);

		// 攻撃範囲設定（ボス or 通常敵）
		list.attackRange_ = 90;
		// クールダウン減少
		if (list.attackCooldown_ > 0.0f)
		{
			list.attackCooldown_ -= 1.0f;
		}

		// 攻撃条件を満たす場合
		if (!anim->isAnimLock_ && dist < list.attackRange_ && list.attackCooldown_ <= 0.0f)
		{
			list.isAttack_ = true;
			animation_->EnemyChangeAnimation(this, AnimationManager::EnemyAnim::ANIM_ATTACK, true);
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

			return;
		}
		float animRatio = anim->currentAnimTime_ / anim->animTotalTime_;

		if (list.isBoss_)
		{
			list.moveVec_ = VScale(dir, MOVE_BOSS_SPEED_WALK);

			list.nextPos_ = VAdd(list.pos_, list.moveVec_);
			list.pos_ = list.nextPos_;

			// 向き調整
			float targetAngleY = atan2f(dir.x, dir.z);
			list.rot_.y = targetAngleY - INIT_MODEL_ROT_OFFSET.y * 2;
		}
		else
		{
			if ((animRatio >= 0.8f && animRatio <= 1.0f) || (animRatio <= 0.1f) || (animRatio >= 0.3f && animRatio <= 0.6f))
			{
				// プレイヤーが攻撃範囲外だったら接近開始
				list.moveVec_ = VScale(dir, MOVE_SPEED_WALK);
				list.nextPos_ = VAdd(list.pos_, list.moveVec_);
				list.pos_ = list.nextPos_;

				// 向き調整
				float targetAngleY = atan2f(dir.x, dir.z);
				list.rot_.y = targetAngleY - INIT_MODEL_ROT_OFFSET.y * 2;
			}
		}

		MV1SetPosition(list.modelId_, list.pos_);
		MV1SetRotationXYZ(list.modelId_, list.rot_);

		// モーション切り替え（移動中なら歩き）
		if (VSize(list.moveVec_) > WALK_START)
		{
			if (anim->animIndex_ != AnimationManager::EnemyAnim::ANIM_WALK && !anim->isAnimLock_)
			{
				animation_->EnemyChangeAnimation(this, AnimationManager::EnemyAnim::ANIM_WALK);

			}
		}
		else if (VSize(list.moveVec_) > WALK_STOP)
		{
			if (anim->animIndex_ != AnimationManager::EnemyAnim::ANIM_IDLE && !anim->isAnimLock_)
			{
				animation_->EnemyChangeAnimation(this, AnimationManager::EnemyAnim::ANIM_IDLE);
			}
		}
	}
	else
	{
		// プレイヤー死亡時は待機
		animation_->EnemyChangeAnimation(this, AnimationManager::EnemyAnim::ANIM_IDLE);
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

void Enemy::DrawDebug(void)
{

	//DrawSphere3D(GetRightHandPosition(), 30, 32, 0xffffff, 0xffffff, false);
	if (list.isBoss_) {
		// 敵の右手の位置を取得して球体をセット
		VECTOR forward = VNorm(VGet(-sinf(GetRotY()), 0.0f, -cosf(GetRotY())));
		VECTOR rightattackCenter = VAdd(GetRightHandPosition(), VScale(forward, 50.0f));
		VECTOR leftattackCenter = VAdd(GetLeftHandPosition(), VScale(forward, 50.0f));

		DrawSphere3D(rightattackCenter, 100.0f, 32, 0xffffff, 0xffffff, false);
		DrawSphere3D(leftattackCenter, 100.0f, 32, 0xffffff, 0xffffff, false);

	}
}
