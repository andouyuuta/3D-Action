#pragma once
#include <DxLib.h>
class ObjectBase
{
public:
	ObjectBase(void);
	virtual ~ObjectBase(void) = 0;		// �f�X�g���N�^

	// ��������
	virtual void Init(void) = 0;		// ������
	virtual void Update(void) = 0;		// �X�V
	virtual void Draw(void) = 0;		// �`��
	virtual void DrawDebug(void) = 0;	// �f�o�b�O�`��
	virtual void Release(void) = 0;		// ���

	// �g��l��Ԃ�
	VECTOR GetScale(void) { return modelInfo.scale_; }
	// �p�x��Ԃ�
	VECTOR GetRot(void) { return modelInfo.rot_; }
	// ���W��Ԃ�
	VECTOR GetPos(void) { return modelInfo.pos_; }

protected:
	struct BaseInfo
	{
		// ��{���
		int modelId_;			// ���f��ID
		VECTOR pos_;			// ���W
		VECTOR rot_;				// ��]
		VECTOR scale_;			// �傫��
	}modelInfo;
};

