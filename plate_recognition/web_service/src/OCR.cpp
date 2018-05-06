/*
 * OCR.cpp
 *
 *  Created on: Feb 19, 2018
 *      Author: Safwen Bouanen
 */
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include "opencv2/core.hpp"
#include <opencv2/ml.hpp>
#include "OCR.hpp"
#include <iostream>
#include <vector>
#include <time.h>
#include "Plate.h"
// basic file operations
#include <fstream>


using namespace cv;
using namespace std;
using namespace cv::ml;


OCR::OCR() {
	// TODO Auto-generated constructor stub

}

OCR::~OCR() {
	// TODO Auto-generated destructor stub
}

bool OCR::verifyArea(int width, int heigh)
{
 //Char sizes 45x77
 //float aspect=45.0f/77.0f;
 float aspect=25.4f/65.0875f;
 float error=0.4;
 //float minHeight=15;
 float minHeight=13;
 //float maxHeight=28;
 float maxHeight=24;

 //We have a different aspect ratio for number 1, and it can be
 //~0.2
 float minArea=minHeight*aspect*minHeight;
 minArea=(1-error)*minArea;

 float maxArea=maxHeight*aspect*maxHeight;
 maxArea=(1+error)*maxArea;

 //area of pixels
 float area=width*heigh;

 if(area > minArea && area < maxArea)
	 return true;
 else
	 return false;
}


bool OCR::verifySizes(Mat r)
{
 //Char sizes 45x77
 //float aspect=45.0f/77.0f;
 float aspect=25.4f/65.0875f;
 float charAspect= (float)r.cols/(float)r.rows;
 float error=0.5;
 //float error=0.35;
 //float minHeight=15;
 float minHeight=13;
 //float maxHeight=28;
 float maxHeight=24;

 //We have a different aspect ratio for number 1, and it can be
 //~0.2
 float minAspect=0.005;
 float maxAspect=aspect+aspect*error;
 //area of pixels
 float area=countNonZero(r);
 //bb area
 float bbArea=r.cols*r.rows;
 //% of pixel in area
 float percPixels=area/bbArea;
 if(percPixels < 0.8 && charAspect > minAspect && charAspect < maxAspect && r.rows >= minHeight && r.rows < maxHeight)
	 return true;
 else
	 return false;
}


