#include "mips.h"
#include <QDebug>
#include <QRegExp>
#include <QMessageBox>
#include "mips_utl.h"

Mips::Mips(){
    labels.clear();
    location.clear();
    bincode.clear();
    labels.reserve(1000000);
    bincode.reserve(1000000);
    memory.resize(1000000);
    data_store.clear();
}

Mips::Mips(const Mips &m){
    labels.reserve(1000000);
    bincode.reserve(1000000);
    labels=m.labels;
    bincode=m.bincode;
    memory.resize(0x00010000);
    data_store.clear();
}

Mips::Mips(QTextStream &in){
    labels.clear();
    location.clear();
    bincode.clear();
    labels.reserve(10000);
    bincode.reserve(10000);
    memory.resize(1000000);
    data_store.clear();
    while (!in.atEnd()){
        QString temp=in.readLine();
        if (!temp.isEmpty()&&!allspace(temp)){
            QRegExp rxfotspace("(\\s*)(.*)");
            int pos = rxfotspace.indexIn(temp);
            if (pos > -1) {
                temp = rxfotspace.cap(2);
            }
            labels.push_back(temp);
        }
    }
}
Mips& Mips::operator=(const Mips m){
    labels=m.labels;
    return *this;
}
bool Mips::isempty(){
    return labels.empty();
}

void Mips::clear(){
    labels.clear();
    location.clear();
    bincode.clear();
}
void Mips::debugshow(){
    for (int i=0;i<labels.size();++i)
        qDebug()<<bincode[i];
}
QString Mips::getBincode(int index){
    return bincode[index];
}

int Mips::length(){
    return bincode.size();
}

