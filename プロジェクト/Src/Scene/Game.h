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

class Game : public SceneBase
{
public:
	static constexpr int OPTION_COUNT = 2;

	// コンストラクタ
	Game(void);

	// デストラクタ
	~Game(void) override;

	void Init(void) override;		// 初期化
	void Update(void) override;		// 更新	
	void Draw(void) override;		// 描画
	void Release(void) override;	// 解放

	// カメラの取得
	Camera* GetCamera(void) const;

private:
	int menuimg_;
	int menuSizeX_;
	int menuSizeY_;
	bool isMenu_;

	int centerX_;
	int centerY_;

	int selectindex_;

	// ポインター
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

	// アイコン
	Aicon* aicon_;

	//当たり判定
	Collision* collision_;

	// アニメーション
	AnimationManager* animation_;

	// HPバー
	HpBar* hpbar_;

	// バフ
	Buff* buff_;
};