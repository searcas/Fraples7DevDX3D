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


}
