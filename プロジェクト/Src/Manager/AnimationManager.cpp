#include "AnimationManager.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "RandomManager.h"
#include "../Object/Player.h"
#include "../Object/EnemyManager.h"
#include "../Object/Enemy.h"

namespace PlayerAnimSpeed
{
	// �A�j���V�����̍Đ����x
	const float ANIM_SPEED = 0.5f;			//�����Ă��鎞
	const float CROUCH_ANIM_SPEED = 1.5f;	//���Ⴊ��ł���Ƃ�
	const float CROUCH_ATTACK_SPEED = 1.0f;	//���Ⴊ�ݍU��
	const float DEAD_ANIM_SPEED = 0.2f;		//����ł��鎞
	const float ATTACK_ANIM_SPEED = 0.85f;	//�U�����Ă���
}

namespace EnemyAnimSpeed
{
	// �A�j���[�V�����̍Đ����x
	const float ANIM_SPEED = 1.0f;					// �����Ă��鎞
	const float ATTACK_ANIM_SPEED = 0.5f;			// �U����
	const float DEAD_ANIM_SPEED = 0.2f;				// ����ł��鎞
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
	// �v���C���[�̃A�j���[�V�������
	if (player_ && playerInfo.animAttachNo_ != -1)
	{
		MV1DetachAnim(player_->GetPlayerModel(), playerInfo.animAttachNo_);
		playerInfo.animAttachNo_ = -1;
	}

	// �G�̃A�j���[�V�������
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

// �v���C���[�U���Đ�(���̃R���{���A���̃R���{�A���̃X�e�b�v�A�����w��A�A�j���[�V�����̎c�莞��(����))
bool AnimationManager::PlayerAttackCombo(int nowcombo, int nextanimidx, int nextstep, float reception, float remainingtime)
{
	InputManager& ins = InputManager::GetInstance();

	//�U���P���ɃA�j���[�V�����������ȏ�i��ł�����U���Q
	if (player_->GetInfo().isAttack_ && player_->GetInfo().comboStep_ == nowcombo)
	{
		//���݂̃A�j���[�V��������(����)
		playerInfo.currentRatio_ = playerInfo.currentAnimTime_ / playerInfo.animTotalTime_;

		//�A�j���[�V�������w��̊����ȏ�{���N���b�N�������Ƃ�
		if (playerInfo.currentRatio_ >= reception && (ins.IsTrgMouseLeft()||ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1,InputManager::JOYPAD_BTN::TOP)))
		{
			player_->SetIsCombo(true);
		}

		// �c�莞�Ԃ����ȉ��{�R���{�\�Ȃ玟�̃A�j���[�V������
		playerInfo.remainingTime_ = playerInfo.animTotalTime_ - playerInfo.currentAnimTime_;
		if (playerInfo.remainingTime_ <= remainingtime && player_->GetInfo().isCombo_)
		{
			// ���̍U���A�j���[�V������
			player_->SetComboStep(nextstep);
			player_->SetIsCombo(false);
			PlayerChangeAnimation(nextanimidx, true);
			return true;
		}
	}
	return false;
}

