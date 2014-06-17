/**
 * @file includes.h
 * cs684 course project: The project duplicates a black and white image
 * read through the camera and redraws it on a chart paper/surface.
 * This piece of code, includes all the header files needed to include in the 
 * VC++ (image processing) part of the project.
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
 

#include <iostream>
#include <assert.h>
#include <fstream>
#include <list>
#include <cv.h>
#include <highgui.h>
#include <math.h>
#include <atlstr.h>

#include "point.h"
#include "vector_extract.h"
#include "operations.h"
#include "stdafx.h"
#include "serialcomm.h"
