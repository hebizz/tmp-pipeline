#include "object.hpp"
#include "calculation.hpp"
#include "strategies.hpp"
#include "redis.hpp"
#include<iostream>
#include<math.h>
#include<time.h>
#include<stdio.h>
#include<thread>
#include<future>      //std::future std::promise
#include<utility>     //std::ref
#include<chrono>      //std::chrono::seconds
#include <ctime>
//#include <hiredis/hiredis.h>

//using namespace cv;

vector<recBarcodeResInfo> vecBarcodeRes;
vector<recBarcodeInfo> vecBarcode;
recBarcodeInfo recBarcode;
vector<recBarcodeGroupInfo> vecBarcodeGroup;
recBarcodeGroupInfo recBarcodeGroup;

vector<vector<Point>> contours;
vector<Vec4i> hierarchy;

cv::Mat src, src_gray;

int ShowCnt = 0;

clock_t start_t, end_t;

int calculation(int id,Redis *r) {
    int ID;
    cv::Mat cvBinImg;
    start_t = clock();
    cvtColor(src, src_gray, CV_BGR2GRAY);
    threshold(src_gray, cvBinImg, 200, 255, THRESH_BINARY);

    calcDataReset();
    // use opencv findContours
    findContours(cvBinImg, contours, hierarchy, CV_RETR_TREE, CHAIN_APPROX_NONE, Point(0, 0));

    findHierarchy();
    findBarcodeLeftAspect();
    findBarcodeRightAspect();
    ID = findIdGroup(id,r);
    if (ID < 0)
    {   
        cout << "error id is ::" << id << "exception ::" << ID << endl;         
        return id;
    }
    return ID;

//    end_t = clock();
//    double tot = (double)(end_t - start_t) / CLOCKS_PER_SEC * 1000.0;
//    cv::Point tp;  tp.x = 50; tp.y = 50;
 //   putText(src, "t::" + to_string(tot), tp, cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0,225,225),1,8,0);
//    cv::imshow("calc", src);
    //cv::imshow("cvBinImg", cvBinImg);

    return 0;
}

int main() {
    
    clock_t  start, finish, srcstart, srcfinish, initstart, initfinish;
    initstart = clock();
    int id = 0;

//    Mat src = imread(argv[1], 1);
//    if (src.empty()) {
//        fprintf(stderr, "Can not load image!\n");
//        return -1;
//    }
    cv::VideoCapture cap;
    
    if(!cap.open("rtmp://192.168.0.123/live/usb2")) {
        printf("error open videocapture");
    }
    
    Redis *r = new Redis();
    
    if(!r->connect("127.0.0.1", 6379))
    {
        printf("connect error!\n");
        return 0;
    }
    
    
    r->flushall();
    initfinish = clock();
    cout << "inittime::" << (double)(initfinish-initstart)/CLOCKS_PER_SEC << endl;
    
    while(1) {
        srcstart = clock();
        if (!cap.read(src)) {
            cap.open("rtmp://192.168.0.123/live/usb2");
            printf ("connection failed");
        } else {
            srcfinish = clock();
            cout << "srctime::" << (double)(srcfinish-srcstart)/CLOCKS_PER_SEC << endl;
            start = clock();
            id = calculation(id,r);
            finish = clock();
            cout <<"quickflow time::" << (double)(finish-start)/CLOCKS_PER_SEC << endl;
            cout << "\n" << endl;
        }

//        cv::waitKey(1);
   }
}
