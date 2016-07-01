#include "mips_utl.h"
#include <QDebug>

bool allspace(QString s){
    for (int i=0;i<s.length();++i){
        if (s[i]!=' ') return false;
    }
    return true;
}

bool isDigitstr(QString src){
    QByteArray ba = src.toLatin1();//QString 转换为 char*
    const char *s = ba.data();
    if (*s=='-') *s++;
    while(*s && *s>='0' && *s<='9') s++;

    if (*s)
    { //不是纯数字
        return 0;
    }
    else
    { //纯数字
        return 1;
    }
}


void getTheMap(std::map<QString,int> &set_map){
    QString ir_set[56] = {"","LW", "LB","LBU","LH","LHU",
                         "SW","SB","SH",
                         "ADD","ADDU","SUB","SUBU","SLT","SLTU","AND","OR","XOR","NOR","SLL","SRL","SRA",
                         "MULT","MULTU","DIV","DIVU",
                         "ADDI","ADDIU","ANDI","ORI","XORI","LUI","SLTI","SLTIU",
                         "BEQ","BNE","BLEZ","BGTZ","BLTZ","BGEZ",
                         "J","JAL","JALR","JR",
                         "MFHI","MFLO","MTHI","MTLO",
                         "ERET","MFC0","MTC0",
                         "BREAK","SYSCALL",
                         "LA","MOVE","LI"};
    for (int i=0;i<56;++i){
        set_map[ir_set[i]]=i;
    }
}

int getTheReg(QString str){
    QString regname[32]={
        "zero","at","v0","v1","a0","a1","a2","a3",
        "t0","t1","t2","t3","t4","t5","t6","t7",
        "s0","s1","s2","s3","s4","s5","s6","s7",
        "t8","t9","k0","k1","gp","sp","fp","ra"
    };
    for (int i=0;i<32;++i){
        if (regname[i]==str){
            return i;
        }
    }
    return -1;
}
QString getTheBin(int num,int dig){
    if (dig>=0){
        QString bincode;
        while (dig){
            int temp=dig%2;
            if (temp)
                bincode+='1';
            else bincode+='0';
            dig/=2;
        }
        QString temp;
        for (int i=bincode.size()-1;i>=0;--i){
            temp+=bincode[i];
        }
        QString res(num-bincode.size(),'0');
        res+=temp;
        return res;
    }else {
        dig=~dig+1;
        QString bincode;
        while (dig){
            int temp=dig%2;
            if (temp)
                bincode+='0';
            else bincode+='1';
            dig/=2;
        }
        if (bincode[0]=='0'){
            bincode[0]='1';
        }else {
            int i;
            for (i=0;i<=bincode.size()-1;++i){
                if (bincode[i]=='0'){
                    bincode[i]='1';
                    break;
                }
                else
                    bincode[i]='0';
            }
            if (i==bincode.size()&&bincode[0]=='0')
                bincode.push_front('1');
        }

        QString temp;
        for (int i=bincode.size()-1;i>=0;--i){
            temp+=bincode[i];
        }
        QString res(num-bincode.size(),'1');
        res+=temp;
        return res;
    }
}
QString getTheBin(int num,long long dig){
    if (dig>=0){
        QString bincode;
        while (dig){
            int temp=dig%2;
            if (temp)
                bincode+='1';
            else bincode+='0';
            dig/=2;
        }
        QString temp;
        for (int i=bincode.size()-1;i>=0;--i){
            temp+=bincode[i];
        }
        QString res(num-bincode.size(),'0');
        res+=temp;
        return res;
    }else {
        dig=~dig+1;
        QString bincode;
        while (dig){
            int temp=dig%2;
            if (temp)
                bincode+='1';
            else bincode+='0';
            dig/=2;
        }
        QString temp;
        for (int i=bincode.size()-1;i>=0;--i){
            temp+=bincode[i];
        }
        QString res(num-bincode.size(),'1');
        res+=temp;
        return res;
    }
}
QString bin2hex(QString str){
    QString hex="0123456789ABCDEF";
    QString res;
    int sum=0;
    for (int i=0;i<str.size();++i){
        if (i!=0&&i%4==0){
            res+=hex[sum];
            sum=0;
        }
        QString temp=str.at(i);
        sum=sum*2+temp.toInt();
    }
    res+=hex[sum];
    return res;
}

