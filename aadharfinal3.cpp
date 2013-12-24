
#include "opencv2/objdetect/objdetect.hpp"    
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <cstdio>
#include <iostream>

/* To include the function which applies retinex */

#include"retinexopencv.cpp"
#include"floodfill.cpp"
using namespace cv;
using namespace std;
void blend(Mat a, Mat b);
 int rb[12]={0};
            int gb[12]={0};
            int bb[12]={0};
            Vec3b cf;
            int yr=0,yg=0,yb=0;
            
int face( char *argv[],int argc );

/* Variables for face center and face radius */

CvPoint fc;
int fr;
 int maxr=0,maxg=0,maxb=0;
string cascadeName = "haarcascades/haarcascade_frontalface_alt.xml";
string nestedCascadeName = "haarcascades/haarcascade_eye_tree_eyeglasses.xml";


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

Mat img0,markerMask,markerMask2, img, img2,img1,markerprev,imgprev,imgf;

Point prevPt(-1, -1);

/* onMouse for the second part of the code */
static void onMouse( int event, int x, int y, int flags, void* )
{
    if( x < 0 || x >= img2.cols || y < 0 || y >= img2.rows )
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
        line( markerMask2, prevPt, pt, Scalar::all(255), 5, 8, 0 );
        line( img2, prevPt, pt, Scalar::all(255), 5, 8, 0 );
        prevPt = pt;
        imshow("Image-RB", img2);
    }
}

