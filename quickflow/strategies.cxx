#include "strategies.hpp"
#include "string"
#include <hiredis/hiredis.h>
//using namespace std;

void calcDataReset(void){

    vecBarcodeRes.clear();
    vecBarcode.clear();
    vecBarcodeGroup.clear();

    contours.clear();
    hierarchy.clear();

}

void findBarcodeLeftAspect() {
    cout << "vecBarcodesize::" << vecBarcode.size() << endl;
    for (int i = 0; i < vecBarcode.size(); ++i) {
        float distMax = 0;
        int distMaxIndex = 0;
        for (int j = 0; j < vecBarcode[i].BarcodeGroup.size(); ++j) {
            float dist = calcDistance(vecBarcode[i].CenterXY, vecBarcode[i].BarcodeGroup[j].CenterXY);//计算每个点到条码中心的坐标。
            if (dist > distMax) {
                distMax = dist;
                distMaxIndex = j;
            }
        }
        vecBarcode[i].StartIndex = vecBarcode[i].BarcodeGroup[distMaxIndex].Index;//起始点编号
        vecBarcode[i].StartXY = vecBarcode[i].BarcodeGroup[distMaxIndex].CenterXY;//起始点中心点坐标
    }
}

void findBarcodeRightAspect() {
    for (int i = 0; i < vecBarcode.size(); ++i) {
        float DistMax = 0;
        float DistMin = 9999;
        int distMaxIndex = 0;
        int distMinIndex = 0;
        float AngleBase = 0;
        
        for (int j = 0; j < vecBarcode[i].BarcodeGroup.size(); ++j) { //遍历小点
            float dist = calcDistance(vecBarcode[i].StartXY, vecBarcode[i].BarcodeGroup[j].CenterXY);
            float angle = calcAngle(vecBarcode[i].StartXY, vecBarcode[i].BarcodeGroup[j].CenterXY);
            if (dist > DistMax) {
                DistMax = dist;
                distMaxIndex = j;
                
                AngleBase = angle;
            }
            if (dist < DistMin && dist> 0.5) {
                DistMin = dist;
                distMinIndex = j;
            }
            vecBarcode[i].BarcodeGroup[j].Angle = angle;
            vecBarcode[i].BarcodeGroup[j].Dist = dist;
        }
        
        vecBarcode[i].SecIndex = distMinIndex;
        vecBarcode[i].SecXY = vecBarcode[i].BarcodeGroup[distMinIndex].CenterXY;
        
        vecBarcode[i].EndIndex = distMaxIndex;
        vecBarcode[i].EndXY = vecBarcode[i].BarcodeGroup[distMaxIndex].CenterXY;
        
        vecBarcode[i].StarEndDist = DistMax;
        vecBarcode[i].StarEndAngle = AngleBase;
        
        float rectHeight = MIN(vecBarcode[i].minRect.size.height, vecBarcode[i].minRect.size.width);
        float rectWeight = MAX(vecBarcode[i].minRect.size.height, vecBarcode[i].minRect.size.width);
        
        float DistRadio = DistMax / DistMin;//策略：起始点 到 第二点和尾部 的比例校验，理论值： 10
        float DistRectRadio = DistMax / rectWeight;//策略:首尾两点间距 与 外轮廓矩形长度比例，理论值：0.8

        
        if (DistRadio > (BARCODE_MAX_BIT + 2)*0.5 //7
            && DistRadio < (BARCODE_MAX_BIT + 2)*1.5//14
            && DistRectRadio > 0.5//0.6
            && DistRectRadio < 1.0)//1.0
        {
            //分别算出 第二点、尾部点 与 矩形窄边界的距离
            Point2f fourPoint2f[4];
            Point2f rectCenterA, rectCenterB;
            vecBarcode[i].minRect.points(fourPoint2f);
            float A = calcDistance(fourPoint2f[0], fourPoint2f[1]);
            float B = calcDistance(fourPoint2f[1], fourPoint2f[2]);
            float C = calcDistance(fourPoint2f[2], fourPoint2f[3]);
            float D = calcDistance(fourPoint2f[3], fourPoint2f[0]);
            if (A < B) {
                rectCenterA.x = 0.5 * (fourPoint2f[0].x + fourPoint2f[1].x);
                rectCenterA.y = 0.5 * (fourPoint2f[0].y + fourPoint2f[1].y);
                rectCenterB.x = 0.5 * (fourPoint2f[2].x + fourPoint2f[3].x);
                rectCenterB.y = 0.5 * (fourPoint2f[2].y + fourPoint2f[3].y);
            }
            else {
                rectCenterA.x = 0.5 * (fourPoint2f[1].x + fourPoint2f[2].x);
                rectCenterA.y = 0.5 * (fourPoint2f[1].y + fourPoint2f[2].y);
                rectCenterB.x = 0.5 * (fourPoint2f[3].x + fourPoint2f[0].x);
                rectCenterB.y = 0.5 * (fourPoint2f[3].y + fourPoint2f[0].y);
            }
            float distSecA = calcDistance(vecBarcode[i].SecXY, rectCenterA);
            float distSecB = calcDistance(vecBarcode[i].SecXY, rectCenterB);
            float distSec = MIN(distSecA, distSecB);
            
            float distEndA = calcDistance(vecBarcode[i].EndXY, rectCenterA);
            float distEndB = calcDistance(vecBarcode[i].EndXY, rectCenterB);
            float distEnd = MIN(distEndA, distEndB);
            
            float SecEndDistRadio = distSec / distEnd;
            
            
            //策略：第二点和尾部点 到 最近矩形边 的比例校验，理论值：1
            if (SecEndDistRadio > 0.5//0.7
                && SecEndDistRadio < 1.3) {
                vecBarcode[i].CheckOk = 1;
                
                float radius;
                Point2f center;
                float rectCmp = MIN(vecBarcode[i].minRect.size.width, vecBarcode[i].minRect.size.height);
                int RadiusErrCnt = 0;
                for (int j = 0; j < vecBarcode[i].BarcodeGroup.size(); ++j)
                    minEnclosingCircle(contours[vecBarcode[i].BarcodeGroup[j].Index], center, radius);
                
                ShowCnt++;

                float StarEndDist = vecBarcode[i].StarEndDist;
                float BaseDist = StarEndDist / (BARCODE_MAX_BIT + 2);//10;
                int  BarcodeNum = 0;
                for (int j = 0; j < vecBarcode[i].BarcodeGroup.size(); ++j) {
                    int Bias = round((vecBarcode[i].BarcodeGroup[j].Dist) / BaseDist);
                    if (Bias >= 2 && Bias <= BARCODE_MAX_BIT + 1)
                        BarcodeNum += 1 << (Bias - 2);
                }
                recBarcodeResInfo recBarcodeRes;

                recBarcodeRes.MainIndex = hierarchy[vecBarcode[i].Index][3];
                recBarcodeRes.PairOk = -1;
                recBarcodeRes.ID = BarcodeNum;
                recBarcodeRes.XY = 0.5*(vecBarcode[i].SecXY + vecBarcode[i].EndXY);
                recBarcodeRes.Angle = vecBarcode[i].StarEndAngle;
                vecBarcodeRes.push_back(recBarcodeRes);
            }
        } else {
            vecBarcode[i].CheckOk = 0;
        }
    }
}

