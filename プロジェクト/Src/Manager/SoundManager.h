#pragma once
#include <DxLib.h>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>

class SoundManager
{
public:

	// �N���G�C�g�C���X�^���X(������SoundManager�������̂�)
	static void CreateInstance(void);
	// �C���X�^���X���擾���郁�\�b�h(�V���O���g��)
	static SoundManager* GetInstance();
	// �C���X�^���X���폜���郁�\�b�h(�Q�[���̏I���O�ɕK���Ă�)
	static void DeleteInstance();

	// ���ʉ�(SE)�����[�h����
	bool LoadSE(const std::string& key, const char* fileName, bool is3DSound = true);
	// BGM�����[�h����
	bool LoadBGM(const std::string& key, const char* fileName, bool is3DSound = true);

	// ���ʉ�(SE)�̍Đ�����
	void PlaySE(const std::string& key);
	// BGM�̍Đ�
	void PlayBGM(const std::string& key);

	// ���ʉ�(SE)�̒�~
	void StopSE(const std::string& key);
	// BGM�̒�~
	void StopBGM(const std::string& key);

	// SE���Đ�����Ă���̂�
	bool IsPlayingSE(const std::string& seName);

	// �X�V
	// ���ʉ�(SE)��3D�ʒu�̐ݒ菈��
	void SetPosSE(const std::string& key, VECTOR vec);
	// ���ʉ�(SE)�̕�������͈͂̐ݒ菈��
	void SetAreaSE(const std::string& key, float radius);
	// // ���ʉ�(SE)�̉��ʂ̐ݒ菈��
	void SetVolumeSE(const std::string& key, int volume);

	// �������
	void ReleaseSound(const std::string& key);
	void ReleaseAll(void);

	// 3D��ԏ��1m
	void Set3DSoundMetre(float Distace);

private:

	// �R���X�g���N�^�ƃf�X�g���N�^��private�ɒu�����Ƃɂ��
	// ���̂Ƃ���Ŏg���Ȃ��悤�ɂ��Ă���i�O���Q�Ƃ�h���j
	// �R���X�g���N�^
	SoundManager();
	// �f�X�g���N�^
	~SoundManager();

	// �R�s�[�R���X�g���N�^
	// ���̏����͕ϐ���SoundManager�����邱�Ƃ��ł��Ȃ��悤�ɂ��鏈��
	// ����ɂ�葼�̂Ƃ���Ő�Ύg���Ȃ����̂ɂȂ�
	SoundManager(const SoundManager& other);

	// �V���O���g���̃C���X�^���X�ւ̃|�C���^
	static SoundManager* instance_;

	// ���ʉ��̃n���h�����i�[����}�b�v
	// �L�[�ƃn���h���̃y�A
	std::unordered_map<std::string, int> seHandles;
	std::unordered_map<std::string, int>bgmHandles;

	// �n���h��ID
	//int bgmHandle;		// BGM

};

