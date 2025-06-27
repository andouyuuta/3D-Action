#include <DxLib.h>
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "../Manager/AnimationManager.h"
#include "../Application.h"
#include "Player.h"
#include "Sword.h"
#include "Enemy.h"
#include "EnemyManager.h"
#include "Stage.h"

// デストラクタ
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
	animation_ = nullptr;

	list.criticalRate_ = 10.0f;
	list.criticalDamage = 1.5f;

	criticalFontHandle_ = -1;
	isDamaged_=false;

	list.flashCounter_ = 0.0f;
}

// デストラクタ
Player::~Player(void)
{
}

// 初期化
void Player::Init(EnemyManager* mng, AnimationManager* anim,Camera* camera)
{
	enemymng_ = mng;
	animation_ = anim;
	camera_ = camera;

	// モデルの読み込み
	list.modelId_ = MV1LoadModel("Data/Model/PlayerModel.mv1");

	// プレイヤーの初期位置設定
	list.pos_ = { 0.0f, 0.0f, -550.0f };
	MV1SetPosition(list.modelId_, list.pos_);

	// プレイヤーの角度設定
	list.rot_ = { 0.0f, 0.0f, 0.0f };
	MV1SetRotationXYZ(list.modelId_, list.rot_);

	// プレイヤーの元の色
	list.orgColor_ = MV1GetDifColorScale(list.modelId_);

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
	list.hp_ = PlayerMaxHp_;

	list.isGameOver_ = false;
	list.isImpact_ = false;
	list.isInvincible_ = false;

	list.dashCount_ = 0.0f;

	// 無敵時間の残り時間(秒)
	invincibleTime_ = 0.0f;

	// クリティカル表示用
	showCriticalText_ = false;
	criticalDisplayTime_ = 0.0f;

	// ダメージ間隔制御用
	damageCooldown_ = 0;

	// フォントの読み込み
	criticalFontHandle_ = CreateFontToHandle("メイリオ", 64, 5, DX_FONTTYPE_ANTIALIASING_EDGE);

	MV1SetupCollInfo(list.modelId_);
}

void Player::Update(float deltaTime)
{
	IsDamagedThisFrame_ = false;	

	// 無敵時間のカウント
	if (list.isInvincible_) 
	{
		// 無敵時間が０以上だったら減少
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
		// クリティカル表示時間が０以上だったら減少
		if (criticalDisplayTime_ > 0.0f)
		{
			criticalDisplayTime_--;
			if (criticalDisplayTime_ <= 0.0f)
			{
				showCriticalText_ = false;
			}
		}
	}

	//移動処理
	UpdateMove();
	SetRotation();
	MV1RefreshCollInfo(list.modelId_);
}

// プレイヤー全体の動き
void Player::UpdateMove(void)
{
	// 入力制御取得
	InputManager& ins = InputManager::GetInstance();

	// 常に高さを０に設定
	if (list.pos_.y != 0.0f)
	{
		list.pos_.y = 0.0f;
	}

	//死亡時に移動させない
	if (list.isDead_)
	{
		list.moveSpeed_ = MOVE_SPEED_STOP;
		animation_->PlayerChangeAnimation(animation_->Death);								// 死亡アニメーション
		return;
	}

	// 被ダメアニメーション中移動させない
	if (list.isImpact_)
	{
		list.moveSpeed_ = MOVE_SPEED_STOP;
		return;
	}

	if (list.isWeapon_)
	{
		if (CrouchUpdate()) { return; }
		if (AttackUpdate()) { return; }
	}

	// 右クリック押したら武器切り替え
	if (ins.IsTrgMouseRight()||ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1,InputManager::JOYPAD_BTN::L_TRIGGER))
	{
		list.isWeapon_ = !list.isWeapon_;											// 今のフラグと反対のフラグを代入
		if (list.isWeapon_)
		{
			animation_->PlayerChangeAnimation(animation_->WeaponOut, true);				//武器取り出し
		}
		else {
			animation_->PlayerChangeAnimation(animation_->Sheach, true);					//武器しまう
		}
		return;
	}

	// 武器切り替えたときの動き
	if (!list.isWeapon_)
	{
		PlayerMove(animation_->NoWeaponIdle, animation_->NoWeaponWalk, animation_->NoWeaponRun);	//武器持っていないときの動き
	}
	else
	{
		PlayerMove(animation_->WeaponIdle, animation_->WeaponWalk, animation_->WeaponRun);			//武器持っているときの動き
	}
}

