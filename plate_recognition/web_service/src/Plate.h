/*
 * Plate.h
 *
 *  Created on: Jan 27, 2018
 *      Author: Safwen Bouanen
 */

#ifndef SRC_PLATE_H_
#define SRC_PLATE_H_

#include "opencv2/core/types.hpp"
#include "opencv2/core/mat.hpp"

using namespace cv;

class Plate {
public:
	Plate();
	Plate(Mat image, Rect rect);
	virtual ~Plate();

	Mat m_image;
	Rect m_rect;
};

#endif /* SRC_PLATE_H_ */
