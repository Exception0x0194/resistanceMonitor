#include "adcpainter.h"

adcPainter::adcPainter(QWidget *parent) : QWidget(parent)
{
    this->resize(250, 130);
    for (int i = 0; i < 11; i++)
    {
        resistancePosY[i] = -1;
    }
}

void adcPainter::addResistance(int r, bool refresh)
{
    for (int i = 10; i > 0; i--)
    {
        resistancePosY[i] = resistancePosY[i - 1];
    }
    resistancePosY[0] = r;
    if (refresh)
        this->repaint();
}

void adcPainter::clearResistance()
{
    for (int i = 0; i < 11; i++)
    {
        resistancePosY[i] = -1;
    }
    this->repaint();
}

void adcPainter::paintEvent(QPaintEvent *event)
{
    painter = new QPainter();
    painter->begin(this);
    if (!painter->isActive())
    {
        printf("Painter is not active.\n");
        return;
    }

    //Backgrouond color
    painter->setPen(Qt::black);
    painter->setBrush(QBrush(Qt::white, Qt::SolidPattern));
    painter->drawRect(0, 0, 250, 130);
    painter->setBrush(Qt::transparent);
    painter->drawRect(10, 10, 230, 110);

    //Grid and label
    painter->setPen(Qt::gray);
    painter->setFont(QFont("Ubuntu", 9));
    for (int i = 0; i <= 10; i++)
    {
        char buffer[10];
        sprintf(buffer, "%d", 7500 - 200 * i);
        QVector<QPoint> pts(4);
        pts[0] = QPoint(10 + 23 * i, 10);
        pts[1] = QPoint(10 + 23 * i, 120);
        pts[2] = QPoint(10, 10 + 11 * i);
        pts[3] = QPoint(240, 10 + 11 * i);
        if (i < 3 || i > 7)
        {
            painter->setPen(Qt::red);
            painter->drawLine(pts[2], pts[3]);
            painter->drawText(0, 5 + 11 * i, 50, 10, Qt::AlignLeft | Qt::AlignVCenter, buffer);
            painter->setPen(Qt::gray);
            painter->drawLine(pts[0], pts[1]);
        }
        else
        {
            painter->setPen(Qt::gray);
            painter->drawLines(pts);
            painter->drawText(0, 5 + 11 * i, 50, 10, Qt::AlignLeft | Qt::AlignVCenter, buffer);
        }
    }

    for (int index = 0; index < 11; index++)
    {
        if (resistancePosY[index] == -1)
            break;

        int posy = -(resistancePosY[index] - 5500) * 11 / 200 + 120;
        QPoint p1(240 - 23 * index, posy);
        if (resistancePosY[index] > 7000 || resistancePosY[index] < 6000)
        {
            painter->setPen(Qt::yellow);
            painter->setBrush(QBrush(Qt::red, Qt::SolidPattern));
        }
        else
        {
            painter->setPen(Qt::yellow);
            painter->setBrush(QBrush(Qt::darkGray, Qt::SolidPattern));
        }
        painter->drawEllipse(p1, 5, 5);

        if (index == 0)
            continue;

        int posy2 = -(resistancePosY[index - 1] - 5500) * 11 / 200 + 120;
        QPoint p2(240 - 23 * (index - 1), posy2);
        painter->setPen(Qt::black);
        painter->drawLine(p1, p2);
    }

    painter->end();
    delete painter;
}

void adcPainter::drawResistanceDot(int index)
{
}
