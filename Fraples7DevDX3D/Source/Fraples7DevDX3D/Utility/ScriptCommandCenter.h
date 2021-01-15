#pragma once
#include <vector>
#include <string>
#include "Core/Common/Exceptions/FraplesException.h"

namespace FraplesDev
{
	class ScriptCommander
	{
	public:
		class Completion : public FraplesException
		{
		public:
			Completion(const std::string& content)noexcept;
			const char* what()const noexcept override;
			const char* GetType()const noexcept override;
			
		private:
			std::string _mContent;
		};
		class Exception : public FraplesException
		{
		public:
			Exception(int line, const char* file, const std::string& script = "", const std::string& message = "")noexcept;
			const char* what()const noexcept override;
			const char* GetType()const noexcept override;
		private:
			std::string script;
			std::string message;
		};
	public:
		ScriptCommander(const std::vector<std::string>& args);
	private:
		void Publish(std::string path)const;
	};

}
