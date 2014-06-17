/**
 * @file stdafx.h
 * cs684 course project: The project duplicates a black and white image
 * read through the camera and redraws it on a chart paper/surface.
 * This piece of code, extracts the vector path from the thinned image.
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


#define PI 3.14159265
#define MAX_HOP 4
#define HEIGHT h
#define WIDTH w
#define THRESH_M 0.7
#define THRESH_ANGLE 10
#define LONG_DISTANCE_THRESH 100
#define null '\0' 


using namespace std;
int h,w;
//Maintains the number of points in consideration
//int NUMBER_OF_FEATURE_POINT = 0;

enum {RIGHT,LEFT}TURN;

point find_neighbour (char** , point);
list<point> traverse_black_line (char** ,point);
void display_image(char**);
char** mark_as_visited (char**,point,point);
list<point> appriximate_straight_line (list<point>,int );
list<double> find_direction_vector (list<point>);
double tan_inverse (double );
list<point> StartPoints(IplImage* , char** );
bool filter1(char** , int ,int );

/**
* all pixels where 0 occues between src and dest are changed to 2.
* This is due to mark them as already visited and not to interfere with future calculation.
*/
char** mark_as_visited (char** image,point src,point dest)
{
	int row_it,col_it;
	int row_start=0,row_end=0;
	int col_start=0,col_end=0;

	// get the row range
	if (src.get_X()<dest.get_X())
	{
		row_start=src.get_X();
		row_end=dest.get_X();
	}
	else
	{
		row_end=src.get_X();
		row_start=dest.get_X();
	}
	// get the col range
	if (src.get_Y()<dest.get_Y())
	{
		col_start=src.get_Y();
		col_end=dest.get_Y();
	}
	else
	{
		col_end=src.get_Y();
		col_start=dest.get_Y();
	}
	for (row_it=row_start; row_it<=row_end;row_it++)
	{
		for (col_it=col_start; col_it<=col_end;col_it++)
		{
			if (image[row_it][col_it] == '0')
			{
				image[row_it][col_it] = '2';
			}
		}
	}
	return image;
}

void display_image (char** image)

{
	int i,j;
	for (i=0;i<=HEIGHT;i++)
	{
		for (j=0;j<=WIDTH;j++)
		{
			cout << image[i][j];
		}
		cout << endl;
	}
}

void display_image (IplImage* img)
{
  int height,width,step,channels;
  uchar *data;
  int i,j;

  // get the image data
  height    = img->height;
  width     = img->width;
  step      = img->widthStep;
  channels  = img->nChannels;
  data      = (uchar *)img->imageData;

  	for (i=0;i<height;i++)
	{
		for (j=0;j<width;j++)
		{
			int pix=data[i*step+j*channels];
			if(pix>10)
				cout<<1<<",";
			else
				cout<<0<<",";
		}
		cout << endl;
	}
}


/**
*Function to find K hop neighbour. Maximum value of K is MAX_HOP
*/
point find_neighbour (char** image, point current)
{
	int hop=1,i,j;
	point dummy;
	int cur_X = current.get_X();
	int cur_Y = current.get_Y();
	for (hop=MAX_HOP;hop > 1;hop--)
	{
		if (cur_X+hop >= HEIGHT || cur_Y+hop >= WIDTH)
			continue;
		for (i=(cur_X-hop)>0?(cur_X-hop):0;i<=(cur_X+hop);i++)
		{
			for (j=(cur_Y-hop)>0?(cur_Y-hop):0;j<=(cur_Y+hop);j++)
			{
				if (abs(current.get_X()-i)==hop || abs(current.get_Y()-j)==hop)
				{
					if (image[i][j]=='0')
					{
						dummy.set_X(i);
						dummy.set_Y(j);
						return dummy;
					}
					else
					{
						//NOTHING
					}
				}
				else
				{
					//NOTHING
				}
			}
		}
	}
	return point (-1,-1);
}




