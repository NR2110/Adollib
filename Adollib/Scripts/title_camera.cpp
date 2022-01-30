
#include "title_camera.h"

#include "../Adollib/Scripts/Main/systems.h"
#include "../Adollib/Scripts/Main/Adollib.h"
#include "../Adollib/Scripts/Imgui/debug.h"
#include "../Adollib/Scripts/Object/transform.h"
#include "../Adollib/Scripts/Object/gameobject_manager.h"
#include "../Adollib/Scripts/Object/component_camera.h"
#include "../Adollib/Scripts/Renderer/directional_shadow.h"

#include "../Adollib/Imgui/imgui.h"

#include "../Adollib/Scripts/Object/component_camera.h"

#include "../Adollib/Scripts/Physics/ray.h"
#include "../Adollib/Scripts/Physics/ALP__physics_manager.h"

#include "title_state_manager.h"

using namespace Adollib;


// ��������V�[���̏��������Ɉ�x�����Ă΂��
void Title_camera::awake()
{
	gameobject->transform->local_pos = Vector3(0, 30, -50);
	//input->setCursorPos(Al_Global::SCREEN_WIDTH * 0.5f, Al_Global::SCREEN_HEIGHT * 0.5f);
}

void Title_camera::start()
{
	title_state_manager = Gameobject_manager::find("title_state_manager")->findComponent<Title_state_manager>();
	title_state_manager->add_statebase(this);

	auto c = gameobject->findComponent<Camera_component>();
	c->directional_shadow->position = Vector3(200, 200, -200);
	c->directional_shadow->direction = Vector3(-1, -2, 1).unit_vect();
	c->directional_shadow->nearZ = 150;
	c->directional_shadow->nearZ = 0.1f;
}

