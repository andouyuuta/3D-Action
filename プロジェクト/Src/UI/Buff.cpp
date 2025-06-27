#include "Buff.h"
#include <DxLib.h>
#include "../Manager/InputManager.h"
#include "../Manager/RandomManager.h"
#include "../Manager/BuffDataManager.h"
#include "../Object/EnemyManager.h"
#include "../Application.h"

// �R���X�g���N�^
Buff::Buff(void)
{
    player_ = nullptr;
    enemymng_ = nullptr;
    isSelected_ = false;
    canExit_ = false;
    selectedIndex_ = 0;
    stickCoolTime_ = 0;
    isUseMouse_ = false;
    lx = 0;
    mouseX_ = 0;
    mouseY_ = 0;
    drawW = 0.0f;
    drawH = 0.0f;

    totalWidth = 0.0f;
    startX = 0.0f;
    x = 0.0f;
    y = 0.0f;

    prevMouseX_ = 0;
    prevMouseY_ = 0;
}

// �f�X�g���N�^
Buff::~Buff(void)
{
    Release();
}

// ������
void Buff::Init(Player* player, EnemyManager* mng)
{
    player_ = player;
    enemymng_ = mng;

    BuffDataManager::GetInstance().LoadFromCSV("Data/BuffData.csv");

    GetMousePoint(&prevMouseX_, &prevMouseY_);

    ShuffleNumber();
}

// �X�V
void Buff::Update(void)
{
    if (isSelected_)
    {
        static int waitTime = 30; // ��0.5�b(60fps)
        if (--waitTime <= 0)
        {
            waitTime = 30;
            canExit_ = true;
        }
        return; // �I��������͑�����󂯕t���Ȃ�
    }

    InputManager& ins = InputManager::GetInstance();
    InputManager::JOYPAD_NO padNo = InputManager::JOYPAD_NO::PAD1;
    InputManager::JOYPAD_IN_STATE pad = ins.GetJPadInputState(padNo);

    if (stickCoolTime_ > 0) stickCoolTime_--;

    lx = pad.AKeyLX;

    // �}�E�X�ʒu�擾
    GetMousePoint(&mouseX_, &mouseY_);

    // ���͐ؑ֔���
    if (mouseX_ != prevMouseX_ || mouseY_ != prevMouseY_)
    {
        isUseMouse_ = true;  // �}�E�X����������
    }
    else if (abs(lx) > STICK_THRESHOLD)
    {
        isUseMouse_ = false;  // �X�e�B�b�N����������
    }

    prevMouseX_ = mouseX_;
    prevMouseY_ = mouseY_;

    if (isUseMouse_)
    {
        MouseSelect();
    }else
    {
        ControllerSelect();
    }
    // A�{�^���Ō���
    if (ins.IsPadBtnTrgDown(padNo, InputManager::JOYPAD_BTN::DOWN)||
        ins.IsTrgMouseLeft())
    {
        isSelected_ = true;
        enemymng_->SetIsWaitingSlect(false);
        ApplyBuffSelect(selectedIndex_);
    }
}


// �`��
void Buff::Draw(void)
{
    // �{�X�|������ɂ͕\�����Ȃ�
    if (enemymng_->GetBossDead())return;

    for (int i = 0; i < DRAW_BUFF; i++)
    {
        int index = number_[i];
        const auto& data = BuffDataManager::GetInstance().GetBuffData(index);

        // �O���t�B�b�N�T�C�Y�擾�i���񂾂��ł�OK�j
        int width = 0, height = 0;
        GetGraphSize(data.handleId_, &width, &height);

        drawW = width * SCALE;
        drawH = height * SCALE;

        x = startX + i * (drawW + SPACING);

        // �I�𒆂̘g
        if (i == selectedIndex_)
        {
            DrawBoxAA((int)(x - 5), (int)(y - 5), (int)(x + drawW + 5), (int)(y + drawH + 5), GetColor(255, 255, 0), FALSE);
        }

        // �`��
        DrawRotaGraph2(
            x + drawW / 2, y + drawH / 2,
            width / 2, height / 2,
            SCALE, 0.0, data.handleId_, TRUE
        );
    }
    // �������
    DrawFormatString(100, 650, GetColor(255, 255, 255), "�� �� �őI���AA�{�^���Ō���");

}