void findHierarchy() {
    cout << "hierarchy size ::" << hierarchy.size() << endl; 
    for (int i = 0; i < hierarchy.size(); i++) {
        if (contours[i].size() > 500)continue;
        if (contours[i].size() < 50)continue;

        vecBarcodeGroup.clear();
        //表示不是最外面的轮廓
        if (hierarchy[i][2] != -1) {
            if (hierarchy[hierarchy[i][2]][2] == -1){

                int Cnt = 0;
                int Index = hierarchy[i][2];//子轮廓编号（里面的小点）
                int CheckMinArea = 1;

                //保存子轮廓的编号到vecBarcodeGroup中
                while (1) {
                    if (hierarchy[Index][2] != -1) {
                        CheckMinArea = -1;    //若还存在子层级，抛弃该轮廓
                        break;
                    }

                    uchar color = src_gray.at<uchar>(contours[Index][0].y, contours[Index][0].x);//获取轮廓点的坐标对应的像素值

                    if (contours[Index].size() > 2 && color > 200) {  //轮廓内像素点数量必须大于10,且为白色
                        ++Cnt;//统计小轮廓的个数
                        recBarcodeGroup.Index = Index;
                        vecBarcodeGroup.push_back(recBarcodeGroup);    //保存子轮廓的编号（里面的信息点）
                        //Scalar color = Scalar(rand() % 255, rand() % 255, rand() % 255);    //随机分配绘图颜色
                        //drawContours(dstImage, contours, Index, color, -1, 8, hierarchy);    //绘制轮廓图
                    }
                    Index = hierarchy[Index][0];//下一个子轮廓
                    if (Index == -1)break;    //检索到最后一个，退出
                }

                if (Cnt <= BARCODE_MAX_BIT + 3 && Cnt >= 3 && CheckMinArea != -1) {  //相同层级的 轮廓 必须在 3~9之间（一条条码的信息点在3-9之间）
                    recBarcode.Index = i;//长条的编号（小点的父轮廓）
                    recBarcode.BarcodeGroup = vecBarcodeGroup;//子轮廓的编号信息

                    // 计算点集的最小外包矩形
                    RotatedRect minRect = minAreaRect(contours[i]);//父轮廓的最小矩形

                    float rectHeight = MIN(minRect.size.height, minRect.size.width);//矩形宽度
                    float rectWeight = MAX(minRect.size.height, minRect.size.width);//矩形长度

                    recBarcode.minRect = minRect;

                    float rectRadio = rectWeight / rectHeight;
                    float RadioMax = 15;
                    float RadioMin = 3;
                    if (rectRadio > RadioMin
                        && rectRadio < RadioMax
                        && rectWeight >=10 && rectWeight < 90
                        && rectHeight >=3 && rectHeight < 50
                            ) {
                       // cout<< "findHer rectWeitht" << rectWeight 
                       //       << "find Her rectHeight" << rectHeight 
	               //      <<endl; 
                        recBarcode.rectRadio = rectRadio;
                        vecBarcode.push_back(recBarcode);//存储父轮廓
                    }
                }
            }
        }
    }

    for (int i = 0; i < vecBarcode.size(); ++i)
    {
        vecBarcode[i].CenterXY = calcCenter(contours[vecBarcode[i].Index]);//计算条码中心坐标
        for (int j = 0; j < vecBarcode[i].BarcodeGroup.size(); ++j)
        {
            vecBarcode[i].BarcodeGroup[j].CenterXY = calcCenter(contours[vecBarcode[i].BarcodeGroup[j].Index]); //计算条码中的每个点的中心坐标
        }
    }
}

