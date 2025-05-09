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
	PlayerToStageCollision();			//プレイヤーとステージの当たり判定
	EnemyToStageCollision(enemies);		//敵とステージの当たり判定
	PlayerToEnemyCollision(enemies);	//プレイヤーと敵の当たり判定
	EnemyToEnemyCollision(enemies);		//敵と敵の当たり判定
	SwordToEnemyCollision(enemies);		// 剣と敵の当たり判定
}

void Collision::Draw(void)
{
	DrawDebag();
}

void Collision::Release(void)
{
}

// プレイヤーとステージの当たり判定
void Collision::PlayerToStageCollision(void)
{
	// プレイヤーの位置を取得、調整
	VECTOR playerpos = VAdd(Player::GetInstance().GetPlayerPos(), VGet(0.0f, 100.0f, 0.0f));

	// 球体でステージとの当たり判定を実行
	auto playertostage = MV1CollCheck_Sphere(stagemodel_, -1, playerpos, 20);
	if (playertostage.HitNum >=1)
	{
		// 最初に当たったポリゴンの法線を使って押し戻す
		VECTOR pushNormal = playertostage.Dim[0].Normal;
		VECTOR pushBack = VScale(pushNormal, 10.0f);

		// プレイヤーの座標を更新
		VECTOR newplayerpos = VAdd(Player::GetInstance().GetPlayerPos(), pushBack);
		Player::GetInstance().SetPlayerPos(newplayerpos);
	}
	// 結果の解放
	MV1CollResultPolyDimTerminate(playertostage);
}

// 敵とステージの当たり判定
void Collision::EnemyToStageCollision(const std::vector<Enemy*>& enemies)
{
	for (auto& enemy : enemies)
	{
		// 死亡済みの敵は無視
		if (enemy->IsDead())continue;

		// 位置取得
		VECTOR enemypos = VAdd(enemy->GetPosition(), VGet(0.0f, 100.0f, 0.0f)); // y軸を追加

		// 球による当たり判定
		auto enemytostage = MV1CollCheck_Sphere(stagemodel_, -1, enemypos, 20.0f);

		if (enemytostage.HitNum >= 1)
		{
			// 最初に当たったポリゴンの法線を使って押し戻す
			VECTOR pushnormal = enemytostage.Dim[0].Normal;
			VECTOR pushback = VScale(pushnormal, 10.0f);

			VECTOR newenemypos = VAdd(enemy->GetPosition(), pushback);
			enemy->SetEnemyPos(newenemypos);
		}
		// 結果の解放
		MV1CollResultPolyDimTerminate(enemytostage);
	}
}

// プレイヤーと敵の当たり判定
void Collision::PlayerToEnemyCollision(const std::vector<Enemy*>& enemies)
{
	const float CHECK_RADIUS = 100.0f;    // ざっくり判定用のXZ距離（広め）

	VECTOR playerPos = Player::GetInstance().GetPlayerPos();
	VECTOR checkPos = VAdd(playerPos,VGet(0.0f,100.0f,0.0f));

	for (auto& enemy : enemies)
	{
		if (enemy->IsDead()) continue;

		// XZ距離だけでチェック
		VECTOR dist = VSub(playerPos, enemy->GetPosition());
		dist.y = 0;
		if (VSize(dist) > CHECK_RADIUS) continue; // 範囲外ならスキップ

		//精密なポリゴン当たり判定
		int modelHandle = enemy->GetModel(); // モデルハンドル取得（Enemy側で用意）
		auto result = MV1CollCheck_Sphere(modelHandle, -1, checkPos, COLLISION_RADIUS);

		if (result.HitNum >= 1)
		{
			// 押し戻しなどの処理
			VECTOR pushNormal = result.Dim[0].Normal;
			VECTOR pushBack = VScale(pushNormal, 7.0f);
			Player::GetInstance().SetPlayerPos(VAdd(playerPos, pushBack));
		}
		MV1CollResultPolyDimTerminate(result);
		//敵が攻撃したときの当たり判定
		EnemyAttackCollision(enemies);

	}

	// クールダウンを一秒減少
	if (Player::GetInstance().GetDamageCooldown() > 0)
	{
		Player::GetInstance().DecreaseCoolDown(1);
	}
}

