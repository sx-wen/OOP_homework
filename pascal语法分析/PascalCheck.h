#pragma once
#include <iostream>
#include <stack>
#include <vector>

using namespace std;

struct STR_LINE {
	string str;
	int line;
};

class PascalCheck {
public:
	int GetKeyType(const string &str);
	void AnalyseOneLine(const STR_LINE &one_str);
	void AnalyseOneString(const STR_LINE &one_str);
	void UpperToLower(string &str);
	void AnalyseRestStack();
	void PrintError();
	stack<STR_LINE> str_line;
	vector<STR_LINE> err_line;

private:
	bool flag_else = 0;
	bool flag_end_ifthen = 0;
	STR_LINE old_str,err_tmp;

};

