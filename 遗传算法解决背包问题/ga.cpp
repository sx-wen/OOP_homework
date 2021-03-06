#include "ga.h"
#include <stdlib.h>
#include <iostream>
using namespace std;

#define RAND     ((double)rand()/(RAND_MAX+1))          //产生随机数0-1

//适应度函数,根据不同需求修改它
//本例是背包问题，限制条件为体积和质量，来查找最大价值
int ga::FitnessFcn(const int idx)
{
	static int values[32] = { 8,9,15,6,16,9,1,4,14,9,3,7,12,4,15,5,18,5,15,4,6,2,12,14,11,9,13,13,14,13,19,4 };
	static int volume[32] = { 11,22,12,21,21,13,1,10,13,8,6,25,13,27,12,23,12,24,23,11,6,24,28,10,20,13,25,23,5,26,30,15 };
	static int weight[32] = { 22,15,4,5,10,19,21,20,8,13,2,3,3,17,12,5,12,4,1,21,14,23,17,15,20,22,25,0,22,15,25,13 };
	static const int V_MAX = 75;
	static const int W_MAX = 80;
	int value = 0;
	int v_sum = 0, w_sum = 0;
	for (int i = 0; i < gene_len; i++) {
		if (genes[idx][i]) {
			v_sum += volume[i];
			w_sum += weight[i];
			value += values[i];
		}
	}
	if (v_sum > V_MAX || w_sum > W_MAX) {	//约束条件，不符合则舍弃重新赋初值
		value = 0;
		p_init = p_init/1.001;
		SelectValid(idx);                   //选择一个随机种群
		FitnessFcn(idx);                    //再求适应度，包含了迭代过程
	}
	return value;
}

//初始化
bool ga::Init(const double p_cross_in,const double p_mutate_in,const int pop_in,const int gene_len_in)
{
	p_cross = p_cross_in;
	p_mutate = p_mutate_in;
	pop = pop_in;
	gene_len = gene_len_in;
	cout << "遗传算法的初始条件为："<< endl;
	cout << "种群规模为： "<<pop_in << endl;
	cout << "基因长度为： "<<gene_len << endl;
	cout << "交叉概率为： "<<p_cross_in << endl;
	cout << "变异概率为： "<<p_mutate_in << endl;

	p_init = 0.5;
	if (p_cross_in < 0 || p_cross_in>1 || p_mutate_in < 0 || p_mutate_in>1|| pop<0 || gene_len<0)
		return false;

    genes = (bool**)malloc((sizeof(bool*)*pop));		//动态申请二维数组，pop*gene_len
	old_gene = (bool**)malloc((sizeof(bool*)*pop));
	for (int i = 0; i < pop; i++) {
		*(genes + i) = (bool*)malloc(sizeof(bool*)*gene_len);
		*(old_gene + i) = (bool*)malloc(sizeof(bool*)*gene_len);
	}
	genes_generation = (bool*)malloc(sizeof(bool)*gene_len);
	fitness = (double *)malloc((sizeof(double)*pop));	//申请内存空间
	cfitness = (double *)malloc((sizeof(double)*pop));
	rfitness = (double *)malloc((sizeof(double)*pop));

	fit_sum = 0;
	for (int i = 0; i < pop; i++) {						//初始化样本
		for (int j = 0; j < gene_len; j++) {
			if (RAND < p_init)	genes[i][j] = true;
			else genes[i][j] = false;
		}
		fitness[i] = FitnessFcn(i);
		fit_sum += fitness[i];
		rfitness[i] = 0;
		cfitness[i] = 0;
	}
	rfitness[0] = fitness[0] / fit_sum;
	cfitness[0] = rfitness[0];
	for (int i = 1; i < pop; i++) {
		rfitness[i] = fitness[i] / fit_sum;			   //相对概率
		cfitness[i] = cfitness[i - 1] + rfitness[i];   //累积概率
	}
   return true;
}

