#include <DxLib.h>
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "Player.h"
#include "Sword.h"
#include "Enemy.h"
#include "EnemyManager.h"
#include "Stage.h"

Player* Player::instance_ = nullptr;

void Player::CreateInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new Player();
	}
	instance_->Init();
}

Player& Player::GetInstance(void)
{
	return *instance_;
}


Player::Player(void)
{
	list.modelid_ = -1;

	list. pos_ = { 0.0f,0.0f,0.0f };
	list.rot_ = { 0.0f,0.0f,0.0f };
	list.scale_ = { 0.0f,0.0f,0.0f };
	list.moveSpeed_ = 0.0f;
	list.moveDir_ = 0;
	list.moveKind_ = 0;

	list.moveVec_ = { 0.0f,0.0f,0.0f };
	list.oldMoveVec_ = { 0.0f,0.0f,0.0f };
	list.moveVecRad_ = { 0.0f,0.0f,0.0f };

	list.localRot_ = { 0.0f,0.0f,0.0f };

	list.animindex_ = 0;
	list.animAttachNo_ = 0;
	list.animTotalTime_ = 0.0f;
	list.currentAnimTime_ = 0.0f;
	list.animlockflg_ = false;

	list.isdead_ = false;
	list.crouchflg_ = false;
	list.crouchattackflg_ = false;
	list.weaponflg_ = false;

	damageCooldown_ = 0;
}

Player::~Player(void)
{
}

void Player::Init(void)
{
	// モデルの読み込み
	list.modelid_ = MV1LoadModel("Data/Model/PlayerModel.mv1");

	// プレイヤーの初期位置設定
	list.pos_ = { 0.0f, 0.0f, -550.0f };
	MV1SetPosition(list.modelid_, list.pos_);

	// プレイヤーの角度設定
	list.rot_ = { 0.0f, 0.0f, 0.0f };
	MV1SetRotationXYZ(list.modelid_, list.rot_);

	// 移動ベクトルが作成する角度
	list.moveVecRad_ = { 0.0f, 0.0f, 0.0f };

	// 直前の移動ベクトル
	list.oldMoveVec_ = { 0.0f, 0.0f, 0.0f };

	// プレイヤーの角度
	list.localRot_ = INIT_MODEL_ROT_OFFSET;

	// プレイヤーの移動速度
	list.moveSpeed_ = MOVE_SPEED_WALK;

	// プレイヤーの移動状態
	list.moveKind_ = 0;

	//死んでいるか
	list.isdead_ = false;

	//しゃがんでいるか
	list.crouchflg_ = false;

	//しゃがみ攻撃しているか
	list.crouchattackflg_ = false;		

	//武器出しているか
	list.weaponflg_ = false;

	//攻撃しているか
	list.attackflg_ = false;

	//攻撃段階
	list.comboflg_ = false;
	list.combostep_ = 0;

	// HPの初期化
	list.hp_ = PLAYER_MAX_HP;

	// ダメージ間隔制御用
	damageCooldown_ = 0;

	//アニメーション関連
	list.animindex_ = 1;
	list.animAttachNo_ = MV1AttachAnim(list.modelid_, list.animindex_);
	list.animTotalTime_ = MV1GetAttachAnimTotalTime(list.modelid_, list.animAttachNo_);
	list.currentAnimTime_ = 0.0f;
	list.animlockflg_ = false;
	MV1SetAttachAnimTime(list.modelid_, list.animAttachNo_, list.currentAnimTime_);

	list.currentratio_ = 0.0f;
	list.remainingtime_ = 0.0f;

	MV1SetupCollInfo(list.modelid_);
}

void Player::Update(const std::vector<Enemy*>& enemies)
{
	// アニメーション処理
 	PlayAnimation();
	//CheckEnemyCollision(enemies);	// 当たり判定
	//DebugAnimation();
	//移動処理
	UpdateMove();
	SetRotation();
}

