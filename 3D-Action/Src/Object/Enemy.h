#pragma once
#include <DxLib.h>
#include <vector>

class Enemy
{
public:
	// �ړ���
	static constexpr float MOVE_SPEED_WALK = 2.5f;		// ����
	static constexpr float MOVE_SPEED_RUN = 8.0f;		// ����
	static constexpr float MOVE_SPEED_STOP = 0.0f;

	// �������f���␳�p�x
	static constexpr VECTOR INIT_MODEL_ROT_OFFSET = { 0.0f,DX_PI_F / 2.0f,0.0f };

	// �����Ă������
	static constexpr VECTOR INIT_DIR = { 0.0f,0.0f,-1.0f };

	// ����p
	static constexpr float VIEW_ANGLE = 30.0f;

	// �A�j���[�V�����̍Đ����x
	static constexpr float ANIM_SPEED = 0.5f;		// �����Ă��鎞
	static constexpr float DEAD_ANIM_SPEED = 0.2f;	// ����ł��鎞

	// �G�̈ړ���ԊǗ��p
	enum EnemyAnim {
		ANIM_IDLE = 0,
		ANIM_WALK,
		ANIM_RUN,
		ANIM_ATTACK,
		ANIM_DEAD
	};

	// �G�̍ő�HP
	static constexpr int ENEMY_MAX_HP = 10;

	// �G�̏��̍\����
	struct Info
	{
		int modelid_;			//���f��ID

		VECTOR scale_;			//�傫��
		VECTOR pos_;			//���W
		VECTOR rot_;			//��]�l
		float moveSpeed_;		//�ړ��X�s�[�h
		int moveDir_;			//�ړ�����
		int moveKind_;			//�v���C���[�̈ړ����
		int hp_;				// HP

		// �ړ��x�N�g��
		VECTOR moveVec_;		//�ړ��x�N�g��
		VECTOR oldMoveVec_;		//���O�̈ړ��x�N�g��
		VECTOR moveVecRad_;		//�ړ��x�N�g������������p�x

		VECTOR localRot_;		//�����p�����p�x

		//�A�j���[�V����
		int animindex_;			//�A�j���[�V�����̎��
		int  animAttachNo_;		//�ݒ肷��A�j���[�V�����̔ԍ�
		float animTotalTime_;	//�A�j���[�V�����̑��Đ�����
		float currentAnimTime_; //���O�̃A�j���[�V�����̎���
		bool animlockflg_;		//�A�j���[�V�������b�N

		bool deadflg_;			//����ł��邩
		bool drawflg_;			//�\�����邩
		bool isHitboxActive_;	// �����蔻�肪�L�����Ă��邩
		bool hasHitPlayer_;		// �U�����ň�x�����q�b�g���Ȃ��悤�ɂ���t���O
		bool attackflg_;		// �U�����Ă��邩
	};

	Enemy(void);		// �R���X�g���N�^
	~Enemy(void);		// �f�X�g���N�^

	void Init(int org);	// ������
	void Update();	// �X�V
	void Draw(void);	// �`��
	void Release(void); // ���

	// �ǐՏ���
	void UpdateMove(void);

	// ���S�t���O
	bool IsDead()const;

	// �Z�b�^�[�֐�
	void SetEnemyPos(const VECTOR& pos);		// �G�̍��W�̐ݒ�

	// �Q�b�^�[�֐�
	struct Info GetInfo(void) { return list; }

	// �G�̌��݈ʒu�̎擾����i�O������ǂݎ���p�j
	[[nodiscard]] int GetModel()const { return list.modelid_; }
	[[nodiscard]] VECTOR GetPosition()const { return list.pos_; }
	[[nodiscard]] float GetRotY()const { return list.rot_.y; }
	[[nodiscard]] int GetHP()const { list.hp_; }
	[[nodiscard]] int GetRightHandIndex(void) { return MV1SearchFrame(list.modelid_, "mixamorig:RightHand"); }
	[[nodiscard]] VECTOR GetRightHandPosition(void) { return MV1GetFramePosition(list.modelid_, GetRightHandIndex()); }
	[[nodiscard]] bool GetAttackFlg(void) { return list.attackflg_; }
	// �G��HP�����炷
	void SetDamage(int dp);
private:
	// �\���̂��i�[����ϐ�
	struct Info list;

	void PlayAnimation(void);		//�A�j���[�V�����Đ�
	void DebugAnimation(void);		//�����L�[�ŃA�j���[�V�����؂�ւ�
	void ChangeAnimation(int idx, bool lock = false);	//�A�j���[�V�����؂�ւ�
};

