#pragma once
#include <DxLib.h>
class Sword
{
public:
	struct Info 
	{
		int model_;
		VECTOR pos_;
		VECTOR rot_;
		VECTOR scale_;
	};

	Sword();
	~Sword();

	void Init(void);
	void Update(void);
	void Draw(void);
	void Release(void);
private:
	Info list;
};

