#include "avaliablefunclistwidget.h"
#include <QDebug>

AvaliableFuncListWidget::AvaliableFuncListWidget(QWidget *parent) : QListWidget(parent)
{

}

void AvaliableFuncListWidget::dropEvent(QDropEvent *event)
{
    QListWidget *source = static_cast<QListWidget *>(event->source());

    if (source == this)
    {
        event->accept();
    }
    else
    {
        if (source->count() == 1)
            event->ignore();
        else
            event->accept();
    }
}
