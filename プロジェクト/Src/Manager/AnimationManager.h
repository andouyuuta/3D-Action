#pragma once
#include <DxLib.h>
#include <functional>
#include <unordered_map>

class Player;
class Enemy;
class EnemyManager;

class AnimationManager
{
public:
	AnimationManager(void);
	~AnimationManager(void);

	void Init(Player* player, EnemyManager* enemymng);
	void Update(void);
	void Release(void);
	enum PlayerAnim
	{
		NoWeaponIdle = 0,		// 武器なし待機
		NoWeaponWalk,			// 武器なし歩き
		NoWeaponRun,			// 武器なし走り
		WeaponOut,				// 武器取り出し	
		Sheach,					// 武器戻す
		WeaponIdle,				// 武器あり待機
		WeaponWalk,				// 武器あり歩き
		WeaponRun,				// 武器あり走り
		Strafe,					// 横歩き
		Jump,					// ジャンプ
		Crouch,					// しゃがみ
		Crouch_Cancel,			// しゃがみ解除
		Crouch_Idle,			// しゃがみ待機
		Crouch_Guard,			// しゃがみ防御
		Crouch_Attack,
		First_Attack,			// 攻撃１
		Force_Attack,			// 攻撃４
		Second_Attack,			// 攻撃２
		Third_Attack,			// 攻撃３
		Death,					// 死亡
		Impact,					// 攻撃受けたとき
		Revive					// 復活
	};

	enum EnemyAnim
	{
		ANIM_IDLE = 0,
		ANIM_WALK,
		ANIM_RUN,
		ANIM_ATTACK,
		ANIM_DEAD,
		ANIM_JUMP_ATTACK,
		ANIM_SWIP,
	};

	struct AnimationInfo
	{
		//アニメーション
		int animIndex_;			//アニメーションの種類
		int  animAttachNo_;		// 設定するアニメーションの番号
		float animTotalTime_;	// アニメーションの総再生時間
		float currentAnimTime_; // 直前のアニメーションの時間
		float currentRatio_;	//現在のアニメーションの再生時間(割合)
		float remainingTime_;	//アニメーションがあとどれくらいで終わるか
		bool isAnimLock_;		//アニメーションロック
	};

	std::unordered_map<Enemy*, AnimationInfo>enemyAnimInfos_;

	void PlayerAnimationPlayback(void);					// プレイヤーアニメーション再生
	void EnemyAnimationPlayback(void);
	void PlayerChangeAnimation(int idx, bool lock = false);	// アニメーション切り替え
	void EnemyChangeAnimation(Enemy* enemy,int idx, bool lock = false);
	bool PlayerAttackCombo(int nowcombo, int nextanimidx, int nextstep, float reception, float remainingtime);	// プレイヤー攻撃再生(今のコンボ数、今のコンボ、次のステップ、割合指定、アニメーションの残り時間(割合))
	bool IsImpactAnimation(void);
	AnimationInfo GetPlayerInfo(void) { return playerInfo; }
	AnimationInfo& GetEnemyInfo(void) { return enemyInfo; }

	void SetAnimLock(bool lock) { playerInfo.isAnimLock_ = lock; }
private:
	bool AttackActive(void);				// 攻撃アニメーションかどうか
	void AdvanceAnimation(float speed, std::function<void()> onEnd, std::function<void()> onUpdate, bool isLoop = false);
	AnimationInfo playerInfo;
	AnimationInfo enemyInfo;

	EnemyManager* enemymng_;
	Player* player_;
};

