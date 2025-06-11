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
}

Player& Player::GetInstance(void)
{
	return *instance_;
}

Player::Player(void)
{
	list.modelId_ = -1;

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

	list.animIndex_ = 0;
	list.animAttachNo_ = 0;
	list.animTotalTime_ = 0.0f;
	list.currentAnimTime_ = 0.0f;
	list.isAnimLock_ = false;

	list.isDead_ = false;
	list.isImpact_ = false;
	list.isCrouch_ = false;
	list.isWeapon_ = false;
	list.hitStopTime_ = 0.0f;
	list.isHitStop_ = false;

	damageCooldown_ = 0;
	IsDamagedThisFrame_ = false;
	criticalDisplayTime_ = 0.0f;
	invincibleTime_ = 0.0f;
	showCriticalText_ = false;

	enemymng_ = nullptr;
}

Player::~Player(void)
{
}

void Player::Init(EnemyManager* mng)
{
	enemymng_ = mng;

	// モデルの読み込み
	list.modelId_ = MV1LoadModel("Data/Model/PlayerModel.mv1");

	// プレイヤーの初期位置設定
	list.pos_ = { 0.0f, 0.0f, -550.0f };
	MV1SetPosition(list.modelId_, list.pos_);

	// プレイヤーの角度設定
	list.rot_ = { 0.0f, 0.0f, 0.0f };
	MV1SetRotationXYZ(list.modelId_, list.rot_);

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

	// 攻撃力の初期化
	list.attackPower_ = AttackPower_;

	//死んでいるか
	list.isDead_ = false;

	//しゃがんでいるか
	list.isCrouch_ = false;

	//武器出しているか
	list.isWeapon_ = false;

	//攻撃しているか
	list.isAttack_ = false;

	//攻撃段階
	list.isCombo_ = false;
	list.comboStep_ = 0;

	// HPの初期化
	list.hp_ = PLAYER_MAX_HP;

	list.isGameOver_ = false;
	list.isImpact_ = false;
	list.isInvincible_ = false;

	// 無敵時間の残り時間(秒)
	invincibleTime_ = 0.0f;

	// クリティカル表示用
	showCriticalText_ = false;
	criticalDisplayTime_ = 0.0f;

	// ダメージ間隔制御用
	damageCooldown_ = 0;

	//アニメーション関連
	list.animIndex_ = 1;
	list.animAttachNo_ = MV1AttachAnim(list.modelId_, list.animIndex_);
	list.animTotalTime_ = MV1GetAttachAnimTotalTime(list.modelId_, list.animAttachNo_);
	list.currentAnimTime_ = 0.0f;
	list.isAnimLock_ = false;
	MV1SetAttachAnimTime(list.modelId_, list.animAttachNo_, list.currentAnimTime_);

	list.currentRatio_ = 0.0f;
	list.remainingTime_ = 0.0f;

	MV1SetupCollInfo(list.modelId_);
}

void Player::Update(float deltaTime)
{
	IsDamagedThisFrame_ = false;

	// 無敵時間のカウント
	if (list.isInvincible_) {
		if (invincibleTime_ > 0.0f)
		{
			invincibleTime_--;
			if (invincibleTime_ <= 0.0f)
			{
				invincibleTime_ = 0.0f;
				list.isInvincible_ = false;
			}
		}
	}

	// クリティカル表示カウント
	if (showCriticalText_)
	{
		criticalDisplayTime_ -= deltaTime;
		if (criticalDisplayTime_ <= 0.0f)
		{
			showCriticalText_ = false;
		}
	}

	//移動処理
	UpdateMove();
	SetRotation();
	MV1RefreshCollInfo(list.modelId_);
	// アニメーション処理
	PlayAnimation();
}

