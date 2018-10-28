#include "centerframe.h"
#include "drawwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QPainter>
#include <QPixmap>
#include <QGridLayout>
#include <QLineEdit>
#include <QDebug>

CenterFrame::CenterFrame(QWidget *parent) : QFrame(parent)
{
    // 创建用户命令区
    createUserCommandArea();
    // 创建界面
    createUI();
}

DrawWidget *CenterFrame::insideWidget() const
{
    return drawWidget;
}


void CenterFrame::createUserCommandArea()
{

    // 选项Group
    group = new QGroupBox(this);
    group->setTitle(tr("选项"));

    int btnWidth=32;
    int btnHeight=32;
    // 准备绘制按钮图标
    QPixmap p(btnWidth-2, btnHeight-2);
    QPainter painter(&p);
    QPen pen(FOREGROUND_COLOR);
    pen.setWidthF(2);
    pen.setStyle(Qt::DotLine);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(pen);

    // 矩形按钮
    btnRect = new QPushButton(group);
    btnRect->setToolTip("绘制矩形");
    btnRect->setCheckable(true);
    btnRect->setIconSize(p.size());
    connect(btnRect,&QPushButton::clicked,this,&CenterFrame::on_btnRectClicked);

    p.fill(BACKGROUND_COLOR);
    painter.drawRect(3,3,p.size().width()-2*3,p.size().height()-2*3);
    btnRect->setIcon (QIcon(p));

    // 圆形按钮
    btnEllipse = new QPushButton(group);
    btnEllipse->setToolTip("绘制圆形");
    btnEllipse->setCheckable(true);
    btnEllipse->setIconSize(p.size());

    p.fill(BACKGROUND_COLOR);
    painter.drawEllipse(3,3,p.size().width()-2*3,p.size().height()-2*3);
    btnEllipse->setIcon (QIcon(p));
    connect(btnEllipse,&QPushButton::clicked,this,&CenterFrame::on_btnEllipseClicked);

    // 直线按钮
    btnLine = new QPushButton(group);
    btnLine->setToolTip("绘制直线");
    btnLine->setCheckable(true);
    btnLine->setIconSize(p.size());

    p.fill(BACKGROUND_COLOR);
    painter.drawLine(3+3,p.size().height()-2*3,p.size().width()-2*3,3+3);
    btnLine->setIcon (QIcon(p));
    connect(btnLine,&QPushButton::clicked,this,&CenterFrame::on_btnLineClicked);

    // 三角形
    btnTriangle = new QPushButton(group);
    btnTriangle->setToolTip("绘制三角形");
    btnTriangle->setCheckable(true);
    btnTriangle->setIconSize(p.size());

    p.fill(BACKGROUND_COLOR);
    // 三角形的三个顶点
    QPointF pt1(3,p.size().height()-3);
    QPointF pt2(p.size().width()/2,3);
    QPointF pt3(-3+p.size().width(),-3+p.size().height());
    QVector<QPointF> pts;
    pts<<pt1<<pt2<<pt2<<pt3<<pt3<<pt1;

    // 使用drawLines时需要注意，点数必须为偶数，两两成对作为一个边
    // 如果是奇数，最后一个点会被舍弃
    painter.drawLines(pts);
    btnTriangle->setIcon (QIcon(p));
    connect(btnTriangle,&QPushButton::clicked,this,&CenterFrame::on_btnTriangleClicked);

    // 文本按钮
    btnText = new QPushButton(group);
    btnText->setToolTip("绘制文本");
    btnText->setCheckable(true);
    btnText->setIconSize(p.size());

    p.fill(BACKGROUND_COLOR);
    QFont font = painter.font();
    font.setFamily("Modern No. 20");
    font.setPixelSize(26);
    painter.setFont(font);

    painter.drawText(p.rect(),Qt::AlignCenter,"T");
    btnText->setIcon (QIcon(p));
    connect(btnText,&QPushButton::clicked,this,&CenterFrame::on_btnTextClicked);

    //菱形按钮
    btnDiamond = new QPushButton(group);
    btnDiamond->setToolTip("绘制菱形");
    btnDiamond->setCheckable(true);
    btnDiamond->setIconSize(p.size());

    p.fill(BACKGROUND_COLOR);
    //菱形的四个顶点
    QPointF pt4(3,p.size().height()/2);
    QPointF pt5(p.size().width()/2,3);
    QPointF pt6(-3+p.size().width(),p.size().height()/2);
    QPointF pt7(p.size().width()/2,-3+p.size().height());
    QVector<QPointF> ptd;
    ptd<<pt4<<pt5<<pt5<<pt6<<pt6<<pt7<<pt7<<pt4;
    painter.drawLines(ptd);
    btnDiamond->setIcon (QIcon(p));
    connect(btnDiamond,&QPushButton::clicked,this,&CenterFrame::on_btnDiamondClicked);


    // 选项Group布局
    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->addWidget(btnRect,0,0);
    gridLayout->addWidget(btnEllipse,0,1);
    gridLayout->addWidget(btnTriangle,1,0);
    gridLayout->addWidget(btnLine,1,1);
    gridLayout->addWidget(btnText,2,0);
    gridLayout->addWidget(btnDiamond,2,1);
    gridLayout->setMargin(3);
    gridLayout->setSpacing(3);
    group->setLayout(gridLayout);
}


