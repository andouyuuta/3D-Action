#pragma once
#include "UIBase.h"
#include "../Object/Player.h"
#include <algorithm>
#include <array>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

class EnemyManager;

class Buff : public UIBase
{
public:
    static constexpr int NUM_BUFFS = 5;         // バフの種類
    static constexpr int DRAW_BUFF = 3;         // 描画する数(変更しない)
    static constexpr float SCALE = 0.75f;       // 描画スケール
    static constexpr int SPACING = 50;          // 画像同士の間隔
    static constexpr int STICK_THRESHOLD = 600; // スティックのしきい値

    Buff(void);
    virtual ~Buff(void);

    // 初期化・更新・描画・解放
    void Init(Player* player, EnemyManager* mng);
    void Update(void) override;
    void Draw(void) override;
    void Release(void) override;

    void Reset(void);
    void ApplyBuffSelect(int index);

    // 状態確認
    bool IsSelected(void) const;
    bool IsCanExit(void) const;

private:
    void MouseSelect(void);
    void ControllerSelect(void);
    void ShuffleNumber(void);

    Player* player_;
    EnemyManager* enemymng_;

    std::array<int, NUM_BUFFS> number_;          // 表示順だけを管理

    int selectedIndex_;
    int stickCoolTime_;                          // スティック連続入力防止のクールタイム

    int lx;
    int mouseX_, mouseY_;
    float drawW, drawH;

    float totalWidth;
    float startX;
    float x, y;

    bool isSelected_;
    bool canExit_;
    bool isUseMouse_;
    int prevMouseX_, prevMouseY_;
};