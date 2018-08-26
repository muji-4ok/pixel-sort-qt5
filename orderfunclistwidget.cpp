#include "orderfunclistwidget.h"
#include <QDebug>

OrderFuncListWidget::OrderFuncListWidget(QWidget *parent) : QListWidget(parent)
{

}

void OrderFuncListWidget::dropEvent(QDropEvent *event)
{
    QListWidget::dropEvent(event);

    std::vector<QString> encountered{};

    for (int i = count() - 1; i >= 0; --i)
    {
        bool duplicate = false;
        QListWidgetItem *it = item(i);

        for (QString s : encountered)
            if (it->text() == s)
                duplicate = true;

        if (duplicate)
            delete takeItem(i);
        else
            encountered.push_back(it->text());
    }
}

std::vector<QString> OrderFuncListWidget::getOrder()
{
    std::vector<QString> out{};

    for (int i = 0; i < count(); ++i)
        out.push_back(item(i)->text());

    return out;
}

void OrderFuncListWidget::setOrder(const std::vector<QString> &funcs)
{
    for (const QString &s : funcs)
        addItem(s);
}
