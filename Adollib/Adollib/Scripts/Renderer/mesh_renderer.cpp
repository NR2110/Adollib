
#include "mesh_renderer.h"

#include "../Object/gameobject.h"
#include "renderer_manager.h"

#include "../Main/systems.h"
#include "Shader/constant_buffer.h"
#include "Shader/vertex_format.h"

#include "material.h"
#include "texture.h"

using namespace Adollib;
using namespace ConstantBuffer;

void Mesh_renderer::render(const Frustum_data& frustum_data) {
	if (material == nullptr) return;

	Systems::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// CB : ConstantBufferPerOBJ
	// GOのtransformの情報をConstantBufferへセットする
	ConstantBufferPerGO g_cb;
	g_cb.world = matrix_world(transform->scale, transform->orientation.get_rotate_matrix(), transform->position);
	Systems::DeviceContext->UpdateSubresource(world_cb.Get(), 0, NULL, &g_cb, 0, 0);
	Systems::DeviceContext->VSSetConstantBuffers(0, 1, world_cb.GetAddressOf());
	Systems::DeviceContext->PSSetConstantBuffers(0, 1, world_cb.GetAddressOf());

	// shaderのactivate
	material->shader_activate();

	//
	if (Systems::BS_type != material->BS_state) Systems::SetBlendState(material->BS_state);
	if (Systems::RS_type != material->RS_state) Systems::SetRasterizerState(material->RS_state);
	if (Systems::DS_type != material->DS_state) Systems::SetDephtStencilState(material->DS_state);

	for (Mesh::mesh& mesh : (*meshes))
	{
		//if (FrustumCulling::frustum_culling(&mesh) == false) continue;

		UINT stride = sizeof(VertexFormat);
		UINT offset = 0;
		Systems::DeviceContext->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		Systems::DeviceContext->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);


		//CB : ConstantBufferPerMaterial
		ConstantBufferPerMaterial cb;
		for (int i = 0; i < MAX_BONES; i++) {
			cb.boneTransforms[i] = matrix44_identity();
		}
		{
			// boneTransform
			if (mesh.skeletalAnimation.size() > 0 && mesh.skeletalAnimation[animeIndex].size() > 0)
			{
				int frame = (int)(mesh.skeletalAnimation.at(animeIndex).animation_tick / mesh.skeletalAnimation.at(animeIndex).sampling_time);
				if (frame > (int)mesh.skeletalAnimation.at(animeIndex).size() - 1)
				{
					frame = 0;
					mesh.skeletalAnimation.at(animeIndex).animation_tick = 0;
				}
				std::vector<Mesh::bone>& skeletal = mesh.skeletalAnimation.at(animeIndex).at(frame);
				size_t number_of_bones = skeletal.size();
				_ASSERT_EXPR(number_of_bones < MAX_BONES, L"'the number_of_bones' exceeds MAX_BONES.");
				for (size_t i = 0; i < number_of_bones; i++)
				{
					XMStoreFloat4x4(&cb.boneTransforms[i], DirectX::XMLoadFloat4x4(&skeletal.at(i).transform));
				}
				mesh.skeletalAnimation.at(animeIndex).animation_tick += 1;
			}
			// specular
			cb.shininess = 1;
			cb.ambientColor = DirectX::XMFLOAT4(0.1f, 0.1f, 0.1f, 1);
			cb.materialColor = material->color.get_XM4();
		//continue;
		}
		Systems::DeviceContext->UpdateSubresource(Mat_cb.Get(), 0, NULL, &cb, 0, 0);
		Systems::DeviceContext->VSSetConstantBuffers(5, 1, Mat_cb.GetAddressOf());
		Systems::DeviceContext->PSSetConstantBuffers(5, 1, Mat_cb.GetAddressOf());

		//CB : ConstantBufferPerMesh
		{
			ConstantBuffer::ConstantBufferPerMesh g_cb;
			g_cb.Mesh_world = mesh.globalTransform;
			Systems::DeviceContext->UpdateSubresource(mesh.mesh_cb.Get(), 0, NULL, &g_cb, 0, 0);
			Systems::DeviceContext->VSSetConstantBuffers(3, 1, mesh.mesh_cb.GetAddressOf());
			Systems::DeviceContext->PSSetConstantBuffers(3, 1, mesh.mesh_cb.GetAddressOf());
		}

		// frustum_cullingを行う
		{
			Vector3 AABB_center = Vector3(0);
			// 中心位置を求める
			{
				for (int i = 0; i < 3; ++i) {
					AABB_center += DOP::AABB_axis[i] * (-mesh.dop.min[i] + mesh.dop.max[i]) * 0.5f;
				}
				AABB_center = vector3_quatrotate(AABB_center, transform->orientation);
			}

			// サイズを求める
			Vector3 AABB_size = Vector3(0);
			{
				Vector3 world_size =
					vector3_quatrotate(DOP::AABB_axis[0], transform->orientation) * (-mesh.dop.min[0] + mesh.dop.max[0]) * 0.5f +
					vector3_quatrotate(DOP::AABB_axis[1], transform->orientation) * (-mesh.dop.min[1] + mesh.dop.max[1]) * 0.5f +
					vector3_quatrotate(DOP::AABB_axis[2], transform->orientation) * (-mesh.dop.min[2] + mesh.dop.max[2]) * 0.5f;

				for (int i = 0; i < 3; ++i) {
					AABB_size[i] += vector3_dot(DOP::AABB_axis[i], world_size);
				}
			}
			DirectX::BoundingBox box;
			box.Center = transform->position + AABB_center;
			box.Extents = AABB_size;

			auto hr = box.ContainedBy(
				frustum_data.NearPlane,
				frustum_data.FarPlane,
				frustum_data.RightPlane,
				frustum_data.LeftPlane,
				frustum_data.TopPlane,
				frustum_data.BottomPlane
			);

			if (hr != DirectX::ContainmentType::CONTAINS)continue;
		}

		for (auto& subset : mesh.subsets)
		{
			//TODO : modelのtexture設定
			//Systems::DeviceContext->PSSetShaderResources(0, 1, subset.diffuse.shaderResourceVirw.GetAddressOf());
			//texture->Set(0);



			// textureをSRVにセット
			texture->Set(0);

			// 描画
			Systems::DeviceContext->DrawIndexed(subset.indexCount, subset.indexStart, 0);

		}
	}



}

void Mesh_renderer::load_texture(const wchar_t* filename) { texture->Load(filename); };

void Mesh_renderer::calculate_boundingbox() {
	//for (auto& mesh : *meshes) {

	//}
	//box.Center = transform->position;
	//box.Extents = meshes
}