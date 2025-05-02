#include "Collision.h"
#include "Player.h"
#include "Stage.h"
#include "Enemy.h"
#include "../Manager/SceneManager.h"

Collision::Collision()
{
	stagemodel_ = -1;
}

Collision::~Collision()
{
}

void Collision::Init(void)
{
	stagemodel_ = Stage::GetInstance().GetStageModel();
}

void Collision::Update(const std::vector<Enemy*>& enemies)
{
	PlayerToStageCollision();
	PlayerToEnemyCollision(enemies);
}

void Collision::Draw(void)
{
	VECTOR playerpos = Player::GetInstance().GetPlayerPos();
	playerpos.y += 100.0f;
	DrawSphere3D(playerpos, 20, 32, 0xffffff, 0xffffff, false);

}

void Collision::Release(void)
{
}

void Collision::PlayerToStageCollision(void)
{
	VECTOR playerpos = Player::GetInstance().GetPlayerPos();
	playerpos.y += 100.0f;

	auto playertostage = MV1CollCheck_Sphere(stagemodel_, -1, playerpos, 20);
	if (playertostage.HitNum >=1)
	{
		//�ŏ��ɓ��������|���S���̖@�����g���ĉ����߂�
		VECTOR pushNormal = playertostage.Dim[0].Normal;
		VECTOR pushBack = VScale(pushNormal, 10.0f);

		//�v���C���[�̍��W���X�V
		VECTOR newplayerpos = VAdd(Player::GetInstance().GetPlayerPos(), pushBack);
		Player::GetInstance().SetPlayerPos(newplayerpos);
	}
	//���ʂ̉��
	MV1CollResultPolyDimTerminate(playertostage);
}

void Collision::EnemyToStageCollision(void)
{
}

void Collision::PlayerToEnemyCollision(const std::vector<Enemy*>& enemies)
{
	const float COLLISION_RADIUS = 50.0f;  // ����͈͔��a

	for (auto& enemy : enemies)
	{
		// ���S�ς݂̓G�͖���
		if (enemy->IsDead()) continue;

		VECTOR distVec = VSub(Player::GetInstance().GetPlayerPos(), enemy->GetPosition());
		// XZ���ʂ̂�
		distVec.y = 0.0f;

		if (VSize(distVec) < COLLISION_RADIUS)
		{
			// �v���C���[�������߂�
			VECTOR pushDir = VNorm(distVec);
			VECTOR pushBack = VScale(pushDir, COLLISION_RADIUS - VSize(distVec) + 0.1f);
			Player::GetInstance().SetPlayerPos(VAdd(Player::GetInstance().GetPlayerPos(), pushBack));
			Player::GetInstance().SetMoveSpeed(Player::MOVE_SPEED_STOP);

			// �ҋ@�A�j���[�V�����ɕύX
			Player::GetInstance().ChangeAnimation(0);

			if (Player::GetInstance().GetDamageCooldown() <= 0)
			{
				// HP�̌���
				Player::GetInstance().SetDecreaseHp(-10);
				//�N�[���^�C��(60�t���[���Ɉ��)
				Player::GetInstance().SetDamageCooldown(60);

				// HP��0�ȉ������S���Ă��Ȃ���Ύ��S����
				if (Player::GetInstance().GetHp()<=0 && !Player::GetInstance().GetIsDeadFlag())
				{
					Player::GetInstance().SetIsDead(true);
					// ���S�A�j���[�V����
					Player::GetInstance().ChangeAnimation(Player::Death, true);
					SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAMEOVER);
				}
			}
		}
	}
	//�N�[���^�C�����ꂸ���炷
	if (Player::GetInstance().GetDamageCooldown() > 0) Player::GetInstance().DecreaseCoolDown(1);
}