// 描画
void Player::Draw(void)
{
	// モデルの描画
	if (list.hp_ <= 50)
	{
		list.flashCounter_++;
		float alpha = (sinf(list.flashCounter_ * FLASH_SPEED) + 1.0f) / 2.0f;

		COLOR_F finalColor;
		finalColor.r = list.orgColor_.r + (1.0f - list.orgColor_.r) * alpha;
		finalColor.g = list.orgColor_.g * (1.0f - alpha);
		finalColor.b = list.orgColor_.b * (1.0f - alpha);
		finalColor.a = 1.0f;
		MV1SetDifColorScale(list.modelId_, finalColor);
	}
	else
	{
		list.flashCounter_ = 0.0f;
		MV1SetDifColorScale(list.modelId_, list.orgColor_);
	}

	if (list.isInvincible_)
	{
		COLOR_F translucentModel = list.orgColor_;
		translucentModel.a = TRANSLUCENT;
		MV1SetDifColorScale(list.modelId_, translucentModel);
	}
	MV1DrawModel(list.modelId_);

	// === デバッグ表示：プレイヤーのHPと攻撃力 ===
	DrawFormatString(20, 500, GetColor(255, 255, 255), "プレイヤーHP：%d / %d", list.hp_, PlayerMaxHp_);
	DrawFormatString(20, 530, GetColor(255, 255, 255), "プレイヤー攻撃力：%d", list.attackPower_);

	// プレイヤー座標表示
	if (list.isInvincible_) {
		DrawFormatString(20, 300, GetColor(0xff, 0xff, 0xff), "無敵時間：%f", invincibleTime_);
	}
	DrawFormatString(20, 150, GetColor(0xff, 0xff, 0xff), "プレイヤーHP：%d", list.hp_);

	// ゲームパッドのデバッグ情報表示（左スティック値）
	InputManager& ins = InputManager::GetInstance();
	InputManager::JOYPAD_IN_STATE pad = ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

	// 左スティックの入力値（-1000～1000）
	float lx = pad.AKeyLX / 1000.0f;
	float ly = pad.AKeyLY / 1000.0f;
	DrawFormatString(20, 400, 0xffffff, "dashCount：%f", list.dashCount_);
	DrawFormatString(20, 580, GetColor(0, 255, 255), "Pad Left Stick X: %.2f", lx);
	DrawFormatString(20, 600, GetColor(0, 255, 255), "Pad Left Stick Y: %.2f", ly);

	// ボタン入力（Xボタン＝DOWN）も確認
	bool isADown = ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN);
	DrawFormatString(20, 620, GetColor(255, 255, 0), "Pad A Button (DOWN): %s", isADown ? "Pressed" : "Not Pressed");

	// クリティカル表示
	if (showCriticalText_)
	{

		// 表示時間に応じて透明度を変える
		// 時間がたつほど透明になってフェードアウトさせる
		int alpha = (int)(255.0f * (criticalDisplayTime_ / 180.0f)); // 最大3秒の残り時間で調整

		// スケールを拡大縮小する(最大1.5倍)
		float scale = 1.0f + 0.5f * (1.0f - (criticalDisplayTime_ / 180.0f));

		// 中央に表示するための座標
		int screenW = Application::SCREEN_SIZE_X / 2;
		int screenH = Application::SCREEN_SIZE_Y / 3;

		// aブレンドモードを設定(透明度を有効化)
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

		// 中央に赤く大きな文字で「CRITICAL!!」と表示(フォントハンドルを使う)
		DrawRotaStringToHandle(
			screenW, screenH,         // 中心X, Y
			scale, scale,             // 拡大率X, Y
			0.0, 0.0,                 // 回転中心X, Y
			0.0,                      // 回転角（回転不要）
			GetColor(255, 0, 0),      // 文字色（赤）
			criticalFontHandle_,      // フォントハンドル
			GetColor(0, 0, 0),        // 縁色（黒）
			FALSE,                    // 縦書きフラグ：FALSE = 横書き
			TEXT("CRITICAL!!")        // 文字列（TCHAR対応）
		);
		// ブレンドモード解除
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}

