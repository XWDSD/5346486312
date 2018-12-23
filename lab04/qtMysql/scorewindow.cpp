#include "scorewindow.h"
#include "ui_scorewindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QString>
#include <QDebug>


ScoreWindow::ScoreWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ScoreWindow)
{
    ui->setupUi(this);

    //加载数据库驱动
    db = QSqlDatabase::addDatabase("QMYSQL");
    //设置账号密码
    db.setHostName("localhost");//设置主机地址
    db.setPort(3306);           //设置端口
    db.setUserName("root");     //设置用户名
    db.setPassword("");         //设置密码
    db.setDatabaseName("test"); //设置数据库名称

    QPushButton *btnQuery=ui->btnQuery;
    connect(btnQuery,&QPushButton::clicked,this,&ScoreWindow::on_btnQuery_clicked);
}

ScoreWindow::~ScoreWindow()
{
    delete ui;
}

void ScoreWindow::on_btnQuery_clicked()
{
    if(db.isOpen())
        db.close();
    if(!db.open()){
        QMessageBox::critical(this,tr("错误"),tr("无法连接数据库！\n请查询数据库连接装置。"));
        return;
    }
    else{
        QString str;
        if(ui->radioStNumber->isChecked())
            str=QString("where sNumber='%1'").arg(ui->edtInput->text());
        else if(ui->radioStName->isChecked())
            str=QString("where sName='%1'").arg(ui->edtInput->text());
        QString queryStr=QString
                ("SELECT * FROM t_stud_info %1;").arg(str);
        queryStudentInfo(queryStr);//查询学生的输入
        queryStr = QString("select t_stud_info.sName as '姓名',t_courses.courseName as '课程名称',"
                                  "t_stud_course_info.scScores as '成绩',t_courses.courseCredit as '学分' "
                                  "from "
                                  "(t_stud_course_info inner join t_courses on t_stud_course_info.scCourseID = t_courses.courseID) "
                                  "inner join t_stud_info on t_stud_info.sNumber = t_stud_course_info.scNumber "
                                  "%1 "
                                  "order by t_stud_course_info.scNumber "
                                  ";").arg(str);
        queryStudentScore(queryStr);  //查询学生的成绩
        calculatePoints(queryResult); //计算绩点
        updateInfoWindow(queryResult);//更新输入窗口
        OutputTheInfo("t_courses","t_courses.csv");
        OutputTheInfo("t_stud_course_info","t_stud_course_info.csv");
        OutputTheInfo("t_stud_info","t_stud_info.csv");
        db.close();

    }

}


void ScoreWindow::queryStudentInfo(QString query1)
{
    QSqlQuery query(query1);
    //查询t_stud_info
    while(query.next())
    {
        ui->number->setText(query.value(0).toString());
        ui->name->setText(query.value(1).toString());
        ui->major->setText(query.value(2).toString());
        ui->grade->setText(query.value(3).toString());
    }

}

void ScoreWindow::queryStudentScore(QString str2)
{
    sum = 0;
    QSqlQuery query(str2);
    QStringList Result;
    Result.append("");
    while(query.next())
    {
        course[sum] = query.value(1).toString();
        Result.append(course[sum]);
        grade[sum] = query.value(2).toString();
        Result.append(grade[sum]);
        credit[sum] = query.value(3).toString();
        Result.append(credit[sum]);
        sum++;
    }
    if(sum==0){
        QMessageBox::information(this,tr("Error Message!"),tr("No select Data！"));
    }
    queryResult=Result;
}


