#include "mainwidget.h"
#include <QApplication>
#include <QDateTime>

int main(int argc, char *argv[])
{
    qSetMessagePattern("%{file}(%{line}): %{message}");             //!< 自定义qDebug的输出pattern
    qRegisterMetaType< QList<QDateTime> >("QList<QDateTime>");      //!< 注册自定义类型，在信号与槽中使用

    QApplication a(argc, argv);
    mainWidget w;
    w.show();

    return a.exec();
}
