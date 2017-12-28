#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qknighttour.h"
#include "qpainter.h"
#include "QVBoxLayout"
#include <QString>
#include <QFileDialog>
#include <QDataStream>
#include <QTextStream>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintPreviewDialog>
#include <QtPrintSupport/QPrintDialog>
#include <QScreen>
#include <qscreen.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //初始化
    ui->setupUi(this);
    setWindowTitle("骑士周游问题");
    int knight_size = ui->lineBoardSize->text().toInt();
    knight =new QKnightTour(knight_size);
    //限制输入规格和初始位置大小
    int max_size = knight->MAX_SIZE;
    QIntValidator* text_edit = new QIntValidator(3,max_size,this);
    ui->lineBoardSize->setValidator(text_edit);
    text_edit->setRange(1,max_size);
    ui->lineInitX->setValidator(text_edit);
    ui->lineInitY->setValidator(text_edit);

    //主函数，需要映射信号槽和信息传递量，实现子线程与主线程之间通信
    connect(knight, SIGNAL(ShowResult(QString)), ui->textResult,  SLOT(setText(QString))); //显示结果
    connect(knight, SIGNAL(update()), this,  SLOT(update()));   //更新显示
    connect(ui->SliderTimerSpeed, SIGNAL(valueChanged(int)), knight,  SLOT(SetRunSpeed(int))); //速度控制
}

MainWindow::~MainWindow()
{
    delete ui;
}

//动态刷新棋盘状态
void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QSize board_size = ui->centralWidget->size();
    int knight_size = ui->lineBoardSize->text().toInt();//棋盘规格
    int b_width = (board_size.width()-300)/knight_size; //自适应控制棋盘大小
    int b_height = board_size.height()/knight_size;

    //QPoint origin(board_size.width()/3,10);
    QPoint origin(300,10);
    QPoint start_point = origin;
    QFont font("宋体",14,QFont::Bold);
    painter.setFont(font);

    int board_color = 0;
    QRect rect(0, 0, 1, 1);
    for(int i=0;i<knight_size;i++){
        board_color = 255*(i%2);        //画黑白棋盘图
        start_point.setY(origin.y() + i*b_height);
        for(int j=0;j<knight_size;j++){
            start_point.setX(origin.x() + j*b_width);
            board_color = 255-board_color;  //每次都需要翻转
            if(knight->board[i][j]){        //已经走过的为灰色
                painter.setBrush(QColor(100, 100, 100));
                rect.setRect(start_point.x(), start_point.y(), b_width, b_height);
                painter.drawRect(start_point.x(), start_point.y(), b_width, b_height);
                painter.drawText(rect,Qt::AlignCenter,QString("%1").arg(knight->board[i][j]));
            }
            else{                           //没走过的为黑白
                painter.setBrush(QColor(board_color, board_color, board_color));
                painter.drawRect(start_point.x(), start_point.y(), b_width, b_height);
            }
        }
    }
    //画当前运动点
    start_point.setY(origin.y() + knight->current.x*b_height);
    start_point.setX(origin.x() + knight->current.y*b_width);
    painter.setBrush(QColor(100, 200, 250));
    painter.drawRect(start_point.x(), start_point.y(), b_width, b_height);
    rect.setRect(start_point.x(), start_point.y(), b_width, b_height);
    painter.drawText(rect,Qt::AlignCenter,QString("%1").arg(knight->board[knight->current.x][knight->current.y]));

}

//控制开始与停止按钮
void MainWindow::on_ButtonStartHorse_clicked(bool checked)
{
    if(checked){
        ui->ButtonStartHorse->setText("开始");
        knight->flag_over = true;
    }else {
        ui->ButtonStartHorse->setText("停止");
        knight->start();

    }
}

//控制子线程（骑士周游算法）暂停与继续按钮
void MainWindow::on_ButtonContinue_clicked(bool checked)
{
    if(checked){
        ui->ButtonContinue->setText("暂停");
        knight->m_mutex.unlock();
    }else {
        ui->ButtonContinue->setText("继续");
        knight->m_mutex.lock();
    }
    if(knight->flag_over == true){
        knight->start();
    }
}

//动态分配内存，适合多棋盘
void MainWindow::on_lineBoardSize_textChanged(const QString &arg1)
{
    int knight_size = arg1.toInt();
    if(knight_size<=0||knight_size>knight->MAX_SIZE){
        ui->lineBoardSize->setText(QString("%1").arg(knight->board_size.x,1,10,QChar(' ')));
        return;
    }
    if(knight->init_pos.x>=knight_size){
        ui->lineInitX->setText(arg1);
        on_lineInitX_textChanged(arg1);
    }
    if(knight->init_pos.y>=knight_size){
        ui->lineInitY->setText(arg1);
        on_lineInitY_textChanged(arg1);
    }
    knight->current.x = knight->init_pos.x;
    knight->current.y = knight->init_pos.y;
    knight->flag_over = true;
    delete knight->board;
    K_POS *const_p = const_cast<K_POS *>(&knight->board_size);
    const_p->x = knight_size;
    const_p->y = knight_size;
    int *const_step = const_cast<int *>(&knight->all_step);
    *const_step = knight_size*knight_size;

    knight->board = new int *[knight_size];
    for (int i = 0; i < knight_size; i++) {
        *(knight->board + i) = new int[knight_size];
    }
    for (int i = 0; i < knight_size; i++) {
        for (int j = 0; j < knight_size; j++)
            knight->board[i][j] = 0;
    }
    update();
}

