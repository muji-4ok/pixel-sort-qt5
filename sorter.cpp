#include "sorter.h"

Point::Point(unsigned short int i, unsigned short int j) : i(i), j(j) {}

bool Sorter::compareLightness(const Point &p1, const Point &p2)
{
    QColor c1 = image.pixelColor(p1.j, p1.i);
    QColor c2 = image.pixelColor(p2.j, p2.i);

    int lightness1 = c1.red() + c1.green() + c1.blue();
    int lightness2 = c2.red() + c2.green() + c2.blue();

    return lightness1 < lightness2;
}

bool Sorter::compareHue(const Point &p1, const Point &p2)
{
    QColor c1 = image.pixelColor(p1.j, p1.i);
    QColor c2 = image.pixelColor(p2.j, p2.i);

    return c1.hue() < c2.hue();
}

bool Sorter::compareSaturation(const Point &p1, const Point &p2)
{
    QColor c1 = image.pixelColor(p1.j, p1.i);
    QColor c2 = image.pixelColor(p2.j, p2.i);

    return c1.saturation() < c2.saturation();
}

bool Sorter::compareValue(const Point &p1, const Point &p2)
{
    QColor c1 = image.pixelColor(p1.j, p1.i);
    QColor c2 = image.pixelColor(p2.j, p2.i);

    return c1.value() < c2.value();
}

Sorter::Sorter(const QImage &img) : image(img)
{
    width = image.width();
    height = image.height();
    srand(time(nullptr));
}

QImage Sorter::sort(QString pathType, int maxIntervals, bool randomizeIntervals,
                    int angle, bool toMerge, bool toReverse, bool toMirror,
                    bool toInterval, int lowThreshold, QString funcType)
{
    std::vector<std::vector<Point>> path;
    std::vector<std::vector<Point>> sortedPath;

    if (pathType == "rows")
        path = rows();
    else if (pathType == "columns")
        path = columns();
    else if (pathType == "rectangles")
        path = rectangles();
    else if (pathType == "angled")
        path = angled(angle);
    else if (pathType == "edges rows")
        path = edgesRows(lowThreshold, lowThreshold * 3, 3);
    else
        throw std::runtime_error("no pathType");

    if (toMerge)
        mergeIntoOne(&path);

    if (toInterval)
        path = applyIntervals(&path, maxIntervals, randomizeIntervals);

    sortedPath = path;

    if (sortedPath.size() > 1)
    {
        if (funcType == "lightness")
        {
            #pragma omp parallel for
            for (int i = 0; i < sortedPath.size(); ++i)
                std::stable_sort(sortedPath[i].begin(), sortedPath[i].end(), [this](const Point &p1, const Point &p2) -> bool {
                    return compareLightness(p1, p2);
                });
        }
        else if (funcType == "value")
        {
            #pragma omp parallel for
            for (int i = 0; i < sortedPath.size(); ++i)
                std::stable_sort(sortedPath[i].begin(), sortedPath[i].end(), [this](const Point &p1, const Point &p2) -> bool {
                    return compareValue(p1, p2);
                });
        }
        else if (funcType == "saturation")
        {
            #pragma omp parallel for
            for (int i = 0; i < sortedPath.size(); ++i)
                std::stable_sort(sortedPath[i].begin(), sortedPath[i].end(), [this](const Point &p1, const Point &p2) -> bool {
                    return compareSaturation(p1, p2);
                });
        }
        else if (funcType == "hue")
        {
            #pragma omp parallel for
            for (int i = 0; i < sortedPath.size(); ++i)
                std::stable_sort(sortedPath[i].begin(), sortedPath[i].end(), [this](const Point &p1, const Point &p2) -> bool {
                    return compareHue(p1, p2);
                });
        }
        else
        {
            throw std::runtime_error("No funcType");
        }
    }
    else
    {
        if (funcType == "lightness")
            concurrency::parallel_radixsort(sortedPath[0].begin(), sortedPath[0].end(), [this](const Point p) -> int {
                QColor c = image.pixelColor(p.j, p.i);
                return c.red() + c.green() + c.blue();
            });
        else if (funcType == "value")
            concurrency::parallel_radixsort(sortedPath[0].begin(), sortedPath[0].end(), [this](const Point p) -> int {
                return image.pixelColor(p.j, p.i).value();
            });
        else if (funcType == "saturation")
            concurrency::parallel_radixsort(sortedPath[0].begin(), sortedPath[0].end(), [this](const Point p) -> int {
                return image.pixelColor(p.j, p.i).saturation();
            });
        else if (funcType == "hue")
            concurrency::parallel_radixsort(sortedPath[0].begin(), sortedPath[0].end(), [this](const Point p) -> int {
                return image.pixelColor(p.j, p.i).hue();
            });
        else
            throw std::runtime_error("No funcType");
    }

    if (toReverse)
        reverseSort(&sortedPath);

    if (toMirror)
        mirror(sortedPath);

    QImage sortedImage = image.copy(0, 0, width, height);

    #pragma omp parallel for
    for (int i = 0; i < path.size(); ++i)
        for (int j = 0; j < path[i].size(); ++j)
        {
            Point before = path[i][j];
            Point after = sortedPath[i][j];
            sortedImage.setPixel(before.j, before.i, image.pixel(after.j, after.i));
        }

    return sortedImage;
}

