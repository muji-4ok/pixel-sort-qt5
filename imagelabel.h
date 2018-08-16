#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QPixmap>
#include <QTimer>

class ImageLabel : public QLabel
{
    Q_OBJECT

public:
    ImageLabel(QWidget *parent=0);

signals:
    void imageNeedsUpdate();

public slots:
    void updateImage();
    void setImage(QImage newImage);

protected:
    virtual void resizeEvent(QResizeEvent * /*event*/) override;

private:
    QPixmap image;
    QTimer *updateTimer;
};

#endif // IMAGELABEL_H
