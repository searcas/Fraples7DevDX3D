#include "DynamicConstant.h"
#include <string>
#include <algorithm>
#include <cctype>
#include "RendererAPI/LayoutCodex.h"
#define DCB_RESOLVE_BASE(eltype) \
size_t LayoutElement::Resolve ## eltype() const noexcept(!IS_DEBUG) \
{ \
	assert( false && "Cannot resolve to" #eltype ); return 0u; \
}

#define DCB_LEAF_ELEMENT_IMPL(eltype,systype,hlslSize) \
size_t eltype::Resolve ## eltype() const noexcept(!IS_DEBUG) \
{ \
	return GetOffsetBegin(); \
} \
size_t eltype::GetOffsetEnd() const noexcept \
{ \
	return GetOffsetBegin() + ComputeSize(); \
} \
size_t eltype::Finalize( size_t offset_in )noexcept(!IS_DEBUG) \
{ \
	offset = offset_in; \
	return offset_in + ComputeSize(); \
} \
size_t eltype::ComputeSize() const noexcept(!IS_DEBUG) \
{ \
	return (hlslSize); \
}\
std::string eltype::GetSignature()const noexcept(!IS_DEBUG)\
{\
	return #eltype;\
}
#define DCB_LEAF_ELEMENT(eltype,systype) DCB_LEAF_ELEMENT_IMPL(eltype,systype,sizeof(systype))

#define DCB_REF_CONVERSION(reftype,eltype,...) \
reftype::operator __VA_ARGS__ eltype::SystemType&() noexcept(!IS_DEBUG) \
{ \
	return *reinterpret_cast<eltype::SystemType*>(pBytes + offset + pLayout->Resolve ## eltype()); \
}
#define DCB_REF_ASSIGN(reftype,eltype) \
eltype::SystemType& reftype::operator=( const eltype::SystemType& rhs ) noexcept(!IS_DEBUG) \
{ \
	return static_cast<eltype::SystemType&>(*this) = rhs; \
}
#define DCB_REF_NONCONST(reftype,eltype) DCB_REF_CONVERSION(reftype,eltype) DCB_REF_ASSIGN(reftype,eltype)
#define DCB_REF_CONST(reftype,eltype) DCB_REF_CONVERSION(reftype,eltype,const)

#define DCB_PTR_CONVERSION(reftype,eltype,...) \
reftype::Ptr::operator __VA_ARGS__ eltype::SystemType*() noexcept(!IS_DEBUG) \
{ \
	return &static_cast<__VA_ARGS__ eltype::SystemType&>(ref); \
}


namespace FraplesDev
{


	namespace MP
	{
		LayoutElement::~LayoutElement()
		{}
		LayoutElement& LayoutElement::operator[](const std::string&)noexcept(!IS_DEBUG)
		{
			assert(false && "cannot access member on non Struct");
			return *this;
		}
		const LayoutElement& LayoutElement::operator[](const std::string& key) const noexcept(!IS_DEBUG)
		{
			return const_cast<LayoutElement&>(*this)[key];
		}
		LayoutElement& LayoutElement::T()noexcept(!IS_DEBUG)
		{
			assert(false);
			return *this;
		}
		const LayoutElement& LayoutElement::T() const noexcept(!IS_DEBUG)
		{
			return const_cast<LayoutElement&>(*this).T();
		}
		size_t LayoutElement::GetOffsetBegin() const noexcept
		{
			return offset;
		}
		size_t LayoutElement::GetSizeInBytes() const noexcept
		{
			return GetOffsetEnd() - GetOffsetBegin();
		}
		size_t LayoutElement::GetNextBoundaryOffset(size_t offset) noexcept
		{
			return offset + (16u - offset % 16u) % 16u;
		}

		// This layout is a special layout returned whenever a Struct is indexed 
		// with a bad key, or an Empty indexed at all. Instead of merely causing an exception
		// when accesssing a non-existent member, returning an Empty allows the existence
		// of elements to be queried at runtime via Exists() which only returns false from an Empty()
		class Empty : public LayoutElement
		{
		public:
			size_t GetOffsetEnd()const noexcept override final
			{
				return 0u;
			}
			bool Exists()const noexcept override final
			{
				return false;
			}
			std::string GetSignature() const noexcept(!IS_DEBUG)
			{
				assert(false);
				return "";
			}
		protected:
			size_t Finalize(size_t offset_in)noexcept(!IS_DEBUG) override final
			{
				return 0u;
			}
			size_t ComputeSize()const noexcept(!IS_DEBUG) override final
			{
				return 0u;
			}
		private:
			size_t size = 0u;
			std::unique_ptr<LayoutElement>pElement;
		}emptyLayoutElement;

		DCB_RESOLVE_BASE(Matrix)
		DCB_RESOLVE_BASE(Float4)
		DCB_RESOLVE_BASE(Float3)
		DCB_RESOLVE_BASE(Float2)
		DCB_RESOLVE_BASE(Float)
		DCB_RESOLVE_BASE(Bool)



		DCB_LEAF_ELEMENT(Matrix, DirectX::XMFLOAT4X4)
		DCB_LEAF_ELEMENT(Float4, DirectX::XMFLOAT4)
		DCB_LEAF_ELEMENT(Float3, DirectX::XMFLOAT3)
		DCB_LEAF_ELEMENT(Float2, DirectX::XMFLOAT2)
		DCB_LEAF_ELEMENT(Float, float)
		DCB_LEAF_ELEMENT_IMPL(Bool, bool, 4u)



		LayoutElement& Struct::operator[](const std::string& key)noexcept(!IS_DEBUG)
		{
			const auto i = map.find(key);
			if (i==map.end())
			{
				return emptyLayoutElement;
			}
			return *i->second;
		}
		size_t Struct::GetOffsetEnd() const noexcept
		{
			// bump up to next boundary (because structs are multiple of 16 in size)
			return LayoutElement::GetNextBoundaryOffset(elements.back()->GetOffsetEnd());
		}
		bool ValidateSymbolName(const std::string& name)noexcept
		{
			//symbols can caontain alphanumeric and underscore, must not start with digit
			return !name.empty() && !std::isdigit(name.front()) &&
				std::all_of(name.begin(), name.end(), [](char c) 
				{return std::isalnum(c) || c == '_'; });
		}
		void Struct::Add(const std::string& name, std::unique_ptr<LayoutElement> pElement) noexcept(!IS_DEBUG)
		{
			assert(ValidateSymbolName(name) && "invalid symbol name in Struct");
			elements.push_back(std::move(pElement));
			//do not allow overwriting of existing symbol
			if (!map.emplace(name, elements.back().get()).second)
			{
				assert(false && "duplicate symbol name in Struct");
			}
		}
		size_t Struct::Finalize(size_t offset_in)noexcept(!IS_DEBUG)
		{
			assert(elements.size() != 0u);
			offset = offset_in;
			auto offsetNext = offset;
			for (auto& el : elements)
			{
				offsetNext = (*el).Finalize(offsetNext);
			}
			return GetOffsetEnd();
		}
		size_t Struct::ComputeSize() const noexcept(!IS_DEBUG)
		{
			// compute offsets of all elements by summing size+padding
			size_t offsetNext = 0u;
			for (auto& el : elements)
			{
				const auto elSize = el->ComputeSize();
				offsetNext += CalculatePaddingBeforeElement(offsetNext, elSize) + elSize;
			}
			// struct size must be multiple of 16 bytes
			return GetNextBoundaryOffset(offsetNext);
		}
		size_t Struct::CalculatePaddingBeforeElement(size_t offset, size_t size) noexcept
		{
			// advance to next boundary if straddling 16-byte boundary
			if (offset / 16u != (offset + size - 1) / 16u)
			{
				return GetNextBoundaryOffset(offset) - offset;
			}
			return offset;
		}



		size_t Array::GetOffsetEnd() const noexcept
		{
			// arrays are not packed in hlsl
			return GetOffsetBegin() + LayoutElement::GetNextBoundaryOffset(pElement->GetSizeInBytes()) * size;
		}
		void Array::Set(std::unique_ptr<LayoutElement> pElement_in, size_t size_in) noexcept(!IS_DEBUG)
		{
			pElement = std::move(pElement_in);
			size = size_in;
		}
		LayoutElement& Array::T()noexcept(!IS_DEBUG)
		{
			return *pElement;
		}
		const LayoutElement& Array::T() const
		{
			return const_cast<Array*>(this)->T();
		}
		std::string Array::GetSignature() const noexcept(!IS_DEBUG)
		{
			using namespace std::string_literals;
			return "Array:"s + std::to_string(size) + "{"s + T().GetSignature() + "}"s;
		}
		bool Array::IndexBounds(size_t index) const noexcept
		{
			return index < size;
		}
		size_t Array::Finalize(size_t offset_in)noexcept(!IS_DEBUG)
		{
			assert(size != 0u && pElement);
			offset = offset_in;
			pElement->Finalize(offset_in);
			return GetOffsetEnd();
		}
		size_t Array::ComputeSize() const noexcept(!IS_DEBUG)
		{
			// arrays are not packed in hlsl
			return LayoutElement::GetNextBoundaryOffset(pElement->ComputeSize()) * size;
		}



		Layout::Layout()noexcept
		{
			struct Enabler : public Struct{};
			pRoot = std::make_shared<Enabler>();
		}
		Layout::Layout(std::shared_ptr<LayoutElement> pRoot)noexcept
			:
			pRoot(std::move(pRoot))
		{}
		size_t Layout::GetSizeInBytes()const noexcept
		{
			return pRoot->GetSizeInBytes();
		}
		std::string Layout::GetSignature() const noexcept(!IS_DEBUG)
		{
			return pRoot->GetSignature();
		}
		LayoutElement& RawLayout::operator[](const std::string& key)noexcept(!IS_DEBUG)
		{
			return (*pRoot)[key];
		}

		std::shared_ptr<LayoutElement>RawLayout::DeliverRoot()noexcept
		{
			auto temp = std::move(pRoot);
			temp->Finalize(0u);
			*this = RawLayout();
			return std::move(temp);
		}
		void RawLayout::ClearRoot()noexcept
		{
			*this =  RawLayout();
		}
		CookedLayout::CookedLayout(std::shared_ptr<LayoutElement>pRoot)noexcept
			:Layout(std::move(pRoot))
		{

		}
		std::shared_ptr<LayoutElement>CookedLayout::RelinquishRoot()const noexcept
		{
			return std::move(pRoot);
		}
		std::shared_ptr<LayoutElement>CookedLayout::ShareRoot()const noexcept
		{
			return pRoot;
		}
		const LayoutElement& CookedLayout::operator[](const std::string& key) const noexcept(!IS_DEBUG)
		{
			return (*pRoot)[key];
		}

	



		ConstElementRef::Ptr::Ptr(ConstElementRef& ref)noexcept
			:
			ref(ref)
		{
		}
		DCB_PTR_CONVERSION(ConstElementRef, Matrix, const)
		DCB_PTR_CONVERSION(ConstElementRef, Float4, const)
		DCB_PTR_CONVERSION(ConstElementRef, Float3, const)
		DCB_PTR_CONVERSION(ConstElementRef, Float2, const)
		DCB_PTR_CONVERSION(ConstElementRef, Float, const)
		DCB_PTR_CONVERSION(ConstElementRef, Bool, const)
		ConstElementRef::ConstElementRef(const LayoutElement* pLayout, char* pBytes, size_t offset)noexcept
			:
			offset(offset),
			pLayout(pLayout),
			pBytes(pBytes)
		{}
		bool ConstElementRef::Exists()const noexcept
		{
			return pLayout->Exists();
		}
		ConstElementRef ConstElementRef::operator[](const std::string& key) noexcept(!IS_DEBUG)
		{
			return { &(*pLayout)[key],pBytes,offset };
		}
		ConstElementRef ConstElementRef::operator[](size_t index) noexcept(!IS_DEBUG)
		{
			const auto& t = pLayout->T();
			//previous call didn't fail assert implies that layout is Array
			assert(static_cast<const Array&>(*pLayout).IndexBounds(index));

			// arrays are not packed in hlsl
			const auto elementSize = LayoutElement::GetNextBoundaryOffset(t.GetSizeInBytes());
			return { &t,pBytes,offset + elementSize * index };
		}
		ConstElementRef::Ptr ConstElementRef::operator&() noexcept(!IS_DEBUG)
		{
			return { *this };
		}
		DCB_REF_CONST(ConstElementRef, Matrix)
		DCB_REF_CONST(ConstElementRef, Float4)
		DCB_REF_CONST(ConstElementRef, Float3)
		DCB_REF_CONST(ConstElementRef, Float2)
		DCB_REF_CONST(ConstElementRef, Float)
		DCB_REF_CONST(ConstElementRef, Bool)


		ElementRef::Ptr::Ptr(ElementRef& ref)noexcept
			:
			ref(ref)
		{}
		DCB_PTR_CONVERSION(ElementRef, Matrix)
		DCB_PTR_CONVERSION(ElementRef, Float4)
		DCB_PTR_CONVERSION(ElementRef, Float3)
		DCB_PTR_CONVERSION(ElementRef, Float2)
		DCB_PTR_CONVERSION(ElementRef, Float)
		DCB_PTR_CONVERSION(ElementRef, Bool)
			ElementRef::ElementRef(const LayoutElement* pLayout, char* pBytes, size_t offset)noexcept
			:
			offset(offset),
			pLayout(pLayout),
			pBytes(pBytes)
		{}
		ElementRef::operator ConstElementRef() const noexcept
		{
			return { pLayout,pBytes,offset };
		}
		bool ElementRef::Exists()const noexcept
		{
			return pLayout->Exists();
		}
		ElementRef ElementRef::operator[](const std::string& key) noexcept(!IS_DEBUG)
		{
			return { &(*pLayout)[key],pBytes,offset };
		}
		ElementRef ElementRef::operator[](size_t index) noexcept(!IS_DEBUG)
		{
			const auto& t = pLayout->T();
			//previous call didn't fail assert implies that layout is array
			assert(static_cast<const Array&>(*pLayout).IndexBounds(index));
			// arrays are not packed in hlsl
			const auto elementSize = LayoutElement::GetNextBoundaryOffset(t.GetSizeInBytes());
			return { &t,pBytes,offset + elementSize * index };
		}
		ElementRef::Ptr ElementRef::operator&() noexcept(!IS_DEBUG)
		{
			return { *this };
		}
		DCB_REF_NONCONST(ElementRef, Matrix)
		DCB_REF_NONCONST(ElementRef, Float4)
		DCB_REF_NONCONST(ElementRef, Float3)
		DCB_REF_NONCONST(ElementRef, Float2)
		DCB_REF_NONCONST(ElementRef, Float)
		DCB_REF_NONCONST(ElementRef, Bool)



		Buffer::Buffer(RawLayout&& lay)noexcept(!IS_DEBUG) 
			: Buffer(LayoutCodex::Resolve(std::move(lay))) 
		{
			
		}
		Buffer::Buffer(const CookedLayout& lay) noexcept(!IS_DEBUG)
			 : _mPLayoutRoot(lay.ShareRoot()),bytes(_mPLayoutRoot->GetOffsetEnd())
		{
		}
		Buffer::Buffer(CookedLayout&& lay)noexcept(!IS_DEBUG)
			: _mPLayoutRoot(lay.RelinquishRoot()), bytes(_mPLayoutRoot->GetOffsetEnd())
		{

		}
		Buffer::Buffer(const Buffer& buf)noexcept
			:_mPLayoutRoot(buf._mPLayoutRoot),bytes(buf.bytes)
		{

		}
		Buffer::Buffer(Buffer&& buf)noexcept
			:_mPLayoutRoot(std::move(buf._mPLayoutRoot))
		{

		}
		ElementRef Buffer::operator[](const std::string& key) noexcept(!IS_DEBUG)
		{
			return { &(*_mPLayoutRoot)[key],bytes.data(),0u };
		}
		ConstElementRef Buffer::operator[](const std::string& key) const noexcept(!IS_DEBUG)
		{
			return const_cast<Buffer&>(*this)[key];
		}
		const char* Buffer::GetData() const noexcept
		{
			return bytes.data();
		}
		size_t Buffer::GetSizeInBytes() const noexcept
		{
			return bytes.size();
		}
		const LayoutElement& Buffer::GetRootLayoutElement() const noexcept
		{
			return *_mPLayoutRoot;
		}
		void Buffer::CopyFrom(const Buffer& other)noexcept(!IS_DEBUG)
		{
			assert(&GetRootLayoutElement() == &other.GetRootLayoutElement());
			std::copy(other.bytes.begin(), other.bytes.end(), bytes.begin());
		}
		std::shared_ptr<LayoutElement> Buffer::ShareLayoutRoot() const noexcept
		{
			return _mPLayoutRoot;
		}
	
		std::string Struct::GetSignature() const noexcept(!IS_DEBUG)
		{
			using namespace std::string_literals;
			auto sig = "Struct{"s;
			for (const auto& el : elements)
			{
				//lookup element name in the map (reverse lookup)
				auto i = std::find_if(map.begin(), map.end(), [&el](const std::pair<std::string, LayoutElement*>& v)
					{
						return &*el == v.second;
					}
				);
				sig += i->first + ":"s + el->GetSignature();
			}
			sig += "}"s;
			return sig;
		}
}
}