/**
 * @file stdafx.h
 * cs684 course project: The project duplicates a black and white image
 * read through the camera and redraws it on a chart paper/surface.
 * This piece of code, stitches all the parts of the project. It is the main
 * program.
 * @author Group5, Abhirup Ghosh, Anirban Basumallik & Gourab Roy, IIT Bombay
 * @date 07/Nov/2010
 * @version 1.0
 *
 * @section LICENSE
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 * This is a property of ERTS Lab, IIT Bombay.
 *
 */

#include "includes.h"
#define null '\0' 


/**
* Global variable to maintain the image matrix
*/
char** mat;

void capture_image (char* );
void vectorGeneration(char* , IplImage* , list<point> );
void sample(char* ,IplImage* );
void show_vector_image (list<point>);


/**
* Show the vector image drawm
*/
void show_vector_image (list<point> path,char* imgfile,IplImage* img1)
{

	cout << "Showing vector image\n";
	IplImage* img=cvLoadImage(imgfile);
	
	if (img==0)
	{
		cout << "Can not open output file to show extracted vector image\n";
		return;
	}

	list<point>::iterator it = path.begin();
	point prev_point = (*it);
	it++;
	for (;it!=path.end();it++)
	{
		cvLine(img, cvPoint((*it).get_Y(),(*it).get_X()), cvPoint(prev_point.get_Y(),prev_point.get_X()), cvScalar(0,255,0), 4);
		cvCircle (img,cvPoint((*it).get_Y(),(*it).get_X()),1,cvScalar (255,0,0),2);
		cvCircle (img,cvPoint(prev_point.get_Y(),prev_point.get_X()),1,cvScalar (255,0,0),2);

		cvLine(img1, cvPoint((*it).get_Y(),(*it).get_X()), cvPoint(prev_point.get_Y(),prev_point.get_X()), cvScalar(0,255,0), 4);
		cvCircle (img1,cvPoint((*it).get_Y(),(*it).get_X()),1,cvScalar (255,0,0),2);
		cvCircle (img1,cvPoint(prev_point.get_Y(),prev_point.get_X()),1,cvScalar (255,0,0),2);

		prev_point = *it;
	}
	//display("mainWin", img );
	display("mainWin2", img1 );
	cvReleaseImage(&img );
}

/**
* Capture image from webcam
*/
void capture_image (char* filemane)
{
	IplImage* frame = 0;		
	//CV_CAP_ANY
	CvCapture* capture = cvCaptureFromCAM( 0 );   
	if( !capture ) 
	{     
		fprintf( stderr, "ERROR: capture is NULL \n" );     
		return;   
	}    
	// Create a window in which the captured images will be presented   
	cvNamedWindow( "mywindow", CV_WINDOW_AUTOSIZE );    
	// Show the image captured from the camera in the window and repeat   
	while( 1 ) 
	{     
		// Get one frame     
		frame = cvQueryFrame( capture );     
		if( !frame ) 
		{       
			fprintf( stderr, "ERROR: frame is null...\n" );
			return;
		}      
		cvShowImage( "mywindow", frame );				
		// Do not release the frame     
		//If ESC key pressed, Key=0x10001B under OpenCV 0.9.7(linux version),     
		//remove higher bits using AND operator     
		if( (cvWaitKey(10) & 255) == 27 ) 
		{
			break;   
		}
	}    
	// Show the image frame.
	cvShowImage( "mywindow", frame );
	cvWaitKey(0);
	// Save the image in specified filename
	if(!cvSaveImage(filemane,frame)) printf("Could not save: %s\n",filemane);
	// Release the capture device and window
	cvReleaseCapture( &capture );
	cvDestroyWindow( "mywindow" ); 
}



/**
* Moves the BOT to the next end point
*/
list<double> moveToNextEnd(list<double> moves, point prev_endCur, point endCur, point startNext, point next_startNext){

	double angle1,angle2,angle3;
	angle1=endCur.angleBetweenPoints(prev_endCur);
	angle2=startNext.angleBetweenPoints(endCur);
	angle3=next_startNext.angleBetweenPoints(startNext);

	cout<<"\n startNext: ";startNext.display();
	cout<<"\n next_startNext: ";next_startNext.display();
	cout<<"\n angle1: "<<angle1<<"\n angle2: "<<angle2<<"\n angle3: "<<angle3;

	double dif_angle=angle2-angle3;
	if(dif_angle>180)
		dif_angle-=360;
	else if(dif_angle<-180)
		dif_angle+=360;
/*	
	if(((angle3/10)*(angle2/10))<0)
		dif_angle=-dif_angle;
*/

	int TURN = (dif_angle) < 0 ? 0 : 1;

	//push in reverse order
	moves.push_front(TURN);
	moves.push_front(fabs(dif_angle));

	dif_angle=angle1-angle2;
	if(dif_angle>180)
		dif_angle-=360;
	else if(dif_angle<-180)
		dif_angle+=360;

	/*
	if(((angle1/10)*(angle2/10))<0)
		dif_angle=-dif_angle;
	*/

	TURN = (dif_angle) < 0 ? 0 : 1;
	double distance=endCur.distance(startNext);

	moves.push_front(distance);

	moves.push_front(2);
	moves.push_front(0);
	moves.push_front(0);

	moves.push_front(TURN);
	moves.push_front(fabs(dif_angle));

	return moves;
}

