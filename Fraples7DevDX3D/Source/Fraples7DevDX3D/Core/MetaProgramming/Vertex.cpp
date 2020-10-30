#include "Vertex.h"


namespace FraplesDev
{
	namespace MP
	{

		const char* VertexLayout::Element::GetCode()const noexcept
		{
			switch (_mType)
			{
			case ElementType::Position2D:
				return Map<ElementType::Position2D>::code;
				break;
			case ElementType::Position3D:
				return Map<ElementType::Position3D>::code;
				break;
			case ElementType::Texture2D:
				return Map<ElementType::Texture2D>::code;
				break;
			case ElementType::Normal:
				return Map<ElementType::Normal>::code;
				break;
			case ElementType::Tangent:
				return Map<ElementType::Tangent>::code;
				break;
			case ElementType::Bitangent:
				return Map<ElementType::Bitangent>::code;
				break;

			case ElementType::Float3Color:
				return Map<ElementType::Float3Color>::code;
				break;
			case ElementType::Float4Color:
				return Map<ElementType::Float4Color>::code;
				break;
			case ElementType::BGRAColor:
				return Map<ElementType::BGRAColor>::code;
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