void Mips::doData(){
    bool ok;

    for (int i=0;i<labels.size();++i){
        QRegExp rxforcom("(#|//)DataAddre:(.*);");
        int pos=rxforcom.indexIn(labels[i]);
        int nextsection0=0;
        if (pos>-1){
            unsigned int tempad=rxforcom.cap(2).toLongLong(&ok,16);
            unsigned int now=0;
            labels.erase(labels.begin()+i);
            while (i<labels.size()){

                QRegExp rxforcom("(#|//)DataAddre:(.*);");
                nextsection0=rxforcom.indexIn(labels[i]);
                if (nextsection0>-1)
                    break;
                rxforcom.setPattern("(#|//)BaseAddre:(.*);");
                nextsection0=rxforcom.indexIn(labels[i]);
                if (nextsection0>-1)
                    break;
                QRegExp rxforlabel("(.*):");
                int pos0 = rxforlabel.indexIn(labels[i]);
                if (pos0 > -1) {
                    data_address[rxforlabel.cap(1)]=tempad+now;
                    labels[i].replace(rxforlabel,"");
                    labels[i]=labels[i].trimmed();
                }
                QString temp=labels[i];
                QRegExp rx("\\d*");
                QString r=rx.cap(0);
                switch (labels[i][0].toLatin1()) {
                case 'd':
                    if(labels[i][1].toLatin1()=='b') {
                        qDebug()<<"db"<<labels[i];
                        QString temp=labels[i];
                        for (int j=3;j<temp.size();++j){
                            if (temp[j].isDigit()){
                                if (temp[j]=='0'){
                                    if (temp[j+1]=='x'){
                                        QRegExp rx("0x[0-9a-z]*");
                                        rx.indexIn(temp,j);
                                        QString r=rx.cap(0);
                                        int num=r.toInt(&ok,16);
                                        data_store[tempad+now]=num;
                                        now++;
                                        j+=rx.matchedLength();
                                    }else{
                                        QRegExp rx("\\d*");
                                        rx.indexIn(temp,j);
                                        QString r=rx.cap(0);
                                        int num=r.toInt();
                                        data_store[tempad+now]=num;
                                        now++;
                                        j+=rx.matchedLength();
                                    }
                                }else{
                                    QRegExp rx("\\d*");
                                    rx.indexIn(temp,j);
                                    QString r=rx.cap(0);
                                    int num=r.toInt();
                                    data_store[tempad+now]=num;
                                    now++;
                                    j+=rx.matchedLength();
                                }
                            }else if (temp[j]=='\''){
                                data_store[tempad+now]=(int)temp[j+1].toLatin1();
                                now++;
                                j+=3;
                            }
                        }
                    }else if (labels[i][1].toLatin1()=='w'){
                        qDebug()<<"dw";
                        QString temp=labels[i];
                        for (int j=3;j<temp.size();++j){
                            if (temp[j].isDigit()){
                                if (temp[j]=='0'){
                                    if (temp[j+1]=='x'){
                                        QRegExp rx("0x[0-9a-z]*");
                                        rx.indexIn(temp,j);
                                        QString r=rx.cap(0);
                                        qDebug()<<r;
                                        r=QString::number(r.toInt(&ok,16),2);
                                        if (r.size()<16){
                                            QString temp0(16-r.size(),'0');
                                            r=temp0+r;
                                        }
                                        QString r0=split(r,0,8);
                                        QString r1=split(r,8,8);
                                        data_store[tempad+now]=r0.toInt(&ok,2);
                                        data_store[tempad+now+1]=r1.toInt(&ok,2);
                                        now+=2;
                                        j+=rx.matchedLength();
                                    }else{
                                        QRegExp rx("\\d*");
                                        rx.indexIn(temp,j);
                                        QString r=rx.cap(0);
                                        r=QString::number(r.toInt(),2);
                                        if (r.size()<16){
                                            QString temp0(16-r.size(),'0');
                                            r=temp0+r;
                                        }
                                        QString r0=split(r,0,8);
                                        QString r1=split(r,8,8);\
                                        data_store[tempad+now]=r0.toInt(&ok,2);
                                        data_store[tempad+now+1]=r1.toInt(&ok,2);
                                        now+=2;
                                        j+=rx.matchedLength();
                                    }
                                }else{
                                    QRegExp rx("\\d*");
                                    rx.indexIn(temp,j);
                                    QString r=rx.cap(0);
                                    r=QString::number(r.toInt(),2);
                                    if (r.size()<16){
                                        QString temp0(16-r.size(),'0');
                                        r=temp0+r;
                                    }

                                    QString r0=split(r,0,8);
                                    QString r1=split(r,8,8);
                                    data_store[tempad+now]=r0.toInt(&ok,2);
                                    data_store[tempad+now+1]=r1.toInt(&ok,2);
                                    now+=2;
                                    j+=rx.matchedLength();
                                }
                            }else if (temp[j]=='\''){
                                data_store[tempad+now]=(int)temp[j+1].toLatin1();
                                data_store[tempad+now+1]=(int)temp[j+2].toLatin1();
                                now+=2;
                                j+=4;
                            }
                        }
                    }else if (labels[i][1].toLatin1()=='d'){
                        qDebug()<<"dw";
                        QString temp=labels[i];
                        for (int j=3;j<temp.size();++j){
                            if (temp[j].isDigit()){
                                if (temp[j]=='0'){
                                    if (temp[j+1]=='x'){
                                        QRegExp rx("0x[0-9a-z]*");
                                        rx.indexIn(temp,j);
                                        QString r=rx.cap(0);
                                        qDebug()<<r;
                                        r=QString::number(r.toInt(&ok,16),2);
                                        if (r.size()<32){
                                            QString temp0(32-r.size(),'0');
                                            r=temp0+r;
                                        }
                                        QString r0=split(r,0,8);
                                        QString r1=split(r,8,8);
                                        QString r2=split(r,16,8);
                                        QString r3=split(r,24,8);
                                        data_store[tempad+now]=r0.toInt(&ok,2);
                                        data_store[tempad+now+1]=r1.toInt(&ok,2);
                                        data_store[tempad+now+2]=r2.toInt(&ok,2);
                                        data_store[tempad+now+3]=r3.toInt(&ok,2);
                                        now+=4;
                                        j+=rx.matchedLength();
                                    }else{
                                        QRegExp rx("\\d*");
                                        rx.indexIn(temp,j);
                                        QString r=rx.cap(0);
                                        r=QString::number(r.toInt(),2);
                                        if (r.size()<32){
                                            QString temp0(32-r.size(),'0');
                                            r=temp0+r;
                                        }

                                        QString r0=split(r,0,8);
                                        QString r1=split(r,8,8);
                                        QString r2=split(r,16,8);
                                        QString r3=split(r,24,8);
                                        data_store[tempad+now]=r0.toInt(&ok,2);
                                        data_store[tempad+now+1]=r1.toInt(&ok,2);
                                        data_store[tempad+now+2]=r2.toInt(&ok,2);
                                        data_store[tempad+now+3]=r3.toInt(&ok,2);
                                        now+=4;
                                        j+=rx.matchedLength();
                                    }
                                }else{
                                    QRegExp rx("\\d*");
                                    rx.indexIn(temp,j);
                                    QString r=rx.cap(0);
                                    r=QString::number(r.toInt(),2);
                                    if (r.size()<32){
                                        QString temp0(32-r.size(),'0');
                                        r=temp0+r;
                                    }

                                    QString r0=split(r,0,8);
                                    QString r1=split(r,8,8);
                                    QString r2=split(r,16,8);
                                    QString r3=split(r,24,8);
                                    data_store[tempad+now]=r0.toInt(&ok,2);
                                    data_store[tempad+now+1]=r1.toInt(&ok,2);
                                    data_store[tempad+now+2]=r2.toInt(&ok,2);
                                    data_store[tempad+now+3]=r3.toInt(&ok,2);
                                    now+=4;
                                    j+=rx.matchedLength();
                                }
                            }else if (temp[j]=='\''){
                                data_store[tempad+now]=(int)temp[j+1].toLatin1();
                                data_store[tempad+now+1]=(int)temp[j+2].toLatin1();
                                data_store[tempad+now+2]=(int)temp[j+3].toLatin1();
                                data_store[tempad+now+3]=(int)temp[j+4].toLatin1();
                                now+=4;
                                j+=6;
                            }
                        }
                    }
                case 'r':

                    switch (labels[i][3].toLatin1()) {
                    case 'b':
                        qDebug()<<"resb";
                        rx.indexIn(temp,5);
                        r=rx.cap(0);
                        qDebug()<<r;
                        for (int i=0;i<r.toInt();++i){
                            data_store[tempad+now+i]=0;
                        }
                        now+=r.toInt();
                        break;
                    case 'w':
                        qDebug()<<"resw";
                        rx.indexIn(temp,5);
                        r=rx.cap(0);
                        qDebug()<<r;
                        for (int i=0;i<2*r.toInt();++i){
                            data_store[tempad+now+i]=0;
                        }
                        now+=2*r.toInt();
                        break;
                    case 'd':
                        qDebug()<<"resd";
                        rx.indexIn(temp,5);
                        r=rx.cap(0);
                        qDebug()<<r;
                        for (int i=0;i<4*r.toInt();++i){
                            data_store[tempad+now+i]=0;
                        }
                        now+=4*r.toInt();
                        break;

                    default:
                        break;
                    }
                    break;

                default:

                    if (labels[i][0].isDigit()){
                        for (int j=0;j<temp.size();++j){
                            if (temp[j].isDigit()){
                                if (temp[j]=='0'){
                                    if (temp[j+1]=='x'){
                                        QRegExp rx("0x[0-9a-z]*");
                                        rx.indexIn(temp,j);
                                        QString r=rx.cap(0);
                                        qDebug()<<r;
                                        r=QString::number(r.toUInt(&ok,16),2);
                                        if (r.size()<32){
                                            QString temp0(32-r.size(),'0');
                                            r=temp0+r;
                                        }
                                        QString r0=split(r,0,8);
                                        QString r1=split(r,8,8);
                                        QString r2=split(r,16,8);
                                        QString r3=split(r,24,8);
                                        data_store[tempad+now]=r0.toInt(&ok,2);
                                        data_store[tempad+now+1]=r1.toInt(&ok,2);
                                        data_store[tempad+now+2]=r2.toInt(&ok,2);
                                        data_store[tempad+now+3]=r3.toInt(&ok,2);
                                        now+=4;
                                        j+=rx.matchedLength();
                                    }else{
                                        QRegExp rx("\\d*");
                                        rx.indexIn(temp,j);
                                        QString r=rx.cap(0);
                                        r=QString::number(r.toInt(),2);
                                        if (r.size()<32){
                                            QString temp0(32-r.size(),'0');
                                            r=temp0+r;
                                        }

                                        QString r0=split(r,0,8);
                                        QString r1=split(r,8,8);
                                        QString r2=split(r,16,8);
                                        QString r3=split(r,24,8);
                                        data_store[tempad+now]=r0.toInt(&ok,2);
                                        data_store[tempad+now+1]=r1.toInt(&ok,2);
                                        data_store[tempad+now+2]=r2.toInt(&ok,2);
                                        data_store[tempad+now+3]=r3.toInt(&ok,2);
                                        now+=4;
                                        j+=rx.matchedLength();
                                    }
                                }else{
                                    QRegExp rx("\\d*");
                                    rx.indexIn(temp,j);
                                    QString r=rx.cap(0);
                                    r=QString::number(r.toInt(),2);
                                    if (r.size()<32){
                                        QString temp0(32-r.size(),'0');
                                        r=temp0+r;
                                    }

                                    QString r0=split(r,0,8);
                                    QString r1=split(r,8,8);
                                    QString r2=split(r,16,8);
                                    QString r3=split(r,24,8);
                                    data_store[tempad+now]=r0.toInt(&ok,2);
                                    data_store[tempad+now+1]=r1.toInt(&ok,2);
                                    data_store[tempad+now+2]=r2.toInt(&ok,2);
                                    data_store[tempad+now+3]=r3.toInt(&ok,2);
                                    now+=4;
                                    j+=rx.matchedLength();
                                }
                            }else if (temp[j]=='\''){
                                data_store[tempad+now]=(int)temp[j+1].toLatin1();
                                data_store[tempad+now+1]=(int)temp[j+2].toLatin1();
                                data_store[tempad+now+2]=(int)temp[j+3].toLatin1();
                                data_store[tempad+now+3]=(int)temp[j+4].toLatin1();
                                now+=4;
                                j+=6;
                            }
                        }
                    }
                    break;
                }
                labels.erase(labels.begin()+i);
            }
            i--;
        }
    }
}

