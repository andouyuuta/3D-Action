#pragma once
#include <DxLib.h>
#include <vector>

class Sword;
class EnemyManager;

class Player
{
public:
	// �ړ���
	static constexpr float MOVE_SPEED_STOP = 0.0f;		// ���Ⴊ��&�~�܂�
	static constexpr float MOVE_SPEED_WALK = 2.5f;		// ����
	static constexpr float MOVE_SPEED_RUN = 8.0f;		// ����

	// �������f���␳�p�x
	static constexpr VECTOR INIT_MODEL_ROT_OFFSET = { 0.0f,DX_PI_F / 2.0f,0.0f };

	// �����Ă������
	static constexpr VECTOR INIT_DIR = { 0.0f,0.0f,-1.0f };

	// ����p
	static constexpr float VIEW_ANGLE = 180.0f;

	// �A�j���V�����̍Đ����x
	static constexpr float ANIM_SPEED = 0.5f;			//�����Ă��鎞
	static constexpr float CROUCH_ANIM_SPEED = 1.5f;	//���Ⴊ��ł���Ƃ�
	static constexpr float CROUCH_ATTACK_SPEED = 1.0f;	//���Ⴊ�ݍU��
	static constexpr float DEAD_ANIM_SPEED = 0.2f;		//����ł��鎞
	static constexpr float ATTACK_ANIM_SPEED = 0.75f;	//�U�����Ă���


	// �v���C���[�̍ő�HP
    int PLAYER_MAX_HP = 200;

	// �v���C���[�̍ő�U����
    int AttackPower_ = 150;

	enum PlayerAnim {
		NoWeaponIdle = 0,		//����Ȃ��ҋ@
		NoWeaponWalk,			//����Ȃ�����
		NoWeaponRun,			//����Ȃ�����
		WeaponOut,				//������o��	
		Sheach,					//����߂�
		WeaponIdle,				//���킠��ҋ@
		WeaponWalk,				//���킠�����
		WeaponRun,				//���킠�葖��
		Strafe,					//������
		Jump,					//�W�����v
		Crouch,					//���Ⴊ��
		Crouch_Cancel,			//���Ⴊ�݉���
		Crouch_Idle,			//���Ⴊ�ݑҋ@
		Crouch_Guard,			//���Ⴊ�ݖh��
		Crouch_Attack,
		First_Attack,			//�U���P
		Second_Attack,			//�U���Q
		Third_Attack,			//�U���R
		Force_Attack,			//�U���S
		Death,					//���S
		Impact,					//�U���󂯂��Ƃ�
	};

	// �v���C���[�̏��̍\����
	struct Info
	{
		int modelId_;			// ���f��ID

		VECTOR pos_;			// ���W
		VECTOR rot_;			// ��]�l
		VECTOR scale_;			// �傫��
		float moveSpeed_;		// �ړ��X�s�[�h
		int moveDir_;			// �ړ�����
		int moveKind_;			// �v���C���[�̈ړ����
		int hp_;				// HP
		int attackPower_;		// �U����

		// �ړ��x�N�g��
		VECTOR moveVec_;		// �ړ��x�N�g��
		VECTOR oldMoveVec_;		// ���O�̈ړ��x�N�g��
		VECTOR moveVecRad_;		// �ړ��x�N�g������������p�x

		VECTOR localRot_;		// �����p�����p�x

		//�A�j���[�V����
		int animIndex_;			//�A�j���[�V�����̎��
		int  animAttachNo_;		// �ݒ肷��A�j���[�V�����̔ԍ�
		float animTotalTime_;	// �A�j���[�V�����̑��Đ�����
		float currentAnimTime_; // ���O�̃A�j���[�V�����̎���
		bool isAnimLock_;		//�A�j���[�V�������b�N

		bool isDead_;			//���S�������ǂ���
		bool isCrouch_;			//���Ⴊ��ł��邩
		bool isWeapon_;			//����o���Ă��邩
		bool isAttack_;			//�U�����Ă��邩
		bool isCombo_;			//�R���{���Ă��邩
		int comboStep_;			//�R���{�i�K

		float currentRatio_;	//���݂̃A�j���[�V�����̍Đ�����(����)
		float remainingTime_;	//�A�j���[�V���������Ƃǂꂭ�炢�ŏI��邩

		bool isGameOver_;		//���S�������ǂ���
		bool isImpact_;			//�U���󂯂���
		bool isInvincible_;		//���G���ǂ���
		bool hitPlayer_;		// �U�����ň�x�����q�b�g���Ȃ��悤�ɂ���t���O

		float hitStopTime_;		// �q�b�g�X�g�b�v�p�^�C�}�[
		bool isHitStop_;		// �q�b�g�X�g�b�v�p�t���O
	
		MATRIX rotationMatrix_;	// ��](�}�g���b�N�X)
	};

	// �C���X�^���X�̐���
	static void CreateInstance(void);

	// �C���X�^���X�̎擾
	static Player& GetInstance(void);


