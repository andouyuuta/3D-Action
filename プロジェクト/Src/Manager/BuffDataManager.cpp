#include "BuffDataManager.h"
#include "../Application.h"
#include <fstream>
#include <sstream>

BuffDataManager BuffDataManager::instance_;

BuffDataManager& BuffDataManager::GetInstance()
{
	return instance_;
}

BuffDataManager::BuffDataManager(void)
{
	
}

void BuffDataManager::Release(void)
{
	for (auto& data : buffList_)
	{
		if (data.handleId_ != -1)
		{
			DeleteGraph(data.handleId_);
			data.handleId_ = -1;
		}
	}
	buffList_.clear();
}

// CSV�t�@�C������o�t����ǂݍ���
void BuffDataManager::LoadFromCSV(const std::string& path)
{
	// �Â��摜�̉��
	for (auto& data : buffList_)
	{
		if (data.handleId_ != -1)
		{
			DeleteGraph(data.handleId_);
			data.handleId_ = -1;
		}
	}

	buffList_.clear();

	std::ifstream file(path);
	if (!file.is_open())
	{
		printfDx("Buff CSV�t�@�C�����J���܂���ł���");
		return;
	}

	std::string line;
	std::getline(file, line);		// ��s�ڂ��΂�

	// �Q�s�ڂ���ǂݍ���
	while (std::getline(file, line))
	{
		// ��s�Ȃ�X�L�b�v
		if (line.empty())continue;

		std::stringstream ss(line);
		std::string token;
		BuffData data;

		try
		{
			// 1��ځFID
			std::getline(ss, token, ',');
			if (token.empty()) continue;
			data.id = std::stoi(token);

			// 2��ځF�摜�p�X
			std::getline(ss, token, ',');
			data.imagePath_ = token;

			// 3��ځF�o���m��
			std::getline(ss, token, ',');
			data.AppearRate = token.empty() ? 0 : std::stoi(token);

			// 4��ځF�U���A�b�v
			std::getline(ss, token, ',');
			data.attackUp_ = token.empty() ? 0 : std::stoi(token);

			// 5��ځFHP�A�b�v
			std::getline(ss, token, ',');
			data.hpUp_ = token.empty() ? 0 : std::stoi(token);

			// 6��ځF�������i��Ȃ�100�j
			std::getline(ss, token, ',');
			data.successRate_ = token.empty() ? 100 : std::stoi(token);

			// 7��ځF�������̒l
			std::getline(ss, token, ',');
			data.valueSuccess_ = token.empty() ? 0 : std::stoi(token);

			// 8��ځF���s���̒l
			std::getline(ss, token, ',');
			data.valueFailure_ = token.empty() ? 0 : std::stoi(token);

			// 9��ځF����
			std::getline(ss, token, ',');
			data.description_ = token;
		}
		catch (const std::invalid_argument& e)
		{
			printfDx("CSV�ǂݍ��ݎ��s�F���l�ϊ��ł��Ȃ��f�[�^������܂�");
			continue;  // ���̍s���X�L�b�v
		}
		catch (const std::out_of_range& e)
		{
			printfDx("CSV�ǂݍ��ݎ��s�F���l���傫�����܂�");
			continue;
		}

		// �摜�ǂݍ���
		data.handleId_ = LoadGraph((Application::PATH_BUFF + data.imagePath_).c_str());
		if (data.handleId_ == -1)
		{
			printfDx(("�摜�ǂݍ��ݎ��s: " + data.imagePath_).c_str());
		}
		else
		{
			GetGraphSize(data.handleId_, &data.graph_Width, &data.graph_Height);
		}

		buffList_.push_back(data);
	}
	file.close();
}

// �w�肳�ꂽID�ɑΉ�����o�t�f�[�^��Ԃ�
const BuffData& BuffDataManager::GetBuffData(int id) const
{
	for (const auto& data : buffList_)
	{
		if (data.id == id)
			return data;
	}
	return buffList_.front();
}

// �C���f�b�N�X�w��Ńo�t��Ԃ�(�o�t�I��\���ȂǂɎg�p)
const BuffData& BuffDataManager::GetBuffDataByIndex(int index) const
{
	return buffList_.at(index);
}

// �o�t�f�[�^�̐���Ԃ�
int BuffDataManager::GetBuffCount() const
{
	return static_cast<int>(buffList_.size());
}