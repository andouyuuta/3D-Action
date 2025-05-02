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
	// コンストラクタ
	Game(void);

	// デストラクタ
	~Game(void) override;

	void Init(void) override;		// 初期化
	void Update(void) override;		// 更新	
	void Draw(void) override;		// 描画
	void Release(void) override;	// 解放

private:
	// グリッド線
	Grid* grid_;

	// カメラ
	Camera* camera_;

	// プレイヤー
	Player* player_;

	//剣
	Sword* sword_;

	//敵
	EnemyManager* enemymng_;

	//ステージ
	Stage* stage_;

	//当たり判定
	Collision* collision_;
};