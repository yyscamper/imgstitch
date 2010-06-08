
#include "ui.h"
#include <windows.h>
#include "match.h"
#include "stitch.h"

#define NUM_POINTS 6
IplImage* g_image = NULL;
CvPoint g_point1[NUM_POINTS];
CvPoint g_point2[NUM_POINTS];
int g_maxNumOfPoints= NUM_POINTS;
int g_minNumOfPoints = NUM_POINTS;
int g_ptr1 = 0;
int g_ptr2 = 0;
CvRect g_rect[2];
void MouseHandler(int event, int x, int y, int flags, void* param);

IplImage* PlotPoints(CvPoint* point, int numOfPoints, IplImage *image, int type)
{	
	IplImage* img = cvCreateImage(cvGetSize(image), image->depth, image->nChannels);
	img = cvCloneImage(image);
	for(int i= 0; i < numOfPoints; i++){	
		switch(type){
			case PLOT_TYPE_DOT:
				cvCircle(img, point[i], 4, CV_RGB(255, 0, 0), -1, 8, 0);
				break;
			case PLOT_TYPE_CIRCLE:
				cvCircle(img, point[i], 4, CV_RGB(255, 0, 0), 1, 8, 0);
				break;
			case PLOT_TYPE_RECT:
				cvRectangle(img, cvPoint(point[i].x - 2, point[i].y - 2), 
					cvPoint(point[i].x + 2, point[i].y + 2), 
					CV_RGB(255, 0, 0), 1, 8, 0);
				break;
			default:
				break;
		}
	}
	return img;
}

void ShowMsgWnd(char* title, char* msg)
{
	if(title == NULL){
		title = "Message Window";
	}
	/**
	 *  当hScale = 0.5, vScale = 0.5 时，每个字的宽度约为10像素
	 */
	IplImage* image = cvCreateImage(cvSize(400, 50), IPL_DEPTH_8U, 3);
	cvSetZero(image);

	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5, 0, 1, 8);
	//int fontWidth = 10;
	//int fontHeight = 20;

	//int numOfLines = image/strlen(msg) * fontWidth;
	cvPutText(image, msg, cvPoint(10,30), &font, CV_RGB(255, 0, 0));
	cvNamedWindow(title);
	cvShowImage(title, image);
}

void ManualSelectMatchPoints(IplImage* image1, IplImage* image2)
{
	CvSize imageSize;
	int gap = 30;
	imageSize.width = image1->width + image2->width + gap;
	imageSize.height = max(image1->height, image2->height);

	g_rect[0].x = 0;
	g_rect[0].y = (imageSize.height - image1->height) / 2;
	g_rect[0].width = image1->width;
	g_rect[0].height = image1->height;

	g_rect[1].x = image1->width + gap;
	g_rect[1].y = (imageSize.height - image2->height) / 2;
	g_rect[1].width = image2->width;
	g_rect[1].height = image2->height;

	g_image = cvCreateImage(imageSize, image1->depth, max(image1->nChannels, image2->nChannels));
	cvSetZero(g_image);
	cvSetImageROI(g_image, g_rect[0]);
	cvCopyImage(image1, g_image);
	cvResetImageROI(g_image);

	cvSetImageROI(g_image, g_rect[1]);
	cvCopyImage(image2, g_image);
	cvResetImageROI(g_image);
	
	g_ptr1 = 0;
	g_ptr2 = 0;
	char* wndName = "Select 4 matching points:";
	cvNamedWindow(wndName);
	cvShowImage(wndName, g_image);
	cvSetMouseCallback(wndName, MouseHandler, &g_rect);
	cvWaitKey(0);
}

bool IsPointSelected(CvPoint thePoint, CvPoint* plotPool, int numOfPoinsInPool, int* dstIndex)
{
	for(int i=0; i<numOfPoinsInPool; i++){
		if ((thePoint.x - plotPool[i].x) * (thePoint.x - plotPool[i].x) 
			+ (thePoint.y - plotPool[i].y) * (thePoint.y - plotPool[i].y)
			<= 16){
				*dstIndex = i;
				return true;
		}
	}
	return false;
}

