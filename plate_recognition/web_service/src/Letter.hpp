/*
 * Letter.hpp
 *
 *  Created on: Mar 4, 2018
 *      Author: Safwen Bouanen
 */

#ifndef SRC_LETTER_HPP_
#define SRC_LETTER_HPP_

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include "Image.hpp"
#include "OCR.hpp"
#include <iostream>
#include <stdlib.h>
#include <string.h>

using namespace std;

class Letter {
public:
	Letter();
	virtual ~Letter();
	void CalculateHorizontalPosition(Mat img);
};

#endif /* SRC_LETTER_HPP_ */