int findIdGroup(int id,Redis *r) {
    vector<recBarcodePairInfo> vecBarcodePair;
    recBarcodePairInfo recBarcodePair;
    int UpID, DownID, flag = 1;
    string tablename;
    
    for (int i = 0; i < vecBarcodeRes.size(); ++i) {
        int MainIndex = vecBarcodeRes[i].MainIndex;
        for (int j = 0; j < vecBarcodeRes.size(); ++j) {
            if (i == j)continue;
            if (vecBarcodeRes[j].PairOk == 1)continue;
            if (MainIndex == vecBarcodeRes[j].MainIndex) {
                recBarcodePair.A = vecBarcodeRes[i];
                recBarcodePair.B = vecBarcodeRes[j];

                //两个独立条码平行校验
                if (fabs(fabs(recBarcodePair.A.Angle) - fabs(recBarcodePair.B.Angle)) < 10) {
                    vecBarcodeRes[i].PairOk = 1;
                    vecBarcodeRes[j].PairOk = 1;
                    vecBarcodePair.push_back(recBarcodePair);

                    Point ShowPos = 0.5*(vecBarcodeRes[i].XY + vecBarcodeRes[j].XY);

                    string ShowInfo;
                    string strStartXY = "[" + to_string(ShowPos.x) + ", " + to_string(ShowPos.y) + "]";
                    double StartEndAngle = 0.5*(vecBarcodeRes[i].Angle + vecBarcodeRes[j].Angle);

                    stringstream strStream;
                    strStream.precision(4);
                    strStream << StartEndAngle;
                    std::string strStarEndAngle = strStream.str();

                    float AngleCenter = calcAngle(vecBarcodeRes[i].XY, vecBarcodeRes[j].XY);

                    float AngleBias = StartEndAngle - AngleCenter;
                    if (AngleBias > 180)
                        AngleBias = AngleBias - 360;
                    else if (AngleBias < -180)
                        AngleBias = AngleBias + 360;

                    if (AngleBias>0) {
                        UpID = vecBarcodeRes[i].ID;
                        DownID = vecBarcodeRes[j].ID;
                    } else {
                        UpID = vecBarcodeRes[j].ID;
                        DownID = vecBarcodeRes[i].ID;
                    }

                    cout << "ID:" << UpID << " ; " << DownID << endl;
                    cout << "XY:" << strStartXY << endl;
                    cout << "AG:" << strStarEndAngle << endl;
                    tablename = "hashinfo_"+to_string(id);
                    r->hmset(tablename,"ID",to_string(UpID)+","+to_string(DownID));
                    r->hmset(tablename,"XY",strStartXY);
                    r->hmset(tablename,"ANGLE",strStarEndAngle);
                    flag = 0;
                    cout << "hmset success ::" << tablename << endl;
                    
    if (flag == 0)
    {   
        
        id += 1;
        return id;
    }
    cout << "detector fail" << endl;
    return id;


                    
//                    ShowPos.y += 30;
//                     putText(src, "ID: " + to_string(UpID) + ", " + to_string(DownID), ShowPos, cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 255, 255), 1, 8, 0);
//                     ShowPos.y += 30;
//                     putText(src, "XY: " + strStartXY, ShowPos, cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 255, 255), 1, 8, 0);
//                     ShowPos.y += 30;
//                     putText(src, "Angle: " + strStarEndAngle, ShowPos, cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 255, 255), 1, 8, 0);
                }
            }
        }
   }
}
