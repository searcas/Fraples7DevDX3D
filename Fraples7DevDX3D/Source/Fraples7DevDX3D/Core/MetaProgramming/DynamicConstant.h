#pragma once
#include <cassert>
#include <DirectXMath.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include <type_traits>

#define RESOLVE_BASE(eltype) \
virtual size_t Resolve ## eltype() const noexcept(!IS_DEBUG) \
{ \
	assert( false && "Cannot resolve to" #eltype ); return 0u; \
}

#define LEAF_ELEMENT(eltype, systype) \
class eltype : public LayoutElement \
{ \
public: \
	using SystemType = systype;\
	using LayoutElement::LayoutElement; \
	size_t Resolve ## eltype() const noexcept(!IS_DEBUG) override final \
	{ \
		return GetOffsetBegin(); \
	} \
	size_t GetOffsetEnd() const noexcept override final \
	{ \
		return GetOffsetBegin() + sizeof( systype ); \
	} \
};

#define REF_CONVERSION(eltype)\
operator eltype::SystemType&() noexcept(!IS_DEBUG)\
{\
	return *reinterpret_cast<eltype::SystemType*>(pBytes + _mOffset + pLayout->Resolve ## eltype());\
}\
eltype::SystemType& operator=( const eltype::SystemType& rhs ) noexcept(!IS_DEBUG)\
{\
	return static_cast<eltype::SystemType&>(*this) = rhs;\
}


namespace FraplesDev
{
	namespace  MP
	{
		class Struct;
		class Array;
		class LayoutElement
		{
		public:
			LayoutElement(size_t offset)
				:
				offset(offset)
			{}
			virtual LayoutElement& operator[](const char*)
			{
				assert(false && "cannot access member on non Struct");
				return *this;
			}
			virtual const LayoutElement& operator[](const char* key) const
			{
				assert(false && "cannot access member on non Struct");
				return *this;
			}
			virtual LayoutElement& T()
			{
				assert(false);
				return *this;
			}
			virtual const LayoutElement& T()const
			{
				assert(false);
				return *this;
			}
			size_t GetOffsetBegin() const noexcept
			{
				return offset;
			}
			virtual size_t GetOffsetEnd() const noexcept = 0;
			size_t GetSizeInBytes()const noexcept
			{
				return GetOffsetEnd() - GetOffsetBegin();
			}
			template <typename T>
			Struct& Add(const std::string& key)noexcept(!IS_DEBUG);
			template<typename T>
			Array& Set(size_t size)noexcept(!IS_DEBUG);
			RESOLVE_BASE(Matrix)
			RESOLVE_BASE(Float4)
			RESOLVE_BASE(Float3)
			RESOLVE_BASE(Float2)
			RESOLVE_BASE(Float)
			RESOLVE_BASE(Bool)
		private:
			size_t offset;
		};

		LEAF_ELEMENT(Matrix, DirectX::XMFLOAT4X4)
		LEAF_ELEMENT(Float4, DirectX::XMFLOAT4)
		LEAF_ELEMENT(Float3, DirectX::XMFLOAT3)
		LEAF_ELEMENT(Float2, DirectX::XMFLOAT2)
		LEAF_ELEMENT(Float, float)
		LEAF_ELEMENT(Bool, BOOL)
			
		class Struct : public LayoutElement
		{
		public:
			using LayoutElement::LayoutElement;
			LayoutElement& operator[](const char* key) override final
			{
				return *map.at(key);
			}
			const LayoutElement& operator[](const char* key) const override final
			{
				return *map.at(key);
			}
			size_t GetOffsetEnd() const noexcept override final
			{
				return elements.empty() ? GetOffsetBegin() : elements.back()->GetOffsetEnd();
			}
			template<typename T>
			Struct& Add(const std::string& name) noexcept(!IS_DEBUG)
			{
				elements.push_back(std::make_unique<T>(GetOffsetEnd()));
				if (!map.emplace(name, elements.back().get()).second)
				{
					assert(false);
				}
				return *this;
			}
		private:
			std::unordered_map<std::string, LayoutElement*> map;
			std::vector<std::unique_ptr<LayoutElement>> elements;
		};
		class Array : public LayoutElement
		{
		public:
			using LayoutElement::LayoutElement;
			size_t GetOffsetEnd()const noexcept override final
			{
				assert(_mPElement);
				return GetOffsetBegin() + _mPElement->GetSizeInBytes() * size;
			}
			template<typename T>
			Array& Set(size_t size_in)noexcept(!IS_DEBUG)
			{
				_mPElement = std::make_unique<T>(GetOffsetBegin());
				size = size_in;
				return *this;
			}
			LayoutElement& T()override final
			{
				return *_mPElement;
			}
			const LayoutElement& T()const override final
			{
				return *_mPElement;
			}
		private:
			size_t size = 0u;
			std::unique_ptr<LayoutElement>_mPElement;
		};
		class ElementRef
		{
		public:
			ElementRef(const LayoutElement* pLayout, char* pBytes, size_t offset)
				:
				_mOffset(offset),
				pLayout(pLayout),
				pBytes(pBytes)
			{}
			ElementRef operator[](const char* key) noexcept(!IS_DEBUG)
			{
				return { &(*pLayout)[key],pBytes,_mOffset };
			}
			ElementRef operator[](size_t index) noexcept(!IS_DEBUG)
			{
				const auto& t = pLayout->T();
				return { &t,pBytes,_mOffset + t.GetSizeInBytes() * index };
			}

			REF_CONVERSION(Matrix)
			REF_CONVERSION(Float4)
			REF_CONVERSION(Float3)
			REF_CONVERSION(Float2)
			REF_CONVERSION(Float)
			REF_CONVERSION(Bool)
				
		private:
			size_t _mOffset;
			const class LayoutElement* pLayout;
			char* pBytes;
		};

		class Buffer
		{
		public:
			Buffer(const Struct& pLayout)
				:
				pLayout(&pLayout),
				bytes(pLayout.GetOffsetEnd())
			{}
			ElementRef operator[](const char* key) noexcept(!IS_DEBUG)
			{
				return { &(*pLayout)[key],bytes.data(),0u };
			}
		private:
			const class Struct* pLayout;
			std::vector<char> bytes;
		};

		//must come after Definitions of Struct and Array
		template<typename T>
		Struct& LayoutElement::Add(const std::string& key) noexcept(!IS_DEBUG)
		{
			auto ps = dynamic_cast<Struct*>(this);
			assert(ps != nullptr);
			return ps->Add<T>(key);
		}
		template<typename T>
		Array& LayoutElement::Set(size_t size)noexcept(!IS_DEBUG)
		{
			auto pa = dynamic_cast<Array*>(this);
			assert(pa != nullptr);
			return pa->Set<T>(size);
		}
	}
}
