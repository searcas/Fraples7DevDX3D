#include "Box.h"
#include "../RendererAPI/GFXContextBase.h"
#include "Cube.h"
#include "imgui.h"

namespace FraplesDev
{
	Box::Box(Graphics& gfx, std::mt19937& rng, 
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist,
		std::uniform_real_distribution<float>& bdist,
		DirectX::XMFLOAT3 material) 
	{
			struct Vertex
			{
				DirectX::XMFLOAT3 pos;
				DirectX::XMFLOAT3 normal;
			};

			auto model = Cube::MakeIndependent<Vertex>();
			model.SetNormalsIndependentFlat();
			AddBind(std::make_shared<VertexBuffer>(gfx, model._mVertices));

			auto pvs = std::make_shared<VertexShader>(gfx, L"PhongVS.cso");
			auto pvsbc = pvs->GetBytecode();


			AddBind(std::move(pvs));
			AddBind(std::make_shared<PixelShader>(gfx, L"PhongPS.cso"));

			AddBind(std::make_shared<IndexBuffer>(gfx, model._mIndices));


			const std::vector<D3D11_INPUT_ELEMENT_DESC>IelementDesc =
			{
				{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			};
			AddBind(std::make_shared<InputLayout>(gfx, IelementDesc, pvsbc));
			AddBind(std::make_shared<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	
		AddBind(std::make_shared<TransformCBuf>(gfx, *this));

		materialConstants.color = material;
		AddBind(std::make_shared<MaterialCbuf>(gfx, materialConstants, 1u));
		DirectX::XMStoreFloat3x3(&mt, DirectX::XMMatrixScaling(1.0f, 1.0f, bdist(rng)));

	}


	const DirectX::XMMATRIX Box::GetTransformXM()const noexcept
	{
		return std::move(DirectX::XMLoadFloat3x3(&mt) *
			BaseObject::GetTransformXM());
	}
	bool Box::SpawnControlWindow(int id, Graphics& gfx) noexcept
	{
		using namespace std::string_literals;
		bool dirty = false;
		bool open = true;
		if (ImGui::Begin(("Box "s+ std::to_string(id)).c_str(),&open))
		{
			ImGui::Text("Material Properties");
			const auto cd  = ImGui::ColorEdit3("Material Color", &materialConstants.color.x);
			const auto sid = ImGui::SliderFloat("Specular Intensity	",&materialConstants.specularIntensity,0.05f,4.0f,"%.2f");
			const auto spd = ImGui::SliderFloat("Specular Power", &materialConstants.specularPower, 1.0f, 200.0f, "%.2f");
			dirty = cd || sid || spd;

			ImGui::Text("Object Position");
			ImGui::SliderFloat("R", &r, 1.0f, 80.0f, "%.1f");
			ImGui::SliderAngle("Theta", &theta, -180.0f, 180.0f);
			ImGui::SliderAngle("Phi", &phi, -180.0f, 180.0f);
			ImGui::Text("Orientation");
			ImGui::SliderAngle("Roll", &roll, -180.0f, 180.0f);
			ImGui::SliderAngle("Pitch", &pitch, -180.0f, 180.0f);
			ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);
		}
		ImGui::End();
		if (dirty)
		{
			SyncMaterial(gfx);
		}
		return open;
	}
	void Box::SyncMaterial(Graphics& gfx) noexcept(!IS_DEBUG)
	{
		auto pConstPS = QueryBindable<MaterialCbuf>();
		assert(pConstPS != nullptr);
		pConstPS->Update(gfx, materialConstants);
	}
}
