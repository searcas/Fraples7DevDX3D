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
		enum class ElementType
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
		class VertexLayout
		{
		public:
	
			template<ElementType> struct Map;
			template<>struct Map<ElementType::Position2D>
			{
				using SysType = DirectX::XMFLOAT2;
				 static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
				 static constexpr const char* semantic = "Position";
				 static constexpr const char* code = "P2";
			};
			template<>struct Map <ElementType::Position3D >
			{
				using SysType = DirectX::XMFLOAT3;
				static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
				static constexpr const char* semantic = "Position";				
				static constexpr const char* code = "P3";

			};
			template<>struct Map<ElementType::Texture2D>
			{
				using SysType = DirectX::XMFLOAT2;
				static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
				static constexpr const char* semantic = "Texcoord";
				static constexpr const char* code = "TC2";

			};
			template<>struct Map<ElementType::Normal>
			{
				using SysType = DirectX::XMFLOAT3;
				static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
				static constexpr const char* semantic = "Normal";
				static constexpr const char* code = "N";

			};
			template<>struct Map<ElementType::Float3Color>
			{
				using SysType = DirectX::XMFLOAT3;
				static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
				static constexpr const char* semantic = "Color";
				static constexpr const char* code = "C3";
			};
			template<>struct Map<ElementType::Float4Color>
			{
				using SysType = DirectX::XMFLOAT4;
				static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
				static constexpr const char* semantic = "Color";
				static constexpr const char* code = "C4";
			};
			template<>struct Map<ElementType::BGRAColor>
			{
				using SysType = MP::BGRAColorX;
				static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
				static constexpr const char* semantic = "Color";
				static constexpr const char* code = "C8";
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
					case ElementType::Position2D:
						return sizeof(Map<ElementType::Position2D>::SysType);
						break;
					case ElementType::Position3D:
						return sizeof(Map<ElementType::Position3D>::SysType);
						break;
					case ElementType::Texture2D:
						return sizeof(Map<ElementType::Texture2D>::SysType);
						break;
					case ElementType::Normal:
						return sizeof(Map<ElementType::Normal>::SysType);
						break;
					case ElementType::Float3Color:
						return sizeof(Map<ElementType::Float3Color>::SysType);
						break;
					case ElementType::Float4Color:
						return sizeof(Map<ElementType::Float4Color>::SysType);
						break;
					case ElementType::BGRAColor:
						return sizeof(Map<ElementType::BGRAColor>::SysType);
						break;
					default:
						assert("Invalid element type" && false);
						return 0;
						break;
					}
				}
				template<ElementType type>
				static constexpr D3D11_INPUT_ELEMENT_DESC GenerateDesc(size_t offset) noexcept
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
					case ElementType::Position2D:
						return GenerateDesc<ElementType::Position2D>(GetOffset());
						break;
					case ElementType::Position3D:
						return GenerateDesc<ElementType::Position3D>(GetOffset());
						break;
					case ElementType::Texture2D:
						return GenerateDesc<ElementType::Texture2D>(GetOffset());
						break;
					case ElementType::Normal:
						return GenerateDesc<ElementType::Normal>(GetOffset());
						break;
					case ElementType::Float3Color:
						return GenerateDesc<ElementType::Float3Color>(GetOffset());
						break;
					case ElementType::Float4Color:
						return GenerateDesc<ElementType::Float4Color>(GetOffset());
						break;
					case ElementType::BGRAColor:
						return GenerateDesc<ElementType::BGRAColor>(GetOffset());
						break;
					default:
						assert("Invalid Element type" && false);
						return {};
						break;
					}
					return {};
				}

				const char* GetCode()const noexcept;
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
		
			std::string GetCode()const noexcept(!IS_DEBUG);
			std::vector<D3D11_INPUT_ELEMENT_DESC>GetD3DLayout()const noexcept(!IS_DEBUG)
			{
				std::vector<D3D11_INPUT_ELEMENT_DESC>desc = {};
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
			template<ElementType Type>
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
				case ElementType::Position2D:
					SetAttribute <ElementType::Position2D> (pAttribute, std::forward<T>(val));
					break;
				case ElementType::Position3D:
					SetAttribute<ElementType::Position3D>(pAttribute, std::forward<T>(val));
					break;
				case ElementType::Texture2D:
					SetAttribute<ElementType::Texture2D>(pAttribute, std::forward<T>(val));
					break;
				case ElementType::Normal:
					SetAttribute<ElementType::Normal>(pAttribute, std::forward<T>(val));
					break;
				case ElementType::Float3Color:
					SetAttribute<ElementType::Float3Color>(pAttribute, std::forward<T>(val));
					break;
				case ElementType::Float4Color:
					SetAttribute<ElementType::Float4Color>(pAttribute, std::forward<T>(val));
					break;
				case ElementType::BGRAColor:
					SetAttribute<ElementType::BGRAColor>(pAttribute, std::forward<T>(val));
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
			template<ElementType DestLayoutType, typename SrcType>
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
			template<ElementType Type>
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
	
			VertexBuffer(VertexLayout layout,size_t size = 0u)noexcept(!IS_DEBUG)
				: _mLayout(std::move(layout))
			{
				Resize(size);
			}
			void Resize(size_t newSize)noexcept(!IS_DEBUG)
			{
				const auto size = Size();
				if (size < newSize)
				{
					_mBuffer.resize(_mBuffer.size() + _mLayout.Size() * (newSize - size));
				}
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