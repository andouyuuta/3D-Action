#pragma once
#include "UIBase.h"

class Player;

class HpBar :UIBase
{
public:
    HpBar(void);
    virtual ~HpBar(void);

    void Init(Player* player);
    void Update(void) override;
    void Draw(void) override;
    void Release(void) override;

private:
    struct HpGauge
    {
        UIInfo ui_;
        int handleframe_;
        int srcX_;
        int srcY_;
    }hp_;

    Player* player_;
};