/* onMouse for the first part of the code */
static void onMouseb( int event, int x, int y, int flags, void* )
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
    img0 = imread(filename, 1);
    Mat imgGray;
    img0.copyTo(img);
    IplImage* moved= new IplImage(img0);
    char check1='n'; 
    
    
    if( face(argv,argc))
    {
            //cout<<"FOUNDIN";
            //cout<<"Center : (" << (int)fc.x << " ," <<(int) fc.y << ") Radius:" <<fr; 
            cvSetImageROI( moved, cvRect(fc.x-2*fr ,fc.y -2*fr , 4*fr,4*fr));
            cvShowImage("o->okay n-not okay",moved);
            cvSaveImage("Moved.jpg",moved);
            check1=cvWaitKey(0);
     }
    else
    {
            retinexopenCV(filename);
            strcpy(argv[1],"n.jpg");
            
            if( face(argv,argc))
            {
                //cout<<"FOUNDIN";
                //cout<<"Center : (" << (int)fc.x << " ," <<(int) fc.y << ") Radius:" <<fr; 
                cvSetImageROI( moved, cvRect(fc.x-2*fr ,fc.y -2*fr , 4*fr,4*fr));
                cvShowImage("o->okay n-not okay",moved);
                cvSaveImage("Moved.jpg",moved);
                check1=cvWaitKey(0);
             }
            
            
    }
            
    
    if (check1!='o')
    {
    
    
        if( img0.empty() )
        {
            cout << "Couldn'g open image " << filename << ". Usage: watershed <image_name>\n";
            return 0;
        }
    
   
    
        namedWindow( "image", 1 );
    
        img0.copyTo(img);
        cvtColor(img, markerMask, CV_BGR2GRAY);
        cvtColor(markerMask, imgGray, CV_GRAY2BGR);
        markerMask = Scalar::all(0);
   
        imshow( "image", img );
        waitKey(0); 
        int c;
        setMouseCallback( "image", onMouseb, 0 );
    
        int automatic=0;
    
        line( markerMask, cvPoint(3,3), cvPoint(3,4*img.rows/5), Scalar::all(255), 15, 8, 0 );
        line( markerMask, cvPoint(img.cols/2,img.rows/4), cvPoint(img.cols/2,img.rows/2), Scalar::all(255), 5, 8, 0 );
        line( markerMask, cvPoint(img.cols-3,3), cvPoint(img.cols-3,4*img.rows/5), Scalar::all(255), 15, 8, 0 );
    
    
        Mat wshed(markerMask.size(), CV_8UC3);
        int firsttime=0;
        for(;;)
        {
        
                
            if(automatic==0)
            {
                c='w';
                automatic++;
            }
        
            if( (char)c == 27 )
                break;
            
            if( (char)c=='m') 
            {      
                markerMask.copyTo(markerprev);
                if(firsttime==0)
                {
                    line( img, cvPoint(3,3), cvPoint(3,4*img.rows/5), Scalar::all(255), 15, 8, 0 );
                    line( img, cvPoint(img.cols/2,img.rows/4), cvPoint(img.cols/2,img.rows/2), Scalar::all(255), 5, 8, 0 );
                    line( img, cvPoint(img.cols-3,3), cvPoint(img.cols-3,4*img.rows/5), Scalar::all(255), 15, 8, 0 );
                    firsttime++;
                }         
                img=  0.5*wshed  + 0.5*img; 
                img.copyTo(imgprev); 
                imshow("image",img);
                c=cvWaitKey(0); 
            }   
            
            if((char)c=='u')
            {
                markerprev.copyTo(markerMask);
                imgprev.copyTo(img);
                imshow("image",img);
                c=cvWaitKey(0); 
  
        
            }
        

        
            if( (char)c == 'r'  )
            {
                markerMask = Scalar::all(0);
                img0.copyTo(img);
                imshow( "image", img );
                c=cvWaitKey(0);
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
                //printf( "execution time = %gms\n", t*1000./getTickFrequency() );
                markers.copyTo(markers2);
			
			
			
	   		    int pass=5,x1=0;
      		    int sum1_j=0,sum2_j=0,sum3_j=0;
      		    int sum1_i=0,sum2_i=0,sum3_i=0;
      		    int area1=0,area2=0,area3=0;
      		    int flg=0;
			
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
		            		    //printf("Found Contour %d at x=%d\n",contCount,j);
		            	   	    if(contCount==2)
		            		    {
		            		    	//printf("Found Contour2--%d\n",j);
		            		    	x1=j;
		            		    	j+=img.cols/20;
		            		    	pass=4;
		            		    }
		            		    else if(contCount==3)
		            		    {
		            		    	//printf("Found Contour3---%d\n",j);
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
			    /*
			    printf("\nPicture Size=(%d,%d)\n",markers.rows,markers.cols);
			    printf("Contour 1: \n Area=%d\nCenter=(%d,%d)\n",area1,sum1_j/area1,sum1_i/area1);
			    printf("Contour 2: \n Area=%d\nCenter=(%d,%d)\n",area2,sum2_j/area2,sum2_i/area2);
			    printf("Contour 3: \n Area=%d\nCenter=(%d,%d)\n",area3,sum3_j/area3,sum3_i/area3);
			    */
			    int xc=sum1_j/area1,yc=sum1_i/area1;
			

			
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
       							wshed.at<Vec3b>(i,j) =Vec3b(0,255,0);              	      		
    		                else if(j>markers.cols/2-4 && j<markers.cols/2+4)
    							wshed.at<Vec3b>(i,j) =Vec3b(0,255,0);              
    		                else
    		                    wshed.at<Vec3b>(i,j) = colorTab[index - 1];
    		        		      		
    		        	}
    		    	}
          		}
      		

          		wshed = wshed*0.5 + imgGray*0.5;
                imshow( "image", wshed );
                c=cvWaitKey(0);
                if((char)c=='r'||(char)c=='w'||(char)c=='m'||(char)c=='u')
                {   

                    continue;
                }
            
      		
          		int xdiff=markers.cols/2-x1;
          		int ydiff=markers.rows/2-yc;
               
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
            
                //cout<<"Moved by"<<xdiff<<','<<ydiff;
           
    
                cvShowImage("image",moved);
                c=waitKey(0);
                    
                if((char)c=='r')
                {
                    markerMask = Scalar::all(0);
                    continue;
                }
                
          
            
                cvSaveImage("Moved.jpg",moved);
                cvResetImageROI(moved);
                break;
            
            
            

            
            }
        }
    }
    
	Mat imgGray2;
	img1=imread("Moved.jpg");
 	

	img1.copyTo(img2);
	img1.copyTo(imgf);
	
	namedWindow("Original cropped");
	imshow("Original cropped",imgf);
	cvWaitKey(0);
	
	
    imshow("Moved",img1);
   
    cvtColor(img2, markerMask2, CV_BGR2GRAY);
    cvtColor(markerMask2, imgGray2, CV_GRAY2BGR);
    markerMask2 = Scalar::all(0);
    Mat wshed2(markerMask2.size(), CV_8UC3);
    imshow( "Image-RB", img2 );
    setMouseCallback( "Image-RB", onMouse, 0 );
    int firsttime2=0;
    for(int count=0;;count++)
    {   
        int radius=img2.cols/3;
        int c=0;
        if(count!=0)
        {
        	 c = waitKey(20);
        }
        if( (char)c == 27 )
            break;

        if( (char)c == 'r' )
        {
            markerMask2 = Scalar::all(0);
            img1.copyTo(img2);
            imshow( "Image-RB", img2 );
        }
        
        if( (char)c=='m') 
        {   
            markerMask2.copyTo(markerprev);
            if(firsttime2==0)
            {
                    line(img2,cvPoint(3*radius/2,3*radius/2-radius/3),cvPoint(3*radius/2,3*radius/2+3*radius/3),CV_RGB(255,255,255),13);
	                line(img2,cvPoint(radius/6,3*radius/2-radius),cvPoint(radius/6,3*radius/2),CV_RGB(255,255,255),20);
	                line(img2,cvPoint(3*radius-radius/6,3*radius/2-radius),cvPoint(3*radius-radius/6,3*radius/2),CV_RGB(255,255,255),20);  
                    firsttime2++;
            }         
            img2=  0.5*wshed2  + 0.5*img2; 
            img2.copyTo(imgprev); 
            imshow("Image-RB",img2);
            c=cvWaitKey(0); 
        
        } 
        if((char)c=='u')
        {
            markerprev.copyTo(markerMask2);
            imgprev.copyTo(img2);
            imshow("Image-RB",img2);
            c=cvWaitKey(0); 
          
        
        }
        

        if( (char)c == 'w' || (char)c == ' ' || count==0 )
        {
            if(count==0)
            {
                 // Creating a mask
	            IplImage imgb=img1;
	            IplImage* mask=cvCreateImage(cvGetSize(&imgb),8,1);
	            
	            cvZero(mask);
	            cvLine(mask,cvPoint(3*radius/2,3*radius/2-radius/3),cvPoint(3*radius/2,3*radius/2+3*radius/3),CV_RGB(255,255,255),13);
	            cvLine(mask,cvPoint(radius/6,3*radius/2-radius),cvPoint(radius/6,3*radius/2*7/5),CV_RGB(255,255,255),20);
	            cvLine(mask,cvPoint(3*radius-radius/6,3*radius/2-radius),cvPoint(3*radius-radius/6,3*radius/2*7/5),CV_RGB(255,255,255),20);
	            markerMask2=mask;
	         
	         }
            int i, j, compCount = 0;
            vector<vector<Point> > contours;
            vector<Vec4i> hierarchy;
            
            findContours(markerMask2, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
            
      		//
            if( contours.empty() )
                continue;
            Mat markers2(markerMask2.size(), CV_32S);
            markers2 = Scalar::all(0);
            int idx = 0;
            for( ; idx >= 0; idx = hierarchy[idx][0], compCount++ )
                drawContours(markers2, contours, idx, Scalar::all(compCount+1), -1, 8, hierarchy, INT_MAX);

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
            watershed( img1, markers2 );
			
            t = (double)getTickCount() - t;
            //printf( "execution time = %gms\n", t*1000./getTickFrequency() );
            Mat imagestore(img2);
            imagestore=Scalar::all(0);
           
            int ix;
                        // paint the watershed image
            for( i = 0; i < markers2.rows; i++ )
                for( j = 0; j < markers2.cols; j++ )
                {
                    int idx = markers2.at<int>(i,j);
                    int ideal=markers2.at<int>(markers2.rows/2,markers2.cols/2);
                    if( idx == -1 )
                        {   
                            
                            wshed2.at<Vec3b>(i,j) = imgf.at<Vec3b>(i,j);
                            imagestore.at<Vec3b>(i,j)=imgf.at<Vec3b>(i,j);
                         }
                    //else if(i>markers2.rows/2-4 && i<markers2.rows/2+4 && j<markers2.cols/2+4 && j>markers2.cols/2-4)
                        
                        //wshed2.at<Vec3b>(i,j) = Vec3b(0,0,0);
                    else if( idx <= 0 || idx > compCount )
                        wshed2.at<Vec3b>(i,j) = Vec3b(0,0,0);
                    else if( idx==ideal)
                        {
                            imagestore.at<Vec3b>(i,j)=imgf.at<Vec3b>(i,j);
                             wshed2.at<Vec3b>(i,j) = img1.at<Vec3b>(i,j);
                            imgGray2.at<Vec3b>(i,j) = Vec3b(0,0,0);
                        }
                    else //if(idx==2)
                    {   cf=imgf.at<Vec3b>(i,j);
                        for(ix=0;ix<11;ix++)
                        {
                            if(  cf[0] >=ix*20 && cf[0]<(ix+1)*20 )
                            {
                                bb[ix]++;
                            }
                            
                            
                            if(  cf[1] >=ix*20 && cf[1]<(ix+1)*20 )
                            {
                                gb[ix]++;
                            }
                            
                            
                            if(  cf[2] >=ix*20 && cf[2]<(ix+1)*20 )
                            {
                                rb[ix]++;
                            }
                            
                            
                            
                                   
                            
                        }
                        
                        
                    	wshed2.at<Vec3b>(i,j) = Vec3b(0,0,0);
                    	imagestore.at<Vec3b>(i,j) = Vec3b(0,0,0);
                    	//imgGray2.at<Vec3b>(i,j) = Vec3b(0,0,0);
                    }
                    //else if( idx==3)
                      //  wshed2.at<Vec3b>(i,j) = Vec3b(255,0,0);
                    
                }
                
             imshow("imgf",imgf);
             cvWaitKey(0);
           
            
            for(ix=0;ix<11;ix++)
            {
                if(yb<bb[ix])
                   { 
                        yb=bb[ix];
                        maxb=ix;
                    }
                
                
                if(yg<gb[ix])
                   {
                        yg=gb[ix];
                    maxg=ix;
                    }
                    
                if(yr<rb[ix])
                    {   yr=rb[ix];
                        maxr=ix;
                     }
                        
            }
            cout<<"Colour"<<maxb*20<<" " <<maxg*20<<" " <<maxr*20;
                       
            for( i = 0; i < markers2.rows; i++ )
                for( j = 0; j < markers2.cols; j++ )
                {
                    int idx = markers2.at<int>(i,j);
                    int ideal=markers2.at<int>(markers2.rows/2,markers2.cols/2);
                    if( idx == -1 )
                        {   
                            
                            1;
                         }
                    //else if(i>markers2.rows/2-4 && i<markers2.rows/2+4 && j<markers2.cols/2+4 && j>markers2.cols/2-4)
                        
                        //wshed2.at<Vec3b>(i,j) = Vec3b(0,0,0);
                    else if( idx <= 0 || idx > compCount )
                        wshed2.at<Vec3b>(i,j) = Vec3b(0,0,0);
                    else if( idx==ideal)
                        {
                            imagestore.at<Vec3b>(i,j)=imgf.at<Vec3b>(i,j);
                             wshed2.at<Vec3b>(i,j) = img1.at<Vec3b>(i,j);
                            imgGray2.at<Vec3b>(i,j) = Vec3b(0,0,0);
                        }
                    else //if(idx==2)
                    {   
                    	wshed2.at<Vec3b>(i,j) = Vec3b(maxb*20+10,maxg*20+10,maxr*20+10);
                    	imagestore.at<Vec3b>(i,j) = Vec3b(maxb*20+10,maxg*20+10,maxr*20+10);
                    	//imgGray2.at<Vec3b>(i,j) = Vec3b(0,0,0);
                    }
                    //else if( idx==3)
                      //  wshed2.at<Vec3b>(i,j) = Vec3b(255,0,0);
                    
                }
           
            
            

            wshed2 = wshed2 + imgGray2*0.5;
            imshow( "Image-RB", wshed2 );
			printf("Blending now.\n");
		    //markers3=markers2;
	        IplImage* imgff= new IplImage(imagestore);
	        cvSaveImage("imagefinal.jpg",imgff);
	         c=cvWaitKey(0);
	        if((char)c=='b')
    	            {
						printf("Blending now.\n");
    	                //blend(markers2, imagestore);
    	                filld("imagefinal.jpg");
                        return 0;
                    }
            //c=cvWaitKey(0);
            if((char)c=='q')
                break;
            
            /*
            cout<<"To save the pic press 's'";
            IplImage* imgn= new IplImage(wshed2); 
            int save=cvWaitKey(0);
            if((char)save=='s')
                cvSaveImage("Watershed.jpg",imgn);
            */
            //int c=cvWaitKey(0);
            
            //if(c=='q')
            //    break;
        }
    }

 
    
    return 0;
}



