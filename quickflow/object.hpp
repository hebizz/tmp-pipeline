#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/features2d.hpp>

#include <vector>

#ifndef __OBJECT_HPP_
#define __OBJECT_HPP_

using namespace cv;
using namespace std;

struct recBarcodeResInfo {
    int MainIndex;            //条码所属的主ID
    int PairOk;
    int ID;                    //条码的子轮廓编号
    Point XY;                //条码的子轮廓中心坐标
    float Angle;            //条码 起始点-每个子轮廓 的角度
};
    
struct recBarcodePairInfo {
    recBarcodeResInfo A;
    recBarcodeResInfo B;
};

//绘制轮廓图
struct recBarcodeGroupInfo {
    int Index;                //条码的子轮廓编号
    Point CenterXY;            //条码的子轮廓中心坐标
    float Dist;                //条码 起始点-每个子轮廓 的距离
    float Angle;            //条码 起始点-每个子轮廓 的角度
};

struct recBarcodeInfo {
    int        Index;            //条码的轮廓编号
    Point    CenterXY;            //条码轮廓的中心坐标
    
    RotatedRect minRect;
    float rectRadio;
    
    int StartIndex;        //条码的起始点
    Point StartXY;        //条码的起始点坐标
    
    int SecIndex;        //条码的起始第二点坐标
    Point SecXY;        //条码的起始第二点坐标
    
    int      EndIndex;            //条码的终止点坐标
    Point EndXY;            //条码的终止点坐标
    
    float StarEndDist;    //条码的 起始点-终止点 的距离
    float StarEndAngle;    //条码的 起始点-终止点 的角度
    
    int CheckOk;
    
    vector<recBarcodeGroupInfo> BarcodeGroup;    //子轮廓信息
};

#endif