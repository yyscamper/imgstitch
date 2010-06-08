
#include "util.h"
#include "highgui.h"

CvPoint*  ExtractPos(CvSeq* seq)
{
	CvPoint* pt = (CvPoint*)calloc(seq->total, sizeof(CvPoint));
	CvMat* pElem = NULL;
	double zoomVal = 0.0;
	for(int i=0; i < seq->total; i++){
		pElem = (CvMat*)cvGetSeqElem(seq, i);
		pt[i].x = cvRound(cvmGet(pElem, 0, 0) * zoomVal);
		pt[i].y = cvRound(cvmGet(pElem, 0 ,1) * zoomVal);
	}
	return pt;
}

void PrintMat(CvMat* m)
{	
	printf("\n");
	for(int r = 0; r < m->rows; r++){
		printf("\n");
		for(int c = 0; c < m->cols; c++){
			printf("%3.4f\t", cvmGet(m, r, c));
		}
	}
	printf("\n");
}
void SetMatVal(CvMat* m, double* val)
{
	for(int r = 0; r < m->rows; r++){
		for(int c = 0; c < m->cols; c++){
			cvmSet(m, r, c, val[r * m->rows + c]);
		}
	}
}

float GetPixelValF32(IplImage* image, int x, int y, int c)
{
	return ((float*)(image->imageData + y * image->widthStep))[x * image->nChannels + c];
}

void SetPixelValF32(IplImage* image, int x, int y, int c, float val)
{
	((float*)(image->imageData + y * image->widthStep))[x * image->nChannels + c] = val;
}

unsigned char GetPixelValU8(IplImage* image, int x, int y, int c)
{
	return ((unsigned char*)(image->imageData + y * image->widthStep))[x * image->nChannels + c];
}

void SetPixelValU8(IplImage* image, int x, int y, int c, unsigned char val)
{
	((unsigned*)(image->imageData + y * image->widthStep))[x * image->nChannels + c] = val;
}

void CopyPixelValF32(IplImage* dstImage, int dstX, int dstY, IplImage* srcImage, int srcX, int srcY)
{
	assert(srcImage->nChannels == dstImage->nChannels && srcImage->depth == IPL_DEPTH_32F && dstImage->depth == IPL_DEPTH_32F);
	for(int c = 0; c < srcImage->nChannels; c++){
		//printf("\nBefore Copy: %f", ((float*)(dstImage->imageData + dstY * dstImage->widthStep))[dstX * dstImage->nChannels + c] );
		((float*)(dstImage->imageData + dstY * dstImage->widthStep))[dstX * dstImage->nChannels + c] 
			= ((float*)(srcImage->imageData + srcY * srcImage->widthStep))[srcX * srcImage->nChannels + c];
		//printf("\nAfter Copy:%f", ((float*)(dstImage->imageData + dstY * dstImage->widthStep))[dstX * dstImage->nChannels + c]);
	}
}

IplImage* GetFloatImage(IplImage* srcImage)
{
	IplImage* dstImage = cvCreateImage(cvGetSize(srcImage), IPL_DEPTH_32F ,srcImage->nChannels);
	if(srcImage->depth == IPL_DEPTH_8U){
		cvConvertScale(srcImage, dstImage, 1/255.0);
	}else if(srcImage->depth == IPL_DEPTH_32F){
		cvCopyImage(srcImage, dstImage);
	}else{
		printf("\nIn GetFloatImage(), the depth of input image is not IPL_DEPTH_8U!");
		return NULL;
	}
	return dstImage;	
}

bool IsPointInRect(CvRect* rect, int x, int y)
{
	if(rect == NULL){
		return false;
	}
	return (x >= rect->x && x < rect->x + rect->width && y >= rect->y && y < rect->y + rect->height);
}

void ShowMatImage(CvMat* mat)
{
	IplImage *img;
	CvSize imgSize = cvSize(mat->cols, mat->rows);
	if(mat->type == CV_8UC1){
		img = cvCreateImageHeader(imgSize, IPL_DEPTH_8U, 1);
	}else if(cvGetElemType(mat) == CV_32FC1){
		img = cvCreateImageHeader(imgSize, IPL_DEPTH_32F, 1);
	}else if(cvGetElemType(mat) == CV_8UC3){
		img = cvCreateImageHeader(imgSize, IPL_DEPTH_8U, 3);
	}else if(cvGetElemType(mat) == CV_32FC3){
		img = cvCreateImageHeader(imgSize, IPL_DEPTH_32F, 3);
	}
	cvGetImage(mat, img);
	cvNamedWindow("Image Show");
	cvShowImage("Image Show", img);
	cvWaitKey(0);
	cvDestroyWindow("Image Show");
	cvReleaseImageHeader(&img);
}


