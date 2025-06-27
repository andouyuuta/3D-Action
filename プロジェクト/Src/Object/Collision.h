#pragma once
#include <DxLib.h>
#include <vector>
#include "Player.h"
#include "../Manager/AnimationManager.h"

class Enemy;
class Game;
class Player;
class Sword;
class Stage;
class AnimationManager;

class Collision
{
public:
    static constexpr float COLLISION_RADIUS = 20.0f;    // �����ȓ����蔻�蔼�a
    static constexpr float CHECK_RADIUS = 100.0f;       // �ȈՂȋ����`�F�b�N�p

    Collision(void);
    ~Collision(void);

    void Init(Player* player, Sword* sword, Stage* stage, AnimationManager* anim);
    void Update(const std::vector<Enemy*>& enemies);
    void Draw(const std::vector<Enemy*>& enemies);
    void Release(void);

private:
    void PlayerToStageCollision();                                      // �v���C���[�ƃX�e�[�W
    void EnemyToStageCollision(const std::vector<Enemy*>& enemies);     // �G�ƃX�e�[�W
    void PlayerToEnemyCollision(const std::vector<Enemy*>& enemies);    // �v���C���[�ƓG
    void EnemyToEnemyCollision(const std::vector<Enemy*> enemies);      // �G�ƓG
    void SwordToEnemyCollision(const std::vector<Enemy*>& enemies);     // ���ƓG

    // �f�o�b�O�`��
    void DrawDebag();

    int stageModel_;
    float attackRadius_;

    Game* game_;
    Player* player_;
    Sword* sword_;
    Stage* stage_;
    AnimationManager* anim_;

    template<typename T>
    T Clamp(const T& value, const T& min, const T& max)
    {
        if (value < min) return min;
        else if (value > max) return max;
        else return value;
    }
};
