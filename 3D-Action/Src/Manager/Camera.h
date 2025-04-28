#pragma once
#include <DxLib.h>
#include "../Common/Vector2.h"

class Player;

class Camera
{
public:
	static constexpr float MOVE_SPEED = 5.0f;
	static constexpr int  ROT_SPEED_DEG = 1;

	// �J�����̍���
	static constexpr float HEIGHT = 200.0f;

	// �Ǐ]�Ώۂ���J�����܂ł̋���
	static constexpr float DIS_FOLLOW2CAMERA = 500.0f;

	Camera(void);
	~Camera(void);

	void Init(void);
	void Update(void);
	void GameUpdate(void);
	void SetBeforeDraw(void);
	void Draw(void);
	void Release(void);

	// �Ǐ]����v���C���[�̐ݒ�
	void SetPlayer(Player* _player);

	VECTOR GetPos(void);

	bool IsMove(VECTOR _moveVec);

	VECTOR GetCameraAngles(void);

	// �J�����̍��W�������ʒu�ɖ߂�
	void ReSet(void);

private:
	// �J�����̈ʒu
	VECTOR pos_;

	// �Ǐ]�Ώۂ���̃��[�J�����W
	VECTOR localPosFormPlayer_;

	// �J�����̊p�x(rad)
	VECTOR angles_;

	// �Ǐ]����v���C���[
	Player* followPlayer_;

	// �J�[�\���̈ʒu
	Vector2 prevMousePos_;
	Vector2 nowMousePos_;
};

