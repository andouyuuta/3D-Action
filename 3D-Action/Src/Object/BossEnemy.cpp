#include "BossEnemy.h"
#include "../Application.h"
#include "../Manager/InputManager.h"

BossEnemy::BossEnemy(void)
{
	list.modelid_=-1;

	VECTOR scale_ = { 1.0f,1.0f,1.0f };
	VECTOR pos_ = { 0.0f,0.0f,0.0f };
	VECTOR rot_ = { 0.0f,0.0f,0.0f };
	float moveSpeed_ = 0.0f;
	int moveDir_ = 0;
	int moveKind_ = 0;

	VECTOR moveVec_ = { 0.0f,0.0f,0.0f };
	VECTOR oldMoveVec_ = { 0.0f,0.0f,0.0f };
	VECTOR moveVecRad_ = { 0.0f,0.0f,0.0f };

	VECTOR localRot_ = { 0.0f,0.0f,0.0f };
	int animindex_ = 0;
	int  animAttachNo_ = 0;
	float animTotalTime_ = 0.0f;
	float currentAnimTime_ = 0.0f;
	bool animlockflg_ = false;

	bool deadflg_ = false;
}

BossEnemy::~BossEnemy(void)
{
}

// ������
void BossEnemy::Init(void)
{
	//���f��
	list.modelid_ = MV1LoadModel((Application::PATH_MODEL + "BossEnemyModel.mv1").c_str());
	//���W
	list.pos_ = { 0.0f,0.0f,50.0f };
	MV1SetPosition(list.modelid_, list.pos_);
	//�傫��
	list.scale_ = { 1.0f,1.0f,1.0f };
	MV1SetScale(list.modelid_, list.scale_);
	//��]
	list.rot_ = { 0.0f,0.0f,0.0f };
	MV1SetRotationXYZ(list.modelid_, list.rot_);

	//�A�j���[�V�����֘A
	list.animindex_ = 0;
	list.animAttachNo_ = MV1AttachAnim(list.modelid_, list.animindex_);
	list.animTotalTime_ = MV1GetAttachAnimTotalTime(list.modelid_, list.animAttachNo_);
	list.currentAnimTime_ = 0.0f;
	list.animlockflg_ = false;
	MV1SetAttachAnimTime(list.modelid_, list.animAttachNo_, list.currentAnimTime_);

	list.moveKind_ = 0;

	list.deadflg_ = false;
}

// �X�V
void BossEnemy::Update(void)
{
	PlayAnimation();
	DebugAnimation();
}

// �`��
void BossEnemy::Draw(void)
{
	MV1DrawModel(list.modelid_);
}

// ���
void BossEnemy::Release(void)
{
	MV1DeleteModel(list.modelid_);
}

//�ړ�
void BossEnemy::UpdateMove(void)
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
void BossEnemy::PlayAnimation(void)
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
	case 3:			//�p���`
	case 4:			//��Ԃ�̍U��
		list.currentAnimTime_ += ANIM_SPEED;
		if (list.currentAnimTime_ >= list.animTotalTime_)
		{
			list.currentAnimTime_ = list.animTotalTime_;
			list.animlockflg_ = false;
		}
		break;
	case 5:			//���S
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
void BossEnemy::DebugAnimation(void)
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
	if (ins.IsTrgDown(KEY_INPUT_5)) {
		ChangeAnimation(5);
	}
}

//�A�j���[�V�����؂�ւ�
void BossEnemy::ChangeAnimation(int idx)
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
