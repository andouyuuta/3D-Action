#pragma once
#include "SceneBase.h"

class GameOver : public SceneBase
{
public:
	// �R���X�g���N�^
	GameOver(void);

	// �f�X�g���N�^
	~GameOver(void) override;

	void Init(void) override;		// ������
	void Update(void) override;		// �X�V	
	void Draw(void) override;		// �`��
	void Release(void) override;	// ���

private:
	// �摜
	int hightLight_;

	// �J�[�\�����͈͓��ɂ��邩�ǂ���
	bool gameFlg_;
	bool titleFlg_;

	// �}�E�X���W
	Vector2 mousePos_;
};