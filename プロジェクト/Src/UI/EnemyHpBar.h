#pragma once
#include "UIBase.h"
#include <vector>

class Enemy;
class EnemyManager;

class EnemyHpBar
{
public:
    EnemyHpBar(void);
    ~EnemyHpBar(void);

    void Init(EnemyManager* manager);
    void Update(void);
    void Draw(void);
    void Release(void);

private:
    EnemyManager* enemymng_;;  // 複数の敵にアクセスするため
    int handleId_;
};
