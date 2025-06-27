#pragma once
#include <DxLib.h>
class ObjectBase
{
public:
	ObjectBase(void);
	virtual ~ObjectBase(void) = 0;		// デストラクタ

	// 初期処理
	virtual void Init(void) = 0;		// 初期化
	virtual void Update(void) = 0;		// 更新
	virtual void Draw(void) = 0;		// 描画
	virtual void DrawDebug(void) = 0;	// デバッグ描画
	virtual void Release(void) = 0;		// 解放

	// 拡大値を返す
	VECTOR GetScale(void) { return modelInfo.scale_; }
	// 角度を返す
	VECTOR GetRot(void) { return modelInfo.rot_; }
	// 座標を返す
	VECTOR GetPos(void) { return modelInfo.pos_; }

protected:
	struct BaseInfo
	{
		// 基本情報
		int modelId_;			// モデルID
		VECTOR pos_;			// 座標
		VECTOR rot_;				// 回転
		VECTOR scale_;			// 大きさ
	}modelInfo;
};

