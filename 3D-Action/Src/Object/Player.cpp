#include <DxLib.h>
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "../Manager/AnimationManager.h"
#include "../Application.h"
#include "Player.h"
#include "Sword.h"
#include "Enemy.h"
#include "EnemyManager.h"
#include "Stage.h"

// �f�X�g���N�^
Player::Player(void)
{
	list.modelId_ = -1;

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

	list.isDead_ = false;
	list.isImpact_ = false;
	list.isCrouch_ = false;
	list.isWeapon_ = false;
	list.hitStopTime_ = 0.0f;
	list.isHitStop_ = false;

	damageCooldown_ = 0;
	IsDamagedThisFrame_ = false;
	criticalDisplayTime_ = 0.0f;
	invincibleTime_ = 0.0f;
	showCriticalText_ = false;

	enemymng_ = nullptr;
	animation_ = nullptr;

	list.criticalRate_ = 10.0f;
	list.criticalDamage = 1.5f;

	criticalFontHandle_ = -1;
	isDamaged_=false;

	list.flashCounter_ = 0.0f;
}

// �f�X�g���N�^
Player::~Player(void)
{
}

// ������
void Player::Init(EnemyManager* mng, AnimationManager* anim,Camera* camera)
{
	enemymng_ = mng;
	animation_ = anim;
	camera_ = camera;

	// ���f���̓ǂݍ���
	list.modelId_ = MV1LoadModel("Data/Model/PlayerModel.mv1");

	// �v���C���[�̏����ʒu�ݒ�
	list.pos_ = { 0.0f, 0.0f, -550.0f };
	MV1SetPosition(list.modelId_, list.pos_);

	// �v���C���[�̊p�x�ݒ�
	list.rot_ = { 0.0f, 0.0f, 0.0f };
	MV1SetRotationXYZ(list.modelId_, list.rot_);

	// �v���C���[�̌��̐F
	list.orgColor_ = MV1GetDifColorScale(list.modelId_);

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

	// �U���͂̏�����
	list.attackPower_ = AttackPower_;

	//����ł��邩
	list.isDead_ = false;

	//���Ⴊ��ł��邩
	list.isCrouch_ = false;

	//����o���Ă��邩
	list.isWeapon_ = false;

	//�U�����Ă��邩
	list.isAttack_ = false;

	//�U���i�K
	list.isCombo_ = false;
	list.comboStep_ = 0;

	// HP�̏�����
	list.hp_ = PlayerMaxHp_;

	list.isGameOver_ = false;
	list.isImpact_ = false;
	list.isInvincible_ = false;

	list.dashCount_ = 0.0f;

	// ���G���Ԃ̎c�莞��(�b)
	invincibleTime_ = 0.0f;

	// �N���e�B�J���\���p
	showCriticalText_ = false;
	criticalDisplayTime_ = 0.0f;

	// �_���[�W�Ԋu����p
	damageCooldown_ = 0;

	// �t�H���g�̓ǂݍ���
	criticalFontHandle_ = CreateFontToHandle("���C���I", 64, 5, DX_FONTTYPE_ANTIALIASING_EDGE);

	MV1SetupCollInfo(list.modelId_);
}

void Player::Update(float deltaTime)
{
	IsDamagedThisFrame_ = false;	

	// ���G���Ԃ̃J�E���g
	if (list.isInvincible_) 
	{
		// ���G���Ԃ��O�ȏゾ�����猸��
		if (invincibleTime_ > 0.0f)
		{
			invincibleTime_--;
			if (invincibleTime_ <= 0.0f)
			{
				invincibleTime_ = 0.0f;
				list.isInvincible_ = false;
			}
		}
	}

	// �N���e�B�J���\���J�E���g
	if (showCriticalText_)
	{
		// �N���e�B�J���\�����Ԃ��O�ȏゾ�����猸��
		if (criticalDisplayTime_ > 0.0f)
		{
			criticalDisplayTime_--;
			if (criticalDisplayTime_ <= 0.0f)
			{
				showCriticalText_ = false;
			}
		}
	}

	//�ړ�����
	UpdateMove();
	SetRotation();
	MV1RefreshCollInfo(list.modelId_);
}

