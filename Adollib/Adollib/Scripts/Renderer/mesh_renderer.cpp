
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

void Mesh_renderer::render() {
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

	// textureをSRVにセット
	texture->Set(0);

	//
	if (Systems::BS_type != material->BS_state) Systems::SetBlendState(material->BS_state);
	if (Systems::RS_type != material->RS_state) Systems::SetRasterizerState(material->RS_state);
	if (Systems::DS_type != material->DS_state) Systems::SetDephtStencilState(material->DS_state);

	for (Mesh::mesh& mesh : *meshes)
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

		for (auto& subset : mesh.subsets)
		{
			//TODO : modelのtexture設定
			//Systems::DeviceContext->PSSetShaderResources(0, 1, subset.diffuse.shaderResourceVirw.GetAddressOf());
			//texture->Set(0);

			// 描画
			Systems::DeviceContext->DrawIndexed(subset.indexCount, subset.indexStart, 0);

		}
	}



}

void Mesh_renderer::load_texture(const wchar_t* filename) { texture->Load(filename); };