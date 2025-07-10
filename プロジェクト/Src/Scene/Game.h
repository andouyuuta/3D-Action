#pragma once
#include "SceneBase.h"

class Camera;
class Grid;
class Player;
class Sword;
class Enemy;
class EnemyManager;
class Stage;
class Collision;
class Aicon;
class Trail3D;
class AnimationManager;
class HpBar;
class Buff;
class MoviePlayer;
class BossEnemy;

class Game : public SceneBase
{
public:
	static constexpr int OPTION_COUNT = 2;

	// �R���X�g���N�^
	Game(void);

	// �f�X�g���N�^
	~Game(void) override;

	void Init(void) override;		// ������
	void Update(void) override;		// �X�V	
	void Draw(void) override;		// �`��
	void Release(void) override;	// ���

private:
	int menuimg_;
	int menuSizeX_;
	int menuSizeY_;
	bool isMenu_;

	int centerX_;
	int centerY_;

	int selectindex_;

	// �����Đ���h���悤�̃t���O
	bool isBossBGMStated;

	const float scale = 1.3f; // �g�嗦

	// �|�C���^�[
	// �J����
	Camera* camera_;

	// �v���C���[
	Player* player_;

	//��
	Sword* sword_;

	//�G
	EnemyManager* enemymng_;

	//�X�e�[�W
	Stage* stage_;

	// �A�C�R��
	Aicon* aicon_;

	//�����蔻��
	Collision* collision_;

	// �A�j���[�V����
	AnimationManager* animation_;

	// HP�o�[
	HpBar* hpBar_;

	// �o�t
	Buff* buff_;
};