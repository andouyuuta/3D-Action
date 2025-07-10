#pragma once
#include <DxLib.h>
#include <string>
#include <vector>

// バフのデータ管理
struct BuffData
{
    int id;                     // 識別番号
    std::string imagePath_;     // 画像パス
    int AppearRate;             // 出現確率
    int attackUp_;              // 攻撃力上昇量
    int hpUp_;                  // HP上昇量
    int successRate_;           // 成功確率(％)
    int valueSuccess_;          // 成功時の値
    int valueFailure_;          // 失敗時の値
    std::string description_;   // 説明

    // CSVファイルには書いていない情報
    int handleId_ = -1;         // 画像ハンドル
    int graph_Width = 0;
    int graph_Height = 0;
};

class BuffDataManager
{
public:
    static BuffDataManager& GetInstance();

    BuffDataManager(void);

    void Release(void);

    void LoadFromCSV(const std::string& path);              // CSVファイルからバフデータを読み込む
    const BuffData& GetBuffData(int id)const;               // 指定IDに対応するバフデータを返す
    const BuffData& GetBuffDataByIndex(int index)const;     // 配列でバフデータを取得
    int GetBuffCount()const;                                // 登録されているデータを返す
private:
    static BuffDataManager instance_;
    std::vector<BuffData> buffList_;
};

