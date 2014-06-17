Project: Vector Image extraction and Autonomous Duplication by Firebird V
--------------------------------------------------------------------------

Team:
--------
Abhirup Ghosh        09305052  
Anirban Basumallik   09305008  
Gourab Roy           09305037 

How to run:
-----------
1. After compiling project.c to hex, burn it on FireBird V.
2. Open the visual studio solution in VC++
3. Mention the correct COM port of serial communication in line number 78 of
file : Final_project/CS684_course_project_group5/serialcomm.h
4. Run the application/ Debug. On execution of that project it will capture an
image through the web-cam connected to its usb port.

NB: OpenCV library needs to be installed on Windows and the project should be
linked correctly to the installed libraries following the instructions
mentioned at the OpenCV installation webpage: 
http://opencv.willowgarage.com/wiki/VisualC%2B%2B

Software Requirements:
----------------------
1. Microsoft Visual Studio 2008:
2. OpenCV: for image acquisition and image processing and displaying
3. AVR Studio 4 : IDE for programming AVR micro-controllers

Hardware Requirements:
----------------------
1. Firebird V
2. AVR ISP mkII programmer
3. Servo mounting hardware to attach the pen on top of the bot
4. A PC with good image processing power like C2D processor, 2GB RAM

Code Files.
-----------
Filename         Purpose                            Executes on

Main.cpp         Main Program                       PC

point.h          Defines a 2D point class           PC

operations.h     Defines image processing           PC
                 functionalities like thinning
                 thresholding etc
                 
vector_extract.h Defines functionalities to extract PC
                 vector path from the thinned
                 image
                 
includes.h       Specify all your includes in this  PC
                 file.
                 
serialcomm.h     Defines serial communication       PC
                 functionalities
                 
project.c        Sends data and receives and        FireBird V
                 executes instructions from PC
                 
File Hierarchy
--------------
artistBot
|
|-readme.txt //Intructions regarding the project and the project folder
|-docs //Doxygen generated documentation
|	|-docs_FBV
|	|-docs_VC
|-Documentation and SRS // SRS and other project related information
|	|-CS684_Documentation.pdf
|	|-group5_SRS.pdf
|-ppt // Project presentations
|	|-12_nov_2010_Group_5_1.pptx
|	|-28_sep_2010_Group_5.pps
|-src // Source Code
|	|-FBV
|	|-VC
|-video: //Project Demo Video
|	|-Team5.mp4