// ���t���[���Ă΂��X�V����
void Title_camera::update()
{

#ifdef UseImgui
	ImGuiWindowFlags flag = 0;
	//flag |= ImGuiWindowFlags_AlwaysAutoResize;
	flag |= ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize;
	flag |= ImGuiWindowFlags_::ImGuiWindowFlags_NoDocking;
	if (ImGui::Begin("camera_manager", 0, flag)) {

		ImGui::InputFloat("rotate_speed", &rotate_speed, 0.01f, 10.0f, "%.2f");
		ImGui::InputFloat("linear_speed", &linear_speed, 1.0f, 300.0f, "%.2f");

		ImGui::End();
	}
#endif // UseImgui

	float timeStep = Al_Global::second_per_frame;

	// debug�p �J���������R�ɓ�����
#ifdef ON_DEBUG
	if (0 && input->getMouseState(Mouse::RBUTTON)) {
		Vector3 position = Vector3(0, 0, 0);
		Quaternion rotate = quaternion_identity();

		static bool set_carsol_stop = true;

		//�J�����̉�]
			//float rotate_pow = 70 * Al_Global::second_per_frame;
		float rotate_pow = rotate_speed;
		Vector3 rotate_vec = Vector3(0, 0, 0);
		rotate_vec.y = input->getCursorPosX() - cursol_pos_save.x;
		rotate_vec.x = input->getCursorPosY() - cursol_pos_save.y;


		rotate_vec.x = ALClamp(rotate_vec.x, max_rotate.x - rotate_vec_save.x, max_rotate.y - rotate_vec_save.x);
		rotate_vec_save += rotate_vec;

		rotate *= quaternion_axis_angle(Vector3(0, 1, 0), +rotate_vec.y);
		rotate *= quaternion_axis_angle(vector3_cross(Vector3(0, 1, 0), vector3_quatrotate(Vector3(0, 0, 1), camera_rot)).unit_vect(), +rotate_vec.x);

		transform->local_orient = camera_rot;

		cursol_pos_save.x = input->getCursorPosX();
		cursol_pos_save.y = input->getCursorPosY();

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
#endif
}

// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��true�ɂȂ������Ă΂��
void Title_camera::onEnable()
{

}

// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��false�ɂȂ������Ă΂��
void Title_camera::onDisable()
{

}

void Title_camera::start_state(Title_state_B state) {

	if (input->getKeyTrigger(Key::Enter) || input->getPadTrigger(0, GamePad::A)) title_state_manager->set_nextstate_A(Title_state_A::Select_stage);

	float eular_x = input->getRStickY(0) * -1.f;
	float eular_y = input->getRStickX(0) * +1.f;

	camera_eular.x = ALEasing(camera_eular.x, eular_x, 0.1f, time->deltaTime());
	camera_eular.y = ALEasing(camera_eular.y, eular_y, 0.1f, time->deltaTime());
	transform->local_orient = quaternion_identity() * quaternion_from_euler(camera_eular);
};

Vector3 debug_easing(Vector3 start, Vector3 goal, float speed, float delta_time) {
	float dd = pow(1 - speed, delta_time * 60);
	return goal + (start - goal) * pow(1 - speed, delta_time * 60);
}

void Title_camera::select_state(Title_state_B state) {

	float back_dis = -10;
	float front_dis = 110;
	float y_dis = 6;

	float t_x_off = 0;//�񎟊֐���x
	float pos_q_func = 0;//���W�v�Z�Ɏg���K���ȓ񎟊֐��̓���
	float rot_q_func = 0;//��]�v�Z�Ɏg���K���ȓ񎟊֐��̓���

	camera_eular.x = ALEasing(camera_eular.x, 0, 0.1f, time->deltaTime());
	camera_eular.y = ALEasing(camera_eular.y, 0, 0.1f, time->deltaTime());

	if (state == Adollib::Title_state_B::Awake) {
		player = nullptr;
		//transform->local_orient = quaternion_identity();
		transform->local_orient = quaternion_identity() * quaternion_from_euler(camera_eular);
	}

	// �����
	if (state == Adollib::Title_state_B::Update_0) {

		float t = title_state_manager->get_timer_stateB() * 3;
		if (t > 1) {
			title_state_manager->set_nextstate_B(Title_state_B::Update_1);
			return;
		}
		t_x_off = t - 1;
		pos_q_func = (-1 * t_x_off * t_x_off + 1); //(0<y<1)

		transform->local_pos = start_pos + (Vector3(0, 0, 1) * back_dis) * pos_q_func; //z�����̒���
		transform->local_pos.y = start_pos.y + y_dis * pos_q_func; //title��floar�̌������������̂��C���������̂� �����Ƃ����W�������グ��

		transform->local_orient = quaternion_identity() * quaternion_from_euler(camera_eular);
	}
	// �߂Â�
	if (state == Adollib::Title_state_B::Update_1) {

		float t = title_state_manager->get_timer_stateB() * 2.5f;
		if (t > 1) {
			title_state_manager->set_nextstate_B(Title_state_B::Update_2);
			return;
		}

		// ���W����
		{
			float c_time = 0.5f;
			// �������W��Update_0�I�����_�̍��W�Ƃ���
			Vector3 s_pos = start_pos + (Vector3(0, 0, 1) * back_dis);
			if (t < c_time) {
				// �������Ȃ���
				t_x_off = t * 1 / c_time;
				pos_q_func = (+1 * t_x_off * t_x_off + 0); //(0<q<1)
				pos_q_func = pow(pos_q_func, 1.1f) * c_time; //(0<q<c_time)
				transform->local_pos = s_pos + (Vector3(0, 0, 1) * (-back_dis + front_dis)) * pos_q_func;
			}
			else {
				// �������Ȃ���
				t_x_off = (t - 1) * 1 / (1 - c_time);
				pos_q_func = (-1 * t_x_off * t_x_off + 1) * (1 - c_time) + c_time; //(0.5<y<1)
				transform->local_pos = s_pos + (Vector3(0, 0, 1) * (-back_dis + front_dis)) * pos_q_func;
			}
			// y����
			transform->local_pos.y = start_pos.y + y_dis * (1 - pos_q_func);
		}

		// ��]����
		{
			float c_time = .55f;
			if (t >= c_time) {
				//t_x_off = (t - c_time) * 1 / (1 - c_time);
				//t_x_off *= t_x_off;
				//Debug::set("t_x_off", t_x_off);
				//rot_q_func = (1 * t_x_off * t_x_off); //(0<y<1)
				t_x_off = (t - c_time) * 1 / (1 - c_time) - 1;
				Debug::set("t_x_off", t_x_off);
				rot_q_func = (-1 * t_x_off * t_x_off + 1); //(0<y<1)
				rot_q_func *= rot_q_func;
				transform->local_orient = quaternion_from_euler(Vector3(0, 9.5f, 0) * rot_q_func) * quaternion_from_euler(camera_eular);
			}
		}


	}
	// stage_select
	if (state == Adollib::Title_state_B::Update_2) {
		float eular_x = input->getRStickY(0) * -1.f;
		float eular_y = input->getRStickX(0) * +1.f;
		camera_eular.x = ALEasing(camera_eular.x, eular_x, 0.1f, time->deltaTime());
		camera_eular.y = ALEasing(camera_eular.y, eular_y, 0.1f, time->deltaTime());
		transform->local_orient = quaternion_from_euler(Vector3(0, 9.5f, 0) * 1) * quaternion_from_euler(camera_eular);
	}

	static float dis_buffer = 0;
	static Vector3 pos_buffer = Vector3(0);
	static float pos_slop = 1;
	if (state == Adollib::Title_state_B::Update_3) {

		if (player == nullptr)
			player = Gameobject_manager::find("Waist", Scenelist::scene_player)->transform;

		if (player->position.y < -10) {
			title_state_manager->set_nextstate_B(Title_state_B::Update_4);
			dis_buffer = vector3_dot(vector3_quatrotate(Vector3(0, 0, 1), transform->orientation), player->position - transform->position);
			pos_buffer = transform->position - vector3_quatrotate(dis_buffer * Vector3(0, 0, -1), transform->local_orient);
		}
	}

	if (state == Adollib::Title_state_B::Update_4) {

		Quaternion rotate = quaternion_identity();

		// ��ʒ��S����player�����ꂷ�����璲��
		{
			Vector3 goal = player->position + Vector3(0, 4, 0);
			Vector3 dir = goal - pos_buffer;
			if (dir.norm() > pos_slop * pos_slop) {
				float dis = ALmin(10 * time->deltaTime(), dir.norm_sqr() - pos_slop);
				pos_buffer += dir.unit_vect() * dis;
			}
		}
		//�J�����̋���
		{
			auto timestep = time->deltaTime();
			//wheel���狗���̒���
			float easing_pow = 0.1f;
			float easing_value = 10 * timestep; //�C�[�W���O�ł̈ړ�����

			float dis = ALClamp(dis_buffer, 10, 20); //�}�E�X�z�C�[���Œ����\�ȍŒ዗�� / �ő勗��

			//dis���C�[�W���O�̂��߂�dis_buffer�ɕۑ�
			const float next = ALEasing(dis_buffer, dis, easing_pow, timestep);
			dis_buffer += ALClamp(next - dis_buffer, -easing_value, easing_value);
		}

		transform->local_pos = pos_buffer + vector3_quatrotate(dis_buffer * Vector3(0, 0, -1), transform->local_orient);

		if (transform->local_pos.y < -70) {
			Scene_manager::set_active(Scenelist::scene_game);
			Scene_manager::set_inactive(Scenelist::scene_title);
		}
	}
};