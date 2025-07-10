#include "AnimationManager.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "RandomManager.h"
#include "../Object/Player.h"
#include "../Object/EnemyManager.h"
#include "../Object/Enemy.h"

namespace PlayerAnimSpeed
{
	// アニメションの再生速度
	const float ANIM_SPEED = 0.5f;			//生きている時
	const float CROUCH_ANIM_SPEED = 1.5f;	//しゃがんでいるとき
	const float CROUCH_ATTACK_SPEED = 1.0f;	//しゃがみ攻撃
	const float DEAD_ANIM_SPEED = 0.2f;		//死んでいる時
	const float ATTACK_ANIM_SPEED = 0.85f;	//攻撃している
}

namespace EnemyAnimSpeed
{
	// アニメーションの再生速度
	const float ANIM_SPEED = 1.0f;					// 生きている時
	const float ATTACK_ANIM_SPEED = 0.5f;			// 攻撃時
	const float DEAD_ANIM_SPEED = 0.2f;				// 死んでいる時
}

AnimationManager::AnimationManager(void)
{
	player_ = nullptr;
	enemymng_ = nullptr;

	playerInfo.animIndex_ = 0;
	playerInfo.animAttachNo_ = 0;
	playerInfo.animTotalTime_ = 0.0f;
	playerInfo.currentAnimTime_ = 0.0f;
	playerInfo.isAnimLock_ = false;

	enemyInfo.animIndex_ = 0;
	enemyInfo.animAttachNo_ = 0;
	enemyInfo.animTotalTime_ = 0.0f;
	enemyInfo.currentAnimTime_ = 0.0f;
	enemyInfo.isAnimLock_ = false;
}

AnimationManager::~AnimationManager(void)
{
}

void AnimationManager::Init(Player* player, EnemyManager* enemymng)
{
	player_ = player;
	enemymng_ = enemymng;

	playerInfo.animIndex_ = 0;
	playerInfo.animAttachNo_ = 0;
	playerInfo.animTotalTime_ = 0.0f;
	playerInfo.currentAnimTime_ = 0.0f;
	playerInfo.isAnimLock_ = false;

	enemyInfo.animIndex_ = 0;
	enemyInfo.animAttachNo_ = 0;
	enemyInfo.animTotalTime_ = 0.0f;
	enemyInfo.currentAnimTime_ = 0.0f;
	enemyInfo.isAnimLock_ = false;
}

void AnimationManager::Update(void)
{
	PlayerAnimationPlayback();
	EnemyAnimationPlayback();
}

void AnimationManager::Release(void)
{
	// プレイヤーのアニメーション解放
	if (player_ && playerInfo.animAttachNo_ != -1)
	{
		MV1DetachAnim(player_->GetPlayerModel(), playerInfo.animAttachNo_);
		playerInfo.animAttachNo_ = -1;
	}

	// 敵のアニメーション解放
	if (enemymng_)
	{
		for (auto& enemy : enemymng_->GetEnemyPtrs())
		{
			auto* anim = enemy->GetAnimInfo();
			if (!anim) continue;

			if (anim->animAttachNo_ != -1)
			{
				MV1DetachAnim(enemy->GetModel(), anim->animAttachNo_);
				anim->animAttachNo_ = -1;
			}
		}
	}
}

// プレイヤー攻撃再生(今のコンボ数、今のコンボ、次のステップ、割合指定、アニメーションの残り時間(割合))
bool AnimationManager::PlayerAttackCombo(int nowcombo, int nextanimidx, int nextstep, float reception, float remainingtime)
{
	InputManager& ins = InputManager::GetInstance();

	//攻撃１中にアニメーションが半分以上進んでいたら攻撃２
	if (player_->GetInfo().isAttack_ && player_->GetInfo().comboStep_ == nowcombo)
	{
		//現在のアニメーション時間(割合)
		playerInfo.currentRatio_ = playerInfo.currentAnimTime_ / playerInfo.animTotalTime_;

		//アニメーションが指定の割合以上＋左クリック押したとき
		if (playerInfo.currentRatio_ >= reception && (ins.IsTrgMouseLeft()||ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1,InputManager::JOYPAD_BTN::TOP)))
		{
			player_->SetIsCombo(true);
		}

		// 残り時間が一定以下＋コンボ可能なら次のアニメーションへ
		playerInfo.remainingTime_ = playerInfo.animTotalTime_ - playerInfo.currentAnimTime_;
		if (playerInfo.remainingTime_ <= remainingtime && player_->GetInfo().isCombo_)
		{
			// 次の攻撃アニメーションへ
			player_->SetComboStep(nextstep);
			player_->SetIsCombo(false);
			PlayerChangeAnimation(nextanimidx, true);
			return true;
		}
	}
	return false;
}

