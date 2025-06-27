#include "BuffDataManager.h"
#include "../Application.h"
#include <fstream>
#include <sstream>

BuffDataManager BuffDataManager::instance_;

BuffDataManager& BuffDataManager::GetInstance()
{
	return instance_;
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

// CSVファイルからバフ情報を読み込む
void BuffDataManager::LoadFromCSV(const std::string& path)
{
	// 古い画像の解放
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
		ErrorLogAdd("Buff CSVファイルが開けませんでした");
		return;
	}

	std::string line;
	std::getline(file, line);		// 一行目を飛ばす

	// ２行目から読み込む
	while (std::getline(file, line))
	{
		std::stringstream ss(line);
		std::string token;
		BuffData data;

		std::getline(ss, token, ','); data.id = std::stoi(token);
		std::getline(ss, token, ','); data.imagePath_ = token;
		std::getline(ss, token, ','); data.attackUp_ = std::stoi(token);
		std::getline(ss, token, ','); data.hpUp_ = std::stoi(token);
		std::getline(ss, token, ','); data.successRate_ = token.empty() ? 100 : std::stoi(token);
		std::getline(ss, token, ','); data.valueSuccess_ = token.empty() ? 0 : std::stoi(token);
		std::getline(ss, token, ','); data.valueFailure_ = token.empty() ? 0 : std::stoi(token);
		std::getline(ss, token, ','); data.description_ = token;
	
		// 画像読み込み
		data.handleId_ = LoadGraph((Application::PATH_BUFF + data.imagePath_).c_str());
		if (data.handleId_ == -1)
		{
			ErrorLogAdd(("画像読み込み失敗" + data.imagePath_).c_str());
		}
		else
		{
			GetGraphSize(data.handleId_, &data.graph_Width, &data.graph_Height);
		}
		buffList_.push_back(data);
	}
	file.close();
}

// 指定されたIDに対応するバフデータを返す
const BuffData& BuffDataManager::GetBuffData(int id) const
{
	for (const auto& data : buffList_)
	{
		if (data.id == id)
			return data;
	}
	return buffList_.front();
}

// インデックス指定でバフを返す(バフ選択表示などに使用)
const BuffData& BuffDataManager::GetBuffDataByIndex(int index) const
{
	return buffList_.at(index);
}

// バフデータの数を返す
int BuffDataManager::GetBuffCount() const
{
	return static_cast<int>(buffList_.size());
}