void Mips::firstprase(){
    doData();
    bool ok;
    for (int i=0;i<labels.size();++i){
        QRegExp rxforbegin("(#|//)BaseAddre:(.*);");
        int pos=rxforbegin.indexIn(labels[i]);
        if (pos>-1){
            BaseAddre=rxforbegin.cap(2).toUInt(&ok,16);
            labels.erase(labels.begin()+i);
        }
        QRegExp rxforcom(".*(#|//)(.*)");
        pos=rxforcom.indexIn(labels[i]);
//        qDebug()<<rxforcom.cap(1)+rxforcom.cap(2);
        if (pos>-1){
            labels[i].replace(QRegExp("(#|//).*"), "");
        }
        QRegExp rxforlabel(".*:");
        pos = rxforlabel.indexIn(labels[i]);
        if (pos > -1) {
            rxforlabel.setPattern("(\\s*)(\\w*)");
            rxforlabel.indexIn(labels[i]);
            if (location[rxforlabel.cap(2)]){
                qDebug()<<"Existing line";
                return;
            }
            location[rxforlabel.cap(2)]=linenum;
            labels[i].replace(QRegExp(".*:"), "");
        }

        QRegExp rxforline(".*;");
        pos = rxforline.indexIn(labels[i]);
        if (pos > -1) {
            labels[i] = rxforline.cap(0);
            linenum++;
            labels[i].replace(QRegExp(";"),"");
        }
        if (labels[i].isEmpty()||allspace(labels[i])){
            labels.erase(labels.begin()+i);
            i--;
        }
    }
    qDebug()<<"\n\n";
    for (int i=0;i<labels.size();++i){
        qDebug()<<labels[i];
    }
}

