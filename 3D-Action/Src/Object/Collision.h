#pragma once
#include <DxLib.h>
#include <vector>
#include "Player.h"

class Enemy;
class Game;

class Collision
{
public:
    static constexpr float COLLISION_RADIUS = 20.0f;    // �����ȓ����蔻�蔼�a
    static constexpr float CHECK_RADIUS = 100.0f;       // �ȈՂȋ����`�F�b�N�p

    Collision();
    ~Collision();

    void Init();
    void Update(const std::vector<Enemy*>& enemies);
    void Draw(const std::vector<Enemy*>& enemies);
    void Release();

private:
    int stageModel_;
    float attackRadius_;

    // �e�픻�菈��
    void PlayerToStageCollision();                                      // �v���C���[�ƃX�e�[�W
    void EnemyToStageCollision(const std::vector<Enemy*>& enemies);     // �G�ƃX�e�[�W
    void PlayerToEnemyCollision(const std::vector<Enemy*>& enemies);    // �v���C���[�ƓG
    void EnemyToEnemyCollision(const std::vector<Enemy*> enemies);      // �G�ƓG
    void SwordToEnemyCollision(const std::vector<Enemy*>& enemies);     // ���ƓG

    // �f�o�b�O�`��
    void DrawDebag();

    Game* game_;
};
