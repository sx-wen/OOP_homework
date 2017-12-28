#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qknighttour.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *);

signals:
    void PushButtonShowVal(const QString&);

private slots:

    void on_ButtonStartHorse_clicked(bool checked);

    void on_ButtonContinue_clicked(bool checked);

    void on_lineBoardSize_textChanged(const QString &arg1);

    void on_lineInitX_textChanged(const QString &arg1);

    void on_lineInitY_textChanged(const QString &arg1);

    void on_BnSaveFile_clicked();

    void on_BnLoadHistory_clicked();


    void on_BnPreviewFile_clicked();

    void printPreviewSlot(QPrinter *printerPixmap);

    void on_BnPrintFile_clicked();

private:
    Ui::MainWindow *ui;
    QKnightTour *knight = new QKnightTour(8); //线程操作

};

#endif // MAINWINDOW_H
