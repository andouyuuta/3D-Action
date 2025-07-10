#include "RandomManager.h"

// ���̂̐���
std::mt19937 RandomManager::mt;		// �����_���G���W���̎���
std::uniform_real_distribution<float> RandomManager::dist(0.0f, 100.0f); // �����̏o��͈͂̐ݒ�

// ������
void RandomManager::Init()
{
	std::random_device rd;		// �����_���f�o�C�X�i�������������鑕�u�j
	mt.seed(rd());				// �����Z���k�E�c�C�X�^(mt)��rd()�̒l���V�[�h�Ƃ��ė^����
}

// �N���e�B�J�����N���邩�̔���
bool RandomManager::Random(float criticalRatePercent)
{
	// dist(mt)��0.0�`100.0�͈̔͂�float�������_���ɐ���
	// ���̒l��criticalRatePercent�����ł���΃N���e�B�J������
	return dist(mt) < criticalRatePercent;
}