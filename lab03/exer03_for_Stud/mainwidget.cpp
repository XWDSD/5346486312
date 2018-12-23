#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <dataworker.h>
#include <QDateTime>



/**
 * @brief 主窗口构造函数
 * @param parent 父窗口指针
 *
 * 负责初始化图表、下拉框，创建数据解析工作类dataWorker并连接相应的信号与槽。
 */
mainWidget::mainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mainWidget)
{
    ui->setupUi(this);
    ui->chartview->setRenderHint(QPainter::Antialiasing);

    initComboMonth();

    resetChart("南京气温");
    addLineSeries(ui->chartview->chart(),"",Qt::red);

    worker = new dataWorker(this);
    connect(worker,&dataWorker::dataParseFinished,this,&mainWidget::updateDataChart);
    connect(worker,&dataWorker::dataParseError,this,&mainWidget::on_dataError);
    connect(worker,&dataWorker::httpRequestError,this,&mainWidget::on_dataError);

}

mainWidget::~mainWidget()
{
    delete ui;
}

/**
 * @brief 初始化下拉框
 *
 */
void mainWidget::initComboMonth()
{
    QStringList month;
    for(int i=10;i>0;i--){
		// 此处为固定时间和日期
		// 请使用QDate/QDateTime将其修正，
		// 用户运行前一个月开始连续10个月的"年-月"
		// (如2018-02、2018-01、2017-12...，假设当前日期为2018年3月12日)
        month<<QString("2016-%1").arg(i,2,10,QChar('0'));
    }
    ui->comboMonth->clear();
    ui->comboMonth->addItems(month);

}

/**
 * @brief 重置chart图表对象
 *
 * 重置chart对象，包括以下几个内容：<br/>
 * 1) 对chart中的各个序列，删除序列数据，并清空序列列表<br/>
 * 2) 因为后面使用时，chart的图注(Legend)使用了非贴附模式(not attatched)，<br/>
 *    因此这里设置为贴附模式(attatchToChart)，并将其隐藏，<br/>
 * 3) 清空控制器对象列表，对QList<pointer*>对象，清空时需要首先删除个对象，<br/>
 *    然后清除列表。如下代码所示：
 *  @code{.cpp}
 *      if(controllerList.count()>0){
 *          qDeleteAll(controllerList);
 *          controllerList.clear();
 *       }
 *  @endcode
 * 4) 最后设置计算线程的计数值为0，同时复位进度条
 *
 */
void mainWidget::resetChart(const QString &title)
{
    QChart* chart = ui->chartview->chart();


    //chart->setTitle(QString("单向开关函数的傅里叶变换%1").arg("\u03c0"));
    chart->setTitle(title);
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // 在后面的显示中，Legend被设为浮动模式，
    // 因此，这里在清除数据时，需要先attach一下
    if(!chart->legend()->isAttachedToChart())
        chart->legend()->attachToChart();

    chart->legend()->hide();
    chart->legend()->setAlignment(Qt::AlignBottom);
    // removeAllSeries函数即将包含的个序列delete掉，
    // 同时也清空序列列表
    chart->removeAllSeries();
}


/**
 * @brief 向Chart中添加序列（Series）
 * @param chart 指向QChart对象指针
 * @param seriesName 序列名称
 * @param color 序列颜色
 * @param lineWidth 序列线宽（默认值为1）
 */
void mainWidget::addLineSeries(QChart *chart, const QString &seriesName, const QColor color, const int lineWidth)
{

    // 新建一个序列
    QLineSeries* series = new QLineSeries;
    // 将序列添加入chart中
    chart->addSeries(series);
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // 设置序列的线宽和颜色
    QPen pen(color);
    pen.setWidth(lineWidth);
    series->setPen(pen);

    series->setPointsVisible(true);

    // 序列名称，显示在legend中
    series->setName(seriesName);

    if(chart->series().count() == 1){
        // 第一次添加序列，新增一个Axes，
        chart->createDefaultAxes();

        // 设置2个坐标轴
        QDateTimeAxis *mAxisX = new QDateTimeAxis;
        mAxisX->setFormat("MM-dd");
        mAxisX->setTitleText("日期");
        mAxisX->setTickCount(10+1);
        mAxisX->setRange(QDateTime::currentDateTime().addMonths(-1),QDateTime::currentDateTime());

        QValueAxis *mAxisY = new QValueAxis;
        mAxisY->setRange(-5,40);
        mAxisY->setLabelFormat("%g");
        mAxisY->setTitleText("摄氏度(°C)");

        chart->setAxisX(mAxisX,series);
        chart->setAxisY(mAxisY,series);
    }else{
        // 将新增加的列与现有坐标轴连接上
        QDateTimeAxis *mAxisX = qobject_cast<QDateTimeAxis*>(chart->axisX());
        QValueAxis *mAxisY = qobject_cast<QValueAxis*>(chart->axisY());
        chart->setAxisX(mAxisX,series);
        chart->setAxisY(mAxisY,series);
    }
}


