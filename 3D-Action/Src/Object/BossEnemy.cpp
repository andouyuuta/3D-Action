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

// 初期化処理
void BossEnemy::Init(int bossModelID, int hp, int attack)
{
    // 通常の敵の初期化処理を利用
    Enemy::Init(bossModelID, hp, attack); // EnemyのInitを呼ぶ
    InitBossStats();                     // ボス特有の設定
    list.isBoss_ = true;
}

//ボスのステータス変更
void BossEnemy::InitBossStats()
{
    list.scale_ = { 2.0f,2.0f,2.0f };             // ボスサイズを拡大
    MV1SetScale(list.modelId_, list.scale_);

    list.hp_ = BOSS_MAX_HP;                       // HPを大幅に上昇
    list.moveSpeed_ = 3.0f;
    list.isBoss_ = true;
}

// 更新処理
void BossEnemy::Update()
{
    VECTOR toPlayer = VSub(Player::GetInstance().GetPlayerPos(), list.pos_);
    //プレイヤーとの距離計算
    float dist = VSize(toPlayer);

    //ジャンプ攻撃
    if (isJumpAttack_)
    {
        PlayAnimation();
        float jumpattackrate = list.currentAnimTime_ / list.animTotalTime_;

        float jamptime = list.animTotalTime_ * JUMP_RATIO;  // ジャンプした時間(割合)
        float landtime = list.animTotalTime_ * LAND_RATIO;  // 着地する時間(割合)

        if (list.currentAnimTime_ < jamptime)
        {
            // ジャンプアニメーションのジャンプする前は移動しない
        }
        else if (list.currentAnimTime_ <= landtime)
        {
            // 空中移動フェーズ
            float moveRate = (list.currentAnimTime_ - jamptime) / (landtime - jamptime);
            list.pos_ = VAdd(VScale(VSub(jumpEndPos_, jumpStartPos_), moveRate), jumpStartPos_);
        }
        else
        {
            // 着地後
            list.pos_ = jumpEndPos_;
            list.pos_.y = 0.0f;
        }

        // モデル更新とアニメ同期
        MV1SetAttachAnimTime(list.modelId_, list.animAttachNo_, list.currentAnimTime_);
        MV1SetPosition(list.modelId_, list.pos_);
        MV1SetRotationXYZ(list.modelId_, list.rot_);

        // 位置更新（ジャンプ移動）
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

    //ジャンプ攻撃する範囲
    if (dist > 300.0f && dist < JUMP_ATTACK_RANGE)
    {
        ChangeAnimation(EnemyAnim::ANIM_JUMP_ATTACK, true);

        jumpStartPos_ = list.pos_;
        jumpEndPos_ = Player::GetInstance().GetPlayerPos();
        // プレイヤーからボスへのベクトル（進行方向の逆）
        VECTOR toPlayer = VNorm(VSub(jumpEndPos_, list.pos_));

        // プレイヤーの後ろ方向にオフセット（例：100.0f ユニット）
        float offsetDistance = 100.0f;
        jumpEndPos_ = VSub(jumpEndPos_, VScale(toPlayer, offsetDistance));

        // y座標は地面に合わせる
        jumpEndPos_.y = 0.0f;
        jumpAttackTime_ = 0.0f;
        isJumpAttack_ = true;

        return;
    }
}

// 描画
void BossEnemy::Draw()
{
    // ボスの描画ロジックが特別でなければそのままでOK
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
        //アニメーションの現在位置
        float currenttime = list.currentAnimTime_;
        float totaltime = list.animTotalTime_;
        float animratio = currenttime / totaltime;
        if (animratio >= 0.3f && animratio <= 0.6f) {

            // 敵の右手の位置を取得して球体をセット
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

//アニメーション再生
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

// アニメーション
void BossEnemy::ChangeAnimation(int idx, bool lock)
{
    if (list.animIndex_ != idx) {
        MV1DetachAnim(list.modelId_, list.animAttachNo_);
        list.animIndex_ = idx;
        list.animAttachNo_ = MV1AttachAnim(list.modelId_, list.animIndex_);
        list.animTotalTime_ = MV1GetAttachAnimTotalTime(list.modelId_, list.animAttachNo_);
        list.currentAnimTime_ = 0.0f;
        MV1SetAttachAnimTime(list.modelId_, list.animAttachNo_, list.currentAnimTime_);
        //ロック指定されたらフラグON
        list.isAnimLock_ = lock;
    }
}
