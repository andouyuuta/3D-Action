#include "Collision.h"
#include "Player.h"
#include "Stage.h"
#include "Enemy.h"
#include "Sword.h"
#include "../Manager/SceneManager.h"

Collision::Collision()
{
	stagemodel_ = -1;
	enemy_ = nullptr;
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
	PlayerToStageCollision();			//�v���C���[�ƃX�e�[�W�̓����蔻��
	EnemyToStageCollision(enemies);		//�G�ƃX�e�[�W�̓����蔻��
	PlayerToEnemyCollision(enemies);	//�v���C���[�ƓG�̓����蔻��
	EnemyToEnemyCollision(enemies);		//�G�ƓG�̓����蔻��
	SwordToEnemyCollision(enemies);		// ���ƓG�̓����蔻��
}

void Collision::Draw(void)
{
	DrawDebag();
}

void Collision::Release(void)
{
}

// �v���C���[�ƃX�e�[�W�̓����蔻��
void Collision::PlayerToStageCollision(void)
{
	// �v���C���[�̈ʒu���擾�A����
	VECTOR playerpos = VAdd(Player::GetInstance().GetPlayerPos(), VGet(0.0f, 100.0f, 0.0f));

	// ���̂ŃX�e�[�W�Ƃ̓����蔻������s
	auto playertostage = MV1CollCheck_Sphere(stagemodel_, -1, playerpos, 20);
	if (playertostage.HitNum >=1)
	{
		// �ŏ��ɓ��������|���S���̖@�����g���ĉ����߂�
		VECTOR pushNormal = playertostage.Dim[0].Normal;
		VECTOR pushBack = VScale(pushNormal, 10.0f);

		// �v���C���[�̍��W���X�V
		VECTOR newplayerpos = VAdd(Player::GetInstance().GetPlayerPos(), pushBack);
		Player::GetInstance().SetPlayerPos(newplayerpos);
	}
	// ���ʂ̉��
	MV1CollResultPolyDimTerminate(playertostage);
}

// �G�ƃX�e�[�W�̓����蔻��
void Collision::EnemyToStageCollision(const std::vector<Enemy*>& enemies)
{
	for (auto& enemy : enemies)
	{
		// ���S�ς݂̓G�͖���
		if (enemy->IsDead())continue;

		// �ʒu�擾
		VECTOR enemypos = VAdd(enemy->GetPosition(), VGet(0.0f, 100.0f, 0.0f)); // y����ǉ�

		// ���ɂ�铖���蔻��
		auto enemytostage = MV1CollCheck_Sphere(stagemodel_, -1, enemypos, 20.0f);

		if (enemytostage.HitNum >= 1)
		{
			// �ŏ��ɓ��������|���S���̖@�����g���ĉ����߂�
			VECTOR pushnormal = enemytostage.Dim[0].Normal;
			VECTOR pushback = VScale(pushnormal, 10.0f);

			VECTOR newenemypos = VAdd(enemy->GetPosition(), pushback);
			enemy->SetEnemyPos(newenemypos);
		}
		// ���ʂ̉��
		MV1CollResultPolyDimTerminate(enemytostage);
	}
}

// �v���C���[�ƓG�̓����蔻��
void Collision::PlayerToEnemyCollision(const std::vector<Enemy*>& enemies)
{
	const float CHECK_RADIUS = 100.0f;    // �������蔻��p��XZ�����i�L�߁j

	VECTOR playerPos = Player::GetInstance().GetPlayerPos();
	VECTOR checkPos = VAdd(playerPos,VGet(0.0f,100.0f,0.0f));

	for (auto& enemy : enemies)
	{
		if (enemy->IsDead()) continue;

		// XZ���������Ń`�F�b�N
		VECTOR dist = VSub(playerPos, enemy->GetPosition());
		dist.y = 0;
		if (VSize(dist) > CHECK_RADIUS) continue; // �͈͊O�Ȃ�X�L�b�v

		//�����ȃ|���S�������蔻��
		int modelHandle = enemy->GetModel(); // ���f���n���h���擾�iEnemy���ŗp�Ӂj
		auto result = MV1CollCheck_Sphere(modelHandle, -1, checkPos, COLLISION_RADIUS);

		if (result.HitNum >= 1)
		{
			// �����߂��Ȃǂ̏���
			VECTOR pushNormal = result.Dim[0].Normal;
			VECTOR pushBack = VScale(pushNormal, 7.0f);
			Player::GetInstance().SetPlayerPos(VAdd(playerPos, pushBack));
		}
		MV1CollResultPolyDimTerminate(result);
		//�G���U�������Ƃ��̓����蔻��
		EnemyAttackCollision(enemies);

	}

	// �N�[���_�E������b����
	if (Player::GetInstance().GetDamageCooldown() > 0)
	{
		Player::GetInstance().DecreaseCoolDown(1);
	}
}

