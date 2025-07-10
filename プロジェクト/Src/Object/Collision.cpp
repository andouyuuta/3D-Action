#include "Collision.h"
#include "Stage.h"
#include "Enemy.h"
#include "Sword.h"
#include "BossEnemy.h"
#include "Stage.h"
#include "../Manager/RandomManager.h"
#include "../Manager/AnimationManager.h"
#include "../Manager/SoundManager.h"
#include "../Application.h"

Collision::Collision()
{
    stageModel_ = -1;
    attackRadius_ = 0.0f;
    game_ = nullptr;
    player_ = nullptr;
    stage_ = nullptr;
    anim_ = nullptr;
    sword_ = nullptr;
}

Collision::~Collision() {}

void Collision::Init(Player* player,Sword* sword,Stage* stage,AnimationManager* anim)
{
    player_ = player;
    sword_ = sword;
    stage_ = stage;
    stageModel_ = stage_->GetStageModel();
    anim_ = anim;

    // シングルトンの呼び出し
    SoundManager* seMana_ = SoundManager::GetInstance();

    //SEのロード
    seMana_->LoadSE("Damage", (Application::PATH_SOUND + "SE/Player_damage.mp3").c_str(), false);
    seMana_->LoadSE("Attack", (Application::PATH_SOUND + "SE/Attack_sword.mp3").c_str(), false);
    seMana_->LoadSE("Sword", (Application::PATH_SOUND + "SE/sword.mp3").c_str(), false);
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
    //DrawDebag();
}

void Collision::Release(void) 
{
    // シングルトンの呼び出し
    SoundManager* seMana_ = SoundManager::GetInstance();

    // SEの停止
    seMana_->StopSE("Damage");
    seMana_->StopSE("Attack");
    seMana_->StopSE("Sword");

    // SEの解放
    seMana_->ReleaseSound("Damage");
    seMana_->ReleaseSound("Attack");
    seMana_->ReleaseSound("Sword");
}

