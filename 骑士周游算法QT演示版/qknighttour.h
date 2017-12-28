#ifndef QKNIGHTTOUR_H
#define QKNIGHTTOUR_H

#include <QThread>
#include <QMutex>
#include <QString>
#include <QObject>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintPreviewDialog>
#include <QtPrintSupport/QPrintDialog>


using namespace std;

struct K_POS
{
    int x;
    int y;
    int val;
};

class QKnightTour: public QThread
{
    Q_OBJECT
public:
    void run();                 //虚函数，线程重写
    QKnightTour(int size);
    ~QKnightTour();
    int TourBoard(K_POS curr);  //从第几步开始
    int FastTourBoard(K_POS curr,int step_idx);//递归+回溯
    void BoardClear();          //清除面板
    void pause();               //暂停
    void goon();                //继续

    K_POS current;              //当前点
    K_POS init_pos;             //初始点
    const K_POS board_size;     //棋盘规格8*8
    //int board[8][8] = { 0 };	//标志性,0代表还没走到，其他数字代表第n步走到的
    int **board;                //动态分配内存
    const int all_step;         //遍历需要8*8=64步
    const int MAX_SIZE=100;     //棋盘最大为100*100
    bool flag_over;             //算法结束标志
    bool flag_load;             //载入文件标志
    QMutex m_mutex;             //互斥量，用于暂停与继续线程的
private:
    int GetNxtVal(K_POS curr);  //获得下一个可行点的路径数
    int GetMinIdx(K_POS *&nxt); //求最小值点
    const int rule_len=8;		//8个潜在可行方向
    K_POS rule[8] = { {1,-2,},{1,2},{-2,1},{-2,-1},{2,1},{2,-1},{-1,2},{-1,-2} };
    int run_speed;              //运行速度

signals:
    void ShowResult(const QString sStr);//打印结果
    void update();              //刷新显示

public slots:
    void SetRunSpeed(int ms);   //获取速度标志

};

#endif // QKNIGHTTOUR_H
