
#include "systems.h"
#include "material.h"
#include "matrix.h"

namespace Adollib {
	 Material::Material() {
		 color = vector4(1, 1, 1, 1);
		 DS_state = State_manager::DStypes::DS_TRUE;
		 RS_state = State_manager::RStypes::RS_CULL_BACK;
		 BS_state = State_manager::BStypes::BS_NONE;

		Systems::CreateConstantBuffer(&Mat_cb, sizeof(ConstantBufferPerMaterial));
	}

	void Material::render() {

		Systems::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Systems::DeviceContext->IASetInputLayout(vertexLayout.Get());

		shader.Activate();

		RS_state = State_manager::RStypes::RS_CULL_NONE;
		if (Systems::BS_type != BS_state) Systems::SetBlendState(BS_state);
		if (Systems::RS_type != RS_state) Systems::SetRasterizerState(RS_state);
		if (Systems::DS_type != DS_state) Systems::SetDephtStencilState(DS_state);

		for (Mesh::mesh mesh : *meshes)
		{
			UINT stride = sizeof(VertexFormat);
			UINT offset = 0;
			Systems::DeviceContext->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
			Systems::DeviceContext->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

			//CB : ConstantBufferPerMaterial
			ConstantBufferPerMaterial cb;
			for (int i = 0; i < MAX_BONES; i++) {
				cb.boneTransforms[i] = matrix_identity().get_XMFLOAT4X4();
			}
			{
				// boneTransform
				if (mesh.skeletalAnimation.size() > 0 && mesh.skeletalAnimation[animeIndex].size() > 0)
				{
					int frame = mesh.skeletalAnimation.at(animeIndex).animation_tick / mesh.skeletalAnimation.at(animeIndex).sampling_time;
					if (frame > mesh.skeletalAnimation.at(animeIndex).size() - 1)
					{
						frame = 0;
						mesh.skeletalAnimation.at(animeIndex).animation_tick = 0;
					}
					vector<Mesh::bone>& skeletal = mesh.skeletalAnimation.at(animeIndex).at(frame);
					size_t number_of_bones = skeletal.size();
					_ASSERT_EXPR(number_of_bones < MAX_BONES, L"'the number_of_bones' exceeds MAX_BONES.");
					for (size_t i = 0; i < number_of_bones; i++)
					{
						XMStoreFloat4x4(&cb.boneTransforms[i], XMLoadFloat4x4(&skeletal.at(i).transform));
					}
					mesh.skeletalAnimation.at(animeIndex).animation_tick += 1;
				}
				//// specular
				cb.shininess = 1;
				cb.ambientColor = XMFLOAT4(0.1f, 0.1f, 0.1f, 1);
				cb.materialColor = color.get_XM4();
			}
			Systems::DeviceContext->UpdateSubresource(Mat_cb.Get(), 0, NULL, &cb, 0, 0);
			Systems::DeviceContext->VSSetConstantBuffers(4, 1, Mat_cb.GetAddressOf());
			Systems::DeviceContext->PSSetConstantBuffers(4, 1, Mat_cb.GetAddressOf());

			for (auto& subset :mesh.subsets)
			{
				Systems::DeviceContext->PSSetShaderResources(0, 1, subset.diffuse.shaderResourceVirw.GetAddressOf());

				// �`��
				Systems::DeviceContext->DrawIndexed(subset.indexCount, subset.indexStart, 0);

			}
		}
	}

}