// �v���C���[�S�̂̓���
void Player::UpdateMove(void)
{
	// ���͐���擾
	InputManager& ins = InputManager::GetInstance();

	// ��ɍ������O�ɐݒ�
	if (list.pos_.y != 0.0f)
	{
		list.pos_.y = 0.0f;
	}

	//���S���Ɉړ������Ȃ�
	if (list.isDead_)
	{
		list.moveSpeed_ = MOVE_SPEED_STOP;
		animation_->PlayerChangeAnimation(animation_->Death);								// ���S�A�j���[�V����
		return;
	}

	// ��_���A�j���[�V�������ړ������Ȃ�
	if (list.isImpact_)
	{
		list.moveSpeed_ = MOVE_SPEED_STOP;
		return;
	}

	if (list.isWeapon_)
	{
		if (CrouchUpdate()) { return; }
		if (AttackUpdate()) { return; }
	}

	// �E�N���b�N�������畐��؂�ւ�
	if (ins.IsTrgMouseRight()||ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1,InputManager::JOYPAD_BTN::L_TRIGGER))
	{
		list.isWeapon_ = !list.isWeapon_;											// ���̃t���O�Ɣ��΂̃t���O����
		if (list.isWeapon_)
		{
			animation_->PlayerChangeAnimation(animation_->WeaponOut, true);				//������o��
		}
		else {
			animation_->PlayerChangeAnimation(animation_->Sheach, true);					//���킵�܂�
		}
		return;
	}

	// ����؂�ւ����Ƃ��̓���
	if (!list.isWeapon_)
	{
		PlayerMove(animation_->NoWeaponIdle, animation_->NoWeaponWalk, animation_->NoWeaponRun);	//���펝���Ă��Ȃ��Ƃ��̓���
	}
	else
	{
		PlayerMove(animation_->WeaponIdle, animation_->WeaponWalk, animation_->WeaponRun);			//���펝���Ă���Ƃ��̓���
	}
}

