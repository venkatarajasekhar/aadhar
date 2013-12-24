
    
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <cstdio>
#include <iostream>

using namespace cv;
using namespace std;

static void help()
{
    cout << "\nThis program demonstrates the famous watershed segmentation algorithm in OpenCV: watershed()\n"
            "Usage:\n"
            "./watershed [image_name -- default is fruits.jpg]\n" << endl;


    cout << "Hot keys: \n"
        "\tESC - quit the program\n"
        "\tr - restore the original image\n"
        "\tw or SPACE - run watershed segmentation algorithm\n"
        "\t\t(before running it, *roughly* mark the areas to segment on the image)\n"
        "\t  (before that, roughly outline several markers on the image)\n";
}
Mat markerMask, img;
Point prevPt(-1, -1);

static void onMouse( int event, int x, int y, int flags, void* )
{
    if( x < 0 || x >= img.cols || y < 0 || y >= img.rows )
        return;
    if( event == CV_EVENT_LBUTTONUP || !(flags & CV_EVENT_FLAG_LBUTTON) )
        prevPt = Point(-1,-1);
    else if( event == CV_EVENT_LBUTTONDOWN )
        prevPt = Point(x,y);
    else if( event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON) )
    {
        Point pt(x, y);
        if( prevPt.x < 0 )
            prevPt = pt;
        line( markerMask, prevPt, pt, Scalar::all(255), 5, 8, 0 );
        line( img, prevPt, pt, Scalar::all(255), 5, 8, 0 );
        prevPt = pt;
        imshow("image", img);
    }
}

