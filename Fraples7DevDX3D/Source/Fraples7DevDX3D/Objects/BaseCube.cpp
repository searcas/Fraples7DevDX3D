#include "BaseCube.h"
#include "Cube.h"
#include "RendererAPI/GFXContextBase.h"
#include "RendererAPI/TransformCBufDual.h"
#include "ImGui/imgui.h"
#include "RendererAPI/Stencil.h"

namespace FraplesDev
{
	BaseCube::BaseCube(Graphics& gfx, float size)
	{
		auto model = Cube::MakeIndependentTextured();
		model.Transform(DirectX::XMMatrixScaling(size, size, size));
		model.SetNormalsIndependentFlat();

		const auto geometryTag = "$cube." + std::to_string(size);
		AddBind(VertexBuffer::Resolve(gfx, geometryTag, model._mVertices));
		AddBind(IndexBuffer::Resolve(gfx, geometryTag, model._mIndices));

		AddBind(Texture::Resolve(gfx, "Images\\brickwall.jpg", 0));
		AddBind(Sampler::Resolve(gfx));

		auto pvs = VertexShader::Resolve(gfx, "PhongVS.cso");
		auto pvsbyte = pvs->GetBytecode();
		AddBind(std::move(pvs));

		AddBind(PixelShader::Resolve(gfx, "PhongPS.cso"));
		AddBind(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, pmc, 1u));
		AddBind(InputLayout::Resolve(gfx, model._mVertices.GetLayout(), pvsbyte));
		AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

		auto tcbdb = std::make_shared<TransformCbufDual>(gfx, *this, 0u, 2u);
		AddBind(tcbdb);

		AddBind(std::make_shared<Stencil>(gfx, Stencil::Mode::Write));

		outLineEffect.push_back(VertexBuffer::Resolve(gfx, geometryTag, model._mVertices));
		outLineEffect.push_back(IndexBuffer::Resolve(gfx, geometryTag, model._mIndices));
		pvs = VertexShader::Resolve(gfx, "SolidVS.cso");
		pvsbyte = pvs->GetBytecode();
		outLineEffect.push_back(std::move(pvs));
		outLineEffect.push_back(PixelShader::Resolve(gfx, "SolidPS.cso"));
		struct SolidColorBuffer
		{
			DirectX::XMFLOAT4 color = { 1.0f,0.4f,0.4f,1.0f };
		}scb;
		outLineEffect.push_back(PixelConstantBuffer<SolidColorBuffer>::Resolve(gfx, scb, 1u));
		outLineEffect.push_back(InputLayout::Resolve(gfx, model._mVertices.GetLayout(), pvsbyte));
		outLineEffect.push_back(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
		outLineEffect.push_back(std::move(tcbdb));
		outLineEffect.push_back(std::make_shared<Stencil>(gfx, Stencil::Mode::Mask));
	}
	void BaseCube::SetPos(DirectX::XMFLOAT3 pos_in) noexcept
	{
		pos = pos_in;
	}
	void BaseCube::SetRotation(float roll_in, float pitch_in, float yaw_in) noexcept
	{
		roll = roll_in;
		pitch = pitch_in;
		yaw = yaw_in;
	}
	const DirectX::XMMATRIX BaseCube::GetTransformXM() const noexcept
	{
		auto xf = DirectX::XMMatrixRotationRollPitchYaw(roll, pitch, yaw) * DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
		if (outLining)
		{
			xf = DirectX::XMMatrixScaling(1.03f, 1.03f, 1.03f) * xf;
		}
		return xf;

	}
	void BaseCube::SpawnControlWindow(Graphics& gfx, const char* name) noexcept
	{
		if (ImGui::Begin(name))
		{
			ImGui::Text("Position");
			ImGui::SliderFloat("X", &pos.x, -80.0f, 80.0f, "%.1f");
			ImGui::SliderFloat("Y", &pos.y, -80.0f, 80.0f, "%.1f");
			ImGui::SliderFloat("Z", &pos.z, -80.0f, 80.0f, "%.1f");


			ImGui::Text("Orientation");
			ImGui::SliderAngle("Roll", &roll, -180.0f, 180.0f);
			ImGui::SliderAngle("Pitch", &pitch, -180.0f, 180.0f);
			ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);
			ImGui::Text("Shading");
			bool change0 = ImGui::SliderFloat("Specular Intensity.", &pmc.specularIntensity, 0.0f, 1.0f);
			bool change1 = ImGui::SliderFloat("Specular Power.", &pmc.specularPower, 0.0f, 100.0f);
			bool checkState = pmc.normalMappingEnabled== TRUE;
			bool change2 = ImGui::Checkbox("Enable Normal Map", &checkState);
			pmc.normalMappingEnabled= checkState ? TRUE : FALSE;
			if (change0 || change1 || change2)
			{
				QueryBindable<PixelConstantBuffer<PSMaterialConstant>>()->Update(gfx, pmc);
			}
			if (ImGui::Button("Reset"))
			{
				pos = { 1.0f,17.0f,-1.0f };
				roll = -0;
				pitch = -0;
				yaw = -0;
				pmc = {};
			}
		}
		ImGui::End();
	}
}