// �`��
void Player::Draw(void)
{
	// ���f���̕`��
	if (list.hp_ <= 50)
	{
		list.flashCounter_++;
		float alpha = (sinf(list.flashCounter_ * FLASH_SPEED) + 1.0f) / 2.0f;

		COLOR_F finalColor;
		finalColor.r = list.orgColor_.r + (1.0f - list.orgColor_.r) * alpha;
		finalColor.g = list.orgColor_.g * (1.0f - alpha);
		finalColor.b = list.orgColor_.b * (1.0f - alpha);
		finalColor.a = 1.0f;
		MV1SetDifColorScale(list.modelId_, finalColor);
	}
	else
	{
		list.flashCounter_ = 0.0f;
		MV1SetDifColorScale(list.modelId_, list.orgColor_);
	}

	if (list.isInvincible_)
	{
		COLOR_F translucentModel = list.orgColor_;
		translucentModel.a = TRANSLUCENT;
		MV1SetDifColorScale(list.modelId_, translucentModel);
	}
	MV1DrawModel(list.modelId_);

	// === �f�o�b�O�\���F�v���C���[��HP�ƍU���� ===
	DrawFormatString(20, 500, GetColor(255, 255, 255), "�v���C���[HP�F%d / %d", list.hp_, PlayerMaxHp_);
	DrawFormatString(20, 530, GetColor(255, 255, 255), "�v���C���[�U���́F%d", list.attackPower_);

	// �v���C���[���W�\��
	if (list.isInvincible_) {
		DrawFormatString(20, 300, GetColor(0xff, 0xff, 0xff), "���G���ԁF%f", invincibleTime_);
	}
	DrawFormatString(20, 150, GetColor(0xff, 0xff, 0xff), "�v���C���[HP�F%d", list.hp_);

	// �Q�[���p�b�h�̃f�o�b�O���\���i���X�e�B�b�N�l�j
	InputManager& ins = InputManager::GetInstance();
	InputManager::JOYPAD_IN_STATE pad = ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

	// ���X�e�B�b�N�̓��͒l�i-1000�`1000�j
	float lx = pad.AKeyLX / 1000.0f;
	float ly = pad.AKeyLY / 1000.0f;
	DrawFormatString(20, 400, 0xffffff, "dashCount�F%f", list.dashCount_);
	DrawFormatString(20, 580, GetColor(0, 255, 255), "Pad Left Stick X: %.2f", lx);
	DrawFormatString(20, 600, GetColor(0, 255, 255), "Pad Left Stick Y: %.2f", ly);

	// �{�^�����́iX�{�^����DOWN�j���m�F
	bool isADown = ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN);
	DrawFormatString(20, 620, GetColor(255, 255, 0), "Pad A Button (DOWN): %s", isADown ? "Pressed" : "Not Pressed");

	// �N���e�B�J���\��
	if (showCriticalText_)
	{

		// �\�����Ԃɉ����ē����x��ς���
		// ���Ԃ����قǓ����ɂȂ��ăt�F�[�h�A�E�g������
		int alpha = (int)(255.0f * (criticalDisplayTime_ / 180.0f)); // �ő�3�b�̎c�莞�ԂŒ���

		// �X�P�[�����g��k������(�ő�1.5�{)
		float scale = 1.0f + 0.5f * (1.0f - (criticalDisplayTime_ / 180.0f));

		// �����ɕ\�����邽�߂̍��W
		int screenW = Application::SCREEN_SIZE_X / 2;
		int screenH = Application::SCREEN_SIZE_Y / 3;

		// a�u�����h���[�h��ݒ�(�����x��L����)
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

		// �����ɐԂ��傫�ȕ����ŁuCRITICAL!!�v�ƕ\��(�t�H���g�n���h�����g��)
		DrawRotaStringToHandle(
			screenW, screenH,         // ���SX, Y
			scale, scale,             // �g�嗦X, Y
			0.0, 0.0,                 // ��]���SX, Y
			0.0,                      // ��]�p�i��]�s�v�j
			GetColor(255, 0, 0),      // �����F�i�ԁj
			criticalFontHandle_,      // �t�H���g�n���h��
			GetColor(0, 0, 0),        // ���F�i���j
			FALSE,                    // �c�����t���O�FFALSE = ������
			TEXT("CRITICAL!!")        // ������iTCHAR�Ή��j
		);
		// �u�����h���[�h����
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}

// ���
void Player::Release(void)
{
	MV1DeleteModel(list.modelId_);
	DeleteFontToHandle(criticalFontHandle_);	// �t�H���g�̉��
}

// �����Ă��邩
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

// �v���C���[�̉�]����
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

	list.rotationMatrix_ = mat;

	// �s����g�p���ă��f���̊p�x��ݒ�
	MV1SetRotationMatrix(list.modelId_, mat);
}

