#include "Enemy.h"
#include "Player.h"
#include "../Application.h"
#include "../Manager/InputManager.h"

namespace
{
	// �ړ���
	const float MOVE_SPEED_WALK = 1.5f;				// ����
	const float MOVE_BOSS_SPEED_WALK = 5.0f;		// �{�X����
	const float MOVE_SPEED_RUN = 8.0f;				// ����
	const float MOVE_SPEED_STOP = 0.0f;
	// �������f���␳�p�x
	const VECTOR INIT_MODEL_ROT_OFFSET = { 0.0f,DX_PI_F / 2.0f,0.0f };
	// �����Ă������
	const VECTOR INIT_DIR = { 0.0f,0.0f,-1.0f };
	// ����p
	const float VIEW_ANGLE = 30.0f;
	// �A�j���[�V�����̍Đ����x
	const float ANIM_SPEED = 0.5f;					// �����Ă��鎞
	const float ATTACK_ANIM_SPEED = 0.5f;			// �U����
	const float DEAD_ANIM_SPEED = 0.2f;				// ����ł��鎞
	
	// �U���̃N�[���^�C��(1.0f��1�b)
	const float ATTACK_COOLDOWN_TIME = 1.0f * 60;
}


Enemy::Enemy(void)
{
	list.modelId_ = -1;
	list.scale_ = VGet(1.0f, 1.0f, 1.0f);
	list.pos_ = VGet(0.0f, 0.0f, 0.0f);
	list.rot_ = VGet(0.0f, 0.0f, 0.0f);

	list.moveSpeed_ = MOVE_SPEED_STOP;
	list.moveDir_ = 0;
	list.moveKind_ = 0;

	list.moveVec_ = VGet(0.0f, 0.0f, 0.0f);
	list.oldMoveVec_ = VGet(0.0f, 0.0f, 0.0f);
	list.moveVecRad_ = VGet(0.0f, 0.0f, 0.0f);
	list.localRot_ = INIT_MODEL_ROT_OFFSET;

	list.animIndex_ = 0;
	list.animAttachNo_ = -1;
	list.animTotalTime_ = 0.0f;
	list.currentAnimTime_ = 0.0f;
	list.isAnimLock_ = false;

	list.isDead_ = false;
	list.isDraw_ = true;
	list.isAttack_ = false;
	list.isBoss_ = false;

	list.isAssign_ = false;

	list.attackRange_ = 0.0f;
}

Enemy::~Enemy(void)
{
}

// ������
void Enemy::Init(int org, int hp, int attack)
{
	//���f��
	list.modelId_ = MV1DuplicateModel(org);
	//���W
	list.pos_ = { 0.0f,0.0f,50.0f };
	list.nextPos_ = { 0.0f,0.0f,0.0f };
	MV1SetPosition(list.modelId_, list.pos_);
	//�傫��
	list.scale_ = { 0.8f,0.8f,0.8f };
	MV1SetScale(list.modelId_, list.scale_);
	//��]
	list.rot_ = { 0.0f,0.0f,0.0f };
	MV1SetRotationXYZ(list.modelId_, list.rot_);

	// �G��HP�̏�����
	list.hp_ = hp;
	//�ő�HP�̕ۑ�
	list.maxHp_ = hp; 
	// �U���͂̏�����
	list.attackPower_ = attack;

	//�A�j���[�V�����֘A
	list.animIndex_ = 0;
	list.animAttachNo_ = MV1AttachAnim(list.modelId_, list.animIndex_);
	list.animTotalTime_ = MV1GetAttachAnimTotalTime(list.modelId_, list.animAttachNo_);
	list.currentAnimTime_ = 0.0f;
	list.isAnimLock_ = false;
	MV1SetAttachAnimTime(list.modelId_, list.animAttachNo_, list.currentAnimTime_);

	list.isDead_ = false;

	list.isHitboxActive_ = false;
}

// �X�V
void Enemy::Update()
{
	UpdateMove();
	PlayAnimation();
	MV1RefreshCollInfo(list.modelId_);
}

