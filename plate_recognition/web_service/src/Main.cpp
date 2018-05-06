//============================================================================
// Name        : Main.cpp
// Author      : Safwen Bouanen
// Version     :
// Copyright   : Your copyright notice
// Description :
//============================================================================
#include "soapH.h"  // include the generated source code headers
#include "ns.nsmap" // include XML namespaces
#include "httpget.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/ml/ml.hpp>
#include "Image.hpp"
#include "OCR.hpp"
#include "Letter.hpp"
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <algorithm>



using namespace cv;
using namespace std;


typedef struct
{
	Point2f center;
	Mat digit;
}digitMat_t;


/*Quicksort algorithm implementation*/
int partition(vector<digitMat_t>& A, int lo, int hi)
{
	digitMat_t pivot = A[hi];
	int i = lo - 1;

    for (int j = lo;  j < hi ; j++)
    {
        if (A[j].center.x < pivot.center.x)
		{
            i = i + 1;
            std::swap(A[i], A[j]);
            digitMat_t tmp;
		}
    }
    std::swap(A[i + 1], A[hi]);
    return (i + 1);
}

void quicksort(vector<digitMat_t>& A, int lo, int hi)
{
	int p = 0;
    if (lo < hi)
    {
        p = partition(A, lo, hi);
        quicksort(A, lo, p - 1 );
        quicksort(A, p + 1, hi);
    }
}



int copy_file(struct soap *soap, const char *name, const char *type)
{ FILE *fd;
  size_t r;
  fd = fopen(name, "rb"); /* open file to copy */
  if (!fd)
    return 404; /* return HTTP not found */
  soap->http_content = type;
  if (soap_response(soap, SOAP_FILE)) /* OK HTTP response header */
  { soap_end_send(soap);
    fclose(fd);
    return soap->error;
  }
  for (;;)
  { r = fread(soap->tmpbuf, 1, sizeof(soap->tmpbuf), fd);
    if (!r)
      break;
    if (soap_send_raw(soap, soap->tmpbuf, r))
    { soap_end_send(soap);
      fclose(fd);
      return soap->error;
    }
  }
  fclose(fd);
  return soap_end_send(soap);
}


int http_GET_handler(struct soap *soap)
{
  /* Use soap->path (from request URL) to determine request: */
  /* we don't like requests to snoop around in dirs, so we must reject request that have paths with a '/' or a '\'. You must at least check for .. to avoid request from snooping around in higher dirs!!! */
  /* Note: soap->path always starts with '/' so we chech path + 1 */
  if (strchr(soap->path + 1, '/') || strchr(soap->path + 1, '\\'))
    return 403; /* HTTP forbidden */
  if (!soap_tag_cmp(soap->path, "*.html"))
    return copy_file(soap, soap->path + 1, "text/html");
  if (!soap_tag_cmp(soap->path, "*.xml"))
    return copy_file(soap, soap->path + 1, "text/xml");
  if (!soap_tag_cmp(soap->path, "*.jpg"))
    return copy_file(soap, soap->path + 1, "image/jpeg");
  if (!soap_tag_cmp(soap->path, "*.gif"))
    return copy_file(soap, soap->path + 1, "image/gif");
  if (!soap_tag_cmp(soap->path, "*.png"))
    return copy_file(soap, soap->path + 1, "image/png");
  if (!soap_tag_cmp(soap->path, "*.ico"))
    return copy_file(soap, soap->path + 1, "image/ico");

  /* For example, we can put WSDL and XSD files behind authentication wall */
  if (!soap_tag_cmp(soap->path, "*.xsd")
   || !soap_tag_cmp(soap->path, "*.wsdl"))
    return copy_file(soap, soap->path + 1, "text/xml");

  return 404; /* HTTP not found */
}


int main()
{
   struct soap soap;
   int m, s; // master and slave sockets
   soap_init(&soap);

   /*************************************/
   soap_register_plugin_arg(&soap, http_get, (void*)http_GET_handler);

   m = soap_bind(&soap, NULL, 18083, 100);
   if (m < 0)
      soap_print_fault(&soap, stderr);
   else
   {
      fprintf(stderr, "Socket connection successful: master socket = %d\n", m);
      for (int i = 1; ; i++)
      {
         s = soap_accept(&soap);
         if (s < 0)
         {
            soap_print_fault(&soap, stderr);
            break;
         }
         fprintf(stderr, "%d: accepted connection from IP=%d.%d.%d.%d socket=%d\n", i,
            (soap.ip >> 24)&0xFF, (soap.ip >> 16)&0xFF, (soap.ip >> 8)&0xFF, soap.ip&0xFF, s);
         if (soap_serve(&soap) != SOAP_OK)	// process RPC request
            soap_print_fault(&soap, stderr); // print error
         fprintf(stderr, "request served\n");
         soap_destroy(&soap);	// clean up class instances
         soap_end(&soap);	// clean up everything and close socket
      }
   }
   soap_done(&soap); // close master socket and detach context
}


