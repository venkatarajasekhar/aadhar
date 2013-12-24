#include<cv.h>
#include<stdio.h>
#include<highgui.h>
#define CV_EVENT_MOUSEMOVE      0
#define CV_EVENT_LBUTTONDOWN    1
#define CV_EVENT_RBUTTONDOWN    2
#define CV_EVENT_MBUTTONDOWN    3
#define CV_EVENT_LBUTTONUP      4
#define CV_EVENT_RBUTTONUP      5
#define CV_EVENT_MBUTTONUP      6
#define CV_EVENT_LBUTTONDBLCLK  7
#define CV_EVENT_RBUTTONDBLCLK  8
#define CV_EVENT_MBUTTONDBLCLK  9

#define CV_EVENT_FLAG_LBUTTON   1
#define CV_EVENT_FLAG_RBUTTON   2
#define CV_EVENT_FLAG_MBUTTON   4
#define CV_EVENT_FLAG_CTRLKEY   8
#define CV_EVENT_FLAG_SHIFTKEY  16
#define CV_EVENT_FLAG_ALTKEY    32
int g,h,l,p,flag=0;
CvConnectedComp comp;
CvPoint seed;
CvScalar color=CV_RGB(250,0,0);
CvSeq* contour=NULL;
//CV_EXTERN_C_FUNCPTR( void (*CvMouseCallback )(int event, int x, int y, int
//flags, void* param) );
void my_mouse_callback(int event,int x,int y,int flags,void* param);
void flood(IplImage* img);
int filld(const char photo[])
{
int f;
IplImage *img,*src;
img=cvLoadImage(photo);
//CvCapture* capture=cvCaptureFromCAM(0);
cvNamedWindow("floodfill",CV_WINDOW_AUTOSIZE);
//img=cvLoadImage("img.jpg");
//cvCvtColor(src,img,CV_RGB2GRAY);
//CvMouseCallback(7,x,y,16,NULL);

cvSetMouseCallback("floodfill",my_mouse_callback,(void*)img);
while(1)
{
	//img=cvQueryFrame(capture);
	//printf
	if(l==4 )
	{
		//printf("Testing.\n");
		flag=1;
		seed=cvPoint(g,h);
		cvFloodFill(img,seed,color,cvScalarAll(50.0),cvScalarAll(10.0),&comp,
CV_FLOODFILL_FIXED_RANGE,NULL);
		printf("ab %d  %d\n",g,h);
	}
	if(flag==1)
	{
		cvFloodFill(img,seed,color,cvScalarAll(50.0),cvScalarAll(10.0),&comp,
		CV_FLOODFILL_FIXED_RANGE,NULL);
		printf("countour total %f %f \n",comp.area,comp.value.val[1]);
		//contour=comp->contour;

//cvDrawContours(img,comp.contour,cvScalarAll(255),cvScalarAll(0),-1,1,8,
//int (0,0));
	}
	cvSaveImage("gen.jpg",img);
	cvShowImage("floodfill",img);
	cvWaitKey(33);
	if(cvWaitKey(33)==27)
	break;

}
cvWaitKey();

cvReleaseImage(&img);

}
void my_mouse_callback(int event,int x,int y,int flags,void* param)
{
l=event;
p++;
if(event==7)
{
	printf("Testing.\n");
	g=x;
	h=y;
	printf("%d  %d\n",x,y);
}
}
void flood(IplImage *img)
{
CvPoint seed=cvPoint(g,h);
CvScalar color=CV_RGB(250,0,0);
cvFloodFill(img,seed,color,cvScalarAll(200.0),cvScalarAll(200.0),NULL,
CV_FLOODFILL_FIXED_RANGE,NULL);
printf("ab %d  %d\n",g,h);

}
