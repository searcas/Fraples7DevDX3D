#pragma once
#include <cassert>
#include <DirectXMath.h>
#include <vector>
#include <memory>
#include <unordered_map>

#define DCB_RESOLVE_BASE(eltype) \
virtual size_t Resolve ## eltype() const noexcept(!IS_DEBUG);

#define DCB_LEAF_ELEMENT_IMPL(eltype,systype,hlslSize) \
class eltype : public LayoutElement \
{ \
	friend LayoutElement;\
public: \
	using SystemType = systype; \
	size_t Resolve ## eltype() const noexcept(!IS_DEBUG) override final;\
	size_t GetOffsetEnd() const noexcept override final;\
	std::string GetSignature()const noexcept(!IS_DEBUG) override final;\
protected: \
	size_t Finalize( size_t offset_in ) override final;\
	size_t ComputeSize() const noexcept(!IS_DEBUG) override final;\
};
#define DCB_LEAF_ELEMENT(eltype,systype) DCB_LEAF_ELEMENT_IMPL(eltype,systype,sizeof(systype))

#define DCB_REF_CONVERSION(eltype,...) \
operator __VA_ARGS__ eltype::SystemType&() noexcept(!IS_DEBUG);
#define DCB_REF_ASSIGN(eltype) \
eltype::SystemType& operator=( const eltype::SystemType& rhs ) noexcept(!IS_DEBUG);
#define DCB_REF_NONCONST(eltype) DCB_REF_CONVERSION(eltype) DCB_REF_ASSIGN(eltype)
#define DCB_REF_CONST(eltype) DCB_REF_CONVERSION(eltype,const)

#define DCB_PTR_CONVERSION(eltype,...) \
operator __VA_ARGS__ eltype::SystemType*() noexcept(!IS_DEBUG);


namespace FraplesDev
{ 
namespace MP
{
	namespace dx = DirectX;
	class LayoutElement
	{
		
		friend class Layout;
		friend class Array;
		friend class Struct;
	public:
		virtual ~LayoutElement();
		//get a string signature for this element(Recursive)
		virtual std::string GetSignature()const noexcept(!IS_DEBUG) = 0;

		inline virtual bool Exists()const noexcept
		{
			return true;
		}
		// [] only works for Structs; access member by name
		virtual LayoutElement& operator[](const std::string&);
		const LayoutElement& operator[](const std::string& key) const;
		// T() only works for Arrays; gets the array type layout object
		virtual LayoutElement& T();
		const LayoutElement& T() const;

		// offset based- functions only work after finalization!
		size_t GetOffsetBegin() const noexcept;
		virtual size_t GetOffsetEnd() const noexcept = 0;
		// get size in bytes derived from offsets
		size_t GetSizeInBytes() const noexcept;

		// only works for Structs; add LayoutElement
		template<typename T>
		LayoutElement& Add(const std::string& key) noexcept(!IS_DEBUG);
		// only works for Arrays; set the type and the # of elements
		template<typename T>
		LayoutElement& Set(size_t size) noexcept(!IS_DEBUG);

		// returns the value of offset bumped up to the next 16-byte boundary (if not already on one)
		static size_t GetNextBoundaryOffset(size_t offset) noexcept;

		DCB_RESOLVE_BASE(Matrix)
			DCB_RESOLVE_BASE(Float4)
			DCB_RESOLVE_BASE(Float3)
			DCB_RESOLVE_BASE(Float2)
			DCB_RESOLVE_BASE(Float)
			DCB_RESOLVE_BASE(Bool)
	protected:
		// sets all offsets for element and subelements, returns offset directly after this element
		virtual size_t Finalize(size_t offset) = 0;
		// computes the size of this element in bytes, considering padding on Arrays and Structs
		virtual size_t ComputeSize() const noexcept(!IS_DEBUG) = 0;
	protected:
		size_t offset = 0u;
	};


	DCB_LEAF_ELEMENT(Matrix, dx::XMFLOAT4X4)
	DCB_LEAF_ELEMENT(Float4, dx::XMFLOAT4)
	DCB_LEAF_ELEMENT(Float3, dx::XMFLOAT3)
	DCB_LEAF_ELEMENT(Float2, dx::XMFLOAT2)
	DCB_LEAF_ELEMENT(Float, float)
	DCB_LEAF_ELEMENT_IMPL(Bool, bool, 4u)


	class Struct : public LayoutElement
	{
		friend LayoutElement;
	public:
		LayoutElement& operator[](const std::string& key) override final;
		size_t GetOffsetEnd() const noexcept override final;
		std::string GetSignature()const noexcept(!IS_DEBUG) override final;

		void Add(const std::string& name, std::unique_ptr<LayoutElement> pElement) noexcept(!IS_DEBUG);
	protected:
		Struct() = default;
		size_t Finalize(size_t offset_in) override final;
		size_t ComputeSize() const noexcept(!IS_DEBUG) override final;
	private:
		static size_t CalculatePaddingBeforeElement(size_t offset, size_t size) noexcept;
	private:
		std::unordered_map<std::string, LayoutElement*> map;
		std::vector<std::unique_ptr<LayoutElement>> elements;
	};

	class Array : public LayoutElement
	{
		friend LayoutElement;
	public:
		size_t GetOffsetEnd() const noexcept override final;
		void Set(std::unique_ptr<LayoutElement> pElement, size_t size_in) noexcept(!IS_DEBUG);
		LayoutElement& T() override final;
		const LayoutElement& T()const;
		std::string GetSignature()const noexcept(!IS_DEBUG) override final;
		bool IndexBounds(size_t index)const noexcept;
	protected:
		Array() = default;
		size_t Finalize(size_t offset_in) override final;
		size_t ComputeSize() const noexcept(!IS_DEBUG) override final;
	private:
		size_t size = 0u;
		std::unique_ptr<LayoutElement> pElement;
	};



