#pragma once

class Grid
{
public:
	// �R���X�g���N�^
	Grid(void);

	// �f�X�g���N�^
	~Grid(void);

	void SystemInit(void);
	void GameInit(void);
	void Update(void);
	void Draw(void);
	void Release(void);

private:
	// ���̒���
	static constexpr float LINE_LEN = 1200.0f;

	// ���̒����̔���
	static constexpr float H_LINE_LEN = LINE_LEN / 2.0f;

	// ���̊Ԋu
	static constexpr float TERM = 50.0f;

	// ���̂̔��a
	static constexpr float SPHERE_RADIUS = 20.0f;

	// ���̂̔��a
	static const int LINE_NUM = static_cast<int>(LINE_LEN / TERM);

	// ���̐��̔���
	static const int H_LINE_NUM = LINE_NUM / 2;

	// ���̂��`������|���S���ׂ̍���
	static const int SPHERE_DIV_NUM = 10;
};