static void help2()
{
    cout << "\nThis program demonstrates the cascade recognizer. Now you can use Haar or LBP features.\n"
            "This classifier can recognize many kinds of rigid objects, once the appropriate classifier is trained.\n"
            "It's most known use is for faces.\n"
            "Usage:\n"
            "./facedetect [--cascade=<cascade_path> this is the primary trained classifier such as frontal face]\n"
               "   [--nested-cascade[=nested_cascade_path this an optional secondary classifier such as eyes]]\n"
               "   [--scale=<image scale greater or equal to 1, try 1.3 for example>]\n"
               "   [--try-flip]\n"
               "   [filename|camera_index]\n\n"
            "see facedetect.cmd for one call:\n"
            "./facedetect --cascade=\"../../data/haarcascades/haarcascade_frontalface_alt.xml\" --nested-cascade=\"../../data/haarcascades/haarcascade_eye.xml\" --scale=1.3\n\n"
            "During execution:\n\tHit any key to quit.\n"
            "\tUsing OpenCV version " << CV_VERSION << "\n" << endl;
}

void detectAndDraw( Mat& img, CascadeClassifier& cascade,
                    CascadeClassifier& nestedCascade,
                    double scale, bool tryflip );

int found=0;
int face( char *argv[] ,int argc)
{
   
    CvCapture* capture = 0;
    Mat frame, frameCopy, image;
    const string scaleOpt = "--scale=";
    size_t scaleOptLen = scaleOpt.length();
    const string cascadeOpt = "--cascade=";
    size_t cascadeOptLen = cascadeOpt.length();
    const string nestedCascadeOpt = "--nested-cascade";
    size_t nestedCascadeOptLen = nestedCascadeOpt.length();
    const string tryFlipOpt = "--try-flip";
    size_t tryFlipOptLen = tryFlipOpt.length();
    string inputName;
    bool tryflip = false;

    //help2();

    CascadeClassifier cascade, nestedCascade;
    double scale = 1;

    for( int i = 1; i < argc; i++ )
    {
        cout << "Processing " << i << " " <<  argv[i] << endl;
        if( cascadeOpt.compare( 0, cascadeOptLen, argv[i], cascadeOptLen ) == 0 )
        {
            cascadeName.assign( argv[i] + cascadeOptLen );
            cout << "  from which we have cascadeName= " << cascadeName << endl;
        }
        else if( nestedCascadeOpt.compare( 0, nestedCascadeOptLen, argv[i], nestedCascadeOptLen ) == 0 )
        {
            if( argv[i][nestedCascadeOpt.length()] == '=' )
                nestedCascadeName.assign( argv[i] + nestedCascadeOpt.length() + 1 );
            if( !nestedCascade.load( nestedCascadeName ) )
                cerr << "WARNING: Could not load classifier cascade for nested objects" << endl;
        }
        else if( scaleOpt.compare( 0, scaleOptLen, argv[i], scaleOptLen ) == 0 )
        {
            if( !sscanf( argv[i] + scaleOpt.length(), "%lf", &scale ) || scale < 1 )
                scale = 1;
            cout << " from which we read scale = " << scale << endl;
        }
        else if( tryFlipOpt.compare( 0, tryFlipOptLen, argv[i], tryFlipOptLen ) == 0 )
        {
            tryflip = true;
            cout << " will try to flip image horizontally to detect assymetric objects\n";
        }
        else if( argv[i][0] == '-' )
        {
            cerr << "WARNING: Unknown option %s" << argv[i] << endl;
        }
        else
            inputName.assign( argv[i] );
    }

    if( !cascade.load( cascadeName ) )
    {
        cerr << "ERROR: Could not load classifier cascade" << endl;
        help();
        return -1;
    }

    if( inputName.empty() || (isdigit(inputName.c_str()[0]) && inputName.c_str()[1] == '\0') )
    {
        capture = cvCaptureFromCAM( inputName.empty() ? 0 : inputName.c_str()[0] - '0' );
        int c = inputName.empty() ? 0 : inputName.c_str()[0] - '0' ;
        if(!capture) cout << "Capture from CAM " <<  c << " didn't work" << endl;
    }
    else if( inputName.size() )
    {
        image = imread( inputName, 1 );
        if( image.empty() )
        {
            capture = cvCaptureFromAVI( inputName.c_str() );
            if(!capture) cout << "Capture from AVI didn't work" << endl;
        }
    }
    else
    {
        image = imread( "lena.jpg", 1 );
        if(image.empty()) cout << "Couldn't read lena.jpg" << endl;
    }

    cvNamedWindow( "result", 1 );

    if( capture )
    {
        cout << "In capture ..." << endl;
        for(;;)
        {
            IplImage* iplImg = cvQueryFrame( capture );
            frame = iplImg;
            if( frame.empty() )
                break;
            if( iplImg->origin == IPL_ORIGIN_TL )
                frame.copyTo( frameCopy );
            else
                flip( frame, frameCopy, 0 );

            detectAndDraw( frameCopy, cascade, nestedCascade, scale, tryflip );

            if( waitKey( 10 ) >= 0 )
                goto _cleanup_;
        }

        waitKey(0);

_cleanup_:
        cvReleaseCapture( &capture );
    }
    else
    {
        cout << "In image read" << endl;
        if( !image.empty() )
        {
            detectAndDraw( image, cascade, nestedCascade, scale, tryflip );
            waitKey(0);
        }
        else if( !inputName.empty() )
        {
            /* assume it is a text file containing the
            list of the image filenames to be processed - one per line */
            FILE* f = fopen( inputName.c_str(), "rt" );
            if( f )
            {
                char buf[1000+1];
                while( fgets( buf, 1000, f ) )
                {
                    int len = (int)strlen(buf), c;
                    while( len > 0 && isspace(buf[len-1]) )
                        len--;
                    buf[len] = '\0';
                    cout << "file " << buf << endl;
                    image = imread( buf, 1 );
                    if( !image.empty() )
                    {
                        detectAndDraw( image, cascade, nestedCascade, scale, tryflip );
                        c = waitKey(0);
                        if( c == 27 || c == 'q' || c == 'Q' )
                            break;
                    }
                    else
                    {
                        cerr << "Aw snap, couldn't read image " << buf << endl;
                    }
                }
                fclose(f);
            }
        }
    }

   // cvDestroyWindow("result");

    return found;
    
}

