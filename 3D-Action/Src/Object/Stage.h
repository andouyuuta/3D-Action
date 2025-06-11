#pragma once
#include <DxLib.h>

class Stage
{
public:

	// �C���X�^���X�̐���
	static void CreateInstance(void);
	// �C���X�^���X�̎擾
	static Stage& GetInstance(void);

	Stage();		// �R���X�g���N�^
	~Stage();		// �f�X�g���N�^

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

	int GetStageModel(void) { return list.model_; }

private:
	// �ÓI�C���X�^���X
	static Stage* instance_;

	Info list;

	void PlayerToStageCollision(void);
};

