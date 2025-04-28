#pragma once
#include <DxLib.h>
#include <vector>
class BossEnemy
{
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
	enum class MODE_ID
	{
		NONE,
		WALK,		// ����
		RUN,		// ����
	};

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
	};

	BossEnemy(void);		// �R���X�g���N�^
	~BossEnemy(void);		// �f�X�g���N�^

	void Init(void);	// ������
	void Update(void);	// �X�V
	void Draw(void);	// �`��
	void Release(void); // ���


	// �f�o�b�N�̕`��
	void DrawDebug(void);

	// �x�N�g���̈ړ�����
	bool IsMove();

private:
	// �\���̂��i�[����ϐ�
	struct Info list;


	void UpdateMove(void);			//�ړ�
	void PlayAnimation(void);		//�A�j���[�V�����Đ�
	void DebugAnimation(void);		//�����L�[�ŃA�j���[�V�����؂�ւ�
	void ChangeAnimation(int idx);	//�A�j���[�V�����؂�ւ�
};