std::vector<std::vector<Point>> Sorter::rows()
{
    std::vector<std::vector<Point>> out{};

    for (int i = 0; i < height; ++i)
    {
        out.push_back({});
        out[i].reserve(width);
    }

    #pragma omp parallel for
    for (int i = 0; i < height; ++i)
    {
        //out.push_back({});

        for (int j = 0; j < width; ++j)
            out[i].push_back(Point(i, j));
    }

    return out;
}

std::vector<std::vector<Point>> Sorter::columns()
{
    std::vector<std::vector<Point>> out{};

    for (int j = 0; j < width; ++j)
    {
        out.push_back({});
        out[j].reserve(height);
    }

    #pragma omp parallel for
    for (int j = 0; j < width; ++j)
    {
        //out.push_back({});

        for (int i = 0; i < height; ++i)
            out[j].push_back(Point(i, j));

    }

    return out;
}

std::vector<std::vector<Point>> Sorter::rectangles()
{
    std::vector<std::vector<Point>> out{};
    int maxBorderDist = width < height ? width / 2 : height / 2;

    for (int borderDist = 0; borderDist < maxBorderDist; ++borderDist)
    {
        int i0 = borderDist;
        int j0 = borderDist;
        int i1 = static_cast<int>(height) - borderDist;
        int j1 = static_cast<int>(width) - borderDist;

        out.push_back({});
        out[borderDist].reserve(j1 - j0 +
                                i1 - i0 - 1 +
                                j1 - 2 - j0 + 1 +
                                i1 - 2 - i0 - 1 + 1);
    }

    #pragma omp parallel for
    for (int borderDist = 0; borderDist < maxBorderDist; ++borderDist)
    {
        int i0 = borderDist;
        int j0 = borderDist;
        int i1 = static_cast<int>(height) - borderDist;
        int j1 = static_cast<int>(width) - borderDist;

        for (int j = j0; j < j1; ++j)
            out[borderDist].push_back(Point(i0, j));

        for (int i = i0 + 1; i < i1; ++i)
            out[borderDist].push_back(Point(i, j1 - 1));

        for (int j = j1 - 2; j >= j0; --j)
            out[borderDist].push_back(Point(i1 - 1, j));

        for (int i = i1 - 2; i >= i0 + 1; --i)
            out[borderDist].push_back(Point(i, j0));

        std::rotate(out[borderDist].begin(),
                    out[borderDist].begin() + (rand() % out[borderDist].size()),
                    out[borderDist].end());
    }

    return out;
}

std::vector<std::vector<Point>> Sorter::angled(int angle)
{
    constexpr double pi = 3.1415926535897;

    bool toFlipY = angle >= 0;

    if (!toFlipY)
        angle *= -1;

    bool toFlipX = angle > 90;

    int times = 0;

    if (toFlipX)
    {
        times = angle / 90;
        angle %= 90;
    }

    bool toTranspose = angle > 45;

    if (toTranspose)
    {
        int temp = width;
        width = height;
        height = temp;
        angle = 90 - angle;
    }

    int deltay = std::round((width - 1) * std::tan(angle * pi / 180));
    int deltax = width - 1;
    int err = 0;
    int deltaerr = deltay;
    int y = 0;
    std::vector<Point> line{};
    line.reserve(deltax + 1);

    #pragma omp parallel for ordered
    for (int x = 0; x < deltax + 1; ++x)
    {
        #pragma omp ordered
        {
            line.push_back(Point(y, x));
            err += deltaerr;

            if (2 * err >= deltax)
            {
                ++y;
                err -= deltax;
            }
        }
    }

    std::vector<std::vector<Point>> out{};

    for (int di = -deltay; di < static_cast<int>(height); ++di)
    {
        out.push_back({});
        unsigned int lineSize = 0;

        #pragma omp parallel for
        for (int i = 0; i < line.size(); ++i)
        {
            int pointI = line[i].i + di;

            if (0 <= pointI && pointI < height)
                ++lineSize;
            else if (lineSize > 0)
                break;
        }

        out[di + deltay].reserve(lineSize);
    }

    #pragma omp parallel for
    for (int di = -deltay; di < static_cast<int>(height); ++di)
    {
        for (const Point& p : line)
        {
            int i = p.i + di;
            int j = p.j;

            if (0 <= i && i < height)
                out[di + deltay].push_back(Point(i, j));
            else if (out[di + deltay].size() > 0)
                break;
        }
    }

    #pragma omp parallel for
    for (int i = 0; i < out.size(); ++i)
    {
        for (int j = 0; j < out[i].size(); ++j)
        {
            if (toFlipY)
                out[i][j].i = height - 1 - out[i][j].i;

            if (times % 2 == 1)
                out[i][j].j = width - 1 - out[i][j].j;

            if (toTranspose)
            {
                unsigned int temp = out[i][j].i;
                out[i][j].i = out[i][j].j;
                out[i][j].j = temp;
            }
        }
    }

    if (toTranspose)
    {
        int temp = width;
        width = height;
        height = temp;
    }

    return out;
}

