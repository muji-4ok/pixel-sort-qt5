#ifndef FUNCLISTWIDGET_H
#define FUNCLISTWIDGET_H

#include <QObject>
#include <QWidget>
#include <QListWidget>
#include <QDropEvent>

class AvaliableFuncListWidget : public QListWidget
{
    Q_OBJECT

public:
    AvaliableFuncListWidget(QWidget *parent);

    virtual void dropEvent(QDropEvent *) override;
};

#endif // FUNCLISTWIDGET_H
