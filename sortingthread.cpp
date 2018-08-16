#include "sortingthread.h"
#include "sorter.h"
#include <QDebug>


SortingThread::SortingThread(QWidget *parent, const QImage &inputImage, const Options &options) : QThread(parent),
    inputImage(inputImage), options(options)
{

}

void SortingThread::run()
{
    Sorter sorter(inputImage);
    QImage outImage = sorter.sort(options.pathType, options.maxIntervals, options.randomizeIntervals, options.angle, options.toMerge,
                                  options.toReverse, options.toMirror, options.toInterval, options.lowThreshold, options.funcType);

    emit resultReady(outImage);
}
