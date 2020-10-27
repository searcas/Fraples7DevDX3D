#pragma once
#include "../RendererAPI/RendererBase.h"
#include "BaseObject.h"
#include <random>


namespace FraplesDev
{
	class Sheet : public BaseObject
	{
	public:
		Sheet(Graphics& gfx, std::mt19937& rng,
			std::uniform_real_distribution<float>& adist,
			std::uniform_real_distribution<float>& ddist,
			std::uniform_real_distribution<float>& odist,
			std::uniform_real_distribution<float>& rdist);
	private:



}