#ifndef SORTER_H
#define SORTER_H


#include <QString>
#include <QImage>
#include <QColor>
#include <QDebug>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <functional>
#include <exception>
#include <vector>
#include <deque>
#include <algorithm>
#include <iterator>
#include <cstdlib>
#include <cmath>
#include <string>
#include <iostream>
#include <omp.h>
#include <ppl.h>

struct Point
{
    unsigned short int i;
    unsigned short int j;

    Point() : i(0), j(0) {}
    Point(unsigned short int i, unsigned short int j);
};

class Sorter
{
public:
    Sorter(const QImage &img);
    QImage sort(QString pathType, int maxIntervals, bool randomizeIntervals,
                int angle, bool toMerge, bool toReverse, bool toMirror,
                bool toInterval, int lowThreshold, QString funcType);

private:
    int width;
    int height;
    const QImage image;

    bool compareLightness(const Point &p1, const Point &p2);
    bool compareHue(const Point &p1, const Point &p2);
    bool compareSaturation(const Point &p1, const Point &p2);
    bool compareValue(const Point &p1, const Point &p2);

    std::vector<std::vector<Point>> rows();
    std::vector<std::vector<Point>> columns();
    std::vector<std::vector<Point>> rectangles();
    std::vector<std::vector<Point>> angled(int angle);

    std::vector<std::vector<Point>> applyIntervals(std::vector<std::vector<Point>>* path, int maxIntervals, bool randomize);
    void mergeIntoOne(std::vector<std::vector<Point>>* path);
    void mirror(std::vector<std::vector<Point>> &path);
    void reverseSort(std::vector<std::vector<Point>>* path);

    cv::Mat getEdges(int lowThreshold, int highThreshold, int kernelSize);
    std::vector<std::vector<Point>> edgesRows(int lowThreshold, int highThreshold, int kernelSize);
};

#endif // SORTER_H