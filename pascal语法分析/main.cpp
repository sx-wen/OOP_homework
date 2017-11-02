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
	ifstream pa_file;                           //���ļ��Զ�ȡ������
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

	while (getline(pa_file, one_str.str)) {     //��ȡһ��string
		one_str.line++;                         //getline���Զ��ƶ�����һ��
        checker.UpperToLower(one_str.str);      //��Сдת������Ϊpascal�����ִ�Сд
		checker.AnalyseOneLine(one_str);        //����ÿ���ַ����Ƿ��йؼ���if/then/else/begin/end
	}
	checker.AnalyseRestStack();                 //�����ջ��
	checker.PrintError();
    pa_file.close();
    return 0;
}
