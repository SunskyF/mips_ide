#include "mips_utl.h"
#include <vector>
#ifndef MIPSCOE_H
#define MIPSCOE_H


class Mipscoe
{
public:
    Mipscoe();
    Mipscoe(const Mipscoe &m);
    Mipscoe(QTextStream &in);
    int length();
    Mipscoe& operator=(const Mipscoe m);
    void prase();
    QString getInst(int i);
    QString split(QString s,int strat,int len);
    QString R_Type(QString s);
    QString I_Type(QString s);
    QString J_Type(QString s);
private:
    std::vector<QString> hex;
    std::vector<QString> instructions;
};

#endif // MIPSCOE_H
