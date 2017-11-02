#include <iostream>
#include <string>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "ga.h"

using namespace std;

#define P_CROSS           0.8           //�������
#define P_MUTATE          0.2           //�������
#define POP_SIZE          64            //��Ⱥ��С
#define GENE_LEN          32            //���򳤶�

int main()
{
    srand((int)time(NULL));
	int iter=1;
	int iter_max = 10000;
	cout << "/****�Ŵ��㷨�����������*****/" << endl;
	cout << "Ĭ�ϵ�������Ϊ"<< iter_max << endl;
	//cin >> iter_max;
    ga ga1;
	ga1.Init(P_CROSS,P_MUTATE,POP_SIZE,GENE_LEN);//��ʼ�����ò���

	do {
		ga1.Select();                      //ѡ��
		ga1.Crossover();                   //����
		ga1.Mutate();                      //����
		if (iter % 500 == 0) {             //��ʾ���
			cout << "iter: " << iter<<" ";
			ga1.PrintBestFit();
		}
	} while (++iter<iter_max);

	ga1.PrintBestFit();                    //��ʾ��ѽ��

    return true;
}