void Player::UpdateMove(void)
{
	// 入力制御取得
	InputManager& ins = InputManager::GetInstance();

	if (list.pos_.y != 0.0f)
	{
		list.pos_.y = 0.0f;
	}

	//死亡時に移動させない
	if (list.isDead_)
	{
		list.moveSpeed_ = MOVE_SPEED_STOP;
		ChangeAnimation(Death);
		return;
	}

	// 被ダメアニメーション中
	if (list.isImpact_)
	{
		list.moveSpeed_ = MOVE_SPEED_STOP;
		return;
	}

	if (ins.IsTrgMouseRight())
	{
		list.isWeapon_ = !list.isWeapon_;
		if (list.isWeapon_)
		{
			ChangeAnimation(WeaponOut, true);					//武器取り出し
		}
		else {
			ChangeAnimation(Sheach, true);						//武器しまう
		}
		return;
	}

	if (!list.isWeapon_)
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
	MV1DrawModel(list.modelId_);

	// プレイヤーHPバー
	const int HP_BAR_WIDTH = 300;
	const int HP_BAR_HEIGHT = 20;
	float hpRatio = (float)list.hp_ / PLAYER_MAX_HP;

	int color = GetColor(0, 255, 0); // 緑
	if (list.hp_ <= 20)
	{
		color = GetColor(255, 0, 0); //赤
	}
	else if (list.hp_ < PLAYER_MAX_HP / 2)
	{
		color = GetColor(255, 255, 0); // 黄
	}

	int x = 20;
	int y = 20;
	DrawBox(x, y, x + HP_BAR_WIDTH, y + HP_BAR_HEIGHT, GetColor(50, 50, 50), TRUE); // 背景（グレー）
	DrawBox(x, y, x + (int)(HP_BAR_WIDTH * hpRatio), y + HP_BAR_HEIGHT, color, TRUE); // HPバー本体
	DrawBox(x, y, x + HP_BAR_WIDTH, y + HP_BAR_HEIGHT, GetColor(255, 255, 255), FALSE); // 枠線


	// === デバッグ表示：プレイヤーのHPと攻撃力 ===
	DrawFormatString(20, 500, GetColor(255, 255, 255), "プレイヤーHP：%d / %d", list.hp_, PLAYER_MAX_HP);
	DrawFormatString(20, 530, GetColor(255, 255, 255), "プレイヤー攻撃力：%d", list.attackPower_);

	// プレイヤー座標表示
	if (list.isInvincible_) {
		DrawFormatString(20, 120, GetColor(0xff, 0xff, 0xff), "無敵時間：%f", invincibleTime_);
	}
	DrawFormatString(20, 150, GetColor(0xff, 0xff, 0xff), "プレイヤーHP：%d", list.hp_);

	// クリティカル表示
	if (showCriticalText_)
	{
		DrawFormatString(20, 20, GetColor(255, 0, 0), "クリティカル！");
	}
}

// 解放
void Player::Release(void)
{
	MV1DeleteModel(list.modelId_);
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

	list.rotationMatrix_ = mat;

	// 行列を使用してモデルの角度を設定
	MV1SetRotationMatrix(list.modelId_, mat);
}

bool Player::CrouchUpdate(void)
{
	InputManager& ins = InputManager::GetInstance();

	//防御(しゃがみ)時には移動させない
	if (ins.IsNew(KEY_INPUT_LCONTROL))
	{
		if (!list.isCrouch_)
		{
			//しゃがみ状態に入るとき
			list.isCrouch_ = true;
			list.isAttack_ = false;
			list.moveSpeed_ = MOVE_SPEED_STOP;		//動きを止める
			ChangeAnimation(Crouch, true);			//しゃがみ再生
			return true;
		}

		if (!list.isAnimLock_)
		{
			ChangeAnimation(Crouch_Idle);			//しゃがみ待機
		}

		return true;	//しゃがみ中
	}
	else if (list.isCrouch_)
	{
		//左コントロールを離したときにしゃがみ解除
		ChangeAnimation(Crouch_Cancel, true);		//しゃがみ解除
		list.isCrouch_ = false;
	}
	return false;		//しゃがみ中ではない
}

bool Player::AttackUpdate(void)
{
	InputManager& ins = InputManager::GetInstance();

	if (!list.isAttack_ && ins.IsTrgMouseLeft())
	{
		list.isAttack_ = true;
		list.comboStep_ = 1;
		list.isCombo_ = false;
		ChangeAnimation(First_Attack, true);
		return true;
	}

	if(AttackCombo(1, Third_Attack, 2, 0.5f, 5.0f))return true;
	if(AttackCombo(2, Force_Attack, 3, 0.5f, 15.0f))return true;
	if(AttackCombo(3, Second_Attack, 4, 0.5f, 15.0f))return true;

	return list.isAttack_;		//攻撃中か
}

