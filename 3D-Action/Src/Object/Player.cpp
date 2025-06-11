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
}

Player& Player::GetInstance(void)
{
	return *instance_;
}

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

	list.animIndex_ = 0;
	list.animAttachNo_ = 0;
	list.animTotalTime_ = 0.0f;
	list.currentAnimTime_ = 0.0f;
	list.isAnimLock_ = false;

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
}

Player::~Player(void)
{
}

void Player::Init(EnemyManager* mng)
{
	enemymng_ = mng;

	// ���f���̓ǂݍ���
	list.modelId_ = MV1LoadModel("Data/Model/PlayerModel.mv1");

	// �v���C���[�̏����ʒu�ݒ�
	list.pos_ = { 0.0f, 0.0f, -550.0f };
	MV1SetPosition(list.modelId_, list.pos_);

	// �v���C���[�̊p�x�ݒ�
	list.rot_ = { 0.0f, 0.0f, 0.0f };
	MV1SetRotationXYZ(list.modelId_, list.rot_);

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
	list.hp_ = PLAYER_MAX_HP;

	list.isGameOver_ = false;
	list.isImpact_ = false;
	list.isInvincible_ = false;

	// ���G���Ԃ̎c�莞��(�b)
	invincibleTime_ = 0.0f;

	// �N���e�B�J���\���p
	showCriticalText_ = false;
	criticalDisplayTime_ = 0.0f;

	// �_���[�W�Ԋu����p
	damageCooldown_ = 0;

	//�A�j���[�V�����֘A
	list.animIndex_ = 1;
	list.animAttachNo_ = MV1AttachAnim(list.modelId_, list.animIndex_);
	list.animTotalTime_ = MV1GetAttachAnimTotalTime(list.modelId_, list.animAttachNo_);
	list.currentAnimTime_ = 0.0f;
	list.isAnimLock_ = false;
	MV1SetAttachAnimTime(list.modelId_, list.animAttachNo_, list.currentAnimTime_);

	list.currentRatio_ = 0.0f;
	list.remainingTime_ = 0.0f;

	MV1SetupCollInfo(list.modelId_);
}

void Player::Update(float deltaTime)
{
	IsDamagedThisFrame_ = false;

	// ���G���Ԃ̃J�E���g
	if (list.isInvincible_) {
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
		criticalDisplayTime_ -= deltaTime;
		if (criticalDisplayTime_ <= 0.0f)
		{
			showCriticalText_ = false;
		}
	}

	//�ړ�����
	UpdateMove();
	SetRotation();
	MV1RefreshCollInfo(list.modelId_);
	// �A�j���[�V��������
	PlayAnimation();
}

void Player::UpdateMove(void)
{
	// ���͐���擾
	InputManager& ins = InputManager::GetInstance();

	if (list.pos_.y != 0.0f)
	{
		list.pos_.y = 0.0f;
	}

	//���S���Ɉړ������Ȃ�
	if (list.isDead_)
	{
		list.moveSpeed_ = MOVE_SPEED_STOP;
		ChangeAnimation(Death);
		return;
	}

	// ��_���A�j���[�V������
	if (list.isImpact_)
	{
		list.moveSpeed_ = MOVE_SPEED_STOP;
		return;
	}

	if (ins.IsTrgMouseRight())
	{
		list.isWeapon_ = !list.isWeapon_;
		if (list.isWeapon_)
		{
			ChangeAnimation(WeaponOut, true);					//������o��
		}
		else {
			ChangeAnimation(Sheach, true);						//���킵�܂�
		}
		return;
	}

	if (!list.isWeapon_)
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
	MV1DrawModel(list.modelId_);

	// �v���C���[HP�o�[
	const int HP_BAR_WIDTH = 300;
	const int HP_BAR_HEIGHT = 20;
	float hpRatio = (float)list.hp_ / PLAYER_MAX_HP;

	int color = GetColor(0, 255, 0); // ��
	if (list.hp_ <= 20)
	{
		color = GetColor(255, 0, 0); //��
	}
	else if (list.hp_ < PLAYER_MAX_HP / 2)
	{
		color = GetColor(255, 255, 0); // ��
	}

	int x = 20;
	int y = 20;
	DrawBox(x, y, x + HP_BAR_WIDTH, y + HP_BAR_HEIGHT, GetColor(50, 50, 50), TRUE); // �w�i�i�O���[�j
	DrawBox(x, y, x + (int)(HP_BAR_WIDTH * hpRatio), y + HP_BAR_HEIGHT, color, TRUE); // HP�o�[�{��
	DrawBox(x, y, x + HP_BAR_WIDTH, y + HP_BAR_HEIGHT, GetColor(255, 255, 255), FALSE); // �g��


	// === �f�o�b�O�\���F�v���C���[��HP�ƍU���� ===
	DrawFormatString(20, 500, GetColor(255, 255, 255), "�v���C���[HP�F%d / %d", list.hp_, PLAYER_MAX_HP);
	DrawFormatString(20, 530, GetColor(255, 255, 255), "�v���C���[�U���́F%d", list.attackPower_);

	// �v���C���[���W�\��
	if (list.isInvincible_) {
		DrawFormatString(20, 120, GetColor(0xff, 0xff, 0xff), "���G���ԁF%f", invincibleTime_);
	}
	DrawFormatString(20, 150, GetColor(0xff, 0xff, 0xff), "�v���C���[HP�F%d", list.hp_);

	// �N���e�B�J���\��
	if (showCriticalText_)
	{
		DrawFormatString(20, 20, GetColor(255, 0, 0), "�N���e�B�J���I");
	}
}

