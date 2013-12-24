#include<stdio.h>
#include<cv.h>
#include<highgui.h>
int hls(const char photo[])
{

IplImage *img=cvLoadImage(photo);
IplImage *hsv=cvCreateImage(cvGetSize(img),8,3);
IplImage *img1=cvCreateImage(cvGetSize(img),8,3);
IplImage *hsvthresh=cvCreateImage(cvGetSize(img),8,3);
cvCvtColor(img,hsv,CV_BGR2HLS); 
//cvInRangeS(hsv, cvScalar(0, 0, 90, 0), cvScalar(0, 0, 100, 0), hsvthresh);
int x,y,r=0,g=0,b=0,count=0;
for( y=0;y<hsv->height;y++)
{
	uchar *ptr=(uchar*)(hsv-> imageData + y * hsv-> widthStep);
	for( x=0;x<hsv->width;x++)
	{
		if(ptr[3*x+1]>225)
		ptr[3*x+1]-=25;
		else if(ptr[3*x+1]>200)
		ptr[3*x+1]-=22;
		else if(ptr[3*x+1]>175)
		ptr[3*x+1]-=18;
		else if(ptr[3*x+1]>150)
		ptr[3*x+1]-=14;
		else if(ptr[3*x+1]>125)
		ptr[3*x+1]-=10;
		else if(ptr[3*x+1]>100)
		ptr[3*x+1]-=8;
		else if(ptr[3*x+1]>75)
		ptr[3*x+1]-=5;
		else if(ptr[3*x+1]>50)
		ptr[3*x+1]+=5;
	}
}
cvCvtColor(hsv,img1,CV_HLS2BGR);
for( y=0;y<hsv->height;y++)
{
	uchar *ptr1=(uchar*)(img-> imageData + y * img-> widthStep);
	uchar *ptr2=(uchar*)(img1-> imageData + y * img1-> widthStep);
	for( x=0;x<hsv->width;x++)
	{
		if(ptr1[3*x]!=ptr2[3*x])
		r++;
		if(ptr1[3*x+1]!=ptr2[3*x+1])
		g++;
		if(ptr1[3*x+2]!=ptr2[3*x+2])
		b++;

	}
}
printf("\n %d %d\n%d  %d  %d\n",count,hsv->width*hsv->height,r,g,b);
cvShowImage("img",img);
cvShowImage("imgthres",img1);
cvSaveImage("hsl.jpg",img1);
cvWaitKey(0);
}