// プレイヤーとステージの衝突
void Collision::PlayerToStageCollision()
{
    // プレイヤーの衝突位置(少し上に)
    VECTOR playerpos = VAdd(player_->GetPlayerPos(), VGet(0.0f, 100.0f, 0.0f));
    auto result = MV1CollCheck_Sphere(stageModel_, -1, playerpos, 50.0f);

    // ヒットしたら押し戻す
    if (result.HitNum >= 1)
    {
        VECTOR pushBack = VScale(result.Dim[0].Normal, 15.0f);
        player_->SetPlayerPos(VAdd(player_->GetPlayerPos(), pushBack));
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
    // シングルトンの呼び出し
    SoundManager* seMana_ = SoundManager::GetInstance();

    // プレイヤーの位置取得（当たり判定位置を少し上に）
    VECTOR checkPos = VAdd(player_->GetPlayerPos(), VGet(0.0f, 100.0f, 0.0f));

    for (auto& enemy : enemies)
    {
        if (!enemy || enemy->IsDead()) continue; // 無効な敵・死亡した敵はスキップ

        // ===== 1. プレイヤーと敵の物理押し戻し処理 =====
        VECTOR dist = VSub(player_->GetPlayerPos(), enemy->GetPosition());
        dist.y = 0; // 高さ無視でXZ平面距離を見る

        if (VSize(dist) <= CHECK_RADIUS)
        {
            auto result = MV1CollCheck_Sphere(enemy->GetModel(), -1, checkPos, COLLISION_RADIUS);

            if (result.HitNum >= 1)
            {
                VECTOR pushDir = VNorm(dist);
                float overlap = CHECK_RADIUS - VSize(dist);
                float pushPower = Clamp(overlap * 0.2f, 0.0f, 5.0f); // 押し戻しベクトル
                VECTOR pushVec = VScale(pushDir, pushPower);
                player_->SetPlayerPos(VAdd(player_->GetPlayerPos(), VScale(pushVec, 0.5f)));
                enemy->SetEnemyPos(VSub(enemy->GetPosition(), VScale(pushVec, 0.5f)));
            }
            MV1CollResultPolyDimTerminate(result);
        }
        // ===== 2. 通常攻撃の当たり判定（アニメーション中のみ） =====
        if (enemy->GetAttackFlg() && VSize(dist) <= CHECK_RADIUS)
        {
            float currentTime_ = enemy->GetAnimInfo()->currentAnimTime_;
            float totalTime_ = enemy->GetAnimInfo()->animTotalTime_;            
            float animRatio_ = currentTime_ / totalTime_; // アニメーション進行度（0.0〜1.0）

            // 敵のスイング時にのみ当たり判定をつける
            if (animRatio_ >= 0.3f && animRatio_ <= 0.5f) // 攻撃判定タイミング
            {
                // 敵の前方方向ベクトル
                VECTOR forward = VNorm(VGet(sinf(enemy->GetRotY()), 0.0f, cosf(enemy->GetRotY())));
                VECTOR attackCenter = VAdd(enemy->GetRightHandPosition(), VScale(forward, 50.0f));

                // ボスの場合は攻撃範囲を広げる
                float radius = (dynamic_cast<BossEnemy*>(enemy)) ? 200.0f : 50.0f;

                // プレイヤーとの当たり判定（球体）
                auto result = MV1CollCheck_Sphere(player_->GetPlayerModel(), -1, attackCenter, radius);
                if (result.HitNum > 0)
                {
                    int damage = enemy->GetAttackPower();
                    if (anim_->GetPlayerInfo().animIndex_ == anim_->Crouch_Idle)
                    {
                        anim_->PlayerChangeAnimation(anim_->Crouch_Guard, true);
                        player_->TakeDamage(damage / 4); // ダメージ処理（内部で多重防止）

                    }
                    else 
                    {
                        player_->TakeDamage(damage); // ダメージ処理（内部で多重防止）
                    }
                    seMana_->PlaySE("Damage");
                }
                MV1CollResultPolyDimTerminate(result);
            }
        }

        // ===== 3. ボスのジャンプ攻撃（両手） =====
        if (BossEnemy* boss = dynamic_cast<BossEnemy*>(enemy))
        {
            if (boss->GetIsJumpAttack()) // ジャンプ攻撃中フラグ
            {
                float currentTime_ = enemy->GetAnimInfo()->currentAnimTime_;
                float totalTime_ = enemy->GetAnimInfo()->animTotalTime_;               
                float animRatio_ = currentTime_ / totalTime_;

                if (animRatio_ >= 0.3f && animRatio_ <= 0.6f) // 攻撃タイミング中のみ
                {
                    // 敵の右手の位置を取得して球体をセット
                    VECTOR forward = VNorm(VGet(-sinf(enemy->GetRotY()), 0.0f, -cosf(enemy->GetRotY())));
                    VECTOR rightattackCenter = VAdd(enemy->GetRightHandPosition(), VScale(forward, 50.0f));
                    VECTOR leftattackCenter = VAdd(enemy->GetLeftHandPosition(), VScale(forward, 50.0f));

                    // 両手で攻撃判定（球体）
                    auto rightHandResult_ = MV1CollCheck_Sphere(player_->GetPlayerModel(), -1, rightattackCenter, 100.0f);
                    auto leftHandResult_ = MV1CollCheck_Sphere(player_->GetPlayerModel(), -1, leftattackCenter, 100.0f);

                    if (rightHandResult_.HitNum > 0 || leftHandResult_.HitNum > 0)
                    {
                        int damage = enemy->GetAttackPower();

                        if (anim_->GetPlayerInfo().animIndex_ == anim_->Crouch_Idle)
                        {
                            anim_->PlayerChangeAnimation(anim_->Crouch_Guard, true);
                            player_->TakeDamage(static_cast<int>(damage * 1.5) / 4); // しゃがみ中は1/4のダメージ
                        }
                        else
                        {
                            player_->TakeDamage(static_cast<int>(damage * 1.5)); // 固定ダメージ（ジャンプ攻撃）
                        }
                        seMana_->PlaySE("Damage");
                    }

                    MV1CollResultPolyDimTerminate(rightHandResult_);
                    MV1CollResultPolyDimTerminate(leftHandResult_);
                    seMana_->StopSE("Damage");
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
    // シングルトンの呼び出し
    SoundManager* seMana_ = SoundManager::GetInstance();

    // === Ult演出が終了していた場合：敵全体にダメージ ===
    if (player_->GetIsUltFinish())
    {
        int baseDamage = static_cast<float>(player_->GetAttackPower());

        for (auto& enemy : enemies)
        {
            if (!enemy || enemy->IsDead()) continue;

            // 一度のUltで複数回ダメージを与えないためチェック
            if (enemy->GetIsAssign()) continue;

            int damage = baseDamage * 2;
            enemy->SetDamage(damage);
            enemy->SetIsAssign(true);
        }

        // ヒットストップ（全体に一律）
        player_->StartHitStop(0.15f);

        // フラグリセット（再実行防止）
        player_->SetIsUltFinish(false);

        return;
    }

    // 武器を持っていない or 攻撃中ではない なら無視
    if (!player_->GetIsWeapon() || !player_->GetIsAttack())
        return;

    // 剣身の中心に球体をセット
    VECTOR swordBody = sword_->GetBodyPosition();
   
    // === 通常の近接攻撃 ===
    for (auto& enemy : enemies)
    {
        // SEの再生
        seMana_->PlaySE("Sword");

        if (!enemy || enemy->IsDead()) continue;

        // この攻撃で当たっていたらスキップ
        if (enemy->GetIsAssign()) continue;

        float radius = (dynamic_cast<BossEnemy*>(enemy)) ? 50.0f : 30.0f;

        auto result = MV1CollCheck_Sphere(enemy->GetModel(), -1, swordBody, radius);

        // 当たっていたらダメージを与える
        if (result.HitNum > 0)
        {
            int damage = player_->GetAttackPower();


            // SEの再生
            seMana_->StopSE("Sword");
            seMana_->PlaySE("Attack");

            // クリティカル判定
            if (RandomManager::Random(player_->GetCriticalRate()))
            {
                damage *= player_->GetCriticalDamage();
                player_->SetCriticalDisplay(true);
            }

            enemy->SetDamage(damage);
            enemy->SetIsAssign(true);

            // ヒットストップ
            player_->StartHitStop(0.08f);
        }

        MV1CollResultPolyDimTerminate(result);
    }
  

    if (player_->GetIsDead())
    {
        // SEの停止
        seMana_->StopSE("Sword");
        seMana_->StopSE("Attack");
    }
}

//デバッグ描画
void Collision::DrawDebag(void)
{
    DrawFormatString(0, 400, 0xffffff, "プレイヤーアニメーション番号：%d", anim_->GetPlayerInfo().animIndex_);
}