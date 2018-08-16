#ifndef SAVETHREAD_H
#define SAVETHREAD_H

#include <QObject>
#include <QWidget>
#include <QThread>
#include <QImage>

class SaveThread : public QThread
{
    Q_OBJECT

public:
    SaveThread(QWidget *parent, QImage inputImage, QString filename);

private:
    QImage image;
    QString filename;
    virtual void run() override;

signals:
    void resultReady();
};

#endif // SAVETHREAD_H
