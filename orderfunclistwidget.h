#ifndef ORDERFUNCLISTWIDGET_H
#define ORDERFUNCLISTWIDGET_H

#include <QObject>
#include <QWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QDropEvent>
#include <vector>

class OrderFuncListWidget : public QListWidget
{
    Q_OBJECT

public:
    OrderFuncListWidget(QWidget *parent);

    virtual void dropEvent(QDropEvent *) override;
    std::vector<QString> getOrder();
    void setOrder(const std::vector<QString> &funcs);
};

#endif // ORDERFUNCLISTWIDGET_H
