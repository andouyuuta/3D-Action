#pragma once
#include "SceneBase.h"

class GameClear : public SceneBase
{
public:
	// �R���X�g���N�^
	GameClear(void);

	// �f�X�g���N�^
	~GameClear(void) override;

	void Init(void) override;		// ������
	void Update(void) override;		// �X�V	
	void Draw(void) override;		// �`��
	void Release(void) override;	// ���

private:
	// �摜

	// �J�[�\�����͈͓��ɂ��邩�ǂ���
	bool mouseFlg_;

	// �}�E�X���W
	Vector2 mousePos_;

};