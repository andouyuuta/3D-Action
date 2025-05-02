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

class Game : public SceneBase
{
public:
	// �R���X�g���N�^
	Game(void);

	// �f�X�g���N�^
	~Game(void) override;

	void Init(void) override;		// ������
	void Update(void) override;		// �X�V	
	void Draw(void) override;		// �`��
	void Release(void) override;	// ���

private:
	// �O���b�h��
	Grid* grid_;

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

	//�����蔻��
	Collision* collision_;
};