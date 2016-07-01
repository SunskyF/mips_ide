#include "mipscoe.h"
#include <QRegExp>
#include <QDebug>
QString regname[32]={
    "zero","at","v0","v1","a0","a1","a2","a3",
    "t0","t1","t2","t3","t4","t5","t6","t7",
    "s0","s1","s2","s3","s4","s5","s6","s7",
    "t8","t9","k0","k1","gp","sp","fp","ra"
};
Mipscoe::Mipscoe()
{
    hex.clear();
    instructions.clear();
}
Mipscoe::Mipscoe(const Mipscoe &m){
    hex=m.hex;
    instructions=m.instructions;
}

Mipscoe::Mipscoe(QTextStream &in){
    hex.clear();
    instructions.clear();
    QString temp=in.readAll();
    temp.replace(QRegExp("memory_initialization_radix=\\d*;"),"");
    temp.replace(QRegExp("memory_initialization_vector="),"");
    QRegExp rx("[0-9A-Za-z]*(,|;)");
    int pos=0;
    while (pos!=-1){
        rx.indexIn(temp,pos);
        if (rx.cap(0)!="") hex.push_back(rx.cap(0).replace(QRegExp("(,|;)"),""));
        pos=rx.pos(0)+rx.cap(0).length();
    }
}
Mipscoe& Mipscoe::operator=(const Mipscoe m){
    hex=m.hex;
    instructions=m.instructions;
    return *this;
}

int Mipscoe::length(){
    return instructions.size()?instructions.size():hex.size();
}

QString Mipscoe::split(QString s,int start,int len){
    QString result;
    for (int i=0;i<len;++i){
        result.push_back(s[start+i]);
    }
    return result;
}

QString Mipscoe::R_Type(QString s){
    int rs,rt,rd,shamt,func;
    bool ok;
    rs=split(s,6,5).toInt(&ok,2);
    rt=split(s,11,5).toInt(&ok,2);
    rd=split(s,16,5).toInt(&ok,2);
    shamt=split(s,21,5).toInt(&ok,2);
    func=split(s,26,6).toInt(&ok,2);
//    qDebug()<<rs<<rt<<rd<<shamt<<func;
    QString result;
    switch (func){
    case 0x20:
        result+="add";
        break;
    case 0x21:
        result+="addu";
        break;
    case 0x22:
        result+="sub";
        break;
    case 0x23:
        result+="subu";
        break;
    case 0x2A:
        result+="slt";
        break;
    case 0x2B:
        result+="sltu";
    case 0x24:
        result+="and";
        break;
    case 0x25:
        result+="or";
        break;
    case 0x26:
        result+="xor";
        break;
    case 0x27:
        result+="nor";
        break;
    case 0x0:
        result+="sll";
        break;
    case 0x2:
        result+="srl";
        break;
    case 0x3:
        result+="sra";
        break;
    case 0x18:
        result+="mult";
        break;
    case 0x19:
        result+="multu";
        break;
    case 0x1A:
        result+="div";
        break;
    case 0x1B:
        result+="divu";
        break;
    case 0x9:
        result+="jalr";
        break;
    case 0x8:
        result+="jr";
        break;
    case 0x10:
        result+="mfhi";
        break;
    case 0x12:
        result+="mflo";
        break;
    case 0x11:
        result+="mthi";
        break;
    case 0x13:
        result+="mtlo";
        break;
    case 0xD:
        result+="break";
        break;
    case 0xC:
        result+="syscall";
        break;
    default:
        break;
    }
    switch (func){
    case 0x20:
    case 0x21:
    case 0x22:
    case 0x23:
    case 0x2A:
    case 0x2B:
    case 0x24:
    case 0x25:
    case 0x26:
    case 0x27:
        result+=" $"+regname[rd]+", $"+regname[rs]+", $"+regname[rt]+";";
        break;
    case 0x0:
    case 0x2:
    case 0x3:
        result+=" $"+regname[rd]+", $"+regname[rt]+", "+QString::number(shamt)+";";
        break;
    case 0x18:
    case 0x19:
    case 0x1A:
    case 0x1B:
        result+=" $"+regname[rs]+", $"+regname[rt]+";";
        break;
    case 0x9:
        result+=" $"+regname[rs]+", $"+regname[rd]+";";
        break;
    case 0x8:
        result+=" $"+regname[rs]+";";
        break;
    case 0x10:
    case 0x12:
        result+=" $"+regname[rd]+";";
        break;
    case 0x11:
    case 0x13:
        result+=" $"+regname[rs]+";";
        break;
    case 0xD:
        break;
    case 0xC:
        break;
    default:
        break;
    }
    return result;
}

