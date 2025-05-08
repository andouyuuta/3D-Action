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
	// �C���X�^���X�̐���
	static void CreateInstance(void);
	// �C���X�^���X�̎擾
	static Sword& GetInstance(void);

	Sword();
	~Sword();

	void Init();
	void Update();
	void Draw(void);
	void Release(void);

	[[nodiscard]] VECTOR GetSwordPosition(void) { return list.pos_; }
private:
	// �ÓI�C���X�^���X
	static Sword* instance_;

	Info list;
};