/**
 * @brief 连接序列中所有Legend（图注）中Marker的信号与槽
 */
void mainWidget::connectMarkers()
{
    QChart* chart = ui->chartview->chart();
    // 将所有的marker的信号与槽进行连接
    for (QLegendMarker* marker: chart->legend()->markers()) {
        // 连接前先断开原先的信号槽连接，避免多次连接信号槽
        disconnect(marker, &QLegendMarker::clicked, this, &mainWidget::handleMarkerClicked);
        connect(marker, &QLegendMarker::clicked, this, &mainWidget::handleMarkerClicked);
    }
}

/**
 * @brief 处理用户点击界面中图注的Marker
 *
 * 本函数实现用户点击时，隐藏或显示序列。<br/>
 * 在序列隐藏时，图注里的Marker也跟着隐藏，因此需要将其显示出来；<br/>
 * 为了区分正常显示与隐藏图注，将被隐藏图注设置为半透明。<br/>
 * 设置隐藏序列的图注包括三部分工作：<br/>
 * 1) 设置Marker的文本颜色(使用labelBrush)<br/>
 * 2) 设置Marker前图标的填充颜色(使用brush)<br/>
 * 3) 设置Marker前图标的边框颜色(使用pen)<br/>
 *
 */
void mainWidget::handleMarkerClicked()
{
    QLegendMarker* marker = qobject_cast<QLegendMarker*> (sender());
    Q_ASSERT(marker);

    switch (marker->type()){
    case QLegendMarker::LegendMarkerTypeXY:{
        // 切换选中序列的可见性，
        marker->series()->setVisible(!marker->series()->isVisible());

        // 使图注(Legend)的marker也显示出来，因为当隐藏序列series时，
        // marker也跟着隐藏，从而使用户无法看到marker
        marker->setVisible(true);

        // 隐藏序列后虽然显示marker，这里使marker变暗，表明该序列已经被隐藏
        qreal alpha = 1.0;

        if (!marker->series()->isVisible()) {
            alpha = 0.3;
        }

        QColor color;
        QBrush brush = marker->labelBrush();
        color = brush.color();
        color.setAlphaF(alpha);
        brush.setColor(color);
        marker->setLabelBrush(brush);

        brush = marker->brush();
        color = brush.color();
        color.setAlphaF(alpha);
        brush.setColor(color);
        marker->setBrush(brush);

        QPen pen = marker->pen();
        color = pen.color();
        color.setAlphaF(alpha);
        pen.setColor(color);
        marker->setPen(pen);

        break;
    }
    default:{
        qDebug() << "未知的marker类型";
        break;
    }

    }
}


/**
 * @brief “开始”按键响应槽函数
 *
 * 该函数首先清理原图表，设置图表标题，然后调用后台数据工作类dataWorker进行数据的获取和解析工作。<br/>
 * dataWorker对象在设置请求年月参数后，发起HTTP的GET请求，在请求完成后以发射finished信号。<br/>
 * 在dataWorker对象的finished信号响应槽函数中，对网络数据进行解析，解析完成发射dataParseFinished信号，<br/>
 * 提醒界面进行图表更新工作。
 *
 */
void mainWidget::on_btnStart_clicked()
{
    // 禁用两个按键
    ui->comboMonth->setEnabled(false);
    ui->btnStart->setEnabled(false);

    // 设置chart的标题
    QString chartTitle = "";
    if(ui->comboMonth->count()>0){
        chartTitle = ui->comboMonth->currentText().replace("-","年");
        chartTitle.append("月 南京气温");
    }else{
        chartTitle="南京气温";
    }
    resetChart(chartTitle);

    // 设置dataWorker对象的请求年月
    worker->setRequestDate(ui->comboMonth->currentText());

    // 发起HTTP请求
    worker->doRequest();

}


/**
 * @brief dataWorker对象的dataParseFinished信号响应槽函数
 * @param date 数据点时间
 * @param tempHigh 数据点的最高温度
 * @param tempLow  数据点的最低温度
 *
 * 该函数执行具体的数据更新工作，将dataWorker对象解析的数据插入序列中，并更新图表。
 * 更新完成后，使能两个按键，使用户可以发起下一次请求。
 */
