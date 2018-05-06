#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include "opencv2/core.hpp"
#include <opencv2/ml.hpp>
#include "Image.hpp"
#include <iostream>
#include <vector>
#include <time.h>
#include "Plate.h"
// basic file operations
#include <fstream>


using namespace cv;
using namespace std;
using namespace cv::ml;


Image::Image() {
	// TODO Auto-generated constructor stub

}

Image::~Image() {
	// TODO Auto-generated destructor stub
}


bool Image::verifySizes(RotatedRect candidate )
{
	float error=0.4;
	//Spain car plate size: 52x11 aspect 4,7272
	//const float aspect=4.7272;
	//California car plate size: 12(inch)x6(inch) aspect 2
	const float aspect=2.5;
	//Set a min and max area. All other patches are discarded
	//int min= 15*aspect*15; // minimum area
	//int max= 125*aspect*125; // maximum area
	int min= 15*aspect*15; // minimum area
	//int max= 74*aspect*74; // maximum area
	int max= 125*aspect*125; // maximum area
	//Get only patches that match to a respect ratio.
	float rmin= aspect-aspect*error;
	float rmax= aspect+aspect*error;
	int area= candidate.size.height * candidate.size.width;
	float r= (float)candidate.size.width / (float)candidate.size.height;
	if(r<1)
		r= 1/r;
	if(( area < min || area > max ) || ( r < rmin || r > rmax ))
	{
		return false;
	}
	else
	{
		return true;
	}

}


