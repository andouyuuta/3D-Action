#include <DxLib.h>
#include "Grid.h"

// �R���X�g���N�^
Grid::Grid(void)
{
}

// �R���X�g���N�^
Grid::~Grid(void)
{
}

// ������
void Grid::SystemInit(void)
{
}

// ������
void Grid::GameInit(void)
{
}

// �X�V
void Grid::Update(void)
{
}

void Grid::Draw(void)
{
	// �����W
	VECTOR sPos = { 0.0f, 0.0f, 0.0f };	// �J�n���W
	VECTOR ePos = { 0.0f, 0.0f, 0.0f };	// �I�����W

	// �O���b�h��������
	for (int ii = -H_LINE_NUM; ii < H_LINE_NUM; ++ii)
	{
		// ���̔ԍ�
		float lineNumber = static_cast<float>(ii);

		// X��(��)�����W�ݒ聨���`�恨���`��
		sPos = { -H_LINE_LEN, 0.0f, lineNumber * TERM };
		ePos = { H_LINE_LEN, 0.0f, lineNumber * TERM };
		DrawLine3D(sPos, ePos, 0xff0000);
		DrawSphere3D(ePos, SPHERE_RADIUS, SPHERE_DIV_NUM, 0xff0000, 0xff0000, true);

		// Z��(��)�����W�ݒ聨���`�恨���`��
		sPos = { lineNumber * TERM, 0.0f, -H_LINE_LEN };
		ePos = { lineNumber * TERM, 0.0f, H_LINE_LEN };
		DrawLine3D(sPos, ePos, 0x0000ff);
		DrawSphere3D(ePos, SPHERE_RADIUS, SPHERE_DIV_NUM, 0x0000ff, 0x0000ff, true);
	}

	// Y��(��)(��{)�����W�ݒ聨���`�恨���`��
	sPos = { 0.0f, -H_LINE_LEN, 0.0f };
	ePos = { 0.0f, H_LINE_LEN, 0.0f };
	DrawLine3D(sPos, ePos, 0x00ff00);
	DrawSphere3D(ePos, SPHERE_RADIUS, SPHERE_DIV_NUM, 0x00ff00, 0x00ff00, true);
}

void Grid::Release(void)
{
}