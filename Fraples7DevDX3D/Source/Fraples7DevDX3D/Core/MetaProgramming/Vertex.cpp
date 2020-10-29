#include "Vertex.h"


namespace FraplesDev
{
	namespace MP
	{

		const char* VertexLayout::Element::GetCode()const noexcept
		{
			switch (_mType)
			{
			case VertexLayout::Position2D:
				return Map<Position2D>::code;
				break;
			case VertexLayout::Position3D:
				return Map<Position3D>::code;
				break;
			case VertexLayout::Texture2D:
				return Map<Texture2D>::code;
				break;
			case VertexLayout::Normal:
				return Map<Normal>::code;
				break;
			case VertexLayout::Float3Color:
				return Map<Float3Color>::code;
				break;
			case VertexLayout::Float4Color:
				return Map<Float4Color>::code;
				break;
			case VertexLayout::BGRAColor:
				return Map<BGRAColor>::code;
				break;
			default:
				assert("Invalid element type" && false);
				return "Invalid";
			}

		}
		std::string VertexLayout::GetCode()const noexcept(!IS_DEBUG)
		{
			std::string code;
			for (const auto& e : _mElements)
			{
				code += e.GetCode();
			}
			return code;
		}
	}
}