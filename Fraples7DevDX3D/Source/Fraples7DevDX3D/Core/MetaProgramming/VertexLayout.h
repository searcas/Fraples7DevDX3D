#pragma once
#include <DirectXMath.h>
#include <vector>
namespace FraplesDev
{
	struct BGRAcolor
	{
		unsigned char a;
		unsigned char r;
		unsigned char g;
		unsigned char b;
	};
	class VertexLayout
	{
	public:
		enum ElementType
		{
			NONE = 0,
			Position2D,
			Position3D,
			Texture2D,
			Normal,
			Float3Color,
			Float4Color,
			BGRAColor
		};
		class Element
		{
		public:
			Element(ElementType type, size_t offset) : _mType(type),
				_mOffset(offset)
			{

			}
			size_t GetOffsetAfter()const noexcept(!IS_DEBUG)
			{
				return _mOffset + Size();
			}
			size_t GetOffset()const noexcept
			{
				return _mOffset;
			}
			size_t Size()const noexcept(!IS_DEBUG)
			{
				return SizeOf(_mType);
			}
			static constexpr size_t SizeOf(ElementType type)noexcept(!IS_DEBUG)
			{
				using namespace DirectX;

				switch (type)
				{
				case FraplesDev::VertexLayout::NONE:
					return 0;
					break;
				case FraplesDev::VertexLayout::Position2D:
					return sizeof(XMFLOAT2);
					break;
				case FraplesDev::VertexLayout::Position3D:
					return sizeof(XMFLOAT3);
					break;
				case FraplesDev::VertexLayout::Texture2D:
					return sizeof(XMFLOAT2);
					break;
				case FraplesDev::VertexLayout::Normal:
					return sizeof(XMFLOAT3);
					break;
				case FraplesDev::VertexLayout::Float3Color:
					return sizeof(XMFLOAT3);
					break;
				case FraplesDev::VertexLayout::Float4Color:
					return sizeof(XMFLOAT4);
					break;
				case FraplesDev::VertexLayout::BGRAColor:
					return sizeof(unsigned int);
					break;
				default:
					assert("Invalid element type" && false);
					return 0;
					break;
				}
			}
			ElementType GetType()const noexcept
			{
				return _mType;
			}
		private:
			ElementType _mType;
			size_t _mOffset;
		};
	public:
		template<ElementType type>
		const Element& Resolve()const noexcept(!IS_DEBUG)
		{
			for (auto& e : _mElements)
			{
				if (e.GetType() == type)
				{
					return e;
				}
			}
			assert("Could not resolve element type" && false);
			return _mElements.front();
		}
		const Element& ResolveByIndex(size_t i)const noexcept(!IS_DEBUG)
		{
			return _mElements[i];
		}

		template<ElementType Type>
		VertexLayout& Append()noexcept (!IS_DEBUG)
		{
			_mElements.emplace_back(Type, Size());
			return *this;
		}
		size_t Size()const noexcept(!IS_DEBUG)
		{
			return _mElements.empty() ? 0u : _mElements.back().GetOffsetAfter();
		}
	private:
		std::vector<Element> _mElements;
	};

