#pragma once
#include "UIBase.h"
#include "../Object/Player.h"
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <array>

class EnemyManager;

class Buff : public UIBase
{
public:
    static constexpr int NUM_BUFFS = 10;         // �o�t�̎��
    static constexpr int DRAW_BUFF = 3;         // �`�悷�鐔(�ύX���Ȃ�)
    static constexpr float SCALE = 0.75f;       // �`��X�P�[��
    static constexpr int SPACING = 50;          // �摜���m�̊Ԋu
    static constexpr int STICK_THRESHOLD = 600; // �X�e�B�b�N�̂������l

    Buff(void);
    virtual ~Buff(void);

    // �������E�X�V�E�`��E���
    void Init(Player* player, EnemyManager* mng);
    void Update(void) override;
    void Draw(void) override;
    void Release(void) override;

    void Reset(void);
    void ApplyBuffSelect(int index);

    // ��Ԋm�F
    bool IsSelected(void) const;
    bool IsCanExit(void) const;

private:
    void MouseSelect(void);
    void ControllerSelect(void);
    void ShuffleNumber(void);

    Player* player_;
    EnemyManager* enemymng_;

    std::array<UIInfo, DRAW_BUFF> buffUIInfos_; // UIInfo���R�Ǘ�
    std::vector<int> number_;                   // �o���m���̒��I�o�tID

    int selectHandle_;

    int selectedIndex_;
    int stickCoolTime_;                          // �X�e�B�b�N�A�����͖h�~�̃N�[���^�C��

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