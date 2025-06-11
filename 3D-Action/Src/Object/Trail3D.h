#pragma once
#include <DxLib.h>
#include <deque>
#include <vector>

class Trail3D
{
public:
	Trail3D();
	~Trail3D();

	void Init(const char* texturePath);
	void Update(const std::vector<VECTOR>& currentPoints);
	void Draw(void);
	void Release(void);
private:
	int trailHandle_;
	std::deque<std::vector<VECTOR>> trailPointsList_;
	const int maxTrailLength_ = 10;
};
