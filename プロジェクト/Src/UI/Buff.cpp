#include "Buff.h"
#include <DxLib.h>
#include "../Manager/InputManager.h"
#include "../Manager/RandomManager.h"
#include "../Manager/BuffDataManager.h"
#include "../Manager/SoundManager.h"
#include "../Object/EnemyManager.h"
#include "../Application.h"
#include <unordered_set>

namespace SelectDraw
{
    constexpr int X = Application::HALF_SCREEN_SIZE_X - 100;
    constexpr int Y = Application::SCREEN_SIZE_Y - 100;
    constexpr int Width = Application::HALF_SCREEN_SIZE_X+100;
    constexpr int Height = Application::SCREEN_SIZE_Y;
}
// �R���X�g���N�^
Buff::Buff(void)
{
    player_ = nullptr;
    enemymng_ = nullptr;
    isSelected_ = false;
    canExit_ = false;
    selectedIndex_ = 0;
    stickCoolTime_ = 0;
    selectHandle_ = -1;
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

    buffUIInfos_ = {};
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

    selectHandle_ = LoadGraph((Application::PATH_IMAGE + "Select.png").c_str());

    BuffDataManager::GetInstance().LoadFromCSV("Data/BuffData.csv");

    GetMousePoint(&prevMouseX_, &prevMouseY_);

    ShuffleNumber();

    // �V���O���g���̌Ăяo��
    SoundManager* seMana_ = SoundManager::GetInstance();

    // SE�̃��[�h
    seMana_->LoadSE("UP", (Application::PATH_SOUND + "SE/Buff_up.mp3").c_str(), false);
}

// �X�V
void Buff::Update(void)
{
    // �V���O���g���̌Ăяo��
    SoundManager* seMana_ = SoundManager::GetInstance();

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
    }
    else
    {
        ControllerSelect();
    }
 
    // ==== ����{�^���}�E�X���� ====
    if (isUseMouse_ &&
        mouseX_ >= SelectDraw::X && mouseX_ <= SelectDraw::Width &&
        mouseY_ >= SelectDraw::Y && mouseY_ <= SelectDraw::Height &&
        ins.IsTrgMouseLeft())
    {
        seMana_->PlaySE("UP");
        isSelected_ = true;
        enemymng_->SetIsWaitingSlect(false);
        ApplyBuffSelect(selectedIndex_);
    }

    // �R���g���[���[�Ō���iA�{�^���j
    if (!isUseMouse_ && ins.IsPadBtnTrgDown(padNo, InputManager::JOYPAD_BTN::DOWN))
    {
        seMana_->PlaySE("UP");
        isSelected_ = true;
        enemymng_->SetIsWaitingSlect(false);
        ApplyBuffSelect(selectedIndex_);
    }

    if (enemymng_->IsWaitingForSkillSelect())
    {
        seMana_->StopSE("UP");
    }
}

// �`��
void Buff::Draw(void)
{
    // �{�X�|������ɂ͕\�����Ȃ�
    if (enemymng_->GetBossDead())return;

    for (int i = 0; i < DRAW_BUFF; i++)
    {
        const UIInfo& info = buffUIInfos_[i];
        int index = number_[i];
        const auto& data = BuffDataManager::GetInstance().GetBuffData(index);

        if (i == selectedIndex_)
        {
            DrawBoxAA(static_cast<float>(info.graph_X - 5), static_cast<float>(info.graph_Y - 5),
                info.graph_X + info.graph_Width * SCALE + 5.0f,
                info.graph_Y + info.graph_Height * SCALE + 5.0f,
                GetColor(255, 255, 0), FALSE);
        }

        DrawRotaGraph2(
            info.graph_X + (int)(info.graph_Width * SCALE / 2),
            info.graph_Y + (int)(info.graph_Height * SCALE / 2),
            info.graph_Width / 2, info.graph_Height / 2,
            SCALE, 0.0, info.handleId_, TRUE);

        // �o���m���\��
        DrawFormatString(info.graph_X, info.graph_Y + (int)(drawH + 10),
            GetColor(100, 255, 100),
            "ID:%d �m��:%d", data.id, data.AppearRate);
    }
    // �������
    DrawFormatString(100, 650, GetColor(255, 255, 255), "�� �� �őI���AA�{�^���Ō���");

    // ����{�^���`��
    DrawExtendGraph(SelectDraw::X, SelectDraw::Y, SelectDraw::Width, SelectDraw::Height, selectHandle_, true);
}

// ���
void Buff::Release(void)
{
    BuffDataManager::GetInstance().Release();
    
    // �V���O���g���̌Ăяo��
    SoundManager* seMana_ = SoundManager::GetInstance();

    // SE�̒�~
    seMana_->StopSE("UP");
    // SE�̉��
    seMana_->ReleaseSound("UP");
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
        int r = RandomManager::Random(100);
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

    if (ins.IsTrgMouseLeft())
    {
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
                    selectedIndex_ = i;
                }
                break; // ������ɔ�������Ηǂ�
            }
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
    number_.clear();
    auto& buffmng = BuffDataManager::GetInstance();
    const int buffCount = BuffDataManager::GetInstance().GetBuffCount();
    
    // �o����⃊�X�g���m���ɉ����č\�z
    std::vector<int> pool;
    for (int index = 0; index < buffCount; ++index)
    {
        const auto& data = buffmng.GetBuffDataByIndex(index);
        for (int j = 0; j < data.AppearRate; ++j)
        {
            pool.push_back(index);
        }
    }
    if (pool.empty())
    {
        ErrorLogAdd("�o�t�̏o����₪��ł�");
        return;
    }

    // �V���b�t�����āA�d�����Ȃ����𒊏o
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(pool.begin(), pool.end(), gen);

    std::unordered_set<int> used;
    for (int i = 0; i < (int)pool.size() && (int)number_.size() < DRAW_BUFF; ++i)
    {
        if (used.count(pool[i]) == 0)
        {
            number_.push_back(pool[i]);
            used.insert(pool[i]);
        }
    }

    // UIInfo ������
    if (!number_.empty())
    {
        // �܂��ŏ��̃o�t�̃T�C�Y�ŕ`��T�C�Y������
        const auto& firstData = buffmng.GetBuffDataByIndex(number_[0]);
        drawW = firstData.graph_Width * SCALE;
        drawH = firstData.graph_Height * SCALE;

        totalWidth = drawW * DRAW_BUFF + SPACING * (DRAW_BUFF - 1);
        startX = Application::HALF_SCREEN_SIZE_X - totalWidth / 2.0f;
        y = Application::HALF_SCREEN_SIZE_Y - drawH / 2.0f;

        for (int i = 0; i < (int)number_.size(); ++i)
        {
            const auto& data = buffmng.GetBuffDataByIndex(number_[i]);
            UIInfo& info = buffUIInfos_[i];

            info.handleId_ = data.handleId_;
            info.graph_Width = data.graph_Width;
            info.graph_Height = data.graph_Height;
            info.graph_X = static_cast<int>(startX + i * (drawW + SPACING));
            info.graph_Y = static_cast<int>(y);
        }
    }

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
