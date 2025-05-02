#include <DxLib.h>
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "Player.h"
#include "Sword.h"
#include "Enemy.h"
#include "EnemyManager.h"
#include "Stage.h"

Player* Player::instance_ = nullptr;

void Player::CreateInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new Player();
	}
	instance_->Init();
}

Player& Player::GetInstance(void)
{
	return *instance_;
}


Player::Player(void)
{
	list.modelid_ = -1;

	list. pos_ = { 0.0f,0.0f,0.0f };
	list.rot_ = { 0.0f,0.0f,0.0f };
	list.scale_ = { 0.0f,0.0f,0.0f };
	list.moveSpeed_ = 0.0f;
	list.moveDir_ = 0;
	list.moveKind_ = 0;

	list.moveVec_ = { 0.0f,0.0f,0.0f };
	list.oldMoveVec_ = { 0.0f,0.0f,0.0f };
	list.moveVecRad_ = { 0.0f,0.0f,0.0f };

	list.localRot_ = { 0.0f,0.0f,0.0f };

	list.animindex_ = 0;
	list.animAttachNo_ = 0;
	list.animTotalTime_ = 0.0f;
	list.currentAnimTime_ = 0.0f;
	list.animlockflg_ = false;

	list.isdead_ = false;
	list.crouchflg_ = false;
	list.crouchattackflg_ = false;
	list.weaponflg_ = false;

	damageCooldown_ = 0;
}

Player::~Player(void)
{
}

void Player::Init(void)
{
	// ���f���̓ǂݍ���
	list.modelid_ = MV1LoadModel("Data/Model/PlayerModel.mv1");

	// �v���C���[�̏����ʒu�ݒ�
	list.pos_ = { 0.0f, 0.0f, -550.0f };
	MV1SetPosition(list.modelid_, list.pos_);

	// �v���C���[�̊p�x�ݒ�
	list.rot_ = { 0.0f, 0.0f, 0.0f };
	MV1SetRotationXYZ(list.modelid_, list.rot_);

	// �ړ��x�N�g�����쐬����p�x
	list.moveVecRad_ = { 0.0f, 0.0f, 0.0f };

	// ���O�̈ړ��x�N�g��
	list.oldMoveVec_ = { 0.0f, 0.0f, 0.0f };

	// �v���C���[�̊p�x
	list.localRot_ = INIT_MODEL_ROT_OFFSET;

	// �v���C���[�̈ړ����x
	list.moveSpeed_ = MOVE_SPEED_WALK;

	// �v���C���[�̈ړ����
	list.moveKind_ = 0;

	//����ł��邩
	list.isdead_ = false;

	//���Ⴊ��ł��邩
	list.crouchflg_ = false;

	//���Ⴊ�ݍU�����Ă��邩
	list.crouchattackflg_ = false;		

	//����o���Ă��邩
	list.weaponflg_ = false;

	//�U�����Ă��邩
	list.attackflg_ = false;

	//�U���i�K
	list.comboflg_ = false;
	list.combostep_ = 0;

	// HP�̏�����
	list.hp_ = PLAYER_MAX_HP;

	// �_���[�W�Ԋu����p
	damageCooldown_ = 0;

	//�A�j���[�V�����֘A
	list.animindex_ = 1;
	list.animAttachNo_ = MV1AttachAnim(list.modelid_, list.animindex_);
	list.animTotalTime_ = MV1GetAttachAnimTotalTime(list.modelid_, list.animAttachNo_);
	list.currentAnimTime_ = 0.0f;
	list.animlockflg_ = false;
	MV1SetAttachAnimTime(list.modelid_, list.animAttachNo_, list.currentAnimTime_);

	list.currentratio_ = 0.0f;
	list.remainingtime_ = 0.0f;

	MV1SetupCollInfo(list.modelid_);
}

void Player::Update(const std::vector<Enemy*>& enemies)
{
	// �A�j���[�V��������
 	PlayAnimation();
	//CheckEnemyCollision(enemies);	// �����蔻��
	//DebugAnimation();
	//�ړ�����
	UpdateMove();
	SetRotation();
}

