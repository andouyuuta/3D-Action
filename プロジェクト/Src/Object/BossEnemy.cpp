#include "BossEnemy.h"
#include <DxLib.h>
#include <corecrt_math.h>
#include "Player.h"
#include "../Manager/SceneManager.h"
#include "../Manager/AnimationManager.h"

BossEnemy::BossEnemy(void)
{
    isSwipe = false;
    isJumpAttack_ = false;
    jumpAttackTime_ = 0.0f;
    jumpStartPos_ = { 0.0f,0.0f,0.0f };
    jumpEndPos_ = { 0.0f,0.0f,0.0f };
}

BossEnemy::~BossEnemy(void) = default;

// ����������
void BossEnemy::Init(int bossModelID, int hp, int attack, Player* player, AnimationManager* anim, AnimationManager::AnimationInfo* animinfo)
{
    // Enemy::Init���Ăяo��
    Enemy::Init(bossModelID, hp, attack, player, anim, animinfo);
    animInfo_ = animinfo;
    InitBossStats();
    list.isBoss_ = true;
}
//�{�X�̃X�e�[�^�X�ύX
void BossEnemy::InitBossStats(void)
{
    list.scale_ = { 2.0f,2.0f,2.0f };             // �{�X�T�C�Y���g��
    MV1SetScale(list.modelId_, list.scale_);

    list.hp_ = BOSS_MAX_HP;                       // HP��啝�ɏ㏸
    list.moveSpeed_ = 3.0f;
    list.isBoss_ = true;
}

// �X�V����
void BossEnemy::Update(void)
{
    VECTOR toPlayer = VSub(player_->GetPlayerPos(), list.pos_);
    float dist = VSize(toPlayer);

    if (isJumpAttack_)
    {
        // animInfo_�͓G�ʂ̃A�j���[�V�������Ȃ̂ł�������g��
        float currentTime = animInfo_->currentAnimTime_;
        float totalTime = animInfo_->animTotalTime_;

        float jamptime = totalTime * JUMP_RATIO;
        float landtime = totalTime * LAND_RATIO;

        if (currentTime < jamptime)
        {
            // �W�����v����O�͈ړ��Ȃ�
        }
        else if (currentTime <= landtime)
        {
            // �󒆈ړ��t�F�[�Y
            float moveRate = (currentTime - jamptime) / (landtime - jamptime);
            list.pos_ = VAdd(VScale(VSub(jumpEndPos_, jumpStartPos_), moveRate), jumpStartPos_);
        }
        else
        {
            // ���n��
            list.pos_ = jumpEndPos_;
            list.pos_.y = 0.0f;
        }

        // ���f���X�V�ƃA�j������
        if (list.modelId_ != -1)
        {
            MV1SetAttachAnimTime(list.modelId_, animInfo_->animAttachNo_, currentTime);
            MV1SetPosition(list.modelId_, list.pos_);
            MV1SetRotationXYZ(list.modelId_, list.rot_);
        }

        // �W�����v�A�j���I���`�F�b�N
        if (currentTime >= totalTime)
        {
            isJumpAttack_ = false;
            jumpAttackTime_ = 0.0f;
            animation_->SetAnimLock(false);
            list.isAttack_ = false;
            animation_->EnemyChangeAnimation(this, animation_->EnemyAnim::ANIM_IDLE);
        }

        return; // �W�����v�U�������D��ŏI��
    }
    else
    {
        Enemy::Update();  // �ʏ�̍X�V����
    }

    // �W�����v�U���J�n����
    if (dist > 300.0f && dist < JUMP_ATTACK_RANGE)
    {
        animation_->EnemyChangeAnimation(this, animation_->EnemyAnim::ANIM_JUMP_ATTACK, true);

        jumpStartPos_ = list.pos_;
        jumpEndPos_ = player_->GetPlayerPos();

        VECTOR dirToPlayer = VNorm(VSub(jumpEndPos_, list.pos_));

        // �v���C���[�̌��ɏ������炷
        const float offsetDistance = 100.0f;
        jumpEndPos_ = VSub(jumpEndPos_, VScale(dirToPlayer, offsetDistance));

        // �n�ʂ̍����ɍ��킹��
        jumpEndPos_.y = 0.0f;

        jumpAttackTime_ = 0.0f;
        isJumpAttack_ = true;

        return;
    }
}

// �`��
void BossEnemy::Draw(void)
{
    // �{�X�̕`�惍�W�b�N�����ʂłȂ���΂��̂܂܂�OK
    Enemy::Draw();
    //VECTOR pos = VAdd(list.pos_, VGet(0.0f, 150.0f, 0.0f));
    //DrawSphere3D(pos, 100.0f, 32, 0xffffff, 0xffffff, false);
    //if (list.isAttack_)
    //{
    //    VECTOR forward = VNorm(VGet(sinf(list.rot_.y), 0.0f, cosf(list.rot_.y)));
    //    VECTOR attackCenter = VAdd(GetRightHandPosition(), VScale(forward, 50.0f));
    //    DrawSphere3D(attackCenter, 120.0f, 32, 0xffffff, 0xffffff, false);
    //}
    //if (isJumpAttack_)
    //{
    //    //�A�j���[�V�����̌��݈ʒu
    //    float currenttime = animation_->GetEnemyInfo().currentAnimTime_;
    //    float totaltime = animation_->GetEnemyInfo().animTotalTime_;
    //    float animratio = currenttime / totaltime;
    //    if (animratio >= 0.3f && animratio <= 0.6f) {

    //        // �G�̉E��̈ʒu���擾���ċ��̂��Z�b�g
    //        VECTOR forward = VNorm(VGet(-sinf(GetRotY()), 0.0f, -cosf(GetRotY())));
    //        VECTOR rightattackCenter = VAdd(GetRightHandPosition(), VScale(forward, 50.0f));
    //        VECTOR leftattackCenter = VAdd(GetLeftHandPosition(), VScale(forward, 50.0f));

    //        DrawSphere3D(rightattackCenter, 100.0f, 32, 0xffffff, 0xffffff, false);
    //        DrawSphere3D(leftattackCenter, 100.0f, 32, 0xffffff, 0xffffff, false);
    //    }
    //}
}