#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QPixmap>
#include <QResizeEvent>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QPaintEvent>

class ImageLabel : public QLabel
{
    Q_OBJECT

public:
    ImageLabel(QWidget *parent=0);
    void scrollImage(int dir);
    void resetScroll();

public slots:
    void setImage(QImage newImage);

protected:
    virtual void resizeEvent(QResizeEvent *) override;
    virtual void wheelEvent(QWheelEvent*) override;
    virtual void mousePressEvent(QMouseEvent*) override;
    virtual void mouseMoveEvent(QMouseEvent*) override;
    virtual void mouseReleaseEvent(QMouseEvent*) override;
    virtual void paintEvent(QPaintEvent*) override;

private:
    QPixmap image;
    double minScale;
    double maxScale;
    double scale;
    double center_x;
    double center_y;
    QPoint lastMousePos;
};

#endif // IMAGELABEL_H