void CenterFrame::createUI()
{

    edtText = new QLineEdit();
    edtText->setToolTip(tr("输入需要绘制的文本"));   // 设置工具提示
    edtText->setVisible(false);                     //  初始时，文本框设为隐藏
    edtText->setClearButtonEnabled(true);           //  在编辑框中显示一个清除按键
    edtText->setMaximumWidth(91);
    // textEdited信号与textChanged信号类似，
    // 区别是前者只在外部编辑时发出，setText等内部函数修改文本时，该信号不会发射
    // textChanged信号只要是文本发生变化都会反射，无论是否是内部函数修改
    connect(edtText,&QLineEdit::textEdited,
            this,&CenterFrame::on_edtTextEdited);   // 连接文本输入时的处理槽函数
    // 右侧垂直布局
    vLayout = new QVBoxLayout();
    vLayout->addWidget(group);
    vLayout->addWidget(edtText);
    vLayout->addStretch();
    vLayout->setMargin(5);


    // 创建绘图区
    drawWidget = new DrawWidget(this);    //新建一个drawWidget对象

    //水平布局
    hLayout = new QHBoxLayout();
    hLayout->addWidget(drawWidget);
    hLayout->addLayout(vLayout);
    hLayout->setStretch(0,1);
    hLayout->setStretch(1,0);
    hLayout->setMargin(0);
    hLayout->setSpacing(0);

    setLayout(hLayout);


    // 使用QSS设置按键的风格
    // 具体风格设置，请参阅Qt文档：Qt Style Sheets Reference
    QString myStyleSheet =
            "QPushButton  { "
                "background-color: none;"
                "border-style: flat;"
                "border-width: 1px;"
                "border-radius: 3px;"
                "border-color: #8f8f91;"
                "min-width: 32px;"
                "min-Height: 32px;"
                "padding: 2px;"
            "}"
            "QPushButton:checked {"
                "background-color: #b7bfcc;"
            "}"
            "QPushButton:hover {"
                "border-style: outset;"
            "}";
    setStyleSheet(myStyleSheet);

}

void CenterFrame::updateButtonStatus()
{
    // 首先将所有按键复位
    btnRect->setChecked(false);
    btnLine->setChecked(false);
    btnTriangle->setChecked(false);
    btnEllipse->setChecked(false);
    btnText->setChecked(false);
    edtText->setVisible(false);

    // 然后根据设置的绘图类型重新切换按键状态
    switch (drawWidget->shapeType()) {
    case ST::Rectangle:
        btnRect->setChecked(true);
        break;
    case ST::Line:
        btnLine->setChecked(true);
        break;
    case ST::Ellipse:
        btnEllipse->setChecked(true);
        break;
    case ST::Triangle:
        btnTriangle->setChecked(true);
        break;
    case ST::Text:
        btnText->setChecked(true);
        edtText->setVisible(true);      // 使编辑框可见
        edtText->setFocus();            // 编辑框获得输入焦点
        break;
    case ST::Diamond:
        btnDiamond->setChecked(true);
        break;
    default:
        break;
    }
}

void CenterFrame::setPenStyle(int penStyle)
{
    drawWidget->setStyle(penStyle);
}

void CenterFrame::setPenWidth(int width)
{
    drawWidget->setWidth(width);
}

void CenterFrame::setPenColor(QColor color)
{
    drawWidget->setColor(color);
}

void CenterFrame::clearPaint()
{
    drawWidget->clear();
}


void CenterFrame::on_btnRectClicked()
{
    if(btnRect->isChecked()){
        updateButtonStatus();
        drawWidget->setShapeType(ST::Rectangle);
    }else{
        drawWidget->setShapeType(ST::None);
    }

}


void CenterFrame::on_btnEllipseClicked()
{
    if(btnEllipse->isChecked()){
        drawWidget->setShapeType(ST::Ellipse);
        updateButtonStatus();
    }else{
        drawWidget->setShapeType(ST::None);
    }

}
void CenterFrame::on_btnLineClicked()
{
    if(btnLine->isChecked()){
        drawWidget->setShapeType(ST::Line);
        updateButtonStatus();
    }else{
        drawWidget->setShapeType(ST::None);
    }
}


void CenterFrame::on_btnTriangleClicked()
{
    if(btnTriangle->isChecked()){
        drawWidget->setShapeType(ST::Triangle);
        updateButtonStatus();
    }else{
        drawWidget->setShapeType(ST::None);
    }
}

void CenterFrame::on_btnDiamondClicked()
{
    if(btnDiamond->isChecked()){
        drawWidget->setShapeType(ST::Diamond);
        updateButtonStatus();
    }else{
        drawWidget->setShapeType(ST::None);
    }
}
void CenterFrame::on_btnTextClicked()
{

    if(btnText->isChecked()){
        drawWidget->setShapeType(ST::Text);
        updateButtonStatus();
    }else{
        drawWidget->setShapeType(ST::None);
    }
}
void CenterFrame::on_edtTextEdited(const QString &text)
{
    drawWidget->setDrawnText(text);
}
