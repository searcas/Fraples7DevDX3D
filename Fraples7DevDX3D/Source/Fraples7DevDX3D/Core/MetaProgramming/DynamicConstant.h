#pragma once
#include <cassert>
#include <DirectXMath.h>
#include <vector>
#include <memory>
#include <unordered_map>


// macro simplifies adding base virtual resolution function to layoutElement
// resolution functions are the system by which polymorhpic type accesses are validated
// the bases here all provide a default behaviour of failing assertion, then for example
// Float1 will overload ResolveFloat1() and provide an implementation that doesn't fail
#define DCB_RESOLVE_BASE(eltype) \
virtual size_t Resolve ## eltype() const noexcept(!IS_DEBUG);

// this allows metaclass templating for leaf layout types like Float1,bool etc.
// macro is used so that the names of the overloade ResolveXXX themselves can be
// generated automatically.
// See LayoutElement base class for function meanings
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
// These macros serve to simplify declaration of all the conversions / assignments
// that need to be declared for the shell reference types(ElementRef / ElementRef::Ptr)

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
		// this abstract class is the base of the Layout system that describes the structure
		// of a dynamic constant buffer. They layout is a kind of tree structure LayoutElements
		class LayoutElement
		{
			// friend relationships are used liberally trough the DyanmicConstant System
			// Instead of seeing the various classes in this system as encapsulated decoupled
			// Units, they must be viewed as aspect of the large monolithic system
			// the reason for the friend relationship is generally so that intermediate
			// classes that the client should not create can have their constructors made
			// private, Finalize() cannot be called on aribtrary LayoutElements, etc.
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
			//needed to further configure an array's type
			virtual LayoutElement& T()noexcept(!IS_DEBUG);
			const LayoutElement& T() const noexcept(!IS_DEBUG);

			// offset based- functions only work after finalization!
			size_t GetOffsetBegin() const noexcept;
			virtual size_t GetOffsetEnd() const noexcept = 0;
			// get size in bytes derived from offsets
			size_t GetSizeInBytes() const noexcept;

			// only works for Structs; add LayoutElement to struct
			template<typename T>
			LayoutElement& Add(const std::string& key) noexcept(!IS_DEBUG);
			// only works for Arrays; set the type and the # of elements
			template<typename T>
			LayoutElement& Set(size_t size) noexcept(!IS_DEBUG);

			// returns the value of offset bumped up to the next 16-byte boundary (if not already on one)
			static size_t GetNextBoundaryOffset(size_t offset) noexcept;

			// These are declarations virtual functions that vthe various leaf LayoutElement types
			// ill override for the purposes of runtime checking whether a type conversion is allowed
			DCB_RESOLVE_BASE(Matrix)
			DCB_RESOLVE_BASE(Float4)
			DCB_RESOLVE_BASE(Float3)
			DCB_RESOLVE_BASE(Float2)
			DCB_RESOLVE_BASE(Float)
			DCB_RESOLVE_BASE(Bool)
		protected:
			// sets all offsets for element and subelements, returns offset directly after this element
			virtual size_t Finalize(size_t offset)noexcept(!IS_DEBUG) = 0;
			// computes the size of this element in bytes, considering padding on Arrays and Structs
			virtual size_t ComputeSize() const noexcept(!IS_DEBUG) = 0;
		protected:
			// Each element stores it's own offset.
			// this makes lookup to find it's position in the byte buffer fast
			// Special Handling is required for situations where arrays are involved
			size_t offset = 0u;
		};


		// Declarations of the Leaf types, see macro at top
		DCB_LEAF_ELEMENT(Matrix, DirectX::XMFLOAT4X4)
		DCB_LEAF_ELEMENT(Float4, DirectX::XMFLOAT4)
		DCB_LEAF_ELEMENT(Float3, DirectX::XMFLOAT3)
		DCB_LEAF_ELEMENT(Float2, DirectX::XMFLOAT2)
		DCB_LEAF_ELEMENT(Float, float)
		// Bool is sepcial case, because it's size on GPU does not match CPU Size
		DCB_LEAF_ELEMENT_IMPL(Bool, bool, 4u)

			// Struct is the core concrete layout type, is essentially of map of
			// string => LayoutElement. See layoutElement for meanings of functions
		class Struct : public LayoutElement
		{
			friend LayoutElement;
		public:
			LayoutElement& operator[](const std::string& key)noexcept(!IS_DEBUG) override final;
			size_t GetOffsetEnd() const noexcept override final;
			std::string GetSignature()const noexcept(!IS_DEBUG) override final;

			void Add(const std::string& name, std::unique_ptr<LayoutElement> pElement) noexcept(!IS_DEBUG);
		protected:
			// Client should not construct elements directly
			Struct() = default;
			size_t Finalize(size_t offset_in) override final;
			size_t ComputeSize() const noexcept(!IS_DEBUG) override final;
		private:
			// Function to calculate padding according to the rules of HLSL structure packing
			static size_t CalculatePaddingBeforeElement(size_t offset, size_t size) noexcept;
		private:
			// Both map and vector are maintained for fast lookup + ordering
			// Elements should be ordered in the order they are added to struct
			std::unordered_map<std::string, LayoutElement*> map;
			std::vector<std::unique_ptr<LayoutElement>> elements;
		};

		// An array is much like a C-Array. Indexing by integer rather than string,
		// all elements must be same type. because each array element does not have it's own
		// LayoutElement, special processing is required for calculating fofsets. 
		// An array offset is added to the offsets stored in the LayoutElement. Nested arrays add their offsets
		class Array : public LayoutElement
		{
			friend LayoutElement;
		public:
			size_t GetOffsetEnd() const noexcept override final;
			void Set(std::unique_ptr<LayoutElement> pElement, size_t size_in) noexcept(!IS_DEBUG);
			LayoutElement& T() noexcept(!IS_DEBUG) override final;
			const LayoutElement& T()const noexcept(!IS_DEBUG);
			std::string GetSignature()const noexcept(!IS_DEBUG) override final;
			bool IndexBounds(size_t index)const noexcept;
		protected:
			// Client should not construct elements directly
			Array() = default;
			size_t Finalize(size_t offset_in) noexcept(!IS_DEBUG)override final;
			size_t ComputeSize() const noexcept(!IS_DEBUG) override final;
		private:
			size_t size = 0u;
			std::unique_ptr<LayoutElement> pElement;
		};

		// The layout class serves as a shell to hold the root of the LayoutElement tree
		// Client does not create LayoutElements directly, create layout and then use it
		// to access the elements and add on from there.
		// When bulding is done, tree is
		// finalized(all offsets calculated) and a flag is set preventing further changes to the tree

		class Layout
		{
			friend class LayoutCodex;
			friend class Buffer;
		public:
			Layout()noexcept;
			LayoutElement& operator[](const std::string& key)noexcept(!IS_DEBUG);
			size_t GetSizeInBytes() const noexcept;
			template<typename T>
			LayoutElement& Add(const std::string& key) noexcept(!IS_DEBUG)
			{
				assert(!finalized && "cannot modify finalized layout");
				return pLayout->Add<T>(key);
			}
			std::string GetSignature()const noexcept(!IS_DEBUG);
			void Finalize()noexcept(!IS_DEBUG);
			bool IsFinalized()const noexcept;
		private:
			//this ctor creates FINALIZED layouts only(used by Codex to return layouts)
			Layout(std::shared_ptr<LayoutElement>pLayout)noexcept;
		private:
			// used by COdex and Buffer to get a shared ptr to the layout
			std::shared_ptr<LayoutElement>ShareRoot()const noexcept;
			bool finalized = false;
			std::shared_ptr<LayoutElement> pLayout;
		};

		// The Reference classes (ElementRef and ConstElementRef) from the shels for 
		// interfacing with a Buffer. Operations such as indexing[] reutrn further Ref objects
		// Ref objects overload assignment and converion so they can be used in expressions.
		// Typechecking is performed at runtime (via the ResolveXXX virtual methods in the LayoutElements)

		class ConstElementRef
		{
			friend class ElementRef;
			friend class Buffer;
		public:
			// A Ref can be further operated on by unary& to return a Ptr shell
			// Ptr shell objects convert to a ptr to the underlying type
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
				//ptr should only be constructable from a ref
				Ptr(ConstElementRef& ref)noexcept;
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
			// refs should only be constructable by other refs or by the buffer
			ConstElementRef(const LayoutElement* pLayout, char* pBytes, size_t offset)noexcept;
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
				// ptr should only be constructable from a ref
				Ptr(ElementRef& ref)noexcept;

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
			// refs should only be construtable by other refs or by the buffer
			ElementRef(const LayoutElement* pLayout, char* pBytes, size_t offset)noexcept;
		private:
			size_t offset;
			const class LayoutElement* pLayout;
			char* pBytes;
		};
		// The buffer object is a combination of a raw byte buffer with a LayoutElement
		// tree structure which acts as an view/interpretation/overlay for those bytes
		// operator [] indexes into the root Struct, returning a Ref shell that can be 
		// used to further inex if struct/array, returning further Ref shells, or used
		// to access the data stored in the buffer if a Leaf element type

		class Buffer
		{
		public:
			static Buffer Make(Layout& lay)noexcept(!IS_DEBUG);
			ElementRef operator[](const std::string& key) noexcept(!IS_DEBUG);
			ConstElementRef operator[](const std::string& key) const noexcept(!IS_DEBUG);
			const char* GetData() const noexcept;
			size_t GetSizeInBytes() const noexcept;
			const LayoutElement& GetLayout() const noexcept;
			std::shared_ptr<LayoutElement> ShareLayout() const noexcept;
			std::string GetSignature()const noexcept(!IS_DEBUG);
		private:
			Buffer(Layout& lay)noexcept;
			Buffer(Layout&& lay)noexcept;
		private:
			std::shared_ptr<LayoutElement>pLayout;
			std::vector<char> bytes;
		};


		// Template defition must be declared after Struct/Array have been defined
		// but still need to be presented in the header because : templates
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
			pa->Set(std::make_unique<Enabler>(), size);
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