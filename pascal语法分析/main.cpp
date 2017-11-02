#include <iostream>
#include <fstream>
#include <string>

#include "PascalCheck.h"

using namespace std;

int main()
{
	string file_name = "test.txt";
	cout << "Only detect the pair of 'if then else' and 'begin end'"  << endl;
	cout << "Please input the file name(e.g. 'test.txt'):" << endl;
	ifstream pa_file;                           //打开文件以读取其内容
    cin >> file_name;
    pa_file.open(file_name.c_str());
	while(pa_file.is_open()==false){
        cout << "Couldn't find " << file_name << " ,please recheck the file name(e.g. 'test.txt')" << endl;
        cin >> file_name;
        pa_file.open(file_name.c_str());
	}

	PascalCheck checker;
	STR_LINE one_str;
	one_str.line = 0;

	while (getline(pa_file, one_str.str)) {     //获取一行string
		one_str.line++;                         //getline会自动移动到下一行
        checker.UpperToLower(one_str.str);      //大小写转换，因为pascal不区分大小写
		checker.AnalyseOneLine(one_str);        //分析每行字符串是否有关键词if/then/else/begin/end
	}
	checker.AnalyseRestStack();                 //最后检查栈底
	checker.PrintError();
    pa_file.close();
    return 0;
}
