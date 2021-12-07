#pragma once
#include "../Adollib/Scripts/Object/component.h"
#include "../Adollib/Scripts/Physics/collider.h"
#include "../Adollib/Scripts/Physics/joint.h"
#include "../Adollib/Scripts/Physics/ray.h"

namespace Adollib
{

	class Stage_manager;
	class Camera;
	class Input_changer;

	class Player : public Component
	{
	private:
		//playerの基本の値 決まったらconstにする
		float waist_move_max_speed = 1000;
		float waist_move_pow = 1000;

		float head_rot_max_speed = 1000;
		float head_rot_max_pow = 1000;
		float head_rot_pow = 1000;

		float waist_rot_max_speed = 1000;
		float waist_rot_max_pow = 1000;
		float waist_rot_pow = 1000;

		float body_rot_max_speed = 1000;
		float body_rot_max_pow = 1000;
		float body_rot_pow = 1000;

		//歩くときの足の力の強さ
		float leg_rot_max_speed = 1000;
		float leg_rot_max_pow = 1000;
		float leg_rot_pow = 100;

		//手を伸ばすときの力の強さ
		float hand_rot_max_speed = 1000;
		float hand_rot_max_pow = 1000;
		float hand_rot_pow = 100;

		//カメラ回転に応じて手を上げ下げする強さ
		float hand_camera_max_rot = ToRadian(45);
		float hand_camera_rot_pow = 2;
		float hand_camera_rot_center = ToRadian(30);

		float jump_y_power = 150;
		float jump_front_power = 150;

		float turn_speed = 200;

	private:
		Quaternion rotate; //rotateのbuffer
		Vector3 dir; //向きのbuffer

		// 力関係
		bool is_gunyatto = false; //trueの時、体がぐにゃっとする
		float gunyatto_pow = 1; //体を起こす力に掛ける値
		float hand_gunyatto_pow = 1; //腕のつかみなどの情報から求たgunyatto_powへのbuffer

		// つかみ関係
		Joint_base* catch_right_joint = nullptr; //右手がつかんでいるjoint
		Joint_base* catch_left_joint = nullptr;	 //左手がつかんでいるjoint
		bool is_maked_right_joint = false; //つかんでいたものが距離で離れたとき、stateのみだと即座に次のものをつかんでしまう 嫌なのでboolで管理
		bool is_maked_left_joint = false;
		BallJoint* Rhand_joint = nullptr; //elbowとhandをつなぐjoint
		BallJoint* Lhand_joint = nullptr; //elbowとhandをつなぐjoint
		float Rhand_joint_ylength_default = 0; //つかんでいるときhandのjointのanchorを伸ばすため 初期値を保存
		float Lhand_joint_ylength_default = 0;

		// 接地判定を行う(sphere_cast)
		Collider* onground_collider = nullptr; //立っているcollider情報
		Ray::Raycast_struct onground_ray_data; //ongroundで使ったdata rayminなど入っている
		Vector3 onground_contactpoint; //ongroundの衝突点
		const float onground_dis = 2.0f; //rayminがこれ以下なら立っている
		const float onground_radius = 0.6f; //sphere_castの半径

		// jump
		bool is_jumping = false; //今ジャンプしているか
		float coyote = 0.3f; //jumpの許容時間

		// 足
		float move_timer = 0; //足の回転を求めるために 入力時間を保存

		// respown関係
		Collider* check_onplayer_coll = nullptr; //respownの接地判定を行うCollider
		float respown_timer = 0; //respown処理用のtimer >0の時 check_respown()でPlayer::updateをreturn (入力を受け付けない、gunyattoする)

	private:
		Stage_manager* stage_manager = nullptr; //stage_managerへのポインタ
		Camera* camera = nullptr; //cameraへのポインタ
		Input_changer* input_changer = nullptr; //自身の保持するinput_changerへのポインタ

	private:
		//::: 毎フレーム呼び出す
		bool check_respown(); //respown処理
		void update_onground(); //onground_colliderを更新
		void update_gnyat_pow(); //体を起こす力の更新
		void reach_out_hands(); //手を伸ばす
		void catch_things(); //物をつかむ
		void push_waist_for_stand(); //rayを飛ばして腰を立たせる
		void linear_move(); //移動
		void angula_move(); //回転
		void accume_move_dir(); //移動方向を計算
		void add_pow_for_stand(); //立つように力を加える
		void move_legs(); //足を動かす
		void make_jump(); //jumpさせる
		//:::