// ���
void Buff::Release(void)
{
    BuffDataManager::GetInstance().Release();
}

void Buff::Reset(void)
{
    ShuffleNumber();
}

// �o�t�K�p�֐�
void Buff::ApplyBuffSelect(int index)
{
    if (!player_) return;

    int actualBuffIndex = number_[index];
    const auto& data = BuffDataManager::GetInstance().GetBuffData(actualBuffIndex);

    // �������t���̃o�t�i�m�����肠��j
    if (data.successRate_ < 100)
    {
        int r = RandomManager::Critical(100);
        if (r < data.successRate_)
        {
            player_->AddAttack(data.valueSuccess_);
            player_->AddMaxHp(data.valueSuccess_);
        }
        else
        {
            player_->AddAttack(data.valueFailure_);
            player_->AddMaxHp(data.valueFailure_);
        }
    }
    else
    {
        // �ʏ�o�t�i�m���s�v�j
        player_->AddAttack(data.attackUp_);
        player_->AddMaxHp(data.hpUp_);
    }
}

void Buff::MouseSelect(void)
{
    InputManager& ins = InputManager::GetInstance();
    // -------- �}�E�X�ɂ��I�� --------
    const auto& data = BuffDataManager::GetInstance().GetBuffDataByIndex(number_[0]);
    if (data.graph_Width != 0)
    {
        drawW = data.graph_Width * SCALE;
        drawH = data.graph_Height * SCALE;
    }

    totalWidth = drawW * DRAW_BUFF + SPACING * (DRAW_BUFF - 1);
    startX = Application::HALF_SCREEN_SIZE_X - totalWidth / 2.0f;
    y = Application::HALF_SCREEN_SIZE_Y - drawH / 2.0f;

    for (int i = 0; i < DRAW_BUFF; ++i)
    {
        float x = startX + i * (drawW + SPACING);

        if (mouseX_ >= x && mouseX_ <= x + drawW &&
            mouseY_ >= y && mouseY_ <= y + drawH)
        {
            selectedIndex_ = i;

            // ���N���b�N�Ō���
            if (ins.IsTrgMouseLeft())
            {
                isSelected_ = true;
                enemymng_->SetIsWaitingSlect(false);
                ApplyBuffSelect(selectedIndex_);
                return;
            }
            break; // ������ɔ�������Ηǂ�
        }
    }
}

void Buff::ControllerSelect(void)
{
    // �R���g���[���[�ɂ��I��
    if (stickCoolTime_ == 0)
    {
        if (lx < -STICK_THRESHOLD && selectedIndex_ > 0)
        {
            selectedIndex_--;
            stickCoolTime_ = 15;
        }
        else if (lx > STICK_THRESHOLD && selectedIndex_ < DRAW_BUFF - 1)
        {
            selectedIndex_++;
            stickCoolTime_ = 15;
        }
    }
}

void Buff::ShuffleNumber(void)
{
    // �摜�T�C�Y�擾��UIInfo�Z�b�g
    for (int i = 0; i < NUM_BUFFS; i++)
    {
        /*if (buffInfo_[i].handleId_ != -1)
        {
            GetGraphSize(buffInfo_[i].handleId_, &buffInfo_[i].graph_Width, &buffInfo_[i].graph_Height);
        }*/
        number_[i] = i;
    }
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(number_.begin(), number_.end(), gen);

    // �I���C���f�b�N�X�Ȃǂ����Z�b�g
    selectedIndex_ = 0;
    stickCoolTime_ = 0;
    isSelected_ = false;
    canExit_ = false;
}

bool Buff::IsSelected(void) const
{
    return isSelected_;
}

bool Buff::IsCanExit(void) const
{
    return canExit_;
}
