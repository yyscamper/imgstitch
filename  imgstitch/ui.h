#ifndef _IMAGE_STITCH_UI_H_
#define _IMAGE_STITCH_UI_H_

#include "cv.h"
#include "highgui.h"
#include "util.h"

//将若干点绘制到图像上
IplImage* PlotPoints(CvPoint* point, int numOfPoints, IplImage *image, int type);
#define PLOT_TYPE_DOT		0
#define PLOT_TYPE_CIRCLE    1
#define PLOT_TYPE_RECT		2

//信息窗口显示
void ShowMsgWnd(char* title, char* msg);

//人工选择两幅图像的匹配点
void ManualSelectMatchPoints(IplImage* image1, IplImage* image2);

//获取匹配点信息
int GetSelectedMatchPoints(CvPoint** point1, CvPoint** point2);

//通过人工选点进行图像拼接
void ManulStitch(IplImage* image1, IplImage* image2);

void PlotMatchLine(IplImage* image, CvPoint* pt1, int num1, CvPoint* pt2, int num2);

IplImage* PlotMatchImage(IplImage* image1, IplImage* image2, CvPoint* pt1, CvPoint* pt2, int numOfPoints);
#endif