void Player::UpdateMove(void)
{
	// 入力制御取得
	InputManager& ins = InputManager::GetInstance();

	//死亡時にも移動させない
	if (list.isdead_)
	{
		list.moveSpeed_ = MOVE_SPEED_STOP;
		ChangeAnimation(Death);
		return;
	}

	if (ins.IsTrgMouseRight())
	{
		list.weaponflg_ = !list.weaponflg_;
		if (list.weaponflg_)
		{
			ChangeAnimation(WeaponOut, true);					//武器取り出し
		}
		else {
			ChangeAnimation(Sheach, true);						//武器しまう
		}
		return;
	}

	if (!list.weaponflg_)
	{
		PlayerMove(NoWeaponIdle, NoWeaponWalk, NoWeaponRun);	//武器持っていないときの動き
	}
	else
	{
		PlayerMove(WeaponIdle, WeaponWalk, WeaponRun);			//武器持っているときの動き
	}
}

void Player::Draw(void)
{
	// モデルの描画
	MV1DrawModel(list.modelid_);

	//// プレイヤー座標表示
	//DrawFormatString(20, 120, GetColor(0xff, 0xff, 0xff), "プレイヤーの座標 : (X, Y, Z) = (% 1.2lf, % 1.2lf, % 1.2lf)", list.pos_.x, list.pos_.y, list.pos_.z);
}

void Player::Release(void)
{
	MV1DeleteModel(list.modelid_);
}

bool Player::IsMove(VECTOR _moveVec)
{
	// XYZすべての座標の移動量の絶対値をとる
	float absX = abs(_moveVec.x);
	float absY = abs(_moveVec.y);
	float absZ = abs(_moveVec.z);

	// 移動ベクトルが X・Y・Z すべて移動されてなければ
	bool isNoMoveX = absX < FLT_EPSILON;
	bool isNoMoveY = absY < FLT_EPSILON;
	bool isNoMoveZ = absZ < FLT_EPSILON;

	// 座標更新せずに抜ける
	if (isNoMoveX && isNoMoveY && isNoMoveZ)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void Player::SetRotation(void)
{
	//------------------------------------
	// 回転行列を使用した角度設定
	//------------------------------------
	
	// 単位行列を設定する
	MATRIX mat = MGetIdent();

	// モデル自体のY軸回転行列を作成する
	MATRIX mGetRotY = MGetRotY(list.rot_.y);

	// モデルの補正用Y軸回転行列を作成する
	MATRIX mGetLocalRotY = MGetRotY(list.localRot_.y);

	// 行列を合成
	mat = MMult(mat, mGetRotY);
	mat = MMult(mat, mGetLocalRotY);

	// 行列を使用してモデルの角度を設定
	MV1SetRotationMatrix(list.modelid_, mat);
}

// 当たり判定
void Player::CheckEnemyCollision(const std::vector<Enemy*>& enemies)
{
	const float COLLISION_RADIUS = 50.0f;  // 判定範囲半径

	for (auto& enemy : enemies)
	{
		// 死亡済みの敵は無視
		if (enemy->IsDead()) continue;

		VECTOR distVec = VSub(list.pos_, enemy->GetPosition());
		// XZ平面のみ
		distVec.y = 0.0f;

		if (VSize(distVec) < COLLISION_RADIUS)
		{
			// プレイヤーを押し戻す
			VECTOR pushDir = VNorm(distVec);
			VECTOR pushBack = VScale(pushDir, COLLISION_RADIUS - VSize(distVec) + 0.1f);
			list.pos_ = VAdd(list.pos_, pushBack);
			MV1SetPosition(list.modelid_, list.pos_);
			list.moveSpeed_ = MOVE_SPEED_STOP;

			// 待機アニメーションに変更
			ChangeAnimation(0);

			if (damageCooldown_ <= 0)
			{
				// HPの減少
				list.hp_ -= 10;
				//クールタイム(60フレームに一回)
				damageCooldown_ = 60;

				// HPが0以下かつ死亡していなければ死亡処理
				if (list.hp_ <= 0 && !list.isdead_)
				{
					list.isdead_ = true;
					// 死亡アニメーション
					ChangeAnimation(19, true);
					SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAMEOVER);
				}
			}
		}
	}

	if (damageCooldown_ > 0) damageCooldown_--;
}

