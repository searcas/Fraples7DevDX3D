#pragma once
#include "RendererAPI/TransformCBuf.h"
#include "Core/MetaProgramming/DynamicConstant.h"


namespace FraplesDev
{
	class TransformCBufScaling : public TransformCBuf
	{
	public:
		TransformCBufScaling(Graphics& gfx, float scale = 1.01f);
		void Accept(TechniqueProbe& probe)override;
		void Bind(Graphics& gfx)noexcept override;
	private:
		static MP::RawLayout MakeLayout();
	private:
		MP::Buffer _mBuf;
	};
}