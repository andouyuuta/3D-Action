#include "Stage.h"

Stage::Stage(void)
{
	list.model_ = -1;
	list.pos_ = { 0.0f,0.0f,0.0f };
	list.rot_ = { 0.0f,0.0f,0.0f };
	list.scale_ = { 1.0f,1.0f,1.0f };
}

Stage::~Stage(void)
{

}

// ‰Šú‰»
void Stage::Init(void)
{
	list.model_ = MV1LoadModel("Data/Model/BattleDome.mv1");

	list.pos_ = { 0.0f,0.0f,0.0f };
	list.rot_ = { 0.0f,0.0f,0.0f };
	list.scale_ = { 1.0f,1.0f,1.0f };

	MV1SetPosition(list.model_, list.pos_);
	MV1SetRotationXYZ(list.model_, list.rot_);
	MV1SetScale(list.model_, list.scale_);

	MV1SetupCollInfo(list.model_);
}

// XVˆ—
void Stage::Update(void)
{
}

// •`‰æ
void Stage::Draw(void)
{
	MV1DrawModel(list.model_);
}

// ‰ğ•ú
void Stage::Release(void)
{
	MV1DeleteModel(list.model_);
}