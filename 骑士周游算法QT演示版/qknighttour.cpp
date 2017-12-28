#include "qknighttour.h"
#include <QString>
#include <qobjectdefs.h>


void QKnightTour::run()
{
    //子线程开始
    emit ShowResult("KnightTour begin");
    int knight_size = board_size.x;
    if(init_pos.x<0||init_pos.y<0||knight_size==0||init_pos.x>MAX_SIZE||init_pos.y>MAX_SIZE){
        emit ShowResult("Error! Outside ...");
        return;
    }
    QString show="";
    if(!TourBoard(init_pos)){
        show = "Error!There is no feasible path for the current setting state!!\n\n";
    }

    for(int i=0;i<board_size.x;i++){
        for(int j=0;j<board_size.y;j++)
            show += QString("%1").arg(board[i][j],4,10,QChar(' '));
        show += '\n';
    }
    emit ShowResult(show);

}

void QKnightTour::SetRunSpeed(int ms)
{
    run_speed = ms*10;
}

void QKnightTour::pause()
{
    m_mutex.lock();
}

void QKnightTour::goon()
{
   m_mutex.unlock();
}

void QKnightTour::BoardClear()
{
    for(int i=0;i<board_size.x;i++){
        for(int j=0;j<board_size.y;j++){
            board[i][j] = 0;
        }
    }
}

int QKnightTour::GetMinIdx(K_POS *&nxt)
{
    //查找可行方向min的
    int min_idx = 0;
    int min_v = nxt[0].val;
    for (int i = 1; i < rule_len; i++) {
        if (nxt[i].val < min_v) {
            min_v = nxt[i].val;
            min_idx = i;
        }
    }
    return min_idx;
}

int QKnightTour::TourBoard(K_POS curr)
{
    flag_over = false;
    //需要判断是否为重载进历史文件，若是则从历史状态开始，否则从第一步开始
    if(flag_load==true){
        curr = current;
        while(!FastTourBoard(curr,curr.val+1)){ //回溯失败了，需要往前推移
            K_POS tmp;
            tmp = curr;
            for(int i=0;i<board_size.x;i++){
                for(int j=0;j<board_size.y;j++){
                    if(board[i][j] == tmp.val-1){
                        curr.x = i;curr.y = j;curr.val = board[i][j];
                    }else if(board[i][j] > tmp.val-1){
                        board[i][j] = 0;
                    }
                }
            }
            if(curr.val==0){
                return false;
            }

        }
        flag_load = false;
    }else{
        BoardClear();
        board[curr.x][curr.y] = 1;
        if(!FastTourBoard(curr,2)){
            return false;
        }
    }
    return true;
}

int QKnightTour::FastTourBoard(K_POS curr, int step_idx)
{
    m_mutex.lock();         //控制算法暂停与继续
    emit update();
    msleep(run_speed);      //控制算法运行速度
    m_mutex.unlock();
    if(flag_over==true){    //直接控制算法停止
        return true;
    }

    if (step_idx > all_step) {	//算法结束标志
        flag_over = true;
        return true;
    }
    K_POS *nxt = new K_POS[rule_len];
    for (int i = 0; i<rule_len; i++) {	  //查找下一个可行点
        nxt[i].x = curr.x + rule[i].x;
        nxt[i].y = curr.y + rule[i].y;
        if (nxt[i].x >= 0 && nxt[i].y >= 0 && nxt[i].x < board_size.x
            && nxt[i].y < board_size.y && !board[nxt[i].x][nxt[i].y]) {
            nxt[i].val = GetNxtVal(nxt[i]);//查找下下个点的可行方向，找最小做剪枝
        }
        else nxt[i].val = all_step;
    }
    int min_idx = GetMinIdx(nxt);
    for (int j = 0; j < 2; j++) { //可能在一个方向会找不到可行路径，换条路径找
        if (nxt[min_idx].val < all_step) {
            curr = nxt[min_idx];    //保存状态，以便显示
            curr.val = step_idx;
            current = curr;
            board[nxt[min_idx].x][nxt[min_idx].y] = step_idx;
            if (FastTourBoard(nxt[min_idx], step_idx + 1)) //递归下去，直到所有点都找到
                return true;
            board[nxt[min_idx].x][nxt[min_idx].y] = 0;
            curr = nxt[min_idx];
            curr.val = 0;
        }
        nxt[min_idx].val = all_step; //寻找第二条可行路径
        min_idx = GetMinIdx(nxt);
    }
    return false;
}


int QKnightTour::GetNxtVal(K_POS curr)
{
    int x, y;
    int cnt = 0;
    for (int i = 0; i<rule_len; i++) { //curr的可行方向集合
        x = curr.x + rule[i].x;
        y = curr.y + rule[i].y;
        if (x>=0 && y>=0 && x < board_size.x && y < board_size.y && !board[x][y]) {
            cnt ++;
        }
    }
    return cnt;
}

QKnightTour::QKnightTour(int size) :all_step(size*size) ,board_size{ size, size }
{
    flag_load = false;
    run_speed = 500;
    init_pos.x=0;init_pos.y=0;
    current.x = 0;current.y = 0;
    board = new int *[size];
    for (int i = 0; i < size; i++) {
        *(board + i) = new int[size];
    }
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++)
            board[i][j] = 0;
    }
}

QKnightTour::~QKnightTour()
{

}
