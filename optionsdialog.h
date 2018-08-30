#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>
#include <QSlider>
#include <QSpinBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QCloseEvent>
#include <QImage>
#include <QPixmap>
#include <QBitmap>
#include <vector>

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
    std::vector<QString> funcs;
    bool toEdge;
    bool toMask;
    QImage mask;
    bool invertMask;

    Options() :
        pathType("rows"), maxIntervals(0), randomizeIntervals(false), angle(0), toMerge(false), toReverse(false), toMirror(false),
        toInterval(false), lowThreshold(0), funcs{"lightness"}, toEdge(false), toMask(false), mask(), invertMask(false) {}

    Options(QString pathType, int maxIntervals, bool randomizeIntervals, int angle, bool toMerge, bool toReverse, bool toMirror,
            bool toInterval, int lowThreshold, std::vector<QString> funcs, bool toEdge, bool toMask, QImage mask, bool invertMask)
        :
        pathType(pathType), maxIntervals(maxIntervals), randomizeIntervals(randomizeIntervals), angle(angle), toMerge(toMerge),
        toReverse(toReverse), toMirror(toMirror), toInterval(toInterval), lowThreshold(lowThreshold), funcs(funcs), toEdge(toEdge),
        toMask(toMask), mask(mask), invertMask(invertMask)
        {}
};


namespace Ui {
    class OptionsDialog;
}

class OptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionsDialog(QWidget *parent, int lastTab, QSize sourceSize);
    ~OptionsDialog();

    Options getOptions();

    void setOptions(Options &options);
    int getLastOptionsTab();

public slots:
    void handleMaskOpenResults(QImage openedImage);

private slots:
    void on_pathComboBox_currentTextChanged(const QString &arg1);

    void on_doIntervalsCheckBox_stateChanged(int arg1);

    void on_doEdgesCheckBox_stateChanged(int arg1);

    void on_doMaskCheckBox_stateChanged(int arg1);

    void on_loadMaskPushButton_clicked();

protected:
    void closeEvent(QCloseEvent *) override;

private:
    Ui::OptionsDialog *ui;
    bool canClose = true;
    QImage mask;
    QSize sourceSize;

    void disableInterface();
    void enableInterface();
};

#endif // OPTIONSDIALOG_H
