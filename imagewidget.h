#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QObject>
#include <QWidget>
#include <QPixmap>
#include <QResizeEvent>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QPaintEvent>

class ImageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ImageWidget(QWidget *parent = nullptr);
    void scrollImage(int dir);
    void resetScroll();

signals:
    void changeRgbInfoMessage(const QString &text);
    void changeRgbInfo(const QColor &);

public slots:
    void setImage(QImage newImage);
    void setQualityZoom(bool checked);

protected:
    void resizeEvent(QResizeEvent *) override;
    void wheelEvent(QWheelEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void paintEvent(QPaintEvent*) override;

private:
    QPixmap pixmap;
    double minScale;
    double maxScale;
    double scale;
    double center_x;
    double center_y;
    QPoint lastMousePos;
    bool qualityZoom = false;

    void clampCenter();
    void capturePixelUnderMouse(int event_x, int event_y);
};

#endif // IMAGEWIDGET_H
