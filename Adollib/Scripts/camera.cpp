
#include "../Adollib/Scripts/Main/systems.h"
#include "../Adollib/Scripts/Main/Adollib.h"
#include "../Adollib/Scripts/Imgui/debug.h"
#include "../Adollib/Scripts/Object/transform.h"
#include "../Adollib/Scripts/Object/gameobject_manager.h"
#include "../Adollib/Scripts/Object/component_camera.h"
#include "../Adollib/Scripts/Renderer/directional_shadow.h"

#include "Camera.h"

#include "../Adollib/Imgui/imgui.h"

#include "../Adollib/Scripts/Object/component_camera.h"

#include "../Adollib/Scripts/Physics/ray.h"
#include "../Adollib/Scripts/Physics/ALP__physics_manager.h"

#include "input_changer.h"

namespace Adollib
{

	// ��������V�[���̏��������Ɉ�x�����Ă΂��
	void Camera::awake()
	{
		gameobject->transform->local_pos = Vector3(0, 30, -50);
		//input->setCursorPos(Al_Global::SCREEN_WIDTH * 0.5f, Al_Global::SCREEN_HEIGHT * 0.5f);


		follow_player = true;


	}

	void Camera::start()
	{
#ifndef ON_DEBUG
		input_changer->set_lockcursol(true);
#else
		ShowCursor(TRUE);
#endif
		auto c = gameobject->findComponent<Camera_component>();
		c->directional_shadow->position = Vector3(300, 320, -200);
		c->directional_shadow->direction = Vector3(-1, -2, 1).unit_vect();
		c->directional_shadow->nearZ = 150;
		c->directional_shadow->nearZ = 0.1f;
	}

