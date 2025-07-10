#include "PlayerHpBar.h"
#include "../Object/Player.h"

PlayerHpBar::PlayerHpBar(void)
{
    player_ = nullptr;
    hp_ = {};
}

PlayerHpBar::~PlayerHpBar(void)
{
}

void PlayerHpBar::Init(Player* player)
{
	player_ = player;
    hp_.ui_.handleId_ = LoadGraph("Data/Image/PlayerGauge.png");
    hp_.handleframe_ = LoadGraph("Data/Image/PlayerGaugeFrame.png");
    // 元画像のサイズ（実際の画像サイズに合わせて調整）
    hp_.ui_.graph_Width = 1747;
    hp_.ui_.graph_Height = 178;

    // 画面上の描画位置
    hp_.ui_.graph_X = 20;
    hp_.ui_.graph_Y = 10;
}

void PlayerHpBar::Update(void)
{
}

void PlayerHpBar::Draw(void)
{
    if (!player_) return;

    float hpRatio = (float)player_->GetHp() / player_->GetMaxHp();
    if (hpRatio < 0.0f) hpRatio = 0.0f;
    if (hpRatio > 1.0f) hpRatio = 1.0f;

    // 描画元のX位置と幅（右側がMAXなので左から削る）
    int srcX = 0;
    int srcY = hp_.srcY_;
    int srcWidth = (int)(hp_.ui_.graph_Width * hpRatio);    //右側を切る
    int srcHeight = hp_.ui_.graph_Height;

    // 描画先のサイズ（画面に表示するHPバーの幅と高さ）
    int drawWidth = (int)(300 * hpRatio);  // 表示上の最大幅 = 300px
    int drawHeight = 40;

    int dstX1 = hp_.ui_.graph_X;
    int dstY1 = hp_.ui_.graph_Y;
    int dstX2 = dstX1 + drawWidth;
    int dstY2 = dstY1 + drawHeight;

    // 描画
    DrawRectExtendGraph(
        dstX1, dstY1,
        dstX2, dstY2,
        srcX, srcY,
        srcWidth, srcHeight,
        hp_.ui_.handleId_,
        TRUE
    );
    DrawExtendGraph(dstX1, dstY1, 325, 50, hp_.handleframe_, true);
}

void PlayerHpBar::Release()
{
    DeleteGraph(hp_.ui_.handleId_);
    hp_.ui_.handleId_ = -1;
}
