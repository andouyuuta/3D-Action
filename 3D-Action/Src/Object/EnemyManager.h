#pragma once
#include <DxLib.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>

class Enemy;
class BossEnemy;

class EnemyManager
{
public:
	static constexpr int ENEMY_COUNT = 1;				// �G�̐�
	

	EnemyManager(void);	// �R���X�g���N�^
	~EnemyManager(void); // �f�X�g���N�^

	void Init();    // �G�̏�����
	void Update();  // �G�̍X�V
	void Draw();    // �G�̕`��
	void Release(); // �������


	// �v���C���[���œG�̃��X�g���擾���邽�߂̊֐�
	std::vector<Enemy*> GetEnemyPtrs() const;
	[[nodiscard]] int GetEnemyCount(void) { return ENEMY_COUNT; }
	[[nodiscard]] int GetAliveEnemyCount()const;
	[[nodiscard]] bool GetBossDead()const { return bossDead_; }
	[[nodiscard]] void SetBossDead(bool dead) { bossDead_ = dead; }

	int currentPhase_ = 0;						// ���݂̃t�F�[�Y�i1~4�j
	const int MAX_PHASE = 3;					// �t�F�[�Y�̍ő吔
	bool isWaitingForSkillSelect_ = false;		// �X�L���̑I�����҂�����

private:
	std::vector<Enemy*> result;
	std::vector<std::unique_ptr<Enemy>> enemies_;
	std::unordered_map<std::string, int> enemyModels;	//���f����z��ŊǗ�
	
	int deadCount_;		// �ʏ�̓G��|������
	bool bossSpawned_;	// �{�X�o��
	bool bossDead_;     // �{�X�̎��S
	bool isPhaseClear_;	// �t�F�[�Y�̃t���O

	void SpawnBoss();

	void SpawnPhaseEnemies();	// �X�|�[���t�F�[�Y�̓G�̏���
	void ShowSkillSelect();		// �X�L��
};