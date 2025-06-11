#include "BossEnemy.h"
#include <DxLib.h>
#include <corecrt_math.h>
#include "Player.h"
#include "../Manager/SceneManager.h"

BossEnemy::BossEnemy()
{
    isSwipe = false;
    isJumpAttack_ = false;
    jumpAttackTime_ = 0.0f;
    jumpStartPos_ = { 0.0f,0.0f,0.0f };
    jumpEndPos_ = { 0.0f,0.0f,0.0f };
}

BossEnemy::~BossEnemy() = default;

// ����������
void BossEnemy::Init(int bossModelID, int hp, int attack)
{
    // �ʏ�̓G�̏����������𗘗p
    Enemy::Init(bossModelID, hp, attack); // Enemy��Init���Ă�
    InitBossStats();                     // �{�X���L�̐ݒ�
    list.isBoss_ = true;
}

//�{�X�̃X�e�[�^�X�ύX
void BossEnemy::InitBossStats()
{
    list.scale_ = { 2.0f,2.0f,2.0f };             // �{�X�T�C�Y���g��
    MV1SetScale(list.modelId_, list.scale_);

    list.hp_ = BOSS_MAX_HP;                       // HP��啝�ɏ㏸
    list.moveSpeed_ = 3.0f;
    list.isBoss_ = true;
}

// �X�V����
void BossEnemy::Update()
{
    VECTOR toPlayer = VSub(Player::GetInstance().GetPlayerPos(), list.pos_);
    //�v���C���[�Ƃ̋����v�Z
    float dist = VSize(toPlayer);

    //�W�����v�U��
    if (isJumpAttack_)
    {
        PlayAnimation();
        float jumpattackrate = list.currentAnimTime_ / list.animTotalTime_;

        float jamptime = list.animTotalTime_ * JUMP_RATIO;  // �W�����v��������(����)
        float landtime = list.animTotalTime_ * LAND_RATIO;  // ���n���鎞��(����)

        if (list.currentAnimTime_ < jamptime)
        {
            // �W�����v�A�j���[�V�����̃W�����v����O�͈ړ����Ȃ�
        }
        else if (list.currentAnimTime_ <= landtime)
        {
            // �󒆈ړ��t�F�[�Y
            float moveRate = (list.currentAnimTime_ - jamptime) / (landtime - jamptime);
            list.pos_ = VAdd(VScale(VSub(jumpEndPos_, jumpStartPos_), moveRate), jumpStartPos_);
        }
        else
        {
            // ���n��
            list.pos_ = jumpEndPos_;
            list.pos_.y = 0.0f;
        }

        // ���f���X�V�ƃA�j������
        MV1SetAttachAnimTime(list.modelId_, list.animAttachNo_, list.currentAnimTime_);
        MV1SetPosition(list.modelId_, list.pos_);
        MV1SetRotationXYZ(list.modelId_, list.rot_);

        // �ʒu�X�V�i�W�����v�ړ��j
        if (list.currentAnimTime_ >= list.animTotalTime_)
        {
            isJumpAttack_ = false;
            jumpAttackTime_ = 0.0f;
            list.isAnimLock_ = false;
            list.isAttack_ = false;
            ChangeAnimation(EnemyAnim::ANIM_IDLE);
        }
        return;
    }
    else
    {
        Enemy::Update();
    }

    //�W�����v�U������͈�
    if (dist > 300.0f && dist < JUMP_ATTACK_RANGE)
    {
        ChangeAnimation(EnemyAnim::ANIM_JUMP_ATTACK, true);

        jumpStartPos_ = list.pos_;
        jumpEndPos_ = Player::GetInstance().GetPlayerPos();
        // �v���C���[����{�X�ւ̃x�N�g���i�i�s�����̋t�j
        VECTOR toPlayer = VNorm(VSub(jumpEndPos_, list.pos_));

        // �v���C���[�̌������ɃI�t�Z�b�g�i��F100.0f ���j�b�g�j
        float offsetDistance = 100.0f;
        jumpEndPos_ = VSub(jumpEndPos_, VScale(toPlayer, offsetDistance));

        // y���W�͒n�ʂɍ��킹��
        jumpEndPos_.y = 0.0f;
        jumpAttackTime_ = 0.0f;
        isJumpAttack_ = true;

        return;
    }
}

// �`��
void BossEnemy::Draw()
{
    // �{�X�̕`�惍�W�b�N�����ʂłȂ���΂��̂܂܂�OK
    Enemy::Draw();
    VECTOR pos = VAdd(list.pos_, VGet(0.0f, 150.0f, 0.0f));
    DrawSphere3D(pos, 100.0f, 32, 0xffffff, 0xffffff, false);
    if (list.isAttack_)
    {
        VECTOR forward = VNorm(VGet(sinf(list.rot_.y), 0.0f, cosf(list.rot_.y)));
        VECTOR attackCenter = VAdd(GetRightHandPosition(), VScale(forward, 50.0f));
        DrawSphere3D(attackCenter, 120.0f, 32, 0xffffff, 0xffffff, false);
    }
    if (isJumpAttack_)
    {
        //�A�j���[�V�����̌��݈ʒu
        float currenttime = list.currentAnimTime_;
        float totaltime = list.animTotalTime_;
        float animratio = currenttime / totaltime;
        if (animratio >= 0.3f && animratio <= 0.6f) {

            // �G�̉E��̈ʒu���擾���ċ��̂��Z�b�g
            VECTOR forward = VNorm(VGet(sinf(list.rot_.y), 0.0f, cosf(list.rot_.y)));
            VECTOR rightattackCenter = VAdd(GetRightHandPosition(), VScale(forward, 50.0f));

            DrawSphere3D(GetRightHandPosition(), 70.0f, 32, 0xffffff, 0xffffff, false);
            DrawSphere3D(GetLeftHandPosition(), 70.0f, 32, 0xffffff, 0xffffff, false);
        }
    }
    if (list.isAttack_)
    {

    }
}

//�A�j���[�V�����Đ�
void BossEnemy::PlayAnimation()
{
    switch (list.animIndex_)
    {
    case ANIM_IDLE:
    case ANIM_WALK:
    case ANIM_RUN:
        list.currentAnimTime_ += BOSS_ANIM_SPEED;
        if (list.currentAnimTime_ > list.animTotalTime_)
        {
            list.currentAnimTime_ = 0.0f;
        }
        break;
    case ANIM_ATTACK:
        list.currentAnimTime_ += BOSS_ANIM_SPEED;
        if (list.currentAnimTime_ >= list.animTotalTime_)
        {
            list.currentAnimTime_ = list.animTotalTime_;
            list.isAnimLock_ = false;
            list.isAttack_ = false;
        }
        break;
    case ANIM_SWIP:
        list.currentAnimTime_ += BOSS_ANIM_SPEED;
        if (list.currentAnimTime_ >= list.animTotalTime_)
        {
            list.currentAnimTime_ = list.animTotalTime_;
            list.isAnimLock_ = false;
            isSwipe = false;
        }
        break;
    case ANIM_JUMP_ATTACK:
        list.currentAnimTime_ += BOSS_JUMP_ANIM_SPEED;
        if (list.currentAnimTime_ >= list.animTotalTime_)
        {
            list.currentAnimTime_ = list.animTotalTime_;
            list.isAnimLock_ = false;
            isJumpAttack_ = false;
        }
        break;
    case ANIM_DEAD:
        list.currentAnimTime_ += BOSS_DEAD_ANIM_SPEED;
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

// �A�j���[�V����
void BossEnemy::ChangeAnimation(int idx, bool lock)
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
