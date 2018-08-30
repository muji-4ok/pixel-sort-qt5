#include "imagewidget.h"
#include <QDebug>
#include <QCursor>
#include <QPainter>
#include <QColor>
#include <QImage>

ImageWidget::ImageWidget(QWidget *parent) : QWidget(parent)
{

}

void ImageWidget::scrollImage(int dir)
{
    if (pixmap.isNull())
        return;

    double diff = 0.05 * std::pow(1.5, scale);

    if (dir > 0)
        scale += diff;
    else
        scale -= diff;

    scale = std::min(maxScale, std::max(minScale, scale));

    repaint();
}

void ImageWidget::resetScroll()
{
    if (pixmap.isNull())
        return;

    scale = minScale;

    repaint();
}

void ImageWidget::setImage(QImage newImage)
{
    pixmap = QPixmap::fromImage(newImage);

    if (pixmap.isNull())
        return;

    center_x = pixmap.width() / 2;
    center_y = pixmap.height() / 2;
    int maxW = std::min(pixmap.width(), width());
    int maxH = std::min(pixmap.height(), height());
    minScale = std::min(static_cast<double>(maxW) / pixmap.width(), static_cast<double>(maxH) / pixmap.height());
    maxScale = std::log2(std::min(pixmap.width(), pixmap.height()));
    scale = minScale;

    repaint();
}

void ImageWidget::setQualityZoom(bool checked)
{
    qualityZoom = checked;

    repaint();
}

void ImageWidget::resizeEvent(QResizeEvent *)
{
    if (pixmap.isNull())
        return;

    int maxW = std::min(pixmap.width(), width());
    int maxH = std::min(pixmap.height(), height());
    minScale = std::min(static_cast<double>(maxW) / pixmap.width(), static_cast<double>(maxH) / pixmap.height());
    maxScale = std::log2(std::min(pixmap.width(), pixmap.height()));
    scale = std::min(maxScale, std::max(minScale, scale));

    repaint();
}

void ImageWidget::wheelEvent(QWheelEvent *event)
{
    QPoint degrees = event->angleDelta();
    scrollImage(degrees.y());

    QPoint localMousePos = mapFromGlobal(QCursor::pos());
    capturePixelUnderMouse(localMousePos.x(), localMousePos.y());
}

void ImageWidget::mousePressEvent(QMouseEvent *event)
{
    if (pixmap.isNull())
        return;

    lastMousePos = event->pos();
}

void ImageWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (pixmap.isNull())
        return;

    if (event->buttons() && Qt::LeftButton)
    {
        QPoint diff = lastMousePos - event->pos();
        lastMousePos = event->pos();
        center_x += diff.x() / scale * 0.9 * std::pow(1.001,
                                                      static_cast<double>(pixmap.width()) / width());
        center_y += diff.y() / scale * 0.9 * std::pow(1.001,
                                                      static_cast<double>(pixmap.height()) / height());
        repaint();
    }

    capturePixelUnderMouse(event->x(), event->y());
}

void ImageWidget::mouseReleaseEvent(QMouseEvent *)
{
//    if (image.isNull())
//        return;
}

void ImageWidget::paintEvent(QPaintEvent *)
{
    if (pixmap.isNull())
        return;

    clampCenter();

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
        QPixmap scaledImage = pixmap.copy(frame).scaled(width(), height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        p.drawPixmap(drawOffX, drawOffY, scaledImage);
    }
    else
    {
        p.setRenderHint(QPainter::SmoothPixmapTransform, false);
        p.drawPixmap(QRect(drawOffX, drawOffY, width(), height()), pixmap, frame);
    }
}

void ImageWidget::clampCenter()
{
    center_x = std::min(pixmap.width() - width() / 2 / scale,
                        std::max(width() / 2 / scale, center_x));
    center_y = std::min(pixmap.height() - height() / 2 / scale,
                        std::max(height() / 2 / scale, center_y));
}

void ImageWidget::capturePixelUnderMouse(int event_x, int event_y)
{
    clampCenter();

    if (scale >= 1)
    {
        int left = center_x - width() / 2 / scale;
        int top = center_y - height() / 2 / scale;

        if (left < 0)
            left /= 2;

        if (top < 0)
            top /= 2;

        int x = event_x / scale + left;
        int y = event_y / scale + top;

        if (0 <= x && x < pixmap.width() && 0 <= y && y < pixmap.height())
        {
            QImage im = pixmap.toImage();
            QColor c = im.pixelColor(x, y);

            emit changeRgbInfo(c);
        }
        else
        {
            emit changeRgbInfoMessage("Out of range");
        }
    }
    else
    {
        emit changeRgbInfoMessage("Too zoomed out");
    }
}