QString Mipscoe::I_Type(QString s){
    int rs,rt,rd,offset;
    bool ok;
    int op=split(s,0,6).toInt(&ok,2);
    rs=split(s,6,5).toInt(&ok,2);
    rt=split(s,11,5).toInt(&ok,2);
    rd=split(s,16,5).toInt(&ok,2);
    if (s[16]=='0'){
        offset=split(s,17,15).toInt(&ok,2);
    }else{
        QString temp=split(s,17,15);
        qDebug()<<temp;
        for (int i=0;i<temp.length();++i){
            if (temp[i]=='0')
                temp[i]='1';
            else
                temp[i]='0';
        }
        offset=-(temp.toInt(&ok,2)+1);
        qDebug()<<temp<<offset;
    }
    QString result;
    switch (op){
    case 0x23:
        result+="lw";
        break;
    case 0x20:
        result+="lb";
        break;
    case 0x24:
        result+="lbu";
        break;
    case 0x21:
        result+="lh";
        break;
    case 0x25:
        result+="lhu";
        break;
    case 0x2B:
        result+="sw";
        break;
    case 0x28:
        result+="sb";
        break;
    case 0x29:
        result+="sh";
        break;
    case 0x8:
        result+="addi";
        break;
    case 0x9:
        result+="addiu";
        break;
    case 0xC:
        result+="andi";
        break;
    case 0xD:
        result+="ori";
        break;
    case 0xE:
        result+="xori";
        break;
    case 0xF:
        result+="lui";
        break;
    case 0xA:
        result+="slti";
        break;
    case 0xB:
        result+="sltiu";
        break;
    case 0x4:
        result+="beq";
        break;
    case 0x5:
        result+="bne";
        break;
    case 0x6:
        result+="blez";
        break;
    case 0x7:
        result+="bgtz";
        break;
    case 0x1:
        if(rd==1) result+="bgez";\
        else result+="bltz";
        break;
    case 0x10:
        if (offset==0x18) result+="eret";
        else if (rs==0) result+="mfc0";
        else if (rs==4) result+="mtc0";
        break;
    default:
        break;
    }
    switch (op){
    case 0x23:
    case 0x20:
    case 0x24:
    case 0x21:
    case 0x25:
    case 0x2B:
    case 0x28:
    case 0x29:
        result+=" $"+regname[rt]+", "+QString::number(offset)+"$("+regname[rs]+")";
        break;
    case 0x8:
    case 0x9:
    case 0xC:
    case 0xD:
    case 0xE:
    case 0xF:
    case 0xA:
    case 0xB:
        result+=" $"+regname[rt]+", $"+regname[rs]+","+QString::number(offset)+";";
        break;
    case 0x4:
    case 0x5:
        result+=" $"+regname[rs]+", $"+regname[rt]+","+QString::number(offset)+";";
        break;
    case 0x6:
    case 0x7:
    case 0x1:
        result+=" $"+regname[rs]+","+QString::number(offset>>2)+";";
        break;
    case 0x10:
        if (offset!=0x18){
            result+=" $"+regname[rd]+", $"+regname[rt]+";";
        }
        break;
    default:
        break;
    }
    return result;
}

QString Mipscoe::J_Type(QString s){
    bool ok;
    int op=split(s,0,6).toInt(&ok,2);
    QString result;
    switch (op){
    case 2:
        result+="j";
        break;
    case 3:
        result+="jal";
        break;
    }
    result+=" "+QString::number(split(s,6,26).toLongLong(&ok,2)<<2,16)+";";
    return result;
}

void Mipscoe::prase(){
	for (int i = 0; i < hex.size(); ++i){
        bool ok;
        QString temp= QString::number(hex[i].toLongLong(&ok,16),2);
        QString ins(std::max(32-temp.size(),0),'0');
        ins+=temp;
        QString op=split(ins,0,6);
        if (op=="000000"){
            instructions.push_back(R_Type(ins));
        }else if (op=="000010"||op=="000011"){
            instructions.push_back(J_Type(ins));
        }else{
            instructions.push_back(I_Type(ins));
        }
	}
}

QString Mipscoe::getInst(int i){
    if (instructions.size()==0)
        return "Error: No Instruction";
    return instructions[i];
}
