#include "Sword.h"
#include "Player.h"

Sword::Sword()
{
	list.model_ = -1;
	list.pos_ = { 0.0f,0.0f,0.0f };
	list.rot_ = { 0.0f,0.0f,0.0f };
	list.scale_ = { 1.0f, 1.0f, 1.0f };
	list.righthandidx_ = -1;
	list.righthandpos_ = { 0.0f,0.0f,0.0f };
}

Sword::~Sword()
{
	Release();
}

void Sword::Init()
{
	list.model_ = MV1LoadModel("Data/Model/Sword.mv1");

	list.pos_ = VGet(0.0f, 0.0f, 0.0f);
	list.rot_ = VGet(0.0f, 0.0f, 0.0f);
	list.scale_ = VGet(1.0f, 1.0f, 1.0f);
	MV1SetPosition(list.model_, list.pos_);
	MV1SetRotationXYZ(list.model_, list.rot_);
	MV1SetScale(list.model_, list.scale_);
}

void Sword::Update()
{
	//モデルか右手が見つからなかったら終了
	if (list.model_ == -1)return;

	//剣の最終位置(補正あり)
	list.pos_ = Player::GetInstance().GetRightHandPosition();
	MV1SetPosition(list.model_, list.pos_);

	list.rot_ = Player::GetInstance().GetPlayerRot();
	list.rot_.x -= 45.0f;
	MV1SetRotationXYZ(list.model_, list.rot_);
}

void Sword::Draw(void)
{
	if (Player::GetInstance().GetWeaposFlag()) {
		MV1DrawModel(list.model_);
	}
}

void Sword::Release(void)
{
	if (list.model_ != -1) {
		MV1DeleteModel(list.model_);
		list.model_ = -1;
	}
}

