#include "ModelException.h"
#include <sstream>

namespace FraplesDev
{
	ModelException::ModelException(std::string note, int line, const char* file)
		: FraplesException(line, file), note(std::move(note))
	{

	}
	const char* ModelException::what() const noexcept
	{
		std::ostringstream oss;
		oss << FraplesException::what() << std::endl << "[Note] " << GetNote();
		_mWhatBuffer = oss.str();
		return _mWhatBuffer.c_str();
	}
	const char* ModelException::GetType() const noexcept
	{
		return "Fraples7 Studio Engine Exception";
	}
	const std::string& ModelException::GetNote() const noexcept
	{
		return note;
	}
}