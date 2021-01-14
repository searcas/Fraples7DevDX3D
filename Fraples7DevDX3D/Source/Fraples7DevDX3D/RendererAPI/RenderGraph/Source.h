#pragma once
#include <string>
#include <memory>
#include "Exception/RenderGraphCompileException.h"

namespace FraplesDev
{
	class Source
	{
	public:
		const std::string& GetName()const noexcept;
		virtual void PostLinkValidation() const = 0;
		virtual std::shared_ptr<class GfxContext>YieldBindable();
		virtual std::shared_ptr<class BufferResource>YieldBuffer();
		virtual ~Source() = default;
	protected:
		Source(std::string name);
	private:
		std::string _mName;
	};

	template<typename T>
	class DirectBufferSource : public Source
	{
	public:
		static std::unique_ptr<DirectBufferSource>Make(std::string name, std::shared_ptr<T>& buffer)
		{
			return std::make_unique<DirectBufferSource>(std::move(name), buffer);
		}
		DirectBufferSource(std::string name, std::shared_ptr<T>& buffer)
			:Source(std::move(name)), _mBuffer(buffer)
		{

		}
		void PostLinkValidation()const override
		{}
		std::shared_ptr<BufferResource>YieldBuffer() override
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
		class DirectContextSource : public Source
		{
		public:
			static std::unique_ptr<DirectContextSource>Make(std::string name, std::shared_ptr<T>& buffer)
			{
				return std::make_unique<DirectContextSource>(std::move(name), buffer);
			}
			DirectContextSource(std::string name, std::shared_ptr<T>& bind)
				: Source(std::move(name)), _mBind(bind)
			{

			}
			void PostLinkValidation()const override
			{}
			std::shared_ptr<GfxContext>YieldBindable() override
			{
				return _mBind;
			}
	private:
		std::shared_ptr<T>& _mBind;
	};
}
