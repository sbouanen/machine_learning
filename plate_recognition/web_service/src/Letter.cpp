/*
 * Letter.cpp
 *
 *  Created on: Mar 4, 2018
 *      Author: Safwen Bouanen
 */

#include "Letter.hpp"



Letter::Letter() {
	// TODO Auto-generated constructor stub

}

Letter::~Letter() {
	// TODO Auto-generated destructor stub
}

void Letter::CalculateHorizontalPosition(Mat img)
{
	float mean_hor_pos = 0;
	cout << "img.cols = " << img.cols << endl;
	cout << "img.rows = " << img.rows << endl;
	cout << "center = " << (img.cols/2.0) << endl;
	cout << "type = "  << img.type() << endl;
	//cout << img << endl;
	for(int j=0; j < img.rows; j++)
	{
		for (int k=0; k < img.cols; k++)
		{
			//cout << (int)(img.at<uchar>(j,k)) << endl;
			if ((int)img.at<uchar>(j,k) != 0)
			{
				//cout << "255" << endl;
				mean_hor_pos = mean_hor_pos + (k - (img.cols/2.0))/img.cols;
			}
		}
	}
	cout << "mean_hor_pos = " << mean_hor_pos << endl;
}
