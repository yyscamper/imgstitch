
#ifndef _IMAGE_STITCH_UTIL_H_
#define _IMAGE_STITCH_UTIL_H_

#include <cv.h>

CvPoint*  ExtractPos(CvSeq* pDescSeq);
void PrintMat(CvMat* m);
void SetMatVal(CvMat* m, double* val);

float GetPixelVal(IplImage* image, int x, int y, int c);
void SetPixelVal(IplImage* image, int x, int y, int c, float val);
void CopyPixelVal(IplImage* dstImage, int dstX, int dstY, IplImage* srcImage, int srcX, int srcY);
IplImage* GetFloatImage(IplImage* srcImage);
bool IsPointInRect(CvRect* rect, int x, int y);

#endif