#include "ModelWindow.h"
namespace FraplesDev
{
	void ModelWindow::Show(Graphics& gfx, const char* windowName, const Node& root)
	{
		/*	windowName = windowName ? windowName : "Model ";

			if (ImGui::Begin(windowName))
			{
				ImGui::Columns(2, nullptr, true);

				root.RenderTree(_mPselectedNode);

				ImGui::NextColumn();
				if (_mPselectedNode != nullptr)
				{
					const auto id = _mPselectedNode->GetId();
					auto i = _mTransforms.find(id);
					if (i ==_mTransforms.end())
					{
						const auto& applied = _mPselectedNode->GetAppliedTransform();
						const auto angles = ExtractEulerAngles(applied);
						const auto translation = ExtractTranslation(applied);
						TransformParameters tp;
						tp.roll = angles.z;
						tp.pitch = angles.x;
						tp.yaw = angles.y;

						tp.x = translation.x;
						tp.y = translation.y;
						tp.z = translation.z;

						auto pMatConst = _mPselectedNode->GetMaterialConstants();
						auto buf = pMatConst != nullptr ? std::optional<MP::Buffer>{*pMatConst} : std::optional<MP::Buffer>{};
						std::tie(i, std::ignore) = _mTransforms.insert({ id,{tp,false,std::move(buf),false} });
					}
					// Link imgui ctrl to our cached transform params
					{
						auto& transform = i->second.transformParams;
						// dirty check
						auto& dirty = i->second.transformParamsDirty;
						const auto dcheck = [&dirty](bool changed) {dirty = dirty || changed; };
						// widgets
						ImGui::Text("Orientation");
						dcheck(ImGui::SliderAngle("Roll", &transform.roll, -180.0f, 180.0f));
						dcheck(ImGui::SliderAngle("Pitch", &transform.pitch, -180.0f, 180.0f));
						dcheck(ImGui::SliderAngle("Yaw", &transform.yaw, -180.0f, 180.0f));

						ImGui::Text("Position");
						dcheck(ImGui::SliderFloat("X", &transform.x, -20.0f, 20.0f));
						dcheck(ImGui::SliderFloat("Y", &transform.y, -20.0f, 20.0f));
						dcheck(ImGui::SliderFloat("Z", &transform.z, -20.0f, 20.0f));
					}
					// link imgui ctrl to our cached material params
					if (i->second.materialCbuf)
					{
						auto& mat = *i->second.materialCbuf;
						//dirty check
						auto& dirty = i->second.materialCbufDirty;
						const auto dcheck = [&dirty](bool changed) {dirty = dirty || changed; };

						ImGui::Text("Material");
						if (auto v = mat["normalMapEnabled"]; v.Exists())
						{
							dcheck(ImGui::Checkbox("Norm Map", &v));
						}
						if (auto v = mat["specularMapEnabled"]; v.Exists())
						{
							dcheck(ImGui::Checkbox("Spec Map", &v));
						}
						if (auto v = mat["hasGlossMap"]; v.Exists())
						{
							dcheck(ImGui::Checkbox("Gloss Map", &v));
						}
						if (auto v = mat["materialColor"]; v.Exists())
						{
							dcheck(ImGui::ColorPicker3("Diff Color", reinterpret_cast<float*>(&static_cast<DirectX::XMFLOAT3&>(v))));
						}
						if (auto v = mat["specularPower"]; v.Exists())
						{
							dcheck(ImGui::SliderFloat("Spec Power", &v, 0.0f, 100.0f, "%.1f"));
						}
						if (auto v = mat["specularColor"]; v.Exists())
						{
							dcheck(ImGui::ColorPicker3("Spec Color", reinterpret_cast<float*>(&static_cast<DirectX::XMFLOAT3&>(v))));
						}
						if (auto v = mat["specularMapWeight"]; v.Exists())
						{
							dcheck(ImGui::SliderFloat("Spec Weight", &v, 0.0f, 4.0f));
						}
						if (auto v = mat["specularIntensity"]; v.Exists())
						{
							dcheck(ImGui::SliderFloat("Spec Intens", &v, 0.0f, 1.0f));
						}
					}
				}
			}
			ImGui::End();
			*/
	}
	void ModelWindow::ApplyParameters() noexcept
	{
		/*
		if (TransformDirty())
		{
			_mPselectedNode->SetAppliedTransform(GetTransform());
			ResetTransformDirty();
		}
		if (MaterialDirty())
		{
			_mPselectedNode->SetMaterialConstants(GetMaterial());
			ResetTransformDirty();
		}
		*/
	}

	DirectX::XMMATRIX ModelWindow::GetTransform() const noexcept
	{
		assert(_mPselectedNode != nullptr);
		return DirectX::XMMatrixIdentity();
	}

	bool ModelWindow::TransformDirty()const noexcept(!IS_DEBUG)
	{
		return false;//_mPselectedNode && _mTransforms.at(_mPselectedNode->GetId()).transformParamsDirty;
	}
	void ModelWindow::ResetTransformDirty()noexcept(!IS_DEBUG)
	{
		//_mTransforms.at(_mPselectedNode->GetId()).transformParamsDirty = false;
	}
	bool ModelWindow::MaterialDirty()const noexcept(!IS_DEBUG)
	{
		return false;//_mPselectedNode && _mTransforms.at(_mPselectedNode->GetId()).materialCbufDirty;
	}
	void ModelWindow::ResetMaterialDirty()noexcept(!IS_DEBUG)
	{
		//_mTransforms.at(_mPselectedNode->GetId()).materialCbufDirty = false;
	}
	bool ModelWindow::IsDirty()const noexcept(!IS_DEBUG)
	{
		return TransformDirty() || MaterialDirty();
	}

}