//攻撃コンボ(今のコンボ、次のアニメーション、次の段階、入力受付時間(後ろからの割合)、アニメーションスキップ時間(後ろから))
bool Player::AttackCombo(int nowcombo, int nextanimidx, int nextstep, float reception, float remainingtime)
{
	InputManager& ins = InputManager::GetInstance();

	//攻撃１中にアニメーションが半分以上進んでいたら攻撃２
	if (list.isAttack_ && list.comboStep_ == nowcombo)
	{
		//現在のアニメーション時間(割合)
		list.currentRatio_ = list.currentAnimTime_ / list.animTotalTime_;

		//アニメーションが指定の割合以上＋左クリック押したとき
		if (list.currentRatio_ >= reception && ins.IsTrgMouseLeft())
		{
			list.isCombo_ = true;
		}

		// 残り時間が一定以下＋コンボ可能なら次のアニメーションへ
		list.remainingTime_ = list.animTotalTime_ - list.currentAnimTime_;
		if (list.remainingTime_ <= remainingtime && list.isCombo_)
		{
			list.comboStep_ = nextstep;
			list.isCombo_ = false;
			ChangeAnimation(nextanimidx, true);
			return true;
		}
	}
	return false;
}

void Player::TakeDamage(int damage)
{
	// このフレームで既にダメージを受けていれば無効（即時の多段防止）
	if (invincibleTime_ > 0.0f) return;

	list.hp_ -= damage;
	// HPが0以下なら死亡状態へ移行
	if (list.hp_ <= 0)
	{
		list.hp_ = 0;
		list.isDead_ = true;
	}

	list.isInvincible_ = true;
	invincibleTime_ = 5.0f * 60.0f;			// 無敵時間を5秒に設定
	ChangeAnimation(Impact, true);  // ダメージアニメーション再生
}

void Player::PlayerMove(int idle, int walk, int run)
{
	InputManager& ins = InputManager::GetInstance();

	if (list.isWeapon_)
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
		MV1SetPosition(list.modelId_, list.pos_);
	}
	else
	{
		// 移動状態の設定
		list.moveKind_ = 0;

		//アニメーションロックされていないとき待機
		if (!list.isAnimLock_)
		{
			ChangeAnimation(idle);
		}
	}
}

void Player::PlayAnimation(void)
{
	DebugAnimation();
	switch (list.animIndex_)
	{
		//ループさせる
	case NoWeaponIdle:			//武器なし待機
	case NoWeaponWalk:			//武器なし歩く
	case NoWeaponRun:			//武器なし走る
	case WeaponIdle:			//武器あり待機
	case WeaponWalk:			//武器あり歩く
	case WeaponRun:				//武器あり走る
	case Strafe:				//横歩き
	case Crouch_Idle:			//しゃがみ待機	
		list.currentAnimTime_ += ANIM_SPEED;
		if (list.currentAnimTime_ > list.animTotalTime_)
		{
			list.currentAnimTime_ = 0.0f;
		}
		list.isAnimLock_ = false;
		break;
		//ループさせない
	case WeaponOut:			//武器取り出し
	case Sheach:			//武器戻す
		list.currentAnimTime_ += ANIM_SPEED;
		if (list.currentAnimTime_ >= list.animTotalTime_)
		{
			list.currentAnimTime_ = list.animTotalTime_;
			list.isAnimLock_ = false;
		}
		break;
	case Jump:				//ジャンプ
		list.currentAnimTime_ += ATTACK_ANIM_SPEED;
		if (list.currentAnimTime_ >= list.animTotalTime_)
		{
			list.currentAnimTime_ = list.animTotalTime_;
			list.isAnimLock_ = false;
		}
		break;
	case First_Attack:		//攻撃１
	case Second_Attack:		//攻撃２
	case Third_Attack:		//攻撃３
	case Force_Attack:		//攻撃４
		list.currentAnimTime_ += ATTACK_ANIM_SPEED;
		if (list.currentAnimTime_ >= list.animTotalTime_)
		{
			list.currentAnimTime_ = list.animTotalTime_;
			list.isAnimLock_ = false;
			list.isAttack_ = false;
			list.comboStep_ = 0;
			list.isCombo_ = false;
		}
		break;
	case Impact:		//攻撃受けたとき
		list.currentAnimTime_ += ATTACK_ANIM_SPEED;
		if (list.currentAnimTime_ >= list.animTotalTime_)
		{
			list.currentAnimTime_ = list.animTotalTime_;
			list.isAnimLock_ = false;
			list.isImpact_ = false;
		}
		else
		{
			list.isImpact_ = true;
		}
		break;
	case Death:		//死亡
		list.currentAnimTime_ += DEAD_ANIM_SPEED;
		if (list.currentAnimTime_ >= list.animTotalTime_)
		{
			list.currentAnimTime_ = list.animTotalTime_;
			list.isAnimLock_ = false;
			if (!list.isGameOver_)
			{
				list.isGameOver_ = true;
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAMEOVER);
			}
		}
		break;
	case Crouch:		//しゃがみ(防御)
		list.currentAnimTime_ += CROUCH_ANIM_SPEED;
		if (list.currentAnimTime_ >= list.animTotalTime_)
		{
			list.currentAnimTime_ = list.animTotalTime_;
			list.isAnimLock_ = false;

			ChangeAnimation(12);		//しゃがみ待機
		}
		break;
	case Crouch_Cancel:		//しゃがみ解除
		list.currentAnimTime_ += CROUCH_ANIM_SPEED;
		if (list.currentAnimTime_ >= list.animTotalTime_)
		{
			list.currentAnimTime_ = list.animTotalTime_;
			list.isAnimLock_ = false;
		}
		break;
	case Crouch_Guard:		//防御したとき
		list.currentAnimTime_ += CROUCH_ANIM_SPEED;
		if (list.currentAnimTime_ >= list.animTotalTime_)
		{
			list.currentAnimTime_ = list.animTotalTime_;
			list.isAnimLock_ = false;
		}
		break;

	default:
		break;
	}
	MV1SetAttachAnimTime(list.modelId_, list.animAttachNo_, list.currentAnimTime_);
	MV1SetPosition(list.modelId_, list.pos_);
}