void Collision::EnemyAttackCollision(const std::vector<Enemy*>& enemies)
{
	// �G�Ƃ̓����蔻��
	for (auto& enemy : enemies)
	{
		if (enemy->GetAttackFlg())
		{
			VECTOR righthandpos = enemy->GetRightHandPosition();
			float angleY = enemy->GetRotY();  // �v���C���[��Y����]
			float attackradius = 20.0f;

			// �v���C���[��Y����]�i�O��j�ɉ����āAX����]�i�㉺�j�Ŏ΂ߏ�ɃI�t�Z�b�g
			VECTOR forward = VGet(sinf(angleY), 0.0f, cosf(angleY));

			VECTOR attackCenter = VAdd(righthandpos, forward);

			auto result = MV1CollCheck_Sphere(Player::GetInstance().GetPlayerModel(), -1, attackCenter, attackradius);
			if (result.HitNum > 0)
			{
				// �N�[���_�E�����łȂ���΃_���[�W���󂯂�
				if (Player::GetInstance().GetDamageCooldown() <= 0)
				{
					Player::GetInstance().SetDamage(10);
					Player::GetInstance().SetDamageCooldown(60);	// ���G���Ԃ̐ݒ�

					if (Player::GetInstance().GetHp() <= 0 && !Player::GetInstance().GetIsDeadFlag())
					{
						Player::GetInstance().ChangeAnimation(Player::Death, true);
						SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAMEOVER);
					}
				}
			}
		}
	}
}


// �G���m�̓����蔻��
void Collision::EnemyToEnemyCollision(const std::vector<Enemy*> enemies)
{
	for (size_t i = 0; i < enemies.size(); ++i)
	{
		Enemy* enemyA = enemies[i];
		if (!enemyA || enemyA->IsDead())continue;

		VECTOR posA = enemyA->GetPosition();

		//���ׂĂ̓G�̃y�A���`�F�b�N
		for (size_t j = i + 1; j < enemies.size(); ++j)
		{
			Enemy* enemyB = enemies[j];
			if (!enemyB || enemyB->IsDead())continue;

			VECTOR posB = enemyB->GetPosition();

			// 2�̊Ԃ̃x�N�g���Ƌ������v�Z
			VECTOR diff = VSub(posB, posA);
			float distance = VSize(diff);

			// �������߂�����ꍇ�ɏ���
			if (distance < COLLISION_RADIUS * 2.0f && distance>0.0f)
			{
				//�����Ԃ��x�N�g�����v�Z
				VECTOR pushDir = VNorm(diff);
				float overlap = COLLISION_RADIUS * 2.0f - distance;
				VECTOR pushVec = VScale(pushDir, overlap * 0.5f);

				//�݂��ɉ����Ԃ�
				enemyA->SetEnemyPos(VSub(posA, pushVec));
				enemyB->SetEnemyPos(VAdd(posB, pushVec));
			}
		}
	}
}

void Collision::SwordToEnemyCollision(const std::vector<Enemy*>& enemies)
{
	if (Player::GetInstance().GetWeaponFlag() && Player::GetInstance().GetAttackFlag()) {

		// ���̍U������p�̈ʒu�Ɣ��a
		VECTOR swordBase = Sword::GetInstance().GetSwordPosition();
		float swordRadius = 30.0f;

		// �v���C���[�̌����ɉ����Č��̒��S�փI�t�Z�b�g����
		float offset = 50.0f;  // �΂ߏ�Ɉړ����鋗��
		float angleY = Player::GetInstance().GetRotY();  // �v���C���[��Y����]
		float angleX = Player::GetInstance().GetRotX();  // �v���C���[��X����]�i�㉺��]�j

		// �v���C���[��Y����]�i�O��j�ɉ����āAX����]�i�㉺�j�Ŏ΂ߏ�ɃI�t�Z�b�g
		VECTOR forward = VGet(sinf(angleY) * offset, sinf(angleX) * (offset + 30.0f), cosf(angleY) * offset);

		// ���̒��S�ʒu���I�t�Z�b�g�Œ���
		VECTOR swordCenter = VAdd(swordBase, forward);

		// �G�Ƃ̓����蔻��
		for (auto& enemy : enemies)
		{
			if (!enemy || enemy->IsDead()) continue;

			int enemyModel = enemy->GetModel();
			auto result = MV1CollCheck_Sphere(enemyModel, -1, swordCenter, swordRadius);
			if (result.HitNum > 0)
			{
				enemy->SetDamage(10);
			}
			MV1CollResultPolyDimTerminate(result);
		}

	}
}


void Collision::DrawDebag()
{
	//�v���C���[�̓���
	VECTOR playerpos = Player::GetInstance().GetPlayerPos();
	playerpos.y += 100.0f;
	DrawSphere3D(playerpos, 20, 32, 0xffffff, 0xffffff, false);

	// ���̍U������p�̈ʒu�Ɣ��a
	VECTOR swordBase = Sword::GetInstance().GetSwordPosition();
	float swordRadius = 30.0f;
	VECTOR swordCenter = swordBase;
	// �v���C���[�̌����ɑO���I�t�Z�b�g�iXZ���ʁj
	float offset = 30.0f;
	float angleY = Player::GetInstance().GetRotY();
	swordCenter.x += sinf(angleY) * offset;
	swordCenter.z += cosf(angleY) * offset;
	// Y���i������j�ɉ��Z
	swordCenter.y += 50.0f;
	DrawSphere3D(swordCenter, swordRadius, 32, 0xffffff, 0xffffff, false);
}
