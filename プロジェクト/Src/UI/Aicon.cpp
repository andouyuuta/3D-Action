#include "Aicon.h"
#include "../Application.h"
#include "../Object/Player.h"

namespace WeaponDraw
{
	constexpr int X = Application::SCREEN_SIZE_X - 100;
	constexpr int Y = Application::SCREEN_SIZE_Y - 100;
	constexpr int Width = Application::SCREEN_SIZE_X;
	constexpr int Height = Application::SCREEN_SIZE_Y;
}

namespace AttackDraw
{
	constexpr int X = Application::SCREEN_SIZE_X - 200;
	constexpr int Y = Application::SCREEN_SIZE_Y - 100;
	constexpr int Width = Application::SCREEN_SIZE_X - 100;
	constexpr int Height = Application::SCREEN_SIZE_Y ;
}

namespace ButtonDraw
{
	constexpr int X = AttackDraw::X + 25;
	constexpr int Y = AttackDraw::Y - 25;
	constexpr int Width = AttackDraw::Width - 25;
	constexpr int Height = AttackDraw::Height - 75;
}

Aicon::Aicon(void)
{
	for (int i = 0; i < ATTACK_AICON::Attack_Kind; ++i)
	{
		attackIcon_[i] = -1;
	}
	for (int i = 0; i < WEAPON_AICON::Weapon_Kind; ++i)
	{
		weaponIcon_[i] = -1;
	}

	for (int i = 0; i < XBOX_BUTTON::Pad_Kind; ++i)
	{
		padIcon_[i] = -1;
	}

	player_ = nullptr;
}

Aicon::~Aicon(void)
{
}

void Aicon::Init(Player* player)
{
	player_ = player;

	weaponIcon_[Sheach] = LoadGraph((Application::PATH_AICON + "Sheach.png").c_str());
	weaponIcon_[Weapon_Out] = LoadGraph((Application::PATH_AICON + "WeaponOut.png").c_str());
	attackIcon_[First_Attack] = LoadGraph((Application::PATH_AICON + "FirstAttack.png").c_str());
	attackIcon_[Second_Attack] = LoadGraph((Application::PATH_AICON + "SecondAttack.png").c_str());
	attackIcon_[Third_Attack] = LoadGraph((Application::PATH_AICON + "ThirdAttack.png").c_str());
	attackIcon_[Force_Attack] = LoadGraph((Application::PATH_AICON + "ForceAttack.png").c_str());
	attackIcon_[Crouch_Attack] = LoadGraph((Application::PATH_AICON + "CrouchAttack.png").c_str());
	padIcon_[LT] = LoadGraph((Application::PATH_AICON + "LT.png").c_str());
	padIcon_[RB] = LoadGraph((Application::PATH_AICON + "RB.png").c_str());
	padIcon_[X] = LoadGraph((Application::PATH_AICON + "resized_X.png").c_str());
	padIcon_[Y] = LoadGraph((Application::PATH_AICON + "resized_Y.png").c_str());
	padIcon_[A] = LoadGraph((Application::PATH_AICON + "resized_A.png").c_str());
	padIcon_[B] = LoadGraph((Application::PATH_AICON + "resized_B.png").c_str());
}

void Aicon::Update(void)
{
}

void Aicon::Draw(void)
{
	// --- 武器アイコンの描画 ---
	if (player_->GetIsWeapon())  // 抜刀状態
	{
		
		DrawExtendGraph(
			WeaponDraw::X, WeaponDraw::Y,
			WeaponDraw::Width, WeaponDraw::Height,
			weaponIcon_[Weapon_Out], true);
		
		DrawExtendGraph(ButtonDraw::X + 100, ButtonDraw::Y, ButtonDraw::Width + 100, ButtonDraw::Height, padIcon_[LT], true);
	}
	else  // 納刀状態
	{
		DrawExtendGraph(
			WeaponDraw::X, WeaponDraw::Y,
			WeaponDraw::Width, WeaponDraw::Height,
			weaponIcon_[Sheach], true);

		DrawExtendGraph(ButtonDraw::X + 100, ButtonDraw::Y, ButtonDraw::Width + 100, ButtonDraw::Height, padIcon_[LT], true);
	}

	if (player_->GetIsWeapon()&&!player_->GetInfo().isCrouch_)
	{
		int comboStep = player_->GetComboStep();

		switch (comboStep)
		{
		case 0:
			DrawExtendGraph(AttackDraw::X, AttackDraw::Y, AttackDraw::Width, AttackDraw::Height, attackIcon_[First_Attack], true);
			DrawExtendGraph(ButtonDraw::X, ButtonDraw::Y, ButtonDraw::Width, ButtonDraw::Height, padIcon_[X], true);
			break;
		case 1:
			DrawExtendGraph(AttackDraw::X, AttackDraw::Y, AttackDraw::Width, AttackDraw::Height, attackIcon_[Second_Attack], true);
			DrawExtendGraph(ButtonDraw::X, ButtonDraw::Y, ButtonDraw::Width, ButtonDraw::Height, padIcon_[Y], true);
			break;
		case 2:
			DrawExtendGraph(AttackDraw::X, AttackDraw::Y, AttackDraw::Width, AttackDraw::Height, attackIcon_[Third_Attack], true);
			DrawExtendGraph(ButtonDraw::X, ButtonDraw::Y, ButtonDraw::Width, ButtonDraw::Height, padIcon_[Y], true);
			break;
		default:
			break;
		}
	}

	// ウルト
	const int ultY = Application::SCREEN_SIZE_Y - ultSize - 20;  // 下から20pxの余白

	// ブレンド設定
	if (player_->GetIsUltUse() && !player_->GetIsUltFinish())
	{
		// ウルト使用可能 → 明るく
		SetDrawBright(255, 255, 255);
	}
	else
	{
		// 使用不可 → 暗く
		SetDrawBright(80, 80, 80);
	}

	DrawExtendGraph(
		ultX, ultY,
		ultX + ultSize, ultY + ultSize,
		padIcon_[B], true
	);

	SetDrawBright(255, 255, 255); // 明るさを戻す
}

void Aicon::Release(void)
{
	// weaponIcon_ の解放
	for (int i = 0; i < Weapon_Kind; ++i)
	{
		if (weaponIcon_[i] != -1)
		{
			DeleteGraph(weaponIcon_[i]);
			weaponIcon_[i] = -1;
		}
	}

	// attackIcon_ の解放
	for (int i = 0; i < Attack_Kind; ++i)
	{
		if (attackIcon_[i] != -1)
		{
			DeleteGraph(attackIcon_[i]);
			attackIcon_[i] = -1;
		}
	}

	for (int i = 0; i < Pad_Kind; ++i)
	{
		DeleteGraph(padIcon_[i]);
		padIcon_[i] = -1;
	}
}
