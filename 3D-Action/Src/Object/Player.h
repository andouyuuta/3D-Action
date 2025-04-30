#pragma once
#include <DxLib.h>
#include <vector>

class Player
{
public:
	// �ړ���
	static constexpr float MOVE_SPEED_STOP = 0.0f;		// ���Ⴊ��&�~�܂�
	static constexpr float MOVE_SPEED_WALK = 2.5f;		// ����
	static constexpr float MOVE_SPEED_RUN = 8.0f;		// ����

	// �������f���␳�p�x
	static constexpr VECTOR INIT_MODEL_ROT_OFFSET = { 0.0f,DX_PI_F / 2.0f,0.0f };

	// �����Ă������
	static constexpr VECTOR INIT_DIR = { 0.0f,0.0f,-1.0f };

	// ����p
	static constexpr float VIEW_ANGLE = 30.0f;

	// �A�j���V�����̍Đ����x
	static constexpr float ANIM_SPEED = 0.5f;			//�����Ă��鎞
	static constexpr float CROUCH_ANIM_SPEED = 1.5f;	//���Ⴊ��ł���Ƃ�
	static constexpr float CROUCH_ATTACK_SPEED = 1.0f;	//���Ⴊ�ݍU��
	static constexpr float DEAD_ANIM_SPEED = 0.2f;		//����ł��鎞

	static constexpr int NoWeaponIdle = 0;				//����Ȃ��ҋ@
	static constexpr int NoWeaponWalk = 1;				//����Ȃ�����
	static constexpr int NoWeaponRun = 2;				//����Ȃ�����
	static constexpr int WeaponOut = 3;					//������o��	
	static constexpr int Sheach = 4;					//����߂�
	static constexpr int WeaponIdle = 5;				//���킠��ҋ@
	static constexpr int WeaponWalk = 6;				//���킠�����
	static constexpr int WeaponRun = 7;					//���킠�葖��
	static constexpr int Strafe = 8;					//������
	static constexpr int Jump = 9;						//�W�����v
	static constexpr int Crouch = 10;					//���Ⴊ��
	static constexpr int Crouch_Cancel = 11;			//���Ⴊ�݉���
	static constexpr int Crouch_Idle = 12;				//���Ⴊ�ݑҋ@
	static constexpr int Crouch_Guard = 13;				//���Ⴊ�ݖh��
	static constexpr int Crouch_Attack = 14;			//���Ⴊ�ݍU��
	static constexpr int Attack_1 = 15;					//�U���P
	static constexpr int Attack_2 = 16;					//�U���Q
	static constexpr int Attack_3 = 17;					//�U���R
	static constexpr int Attack_4 = 18;					//�U���S
	static constexpr int Death = 19;					//���S

	// �v���C���[�̏��̍\����
	struct Info
	{
		int modelid_;			// ���f��ID

		VECTOR pos_;			// ���W
		VECTOR rot_;			// ��]�l
		VECTOR scale_;			// �傫��
		float moveSpeed_;		// �ړ��X�s�[�h
		int moveDir_;			// �ړ�����
		int moveKind_;			// �v���C���[�̈ړ����

		// �ړ��x�N�g��
		VECTOR moveVec_;		// �ړ��x�N�g��
		VECTOR oldMoveVec_;		// ���O�̈ړ��x�N�g��
		VECTOR moveVecRad_;		// �ړ��x�N�g������������p�x

		VECTOR localRot_;		// �����p�����p�x

		//�A�j���[�V����
		int animindex_;			//�A�j���[�V�����̎��
		int  animAttachNo_;		// �ݒ肷��A�j���[�V�����̔ԍ�
		float animTotalTime_;	// �A�j���[�V�����̑��Đ�����
		float currentAnimTime_; // ���O�̃A�j���[�V�����̎���
		bool animlockflg_;		//�A�j���[�V�������b�N

		bool isdead_;			//���S�������ǂ���
		bool crouchflg_;		//���Ⴊ��ł��邩
		bool crouchattackflg_;	//���Ⴊ�ݍU�����Ă��邩
		bool weaponflg_;		//����o���Ă��邩
		bool attackflg_;		//�U�����Ă��邩
		bool comboflg_;	//�R���{���Ă��邩
		int combostep_;			//�R���{�i�K

		float currentratio_;	//���݂̃A�j���[�V�����̍Đ�����(����)
		float remainingtime_;	//�A�j���[�V���������Ƃǂꂭ�炢�ŏI��邩

	};

	Player(void);		// �R���X�g���N�^
	~Player(void);		// �f�X�g���N�^

	void SystemInit(void);	// ������
	void GameInit(void);
	void Update(void);	// �X�V
	void UpdateMove(void);
	void Draw(void);	// �`��
	void Release(void); // ���

	// �x�N�g���̈ړ�����
	bool IsMove(VECTOR _moveVec);

	void SetRotation(void);

	[[nodiscard]] VECTOR GetPlayerPos(void) { return list.pos_; }

private:
	// �\���̂��i�[����ϐ�
	struct Info list;

	void PlayerMove(int idle, int walk, int run);		//�v���C���[�̓���
	bool CrouchUpdate(void);							//���Ⴊ��
	bool AttackUpdate(void);							//�U��
	bool AttackCombo(int nowcombo, int nextanimidx, int nextstep, float reception, float remainingtime);
	void PlayAnimation(void);							//�A�j���[�V�����Đ�
	void DebugAnimation(void);							//�����L�[�ŃA�j���[�V�����؂�ւ�
	void ChangeAnimation(int idx, bool lock = false);	//�A�j���[�V�����؂�ւ�
};