void Mips::secondprase(){
    std::map<QString,int> inst_set;
    getTheMap(inst_set);
    for (int i=0;i<labels.size();++i){
        QRegExp rx("\\w*");
        int pos = rx.indexIn(labels[i]);
        QString ins=rx.cap(0).toUpper();
        QString temp;
        int instcode=inst_set[ins];
        if (instcode==0&&ins!=QString("LW")){
            qDebug()<<"error"<<labels[i];
            return;
        }
        if (instcode<=5){
            bincode.push_back(lfunc(instcode,labels[i]));
        }else if (instcode<=8){
            bincode.push_back(sfunc(instcode,labels[i]));
        }else if (instcode<=21){
            bincode.push_back(alurfunc(instcode,labels[i]));
        }else if (instcode<=25){
            bincode.push_back(muldivfunc(instcode,labels[i]));
        }else if (instcode<=33){
            bincode.push_back(aluifunc(instcode,labels[i]));
        }else if (instcode<=39){
            bincode.push_back(branchfunc(instcode,labels[i],i,location));
        }else if (instcode<=43){
            bincode.push_back(transfunc(instcode,labels[i],i,BaseAddre,location));
        }else if (instcode<=47){
            bincode.push_back(trfunc(instcode,labels[i]));
        }else if (instcode<=50){
            bincode.push_back(pfunc(instcode,labels[i]));
        }else if (instcode<=52){
            bincode.push_back(trapfunc(instcode,labels[i]));
        }else if (instcode<=55){
            if (instcode==53){
                QString str=labels[i];
                bincode.push_back(aluifunc(31,"lui $at, 64"));
                rx.setPattern("\\$(\\w*)");
                rx.indexIn(str);
                QString bin2=getTheBin(6,0xd);
                int rt=getTheReg(rx.cap(1).toLower());
                bin2+=getTheBin(5,1);
                bin2+=getTheBin(5,rt);
                int pos=rx.pos(1)+rx.cap(1).length();
                rx.setPattern("\\s*,\\s*");
                rx.indexIn(str,pos);
                pos=rx.pos(0)+rx.cap(0).length();
                rx.setPattern("\\w*");
                rx.indexIn(str,pos);
                QString lab=rx.cap(0);
                auto iter=location.find(lab);
                if (iter!=location.end()){
                    bin2+=getTheBin(16,location[lab]<<2);
                }else {
                    bin2+=getTheBin(16,data_address[lab]<<2);
                }
                bincode.push_back(bin2);
            }else
                bincode.push_back(pesfunc(instcode,labels[i]));
        }
    }
    for (int i=0;i<bincode.size();++i){
        memory[(BaseAddre+4*i)>>2]=bincode[i];
    }
}
