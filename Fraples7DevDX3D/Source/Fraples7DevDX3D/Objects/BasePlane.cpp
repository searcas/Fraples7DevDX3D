#include "RendererAPI/GFXContextBase.h"
#include "BasePlane.h"
#include "Plane.h"

namespace FraplesDev
{
	BasePlane::BasePlane(Graphics& gfx, float size)
	{

		auto model = Plane::Make();
		model.Transform(DirectX::XMMatrixScaling(size, size, 1.0f));

		const auto geometryTag = "$plane." + std::to_string(size);
		AddBind(VertexBuffer::Resolve(gfx, geometryTag, model._mVertices));
		AddBind(IndexBuffer::Resolve(gfx, geometryTag, model._mIndices));
		AddBind(Texture::Resolve(gfx, "Images\\brickwall.jpg"));
		AddBind(Texture::Resolve(gfx,"Images\\brickwall_normal.jpg",1u));

		auto pvs = VertexShader::Resolve(gfx, "PhongVS.cso");
		auto pvsbc = pvs->GetBytecode();
		AddBind(std::move(pvs));

		AddBind(PixelShader::Resolve(gfx, "PhongPSNormalMapObject.cso"));

	
		AddBind(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, pmc, 1u));
		AddBind(InputLayout::Resolve(gfx, model._mVertices.GetLayout(), pvsbc));
		AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

		AddBind(std::make_shared<TransformCbufDual>(gfx, *this, 0u, 2u));
	}


	void FraplesDev::BasePlane::SetPosXYZ(DirectX::XMFLOAT3 pos) noexcept
	{
		this->pos = pos;
	}

	void FraplesDev::BasePlane::SetRotationRPY(float roll_in, float pitch_in, float yaw_in) noexcept
	{
		roll = roll_in;
		pitch = pitch_in;
		yaw = yaw_in;
		
	}

	const DirectX::XMMATRIX FraplesDev::BasePlane::GetTransformXM() const noexcept
	{
		return DirectX::XMMatrixRotationRollPitchYaw(roll, pitch, yaw) * DirectX::XMMatrixTranslation(pos.x, pos.y, pos.y);
	}
	void BasePlane::SpawnControlWindow(Graphics& gfx)noexcept
	{
		if (ImGui::Begin("Plane"))
		{
				ImGui::Text("Position");
				ImGui::SliderFloat("X", &pos.x, -80.0f, 80.0f,"%.1f");
				ImGui::SliderFloat("Y", &pos.y, -80.0f, 80.0f,"%.1f");
				ImGui::SliderFloat("Z", &pos.z, -80.0f, 80.0f,"%.1f");


				ImGui::Text("Orientation");
				ImGui::SliderAngle("Roll", &roll, -180.0f, 180.0f);
				ImGui::SliderAngle("Pitch", &pitch, -180.0f, 180.0f);
				ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);
				ImGui::Text("Shading");
				bool change0 = ImGui::SliderFloat("Specular Intensity.", &pmc.specularIntensity, 0.0f, 1.0f);
				bool change1 = ImGui::SliderFloat("Specular Power.", &pmc.specularPower, 0.0f, 100.0f);
				bool checkState = pmc.normalMappingEnabled == TRUE;
				bool change2 = ImGui::Checkbox("Enable Normal Map", &checkState);
				pmc.normalMappingEnabled = checkState ? TRUE : FALSE;
				if (change0 || change1 || change2)
				{
					QueryBindable<PixelConstantBuffer<PSMaterialConstant>>()->Update(gfx, pmc);
				}
				if (ImGui::Button("Reset"))
				{
					pos = {1.0f,17.0f,-1.0f};
					roll = -0;
					pitch = -0;
					yaw = -0;
					pmc = {};
				}
		}
		ImGui::End();
	}
}