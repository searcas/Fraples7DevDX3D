#pragma once
#include <DirectXMath.h>
#include <vector>
#include "GraphicAPI/Graphics.h"
namespace FraplesDev
{
	//MetaProgramming
	namespace MP {
		struct BGRAColorX
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
				Position2D,
				Position3D,
				Texture2D,
				Normal,
				Float3Color,
				Float4Color,
				BGRAColor,
				Count
			};
			template<ElementType> struct Map;
			template<>struct Map<Position2D>
			{
				using SysType = DirectX::XMFLOAT2;
				 static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
				 static constexpr const char* semantic = "Position";
			};
			template<>struct Map<Position3D>
			{
				using SysType = DirectX::XMFLOAT3;
				static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
				static constexpr const char* semantic = "Position";
			};
			template<>struct Map<Texture2D>
			{
				using SysType = DirectX::XMFLOAT2;
				static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
				static constexpr const char* semantic = "Texcoord";
			};
			template<>struct Map<Normal>
			{
				using SysType = DirectX::XMFLOAT3;
				static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
				static constexpr const char* semantic = "Normal";
			};
			template<>struct Map<Float3Color>
			{
				using SysType = DirectX::XMFLOAT3;
				static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
				static constexpr const char* semantic = "Color";
			};
			template<>struct Map<Float4Color>
			{
				using SysType = DirectX::XMFLOAT4;
				static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
				static constexpr const char* semantic = "Color";
			};
			template<>struct Map<BGRAColor>
			{
				using SysType = MP::BGRAColorX;
				static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
				static constexpr const char* semantic = "Color";
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

					switch (type)
					{
					case VertexLayout::Position2D:
						return sizeof(Map<Position2D>::SysType);
						break;
					case VertexLayout::Position3D:
						return sizeof(Map<Position3D>::SysType);
						break;
					case VertexLayout::Texture2D:
						return sizeof(Map<Texture2D>::SysType);
						break;
					case VertexLayout::Normal:
						return sizeof(Map<Normal>::SysType);
						break;
					case VertexLayout::Float3Color:
						return sizeof(Map<Float3Color>::SysType);
						break;
					case VertexLayout::Float4Color:
						return sizeof(Map<Float4Color>::SysType);
						break;
					case VertexLayout::BGRAColor:
						return sizeof(Map<BGRAColor>::SysType);
						break;
					default:
						assert("Invalid element type" && false);
						return 0;
						break;
					}
				}
				template<ElementType type>
				static constexpr D3D11_INPUT_ELEMENT_DESC GenerateDesc(size_t offset)
				{
					return { Map<type>::semantic,0,Map<type>::dxgiFormat,0,(UINT)offset,D3D11_INPUT_PER_VERTEX_DATA,0 };
				}
				ElementType GetType()const noexcept
				{
					return _mType;
				}
				D3D11_INPUT_ELEMENT_DESC GetDesc() const noexcept(!IS_DEBUG)
				{
					switch (_mType)
					{
					case FraplesDev::MP::VertexLayout::Position2D:
						return GenerateDesc<Position2D>(GetOffset());
						break;
					case FraplesDev::MP::VertexLayout::Position3D:
						return GenerateDesc<Position3D>(GetOffset());
						break;
					case FraplesDev::MP::VertexLayout::Texture2D:
						return GenerateDesc<Texture2D>(GetOffset());
						break;
					case FraplesDev::MP::VertexLayout::Normal:
						return GenerateDesc<Normal>(GetOffset());
						break;
					case FraplesDev::MP::VertexLayout::Float3Color:
						return GenerateDesc<Float3Color>(GetOffset());
						break;
					case FraplesDev::MP::VertexLayout::Float4Color:
						return GenerateDesc<Float4Color>(GetOffset());
						break;
					case FraplesDev::MP::VertexLayout::BGRAColor:
						return GenerateDesc<BGRAColor>(GetOffset());
						break;
					default:
						assert("Invalid Element type" && false);
						break;
					}
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

			VertexLayout& Append(ElementType type)noexcept (!IS_DEBUG)
			{
				_mElements.emplace_back(type, Size());
				return *this;
			}
			size_t Size()const noexcept(!IS_DEBUG)
			{
				return _mElements.empty() ? 0u : _mElements.back().GetOffsetAfter();
			}
			size_t GetElementCount()const noexcept
			{
				return _mElements.size();
			}
		
			std::vector<D3D11_INPUT_ELEMENT_DESC>GetD3DLayout()const noexcept(!IS_DEBUG)
			{
				std::vector<D3D11_INPUT_ELEMENT_DESC>desc;
				desc.reserve(GetElementCount());
				for (const auto& e : _mElements)
				{
					desc.push_back(e.GetDesc());
				}
				return desc;
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
				auto pAttribute = _mPdata + _mLayout.Resolve<Type>().GetOffset();
				return *reinterpret_cast<typename VertexLayout::Map<Type>::SysType*>(pAttribute);
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
					SetAttribute <VertexLayout::Position2D> (pAttribute, std::forward<T>(val));
					break;
				case VertexLayout::Position3D:
					SetAttribute<VertexLayout::Position3D>(pAttribute, std::forward<T>(val));
					break;
				case VertexLayout::Texture2D:
					SetAttribute<VertexLayout::Texture2D>(pAttribute, std::forward<T>(val));
					break;
				case VertexLayout::Normal:
					SetAttribute<VertexLayout::Normal>(pAttribute, std::forward<T>(val));
					break;
				case VertexLayout::Float3Color:
					SetAttribute<VertexLayout::Float3Color>(pAttribute, std::forward<T>(val));
					break;
				case VertexLayout::Float4Color:
					SetAttribute<VertexLayout::Float4Color>(pAttribute, std::forward<T>(val));
					break;
				case VertexLayout::BGRAColor:
					SetAttribute<VertexLayout::BGRAColor>(pAttribute, std::forward<T>(val));
					break;
				default:
					assert("Bad element type" && false);
					break;
				}
			}