// ���
void Player::Release(void)
{
	MV1DeleteModel(list.modelId_);
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

	list.rotationMatrix_ = mat;

	// �s����g�p���ă��f���̊p�x��ݒ�
	MV1SetRotationMatrix(list.modelId_, mat);
}

bool Player::CrouchUpdate(void)
{
	InputManager& ins = InputManager::GetInstance();

	//�h��(���Ⴊ��)���ɂ͈ړ������Ȃ�
	if (ins.IsNew(KEY_INPUT_LCONTROL))
	{
		if (!list.isCrouch_)
		{
			//���Ⴊ�ݏ�Ԃɓ���Ƃ�
			list.isCrouch_ = true;
			list.isAttack_ = false;
			list.moveSpeed_ = MOVE_SPEED_STOP;		//�������~�߂�
			ChangeAnimation(Crouch, true);			//���Ⴊ�ݍĐ�
			return true;
		}

		if (!list.isAnimLock_)
		{
			ChangeAnimation(Crouch_Idle);			//���Ⴊ�ݑҋ@
		}

		return true;	//���Ⴊ�ݒ�
	}
	else if (list.isCrouch_)
	{
		//���R���g���[���𗣂����Ƃ��ɂ��Ⴊ�݉���
		ChangeAnimation(Crouch_Cancel, true);		//���Ⴊ�݉���
		list.isCrouch_ = false;
	}
	return false;		//���Ⴊ�ݒ��ł͂Ȃ�
}

bool Player::AttackUpdate(void)
{
	InputManager& ins = InputManager::GetInstance();

	if (!list.isAttack_ && ins.IsTrgMouseLeft())
	{
		list.isAttack_ = true;
		list.comboStep_ = 1;
		list.isCombo_ = false;
		ChangeAnimation(First_Attack, true);
		return true;
	}

	if(AttackCombo(1, Third_Attack, 2, 0.5f, 5.0f))return true;
	if(AttackCombo(2, Force_Attack, 3, 0.5f, 15.0f))return true;
	if(AttackCombo(3, Second_Attack, 4, 0.5f, 15.0f))return true;

	return list.isAttack_;		//�U������
}

