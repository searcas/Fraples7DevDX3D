#include "RendererAPI/GFXContextBase.h"
#include "BasePlane.h"
#include "Plane.h"
namespace FraplesDev
{
	BasePlane::BasePlane(Graphics& gfx, float size)
	{

		auto model = Plane::Make();
		model.Transform(DirectX::XMMatrixScaling(size, size, 1.0f));

		const auto geometryTag = "$plane." + std::to_string(size);
		AddBind(VertexBuffer::Resolve(gfx, geometryTag, model._mVertices));
		AddBind(IndexBuffer::Resolve(gfx, geometryTag, model._mIndices));
		AddBind(Texture::Resolve(gfx,"Images\\brickwall.jpg"));

		auto pvs = VertexShader::Resolve(gfx, "PhongVS.cso");
		auto pvsbc = pvs->GetBytecode();
		AddBind(std::move(pvs));

		AddBind(PixelShader::Resolve(gfx, "PhongPS.cso"));

		struct PSMaterialConstant
		{
			float specularIntensity = 0.8f;
			float specularPower = 45.0f;
			float padding[2];
		}pmc;
		AddBind(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, pmc, 1u));
		AddBind(InputLayout::Resolve(gfx, model._mVertices.GetLayout(), pvsbc));
		AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

		AddBind(std::make_shared<TransformCBuf>(gfx, *this));
	}


	void FraplesDev::BasePlane::SetPosXYZ(DirectX::XMFLOAT3 pos) noexcept
	{
		this->pos = pos;
	}

	void FraplesDev::BasePlane::SetRotationRPY(float roll, float pitch, float yaw) noexcept
	{
		this->roll = roll;
		this->pitch = pitch;
		this->yaw = yaw;
		
	}

	const DirectX::XMMATRIX FraplesDev::BasePlane::GetTransformXM() const noexcept
	{
		return DirectX::XMMatrixRotationRollPitchYaw(roll, pitch, yaw) * DirectX::XMMatrixTranslation(pos.x, pos.y, pos.y);
	}
}