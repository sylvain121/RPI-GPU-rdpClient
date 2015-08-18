#include "controller.hpp"


class WindowsController : public Controller
{
public : 
	void setScreenCoordinates()
	{

		Monitor monitor;
		RECT screenCoordinates;
		int monitorCount = GetSystemMetrics(SM_CMONITORS);
		if (monitorCount > 1 && this->monitorID == -1) {
			cerr << "There are more than one monitor available, select which monitor to use with\n./server -monitor <n> <port>" << endl;

		}
		else {
			if (monitorID < 0 || monitorID >= monitor.monitors.size()) {
				cerr << "The chosen monitor " << monitorID << " is invalid, select from the following:\n";
				for (int i = 0; i<monitor.monitors.size(); i++) {
					RECT r = monitor.monitors[i];
					cerr << "Monitor " << i << ":" << "[" << r.left << " " << r.top << "," << r.bottom << " " << r.right << "]" << endl;
				}
			}
			screenCoordinates = monitor.monitors[monitorID];
		}
	}


private : 
	RECT screenCoordinates;


	void mouse_motion(SendStruct* s) {

		SetCursorPos(s->x + screenCoordinates.left, s->y + screenCoordinates.top);
	}

	void mouse_button_down(SendStruct* s)
	{
		INPUT input = { 0 };
		::ZeroMemory(&input, sizeof(INPUT));
		switch (s->button) {
		case 1: // left button

			input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
			break;
		case 2: // middle button
			input.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
			break;
		case 3: // third button
			input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
			break;
		case 4: // scroll up
			input.mi.dwFlags = MOUSEEVENTF_WHEEL;
			input.mi.mouseData = 100;
			break;
		case 5: // scroll down
			input.mi.dwFlags = MOUSEEVENTF_WHEEL;
			input.mi.mouseData = -100;
			break;
		}
		input.type = INPUT_MOUSE;
		::SendInput(1, &input, sizeof(INPUT));
	}

	void mouse_button_up(SendStruct* s)
	{
		INPUT input = { 0 };
		::ZeroMemory(&input, sizeof(INPUT));
		switch (s->button) {
		case 1: // left button
			input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
			break;
		case 2: // middle button
			input.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
			break;
		case 3: // third button
			input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
			break;
		}
		if (input.mi.dwFlags) {
			input.type = INPUT_MOUSE;
			::SendInput(1, &input, sizeof(INPUT));
		}
	}

	void key_down(SendStruct* s)
	{
		INPUT input = { 0 };
		::ZeroMemory(&input, sizeof(INPUT));
		input.type = INPUT_KEYBOARD;
		input.ki.wScan = s->keycode;
		input.ki.wVk = 0;
		input.ki.dwFlags = KEYEVENTF_UNICODE;
		::SendInput(1, &input, sizeof(INPUT));
	}

	void key_up(SendStruct* s)
	{
		INPUT input = { 0 };
		::ZeroMemory(&input, sizeof(INPUT));
		input.type = INPUT_KEYBOARD;
		input.ki.wScan = s->keycode;
		input.ki.wVk = 0;
		input.ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
		::SendInput(1, &input, sizeof(INPUT));
	}



};