// プレイヤーのアニメーション再生
void AnimationManager::PlayerAnimationPlayback(void)
{
	switch (playerInfo.animIndex_)	// アニメーション番号
	{
		//ループさせる
	case NoWeaponIdle:			//武器なし待機
	case NoWeaponWalk:			//武器なし歩く
	case NoWeaponRun:			//武器なし走る
	case WeaponIdle:			//武器あり待機
	case WeaponWalk:			//武器あり歩く
	case WeaponRun:				//武器あり走る
	case Strafe:				//横歩き
	case Crouch_Idle:			//しゃがみ待機	
		AdvanceAnimation(PlayerAnimSpeed::ANIM_SPEED, []() {}, []() {}, true);
		break;
		//ループさせない
	case WeaponOut:			//武器取り出し
	case Sheach:			//武器戻す
		AdvanceAnimation(PlayerAnimSpeed::ANIM_SPEED, []() {}, []() {});
		break;
	case Jump:				//ジャンプ
		AdvanceAnimation(PlayerAnimSpeed::ATTACK_ANIM_SPEED, []() {}, []() {});
		break;
	case First_Attack:		//攻撃１
	case Second_Attack:		//攻撃２
	case Third_Attack:		//攻撃３
	case Force_Attack:		//攻撃４
		AdvanceAnimation(PlayerAnimSpeed::ATTACK_ANIM_SPEED, [this]() {
			player_->SetIsAttack(false);
			player_->SetComboStep(0);
			player_->SetIsCombo(false);
			}, []() {});
		break;
	case Impact:		//攻撃受けたとき
		AdvanceAnimation(PlayerAnimSpeed::ATTACK_ANIM_SPEED, [this]() {
			// 攻撃/コンボフラグのクリア
			player_->SetIsImpact(false);
			player_->SetIsAttack(false);
			player_->SetComboStep(0);
			player_->SetIsCombo(false);
			}, [this]() {
				player_->SetIsImpact(true);
			});
		break;
	case Death:		//死亡
		AdvanceAnimation(PlayerAnimSpeed::DEAD_ANIM_SPEED, [this]() {
			if (!player_->GetIsRevive())
			{
				if (RandomManager::Random(100.0f))
				{
					player_->Revive();
					PlayerChangeAnimation(PlayerAnim::Revive, true);
				}
				else
				{
					SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAMEOVER);
				}
			}
			else
			{
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAMEOVER);
			}
			}, []() {});
		break;
	case Revive:
		AdvanceAnimation(PlayerAnimSpeed::ANIM_SPEED, []() {}, []() {});
		break;
	case Crouch:		//しゃがみ(防御)
	case Crouch_Cancel:		//しゃがみ解除
	case Crouch_Guard:		//防御したとき
		AdvanceAnimation(PlayerAnimSpeed::CROUCH_ANIM_SPEED, []() {}, []() {});
		break;
	default:
		break;
	}
	MV1SetAttachAnimTime(player_->GetPlayerModel(), playerInfo.animAttachNo_, playerInfo.currentAnimTime_);
	MV1SetPosition(player_->GetPlayerModel(), player_->GetPlayerPos());
}

void AnimationManager::EnemyAnimationPlayback(void)
{
	for (auto& enemy : enemymng_->GetEnemyPtrs())
	{
		auto* anim = enemy->GetAnimInfo();
		if (!anim) continue;

		switch (anim->animIndex_)
		{
			// ループさせる
		case ANIM_IDLE:
		case ANIM_WALK:
		case ANIM_RUN:
			anim->currentAnimTime_ += EnemyAnimSpeed::ANIM_SPEED;
			if (anim->currentAnimTime_ > anim->animTotalTime_)
			{
				anim->currentAnimTime_ = 0.0f;
			}
			break;
			// ループさせない
		case ANIM_ATTACK:
			anim->currentAnimTime_ += EnemyAnimSpeed::ATTACK_ANIM_SPEED;
			if (anim->currentAnimTime_ >= anim->animTotalTime_)
			{
				anim->currentAnimTime_ = anim->animTotalTime_;
				anim->isAnimLock_ = false;
				enemy->SetIsAttack(false);
			}
			break;
		case ANIM_JUMP_ATTACK:
			anim->currentAnimTime_ += EnemyAnimSpeed::ATTACK_ANIM_SPEED;
			if (anim->currentAnimTime_ >= anim->animTotalTime_)
			{
				anim->currentAnimTime_ = anim->animTotalTime_;
				anim->isAnimLock_ = false;
				enemy->SetIsAttack(false);
			}
			break;
		case ANIM_DEAD:
			anim->currentAnimTime_ += EnemyAnimSpeed::DEAD_ANIM_SPEED;
			if (anim->currentAnimTime_ >= anim->animTotalTime_)
			{
				anim->currentAnimTime_ = anim->animTotalTime_;
				anim->isAnimLock_ = false;
			}
			break;
		default:
			break;
		}

		MV1SetAttachAnimTime(enemy->GetModel(), anim->animAttachNo_, anim->currentAnimTime_);
		MV1SetPosition(enemy->GetModel(), enemy->GetPosition());
	}
}

