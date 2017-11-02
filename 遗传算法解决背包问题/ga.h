#ifndef GA_H_INCLUDED
#define GA_H_INCLUDED

#include <iostream>

class ga{
public:
    ga(void);
    ~ga(void);
    bool Init(const double p_cross_in,const double p_mutate_in,const int pop_in,const int gene_len_in);
	void Select(void);
    void Crossover(void);
	void Mutate(void);
	int FitnessFcn(const int idx);
	void PrintUnit(const int idx);
	void PrintBestFit(void)const;
    bool **genes;
    double *fitness;
	double *rfitness;	//�������
	double *cfitness;	//�ۼӸ���

private:
	double p_cross;
	double p_mutate;
	double p_init;
	int pop;
	int gene_len;
	bool **old_gene;
	double fit_sum;
	int SelectFindNext(double pro);
	void SelectValid(int idx);
	void SaveBestGroup();
	bool *genes_generation;
	double fit_generation;

};

#endif // GA_HPP_INCLUDED
