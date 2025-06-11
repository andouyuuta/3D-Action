#pragma once
#include <DxLib.h>

class Enemy
{
public:
	// �G�̈ړ���ԊǗ��p
	enum EnemyAnim {
		ANIM_IDLE = 0,
		ANIM_WALK,
		ANIM_RUN,
		ANIM_ATTACK,
		ANIM_DEAD,
		ANIM_JUMP_ATTACK,
		ANIM_SWIP,
	};

	// �G�̍ő�HP
	int ENEMY_MAX_HP = 300;

	// �G�̍ő�U����
	int AttackPower_ = 40;

	// �G�̏��̍\����
	struct Info
	{
		int modelId_;			// ���f��ID

		VECTOR scale_;			// �傫��
		VECTOR pos_;			// ���W
		VECTOR nextPos_;		// ���̍��W
		VECTOR rot_;			// ��]�l
		float moveSpeed_;		// �ړ��X�s�[�h
		int moveDir_;			// �ړ�����
		int moveKind_;			// �v���C���[�̈ړ����
		int hp_;				// HP
		int maxHp_;				// �ő�HP
		int attackPower_;		// �U����

		// �ړ��x�N�g��
		VECTOR moveVec_;		// �ړ��x�N�g��
		VECTOR oldMoveVec_;		// ���O�̈ړ��x�N�g��
		VECTOR moveVecRad_;		// �ړ��x�N�g������������p�x
		VECTOR localRot_;		// �����p�����p�x

		//�A�j���[�V����
		int animIndex_;			// �A�j���[�V�����̎��
		int  animAttachNo_;		// �ݒ肷��A�j���[�V�����̔ԍ�
		float animTotalTime_;	// �A�j���[�V�����̑��Đ�����
		float currentAnimTime_; // ���O�̃A�j���[�V�����̎���
		bool isAnimLock_;		// �A�j���[�V�������b�N

		bool isDead_;			// ����ł��邩
		bool isDraw_;			// �\�����邩
		bool isHitboxActive_;	// �����蔻�肪�L�����Ă��邩
		bool isAttack_;			// �U�����Ă��邩

		//�{�X�֘A
		bool isBoss_;			// �{�X���ǂ���

		float attackRange_;		// �U���\����
		float attackCooldown_;	// �U���N�[���_�E��

		bool isAssign_;			// ���Ă�ꂽ��
	};

	Enemy(void);				// �R���X�g���N�^
	virtual ~Enemy(void);		// �f�X�g���N�^

	virtual void Init(int org, int hp, int attack);	// ������
	virtual void Update();		// �X�V
	virtual void Draw(void);	// �`��
	virtual void Release(void); // ���

	// �ǐՏ���
	void UpdateMove(void);

	// ���S�t���O
	bool IsDead()const;

	// �G�̌��݈ʒu�̎擾����i�O������ǂݎ���p�j
	[[nodiscard]] int GetModel(void)const { return list.modelId_; }
	[[nodiscard]] VECTOR GetPosition(void)const { return list.pos_; }
	[[nodiscard]] float GetRotY(void)const { return list.rot_.y; }
	[[nodiscard]] int GetHP(void)const { return list.hp_; }
	[[nodiscard]] VECTOR GetRightHandPosition(void);
	[[nodiscard]] VECTOR GetLeftHandPosition(void);
	[[nodiscard]] bool GetAttackFlg(void) { return list.isAttack_; }
	struct Info GetInfo(void) { return list; }
	bool GetIsBoss(void)const { return list.isBoss_; }
	int GetAttackPower(void)const { return list.attackPower_; }	// �G�̍U����

	// �Z�b�^�[�֐�
	void SetEnemyPos(const VECTOR& pos);		// �G�̍��W�̐ݒ�
	void SetDamage(int dp);						// �G��HP�����炷
	void SetIsBoss(bool boss) { list.isBoss_ = boss; }

	void SetIsAssign(bool flg) { list.isAssign_ = flg; }
	bool GetIsAssign(void) { return list.isAssign_; }
protected:
	// �\���̂��i�[����ϐ�
	struct Info list;

	// �ÓI�C���X�^���X
	static Enemy* instance_;

	virtual void PlayAnimation(void);		//�A�j���[�V�����Đ�
	//void DebugAnimation(void);		//�����L�[�ŃA�j���[�V�����؂�ւ�
	virtual void ChangeAnimation(int idx, bool lock = false);	//�A�j���[�V�����؂�ւ�

};

