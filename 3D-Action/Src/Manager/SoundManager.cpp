#include "SoundManager.h"

// �ÓI�����o�[�ϐ��̏�����
SoundManager* SoundManager::instance_ = nullptr;

// �R���X�g���N�^
SoundManager::SoundManager()
	: bgmHandle(-1)    // BGM�n���h���̏�����
{
}

// �f�X�g���N�^
SoundManager::~SoundManager()
{
	// SE�n���h���̍폜
	for (auto& se : seHandles)
	{
		// �L���ȃn���h���݂̂̍폜
		if(se.second != -1)DeleteSoundMem(se.second);
	}

	// BGM�n���h���̍폜
	if (bgmHandle != -1)DeleteSoundMem(bgmHandle);	// �폜
}

// �N���G�C�g�C���X�^���X
void SoundManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		// ����A�N�Z�X���ɃC���X�^���X�𐶐�
		instance_ = new SoundManager;
	}
}

// �V���O���g���̃C���X�^���X�擾
SoundManager* SoundManager::GetInstance()
{
	return instance_;
}

// �V���O���g���̃C���X�^���X�̍폜
void SoundManager::DeleteInstance()
{
	if (instance_ != nullptr)
	{
		delete instance_;
		// �|�C���^��NULL�ɂ���
		instance_ = nullptr;
	}
}

// �R�s�[�R���X�g���N�^
SoundManager::SoundManager(const SoundManager& other)
	: bgmHandle(-1)
{
}

// SE�̃��[�h
bool SoundManager::LoadSE(const std::string& key, const char* fileName,bool is3DSound)
{
	int handle = -1;
	if (is3DSound)
	{
		SetCreate3DSoundFlag(true);			// 3D�T�E���h�̃t���O��L���ɂ�3D�����Ƃ��ēǂݍ���
		int handle = LoadSoundMem(fileName);	// �ǂݍ���		// �z��̊i�[
		SetCreate3DSoundFlag(false);		// ������ false �ɂ��Ȃ���
											// �ȍ~�̓ǂݍ��݂̑S�Ă�3D�����Ƃ��ēǂݍ��܂��
	}
	else
	{
		handle = LoadSoundMem(fileName);
	}
	
	if (handle != -1)
	{
		// �}�b�v�Ƀn���h����o�^
		seHandles[key] = handle;
		return true;
	}

	return false;
}

// BGM�̃��[�h
bool SoundManager::LoadBGM(const std::string& key, const char* fileName, bool is3DSound)
{
	int handle = -1;
	if (is3DSound)
	{
		SetCreate3DSoundFlag(true);			// 3D�T�E���h�̃t���O��L���ɂ�3D�����Ƃ��ēǂݍ���
		int handle = LoadSoundMem(fileName);	// �ǂݍ���		// �z��̊i�[
		SetCreate3DSoundFlag(false);		// ������ false �ɂ��Ȃ���
											// �ȍ~�̓ǂݍ��݂̑S�Ă�3D�����Ƃ��ēǂݍ��܂��
	}
	else
	{
		handle = LoadSoundMem(fileName);
	}

	if (handle != -1)
	{
		// �}�b�v�Ƀn���h����o�^
		seHandles[key] = handle;
		return true;
	}

	return false;
}

// SE�̍Đ�
void SoundManager::PlaySE(const std::string& key)
{
	auto se = seHandles.find(key); // ���ʉ����}�b�v���猟��
	if (se != seHandles.end() && se->second != -1)
	{
		// �Đ������ǂ���			
		if (CheckSoundMem(se->second) != 1)
		{
			PlaySoundMem(se->second, DX_PLAYTYPE_BACK,true); // ���ʉ��̍Đ�
		}
	}
}

// BGM�̍Đ�
void SoundManager::PlayBGM(const std::string& key)
{
	auto se = seHandles.find(key); // ���ʉ����}�b�v���猟��
	if (se != seHandles.end() && se->second != -1)
	{
		// �Đ������ǂ���			
		if (CheckSoundMem(se->second) != 1)
		{
			PlaySoundMem(se->second, DX_PLAYTYPE_LOOP, true); // ���ʉ��̍Đ�
		}
	}
}

// SE�̒�~
void SoundManager::StopSE(const std::string& key)
{
	auto se = seHandles.find(key); // ���ʉ����}�b�v���猟��
	if (se != seHandles.end() && se->second != -1)
	{
		StopSoundMem(se->second); // ���ʉ��̒�~
	}
}

// BGM�̒�~
void SoundManager::StopBGM(const std::string& key)
{
	auto se = seHandles.find(key); // ���ʉ����}�b�v���猟��
	if (se != seHandles.end() && se->second != -1)
	{
		StopSoundMem(se->second); // ���ʉ��̒�~
	}
}

bool SoundManager::IsPlayingSE(const std::string& seName)
{
	auto se = seHandles.find(seName); // ���ʉ����}�b�v���猟��
	if (se != seHandles.end())
	{
		// �n���h�������݂���ꍇ�A�Đ���Ԃ̊m�F
		int handle = se->second;
		// �Đ����Ȃ�1���Ԃ����
		return CheckSoundMem(handle) == 1;
	}
	// �n���h�������݂��Ȃ��ꍇ�́A�Đ�����ĂȂ��Ƃ݂Ȃ�
	return false;
}

// ���ʉ���3D�ʒu��ݒ�
void SoundManager::SetPosSE(const std::string& key, VECTOR vec)
{
	auto se = seHandles.find(key); // ���ʉ����}�b�v���猟��
	if (se != seHandles.end() && se->second != -1)
	{
		Set3DPositionSoundMem(vec, se->second); // 3D�ʒu�̐ݒ�
	}
}

// ���ʉ��̕�������͈͂�ݒ�
void SoundManager::SetAreaSE(const std::string& key, float radius)
{
	auto se = seHandles.find(key); // ���ʉ����}�b�v���猟��
	if (se != seHandles.end() && se->second != -1)
	{
		Set3DRadiusSoundMem(radius, se->second); // ��������͈͂̐ݒ�
	}
}

// ���ʉ��̉��ʂ�ݒ�
void SoundManager::SetVolumeSE(const std::string& key, int volume)
{
	auto se = seHandles.find(key); // ���ʉ����}�b�v���猟��
	if (se != seHandles.end() && se->second != -1)
	{
		ChangeVolumeSoundMem(volume, se->second); // ���ʂ̐ݒ�
	}
}

// �������
void SoundManager::ReleaseSound(const std::string& key)
{
	auto se = seHandles.find(key); // ���ʉ����}�b�v���猟��
	if (se != seHandles.end() && se->second != -1)
	{
		DeleteSoundMem( se->second); // ���ʂ̐ݒ�
	}
}

// 3D��Ԃ̂P���[�g���ɑ������鋗���̐ݒ�
void SoundManager::Set3DSoundMetre(float Distace)
{
	// 3D��Ԃ�1���[�g���ɑ������鋗���̐ݒ�
	Set3DSoundOneMetre(Distace);
}