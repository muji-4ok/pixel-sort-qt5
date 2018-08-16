#ifndef SORTINGTHREAD_H
#define SORTINGTHREAD_H

#include <QObject>
#include <QWidget>
#include <QImage>
#include <QPixmap>
#include <QThread>
#include "optionsdialog.h"

class SortingThread : public QThread
{
    Q_OBJECT

public:
    SortingThread(QWidget *parent, const QImage &inputImage, const Options &options);

private:
    virtual void run() override;
    const QImage inputImage;
    const Options options;

signals:
    void resultReady(QImage outImage);
};

#endif // SORTINGTHREAD_H
