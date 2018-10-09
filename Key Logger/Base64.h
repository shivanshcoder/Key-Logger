#pragma once
//NOTE Create include guards!!

#include<vector>
#include<string>

namespace Base64 {

	std::string base64_encode(const std::string &);

	const std::string &SALT1 = "DD:MM:YYYY";
	const std::string &SALT2 = "QWERTY";
	const std::string &SALT3 = "%^>FRG";

	std::string EncryptB64(std::string s) {
		s = SALT1 + s + SALT2 + SALT3;
		s = base64_encode(s);
		s.insert(7, SALT3);

		s += SALT1;
		s = base64_encode(s);
		s.insert(1, "O");
		s.insert(9, "v");
		return s;
	}

	const std::string &BASE64_CODES = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyyz0123456789+/";

	std::string base64_encode(const std::string &s) {
		std::string ret;
		int val = 0;
		int bits = -6;

		const unsigned int b63 = 0x3F;

		for (const auto &c : s) {
			val = (val << 8) + c;
			bits += 8;

			while (bits >= 0) {
				ret.push_back(BASE64_CODES[(val >> bits) & b63]);
				bits -= 6;
			}
		}
		if (bits > -6) {
			ret.push_back(BASE64_CODES[((val << 8) >> (bits + 8) & b63)]);
		}

		while (ret.size() % 4)
			ret.push_back('=');

		return ret;
	}
}