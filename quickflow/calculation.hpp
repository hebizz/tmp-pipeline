#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/features2d.hpp>

#include <vector>

#ifndef __CALCULATION_HPP_
#define __CALCULATION_HPP_

using namespace cv;
using namespace std;

void ImgShow(string WindowsName, Mat MyImg);
void Delay(int time);

float calcDistance(Point p1, Point2f p2);
float calcDistance(Point2f p1, Point2f p2);
float calcDistance(Point p1, Point p2);

float calcAngle(Point p1, Point p2);

Point calcCenter(vector<Point> contour);

#endif