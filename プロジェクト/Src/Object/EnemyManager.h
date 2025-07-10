#pragma once
#include <DxLib.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include "../Manager/AnimationManager.h"
#include <set>

class Player;
class Enemy;
class BossEnemy;
class AnimationManager;
class Buff;

class EnemyManager
{
public:
	static constexpr int ENEMY_COUNT = 1;				// �G�̐�

	EnemyManager(void);		// �R���X�g���N�^
	~EnemyManager(void);	// �f�X�g���N�^

	void Init(Player* player, AnimationManager* anim, const AnimationManager::AnimationInfo& animinfo, Buff* buff);    // �G�̏�����
	void Update(void);		// �G�̍X�V
	void Draw(void);		// �G�̕`��
	void Release(void);		// �������

	// �Q�b�^�[�֐�
	[[nodiscard]] std::vector<Enemy*> GetEnemyPtrs(void) const;
	[[nodiscard]] int GetEnemyCount(void) { return ENEMY_COUNT; }
	[[nodiscard]] int GetAliveEnemyCount(void)const;
	[[nodiscard]] bool GetBossDead(void)const { return bossDead_; }
	[[nodiscard]] void SetBossDead(bool dead) { bossDead_ = dead; }

	int currentPhase_ = 0;						// ���݂̃t�F�[�Y
	const int MAX_PHASE = 5;					// �t�F�[�Y�̍ő吔
	bool isWaitingForSkillSelect_ = false;		// �X�L���̑I�����҂�����
	bool IsBoss_;

	bool GetBossSpawn(void) { return bossSpawned_; }
	bool GetIsBoss(void) { return IsBoss_; }

	// ��Ԏ擾�֐�
	bool IsWaitingForSkillSelect(void) const;

	// �Q�b�^�[
	int GetTotalHp(void)const { return totalHp_; }
	int GetTotalAttack(void)const { return totalAttack_; }

	void SetIsWaitingSlect(bool select) { isWaitingForSkillSelect_ = select; }
	void ShowSkillSelect(void);		// �X�L���I��
private:

	// �|�C���^�[
	Buff* buff_;
	Enemy* enemy_;
	BossEnemy* boss_;

	std::vector<Enemy*> result;
	std::vector<std::unique_ptr<Enemy>> enemies_;
	std::unordered_map<std::string, int> enemyModels;	//���f����z��ŊǗ�
	int deadCount_;					// �ʏ�̓G��|������
	bool bossSpawned_;				// �{�X�o��
	bool bossDead_;					// �{�X�̎��S
	bool isPhaseClear_;				// �t�F�[�Y�̃t���O
	int totalHp_;
	int totalAttack_;

	void SpawnBoss(void);

	void SpawnPhaseEnemies(void);	// �X�|�[���t�F�[�Y�̓G�̏���

	Player* player_;
	AnimationManager* animation_;
	AnimationManager::AnimationInfo animInfo_;

	
};