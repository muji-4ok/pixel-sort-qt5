#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <QThread>
#include <QClipboard>
#include <QMimeData>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSettings settings("EgorCO", "Pixel Sorter");

    if (!settings.contains("lastDirectory"))
        settings.setValue("lastDirectory", ".");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    QSettings settings("EgorCO", "Pixel Sorter");

    QString filename = QFileDialog::getOpenFileName(this, "Open image", settings.value("lastDirectory").toString(),
                                                    "Image Files (*.png *.jpg *.jpeg *.bmp *.gif *.pbm *.pgm *.ppm *.xbm *.xpm)");

    if (filename.isNull())
        return;

    QFileInfo fileInfo(filename);
    settings.setValue("lastDirectory", fileInfo.dir().absolutePath());

    lastSavedFilename = QString();
    lastOpenedFilename = filename;

    canClose = false;
    disableInterface();

    OpenThread *thread = new OpenThread(this, filename);
    connect(thread, &OpenThread::resultReady, this, &MainWindow::handleOpenResults);
    connect(thread, &OpenThread::finished, thread, &QObject::deleteLater);
    thread->start();
}

void MainWindow::on_sortButton_clicked()
{
    if (sourceImage.isNull())
    {
        ui->statusBar->showMessage("No image to sort");
        return;
    }

    disableInterface();

    SortingThread *thread = new SortingThread(this, sourceImage, options);
    connect(thread, &SortingThread::resultReady, this, &MainWindow::handleSortResults);
    connect(thread, &SortingThread::finished, thread, &QObject::deleteLater);
    thread->start();
}

void MainWindow::handleSortResults(QImage result)
{
    enableInterface();
    ui->statusBar->showMessage("Sort complete");

    displayImage = result;
    ui->imageLabel->setImage(displayImage);
}

void MainWindow::handleOpenResults(QImage openedImage)
{
    sourceImage = openedImage;
    displayImage = sourceImage.copy(0, 0, sourceImage.width(), sourceImage.height());
    ui->imageLabel->setImage(displayImage);

    canClose = true;
    enableInterface();
    ui->statusBar->showMessage("Opened " + lastOpenedFilename);
}

void MainWindow::handleSaveResults()
{
    canClose = true;
    enableInterface();
    ui->statusBar->showMessage("Saved " + lastSavedFilename);
}

void MainWindow::on_optionsButton_clicked()
{
    OptionsDialog dialog(this);
    dialog.setOptions(options);

    if (dialog.exec() == QDialog::Accepted)
    {
        options = dialog.getOptions();
    }
}

void MainWindow::on_actionReset_triggered()
{
    if (sourceImage.isNull())
    {
        ui->statusBar->showMessage("No image to reset");
        return;
    }

    displayImage = sourceImage.copy(0, 0, sourceImage.width(), sourceImage.height());

    ui->imageLabel->setImage(displayImage);
    ui->statusBar->showMessage("Image reset");
}

void MainWindow::on_actionSave_as_triggered()
{
    if (sourceImage.isNull())
    {
        ui->statusBar->showMessage("No image to save");
        return;
    }

    QSettings settings("EgorCO", "Pixel Sorter");

    QString filename = QFileDialog::getSaveFileName(this, "Save image", settings.value("lastDirectory").toString(),
                                                    "Image Files (*.png *.jpg *.jpeg *.bmp *.gif *.pbm *.pgm *.ppm *.xbm *.xpm)");

    if (filename.isNull())
        return;

    canClose = false;
    disableInterface();

    lastSavedFilename = filename;
    saveImage(filename);
}

void MainWindow::on_actionSave_triggered()
{
    if (lastSavedFilename.isNull())
    {
        on_actionSave_as_triggered();
    }
    else
    {
        saveImage(lastSavedFilename);
    }

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (canClose)
        QMainWindow::closeEvent(event);
    else
        event->ignore();
}

void MainWindow::enableInterface()
{
    QGuiApplication::restoreOverrideCursor();
    ui->menuBar->setEnabled(true);
    ui->sortButton->setEnabled(true);
    ui->optionsButton->setEnabled(true);
    ui->imageLabel->setEnabled(true);
}

void MainWindow::disableInterface()
{
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    ui->menuBar->setEnabled(false);
    ui->sortButton->setEnabled(false);
    ui->optionsButton->setEnabled(false);
    ui->imageLabel->setEnabled(false);
}

void MainWindow::saveImage(QString filename)
{
    SaveThread *thread = new SaveThread(this, displayImage, filename);
    connect(thread, &SaveThread::resultReady, this, &MainWindow::handleSaveResults);
    connect(thread, &SaveThread::finished, thread, &QObject::deleteLater);
    thread->start();
}

void MainWindow::on_actionCopy_triggered()
{
    if (displayImage.isNull())
        return;

    QClipboard *clibboard = QGuiApplication::clipboard();
    clibboard->setImage(displayImage);
}

void MainWindow::on_actionPaste_triggered()
{
    QClipboard *clipboard = QGuiApplication::clipboard();

    if (const QMimeData *mimeData = clipboard->mimeData())
    {
        if (mimeData->hasImage())
        {
            const QImage image = qvariant_cast<QImage>(mimeData->imageData());

            if (!image.isNull())
            {
                sourceImage = image;
                displayImage = sourceImage;
                ui->imageLabel->setImage(displayImage);
            }
        }
    }


}

void MainWindow::on_actionZoom_in_triggered()
{
    ui->imageLabel->scrollImage(1);
}

void MainWindow::on_actionZoom_out_triggered()
{
    ui->imageLabel->scrollImage(-1);
}

void MainWindow::on_actionReset_zoom_triggered()
{
    ui->imageLabel->resetScroll();
}
