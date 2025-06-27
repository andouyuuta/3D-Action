#pragma once
#include <DxLib.h>
#include <vector>

class Sword;
class EnemyManager;
class AnimationManager;
class InputManager;
class Camera;

class Player
{
public:
	// �萔
	static constexpr float MOVE_SPEED_STOP = 0.0f;
	static constexpr float MOVE_SPEED_WALK = 2.5f;
	static constexpr float MOVE_SPEED_RUN = 8.0f;
	static constexpr float FLASH_SPEED = 0.1f;
	static constexpr float TRANSLUCENT = 0.5f;
	static constexpr VECTOR INIT_MODEL_ROT_OFFSET = { 0.0f, DX_PI_F / 2.0f, 0.0f };
	static constexpr VECTOR INIT_DIR = { 0.0f, 0.0f, -1.0f };
	static constexpr float VIEW_ANGLE = 180.0f;

	// �X�e�[�^�X
	int PlayerMaxHp_ = 200;
	int AttackPower_ = 150;

	// �v���C���[���\����
	struct Info
	{
		int modelId_;
		VECTOR pos_;
		VECTOR rot_;
		VECTOR scale_;
		COLOR_F orgColor_;
		float moveSpeed_;
		int moveDir_;
		int moveKind_;
		int hp_;
		int attackPower_;

		VECTOR moveVec_;
		VECTOR oldMoveVec_;
		VECTOR moveVecRad_;

		VECTOR localRot_;
		MATRIX rotationMatrix_;

		bool isDead_;
		bool isCrouch_;
		bool isWeapon_;
		bool isAttack_;
		bool isCombo_;
		bool isGameOver_;
		bool isImpact_;
		bool isInvincible_;
		bool hitPlayer_;

		int comboStep_;
		float dashCount_;

		float criticalRate_;
		float criticalDamage;

		float hitStopTime_;
		bool isHitStop_;

		float flashCounter_;
	};

public:
	Player(void);
	~Player(void);

	// �������E�X�V�E�`��
	void Init(EnemyManager* mng, AnimationManager* anim, Camera* camera);
	void Update(float deltaTime);
	void Draw(void);
	void Release(void);

	// ��ԍX�V
	void HitStop(void);
	void FullRecoveryHp(void);
	void AddAttack(int amount);
	void AddMaxHp(int amount);
	void SetCriticalDisplay(bool enable);

	// �Q�b�^�[
	[[nodiscard]] Info GetInfo(void) const { return list; }
	[[nodiscard]] int GetPlayerModel(void) const { return list.modelId_; }
	[[nodiscard]] VECTOR GetPlayerPos(void) const { return list.pos_; }
	[[nodiscard]] VECTOR GetPlayerRot(void) const { return list.rot_; }
	[[nodiscard]] float GetRotX(void) const { return list.rot_.x; }
	[[nodiscard]] float GetRotY(void) const { return list.rot_.y; }
	[[nodiscard]] VECTOR GetRightHandPosition(void) const { return MV1GetFramePosition(list.modelId_, GetRightHandIndex()); }
	[[nodiscard]] int GetRightHandIndex(void) const { return MV1SearchFrame(list.modelId_, "mixamorig:RightHand"); }
	[[nodiscard]] int GetDamageCooldown(void) const { return damageCooldown_; }
	[[nodiscard]] int GetHp(void) const { return list.hp_; }
	[[nodiscard]] int GetAttackPower(void) const { return list.attackPower_; }
	[[nodiscard]] int GetMaxHp(void) const { return PlayerMaxHp_; }
	[[nodiscard]] int GetComboStep(void) const { return list.comboStep_; }
	[[nodiscard]] int GetCriticalRate(void) const { return list.criticalRate_; }
	[[nodiscard]] float GetCriticalDamage(void) const { return list.criticalDamage; }
	[[nodiscard]] MATRIX GetRotationMat(void) const { return list.rotationMatrix_; }
	[[nodiscard]] bool GetIsWeapon(void) const { return list.isWeapon_; }
	[[nodiscard]] bool GetIsDead(void) const { return list.isDead_; }
	[[nodiscard]] bool GetIsAttack(void) const { return list.isAttack_; }
	[[nodiscard]] bool GetIsInvincible(void) const { return list.isInvincible_; }
	[[nodiscard]] bool GetIsHitStop(void) const { return list.isHitStop_; }

	// �Z�b�^�[
	void SetPlayerPos(VECTOR pos) { list.pos_ = pos; }
	void SetPlayerSpeed(const VECTOR& speed) { list.moveVec_ = speed; }
	void SetMoveSpeed(float speed) { list.moveSpeed_ = speed; }
	void SetDamageCooldown(int damage) { damageCooldown_ = damage; }
	void DecreaseCoolDown(int damage) { damageCooldown_ -= damage; }
	void TakeDamage(int damage);
	void SetIsDead(bool flag) { list.isDead_ = flag; }
	void SetIsAttack(bool flag) { list.isAttack_ = flag; }
	void SetIsCombo(bool flag) { list.isCombo_ = flag; }
	void SetComboStep(int value) { list.comboStep_ = value; }
	void SetIsImpact(bool flag) { list.isImpact_ = flag; }
	void SetIsCrouch(bool flag) { list.isCrouch_ = flag; }
	void SetInvincible(bool flag) { list.isInvincible_ = flag; }
	void SetInvincibleTime(float sec) { invincibleTime_ = sec * 60; }
	void SetHitPlayer(bool flag) { list.hitPlayer_ = flag; }
	void StartHitStop(float time) { list.hitStopTime_ = time; list.isHitStop_ = true; }
	void AddCriticalRate(int value) { list.criticalRate_ += value; }
	void AddCriticalDamage(int value) { list.criticalDamage += value; }

	// ����n
	bool IsMove(VECTOR moveVec);
	void SetRotation(void);

	// �t���[�����_���[�W�󂯂���
	bool IsDamagedThisFrame_ = false;

private:
	EnemyManager* enemymng_;
	AnimationManager* animation_;
	Camera* camera_;

	// �v���C���[���
	Info list;

	// �N�[���_�E���E���G
	int damageCooldown_ = 0;
	bool isDamaged_ = false;
	float invincibleTime_ = 0.0f;

	// �N���e�B�J���\��
	bool showCriticalText_ = false;
	float criticalDisplayTime_ = 0.0f;
	int criticalFontHandle_ = -1;

	void UpdateMove(void);		// �S�̂̓���
	void PlayerMove(int idleAnim, int walkAnim, int runAnim);		// �����Ȃǂ̓���
	bool CrouchUpdate(void);	// ���Ⴊ��
	bool AttackUpdate(void);	// �U��

	void DrawDebug(void);
};