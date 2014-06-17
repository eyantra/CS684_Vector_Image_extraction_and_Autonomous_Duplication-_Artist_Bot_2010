/**
 * @file point.h
 * cs684 course project: The project duplicates a black and white image
 * read through the camera and redraws it on a chart paper/surface.
 * This piece of code, defines the point class and the related operations.
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
 
#define INF 56
#define PI 3.14159265

class point
{
	int x,y;
public:
	point (int _x,int _y){x = _x; y = _y;}
	point (){x=0;y=0;}
	bool operator== (point p){return ((p.x==x && p.y==y)?1:0);}
	bool operator!= (point p){return ((p.x==x && p.y==y)?0:1);}
	
	///<summary>
	/// Calculates slope between p1 and p2
	///</summary>
	double slope (point p1)
	{
		double m;
		double x1=x,y1=y, x2=p1.x,y2=p1.y;
		double deltaX,deltaY;
		if (y==p1.y && x1-x2>0) //Checks for divide by zero error
			return INF;
		if (y==p1.y && x1-x2<0) //Checks for divide by zero error
			return -INF;

		try
		{
			if (x==p1.x)
				return 0;
			deltaX = x1-x2;
			deltaY = y1-y2;
			m = deltaX/deltaY;
			return m;
		}
		catch (int x)
		{
			if (x>0)
				return INF;
			else
				return -INF;
		}
	}
	

	///<summary>
	/// Calculates angle between points
	///</summary>
	double angleBetweenPoints (point p2)
	{
		double x1=x,y1=y, x2=p2.x,y2=p2.y;
		double deltaX,deltaY;
		double angle=0,slope;

		deltaX = x1-x2;
		deltaY = y1-y2;

		if(deltaY<0){
			slope=deltaX/deltaY;
			angle = atan (slope) * 180 / PI;
			if(deltaX<0)
				angle-=180;
			else if(deltaX==0)
				angle=180;
			else
				angle+=180;
		}
		else if(deltaY==0){
			if(deltaX>0)
				angle=90;
			else if(deltaX<0)
				angle=-90;
		}
		else{
			slope=deltaX/deltaY;
			angle = atan (slope) * 180 / PI;
		}

		
		return angle;
	}


	///<summary>
	/// Calculates distance between p1 and p2
	///</summary>
	double distance (point p1)
	{
		double d;
		double x1=x,y1=y, x2=p1.x,y2=p1.y;
		d = pow (pow((x1-x2),2)+pow((y1-y2),2),0.5);
		return d;
	}

	///<summary>
	/// returns true for (-1,-1) as this indicates an invalid point
	/// otherwise return flase
	///</summary>
	bool invalid ()
	{
		if (x==-1 && y==-1)
			return true;
		return false;
	}

	int get_X (){return x;}
	int get_Y (){return y;}
	void set_X(int x){this->x=x;}
	void set_Y(int y){this->y=y;}

	///<summary>
	/// Displays a point
	///</summary>
	void display ()
	{
		std::cout << "(" << this->y << "," << this->x << ")";
	}
};
