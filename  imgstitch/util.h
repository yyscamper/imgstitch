
#ifndef _IMAGE_STITCH_UTIL_H_
#define _IMAGE_STITCH_UTIL_H_

#include <cv.h>

CvPoint*  ExtractPos(CvSeq* pDescSeq);
void PrintMat(CvMat* m);
void SetMatVal(CvMat* m, double* val);

unsigned char GetPixelValU8(IplImage* image, int x, int y, int c);
void SetPixelValU8(IplImage* image, int x, int y, int c, unsigned char val);

float GetPixelValF32(IplImage* image, int x, int y, int c);
void SetPixelValF32(IplImage* image, int x, int y, int c, float val);
void CopyPixelValF32(IplImage* dstImage, int dstX, int dstY, IplImage* srcImage, int srcX, int srcY);
IplImage* GetFloatImage(IplImage* srcImage);
bool IsPointInRect(CvRect* rect, int x, int y);

void ShowMatImage(CvMat* mat);
#endif