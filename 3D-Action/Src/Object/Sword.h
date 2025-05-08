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

		int righthandidx_;
		VECTOR righthandpos_;
	};
	// インスタンスの生成
	static void CreateInstance(void);
	// インスタンスの取得
	static Sword& GetInstance(void);

	Sword();
	~Sword();

	void Init();
	void Update();
	void Draw(void);
	void Release(void);

	[[nodiscard]] VECTOR GetSwordPosition(void) { return list.pos_; }
private:
	// 静的インスタンス
	static Sword* instance_;

	Info list;
};