void mainWidget::updateDataChart(QList<QDateTime> date, QList<qreal> tempHigh, QList<qreal> tempLow)
{
    QChart* chart = ui->chartview->chart();

    // 添加第一条数据曲线
    addLineSeries(chart,"日最高温度",Qt::red,2);
    QLineSeries* seriesHigh = qobject_cast<QLineSeries*> (chart->series().last());
    seriesHigh->setPointsVisible(ui->cbShowPoint->isChecked());

    // 添加第二条数据曲线
    addLineSeries(chart,"日最低温度",Qt::blue,2);
    QLineSeries* seriesLow = qobject_cast<QLineSeries*> (chart->series().last());
    seriesLow->setPointsVisible(ui->cbShowPoint->isChecked());

    // 向每条曲线中添加数据
    for (int i=0; i<date.count();i++){
        seriesHigh->append(date.at(i).toMSecsSinceEpoch(),tempHigh.at(i));
        seriesLow->append(date.at(i).toMSecsSinceEpoch(),tempLow.at(i));
    }

    // 设置坐标轴
    QDateTimeAxis *mAxisX = qobject_cast<QDateTimeAxis*>(chart->axisX());
    mAxisX->setRange(date.first(),date.last());

    connectMarkers();
    // 显示图注
    chart->legend()->show();
    // 更新图表
    chart->update();

    // 使能两个按钮
    ui->comboMonth->setEnabled(true);
    ui->btnStart->setEnabled(true);
}

/**
 * @brief 显示数据点功能
 *
 * 显示数据点复选框的响应槽函数，当复选框选中时，通过设置series的PointsVisible属性实现。
 *
 */
void mainWidget::on_cbShowPoint_clicked()
{
    QChart* chart = ui->chartview->chart();
    for(QAbstractSeries* series: chart->series() ){
        qobject_cast<QLineSeries*>(series)->setPointsVisible(ui->cbShowPoint->isChecked());
    }
}


/**
 * @brief 图注(Legend)对齐按键响应槽函数
 *
 * 该函数对当前图注的对齐方式进行修改的一个较好实现方法。<br/>
 * 首先获取信号发送者（sender()），并加入一个判断。如果该发送者是按键，那么修改按键标题，<br/>
 * 如果是程序内调用，即用代码实现位置的切换，那么此时就不能修改标题<br/>
 *
 */
void mainWidget::on_btnLegendAlign_clicked()
{
     QPushButton *button = qobject_cast<QPushButton *>(sender());

     QChart* chart = ui->chartview->chart();
     Qt::Alignment align = chart->legend()->alignment();
     switch (align) {
     case Qt::AlignLeft:{
         align = Qt::AlignTop;
         if(button)
             button->setText("图注(顶)");
     }
         break;
     case Qt::AlignTop:{
         align = Qt::AlignRight;
         if(button)
             button->setText("图注(右)");
     }
         break;
     case Qt::AlignRight:{
         align = Qt::AlignBottom;
         if(button)
             button->setText("图注(底)");
     }
         break;
     case Qt::AlignBottom:{
         align = Qt::AlignLeft;
         if(button)
             button->setText("图注(左)");
     }
         break;
     default:
         break;
     }
     // 设置图注对齐方式
     chart->legend()->setAlignment(align);

}


/**
 * @brief 设置图注是否使用粗体显示
 */
void mainWidget::on_cbLegendBold_clicked()
{
    QChart* chart = ui->chartview->chart();
    QFont font = chart->legend()->font();
    font.setBold(!font.bold());
    chart->legend()->setFont(font);
}

/**
 * @brief 设置图注是否使用斜体显示
 */
void mainWidget::on_cbLegendItalic_clicked()
{
    QChart* chart = ui->chartview->chart();
    QFont font = chart->legend()->font();
    font.setItalic(!font.italic());
    chart->legend()->setFont(font);
}

/**
 * @brief mainWidget::on_dataError
 *
 * on_dataError槽接收dataworker的信号，
 * 当dataworker处理数据出错时，发送两个信号dataParseError和httpRequestError，
 * 该槽函数响应上述两个信号，使能界面上的各个按键，以保证程序正常运行
 *
 * @param error 具体的错误信息
 *
 */
void mainWidget::on_dataError(QString error)
{
    qDebug()<<error;
    // 使能两个按钮
    ui->comboMonth->setEnabled(true);
    ui->btnStart->setEnabled(true);
}
