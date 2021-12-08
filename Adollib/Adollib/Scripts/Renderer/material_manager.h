#pragma once

#include <vector>
#include <string>
#include <list>
#include <memory>
#include "../Math/math.h"

namespace Adollib {
	class Material;

	class Material_manager {
	private:
		static std::list<std::shared_ptr<Material>> materials;

	public:
		static int get_materials_size() { return materials.size(); };

		static std::shared_ptr<Material> make_material(const std::string& name = "");

		static std::shared_ptr<Material> find_material(const std::string& name);

	};

}