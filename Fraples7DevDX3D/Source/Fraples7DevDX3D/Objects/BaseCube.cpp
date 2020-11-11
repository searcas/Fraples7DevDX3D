#include "BaseCube.h"
#include "Cube.h"
#include "RendererAPI/GFXContextBase.h"
#include "RendererAPI/TransformCBufDual.h"
#include "ImGui/imgui.h"
#include "RendererAPI/Stencil.h"
#include "RendererAPI/RenderPriority/TechniqueProbe.h"
#include "Core/MetaProgramming/DynamicConstant.h"
#include "RendererAPI/ConstantBuffersEx.h"
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
			Technique shade("Shade");
			{
				Step only(0);
				only.AddContext(Texture::Resolve(gfx, "Images\\brickwall.jpg", 0));
				only.AddContext(Sampler::Resolve(gfx));

				auto pvs = VertexShader::Resolve(gfx, "PhongVS.cso");
				auto pvsbyte = pvs->GetBytecode();
				only.AddContext(std::move(pvs));
				only.AddContext(PixelShader::Resolve(gfx, "PhongPS.cso"));
				MP::RawLayout layout;
				layout.Add<MP::Float>("specularIntensity");
				layout.Add<MP::Float>("specularPower");
				auto buf = MP::Buffer(std::move(layout));
				buf["specularIntensity"] = 0.1f;
				buf["specularPower"] = 20.0f;
				only.AddContext(std::make_shared<CachingPixelConstantBufferEx>(gfx, buf, 1u));
				only.AddContext(InputLayout::Resolve(gfx, model._mVertices.GetLayout(), pvsbyte));
				only.AddContext(std::make_shared<TransformCBuf>(gfx));
				shade.AddStep(std::move(only));
			}
			AddTechnique(std::move(shade));
		}
		{
			Technique outLine("Outline");
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
				MP::RawLayout layout;
				layout.Add<MP::Float4>("color");
				auto buf = MP::Buffer(std::move(layout));
				buf["color"] = DirectX::XMFLOAT4{ 1.0f,0.4f,0.4f,1.0f };
				draw.AddContext(std::make_shared<CachingPixelConstantBufferEx>(gfx, buf, 1u));
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
					TransformCbufScaling(Graphics& gfx, float scale = 1.04f)
						:TransformCBuf(gfx),_mBuf(MakeLayout())
					{

					}
					void Accept(TechniqueProbe& probe)override
					{
						probe.VisitBuffer(_mBuf);
					}
					void Bind(Graphics& gfx)noexcept override
					{
						const auto scale = _mBuf["scale"];
						const auto scaleMatrix = DirectX::XMMatrixScaling(scale, scale, scale);
						auto xf = GetTransforms(gfx);

						xf.modelView = xf.modelView * scaleMatrix;
						xf.modelViewProj = xf.modelViewProj * scaleMatrix;
						UpdateBindImpl(gfx, xf);
					}
				private:
					static MP::RawLayout MakeLayout()
					{
						MP::RawLayout layout;
						layout.Add<MP::Float>("scale");
						return layout;
					}
				private:
					MP::Buffer _mBuf;
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
				
			class Probe : public TechniqueProbe
			{
			public:
				void OnSetTechnique()override
				{
					using namespace std::string_literals;

					ImGui::TextColored({ 0.4f, 1.0f,0.6f,1.0f },_mPtech->GetName().c_str());
					bool active = _mPtech->IsActivated();

					ImGui::Checkbox(("Tech Activate##"s + _mPtech->GetName()).c_str(), &active);
					_mPtech->SetActiveState(active);
				}
				bool VisitBuffer(MP::Buffer& buf)override
				{
					bool dirty = false;
					const auto dCheck = [&dirty](bool changed) {dirty = dirty || changed; };

					if (auto v = buf["scale"];v.Exists())
					{
						dCheck(ImGui::SliderFloat("Scale", &v, 1.0f, 2.0f, "%.3f"));
					}
					if (auto v = buf["color"]; v.Exists())
					{
						dCheck(ImGui::ColorPicker4("Color", reinterpret_cast<float*>(&static_cast<DirectX::XMFLOAT4&>(v))));
					}
					if (auto v = buf["specularIntensity"]; v.Exists())
					{
						dCheck(ImGui::SliderFloat("Specular Intensity", &v, 0.0f, 1.0f));
					}
					if (auto v = buf["specularPower"]; v.Exists())
					{
						dCheck(ImGui::SliderFloat("Glossiness", &v, 1.0f, 100.0f,"%.1f"));
					}
					return dirty;
				}

			};
			static Probe probe;
			Accept(probe);
		}
		ImGui::End();
	}
}