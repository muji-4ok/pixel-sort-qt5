#include "imagelabel.h"
#include <QDebug>
#include <QCursor>
#include <QPainter>
#include <QColor>

ImageLabel::ImageLabel(QWidget *parent) : QLabel(parent)
{
    setCursor(Qt::OpenHandCursor);
}

void ImageLabel::scrollImage(int dir)
{
    if (image.isNull())
        return;

    double diff = 0.05 * std::pow(1.5, scale);

    if (dir > 0)
        scale += diff;
    else
        scale -= diff;

    scale = std::min(maxScale, std::max(minScale, scale));

    repaint();
}

void ImageLabel::resetScroll()
{
    if (image.isNull())
        return;

    scale = minScale;

    repaint();
}

void ImageLabel::setImage(QImage newImage)
{
    image = QPixmap::fromImage(newImage);

    if (image.isNull())
        return;

    center_x = image.width() / 2;
    center_y = image.height() / 2;
    int maxW = std::min(image.width(), width());
    int maxH = std::min(image.height(), height());
    minScale = std::min(static_cast<double>(maxW) / image.width(), static_cast<double>(maxH) / image.height());
    maxScale = std::log2(std::min(image.width(), image.height()));
    scale = minScale;

    repaint();
}

void ImageLabel::setQualityZoom(bool checked)
{
    qualityZoom = checked;

    repaint();
}

void ImageLabel::resizeEvent(QResizeEvent *)
{
    if (image.isNull())
        return;

    int maxW = std::min(image.width(), width());
    int maxH = std::min(image.height(), height());
    minScale = std::min(static_cast<double>(maxW) / image.width(), static_cast<double>(maxH) / image.height());
    maxScale = std::log2(std::min(image.width(), image.height()));
    scale = std::min(maxScale, std::max(minScale, scale));

    repaint();
}

void ImageLabel::wheelEvent(QWheelEvent *event)
{
    QPoint degrees = event->angleDelta();
    scrollImage(degrees.y());
}

void ImageLabel::mousePressEvent(QMouseEvent *event)
{
    if (image.isNull())
        return;

    setCursor(Qt::ClosedHandCursor);

    lastMousePos = event->pos();
}

void ImageLabel::mouseMoveEvent(QMouseEvent *event)
{
    if (image.isNull())
        return;

    QPoint diff = lastMousePos - event->pos();
    lastMousePos = event->pos();
    center_x += diff.x() / scale * 0.9 * std::pow(1.001,
                                                  static_cast<double>(image.width()) / width());
    center_y += diff.y() / scale * 0.9 * std::pow(1.001,
                                                  static_cast<double>(image.height()) / height());

    repaint();
}

void ImageLabel::mouseReleaseEvent(QMouseEvent *)
{
    if (image.isNull())
        return;

    setCursor(Qt::OpenHandCursor);
}

void ImageLabel::paintEvent(QPaintEvent *)
{
    if (image.isNull())
        return;

    center_x = std::min(image.width() - width() / 2 / scale,
                        std::max(width() / 2 / scale, center_x));
    center_y = std::min(image.height() - height() / 2 / scale,
                        std::max(height() / 2 / scale, center_y));

    int left = center_x - width() / 2 / scale;
    int top = center_y - height() / 2 / scale;
    int w = width() / scale;
    int h = height() / scale;

    int drawOffX = 0;
    int drawOffY = 0;

    if (left < 0)
    {
        drawOffX = (-left / 2) * scale;
        left = 0;
    }

    if (top < 0)
    {
        drawOffY = (-top / 2) * scale;
        top = 0;
    }

    QRect frame(left, top, w, h);
    QPainter p(this);

    if (scale < 1 && qualityZoom)
    {
        p.setRenderHint(QPainter::SmoothPixmapTransform, true);
        QPixmap scaledImage = image.copy(frame).scaled(width(), height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        p.drawPixmap(drawOffX, drawOffY, scaledImage);
    }
    else
    {
        p.setRenderHint(QPainter::SmoothPixmapTransform, false);
        p.drawPixmap(QRect(drawOffX, drawOffY, width(), height()), image, frame);
    }
}