// �`��
void Enemy::Draw(void)
{
	if (!list.isDead_) 
	{
		MV1DrawModel(list.modelId_);
		VECTOR worldPos = VAdd(list.pos_, VGet(0.0f, 50.0f, 0.0f)); // �G�̏�̈ʒu
		worldPos.y += 100.0f;  // ���̏�
		float screenX, screenY;

		if (GetTransformPosition(&worldPos, &screenX, &screenY) == 0)
		{
			const int BAR_WIDTH = 40;
			const int BAR_HEIGHT = 5;

			// HP�o�[�`��
			float hpRatio = (float)list.hp_ / list.maxHp_;
			if (hpRatio < 0.0f) hpRatio = 0.0f;
			if (hpRatio > 1.0f) hpRatio = 1.0f;

			int barX = static_cast<int>(screenX) - BAR_WIDTH / 2;
			int barY = static_cast<int>(screenY) - BAR_HEIGHT / 2;

			// �w�i�o�[�i�O���[�j
			DrawBox(barX, barY, barX + BAR_WIDTH, barY + BAR_HEIGHT, GetColor(80, 80, 80), TRUE);
			// HP�o�[�i�ԁj
			DrawBox(barX, barY, barX + (int)(BAR_WIDTH * hpRatio), barY + BAR_HEIGHT, GetColor(255, 0, 0), TRUE);
			// �f�o�b�O�\��
			DrawFormatString(0, 600, GetColor(255, 255, 255), "�GHP�F%d / %d", list.hp_, list.maxHp_);
			DrawFormatString(0, 630, GetColor(255, 255, 255), "�G�U���́F%d", list.attackPower_);
		}
	}
}

// ���
void Enemy::Release(void)
{
	MV1DeleteModel(list.modelId_);
}

// �ړ�����
void Enemy::UpdateMove(void)
{
	// �U�����͈ړ����Ȃ�
	if (list.isAnimLock_) return;

	// �����������I�ɒn�ʂɌŒ�i�s�v�Ȃ�폜�j
	if (list.pos_.y != 0.0f)
	{
		list.pos_.y = 0.0f;
	}

	if (!Player::GetInstance().GetIsDead())
	{
		// �v���C���[�Ƃ�XZ�����x�N�g��
		VECTOR toPlayer = VSub(Player::GetInstance().GetPlayerPos(), list.pos_);
		toPlayer.y = 0.0f;

		VECTOR dir = VNorm(toPlayer); // ���K�����������x�N�g��

		// �v���C���[�̕����Ɂu�w�苗���v�����I�t�Z�b�g�������W���U����ɂ���
		const float attackOffsetDistance = 20.0f;
		VECTOR targetAttackPos = VAdd(Player::GetInstance().GetPlayerPos(), VScale(dir, attackOffsetDistance));

		// �U���ΏۂƂ̋����𑪂�
		float dist = VSize(VSub(targetAttackPos, list.pos_));

		// �U���͈͐ݒ�i�{�X or �ʏ�G�j
		//list.attackRange_ = list.isBoss_ ? 100.0f : 50.0f;
		list.attackRange_ = 100.0f;
		// �N�[���_�E������
		if (list.attackCooldown_ > 0.0f)
		{
			list.attackCooldown_ -= 1.0f;
		}

		// �U�������𖞂����ꍇ
		if (!list.isAnimLock_ && dist < list.attackRange_ && list.attackCooldown_ <= 0.0f)
		{
			list.isAttack_ = true;
			ChangeAnimation(EnemyAnim::ANIM_ATTACK, true);
			list.attackCooldown_ = ATTACK_COOLDOWN_TIME;
			return;
		}

		// �U���͈͓������N�[���_�E���� �� �����Ȃ�
		if (dist < list.attackRange_)
		{
			// �U���͈͓��ɂ���̂ňړ����Ȃ��i�����������킹��j
			float targetAngleY = atan2f(dir.x, dir.z);
			list.rot_.y = targetAngleY - INIT_MODEL_ROT_OFFSET.y * 2;

			MV1SetPosition(list.modelId_, list.pos_);
			MV1SetRotationXYZ(list.modelId_, list.rot_);

			// ���[�V�����؂�ւ��i�����Ȃ��j
			if (list.animIndex_ != EnemyAnim::ANIM_IDLE && !list.isAnimLock_)
			{
				ChangeAnimation(ANIM_IDLE);
			}
			return;
		}

		float animRatio = list.currentAnimTime_ / list.animTotalTime_;

		if ((animRatio >= 0.8f && animRatio <= 1.0f) || (animRatio <= 0.1f) || (animRatio >= 0.3f && animRatio <= 0.6f))
		{

			// �v���C���[���U���͈͊O��������ڋߊJ�n
			if (list.isBoss_)
			{
				list.moveVec_ = VScale(dir, MOVE_BOSS_SPEED_WALK);
			}
			else
			{
				list.moveVec_ = VScale(dir, MOVE_SPEED_WALK);
			}

			list.nextPos_ = VAdd(list.pos_, list.moveVec_);
			list.pos_ = list.nextPos_;

			// ��������
			float targetAngleY = atan2f(dir.x, dir.z);
			list.rot_.y = targetAngleY - INIT_MODEL_ROT_OFFSET.y * 2;
		}

		MV1SetPosition(list.modelId_, list.pos_);
		MV1SetRotationXYZ(list.modelId_, list.rot_);

		// ���[�V�����؂�ւ��i�ړ����Ȃ�����j
		if (VSize(list.moveVec_) > 0.01f)
		{
			if (list.animIndex_ != EnemyAnim::ANIM_WALK && !list.isAnimLock_)
			{
				ChangeAnimation(ANIM_WALK);
			}
		}
		else
		{
			if (list.animIndex_ != EnemyAnim::ANIM_IDLE && !list.isAnimLock_)
			{
				ChangeAnimation(ANIM_IDLE);
			}
		}
	}
	else
	{
		// �v���C���[���S���͑ҋ@
		ChangeAnimation(ANIM_IDLE);
	}
}