bool Player::CrouchUpdate(void)
{
	InputManager& ins = InputManager::GetInstance();

	//防御(しゃがみ)時には移動させない
	if (ins.IsNew(KEY_INPUT_LCONTROL))
	{
		if (!list.crouchflg_)
		{
			//しゃがみ状態に入るとき
			list.crouchflg_ = true;
			list.crouchattackflg_ = false;
			list.moveSpeed_ = MOVE_SPEED_STOP;		//動きを止める
			ChangeAnimation(Crouch, true);			//しゃがみ再生
			return true;
		}

		if (!list.animlockflg_ && !list.crouchattackflg_)
		{
			ChangeAnimation(Crouch_Idle);			//しゃがみ待機
		}

		//しゃがんでいるときに左クリック押したら
		if (!list.crouchattackflg_ && ins.IsTrgMouseLeft())
		{
			ChangeAnimation(Crouch_Attack);			//しゃがみ攻撃
			list.crouchattackflg_ = true;
		}
		return true;	//しゃがみ中
	}
	else if (list.crouchflg_)
	{
		//左コントロールを離したときにしゃがみ解除
		ChangeAnimation(Crouch_Cancel, true);		//しゃがみ解除
		list.crouchflg_ = false;
		list.crouchattackflg_ = false;
	}
	return false;		//しゃがみ中ではない
}

bool Player::AttackUpdate(void)
{
	InputManager& ins = InputManager::GetInstance();

	if (!list.attackflg_ && ins.IsTrgMouseLeft())
	{
		list.attackflg_ = true;
		list.combostep_ = 1;
		list.comboflg_ = false;
		ChangeAnimation(Attack_1, true);
		return true;
	}

	if(AttackCombo(1, Attack_3, 2, 0.5f, 5.0f))return true;
	if(AttackCombo(2, Attack_4, 3, 0.5f, 15.0f))return true;
	if(AttackCombo(3, Attack_2, 4, 0.5f, 15.0f))return true;

	//アニメーションが終わったら攻撃解除
	if (!list.animlockflg_ && list.attackflg_)
	{
		list.attackflg_ = false;
		list.combostep_ = 0;
		list.comboflg_ = false;
	}

	return list.attackflg_;		//攻撃中か
}

//攻撃コンボ(今のコンボ、次のアニメーション、次の段階、入力受付時間(後ろからの割合)、アニメーションスキップ時間(後ろから))
bool Player::AttackCombo(int nowcombo, int nextanimidx, int nextstep, float reception, float remainingtime)
{
	InputManager& ins = InputManager::GetInstance();

	//攻撃１中にアニメーションが半分以上進んでいたら攻撃２
	if (list.attackflg_ && list.combostep_ == nowcombo)
	{
		//現在のアニメーション時間(割合)
		list.currentratio_ = list.currentAnimTime_ / list.animTotalTime_;

		//アニメーションが半分以上＋左クリック押したとき
		if (list.currentratio_ >= reception && ins.IsTrgMouseLeft())
		{
			list.comboflg_ = true;
		}

		//アニメーションの残り時間
		list.remainingtime_ = list.animTotalTime_ - list.currentAnimTime_;
		if (list.remainingtime_ <= remainingtime && list.comboflg_)
		{
			ChangeAnimation(nextanimidx, true);
			list.combostep_ = nextstep;
			list.comboflg_ = false;
			return true;
		}
	}
	return false;
}

