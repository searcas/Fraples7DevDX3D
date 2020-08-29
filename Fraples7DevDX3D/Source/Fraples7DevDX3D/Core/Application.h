#pragma once

#include "../Platform/Windows/Window.h"
#include "../Core/Debugging/Timer.h"
class Application
{
public:
	Application(const char* name, int width, int height);
	Application();
	~Application();
	int StartApp();
private:
	void Update();
private:
	FraplesDev::Window* _mWin;
	FraplesDev::Timer _mTimer;
	float _mSpeedFactor = 1.0f;
};

