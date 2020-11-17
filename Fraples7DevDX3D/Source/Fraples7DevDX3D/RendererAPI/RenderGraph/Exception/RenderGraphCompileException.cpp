#include "RenderGraphCompileException.h"
#include <sstream>
RenderGraphCompileException::RenderGraphCompileException(std::string message, int line, const char* file) noexcept
	:FraplesException(line, file), _mMessage(std::move(message))
{
}

const char* RenderGraphCompileException::what() const noexcept
{
	std::stringstream ss;
	ss << FraplesException::what() << std::endl;
	ss << "[Message]" << std::endl;
	ss << _mMessage;
	_mWhatBuffer = ss.str();
	return _mWhatBuffer.c_str();
}

const char* RenderGraphCompileException::GetType() const noexcept
{
	return "Render Graph Compile Exception";
}

const std::string& RenderGraphCompileException::GetMessage() const noexcept
{
	return _mMessage;
}