QString lfunc(int inst,QString str){
    int opcode;
    int offset;
    int rs;
    int rt;
    switch (inst) {
    case 1:
        opcode=0x23;
        break;
    case 2:
        opcode=0x20;
        break;
    case 3:
        opcode=0x24;
        break;
    case 4:
        opcode=0x21;
        break;
    case 5:
        opcode=0x25;
        break;
    default:
        break;
    }
    QRegExp rx("(-?\\d*)\\(\\$(\\w*)\\)");
    int pos = rx.indexIn(str);
    if (pos>-1){
        offset=rx.cap(1).toInt();
        rs=getTheReg(rx.cap(2).toLower());
    }
    rx.setPattern("\\$(\\w*)");
    rx.indexIn(str);
    rt=getTheReg(rx.cap(1).toLower());
    QString bin;
    bin+=getTheBin(6,opcode);
    bin+=getTheBin(5,rs);
    bin+=getTheBin(5,rt);
    bin+=getTheBin(16,offset);
    return bin;
}

QString sfunc(int inst,QString str){
    int opcode;
    int offset;
    int rs;
    int rt;
    switch (inst) {
    case 6:
        opcode=0x2b;
        break;
    case 7:
        opcode=0x28;
        break;
    case 8:
        opcode=0x29;
        break;
    default:
        break;
    }
    QRegExp rx("(-?\\d*)\\(\\$(\\w*)\\)");
    int pos = rx.indexIn(str);
    if (pos>-1){
        offset=rx.cap(1).toInt();
        rs=getTheReg(rx.cap(2).toLower());
    }
    rx.setPattern("\\$(\\w*)");
    rx.indexIn(str);
    rt=getTheReg(rx.cap(1).toLower());
    QString bin;
    bin+=getTheBin(6,opcode);
    bin+=getTheBin(5,rs);
    bin+=getTheBin(5,rt);
    bin+=getTheBin(16,offset);
    return bin;
}

QString alurfunc(int inst,QString str){
    int opcode=0;
    int func;
    int shamt=0;
    int rs;
    int rt;
    int rd;
    QString bin;
    switch (inst) {
    case 9:
        func=0x20;
        break;
    case 10:
        func=0x21;
        break;
    case 11:
        func=0x22;
        break;
    case 12:
        func=0x23;
        break;
    case 13:
        func=0x2a;
        break;
    case 14:
        func=0x2b;
        break;
    case 15:
        func=0x24;
        break;
    case 16:
        func=0x25;
        break;
    case 17:
        func=0x26;
        break;
    case 18:
        func=0x27;
        break;
    case 19:
        func=0x0;
        break;
    case 20:
        func=0x2;
        break;
    case 21:
        func=0x3;
        break;
    default:
        break;
    }
    QRegExp rx;
    int pos=0;
    if (inst<19){
        rx.setPattern("\\$(\\w*)");
        rx.indexIn(str,pos);
        rd=getTheReg(rx.cap(1).toLower());
        pos=rx.pos(1)+rx.cap(1).length();
        rx.indexIn(str,pos);
        rs=getTheReg(rx.cap(1).toLower());
        pos=rx.pos(1)+rx.cap(1).length();
        rx.indexIn(str,pos);
        rt=getTheReg(rx.cap(1).toLower());
        bin+=getTheBin(6,opcode);
        bin+=getTheBin(5,rs);
        bin+=getTheBin(5,rt);
        bin+=getTheBin(5,rd);
        bin+=getTheBin(5,shamt);
        bin+=getTheBin(6,func);
    }else {
        rs=0;
        rx.setPattern("\\$(\\w*)");
        rx.indexIn(str,pos);
        rd=getTheReg(rx.cap(1).toLower());
        pos=rx.pos(1)+rx.cap(1).length();

        rx.indexIn(str,pos);
        rt=getTheReg(rx.cap(1).toLower());
        pos=rx.pos(1)+rx.cap(1).length();

        rx.setPattern("\\s*,\\s*");
        rx.indexIn(str,pos);
        pos=rx.pos(0)+rx.cap(0).length();
        rx.setPattern("\\d*");
        rx.indexIn(str,pos);
        shamt=rx.cap(0).toInt();
        bin+=getTheBin(6,opcode);
        bin+=getTheBin(5,rs);
        bin+=getTheBin(5,rt);
        bin+=getTheBin(5,rd);
        bin+=getTheBin(5,shamt);
        bin+=getTheBin(6,func);
    }
    return bin;
}

