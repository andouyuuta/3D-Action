#pragma once
#include "Enemy.h"

class BossEnemy : public Enemy
{
public:
    static constexpr float JUMP_RATIO = 0.1f;           // �W�����v�����Ƃ��̃A�j���[�V��������
    static constexpr float LAND_RATIO = 0.4f;           // ���n�����Ƃ��̃A�j���[�V��������
    static constexpr float JUMP_ATTACK_RANGE = 500.0f;  // ��т����蔭������
    static constexpr float JUMP_ATTACK_DURATION = 0.8f;
    static constexpr float BOSS_MAX_HP = 1500.0f;

    // �������f���␳�p�x
    static constexpr VECTOR INIT_MODEL_ROT_OFFSET = { 0.0f,DX_PI_F / 2.0f,0.0f };

    static constexpr float BOSS_ANIM_SPEED = 1.0f;      //�A�j���[�V�����X�s�[�h
    static constexpr float BOSS_DEAD_ANIM_SPEED = 0.5f; //���S
    static constexpr float BOSS_JUMP_ANIM_SPEED = 0.8f;	// �W�����v�U��

    BossEnemy();
    ~BossEnemy() override;

    void Init(int bossModelID, int hp, int attack) override;  // ���f���ǂݍ��݂�A�j���[�V�������������I�[�o�[���C�h
    void Update() override;               // �X�V����
    void Draw() override;                 // �`��

    bool GetIsJumpAttack() { return isJumpAttack_; }

private:
    void InitBossStats();                 // �{�X��p�̏����ݒ�

    bool isSwipe;
    bool isJumpAttack_ ;
    float jumpAttackTime_;
    VECTOR jumpStartPos_;
    VECTOR jumpEndPos_;

    void PlayAnimation()override;
    void ChangeAnimation(int idx, bool lock = false)override;	//�A�j���[�V�����؂�ւ�
};