void Player::PlayerMove(int idle, int walk, int run)
{
	InputManager& ins = InputManager::GetInstance();

	if (list.weaponflg_)
	{
		if (CrouchUpdate()) { return; }
		if (AttackUpdate()) { return; }
	}

	// WASDでプレイヤー移動
	list.moveVec_ = { 0.0f, 0.0f, 0.0f };

	// 左・右・手前・奥のベクトルを作成する
	VECTOR RIGHT_MOVE_VEC = { 1.0f,  0.0f,  0.0f };
	VECTOR LEFT_MOVE_VEC = { -1.0f,  0.0f,  0.0f };
	VECTOR FRONT_MOVE_VEC = { 0.0f,  0.0f,  1.0f };
	VECTOR BACK_MOVE_VEC = { 0.0f,  0.0f, -1.0f };

	// 入力方向に移動ベクトルを追加する
	if (ins.IsNew(KEY_INPUT_W)) { list.moveVec_ = VAdd(list.moveVec_, FRONT_MOVE_VEC); }
	if (ins.IsNew(KEY_INPUT_A)) { list.moveVec_ = VAdd(list.moveVec_, LEFT_MOVE_VEC); }
	if (ins.IsNew(KEY_INPUT_S)) { list.moveVec_ = VAdd(list.moveVec_, BACK_MOVE_VEC); }
	if (ins.IsNew(KEY_INPUT_D)) { list.moveVec_ = VAdd(list.moveVec_, RIGHT_MOVE_VEC); }

	// ベクトルの移動が行われていたら座標更新
	if (IsMove(list.moveVec_))
	{
		// 移動状態の設定
		list.moveKind_ = 1;

		// カメラ角度分設定する
		VECTOR cameraAngles = SceneManager::GetInstance().GetCamera()->GetCameraAngles();
		MATRIX cameraMatY = MGetRotY(cameraAngles.y);
		list.moveVec_ = VTransform(list.moveVec_, cameraMatY);

		// スタミナが切れているかどうか
		bool spFlg_ = false;
		if (spFlg_)
		{
			// スタミナない状態
			list.moveSpeed_ = MOVE_SPEED_STOP;
		}
		else
		{
			if (ins.IsNew(KEY_INPUT_LSHIFT))
			{
				// ダッシュ状態
				list.moveSpeed_ = MOVE_SPEED_RUN;
				ChangeAnimation(run);
			}
			else
			{
				// 歩き状態
				list.moveSpeed_ = MOVE_SPEED_WALK;
				ChangeAnimation(walk);
			}
		}

		// 座標更新
		list.moveVec_ = VNorm(list.moveVec_);
		list.moveVec_ = VScale(list.moveVec_, list.moveSpeed_);
		list.pos_ = VAdd(list.pos_, list.moveVec_);
		list.oldMoveVec_ = list.moveVec_;

		// 方向を角度に変換する( XZ平面 Y軸)
		list.moveVecRad_.y = atan2f(list.moveVec_.x, list.moveVec_.z);

		// シンプルに計算角度を設定してみる
		list.rot_.y = list.moveVecRad_.y;

		// 座標設定
		MV1SetPosition(list.modelid_, list.pos_);
	}
	else
	{
		// 移動状態の設定
		list.moveKind_ = 0;

		//アニメーションロックされていないとき待機
		if (!list.animlockflg_)
		{
			ChangeAnimation(idle);
		}
	}

	if (list.pos_.y != 0.0f)
	{
		list.pos_.y = 0.0f;
	}
}