vector<Plate> Image::locatePlate(Mat image )
		{
		    Mat img_gray;
			cvtColor(image, img_gray, CV_BGR2GRAY);
			blur(img_gray, img_gray, Size(5,5));

			Mat img_sobel;
			Sobel(img_gray, img_sobel, CV_8U, 1, 0, 3, 1, 0);

			Mat img_threshold;
			threshold(img_sobel, img_threshold, 0, 255, CV_THRESH_OTSU+CV_THRESH_BINARY);

			Mat element = getStructuringElement(MORPH_RECT, Size(17, 3));
			//Mat element = getStructuringElement(MORPH_RECT, Size(10, 4));

			morphologyEx(img_threshold, img_threshold, CV_MOP_CLOSE, element);

			//Find contours of possibles plates
			vector<vector<Point>> contours;

			findContours(img_threshold,
						 contours, // a vector of contours
						 CV_RETR_EXTERNAL, // retrieve the external contours
						 CV_CHAIN_APPROX_NONE); // all pixels of each contour

			Mat imageContours = image.clone();
			//Mat drawing = Mat::zeros( image.size(), CV_8UC3 );
			Scalar color = Scalar( 255, 255, 255 );
			drawContours( imageContours, contours, -1, color, 1, 8);//, vector<Vec4i>(), 0, Point() );
			namedWindow( "image1", WINDOW_AUTOSIZE );// Create a window for display.
			imshow( "image1", imageContours);


			//Start to iterate to each contour found
			vector<vector<Point> >::iterator itc= contours.begin();
			vector<RotatedRect> rects;
			Mat image2 = image.clone();
			//Remove patch that has no inside limits of aspect ratio and area.
			while (itc!=contours.end()) {
			//Create bounding rect of object
				RotatedRect mr= minAreaRect(Mat(*itc));

/*				// rotated rectangle
				Point2f rect_points[4];
				mr.points( rect_points );
				for( int j = 0; j < 4; j++ )
					line( imageContours, rect_points[j], rect_points[(j+1)%4], Scalar( 255, 255, 0 ), 1, 8 );
*/

				if( !verifySizes(mr))
				{
					itc= contours.erase(itc);
				}
				else
				{
					++itc;
					rects.push_back(mr);

					// rotated rectangle
					Point2f rect_points[4];
					mr.points( rect_points );
					for( int j = 0; j < 4; j++ )
						line( imageContours, rect_points[j], rect_points[(j+1)%4], Scalar( 255, 255, 0 ), 1, 8 );

				}
			}

			namedWindow( "image2", WINDOW_AUTOSIZE );// Create a window for display.
			imshow( "image2", imageContours);

			vector<Mat> mask;

			for(int i=0; i< rects.size(); i++)
			{
				//For better rect cropping for each possible box
				//Make floodfill algorithm because the plate has white background
				//And then we can retrieve more clearly the contour box
				Mat result; // added
				////circle(img_threshold, rects[i].center, 3, Scalar(0,255,0), -1);
				//circle(image, rects[i].center, 10, Scalar(0,255,0), -1);

				//get the min size between width and height
				float minSize = (rects[i].size.width < rects[i].size.height)?rects[i].size.width:rects[i].size.height;
				minSize = minSize - minSize*0.5;

				//initialize rand and get 5 points around center for floodfill algorithm
				srand ( time(NULL) );
				//Initialize floodfill parameters and variables
				Mat output_mask;  //Added
				output_mask.create(image.rows + 2, image.cols + 2, CV_8UC1);
				output_mask = Scalar::all(0);
				int loDiff = 30;
				int upDiff = 30;
				int connectivity = 4;
				int newMaskVal = 255;
				int NumSeeds = 1000;
				Rect ccomp;

				int flags = connectivity + (newMaskVal << 8 ) + CV_FLOODFILL_FIXED_RANGE + CV_FLOODFILL_MASK_ONLY;
				for(int j=0; j<NumSeeds; j++){
					Point seed;
					seed.x=rects[i].center.x+rand()%(int)minSize-(minSize/2);
					seed.y=rects[i].center.y+rand()%(int)minSize-(minSize/2);
					//circle(result, seed, 1, Scalar(0,255,255), -1);
					//circle(image, seed, 1, Scalar(0,255,255), -1);
					int area = floodFill(image, output_mask, seed, Scalar(255,0,0), &ccomp,
										 Scalar(loDiff, loDiff, loDiff), Scalar(upDiff, upDiff, upDiff), flags);
				}
				mask.push_back(output_mask);

				//imshow( std::to_string(i), mask[i]);
				//imshow("image", mask[i]);
			}

			vector<Plate> output;
			Mat imagePlate = image.clone();

			for(int j=0; j< mask.size(); j++)
			{
			//Check new floodfill mask match for a correct patch.
			//Get all points detected for minimal rotated Rect
				vector<Point> pointsInterest;
				Mat_<uchar>::iterator itMask= mask[j].begin<uchar>();
				Mat_<uchar>::iterator end= mask[j].end<uchar>();
				for( ; itMask!=end; ++itMask){
					if(*itMask==255)
					pointsInterest.push_back(itMask.pos());
				}
				RotatedRect minRect = minAreaRect(pointsInterest);

				if(verifySizes(minRect)){

					// rotated rectangle
					Point2f rect_points[4];
					minRect.points( rect_points );
					for( int j = 0; j < 4; j++ )
						line( imagePlate, rect_points[j], rect_points[(j+1)%4], color, 1, 8 );


					cout << "region" << endl;
					//Get rotation matrix
					float r= (float)minRect.size.width / (float)minRect.size.height;
					float angle=minRect.angle;
					if(r<1)
						angle=90+angle;

					Mat rotmat= getRotationMatrix2D(minRect.center, angle,1);

					//Create and rotate image
					Mat img_rotated;
					warpAffine(image, img_rotated, rotmat, image.size(), CV_INTER_CUBIC);

					//Crop image
					Size rect_size=minRect.size;
					if(r < 1)
						swap(rect_size.width, rect_size.height);
					Mat img_crop;
					getRectSubPix(img_rotated, rect_size, minRect.center, img_crop);
/*				    imshow( "image_crop" + std::to_string(j), img_crop );                   // Show our image inside it.*/

				    Mat resultResized;
				    //resultResized.create(33,144, CV_8UC3);
				    resultResized.create(46,85, CV_8UC3);
				    resize(img_crop, resultResized, resultResized.size(), 0, 0, INTER_CUBIC);
				    //Equalize cropped image
				    Mat grayResult;
				    cvtColor(resultResized, grayResult, CV_BGR2GRAY);
				    blur(grayResult, grayResult, Size(3,3));
				    equalizeHist(grayResult, grayResult);

/*				    FileStorage fs("file.xml", FileStorage::APPEND);
				    fs << "grayResult" + to_string(j) << grayResult;*/


				    //cout << grayResult << endl;
				    /*cv::MatIterator_<int> _it = grayResult.begin<int>();
				    for(;_it!=grayResult.end<int>(); _it++){
				        std::cout << *_it << std::endl;
				    }*/
					//cout << grayResult[1][1] << endl;
/*				    imshow( "image_gray" + std::to_string(j), grayResult );*/
				    output.push_back(Plate(grayResult,minRect.boundingRect()));

				}
				else
				{
					cout << "not region" << endl;
				}
			}

			namedWindow( "imageContours1", WINDOW_AUTOSIZE );// Create a window for display.
			imshow( "imageContours1", imagePlate);
/*			namedWindow( "image1", WINDOW_AUTOSIZE );// Create a window for display.
			imshow( "image1", image1);
		    fs.release();*/
			return output;

		}





