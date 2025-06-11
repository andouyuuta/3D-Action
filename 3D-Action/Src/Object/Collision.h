#pragma once
#include <DxLib.h>
#include <vector>
#include "Player.h"

class Enemy;
class Game;

class Collision
{
public:
    static constexpr float COLLISION_RADIUS = 20.0f;    // 精密な当たり判定半径
    static constexpr float CHECK_RADIUS = 100.0f;       // 簡易な距離チェック用

    Collision();
    ~Collision();

    void Init();
    void Update(const std::vector<Enemy*>& enemies);
    void Draw(const std::vector<Enemy*>& enemies);
    void Release();

private:
    int stageModel_;
    float attackRadius_;

    // 各種判定処理
    void PlayerToStageCollision();                                      // プレイヤーとステージ
    void EnemyToStageCollision(const std::vector<Enemy*>& enemies);     // 敵とステージ
    void PlayerToEnemyCollision(const std::vector<Enemy*>& enemies);    // プレイヤーと敵
    void EnemyToEnemyCollision(const std::vector<Enemy*> enemies);      // 敵と敵
    void SwordToEnemyCollision(const std::vector<Enemy*>& enemies);     // 剣と敵

    // デバッグ描画
    void DrawDebag();

    Game* game_;
};