void Player::PlayAnimation(void)
{
	DebugAnimation();			//数字キーでアニメーション切り替え

	switch (list.animindex_)
	{
		//ループさせる
	case 0:			//武器なし待機
	case 1:			//武器なし歩く
	case 2:			//武器なし走る
	case 5:			//武器あり待機
	case 6:			//武器あり歩く
	case 7:			//武器あり走る
	case 8:			//武器あり横歩き
	case 12:		//しゃがみ待機	
		list.currentAnimTime_ += ANIM_SPEED;
		if (list.currentAnimTime_ > list.animTotalTime_)
		{
			list.currentAnimTime_ = 0.0f;
		}
		list.animlockflg_ = false;
		break;
		//ループさせない
	case 3:			//武器取り出し
	case 4:			//武器戻す
		list.currentAnimTime_ += ANIM_SPEED;
		if (list.currentAnimTime_ >= list.animTotalTime_)
		{
			list.currentAnimTime_ = list.animTotalTime_;
			list.animlockflg_ = false;
		}
		break;
	case 9:			//ジャンプ
	case 13:		//防御したとき
	case 15:		//攻撃１
	case 16:		//攻撃２
	case 17:		//攻撃３
	case 18:		//攻撃４
		list.currentAnimTime_ += ANIM_SPEED;
		if (list.currentAnimTime_ >= list.animTotalTime_)
		{
			list.currentAnimTime_ = list.animTotalTime_;
			list.animlockflg_ = false;
		}
		break;
	case 19:		//死亡
		list.currentAnimTime_ += DEAD_ANIM_SPEED;
		if (list.currentAnimTime_ >= list.animTotalTime_)
		{
			list.currentAnimTime_ = list.animTotalTime_;
			list.animlockflg_ = false;
		}
		break;
	case 10:		//しゃがみ(防御)
		list.currentAnimTime_ += CROUCH_ANIM_SPEED;
		if (list.currentAnimTime_ >= list.animTotalTime_)
		{
			list.currentAnimTime_ = list.animTotalTime_;
			list.animlockflg_ = false;

			ChangeAnimation(12);		//しゃがみ待機
		}
		break;
	case 11:		//しゃがみ解除
		list.currentAnimTime_ += CROUCH_ANIM_SPEED;
		if (list.currentAnimTime_ >= list.animTotalTime_)
		{
			list.currentAnimTime_ = list.animTotalTime_;
			list.animlockflg_ = false;
		}
		break;
	case 14:		//しゃがみ攻撃
		list.currentAnimTime_ += CROUCH_ATTACK_SPEED;
		if (list.currentAnimTime_ >= list.animTotalTime_)
		{
			list.currentAnimTime_ = list.animTotalTime_;
			list.animlockflg_ = false;
			list.crouchattackflg_ = false;	//しゃがみ攻撃終了

			ChangeAnimation(Crouch_Idle);			//しゃがみ待機
		}
		break;
	default:
		break;
	}
	MV1SetAttachAnimTime(list.modelid_, list.animAttachNo_, list.currentAnimTime_);
	MV1SetPosition(list.modelid_, list.pos_);
}

//矢印キーでアニメーション切り替え
void Player::DebugAnimation(void)
{
	InputManager& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_0)) {
		ChangeAnimation(19);
	}
	if (ins.IsTrgDown(KEY_INPUT_1)) {
		ChangeAnimation(1);
	}
	if (ins.IsTrgDown(KEY_INPUT_2)) {
		ChangeAnimation(2);
	}
	if (ins.IsTrgDown(KEY_INPUT_3)) {
		ChangeAnimation(3);
	}
	if (ins.IsTrgDown(KEY_INPUT_4)) {
		ChangeAnimation(4);
	}
}

//アニメーション切り替え
void Player::ChangeAnimation(int idx, bool lock)
{
	if (list.animindex_ != idx) {
		MV1DetachAnim(list.modelid_, list.animAttachNo_);
		list.animindex_ = idx;
		list.animAttachNo_ = MV1AttachAnim(list.modelid_, list.animindex_);
		list.animTotalTime_ = MV1GetAttachAnimTotalTime(list.modelid_, list.animAttachNo_);
		list.currentAnimTime_ = 0.0f;
		MV1SetAttachAnimTime(list.modelid_, list.animAttachNo_, list.currentAnimTime_);
		//ロック指定されたらフラグON
		list.animlockflg_ = lock;
	}
}