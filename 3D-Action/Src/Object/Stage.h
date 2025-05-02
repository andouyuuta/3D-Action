#pragma once
#include <DxLib.h>

class Stage
{
public:

	// インスタンスの生成
	static void CreateInstance(void);
	// インスタンスの取得
	static Stage& GetInstance(void);

	Stage();
	~Stage();

	void Init(void);
	void Update(void);
	void Draw(void);
	void Release(void);

	struct Info
	{
		int model_;
		VECTOR pos_;
		VECTOR rot_;
		VECTOR scale_;
	};

	int GetStageModel(void) { return list.model_; }

private:
	// 静的インスタンス
	static Stage* instance_;

	Info list;

	void PlayerToStageCollision(void);
};

