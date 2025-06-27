#pragma once
#include <DxLib.h>

class Player;

class Aicon
{
public:
	static constexpr int MAX_ICON = 4;
	enum WEAPON_AICON
	{
		Sheach = 0,			// •Ší‚µ‚Ü‚¤
		Weapon_Out,			// •Šíæ‚èo‚µ
			
		Weapon_Kind,
	};

	enum ATTACK_AICON
	{
		First_Attack = 0,	// UŒ‚‚P
		Second_Attack,		// UŒ‚‚Q
		Third_Attack,		// UŒ‚‚R
		Force_Attack,		// UŒ‚‚S
		Crouch_Attack,		// ‚µ‚á‚ª‚İUŒ‚

		Attack_Kind
	};

	Aicon();
	~Aicon();

	void Init(Player* player_);
	void Update(void);
	void Draw(void);
	void Release(void);

private:
	int weaponIcon_[Weapon_Kind];
	int attackIcon_[Attack_Kind];

	Player* player_;
};