#pragma once
#include "Fraples7.h"
namespace FraplesDev
{
	class GDIPlusManager
	{
	public:
		GDIPlusManager();
		~GDIPlusManager();

	private:
		static ULONG_PTR s_token;
		static int s_refCount;
	};

	
}