void Player::UpdateMove(void)
{
	// ���͐���擾
	InputManager& ins = InputManager::GetInstance();

	//���S���ɂ��ړ������Ȃ�
	if (list.isdead_)
	{
		list.moveSpeed_ = MOVE_SPEED_STOP;
		ChangeAnimation(Death);
		return;
	}

	if (ins.IsTrgMouseRight())
	{
		list.weaponflg_ = !list.weaponflg_;
		if (list.weaponflg_)
		{
			ChangeAnimation(WeaponOut, true);					//������o��
		}
		else {
			ChangeAnimation(Sheach, true);						//���킵�܂�
		}
		return;
	}

	if (!list.weaponflg_)
	{
		PlayerMove(NoWeaponIdle, NoWeaponWalk, NoWeaponRun);	//���펝���Ă��Ȃ��Ƃ��̓���
	}
	else
	{
		PlayerMove(WeaponIdle, WeaponWalk, WeaponRun);			//���펝���Ă���Ƃ��̓���
	}
}

void Player::Draw(void)
{
	// ���f���̕`��
	MV1DrawModel(list.modelid_);

	//// �v���C���[���W�\��
	//DrawFormatString(20, 120, GetColor(0xff, 0xff, 0xff), "�v���C���[�̍��W : (X, Y, Z) = (% 1.2lf, % 1.2lf, % 1.2lf)", list.pos_.x, list.pos_.y, list.pos_.z);
}

void Player::Release(void)
{
	MV1DeleteModel(list.modelid_);
}

bool Player::IsMove(VECTOR _moveVec)
{
	// XYZ���ׂĂ̍��W�̈ړ��ʂ̐�Βl���Ƃ�
	float absX = abs(_moveVec.x);
	float absY = abs(_moveVec.y);
	float absZ = abs(_moveVec.z);

	// �ړ��x�N�g���� X�EY�EZ ���ׂĈړ�����ĂȂ����
	bool isNoMoveX = absX < FLT_EPSILON;
	bool isNoMoveY = absY < FLT_EPSILON;
	bool isNoMoveZ = absZ < FLT_EPSILON;

	// ���W�X�V�����ɔ�����
	if (isNoMoveX && isNoMoveY && isNoMoveZ)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void Player::SetRotation(void)
{
	//------------------------------------
	// ��]�s����g�p�����p�x�ݒ�
	//------------------------------------
	
	// �P�ʍs���ݒ肷��
	MATRIX mat = MGetIdent();

	// ���f�����̂�Y����]�s����쐬����
	MATRIX mGetRotY = MGetRotY(list.rot_.y);

	// ���f���̕␳�pY����]�s����쐬����
	MATRIX mGetLocalRotY = MGetRotY(list.localRot_.y);

	// �s�������
	mat = MMult(mat, mGetRotY);
	mat = MMult(mat, mGetLocalRotY);

	// �s����g�p���ă��f���̊p�x��ݒ�
	MV1SetRotationMatrix(list.modelid_, mat);
}

// �����蔻��
void Player::CheckEnemyCollision(const std::vector<Enemy*>& enemies)
{
	const float COLLISION_RADIUS = 50.0f;  // ����͈͔��a

	for (auto& enemy : enemies)
	{
		// ���S�ς݂̓G�͖���
		if (enemy->IsDead()) continue;

		VECTOR distVec = VSub(list.pos_, enemy->GetPosition());
		// XZ���ʂ̂�
		distVec.y = 0.0f;

		if (VSize(distVec) < COLLISION_RADIUS)
		{
			// �v���C���[�������߂�
			VECTOR pushDir = VNorm(distVec);
			VECTOR pushBack = VScale(pushDir, COLLISION_RADIUS - VSize(distVec) + 0.1f);
			list.pos_ = VAdd(list.pos_, pushBack);
			MV1SetPosition(list.modelid_, list.pos_);
			list.moveSpeed_ = MOVE_SPEED_STOP;

			// �ҋ@�A�j���[�V�����ɕύX
			ChangeAnimation(0);

			if (damageCooldown_ <= 0)
			{
				// HP�̌���
				list.hp_ -= 10;
				//�N�[���^�C��(60�t���[���Ɉ��)
				damageCooldown_ = 60;

				// HP��0�ȉ������S���Ă��Ȃ���Ύ��S����
				if (list.hp_ <= 0 && !list.isdead_)
				{
					list.isdead_ = true;
					// ���S�A�j���[�V����
					ChangeAnimation(19, true);
					SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAMEOVER);
				}
			}
		}
	}

	if (damageCooldown_ > 0) damageCooldown_--;
}

