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
    EnemyManager* enemymng_;;  // �����̓G�ɃA�N�Z�X���邽��
    int handleId_;
};