void Sorter::mergeIntoOne(std::vector<std::vector<Point>>* path)
{
    size_t endSize = 0;

    #pragma omp parallel for
    for (int i = 0; i < path->size(); ++i)
        endSize += (*path)[i].size();

    (*path)[0].reserve(endSize);

    #pragma omp parallel for ordered
    for (int i = 1; i < path->size(); ++i)
    {
        #pragma omp ordered
        (*path)[0].insert((*path)[0].end(), (*path)[i].begin(), (*path)[i].end());
    }

    path->erase(path->begin() + 1, path->end());

    //while ((*path).size() > 1)
    //{
    //    (*path)[0].insert((*path)[0].end(), (*path)[1].begin(), (*path)[1].end());
    //    path->erase(path->begin() + 1);
    //}

    //std::cout << "Merge done\n";
}

void Sorter::reverseSort(std::vector<std::vector<Point>>* path)
{
    #pragma omp parallel for
    for (int i = 0; i < path->size(); ++i)
        std::reverse((*path)[i].begin(), (*path)[i].end());
}

void Sorter::mirror(std::vector<std::vector<Point>> &path)
{
    for (std::vector<Point> &seq : path)
    {
        std::deque<Point> mirrored{};

        for (int i = seq.size() - 1; i >= 0; --i)
        {
            if (i % 2 == 0)
                mirrored.push_back(seq[i]);
            else
                mirrored.push_front(seq[i]);

            seq.erase(seq.begin() + i);
        }

        seq.assign(mirrored.begin(), mirrored.end());
    }

    //std::cout << "Mirror done\n";
}

std::vector<std::vector<Point>> Sorter::applyIntervals(std::vector<std::vector<Point>>* path, int maxIntervals, bool randomize)
{
    std::vector<std::vector<Point>> out{};

    if (maxIntervals < 2)
        return *path;

    for (auto &seq : (*path))
    {
        int start = 0;

        while (start < static_cast<int>(seq.size()))
        {
            int size;

            if (randomize)
                size = rand() % maxIntervals;
            else
                size = maxIntervals;

            std::vector<Point> segment;

            if (start + size < static_cast<int>(seq.size()))
                segment = { seq.begin() + start, seq.begin() + start + size };
            else
                segment = { seq.begin() + start, seq.end() };

            start += size;

            if (segment.size() > 0)
                out.push_back(segment);
        }
    }

    //std::cout << "Intervals done\n";

    return out;
}

cv::Mat Sorter::getEdges(int lowThreshold, int highThreshold, int kernelSize)
{
//    unsigned char *pixPtr = const_cast<unsigned char *>(image->getPixelsPtr());
//    image.convertToFormat(QImage::Format_RGB888);
    const unsigned char *pixPtr = image.bits();
    cv::Mat src(height, width, CV_8UC4, const_cast<unsigned char*>(pixPtr), cv::Mat::AUTO_STEP);

//    cv::namedWindow("SOURCE", cv::WINDOW_AUTOSIZE);
//    cv::imshow("SOURCE", src);
//    cv::waitKey(0);

    cv::Mat edges;
    src.copyTo(edges);

    cv::cvtColor(edges, edges, cv::COLOR_RGB2GRAY);
    cv::blur(edges, edges, cv::Size(3, 3));
    cv::Canny(edges, edges, lowThreshold, highThreshold, kernelSize);

//    cv::namedWindow("EDGES", cv::WINDOW_AUTOSIZE);
//    cv::imshow("EDGES", edges);
//    cv::waitKey(0);

    //std::cout << "GetEdges done\n";

    return edges;
}

std::vector<std::vector<Point>> Sorter::edgesRows(int lowThreshold, int highThreshold, int kernelSize)
{
    std::vector<std::vector<Point>> out{};
    cv::Mat edges = getEdges(lowThreshold, highThreshold, kernelSize);

    //cv::namedWindow("DEBUGGING", cv::WINDOW_AUTOSIZE);
    //cv::imshow("DEBUGGING", edges);
    //cv::waitKey(0);

    for (int i = 0; i < height; ++i)
    {
        out.push_back({});
        unsigned char *row = edges.ptr<unsigned char>(i);

        for (int j = 0; j < width; ++j)
            if (row[j] > 0 && out[out.size() - 1].size() > 0)
                out.push_back({});
            else
                out[out.size() - 1].push_back(Point(i, j));

        if (out[out.size() - 1].size() == 0)
            out.erase(out.end() - 1);
    }

    //std::cout << "EdgesRows done\n";

    return out;
}