bool Player::CrouchUpdate(void)
{
	InputManager& ins = InputManager::GetInstance();

	//�h��(���Ⴊ��)���ɂ͈ړ������Ȃ�
	if (ins.IsNew(KEY_INPUT_LCONTROL))
	{
		if (!list.crouchflg_)
		{
			//���Ⴊ�ݏ�Ԃɓ���Ƃ�
			list.crouchflg_ = true;
			list.crouchattackflg_ = false;
			list.moveSpeed_ = MOVE_SPEED_STOP;		//�������~�߂�
			ChangeAnimation(Crouch, true);			//���Ⴊ�ݍĐ�
			return true;
		}

		if (!list.animlockflg_ && !list.crouchattackflg_)
		{
			ChangeAnimation(Crouch_Idle);			//���Ⴊ�ݑҋ@
		}

		//���Ⴊ��ł���Ƃ��ɍ��N���b�N��������
		if (!list.crouchattackflg_ && ins.IsTrgMouseLeft())
		{
			ChangeAnimation(Crouch_Attack);			//���Ⴊ�ݍU��
			list.crouchattackflg_ = true;
		}
		return true;	//���Ⴊ�ݒ�
	}
	else if (list.crouchflg_)
	{
		//���R���g���[���𗣂����Ƃ��ɂ��Ⴊ�݉���
		ChangeAnimation(Crouch_Cancel, true);		//���Ⴊ�݉���
		list.crouchflg_ = false;
		list.crouchattackflg_ = false;
	}
	return false;		//���Ⴊ�ݒ��ł͂Ȃ�
}

bool Player::AttackUpdate(void)
{
	InputManager& ins = InputManager::GetInstance();

	if (!list.attackflg_ && ins.IsTrgMouseLeft())
	{
		list.attackflg_ = true;
		list.combostep_ = 1;
		list.comboflg_ = false;
		ChangeAnimation(Attack_1, true);
		return true;
	}

	if(AttackCombo(1, Attack_3, 2, 0.5f, 5.0f))return true;
	if(AttackCombo(2, Attack_4, 3, 0.5f, 15.0f))return true;
	if(AttackCombo(3, Attack_2, 4, 0.5f, 15.0f))return true;

	//�A�j���[�V�������I�������U������
	if (!list.animlockflg_ && list.attackflg_)
	{
		list.attackflg_ = false;
		list.combostep_ = 0;
		list.comboflg_ = false;
	}

	return list.attackflg_;		//�U������
}

//�U���R���{(���̃R���{�A���̃A�j���[�V�����A���̒i�K�A���͎�t����(��납��̊���)�A�A�j���[�V�����X�L�b�v����(��납��))
bool Player::AttackCombo(int nowcombo, int nextanimidx, int nextstep, float reception, float remainingtime)
{
	InputManager& ins = InputManager::GetInstance();

	//�U���P���ɃA�j���[�V�����������ȏ�i��ł�����U���Q
	if (list.attackflg_ && list.combostep_ == nowcombo)
	{
		//���݂̃A�j���[�V��������(����)
		list.currentratio_ = list.currentAnimTime_ / list.animTotalTime_;

		//�A�j���[�V�����������ȏ�{���N���b�N�������Ƃ�
		if (list.currentratio_ >= reception && ins.IsTrgMouseLeft())
		{
			list.comboflg_ = true;
		}

		//�A�j���[�V�����̎c�莞��
		list.remainingtime_ = list.animTotalTime_ - list.currentAnimTime_;
		if (list.remainingtime_ <= remainingtime && list.comboflg_)
		{
			ChangeAnimation(nextanimidx, true);
			list.combostep_ = nextstep;
			list.comboflg_ = false;
			return true;
		}
	}
	return false;
}

