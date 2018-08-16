#include "savethread.h"

SaveThread::SaveThread(QWidget *parent, QImage inputImage, QString filename) : QThread(parent), image(inputImage), filename(filename)
{

}

void SaveThread::run()
{
    image.save(filename);

    emit resultReady();
}
