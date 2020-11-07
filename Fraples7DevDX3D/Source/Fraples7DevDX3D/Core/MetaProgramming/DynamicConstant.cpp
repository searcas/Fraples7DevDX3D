#include "DynamicConstant.h"
#include <string>
#include <algorithm>

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
size_t eltype::Finalize( size_t offset_in ) \
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
		LayoutElement& LayoutElement::operator[](const std::string&)
		{
			assert(false && "cannot access member on non Struct");
			return *this;
		}
		const LayoutElement& LayoutElement::operator[](const std::string& key) const
		{
			return const_cast<LayoutElement&>(*this)[key];
		}
		LayoutElement& LayoutElement::T()
		{
			assert(false);
			return *this;
		}
		const LayoutElement& LayoutElement::T() const
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
			size_t Finalize(size_t offset_in)override final
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



		DCB_LEAF_ELEMENT(Matrix, dx::XMFLOAT4X4)
		DCB_LEAF_ELEMENT(Float4, dx::XMFLOAT4)
		DCB_LEAF_ELEMENT(Float3, dx::XMFLOAT3)
		DCB_LEAF_ELEMENT(Float2, dx::XMFLOAT2)
		DCB_LEAF_ELEMENT(Float, float)
		DCB_LEAF_ELEMENT_IMPL(Bool, bool, 4u)



		LayoutElement& Struct::operator[](const std::string& key)
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
		void Struct::Add(const std::string& name, std::unique_ptr<LayoutElement> pElement) noexcept(!IS_DEBUG)
		{
			elements.push_back(std::move(pElement));
			if (!map.emplace(name, elements.back().get()).second)
			{
				assert(false);
			}
		}
		size_t Struct::Finalize(size_t offset_in)
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
		LayoutElement& Array::T()
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
		size_t Array::Finalize(size_t offset_in)
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



		Layout::Layout()
			:
			pLayout(std::make_shared<Struct>())
		{}
		Layout::Layout(std::shared_ptr<LayoutElement> pLayout)
			:
			pLayout(std::move(pLayout))
		{}
		LayoutElement& Layout::operator[](const std::string& key)
		{
			assert(!finalized && "cannot modify finalized layout");
			return (*pLayout)[key];
		}
		size_t Layout::GetSizeInBytes() const noexcept
		{
			return pLayout->GetSizeInBytes();
		}
		std::shared_ptr<LayoutElement> Layout::Finalize()
		{
			pLayout->Finalize(0);
			finalized = true;
			return pLayout;
		}

		std::string Layout::GetSignature() const noexcept(!IS_DEBUG)
		{
			return pLayout->GetSignature();
		}



		ConstElementRef::Ptr::Ptr(ConstElementRef& ref)
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
		ConstElementRef::ConstElementRef(const LayoutElement* pLayout, char* pBytes, size_t offset)
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


		ElementRef::Ptr::Ptr(ElementRef& ref)
			:
			ref(ref)
		{}
		DCB_PTR_CONVERSION(ElementRef, Matrix)
		DCB_PTR_CONVERSION(ElementRef, Float4)
		DCB_PTR_CONVERSION(ElementRef, Float3)
		DCB_PTR_CONVERSION(ElementRef, Float2)
		DCB_PTR_CONVERSION(ElementRef, Float)
		DCB_PTR_CONVERSION(ElementRef, Bool)
			ElementRef::ElementRef(const LayoutElement* pLayout, char* pBytes, size_t offset)
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




		Buffer::Buffer(Layout& lay)
			:pLayout(std::static_pointer_cast<Struct>(lay.Finalize())), bytes(pLayout->GetOffsetEnd())
		{}
		ElementRef Buffer::operator[](const std::string& key) noexcept(!IS_DEBUG)
		{
			return { &(*pLayout)[key],bytes.data(),0u };
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
		const LayoutElement& Buffer::GetLayout() const noexcept
		{
			return *pLayout;
		}
		std::shared_ptr<LayoutElement> Buffer::CloneLayout() const
		{
			return pLayout;
		}
		std::string Buffer::GetSignature() const noexcept(!IS_DEBUG)
		{
			return pLayout->GetSignature();
		}
		std::string Struct::GetSignature() const noexcept(!IS_DEBUG)
		{
			using namespace std::string_literals;
			auto sig = "Struct{"s;
			for (const auto& el : elements)
			{
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