// �v���C���[�̃A�j���[�V�����Đ�
void AnimationManager::PlayerAnimationPlayback(void)
{
	switch (playerInfo.animIndex_)	// �A�j���[�V�����ԍ�
	{
		//���[�v������
	case NoWeaponIdle:			//����Ȃ��ҋ@
	case NoWeaponWalk:			//����Ȃ�����
	case NoWeaponRun:			//����Ȃ�����
	case WeaponIdle:			//���킠��ҋ@
	case WeaponWalk:			//���킠�����
	case WeaponRun:				//���킠�葖��
	case Strafe:				//������
	case Crouch_Idle:			//���Ⴊ�ݑҋ@	
		AdvanceAnimation(PlayerAnimSpeed::ANIM_SPEED, []() {}, []() {}, true);
		break;
		//���[�v�����Ȃ�
	case WeaponOut:			//������o��
	case Sheach:			//����߂�
		AdvanceAnimation(PlayerAnimSpeed::ANIM_SPEED, []() {}, []() {});
		break;
	case Jump:				//�W�����v
		AdvanceAnimation(PlayerAnimSpeed::ATTACK_ANIM_SPEED, []() {}, []() {});
		break;
	case First_Attack:		//�U���P
	case Second_Attack:		//�U���Q
	case Third_Attack:		//�U���R
	case Force_Attack:		//�U���S
		AdvanceAnimation(PlayerAnimSpeed::ATTACK_ANIM_SPEED, [this]() {
			player_->SetIsAttack(false);
			player_->SetComboStep(0);
			player_->SetIsCombo(false);
			}, []() {});
		break;
	case Impact:		//�U���󂯂��Ƃ�
		AdvanceAnimation(PlayerAnimSpeed::ATTACK_ANIM_SPEED, [this]() {
			// �U��/�R���{�t���O�̃N���A
			player_->SetIsImpact(false);
			player_->SetIsAttack(false);
			player_->SetComboStep(0);
			player_->SetIsCombo(false);
			}, [this]() {
				player_->SetIsImpact(true);
			});
		break;
	case Death:		//���S
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
	case Crouch:		//���Ⴊ��(�h��)
	case Crouch_Cancel:		//���Ⴊ�݉���
	case Crouch_Guard:		//�h�䂵���Ƃ�
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
			// ���[�v������
		case ANIM_IDLE:
		case ANIM_WALK:
		case ANIM_RUN:
			anim->currentAnimTime_ += EnemyAnimSpeed::ANIM_SPEED;
			if (anim->currentAnimTime_ > anim->animTotalTime_)
			{
				anim->currentAnimTime_ = 0.0f;
			}
			break;
			// ���[�v�����Ȃ�
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

// �A�j���[�V�����؂�ւ�
void AnimationManager::PlayerChangeAnimation(int idx, bool lock)
{
	// �ԍ����������ύX����
	if (playerInfo.animIndex_ != idx) {
		MV1DetachAnim(player_->GetPlayerModel(), playerInfo.animAttachNo_);										// �A�j���[�V�����폜
		playerInfo.animIndex_ = idx;																	// �w�肳�ꂽ�A�j���[�V��������
		// �A�j���[�V�������Đݒ�
		playerInfo.animAttachNo_ = MV1AttachAnim(player_->GetPlayerModel(), playerInfo.animIndex_);
		playerInfo.animTotalTime_ = MV1GetAttachAnimTotalTime(player_->GetPlayerModel(), playerInfo.animAttachNo_);
		playerInfo.currentAnimTime_ = 0.0f;
		MV1SetAttachAnimTime(player_->GetPlayerModel(), playerInfo.animAttachNo_, playerInfo.currentAnimTime_);

		//���b�N�w�肳�ꂽ��t���OON
		playerInfo.isAnimLock_ = lock;

		//�@�U���̐؂�ւ���������
		if (AttackActive())
		{
			// �U���ɐ؂�ւ�����^�C�~���O�őS���̓G�̓����蔻������Z�b�g
			for (auto& enemy : enemymng_->GetEnemyPtrs())
			{
				enemy->SetIsAssign(false);
			}
		}
	}
}

void AnimationManager::EnemyChangeAnimation(Enemy* enemy, int idx, bool lock)
{
	auto anim = enemy->GetAnimInfo();  // anim �� AnimationInfo*

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

// �_���[�W���󂯂��A�j���[�V�����Đ�����
bool AnimationManager::IsImpactAnimation(void)
{
	return playerInfo.animIndex_ == Impact && playerInfo.currentAnimTime_ < playerInfo.animTotalTime_;
}

// �U���A�j���[�V������
bool AnimationManager::AttackActive(void)
{
	if (playerInfo.animIndex_ == First_Attack || playerInfo.animIndex_ == Second_Attack
		|| playerInfo.animIndex_ == Third_Attack || playerInfo.animIndex_ == Force_Attack)
	{
		return true;
	}
	return false;
}

//�@�A�j���[�V�����i�s
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