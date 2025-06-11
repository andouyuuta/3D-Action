#include "Collision.h"
#include "Stage.h"
#include "Enemy.h"
#include "Sword.h"
#include "BossEnemy.h"

Collision::Collision()
{
    stageModel_ = -1;
    attackRadius_ = 0.0f;
    game_ = nullptr;
}
Collision::~Collision() {}

void Collision::Init()
{
    stageModel_ = Stage::GetInstance().GetStageModel();
}

void Collision::Update(const std::vector<Enemy*>& enemies)
{
    SwordToEnemyCollision(enemies);
    PlayerToEnemyCollision(enemies);
    EnemyToEnemyCollision(enemies);
    PlayerToStageCollision();
    EnemyToStageCollision(enemies);
}

void Collision::Draw(const std::vector<Enemy*>& enemies)
{
    DrawDebag();
    //for (auto& enemy : enemies)
    //{
    //    // 敵の右手の位置を取得して球体をセット
    //    VECTOR forward = VNorm(VGet(sinf(enemy->GetRotY()), 0.0f, cosf(enemy->GetRotY())));
    //    VECTOR attackCenter = VAdd(enemy->GetRightHandPosition(), VScale(forward, 50.0f));
    //    DrawSphere3D(attackCenter, 20.0f, 32, 0xffffff, 0xffffff, false);
    //    VECTOR pos = VAdd(enemy->GetPosition(), VGet(0.0f, 100.0f, 0.0f));
    //    DrawSphere3D(pos, 100.0f, 32, 0xffffff, 0xffffff, false);
    //}
}

void Collision::Release() {}

// プレイヤーとステージの衝突
void Collision::PlayerToStageCollision()
{
    // プレイヤーの衝突位置(少し上に)
    VECTOR playerpos = VAdd(Player::GetInstance().GetPlayerPos(), VGet(0.0f, 100.0f, 0.0f));
    auto result = MV1CollCheck_Sphere(stageModel_, -1, playerpos, 50.0f);

    // ヒットしたら押し戻す
    if (result.HitNum >= 1)
    {
        VECTOR pushBack = VScale(result.Dim[0].Normal, 15.0f);
        Player::GetInstance().SetPlayerPos(VAdd(Player::GetInstance().GetPlayerPos(), pushBack));
    }
    MV1CollResultPolyDimTerminate(result);
}

// 敵とステージの衝突
void Collision::EnemyToStageCollision(const std::vector<Enemy*>& enemies)
{
    for (auto& enemy : enemies)
    {
        if (enemy->IsDead()) continue;

        //当たり判定
        if (!enemy->GetIsBoss())
        {
            VECTOR pos = VAdd(enemy->GetPosition(), VGet(0.0f, 100.0f, 0.0f));
            auto result = MV1CollCheck_Sphere(stageModel_, -1, pos, 20.0f);     //通常敵
            // ヒットしたら押し戻す
            if (result.HitNum >= 1)
            {
                VECTOR pushBack = VScale(result.Dim[0].Normal, 10.0f);
                enemy->SetEnemyPos(VAdd(enemy->GetPosition(), pushBack));
            }
            MV1CollResultPolyDimTerminate(result);
        }
        else
        {
            VECTOR pos = VAdd(enemy->GetPosition(), VGet(0.0f, 150.0f, 0.0f));
            auto result = MV1CollCheck_Sphere(stageModel_, -1, pos, 100.0f);    //ボス敵
            // ヒットしたら押し戻す
            if (result.HitNum >= 1)
            {
                VECTOR pushBack = VScale(result.Dim[0].Normal, 10.0f);
                enemy->SetEnemyPos(VAdd(enemy->GetPosition(), pushBack));
            }
            MV1CollResultPolyDimTerminate(result);
        }
        
    }
}

