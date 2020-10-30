
#include "../Adollib/Scripts/Object/gameobject_manager.h"

#include "camera_manager.h"
#include <DirectXMath.h>

#include "camera_s.h"
#include "../Adollib/Scripts/Main/Adollib.h"


namespace Adollib
{
	// 所属するシーンの初期化時に一度だけ呼ばれる
	void camera_manager_s::awake() {
			set_camera(Vector3(0, 30, -50), Vector3(0, 0, 1));
			Al_Global::render_collider_flag = false;
	}

	void camera_manager_s::start()
	{
		//	set_camera(Vector3(0, 0, 20), Vector3(0, 0, 1));

	}

	// 毎フレーム呼ばれる更新処理
	void camera_manager_s::update()
	{
		if (input->getKeyTrigger(Key::C))
			Al_Global::render_collider_flag = Al_Global::render_collider_flag == false ? true : false;

		Vector3 veA(2, 9, 5);
		Vector3 veB(3, 5, 7);
		DirectX::XMFLOAT3 XMA (2, 9, 5);
		DirectX::XMFLOAT3 XMB (3, 5, 7);
		DirectX::XMVECTOR VEA = DirectX::XMLoadFloat3(&XMA);
		DirectX::XMVECTOR VEB = DirectX::XMLoadFloat3(&XMB);
		DirectX::XMFLOAT3 XMCross;

		DirectX::XMStoreFloat3(&XMCross, DirectX::XMVector3Cross(VEA, VEB));
		Vector3 vecCross = vector3_cross(veA, veB);


		matrix ma = quaternion_from_euler(32, 65, 8).get_rotate_matrix();
		DirectX::XMMATRIX XMmat = ma.get_XMMATRIX();

		DirectX::XMStoreFloat3(&XMA, DirectX::XMVector3Transform(VEA, XMmat));
		veA = vector3_trans(veA, ma);

		int adfsgdhfjgkh = 0;


		//quaternion A = quaternion_from_euler(180, 0, 0);

		//Vector3 B = A.euler();

		//if (10 < fabsf(B.z)) {
		//	int dafsgdf = 0;
		//}
		//if (10 < fabsf(B.x)) {
		//	int dafsgdf = 0;
		//}
		//quaternion C = quaternion_from_euler(B);

		//quaternion X = quaternion_angle_axis(45, Vector3(1, 0, 0));
		//quaternion Y = quaternion_angle_axis(45, Vector3(0, 1, 0));
		//quaternion Z = quaternion_angle_axis(45, Vector3(0, 0, 1));
		//quaternion Z_X_Y = Z * X * Y;
		//Vector3 Ea = Q.euler();

		//Vector3 n = Vector3(0, 0, 1);
		//Vector3 Va = vector3_be_rotated_by_quaternion(n, Q);

		//matrix M = Q.get_rotate_matrix();
		//Vector3 Vb = vector3_trans(n, M);
		//M._41 = 100;
		//M._42 = 50;
		//M._43 = 80;

		//matrix Mi = matrix_inverse(M);

		//Vector3 Vc = vector3_trans(n, Mi);
		//Vector3 Vd = vector3_trans(Vc, M);


		int fff = 0;

		/*quaternion A = quaternion_by_euler(45, 30, -40);
		DirectX::XMVECTOR Q = DirectX::XMQuaternionRotationRollPitchYaw(ToRadian(45), ToRadian(30), ToRadian(-40));
		DirectX::XMFLOAT3 AA;
		DirectX::XMStoreFloat3(&AA, Q);


		Vector3 V = Vector3(0, 0, 1);
		V = vector3_be_rotated_by_quaternion(V, A);

		Vector3 B = A.euler();

		DirectX::XMFLOAT3 XF = DirectX::XMFLOAT3(0, 0, 1);
		DirectX::XMVECTOR XV;
		XV = DirectX::XMLoadFloat3(&XF);
		DirectX::XMMATRIX XM = matrix_to_XMMATRIX(A.get_rotate_matrix());

		DirectX::XMVECTOR XV1 = DirectX::XMVector3Transform(XV, XM);
		DirectX::XMFLOAT3 XFR1;
		DirectX::XMStoreFloat3(&XFR1, XV1);

		XV = DirectX::XMLoadFloat3(&XF);
		XM = DirectX::XMMatrixRotationZ(ToRadian(-40)) * DirectX::XMMatrixRotationX(ToRadian(45)) * DirectX::XMMatrixRotationY(ToRadian(30));
		DirectX::XMVECTOR XV2 = DirectX::XMVector3Transform(XV, XM);
		DirectX::XMFLOAT3 XFR2;
		DirectX::XMStoreFloat3(&XFR2, XV2);

		float AAA = 0;*/
	}

	// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
	void camera_manager_s::onEnable()
	{

	}

	// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
	void camera_manager_s::onDisable()
	{

	}

}


namespace Adollib {
	void camera_manager_s::set_camera(Vector3 pos, Vector3 look_pos) {
		Camera* camera = nullptr;
		camera = Gameobject_manager::create_camera("camera");
		camera->addComponent<camera_s>();

		camera->transform->local_pos = pos;
		camera->transform->local_orient = quaternion_look_at(pos, look_pos);

		//camera->transform->local_position = pos.get_XM3();
		//camera->transform->local_rotation = quaternion_look_at(pos, Vector3(0, 0, 0)).euler().get_XM3();
		//camera->transform->position = pos.get_XM3();
		//camera->transform->position = DirectX::XMFLOAT3(0, 0, 0);

	}
}