//�U���R���{(���̃R���{�A���̃A�j���[�V�����A���̒i�K�A���͎�t����(��납��̊���)�A�A�j���[�V�����X�L�b�v����(��납��))
bool Player::AttackCombo(int nowcombo, int nextanimidx, int nextstep, float reception, float remainingtime)
{
	InputManager& ins = InputManager::GetInstance();

	//�U���P���ɃA�j���[�V�����������ȏ�i��ł�����U���Q
	if (list.isAttack_ && list.comboStep_ == nowcombo)
	{
		//���݂̃A�j���[�V��������(����)
		list.currentRatio_ = list.currentAnimTime_ / list.animTotalTime_;

		//�A�j���[�V�������w��̊����ȏ�{���N���b�N�������Ƃ�
		if (list.currentRatio_ >= reception && ins.IsTrgMouseLeft())
		{
			list.isCombo_ = true;
		}

		// �c�莞�Ԃ����ȉ��{�R���{�\�Ȃ玟�̃A�j���[�V������
		list.remainingTime_ = list.animTotalTime_ - list.currentAnimTime_;
		if (list.remainingTime_ <= remainingtime && list.isCombo_)
		{
			list.comboStep_ = nextstep;
			list.isCombo_ = false;
			ChangeAnimation(nextanimidx, true);
			return true;
		}
	}
	return false;
}

void Player::TakeDamage(int damage)
{
	// ���̃t���[���Ŋ��Ƀ_���[�W���󂯂Ă���Ζ����i�����̑��i�h�~�j
	if (invincibleTime_ > 0.0f) return;

	list.hp_ -= damage;
	// HP��0�ȉ��Ȃ玀�S��Ԃֈڍs
	if (list.hp_ <= 0)
	{
		list.hp_ = 0;
		list.isDead_ = true;
	}

	list.isInvincible_ = true;
	invincibleTime_ = 5.0f * 60.0f;			// ���G���Ԃ�5�b�ɐݒ�
	ChangeAnimation(Impact, true);  // �_���[�W�A�j���[�V�����Đ�
}

void Player::PlayerMove(int idle, int walk, int run)
{
	InputManager& ins = InputManager::GetInstance();

	if (list.isWeapon_)
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
		MV1SetPosition(list.modelId_, list.pos_);
	}
	else
	{
		// �ړ���Ԃ̐ݒ�
		list.moveKind_ = 0;

		//�A�j���[�V�������b�N����Ă��Ȃ��Ƃ��ҋ@
		if (!list.isAnimLock_)
		{
			ChangeAnimation(idle);
		}
	}
}

void Player::PlayAnimation(void)
{
	DebugAnimation();
	switch (list.animIndex_)
	{
		//���[�v������
	case NoWeaponIdle:			//����Ȃ��ҋ@
	case NoWeaponWalk:			//����Ȃ�����
	case NoWeaponRun:			//����Ȃ�����
	case WeaponIdle:			//���킠��ҋ@
	case WeaponWalk:			//���킠�����
	case WeaponRun:				//���킠�葖��
	case Strafe:				//������
	case Crouch_Idle:			//���Ⴊ�ݑҋ@	
		list.currentAnimTime_ += ANIM_SPEED;
		if (list.currentAnimTime_ > list.animTotalTime_)
		{
			list.currentAnimTime_ = 0.0f;
		}
		list.isAnimLock_ = false;
		break;
		//���[�v�����Ȃ�
	case WeaponOut:			//������o��
	case Sheach:			//����߂�
		list.currentAnimTime_ += ANIM_SPEED;
		if (list.currentAnimTime_ >= list.animTotalTime_)
		{
			list.currentAnimTime_ = list.animTotalTime_;
			list.isAnimLock_ = false;
		}
		break;
	case Jump:				//�W�����v
		list.currentAnimTime_ += ATTACK_ANIM_SPEED;
		if (list.currentAnimTime_ >= list.animTotalTime_)
		{
			list.currentAnimTime_ = list.animTotalTime_;
			list.isAnimLock_ = false;
		}
		break;
	case First_Attack:		//�U���P
	case Second_Attack:		//�U���Q
	case Third_Attack:		//�U���R
	case Force_Attack:		//�U���S
		list.currentAnimTime_ += ATTACK_ANIM_SPEED;
		if (list.currentAnimTime_ >= list.animTotalTime_)
		{
			list.currentAnimTime_ = list.animTotalTime_;
			list.isAnimLock_ = false;
			list.isAttack_ = false;
			list.comboStep_ = 0;
			list.isCombo_ = false;
		}
		break;
	case Impact:		//�U���󂯂��Ƃ�
		list.currentAnimTime_ += ATTACK_ANIM_SPEED;
		if (list.currentAnimTime_ >= list.animTotalTime_)
		{
			list.currentAnimTime_ = list.animTotalTime_;
			list.isAnimLock_ = false;
			list.isImpact_ = false;
		}
		else
		{
			list.isImpact_ = true;
		}
		break;
	case Death:		//���S
		list.currentAnimTime_ += DEAD_ANIM_SPEED;
		if (list.currentAnimTime_ >= list.animTotalTime_)
		{
			list.currentAnimTime_ = list.animTotalTime_;
			list.isAnimLock_ = false;
			if (!list.isGameOver_)
			{
				list.isGameOver_ = true;
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAMEOVER);
			}
		}
		break;
	case Crouch:		//���Ⴊ��(�h��)
		list.currentAnimTime_ += CROUCH_ANIM_SPEED;
		if (list.currentAnimTime_ >= list.animTotalTime_)
		{
			list.currentAnimTime_ = list.animTotalTime_;
			list.isAnimLock_ = false;

			ChangeAnimation(12);		//���Ⴊ�ݑҋ@
		}
		break;
	case Crouch_Cancel:		//���Ⴊ�݉���
		list.currentAnimTime_ += CROUCH_ANIM_SPEED;
		if (list.currentAnimTime_ >= list.animTotalTime_)
		{
			list.currentAnimTime_ = list.animTotalTime_;
			list.isAnimLock_ = false;
		}
		break;
	case Crouch_Guard:		//�h�䂵���Ƃ�
		list.currentAnimTime_ += CROUCH_ANIM_SPEED;
		if (list.currentAnimTime_ >= list.animTotalTime_)
		{
			list.currentAnimTime_ = list.animTotalTime_;
			list.isAnimLock_ = false;
		}
		break;

	default:
		break;
	}
	MV1SetAttachAnimTime(list.modelId_, list.animAttachNo_, list.currentAnimTime_);
	MV1SetPosition(list.modelId_, list.pos_);
}