void detectAndDraw( Mat& img, CascadeClassifier& cascade,
                    CascadeClassifier& nestedCascade,
                    double scale, bool tryflip )
{
    int i = 0;
    double t = 0;
    vector<Rect> faces, faces2;
    const static Scalar colors[] =  { CV_RGB(0,0,255),
        CV_RGB(0,128,255),
        CV_RGB(0,255,255),
        CV_RGB(0,255,0),
        CV_RGB(255,128,0),
        CV_RGB(255,255,0),
        CV_RGB(255,0,0),
        CV_RGB(255,0,255)} ;
    Mat gray, smallImg( cvRound (img.rows/scale), cvRound(img.cols/scale), CV_8UC1 );

    cvtColor( img, gray, CV_BGR2GRAY );
    resize( gray, smallImg, smallImg.size(), 0, 0, INTER_LINEAR );
    equalizeHist( smallImg, smallImg );

    t = (double)cvGetTickCount();
    cascade.detectMultiScale( smallImg, faces,
        1.1, 2, 0
        //|CV_HAAR_FIND_BIGGEST_OBJECT
        //|CV_HAAR_DO_ROUGH_SEARCH
        |CV_HAAR_SCALE_IMAGE
        ,
        Size(30, 30) );
    if( tryflip )
    {
        flip(smallImg, smallImg, 1);
        cascade.detectMultiScale( smallImg, faces2,
                                 1.1, 2, 0
                                 //|CV_HAAR_FIND_BIGGEST_OBJECT
                                 //|CV_HAAR_DO_ROUGH_SEARCH
                                 |CV_HAAR_SCALE_IMAGE
                                 ,
                                 Size(30, 30) );
        for( vector<Rect>::const_iterator r = faces2.begin(); r != faces2.end(); r++ )
        {
            faces.push_back(Rect(smallImg.cols - r->x - r->width, r->y, r->width, r->height));
        }
    }
    t = (double)cvGetTickCount() - t;
    //printf( "detection time = %g ms\n", t/((double)cvGetTickFrequency()*1000.) );
    for( vector<Rect>::const_iterator r = faces.begin(); r != faces.end(); r++, i++ )
    {   found++;
        Mat smallImgROI;
        vector<Rect> nestedObjects;
        Point center;
        Scalar color = colors[i%8];
        int radius;

        double aspect_ratio = (double)r->width/r->height;
        if( 0.75 < aspect_ratio && aspect_ratio < 1.3 )
        {
            center.x = cvRound((r->x + r->width*0.5)*scale);
            center.y = cvRound((r->y + r->height*0.5)*scale);
            radius = cvRound((r->width + r->height)*0.25*scale);
            circle( img, center, radius, color, 3, 8, 0 );
            fc=cvPoint(center.x,center.y);
            fr=radius;
        }
        else
            rectangle( img, cvPoint(cvRound(r->x*scale), cvRound(r->y*scale)),
                       cvPoint(cvRound((r->x + r->width-1)*scale), cvRound((r->y + r->height-1)*scale)),
                       color, 3, 8, 0);
        if( nestedCascade.empty() )
            continue;
        smallImgROI = smallImg(*r);
        nestedCascade.detectMultiScale( smallImgROI, nestedObjects,
            1.1, 2, 0
            //|CV_HAAR_FIND_BIGGEST_OBJECT
            //|CV_HAAR_DO_ROUGH_SEARCH
            //|CV_HAAR_DO_CANNY_PRUNING
            |CV_HAAR_SCALE_IMAGE
            ,
            Size(30, 30) );
        for( vector<Rect>::const_iterator nr = nestedObjects.begin(); nr != nestedObjects.end(); nr++ )
        {
            center.x = cvRound((r->x + nr->x + nr->width*0.5)*scale);
            center.y = cvRound((r->y + nr->y + nr->height*0.5)*scale);
            radius = cvRound((nr->width + nr->height)*0.25*scale);
            circle( img, center, radius, color, 3, 8, 0 );
        }
    }
    /*
    if(found)
        cout<<"FOUND\n";
    else 
        cout<<"NOT FOUND\n";
    */    
    cv::imshow( "result", img );

       
}