void Player::PlayerMove(int idle, int walk, int run)
{
	InputManager& ins = InputManager::GetInstance();

	if (list.weaponflg_)
	{
		if (CrouchUpdate()) { return; }
		if (AttackUpdate()) { return; }
	}

	// WASD�Ńv���C���[�ړ�
	list.moveVec_ = { 0.0f, 0.0f, 0.0f };

	// ���E�E�E��O�E���̃x�N�g�����쐬����
	VECTOR RIGHT_MOVE_VEC = { 1.0f,  0.0f,  0.0f };
	VECTOR LEFT_MOVE_VEC = { -1.0f,  0.0f,  0.0f };
	VECTOR FRONT_MOVE_VEC = { 0.0f,  0.0f,  1.0f };
	VECTOR BACK_MOVE_VEC = { 0.0f,  0.0f, -1.0f };

	// ���͕����Ɉړ��x�N�g����ǉ�����
	if (ins.IsNew(KEY_INPUT_W)) { list.moveVec_ = VAdd(list.moveVec_, FRONT_MOVE_VEC); }
	if (ins.IsNew(KEY_INPUT_A)) { list.moveVec_ = VAdd(list.moveVec_, LEFT_MOVE_VEC); }
	if (ins.IsNew(KEY_INPUT_S)) { list.moveVec_ = VAdd(list.moveVec_, BACK_MOVE_VEC); }
	if (ins.IsNew(KEY_INPUT_D)) { list.moveVec_ = VAdd(list.moveVec_, RIGHT_MOVE_VEC); }

	// �x�N�g���̈ړ����s���Ă�������W�X�V
	if (IsMove(list.moveVec_))
	{
		// �ړ���Ԃ̐ݒ�
		list.moveKind_ = 1;

		// �J�����p�x���ݒ肷��
		VECTOR cameraAngles = SceneManager::GetInstance().GetCamera()->GetCameraAngles();
		MATRIX cameraMatY = MGetRotY(cameraAngles.y);
		list.moveVec_ = VTransform(list.moveVec_, cameraMatY);

		// �X�^�~�i���؂�Ă��邩�ǂ���
		bool spFlg_ = false;
		if (spFlg_)
		{
			// �X�^�~�i�Ȃ����
			list.moveSpeed_ = MOVE_SPEED_STOP;
		}
		else
		{
			if (ins.IsNew(KEY_INPUT_LSHIFT))
			{
				// �_�b�V�����
				list.moveSpeed_ = MOVE_SPEED_RUN;
				ChangeAnimation(run);
			}
			else
			{
				// �������
				list.moveSpeed_ = MOVE_SPEED_WALK;
				ChangeAnimation(walk);
			}
		}

		// ���W�X�V
		list.moveVec_ = VNorm(list.moveVec_);
		list.moveVec_ = VScale(list.moveVec_, list.moveSpeed_);
		list.pos_ = VAdd(list.pos_, list.moveVec_);
		list.oldMoveVec_ = list.moveVec_;

		// �������p�x�ɕϊ�����( XZ���� Y��)
		list.moveVecRad_.y = atan2f(list.moveVec_.x, list.moveVec_.z);

		// �V���v���Ɍv�Z�p�x��ݒ肵�Ă݂�
		list.rot_.y = list.moveVecRad_.y;

		// ���W�ݒ�
		MV1SetPosition(list.modelid_, list.pos_);
	}
	else
	{
		// �ړ���Ԃ̐ݒ�
		list.moveKind_ = 0;

		//�A�j���[�V�������b�N����Ă��Ȃ��Ƃ��ҋ@
		if (!list.animlockflg_)
		{
			ChangeAnimation(idle);
		}
	}

	if (list.pos_.y != 0.0f)
	{
		list.pos_.y = 0.0f;
	}
}

