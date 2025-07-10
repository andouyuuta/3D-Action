#include "RandomManager.h"

// 実体の生成
std::mt19937 RandomManager::mt;		// ランダムエンジンの実体
std::uniform_real_distribution<float> RandomManager::dist(0.0f, 100.0f); // 乱数の出る範囲の設定

// 初期化
void RandomManager::Init()
{
	std::random_device rd;		// ランダムデバイス（乱数生成させる装置）
	mt.seed(rd());				// メルセンヌ・ツイスタ(mt)にrd()の値をシードとして与える
}

// クリティカルが起きるかの判定
bool RandomManager::Random(float criticalRatePercent)
{
	// dist(mt)は0.0～100.0の範囲のfloatをランダムに生成
	// その値がcriticalRatePercent未満であればクリティカル判定
	return dist(mt) < criticalRatePercent;
}