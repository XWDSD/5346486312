#ifndef DRAWWIDGET_H
#define DRAWWIDGET_H
#include <common.h>
#include <QWidget>
#include <QPixmap>
#include <QPoint>
#include <QPainter>
#include <QPalette>


class DrawWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DrawWidget(QWidget *parent = 0);
    ~DrawWidget();

    void setShapeType(ST::ShapeType type);
    ST::ShapeType shapeType();
    void setDrawnText(QString text);


protected:
    void mousePressEvent (QMouseEvent *e);
    void mouseMoveEvent (QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void paintEvent (QPaintEvent *);
    void resizeEvent (QResizeEvent *);

    void drawShape(const QPointF ptStart,const QPointF ptEnd,const ST::ShapeType drawType);
    QRectF textRect(const QPointF ptStart, const QPointF ptEnd, QString displayText, QFontMetrics fm);

signals:
public slots:
    void setStyle(int);
    void setWidth(int);
    void setColor(QColor);
    void clear();

private :
    QPixmap *pix;
    QPoint startpos;
    QPoint endpos;
    bool canDraw;
    int style;
    int weight;
    QColor color;
    ST::ShapeType drawType;
    QString drawnText;
};
#endif // DRAWWIDGET_H
