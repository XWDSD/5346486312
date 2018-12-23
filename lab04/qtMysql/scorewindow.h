#ifndef SCOREWINDOW_H
#define SCOREWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include<QMessageBox>
#include <QString>
#include <dialog.h>

namespace Ui {
class ScoreWindow;
}

class ScoreWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ScoreWindow(QWidget *parent = 0);
    ~ScoreWindow();

    QSqlDatabase db;
    QSqlDatabase xb;

private slots:
    void on_writeButton_clicked();

    void on_exportButton_clicked();

private:
    Dialog *btndialog;
    QStringList queryResult;
    QStringList queryResult2;
    QString course[10];
    QString grade[10];
    QString credit[10];
    QString GPA[10];
    QString addCredit[10];
    QStringList allcourselist;
    QStringList allNamelist;
    QString finalGPA;
    QString queryStr3;
    QString queryStr4;
    QString queryStr5;
    QString queryStr6;
    QString queryStr7;
    QString path;

    int sum;
private:
    void queryStudentInfo(QString queryStr);
    void queryStudentScore(QString);
    void calculatePoints(QStringList str3);
    void updateInfoWindow(QStringList);

public slots:
    void on_importButton_clicked();

    void on_btnQuery_clicked();

    void OutputTheInfo(const QString &tableName,const QString &csvFileName);


private:
    Ui::ScoreWindow *ui;
};

#endif // SCOREWINDOW_H
