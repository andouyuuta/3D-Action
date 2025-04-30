#include "Enemy.h"
#include "../Application.h"
#include "../Manager/InputManager.h"

namespace
{
	// �ړ���
	const float MOVE_SPEED_WALK = 2.5f;	// ����
	const float MOVE_SPEED_RUN = 8.0f;	// ����
	const float MOVE_SPEED_STOP = 0.0f;

	// �������f���␳�p�x
	const VECTOR INIT_MODEL_ROT_OFFSET = { 0.0f,DX_PI_F / 2.0f,0.0f };

	// �����Ă������
	const VECTOR INIT_DIR = { 0.0f,0.0f,-1.0f };

	// ����p
	const float VIEW_ANGLE = 30.0f;

	// �A�j���[�V�����̍Đ����x
	const float ANIM_SPEED = 0.5f;		// �����Ă��鎞
	const float DEAD_ANIM_SPEED = 0.2f;	// ����ł��鎞
}

Enemy::Enemy(void)
{
	list.modelid_ = -1;
	list.scale_ = VGet(1.0f, 1.0f, 1.0f);
	list.pos_ = VGet(0.0f, 0.0f, 0.0f);
	list.rot_ = VGet(0.0f, 0.0f, 0.0f);

	list.moveSpeed_ = MOVE_SPEED_STOP;
	list.moveDir_ = 0;
	list.moveKind_ = 0;

	list.moveVec_ = VGet(0.0f, 0.0f, 0.0f);
	list.oldMoveVec_ = VGet(0.0f, 0.0f, 0.0f);
	list.moveVEcRad_ = VGet(0.0f, 0.0f, 0.0f);
	list.localRot_ = INIT_MODEL_ROT_OFFSET;

	list.animindex_ = 0;
	list.animAttachNo_ = -1;
	list.animTotalTime_ = 0.0f;
	list.currentAnimTime_ = 0.0f;
	list.animlockflg_ = false;

	list.deadflg_ = false;
}

Enemy::~Enemy(void)
{
}

// ������
void Enemy::Init(int org)
{
	//���f��
	list.modelid_ = MV1DuplicateModel(org);
	//���W
	list.pos_ = { 0.0f,0.0f,50.0f };
	MV1SetPosition(list.modelid_, list.pos_);
	//�傫��
	list.scale_ = { 1.0f,1.0f,1.0f };
	MV1SetScale(list.modelid_, list.scale_);
	//��]
	list.rot_ = { 0.0f,0.0f,0.0f };
	MV1SetRotationXYZ(list.modelid_, list.rot_);

	// �G��HP�̏�����
	list.hp_ = ENEMY_MAX_HP;


	//�A�j���[�V�����֘A
	list.animindex_ = 0;
	list.animAttachNo_ = MV1AttachAnim(list.modelid_, list.animindex_);
	list.animTotalTime_ = MV1GetAttachAnimTotalTime(list.modelid_, list.animAttachNo_);
	list.currentAnimTime_ = 0.0f;
	list.animlockflg_ = false;
	MV1SetAttachAnimTime(list.modelid_, list.animAttachNo_, list.currentAnimTime_);

	list.deadflg_ = false;
}

// �X�V
void Enemy::Update(void)
{
	PlayAnimation();
	DebugAnimation();
}

// �`��
void Enemy::Draw(void)
{
	MV1DrawModel(list.modelid_);
}

// ���
void Enemy::Release(void)
{
	MV1DeleteModel(list.modelid_);
}

// ���S�t���O
bool Enemy::IsDead() const
{
	return list.deadflg_;
}

void Enemy::SetEnemyPos(const VECTOR& pos)
{
	list.pos_ = pos;
	MV1SetPosition(list.modelid_, list.pos_);
}


// �G�̌��݈ʒu�̎擾����i�O������ǂݎ���p�j
VECTOR Enemy::GetPosition() const
{
	return list.pos_;
}

// �G��HP�̎擾
int Enemy::GetHP() const
{
	return list.hp_;
}

// �G��HP�����炷
void Enemy::SetDamage(int dp)
{
	list.hp_ -= dp;
	// HP��0�ȉ��Ȃ玀�S��Ԃֈڍs
	if (list.hp_ <= 0)
	{
		list.hp_ = 0;
		// ���񂾃t���O
		list.deadflg_ = true;
	}
}

