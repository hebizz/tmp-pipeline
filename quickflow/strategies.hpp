#include "object.hpp"
#include "calculation.hpp"
#include "redis.hpp"
#include <iostream>

#ifndef __STRATEGIES_HPP_
#define __STRATEGIES_HPP_

#define BARCODE_MAX_BIT 6

extern vector<vector<Point>> contours;
extern vector<Vec4i> hierarchy;

extern vector<recBarcodeResInfo> vecBarcodeRes;
extern vector<recBarcodeInfo> vecBarcode;
extern recBarcodeInfo recBarcode;
extern vector<recBarcodeGroupInfo> vecBarcodeGroup;
extern recBarcodeGroupInfo recBarcodeGroup;

extern int ShowCnt;
extern cv::Mat src;
extern cv::Mat src_gray;


void calcDataReset(void);

void findBarcodeLeftAspect();
void findBarcodeRightAspect();

void findHierarchy();
int findIdGroup(int id,Redis *r);

#endif