// ���Ⴊ�ݍX�V
bool Player::CrouchUpdate(void)
{
	InputManager& ins = InputManager::GetInstance();

	//���R���g���[���ł��Ⴊ��
	if (ins.IsNew(KEY_INPUT_LCONTROL)||ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1,InputManager::JOYPAD_BTN::R_BUTTON))
	{
		//���Ⴊ�ݏ�Ԃɓ���Ƃ�
		if (!list.isCrouch_)
		{
			list.isCrouch_ = true;				
			list.isAttack_ = false;
			list.moveSpeed_ = MOVE_SPEED_STOP;		// �������~�߂�
			animation_->PlayerChangeAnimation(animation_->Crouch, true);			// ���Ⴊ�ݍĐ�
			return true;
		}

		// �L�[���������Ă�����ҋ@
		if (!animation_->GetPlayerInfo().isAnimLock_)
		{
			animation_->PlayerChangeAnimation(animation_->Crouch_Idle);			// ���Ⴊ�ݑҋ@
		}

		return true;	//���Ⴊ�ݒ�
	}
	// ���Ⴊ��ł���ꍇ�ɃL�[��b�����炵�Ⴊ�݉���
	else if (list.isCrouch_)
	{
		animation_->PlayerChangeAnimation(animation_->Crouch_Cancel, true);		// ���Ⴊ�݉���
		list.isCrouch_ = false;
	}
	return false;		// ���Ⴊ�ݒ��ł͂Ȃ�
}

// �U���X�V����
bool Player::AttackUpdate(void)
{
	InputManager& ins = InputManager::GetInstance();

	// �U�����Ă��Ȃ���ԂŁA���N���b�N���������Ƃ� or �E�g���K�[�������Ƃ�
	if (!list.isAttack_ && (ins.IsTrgMouseLeft()||		
		ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1,InputManager::JOYPAD_BTN::LEFT)))
	{
		list.isAttack_ = true;					// �U���t���O��true��
		list.comboStep_ = 1;					// �R���{�i�K���P��
		list.isCombo_ = false;					// �R���{����true��
		animation_->PlayerChangeAnimation(animation_->First_Attack, true);
		return true;
	}

	if(animation_->PlayerAttackCombo(1, animation_->Second_Attack, 2, 0.5f, 5.0f))return true;		// �P�i�K�ڂŔ����ȏ�(0.5f)�̂Ƃ��ɍU���{�^����������A�j���[�V�����X�L�b�v(��납��5.0f)���Ď��̃A�j���[�V������
	if(animation_->PlayerAttackCombo(2, animation_->Third_Attack, 3, 0.5f, 15.0f))return true;		// �Q�i�K�ڂŔ����ȏ�(0.5f)�̂Ƃ��ɍU���{�^����������A�j���[�V�����X�L�b�v(��납��15.0f)���Ď��̃A�j���[�V������

	return list.isAttack_;		//�U������
}

// �v���C���[���_���[�W���󂯂��Ƃ�
void Player::TakeDamage(int damage)
{
	// ���̃t���[���Ŋ��Ƀ_���[�W���󂯂Ă���Ζ����i�����̑��i�h�~�j
	if (invincibleTime_ > 0.0f) return;

	// HP�����炷
	list.hp_ -= damage;
	// HP��0�ȉ��Ȃ玀�S��Ԃֈڍs
	if (list.hp_ <= 0)
	{
		list.hp_ = 0;
		list.isDead_ = true;
	}
	else
	{
		list.isInvincible_ = true;
		if (animation_->GetPlayerInfo().animIndex_ == animation_->Crouch_Guard)
		{
			SetInvincibleTime(3.0f);
			animation_->PlayerChangeAnimation(animation_->Crouch_Guard, true);
		}
		else
		{
			SetInvincibleTime(5.0f);								// ���G���Ԃ�5�b�ɐݒ�
			animation_->PlayerChangeAnimation(animation_->Impact, true);  // �_���[�W�A�j���[�V�����Đ�
		}

	}
}

