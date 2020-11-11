#include "FraplesException.h"
#include <sstream>
namespace FraplesDev {

	FraplesException::FraplesException(int line, const char* file) noexcept :
		_mLine(line),_mFile(file) 
	{
	}

	const char* FraplesException::what() const noexcept
	{
		std::ostringstream oss;
		oss << GetType() << std::endl << GetOriginString();
		_mWhatBuffer = oss.str();
		return _mWhatBuffer.c_str();
	}

	const char* FraplesException::GetType() const noexcept
	{
		return "Fraples Exception";
	}

	int FraplesException::GetLine() const noexcept
	{
		return _mLine;
	}

	const std::string& FraplesException::GetFile() const noexcept
	{
		return _mFile;
	}

	std::string FraplesException::GetOriginString() const noexcept
	{
		std::ostringstream oss;
		oss << "[File] " << _mFile << std::endl << "[Line] " << _mLine;
		return oss.str();
	}
}