int ns__ocr(soap* sp, int* image, int rows, int cols, int* status)
{
/*************************prediction algorithm**************************/
/**********************************************************************/

	uint8_t image_uchar[rows*(cols+1)];

    int p = 0;
    while (p < rows*(cols+1))
    {
    	for (int q = 0; q < cols+1; q++)
    	{
    		image_uchar[p] = (uint8_t)((0xFF & image[p+48]));
    		p++;
    	}
    }

	cv::Mat imgCandidate(rows, cols+1, CV_8UC1, image_uchar, cv::Mat::AUTO_STEP);

/***********************************************************************/
	Mat img_threshold;
	threshold(imgCandidate, img_threshold, 60, 255, CV_THRESH_BINARY_INV);

	imshow("Threshold plate", img_threshold);
	Mat img_contours;
	img_threshold.copyTo(img_contours);

	//Find contours of possibles characters
	vector<vector<Point>> contours;
	findContours(img_contours,
				 contours, // a vector of contours
				 CV_RETR_EXTERNAL, // retrieve the external contours
				 CV_CHAIN_APPROX_NONE); // all pixels of each contour

	vector<vector<Point>>::iterator itc= contours.begin();
	vector<RotatedRect> rects;


/***********************************************************************/
	while (itc!=contours.end())
	{
		//Create bounding rect of object
		RotatedRect mr= minAreaRect(Mat(*itc));
		++itc;

		//Get rotation matrix
		float r= (float)mr.size.width / (float)mr.size.height;

		if ((mr.center.y < (img_contours.rows/2)*(1 + 0.5)) && (mr.center.y > (img_contours.rows/2)*(1 - 0.5)))
		{
			if(OCR::verifyArea(mr.size.width, mr.size.height))
			{
				//Crop image
				if(r > 1)
					swap(mr.size.width, mr.size.height);

				rects.push_back(mr);
			}
		}
	}

	float pHeight = 0.0;
	float y_rect = 0.0;
	vector<RotatedRect>::iterator iRec = rects.begin();
	while (iRec != rects.end())
	{
		y_rect = y_rect + iRec->center.y/rects.size();
		pHeight = pHeight + iRec->size.height/rects.size();
		iRec++;
	}
	Mat Dst (img_threshold, Rect(0, (int)(y_rect/2)+2, img_contours.cols, (int)(pHeight)+4) );
	imshow("dst", Dst);

/***********************************************************************/
	vector<vector<Point>> contours1;

	findContours(Dst,
				 contours1, // a vector of contours
				 CV_RETR_EXTERNAL, // retrieve the external contours
				 CV_CHAIN_APPROX_NONE); // all pixels of each contour
	itc= contours1.begin();

	Mat dst_rgb(Dst.size(), CV_8UC3);
	cv::cvtColor(Dst, dst_rgb, CV_GRAY2RGB);

	vector<digitMat_t> digits;

	//Remove patch that has no inside limits of aspect ratio and area.
	int i = 0;
	while (itc!=contours1.end())
	{
	//Create bounding rect of object
		RotatedRect mr= minAreaRect(Mat(*itc));
		++itc;
		rects.push_back(mr);

		// rotated rectangle
		Point2f rect_points[4];
		mr.points( rect_points );
		for( int j = 0; j < 4; j++ )
			line( dst_rgb, rect_points[j], rect_points[(j+1)%4], Scalar( 0, 0, 255 ), 1, 8 );
		imshow( "dst_rgb", dst_rgb);


		//Get rotation matrix
		float r= (float)mr.size.width / (float)mr.size.height;
		float angle=mr.angle;
		if(r>1)
			angle=90+angle;

		//if ((mr.center.y < (img_contours.rows/2)*(1 + 0.5)) && (mr.center.y > (img_contours.rows/2)*(1 - 0.5)))
		//{
			if(OCR::verifyArea(mr.size.width, mr.size.height))
			{
				Mat rotmat= getRotationMatrix2D(mr.center, angle,1);

				//Create and rotate image
				Mat img_rotated;
				warpAffine(Dst, img_rotated, rotmat, Dst.size(), CV_INTER_CUBIC);

				//Crop image
				Size rect_size=mr.size;
				if(r > 1)
					swap(rect_size.width, rect_size.height);
				Mat img_crop;

				getRectSubPix(img_rotated, rect_size, mr.center, img_crop);


				Mat resultResized;
				resultResized.create(27,10, CV_8U);
				resize(img_crop, resultResized, resultResized.size(), 0, 0, INTER_CUBIC);

				digitMat_t letDig;
				letDig.center = mr.center;
				letDig.digit = resultResized;
				digits.push_back(letDig);

				//imshow( "resultResized" + std::to_string(i), resultResized );
			}
			i++;
		//}

	}

	quicksort(digits, 0, digits.size() - 1);
	vector<digitMat_t>::iterator itd = digits.begin();

	int n = 0;
	while (itd!=digits.end())
	{
		imshow( "element" + std::to_string(n), digits[n].digit);
		itd++;
		n++;
	}

	Ptr<cv::ml::ANN_MLP> ann_ml = ml::ANN_MLP::load("ANN_Digits_Model.xml");

	int u = 0;
	int pos[7] = {0, -1, -1, -1, 4, 5, 6}; // indexes of numerical digits within the plate

	cout << "\nPredicted Registration Identifier: ";

	while (u < 7)
	{
		if (pos[u] != -1)
		{
			Mat imgCandidate = digits[pos[u]].digit;   // Read the file
			imshow( "digit" + std::to_string(u), imgCandidate );

			Mat p= imgCandidate.reshape(1, 1);//convert img to 1 row m features
			p.convertTo(p, CV_32FC1);
			Mat result(1, 10, CV_32FC1);
			int response = ann_ml->predict( p, result );
			cout << response;
		}
		else
		{
			cout << "X";
		}

		u++;
	}
	cout << "\n";

/****************************************************************/
/****************************************************************/
	waitKey(0);
	return SOAP_OK;
}
