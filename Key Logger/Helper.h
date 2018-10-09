#pragma once
#define _CRT_SECURE_NO_WARNINGS
//NOTE INCLUDE GUARD NEEDED

#include<ctime>
#include<string>
#include<sstream>
#include<fstream>


namespace HelperFunc {

	//NOTE can make changes for ToString which return string of specified length like 001
	template<typename T>
	std::string ToString(const T &);

	struct DateTime {

		DateTime() {
			time_t ms;
			time(&ms);

			tm *info = localtime(&ms);
			
			day = info->tm_mday;
			month = info->tm_mon + 1;
			year = 1900 + info->tm_year;

			hour = info->tm_hour;
			min = info->tm_min;
			sec = info->tm_sec;
		}

		DateTime(int _day, int _month, int _year, int _hour, int _min, int _sec)
			:day(_day), month(_month), year(_year), hour(_hour), min(_min), sec(_sec) {}

		DateTime(int _day, int _month, int _year)
			:day(_day), month(_month), year(_year), hour(0), min(0), sec(0) {}

		DateTime Now()const {
			return DateTime();
		}

		std::string GetDateString(const std::string &sep = "\\")const {

			return ToString(day) + sep + ToString(month) + sep + ToString(year);
		}

		std::string GetTimeString(const std::string &sep = ":") const{
			return ToString(hour) + sep + ToString(min) + sep + ToString(sec);
		}

		std::string GetDateTimeString(const std::string &sep = ":")const {
			return GetDateString(sep) + " " + GetTimeString(sep);
		}


		int day, month, year;
		int hour, min, sec;
	};

	template<typename T> 
	std::string ToString(const T &e) {
		std::ostringstream s;
		s << e;

		return s.str();
	}

	void WriteAppLog(const std::string &s) {
		std::ofstream file("AppLog.txt",std::ios::app);

		file << "{ " << DateTime().GetDateTimeString() << " } \n"
			<< s << "\n";
		file.close();


	}

}
