#include "EnemyHpBar.h"
#include "../Object/Enemy.h"
#include "../Object/EnemyManager.h"

// �R���X�g���N�^
EnemyHpBar::EnemyHpBar()
{
    enemymng_ = nullptr;
    handleId_ = -1;
}

// �f�X�g���N�^
EnemyHpBar::~EnemyHpBar()
{
}

// ������
void EnemyHpBar::Init(EnemyManager* manager)
{
    enemymng_ = manager;
}

// �X�V����
void EnemyHpBar::Update()
{
}

// �`�揈��
void EnemyHpBar::Draw()
{
    if (!enemymng_) return;

    // �G�S�̂��擾
    const auto& enemies = enemymng_->GetEnemyPtrs();

    for (const auto& enemy : enemies)
    {
        if (!enemy || enemy->IsDead()) continue;

        if (CheckCameraViewClip(enemy->GetPosition()))
            continue;

        float hpRatio = (float)enemy->GetHP() / enemy->GetMaxHp();
        if (hpRatio < 0.0f) hpRatio = 0.0f;
        if (hpRatio > 1.0f) hpRatio = 1.0f;

        // �G�̓���ɃI�t�Z�b�g
        VECTOR worldPos = enemy->GetPosition();
        worldPos.y += 100.0f; // �����F�K�v�ɉ�����110�`130�ł�OK

        // �X�N���[�����W�֕ϊ�
        VECTOR screen = ConvWorldPosToScreenPos(worldPos);

        // HP�o�[�̕\���ʒu�i�X�N���[����j
        int x = static_cast<int>(screen.x) - 30;
        int y = static_cast<int>(screen.y) - 10;

        // HP�o�[�T�C�Y
        int barWidth = static_cast<int>(60 * hpRatio);
        int barHeight = 8;

        // �`��
        DrawBox(x, y, x + barWidth, y + barHeight, GetColor(255, 0, 0), TRUE);  // ���g
        DrawBox(x, y, x + 60, y + barHeight, GetColor(255, 255, 255), FALSE);   // �g��   
    }
}

// ��������i���󃊃\�[�X�Ȃ��j
void EnemyHpBar::Release()
{
}