#pragma once
#include <DxLib.h>
#include <functional>
#include <unordered_map>

class Player;
class Enemy;
class EnemyManager;

class AnimationManager
{
public:
	AnimationManager(void);
	~AnimationManager(void);

	void Init(Player* player, EnemyManager* enemymng);
	void Update(void);
	void Release(void);
	enum PlayerAnim
	{
		NoWeaponIdle = 0,		// ����Ȃ��ҋ@
		NoWeaponWalk,			// ����Ȃ�����
		NoWeaponRun,			// ����Ȃ�����
		WeaponOut,				// ������o��	
		Sheach,					// ����߂�
		WeaponIdle,				// ���킠��ҋ@
		WeaponWalk,				// ���킠�����
		WeaponRun,				// ���킠�葖��
		Strafe,					// ������
		Jump,					// �W�����v
		Crouch,					// ���Ⴊ��
		Crouch_Cancel,			// ���Ⴊ�݉���
		Crouch_Idle,			// ���Ⴊ�ݑҋ@
		Crouch_Guard,			// ���Ⴊ�ݖh��
		Crouch_Attack,
		First_Attack,			// �U���P
		Force_Attack,			// �U���S
		Second_Attack,			// �U���Q
		Third_Attack,			// �U���R
		Death,					// ���S
		Impact,					// �U���󂯂��Ƃ�
		Revive					// ����
	};

	enum EnemyAnim
	{
		ANIM_IDLE = 0,
		ANIM_WALK,
		ANIM_RUN,
		ANIM_ATTACK,
		ANIM_DEAD,
		ANIM_JUMP_ATTACK,
		ANIM_SWIP,
	};

	struct AnimationInfo
	{
		//�A�j���[�V����
		int animIndex_;			//�A�j���[�V�����̎��
		int  animAttachNo_;		// �ݒ肷��A�j���[�V�����̔ԍ�
		float animTotalTime_;	// �A�j���[�V�����̑��Đ�����
		float currentAnimTime_; // ���O�̃A�j���[�V�����̎���
		float currentRatio_;	//���݂̃A�j���[�V�����̍Đ�����(����)
		float remainingTime_;	//�A�j���[�V���������Ƃǂꂭ�炢�ŏI��邩
		bool isAnimLock_;		//�A�j���[�V�������b�N
	};

	std::unordered_map<Enemy*, AnimationInfo>enemyAnimInfos_;

	void PlayerAnimationPlayback(void);					// �v���C���[�A�j���[�V�����Đ�
	void EnemyAnimationPlayback(void);
	void PlayerChangeAnimation(int idx, bool lock = false);	// �A�j���[�V�����؂�ւ�
	void EnemyChangeAnimation(Enemy* enemy,int idx, bool lock = false);
	bool PlayerAttackCombo(int nowcombo, int nextanimidx, int nextstep, float reception, float remainingtime);	// �v���C���[�U���Đ�(���̃R���{���A���̃R���{�A���̃X�e�b�v�A�����w��A�A�j���[�V�����̎c�莞��(����))
	bool IsImpactAnimation(void);
	AnimationInfo GetPlayerInfo(void) { return playerInfo; }
	AnimationInfo& GetEnemyInfo(void) { return enemyInfo; }

	void SetAnimLock(bool lock) { playerInfo.isAnimLock_ = lock; }
private:
	bool AttackActive(void);				// �U���A�j���[�V�������ǂ���
	void AdvanceAnimation(float speed, std::function<void()> onEnd, std::function<void()> onUpdate, bool isLoop = false);
	AnimationInfo playerInfo;
	AnimationInfo enemyInfo;

	EnemyManager* enemymng_;
	Player* player_;
};