// ���S�t���O
bool Enemy::IsDead() const
{
	return list.isDead_;
}

void Enemy::SetEnemyPos(const VECTOR& pos)
{
	list.pos_ = pos;
	MV1SetPosition(list.modelId_, list.pos_);
}

VECTOR Enemy::GetRightHandPosition(void)
{
	int frameIndex = MV1SearchFrame(list.modelId_, "mixamorig:RightHand");
	if (frameIndex != -1)
	{
		// ���f���̃A�j���[�V������ʒu�E��]�𔽉f��������ł̃��[���h���W���擾
		return MV1GetFramePosition(list.modelId_, frameIndex);
	}
	else
	{
		// �t���[����������Ȃ������ꍇ�͓K���ɏ�����̈ʒu��Ԃ�
		return VAdd(list.pos_, VGet(0.0f, 100.0f, 0.0f));
	}
}
VECTOR Enemy::GetLeftHandPosition(void)
{
	int frameIndex = MV1SearchFrame(list.modelId_, "mixamorig:LeftHand");
	if (frameIndex != -1)
	{
		// ���f���̃A�j���[�V������ʒu�E��]�𔽉f��������ł̃��[���h���W���擾
		return MV1GetFramePosition(list.modelId_, frameIndex);
	}
	else
	{
		// �t���[����������Ȃ������ꍇ�͓K���ɏ�����̈ʒu��Ԃ�
		return VAdd(list.pos_, VGet(0.0f, 100.0f, 0.0f));
	}
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
		list.isDead_ = true;
	}
}

//�A�j���[�V�����Đ�
void Enemy::PlayAnimation(void)
{
	switch (list.animIndex_)
	{
	//���[�v������
	case ANIM_IDLE:				//�ҋ@
	case ANIM_WALK:				//����
	case ANIM_RUN:				//����
		list.currentAnimTime_ += ANIM_SPEED;
		if (list.currentAnimTime_ > list.animTotalTime_)
		{
			list.currentAnimTime_ = 0.0f;
		}
		break;
	//���[�v�����Ȃ�
	case ANIM_ATTACK:			//�U��
		list.currentAnimTime_ += ATTACK_ANIM_SPEED;
		if (list.currentAnimTime_ >= list.animTotalTime_)
		{
			list.currentAnimTime_ = list.animTotalTime_;
			list.isAnimLock_ = false;
			list.isAttack_ = false;
		}
		break;
	case ANIM_DEAD:				//���S
		list.currentAnimTime_ += DEAD_ANIM_SPEED;
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

//�A�j���[�V�����؂�ւ�
void Enemy::ChangeAnimation(int idx, bool lock)
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
	}
}
