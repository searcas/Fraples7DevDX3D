#include "MultiCamera.h"
#include "Camera.h"
#include "imgui.h"
#include "GraphicAPI/Graphics.h"
namespace FraplesDev
{
	void MultiCamera::SpawnWindow(Graphics& gfx)
	{
		if (ImGui::Begin("Cameras"))
		{
			if (ImGui::BeginCombo("Active Camera", (*this)->GetName().c_str()))
			{
				for (int i = 0; i < std::size(_mCameras); i++)
				{
					const bool isSelected = i == _mActive;
					if (ImGui::Selectable(_mCameras[i]->GetName().c_str(), isSelected))
					{
						_mActive = i;
					}
				}
				ImGui::EndCombo();
			}
			if (ImGui::BeginCombo("Controlled Camera",GetControlledCamera().GetName().c_str()))
			{
				for (int i = 0; i < std::size(_mCameras); i++)
				{
						const bool isSelected = i ==_mControlled;
						if (ImGui::Selectable(_mCameras[i]->GetName().c_str(), isSelected))
						{
							_mControlled = i;
						}
				}
				ImGui::EndCombo();
			}
			GetControlledCamera().SpawnControllWindow(gfx);
		}
		ImGui::End();
	}
	void MultiCamera::Bind(Graphics& gfx)
	{
		gfx.SetCamera((*this)->GetMatrix());
	}
	void MultiCamera::AddCamera(std::shared_ptr<Camera> pCam)
	{
		_mCameras.push_back(std::move(pCam));
	}
	Camera* MultiCamera::operator->()
	{
		return _mCameras[_mActive].get();
	}
	void MultiCamera::LinnkTechniques(RenderGraph& rg)
	{
		for (auto& pcam : _mCameras)
		{
			pcam->LinkTechniques(rg);
		}
	}
	void MultiCamera::Submit() const
	{
		for (size_t i = 0; i < _mCameras.size(); i++)
		{
			// We don't wanna render selected camera no sense there
			if (i != _mActive)
			{
				_mCameras[i]->Submit();
			}
		}
	}
}