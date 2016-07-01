#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mips.h"
#include "mipscoe.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void openFile();
    void saveFile();
    void saveasFile();
    void newFile();
    void closeEvent(QCloseEvent *event);
    void showMips(Mips &m);
    void Build();
    void Bin();
    void Coe();
    void ShowBin();
    void Discoe();
    void Disbin();
    void showdata();
signals:
    void hasSaved();
private:
    Ui::MainWindow *ui;
    QString path="";
    Mips instrutions;
    Mipscoe coeinst;
    bool hasbuild=false;
};

#endif // MAINWINDOW_H
