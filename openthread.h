#ifndef OPENTHREAD_H
#define OPENTHREAD_H

#include <QObject>
#include <QWidget>
#include <QThread>

class OpenThread : public QThread
{
    Q_OBJECT

public:
    OpenThread(QWidget *parent, QString filename);

private:
    const QString filename;
    virtual void run() override;

signals:
    void resultReady(QImage openedImage);
};

#endif // OPENTHREAD_H
