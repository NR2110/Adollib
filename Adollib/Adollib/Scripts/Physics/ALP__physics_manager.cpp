#include "ALP__physics_function.h"
#include "ALP__physics_manager.h"
#include "../Scene/scene.h"
#include "../Main/Adollib.h"

#include "../Object/gameobject_manager.h"
#include "../Imgui/work_meter.h"

#include "collider.h"

using namespace Adollib;
using namespace physics_function;
using namespace Contacts;

//::: static�����o�̏����� :::::
namespace Adollib
{
	float Phyisics_manager::gravity = Physics_manager_default::gravity; //�d��

	physics_function::ALP_Physics Phyisics_manager::default_physics = ALP_Physics(
		Physics_manager_default::inertial_mass, //����
		Physics_manager_default::drag, //��C��R
		Physics_manager_default::anglar_drag, //��C��R
		Physics_manager_default::dynamic_friction,//�����C
		Physics_manager_default::static_friction, //�Ö��C
		Physics_manager_default::restitution,	 //�����W��

		Physics_manager_default::is_fallable, //�����Ȃ�
		Physics_manager_default::is_kinematic, //�e�������Ȃ�(fall�͂���)
		Physics_manager_default::is_moveable, //�����Ȃ�
		Physics_manager_default::is_hitable //�Փ˂��Ȃ�
	); //�����ݒ肵�Ă��Ȃ��Ƃ���pyisics�̒l

	float Phyisics_manager::bounce_threshold = Physics_manager_default::bounce_threshold; //���˕Ԃ��臒l
	float Phyisics_manager::sleep_threshold = Physics_manager_default::sleep_threshold; //sleep��臒l

	float Phyisics_manager::contact_allowable_error = Physics_manager_default::contact_allowable_error; //����ȏ�߂��Ɠ���Փ˓_�Ƃ���
	float Phyisics_manager::contact_threrhold_normal = Physics_manager_default::contact_threrhold_normal; //�Փ˓_��臒l
	float Phyisics_manager::contact_threrhold_tangent = Physics_manager_default::contact_threrhold_tangent;//�Փ˓_��臒l

	float Phyisics_manager::bias = Physics_manager_default::bias;//�߂荞�݂𒼂���
	float Phyisics_manager::slop = Physics_manager_default::slop;//�Փ˂̋��e�l

	int Phyisics_manager::solver_iterations = Physics_manager_default::solver_iterations; //�Փ˂̐��x
	bool Phyisics_manager::hit_backfaces_flag = Physics_manager_default::hit_backfaces_flag;//mesh�̌�납��Փ˂��邩

	float Phyisics_manager::timeStep = Physics_manager_default::timeStep;
}

namespace Adollib
{
	int Phyisics_manager::frame_count;

	//collider_component�̃|�C���^�z��
	std::unordered_map<Scenelist, std::list<Collider*>> Phyisics_manager::colliders;

	//�edata�̎��Ԕz��
	std::unordered_map<Scenelist, std::list<physics_function::ALP_Collider>> Phyisics_manager::ALP_colliders;
	std::unordered_map<Scenelist, std::list<physics_function::ALP_Physics>> Phyisics_manager::ALP_physicses;

	std::vector<physics_function::Contacts::Contact_pair> Phyisics_manager::pairs;
	std::vector<physics_function::Contacts::Collider_2> Phyisics_manager::broad_mid_pair;

	bool Phyisics_manager::is_changed_coll = false; //�V����Collider���ǉ����ꂽ�A�폜���ꂽ�ꍇtrue
}

namespace Adollib
{

	bool Phyisics_manager::update(Scenelist Sce)
	{

		if (frame_count < 10) {
			resetforce(ALP_physicses[Sce]);
			frame_count++;
			return true;
		}


		Work_meter::start("refresh_ALP_from_data");
		// get_data��data��Collider�ɓ��� 
		refresh_ALP_from_data(ALP_colliders[Sce]);	
		Work_meter::stop("refresh_ALP_from_data");


		// Collider��World���̍X�V
		update_world_trans(ALP_colliders[Sce]);

		// �O�͂̍X�V
		applyexternalforce(ALP_physicses[Sce]);

		timeStep = Al_Global::second_per_frame;

		// ��G�c�ȓ����蔻��
		Work_meter::start("Broad,Mid,Narrow");

		Work_meter::start("Broadphase");
		Work_meter::tag_start("Broadphase");
		Broadphase(ALP_physicses[Sce], broad_mid_pair);
		is_changed_coll = false;
		Work_meter::tag_stop();
		Work_meter::stop("Broadphase");

		Work_meter::start("Midphase");
		Work_meter::tag_start("Midphase");
		Midphase(broad_mid_pair, pairs);
		Work_meter::tag_stop();
		Work_meter::stop("Midphase");

		// �Փː���
		Work_meter::start("Narrowphase");
		Work_meter::tag_start("Narrowphase");
		generate_contact(pairs);
		Work_meter::tag_stop();
		Work_meter::stop("Narrowphase");

		Work_meter::stop("Broad,Mid,Narrow");

		// �Փˉ���
		Work_meter::start("Resolve");
		Work_meter::tag_start("Resolve");
		resolve_contact(ALP_colliders[Sce], pairs);
		Work_meter::tag_stop();
		Work_meter::stop("Resolve");

		// �ʒu�̍X�V
		integrate(ALP_physicses[Sce]);

		// GO��Collider�̉e����n��
		solv_resolve(ALP_colliders[Sce]);
		resolve_gameobject(ALP_colliders[Sce]);

		return true;

	}


	void Phyisics_manager::destroy(Scenelist Sce) {
		colliders[Sce].clear();
	}
}


namespace Adollib {






}