//�ړ�
void Enemy::UpdateMove(void)
{
	//// ���͐���擾
	//InputManager& ins = InputManager::GetInstance();

	//if (list.animlockflg_)
	//{
	//	return;
	//}

	////���S���ɂ��ړ������Ȃ�
	//if (list.deadflg_)
	//{
	//	list.moveSpeed_ = MOVE_SPEED_STOP;
	//	ChangeAnimation(19);
	//	return;
	//}

	//// WASD�Ńv���C���[�ړ�
	//list.moveVec_ = { 0.0f, 0.0f, 0.0f };

	//// ���E�E�E��O�E���̃x�N�g�����쐬����
	//VECTOR RIGHT_MOVE_VEC = { 1.0f,  0.0f,  0.0f };
	//VECTOR LEFT_MOVE_VEC = { -1.0f,  0.0f,  0.0f };
	//VECTOR FRONT_MOVE_VEC = { 0.0f,  0.0f,  1.0f };
	//VECTOR BACK_MOVE_VEC = { 0.0f,  0.0f, -1.0f };

	//// ���͕����Ɉړ��x�N�g����ǉ�����
	//if (ins.IsNew(KEY_INPUT_UP)) { list.moveVec_ = VAdd(list.moveVec_, FRONT_MOVE_VEC); }
	//if (ins.IsNew(KEY_INPUT_LEFT)) { list.moveVec_ = VAdd(list.moveVec_, LEFT_MOVE_VEC); }
	//if (ins.IsNew(KEY_INPUT_DOWN)) { list.moveVec_ = VAdd(list.moveVec_, BACK_MOVE_VEC); }
	//if (ins.IsNew(KEY_INPUT_RIGHT)) { list.moveVec_ = VAdd(list.moveVec_, RIGHT_MOVE_VEC); }



	//// �x�N�g���̈ړ����s���Ă�������W�X�V
	//if (IsMove(list.moveVec_))
	//{
	//	// �ړ���Ԃ̐ݒ�
	//	list.moveKind_ = 1;

	//	//// �J�����p�x���ݒ肷��
	//	//VECTOR cameraAngles = SceneManager::GetInstance().GetCamera()->GetCameraAngles();
	//	//MATRIX cameraMatY = MGetRotY(cameraAngles.y);
	//	//list.moveVec_ = VTransform(list.moveVec_, cameraMatY);

	//	// �X�^�~�i���؂�Ă��邩�ǂ���
	//	bool spFlg_ = false;

	//	if (spFlg_)
	//	{
	//		// �X�^�~�i�Ȃ����
	//		list.moveSpeed_ = MOVE_SPEED_STOP;
	//	}
	//	else
	//	{
	//		if (ins.IsNew(KEY_INPUT_LSHIFT))
	//		{
	//			// �_�b�V�����
	//			list.moveSpeed_ = MOVE_SPEED_RUN;
	//			ChangeAnimation(2);
	//		}
	//		else
	//		{
	//			// �������
	//			list.moveSpeed_ = MOVE_SPEED_WALK;
	//			ChangeAnimation(1);
	//		}
	//	}

	//	// ���W�X�V
	//	list.moveVec_ = VNorm(list.moveVec_);
	//	list.moveVec_ = VScale(list.moveVec_, list.moveSpeed_);
	//	list.pos_ = VAdd(list.pos_, list.moveVec_);
	//	list.oldMoveVec_ = list.moveVec_;

	//	// �������p�x�ɕϊ�����( XZ���� Y��)
	//	list.moveVecRad_.y = atan2f(list.moveVec_.x, list.moveVec_.z);

	//	// �V���v���Ɍv�Z�p�x��ݒ肵�Ă݂�
	//	list.rot_.y = list.moveVecRad_.y;

	//	// ���W�ݒ�
	//	MV1SetPosition(list.modelid_, list.pos_);
	//}
	//else
	//{
	//	// �ړ���Ԃ̐ݒ�
	//	list.moveKind_ = 0;

	//	//�A�j���[�V�������b�N����Ă��Ȃ��Ƃ��ҋ@
	//	if (!list.animlockflg_)
	//	{
	//		ChangeAnimation(0);
	//	}
	//}

}

//�A�j���[�V�����Đ�
void Enemy::PlayAnimation(void)
{
	DebugAnimation();			//�����L�[�ŃA�j���[�V�����؂�ւ�

	switch (list.animindex_)
	{
		//���[�v������
	case 0:			//�ҋ@
	case 1:			//����
	case 2:			//����
		list.currentAnimTime_ += ANIM_SPEED;
		if (list.currentAnimTime_ > list.animTotalTime_)
		{
			list.currentAnimTime_ = 0.0f;
		}
		break;
		//���[�v�����Ȃ�
	case 3:			//�U��
		list.currentAnimTime_ += ANIM_SPEED;
		if (list.currentAnimTime_ >= list.animTotalTime_)
		{
			list.currentAnimTime_ = list.animTotalTime_;
			list.animlockflg_ = false;
		}
		break;
	case 4:			//���S
		list.currentAnimTime_ += DEAD_ANIM_SPEED;
		if (list.currentAnimTime_ >= list.animTotalTime_)
		{
			list.currentAnimTime_ = list.animTotalTime_;
			list.animlockflg_ = false;
		}
		break;
	default:
		break;
	}
	MV1SetAttachAnimTime(list.modelid_, list.animAttachNo_, list.currentAnimTime_);
	MV1SetPosition(list.modelid_, list.pos_);
}

//���L�[�ŃA�j���[�V�����؂�ւ�
void Enemy::DebugAnimation(void)
{
	InputManager& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_0)) {
		ChangeAnimation(0);
	}
	if (ins.IsTrgDown(KEY_INPUT_1)) {
		ChangeAnimation(1);
	}
	if (ins.IsTrgDown(KEY_INPUT_2)) {
		ChangeAnimation(2);
	}
	if (ins.IsTrgDown(KEY_INPUT_3)) {
		ChangeAnimation(3);
	}
	if (ins.IsTrgDown(KEY_INPUT_4)) {
		ChangeAnimation(4);
	}
}

//�A�j���[�V�����؂�ւ�
void Enemy::ChangeAnimation(int idx)
{
	if (list.animindex_ != idx) {
		MV1DetachAnim(list.modelid_, list.animAttachNo_);
		list.animindex_ = idx;
		list.animAttachNo_ = MV1AttachAnim(list.modelid_, list.animindex_);
		list.animTotalTime_ = MV1GetAttachAnimTotalTime(list.modelid_, list.animAttachNo_);
		list.currentAnimTime_ = 0.0f;
		MV1SetAttachAnimTime(list.modelid_, list.animAttachNo_, list.currentAnimTime_);
	}
}
