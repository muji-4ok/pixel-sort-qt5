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

    connect(ui->actionQualityZoomCheckBox, &QAction::toggled, ui->imageWidget, &ImageWidget::setQualityZoom);
    connect(ui->imageWidget, &ImageWidget::changeRgbInfoMessage, ui->pixelInfoLineEdit, &QLineEdit::setText);
    connect(ui->imageWidget, &ImageWidget::changeRgbInfo, this, &MainWindow::changePixelInfo);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    QSettings settings("EgorCO", "Pixel Sorter");

    QString filename = QFileDialog::getOpenFileName(this, "Open image", settings.value("lastDirectory").toString(),
                                                    QString("Image Files (*.png *.jpg *.jpeg *.bmp *.gif *.pbm *.pgm *.ppm *.xbm *.xpm);;") +
                                                    "PNG (*.png);;" +
                                                    "JPEG (*.jpg *.jpeg);;" +
                                                    "GIF (*.gif);;" +
                                                    "Bitmap (*.bmp *.pbm *.pgm *.ppm *.xbm *.xpm)");

    if (filename.isNull())
        return;

    QFileInfo fileInfo(filename);
    settings.setValue("lastDirectory", fileInfo.dir().absolutePath());

    lastSavedFilename.clear();
    lastOpenedFilename = filename;

    canClose = false;
    disableInterface();

    OpenThread *thread = new OpenThread(this, filename);
    connect(thread, &OpenThread::resultReady, this, &MainWindow::handleOpenResults);
    connect(thread, &OpenThread::finished, thread, &QObject::deleteLater);
    thread->start();
}

void MainWindow::handleOpenResults(QImage openedImage)
{
    if (openedImage.isNull())
    {
        ui->statusBar->showMessage("Failed to open " + lastOpenedFilename);
        canClose = true;
        enableInterface();
        return;
    }

    options.mask = QImage();
    sourceImage = std::move(openedImage);
    displayImage = sourceImage.copy(0, 0, sourceImage.width(), sourceImage.height());
    ui->imageWidget->setImage(displayImage);

    canClose = true;
    enableInterface();
    ui->statusBar->showMessage("Opened " + lastOpenedFilename);
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

    displayImage = std::move(result);
    ui->imageWidget->setImage(displayImage);
}

void MainWindow::on_optionsButton_clicked()
{
    OptionsDialog dialog(this, lastOptionsTab, sourceImage.size());
    dialog.setOptions(options);

    if (dialog.exec() == QDialog::Accepted)
    {
        options = dialog.getOptions();

        if (sortAfterChange)
            ui->sortButton->click();
    }

    lastOptionsTab = dialog.getLastOptionsTab();
}

void MainWindow::on_actionReset_triggered()
{
    if (sourceImage.isNull())
    {
        ui->statusBar->showMessage("No image to reset");
        return;
    }

    displayImage = sourceImage.copy(0, 0, sourceImage.width(), sourceImage.height());

    ui->imageWidget->setImage(displayImage);
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
                                                    QString("Image Files (*.png *.jpg *.jpeg *.bmp *.gif *.pbm *.pgm *.ppm *.xbm *.xpm);;") +
                                                    "PNG (*.png);;" +
                                                    "JPEG (*.jpg *.jpeg);;" +
                                                    "GIF (*.gif);;" +
                                                    "Bitmap (*.bmp *.pbm *.pgm *.ppm *.xbm *.xpm)");

    if (filename.isNull())
        return;

    canClose = false;
    disableInterface();

    lastSavedFilename = filename;
    saveImage(filename);
}

void MainWindow::handleSaveResults()
{
    canClose = true;
    enableInterface();
    ui->statusBar->showMessage("Saved " + lastSavedFilename);
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

void MainWindow::saveImage(QString filename)
{
    SaveThread *thread = new SaveThread(this, displayImage, filename);
    connect(thread, &SaveThread::resultReady, this, &MainWindow::handleSaveResults);
    connect(thread, &SaveThread::finished, thread, &QObject::deleteLater);
    thread->start();
}

void MainWindow::enableInterface()
{
    QGuiApplication::restoreOverrideCursor();
    ui->menuBar->setEnabled(true);
    ui->sortButton->setEnabled(true);
    ui->optionsButton->setEnabled(true);
    ui->imageWidget->setEnabled(true);
    ui->pixelInfoLineEdit->setEnabled(true);
    ui->pixelInfoComboBox->setEnabled(true);
}

void MainWindow::disableInterface()
{
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    ui->menuBar->setEnabled(false);
    ui->sortButton->setEnabled(false);
    ui->optionsButton->setEnabled(false);
    ui->imageWidget->setEnabled(false);
    ui->pixelInfoLineEdit->setEnabled(false);
    ui->pixelInfoComboBox->setEnabled(false);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (canClose)
        QMainWindow::closeEvent(event);
    else
        event->ignore();
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
                ui->imageWidget->setImage(displayImage);
            }
        }
    }
}

void MainWindow::on_actionZoom_in_triggered()
{
    ui->imageWidget->scrollImage(ImageWidget::Scroll_UP);
}

void MainWindow::on_actionZoom_out_triggered()
{
    ui->imageWidget->scrollImage(ImageWidget::Scroll_DOWN);
}

void MainWindow::on_actionReset_zoom_triggered()
{
    ui->imageWidget->resetScroll();
}

void MainWindow::on_actionSort_after_changing_options_2_toggled(bool arg1)
{
    sortAfterChange = arg1;
}

void MainWindow::changePixelInfo(const QColor &c)
{
    QString infoType = ui->pixelInfoComboBox->currentText();

    ui->pixelInfoColorLabel->setStyleSheet("QLabel { background-color : rgb(" +
                                           QString::number(c.red()) + "," +
                                           QString::number(c.green()) + "," +
                                           QString::number(c.blue()) + "); border: 2px solid black }");

    if (infoType == "RGB")
    {
        ui->pixelInfoLineEdit->setText(QString::number(c.red()) + " " + QString::number(c.green()) + " " + QString::number(c.blue()));
    }
    else if (infoType == "HSV")
    {
        ui->pixelInfoLineEdit->setText(QString::number(c.hue() + 1) + " " + QString::number(c.saturation()) + " " + QString::number(c.value()));
    }
    else if (infoType == "lightness")
    {
        ui->pixelInfoLineEdit->setText(QString::number(c.red() + c.green() + c.blue()));
    }
}
