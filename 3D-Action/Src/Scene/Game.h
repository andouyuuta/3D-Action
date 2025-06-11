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
};