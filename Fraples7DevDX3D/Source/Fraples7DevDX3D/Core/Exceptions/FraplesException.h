#pragma once
#include<exception>
#include <string>






namespace FraplesDev
{

	class FraplesException : public std::exception
	{
	public:
		FraplesException(int line, const char* file) noexcept;
		const char* what() const noexcept override;
		virtual const char* GetType()const noexcept;
		int GetLine() const noexcept;
		const std::string& GetFile() const noexcept;
		std::string GetOriginString() const noexcept;
	private:
		int	_mLine;
		std::string _mFile;
	protected:
		mutable std::string _mWhatBuffer;
	};

}