//プレイヤーと敵の衝突
void Collision::PlayerToEnemyCollision(const std::vector<Enemy*>& enemies)
{
    // プレイヤーの位置取得（当たり判定位置を少し上に）
    VECTOR checkPos = VAdd(Player::GetInstance().GetPlayerPos(), VGet(0.0f, 100.0f, 0.0f));

    for (auto& enemy : enemies)
    {
        if (!enemy || enemy->IsDead()) continue; // 無効な敵・死亡した敵はスキップ

        // ===== 1. プレイヤーと敵の物理押し戻し処理 =====
        VECTOR dist = VSub(Player::GetInstance().GetPlayerPos(), enemy->GetPosition());
        dist.y = 0; // 高さ無視でXZ平面距離を見る

        if (VSize(dist) <= CHECK_RADIUS)
        {
            auto result = MV1CollCheck_Sphere(enemy->GetModel(), -1, checkPos, COLLISION_RADIUS);

            if (result.HitNum >= 1)
            {
                VECTOR pushBack = VScale(result.Dim[0].Normal, 7.0f); // 押し戻しベクトル
                Player::GetInstance().SetPlayerPos(VAdd(Player::GetInstance().GetPlayerPos(), pushBack));
            }
            MV1CollResultPolyDimTerminate(result);
        }

        // ===== 2. 通常攻撃の当たり判定（アニメーション中のみ） =====
        if (enemy->GetAttackFlg() && VSize(dist) <= CHECK_RADIUS)
        {
            float currentTime_ = enemy->GetInfo().currentAnimTime_;
            float totalTime_ = enemy->GetInfo().animTotalTime_;
            float animRatio_ = currentTime_ / totalTime_; // アニメーション進行度（0.0〜1.0）

            // 敵のスイング時にのみ当たり判定をつける
            if (animRatio_ >= 0.3f && animRatio_ <= 0.5f) // 攻撃判定タイミング
            {
                // 敵の前方方向ベクトル
                VECTOR forward = VNorm(VGet(sinf(enemy->GetRotY()), 0.0f, cosf(enemy->GetRotY())));
                VECTOR attackCenter = VAdd(enemy->GetRightHandPosition(), VScale(forward, 50.0f));

                // ボスの場合は攻撃範囲を広げる
                float radius = (dynamic_cast<BossEnemy*>(enemy)) ? 200.0f : 30.0f;

                // プレイヤーとの当たり判定（球体）
                auto result = MV1CollCheck_Sphere(Player::GetInstance().GetPlayerModel(), -1, attackCenter, radius);
                if (result.HitNum > 0)
                {
                        int damage = enemy->GetAttackPower();
                        Player::GetInstance().TakeDamage(damage); // ダメージ処理（内部で多重防止）
                }
                MV1CollResultPolyDimTerminate(result);
            }
        }

        // ===== 3. ボスのジャンプ攻撃（両手） =====
        if (BossEnemy* boss = dynamic_cast<BossEnemy*>(enemy))
        {
            if (boss->GetIsJumpAttack()) // ジャンプ攻撃中フラグ
            {
                float currentTime_ = enemy->GetInfo().currentAnimTime_;
                float totalTime_ = enemy->GetInfo().animTotalTime_;
                float animRatio_ = currentTime_ / totalTime_;

                if (animRatio_ >= 0.3f && animRatio_ <= 0.6f) // 攻撃タイミング中のみ
                {
                    // 両手で攻撃判定（球体）
                    auto rightHandResult_ = MV1CollCheck_Sphere(Player::GetInstance().GetPlayerModel(), -1, enemy->GetRightHandPosition(), 70.0f);
                    auto leftHandResult_ = MV1CollCheck_Sphere(Player::GetInstance().GetPlayerModel(), -1, enemy->GetLeftHandPosition(), 70.0f);

                    if (rightHandResult_.HitNum > 0 || leftHandResult_.HitNum > 0)
                    {
                        Player::GetInstance().TakeDamage(10); // 固定ダメージ（ジャンプ攻撃）
                    }

                    MV1CollResultPolyDimTerminate(rightHandResult_);
                    MV1CollResultPolyDimTerminate(leftHandResult_);
                }
            }
        }
    }
}

// 敵と敵の衝突
void Collision::EnemyToEnemyCollision(const std::vector<Enemy*> enemies)
{
    for (size_t i = 0; i < enemies.size(); ++i)
    {
        auto* enemyA = enemies[i];
        if (!enemyA || enemyA->IsDead()) continue;

        VECTOR posA = enemyA->GetPosition();

        for (size_t j = i + 1; j < enemies.size(); ++j)
        {
            auto* enemyB = enemies[j];
            if (!enemyB || enemyB->IsDead()) continue;

            VECTOR diff = VSub(enemyB->GetPosition(), posA);
            float dist = VSize(diff);

            // 指定距離未満で押し返す
            if (dist < COLLISION_RADIUS * 2.0f && dist > 0.0f)
            {
                VECTOR pushDir = VNorm(diff);
                VECTOR pushVec = VScale(pushDir, (COLLISION_RADIUS * 2.0f - dist) * 0.5f);

                enemyA->SetEnemyPos(VSub(posA, pushVec));
                enemyB->SetEnemyPos(VAdd(enemyB->GetPosition(), pushVec));
            }
        }
    }
}

// 剣と敵の衝突
void Collision::SwordToEnemyCollision(const std::vector<Enemy*>& enemies)
{
    // 武器を持っていない or 攻撃中ではない なら無視
    if (!Player::GetInstance().GetIsWeapon() || !Player::GetInstance().GetIsAttack())
        return;

    // 剣身の中心に球体をセット
    VECTOR swordBody = Sword::GetInstance().GetBodyPosition();

    for (auto& enemy : enemies)
    {
        if (!enemy || enemy->IsDead()) continue;

        //この攻撃で当たっていたらスキップ
        if (enemy->GetIsAssign()) continue;

        auto result = MV1CollCheck_Sphere(enemy->GetModel(), -1, swordBody, 30.0f);
        
        // 当たっていたらダメージを与える
        if (result.HitNum > 0)
        {
            // 敵に攻撃が当たったとき
            int attackPower = Player::GetInstance().GetAttackPower();
            int damage = attackPower;

            // 10%の確率でクリティカル
            if (GetRand(9) == 0)
            {
                damage *= 2;
                Player::GetInstance().SetCriticalDisplay(true);  
            }

            enemy->SetDamage(damage);
            // この攻撃中は当たらないようにする
            enemy->SetIsAssign(true);

            // ヒットストップ開始
            Player::GetInstance().StartHitStop(0.08f);
        }

        MV1CollResultPolyDimTerminate(result);
    }
}

//デバッグ描画
void Collision::DrawDebag()
{
    ////プレイヤーの真ん中
    //VECTOR playerPos = VAdd(Player::GetInstance().GetPlayerPos(), VGet(0.0f, 100.0f, 0.0f));
    //DrawSphere3D(playerPos, 20, 32, 0xffffff, 0xffffff, false);

    //// 剣の当たり判定
    //VECTOR swordBase = Sword::GetInstance().GetSwordPosition();
    //VECTOR forward = VNorm(VGet(sinf(Player::GetInstance().GetRotY()), 0.0f, cosf(Player::GetInstance().GetRotY())));
    //VECTOR swordCenter = VAdd(swordBase, VScale(forward, 50.0f));
    //swordCenter.y += 50.0f;
    //DrawSphere3D(swordCenter, 30.0f, 32, 0xffffff, 0xffffff, false);
}
