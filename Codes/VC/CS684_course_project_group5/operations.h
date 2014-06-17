/**
 * @file operations.h
 * cs684 course project: The project duplicates a black and white image
 * read through the camera and redraws it on a chart paper/surface.
 * This piece of code, does the initial part of the image processing operations
 * like cropping, thresholding, and thinning of the web cam captures image.
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


void display (char* , IplImage* );
void write2file(short** ,int , int , char* );
void Thinning(IplImage* , char** );
void Thresholding(IplImage* , int );
void crop(IplImage* , point* , point* , int );
void BoundaryDetection(IplImage* , int , int , int );


void write2file(short** mat,int r, int c, char* file){
  ofstream myfile;
  myfile.open (file);
  for(int i=0;i<r;i++){
		for(int j=0;j<c;j++)
		  myfile << mat[i][j];
	myfile<<"\n";
  }
  myfile.close();
}

/**
* Thins a given image
*/
void Thinning(IplImage* img, char** mat)
{
//	int height=ImgHeader.image_length,width=ImgHeader.image_width;
	int height,width,step,channels;
	uchar *data;
	short **oldImg, **newImg, **thinImg;
	int i, j, k;
	int nChanges;					// no. of changes
	int p, p1, p2, p3, p4,			// 3X3 window around pixel p
		p5, p6, p7, p8;	
	bool remove;
	
    // get the image data
    height    = img->height;
    width     = img->width;
    step      = img->widthStep;
    channels  = img->nChannels;
    data      = (uchar *)img->imageData;

	short** imgMat;
	imgMat=new short*[height+1];
	for(i=0;i<height+1;i++)
		imgMat[i]=new short[width+1];

	for(i=0;i<height;i++)
		for(j=0;j<width;j++){
			int pix=data[i*step+j*channels];
				if(pix>10)
					imgMat[i][j]=0;
				else
					imgMat[i][j]=1;
		}
//	char* file1="input.txt",file2="output.txt";
//	write2file(imgMat,height,width,"input.txt");
	oldImg  = new short *[height+2];
	newImg  = new short *[height+2];
	thinImg = new short *[height];

	for(i = 0; i < (height+2); i++)
	{
		oldImg[i] = new short [width+2];
		newImg[i] = new short [width+2];
		for(j = 0; j < (width+2); j++) 
			newImg[i][j] = 0;
	}
	for(i = 0; i < height; i++)
	{
		thinImg[i] = new short [width];
		for(j = 0; j < width; j++) 
			//entering the data matrix into newImg[][]
			newImg[i+1][j+1]=imgMat[i][j];
	}
	for(i = 0; i < height+2; i++)
	{
		for(j = 0; j < width+2; j++) 
			oldImg[i][j] = newImg[i][j];
	}

	int substep=1;
	int init=1,final=8;

	do
	{
		nChanges = 0;
		for(substep = init; substep <= final; substep++)		// step1.
		{
			for(i = 0; i < height; i++)
			{	
				for(j = 0; j < width; j++)
				{
					p1 = oldImg[i][j];   p2 = oldImg[i][j+1];   p3 = oldImg[i][j+2];
					p4 = oldImg[i+1][j]; p  = oldImg[i+1][j+1]; p5 = oldImg[i+1][j+2];
					p6 = oldImg[i+2][j]; p7 = oldImg[i+2][j+1]; p8 = oldImg[i+2][j+2];
					remove = false;
					//int substep=4;
					if(p==0)
						continue;

					switch(substep){
					case 1:	if(p4 == 1 && p == 1 && p5 == 0)	// template (a) -- p4=1, p=1 and p5=0
							{					
								if((p2 == 0 && p3 == 1) ||		// p is a critical pixel
								   (p7 == 0 && p8 == 1))
									break;
								else
								{
									if((p1+p2+p3+p6+p7+p8) == 0)		// p is an end pixel
										break;
									else
										remove = true;
								}
							}
						break;
					case 2:	if(p4 == 0 && p == 1 && p5 == 1)	// template (c) -- p4=0, p=1 and p5=1
							{
								if((p2 == 0 && p1 == 1) ||		// p is a critical pixel
								   (p7 == 0 && p6 == 1))
									break;
								else
								{
									if((p1+p2+p3+p6+p7+p8) == 0)		// p is an end pixel
										break;
									else
										remove = true;
								}
							}
						break;
					case 3:	if(p2 == 0 && p == 1 && p7 == 1)	// template (b) -- p2=0, p=1 and p7=1
							{
								if((p4 == 0 && p1 == 1) ||		// p is a critical pixel
								   (p5 == 0 && p3 == 1))
									break;
								else
								{
									if((p1+p3+p4+p5+p6+p8) == 0)		// p is an end pixel
										break;
									else
										remove = true;
								}
							}
						break;
					case 4:	if(p2 == 1 && p == 1 && p7 == 0)	// template (d) -- p2=1, p=1 and p7=0
							{
								if((p4 == 0 && p6 == 1) ||		// p is a critical pixel
								   (p5 == 0 && p8 == 1))
									break;
								else
								{
									if((p1+p3+p4+p5+p6+p8) == 0)		// p is an end pixel
										break;
									else
										remove = true;
								}
							}
						break;
					case 5:	if(p1 == 1 && p == 1 && p8 == 0)	// template (d) -- p1=1, p=1 and p8=0
							{
								if((p4 == 0 && p6 == 1) ||		// p is a critical pixel
								   (p2 == 0 && p3 == 1) ||
								   (p2==0 && p3==0 && p5==1) ||
								   (p4==0 && p6==0 && p7==1) )
									break;
								else
								{
									if((p2+p3+p4+p5+p6+p7) == 0)		// p is an end pixel
										break;
									else
										remove = true;
								}
							}
						break;
					case 6:	if(p3 == 1 && p == 1 && p6 == 0)	// template (d) -- p3=1, p=1 and p6=0
							{
								if((p2 == 0 && p1 == 1) ||		// p is a critical pixel
								   (p5 == 0 && p8 == 1) ||
								   (p2==0 && p1==0 && p4==1) ||
								   (p5==0 && p8==0 && p7==1) )
									break;
								else
								{
									if((p1+p2+p4+p5+p7+p8) == 0)		// p is an end pixel
										break;
									else
										remove = true;
								}
							}
						break;
					case 7:	if(p6 == 1 && p == 1 && p3 == 0)	// template (d) -- p6=1, p=1 and p3=0
							{
								if((p4 == 0 && p1 == 1) ||		// p is a critical pixel
								   (p7 == 0 && p8 == 1) ||
								   (p4==0 && p1==0 && p2==1) ||
								   (p7==0 && p8==0 && p5==1) )
									break;
								else
								{
									if((p1+p2+p4+p5+p7+p8) == 0)		// p is an end pixel
										break;
									else
										remove = true;
								}
							}
						break;
					case 8:	if(p8 == 1 && p == 1 && p1 == 0)	// template (d) -- p8=1, p=1 and p1=0
							{
								if((p5 == 0 && p3 == 1) ||		// p is a critical pixel
								   (p7 == 0 && p6 == 1) ||
								   (p5==0 && p3==0 && p2==1) ||
								   (p7==0 && p6==0 && p4==1) )
									break;
								else
								{
									if((p2+p3+p4+p5+p6+p7) == 0)		// p is an end pixel
										break;
									else
										remove = true;
								}
							}
						break;
					}		// end of switch()
					if(remove) // && (newImg[i+1][j+1] == 1)) 		// p is removable
					{
						newImg[i+1][j+1] = 0;
						nChanges++;
					}
				}
			}
			for(int i = 0; i < height+2; i++)
			{
				for(int j = 0; j < width+2; j++) oldImg[i][j] = newImg[i][j];
			}
		}		// end of step1

		//show the imgae
			for(i=0;i<height-1;i++)
			{
				for(j=0;j<width-1;j++) {
					thinImg[i][j] = newImg[i+1][j+1];
					if(thinImg[i][j] == 2) thinImg[i][j] = 1;
					int pix;
					if(thinImg[i][j]==0)
						pix=255;
					else
						pix=0;
					for(k=0;k<channels;k++)
						data[i*step+j*channels+k]=pix;
				}
			}
		//display("Steps2", img );
		//init=1;
		//final=4;

	}while(nChanges > 0);
	for(i = 0; i < height; i++)
	{
		for(j = 0; j < width; j++)
		{
			thinImg[i][j] = newImg[i+1][j+1];
			if(thinImg[i][j] == 2) thinImg[i][j] = 1;
		}
	}

//	write2file(thinImg,height,width,"output.txt");

	for(i=0;i<height-1;i++)
	{
		for(j=0;j<width-1;j++) {
			imgMat[i][j]=thinImg[i][j];
			int pix;
			if(thinImg[i][j]==0)
			{
				pix=255;
				mat[i][j]='1';
			}
			else
			{
				pix=0;
				mat[i][j]='0';
			}
			for(k=0;k<channels;k++)
				data[i*step+j*channels+k]=pix;
		}
	}
	delete(oldImg); delete(newImg); delete(thinImg);
	//ImageDump(0,0,height,width);
}

