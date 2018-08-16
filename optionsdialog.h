#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>
#include <QSlider>
#include <QSpinBox>

struct Options
{
    QString pathType;
    int maxIntervals;
    bool randomizeIntervals;
    int angle;
    bool toMerge;
    bool toReverse;
    bool toMirror;
    bool toInterval;
    int lowThreshold;
    QString funcType;

    Options() :
        pathType("rows"), maxIntervals(0), randomizeIntervals(false), angle(0), toMerge(false), toReverse(false), toMirror(false),
        toInterval(false), lowThreshold(0), funcType("lightness") {}

    Options(QString pt, int mi, bool ri, int a, bool tom, bool tor, bool tomi, bool toi, int lt, QString ft) :
        pathType(pt), maxIntervals(mi), randomizeIntervals(ri), angle(a), toMerge(tom), toReverse(tor), toMirror(tomi),
        toInterval(toi), lowThreshold(lt), funcType(ft) {}
};


namespace Ui {
    class OptionsDialog;
}

class OptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionsDialog(QWidget *parent = 0);
    ~OptionsDialog();

    Options getOptions();

    void setOptions(Options &options);

private slots:
    void on_pathComboBox_currentTextChanged(const QString &arg1);

    void on_doIntervalsCheckBox_stateChanged(int arg1);

private:
    Ui::OptionsDialog *ui;
};

#endif // OPTIONSDIALOG_H
