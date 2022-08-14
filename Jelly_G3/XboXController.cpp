#include "XBoxController.h"

gcroot<ScpBus^> XBoxController::Bus = gcnew ScpBus;
gcroot<X360Controller^> XBoxController::Controller = gcnew X360Controller;
int XBoxController::ControllerNumber = -1;

void XBoxController::PlugIn()
{
	Unplug();
	int i = 1;
	while (!Bus->PlugIn(i))
	{
		++i;
		if (i > 10) return;
	}
	ControllerNumber = i;
}

void XBoxController::Unplug()
{
	if (ControllerNumber == -1) return;
	Bus->Unplug(ControllerNumber);
	ControllerNumber = -1;
}

void XBoxController::UnplugAll()
{
	Bus->UnplugAll();
	ControllerNumber = -1;
}

void XBoxController::Press(X360Buttons buttons)
{
	Controller->Buttons = buttons;
	Bus->Report(ControllerNumber, Controller->GetReport());
}
