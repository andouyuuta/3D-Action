#pragma once
#include <DxLib.h>
#include <deque>
#include <vector>

class Player;
class Trail3D;

class Sword
{
public:
	struct Info 
	{
		int model_;
		VECTOR pos_;
		VECTOR rot_;
		VECTOR scale_;

		int rightHandIdx_;
		VECTOR rightHandPos_;
	};

	Sword(void);
	~Sword(void);

	void Init(Player* player);
	void Update(void);
	void Draw(void);
	void Release(void);

	std::vector<VECTOR> GetBladePoints(float startY, float endY, float interval) const;

	[[nodiscard]] int GetBodyFrameIdx(void) { return MV1SearchFrame(list.model_, "pCube1.001"); }
	[[nodiscard]] VECTOR GetBodyPosition(void) { return MV1GetFramePosition(list.model_, GetBodyFrameIdx()); }
	[[nodiscard]] VECTOR GetSwordPosition(void) { return list.pos_; }
private:

	Info list;

	Trail3D* trail_;
	Player* player_;
};