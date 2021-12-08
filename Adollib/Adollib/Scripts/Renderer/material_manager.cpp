#include "material_manager.h"

#include "material.h"

using namespace Adollib;

std::list<std::shared_ptr<Material>> Material_manager::materials;

std::shared_ptr<Material> Material_manager::make_material(const std::string& name) {

	if(name != "")
		for (auto& mat : materials) {
			if (mat->name == name) return mat;
		}

	auto material = std::make_shared<Material>();
	material->name = name;
	material->Load_VS("./DefaultShader/default_vs.cso");
	material->Load_PS("./DefaultShader/default_ps.cso");

	materials.emplace_back(material);

	return material;
};

std::shared_ptr<Material> Material_manager::find_material(const std::string& name){
	for (auto& mat : materials) {
		if (mat->name == name)return mat;
	}

	assert(0 && "this name material is no exist");
};