			//enables parameter pack setting of multiple parameters by element index
			template<typename T, typename ...Args>
			void SetAttributeByIndex(size_t i, T&& t, Args&&... args)
			{
				SetAttributeByIndex(i, std::forward<T>(t));
				SetAttributeByIndex(i + 1, std::forward<Args>(args)...);
			}
		protected:
			Vertex(char* pData, const VertexLayout& layout) noexcept (!IS_DEBUG)
				:_mPdata(pData), _mLayout(layout)
			{
				assert(pData != nullptr);
			}
		private:
			//helper to reduce code duplcation in SetAtttributeByIndex
			template<VertexLayout::ElementType DestLayoutType, typename SrcType>
			void SetAttribute(char* pAttribute, SrcType&& val)noexcept(!IS_DEBUG)
			{
				using dest = typename VertexLayout::Map<DestLayoutType>::SysType;
				if constexpr (std::is_assignable<dest, SrcType>::value)
				{
					*reinterpret_cast<dest*>(pAttribute) = val;
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

		class ConstVertex
		{
		public:
			ConstVertex(const Vertex& v) noexcept(!IS_DEBUG)
				:vertex(v)
			{

			}
			template<VertexLayout::ElementType Type>
			const auto& Attr()const noexcept(!IS_DEBUG)
			{
				return const_cast<Vertex&>(vertex).Attr<Type>();
			}
		private:
			Vertex vertex;
		};
		class VertexBuffer
		{
		public:
			VertexBuffer(VertexLayout layout)noexcept(!IS_DEBUG)
				: _mLayout(std::move(layout))
			{

			}
			const char* GetData()const noexcept(!IS_DEBUG)
			{
				return _mBuffer.data();
			}
			
			const VertexLayout& GetLayout()const noexcept
			{
				return _mLayout;
			}
			size_t SizeOfBytes()const noexcept(!IS_DEBUG)
			{
				return _mBuffer.size();
			}
			size_t Size() const noexcept(!IS_DEBUG)
			{
				return _mBuffer.size() / _mLayout.Size();
			}
			template<typename ...Args>
			void EmplaceBack(Args&&...args)noexcept (!IS_DEBUG)
			{
				assert(sizeof...(args) == _mLayout.GetElementCount() && "Param count doesn't match number of vertex elements");
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
			ConstVertex Back()const noexcept(!IS_DEBUG)
			{
				return const_cast<VertexBuffer*>(this)->Back();
			}
			ConstVertex Front()const noexcept(!IS_DEBUG)
			{
				return const_cast<VertexBuffer*>(this)->Front();
			}
			ConstVertex operator[](size_t i)const noexcept(!IS_DEBUG)
			{
				return const_cast<VertexBuffer&>(*this)[i];
			}
		private:
			std::vector<char>_mBuffer;
			VertexLayout _mLayout;
		};


	}
}