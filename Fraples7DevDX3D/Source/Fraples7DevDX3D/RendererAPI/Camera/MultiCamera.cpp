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
			if (ImGui::BeginCombo("Active Camera", _mCameras[_mSelected]->GetName().c_str()))
			{
				for (int i = 0; i < std::size(_mCameras); i++)
				{
					const bool isSelected = i == _mSelected;
					if (ImGui::Selectable(_mCameras[i]->GetName().c_str(),isSelected))
					{
						_mSelected = i;
					}
				}
				ImGui::EndCombo();
			}
			GetCamera().SpawnControllWindow(gfx);
		}
		ImGui::End();
	}
	void MultiCamera::Bind(Graphics& gfx)
	{
		gfx.SetCamera(GetCamera().GetMatrix());
	}
	void MultiCamera::AddCamera(std::unique_ptr<Camera> pCam)
	{
		_mCameras.push_back(std::move(pCam));
	}
	Camera& MultiCamera::GetCamera()
	{
		return *_mCameras[_mSelected];
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
			if (i != _mSelected)
			{
				_mCameras[i]->Submit();
			}
		}
	}
}