QString muldivfunc(int inst,QString str){
    int opcode=0;
    int func;
    int rs;
    int rt;
    switch (inst) {
    case 22:
        func=0x18;
        break;
    case 23:
        func=0x19;
        break;
    case 24:
        func=0x1A;
        break;
    case 25:
        func=0x1B;
        break;
    default:
        break;
    }
    QRegExp rx;
    rx.setPattern("\\$(\\w*)");
    rx.indexIn(str);
    rs=getTheReg(rx.cap(1).toLower());
    int pos=rx.pos(1)+rx.cap(1).length();
    rx.setPattern("\\$(\\w*)");
    rx.indexIn(str,pos);
    rt=getTheReg(rx.cap(1).toLower());
    QString bin;
    bin+=getTheBin(6,opcode);
    bin+=getTheBin(5,rs);
    bin+=getTheBin(5,rt);
    bin+=getTheBin(10,0);
    bin+=getTheBin(6,func);
    return bin;
}

QString aluifunc(int inst,QString str){
    int opcode;
    int imm;
    int rs;
    int rt;
    switch (inst) {
    case 26:
        opcode=0x8;
        break;
    case 27:
        opcode=0x9;
        break;
    case 28:
        opcode=0xC;
        break;
    case 29:
        opcode=0xD;
        break;
    case 30:
        opcode=0xE;
        break;
    case 31:
        opcode=0xF;
        break;
    case 32:
        opcode=0xA;
        break;
    case 33:
        opcode=0xB;
        break;
    default:
        break;
    }
    QRegExp rx;
    rx.setPattern("\\$(\\w*)");
    rx.indexIn(str);
    rt=getTheReg(rx.cap(1).toLower());
    int pos=rx.pos(1)+rx.cap(1).length();
    rx.setPattern("\\$(\\w*)");
    rx.indexIn(str,pos);
    rs=getTheReg(rx.cap(1).toLower());
    pos=rx.pos(1)+rx.cap(1).length();
    rx.setPattern("\\s*,\\s*");
    rx.indexIn(str,pos);
    pos=rx.pos(0)+rx.cap(0).length();
    rx.setPattern("-?\\d*");
    rx.indexIn(str,pos);
    imm=rx.cap(0).toInt();
    QString bin;
    if (inst==31){
        QRegExp rx;
        rx.setPattern("\\$(\\w*)");
        rx.indexIn(str);
        rt=getTheReg(rx.cap(1).toLower());
        int pos0=rx.pos(1)+rx.cap(1).length();
        rx.setPattern("\\s*,\\s*");
        rx.indexIn(str,pos0);
        pos0=rx.pos(0)+rx.cap(0).length();
        rx.setPattern("-?\\d*");
        rx.indexIn(str,pos0);
        imm=rx.cap(0).toInt();
        bin+=getTheBin(6,opcode);
        bin+=getTheBin(5,0);
        bin+=getTheBin(5,rt);
        bin+=getTheBin(16,imm);
        return bin;
    }
    bin+=getTheBin(6,opcode);
    bin+=getTheBin(5,rs);
    bin+=getTheBin(5,rt);
    bin+=getTheBin(16,imm);
    return bin;
}
//******************************************4.16
QString branchfunc(int inst,QString str, int pc,std::map<QString,long long> &m){
    int opcode;
    int offset;
    int rs;
    int rt;
    switch (inst) {
    case 34:
        opcode=0x4;
        break;
    case 35:
        opcode=0x5;
        break;
    case 36:
        opcode=0x6;
        rt=0;
        break;
    case 37:
        opcode=0x7;
        rt=0;
        break;
    case 38:
        opcode=0x1;
        rt=0;
        break;
    case 39:
        opcode=0x1;
        rt=1;
        break;
    default:
        break;
    }
    QRegExp rx;
    QString bin;
    if (inst<36){
        rx.setPattern("\\$(\\w*)");
        rx.indexIn(str);
        rs=getTheReg(rx.cap(1).toLower());
        int pos=rx.pos(1)+rx.cap(1).length();
        rx.indexIn(str,pos);
        rt=getTheReg(rx.cap(1).toLower());
        pos=rx.pos(1)+rx.cap(1).length();

        rx.setPattern("\\s*,\\s*");
        rx.indexIn(str,pos);
        pos=rx.pos(0)+rx.cap(0).length();
        rx.setPattern("-?\\w*");
        rx.indexIn(str,pos);
        if (isDigitstr(rx.cap(0))){
            offset=rx.cap(0).toInt();
        }else{
            int labellocation=m[rx.cap(0)];
            offset=labellocation-(pc+1);
        }
        bin+=getTheBin(6,opcode);
        bin+=getTheBin(5,rs);
        bin+=getTheBin(5,rt);
        bin+=getTheBin(16,offset);
    }else {
        rx.setPattern("\\$(\\w*)");
        rx.indexIn(str);
        rs=getTheReg(rx.cap(1).toLower());
        int pos=rx.pos(1)+rx.cap(1).length();
        rx.setPattern("\\s*,\\s*");
        rx.indexIn(str,pos);
        pos=rx.pos(0)+rx.cap(0).length();
        rx.setPattern("-?\\w*");
        rx.indexIn(str,pos);
        if (isDigitstr(rx.cap(0))){
            offset=rx.cap(0).toInt();
        }else{
            int labellocation=m[rx.cap(0)];
            offset=labellocation-(pc+1);
        }
        rt=0;
        bin+=getTheBin(6,opcode);
        bin+=getTheBin(5,rs);
        bin+=getTheBin(5,rt);
        bin+=getTheBin(16,offset);
    }
//    qDebug()<<offset;
    return bin;
}
/*4.18   */
QString transfunc(int inst,QString str, int pc, long long textaddress,std::map<QString, long long> &m){
    int opcode;
    int func;
    QString bin;
    QRegExp rx;
    switch (inst) {
    case 40:
        opcode=0x2;
        break;
    case 41:
        opcode=0x3;
        break;
    case 42:
        opcode=0x0;
        func=0x9;
        break;
    case 43:
        opcode=0x0;
        func=0x8;
        break;
    default:
        break;
    }
    if (inst<=41){
        QRegExp rx;
        rx.setPattern("(\\w*\\s*)");
        rx.indexIn(str);
        int pos=rx.pos(0)+rx.cap(0).length();
        rx.setPattern("\\w*");
        rx.indexIn(str,pos);
        QString jlabel=rx.cap(0);
        bin+=getTheBin(6,opcode);
        long long offset=(m[jlabel]*4)+textaddress;
        bin+=getTheBin(26,offset>>2);
    }else {
        int rs,rt,rd;
        if (inst==42){
            rx.setPattern("\\$(\\w*)");
            rx.indexIn(str);
            rs=getTheReg(rx.cap(1).toLower());
            int pos=rx.pos(1)+rx.cap(1).length();
            rx.indexIn(str,pos);
            rd=getTheReg(rx.cap(1).toLower());
            bin+=getTheBin(6,opcode);
            bin+=getTheBin(5,rs);
            bin+=getTheBin(5,0);
            bin+=getTheBin(5,rd);
            bin+=getTheBin(5,0);
            bin+=getTheBin(6,func);
        }else {
            rx.setPattern("\\$(\\w*)");
            rx.indexIn(str);
            rs=getTheReg(rx.cap(1).toLower());
            bin+=getTheBin(6,opcode);
            bin+=getTheBin(5,rs);
            bin+=getTheBin(15,0);
            bin+=getTheBin(6,func);
        }
    }
    return bin;
}
/*4.19 */
QString trfunc(int inst,QString str){
    int opcode=0,func;
    switch (inst) {
    case 44:
        func=0x10;
        break;
    case 45:
        func=0x12;
        break;
    case 46:
        func=0x11;
        break;
    case 47:
        func=0x13;
        break;
    default:
        break;
    }
    QRegExp rx;
    QString bin;
    int rs=0,rt=0,rd=0;
    rx.setPattern("\\$(\\w*)");
    rx.indexIn(str);
    if (inst<=45) rd=getTheReg(rx.cap(1).toLower());
    else rs=getTheReg(rx.cap(1).toLower());
    bin+=getTheBin(6,opcode);
    bin+=getTheBin(5,rs);
    bin+=getTheBin(5,rt);
    bin+=getTheBin(5,rd);
    bin+=getTheBin(5,0);
    bin+=getTheBin(6,func);
    return bin;
}
/*4.19 */
QString pfunc(int inst,QString str){
    int opcode=0x10;
    int func;
    int rt,rs,rd;
    QString bin;
    if (inst==48){
        func=0x18;
        bin+=getTheBin(6,opcode);
        bin+=getTheBin(1,1);
        bin+=getTheBin(19,0);
        bin+=getTheBin(6,func);
    }else {

        QRegExp rx;
        rx.setPattern("\\$(\\w*)");
        rx.indexIn(str);
        rt=getTheReg(rx.cap(1).toLower());
        int pos=rx.pos(1)+rx.cap(1).length();
        rx.setPattern("\\$(\\w*)");
        rx.indexIn(str,pos);
        rd=getTheReg(rx.cap(1).toLower());
        if (inst==49) rs=0x0;
        else if (inst==50) {
            rs=0x4;
            int temp=rt;
            rt=rd;
            rd=temp;
        }
        bin=getTheBin(6,opcode)+getTheBin(5,rs)+getTheBin(5,rt)+getTheBin(5,rd)+getTheBin(11,0);
    }
    return bin;
}
/*4.19 */
QString trapfunc(int inst,QString str){
    int opcode=0;
    int func;
    int code;
    QString bin;
    switch (inst) {
    case 51:
        func=0xD;
        bin+=getTheBin(6,opcode);
        bin+=getTheBin(20,code);
        bin+=getTheBin(6,func);
        break;
    case 52:
        func=0xC;
        bin+=getTheBin(6,opcode);
        bin+=getTheBin(20,0);
        bin+=getTheBin(6,func);
        break;
    default:
        break;
    }
    return bin;
}