	class Layout
	{
		friend class LayoutCodex;
		friend class Buffer;
	public:
		Layout();
		//this ctor creates finalized layout's only
		Layout(std::shared_ptr<LayoutElement> pLayout);
		LayoutElement& operator[](const std::string& key);
		size_t GetSizeInBytes() const noexcept;
		template<typename T>
		LayoutElement& Add(const std::string& key) noexcept(!IS_DEBUG)
		{
			assert(!finalized && "cannot modify finalized layout");
			return pLayout->Add<T>(key);
		}
		std::string GetSignature()const noexcept(!IS_DEBUG);
		void Finalize();
		bool IsFinalized()const noexcept;
		std::shared_ptr<LayoutElement>ShareRoot()const noexcept;
	private:
		bool finalized = false;
		std::shared_ptr<LayoutElement> pLayout;
	};


	class ConstElementRef
	{
		friend class ElementRef;
		friend class Buffer;
	public:
		class Ptr
		{
			friend ConstElementRef;
		public:

			DCB_PTR_CONVERSION(Matrix, const)
			DCB_PTR_CONVERSION(Float4, const)
			DCB_PTR_CONVERSION(Float3, const)
			DCB_PTR_CONVERSION(Float2, const)
			DCB_PTR_CONVERSION(Float, const)
			DCB_PTR_CONVERSION(Bool, const)
		private:
			Ptr(ConstElementRef& ref);
			ConstElementRef& ref;
		};
	public:
		bool Exists()const noexcept;
		ConstElementRef operator[](const std::string& key) noexcept(!IS_DEBUG);
		ConstElementRef operator[](size_t index) noexcept(!IS_DEBUG);
		Ptr operator&() noexcept(!IS_DEBUG);

		DCB_REF_CONST(Matrix)
		DCB_REF_CONST(Float4)
		DCB_REF_CONST(Float3)
		DCB_REF_CONST(Float2)
		DCB_REF_CONST(Float)
		DCB_REF_CONST(Bool)
	private:
		ConstElementRef(const LayoutElement* pLayout, char* pBytes, size_t offset);
	private:
		size_t offset;
		const class LayoutElement* pLayout;
		char* pBytes;
	};

	class ElementRef
	{
		friend class Buffer;
	public:
		class Ptr
		{
			friend ElementRef;
		public:

			DCB_PTR_CONVERSION(Matrix)
				DCB_PTR_CONVERSION(Float4)
				DCB_PTR_CONVERSION(Float3)
				DCB_PTR_CONVERSION(Float2)
				DCB_PTR_CONVERSION(Float)
				DCB_PTR_CONVERSION(Bool)
		private:
			Ptr(ElementRef& ref);

			ElementRef& ref;
		};
	public:
		bool Exists()const noexcept;
		operator ConstElementRef() const noexcept;
		ElementRef operator[](const std::string& key) noexcept(!IS_DEBUG);
		ElementRef operator[](size_t index) noexcept(!IS_DEBUG);
		Ptr operator&() noexcept(!IS_DEBUG);

		DCB_REF_NONCONST(Matrix)
			DCB_REF_NONCONST(Float4)
			DCB_REF_NONCONST(Float3)
			DCB_REF_NONCONST(Float2)
			DCB_REF_NONCONST(Float)
			DCB_REF_NONCONST(Bool)
	private:
		ElementRef(const LayoutElement* pLayout, char* pBytes, size_t offset);
	private:
		size_t offset;
		const class LayoutElement* pLayout;
		char* pBytes;
	};


	class Buffer
	{
	public:
		static Buffer Make(Layout& lay)noexcept(!IS_DEBUG);
		ElementRef operator[](const std::string& key) noexcept(!IS_DEBUG);
		ConstElementRef operator[](const std::string& key) const noexcept(!IS_DEBUG);
		const char* GetData() const noexcept;
		size_t GetSizeInBytes() const noexcept;
		const LayoutElement& GetLayout() const noexcept;
		std::shared_ptr<LayoutElement> ShareLayout() const;
		std::string GetSignature()const noexcept(!IS_DEBUG);
	private:
		Buffer(Layout& lay);
		Buffer(Layout&& lay);
	private:
		std::shared_ptr<LayoutElement>pLayout;
		std::vector<char> bytes;
	};


	// must come after Definitions of Struct and Array
	template<typename T>
	LayoutElement& LayoutElement::Add(const std::string& key) noexcept(!IS_DEBUG)
	{
		auto ps = dynamic_cast<Struct*>(this);
		assert(ps != nullptr);
		//neeed to allow make_unique access to the ctor
		struct Enabler : public T {};
		ps->Add(key, std::make_unique<Enabler>());
		return *this;
	}

	template<typename T>
	LayoutElement& LayoutElement::Set(size_t size) noexcept(!IS_DEBUG)
	{
		auto pa = dynamic_cast<Array*>(this);
		assert(pa != nullptr);
		//neeed to allow make_unique access to the ctor
		struct Enabler : public T {};
		ps->Set(std::make_unique<Enabler>(), size);
		return *this;
	}
}
}
#undef DCB_RESOLVE_BASE
#undef DCB_LEAF_ELEMENT_IMPL
#undef DCB_LEAF_ELEMENT
#undef DCB_REF_CONVERSION
#undef DCB_REF_ASSIGN
#undef DCB_REF_NONCONST
#undef DCB_REF_CONST
#undef DCB_PTR_CONVERSION