/**
* Traverse the black line from the start_point given to another end point
*/
list<point> traverse_black_line (char** image,point start_point)
{
	int max_points = HEIGHT;
	int line_it=0;
	point neighbour;
	point zero_point;
	int direction = 0;
	list<point>path;

	point old_point(start_point),current_point(start_point);
	path.push_back (start_point);
	while (line_it<max_points)
	{
		line_it++;
		neighbour = find_neighbour (image,current_point);
		//neighbour.display();
		if (neighbour.invalid())
			break;
		old_point=current_point;
		current_point=neighbour;
		
		image = mark_as_visited (image,old_point,current_point);
		
		path.push_back (current_point);
		line_it++;
		if (line_it>= HEIGHT+MAX_HOP)
			break;
	
	}
	//remove the end points.
	//list<point>::const_iterator it = ;
	
	return path;
}

/**
* Returns the vectors constructed from the given path
*/
list<point> appriximate_straight_line (list<point> path, int long_distance_thresh)
{
	int refined_point_iterator=0;
	point f_point,prev_point;
	double m;
	double angle,delta_angle,mean_angle=0;
	double mean_slope=0; // Mean of the slopes of the points in between two feature points.
	double count_omit_point=0; //counts how many points are there in between two feature points. made double to make mean_slope calculation
	
	list<point> refined_path;
	list<point>::iterator it = path.begin();
	refined_path.push_back (*it);
	f_point = *it;
	prev_point = f_point;
	it++;
	if(it==path.end())
		return path;
	mean_angle= it->angleBetweenPoints(f_point);//tan_inverse((*it).slope (f_point));

	for (;it!=path.end();it++)
	{
		//cout<<"\npath[i]= "; (*it).display(); 
		//cout << " f_point= "; f_point.display();
		count_omit_point++;
		m = f_point.slope (*it);
		angle = f_point.angleBetweenPoints(*it);//tan_inverse (m);
		
		delta_angle = mean_angle>angle ? mean_angle-angle : angle-mean_angle;

		//printf ("  m=%4.2f mean_angle=%4.2f delta_angle=%4.2f ",m,mean_angle,delta_angle);
		// Select a new feature point when
		//  ---slope with f_point is greater than the abg slope
		//  ---Or long enough distance traversed till the last feature point
		if (abs(delta_angle) >= THRESH_ANGLE || count_omit_point > long_distance_thresh)//LONG_DISTANCE_THRESH)
		{
			if (f_point.distance (prev_point) <= 10)
				; //Do not accept too close f_points
			else
			{
				refined_path.push_back (prev_point);
				f_point = prev_point;
				if (!(count_omit_point > long_distance_thresh)) // Point selected due to thresh_angle
					mean_angle = prev_point.angleBetweenPoints(*it);//tan_inverse(prev_point.slope(*it));
				else
					mean_angle = ((mean_angle*(count_omit_point-1))+angle)/count_omit_point;
				//printf ("NEW FEATURE PT SEL."); prev_point.display();
				count_omit_point=0;
			}
		}
		else
			mean_angle = ((mean_angle*(count_omit_point-1))+angle)/count_omit_point;
		prev_point = (*it);
	}


	refined_path.push_back (prev_point);
	return refined_path;
}


void create_refined_file (list<point> path)
{
	list<point>::iterator it = path.begin();
	char** image = new char* [HEIGHT+1];
	for (int i=0;i<=HEIGHT;i++)
		image[i] = new char [WIDTH+2];
	int i=0,j=0;
	for (i=0;i<=HEIGHT;i++)
		for (j=0;j<=WIDTH+2;j++)
			image[i][j] = '0';
	for (;it!=path.end();it++)
		image[(*it).get_X()][(*it).get_Y()] = '1';
}

