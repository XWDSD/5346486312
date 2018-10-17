#include <QDebug>
#include <QVector>
#include <QTextStream>
#include <QFile>
#include <QCoreApplication>
namespace SK {
enum SortKind{
    col01    =   0x00000001<<0,         //!< 第1列
    col02    =   0x00000001<<1,         //!< 第2列
    col03    =   0x00000001<<2,         //!< 第3列
    col04    =   0x00000001<<3,         //!< 第4列
    col05    =   0x00000001<<4,         //!< 第5列
    col06    =   0x00000001<<5,         //!< 第6列
    col07    =   0x00000001<<6,         //!< 第7列
    col08    =   0x00000001<<7,         //!< 第8列

};
}


typedef struct{
   QStringList s;
    // 简化结构定义
} studData;

QDebug operator<< (QDebug d, const studData &data) {
    for(int i=0;i<data.s.size();i++)
     {
         d.noquote().nospace()<<QString(data.s.at(i))<<" ";
     }      //简化for循环

    // 补全运算符重载函数，使其可以直接输出studData结构
    return d;
}

// 比较类，用于std::sort第三个参数
class myCmp {
public:
    myCmp(int selectedColumn) { this->currentColumn = selectedColumn; }
    bool operator() (const studData& d1,const studData& d2);
private:
    int currentColumn;
};
#define compare(a)   (d1.s.at(a)>=d2.s.at(a))?  1:0   //定义比较宏
bool myCmp::operator()(const studData &d1, const studData &d2)
{
    bool result = false;
    quint32 sortedColumn = 0x00000001<<currentColumn;
    switch (sortedColumn) {
    case SK::col01:result=compare(1);break;
    case SK::col02:result=compare(2);break;
    case SK::col03:result=compare(3);break;
    case SK::col04:result=compare(4);break;
    case SK::col05:result=compare(5);break;
    case SK::col06:result=compare(6);break;
    case SK::col07:result=compare(7);break;
    case SK::col08:result=compare(8);break;
    default:;break;
    // 补全运算符重载函数
    }
    return result;
}


class ScoreSorter
{
public:
    ScoreSorter(QString dataFile);
    void readFile();
    void doSort();
private:
    QString tempFile;
    QList<studData> data;
    studData list;
    void out_file(quint8 lie);
};


// 请补全
ScoreSorter::ScoreSorter(QString dataFile)
{
    this->tempFile=dataFile;
}


void ScoreSorter::readFile()
{
     QFile file(this->tempFile);
     if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
     {
         qDebug()<<"Can't open the file!"<<endl;
     }
     QString titile(file.readLine());
     this->list.s = titile.split(" ", QString::SkipEmptyParts);
     if((this->list.s).last() == "\n") this->list.s.removeLast();
     studData lastdata;
     while(!file.atEnd())
     {
         QByteArray line = file.readLine();
         QString str(line);
         lastdata.s = str.split(" ", QString::SkipEmptyParts);
         if((lastdata.s).last() == "\n") lastdata.s.removeLast();
         if(lastdata.s.size()==0) continue;
         this->data.append(lastdata);
     }
     file.close();
 }


void ScoreSorter::doSort()
{
    for(int i=1;i<this->list.s.size();i++)
        {
            myCmp cmp(i-1);
            std::sort(this->data.begin() , this->data.end() ,cmp );
            qDebug()<<"排序后输出，当前排序第"<<i+1<<"列：";
            qDebug() << "\t"<< (this->list);
            for(int i=0;i<this->data.size();i++)  qDebug() << this->data.at(i);
            qDebug()<<"====================================================\n";
            this->out_file(i+1);
        }
}



void ScoreSorter::out_file(quint8 lie)
{
    QFile file("sorted_"+this->tempFile);
    file.open(QIODevice::ReadWrite | QIODevice::Append);
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    stream<<QString("排序后输出，当前排序第 ")<<lie <<QString(" 列：")<<"\r\n";
    stream<<"\t";
    for(int j=0;j<this->list.s.size();j++)
        stream<<this->list.s.at(j)<<"\t";
    stream<<"\r\n";
    for(int i=0;i<this->data.size();i++)
    {
        for(int j=0;j<this->list.s.size();j++)
            stream<<this->data.at(i).s.at(j)<<"\t";
        stream<<"\r\n";
    }
    stream<<"===================================================="<<"\r\n\r\n";
    file.close();
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    //qInstallMessageHandler(myMessageOutput);
    QString datafile = "C:/Users/ASUS/Desktop/github/5346486312/homework02/data.txt";

    // 如果排序后文件已存在，则删除之
    QFile f("sorted_"+datafile);
    if (f.exists())
    {
        f.remove();
    }

    ScoreSorter s(datafile);
    s.readFile();        //读取
    s.doSort();           //执行
    return 0;
}