void blend(Mat markers3, Mat b2)
{

Mat b1=imread("Moved.jpg");
//Mat b2=imread("imagefinal.jpg");

Mat b3(b1);
b3=Scalar::all(0);  
cout<<"Colour"<<maxb<<" " <<maxg<<" " <<maxr;
int i,j;
    for( i = 0; i < markers3.rows; i++ )
        for( j = 0; j < markers3.cols; j++ )
                {
                    int idx = markers3.at<int>(i,j);
                    if( idx == -1 )
                    {
                        if(i-1>0)
                        {   
                            b3.at<Vec3b>(i-1,j)=0.5*b1.at<Vec3b>(i-1,j) + 0.5*b2.at<Vec3b>(i-1,j);
                            if(j-1>0)          
                                {
                                    b3.at<Vec3b>(i-1,j-1)=0.5*b1.at<Vec3b>(i-1,j-1) + 0.5*b2.at<Vec3b>(i-1,j-1);
                                    b3.at<Vec3b>(i,j-1)=0.5*b1.at<Vec3b>(i,j-1) + 0.5*b2.at<Vec3b>(i,j-1);
                                }
                            
                            if(j+1>0)
                                {
                                    b3.at<Vec3b>(i-1,j+1)=0.5*b1.at<Vec3b>(i-1,j+1) + 0.5*b2.at<Vec3b>(i-1,j+1);          
                                    b3.at<Vec3b>(i,j+1)=0.5*b1.at<Vec3b>(i,j+1) + 0.5*b2.at<Vec3b>(i,j+1);
                                 }
                                      
                        }
                        if(i+1>0)
                        {   
                            b3.at<Vec3b>(i+1,j)=0.5*b1.at<Vec3b>(i+1,j) + 0.5*b2.at<Vec3b>(i+1,j);
                            if(j-1>0)          
                                b3.at<Vec3b>(i+1,j-1)=0.5*b1.at<Vec3b>(i+1,j-1) + 0.5*b2.at<Vec3b>(i+1,j-1);
                            
                            if(j+1>0)
                                b3.at<Vec3b>(i+1,j+1)=0.5*b1.at<Vec3b>(i+1,j+1) + 0.5*b2.at<Vec3b>(i+1,j+1);          
                                      
                        }
                        
                        
                        
                    }
                    else
                        b3.at<Vec3b>(i,j)=b2.at<Vec3b>(i,j);
				}
				printf("Teets.\n");
//imshow("b1",b1);
//imshow("b2",b2);
//imshow("Blend",b3);

cout<<"Hello";
waitKey(0);
int i1,j1;
       printf("Teets.\n");
       for(i=1;i<markers3.rows-1;i++)
        for(j=1;j<markers3.cols-1;j++)
        {  if(markers3.at<int>(i,j)==-1)
           if(i-1>0 &&  i+1 >0 && j-1>0 && j+1>0)
           {
                for(i1=0;i1<3;i1++)
                    for(j1=0;j1<3;j1++)
                    {
                        b3.at<Vec3b>(i,j)+=1.0/9*b3.at<Vec3b>(i+i1-1,j+j1-1);  
                    }
                
           } 
        
        }
        printf("Teets.\n");
imshow("Blur",b3);
IplImage* blurred=new IplImage(b3);
cvWaitKey(0);
printf("Teets.\n");
cvSaveImage("blurred.jpg",blurred);
//filld("blurred.jpg");
//printf("Teets.\n");

}


