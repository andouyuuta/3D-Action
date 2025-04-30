#include "SoundManager.h"

// 静的メンバー変数の初期化
SoundManager* SoundManager::instance_ = nullptr;

// コンストラクタ
SoundManager::SoundManager()
	: bgmHandle(-1)    // BGMハンドルの初期化
{
}

// デストラクタ
SoundManager::~SoundManager()
{
	// SEハンドルの削除
	for (auto& se : seHandles)
	{
		// 有効なハンドルのみの削除
		if(se.second != -1)DeleteSoundMem(se.second);
	}

	// BGMハンドルの削除
	if (bgmHandle != -1)DeleteSoundMem(bgmHandle);	// 削除
}

// クリエイトインスタンス
void SoundManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		// 初回アクセス時にインスタンスを生成
		instance_ = new SoundManager;
	}
}

// シングルトンのインスタンス取得
SoundManager* SoundManager::GetInstance()
{
	return instance_;
}

// シングルトンのインスタンスの削除
void SoundManager::DeleteInstance()
{
	if (instance_ != nullptr)
	{
		delete instance_;
		// ポインタをNULLにする
		instance_ = nullptr;
	}
}

// コピーコンストラクタ
SoundManager::SoundManager(const SoundManager& other)
	: bgmHandle(-1)
{
}

// SEのロード
bool SoundManager::LoadSE(const std::string& key, const char* fileName,bool is3DSound)
{
	int handle = -1;
	if (is3DSound)
	{
		SetCreate3DSoundFlag(true);			// 3Dサウンドのフラグを有効にし3D音声として読み込む
		int handle = LoadSoundMem(fileName);	// 読み込み		// 配列の格納
		SetCreate3DSoundFlag(false);		// ここで false にしないと
											// 以降の読み込みの全てが3D音声として読み込まれる
	}
	else
	{
		handle = LoadSoundMem(fileName);
	}
	
	if (handle != -1)
	{
		// マップにハンドルを登録
		seHandles[key] = handle;
		return true;
	}

	return false;
}

// BGMのロード
bool SoundManager::LoadBGM(const std::string& key, const char* fileName, bool is3DSound)
{
	int handle = -1;
	if (is3DSound)
	{
		SetCreate3DSoundFlag(true);			// 3Dサウンドのフラグを有効にし3D音声として読み込む
		int handle = LoadSoundMem(fileName);	// 読み込み		// 配列の格納
		SetCreate3DSoundFlag(false);		// ここで false にしないと
											// 以降の読み込みの全てが3D音声として読み込まれる
	}
	else
	{
		handle = LoadSoundMem(fileName);
	}

	if (handle != -1)
	{
		// マップにハンドルを登録
		seHandles[key] = handle;
		return true;
	}

	return false;
}

// SEの再生
void SoundManager::PlaySE(const std::string& key)
{
	auto se = seHandles.find(key); // 効果音をマップから検索
	if (se != seHandles.end() && se->second != -1)
	{
		// 再生中かどうか			
		if (CheckSoundMem(se->second) != 1)
		{
			PlaySoundMem(se->second, DX_PLAYTYPE_BACK,true); // 効果音の再生
		}
	}
}

// BGMの再生
void SoundManager::PlayBGM(const std::string& key)
{
	auto se = seHandles.find(key); // 効果音をマップから検索
	if (se != seHandles.end() && se->second != -1)
	{
		// 再生中かどうか			
		if (CheckSoundMem(se->second) != 1)
		{
			PlaySoundMem(se->second, DX_PLAYTYPE_LOOP, true); // 効果音の再生
		}
	}
}

// SEの停止
void SoundManager::StopSE(const std::string& key)
{
	auto se = seHandles.find(key); // 効果音をマップから検索
	if (se != seHandles.end() && se->second != -1)
	{
		StopSoundMem(se->second); // 効果音の停止
	}
}

// BGMの停止
void SoundManager::StopBGM(const std::string& key)
{
	auto se = seHandles.find(key); // 効果音をマップから検索
	if (se != seHandles.end() && se->second != -1)
	{
		StopSoundMem(se->second); // 効果音の停止
	}
}

bool SoundManager::IsPlayingSE(const std::string& seName)
{
	auto se = seHandles.find(seName); // 効果音をマップから検索
	if (se != seHandles.end())
	{
		// ハンドルが存在する場合、再生状態の確認
		int handle = se->second;
		// 再生中なら1が返される
		return CheckSoundMem(handle) == 1;
	}
	// ハンドルが存在しない場合は、再生されてないとみなす
	return false;
}

// 効果音の3D位置を設定
void SoundManager::SetPosSE(const std::string& key, VECTOR vec)
{
	auto se = seHandles.find(key); // 効果音をマップから検索
	if (se != seHandles.end() && se->second != -1)
	{
		Set3DPositionSoundMem(vec, se->second); // 3D位置の設定
	}
}

// 効果音の聞こえる範囲を設定
void SoundManager::SetAreaSE(const std::string& key, float radius)
{
	auto se = seHandles.find(key); // 効果音をマップから検索
	if (se != seHandles.end() && se->second != -1)
	{
		Set3DRadiusSoundMem(radius, se->second); // 聞こえる範囲の設定
	}
}

// 効果音の音量を設定
void SoundManager::SetVolumeSE(const std::string& key, int volume)
{
	auto se = seHandles.find(key); // 効果音をマップから検索
	if (se != seHandles.end() && se->second != -1)
	{
		ChangeVolumeSoundMem(volume, se->second); // 音量の設定
	}
}

// 解放処理
void SoundManager::ReleaseSound(const std::string& key)
{
	auto se = seHandles.find(key); // 効果音をマップから検索
	if (se != seHandles.end() && se->second != -1)
	{
		DeleteSoundMem( se->second); // 音量の設定
	}
}

// 3D空間の１メートルに相当する距離の設定
void SoundManager::Set3DSoundMetre(float Distace)
{
	// 3D空間の1メートルに相当する距離の設定
	Set3DSoundOneMetre(Distace);
}