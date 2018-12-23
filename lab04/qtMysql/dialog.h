#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QtSql>
#include <QSpinBox>


namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
    void setquery(QSqlDatabase getdb, QStringList getcourselist,QStringList getnamelist);
    void InsertStudentInfo(QString sql);

private slots:



    void on_okButton_clicked();

    void on_cancelButton_clicked();

    void on_chooseButton_clicked();



private:
    QSqlDatabase test;
    QStringList allcourselist2;
    QStringList allnamelist;
    QStringList addgradepoint;
    QStringList addcourse;
    QStringList addcoursenum;
    QString  Number;
    QString Name;
    QString Major;
    QString Class;
    QString EnrolledYear;
    QString Phone;
    void achievedata();
    void idtextchanged();
    void nametextchanged();
    void gradeedited();
    void gradetextchanged();
    QLineEdit *lineedit;
    QSpinBox *spinBox;
    QComboBox *combo1;
    QComboBox *combo;
    int count=0;

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
