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
                    bool toInterval, int lowThreshold, std::vector<QString> funcs,
                    bool toEdge, bool toMask, const QImage &mask, bool invertMask)
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
    else if (pathType == "octagons")
        path = octagons();
    else
        throw std::runtime_error("no pathType");

    if (toEdge)
        applyEdges(path, lowThreshold, lowThreshold * 3, 3);
    else if (toMask && !mask.isNull())
        applyMask(path, mask, invertMask);

    if (toMerge)
        mergeIntoOne(path);

    if (toInterval)
        applyIntervals(path, maxIntervals, randomizeIntervals);

    sortedPath = path;
    Comparator cmp(this, funcs);

    if (sortedPath.size() > 1)
    {
        #pragma omp parallel for
        for (int i = 0; i < sortedPath.size(); ++i)
            std::sort(sortedPath[i].begin(), sortedPath[i].end(), cmp);
    }
    else
    {
        concurrency::parallel_buffered_sort(sortedPath[0].begin(), sortedPath[0].end(), cmp);
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

QColor Sorter::pixelAt(int i, int j)
{
    return image.pixelColor(j, i);
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

bool insideImage(const int &i, const int &j, const int &width, const int &height)
{
    return 0 <= i && i < height && 0 <= j && j < width;
}

std::vector<std::vector<Point>> Sorter::octagons()
{
    std::vector<std::vector<Point>> out {};

    int maxRadius = std::round(std::sqrt(std::pow(height / 2.0, 2) + std::pow(width / 2.0, 2)) * 2 / std::sqrt(2));
    int i_center = height / 2;
    int j_center = width / 2;
    std::vector<std::vector<bool>> pointsDone(maxRadius, std::vector<bool>(maxRadius));

    for (int radius = 0, row = 0; radius < maxRadius; ++radius, ++row)
    {
        std::vector<Point> offsetSegment{};
        out.push_back({});
        int i_off = 0;
        int j_off = radius;

        while (true)
        {
            if (!pointsDone[i_off][j_off])
                offsetSegment.push_back(Point(i_off, j_off));

            pointsDone[i_off][j_off] = true;

            if (i_off == radius && j_off == 0)
                break;

            if (radius % 2 == 0)
            {
                if ((i_off + 1) <= radius && !pointsDone[i_off + 1][j_off])
                {
                    offsetSegment.push_back(Point(i_off + 1, j_off));

                    pointsDone[i_off + 1][j_off] = true;
                }
            }

            if (i_off < radius)
                ++i_off;

            if (j_off > 0 && !pointsDone[i_off][j_off - 1])
                --j_off;
        }

        for (Point &p : offsetSegment)
            if (insideImage(i_center + p.i, j_center + p.j, width, height))
                out[row].push_back(Point(i_center + p.i, j_center + p.j));

        for (int i = offsetSegment.size() - 1; i >= 0; --i)
            if (insideImage(i_center + offsetSegment[i].i, j_center - offsetSegment[i].j, width, height))
                out[row].push_back(Point(i_center + offsetSegment[i].i, j_center - offsetSegment[i].j));

        for (Point &p : offsetSegment)
            if (insideImage(i_center - p.i, j_center - p.j, width, height))
                out[row].push_back(Point(i_center - p.i, j_center - p.j));

        for (int i = offsetSegment.size() - 1; i >= 0; --i)
            if (insideImage(i_center - offsetSegment[i].i, j_center + offsetSegment[i].j, width, height))
                out[row].push_back(Point(i_center - offsetSegment[i].i, j_center + offsetSegment[i].j));

        if (out[row].size() == 0)
            continue;

        std::rotate(out[row].begin(),
                    out[row].begin() + (rand() % out[row].size()),
                    out[row].end());
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

void Sorter::mergeIntoOne(std::vector<std::vector<Point>> &path)
{
    size_t endSize = 0;

    #pragma omp parallel for
    for (int i = 0; i < path.size(); ++i)
        endSize += path[i].size();

    path[0].reserve(endSize);

    #pragma omp parallel for ordered
    for (int i = 1; i < path.size(); ++i)
    {
        #pragma omp ordered
        path[0].insert(path[0].end(), path[i].begin(), path[i].end());
    }

    path.erase(path.begin() + 1, path.end());

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

void Sorter::applyIntervals(std::vector<std::vector<Point>> &path, int maxIntervals, bool randomize)
{
    std::vector<std::vector<Point>> out{};

    if (maxIntervals < 2)
        return;

    for (auto &seq : path)
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

    path = out;
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

void Sorter::applyEdges(std::vector<std::vector<Point>> &path, int lowThreshold, int highThreshold, int kernelSize)
{
    std::vector<std::vector<Point>> out{};
    const cv::Mat edges = getEdges(lowThreshold, highThreshold, kernelSize);

    for (int i = 0; i < path.size(); ++i)
    {
        bool wasEdge = false;
        std::vector<Point> segment{};

        for (int j = 0; j < path[i].size(); ++j)
        {
            bool edge = edges.at<unsigned char>(path[i][j].i, path[i][j].j) > 0;

            if (!edge)
                segment.push_back(path[i][j]);

            if (edge && !wasEdge)
            {
                out.push_back(segment);
                segment = {};
            }

            wasEdge = edge;
        }

        if (segment.size() > 0)
            out.push_back(segment);
    }

    path = out;
}

void Sorter::applyMask(std::vector<std::vector<Point> > &path, const QImage &mask, bool invert)
{
    std::vector<std::vector<Point>> out{};

    for (int i = 0; i < path.size(); ++i)
    {
        bool wasSkipped= false;
        std::vector<Point> segment{};

        for (int j = 0; j < path[i].size(); ++j)
        {
            bool skip = mask.pixelColor(path[i][j].j, path[i][j].i) != QColor(0, 0, 0);

            if (invert)
                skip = !skip;

            if (!skip)
                segment.push_back(path[i][j]);

            if (skip && !wasSkipped)
            {
                out.push_back(segment);
                segment = {};
            }

            wasSkipped = skip;
        }

        if (segment.size() > 0)
            out.push_back(segment);
    }

    path = out;
}

Comparator::Comparator(Sorter *s, std::vector<QString> funcTypes) : sorter(s)
{
    for (QString funcType : funcTypes)
    {
        if (funcType == "lightness")
            funcs.push_back([](const QColor &c1, const QColor &c2){
                return (c1.red() + c1.green() + c1.blue()) - (c2.red() + c2.green() + c2.blue());
            });
        else if (funcType == "hue")
            funcs.push_back([](const QColor &c1, const QColor &c2){
                return c1.hue() - c2.hue();
            });
        else if (funcType == "saturation")
            funcs.push_back([](const QColor &c1, const QColor &c2){
                return c1.saturation() - c2.saturation();
            });
        else if (funcType == "value")
            funcs.push_back([](const QColor &c1, const QColor &c2){
                return c1.value() - c2.value();
            });
        else
            throw std::runtime_error("Incorrect sort func type");
    }

}

bool Comparator::operator()(const Point &p1, const Point &p2) const
{
    QColor c1 = sorter->pixelAt(p1.i, p1.j);
    QColor c2 = sorter->pixelAt(p2.i, p2.j);

    for (auto func : funcs)
    {
        int res = func(c1, c2);

        if (res == 0)
            continue;

        return res < 0;
    }

    return false;
}