void PlotMatchLine(IplImage* image, CvPoint* pt1, int num1, CvPoint* pt2, int num2)
{
	int radius = 2;
	if(image == NULL || pt1 == NULL || pt2 == NULL || (num1 == 0 && num2 == 0)){
		return;
	}
	for(int i=0; i<num1; i++){
		cvCircle(image, pt1[i], radius, CV_RGB(255, 0, 0), -1, 8, 0);
	}
	for(int i=0; i<num2; i++){
		cvCircle(image, pt2[i], radius, CV_RGB(255, 0, 0), -1, 8, 0);
	}
	for(int i=0; i < min(num1, num2); i++){
		cvLine(image, pt1[i], pt2[i], CV_RGB(255, 0, 0), 1, 8, 0);
	}
}

void SwapPoint(CvPoint* pt1, CvPoint* pt2)
{
	CvPoint temp;
	temp.x = pt1->x;
	temp.y = pt1->y;
	pt1->x = pt2->x;
	pt1->y = pt2->y;
	pt2->x = temp.x;
	pt2->y = temp.y;
}

void MouseHandler(int event, int x, int y, int flags, void* param)
{
	CvRect* rect = (CvRect*)param;
	static IplImage* cpyImage = NULL;
	int theDeletedIndex = 0;
	
	if(cpyImage != NULL){
		cvReleaseImage(&cpyImage);
	}

	switch(event){
		case CV_EVENT_LBUTTONDOWN:
			if(IsPointInRect(rect, x, y)){
				if(g_ptr1 < g_maxNumOfPoints && g_ptr1 <= g_ptr2){
					g_point1[g_ptr1].x = x;
					g_point1[g_ptr1].y = y;
					g_ptr1++;
				}
			}else if(IsPointInRect(rect + 1, x, y)){
				if(g_ptr2 < g_maxNumOfPoints && g_ptr2 <= g_ptr1){
					g_point2[g_ptr2].x = x;
					g_point2[g_ptr2].y = y;
					g_ptr2++;
				}
			}
			break;
		case CV_EVENT_RBUTTONDOWN:
			
			if (IsPointSelected(cvPoint(x, y), g_point1, g_ptr1, &theDeletedIndex)){
				if(g_ptr1 < g_ptr2){
					break;
				}
				g_point1[theDeletedIndex] = g_point1[g_ptr1 - 1];
				if(theDeletedIndex < g_ptr2){
					SwapPoint(g_point2 + theDeletedIndex, g_point2 + g_ptr2 - 1);
				}
				if(--g_ptr1 < 0){
					g_ptr1 = 0;
				}
			}else if( IsPointSelected(cvPoint(x, y), g_point2, g_ptr2, &theDeletedIndex)){
				if(g_ptr2 < g_ptr1){
					break;
				}
				g_point2[theDeletedIndex] = g_point2[g_ptr2 - 1];
				if(theDeletedIndex < g_ptr1){
					SwapPoint(g_point1 + theDeletedIndex, g_point1 + g_ptr1 - 1);
				}
				
				if(--g_ptr2 < 0){
					g_ptr2 = 0;
				}
			}
			break;
		default:
			break;
	}
	
	/*printf("\n\nThe Selected Points: \n");
	printf("Left:  ");
	for(int i=0; i < g_ptr1; i++){
		printf("(%3d, %3d) ", g_point1[i].x, g_point1[i].y);
	}
	printf("\nRight: ");
	for(int i=0; i < g_ptr2; i++){
		printf("(%3d, %3d) ", g_point2[i].x, g_point2[i].y);
	}*/

	//Only for debug////////////////////////////////////////////
	//g_point1[0].x = 91;
	//g_point1[0].y = 98;
	//g_point1[1].x = 69;
	//g_point1[1].y = 99;
	//g_point1[2].x = 107;
	//g_point1[2].y = 185;
	//g_point1[3].x = 200;
	//g_point1[3].y = 258;

	//g_point2[0].x = 308;
	//g_point2[0].y = 80;
	//g_point2[1].x = 289;
	//g_point2[1].y = 70;
	//g_point2[2].x = 280;
	//g_point2[2].y = 163;
	//g_point2[3].x = 323;
	//g_point2[3].y = 272;

	//g_ptr1 = 4;
	//g_ptr2 = 4;
	///////////////////////////////////////////////////////////////////

	char* wndName = "Select 4 matching points:";
	cvNamedWindow(wndName);
	cpyImage = cvCloneImage(g_image);
	PlotMatchLine(cpyImage, g_point1, g_ptr1, g_point2, g_ptr2);
	cvShowImage(wndName, cpyImage);
}

