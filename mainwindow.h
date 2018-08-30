#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include <QImage>
#include <QColor>
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

public slots:
    void handleSortResults(QImage resultImage);
    void handleOpenResults(QImage openedImage);
    void handleSaveResults();

    void changePixelInfo(const QColor &);

private slots:
    void on_actionOpen_triggered();

    void on_sortButton_clicked();

    void on_optionsButton_clicked();

    void on_actionReset_triggered();

    void on_actionSave_as_triggered();

    void on_actionSave_triggered();

    void on_actionCopy_triggered();

    void on_actionPaste_triggered();

    void on_actionZoom_in_triggered();

    void on_actionZoom_out_triggered();

    void on_actionReset_zoom_triggered();

    void on_actionSort_after_changing_options_2_toggled(bool arg1);

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
    bool sortAfterChange = true;
    int lastOptionsTab = 0;

    void enableInterface();
    void disableInterface();

    void saveImage(QString filename);
};

#endif // MAINWINDOW_H
