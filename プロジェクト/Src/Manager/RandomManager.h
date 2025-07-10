#pragma once
#include <random>

class RandomManager
{
public:
	static void Init(void);								// 初期化
	static bool Random(float criticalRatePercent);		// ランダム発動したか

private:
	static std::mt19937 mt;								// メルセンヌ・ツイスタ(疑似乱数エンジン)
	static std::uniform_real_distribution<float>dist;	// 0.0～100.0
};