/**
* Converts an image into a binary image according to a given threshold
* threshold: the given threshold
*/
void Thresholding(IplImage* img, int threshold){
	int height,width,step,channels;
	uchar *data;

    // get the image data
    height    = img->height;
    width     = img->width;
    step      = img->widthStep;
    channels  = img->nChannels;
    data      = (uchar *)img->imageData;

	int i,j,k;

	for(i=0;i<height;i++)
		for(j=0;j<width;j++){
			int count=0;
			for(k=0;k<channels;k++){
				if(data[i*step+j*channels+k]>threshold)
					count++;
			}
			if(count==channels)
				for(k=0;k<channels;k++)
					data[i*step+j*channels+k]=255;
			else
				for(k=0;k<channels;k++)
					data[i*step+j*channels+k]=0;
		}

}

/**
* Any pixiel outside the red rectangle is converted to white pixel
*/
void crop(IplImage* img, point* p1, point* p2, int red_threshold){
	int height,width,step,channels;
	uchar *data;

    // get the image data
    height    = img->height;
    width     = img->width;
    step      = img->widthStep;
    channels  = img->nChannels;
    data      = (uchar *)img->imageData;
	
	int i,j,k;
	
	p1->display();
	p2->display();
	for(i=0;i<height;i++)
		for(j=0;j<width;j++)
		for(k=0;k<channels;k++){
			if(i<p1->get_X()+35 || i>p2->get_X()-35 || j<p1->get_Y()+35 || j>p2->get_Y()-35)
				data[i*step+j*channels+k]=255;


		}

}