// 解放
void Player::Release(void)
{
	MV1DeleteModel(list.modelId_);
	DeleteFontToHandle(criticalFontHandle_);	// フォントの解放
}

// 動いているか
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

// プレイヤーの回転処理
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

// しゃがみ更新
bool Player::CrouchUpdate(void)
{
	InputManager& ins = InputManager::GetInstance();

	//左コントロールでしゃがみ
	if (ins.IsNew(KEY_INPUT_LCONTROL)||ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1,InputManager::JOYPAD_BTN::R_BUTTON))
	{
		//しゃがみ状態に入るとき
		if (!list.isCrouch_)
		{
			list.isCrouch_ = true;				
			list.isAttack_ = false;
			list.moveSpeed_ = MOVE_SPEED_STOP;		// 動きを止める
			animation_->PlayerChangeAnimation(animation_->Crouch, true);			// しゃがみ再生
			return true;
		}

		// キー押し続けていたら待機
		if (!animation_->GetPlayerInfo().isAnimLock_)
		{
			animation_->PlayerChangeAnimation(animation_->Crouch_Idle);			// しゃがみ待機
		}

		return true;	//しゃがみ中
	}
	// しゃがんでいる場合にキーを話したらしゃがみ解除
	else if (list.isCrouch_)
	{
		animation_->PlayerChangeAnimation(animation_->Crouch_Cancel, true);		// しゃがみ解除
		list.isCrouch_ = false;
	}
	return false;		// しゃがみ中ではない
}

// 攻撃更新処理
bool Player::AttackUpdate(void)
{
	InputManager& ins = InputManager::GetInstance();

	// 攻撃していない状態で、左クリックを押したとき or 右トリガー押したとき
	if (!list.isAttack_ && (ins.IsTrgMouseLeft()||		
		ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1,InputManager::JOYPAD_BTN::LEFT)))
	{
		list.isAttack_ = true;					// 攻撃フラグをtrueに
		list.comboStep_ = 1;					// コンボ段階を１に
		list.isCombo_ = false;					// コンボ中をtrueに
		animation_->PlayerChangeAnimation(animation_->First_Attack, true);
		return true;
	}

	if(animation_->PlayerAttackCombo(1, animation_->Second_Attack, 2, 0.5f, 5.0f))return true;		// １段階目で半分以上(0.5f)のときに攻撃ボタン押したらアニメーションスキップ(後ろから5.0f)して次のアニメーションへ
	if(animation_->PlayerAttackCombo(2, animation_->Third_Attack, 3, 0.5f, 15.0f))return true;		// ２段階目で半分以上(0.5f)のときに攻撃ボタン押したらアニメーションスキップ(後ろから15.0f)して次のアニメーションへ

	return list.isAttack_;		//攻撃中か
}

