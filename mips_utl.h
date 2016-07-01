#include <QString>
#include <map>
#include <QtAlgorithms>
#include <QTextStream>
#ifndef MIPS_UTL_H
#define MIPS_UTL_H

bool allspace(QString s);
void getTheMap(std::map<QString,int> &set_map);
int getTheReg(QString str);
QString getTheBin(int num,int dig);
QString getTheBin(int num,long long dig);
QString bin2hex(QString str);

bool isDigitstr(QString src);

QString sfunc(int inst,QString str);
QString lfunc(int inst,QString str);
QString alurfunc(int inst,QString str);
QString muldivfunc(int inst,QString str);
QString aluifunc(int inst,QString str);
QString branchfunc(int inst, QString str, int pc, std::map<QString, long long > &m);
QString transfunc(int inst,QString str, int pc, long long textaddress,std::map<QString, long long> &m);
QString trfunc(int inst,QString str);
QString pfunc(int inst,QString str);
QString trapfunc(int inst,QString str);
QString pesfunc(int inst,QString str);

#endif // MIPS_UTL_H
