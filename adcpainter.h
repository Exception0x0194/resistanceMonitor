#ifndef ADCPAINTER_H
#define ADCPAINTER_H

#include <QWidget>
#include <QPainter>

class adcPainter : public QWidget
{
    Q_OBJECT
public:
    explicit adcPainter(QWidget *parent = 0);
    void addResistance(int r, bool refresh);
    void clearResistance();

private:
    QPainter *painter;
    int resistancePosY[11];
    void drawResistanceDot(int index);

protected:
    void paintEvent(QPaintEvent *event);

signals:
};

#endif // ADCPAINTER_H
