#include "Projection.h"
#include "imgui.h"
#include <utility>

namespace FraplesDev
{
	Projection::Projection(float width, float height, float nearZ, float farZ)
		:_mWidth(std::move(width)), _mHeight(std::move(height)), _mNearZ(std::move(nearZ)), _mFarZ(std::move(farZ))
	{

	}
	DirectX::XMMATRIX Projection::GetMatrix() const
	{
		return DirectX::XMMatrixPerspectiveLH(_mWidth, _mHeight, _mNearZ, _mFarZ);
	}
	void Projection::RenderWidgets()
	{
		ImGui::SliderFloat("Width", &_mWidth, 0.01f, 4.0f, "%.2f");
		ImGui::SliderFloat("Height", &_mHeight, 0.01f, 4.0f, "%.2f");
		ImGui::SliderFloat("Near Z", &_mNearZ, 0.01f, 400.0f, "%.2f");
		ImGui::SliderFloat("Far Z", &_mFarZ, 0.01f, 400.0f, "%.2f");
	}
}