#pragma once
#include <string>
#include <memory>
#include "Exception/RenderGraphCompileException.h"

namespace FraplesDev
{
	class PassOutput
	{
	public:
		const std::string& GetName()const noexcept;
		virtual void PostLinkValidation() const = 0;
		virtual std::shared_ptr<class GfxContext>YieldImmutable();
		virtual std::shared_ptr<class BufferResource>YieldBuffer();
		virtual ~PassOutput() = default;
	protected:
		PassOutput(std::string name);
	private:
		std::string _mName;
	};

	template<typename T>
	class BufferOutput : public PassOutput
	{
	public:
		static std::unique_ptr<BufferOutput>Make(std::string name, std::shared_ptr<T>& buffer)
		{
			return std::make_unique<BufferOutput>(std::move(name), buffer);
		}
		BufferOutput(std::string name, std::shared_ptr<T>& buffer)
			:PassOutput(std::move(name)), _mBuffer(buffer)
		{

		}
		void PostLinkValidation()const override
		{}
		std::shared_ptr<BufferResource>YieldBuffer()
		{
			if (_mLinked)
			{
				throw RGC_EXCEPTION("Mutable output bound twice: " + GetName());
			}
			_mLinked = true;
			return _mBuffer;
		}
		std::shared_ptr<T>&_mBuffer;
		bool _mLinked = false;
	};
		template<typename T>
		class ImmutableOutput : public PassOutput
		{
		public:
			static std::unique_ptr<ImmutableOutput>Make(std::string name, std::shared_ptr<T>& buffer)
			{
				return std::make_unique<ImmutableOutput>(std::move(name), buffer);
			}
			ImmutableOutput(std::string name, std::shared_ptr<T>& bind)
				: PassOutput(std::move(name)), _mBind(bind)
			{

			}
			void PostLinkValidate()const 
			{}
			std::shared_ptr<GfxContext>YieldImmutable()
			{
				_mBind;
			}
	private:
		std::shared_ptr<T>& _mBind;
	};
}