// アニメーション切り替え
void AnimationManager::PlayerChangeAnimation(int idx, bool lock)
{
	// 番号が違ったら変更する
	if (playerInfo.animIndex_ != idx) {
		MV1DetachAnim(player_->GetPlayerModel(), playerInfo.animAttachNo_);										// アニメーション削除
		playerInfo.animIndex_ = idx;																	// 指定されたアニメーションを代入
		// アニメーションを再設定
		playerInfo.animAttachNo_ = MV1AttachAnim(player_->GetPlayerModel(), playerInfo.animIndex_);
		playerInfo.animTotalTime_ = MV1GetAttachAnimTotalTime(player_->GetPlayerModel(), playerInfo.animAttachNo_);
		playerInfo.currentAnimTime_ = 0.0f;
		MV1SetAttachAnimTime(player_->GetPlayerModel(), playerInfo.animAttachNo_, playerInfo.currentAnimTime_);

		//ロック指定されたらフラグON
		playerInfo.isAnimLock_ = lock;

		//　攻撃の切り替えだったら
		if (AttackActive())
		{
			// 攻撃に切り替わったタイミングで全部の敵の当たり判定をリセット
			for (auto& enemy : enemymng_->GetEnemyPtrs())
			{
				enemy->SetIsAssign(false);
			}
		}
	}
}

void AnimationManager::EnemyChangeAnimation(Enemy* enemy, int idx, bool lock)
{
	auto anim = enemy->GetAnimInfo();  // anim は AnimationInfo*

	if (anim->animIndex_ != idx)
	{
		MV1DetachAnim(enemy->GetModel(), anim->animAttachNo_);
		anim->animIndex_ = idx;
		anim->animAttachNo_ = MV1AttachAnim(enemy->GetModel(), anim->animIndex_);
		anim->animTotalTime_ = MV1GetAttachAnimTotalTime(enemy->GetModel(), anim->animAttachNo_);

		anim->currentAnimTime_ = 0.0f;

		anim->isAnimLock_ = lock;

		MV1SetAttachAnimTime(enemy->GetModel(), anim->animAttachNo_, anim->currentAnimTime_);
	}
	MV1SetAttachAnimTime(enemy->GetModel(), anim->animAttachNo_, anim->currentAnimTime_);
}

// ダメージを受けたアニメーション再生中か
bool AnimationManager::IsImpactAnimation(void)
{
	return playerInfo.animIndex_ == Impact && playerInfo.currentAnimTime_ < playerInfo.animTotalTime_;
}

// 攻撃アニメーションか
bool AnimationManager::AttackActive(void)
{
	if (playerInfo.animIndex_ == First_Attack || playerInfo.animIndex_ == Second_Attack
		|| playerInfo.animIndex_ == Third_Attack || playerInfo.animIndex_ == Force_Attack)
	{
		return true;
	}
	return false;
}

//　アニメーション進行
void AnimationManager::AdvanceAnimation(float speed, std::function<void()> onEnd, std::function<void()> onUpdate, bool isLoop)
{
	playerInfo.currentAnimTime_ += speed;

	if (isLoop)
	{
		if (playerInfo.currentAnimTime_ > playerInfo.animTotalTime_)
		{
			playerInfo.currentAnimTime_ = 0.0f;
		}
		playerInfo.isAnimLock_ = false;
		return;
	}

	if (playerInfo.currentAnimTime_ >= playerInfo.animTotalTime_)
	{
		playerInfo.currentAnimTime_ = playerInfo.animTotalTime_;
		playerInfo.isAnimLock_ = false;

		if (onEnd) onEnd();
	}
	else
	{
		if (onUpdate)onUpdate();
	}
}