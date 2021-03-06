#include "BaseCube.h"
#include "Cube.h"
#include "RendererAPI/GFXContextBase.h"
#include "ImGui/imgui.h"
#include "RendererAPI/Stencil/Stencil.h"
#include "RendererAPI/RenderPriority/TechniqueProbe.h"
#include "Core/MetaProgramming/DynamicConstant.h"
#include "RendererAPI/ConstantBuffersEx.h"
#include "RendererAPI/TransformCBuf/TransformCBufScaling.h"
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

		auto tcb = std::make_shared<TransformCBuf>(gfx);
		{
			Technique shade("Shade", Channel::main);
			{
				Step only("lambertian");
				only.AddContext(Texture::Resolve(gfx, "Images\\brickwall.jpg"));
				only.AddContext(Sampler::Resolve(gfx));

				auto pvs = VertexShader::Resolve(gfx, "ShadowTest_VS.cso");
				only.AddContext(InputLayout::Resolve(gfx, model._mVertices.GetLayout(), *pvs));
				only.AddContext(std::move(pvs));
				only.AddContext(PixelShader::Resolve(gfx, "ShadowTest_PS.cso"));
				MP::RawLayout layout;
				layout.Add<MP::Float3>("specularColor");
				layout.Add<MP::Float>("specularWeight");
				layout.Add<MP::Float>("specularGloss");
				auto buf = MP::Buffer(std::move(layout));
				buf["specularColor"] = DirectX::XMFLOAT3{ 1.0f,1.0f,1.0f };
				buf["specularWeight"] = 0.1f;
				buf["specularGloss"] = 20.0f;
				only.AddContext(std::make_shared<CachingPixelConstantBufferEx>(gfx, buf, 1u));
				only.AddContext(std::make_shared<TransformCBuf>(gfx));
				shade.AddStep(std::move(only));
			}
			AddTechnique(std::move(shade));
		}
		{
			Technique outLine("Outline", Channel::main);
			{
				Step mask("outlineMask");


				//TODO: Better Sub-Layout generation tech for future consideration maybe
				mask.AddContext(InputLayout::Resolve(gfx, model._mVertices.GetLayout(), *VertexShader::Resolve(gfx,"Solid_VS.cso")));
				mask.AddContext(std::move(tcb));

				//TODO: might need to specify rasterizer when doubled-sided models start being used
				outLine.AddStep(std::move(mask));
			}
			{
				Step draw("outlineDraw");

				MP::RawLayout lay;
				lay.Add<MP::Float4>("color");
				auto buf = MP::Buffer(std::move(lay));
				buf["color"] = DirectX::XMFLOAT4{ 1.0f,0.4f,0.4f,1.0f };
				draw.AddContext(std::make_shared<CachingPixelConstantBufferEx>(gfx, buf, 1u));
				draw.AddContext(InputLayout::Resolve(gfx, model._mVertices.GetLayout(), *VertexShader::Resolve(gfx, "Solid_VS.cso")));
				draw.AddContext(std::make_shared<TransformCBuf>(gfx));
				outLine.AddStep(std::move(draw));
			}

			AddTechnique(std::move(outLine));
		}
		// Shadow map technique
		{
			Technique map{ "ShadowMap",Channel::shadow, true };
			{
				Step draw("shadowMap");

				//TODO: better sub-layout generation tech for future consideration maybe
				draw.AddContext(InputLayout::Resolve(gfx, model._mVertices.GetLayout(), *VertexShader::Resolve(gfx, "Solid_VS.cso")));
				draw.AddContext(std::make_shared<TransformCBuf>(gfx));
				
				//TODO: might need to specify rasterizer when doubled-sided models start being used

				map.AddStep(std::move(draw));
			}
			AddTechnique(std::move(map));
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
				
			class Probe : public TechniqueProbe
			{
			public:
				void OnSetTechnique()override
				{
					using namespace std::string_literals;

					ImGui::TextColored({ 1.0f, 0.1f,0.1f,0.5f },_mPtech->GetName().c_str());
					bool active = _mPtech->IsActivated();

					ImGui::Checkbox(("Tech Activate##"s + std::to_string(_mTechIdx)).c_str(), &active);
					_mPtech->SetActiveState(active);
				}
				bool OnVisitBuffer(MP::Buffer& buf)override
				{
					bool dirty = false;
					const auto dCheck = [&dirty](bool changed) {dirty = dirty || changed; };
					auto tag = [tagScratch = std::string{}, tagString = "##" + std::to_string(_mBufIdx)](const char* label) mutable
					{
						tagScratch = label + tagString;
						return tagScratch.c_str();
					};

					if (auto v = buf["scale"]; v.Exists())
					{
						dCheck(ImGui::SliderFloat(tag("Scale"), &v, 1.02f, 2.0f, "%.3f"));
					}
					if (auto v = buf["color"]; v.Exists())
					{
						dCheck(ImGui::ColorPicker4(tag("Color"), reinterpret_cast<float*>(&static_cast<DirectX::XMFLOAT4&>(v))));
					}
					if (auto v = buf["specularIntensity"]; v.Exists())
					{
						dCheck(ImGui::SliderFloat(tag("Specular Intensity"), &v, 0.0f, 1.0f));
					}
					if (auto v = buf["specularPower"]; v.Exists())
					{
						dCheck(ImGui::SliderFloat(tag("Glossiness"), &v, 1.0f, 100.0f,"%.1f"));
					}
					return dirty;
				}

			}probe;
			Accept(probe);
		}
		ImGui::End();
	}
}