//矢印キーでアニメーション切り替え
void Player::DebugAnimation(void)
{
	InputManager& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_0)) {
		list.isImpact_ = true;
	}
	if (ins.IsTrgDown(KEY_INPUT_1)) {
		ChangeAnimation(Crouch_Guard);
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

bool Player::AttackActive(void)
{
	if (list.animIndex_ == First_Attack || list.animIndex_ == Second_Attack
		|| list.animIndex_ == Third_Attack || list.animIndex_ == Force_Attack)
	{
		return true;
	}
	return false;
}

//アニメーション切り替え
void Player::ChangeAnimation(int idx, bool lock)
{
	if (list.animIndex_ != idx) {
		MV1DetachAnim(list.modelId_, list.animAttachNo_);
		list.animIndex_ = idx;
		list.animAttachNo_ = MV1AttachAnim(list.modelId_, list.animIndex_);
		list.animTotalTime_ = MV1GetAttachAnimTotalTime(list.modelId_, list.animAttachNo_);
		list.currentAnimTime_ = 0.0f;
		MV1SetAttachAnimTime(list.modelId_, list.animAttachNo_, list.currentAnimTime_);
		//ロック指定されたらフラグON
		list.isAnimLock_ = lock;

		if (AttackActive())
		{
			// 攻撃に切り替わったタイミングで全敵のassignをリセット
			for (auto& enemy : enemymng_->GetEnemyPtrs())
			{
				enemy->SetIsAssign(false);
			}
		}
	}
}

void Player::HitStop(void)
{
	if (list.isHitStop_)
	{
		list.hitStopTime_ -= SceneManager::GetInstance().GetDeltaTime();

		if (list.hitStopTime_ <= 0.0f)
		{
			list.hitStopTime_ = 0.0f;
			list.isHitStop_ = false;
		}
		return;
	}
}

// 全回復
void Player::RecoverHp()
{
	list.hp_ = PLAYER_MAX_HP; // 最大値にする（もしMaxHPが増えているならその変数で）
}

// 攻撃力を上げる
void Player::AddAttack(int amount)
{
	list.attackPower_ += amount;

	// 攻撃力の最小値の制限
	if (list.attackPower_ <= 10)
	{
		list.attackPower_ = 10;
	}
}

// HP,攻撃力を上げる
void Player::AddMaxHp(int amount)
{
	PLAYER_MAX_HP += amount;
	list.hp_ += amount; // 今のHPも増やす
}

// クリティカル表示
void Player::SetCriticalDisplay(bool enable)
{
	showCriticalText_ = enable;
	if (enable) criticalDisplayTime_ = 3.0f;  // 表示を3秒間
}

// アニメーション中かどうか
bool Player::IsPlayingImpactAnimation() const
{
	return list.animIndex_ == Impact && list.currentAnimTime_ < list.animTotalTime_;
}
