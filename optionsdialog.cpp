#include "optionsdialog.h"
#include "ui_optionsdialog.h"
#include "openthread.h"
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QSettings>
#include <utility>

OptionsDialog::OptionsDialog(QWidget *parent, int lastTab, QSize sourceSize) :
    QDialog(parent), sourceSize(sourceSize),
    ui(new Ui::OptionsDialog)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(lastTab);

    connect(ui->angleSlider, &QSlider::valueChanged, ui->angleSpinBox, &QSpinBox::setValue);
    connect(ui->angleSpinBox, SIGNAL(valueChanged(int)), ui->angleSlider, SLOT(setValue(int)));

    connect(ui->intervalSlider, &QSlider::valueChanged, ui->intervalSpinBox, &QSpinBox::setValue);
    connect(ui->intervalSpinBox, SIGNAL(valueChanged(int)), ui->intervalSlider, SLOT(setValue(int)));

    connect(ui->thresholdSlider, &QSlider::valueChanged, ui->thresholdSpinBox, &QSpinBox::setValue);
    connect(ui->thresholdSpinBox, SIGNAL(valueChanged(int)), ui->thresholdSlider, SLOT(setValue(int)));

    on_pathComboBox_currentTextChanged(ui->pathComboBox->currentText());
    on_doIntervalsCheckBox_stateChanged(ui->doIntervalsCheckBox->isChecked());
    on_doEdgesCheckBox_stateChanged(ui->doEdgesCheckBox->isChecked());
    on_doMaskCheckBox_stateChanged(ui->doMaskCheckBox->isChecked());
}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}

Options OptionsDialog::getOptions()
{
    Options options(ui->pathComboBox->currentText(), ui->intervalSlider->value(), ui->randomizeCheckBox->isChecked(),
                    ui->angleSlider->value(), ui->mergeCheckBox->isChecked(), ui->reverseCheckBox->isChecked(), ui->mirrorCheckBox->isChecked(),
                    ui->doIntervalsCheckBox->isChecked(), ui->thresholdSlider->value(), ui->orderFuncListWidget->getOrder(),
                    ui->doEdgesCheckBox->isChecked(), ui->doMaskCheckBox->isChecked(), mask, ui->skipBlackRadioButton->isChecked());
    return options;
}

void OptionsDialog::setOptions(Options &options)
{
    ui->pathComboBox->setCurrentText(options.pathType);
    ui->intervalSlider->setValue(options.maxIntervals);
    ui->randomizeCheckBox->setChecked(options.randomizeIntervals);
    ui->angleSlider->setValue(options.angle);
    ui->mergeCheckBox->setChecked(options.toMerge);
    ui->reverseCheckBox->setChecked(options.toReverse);
    ui->mirrorCheckBox->setChecked(options.toMirror);
    ui->doIntervalsCheckBox->setChecked(options.toInterval);
    ui->thresholdSlider->setValue(options.lowThreshold);
    ui->orderFuncListWidget->setOrder(options.funcs);
    ui->doEdgesCheckBox->setChecked(options.toEdge);
    ui->doMaskCheckBox->setChecked(options.toMask);
    mask = options.mask;

    if (!mask.isNull())
    {
        QBitmap bit = QBitmap::fromImage(mask.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
//        QPixmap pix = QPixmap::fromImage(mask.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        ui->maskLabel->setPixmap(bit);
    }

    ui->skipBlackRadioButton->setChecked(options.invertMask);
}

int OptionsDialog::getLastOptionsTab()
{
    return ui->tabWidget->currentIndex();
}

void OptionsDialog::handleMaskOpenResults(QImage openedImage)
{
    if (openedImage.isNull() || openedImage.size() != sourceSize)
    {
        canClose = true;
        enableInterface();
        return;
    }

    mask = openedImage.convertToFormat(QImage::Format_Mono);
    QBitmap bit = QBitmap::fromImage(mask.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
//    QPixmap pix = QPixmap::fromImage(mask.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->maskLabel->setPixmap(bit);

    canClose = true;
    enableInterface();
}

void OptionsDialog::on_pathComboBox_currentTextChanged(const QString &arg1)
{
    ui->angleSlider->setEnabled(false);
    ui->angleSpinBox->setEnabled(false);

    if (arg1 == "angled")
    {
        ui->angleSlider->setEnabled(true);
        ui->angleSpinBox->setEnabled(true);
    }
}

void OptionsDialog::on_doIntervalsCheckBox_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked)
    {
        ui->intervalSlider->setEnabled(true);
        ui->intervalSpinBox->setEnabled(true);
        ui->randomizeCheckBox->setEnabled(true);
    }
    else if (arg1 == Qt::Unchecked)
    {
        ui->intervalSlider->setEnabled(false);
        ui->intervalSpinBox->setEnabled(false);
        ui->randomizeCheckBox->setEnabled(false);
    }
    else
        throw std::runtime_error("Checkbox cannot be partially checked");
}

void OptionsDialog::on_doEdgesCheckBox_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked)
    {
        ui->thresholdSlider->setEnabled(true);
        ui->thresholdSpinBox->setEnabled(true);
        ui->doMaskCheckBox->setChecked(false);
    }
    else if (arg1 == Qt::Unchecked)
    {
        ui->thresholdSlider->setEnabled(false);
        ui->thresholdSpinBox->setEnabled(false);
    }
    else
        throw std::runtime_error("Checkbox cannot be partially checked");

}

void OptionsDialog::on_doMaskCheckBox_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked)
    {
        ui->loadMaskPushButton->setEnabled(true);
        ui->doEdgesCheckBox->setChecked(false);
    }
    else if (arg1 == Qt::Unchecked)
    {
        ui->loadMaskPushButton->setEnabled(false);
    }
    else
        throw std::runtime_error("Checkbox cannot be partially checked");
}

void OptionsDialog::on_loadMaskPushButton_clicked()
{
    QSettings settings("EgorCO", "Pixel Sorter");

    QString filename = QFileDialog::getOpenFileName(this, "Open mask", settings.value("lastDirectory").toString(),
                                                    QString("Image Files (*.png *.jpg *.jpeg *.bmp *.gif *.pbm *.pgm *.ppm *.xbm *.xpm);;") +
                                                    "PNG (*.png);;" +
                                                    "JPEG (*.jpg *.jpeg);;" +
                                                    "GIF (*.gif);;" +
                                                    "Bitmap (*.bmp *.pbm *.pgm *.ppm *.xbm *.xpm)");

    if (filename.isNull())
        return;

    QFileInfo fileInfo(filename);
    settings.setValue("lastDirectory", fileInfo.dir().absolutePath());

    canClose = false;
    disableInterface();

    OpenThread *thread = new OpenThread(this, filename);
    connect(thread, &OpenThread::resultReady, this, &OptionsDialog::handleMaskOpenResults);
    connect(thread, &OpenThread::finished, thread, &QObject::deleteLater);
    thread->start();
}

void OptionsDialog::closeEvent(QCloseEvent *event)
{
    if (canClose)
        QDialog::closeEvent(event);
    else
        event->ignore();
}

void OptionsDialog::disableInterface()
{
    setEnabled(false);
}

void OptionsDialog::enableInterface()
{
    setEnabled(true);
}
