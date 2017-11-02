#include <iostream>
#include <string>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "ga.h"

using namespace std;

#define P_CROSS           0.8           //交叉概率
#define P_MUTATE          0.2           //变异概率
#define POP_SIZE          64            //种群大小
#define GENE_LEN          32            //基因长度

int main()
{
    srand((int)time(NULL));
	int iter=1;
	int iter_max = 10000;
	cout << "/****遗传算法解决背包问题*****/" << endl;
	cout << "默认迭代次数为"<< iter_max << endl;
	//cin >> iter_max;
    ga ga1;
	ga1.Init(P_CROSS,P_MUTATE,POP_SIZE,GENE_LEN);//初始化设置参数

	do {
		ga1.Select();                      //选择
		ga1.Crossover();                   //交叉
		ga1.Mutate();                      //变异
		if (iter % 500 == 0) {             //显示结果
			cout << "iter: " << iter<<" ";
			ga1.PrintBestFit();
		}
	} while (++iter<iter_max);

	ga1.PrintBestFit();                    //显示最佳结果

    return true;
}
