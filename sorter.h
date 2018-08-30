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

#include <bitset>
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
                bool toInterval, int lowThreshold, std::vector<QString> funcs,
                bool toEdge, bool toMask, const QImage &mask, bool invertMask);
    QColor pixelAt(int i, int j);

private:
    int width;
    int height;
    const QImage image;

    std::vector<std::vector<Point>> rows();
    std::vector<std::vector<Point>> columns();
    std::vector<std::vector<Point>> rectangles();
    std::vector<std::vector<Point>> octagons();
    std::vector<std::vector<Point>> angled(int angle);

    void applyIntervals(std::vector<std::vector<Point>> &path, int maxIntervals, bool randomize);
    void mergeIntoOne(std::vector<std::vector<Point>> &path);
    void mirror(std::vector<std::vector<Point>> &path);
    void reverseSort(std::vector<std::vector<Point>>* path);

    cv::Mat getEdges(int lowThreshold, int highThreshold, int kernelSize);
    void applyEdges(std::vector<std::vector<Point>> &path, int lowThreshold, int highThreshold, int kernelSize);
    void applyMask(std::vector<std::vector<Point>> &path, const QImage &mask, bool invert);
};

class Comparator
{
public:
    Comparator(Sorter *s, std::vector<QString> funcTypes);
    bool operator() (const Point& p1, const Point&p2) const;

private:
    Sorter *sorter;
    std::vector<std::function<int(const QColor&, const QColor&)>> funcs{};
};

#endif // SORTER_H