void Player::PlayAnimation(void)
{
	DebugAnimation();			//�����L�[�ŃA�j���[�V�����؂�ւ�

	switch (list.animindex_)
	{
		//���[�v������
	case 0:			//����Ȃ��ҋ@
	case 1:			//����Ȃ�����
	case 2:			//����Ȃ�����
	case 5:			//���킠��ҋ@
	case 6:			//���킠�����
	case 7:			//���킠�葖��
	case 8:			//���킠�艡����
	case 12:		//���Ⴊ�ݑҋ@	
		list.currentAnimTime_ += ANIM_SPEED;
		if (list.currentAnimTime_ > list.animTotalTime_)
		{
			list.currentAnimTime_ = 0.0f;
		}
		list.animlockflg_ = false;
		break;
		//���[�v�����Ȃ�
	case 3:			//������o��
	case 4:			//����߂�
		list.currentAnimTime_ += ANIM_SPEED;
		if (list.currentAnimTime_ >= list.animTotalTime_)
		{
			list.currentAnimTime_ = list.animTotalTime_;
			list.animlockflg_ = false;
		}
		break;
	case 9:			//�W�����v
	case 13:		//�h�䂵���Ƃ�
	case 15:		//�U���P
	case 16:		//�U���Q
	case 17:		//�U���R
	case 18:		//�U���S
		list.currentAnimTime_ += ANIM_SPEED;
		if (list.currentAnimTime_ >= list.animTotalTime_)
		{
			list.currentAnimTime_ = list.animTotalTime_;
			list.animlockflg_ = false;
		}
		break;
	case 19:		//���S
		list.currentAnimTime_ += DEAD_ANIM_SPEED;
		if (list.currentAnimTime_ >= list.animTotalTime_)
		{
			list.currentAnimTime_ = list.animTotalTime_;
			list.animlockflg_ = false;
		}
		break;
	case 10:		//���Ⴊ��(�h��)
		list.currentAnimTime_ += CROUCH_ANIM_SPEED;
		if (list.currentAnimTime_ >= list.animTotalTime_)
		{
			list.currentAnimTime_ = list.animTotalTime_;
			list.animlockflg_ = false;

			ChangeAnimation(12);		//���Ⴊ�ݑҋ@
		}
		break;
	case 11:		//���Ⴊ�݉���
		list.currentAnimTime_ += CROUCH_ANIM_SPEED;
		if (list.currentAnimTime_ >= list.animTotalTime_)
		{
			list.currentAnimTime_ = list.animTotalTime_;
			list.animlockflg_ = false;
		}
		break;
	case 14:		//���Ⴊ�ݍU��
		list.currentAnimTime_ += CROUCH_ATTACK_SPEED;
		if (list.currentAnimTime_ >= list.animTotalTime_)
		{
			list.currentAnimTime_ = list.animTotalTime_;
			list.animlockflg_ = false;
			list.crouchattackflg_ = false;	//���Ⴊ�ݍU���I��

			ChangeAnimation(Crouch_Idle);			//���Ⴊ�ݑҋ@
		}
		break;
	default:
		break;
	}
	MV1SetAttachAnimTime(list.modelid_, list.animAttachNo_, list.currentAnimTime_);
	MV1SetPosition(list.modelid_, list.pos_);
}

//���L�[�ŃA�j���[�V�����؂�ւ�
void Player::DebugAnimation(void)
{
	InputManager& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_0)) {
		ChangeAnimation(19);
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
void Player::ChangeAnimation(int idx, bool lock)
{
	if (list.animindex_ != idx) {
		MV1DetachAnim(list.modelid_, list.animAttachNo_);
		list.animindex_ = idx;
		list.animAttachNo_ = MV1AttachAnim(list.modelid_, list.animindex_);
		list.animTotalTime_ = MV1GetAttachAnimTotalTime(list.modelid_, list.animAttachNo_);
		list.currentAnimTime_ = 0.0f;
		MV1SetAttachAnimTime(list.modelid_, list.animAttachNo_, list.currentAnimTime_);
		//���b�N�w�肳�ꂽ��t���OON
		list.animlockflg_ = lock;
	}
}