int main( int argc, char** argv )
{
    char* filename = argc >= 2 ? argv[1] : (char*)"fruits.jpg";
    
    Mat img0 = imread(filename, 1), imgGray;

    if( img0.empty() )
    {
        cout << "Couldn'g open image " << filename << ". Usage: watershed <image_name>\n";
        return 0;
    }
    help();
    namedWindow( "image", 1 );
    img0.copyTo(img);
    cvtColor(img, markerMask, CV_BGR2GRAY);
    cvtColor(markerMask, imgGray, CV_GRAY2BGR);
    markerMask = Scalar::all(0);
   
     imshow( "image", img );
    waitKey(0); 
    setMouseCallback( "image", onMouse, 0 );
    
    int automatic=0;
    
    line( markerMask, cvPoint(3,3), cvPoint(3,4*img.rows/5), Scalar::all(255), 15, 8, 0 );
    line( markerMask, cvPoint(img.cols/2,img.rows/4), cvPoint(img.cols/2,img.rows/2), Scalar::all(255), 5, 8, 0 );
    line( markerMask, cvPoint(img.cols-3,3), cvPoint(img.cols-3,4*img.rows/5), Scalar::all(255), 15, 8, 0 );
    
    #ifdef MARKERSHOW
        imshow("markermask",markerMask);
        waitKey(0);
    #endif    
    
    
    for(;;)
    {
      
        int c = waitKey(0);
        
        if(automatic==0)
        {
            c='w';
            automatic++;
        }
        
        if( (char)c == 27 )
            break;

        
        if( (char)c == 'r' )
        {
            markerMask = Scalar::all(0);
            img0.copyTo(img);
            imshow( "image", img );
        }
        if( (char)c == 'w' || (char)c == ' ' )
        {
            int i, j, compCount = 0;
            vector<vector<Point> > contours;
            vector<Vec4i> hierarchy;

            findContours(markerMask, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

            if( contours.empty() )
                continue;
            Mat markers(markerMask.size(), CV_32S);
            Mat markers2(markerMask.size(), CV_32S);
            markers = Scalar::all(0);
            int idx = 0;
            for( ; idx >= 0; idx = hierarchy[idx][0], compCount++ )
                drawContours(markers, contours, idx, Scalar::all(compCount+1), -1, 8, hierarchy, INT_MAX);

            if( compCount == 0 )
                continue;

            vector<Vec3b> colorTab;
            for( i = 0; i < compCount; i++ )
            {
                int b = theRNG().uniform(0, 255);
                int g = theRNG().uniform(0, 255);
                int r = theRNG().uniform(0, 255);

                colorTab.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
            }

            double t = (double)getTickCount();
            watershed( img0, markers );
            
            t = (double)getTickCount() - t;
            printf( "execution time = %gms\n", t*1000./getTickFrequency() );
            markers.copyTo(markers2);
			
			Mat wshed(markers.size(), CV_8UC3);
			
	   		int pass=5,x1=0;
      		int sum1_j=0,sum2_j=0,sum3_j=0;
      		int sum1_i=0,sum2_i=0,sum3_i=0;
      		int area1=0,area2=0,area3=0;
			
			for( i = 0; i < markers.rows; i++ )
            {
            	int contCount=0;
		        for( j = 0; j < markers.cols; j++ )
		        {
		        	int index = markers.at<int>(i,j);
		    
		            if( index == -1 )
		            {
		            	contCount++;
		            	if(i==markers.rows/3)
		            	{
		            		printf("Found Contour %d at x=%d\n",contCount,j);
		            	   	if(contCount==2)
		            		{
		            			printf("Found Contour2--%d\n",j);
		            			x1=j;
		            			j+=10;
		            			pass=4;
		            		}
		            		else if(contCount==3)
		            		{
		            			printf("Found Contour3---%d\n",j);
		            			x1=(j+x1)/2;
		            			pass=5;
		            		}
		            	}
		            }
		            else if (index==1)
		            {
		            	sum1_j+=j;
		            	area1+=1;
		            	sum1_i+=i;
		            }
		            else if (index==2)
		            {
		            	sum2_j+=j;
		            	area2+=1;
		            	sum2_i+=i;
		            }
		            else if (index==3)
		            {
		            	sum3_j+=j;
		             	area3+=1;
		           		sum3_i+=i;
		            }
		    	}
			}            
			printf("\nPicture Size=(%d,%d)\n",markers.rows,markers.cols);
			printf("Contour 1: \n Area=%d\nCenter=(%d,%d)\n",area1,sum1_j/area1,sum1_i/area1);
			printf("Contour 2: \n Area=%d\nCenter=(%d,%d)\n",area2,sum2_j/area2,sum2_i/area2);
			printf("Contour 3: \n Area=%d\nCenter=(%d,%d)\n",area3,sum3_j/area3,sum3_i/area3);
			int xc=sum1_j/area1,yc=sum1_i/area1;
			//int xdiff=pc-xc,ydiff=pc-yc;
			
            for( i = 0; i < markers.rows; i++ )
            {
		        for( j = 0; j < markers.cols; j++ )
		        {
		            int index = markers.at<int>(i,j);
		            if( index == -1 )
		            {
		                wshed.at<Vec3b>(i,j) = Vec3b(255,255,255);
		            }
		            else if( index <= 0 || index > compCount )
		            {
		                wshed.at<Vec3b>(i,j) = Vec3b(0,0,0);
		            }
		            else
		            {
						
		            	if(j>x1-4 && j<x1+4 && i>markers.rows/3-4 && i<markers.rows/3+4)
							wshed.at<Vec3b>(i,j) =Vec3b(255,0,0);              
						else if(j>xc-4 && j<xc+4 && i>yc-4 && i<yc+4)
							wshed.at<Vec3b>(i,j) =Vec3b(0,0,255);              	      		
		                else if(j>markers.cols/2-4 && j<markers.cols/2+4)
							wshed.at<Vec3b>(i,j) =Vec3b(0,255,0);              
		                else
		                    wshed.at<Vec3b>(i,j) = colorTab[index - 1];
		        		      		
		        	}
		    	}
      		}
      		
      		cvNamedWindow("image",CV_WINDOW_NORMAL);
      		wshed = wshed*0.5 + imgGray*0.5;
            imshow( "image", wshed );
            c=cvWaitKey(0);
            if(c=='r'||c=='w'||c=='m')
                {   
                    
                    continue;
                }
            
      		
      		int xdiff=markers.cols/2-x1;
      		int ydiff=markers.rows/2-yc;
            IplImage* moved= new IplImage(img0);
            xdiff*=2;
            ydiff/=3;
            
            if(xdiff>0&& ydiff>0)
            {
            
                cvSetImageROI(moved,cvRect(0,0,img0.cols-xdiff,img0.rows-ydiff));
                
                
                
            
            
            }
            else if(xdiff>0&& ydiff<0)
            {
                ydiff*=-1;
            
                cvSetImageROI(moved,cvRect(0,ydiff,img0.cols-xdiff,img0.rows-ydiff));
                
                
                 
            
            
            
            }
            else if(xdiff<0&&ydiff>0)
            {
                xdiff*=-1;
                cvSetImageROI(moved,cvRect(xdiff,0,img0.cols-xdiff,img0.rows-ydiff));
                
                
                 
            
            
            }
            else if(xdiff<0&&ydiff<0)
            {
                ydiff*=-1;
                xdiff*=-1;
                cvSetImageROI(moved,cvRect(xdiff,ydiff,img0.cols-xdiff,img0.rows-ydiff));
                
                
                 
            
            
            }
            
            cout<<"Moved by"<<xdiff<<','<<ydiff;
           
            cvNamedWindow("O->okay,R->redo",CV_WINDOW_NORMAL);
            cvShowImage("O->okay,R->redo",moved);
            c=waitKey(0);
                   
            if(c=='r')
                {
                    cvDestroyWindow("O->okay,R->redo");
                    cvDestroyWindow("Press R to remark");
                    markerMask = Scalar::all(0);
                    continue;
                 }
                
          
            
            cvSaveImage("Moved.jpg",moved);
            cvResetImageROI(moved);
            break;
            
            
            
            //printf("The x -coordinate of the center is %d.\n",x1);
            
        }
    }

    return 0;
}




