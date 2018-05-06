/*
 * OCR.hpp
 *
 *  Created on: Feb 19, 2018
 *      Author: Safwen Bouanen
 */

#ifndef SRC_OCR_HPP_
#define SRC_OCR_HPP_

#include "opencv2/core/types.hpp"
#include "opencv2/core/mat.hpp"
#include "Plate.h"
#include <vector>


class OCR {
public:
	OCR();
	virtual ~OCR();
	static bool verifyArea(int width, int heigh);
	static bool verifySizes(Mat r);
};



#endif /* SRC_OCR_HPP_ */
