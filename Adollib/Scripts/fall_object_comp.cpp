
#include "fall_object_comp.h"

#include "../Adollib/Scripts/Main/systems.h"
#include "../Adollib/Scripts/Main/Adollib.h"
#include "../Adollib/Scripts/Imgui/debug.h"
#include "../Adollib/Scripts/Object/transform.h"
#include "../Adollib/Scripts/Object/gameobject.h"
#include "../Adollib/Scripts/Object/gameobject_manager.h"
#include "../Adollib/Scripts/Renderer/renderer_base.h"
#include "../Adollib/Scripts/Renderer/material_manager.h"
#include "../Adollib/Scripts/Renderer/texture.h"
#include "../Adollib/Scripts/Renderer/croth_renderer.h"
#include "../Adollib/Scripts/Physics/collider_croth.h"

#include "../Adollib/Imgui/imgui.h"

#include "../Adollib/Scripts/Physics/ALP__physics_manager.h"

using namespace Adollib;

// ��������V�[���̏��������Ɉ�x�����Ă΂��
void Fall_object_comp::awake()
{

}

void Fall_object_comp::start()
{

}

// ���t���[���Ă΂��X�V����
void Fall_object_comp::update()
{
	if (check_transform->position.y < delete_y) Gameobject_manager::deleteGameobject(gameobject, true);

}

// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��true�ɂȂ������Ă΂��
void Fall_object_comp::onEnable()
{

}

// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��false�ɂȂ������Ă΂��
void Fall_object_comp::onDisable()
{

}


