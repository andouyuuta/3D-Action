#pragma once
#include "Enemy.h"

class Player;

class BossEnemy : public Enemy
{
public:
    // �X�e�[�^�X
    static constexpr float BOSS_MAX_HP = 1500.0f;
    static constexpr float JUMP_ATTACK_RANGE = 500.0f;
    static constexpr float JUMP_ATTACK_DURATION = 0.8f;

    // �A�j���[�V��������
    static constexpr float JUMP_RATIO = 0.1f;
    static constexpr float LAND_RATIO = 0.4f;

    // �A�j���[�V�����X�s�[�h
    static constexpr float BOSS_ANIM_SPEED = 1.0f;
    static constexpr float BOSS_DEAD_ANIM_SPEED = 0.5f;
    static constexpr float BOSS_JUMP_ANIM_SPEED = 0.8f;

    // �������f���␳�p�x
    static constexpr VECTOR INIT_MODEL_ROT_OFFSET = { 0.0f, DX_PI_F / 2.0f, 0.0f };

    // === �R���X�g���N�^�^�f�X�g���N�^ ===
    BossEnemy(void);
    ~BossEnemy(void) override;

    // === �I�[�o�[���C�h�֐� ===
    void Init(int bossModelID, int hp, int attack, Player* player, AnimationManager* anim, AnimationManager::AnimationInfo* animinfo) override;
    void Update(void) override;
    void Draw(void) override;

    // === �Q�b�^�[ ===
    [[nodiscard]] bool GetIsJumpAttack(void) const { return isJumpAttack_; }

private:
    void InitBossStats(void);  // �{�X�ŗL�����ݒ�

    // === ��ԕϐ� ===
    bool isSwipe;
    bool isJumpAttack_;
    float jumpAttackTime_;

    VECTOR jumpStartPos_;
    VECTOR jumpEndPos_;
};
