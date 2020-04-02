/* Applied Video Sequence Analysis (AVSA)
 *
 *	LAB1.0: Background Subtraction - Unix version
 *	fgesg.cpp
 *
 * 	Authors: José M. Martínez (josem.martinez@uam.es), Paula Moral (paula.moral@uam.es) & Juan Carlos San Miguel (juancarlos.sanmiguel@uam.es)
 *	VPULab-UAM 2020
 */

#include <opencv2/opencv.hpp>
#include "fgseg.hpp"

using namespace fgseg;
using namespace cv;
//default constructor
bgs::bgs(double threshold, bool rgb){
	_rgb = rgb ;
    _threshold = threshold;

}

//default destructor
bgs::~bgs(void)
{

}

//method to initialize bkg (first frame - hot start)
void bgs::init_bkg(cv::Mat Frame)
{

	if (!_rgb){
		cvtColor(Frame, Frame, COLOR_BGR2GRAY); // to work with gray even if input is color
		_bkg = Mat::zeros(Size(Frame.cols,Frame.rows), CV_8UC1); // void function for Lab1.0 - returns zero matrix//ADD YOUR CODE HERE
		//...
		Frame.copyTo(_bkg) ;
		//initialize sigma and mue
		mue = Mat::zeros(Size(Frame.cols, Frame.rows),CV_8UC1);
		sigma = Mat::zeros(Size(Frame.cols,Frame.rows), CV_8UC1);
		Frame.copyTo(mue);
		double low = 20,high =30;
		randu(sigma,Scalar(low),Scalar(high));
		sigma = sigma.mul(sigma);
		//...
	}
	else{

		cout << "Color currently not supported" << endl;
		exit(1);
	}
}


//method to perform BackGroundSubtraction
void bgs::bkgSubtraction(cv::Mat Frame)
{
	if (!_rgb){
		cvtColor(Frame, Frame, COLOR_BGR2GRAY); // to work with gray even if input is color
		_frame = Mat::zeros(Size(Frame.cols,Frame.rows), CV_8UC1); // void function for Lab1.0 - returns zero matrix
		_diff = Mat::zeros(Size(Frame.cols,Frame.rows), CV_8UC1); // void function for Lab1.0 - returns zero matrix
		_bgsmask = Mat::zeros(Size(Frame.cols,Frame.rows), CV_8UC1); // void function for Lab1.0 - returns zero matrix
		//ADD YOUR CODE HERE
		//...
		Frame.copyTo(_frame);
		absdiff(_frame, _bkg, _diff); // the absolute difference between current frame and the background assigned to _diff
		threshold(_diff,_bgsmask,_threshold,255,CV_THRESH_BINARY);
	}
	else{
		cout << "Color currently not supported" << endl;
		exit(1);
	}
}

//method to detect and remove shadows in the BGS mask to create FG mask
void bgs::removeShadows()
{
	if (!_rgb){
		_shadowmask = Mat::zeros(Size(_bgsmask.cols,_bgsmask.rows), CV_8UC1); // void function for Lab1.0 - returns zero matrix
		_fgmask = Mat::zeros(Size(_bgsmask.cols,_bgsmask.rows), CV_8UC1); // void function for Lab1.0 - returns zero matrix
		//ADD YOUR CODE HERE
		//...

		absdiff(_bgsmask, _bgsmask, _shadowmask);// currently void function mask=0 (should create shadow mask)
		absdiff(_bgsmask, _shadowmask, _fgmask); // eliminates shadows from bgsmask
		//...
	}
	else{
		cout << "Color currently not supported" << endl;
		exit(1);
	}
}

//ADD ADDITIONAL FUNCTIONS HERE
// single Gaussian
void bgs:: avsa_single_gaussian(double alpha )
{
	// for now we only implement for gray scale image
	if (!_rgb){
		// current pixel and mean difference for std updating
		Mat new_difference = Mat::zeros(Size(mue.cols, mue.rows),CV_8UC1);
		cv::absdiff(_frame ,mue,new_difference);

		// pixelwise background maintenance
		for (int i =0; i<_bkg.rows; i++){
			for (int j =0; j<_bkg.cols; j++){


				// check if pixel is under Gaussian and update the background
				if (_frame.at<uchar>(i,j)>(mue.at<uchar>(i,j) - sqrt(sigma.at<uchar>(i,j))) &&
						_frame.at<uchar>(i,j)<(mue.at<uchar>(i,j) + sqrt(sigma.at<uchar>(i,j)))){
					_bkg.at<uchar>(i,j) = _frame.at<uchar>(i,j);
				}else{
					mue.at<uchar>(i,j) = (uchar)(alpha*_frame.at<uchar>(i,j) + (1 - alpha)*mue.at<uchar>(i,j));//this is the new mue which will be copied to mue
					sigma.at<uchar>(i,j) = (uchar)(alpha *(new_difference.at<uchar>(i,j)*new_difference.at<uchar>(i,j))  + (1-alpha)*(sigma.at<uchar>(i,j)));// this sigma is already squared
					_bkg.at<uchar>(i,j) = mue.at<uchar>(i,j);
				}
			}
		}
	}else
		std::cout<<"not supported for now\n";

}


/*****************************************************************
 *  Morphology operations just to make a little bit post-processing
 *  to make the final output a little bit better
 * **************************************************************
 */
void bgs:: Morpholody_Open(int elmtSize){
	Mat struct_element = cv::getStructuringElement(cv::MORPH_ELLIPSE,Size(2*elmtSize + 1,2*elmtSize + 1),Point(elmtSize,elmtSize));
	cv::morphologyEx(_diff,_diff,cv::MORPH_CLOSE, struct_element);

}
