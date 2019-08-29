#include "calculation.hpp"

void ImgShow(string WindowsName, cv::Mat MyImg)
{
    cv::imshow(WindowsName, MyImg);
}
void Delay(int time) {
    clock_t now = clock();
    while (clock()-now < time);
}

float calcDistance(Point p1, Point2f p2) {
    float distance;
    distance = sqrt((p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y));
    return distance;
}

float calcDistance(Point2f p1, Point2f p2) {
    float distance;
    distance = sqrt((p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y));
    return distance;
}

float calcDistance(Point p1, Point p2) {
    float distance;
    distance = sqrt((p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y));
    return distance;
}

float calcAngle(Point p1, Point p2) {
    float angle;
    angle = atan2(float(p2.y - p1.y), float(p2.x - p1.x));
    angle = -(angle * 180.0) / 3.141592653589;
    return angle;
}

Point calcCenter(vector<Point> contour) {
    int centerx = 0, centery = 0, n = contour.size();
    int xMax = 0;
    int xMin = 9999;
    int yMax = 0;
    int yMin = 9999;

    for (int i = 0; i < n; ++i)
    {
        xMax = contour[i].x > xMax ? contour[i].x : xMax;
        xMin = contour[i].x < xMin ? contour[i].x : xMin;
        yMax = contour[i].y > yMax ? contour[i].y : yMax;
        yMin = contour[i].y < yMin ? contour[i].y : yMin;
    }
    centerx = 0.5 * (xMax + xMin);
    centery = 0.5 * (yMax + yMin);
    Point pointXY = Point(centerx, centery);
    return pointXY;
}