//���L�[�ŃA�j���[�V�����؂�ւ�
void Player::DebugAnimation(void)
{
	InputManager& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_0)) {
		list.isImpact_ = true;
	}
	if (ins.IsTrgDown(KEY_INPUT_1)) {
		ChangeAnimation(Crouch_Guard);
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

bool Player::AttackActive(void)
{
	if (list.animIndex_ == First_Attack || list.animIndex_ == Second_Attack
		|| list.animIndex_ == Third_Attack || list.animIndex_ == Force_Attack)
	{
		return true;
	}
	return false;
}

//�A�j���[�V�����؂�ւ�
void Player::ChangeAnimation(int idx, bool lock)
{
	if (list.animIndex_ != idx) {
		MV1DetachAnim(list.modelId_, list.animAttachNo_);
		list.animIndex_ = idx;
		list.animAttachNo_ = MV1AttachAnim(list.modelId_, list.animIndex_);
		list.animTotalTime_ = MV1GetAttachAnimTotalTime(list.modelId_, list.animAttachNo_);
		list.currentAnimTime_ = 0.0f;
		MV1SetAttachAnimTime(list.modelId_, list.animAttachNo_, list.currentAnimTime_);
		//���b�N�w�肳�ꂽ��t���OON
		list.isAnimLock_ = lock;

		if (AttackActive())
		{
			// �U���ɐ؂�ւ�����^�C�~���O�őS�G��assign�����Z�b�g
			for (auto& enemy : enemymng_->GetEnemyPtrs())
			{
				enemy->SetIsAssign(false);
			}
		}
	}
}

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
void Player::RecoverHp()
{
	list.hp_ = PLAYER_MAX_HP; // �ő�l�ɂ���i����MaxHP�������Ă���Ȃ炻�̕ϐ��Łj
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

// HP,�U���͂��グ��
void Player::AddMaxHp(int amount)
{
	PLAYER_MAX_HP += amount;
	list.hp_ += amount; // ����HP�����₷
}

// �N���e�B�J���\��
void Player::SetCriticalDisplay(bool enable)
{
	showCriticalText_ = enable;
	if (enable) criticalDisplayTime_ = 3.0f;  // �\����3�b��
}

// �A�j���[�V���������ǂ���
bool Player::IsPlayingImpactAnimation() const
{
	return list.animIndex_ == Impact && list.currentAnimTime_ < list.animTotalTime_;
}
