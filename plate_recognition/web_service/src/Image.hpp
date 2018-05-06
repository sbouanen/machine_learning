/*
 * Image.hpp
 *
 *  Created on: Feb 6, 2018
 *      Author: Safwen Bouanen
 */

#ifndef SRC_IMAGE_HPP_
#define SRC_IMAGE_HPP_

#include "opencv2/core/types.hpp"
#include "opencv2/core/mat.hpp"
#include "Plate.h"
#include <vector>


class Image {
public:
	Image();
	virtual ~Image();
	std::vector<Plate> locatePlate(Mat image);
	bool verifySizes(RotatedRect candidate );
};


#endif /* SRC_IMAGE_HPP_ */
