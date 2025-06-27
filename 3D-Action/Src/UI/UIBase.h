#pragma once
#include <DxLib.h>

class Player;

class UIBase
{
public:
	UIBase(void);
	virtual ~UIBase(void);

	virtual void Init(void);
	virtual void Update(void);
	virtual void Draw(void);
	virtual void Release(void);
protected:
	struct UIInfo
	{
		int handleId_;
		int graph_X;
		int graph_Y;
		int graph_Width;
		int graph_Height;
	};

};

