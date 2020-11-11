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
		VertexLayout& VertexLayout::Append(ElementType type) noexcept(!IS_DEBUG)
		{
			_mElements.emplace_back(type, Size());
			return *this;
		}
		size_t VertexLayout::Size() const noexcept(!IS_DEBUG)
		{
			return _mElements.empty() ? 0u : _mElements.back().GetOffsetAfter();
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
		std::vector<D3D11_INPUT_ELEMENT_DESC> VertexLayout::GetD3DLayout() const noexcept(!IS_DEBUG)
		{
			std::vector<D3D11_INPUT_ELEMENT_DESC>desc = {};
			desc.reserve(GetElementCount());
			for (const auto& e : _mElements)
			{
				desc.push_back(e.GetDesc());
			}
			return desc;
		}
		Vertex::Vertex(char* pData, const VertexLayout& layout) noexcept(!IS_DEBUG) :_mPdata(pData), _mLayout(layout)
		{
			assert(pData != nullptr);
		}
		VertexBuffer::VertexBuffer(VertexLayout layout, size_t size) noexcept(!IS_DEBUG)
			: _mLayout(std::move(layout))
		{
			Resize(size);
		}
		void VertexBuffer::Resize(size_t newSize) noexcept(!IS_DEBUG)
		{
			const auto size = Size();
			if (size < newSize)
			{
				_mBuffer.resize(_mBuffer.size() + _mLayout.Size() * (newSize - size));
			}
		}
		const char* VertexBuffer::GetData() const noexcept(!IS_DEBUG)
		{
			return _mBuffer.data();
		}
		const VertexLayout& VertexBuffer::GetLayout()const noexcept
		{
			return _mLayout;
		}
		size_t VertexBuffer::SizeOfBytes() const noexcept(!IS_DEBUG)
		{
			return _mBuffer.size();
		}
		size_t VertexBuffer::Size() const noexcept(!IS_DEBUG)
		{
			return _mBuffer.size() / _mLayout.Size();
		}
		Vertex VertexBuffer::Back() noexcept(!IS_DEBUG)
		{
			assert(_mBuffer.size() != 0u);
			return Vertex{ _mBuffer.data() + _mBuffer.size() - _mLayout.Size(), _mLayout };
		}
		Vertex VertexBuffer::Front() noexcept(!IS_DEBUG)
		{
			assert(_mBuffer.size() != 0u);
			return Vertex{ _mBuffer.data(), _mLayout };
		}
		Vertex VertexBuffer::operator[](size_t i) noexcept(!IS_DEBUG)
		{
			assert(i < Size());
			return Vertex{ _mBuffer.data() + _mLayout.Size() * i,_mLayout };
		}
		ConstVertex VertexBuffer::Back() const noexcept(!IS_DEBUG)
		{
			return const_cast<VertexBuffer*>(this)->Back();
		}
		ConstVertex VertexBuffer::Front() const noexcept(!IS_DEBUG)
		{
			return const_cast<VertexBuffer*>(this)->Front();
		}
		ConstVertex VertexBuffer::operator[](size_t i) const noexcept(!IS_DEBUG)
		{
			return const_cast<VertexBuffer&>(*this)[i];
		}
		ConstVertex::ConstVertex(const Vertex& v) noexcept(!IS_DEBUG)
			: vertex(v)
		{

		}
}
}