//棋盘初始点设置
void MainWindow::on_lineInitX_textChanged(const QString &arg1)
{
    int tmp = arg1.toInt()-1;
    if(tmp>knight->board_size.x-1){
        knight->init_pos.x = knight->board_size.x-1;
        ui->lineInitX->setText(QString("%1").arg(knight->board_size.x,1,10,QChar(' ')));
    }else{
        knight->init_pos.x = tmp;
    }
}

//棋盘初始点设置
void MainWindow::on_lineInitY_textChanged(const QString &arg1)
{
    int tmp = arg1.toInt()-1;
    if(tmp>knight->board_size.y-1){
        knight->init_pos.y = knight->board_size.y-1;
        ui->lineInitY->setText(QString("%1").arg(knight->board_size.y,1,10,QChar(' ')));
    }else{
         knight->init_pos.y = tmp;
    }
}

//保存文件
void MainWindow::on_BnSaveFile_clicked()
{
    //保存状态文件
    QString file_path;
    file_path = QFileDialog::getSaveFileName(this,
        tr("Open Config"), "", tr(""));
    if(file_path.isEmpty())return;
    QFile file(file_path);
    file.open(QIODevice::WriteOnly| QIODevice::Text);
    QTextStream  in(&file);

    in<<knight->init_pos.x+1<<" ";    //保存当前状态
    in<<knight->init_pos.y+1<<" ";
    in<<knight->board_size.x<<" ";
    in<<knight->current.x<<" ";     //保存当前点
    in<<knight->current.y<<" ";
    in<<knight->current.val<<" ";
    in<<"\n";
    for (int i = 0; i < knight->board_size.x; i++) {
        for (int j = 0; j < knight->board_size.y; j++)
            in<<knight->board[i][j]<<" ";
        in<<"\n";
    }
    file.close();

}

//读取文件
void MainWindow::on_BnLoadHistory_clicked()
{
    knight->flag_over = true;
    //载入历史文件
    QString file_path,dirString;
    file_path = QFileDialog::getOpenFileName(this,
        tr("Open Config"), "", tr(""));
    if(file_path.isEmpty())return;
    QFile file(file_path);
    file.open(QIODevice::ReadOnly| QIODevice::Text);
    QString str,str2;
    QTextStream out(&file);
    if(file.isOpen()){
        out>>str;                       //读取状态并设置显示
        ui->lineInitX->setText(str);
        knight->init_pos.x=str.toInt()-1;
        out>>str;
        ui->lineInitY->setText(str);
        knight->init_pos.y=str.toInt()-1;
        out>>str;
        ui->lineBoardSize->setText(str);
        on_lineBoardSize_textChanged(str);

        out>>str; knight->current.x=str.toInt();        //读取当前点
        out>>str; knight->current.y=str.toInt();
        out>>str; knight->current.val=str.toInt();
        for (int i = 0; i < knight->board_size.x; i++) {
            for (int j = 0; j < knight->board_size.y; j++){
                out>>str; knight->board[i][j] = str.toInt();
            }
        }
    }

    QString show="";        //显示所有状态

    for(int i=0;i<knight->board_size.x;i++){
        for(int j=0;j<knight->board_size.y;j++)
            show += QString("%1").arg(knight->board[i][j],4,10,QChar(' '));
        show += '\n';
    }
    ui->textResult->setText(show);
    knight->flag_load = true;
    update();
}


void MainWindow::on_BnPreviewFile_clicked()
{
    QPrinter printer;
    printer.setPageSize(QPrinter::Custom);
    printer.setPaperSize(QSizeF(ui->centralWidget->height(), ui->centralWidget->width()),
                               QPrinter::Point);
    QPrintPreviewDialog preview(&printer, this);
    preview.setMinimumSize(1000,600);
    connect(&preview, SIGNAL(paintRequested(QPrinter*)), SLOT(printPreviewSlot(QPrinter*)));
    preview.exec ();
}

void MainWindow::printPreviewSlot(QPrinter *printerPixmap)
{
    printerPixmap->setOrientation(QPrinter::Landscape);
    QPixmap pixmap = QPixmap::grabWindow(this->winId(), 0,0,-1,-1);//获取界面的图片
    QPainter painterPixmap(this);
    painterPixmap.begin(printerPixmap);
    QRect rect = painterPixmap.viewport();
    int x = rect.width() / pixmap.width();
    int y = rect.height() / pixmap.height();
    painterPixmap.scale(x, y);
    painterPixmap.drawPixmap(0, 0, pixmap);
    painterPixmap.end();

}

void MainWindow::on_BnPrintFile_clicked()
{
    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPageSize(QPrinter::Custom);
    printer.setPaperSize(QSizeF(ui->centralWidget->height(), ui->centralWidget->width()),
                               QPrinter::Point);
    QString fileName = QFileDialog::getSaveFileName(this, tr("保存PDF文件"), QString(), "*.pdf");
    if(fileName.isEmpty()){
        return ;
    }
    printer.setOutputFileName(fileName);
    QPainter painterPixmap;
    printer.setOrientation(QPrinter::Landscape);
    painterPixmap.begin(&printer);
    QPixmap pixmap = QPixmap::grabWindow(this->winId(), 0,0,-1,-1);//获取界面的图片
    QRect rect = painterPixmap.viewport();
    int x = rect.width() / pixmap.width();
    int y = rect.height() / pixmap.height();
    painterPixmap.scale(x, y);
    painterPixmap.drawPixmap(0, 0, pixmap);
    painterPixmap.end();
}
