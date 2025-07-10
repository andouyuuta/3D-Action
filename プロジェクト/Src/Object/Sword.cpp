#include "Sword.h"
#include "Player.h"
#include "Trail3D.h"

// コンストラクタ
Sword::Sword(void)
{
	player_ = nullptr;
	trail_ = nullptr;

	list.model_ = -1;
	list.pos_ = { 0.0f,0.0f,0.0f };
	list.rot_ = { 0.0f,0.0f,0.0f };
	list.scale_ = { 0.5f,0.5f,0.5f };
	list.rightHandIdx_ = -1;
	list.rightHandPos_ = { 0.0f,0.0f,0.0f };
}

// デストラクタ
Sword::~Sword(void)
{
	Release();
}

// 初期化
void Sword::Init(Player* player)
{
	player_ = player;

	list.model_ = MV1LoadModel("Data/Model/Sword.mv1");

	list.pos_ = VGet(0.0f, 0.0f, 0.0f);
	list.rot_ = VGet(0.0f, 0.0f, 0.0f);
	MV1SetPosition(list.model_, list.pos_);
	MV1SetRotationXYZ(list.model_, list.rot_);
	MV1SetScale(list.model_, list.scale_);

	MV1SetupCollInfo(list.model_);

	trail_ = new Trail3D();
	trail_->Init("Data/Image/SwordFlash.png");
}

// 更新処理
void Sword::Update(void)
{
	int playerModel = player_->GetPlayerModel();
	int rightHandIdx = player_->GetRightHandIndex();

	if (rightHandIdx != -1)
	{
		list.pos_ = MV1GetFramePosition(playerModel, rightHandIdx);
		MV1SetPosition(list.model_, list.pos_);
		// プレイヤーの右手ボーンのワールド回転行列を取得
		MATRIX handMat = MV1GetFrameLocalWorldMatrix(playerModel, rightHandIdx);

		// 剣のモデルにその行列をセット（位置はすでに設定済みなので回転だけ反映）
		MV1SetRotationMatrix(list.model_, handMat);
	}

	MV1SetScale(list.model_, list.scale_);
	MV1RefreshCollInfo(list.model_);

	// 剣身の複数点取得
	std::vector<VECTOR> bladePoints = GetBladePoints(2.0f, 0.5f, 0.1f);
	trail_->Update(bladePoints);
}

// 描画
void Sword::Draw(void)
{
	if (player_->GetIsWeapon()) {
		MV1DrawModel(list.model_);
		trail_->Draw();
	}
}

// 解放
void Sword::Release(void)
{
	if (list.model_ != -1) {
		MV1DeleteModel(list.model_);
		list.model_ = -1;
	}
	trail_->Release();
}

// 剣の先端から根元までを計算(先端、根元、間隔)
std::vector<VECTOR> Sword::GetBladePoints(float startY, float endY, float interval)const
{
	std::vector<VECTOR> points;

	// 剣のワールド行列取得
	MATRIX swordMat = MV1GetLocalWorldMatrix(list.model_);

	// 剣身のローカル座標上の複数点（例：根元〜先端を3分割）
	for (float y = startY; y >= endY; y -= interval)
	{
		VECTOR localPoint = VGet(0.0f, y, 0.0f);
		points.push_back(VTransform(localPoint, swordMat));
	}
	return points;
}