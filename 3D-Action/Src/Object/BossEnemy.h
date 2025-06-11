#pragma once
#include "Enemy.h"

class BossEnemy : public Enemy
{
public:
    static constexpr float JUMP_RATIO = 0.1f;           // ジャンプしたときのアニメーション割合
    static constexpr float LAND_RATIO = 0.4f;           // 着地したときのアニメーション割合
    static constexpr float JUMP_ATTACK_RANGE = 500.0f;  // 飛びかかり発動距離
    static constexpr float JUMP_ATTACK_DURATION = 0.8f;
    static constexpr float BOSS_MAX_HP = 1500.0f;

    // 初期モデル補正角度
    static constexpr VECTOR INIT_MODEL_ROT_OFFSET = { 0.0f,DX_PI_F / 2.0f,0.0f };

    static constexpr float BOSS_ANIM_SPEED = 1.0f;      //アニメーションスピード
    static constexpr float BOSS_DEAD_ANIM_SPEED = 0.5f; //死亡
    static constexpr float BOSS_JUMP_ANIM_SPEED = 0.8f;	// ジャンプ攻撃

    BossEnemy();
    ~BossEnemy() override;

    void Init(int bossModelID, int hp, int attack) override;  // モデル読み込みやアニメーション初期化をオーバーライド
    void Update() override;               // 更新処理
    void Draw() override;                 // 描画

    bool GetIsJumpAttack() { return isJumpAttack_; }

private:
    void InitBossStats();                 // ボス専用の初期設定

    bool isSwipe;
    bool isJumpAttack_ ;
    float jumpAttackTime_;
    VECTOR jumpStartPos_;
    VECTOR jumpEndPos_;

    void PlayAnimation()override;
    void ChangeAnimation(int idx, bool lock = false)override;	//アニメーション切り替え
};