// プレイヤーがダメージを受けたとき
void Player::TakeDamage(int damage)
{
	// このフレームで既にダメージを受けていれば無効（即時の多段防止）
	if (invincibleTime_ > 0.0f) return;

	// HPを減らす
	list.hp_ -= damage;
	// HPが0以下なら死亡状態へ移行
	if (list.hp_ <= 0)
	{
		list.hp_ = 0;
		list.isDead_ = true;
	}
	else
	{
		list.isInvincible_ = true;
		if (animation_->GetPlayerInfo().animIndex_ == animation_->Crouch_Guard)
		{
			SetInvincibleTime(3.0f);
			animation_->PlayerChangeAnimation(animation_->Crouch_Guard, true);
		}
		else
		{
			SetInvincibleTime(5.0f);								// 無敵時間を5秒に設定
			animation_->PlayerChangeAnimation(animation_->Impact, true);  // ダメージアニメーション再生
		}

	}
}

// プレイヤーの移動処理
void Player::PlayerMove(int idle, int walk, int run)
{
	InputManager& ins = InputManager::GetInstance();

	// WASDでプレイヤー移動
	list.moveVec_ = { 0.0f, 0.0f, 0.0f };

	// 左・右・手前・奥のベクトルを作成する
	VECTOR RIGHT_MOVE_VEC = { 1.0f,  0.0f,  0.0f };
	VECTOR LEFT_MOVE_VEC = { -1.0f,  0.0f,  0.0f };
	VECTOR FRONT_MOVE_VEC = { 0.0f,  0.0f,  1.0f };
	VECTOR BACK_MOVE_VEC = { 0.0f,  0.0f, -1.0f };

	InputManager::JOYPAD_IN_STATE pad = ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

	// 左スティックの入力値（-1000～1000）
	float lx = pad.AKeyLX / 1000.0f;
	float ly = pad.AKeyLY / 1000.0f;

	// 入力方向に移動ベクトルを追加する
	if (ins.IsNew(KEY_INPUT_W) || ly < 0) { list.moveVec_ = VAdd(list.moveVec_, FRONT_MOVE_VEC); }
	if (ins.IsNew(KEY_INPUT_A) || lx < 0) { list.moveVec_ = VAdd(list.moveVec_, LEFT_MOVE_VEC); }
	if (ins.IsNew(KEY_INPUT_S) || ly > 0) { list.moveVec_ = VAdd(list.moveVec_, BACK_MOVE_VEC); }
	if (ins.IsNew(KEY_INPUT_D) || lx > 0) { list.moveVec_ = VAdd(list.moveVec_, RIGHT_MOVE_VEC); }

	// ベクトルの移動が行われていたら座標更新
	if (IsMove(list.moveVec_))
	{
		// 移動状態の設定
		list.moveKind_ = 1;

		list.dashCount_++;

		// カメラ角度分設定する
		VECTOR cameraAngles = camera_->GetCameraAngles();
		MATRIX cameraMatY = MGetRotY(cameraAngles.y);
		list.moveVec_ = VTransform(list.moveVec_, cameraMatY);

		if (list.dashCount_ >= 100.0f)
		{
			// ダッシュ状態
			list.moveSpeed_ = MOVE_SPEED_RUN;
			animation_->PlayerChangeAnimation(run);
			list.dashCount_ = 100.0f;
		}
		else
		{
			// 歩き状態
			list.moveSpeed_ = MOVE_SPEED_WALK;
			animation_->PlayerChangeAnimation(walk);
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

		list.dashCount_ = 0.0f;

		//アニメーションロックされていないとき待機
		if (!animation_->GetPlayerInfo().isAnimLock_)
		{
			animation_->PlayerChangeAnimation(idle);
		}
	}
}

// 攻撃が当たっていたらヒットストップ
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
void Player::FullRecoveryHp()
{
	list.hp_ = PlayerMaxHp_; // 最大値にする（もしMaxHPが増えているならその変数で）
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

// HPを上げる
void Player::AddMaxHp(int amount)
{
	PlayerMaxHp_ += amount;
	list.hp_ += amount; // 今のHPも増やす
}

// クリティカル表示
void Player::SetCriticalDisplay(bool enable)
{
	showCriticalText_ = enable;
	if (enable)
	{
		criticalDisplayTime_ = 180.0f;  // 表示を3秒間
	}
}
