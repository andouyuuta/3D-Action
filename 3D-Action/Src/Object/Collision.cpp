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
		//最初に当たったポリゴンの法線を使って押し戻す
		VECTOR pushNormal = playertostage.Dim[0].Normal;
		VECTOR pushBack = VScale(pushNormal, 10.0f);

		//プレイヤーの座標を更新
		VECTOR newplayerpos = VAdd(Player::GetInstance().GetPlayerPos(), pushBack);
		Player::GetInstance().SetPlayerPos(newplayerpos);
	}
	//結果の解放
	MV1CollResultPolyDimTerminate(playertostage);
}

void Collision::EnemyToStageCollision(void)
{
}

void Collision::PlayerToEnemyCollision(const std::vector<Enemy*>& enemies)
{
	const float COLLISION_RADIUS = 50.0f;  // 判定範囲半径

	for (auto& enemy : enemies)
	{
		// 死亡済みの敵は無視
		if (enemy->IsDead()) continue;

		VECTOR distVec = VSub(Player::GetInstance().GetPlayerPos(), enemy->GetPosition());
		// XZ平面のみ
		distVec.y = 0.0f;

		if (VSize(distVec) < COLLISION_RADIUS)
		{
			// プレイヤーを押し戻す
			VECTOR pushDir = VNorm(distVec);
			VECTOR pushBack = VScale(pushDir, COLLISION_RADIUS - VSize(distVec) + 0.1f);
			Player::GetInstance().SetPlayerPos(VAdd(Player::GetInstance().GetPlayerPos(), pushBack));
			Player::GetInstance().SetMoveSpeed(Player::MOVE_SPEED_STOP);

			// 待機アニメーションに変更
			Player::GetInstance().ChangeAnimation(0);

			if (Player::GetInstance().GetDamageCooldown() <= 0)
			{
				// HPの減少
				Player::GetInstance().SetDecreaseHp(-10);
				//クールタイム(60フレームに一回)
				Player::GetInstance().SetDamageCooldown(60);

				// HPが0以下かつ死亡していなければ死亡処理
				if (Player::GetInstance().GetHp()<=0 && !Player::GetInstance().GetIsDeadFlag())
				{
					Player::GetInstance().SetIsDead(true);
					// 死亡アニメーション
					Player::GetInstance().ChangeAnimation(Player::Death, true);
					SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAMEOVER);
				}
			}
		}
	}
	//クールタイムを一ずつ減らす
	if (Player::GetInstance().GetDamageCooldown() > 0) Player::GetInstance().DecreaseCoolDown(1);
}