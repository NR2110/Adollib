//#pragma once
//
//#include "../Adollib/Scripts/Object/component.h"
//#include <vector>
//#include <memory>
//
//namespace Adollib {
//	class Collider_Croth;
//	class Collider_croth_comp : public Component {
//
//	private:
//		std::shared_ptr<Collider_Croth> coll = nullptr;
//
//		std::shared_ptr<std::vector<std::vector<std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3>>>> vertex_offset;
//
//	public:
//		std::shared_ptr<Collider_Croth> get_collider() { return coll; };
//
//	public:
//
//		void awake() override;
//		void update() override;
//		void finalize() override;
//		void Update_hierarchy() override;
//
//	};
//
//}