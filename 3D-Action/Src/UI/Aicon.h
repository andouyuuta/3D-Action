#pragma once
#include <DxLib.h>

class Player;

class Aicon
{
public:
	static constexpr int MAX_ICON = 4;
	enum WEAPON_AICON
	{
		Sheach = 0,			// ���킵�܂�
		Weapon_Out,			// ������o��
			
		Weapon_Kind,
	};

	enum ATTACK_AICON
	{
		First_Attack = 0,	// �U���P
		Second_Attack,		// �U���Q
		Third_Attack,		// �U���R
		Force_Attack,		// �U���S
		Crouch_Attack,		// ���Ⴊ�ݍU��

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