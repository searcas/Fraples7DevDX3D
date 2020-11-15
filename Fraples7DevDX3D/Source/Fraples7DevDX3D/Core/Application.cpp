#include "Application.h"
#include "Common/Exceptions/Macros/WindowsThrowMacros.h"
#include "Utility/Timer.h"
// ########## Objects ################
//#include "../Objects/Sphere.h"
//#include "../Objects/Melon.h"
//#include "../Objects/Sheet.h"
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

namespace FraplesDev
{
	
	Application::Application(const char* name, int width, int height, const std::string& commandLine)
		:_mWin(name, width, height), light(_mWin.GetGFX()), scriptCommander(Utility::TokenizeQuoted(commandLine))
	{
		//QA::TestMaterialSystemLoading(_mWin.GetGFX());
		cube1.SetPos({ 4.0f,0.0f,0.0f });
		cube2.SetPos({ 0.0f,4.0f,0.0f });
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
			{// if return optional has value, means we're quitting so return exit code
				return *ecode;
			}
			// execute the game logic
			// const auto timer = _mTimer.Start() * _mSpeedFactor;
			DoFrame();
		}
	}

	void Application::SpawnFunc()
	{
		//_mSponza.ShowModelInfo(_mWin.GetGFX(), "SPONZAAAAAAAAAAaaaa");
		_mCamera.SpawnControllWindow();
		light.SpawnControlWindow();
		SpawnAppInfoWindow();
		ShowRawInputWindow();
	//	bluePlane.SpawnControlWindow(_mWin.GetGFX(),"BLUE ONE");
	//	redPlane.SpawnControlWindow(_mWin.GetGFX(), "RED ONE");
		cube1.SpawnControlWindow(_mWin.GetGFX(), "Cube 1");
		cube2.SpawnControlWindow(_mWin.GetGFX(), "Cube 2");
	}

	void Application::RenderObj()
	{
		light.Submit(fc);
		//_mPloaded->Submit(fc, DirectX::XMMatrixIdentity());
		_mSponza.Submit(fc);
		cube1.Submit(fc);
		cube2.Submit(fc);
	//	gobber.Submit(fc);
		fc.Execute(_mWin.GetGFX());
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
				bool active = _mPtech->IsActivated();
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
					dCheck(ImGui::SliderFloat(tag("Scale"), &v, 0.0f, 1.0f, "%.3f"));
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
				}
				if (pSelectedNode != nullptr)
				{
					TechProbe probe;
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
					//used to change the highlighted node on selection change
					struct Probe : public TechniqueProbe
					{
						virtual void OnSetTechnique()
						{
							if (_mPtech->GetName() == "Outline")
							{
								_mPtech->SetActiveState(highlighted);
							}
						}
						bool highlighted = true;
					}probe;
					// remove highlight on prev-selected node
					if (pSelectedNode != nullptr)
					{
						pSelectedNode->Accept(probe);
					}
					//add highlight to newly-selected node
					probe.highlighted = true;
					node.Accept(probe);

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
		fc.Reset();
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
	/*
	void Application::SpawnBoxWindowManagerWindow()noexcept
	{

		if (ImGui::Begin("Boxes"))
		{
			using namespace std::string_literals;
			const auto preview = comboBoxIndex ? std::to_string(*comboBoxIndex) : "Chose a box..."s;
			if (ImGui::BeginCombo("Box Number ", preview.c_str()))
			{
				for (int i = 0; i < boxes.size(); i++)
				{
					const bool selected = *comboBoxIndex == i;
					if (ImGui::Selectable(std::to_string(i).c_str(), selected))
					{
						comboBoxIndex = i;
					}
					if (selected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			if (ImGui::Button("Spawn Control Window") && comboBoxIndex)
			{
				boxControlIds.insert(*comboBoxIndex);
				comboBoxIndex.reset();
			}
		}
		ImGui::End();
	}
	*/
	/*
	//imgui box attribute control windows
	void Application::SpawnBoxWindows() noexcept
	{
		for (auto i = boxControlIds.begin(); i!= boxControlIds.end();)
		{
			if (!boxes[*i]->SpawnControlWindow(*i,_mWin.GetGFX()))
			{
				i = boxControlIds.erase(i);
			}
			else
			{
				i++;
			}
		}
		_mWin.GetGFX().EndFrame();
	}
	*/
	void Application::ShowRawInputWindow() noexcept
	{
		while (const auto d = _mWin._mMouse.ReadRawDelta())
		{
			_mX += d->x;
			_mY += d->y;
		}
		if (ImGui::Begin("Raw Input"))
		{
			ImGui::Text("(X, Y): (%d, %d)", _mX, _mY);
			ImGui::Text("Cursor: %s", _mWin.IsCursorEnabled() ? "Enabled" : "Disabled");
		}
		ImGui::End();
	}
	
}

