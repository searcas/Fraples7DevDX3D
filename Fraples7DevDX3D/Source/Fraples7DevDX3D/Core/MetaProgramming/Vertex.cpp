#define DVTX_SOURCE_FILE
#include "Vertex.h"


namespace FraplesDev
{
	namespace MP
	{
	
		VertexLayout::Element::Element(ElementType type, size_t offset)
			: _mType(type),
			_mOffset(offset)
		{

		}
		size_t VertexLayout::Element::GetOffsetAfter() const noexcept(!IS_DEBUG)
		{
			return _mOffset + Size();
		}
		template<ElementType type>
		struct SysSizeLookup
		{
			static constexpr auto Exec()noexcept
			{
				return sizeof(MP::VertexLayout::Map<type>::SysType);
			}
		};
		constexpr size_t VertexLayout::Element::SizeOf(ElementType type) noexcept(!IS_DEBUG)
		{
			return Bridge<SysSizeLookup>(type);
		}
		size_t VertexLayout::Element::Size() const noexcept(!IS_DEBUG)
		{
			return SizeOf(_mType);
		}

		ElementType VertexLayout::Element::GetType() const noexcept
		{
			return _mType;
		}

		template<ElementType type>
		struct CodeLookup
		{
			static constexpr auto Exec()noexcept
			{
				return VertexLayout::Map<type>::code;
			}
		};
		const char* VertexLayout::Element::GetCode()const noexcept
		{

			return Bridge<CodeLookup>(_mType);

		}
		template<ElementType type>struct DescGenerate
		{
			static constexpr D3D11_INPUT_ELEMENT_DESC Exec(size_t offset)noexcept
			{
				return { VertexLayout::Map<type>::semantic,0,
					VertexLayout::Map<type>::dxgiFormat,0,
					(UINT)offset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			}
		};
		D3D11_INPUT_ELEMENT_DESC VertexLayout::Element::GetDesc() const noexcept(!IS_DEBUG)
		{
			return Bridge<DescGenerate>(_mType, GetOffset());
		}
		VertexLayout& VertexLayout::Append(ElementType type) noexcept(!IS_DEBUG)
		{
			_mElements.emplace_back(type, Size());
			return *this;
		}
		size_t MP::VertexLayout::Size()const noexcept(!IS_DEBUG)
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
		template<ElementType type>
		struct AttributeAiMeshFill
		{
			static constexpr void Exec(VertexBuffer* pBuf, const aiMesh& mesh)noexcept(!IS_DEBUG)
			{
				for (auto end = mesh.mNumVertices,i = 0u; i < end; i++)
				{
					(*pBuf)[i].Attr<type>() = VertexLayout::Map<type>::Extract(mesh, i);
				}
			}
		};
		VertexBuffer::VertexBuffer(VertexLayout layout_in, const aiMesh& mesh)
			:_mLayout(std::move(layout_in))
		{
			Resize(mesh.mNumVertices);
			for (size_t i = 0,end = _mLayout.GetElementCount(); i < end; i++)
			{
				VertexLayout::Bridge<AttributeAiMeshFill>(_mLayout.ResolveByIndex(i).GetType(), this, mesh);
			}
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