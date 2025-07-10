#pragma once
#include <DxLib.h>

class Stage
{
public:
	Stage(void);		// �R���X�g���N�^
	~Stage(void);		// �f�X�g���N�^

	void Init(void);	// ������
	void Update(void);	// �X�V
	void Draw(void);	// �`��
	void Release(void); // ���

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