/**
* returns true if the center of the filter is the end of a line segment
* Else return false
*/
bool filter1(char** mat, int i,int j){
	if(mat[i][j]=='1')
		return false;

	int count=0,k,l;

	for(k=i-1;k<=i+1;k++)
		for(l=j-1;l<=j+1;l++)
			if(mat[k][l]=='0')
				count++;
	if(count==2)
		return true;

	if(count==3){
		if(mat[i-1][j-1]=='0' && (mat[i-1][j]=='0' || mat[i][j-1])=='0' )
			return true;
		else if(mat[i+1][j+1]=='0' && (mat[i+1][j]=='0' || mat[i][j+1]=='0') )
			return true;
		else if(mat[i+1][j-1]=='0' && (mat[i][j-1]=='0' || mat[i+1][j]=='0') )
			return true;
		else if(mat[i-1][j+1]=='0' && (mat[i][j+1]=='0' || mat[i-1][j]=='0') )
			return true;
	}
	return false;
}

/**
* Locate end points
* Returns a list of end points
*/
list<point> StartPoints(IplImage* img, char** mat){
	int i,j;
	int height=img->height;
	int width=img->width;
	double dist = 0;

	list<point> endPoints;
	point p_prev;

	for(i=1;i<height-1;i++){
		for(j=1;j<width-1;j++){
			if(filter1(mat,i,j)==true){
				//cout<<"\n\n\n\n END point : <"<<i<<","<<j<<">";
				const point p(i,j);
				point p1(i,j);
				point p2(p_prev.get_X(),p_prev.get_Y());
				dist = p1.distance (p2);
				if(pow(dist,.5)<1)
				{
					cvLine(img, cvPoint(p1.get_Y(),p1.get_X()), cvPoint(p2.get_Y(),p2.get_X()), cvScalar(0,0,0), 1);
				}
				else
				{
					endPoints.push_back(p);
					cvCircle (img,cvPoint(j,i),5,cvScalar (255,0,0),1);
					p_prev=p1;
				}
			}
		}
	}
	cvShowImage("mainWin2", img );
	cvWaitKey(0);
	return endPoints;
}



/**
* Give the angle in degree for a given slope
*/
double tan_inverse (double slope)
{
	double angle;
	// Assumption being 
	if (slope == 56)
		return 90;
	else if (slope == -56)
		return -90;
	angle = atan (slope) * 180 / PI;
	return angle;
}

/**
* Give the direction vector for a given path
* Its a consecutive instruction of forward and the turn by a specific amount of angle
* 1 -> turn right
* 0 -> turn left
*/
list<double> find_direction_vector (list<point> path)
{
	double distance,slope,angle,prev_angle=-90;
	char turn;
	list<point>::iterator it = path.begin();
	list<double> moves;

	point prev_point = (*it);
	it->display (); 
	it++;

	if(it==path.end())
		return moves;

	distance = (*it).distance (prev_point);
	moves.push_back(distance);
	prev_angle = it->angleBetweenPoints(prev_point);//tan_inverse (it->slope(prev_point));
	
	prev_point.display(); 
	(*it).display(); 
	cout << "   " << distance << endl;
	it++;

	for (;it!=path.end();it++)
	{
		distance = (*it).distance (prev_point);
		slope = (*it).slope (prev_point);
		angle = (*it).angleBetweenPoints(prev_point);//tan_inverse(slope);
		
		double	dif_angle=prev_angle-angle;

		if(dif_angle>180)
			dif_angle-=360;
		else if(dif_angle<-180)
			dif_angle+=360;

/*
		if(((prev_angle)*(angle))<0)
			dif_angle=-dif_angle;
*/
		TURN = (dif_angle) < 0 ? RIGHT : LEFT;
		
		moves.push_back(fabs(dif_angle));
		moves.push_back(TURN);
		moves.push_back(distance);

		prev_point.display(); (*it).display();cout << "  prev_angle:" << prev_angle << "  angle:" << angle << 
			"  diff:" << dif_angle << "  Turn "<<TURN << endl;
		prev_angle = angle;

		prev_point = *it;
	}
	
	return moves;
}



