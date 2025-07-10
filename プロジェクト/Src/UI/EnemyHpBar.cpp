#include "EnemyHpBar.h"
#include "../Object/Enemy.h"
#include "../Object/EnemyManager.h"

// コンストラクタ
EnemyHpBar::EnemyHpBar()
{
    enemymng_ = nullptr;
    handleId_ = -1;
}

// デストラクタ
EnemyHpBar::~EnemyHpBar()
{
}

// 初期化
void EnemyHpBar::Init(EnemyManager* manager)
{
    enemymng_ = manager;
}

// 更新処理
void EnemyHpBar::Update()
{
}

// 描画処理
void EnemyHpBar::Draw()
{
    if (!enemymng_) return;

    // 敵全体を取得
    const auto& enemies = enemymng_->GetEnemyPtrs();

    for (const auto& enemy : enemies)
    {
        if (!enemy || enemy->IsDead()) continue;

        if (CheckCameraViewClip(enemy->GetPosition()))
            continue;

        float hpRatio = (float)enemy->GetHP() / enemy->GetMaxHp();
        if (hpRatio < 0.0f) hpRatio = 0.0f;
        if (hpRatio > 1.0f) hpRatio = 1.0f;

        // 敵の頭上にオフセット
        VECTOR worldPos = enemy->GetPosition();
        worldPos.y += 100.0f; // 調整：必要に応じて110～130でもOK

        // スクリーン座標へ変換
        VECTOR screen = ConvWorldPosToScreenPos(worldPos);

        // HPバーの表示位置（スクリーン上）
        int x = static_cast<int>(screen.x) - 30;
        int y = static_cast<int>(screen.y) - 10;

        // HPバーサイズ
        int barWidth = static_cast<int>(60 * hpRatio);
        int barHeight = 8;

        // 描画
        DrawBox(x, y, x + barWidth, y + barHeight, GetColor(255, 0, 0), TRUE);  // 中身
        DrawBox(x, y, x + 60, y + barHeight, GetColor(255, 255, 255), FALSE);   // 枠線   
    }
}

// 解放処理（現状リソースなし）
void EnemyHpBar::Release()
{
}