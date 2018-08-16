#include "imagelabel.h"
#include <QDebug>

ImageLabel::ImageLabel(QWidget *parent) : QLabel(parent)
{
    updateTimer = new QTimer(this);
    updateTimer->setSingleShot(true);
    connect(updateTimer, &QTimer::timeout, this, &ImageLabel::updateImage);
}

void ImageLabel::updateImage()
{
    if (image.isNull())
    {
        return;
    }

    if (image.width() > width() || image.height() > height())
        setPixmap(image.scaled(width(), height(), Qt::KeepAspectRatio, Qt::TransformationMode::SmoothTransformation));
    else
        setPixmap(image);
}

void ImageLabel::setImage(QImage newImage)
{
    image = QPixmap::fromImage(newImage);
}

void ImageLabel::resizeEvent(QResizeEvent * /*event*/)
{
    updateTimer->start(75);
}
