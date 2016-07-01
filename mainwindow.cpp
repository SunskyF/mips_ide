#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QTextEdit>
#include <QTextBlock>
#include <QTextCursor>
#include <QDebug>
#include "mips_utl.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openFile);
    connect(ui->actionSave_as, &QAction::triggered, this, &MainWindow::saveasFile);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveFile);
    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::newFile);
    connect(ui->actionBuild, &QAction::triggered, this, &MainWindow::Build);
    connect(ui->actionBin, &QAction::triggered, this, &MainWindow::Bin);
    connect(ui->actionCoe, &QAction::triggered, this, &MainWindow::Coe);
    connect(ui->actionShowbin, &QAction::triggered, this, &MainWindow::ShowBin);
    connect(ui->actionDiscoe, &QAction::triggered, this, &MainWindow::Discoe);
    connect(ui->actionDisbin, &QAction::triggered, this, &MainWindow::Disbin);
    connect(ui->actionShowdata, &QAction::triggered, this, &MainWindow::showdata);
    connect(ui->textEdit, &QTextEdit::textChanged, [=]() {
        this->setWindowModified(true);
    });
    connect(this, &MainWindow::hasSaved, [=]() {
        this->setWindowModified(false);
    });
    if (path!="") this->setWindowTitle(path+"[*]");
    else this->setWindowTitle("Mips-ide by fh");
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::newFile(){
    ui->textEdit->setEnabled(true);
    path="untitled";
    this->setWindowTitle("Mips-ide by fh"+path+"[*]");
    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.setPosition(0);
    ui->textEdit->setTextCursor(cursor);
    ui->textEdit->ensureCursorVisible();
    ui->textEdit->setFocus();
    ui->textBrowser->clear();
    hasbuild=false;
}

void MainWindow::showdata(){
    Mips exec_data=instrutions;
    exec_data.firstprase();
    exec_data.secondprase();
    showMips(exec_data);
    QString code;
    int cnt=0;
    QString temp;
    for (auto i=exec_data.data_store.begin();i!=exec_data.data_store.end();++i,++cnt){
        if (cnt%4==0) {
            code+=QString::number(i->first,16);
            code+="    ";
        }
        temp=QString::number(i->second,2);
        if (temp.size()<8){
            QString temp0(8-temp.size(),'0');
            temp=temp0+temp;
        }
        code+=temp+" ";
        if (cnt%4==3) code+='\n';
        qDebug()<<temp;
    }
    if (cnt%4!=0){
        code+=temp+'\n';
    }
    qDebug()<<cnt;
    ui->textBrowser->setText(code);
}

void MainWindow::openFile()
{
    path = QFileDialog::getOpenFileName(this,
                                        tr("Open File"),
                                        ".",
                                        tr("Text Files(*.asm *.ASM *.bin *.coe)"));
    QFileInfo filepath(path);
    if(filepath.exists()) {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::warning(this, tr("Read File"),
                                 tr("Cannot open file:\n%1").arg(path));
            return;
        }
        QFileInfo fi(path);
        QTextStream in(&file);
        ui->textEdit->setText(in.readAll());
        ui->textEdit->setEnabled(true);
        this->setWindowTitle(fi.completeBaseName()+"[*]");
        emit hasSaved();
        in.seek(0);
        if (fi.suffix()=="asm"||fi.suffix()=="ASM"){
            Mips mip(in);
            instrutions=mip;
        }else if (fi.suffix()=="coe"){
            Mipscoe mipcoe(in);
            coeinst=mipcoe;
        }else {

        }
        file.close();
        ui->textBrowser->clear();
        hasbuild=false;
    }

}
void MainWindow::saveFile()
{
    QFileInfo filepath(path);
    if(filepath.exists()) {
        QFile file(path);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this, tr("Write File"),
                                       tr("Cannot open file:\n%1").arg(path));
            return;
        }
        QTextStream out(&file);
        out << ui->textEdit->toPlainText();
        QTextStream in(&file);
        in.seek(0);
        QFileInfo fi(path);
        if (fi.suffix()=="asm"||fi.suffix()=="ASM"){
            Mips mip(in);
            instrutions=mip;
        }else if (fi.suffix()=="coe"){
            Mipscoe mipcoe(in);
            coeinst=mipcoe;
        }else {

        }
        file.close();
        emit hasSaved();
    }else
        saveasFile();
}
void MainWindow::saveasFile()
{
    if (path=="") return;
    path = QFileDialog::getSaveFileName(this,
                                        tr("Save File"),
                                        ".",
                                        tr("Text Files(*.asm *.ASM)"));
    if(!path.isEmpty()) {
        QFile file(path);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this, tr("Write File"),
                                       tr("Cannot open file:\n%1").arg(path));
            return;
        }
        QTextStream out(&file);
        out << ui->textEdit->toPlainText();
        QTextStream in(&file);
        in.seek(0);
        QFileInfo fi(path);
        if (fi.suffix()=="asm"||fi.suffix()=="ASM"){
            Mips mip(in);
            instrutions=mip;
        }else if (fi.suffix()=="coe"){
            Mipscoe mipcoe(in);
            coeinst=mipcoe;
        }else {

        }
        file.close();
        this->setWindowTitle("Mips-ide by fh"+fi.completeBaseName()+"[*]");
        emit hasSaved();
    }
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (isWindowModified()) {
        bool exit = QMessageBox::question(this,
                                      tr("Quit"),
                                      tr("Are you sure to quit this application?"),
                                      QMessageBox::Yes | QMessageBox::No,
                                      QMessageBox::No) == QMessageBox::Yes;
        if (exit) {
            event->accept();
        } else {
            event->ignore();
        }
    } else {
        event->accept();
    }
}

