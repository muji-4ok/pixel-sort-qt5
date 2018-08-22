#include "optionsdialog.h"
#include "ui_optionsdialog.h"

OptionsDialog::OptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDialog)
{
    ui->setupUi(this);

    connect(ui->angleSlider, &QSlider::valueChanged, ui->angleSpinBox, &QSpinBox::setValue);
    connect(ui->angleSpinBox, SIGNAL(valueChanged(int)), ui->angleSlider, SLOT(setValue(int)));

    connect(ui->intervalSlider, &QSlider::valueChanged, ui->intervalSpinBox, &QSpinBox::setValue);
    connect(ui->intervalSpinBox, SIGNAL(valueChanged(int)), ui->intervalSlider, SLOT(setValue(int)));

    connect(ui->thresholdSlider, &QSlider::valueChanged, ui->thresholdSpinBox, &QSpinBox::setValue);
    connect(ui->thresholdSpinBox, SIGNAL(valueChanged(int)), ui->thresholdSlider, SLOT(setValue(int)));

    on_pathComboBox_currentTextChanged(ui->pathComboBox->currentText());
    on_doIntervalsCheckBox_stateChanged(ui->doIntervalsCheckBox->isChecked());
    on_doEdgesCheckBox_stateChanged(ui->doEdgesCheckBox->isChecked());
}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}

Options OptionsDialog::getOptions()
{
    Options options(ui->pathComboBox->currentText(), ui->intervalSlider->value(), ui->randomizeCheckBox->isChecked(),
                    ui->angleSlider->value(), ui->mergeCheckBox->isChecked(), ui->reverseCheckBox->isChecked(), ui->mirrorCheckBox->isChecked(),
                    ui->doIntervalsCheckBox->isChecked(), ui->thresholdSlider->value(), ui->funcComboBox->currentText(),
                    ui->doEdgesCheckBox->isChecked());
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
    ui->funcComboBox->setCurrentText(options.funcType);
    ui->doEdgesCheckBox->setChecked(options.toEdge);
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
        std::runtime_error("Checkbox cannot be partially checked");
}

void OptionsDialog::on_doEdgesCheckBox_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked)
    {
        ui->thresholdSlider->setEnabled(true);
        ui->thresholdSpinBox->setEnabled(true);
    }
    else if (arg1 == Qt::Unchecked)
    {
        ui->thresholdSlider->setEnabled(false);
        ui->thresholdSpinBox->setEnabled(false);
    }
    else
        std::runtime_error("Checkbox cannot be partially checked");

}
