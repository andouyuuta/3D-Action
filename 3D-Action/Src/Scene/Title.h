#pragma once
#include <DxLib.h>
#include <string>
#include "SceneBase.h"
#include "../Common/Vector2.h"


class Title : public SceneBase
{
public:

	//�@�e�L�X�g�֘A
	//�@�Q�[���X�^�[�g
	static constexpr float GAMESTART_TEXT_X = 1000.0f;							// ���� X���W
	static constexpr float GAMESTART_TEXT_Y = 550.0f;							//		Y���W
	static constexpr float GAMESTART_TEXT_X_END = GAMESTART_TEXT_X + 350.0f;	// �E�� X���W
	static constexpr float GAMESTART_TEXT_Y_END = GAMESTART_TEXT_Y + 60.0f;		//		Y���W

	// �^�C�g���֌W�̍\����
	struct text
	{
		Vector2 textLeftUpPos_;		// �����̍���̍��W
		Vector2 textRightDownPos_;	// �����̉E��̍��W
		bool textFlg_;				// �����I���t���O
	};

	// �R���X�g���N�^
	Title(void);

	// �f�X�g���N�^
	~Title(void) override;

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