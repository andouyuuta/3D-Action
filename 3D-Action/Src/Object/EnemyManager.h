#pragma once
#include "Enemy.h"
#include <DxLib.h>
#include <vector>

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

	[[nodiscard]] int GetEnemyCount(void) { return ENEMY_COUNT; }
	[[nodiscard]] int GetAliveEnemyCount()const;
private:
	std::vector<Enemy*> enemyies_;			// �G�̃��X�g
	int enemymodel_;
	int bossenemymodel_;

	int ENEMY_COUNT = 10;	// �G�̐�
};