void MainWindow::showMips(Mips &m){
    QString hexcode;
    qlonglong textbegin=m.BaseAddre;
    for (int i=0;i<m.length();++i){
        QString temp=bin2hex(m.getBincode(i)),tempstr;
        tempstr=QString::number(textbegin+i*4,16)+" ";
        for (int j=0;j<temp.size();++j){
            tempstr+=temp.at(j);
            if (j%2){
                tempstr+=" ";
            }
        }
        hexcode+=tempstr+'\n';
    }
    ui->textBrowser->setText(hexcode);
}

void MainWindow::Build(){
    if (instrutions.isempty()){
        QMessageBox::warning(this, tr("Build"),
                                   tr("No file"));
        return;
    }
    Mips exec_data=instrutions;
    exec_data.firstprase();
    exec_data.secondprase();
    showMips(exec_data);
    hasbuild=true;
}

void MainWindow::ShowBin(){
    Mips exec_data=instrutions;
    exec_data.firstprase();
    exec_data.secondprase();
    hasbuild=true;
    QString bincode;
    for (int i=0;i<exec_data.length();++i){
        bincode+=QString::number(exec_data.BaseAddre+4*i,16)+"    "+exec_data.getBincode(i)+'\n';
    }
    ui->textBrowser->setText(bincode);
}

void MainWindow::Bin(){
    QString binpath = QFileDialog::getSaveFileName(this,
                                        tr("Save Bin File"),
                                        ".",
                                        tr("Text Files(*.bin)"));
    if(!path.isEmpty()) {
        Mips exec_data=instrutions;
        QFile file(binpath);
        exec_data.firstprase();
        exec_data.secondprase();
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::warning(this, tr("Write File"),
                                       tr("Cannot open file\n"));
            return;
        }
        QDataStream out(&file);
        for (int i=0;i<exec_data.length();++i){
            bool ok;
            out<<exec_data.getBincode(i).toLongLong(&ok,2);
        }
        file.close();
        QMessageBox::warning(this, tr("Write File"),tr("Bin file saved\n"));
    }
}
void MainWindow::Coe(){
    QString coepath = QFileDialog::getSaveFileName(this,
                                        tr("Save Coe File"),
                                        ".",
                                        tr("Text Files(*.coe)"));
        Mips exec_data=instrutions;
        exec_data.firstprase();
        exec_data.secondprase();
        QString coefile="memory_initialization_radix=16;\nmemory_initialization_vector=\n";
        QString temp;
        for (int i=0;i<exec_data.length();++i){
            if (i==exec_data.length()-1)
                temp=";";
            else
                temp=",";
            if((i+1)%4==0)
                temp+="\n";
            coefile+=bin2hex(exec_data.getBincode(i))+temp;
        }
        QFile file(coepath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this, tr("Write File"),
                                       tr("Cannot open file:\n"));
            return;
        }
        QTextStream out(&file);
        out << coefile;
        file.close();
        QMessageBox::warning(this, tr("Write File"),tr("Coe file saved\n"));
}

void MainWindow::Discoe(){
    Mipscoe coe=coeinst;
    coe.prase();
    QString ins4coe;
    for (int i=0;i<coe.length();++i){
        ins4coe+=QString::number(i+1)+" "+coe.getInst(i)+"\n";
    }
    ui->textBrowser->setText(ins4coe);
}

void MainWindow::Disbin(){

}
