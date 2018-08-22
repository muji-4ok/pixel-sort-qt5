#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include <QImage>
#include <QSettings>
#include <QCloseEvent>
#include "sortingthread.h"
#include "openthread.h"
#include "savethread.h"
#include "optionsdialog.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void imageNeedsUpdate();
//    void setImageSignal(QImage newImage);

public slots:
    void handleSortResults(QImage resultImage);
    void handleOpenResults(QImage openedImage);
    void handleSaveResults();

private slots:
    void on_actionOpen_triggered();

    void on_sortButton_clicked();

    void on_optionsButton_clicked();

    void on_actionReset_triggered();

    void on_actionSave_as_triggered();

    void on_actionSave_triggered();

    void on_actionCopy_triggered();

    void on_actionPaste_triggered();

protected:
    virtual void closeEvent(QCloseEvent *) override;

private:
    Ui::MainWindow *ui;
    QImage sourceImage;
    QImage displayImage;
    Options options;
    QString lastSavedFilename;
    QString lastOpenedFilename;
    bool canClose = true;

    void enableInterface();
    void disableInterface();

    void saveImage(QString filename);
};

#endif // MAINWINDOW_H
