#pragma once
#include <DirectXMath.h>
#include <vector>
#include "GraphicAPI/Graphics.h"
#include <assimp/scene.h>
#include <utility>

#define DVTX_ELEMENT_AI_EXTRACTOR(member)\
static SysType Extract(const aiMesh& mesh,size_t i)noexcept\
{return *reinterpret_cast<const SysType*>(&mesh.member[i]);}

#define LAYOUT_ELEMENT_TYPES\
		X(Position2D)\
		X(Position3D)\
		X(Texture2D)\
		X(Normal)\
		X(Tangent)\
		X(Bitangent)\
		X(Float3Color)\
		X(Float4Color)\
		X(BGRAColor)\
		X(Count)

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
#define X(el)el, 
LAYOUT_ELEMENT_TYPES
#undef X
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
				 DVTX_ELEMENT_AI_EXTRACTOR(mVertices)
			};
			template<>struct Map <ElementType::Position3D >
			{
				using SysType = DirectX::XMFLOAT3;
				static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
				static constexpr const char* semantic = "Position";				
				static constexpr const char* code = "P3";
				DVTX_ELEMENT_AI_EXTRACTOR(mVertices)
			};
			template<>struct Map<ElementType::Texture2D>
			{
				using SysType = DirectX::XMFLOAT2;
				static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
				static constexpr const char* semantic = "Texcoord";
				static constexpr const char* code = "TC2";
				DVTX_ELEMENT_AI_EXTRACTOR(mTextureCoords[0])
			};
			template<>struct Map<ElementType::Normal>
			{
				using SysType = DirectX::XMFLOAT3;
				static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
				static constexpr const char* semantic = "Normal";
				static constexpr const char* code = "N";
				DVTX_ELEMENT_AI_EXTRACTOR(mNormals)
			};
			template<>struct Map<ElementType::Tangent>
			{
				using SysType = DirectX::XMFLOAT3;
				static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
				static constexpr const char* semantic = "Tangent";
				static constexpr const char* code = "Nt";
				DVTX_ELEMENT_AI_EXTRACTOR(mTangents)
			};
			template<>struct Map<ElementType::Bitangent>
			{
				using SysType = DirectX::XMFLOAT3;
				static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
				static constexpr const char* semantic = "Bitangent";
				static constexpr const char* code = "Nb";
				DVTX_ELEMENT_AI_EXTRACTOR(mBitangents)
			};
			template<>struct Map<ElementType::Float3Color>
			{
				using SysType = DirectX::XMFLOAT3;
				static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
				static constexpr const char* semantic = "Color";
				static constexpr const char* code = "C3";
				DVTX_ELEMENT_AI_EXTRACTOR(mColors[0])
			};
			template<>struct Map<ElementType::Float4Color>
			{
				using SysType = DirectX::XMFLOAT4;
				static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
				static constexpr const char* semantic = "Color";
				static constexpr const char* code = "C4";
				DVTX_ELEMENT_AI_EXTRACTOR(mColors[0])
			};
			template<>struct Map<ElementType::BGRAColor>
			{
				using SysType = MP::BGRAColorX;
				static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
				static constexpr const char* semantic = "Color";
				static constexpr const char* code = "C8";
				DVTX_ELEMENT_AI_EXTRACTOR(mColors[0])
			};
			template <> struct Map<ElementType::Count>
			{
				using SysType = long double;
				static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_UNKNOWN;
				static constexpr const char* semantic = "!INVALID!";
				static constexpr const char* code = "!INV!";
				DVTX_ELEMENT_AI_EXTRACTOR(mFaces)
			};
			template <template<ElementType> class F,typename... Args>
			static constexpr auto Bridge(ElementType type, Args&& ...args)noexcept(!IS_DEBUG)
			{
				switch (type)
				{
#define X(el) case ElementType::el: return F<ElementType::el>::Exec(std::forward<Args>(args)...);
					LAYOUT_ELEMENT_TYPES
#undef X
				default:
					break;
				}
			}
			class Element
			{
			public:
				Element(ElementType type, size_t offset);
				size_t GetOffsetAfter()const noexcept(!IS_DEBUG);
				
				inline size_t GetOffset()const noexcept
				{
					return _mOffset;
				}
				static constexpr size_t SizeOf(ElementType tyoe)noexcept(!IS_DEBUG);
				size_t Size()const noexcept(!IS_DEBUG);
				ElementType GetType()const noexcept;
				D3D11_INPUT_ELEMENT_DESC GetDesc() const noexcept(!IS_DEBUG);
				const char* GetCode()const noexcept;
			private:
				mutable ElementType _mType;
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
			inline const Element& ResolveByIndex(size_t i)const noexcept(!IS_DEBUG)
			{
				return _mElements[i];
			}

			VertexLayout& Append(ElementType type)noexcept (!IS_DEBUG);
			size_t Size()const noexcept(!IS_DEBUG);
			inline size_t GetElementCount()const noexcept
			{
				return _mElements.size();
			}
			
			bool Has(ElementType type)const noexcept;
	
			std::string GetCode()const noexcept(!IS_DEBUG);
			std::vector<D3D11_INPUT_ELEMENT_DESC>GetD3DLayout()const noexcept(!IS_DEBUG);
		private:
			std::vector<Element> _mElements;
		};

		class Vertex
		{
			friend class VertexBuffer;
		private:
			// necessary for bridge to SetAttribute
			template<ElementType type>
			struct AttributeSetting
			{
				template<typename T>
				static constexpr auto Exec(Vertex* pVertex, char* pAttribute, T&& val)noexcept(!IS_DEBUG)
				{
					return pVertex->SetAttribute<type>(pAttribute, std::forward<T>(val));
				}
			};
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
				
				VertexLayout::Bridge<AttributeSetting>(element.GetType(), this, pAttribute, std::forward<T>(val));
			}

	
		protected:
			Vertex(char* pData, const VertexLayout& layout) noexcept (!IS_DEBUG);
		private:

			//enables parameter pack setting of multiple parameters by element index
			template<typename T, typename ...Args>
			void SetAttributeByIndex(size_t i, T&& t, Args&&... args)
			{
				SetAttributeByIndex(i, std::forward<T>(t));
				SetAttributeByIndex(i + 1, std::forward<Args>(args)...);
			}
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
			ConstVertex(const Vertex& v) noexcept(!IS_DEBUG);
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

			VertexBuffer(VertexLayout layout_in, const aiMesh& mesh);
			VertexBuffer(VertexLayout layout, size_t size = 0u)noexcept(!IS_DEBUG);
			void Resize(size_t newSize)noexcept(!IS_DEBUG);
			const char* GetData()const noexcept(!IS_DEBUG);
			
			const VertexLayout& GetLayout()const noexcept;

			size_t SizeOfBytes()const noexcept(!IS_DEBUG);
			size_t Size() const noexcept(!IS_DEBUG);
		
			template<typename ...Args>
			void EmplaceBack(Args&&...args)noexcept (!IS_DEBUG)
			{
				assert(sizeof...(args) == _mLayout.GetElementCount() && "Param count doesn't match number of vertex elements");
				_mBuffer.resize(_mBuffer.size() + _mLayout.Size());
				Back().SetAttributeByIndex(0u, std::forward<Args>(args)...);
			}
			Vertex Back()noexcept(!IS_DEBUG);
			Vertex Front()noexcept(!IS_DEBUG);
			Vertex operator[](size_t i)noexcept(!IS_DEBUG);
			
			ConstVertex Back()const noexcept(!IS_DEBUG);
			ConstVertex Front()const noexcept(!IS_DEBUG);
			ConstVertex operator[](size_t i)const noexcept(!IS_DEBUG);
		private:
			std::vector<char>_mBuffer;
			VertexLayout _mLayout;
		};


	}
}
#undef DVTX_ELEMENT_AI_EXTRACTOR
#ifndef DVTX_SOURCE_FILE
#undef LAYOUT_ELEMENT_TYPES
#endif // !DVTX_SOURCE_FILE
