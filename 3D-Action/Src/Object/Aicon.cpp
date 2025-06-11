#include "Aicon.h"
#include "../Application.h"
#include "Player.h"

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

Aicon::Aicon()
{
}

Aicon::~Aicon()
{
}

void Aicon::Init(void)
{
	weaponIcon_[Sheach] = LoadGraph((Application::PATH_AICON + "Sheach.png").c_str());
	weaponIcon_[Weapon_Out] = LoadGraph((Application::PATH_AICON + "WeaponOut.png").c_str());
	attackIcon_[First_Attack] = LoadGraph((Application::PATH_AICON + "FirstAttack.png").c_str());
	attackIcon_[Second_Attack] = LoadGraph((Application::PATH_AICON + "SecondAttack.png").c_str());
	attackIcon_[Third_Attack] = LoadGraph((Application::PATH_AICON + "ThirdAttack.png").c_str());
	attackIcon_[Force_Attack] = LoadGraph((Application::PATH_AICON + "ForceAttack.png").c_str());
	attackIcon_[Crouch_Attack] = LoadGraph((Application::PATH_AICON + "CrouchAttack.png").c_str());
}

void Aicon::Update(void)
{
}

void Aicon::Draw(void)
{
	if (Player::GetInstance().GetIsWeapon())
	{
		DrawExtendGraph(WeaponDraw::X, WeaponDraw::Y,WeaponDraw::Width,WeaponDraw::Height, weaponIcon_[Weapon_Out], true);
	}
	else
	{
		DrawExtendGraph(WeaponDraw::X, WeaponDraw::Y, WeaponDraw::Width, WeaponDraw::Height, weaponIcon_[Sheach], true);
	}

	if (Player::GetInstance().GetIsWeapon())
	{
		if (Player::GetInstance().GetComboStep() == 0)
		{
			DrawExtendGraph(AttackDraw::X, AttackDraw::Y, AttackDraw::Width, AttackDraw::Height, attackIcon_[First_Attack], true);
		}

		if (Player::GetInstance().GetComboStep() == 1)
		{
			DrawExtendGraph(AttackDraw::X, AttackDraw::Y, AttackDraw::Width, AttackDraw::Height, attackIcon_[Second_Attack], true);
		}

		if (Player::GetInstance().GetComboStep() == 2)
		{
			DrawExtendGraph(AttackDraw::X, AttackDraw::Y, AttackDraw::Width, AttackDraw::Height, attackIcon_[Third_Attack], true);
		}

		if (Player::GetInstance().GetComboStep() == 3)
		{
			DrawExtendGraph(AttackDraw::X, AttackDraw::Y, AttackDraw::Width, AttackDraw::Height, attackIcon_[Force_Attack], true);
		}
	}
}

void Aicon::Release(void)
{
	// weaponIcon_ ‚Ì‰ð•ú
	for (int i = 0; i < Weapon_Kind; ++i)
	{
		if (weaponIcon_[i] != -1)
		{
			DeleteGraph(weaponIcon_[i]);
			weaponIcon_[i] = -1;
		}
	}

	// attackIcon_ ‚Ì‰ð•ú
	for (int i = 0; i < Attack_Kind; ++i)
	{
		if (attackIcon_[i] != -1)
		{
			DeleteGraph(attackIcon_[i]);
			attackIcon_[i] = -1;
		}
	}
}
