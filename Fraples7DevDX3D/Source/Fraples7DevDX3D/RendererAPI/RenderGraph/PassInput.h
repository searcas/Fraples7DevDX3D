#pragma once
#include <string>
namespace FraplesDev
{
	class PassInput
	{
	public:
		const std::string& GetRegisteredName()const noexcept;
		const std::string& GetPassName()const noexcept;
		const std::string& GetOutputName()const noexcept;
		void SetTarget(std::string passName, std::string outputName);
		virtual void Bind(class PassOuput&) = 0;
		virtual void PostLinkValidate()const = 0;
		virtual ~PassInput() = default;
	protected:
		PassInput(std::string registeredName);
	private:
		std::string _mRegisteredName;
		std::string _mPassName;
		std::string _mOutputName;
	};

	template<typename T>
	class BufferInput : public PassInput
	{
		static_assert(std::is_base_of_v<BufferResource, T>, "BufferInput target type must be a BufferResource type");
	public:
		static std::unique_ptr<PassInput>Make(std::string registerName, std::shared_ptr<T>& target)
		{
			return std::make_unique<BufferInput>(std::move(registerName), target);
		}
		void PostLinkValidate()const override
		{
			if (!_mLinked)
			{
				throw RGC_EXCEPTION("Unlinked input: " + GetRegisteredName());
			}

		}
		void Bind(PassOutput& out)override
		{
			auto p = std::dynamic_pointer_cast<T>(out.YieldBuffer());
			if (!p)
			{
				std::ostringstream oss;
				oss << "Binding Input [" << GetRegisteredName() << "] to ouput [" << GetPassName() << "." << GetOutputName() << "]"
					<< " { " << typeid(T).name() << " } not compatible with {" << typeid(*out.YieldBuffer().get()).name() << "}";
				throw RGC_EXCEPTION(oss.str());
			}
			_mTarget = std::move(p);
			_mLinked = true;
		}
		BufferInput(std::string registeredName, std::shared_ptr<T>& bind)
			:PassInput(std::move(registeredName)), _mTarget(bind)
		{

		}

	private:
		std::shared_ptr<T>& _mTarget;
		bool _mLinked = false;
	};

	template <typename T>
	class ImmutableInput : public PassInput
	{
		static_assert(std::is_base_of<GfxContext, T>, "ImmutableInput target type must be a from a GfxContext");

	public:
		static std::unique_ptr<PassInput>Make(std::string registeredName, std::shared_ptr<T>& target)
		{
			return std::make_unique<ImmutableInput>(std::move(registeredName), target);
		}
		void PostValidate()const override
		{
			if (!_mLinked)
			{
				throw RGC_EXCEPTION("Unlinked input: " + GetRegisteredName());
			}
		}
		void Bind(PassOutput& out)override
		{
			auto p = std::dynamic_pointer_cast<T>(out.YieldImmutable());
			if (!p)
			{
				std::ostringstream oss;
				oss << "Binding input [" << GetRegisteredName() << "] to output [" << GetPassName() << "." << GetOutputName() << "]"
					<< "{ " << typeid(T).name() << " } does not match { " << typeid(*out.YieldImmutable().get()).name() << " }";
				throw RGC_EXCEPTION(oss.str());
			}
			_mTarget = std::move(p);
		}
		ImmutableInput(std::string registeredName, std::shared_ptr<GfxContext>& target)
			:PassInput(std::move(registeredName)), _mTarget(target)
		{

		}
	private:
		std::shared_ptr<T>& _mTarget;
		bool _mLinked = false;
	};
}