	Player(void);		// �R���X�g���N�^
	~Player(void);		// �f�X�g���N�^

	void Init(EnemyManager* mng);	// ������
	void Update(float deltaTime);	// �X�V
	void Draw(void);	// �`��
	void Release(void); // ���

	// �x�N�g���̈ړ�����
	bool IsMove(VECTOR _moveVec);

	void SetRotation(void);

	// �Q�b�^�[�֐�
	struct Info GetInfo(void) { return list; }

	// �v���C���[�̃X�s�[�h��ݒ�
	void SetPlayerSpeed(const VECTOR& speed) { list.moveVec_ = speed; }	// ���x�ݒ�

	void ChangeAnimation(int idx, bool lock = false);	//�A�j���[�V�����؂�ւ�

	void HitStop(void);

	void RecoverHp();		// �S��
	void AddAttack(int amount); // �U���͂��グ��
	void AddMaxHp(int amount);  // HP,�U���͂��グ��

	void SetCriticalDisplay(bool enable);	// �N���e�B�J���\������

	bool IsPlayingImpactAnimation()const;	// �A�j���[�V���������ǂ���
	bool AttackActive(void);				// �U���A�j���[�V�������ǂ���

	bool IsDamagedThisFrame_;				// ���̃t���[���Ń_���[�W���󂯂���

	// �Q�b�^�[
	[[nodiscard]] int GetPlayerModel(void) { return list.modelId_; }
	[[nodiscard]] VECTOR GetPlayerPos(void) { return list.pos_; }
	[[nodiscard]] VECTOR GetPlayerRot(void) { return list.rot_; }
	[[nodiscard]] float GetRotX(void) { return list.rot_.x; }
	[[nodiscard]] float GetRotY(void) { return list.rot_.y; }
	[[nodiscard]] bool GetIsWeapon(void) { return list.isWeapon_; }
	[[nodiscard]] bool GetIsDead(void) { return list.isDead_; }
	[[nodiscard]] int GetRightHandIndex(void) { return MV1SearchFrame(list.modelId_, "mixamorig:RightHand"); }
	[[nodiscard]] VECTOR GetRightHandPosition(void) { return MV1GetFramePosition(list.modelId_, GetRightHandIndex()); }
	[[nodiscard]] int GetDamageCooldown(void) { return damageCooldown_; }
	[[nodiscard]] int GetHp(void) { return list.hp_; }
	[[nodiscard]] bool GetIsAttack(void){return list.isAttack_;}
	[[nodiscard]] bool GetIsInvincible(void)const { return list.isInvincible_; }
	[[nodiscard]] bool GetIsHitStop(void)const { return list.isHitStop_; }
	[[nodiscard]] int GetComboStep(void)const { return list.comboStep_; }
	[[nodiscard]] MATRIX GetRotationMat(void)const { return list.rotationMatrix_; }
	int GetAttackPower()const { return list.attackPower_ ; }	// �v���C���[�̍U����

	// �Z�b�^�[
	[[nodiscard]] void SetPlayerPos(VECTOR pos) { list.pos_ = pos; MV1SetPosition(list.modelId_, list.pos_); }
	[[nodiscard]] void SetDamageCooldown(int damage) { damageCooldown_ = damage; }
	[[nodiscard]] void SetMoveSpeed(float speed) { list.moveSpeed_ = speed; }
	[[nodiscard]] void DecreaseCoolDown(int damage) { damageCooldown_ -= damage; }
	[[nodiscard]] void TakeDamage(int damage);
	[[nodiscard]] void SetIsDead(bool dead) { list.isDead_ = dead; }
	[[nodiscard]] void SetInvincible(bool flg) { list.isInvincible_ = flg; }
	[[nodiscard]] void SetHitPlayer(bool hitflg) { list.hitPlayer_ = hitflg; }
	[[nodiscard]] void StartHitStop(float time) { list.hitStopTime_ = time; list.isHitStop_ = true; }
private:

	// �G
	EnemyManager* enemymng_;

	// �ÓI�C���X�^���X
	static Player* instance_;

	// �\���̂��i�[����ϐ�
	struct Info list;

	int damageCooldown_; // �_���[�W�Ԋu����p

	// �v���C���[���_���[�W�����ǂ���(���i�q�b�g�h�~�p)
	bool isDamaged_;
	// ���G���Ԃ̎c�莞��(�b)
	float invincibleTime_;

	// �N���e�B�J���\���p
	bool showCriticalText_;
	float criticalDisplayTime_;


	void UpdateMove(void);								//�ړ��X�V
	void PlayerMove(int idle, int walk, int run);		//�v���C���[�̓���
	bool CrouchUpdate(void);							//���Ⴊ��
	bool AttackUpdate(void);							//�U��
	bool AttackCombo(int nowcombo, int nextanimidx, int nextstep, float reception, float remainingtime);
	void PlayAnimation(void);							//�A�j���[�V�����Đ�
	void DebugAnimation(void);							//�����L�[�ŃA�j���[�V�����؂�ւ�

};