void ScoreWindow::calculatePoints(QStringList str3)//计算绩点
{
    int i=0;
    int j=0;
    int k=0;

    double addCredit1 = 0;
    for (i = 0;i<sum;i++)
    {
        double GPA1;
        if(grade[i]=="优秀"||grade[i]=="A")  GPA[j++] = "4";
        else if(grade[i]=="良好"||grade[i]=="B")  GPA[j++] = "3.5";
        else if(grade[i]=="中等"||grade[i]=="C")  GPA[j++] = "2.5";
        else if(grade[i]=="及格"||grade[i]=="D")  GPA[j++] = "1.5";
        else if(grade[i]=="旷考"||grade[i]=="作弊"||grade[i]=="E")  GPA[j++] = "0";
        else if(grade[i]=="缓考"){
            GPA[j++] = "0";
            addCredit[k]=credit[i];
            addCredit1 += addCredit[k].toDouble();
            k++;
        }
        else {
            double a=grade[i].toDouble();
            if(a<60) GPA[j++] = "0";
            else if(90<=a&&a<=100)
            {
               GPA1 =4.00;
               GPA[j++] = QString("%1").arg(GPA1);
            }
            else if(86<=a&&a<=89)
            {
               GPA1 =3.70;
               GPA[j++] = QString("%1").arg(GPA1);
            }
            else if(82<=a&&a<=85)
            {
               GPA1 =3.30;
               GPA[j++] = QString("%1").arg(GPA1);
            }
            else if(79<=a&&a<=81)
            {
               GPA1 =3.00;
               GPA[j++] = QString("%1").arg(GPA1);
            }
            else if(75<=a&&a<=78)
            {
               GPA1 =2.70;
               GPA[j++] = QString("%1").arg(GPA1);
            }
            else if(71<=a&&a<=74)
            {
               GPA1 =2.30;
               GPA[j++] = QString("%1").arg(GPA1);
            }
            else if(68<=a&&a<=70)
            {
               GPA1 =2.00;
               GPA[j++] = QString("%1").arg(GPA1);
            }
            else if(64<=a&&a<=67)
            {
               GPA1 =1.70;
               GPA[j++] = QString("%1").arg(GPA1);
            }
            else if(60<=a&&a<=63)
            {
               GPA1 =1.30;
               GPA[j++] = QString("%1").arg(GPA1);
            }
        }
    }


    double GPA2= 0;
    for(i = 0;i < sum;i++)
    {
        GPA2 = GPA2 + GPA[i].toDouble()/sum;
    }
    finalGPA = QString("绩点：%1").arg(GPA2);
    ui->GPA->setText(finalGPA);
    for(i=0;i<sum;i++)
    {
       str3.append(GPA[i]);
    }
    str3.append(finalGPA);
    queryResult2 = str3;
}

void ScoreWindow::updateInfoWindow(QStringList)
{
    ui->textEdit->clear();
    QString st[7];
    st[0]=QString("1");
    st[1]=QString("2");
    st[2]=QString("3");
    st[3]=QString("4");
    st[4]=QString("5");
    st[5]=QString("6");
    st[6]=QString("7");

    QString s = QString("%1的成绩单：").arg(ui->name->text());
    QString text = "<table border='1'  cellpadding='10' >"
                  "<caption><td>"+s+"</td></caption>"
                  " <tr>   <td>编号</td>   <td>课程名称</td>   <td>成绩</td>    <td>学分</td>   <td>绩点</td> </tr> ";

    for(int i=0;i<sum;i++)
    {
        text.append(" <tr> <td>"+st[i]+"</td>   <td>"+course[i]+"</td>   <td>"+grade[i]+"</td>    <td>"+credit[i]+"</td>   <td>"+GPA[i]+"</td> </tr> ");
    }

    //清空
    for(int i=0;i<7;i++)
    {
        course[i]=QString("");
        grade[i]=QString("");
        credit[i]=QString("");
        GPA[i]=QString("");

    }
    ui->textEdit->insertHtml(text);
    ui->textEdit->setTextColor(Qt::red);
    ui->textEdit->setCurrentFont(QFont("楷体",15));
    ui->textEdit->insertPlainText(finalGPA);
}

//录入
void ScoreWindow::on_writeButton_clicked()
{
    if(db.isOpen())
        db.close();

    if(!db.open())
    {
        QMessageBox::critical((this),tr("错误"),tr("无法连接到数据库！\n请检查数据库连接配置"));
        return;
    }else {
        QString  stb = QString("select courseName from t_courses");

        QSqlQuery query(db);
        query.exec(stb);//显示子窗口

        while(query.next()){
            allcourselist<<query.value(0).toString();
        }

    }
    btndialog = new Dialog(this);
    btndialog->setquery(db,allcourselist,allNamelist);//setquery 执行给定数据库连接db的query查询
    btndialog->setWindowTitle("信息录入");
    btndialog->show();
    OutputTheInfo("t_courses","t_courses.csv");
    OutputTheInfo("t_stud_course_info","t_stud_course_info.csv");
    OutputTheInfo("t_stud_info","t_stud_info.csv");
}

