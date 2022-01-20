
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

// 所属するシーンの初期化時に一度だけ呼ばれる
void Fall_object_comp::awake()
{

}

void Fall_object_comp::start()
{

}

// 毎フレーム呼ばれる更新処理
void Fall_object_comp::update()
{
	if (check_transform->position.y < delete_y) Gameobject_manager::deleteGameobject(gameobject, true);

}

// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
void Fall_object_comp::onEnable()
{

}

// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
void Fall_object_comp::onDisable()
{

}