int GetSelectedMatchPoints(CvPoint** point1, CvPoint** point2)
{
	int num = min(g_ptr1, g_ptr2);
	if(num < g_minNumOfPoints){
		//ShowMsgWnd("Notice", "Do not have enough match points!");
		*point1 = NULL;
		*point2 = NULL;
		return 0;
	}
	CvPoint *p1 = NULL, *p2 = NULL;
	p1 = (CvPoint*)calloc(num, sizeof(CvPoint));
	p2 = (CvPoint*)calloc(num, sizeof(CvPoint));
	for(int i=0; i<num; i++){
		memcpy(p1 + i, g_point1 + i, sizeof(CvPoint));
		memcpy(p2 + i, g_point2 + i, sizeof(CvPoint));

		p1[i].x -= g_rect[0].x;
		p1[i].y -= g_rect[0].y;
		p2[i].x -= g_rect[1].x;
		p2[i].y -= g_rect[1].y;
	}
	*point1 = p1;
	*point2 = p2;
	return num;
}

DWORD WINAPI ManualStitchThread(LPVOID param)
{
	IplImage* image1 = ((IplImage**)param)[0];
	IplImage* image2 = ((IplImage**)param)[1];

	CvPoint *point1 = NULL, *point2 = NULL;
	int numOfPoints;
	int sleepCnt = 0;
	while(1){
		numOfPoints = GetSelectedMatchPoints(&point1, &point2);
		if(numOfPoints > 0){
			break;
		}
		int key = cvWaitKey(100);
		if(key == 27){
			return 0;
		}else{
			//printf("\nSleep %d times", sleepCnt++);
			//Sleep(10000);
		}
	}

	printf("\n\nThe Selected Points: \n");
	printf("Left:  ");
	for(int i=0; i < g_ptr1; i++){
		printf("(%3d, %3d) ", g_point1[i].x - g_rect[0].x, g_point1[i].y - g_rect[0].y);
	}
	printf("\nRight: ");
	for(int i=0; i < g_ptr2; i++){
		printf("(%3d, %3d) ", g_point2[i].x - g_rect[1].x, g_point2[i].y - g_rect[1].y);
	}
	
	printf("\nMapMat");
	CvMat* mapMat = GetMapMat(point2, point1, numOfPoints);
	PrintMat(mapMat);
	CvMat* invMap1 = GetMapMat(point1, point2, numOfPoints);
	printf("\nInvMap Using Points");
	PrintMat(invMap1);
	CvMat* invMap2 = GetInvMapMat(mapMat);
	printf("\nInvMapMat Using GeInvMapMat");
	PrintMat(invMap2);

	IplImage* dstImage = StitchTwoImages(image1, image2, mapMat);
	cvNamedWindow("Stitch Image");
	cvShowImage("Stitch Image", dstImage);

	cvWaitKey(0);
	cvDestroyAllWindows();
	cvReleaseImage(&image1);
	cvReleaseImage(&image2);
	cvReleaseImage(&dstImage);
	cvReleaseMat(&mapMat);
	free(point1);
	free(point2);	
	return 0;
}

void ManulStitch(IplImage* image1, IplImage* image2)
{
	DWORD threadID;
	IplImage* imageArr[2];
	imageArr[0] = image1;
	imageArr[1] = image2;

	HANDLE hThread = CreateThread(NULL, 0, ManualStitchThread, imageArr, 0, &threadID);
	ManualSelectMatchPoints(image1, image2);
	//等待线程结束。
	WaitForSingleObject(hThread, INFINITE);
	//删除的线程资源。
	CloseHandle(hThread);
}