//将表导出为csv格式表格
void ScoreWindow::OutputTheInfo(const QString &tableName,const QString &csvFileName)
{
    QSqlTableModel *exportModel = new QSqlTableModel();

    exportModel->setTable(tableName);
    exportModel->select();
    QStringList strList;//记录数据库中的一行报警数据
    QString strString;
    const QString FILE_PATH(csvFileName);

    QFile csvFile(FILE_PATH);
    if (csvFile.open(QIODevice::ReadWrite))
    {
        for (int i=0;i<exportModel->rowCount();i++)
        {
            for(int j=0;j<exportModel->columnCount();j++)
            {
                strList.insert(j,exportModel->data(exportModel->index(i,j)).toString());//把每一行的每一列数据读取到strList中
            }
            strString = strList.join(", ")+"\n";//给两个列数据之前加“,”号，一行数据末尾加回车
            strList.clear();//记录一行数据后清空，再记下一行数据
            csvFile.write(strString.toUtf8());//使用方法：转换为Utf8格式后在windows下的excel打开是乱码,可先用Sublime打开并转码为unicode，再次用excel打开即可
            qDebug()<<strString.toUtf8();
        }
        csvFile.close();
    }
}


//导入
void ScoreWindow::on_importButton_clicked()
{
    path = QFileDialog::getOpenFileName(this, tr("Open csv"), ".", tr("csv Files(*.csv)"));
    if(path.length() == 0) {
        return;
        QMessageBox::information(NULL, tr("Path"), tr("You didn't select any files."));
    } else {
        QMessageBox::information(NULL, tr("Path"), tr("You selected ") + path);
    }
    QStringList  tempText;
    QFile file(path);
    qDebug()<<path;

    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::critical(NULL,tr("错误"),tr("打开文件失败！"));
        return;
    }
    QTextStream out(&file);
    while(!out.atEnd()){
        tempText.append(out.readLine().trimmed());//返回文件的一行到列表，不包含换行符
    }

    file.close();
    if(tempText.isEmpty())
        return;
    if(db.isOpen())
    {
        db.close(); //重启db
    }
    if(!db.open())
    {
        qDebug() << "Error: Failed to connect database." << db.lastError();
        return;
    }
    else{
        QString tempdata;
        QStringList singledata;
        QString  select_sql;
        QStringList signalcourseID;
        QSqlQuery sql_query;       //数据库操作类

        tempdata=tempText.at(0);
        singledata=tempdata.split(',');
        qDebug()<<singledata;

        for(int i=0;i<4;i++){
            select_sql = QString("select courseID from t_courses where courseName='%1';").arg(singledata.at(5+i));

            if(!sql_query.exec(select_sql))
            {
                qDebug()<<sql_query.lastError();
            }
            else
            {
                while(sql_query.next())
                {
                    signalcourseID<<sql_query.value(0).toString();

                }

            }
        }
        qDebug()<<signalcourseID;

        for(int i=1;i<tempText.size();i++)
        {
            tempdata=tempText.at(i);
            singledata=tempdata.split(',');
            qDebug()<<singledata;

            for(int j=0;j<4;j++)
            {
                select_sql = QString("insert into t_stud_course_info value ('','%1','%2','%3','','');").arg(singledata.at(1),signalcourseID.at(j),singledata.at(5+j));
                qDebug()<<singledata.at(1)<<signalcourseID.at(j)<<singledata.at(5+j);
                if(!sql_query.exec(select_sql))
                {
                    qDebug()<<sql_query.lastError();
                }
                else
                {
                    qDebug()<<"插入成功";
                }
            }
        }

        for(int i=1;i<tempText.size();i++)
        {
            tempdata=tempText.at(i);
            singledata=tempdata.split(',');

            select_sql = QString("insert into t_stud_info value ('%1','%2','%3','%4','','');").arg(singledata.at(1),singledata.at(2),singledata.at(3),singledata.at(4));
            qDebug()<<singledata.at(1)<<singledata.at(2)<<singledata.at(3)<<singledata.at(4);

            if(!sql_query.exec(select_sql))
            {
                qDebug()<<sql_query.lastError();
            }
            else
            {
                qDebug()<<"插入成功";
            }
        }
        QMessageBox::information(NULL, tr("Info"),tr("导入成功"));
    }
    OutputTheInfo("t_courses","t_courses.csv");
    OutputTheInfo("t_stud_course_info","t_stud_course_info.csv");
    OutputTheInfo("t_stud_info","t_stud_info.csv");
}



