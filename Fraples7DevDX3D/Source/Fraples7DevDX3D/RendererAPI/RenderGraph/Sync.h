#pragma once
#include "Source.h"
#include "BufferResource.h"
#include <memory>
#include <sstream>
#include <string>
namespace FraplesDev
{
	class Sync
	{
	public:
		const std::string& GetRegisteredName()const noexcept;
		const std::string& GetPassName()const noexcept;
		const std::string& GetSourceName()const noexcept;
		void SetTarget(std::string passName, std::string outputName);
		virtual void Bind(class Source&) = 0;
		virtual void PostLinkValidate()const = 0;
		virtual ~Sync() = default;
	protected:
		Sync(std::string registeredName);
	private:
		std::string _mRegisteredName;
		std::string _mPassName;
		std::string _mOutputName;
	};

	template<typename T>
	class DirectBufferSnyc : public Sync
	{
		static_assert(std::is_base_of_v<BufferResource, T>, "DirectBufferSnyc target type must be a BufferResource type");
	public:
		static std::unique_ptr<Sync>Make(std::string registerName, std::shared_ptr<T>& target)
		{
			return std::make_unique<DirectBufferSnyc>(std::move(registerName), target);
		}
		void PostLinkValidate()const override
		{
			if (!_mLinked)
			{
				throw RGC_EXCEPTION("Unlinked input: " + GetRegisteredName());
			}

		}
		void Bind(Source& out)override
		{
			auto p = std::dynamic_pointer_cast<T>(out.YieldBuffer());
			if (!p)
			{
				std::ostringstream oss;
				oss << "Binding Input [" << GetRegisteredName() << "] to ouput [" << GetPassName() << "." << GetSourceName() << "]"
					<< " { " << typeid(T).name() << " } not compatible with {" << typeid(*out.YieldBuffer().get()).name() << "}";
				throw RGC_EXCEPTION(oss.str());
			}
			_mTarget = std::move(p);
			_mLinked = true;
		}
		DirectBufferSnyc(std::string registeredName, std::shared_ptr<T>& bind)
			:Sync(std::move(registeredName)), _mTarget(bind)
		{

		}

	private:
		std::shared_ptr<T>& _mTarget;
		bool _mLinked = false;
	};

	template <typename T>
	class DirectContextSync : public Sync
	{
		static_assert(std::is_base_of_v<GfxContext, T>, "DirectContextSync target type must be a from a GfxContext");
	public:
		static std::unique_ptr<Sync>Make(std::string registeredName, std::shared_ptr<T>& target)
		{
			return std::make_unique<DirectContextSync>(std::move(registeredName), target);
		}

		void PostLinkValidate()const override
		{
			if (!_mLinked)
			{
				throw RGC_EXCEPTION("Unlinked input: " + GetRegisteredName());
			}
		}
		void Bind(Source& out)override
		{
			auto p = std::dynamic_pointer_cast<T>(out.YieldBindable());
			if (!p)
			{
				std::ostringstream oss;
				oss << "Binding input [" << GetRegisteredName() << "] to output [" << GetPassName() << "." << GetSourceName() << "]"
					<< "{ " << typeid(T).name() << " } does not match { " << typeid(*out.YieldBindable().get()).name() << " }";
				throw RGC_EXCEPTION(oss.str());
			}
			_mTarget = std::move(p);
			_mLinked = true;
		}
		DirectContextSync(std::string registeredName, std::shared_ptr<T>& target)
			:Sync(std::move(registeredName)), _mTarget(target)
		{

		}
	private:
		std::shared_ptr<T>& _mTarget;
		bool _mLinked = false;
	};
}
