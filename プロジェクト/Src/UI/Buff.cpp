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
// コンストラクタ
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

// デストラクタ
Buff::~Buff(void)
{
    Release();
}

// 初期化
void Buff::Init(Player* player, EnemyManager* mng)
{
    player_ = player;
    enemymng_ = mng;

    selectHandle_ = LoadGraph((Application::PATH_IMAGE + "Select.png").c_str());

    BuffDataManager::GetInstance().LoadFromCSV("Data/BuffData.csv");

    GetMousePoint(&prevMouseX_, &prevMouseY_);

    ShuffleNumber();

    // シングルトンの呼び出し
    SoundManager* seMana_ = SoundManager::GetInstance();

    // SEのロード
    seMana_->LoadSE("UP", (Application::PATH_SOUND + "SE/Buff_up.mp3").c_str(), false);
}

// 更新
void Buff::Update(void)
{
    // シングルトンの呼び出し
    SoundManager* seMana_ = SoundManager::GetInstance();

    if (isSelected_)
    {
        static int waitTime = 30; // 約0.5秒(60fps)
        if (--waitTime <= 0)
        {
            waitTime = 30;
            canExit_ = true;
        }
        return; // 選択完了後は操作を受け付けない
    }

    InputManager& ins = InputManager::GetInstance();
    InputManager::JOYPAD_NO padNo = InputManager::JOYPAD_NO::PAD1;
    InputManager::JOYPAD_IN_STATE pad = ins.GetJPadInputState(padNo);

    if (stickCoolTime_ > 0) stickCoolTime_--;

    lx = pad.AKeyLX;

    // マウス位置取得
    GetMousePoint(&mouseX_, &mouseY_);

    // 入力切替判定
    if (mouseX_ != prevMouseX_ || mouseY_ != prevMouseY_)
    {
        isUseMouse_ = true;  // マウスが動いたら
    }
    else if (abs(lx) > STICK_THRESHOLD)
    {
        isUseMouse_ = false;  // スティックが動いたら
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
 
    // ==== 決定ボタンマウス判定 ====
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

    // コントローラーで決定（Aボタン）
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

// 描画
void Buff::Draw(void)
{
    // ボス倒した後には表示しない
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

        // 出現確率表示
        DrawFormatString(info.graph_X, info.graph_Y + (int)(drawH + 10),
            GetColor(100, 255, 100),
            "ID:%d 確率:%d", data.id, data.AppearRate);
    }
    // 操作説明
    DrawFormatString(100, 650, GetColor(255, 255, 255), "← → で選択、Aボタンで決定");

    // 決定ボタン描画
    DrawExtendGraph(SelectDraw::X, SelectDraw::Y, SelectDraw::Width, SelectDraw::Height, selectHandle_, true);
}

// 解放
void Buff::Release(void)
{
    BuffDataManager::GetInstance().Release();
    
    // シングルトンの呼び出し
    SoundManager* seMana_ = SoundManager::GetInstance();

    // SEの停止
    seMana_->StopSE("UP");
    // SEの解放
    seMana_->ReleaseSound("UP");
}

void Buff::Reset(void)
{
    ShuffleNumber();
}

// バフ適用関数
void Buff::ApplyBuffSelect(int index)
{
    if (!player_) return;

    int actualBuffIndex = number_[index];
    const auto& data = BuffDataManager::GetInstance().GetBuffData(actualBuffIndex);

    // 成功率付きのバフ（確率判定あり）
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
        // 通常バフ（確率不要）
        player_->AddAttack(data.attackUp_);
        player_->AddMaxHp(data.hpUp_);
    }
}

void Buff::MouseSelect(void)
{
    InputManager& ins = InputManager::GetInstance();
    // -------- マウスによる選択 --------
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

                // 左クリックで決定
                if (ins.IsTrgMouseLeft())
                {
                    selectedIndex_ = i;
                }
                break; // 一つだけに反応すれば良い
            }
        }
    }
}

void Buff::ControllerSelect(void)
{
    // コントローラーによる選択
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
    
    // 出現候補リストを確率に応じて構築
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
        ErrorLogAdd("バフの出現候補が空です");
        return;
    }

    // シャッフルして、重複しない数を抽出
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

    // UIInfo 初期化
    if (!number_.empty())
    {
        // まず最初のバフのサイズで描画サイズを決定
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