//导出
void ScoreWindow::on_exportButton_clicked()
{
   QSqlDatabase xb = QSqlDatabase::addDatabase("QSQLITE","db2");
   xb.setDatabaseName("another.sqlite3");//创建了一个SQLite数据库的默认连接

   if(!xb.open())
   {
       QMessageBox::critical(NULL,tr("错误"),tr("数据库连接失败或超时！"),QMessageBox::Cancel);
       return;
   }


   //第一个表导出
   QFile file1("t_courses.csv");
   if(!file1.open(QIODevice::ReadOnly | QIODevice::Text))
   {
       QMessageBox::critical(NULL,tr("错误"),tr("文件打开失败！"),QMessageBox::Cancel);
       return;
   }

   //在sqlite中创建表格
   QSqlQuery query1(xb);
   query1.exec("create table t_courses (courseID int primary key, courseName varchar(25),"
              "courseCredit int,courseType int)");

   QTextStream in1(&file1);//QTextStream读取文件
   QTextCodec *codec1=QTextCodec::codecForName("utf-8");
   in1.setCodec(codec1);

   QStringList list1;
   list1.clear();

   //循环读取
   while(!in1.atEnd())
   {
      QString fileLine = in1.readLine();  //从第一行读取至下一行
      list1 = fileLine.split(",", QString::SkipEmptyParts);//将每一行内容传至列表保存

      //准备执行SQL查询
      query1.prepare("INSERT INTO t_courses (courseID, courseName,courseCredit,courseType) VALUES (:1,:2,:3,:4)");
      //绑定要插入的值
      query1.bindValue(":1", list1.at(0));
      query1.bindValue(":2", list1.at(1));
      query1.bindValue(":3", list1.at(2));
      query1.bindValue(":4", list1.at(3));

      query1.exec();
   }
   qDebug()<<"成功导出一表";
   query1.clear();


   //第二个表导出
   QFile file2("t_stud_course_info.csv");
   if(!file2.open(QIODevice::ReadOnly | QIODevice::Text))
   {
       QMessageBox::critical(NULL,tr("错误"),tr("文件打开失败！"),QMessageBox::Cancel);
       return;
   }

   QSqlQuery query2(xb);
   query2.exec("create  table t_stud_course_info (ID int primary key, scNumber int,"
              "scCourseID int,scScores int,scYear varchar(25),scTerm int)");

   QTextStream in2(&file2);
   QTextCodec *codec2=QTextCodec::codecForName("utf-8");
   in2.setCodec(codec2);

   QStringList list2;
   list2.clear();

   while(!in2.atEnd())
   {
      QString fileLine = in2.readLine();
      list2 = fileLine.split(",", QString::SkipEmptyParts);

      query2.prepare("INSERT INTO t_stud_course_info (ID, scNumber,scCourseID,scScores,scYear,scTerm) VALUES (:1,:2,:3,:4,:5,:6)"); //准备执行SQL查询
      query2.bindValue(":1", list2.at(0));
      query2.bindValue(":2", list2.at(1));
      query2.bindValue(":3", list2.at(2));
      query2.bindValue(":4", list2.at(3));
      query2.bindValue(":5", list2.at(4));
      query2.bindValue(":6", list2.at(5));

      query2.exec();
   }
   qDebug()<<"成功导出二表";
   query2.clear();


   //第三个表导出
   QFile file3("t_stud_info.csv");
   if(!file3.open(QIODevice::ReadOnly | QIODevice::Text))
   {
       QMessageBox::critical(NULL,tr("错误"),tr("文件打开失败！"),QMessageBox::Cancel);
       return;
   }

   QSqlQuery query3(xb);
   query3.exec("create table t_stud_info (sNumber int primary key, sName varchar(25),"
              "sMajor varchar(25),sClass varchar(25),sEnrolledYear int,sPhone int)");

   QTextStream in3(&file3);  //QTextStream读取数据
   QTextCodec *codec3=QTextCodec::codecForName("utf-8");
   in3.setCodec(codec3);

   QStringList list3;
   list3.clear();

   while(!in3.atEnd())
   {
      QString fileLine = in3.readLine();
      list3 = fileLine.split(",", QString::SkipEmptyParts);

      query3.prepare("INSERT INTO t_stud_info (sNumber, sName,sMajor,sClass,sEnrolledYear,sPhone) VALUES (:1,:2,:3,:4,:5,:6)");
      query3.bindValue(":1", list3.at(0));
      query3.bindValue(":2", list3.at(1));
      query3.bindValue(":3", list3.at(2));
      query3.bindValue(":4", list3.at(3));
      query3.bindValue(":5", list3.at(4));
      query3.bindValue(":6", list3.at(5));

      query3.exec();
   }
   qDebug()<<"成功导出三表";
   query3.clear();

   //关闭连接
   xb.close();
   QMessageBox::information(NULL, tr("Info"),tr("导出成功"));
   return;
}
