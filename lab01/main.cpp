#include <QCoreApplication>
#include<QDebug>
#include<QList>
class student      //创建student类
{
public:
    int number;
    QString name;
    int score1;
    int score2;
};             //定义 姓名name，学号number，课程1score1，课程2score2
//利用bool进行排序
bool compare_name(student s1,student s2)
{
    if (s1.name>s2.name)
        return 1;
    else
        return 0;
}                //姓名排序
bool compare_number(student s1,student s2)
{
    if (s1.number>s2.number)
        return 1;
    else
        return 0;
}                // 学号排序
bool compare_score1(student s1,student s2)
{
    if (s1.score1>s2.score1)
        return 1;
    else
        return 0;
}               //课程1成绩排序
bool compare_score2(student s1,student s2)
{
    if (s1.score2>s2.score2)
        return 1;
    else
        return 0;
}         //课程2成绩排序
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    student student1,student2,student3,student4;
            student1.number=1403130209;
            student1.name="鲁智深";
            student1.score1=80;
            student1.score2=72;
            student2.number=1403140101;
            student2.name="林冲";
            student2.score1=82;
            student2.score2=76;
            student3.number=1403140102;
            student3.name="宋江";
            student3.score1=76;
            student3.score2=85;
            student4.number=1403140103;
            student4.name="武松";
            student4.score1=88;
            student4.score2=80;                    //赋值过程
            QList<student> list;
            list<<student1<<student2<<student3<<student4;
            qDebug("原表格顺序为：");
                qDebug()<<"  学号\t"<<"\t   姓名\t"<<"\t课程1"<<"\t课程2";
                for(int i=0;i<4;i++)
                    {
                        qDebug()<<list.at(i).number<<"\t"<<list.at(i).name<<"\t"<<list.at(i).score1<<"\t"<<list.at(i).score2;
                    }
                    qDebug("---------");
                       std::sort(list.begin(),list.end(),compare_number);
                        qDebug("按学号排序输出");
                        qDebug()<<"  学号\t"<<"\t   姓名\t"<<"\t课程1"<<"\t课程2";
                        for(int i=0;i<4;i++)
                        {
                            qDebug()<<list.at(i).number<<"\t"<<list.at(i).name<<"\t"<<list.at(i).score1<<"\t"<<list.at(i).score2;
                        }
                        qDebug("\n");

                         std:: sort(list.begin(),list.end(),compare_name);
                        qDebug("按姓名排序输出");
                        qDebug()<<"  学号\t"<<"\t   姓名\t"<<"\t课程1"<<"\t课程2";
                        for(int i=0;i<4;i++)
                        {
                            qDebug()<<list.at(i).number<<"\t"<<list.at(i).name<<"\t"<<list.at(i).score1<<"\t"<<list.at(i).score2;
                        }
                        qDebug("\n");

                        std:: sort(list.begin(),list.end(),compare_score1);
                        qDebug("按课程1成绩降序输出");
                        qDebug()<<"  学号\t"<<"\t   姓名\t"<<"\t课程1"<<"\t课程2";
                        for(int i=0;i<4;i++)
                        {
                            qDebug()<<list.at(i).number<<"\t"<<list.at(i).name<<"\t"<<list.at(i).score1<<"\t"<<list.at(i).score2;
                        }
                        qDebug("\n");

                       std:: sort(list.begin(),list.end(),compare_score2);
                        qDebug("按课程2成绩降序输出");
                        qDebug()<<"  学号\t"<<"\t   姓名\t"<<"\t课程1"<<"\t课程2";
                        for(int i=0;i<4;i++)
                        {
                            qDebug()<<list.at(i).number<<"\t"<<list.at(i).name<<"\t"<<list.at(i).score1<<"\t"<<list.at(i).score2;
                        }

    return a.exec();
}
