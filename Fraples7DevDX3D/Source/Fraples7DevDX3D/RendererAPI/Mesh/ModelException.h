#pragma once
#include "Model.h"
#include "Core/Common/Exceptions/FraplesException.h"

namespace FraplesDev
{
	class ModelException : public FraplesException
	{
	public:
		ModelException(std::string note, int line, const char* file);
		const char* what() const noexcept override;
		const char* GetType()const noexcept override;
		const std::string& GetNote()const noexcept;
	private:
		std::string note = "";
	};
}