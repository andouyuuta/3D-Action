#pragma once
#include <random>

class RandomManager
{
public:
	static void Init(void);									// ������
	static bool Critical(float criticalRatePercent);		// �N���e�B�J������������

private:
	static std::mt19937 mt;								// �����Z���k�E�c�C�X�^(�^�������G���W��)
	static std::uniform_real_distribution<float>dist;	// 0.0�`100.0
};

