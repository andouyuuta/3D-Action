#pragma once
#include <DxLib.h>
#include "../Manager/AnimationManager.h"

class Player;

class Enemy
{
public:
	static constexpr float WALK_START = 0.05f;	// �����n�߂锻��
	static constexpr float WALK_STOP = 0.02f;	// �~�܂�������
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

	virtual void Init(int org, int hp, int attack, Player* player, AnimationManager* anim, AnimationManager::AnimationInfo* animinfo);	// ������
	virtual void Update(void);		// �X�V
	virtual void Draw(void);	// �`��
	virtual void Release(void); // ���

	// �ǐՏ���
	void UpdateMove(void);

	// ���S�t���O
	bool IsDead(void)const;

	// �G�̌��݈ʒu�̎擾����i�O������ǂݎ���p�j
	[[nodiscard]] int GetModel(void)const { return list.modelId_; }
	[[nodiscard]] VECTOR GetPosition(void)const { return list.pos_; }
	[[nodiscard]] float GetRotY(void)const { return list.rot_.y; }
	[[nodiscard]] int GetHP(void)const { return list.hp_; }
	[[nodiscard]] VECTOR GetRightHandPosition(void);
	[[nodiscard]] VECTOR GetLeftHandPosition(void);
	[[nodiscard]] bool GetAttackFlg(void) { return list.isAttack_; }
	//struct Info GetInfo(void) { return list; }
	[[nodiscard]] bool GetIsBoss(void)const { return list.isBoss_; }
	[[nodiscard]] int GetAttackPower(void)const { return list.attackPower_; }	// �G�̍U����
	[[nodiscard]] bool GetIsAssign(void) { return list.isAssign_; }

	// �Z�b�^�[�֐�
	void SetEnemyPos(const VECTOR& pos);		// �G�̍��W�̐ݒ�
	void SetDamage(int dp);						// �G��HP�����炷
	void SetIsBoss(bool boss) { list.isBoss_ = boss; }
	void SetIsAttack(bool flg) { list.isAttack_ = flg; }

	void SetIsAssign(bool flg) { list.isAssign_ = flg; }

	AnimationManager::AnimationInfo* GetAnimInfo() { return animInfo_; }
	void SetAnimInfo(AnimationManager::AnimationInfo* info) { animInfo_ = info; }
protected:
	void DrawDebug(void);

	// �\���̂��i�[����ϐ�
	struct Info list;

	// �ÓI�C���X�^���X
	static Enemy* instance_;

	Player* player_;
	AnimationManager* animation_;
	AnimationManager::AnimationInfo* animInfo_;
};