#pragma once
#include <DxLib.h>

class Stage
{
public:

	// インスタンスの生成
	static void CreateInstance(void);
	// インスタンスの取得
	static Stage& GetInstance(void);

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

	int GetStageModel(void) { return list.model_; }

private:
	// 静的インスタンス
	static Stage* instance_;

	Info list;

	void PlayerToStageCollision(void);
};