/**
* Generates vectors from the image
*/
void vectorGeneration(char* filename, IplImage* img, list<point> endPoints){
	list<point> path;
	list<point> refined_path;
	unsigned int size=endPoints.size();
	list<double> whole_moves;
	point lastPoint=point(-1,-1);
	point lastlastPoint,frontNext;
	bool first=true;

	while(endPoints.empty()==false)
	{


		point start_point=endPoints.front();
		endPoints.pop_front();
		path = traverse_black_line(mat,start_point);
		endPoints.remove (path.back());
		
		if(path.size()==1)
			continue;


		refined_path = appriximate_straight_line(path,8);
		//cout << "\n\n\n 2nd refine \n\n";
		//refined_path=joining_collinear_points(refined_path);
		refined_path = appriximate_straight_line(refined_path,1);
		//create_refined_file (refined_path);
		if (refined_path.size()<=1)
			continue;

		show_vector_image(refined_path,filename,img);
		size=endPoints.size();
		//cout<<" \n\n\n Size of list"<<size;
		list<double> moves=find_direction_vector(refined_path);

		frontNext=refined_path.front();
		refined_path.pop_front();

		if(first){
			double angle1=-90,angle2=refined_path.front().angleBetweenPoints(frontNext);
			double dif_angle=angle1-angle2;
			if(dif_angle>180)
				dif_angle-=360;
			else if(dif_angle<-180)
				dif_angle+=360;

			cout<<"\n"<<angle1<<"   "<<angle2;
			frontNext.display();
			refined_path.front().display();

			int TURN = (dif_angle) < 0 ? 0 : 1;
			//push in reverse order
			moves.push_front(TURN);
			moves.push_front(fabs(dif_angle));
			moves.push_front(0);
			first=false;
		}

		if(lastPoint.get_X()!=-1)
			moves=moveToNextEnd(moves, lastlastPoint, lastPoint, frontNext, refined_path.front());

		refined_path.push_front(frontNext);
		lastPoint=point(refined_path.back());

		refined_path.pop_back();
		lastlastPoint=refined_path.back();

		if(moves.size()==0)
			continue;


		list<double>::iterator it;
		it=moves.begin();

		int i;
		for(i=0;it!=moves.end();it++,i++)
		{
			whole_moves.push_back (*it);
		}
		
	}

		whole_moves.push_back(0);
		whole_moves.push_back(0);

		int *moves_int=new int[whole_moves.size()];

	//	whole_moves.clear();

		//whole_moves.push_back(0);
		//whole_moves.push_back(0);
		//whole_moves.push_back(2);
		int k;
		//for(k=0;k<3;k++)
		{
	/*		whole_moves.push_back(3);
			whole_moves.push_back(5);
			whole_moves.push_back(1);

			whole_moves.push_back(40);
			whole_moves.push_back(4);
			whole_moves.push_back(1);

			whole_moves.push_back(40);
			whole_moves.push_back(8);
			whole_moves.push_back(0);

			whole_moves.push_back(40);
			whole_moves.push_back(12);
			whole_moves.push_back(1);

			whole_moves.push_back(40);
			whole_moves.push_back(16);
			whole_moves.push_back(0);

			whole_moves.push_back(40);
			whole_moves.push_back(0);
			whole_moves.push_back(0);
	*/
		}

		list<double>::iterator it;
		it=whole_moves.begin();

		int i;
		for(i=0;it!=whole_moves.end();it++,i++)
		{
			moves_int[i]=(int)(*it);
			cout<<"\n ";
			if (i%3==0)
				cout << "DIST ";
			else if (i%3==1)
				cout <<"ANGLE ";
			else if (i%3==2)
				cout <<"DIRECTION ";
			cout<<moves_int[i];
		}
		
	//	getchar();

		
		writeOnBot(moves_int,whole_moves.size());

}

void sample(char* imgfile,IplImage* img)
{
//  IplImage* img = 0; 
  int height,width,step,channels;
  char *data;
  int i;

  // load an image  
  img=cvLoadImage(imgfile);
  if(!img)
  {
    printf("Could not load image file: %s\n",imgfile);
    exit(0);
  }

  // get the image data
  height    = img->height;
  width     = img->width;
  step      = img->widthStep;
  channels  = img->nChannels;
  data      = img->imageData;

  //defining global variables 
  h=height;
  w=width;

  printf("Processing a %dx%d image with %d channels\n",height,width,channels); 
  cout<<"\n\n height "<<height<<"\nwidth "<<width<<"\nwidthStep "<<step<<"\nChannel "<<channels;

  // create a window
  cvNamedWindow("mainWin", CV_WINDOW_AUTOSIZE); 
  cvMoveWindow("mainWin", 100, 100);

  mat=new char*[height+1];
	for(i=0;i<height+1;i++)
		mat[i]=new char[width+1];

  /**
  * Apply median filter to smooth
  */
  cvSmooth(img,img,CV_MEDIAN);// display ("mainWinSmooth",img);
  
  /**
  * Red boundary detection 
  * make all pixles which are outside the red boundary white.
  */
  BoundaryDetection(img,140,60,60);

  /**
  * Thresholding according to grey scale
  */
  Thresholding(img,150);//   display ("mainWinThes", img );

  /**
  * Thinning the thresholded image
  */
  Thinning(img,mat);// 
  display("mainWinThinned", img );


  list<point> endPoints = StartPoints(img,mat);

  /**
  * Generate vectors and do further processing
  */
  vectorGeneration(imgfile, img, endPoints);

  //release all resouces that were taken
  cvReleaseImage(&img );
}


/***
 *Main Function
 */
int main ()
{
	IplImage* img=0;
	char filename[100];
	strcpy(filename,"capture.jpg");
	//cout<<"\n Enter the file name: ";
	//cin>>filename;
//	char* filename = "test3.jpg";
	//capture_image(filename);
 	sample(filename,img);

	getchar();
	return 0;
}