//保存每代中最佳个体
void ga::SaveBestGroup()
{
	double best_fit = fitness[0];
	int best_idx = 0;
	for (int i = 1; i < pop; i++) {                    //冒泡
		if (best_fit < fitness[i]) {
			best_fit = fitness[i];
			best_idx = i;
		}
	}
	for (int i = 0; i < gene_len; i++) {
		genes_generation[i] = genes[best_idx][i];
	}
	fit_generation = best_fit;
}
//选择
void ga::Select(void)
{
	for (int i = 0; i < gene_len; i++) {
		genes[0][i] = genes_generation[i];			//将最优个体引入到计算里，不然会计算不出来
	}

	fit_sum = 0;
	for (int i = 0; i < pop; i++) {
		fitness[i] = FitnessFcn(i);
		fit_sum += fitness[i];
	}
	SaveBestGroup();								//保存每次迭代中的最优个体

	rfitness[0] = fitness[0] / fit_sum;             //轮盘法叠加概率
	cfitness[0] = rfitness[0];
	for (int i = 1; i < pop; i++) {
		rfitness[i] = fitness[i] / fit_sum;			//相对概率
		cfitness[i] = cfitness[i - 1] + rfitness[i];//累积概率
	}

	for (int i = 0; i < pop; i++) {
		for (int j = 0; j < gene_len; j++) {
			old_gene[i][j] = genes[i][j];
		}
	}
	for (int i = 1; i < pop; i++) {					//轮盘法查找下一次的基因
		double tmp;
		int idx;
		tmp = RAND;
		idx = SelectFindNext(tmp);
		for (int j = 0; j < gene_len; j++) {
			genes[i][j] = old_gene[idx][j];
		}
	}
}
//查找value的所在区间，用于轮盘法
int ga::SelectFindNext(double value)
{
	int low, high, mid;
	low = 0;
	high = pop - 1;
	while (low <= high)                             //二分法查找
	{
		mid = (low + high) / 2;
		if (cfitness[mid] <= value && cfitness[mid+1] > value)
			return mid;
		if (cfitness[mid]>value)
			high = mid - 1;
		if (cfitness[mid+1]<=value)
			low = mid + 2;
	}
	return false;
}
//随机产生数，以满足约束条件
void ga::SelectValid(int idx)
{
	for (int j = 0; j < gene_len; j++) {
		if (RAND < p_init)	genes[idx][j] = true;
		else genes[idx][j] = false;
	}
}
//交叉
void ga::Crossover()
{
	int father,mother,cr_idx;
	double tmp;
	for (int i = 0; i < pop; i++) {
		if (RAND < p_cross) {				  	 //随机选择是否交叉
			tmp = RAND;
			father = SelectFindNext(tmp);		 //优势个体
			//mother = i;						 //随机选择
			tmp = RAND;
			mother = SelectFindNext(tmp);		 //优势个体
			cr_idx = RAND*(gene_len - 1);
			for (int j = cr_idx; j < gene_len; j++) {
				int k;
				k = genes[father][j];
				genes[father][j] = genes[mother][j];
				genes[mother][j] = k;
			}
		}
	}
}
//变异
void ga::Mutate(void)
{
	for (int i = 0; i < pop; i++) {
		for (int j = 0; j < gene_len; j++) {
			if(RAND<p_mutate)	genes[i][j]=!genes[i][j];
		}
	}
}
//打印最佳个体
void ga::PrintBestFit()const
{
	cout << "BestFit: " << fit_generation << " ";
	cout << "the route is: " ;
	for (int i = 0; i < gene_len; i++) {
		if (genes_generation[i]) {
			cout << 1+i << "  ";
		}
	}
	cout << endl;
}

//打印一个种群的基因
void ga::PrintUnit(const int idx)
{
	cout << idx << " genes is: ";
	for (int i = 0; i < gene_len; i++) {
		cout << genes[idx][i]<<"";
	}
	cout << endl;
}

//构造函数，无返回类型
ga::ga(void)
{

}
//析构函数，释放内存
ga::~ga(void)
{
    free(genes);
    free(old_gene);
    free(genes_generation);
    free(fitness);
    free(cfitness);
    free(rfitness);
}
