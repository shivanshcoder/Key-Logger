
#include<iostream>
#include"Base64.h"
#include"Helper.h"
#include"KeyConstants.h"
#include"IO.h"
#include<Windows.h>

using namespace std;

void setData() {

	ifstream in("input.txt");
	ofstream out("output.txt");
	/*
	{ code, {"", "  "}},
	*/
	string s1, s2, s3;
	string line;
	char buffer[32];
	while (in) {
		in >> s1 >> s2;
		//std::getline(in, buffer,32,"\n");
		std::getline(in, s3);
		//s3[0] = " ";
		s3.replace(0, 1, "");
		//in.getline()
		out << "{ " << s2 << ", {\"[" << s1 << "]\", \"[" << s3 << "]\"}},\n";
	}

}

int main() {

	MSG msg;
	//while (GetMessage(&msg, NULL, 0, 0)) {
	//	TranslateMessage(&msg);
	//	DispatchMessage(&msg);
	//}
	//std::cout << "MAIN()";
	return 0;
}