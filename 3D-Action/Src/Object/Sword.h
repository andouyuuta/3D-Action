#pragma once
#include <DxLib.h>
#include <deque>
#include <vector>

class Trail3D;

class Sword
{
public:
	struct Info 
	{
		int model_;
		VECTOR pos_;
		VECTOR rot_;
		VECTOR scale_;

		int rightHandIdx_;
		VECTOR rightHandPos_;
	};
	// �C���X�^���X�̐���
	static void CreateInstance(void);
	// �C���X�^���X�̎擾
	static Sword& GetInstance(void);

	Sword();
	~Sword();

	void Init(void);
	void Update(void);
	void Draw(void);
	void Release(void);

	std::vector<VECTOR> GetBladePoints(float startY, float endY, float interval) const;

	[[nodiscard]] int GetBodyFrameIdx(void) { return MV1SearchFrame(list.model_, "pCube1.001"); }
	[[nodiscard]] VECTOR GetBodyPosition(void) { return MV1GetFramePosition(list.model_, GetBodyFrameIdx()); }
	[[nodiscard]] VECTOR GetSwordPosition(void) { return list.pos_; }
private:
	// �ÓI�C���X�^���X
	static Sword* instance_;

	Info list;

	Trail3D* trail;
};

//#pragma once
//#include <DxLib.h>
//#include <deque>
//#include <vector>
//
//class Trail3D;
//
//class Sword
//{
//public:
//	struct Info
//	{
//		int model_;
//		VECTOR pos_;
//		VECTOR rot_;
//		VECTOR scale_;
//
//		int rightHandIdx_;
//		VECTOR rightHandPos_;
//	}list;
//
//	// �C���X�^���X�̐���
//	static void CreateInstance(void);
//	// �C���X�^���X�̎擾
//	static Sword& GetInstance(void);
//
//	Sword();
//	~Sword();
//
//	void Init();
//	void Update();
//	void Draw(void);
//	void Release(void);
//
//	std::vector<VECTOR> GetBladePoints(float startY, float endY, float interval) const;
//
//	[[nodiscard]] int GetBodyFrameIdx(void) { return MV1SearchFrame(list.model_, "pCube1.001"); }
//	[[nodiscard]] VECTOR GetBodyPosition(void) { return MV1GetFramePosition(list.model_, GetBodyFrameIdx()); }
//
//	[[nodiscard]] VECTOR GetSwordPosition(void) { return list.pos_; }
//private:
//	// �ÓI�C���X�^���X
//	static Sword* instance_;
//
//	Trail3D* trail3D;
//
//	int trailHandle_;
//};