/**
* Detects the red rectangle and crops the image accordingly
*/
void BoundaryDetection(IplImage* img, int red_threshold, int green_thres, int blue_thres){
	int height,width,step,channels;
	uchar *data;

    // get the image data
    height    = img->height;
    width     = img->width;
    step      = img->widthStep;
    channels  = img->nChannels;
    data      = (uchar *)img->imageData;
	
	int i,j;

	point p1=point(20,100),p2=point(height-70,width-100);
	crop(img,&p1,&p2,red_threshold);
	display("Cropped",img);
}


/**
* Displays the image img in the window specified
*/
void display (char* window_name, IplImage* img)
{
	cvShowImage(window_name, img );
	cvWaitKey(0);
}



list<point> joining_collinear_points(list<point> path){
	list<point>::iterator it=path.begin();
	list<point> finalPath;

	finalPath.push_back(*it);

	int x1,x2,x3,y1,y2,y3;
	x1=it->get_X();
	y1=it->get_Y();
	it++;
	int i,size=path.size();
	for(i=1;i<size-1;){
		x2=it->get_X();
		y2=it->get_Y();
		double m1=((double)(y2-y1))/((double)(x2-x1));
		const point p=*it;
		it++;
		i++;
		x3=it->get_X();
		y3=it->get_Y();
		double m2=((double)(y3-y1))/((double)(x3-x1));

		if( (m2==m1) || (m1==m2) ){
		}
		else{
			finalPath.push_back(p);
			finalPath.push_back(*it);
			x1=x2;
			y1=y2;
		}


	}


	return finalPath;
}
