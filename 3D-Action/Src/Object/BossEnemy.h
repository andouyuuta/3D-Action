#pragma once
#include "Enemy.h"

class Player;

class BossEnemy : public Enemy
{
public:
    // ステータス
    static constexpr float BOSS_MAX_HP = 1500.0f;
    static constexpr float JUMP_ATTACK_RANGE = 500.0f;
    static constexpr float JUMP_ATTACK_DURATION = 0.8f;

    // アニメーション割合
    static constexpr float JUMP_RATIO = 0.1f;
    static constexpr float LAND_RATIO = 0.4f;

    // アニメーションスピード
    static constexpr float BOSS_ANIM_SPEED = 1.0f;
    static constexpr float BOSS_DEAD_ANIM_SPEED = 0.5f;
    static constexpr float BOSS_JUMP_ANIM_SPEED = 0.8f;

    // 初期モデル補正角度
    static constexpr VECTOR INIT_MODEL_ROT_OFFSET = { 0.0f, DX_PI_F / 2.0f, 0.0f };

    // === コンストラクタ／デストラクタ ===
    BossEnemy(void);
    ~BossEnemy(void) override;

    // === オーバーライド関数 ===
    void Init(int bossModelID, int hp, int attack, Player* player, AnimationManager* anim, AnimationManager::AnimationInfo* animinfo) override;
    void Update(void) override;
    void Draw(void) override;

    // === ゲッター ===
    [[nodiscard]] bool GetIsJumpAttack(void) const { return isJumpAttack_; }

private:
    void InitBossStats(void);  // ボス固有初期設定

    // === 状態変数 ===
    bool isSwipe;
    bool isJumpAttack_;
    float jumpAttackTime_;

    VECTOR jumpStartPos_;
    VECTOR jumpEndPos_;
};
