#pragma once
#include <DxLib.h>
#include <vector>
#include "Enemy.h"

class EnemyManager
{
public:
	EnemyManager(void);	// �R���X�g���N�^
	~EnemyManager(void); // �f�X�g���N�^

	void Init();    // �G�̏�����
	void Update();  // �G�̍X�V
	void Draw();    // �G�̕`��
	void Release(); // �������

	// �v���C���[���œG�̃��X�g���擾���邽�߂̊֐�
	const std::vector<Enemy*>& GetEnemies() const;

private:
	std::vector<Enemy*> enemyies_;			// �G�̃��X�g
	int enemymodel_;
	int bossenemymodel_;

	static constexpr int ENEMY_COUNT = 100;	// �G�̐�
};