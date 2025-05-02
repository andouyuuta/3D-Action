#include "Stage.h"
#include "Player.h"

Stage* Stage::instance_ = nullptr;

void Stage::CreateInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new Stage();
	}
	instance_->Init();
}

Stage& Stage::GetInstance(void)
{
	return *instance_;
}

Stage::Stage()
{
	list.model_ = -1;
	list.pos_ = { 0.0f,0.0f,0.0f };
	list.rot_ = { 0.0f,0.0f,0.0f };
	list.scale_ = { 1.0f,1.0f,1.0f };
}

Stage::~Stage()
{

}

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

void Stage::Update(void)
{

}
void Stage::Draw(void)
{
	MV1DrawModel(list.model_);
}
void Stage::Release(void)
{
	MV1DeleteModel(list.model_);
}

//プレイヤーとステージの当たり判定
void Stage::PlayerToStageCollision(void)
{
	//プレイヤーの座標から縦のカプセル
	VECTOR playerpos = Player::GetInstance().GetPlayerPos();
	VECTOR capsulelow = playerpos;
	VECTOR capsulehigh = playerpos;
	capsulehigh.y += 100.0f;
	MV1CollCheck_Capsule(list.model_, -1, playerpos, capsulehigh, 100.0f);
}
