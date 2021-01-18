#include "Projection.h"
#include "ImGui/imgui.h"
namespace FraplesDev
{
	Projection::Projection(Graphics& gfx, float width, float height, float nearZ, float farZ)
		:_mWidth(width), _mHeight(height), _mNearZ(nearZ), _mFarZ(farZ)
		, _mFrust(gfx, width, height, nearZ, farZ)
		, _mHomeWidth(width), _mHomeHeight(height), _mHomeNearZ(nearZ), _mHomeFarZ(farZ)
	{

	}
	DirectX::XMMATRIX Projection::GetMatrix() const
	{
		return DirectX::XMMatrixPerspectiveLH(_mWidth, _mHeight, _mNearZ, _mFarZ);
	}
	void Projection::SetPosition(DirectX::XMFLOAT3 pos)
	{
		_mFrust.SetPosition(pos);
	}
	void Projection::SetRotation(DirectX::XMFLOAT3 rot)
	{
		_mFrust.SetRotation(rot);
	}
	void Projection::Submit() const
	{
		_mFrust.Submit();
	}
	void Projection::Reset(Graphics& gfx)
	{
		_mWidth = _mHomeWidth;
		_mHeight = _mHomeHeight;
		_mNearZ = _mHomeNearZ;
		_mFarZ = _mHomeFarZ;
		_mFrust.SetVertices(gfx, _mWidth, _mHeight, _mNearZ, _mFarZ);
	}
	void Projection::LinkTechniques(RenderGraph& rg)
	{
		_mFrust.LinkTechniques(rg);
	}
	void Projection::RenderWidgets(Graphics& gfx)
	{
		bool dirty = false;
		const auto dcheck = [&dirty](bool d) {dirty = dirty || d; };
		ImGui::Text("Projection");
		dcheck(ImGui::SliderFloat("Width", &_mWidth, 0.01f, 4.0f, "%.2f"));
		dcheck(ImGui::SliderFloat("Height", &_mHeight, 0.01f, 4.0f, "%.2f"));
		dcheck(ImGui::SliderFloat("Near Z", &_mNearZ, 0.5f, _mFarZ - 0.01f, "%.2f"));
		dcheck(ImGui::SliderFloat("Far Z", &_mFarZ, _mNearZ + 0.01f, 400.0f, "%.2f"));

		if (dirty)
		{
			_mFrust.SetVertices(gfx, _mWidth, _mHeight, _mNearZ, _mFarZ);
		}
	}
}