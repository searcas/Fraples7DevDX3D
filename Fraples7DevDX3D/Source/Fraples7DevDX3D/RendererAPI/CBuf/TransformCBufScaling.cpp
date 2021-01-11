#include "TransformCBufScaling.h"
#include "RendererAPI/RenderPriority/TechniqueProbe.h"
namespace FraplesDev
{


	TransformCBufScaling::TransformCBufScaling(Graphics& gfx, float scale)
		:TransformCBuf(gfx),_mBuf(std::move(MakeLayout()))
	{
		_mBuf["scale"] = scale;
	}

	void TransformCBufScaling::Accept(TechniqueProbe& probe)
	{
		probe.VisitBuffer(_mBuf);
	}

	void TransformCBufScaling::Bind(Graphics& gfx) noexcept
	{
		const float scale = _mBuf["scale"];
		const auto scaleMatrix = DirectX::XMMatrixScaling(scale, scale, scale);
		auto xf = GetTransforms(gfx);
		xf.modelView *= scaleMatrix;
		xf.modelViewProj *= scaleMatrix;
		UpdateBindImpl(gfx, xf);
	}

	MP::RawLayout TransformCBufScaling::MakeLayout()
	{
		MP::RawLayout layout;
		layout.Add<MP::Float>("scale");
		return layout;
	}
}