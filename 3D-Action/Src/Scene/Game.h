#pragma once
#include "SceneBase.h"

class Camera;
class Grid;
class Player;
class Enemy;
class EnemyManager;

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
	// ���e�B�N��
	int dot_;

	// �O���b�h��
	Grid* grid_;

	// �J����
	Camera* camera_;

	// �v���C���[
	Player* player_;

	//�G
	Enemy* enemy_;
	EnemyManager* enemymng_;

	// �摜

};