QString pesfunc(int inst,QString str){
    int opcode;
    int func;
    int rd,rs,rt;
    int pos;
    QString bin;
    QRegExp rx;
    if (inst==54){
        opcode=0x0;
        func=0x21;
        rx.setPattern("\\$(\\w*)");
        rx.indexIn(str);
        rd=getTheReg(rx.cap(1).toLower());
        pos=rx.pos(1)+rx.cap(1).length();
        rx.indexIn(str,pos);
        rt=getTheReg(rx.cap(1).toLower());
        bin+=getTheBin(6,opcode);
        bin+=getTheBin(5,0);
        bin+=getTheBin(5,rt);
        bin+=getTheBin(5,rd);
        bin+=getTheBin(5,0);
        bin+=getTheBin(6,func);
    }else if (inst==55){
        QString temp="ori";
        rx.setPattern("\\$(\\w*)");
        rx.indexIn(str);
        QString rdstr=rx.cap(1).toLower();
        temp=temp+" "+"$"+rdstr+", $zero, ";
        pos=rx.pos(1)+rx.cap(1).length();
        rx.setPattern("\\s*,\\s*");
        rx.indexIn(str,pos);
        pos=rx.pos(0)+rx.cap(0).length();
        rx.setPattern("\\d*");
        rx.indexIn(str,pos);
        temp+=rx.cap(0);
        bin=aluifunc(29,temp);
    }
    return bin;
}
