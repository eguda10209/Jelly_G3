#pragma once

#include <vcclr.h>

#using "ScpDriverInterface.dll"
using namespace ScpDriverInterface;

static class XBoxController
{
private:
	static int ControllerNumber;
	static gcroot<ScpBus^> Bus;
	static gcroot<X360Controller^> Controller;
public:
	static void PlugIn();
	static void Unplug();
	static void UnplugAll();
	static void Press(X360Buttons buttons);
};

