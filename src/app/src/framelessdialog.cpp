#include <QWindow>
#include <QDialog>

#include <QWidget>

#include <QMouseEvent>

#include "framelessdialog.h"

FramelessDialog::FramelessDialog(QWidget *widget) : QDialog()
{
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
}

void FramelessDialog::mousePressEvent(QMouseEvent *event) 
{
    if (event->button() == Qt::LeftButton) {
        Qt::Edges edges = getMouseEdges(event->pos());
        
        if (edges != Qt::Edges()) {
            windowHandle()->startSystemResize(edges);
            event->accept();
        }
        else if (auto window = windowHandle()) {
            window->startSystemMove();
            event->accept();
        }
    }
}

void FramelessDialog::mouseMoveEvent(QMouseEvent *event)
{
    Qt::Edges edges = getMouseEdges(event->pos());

    if ((edges & Qt::LeftEdge && edges & Qt::TopEdge) || (edges & Qt::RightEdge && edges & Qt::BottomEdge))
        setCursor(Qt::SizeFDiagCursor);
    else if ((edges & Qt::RightEdge && edges & Qt::TopEdge) || (edges & Qt::LeftEdge && edges & Qt::BottomEdge))
        setCursor(Qt::SizeBDiagCursor);
    else if (edges & Qt::LeftEdge || edges & Qt::RightEdge)
        setCursor(Qt::SizeHorCursor);
    else if (edges & Qt::TopEdge || edges & Qt::BottomEdge)
        setCursor(Qt::SizeVerCursor);
    else
        setCursor(Qt::ArrowCursor);
}

Qt::Edges FramelessDialog::getMouseEdges(const QPoint &pos)
{
    Qt::Edges edges = Qt::Edges();
    int x = pos.x();
    int y = pos.y();
    int w = width();
    int h = height();

    if (x < borderWidth) edges |= Qt::LeftEdge;
    if (x > w - borderWidth) edges |= Qt::RightEdge;
    if (y < borderWidth) edges |= Qt::TopEdge;
    if (y > h - borderWidth) edges |= Qt::BottomEdge;

    return edges;
}