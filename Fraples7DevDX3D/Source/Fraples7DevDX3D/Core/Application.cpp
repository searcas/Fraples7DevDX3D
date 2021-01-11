#include "Application.h"
#include "Common/Exceptions/Macros/WindowsThrowMacros.h"
#include "Utility/Timer.h"
// ########## Objects ################

//########### End Objects ###########
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "Core/MetaProgramming/Vertex.h"
#include "RendererAPI/VertexBuffer.h"
#include "Utility/Fraples7Utility.h"
#include <algorithm>
#include <cstring>
#include "QualityAssurance/TestingQA.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "RendererAPI/Mesh/ModelProbe.h"
#include "Core/Math/FraplesXM.h"
#include "RendererAPI/Mesh/NodeSystem.h"
#include "RendererAPI/RenderGraph/BufferClearPass.h"
#include "RendererAPI/RenderGraph/LambertianPass.h"
#include "RendererAPI/RenderGraph/OutlineMaskGenerationPass.h"
#include "RendererAPI/RenderGraph/OutlineRenderingPass.h"
namespace FraplesDev
{
	
	Application::Application(const char* name, int width, int height, const std::string& commandLine)
		:_mWin(name, width, height), light(_mWin.GetGFX()), scriptCommander(Utility::TokenizeQuoted(commandLine))
	{
		//QA::TestMaterialSystemLoading(_mWin.GetGFX());
		QA::TestDynamicConstant();
		cube1.SetPos({ 4.0f,0.0f,0.0f });
		cube2.SetPos({ 0.0f,4.0f,0.0f });
		{
			{
				auto bcp = std::make_unique<BufferClearPass>("clear");
				bcp->SetInputSource("renderTarget", "$.backbuffer");
				bcp->SetInputSource("depthStencil", "$.masterDepth");
				renderGraph.AppendPass(std::move(bcp));
			}
			{
				auto lp = std::make_unique<LambertianPass>(_mWin.GetGFX(),"lambertian");
				lp->SetInputSource("renderTarget", "clear.renderTarget");
				lp->SetInputSource("depthStencil", "clear.depthStencil");
				renderGraph.AppendPass(std::move(lp));
			}
			{
				auto pass = std::make_unique<OutlineMaskGenerationPass>(_mWin.GetGFX(), "outlineMask");
				pass->SetInputSource("depthStencil", "lambertian.depthStencil");
				renderGraph.AppendPass(std::move(pass));
			}
			{
				auto pass = std::make_unique<OutlineRenderingPass>(_mWin.GetGFX(), "outlineDraw");
				pass->SetInputSource("renderTarget", "lambertian.renderTarget");
				pass->SetInputSource("depthStencil", "outlineMask.depthStencil");
				renderGraph.AppendPass(std::move(pass));
			}

			renderGraph.SetSinkTarget("backbuffer", "outlineDraw.renderTarget");
			renderGraph.Finalize();
			cube1.LinkTechniques(renderGraph);
			cube2.LinkTechniques(renderGraph);
			light.LinkTechniques(renderGraph);
			_mSponza.LinkTechniques(renderGraph);
		}
		//bluePlane.SetPosXYZ(_mCamera.GetPos());
		//redPlane.SetPosXYZ(_mCamera.GetPos());
		_mWin.GetGFX().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 800.0f));
	}

	Application::~Application()
	{

	}

	int Application::StartApp()
	{
		// process all messages pending, but to not block for new messages
		while (true)
		{
			if (const auto& ecode = FraplesDev::Window::ProcessMessages())
			{
				// if return optional has value, means we're quitting so return exit code
				return *ecode;
			}
			// execute the game logic
			// const auto timer = _mTimer.Start() * _mSpeedFactor;
			DoFrame();
		}
	}

	void Application::SpawnFunc()
	{
		_mCamera.SpawnControllWindow();
		light.SpawnControlWindow();
		SpawnAppInfoWindow();
		ShowRawInputWindow();
		//bluePlane.SpawnControlWindow(_mWin.GetGFX(),"BLUE ONE");
		//redPlane.SpawnControlWindow(_mWin.GetGFX(), "RED ONE");
		cube1.SpawnControlWindow(_mWin.GetGFX(), "Cube 1");
		cube2.SpawnControlWindow(_mWin.GetGFX(), "Cube 2");
	}

	void Application::RenderObj()
	{
		light.Submit();
		_mSponza.Submit();
		cube1.Submit();
		cube2.Submit();
	//	gobber.Submit(fc);
		renderGraph.Execute(_mWin.GetGFX());
	}

	void Application::DoFrame()
	{
		const auto dt = _mTimer.Get() * speed_accelerator;
		_mWin.GetGFX().BeginFrame(0.000069f, 0.000069f, 0.06f);
		//_mWin.GetGFX().BeginFrame(0.19f, 0.19f, 0.19f);
		_mWin.GetGFX().SetCamera(_mCamera.GetMatrix());
		light.Bind(_mWin.GetGFX(), _mCamera.GetMatrix());
		RenderObj();

		class TechProbe : public TechniqueProbe
		{
		public:
			void OnSetTechnique()override
			{
				using namespace std::string_literals;
				ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, _mPtech->GetName().c_str());
				bool active = _mPtech->IsActivated() ? true : true;
				ImGui::Checkbox(("Tech Active##"s + std::to_string(_mTechIdx)).c_str(), &active);
				_mPtech->SetActiveState(active);
			}
			bool OnVisitBuffer(MP::Buffer& buf)override
			{
				float dirty = false;
				const auto dCheck = [&dirty](bool changed) { dirty = dirty || changed; };

				auto tag = [tagScratch = std::string{}, tagString = "##" + std::to_string(_mBufIdx)]
				(const char* label)mutable
				{
					tagScratch = label + tagString;
					return tagScratch.c_str();
				};

				if (auto v = buf["scale"];v.Exists())
				{
					dCheck(ImGui::SliderFloat(tag("Scale"), &v, 1.0f, 2.0f, "%.3f"));
				}
				if (auto v = buf["offset"]; v.Exists())
				{
					dCheck(ImGui::SliderFloat(tag("Offset"), &v, 0.0f, 1.0f, "%.3f"));
				}
				if (auto v = buf["materialColor"]; v.Exists())
				{
					dCheck(ImGui::ColorPicker3(tag("Color"), reinterpret_cast<float*>(&static_cast<DirectX::XMFLOAT3&>(v))));
				}
				if (auto v = buf["specularlColor"]; v.Exists())
				{
					dCheck(ImGui::ColorPicker3(tag("Specular Color"), reinterpret_cast<float*>(&static_cast<DirectX::XMFLOAT3&>(v))));
				}
				if (auto v = buf["specularGloss"]; v.Exists())
				{
					dCheck(ImGui::SliderFloat(tag("Glossiness"), &v, 1.0f, 100.0f, "%.1f"));
				}
				if (auto v = buf["specularWeight"]; v.Exists())
				{
					dCheck(ImGui::SliderFloat(tag("Specular Weight"), &v, 0.0f, 2.0f));
				}
				if (auto v = buf["useSpecularMap"]; v.Exists())
				{
					dCheck(ImGui::Checkbox(tag("Enable Specular Map"), &v));
				}
				if (auto v = buf["useNormalMap"]; v.Exists())
				{
					dCheck(ImGui::Checkbox(tag("Enable Normal Map"), &v));
				}
				if (auto v = buf["normalMapWeight"]; v.Exists())
				{
					dCheck(ImGui::SliderFloat(tag("Normal Map Weight"), &v,0.0f,2.0f));
				}
				return dirty;
			}
		};
		class MP : public ModelProbe
		{
		public:
			void SpawnWindow(Model& model)
			{
				ImGui::Begin("Model");
				ImGui::Columns(2, nullptr, true);
				model.Accept(*this);

				ImGui::NextColumn();

				if (pSelectedNode != nullptr)
				{
					bool dirty = false;
					const auto dcheck = [&dirty](bool changed) {dirty = dirty || changed; };
					auto& tf = ResolveTransform();

					ImGui::TextColored({ 1.0f,0.2f,0.2f,0.7f }, "Translation");
					dcheck(ImGui::SliderFloat("X", &tf.x, -60.0f, 60.0f));
					dcheck(ImGui::SliderFloat("Y", &tf.y, -60.0f, 60.0f));
					dcheck(ImGui::SliderFloat("Z", &tf.z, -60.0f, 60.0f));

					//So we gotta be careful here cuz We can't name again X Y Z
					//Gonna Trigger both childs and move both coords
					ImGui::TextColored({ 0.2f,1.0f,0.2f,0.7f }, "Orientation");
					dcheck(ImGui::SliderAngle("X-Rot", &tf.xRot, -180.0f, 180.0f));
					dcheck(ImGui::SliderAngle("Y-Rot", &tf.yRot, -180.0f, 180.0f));
					dcheck(ImGui::SliderAngle("Z-Rot", &tf.zRot, -180.0f, 180.0f));

					if (dirty)
					{
						pSelectedNode->SetAppliedTransform(
							DirectX::XMMatrixRotationX(tf.xRot) * 
							DirectX::XMMatrixRotationY(tf.yRot) * 
							DirectX::XMMatrixRotationZ(tf.zRot) * 
							DirectX::XMMatrixTranslation(tf.x, tf.y, tf.z));
					}
					if (ImGui::Button("Reset"))
					{
						tf = {};
						pSelectedNode->SetAppliedTransform(
							DirectX::XMMatrixRotationX(tf.xRot) *
							DirectX::XMMatrixRotationY(tf.yRot) *
							DirectX::XMMatrixRotationZ(tf.zRot) *
							DirectX::XMMatrixTranslation(tf.x, tf.y, tf.z));

					}
					TechProbe probe{};
					pSelectedNode->Accept(probe);
				}
				ImGui::End();
			}
		
		protected:
			Node* pSelectedNode = nullptr;
			struct TransformParameters
			{
				float xRot = 0.0f;
				float yRot = 0.0f;
				float zRot = 0.0f;
				float x = 0.0f;
				float y = 0.0f;
				float z = 0.0f;
			};
			std::unordered_map<int, TransformParameters>_mTransformParams;

			TransformParameters& ResolveTransform()noexcept
			{
				const auto id = pSelectedNode->GetId();
				auto i = _mTransformParams.find(id);
				if (i ==_mTransformParams.end())
				{
					return LoadTransform(id);
				}
				return i->second;
			}
			TransformParameters& LoadTransform(int id)noexcept
			{
				const auto& applied = pSelectedNode->GetAppliedTransform();
				const auto angles = ExtractEulerAngles(applied);
				const auto translation = ExtractTranslation(applied);
				TransformParameters tp;
				tp.xRot = angles.x;
				tp.yRot = angles.y;
				tp.zRot = angles.z;
				tp.x = translation.x;
				tp.y = translation.y;
				tp.z = translation.z;
				return _mTransformParams.insert({ id,{ tp }}).first->second;
			}
			bool PushNode(Node& node)override
			{
				// if there is no selected node,  set selectedId to an impossible value;
				const int selectedId = (pSelectedNode == nullptr) ? -1 : pSelectedNode->GetId();
				// build up flags for current node
				const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ((node.GetId() == selectedId) ? ImGuiTreeNodeFlags_Selected : 0) |
					(node.HasChildren() ? 0 : ImGuiTreeNodeFlags_Leaf);
				//render this node
				const auto expanded = ImGui::TreeNodeEx((void*)(intptr_t)node.GetId(), node_flags, node.GetName().c_str());
				// processing for selectin node
				if (ImGui::IsItemClicked())
				{
					pSelectedNode = &node;
				}
				// signal if children should also be recursed
				return expanded;
			}
			void PopNode(Node& node)override
			{
				ImGui::TreePop();
			}
	
		};
		static MP modelProbe;
		modelProbe.SpawnWindow(_mSponza);
		while (const auto e = _mWin._mKey.Readkey())
		{
			if (!e->isPressed())
				continue;
			
			switch (e->GetCode())
			{
			case VK_ESCAPE:
				if (_mWin.IsCursorEnabled())
				{
					_mWin.DisableCursor();
					_mWin._mMouse.EnableRaw();
				}
				else
				{
					_mWin.EnableCursor();
					_mWin._mMouse.DisableRaw();
				}
				break;
			case VK_F1:
				show_demo_window = true;
				break;
			}
		}
		if (!_mWin.IsCursorEnabled())
		{
			if (_mWin._mKey.KeyIsPressed('W'))
			{
				_mCamera.Translate({ 0.0f, 0.0f, dt });
			}
			if (_mWin._mKey.KeyIsPressed('S'))
			{
				_mCamera.Translate({ 0.0f, 0.0f, -dt });
			}
			if (_mWin._mKey.KeyIsPressed('A'))
			{
				_mCamera.Translate({ -dt, 0.0f, 0.0f });
			}
			if (_mWin._mKey.KeyIsPressed('D'))
			{
				_mCamera.Translate({ dt, 0.0f, 0.0f });
			}
			if (_mWin._mKey.KeyIsPressed(VK_SPACE))
			{
				_mCamera.Translate({ 0.0f, dt, 0.0f });
			}
			if (_mWin._mKey.KeyIsPressed(VK_CONTROL))
			{
				_mCamera.Translate({ 0.0f, -dt, 0.0f });
			}
			
		}

		while (const auto delta = _mWin._mMouse.ReadRawDelta())
		{
			if (!_mWin.IsCursorEnabled())
			{
				_mCamera.Rotate((float)delta->x, (float)delta->y);
			}

		}
		SpawnFunc();
		_mWin.GetGFX().EndFrame();
		renderGraph.Reset();
	}

	void Application::ShowImguiDemoWindow()
	{
		static bool show_demo_window = true;
		if (show_demo_window)
		{
			ImGui::ShowDemoWindow(&show_demo_window);
		}
	}

	void Application::SpawnAppInfoWindow()noexcept
	{
		if (ImGui::Begin("Perfomance"))
		{
			ImGui::BulletText("Application Average %.3f ms/frame (%.1f Fps)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		}
		ImGui::End();
	}
	void Application::ShowRawInputWindow() noexcept
	{
		
		if (ImGui::Begin("Raw Input"))
		{
			ImGui::Text("(X, Y): (%d, %d)", _mWin._mMouse.GetPosX(), _mWin._mMouse.GetPosY());
			ImGui::Text("Cursor: %s", _mWin.IsCursorEnabled() ? "Enabled" : "Disabled");
		}
		ImGui::End();
	}
	
}

