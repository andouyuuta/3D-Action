#pragma once
#include <DxLib.h>

class Stage
{
public:
	Stage();		// コンストラクタ
	~Stage();		// デストラクタ

	void Init(void);	// 初期化
	void Update(void);	// 更新
	void Draw(void);	// 描画
	void Release(void); // 解放

	struct Info
	{
		int model_;
		VECTOR pos_;
		VECTOR rot_;
		VECTOR scale_;
	};

	[[nodiscard]] int GetStageModel(void) { return list.model_; }

private:
	Info list;
};