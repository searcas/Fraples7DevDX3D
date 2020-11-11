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
		_mPvertices = VertexBuffer::Resolve(gfx, geometryTag, model._mVertices);
		_mPindices = IndexBuffer::Resolve(gfx, geometryTag, model._mIndices);
		_mPtopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		{
			Technique standard;
			{
				Step only(0);
				only.AddContext(Texture::Resolve(gfx, "Images\\brickwall.jpg", 0));
				only.AddContext(Sampler::Resolve(gfx));

				auto pvs = VertexShader::Resolve(gfx, "PhongVS.cso");
				auto pvsbyte = pvs->GetBytecode();
				only.AddContext(std::move(pvs));

				only.AddContext(PixelShader::Resolve(gfx, "PhongPS.cso"));
				only.AddContext(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, pmc, 1u));
				only.AddContext(InputLayout::Resolve(gfx, model._mVertices.GetLayout(), pvsbyte));
				only.AddContext(std::make_shared<TransformCBuf>(gfx));
				standard.AddStep(std::move(only));
			}
			AddTechnique(std::move(standard));
		}
		{
			Technique outLine;
			{
				Step mask(1);

				auto pvs = VertexShader::Resolve(gfx, "SolidVS.cso");
				auto pvsbyte = pvs->GetBytecode();
				mask.AddContext(std::move(pvs));

				//TODO: Better Sub-Layout generation tech for future consideration maybe
				mask.AddContext(InputLayout::Resolve(gfx, model._mVertices.GetLayout(), pvsbyte));
				mask.AddContext(std::make_shared<TransformCBuf>(gfx));

				//TODO: might need to specify rasterizer when doubled-sided models start being used
				outLine.AddStep(std::move(mask));
			}
			{
				Step draw(2);

				//these can be pass-constant (tricky due to layout issues)
				auto pvs = VertexShader::Resolve(gfx, "SolidVS.cso");
				auto pvsbyte = pvs->GetBytecode();
				draw.AddContext(std::move(pvs));

				// this can be pass-constant
				draw.AddContext(PixelShader::Resolve(gfx, "SolidPS.cso"));

				//TODO: Better Sub-Layout generation tech for future consideration maybe
				draw.AddContext(InputLayout::Resolve(gfx, model._mVertices.GetLayout(), pvsbyte));

				//quick and dirty... nicer soulution maybe takes a lamba... we'll see
				class TransformCbufScaling : public TransformCBuf
				{
				public:
					using TransformCBuf::TransformCBuf;

					void Bind(Graphics& gfx)noexcept override
					{
						const auto scale = DirectX::XMMatrixScaling(1.04f, 1.04f, 1.04f);
						auto xf = GetTransforms(gfx);
						xf.modelView = xf.modelView * scale;
						xf.modelViewProj = xf.modelViewProj * scale;
						UpdateBindImpl(gfx, xf);
					}
				};
				draw.AddContext(std::make_shared<TransformCbufScaling>(gfx));
				//TODO: might need to specify rasterizer when doubled-sided models start being used
				outLine.AddStep(std::move(draw));
			}
			AddTechnique(std::move(outLine));
		}
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
		return DirectX::XMMatrixRotationRollPitchYaw(roll, pitch, yaw) * DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
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
		/*	ImGui::Text("Shading");
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
		}*/
		}
		ImGui::End();
	}
}