#include "RendererAPI/GFXContextBase.h"
#include "BasePlane.h"
#include "Plane.h"

namespace FraplesDev
{
	BasePlane::BasePlane(Graphics& gfx, float size,DirectX::XMFLOAT4 color):pmc({color})
	{

		auto model = Plane::Make();
		model.Transform(DirectX::XMMatrixScaling(size, size, 1.0f));

		const auto geometryTag = "$plane." + std::to_string(size);
		AddBind(VertexBuffer::Resolve(gfx, geometryTag, model._mVertices));
		AddBind(IndexBuffer::Resolve(gfx, geometryTag, model._mIndices));
		
		auto pvs = VertexShader::Resolve(gfx, "SolidVS.cso");
		auto pvsbc = pvs->GetBytecode();
		AddBind(std::move(pvs));

		AddBind(PixelShader::Resolve(gfx, "SolidPS.cso"));

	
		AddBind(std::make_shared<PixelConstantBuffer<PSMaterialConstant>>(gfx, pmc, 1u));
		AddBind(InputLayout::Resolve(gfx, model._mVertices.GetLayout(), pvsbc));
		AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

		AddBind(std::make_shared<TransformCBuf>(gfx, *this,0u));
		AddBind(Blending::Resolve(gfx, true, 0.5f));
		AddBind(Rasterizer::Resolve(gfx, true));
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
	void BasePlane::SpawnControlWindow(Graphics& gfx,const std::string&name)noexcept
	{
		if (ImGui::Begin(name.c_str()))
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
				auto pBlender = QueryBindable<Blending>();
				float factor = pBlender->GetFactor();

				ImGui::SliderFloat("Translucency", &factor, 0.0f, 1.0f);
				pBlender->SetFactor(factor);
		}
		ImGui::End();
	}
}