		void turn_gunyatto_dir();
		void set_default_drag();

	public:
		void delete_catchjoint(); //"物を持つ"jointを削除する
		void respown(); //respown処理


	private:
		//::: GO :::
		static constexpr int Human_gameobject_size = 13;
		union {
			Gameobject* Human_gameobjects[Human_gameobject_size] = { nullptr };
			struct {
				Gameobject* Head    ;
				Gameobject* Lsholder;
				Gameobject* Lelbow  ;
				Gameobject* Lhand   ;
				Gameobject* Rsholder;
				Gameobject* Relbow  ;
				Gameobject* Rhand	;
				Gameobject* Body    ;
				Gameobject* Waist   ;
				Gameobject* Rleg    ;
				Gameobject* Rfoot   ;
				Gameobject* Lleg    ;
				Gameobject* Lfoot   ;
			};
		};

		//::: collider :::
		static constexpr int Human_collider_size = 13;
		union {
			Collider* Human_colliders[Human_collider_size] = { nullptr };
			struct {
				Collider* Head_collider;
				Collider* Lsholder_collider;
				Collider* Lelbow_collider;
				Collider* Lhand_collider;
				Collider* Rsholder_collider;
				Collider* Relbow_collider;
				Collider* Rhand_collider;
				Collider* Body_collider;
				Collider* Waist_collider;
				Collider* Rleg_collider;
				Collider* Rfoot_collider;
				Collider* Lleg_collider;
				Collider* Lfoot_collider;
			};
		};

		//::: dafault_drag :::
		union {
			float Human_default_drags[Human_collider_size] = { 0 };
		};

	public:
		void set_player_colliders(
		Collider* l_Head_collider		,
		Collider* l_Lsholder_collider	,
		Collider* l_Lelbow_collider		,
		Collider* l_Lhand_collider		,
		Collider* l_Rsholder_collider	,
		Collider* l_Relbow_collider		,
		Collider* l_Rhand_collider		,
		Collider* l_Body_collider		,
		Collider* l_Waist_collider		,
		Collider* l_Rleg_collider		,
		Collider* l_Rfoot_collider		,
		Collider* l_Lleg_collider		,
		Collider* l_Lfoot_collider
		) {
			Head_collider		= l_Head_collider;
			Lsholder_collider	= l_Lsholder_collider;
			Lelbow_collider		= l_Lelbow_collider;
			Lhand_collider		= l_Lhand_collider;
			Rsholder_collider	= l_Rsholder_collider;
			Relbow_collider		= l_Relbow_collider;
			Rhand_collider		= l_Rhand_collider;
			Body_collider		= l_Body_collider;
			Waist_collider		= l_Waist_collider;
			Rleg_collider		= l_Rleg_collider;
			Rfoot_collider		= l_Rfoot_collider;
			Lleg_collider		= l_Lleg_collider;
			Lfoot_collider		= l_Lfoot_collider;

			Head		= Head_collider		->gameobject;
			Lsholder	= Lsholder_collider	->gameobject;
			Lelbow		= Lelbow_collider	->gameobject;
			Lhand		= Lhand_collider	->gameobject;
			Rsholder	= Rsholder_collider	->gameobject;
			Relbow		= Relbow_collider	->gameobject;
			Rhand		= Rhand_collider	->gameobject;
			Body		= Body_collider		->gameobject;
			Waist		= Waist_collider	->gameobject;
			Rleg		= Rleg_collider		->gameobject;
			Rfoot		= Rfoot_collider	->gameobject;
			Lleg		= Lleg_collider		->gameobject;
			Lfoot		= Lfoot_collider	->gameobject;

			for (int i = 0; i < Human_collider_size; i++) {
				Human_default_drags[i] = Human_colliders[i]->physics_data.drag;
			}
		}

		void set_player_joints(
			BallJoint* l_Lhand_joint,
			BallJoint* l_Rhand_joint
		) {
			Lhand_joint = l_Lhand_joint;
			Rhand_joint = l_Rhand_joint;
		}

	public:
		void awake() override;

		// 所属するシーンの初期化時に一度だけ呼ばれる
		void start()override;

		// 毎フレーム呼ばれる更新処理
		void update()override;

		void Update_hierarchy() override;

		// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
		void onEnable()override;

		// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
		void onDisable()override;

	};

}