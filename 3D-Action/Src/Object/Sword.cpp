#include "Sword.h"

Sword::Sword()
{
	list.model_ = -1;
	list.pos_ = { 0.0f,0.0f,0.0f };
	list.rot_ = { 0.0f,0.0f,0.0f };
	list.scale_ = { 1.0f, 1.0f, 1.0f };
}

Sword::~Sword()
{
}

void Sword::Init(void)
{
	list.model_ = MV1LoadModel("Data/Model/Sword.mv1");
	list.pos_ = { 0.0f,0.0f,0.0f };
	list.rot_ = { 0.0f,0.0f,0.0f };
	list.scale_ = { 1.0f,1.0f,1.0f };
	MV1SetPosition(list.model_, list.pos_);
	MV1SetRotationXYZ(list.model_, list.rot_);
	MV1SetScale(list.model_, list.scale_);
}

void Sword::Update(void)
{
}

void Sword::Draw(void)
{
	MV1DrawModel(list.model_);
}

void Sword::Release(void)
{
}
