#include "openthread.h"

OpenThread::OpenThread(QWidget *parent, QString filename) : QThread(parent), filename(filename)
{

}

void OpenThread::run()
{
    QImage image(filename);
    emit resultReady(image);
}