	class Vertex
	{
		friend class VertexBuffer;
	public:
		template<VertexLayout::ElementType Type>
		auto& Attr()noexcept (!IS_DEBUG)
		{
			using namespace DirectX;
			const auto& element = _mLayout.Resolve<Type>();
			auto pAttribute = _mPdata + element.GetOffset();
			if constexpr(Type == VertexLayout::Position2D)
			{
				return *reinterpret_cast<XMFLOAT2*>(pAttribute);
			}
			else if constexpr (Type == VertexLayout::Position3D)
			{
				return *reinterpret_cast<XMFLOAT3*>(pAttribute);
			}
			else if constexpr (Type == VertexLayout::Texture2D)
			{
				return *reinterpret_cast<XMFLOAT2*>(pAttribute);
			}
			else if constexpr (Type == VertexLayout::Normal)
			{
				return *reinterpret_cast<XMFLOAT3*>(pAttribute);
			}
			else if constexpr (Type == VertexLayout::Float3Color)
			{
				return *reinterpret_cast<XMFLOAT3*>(pAttribute);
			}
			else if constexpr (Type == VertexLayout::Float4Color)
			{
				return *reinterpret_cast<XMFLOAT4*>(pAttribute);
			}
			else if constexpr (Type == VertexLayout::BGRAColor)
			{
				return *reinterpret_cast<BGRAcolor*>(pAttribute);
			}
			else
			{
				assert("Bad element type" && false);
				return *reinterpret_cast<char*>(pAttribute);
			}
		}
		template <typename T>
		void SetAttributeByIndex(size_t i, T&& val)noexcept(!IS_DEBUG)
		{
			using namespace DirectX;

			const auto& element = _mLayout.ResolveByIndex(i);
			auto pAttribute = _mPdata + element.GetOffset();
			switch (element.GetType())
			{
			case VertexLayout::Position2D:
				SetAttribute<XMFLOAT2>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::Position3D:
				SetAttribute<XMFLOAT3>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::Texture2D:
				SetAttribute<XMFLOAT2>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::Normal:
				SetAttribute<XMFLOAT3>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::Float3Color:
				SetAttribute<XMFLOAT3>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::Float4Color:
				SetAttribute<XMFLOAT4>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::BGRAColor:
				SetAttribute<BGRAcolor>(pAttribute, std::forward<T>(val));
				break;
			default:
				assert("Bad element type" && false);
				break;
			}
		}
		Vertex(char* pData, const VertexLayout& layout)noexcept(!IS_DEBUG)
			:_mPdata(pData),_mLayout(layout)
		{
			assert(pData != nullptr);
		}
		//enables parameter pack setting of multiple parameters by element index
		template<typename T, typename ...Args>
		void SetAttributeByIndex(size_t i, T&& t, Args&&... args)
		{
			SetAttributeByIndex(i, std::forward<T>(t));
			SetAttributeByIndex(i + 1, std::forward<Args>(args)...);
		}

		//helper to reduce code duplcation in SetAtttributeByIndex
		template<typename Dest,typename Src>
		void SetAttribute(char* pAttribute, Src&& val)noexcept(!IS_DEBUG)
		{
			if constexpr (std::is_assignable<Dest, Src>::value)
			{
				*reinterpret_cast<Dest*>(pAttribute) = val;
			}
			else
			{
				assert("Parameter attribute type mismatch" && false);
			}
		}
	private:
		char* _mPdata = nullptr;
		const VertexLayout& _mLayout;
	};

	class VertexBuffer
	{
	public:
		VertexBuffer(VertexLayout layout)noexcept(!IS_DEBUG)
			: _mLayout(std::move(layout))
		{

		}
		const VertexLayout& GetLayout()const noexcept
		{
			return _mLayout;
		}
		size_t Size()const noexcept(!IS_DEBUG)
		{
			return _mBuffer.size() / _mLayout.Size();
		}
		template<typename ...Args>
		void EmplaceBack(Args&&...args)noexcept (!IS_DEBUG)
		{
			_mBuffer.resize(_mBuffer.size() + _mLayout.Size());
			Back().SetAttributeByIndex(0u, std::forward<Args>(args)...);
		}
		Vertex Back()noexcept(!IS_DEBUG)
		{
			assert(_mBuffer.size() != 0u);
			return Vertex{ _mBuffer.data() + _mBuffer.size() - _mLayout.Size(), _mLayout };
		}
		Vertex Front()noexcept(!IS_DEBUG)
		{
			assert(_mBuffer.size() != 0u);
			return Vertex{ _mBuffer.data(), _mLayout };
		}
		Vertex operator[](size_t i)noexcept(!IS_DEBUG)
		{
			assert(i < Size());
			return Vertex{ _mBuffer.data() + _mLayout.Size() * i,_mLayout };
		}

	private:
		std::vector<char>_mBuffer;
		VertexLayout _mLayout;
	};


}