	// ���t���[���Ă΂��X�V����
	void Camera::update()
	{
		//gameobject->aspect = (float)Al_Global::SCREEN_WIDTH / Al_Global::SCREEN_HEIGHT;

#ifdef UseImgui
		ImGuiWindowFlags flag = 0;
		//flag |= ImGuiWindowFlags_AlwaysAutoResize;
		flag |= ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize;
		flag |= ImGuiWindowFlags_::ImGuiWindowFlags_NoDocking;
		if (ImGui::Begin("camera_manager", 0, flag)) {

			ImGui::InputFloat("rotate_speed", &rotate_speed, 0.01f, 10.0f, "%.2f");
			ImGui::InputFloat("linear_speed", &linear_speed, 1.0f, 300.0f, "%.2f");
			ImGui::Checkbox("follow_player", &follow_player);
			if (follow_player) ImGui::DragFloat("dis", &dis, 0.1f);

			ImGui::End();
		}
#endif // UseImgui

		float timeStep = Al_Global::second_per_frame;

		if (follow_player == false) {

			Vector3 position = Vector3(0, 0, 0);
			Quaternion rotate = quaternion_identity();

			static bool set_carsol_stop = true;

			//�J�����̉�]
			if (input->getMouseState(Mouse::RBUTTON)) {
				//float rotate_pow = 70 * Al_Global::second_per_frame;
				float rotate_pow = rotate_speed;
				Vector3 rotate_vec = Vector3(0, 0, 0);
				rotate_vec.y = input_changer->cursol_move.y;
				rotate_vec.x = input_changer->cursol_move.x;

				rotate_vec.x = ALClamp(rotate_vec.x, max_rotate.x - rotate_vec_save.x, max_rotate.y - rotate_vec_save.x);
				rotate_vec_save += rotate_vec;

				rotate *= quaternion_axis_angle(Vector3(0, 1, 0), +rotate_vec.y);
				rotate *= quaternion_axis_angle(vector3_cross(Vector3(0, 1, 0), vector3_quatrotate(Vector3(0, 0, 1), camera_rot)).unit_vect(), +rotate_vec.x);

				transform->local_orient = camera_rot;
			}

			//�J�����̈ړ�
			{
				Vector3 move_vec = Vector3(0, 0, 0);
				float   move_pow = linear_speed * Al_Global::second_per_frame;
				if (input->getKeyState(Key::LeftShift)) move_pow = linear_speed * 2 * Al_Global::second_per_frame;
				if (input->getKeyState(Key::W))move_vec += Vector3(0, 0, +1);
				if (input->getKeyState(Key::S))move_vec += Vector3(0, 0, -1);
				if (input->getKeyState(Key::D))move_vec += Vector3(+1, 0, 0);
				if (input->getKeyState(Key::A))move_vec += Vector3(-1, 0, 0);

				//����Ȃ���]������
				Vector3 eu = camera_rot.euler();
				Quaternion y_axis_rotate = quaternion_from_euler(0, eu.y, 0);
				position += vector3_quatrotate(move_vec, y_axis_rotate).unit_vect() * move_pow;

				//y�������̈ړ�
				if (input->getKeyState(Key::Space))position += Vector3(0, 1, 0) * move_pow;
				if (input->getKeyState(Key::LeftControl))position += Vector3(0, -1, 0) * move_pow;
			}

			transform->local_pos += position;
			camera_rot *= rotate;

		}
		else {
			Quaternion rotate = quaternion_identity();

			//�J�����̉�]
			if (input_changer->get_lockcursol()) {
				//float rotate_pow = 70 * Al_Global::second_per_frame;
				float rotate_pow = rotate_speed;
				Vector3 rotate_vec = Vector3(0, 0, 0);
				rotate_vec.y = input_changer->cursol_move.y;
				rotate_vec.x = input_changer->cursol_move.x;

				// min,max�ɂ܂Ƃ߂�
				rotate_vec.x = ALClamp(rotate_vec.x, max_rotate.x - rotate_vec_save.x, max_rotate.y - rotate_vec_save.x);
				rotate_vec_save += rotate_vec;

				rotate *= quaternion_axis_angle(vector3_cross(Vector3(0, 1, 0), vector3_quatrotate(Vector3(0, 0, 1), camera_rot)).unit_vect(), +rotate_vec.x);
				rotate *= quaternion_axis_angle(Vector3(0, 1, 0), +rotate_vec.y);

				//camera_rot_goal *= rotate;
			}
			// easing
			{
				camera_rot_goal *= rotate;
				camera_rot = ALEasing(camera_rot, camera_rot_goal, 0.8f, timeStep);
			}

			// �ړ�����rad�ɒl�������Ă���
			//// �ړ����͍ő�p�x�𒲐�
			{
				transform->local_orient = camera_rot;
				if (input_changer->dir.norm() != 0)
				{
					rotate_min_pow_bufer += 0.5f * time->deltaTime();
					if (rotate_min_pow_bufer > 1)rotate_min_pow_bufer = 1;
				}
				else
				{
					rotate_min_pow_bufer -= 0.5f * time->deltaTime();
					if (rotate_min_pow_bufer < 0)rotate_min_pow_bufer = 0;
				}

				if (rotate_min_pow_bufer != 0)
				{
					const Vector3 dir = vector3_quatrotate(Vector3(0, 0, 1), camera_rot);
					const Vector3 right = vector3_cross(Vector3(0, 1, 0), vector3_quatrotate(Vector3(0, 0, 1), camera_rot));

					float tan_rad = vector3_radian(vector3_cross(right, Vector3(0, 1, 0)), dir);
					if (dir.y > 0)tan_rad *= -1;

					float tan_rad_cl = tan_rad;
					if (dir.y > 0) tan_rad_cl *= 1 + (rotate_min_pow_yp - 1) * rotate_min_pow_bufer;
					else           tan_rad_cl *= 1 + (rotate_min_pow_ym - 1) * rotate_min_pow_bufer;

					transform->local_orient *= quaternion_axis_radian(right, tan_rad_cl - tan_rad);

				}
			}

			// ��ʒ��S����player�����ꂷ�����璲��
			{
				Vector3 goal = player->position + Vector3(0, 4, 0);
				Vector3 dir = goal - pos_buffer;
				if (dir.norm() > pos_slop * pos_slop) {
					pos_buffer += dir.unit_vect() * (dir.norm_sqr() - pos_slop);
				}
			}

			//�J�����̋���
			{
				//wheel���狗���̒���
				float easing_pow = 0.1f;
				float easing_value = 10 * timeStep; //�C�[�W���O�ł̈ړ�����
				if (ray_timer > 0)easing_value = 0;
				ray_timer -= timeStep;

				// �z�C�[���������Ă���� ���݂̋�������wheel�œ��������Ƃ����dis�ɂ���
				if (input_changer->wheel_move != 0) {
					easing_value = 10000 * timeStep; // ���R�ɖ߂��葁������
					easing_pow = 0.8f;
					dis = dis_buffer;
					dis += -input_changer->wheel_move * 0.02f;

					if (dis_buffer < 10 && dis < dis_buffer)dis = dis_buffer;
					if (dis_buffer > 20 && dis > dis_buffer)dis = dis_buffer;

					if (dis_buffer == ALClamp(dis_buffer, 10, 50)) dis = ALClamp(dis, 10, 50);
				}
				else dis = ALClamp(dis, 10, 20); //�}�E�X�z�C�[���Œ����\�ȍŒ዗�� / �ő勗��

				//dis���C�[�W���O�̂��߂�dis_buffer�ɕۑ�
				const float next = ALEasing(dis_buffer, dis, easing_pow, timeStep);
				dis_buffer += ALClamp(next - dis_buffer, -easing_value, easing_value);

				//ray���΂���camera��stage�ɂ߂荞�܂Ȃ��悤��
				Ray ray;
				ray.position = pos_buffer;
				ray.direction = vector3_quatrotate(Vector3(0, 0, -1), transform->local_orient);
				Ray::Raycast_struct str;
				str.collider_tag = Collider_tags::Caera_not_sunk_Stage;

				const float sphere_radius = 1.0f;

				ray.sphere_cast(sphere_radius, str);

				// �J�����̌��ɕǂ������timer��set timer��>0�̎����R�Ɍ��̋����܂Ŗ߂�Ȃ�
				if (ray_timer > 0 && str.raymin - 1 < dis_buffer) ray_timer = 0.4f;
				//�J�������߂荞��ł�����ʒu�𒲐�
				if (str.raymin < dis_buffer) {
					easing_value = 5 * timeStep;
					dis_buffer = str.raymin;
					ray_timer = 0.4f;
					if (dis > 30)dis = 30; //�ő勗��
				}

			}
			transform->local_pos = pos_buffer + vector3_quatrotate(dis_buffer * Vector3(0, 0, -1), transform->local_orient);

		}

#ifdef ON_DEBUG
		if (input->getKeyTrigger(Key::RightControl)) {
			input_changer->set_lockcursol(!input_changer->get_lockcursol());
		}
#endif

		timeStep = 0;
	}

	// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��true�ɂȂ������Ă΂��
	void Camera::onEnable()
	{

	}

	// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��false�ɂȂ������Ă΂��
	void Camera::onDisable()
	{

	}

}
