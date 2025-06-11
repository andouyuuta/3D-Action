#include "Trail3D.h"

Trail3D::Trail3D()
	: trailHandle_(-1)
{}

Trail3D::~Trail3D()
{
	if (trailHandle_ != -1) {
		DeleteGraph(trailHandle_);
	}
}

void Trail3D::Init(const char* texturePath)
{
	trailHandle_ = LoadGraph(texturePath);
}

void Trail3D::Update(const std::vector<VECTOR>& currentPoints)
{
	trailPointsList_.push_front(currentPoints);

	// 最大の長さを超えたら古いものから削除
	if (trailPointsList_.size() > maxTrailLength_) {
		trailPointsList_.pop_back();
	}
}

void Trail3D::Draw()
{
	// 軌跡がないなら何もしない
	if (trailPointsList_.size() < 2) return;

	for (size_t i = 0; i < trailPointsList_.size() - 1; ++i) {
		const auto& currFrame = trailPointsList_[i];
		const auto& nextFrame = trailPointsList_[i + 1];

		// フレームが古くなれば透明度を下げる
		int alpha = 255 - int(i * 25);
		if (alpha < 0) alpha = 0;

		// ブレンドモードをアルファに
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

		for (size_t j = 0; j < currFrame.size() - 1; ++j) {
			// 3D座標をスクリーン座標に変換
			VECTOR currStart = ConvWorldPosToScreenPos(currFrame[j]);
			VECTOR currEnd = ConvWorldPosToScreenPos(currFrame[j + 1]);
			VECTOR nextEnd = ConvWorldPosToScreenPos(nextFrame[j + 1]);
			VECTOR nextStart = ConvWorldPosToScreenPos(nextFrame[j]);

			// 4点を使ってテクスチャを描画
			DrawModiGraphF(
				currStart.x, currStart.y,
				currEnd.x, currEnd.y,
				nextEnd.x, nextEnd.y,
				nextStart.x, nextStart.y,
				trailHandle_,
				true
			);
		}

		// ブレンドモードを戻す
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}

void Trail3D::Release(void)
{
	DeleteGraph(trailHandle_);
}
