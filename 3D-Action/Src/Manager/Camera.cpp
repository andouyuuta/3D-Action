#include <DxLib.h>
#include "Camera.h"
#include "../Manager/InputManager.h"
#include "../Application.h"
#include "../Object/Player.h"

Camera::Camera(void)
{
}

Camera::~Camera(void)
{
}

void Camera::Init(void)
{
	// �J�����̏����ʒu
	pos_ = { 0.0f, 0.0f, 0.0f };

	// �Ǐ]�Ώۂ���̃��[�J�����W�ݒ�
	localPosFormPlayer_ = { 0.0f, HEIGHT, -DIS_FOLLOW2CAMERA };

	// �J�����̏����p�x
	angles_ = { 0.0f, 0.0f, 0.0f };

	// ���O�̃}�E�X���W
	prevMousePos_ = { 0, 0 };

	// ���݂̃}�E�X���W
	nowMousePos_ = { 0, 0 };
}

void Camera::Update(void)
{
}

void Camera::GameUpdate(void)
{
	// ���͐���擾
	InputManager& ins = InputManager::GetInstance();

	// �N���b�v������ݒ肷��
	SetCameraNearFar(5.0f, 30000.0f);

	// �J�����̐ݒ�(�ʒu�Ɗp�x�ɂ�鐧��)
	VECTOR UP_VECTOR = { 0.0f, 1.0f, 0.0f };
	VECTOR followPos = followPlayer_->GetPlayerPos();
	VECTOR Localpos_ = VAdd(followPos, localPosFormPlayer_);
	SetCameraPositionAndTargetAndUpVec(
		Localpos_,
		followPos,
		UP_VECTOR
	);

	// �}�E�X���x��ݒ�
	float mouseSensi = 0.0008f;  // �}�E�X���x�̒����l

	// �p�x�̐����i����X���̐��������̉�]�𐧌�)
	const float MAX_PITCH = DX_PI_F / 2.0f - 0.01f; // +90�x�܂�
	const float MIN_PITCH = -DX_PI_F / 2.0f + 0.01f; // -90�x�܂�

	// ��ʂ̒������W�i�}�E�X�������Ƀ��Z�b�g����j
	VECTOR screenCenter = VGet(Application::HALF_SCREEN_SIZE_X, Application::HALF_SCREEN_SIZE_Y, 0.0f);

	// �}�E�X���W
	prevMousePos_ = nowMousePos_;
	nowMousePos_ = ins.GetMousePos();

	// �}�E�X�̍������v�Z
	float deltaX = nowMousePos_.x - screenCenter.x;
	float deltaY = nowMousePos_.y - screenCenter.y;

	// �}�E�X�̍������g������]����
	angles_.y += deltaX * mouseSensi;  // ������]
	angles_.x += deltaY * mouseSensi;  // ������]

	// �s�b�`�iX����]�j�̐������s���i���������̂ݐ����j
	if (angles_.x > MAX_PITCH) angles_.x = MAX_PITCH;
	if (angles_.x < MIN_PITCH) angles_.x = MIN_PITCH;

	// �}�E�X����ʒ����Ƀ��Z�b�g
	SetMousePoint(screenCenter.x, screenCenter.y);

	// �P�ʍs����擾
	MATRIX mat = MGetIdent();

	// XY�̉�]�s������ꂼ��擾
	MATRIX matRotX = MGetRotX(angles_.x);
	MATRIX matRotY = MGetRotY(angles_.y);

	// �P�ʍs��ɍ������Ă���
	mat = MMult(mat, matRotX);
	mat = MMult(mat, matRotY);

	// ���[�J�����W�̉�]
	VECTOR localPos = VTransform(localPosFormPlayer_, mat);

	// �Ǐ]�Ώۂ̃��[�J�����W��ǉ����ă��[���h���W�ɂ���
	pos_ = VAdd(followPos, localPos);

	// �J�����̈ʒu�Ɗp�x��ݒ�
	SetCameraPositionAndAngle(
		pos_,
		angles_.x,  // ������]
		angles_.y,  // ������]
		angles_.z   // Z���̉�]�i�K�v�ł���΁j
	);
}

// �J�����ݒ�i���t���[�����s�j
void Camera::SetBeforeDraw(void)
{
	// �N���b�v������ݒ肷��
	SetCameraNearFar(10.0f, 30000.0f);

	// �J�����̐ݒ�i�ʒu�Ɗp�x�ɂ�鐧��j
	SetCameraPositionAndAngle(
		pos_,
		angles_.x,
		angles_.y,
		angles_.z
	);
}

void Camera::Draw(void)
{
	InputManager& ins = InputManager::GetInstance();
	Vector2 mousePos_ = ins.GetMousePos();

	DrawFormatString(20, 40, GetColor(0xff, 0xff, 0xff), "MousePosX = %d", mousePos_.x);
	DrawFormatString(20, 60, GetColor(0xff, 0xff, 0xff), "MousePosY = %d", mousePos_.y);
}

void Camera::Release(void)
{
}

void Camera::SetPlayer(Player* _player)
{
	followPlayer_ = _player;
}

VECTOR Camera::GetPos(void)
{
	return pos_;
}

bool Camera::IsMove(VECTOR _moveVec)
{
	// XYZ���ׂĂ̍��W�̈ړ��ʂ̐�Βl���Ƃ�
	float absX = abs(_moveVec.x);
	float absY = abs(_moveVec.y);
	float absZ = abs(_moveVec.z);

	// �ړ��x�N�g���� X�EY�EZ ���ׂĈړ�����Ă��Ȃ����
	bool isNoMoveX = absX < FLT_EPSILON;
	bool isNoMoveY = absY < FLT_EPSILON;
	bool isNoMoveZ = absZ < FLT_EPSILON;

	// ���W�X�V������������
	if (isNoMoveX && isNoMoveY && isNoMoveZ)
	{
		return false;
	}
	else
	{
		return true;
	}
}

VECTOR Camera::GetCameraAngles(void)
{
	return angles_;
}

void Camera::ReSet(void)
{
	// �J�����̈ʒu
	pos_ = { 0.0f, 0.0f, 0.0f };

	// �J�����̊p�x
	angles_ = { 0.0f, 0.0f, 0.0f };
}