// �v���C���[�̈ړ�����
void Player::PlayerMove(int idle, int walk, int run)
{
	InputManager& ins = InputManager::GetInstance();

	// WASD�Ńv���C���[�ړ�
	list.moveVec_ = { 0.0f, 0.0f, 0.0f };

	// ���E�E�E��O�E���̃x�N�g�����쐬����
	VECTOR RIGHT_MOVE_VEC = { 1.0f,  0.0f,  0.0f };
	VECTOR LEFT_MOVE_VEC = { -1.0f,  0.0f,  0.0f };
	VECTOR FRONT_MOVE_VEC = { 0.0f,  0.0f,  1.0f };
	VECTOR BACK_MOVE_VEC = { 0.0f,  0.0f, -1.0f };

	InputManager::JOYPAD_IN_STATE pad = ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

	// ���X�e�B�b�N�̓��͒l�i-1000�`1000�j
	float lx = pad.AKeyLX / 1000.0f;
	float ly = pad.AKeyLY / 1000.0f;

	// ���͕����Ɉړ��x�N�g����ǉ�����
	if (ins.IsNew(KEY_INPUT_W) || ly < 0) { list.moveVec_ = VAdd(list.moveVec_, FRONT_MOVE_VEC); }
	if (ins.IsNew(KEY_INPUT_A) || lx < 0) { list.moveVec_ = VAdd(list.moveVec_, LEFT_MOVE_VEC); }
	if (ins.IsNew(KEY_INPUT_S) || ly > 0) { list.moveVec_ = VAdd(list.moveVec_, BACK_MOVE_VEC); }
	if (ins.IsNew(KEY_INPUT_D) || lx > 0) { list.moveVec_ = VAdd(list.moveVec_, RIGHT_MOVE_VEC); }

	// �x�N�g���̈ړ����s���Ă�������W�X�V
	if (IsMove(list.moveVec_))
	{
		// �ړ���Ԃ̐ݒ�
		list.moveKind_ = 1;

		list.dashCount_++;

		// �J�����p�x���ݒ肷��
		VECTOR cameraAngles = camera_->GetCameraAngles();
		MATRIX cameraMatY = MGetRotY(cameraAngles.y);
		list.moveVec_ = VTransform(list.moveVec_, cameraMatY);

		if (list.dashCount_ >= 100.0f)
		{
			// �_�b�V�����
			list.moveSpeed_ = MOVE_SPEED_RUN;
			animation_->PlayerChangeAnimation(run);
			list.dashCount_ = 100.0f;
		}
		else
		{
			// �������
			list.moveSpeed_ = MOVE_SPEED_WALK;
			animation_->PlayerChangeAnimation(walk);
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
		MV1SetPosition(list.modelId_, list.pos_);
	}
	else
	{
		// �ړ���Ԃ̐ݒ�
		list.moveKind_ = 0;

		list.dashCount_ = 0.0f;

		//�A�j���[�V�������b�N����Ă��Ȃ��Ƃ��ҋ@
		if (!animation_->GetPlayerInfo().isAnimLock_)
		{
			animation_->PlayerChangeAnimation(idle);
		}
	}
}

// �U�����������Ă�����q�b�g�X�g�b�v
void Player::HitStop(void)
{
	if (list.isHitStop_)
	{
		list.hitStopTime_ -= SceneManager::GetInstance().GetDeltaTime();

		if (list.hitStopTime_ <= 0.0f)
		{
			list.hitStopTime_ = 0.0f;
			list.isHitStop_ = false;
		}
		return;
	}
}

// �S��
void Player::FullRecoveryHp()
{
	list.hp_ = PlayerMaxHp_; // �ő�l�ɂ���i����MaxHP�������Ă���Ȃ炻�̕ϐ��Łj
}

// �U���͂��グ��
void Player::AddAttack(int amount)
{
	list.attackPower_ += amount;

	// �U���͂̍ŏ��l�̐���
	if (list.attackPower_ <= 10)
	{
		list.attackPower_ = 10;
	}
}

// HP���グ��
void Player::AddMaxHp(int amount)
{
	PlayerMaxHp_ += amount;
	list.hp_ += amount; // ����HP�����₷
}

// �N���e�B�J���\��
void Player::SetCriticalDisplay(bool enable)
{
	showCriticalText_ = enable;
	if (enable)
	{
		criticalDisplayTime_ = 180.0f;  // �\����3�b��
	}
}
