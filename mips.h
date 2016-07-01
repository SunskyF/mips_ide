#include <vector>
#include <map>
#include <QString>
#include <QTextStream>
#ifndef MIPS_H
#define MIPS_H

class Mips{
public:
    Mips();
    Mips(const Mips &m);
    Mips(QTextStream &in);
    void firstprase();
    void secondprase();
    void clear();
    bool isempty();
    void debugshow();
    int length();
    QString getBincode(int index);
    Mips& operator=(const Mips m);
    QString currenttextaddress(long long offset);
    QString currentdataaddress(long long offset);
    void doData();

    std::vector<QString> memory;

    unsigned int BaseAddre=0x00000000;
    unsigned int DataAddre=0x00001000;\
    std::map<unsigned int,int> data_store;//to store data

    QString split(QString s,int start,int len){
        QString result;
        for (int i=0;i<len;++i){
            result.push_back(s[start+i]);
        }
        return result;
    }
private:


    int linenum=0;
    std::vector<QString> labels;//the instructions
    std::map<QString,long long> location;//the label locations
    std::vector<QString> bincode;//binary code
    std::map<QString,long long> data_address;//to label data

};

#endif // MIPS_H
