#include "Enemy.h"
#include "Player.h"
#include "../Application.h"
#include "../Manager/InputManager.h"
#include "../Manager/SoundManager.h"

namespace
{
	// 移動量
	const float MOVE_SPEED_WALK = 1.5f;				// 歩き
	const float MOVE_BOSS_SPEED_WALK = 5.0f;		// ボス走る
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

	// シングルトンの呼び出し
	SoundManager* seMana_ = SoundManager::GetInstance();

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

	//SEのロード
	seMana_->LoadSE("Voice", (Application::PATH_SOUND + "SE/Zombie_voice.mp3").c_str(), false);
	seMana_->LoadSE("ZB_Attack", (Application::PATH_SOUND + "SE/Zombie_attack.mp3").c_str(), false);
	seMana_->LoadSE("Boss_walk", (Application::PATH_SOUND + "SE/Boss_walk.mp3").c_str(), false);
}

// 更新
void Enemy::Update(void)
{
	// シングルトンの呼び出し
	SoundManager* seMana_ = SoundManager::GetInstance();
	UpdateMove();
	MV1RefreshCollInfo(list.modelId_);
	// SEの座標の設定
	seMana_->SetPosSE("Voice", list.pos_);
	seMana_->SetPosSE("ZB_Attack", list.pos_);
	// SEの聞こえる範囲
	seMana_->SetAreaSE("Voice", 100.0f);
	seMana_->SetAreaSE("ZB_Attack", 500.0f);
	// SEの音量設定
	seMana_->SetVolumeSE("Voice", 150);
	seMana_->SetVolumeSE("ZB_Attack", 200);
}

// 描画
void Enemy::Draw(void)
{

	if (!list.isDead_) 
	{
		MV1DrawModel(list.modelId_);
		//DrawBox(list.pos_.x, list.pos_.y, list.pos_.x + 80, list.pos_.y + 10, 0xff0000, false);
	}
}

// 解放
void Enemy::Release(void)
{
	// シングルトンの呼び出し
	SoundManager* seMana_ = SoundManager::GetInstance();

	MV1DeleteModel(list.modelId_);

	// SEの停止
	seMana_->StopSE("Voice");
	seMana_->StopSE("ZB_Attack");
	seMana_->StopSE("Boss_walk");

	// 解放処理
	seMana_->ReleaseSound("Voice");
	seMana_->ReleaseSound("ZB_Attack");
	seMana_->ReleaseSound("Boss_walk");

}

// 移動処理
void Enemy::UpdateMove(void)
{
	const auto& anim = GetAnimInfo();

	// シングルトンの呼び出し
	SoundManager* seMana_ = SoundManager::GetInstance();

	// 攻撃中は移動しない
	if (anim->isAnimLock_) return;

	// 高さを強制的に地面に固定（不要なら削除可）
	if (list.pos_.y != 0.0f)
	{
		list.pos_.y = 0.0f;
	}

	if (!player_->GetIsDead())
	{ 
		// SEの再生
		seMana_->PlaySE("Voice");

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
			// SEの再生
			seMana_->PlaySE("ZB_Attack");
			list.isAttack_ = true;
			animation_->EnemyChangeAnimation(this, AnimationManager::EnemyAnim::ANIM_ATTACK, true);
			list.attackCooldown_ = ATTACK_COOLDOWN_TIME;
			return;
		}

		// 攻撃範囲内だがクールダウン中 → 動かない
		if (dist < list.attackRange_)
		{

			// SEの停止
			seMana_->StopSE("ZB_Attack");

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
			// SEの再生
			seMana_->PlaySE("Boss_walk");
			list.moveVec_ = VScale(dir, MOVE_BOSS_SPEED_WALK);

			list.nextPos_ = VAdd(list.pos_, list.moveVec_);
			list.pos_ = list.nextPos_;

			// 向き調整
			float targetAngleY = atan2f(dir.x, dir.z);
			list.rot_.y = targetAngleY - INIT_MODEL_ROT_OFFSET.y * 2;
		}
		else
		{
			// SEの停止
				seMana_->StopSE("Boss_walk");
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

	if (player_->GetIsDead())
	{
		// SEの停止
		seMana_->StopSE("Voice");
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
	MV1RefreshCollInfo(list.modelId_);
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
