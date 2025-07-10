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
    // ���摜�̃T�C�Y�i���ۂ̉摜�T�C�Y�ɍ��킹�Ē����j
    hp_.ui_.graph_Width = 1747;
    hp_.ui_.graph_Height = 178;

    // ��ʏ�̕`��ʒu
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

    // �`�挳��X�ʒu�ƕ��i�E����MAX�Ȃ̂ō�������j
    int srcX = 0;
    int srcY = hp_.srcY_;
    int srcWidth = (int)(hp_.ui_.graph_Width * hpRatio);    //�E����؂�
    int srcHeight = hp_.ui_.graph_Height;

    // �`���̃T�C�Y�i��ʂɕ\������HP�o�[�̕��ƍ����j
    int drawWidth = (int)(300 * hpRatio);  // �\����̍ő啝 = 300px
    int drawHeight = 40;

    int dstX1 = hp_.ui_.graph_X;
    int dstY1 = hp_.ui_.graph_Y;
    int dstX2 = dstX1 + drawWidth;
    int dstY2 = dstY1 + drawHeight;

    // �`��
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