void Collision::EnemyAttackCollision(const std::vector<Enemy*>& enemies)
{
	// 敵との当たり判定
	for (auto& enemy : enemies)
	{
		if (enemy->GetAttackFlg())
		{
			VECTOR righthandpos = enemy->GetRightHandPosition();
			float angleY = enemy->GetRotY();  // プレイヤーのY軸回転
			float attackradius = 20.0f;

			// プレイヤーのY軸回転（前後）に加えて、X軸回転（上下）で斜め上にオフセット
			VECTOR forward = VGet(sinf(angleY), 0.0f, cosf(angleY));

			VECTOR attackCenter = VAdd(righthandpos, forward);

			auto result = MV1CollCheck_Sphere(Player::GetInstance().GetPlayerModel(), -1, attackCenter, attackradius);
			if (result.HitNum > 0)
			{
				// クールダウン中でなければダメージを受ける
				if (Player::GetInstance().GetDamageCooldown() <= 0)
				{
					Player::GetInstance().SetDamage(10);
					Player::GetInstance().SetDamageCooldown(60);	// 無敵時間の設定

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


// 敵同士の当たり判定
void Collision::EnemyToEnemyCollision(const std::vector<Enemy*> enemies)
{
	for (size_t i = 0; i < enemies.size(); ++i)
	{
		Enemy* enemyA = enemies[i];
		if (!enemyA || enemyA->IsDead())continue;

		VECTOR posA = enemyA->GetPosition();

		//すべての敵のペアをチェック
		for (size_t j = i + 1; j < enemies.size(); ++j)
		{
			Enemy* enemyB = enemies[j];
			if (!enemyB || enemyB->IsDead())continue;

			VECTOR posB = enemyB->GetPosition();

			// 2体間のベクトルと距離を計算
			VECTOR diff = VSub(posB, posA);
			float distance = VSize(diff);

			// 距離が近すぎる場合に処理
			if (distance < COLLISION_RADIUS * 2.0f && distance>0.0f)
			{
				//押し返しベクトルを計算
				VECTOR pushDir = VNorm(diff);
				float overlap = COLLISION_RADIUS * 2.0f - distance;
				VECTOR pushVec = VScale(pushDir, overlap * 0.5f);

				//互いに押し返す
				enemyA->SetEnemyPos(VSub(posA, pushVec));
				enemyB->SetEnemyPos(VAdd(posB, pushVec));
			}
		}
	}
}

void Collision::SwordToEnemyCollision(const std::vector<Enemy*>& enemies)
{
	if (Player::GetInstance().GetWeaponFlag() && Player::GetInstance().GetAttackFlag()) {

		// 剣の攻撃判定用の位置と半径
		VECTOR swordBase = Sword::GetInstance().GetSwordPosition();
		float swordRadius = 30.0f;

		// プレイヤーの向きに応じて剣の中心へオフセットする
		float offset = 50.0f;  // 斜め上に移動する距離
		float angleY = Player::GetInstance().GetRotY();  // プレイヤーのY軸回転
		float angleX = Player::GetInstance().GetRotX();  // プレイヤーのX軸回転（上下回転）

		// プレイヤーのY軸回転（前後）に加えて、X軸回転（上下）で斜め上にオフセット
		VECTOR forward = VGet(sinf(angleY) * offset, sinf(angleX) * (offset + 30.0f), cosf(angleY) * offset);

		// 剣の中心位置をオフセットで調整
		VECTOR swordCenter = VAdd(swordBase, forward);

		// 敵との当たり判定
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
	//プレイヤーの胴体
	VECTOR playerpos = Player::GetInstance().GetPlayerPos();
	playerpos.y += 100.0f;
	DrawSphere3D(playerpos, 20, 32, 0xffffff, 0xffffff, false);

	// 剣の攻撃判定用の位置と半径
	VECTOR swordBase = Sword::GetInstance().GetSwordPosition();
	float swordRadius = 30.0f;
	VECTOR swordCenter = swordBase;
	// プレイヤーの向きに前方オフセット（XZ平面）
	float offset = 30.0f;
	float angleY = Player::GetInstance().GetRotY();
	swordCenter.x += sinf(angleY) * offset;
	swordCenter.z += cosf(angleY) * offset;
	// Y軸（上方向）に加算
	swordCenter.y += 50.0f;
	DrawSphere3D(swordCenter, swordRadius, 32, 0xffffff, 0xffffff, false);
}
