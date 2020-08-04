/****************************************************************************
* BCPware
*
*  Copyright 2018 by
*
* This program is free software: you can redistribute
* it and/or modify it under the terms of the GNU General Public
* License as published by the Free Software Foundation, either
* version 3 of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will
* be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
****************************************************************************/
#ifndef __SKT_IMAGE_PROCESS
#define __SKT_IMAGE_PROCESS


#include<opencv2/opencv.hpp>

#include <random>
#include <omp.h>
//#include "IccCmm.h"
//#include "IccUtil.h"
#include "image_paralle.h"
#include "lcms2.h"

using namespace std;

using namespace cv;




namespace SKT
{
	static void rotateImage(cv::Mat &M1, cv::Mat &dstM1);
	static double pixelToMm(int pixelOffset, int dpi)
	{
		cout << pixelOffset*25.4 / dpi;
		return pixelOffset*25.4 / dpi;
	}
	static double pixelRatioToMm(int pixelOffset, float Ratio)
	{
		cout << pixelOffset / Ratio;
		return pixelOffset / Ratio;
	}
	static double mmToPixel(int mmOffset, int dpi)
	{
		cout << mmOffset*dpi / 25.4;
		return mmOffset*dpi / 25.4;
	}

	static void  addnoise2(cv::Mat binder, cv::Mat mask, cv::Mat &dst)
	{
		for (int r = 0; r < binder.rows; r++)
			for (int c = 0; c < binder.cols; c++)
			{
				dst.at<cv::Vec3b>(r, c).val[0] = 255;
				dst.at<cv::Vec3b>(r, c).val[1] = 255;
				dst.at<cv::Vec3b>(r, c).val[2] = 255;
				if (binder.at<uchar>(r, c) == 0 && mask.at<uchar>(r, c) == 255)
				{
					int ccc = (r*binder.rows + c) % 4;

					switch (ccc)
					{
					case 0:
						dst.at<cv::Vec3b>(r, c).val[0] = 255;
						dst.at<cv::Vec3b>(r, c).val[1] = 255;
						dst.at<cv::Vec3b>(r, c).val[2] = 0;
						break;
					case 1:
						dst.at<cv::Vec3b>(r, c).val[0] = 0;
						dst.at<cv::Vec3b>(r, c).val[1] = 255;
						dst.at<cv::Vec3b>(r, c).val[2] = 255;
						break;
					case 2:
						dst.at<cv::Vec3b>(r, c).val[0] = 255;
						dst.at<cv::Vec3b>(r, c).val[1] = 0;
						dst.at<cv::Vec3b>(r, c).val[2] = 255;
						break;

					default:
						dst.at<cv::Vec3b>(r, c).val[0] = 255;
						dst.at<cv::Vec3b>(r, c).val[1] = 255;
						dst.at<cv::Vec3b>(r, c).val[2] = 255;
						break;

					}
				}



			}
	}

	static void  combineBinderPattern(cv::Mat binderSrc, cv::Mat src2, cv::Mat &combineDst)
	{
		for (int r = 0; r < binderSrc.rows; r++)
			for (int c = 0; c < binderSrc.cols; c++)
			{
				if (binderSrc.at<uchar>(r, c) == 0)
				{
					if (src2.at<cv::Vec3b>(r, c).val[0]>0 || src2.at<cv::Vec3b>(r, c).val[1]>0 ||
						src2.at<cv::Vec3b>(r, c).val[2] > 0)
					{
						combineDst.at<uchar>(r, c) = 255 - (src2.at<cv::Vec3b>(r, c).val[0] + src2.at<cv::Vec3b>(r, c).val[1] + src2.at<cv::Vec3b>(r, c).val[2]) / 3.;
						/*if (((int)src2.at<cv::Vec4b>(r, c).val[0])!=255)
						cout <<(int) src2.at<cv::Vec4b>(r, c).val[0];
						*/
					}
					else
						combineDst.at<uchar>(r, c) = 0;
				}
				else combineDst.at<uchar>(r, c) = 255;

			}
	}

	static void  combineColorPattern_2(cv::Mat colorSrc, cv::Mat src2, cv::Mat &combineColorDst)
	{
		for (int r = 0; r < colorSrc.rows; r++)
			for (int c = 0; c < colorSrc.cols; c++)
			{
				int aa = (int)colorSrc.at<cv::Vec3b>(r, c).val[0] + (int)src2.at<cv::Vec3b>(r, c).val[0];
				int bb = (int)colorSrc.at<cv::Vec3b>(r, c).val[1] + (int)src2.at<cv::Vec3b>(r, c).val[1];
				int cc = (int)colorSrc.at<cv::Vec3b>(r, c).val[2] + (int)src2.at<cv::Vec3b>(r, c).val[2];

				/*combineColorDst.at<cv::Vec3b>(r, c).val[0] = aa;
				combineColorDst.at<cv::Vec3b>(r, c).val[1] = bb;
				combineColorDst.at<cv::Vec3b>(r, c).val[2] = cc;*/
				//if (aa>255)cout << aa << endl;

				combineColorDst.at<cv::Vec3b>(r, c).val[0] = aa - 255;

				combineColorDst.at<cv::Vec3b>(r, c).val[1] = bb - 255;

				combineColorDst.at<cv::Vec3b>(r, c).val[2] = cc - 255;


				//combineColorDst.at<cv::Vec4b>(r, c).val[0] = 255;
				//combineColorDst.at<cv::Vec4b>(r, c).val[1] = colorSrc.at<cv::Vec4b>(r, c).val[1] + src2.at<cv::Vec4b>(r, c).val[1];
				//combineColorDst.at<cv::Vec4b>(r, c).val[2] = colorSrc.at<cv::Vec4b>(r, c).val[2] + src2.at<cv::Vec4b>(r, c).val[2];			

			}
	}

	static void getrealOutline(cv::Mat binderSrc, cv::Mat &colorOutline, cv::Mat blkOutline)
	{
		cv::cvtColor(blkOutline, blkOutline, CV_BGR2GRAY);

		for (int r = 0; r < binderSrc.rows; r++)
			for (int c = 0; c < binderSrc.cols; c++)
			{
				if (!(binderSrc.at<uchar>(r, c) == 0 && blkOutline.at<uchar>(r, c) == 0))
				{
					colorOutline.at<cv::Vec3b>(r, c).val[0] = 255;
					colorOutline.at<cv::Vec3b>(r, c).val[1] = 255;
					colorOutline.at<cv::Vec3b>(r, c).val[2] = 255;
				}

			}

	}

	static cv::Mat resizeIamge(cv::Mat *oriImage, int plus_rows)
	{

		//cv::Mat bigImage(oriImage->rows+100,oriImage->cols,oriImage->type());
		//oriImage->copyTo(bigImage(cv::Rect(0, 100, oriImage->rows, oriImage->cols)));	

		//cv::Mat bigImage(oriImage->rows + plus_rows, oriImage->cols, oriImage->type());
		//bigImage.setTo(cv::Scalar(255, 255, 255));
		////oriImage->copyTo(bigImage(cv::Rect(0, plus_rows, oriImage->rows, oriImage->cols)));
		//oriImage->copyTo(bigImage.rowRange(2308, 4713).colRange(0, 2480));


		cv::Mat bigImage(oriImage->rows + plus_rows, oriImage->cols, oriImage->type());
		switch (oriImage->type())
		{
		case CV_8UC3:
		{
			//cv::Mat bigImage(oriImage->rows + plus_rows, oriImage->cols, oriImage->type());
			//bigImage.setTo(cv::Scalar(255, 255, 255));
			////oriImage->copyTo(bigImage(cv::Rect(0, plus_rows, oriImage->rows, oriImage->cols)));
			//oriImage->copyTo(bigImage.rowRange(2308, 4713).colRange(0, 2480));


			bigImage.setTo(cv::Scalar(255, 255, 255));
			//oriImage->copyTo(bigImage(cv::Rect(0, plus_rows, oriImage->rows, oriImage->cols)));
			//oriImage->copyTo(bigImage.rowRange(0, 2405).colRange(0, 2480));
			oriImage->copyTo(bigImage.rowRange(plus_rows, plus_rows + oriImage->rows).colRange(0, oriImage->cols));
			break;
		}
		case CV_8UC1:
		{
			//cv::Mat bigImage(oriImage->rows + plus_rows, oriImage->cols, oriImage->type());
			//bigImage.setTo(cv::Scalar(255));
			////oriImage->copyTo(bigImage(cv::Rect(0, plus_rows, oriImage->rows, oriImage->cols)));
			//oriImage->copyTo(bigImage.rowRange(2308, 4713).colRange(0, 2480));

			//cv::Mat bigImage(oriImage->rows + plus_rows, oriImage->cols, oriImage->type());
			bigImage.setTo(cv::Scalar(255));
			//oriImage->copyTo(bigImage(cv::Rect(0, plus_rows, oriImage->rows, oriImage->cols)));
			oriImage->copyTo(bigImage.rowRange(plus_rows, plus_rows + oriImage->rows).colRange(0, oriImage->cols));

		}break;
		}




		return bigImage;
	}

	typedef enum{ RGB, CMY }pixelFormat;
	static cv::Mat resizeIamgecols(cv::Mat *oriImage, int plus_cols, pixelFormat pixformat)
	{

		//cv::Mat bigImage(oriImage->rows + plus_cols, oriImage->cols, oriImage->type());
		cv::Mat bigImage(oriImage->rows, oriImage->cols + plus_cols, oriImage->type());
		switch (oriImage->type())
		{
		case CV_8UC3:
		{
			if (pixformat == pixelFormat::RGB)
				bigImage.setTo(cv::Scalar(255, 255, 255));
			else if (pixformat == pixelFormat::CMY)
				bigImage.setTo(cv::Scalar(0, 0, 0));
			//oriImage->copyTo(bigImage.rowRange(plus_cols, plus_cols + oriImage->rows).colRange(0, oriImage->cols));
			oriImage->copyTo(bigImage.rowRange(0, oriImage->rows).colRange(plus_cols, plus_cols + oriImage->cols));
			break;
		}
		case CV_8UC1:
		{
			bigImage.setTo(cv::Scalar(255));
			oriImage->copyTo(bigImage.rowRange(0, oriImage->rows).colRange(plus_cols, plus_cols + oriImage->cols));

		}break;
		}




		return bigImage;
	}



	static cv::Mat resizeXAxis(cv::Mat *oriImage, float meFault)
	{
		cv::Mat bigImage(oriImage->rows*meFault, oriImage->cols, oriImage->type());
		//bigImage.setTo(cv::Scalar(255, 255, 255));
		cv::resize(*oriImage, bigImage, bigImage.size(), cv::INTER_AREA);
		return bigImage;

	}
	static cv::Mat resizeXYAxis(cv::Mat *oriImage, int newRows, int newCols)
	{
		//cv::Mat bigImage(oriImage->rows + plus_rows, oriImage->cols, oriImage->type());
		//switch (oriImage->type())
		//{
		//case CV_8UC3:
		//{			
		//				bigImage.setTo(cv::Scalar(255, 255, 255));						
		//				oriImage->copyTo(bigImage.rowRange(plus_rows, plus_rows + oriImage->rows).colRange(0, oriImage->cols));
		//				break;
		//}
		//case CV_8UC1:
		//{						
		//				bigImage.setTo(cv::Scalar(255));
		//				//oriImage->copyTo(bigImage(cv::Rect(0, plus_rows, oriImage->rows, oriImage->cols)));
		//				oriImage->copyTo(bigImage.rowRange(plus_rows, plus_rows + oriImage->rows).colRange(0, oriImage->cols));

		//}break;
		//}
		cv::Mat bigImage(newRows, newCols, oriImage->type());
		//bigImage.setTo(cv::Scalar(255, 255, 255));
		cv::resize(*oriImage, bigImage, bigImage.size(), cv::INTER_AREA);
		return bigImage;
	}
	static cv::Mat erodeImage(cv::Mat *oriImage, int iterations)
	{
		cv::Mat erosion_dst;
		int element_size_ = 1;
		cv::Mat element = getStructuringElement(cv::MORPH_ELLIPSE,
			cv::Size(2 * element_size_ + 1, 2 * element_size_ + 1),
			cv::Point(element_size_, element_size_));

		/// Apply the erosion operation
		cv::erode(*oriImage, erosion_dst, element, Point(-1, -1), iterations);
		return erosion_dst;
	}
	static cv::Mat dilateImage(cv::Mat *oriImage, int iterations)
	{
		cv::Mat dilate_dst;
		int element_size_ = 1;
		cv::Mat element = getStructuringElement(cv::MORPH_RECT,
			cv::Size(2 * element_size_ + 1, 2 * element_size_ + 1),
			cv::Point(element_size_, element_size_));

		/// Apply the erosion operation
		cv::dilate(*oriImage, dilate_dst, element, Point(-1, -1), iterations);
		return dilate_dst;
	}
	static cv::Mat binderAddPattern(cv::Mat *capTemp, cv::Mat *outline_mask)
	{

		cv::cvtColor(*outline_mask, *outline_mask, CV_BGR2GRAY);
		for (int r = 0; r < capTemp->rows; r++)
			for (int c = 0; c < capTemp->cols; c++)
			{
				if (capTemp->at<uchar>(r, c) == 0 && outline_mask->at<uchar>(r, c) == 255)
				{
					//capTemp->at<uchar>(r, c) = 254;
					capTemp->at<uchar>(r, c) = 0;
				}
			}

		//////////chessBoard//////////
		int blockSize = 75;
		int size_rows = capTemp->rows;
		int size_cols = capTemp->cols;
		cv::Mat chessBoard(size_rows, size_cols, CV_8U, cv::Scalar(0));
		//int sig = 0;
		unsigned char color = 0;
		//unsigned char color2 = 128;
		for (int i = 0; i < size_rows; i += blockSize)
		{
			if ((i + blockSize) < size_rows){
				color = ~color;
				for (int j = 0; j < size_cols; j += blockSize){
					if ((j + blockSize) < size_cols){
						cv::Mat ROI = chessBoard(cv::Rect(j, i, blockSize, blockSize));
						ROI.setTo(cv::Scalar(color));
						color = ~color;
					}
				}
			}
		}
		//////////cross///////////////////////////
		//unsigned char color_cols = 0;
		//unsigned char color_rows = 0;
		//for (int i = 0; i < size_rows; i += blockSize)
		//{
		//	if ((i + blockSize) < size_rows){
		//		//color = ~color;
		//		for (int j = 0; j < size_cols; j += blockSize){
		//			if ((j + blockSize) < size_cols){
		//				cv::Mat ROI = chessBoard(cv::Rect(j, i, blockSize, blockSize));
		//				ROI.setTo(cv::Scalar(color_cols));

		//				if (int((i / blockSize) % 2)>0)
		//					color_cols = 0;
		//				else 
		//					color_cols = ~color_cols;
		//				
		//				
		//			}
		//		}
		//	}
		//}
		//////////add chessBoard to cap //////////		
		for (int r = 0; r < capTemp->rows; r++)
		{
			for (int c = 0; c < capTemp->cols; c++)
			{
				if (capTemp->at<uchar>(r, c) == 254 && chessBoard.at<uchar>(r, c) == 0)
				{
					capTemp->at<uchar>(r, c) = chessBoard.at<uchar>(r, c);
				}

			}
		}
		/////////////////////////////////






		return *capTemp;
	}

	static void createHexPatternImage(cv::Mat &cellImg, cv::Mat &outPatternImg)
	{
		//cv::Mat pattern_board(size_rows, size_cols, CV_8U, cv::Scalar(0));
		int size_rows = outPatternImg.rows;
		int size_cols = outPatternImg.cols;

		int hexagon_rows = cellImg.rows;
		int hexagon_cols = cellImg.cols;
		int start_j = cellImg.cols / 2;
		int j = 0;
		for (int i = 0; i < size_rows; i += (hexagon_rows))
		{
			if (((i / hexagon_rows) % 2)>0)j = 0;
			else j = start_j;
			if ((i + hexagon_rows) < size_rows)
			{
				for (; j < size_cols; j += (hexagon_cols))
				{
					if ((j + hexagon_cols) < size_cols)
					{
						//cv::Mat ROI = pattern_board(cv::Rect(j, i, hexagon_cols, hexagon_rows));
						//patternImg.copyTo(ROI);
						cellImg.copyTo(outPatternImg(cv::Rect(j, i, hexagon_cols, hexagon_rows)));
					}
				}
			}
		}
	}
	static cv::Mat pasteHexBoardtoImg(cv::Mat &capTempMask, cv::Mat &hexBoard, cv::Mat &shellMask, cv::Mat &capTemp)
	{
		if (hexBoard.type() != CV_8UC1)
			cv::cvtColor(hexBoard, hexBoard, CV_BGR2GRAY);
		if (shellMask.type() != CV_8UC1)
			cv::cvtColor(shellMask, shellMask, CV_BGR2GRAY);
		if (capTemp.type() != CV_8UC1)
			cv::cvtColor(capTemp, capTemp, CV_BGR2GRAY);



		for (int r = 0; r < capTemp.rows; r++)
			for (int c = 0; c < capTemp.cols; c++)
			{
				if (capTempMask.at<uchar>(r, c) == 0 && shellMask.at<uchar>(r, c) == 255)
				{
					capTemp.at<uchar>(r, c) = hexBoard.at<uchar>(r, c);
				}
			}

		return capTemp;
	}

	static cv::Mat binderAddHexagon(cv::Mat *capTemp, cv::Mat *outline_mask, cv::Mat &patternImg)
	{

		cv::cvtColor(*outline_mask, *outline_mask, CV_BGR2GRAY);
		for (int r = 0; r < capTemp->rows; r++)
			for (int c = 0; c < capTemp->cols; c++)
			{
				if (capTemp->at<uchar>(r, c) == 0 && outline_mask->at<uchar>(r, c) == 255)
				{
					capTemp->at<uchar>(r, c) = 254;//become white except mask area
					//capTemp->at<uchar>(r, c) = 0;
				}
			}

		int size_rows = capTemp->rows;
		int size_cols = capTemp->cols;
		//////////Hexagon/////////////
		/*cv::Mat hexagonImg;
		hexagonImg = cv::imread("D:/VC_TempWorkSpace/Mesh_Test/SKT_version1/meshlab/src/sampleplugins/generate_print_func/image/hexagon_2.png", CV_LOAD_IMAGE_GRAYSCALE);

		cv::Mat pattern_board(size_rows, size_cols, CV_8U, cv::Scalar(0));
		int hexagon_rows = hexagonImg.rows;
		int hexagon_cols = hexagonImg.cols;
		for (int i = 0; i < size_rows; i += hexagon_rows)
		{
		if ((i + hexagon_rows) < size_rows)
		{
		for (int j = 0; j < size_cols; j += hexagon_cols)
		{
		if ((j + hexagon_cols) < size_cols)
		{
		cv::Mat ROI = pattern_board(cv::Rect(j, i, hexagon_cols, hexagon_rows));
		hexagonImg.copyTo(ROI);
		}
		}
		}
		}	*/
		switch (1)
		{
#pragma region area0
		case 0:
		{

			//////////Hexagon/////////////
			//cv::Mat hexagonImg;
			//hexagonImg = cv::imread("D:/VC_TempWorkSpace/Mesh_Test/SKT_version1/meshlab/src/sampleplugins/generate_print_func/image/hexagon_2.png", CV_LOAD_IMAGE_GRAYSCALE);
			//hexagonImg = cv::imread(path.toStdString() + "/hexagon_2.png", CV_LOAD_IMAGE_GRAYSCALE);
			cv::Mat pattern_board(size_rows, size_cols, CV_8U, cv::Scalar(0));
			int hexagon_rows = patternImg.rows;
			int hexagon_cols = patternImg.cols;
			for (int i = 0; i < size_rows; i += (hexagon_rows))
			{
				if ((i + hexagon_rows) < size_rows)
				{
					for (int j = 0; j < size_cols; j += (hexagon_cols))
					{
						if ((j + hexagon_cols) < size_cols)
						{
							cv::Mat ROI = pattern_board(cv::Rect(j, i, hexagon_cols, hexagon_rows));
							patternImg.copyTo(ROI);
						}
					}
				}
			}
			//////////add chessBoard to cap //////////		
			for (int r = 0; r < capTemp->rows; r++)
			{
				for (int c = 0; c < capTemp->cols; c++)
				{
					if (capTemp->at<uchar>(r, c) == 254 && pattern_board.at<uchar>(r, c) == 0)
					{
						capTemp->at<uchar>(r, c) = pattern_board.at<uchar>(r, c);

					}
				}
			}
			/////////////////////////////////
		}break;
#pragma endregion area0

		case 1:
		{
			//////////Hexagon/////////////
			//cv::Mat hexagonImg;
			//hexagonImg = cv::imread(path.toStdString() + "/hexagon_4.png", CV_LOAD_IMAGE_GRAYSCALE);
			cv::Mat pattern_board(size_rows, size_cols, CV_8U, cv::Scalar(0));
			int hexagon_rows = patternImg.rows;
			int hexagon_cols = patternImg.cols;
			int start_j = patternImg.cols / 2;
			int j = 0;
			for (int i = 0; i < size_rows; i += (hexagon_rows))
			{
				if (((i / hexagon_rows) % 2)>0)j = 0;
				else j = start_j;
				if ((i + hexagon_rows) < size_rows)
				{
					for (; j < size_cols; j += (hexagon_cols))
					{
						if ((j + hexagon_cols) < size_cols)
						{
							//cv::Mat ROI = pattern_board(cv::Rect(j, i, hexagon_cols, hexagon_rows));
							//patternImg.copyTo(ROI);
							patternImg.copyTo(pattern_board(cv::Rect(j, i, hexagon_cols, hexagon_rows)));
						}
					}
				}
			}
			//////////add chessBoard to cap //////////		
			for (int c = 0; c < capTemp->cols; c++)
			{

				unsigned char* capTempData = (unsigned char*)capTemp->col(c).data;
				unsigned char* pattern_boardData = (unsigned char*)pattern_board.col(c).data;
				int blackcols = (int)(pattern_board.step / pattern_board.elemSize1());//
				for (int r = 0; r < capTemp->rows; r++)
				{
					////qDebug() << "pattern_board.at<uchar>(r, c)"<<pattern_board.at<uchar>(r, c);
					/*if (capTemp->at<uchar>(r, c) == 254 && pattern_board.at<uchar>(r, c) == 0)
					{
					capTemp->at<uchar>(r, c) = pattern_board.at<uchar>(r, c);

					}*/
					if (int(capTempData[r*blackcols]) == 254 && int(pattern_boardData[r*blackcols]) == 0)
					{
						capTempData[r*blackcols] = pattern_boardData[r*blackcols];
					}
				}
			}

		}
		break;

		}
		return *capTemp;
	}
	static cv::Mat binderAddPatternPercent(cv::Mat *capTemp, cv::Mat *outline_mask, cv::Mat &patternImg)
	{

		cv::cvtColor(*outline_mask, *outline_mask, CV_BGR2GRAY);
		for (int r = 0; r < capTemp->rows; r++)
			for (int c = 0; c < capTemp->cols; c++)
			{
				if (capTemp->at<uchar>(r, c) == 0 && outline_mask->at<uchar>(r, c) == 255)
				{
					capTemp->at<uchar>(r, c) = 254;//become white except mask area
					//capTemp->at<uchar>(r, c) = 0;
				}
			}

		int size_rows = capTemp->rows;
		int size_cols = capTemp->cols;

		switch (1)
		{
		case 1:
		{
			//////////Hexagon/////////////
			//cv::Mat hexagonImg;
			//hexagonImg = cv::imread(path.toStdString() + "/hexagon_4.png", CV_LOAD_IMAGE_GRAYSCALE);
			cv::Mat pattern_board(size_rows, size_cols, CV_8U, cv::Scalar(0));
			int hexagon_rows = patternImg.rows;
			int hexagon_cols = patternImg.cols;
			int start_j = patternImg.cols / 2;

			for (int i = 0; i < size_rows; i += (hexagon_rows))
			{

				if ((i + hexagon_rows) < size_rows)
				{
					for (int j = 0; j < size_cols; j += (hexagon_cols))
					{
						if ((j + hexagon_cols) < size_cols)
						{
							cv::Mat ROI = pattern_board(cv::Rect(j, i, hexagon_cols, hexagon_rows));
							patternImg.copyTo(ROI);
						}
					}
				}
			}
			//////////add chessBoard to cap //////////		
			for (int c = 0; c < capTemp->cols; c++)
			{

				unsigned char* capTempData = (unsigned char*)capTemp->col(c).data;
				unsigned char* pattern_boardData = (unsigned char*)pattern_board.col(c).data;
				int blackcols = (int)(pattern_board.step / pattern_board.elemSize1());//
				for (int r = 0; r < capTemp->rows; r++)
				{
					////qDebug() << "pattern_board.at<uchar>(r, c)"<<pattern_board.at<uchar>(r, c);
					/*if (capTemp->at<uchar>(r, c) == 254 && pattern_board.at<uchar>(r, c) == 0)
					{
					capTemp->at<uchar>(r, c) = pattern_board.at<uchar>(r, c);

					}*/
					if (int(capTempData[r*blackcols]) == 254 && int(pattern_boardData[r*blackcols]) == 0)
					{
						capTempData[r*blackcols] = pattern_boardData[r*blackcols];
					}
				}
			}

		}
		break;

		}
		return *capTemp;
	}


	static bool MagicSquarePattern(cv::Mat &patternImg, int percent, int CELL_SIZE)
	{
		int maxrectlength = patternImg.cols;//===groove size
		int dots = ((maxrectlength*maxrectlength) * (percent / 100.)) + 0.5;
		double steps;
		if (dots == 0)return false;
		else
			steps = (maxrectlength*maxrectlength) / (double)dots;

		/*=================================

		======================================*/

		int x, y;
		std::vector<std::vector<int>> array(CELL_SIZE, std::vector<int>(CELL_SIZE));

		x = 0;
		y = CELL_SIZE / 2;

		for (int i = 0; i < dots; i++)
		{
			if ((x < 0 && y > CELL_SIZE - 1) ||
				(x >= 0 && y <= CELL_SIZE - 1 && array[x][y] != 0))
			{
				x = x + 2;
				y = y - 1;
			}

			x = (x + CELL_SIZE) % CELL_SIZE;
			y = y % CELL_SIZE;

			int cc = x;
			int rr = y;

			patternImg.at<uchar>(cc, rr) = 0;

			array[x][y] = i + 1;

			x = x - 1;
			y = y + 1;
		}


		int TotalNumberOfPixels = patternImg.rows * patternImg.cols;
		int ZeroPixels = TotalNumberOfPixels - countNonZero(patternImg);

		return true;
	}


	static void rot90(cv::Mat &matImage, cv::Mat &des, int rotflag){
		//1=CW, 2=CCW, 3=180
		if (rotflag == 1){
			transpose(matImage, des);
			flip(des, des, 1); //transpose+flip(1)=CW
		}
		else if (rotflag == 2) {
			transpose(matImage, des);
			flip(des, des, 0); //transpose+flip(0)=CCW     
		}
		else if (rotflag == 3){
			flip(matImage, des, -1);    //flip(-1)=180          
		}
		else if (rotflag != 0){ //if not 0,1,2,3:
			cout << "Unknown rotation flag(" << rotflag << ")" << endl;
		}
	}

	static void createPatternImage(cv::Mat &cellImg, cv::Mat &outPatternImg)
	{
		int size_rows = outPatternImg.rows;
		int size_cols = outPatternImg.cols;


		//////////magicSquare/////////////			 

		int hexagon_rows = cellImg.rows;
		int hexagon_cols = cellImg.cols;
		int CELL_SIZE = cellImg.rows;
		int start_j = cellImg.cols / 2;


		cv::Mat cw(cellImg.size(), CV_8U, cv::Scalar(0));
		cv::Mat ccw(cellImg.size(), CV_8U, cv::Scalar(0));
		cv::Mat ccw180(cellImg.size(), CV_8U, cv::Scalar(0));

		rot90(cellImg, cw, 1);
		rot90(cellImg, ccw, 2);
		rot90(cellImg, ccw180, 3);


		std::vector<cv::Mat> patternImgs;
		patternImgs.push_back(cellImg);
		patternImgs.push_back(cw);
		patternImgs.push_back(ccw180);
		patternImgs.push_back(ccw);

		//cv::Mat magic_square_pattern_board(size_rows, size_cols, CV_8U, cv::Scalar(255));
		int count = 0;

		for (int i = 0; i < size_rows + 1; i += (hexagon_rows))
		{
			if ((i + hexagon_rows) < size_rows + 1)
			{
				for (int j = 0; j < size_cols + 1; j += (hexagon_cols))
				{
					if ((j + hexagon_cols) < size_cols + 1)
					{

						cv::Mat ROI = outPatternImg(cv::Rect(j, i, hexagon_cols, hexagon_rows));

						int x_ten = j / CELL_SIZE;
						int y_ten = i / CELL_SIZE;

						if (y_ten % 2)
						{
							if ((x_ten % 2))
								patternImgs[0].copyTo(ROI);
							else
								patternImgs[1].copyTo(ROI);
						}
						else
						{
							if ((x_ten % 2))
								patternImgs[3].copyTo(ROI);
							else
								patternImgs[2].copyTo(ROI);
						}

						/*if (y_ten % 2)
						{
						if ((x_ten % 2))
						patternImgs[2].copyTo(ROI);
						else
						patternImgs[3].copyTo(ROI);
						}
						else
						{
						if ((x_ten % 2))
						patternImgs[1].copyTo(ROI);
						else
						patternImgs[0].copyTo(ROI);
						}*/

					}
				}
			}
		}



	}


	static cv::Mat pastePatternToImage(cv::Mat &capTemp, cv::Mat *outline_mask, cv::Mat &patternImg, int CELL_SIZE, bool inv = true)
	{
		cv::cvtColor(*outline_mask, *outline_mask, CV_BGR2GRAY);
		for (int r = 0; r < capTemp.rows; r++)
			for (int c = 0; c < capTemp.cols; c++)
			{
				if (capTemp.at<uchar>(r, c) == 0 && outline_mask->at<uchar>(r, c) == 255)
				{
					capTemp.at<uchar>(r, c) = 254;//become white except mask area				
				}
			}

		int size_rows = capTemp.rows;
		int size_cols = capTemp.cols;


		//////////Hexagon/////////////			 
		//cv::Mat pattern_board(size_rows, size_cols, CV_8U, cv::Scalar(0));
		int hexagon_rows = patternImg.rows;
		int hexagon_cols = patternImg.cols;
		int start_j = patternImg.cols / 2;


		cv::Mat cw(patternImg.size(), CV_8U, cv::Scalar(0));
		cv::Mat ccw(patternImg.size(), CV_8U, cv::Scalar(0));
		cv::Mat ccw180(patternImg.size(), CV_8U, cv::Scalar(0));

		rot90(patternImg, cw, 1);
		rot90(patternImg, ccw, 2);
		rot90(patternImg, ccw180, 3);


		std::vector<cv::Mat> patternImgs;
		patternImgs.push_back(patternImg);
		patternImgs.push_back(cw);
		patternImgs.push_back(ccw180);
		patternImgs.push_back(ccw);

		cv::Mat magic_square_pattern_board(size_rows, size_cols, CV_8U, cv::Scalar(255));
		int count = 0;

		for (int i = 0; i < size_rows + 1; i += (hexagon_rows))
		{
			if ((i + hexagon_rows) < size_rows + 1)
			{
				for (int j = 0; j < size_cols + 1; j += (hexagon_cols))
				{
					if ((j + hexagon_cols) < size_cols + 1)
					{

						cv::Mat ROI = magic_square_pattern_board(cv::Rect(j, i, hexagon_cols, hexagon_rows));

						int x_ten = j / CELL_SIZE;
						int y_ten = i / CELL_SIZE;

						if (y_ten % 2)
						{
							if ((x_ten % 2))
								patternImgs[0].copyTo(ROI);
							else
								patternImgs[1].copyTo(ROI);
						}
						else
						{
							if ((x_ten % 2))
								patternImgs[3].copyTo(ROI);
							else
								patternImgs[2].copyTo(ROI);
						}

						/*if (y_ten % 2)
						{
						if ((x_ten % 2))
						patternImgs[2].copyTo(ROI);
						else
						patternImgs[3].copyTo(ROI);
						}
						else
						{
						if ((x_ten % 2))
						patternImgs[1].copyTo(ROI);
						else
						patternImgs[0].copyTo(ROI);
						}*/




					}
				}
			}
		}

		//////////add chessBoard to cap //////////		
		for (int r = 0; r < capTemp.rows; r++)
		{
			for (int c = 0; c < capTemp.cols; c++)
			{
				if (capTemp.at<uchar>(r, c) == 254 && magic_square_pattern_board.at<uchar>(r, c) == 0)
				{
					capTemp.at<uchar>(r, c) = magic_square_pattern_board.at<uchar>(r, c);

				}
			}
		}





		return capTemp;
	}
	static cv::Mat pastePatternToInner_and_grayscale(cv::Mat &capTemp, cv::Mat &colorImgOut, cv::Mat &captempMask, cv::Mat *outline_mask, cv::Mat &patternImg, bool innerPatternC, bool innerPatternM, bool innerPatternY, bool innerPatternB, bool inv = true)
	{
		cv::cvtColor(*outline_mask, *outline_mask, CV_BGR2GRAY);
		for (int r = 0; r < capTemp.rows; r++)
			for (int c = 0; c < capTemp.cols; c++)
			{
				if (captempMask.at<uchar>(r, c) == 0 && outline_mask->at<uchar>(r, c) == 255 && patternImg.at<uchar>(r, c) == 0)
				{
					//capTemp.at<uchar>(r, c) = patternImg.at<uchar>(r, c);

					//CMYB Grayscale
					/*if (!innerPatternB)
						capTemp.at<uchar>(r, c) = 255;*/
					capTemp.at<uchar>(r, c) = innerPatternB ? 0 : 255;
					colorImgOut.at<cv::Vec3b>(r, c) = Vec3b(innerPatternY ? 0 : 255, innerPatternM ? 0 : 255, innerPatternC ? 0 : 255);
				}
			}

		return capTemp;
	}

	static void pastePatternToShell_and_grayscale(cv::Mat &capTemp, cv::Mat &colorImgOut,
		cv::Mat &capTempMask, cv::Mat &perimeter_mask, cv::Mat &top_mask,
		cv::Mat &capDownMask,
		cv::Mat &H_patternImg, cv::Mat &Z_patternImg, cv::Mat &Bottom_patternImg,
		bool perimeterB, bool perimeterC, bool perimeterM, bool perimeterY,
		bool up_downB, bool up_downC, bool up_downM, bool up_downY,
		bool bottomB, bool bottomC, bool bottomM, bool bottomY, bool inv = true)
	{
		if (perimeter_mask.type() != CV_8UC1)
			cv::cvtColor(perimeter_mask, perimeter_mask, CV_BGR2GRAY);
		if (top_mask.type() != CV_8UC1)
			cv::cvtColor(top_mask, top_mask, CV_BGR2GRAY);
		if (capDownMask.type() != CV_8UC1)
			cv::cvtColor(capDownMask, capDownMask, CV_BGR2GRAY);

		for (int r = 0; r < capTemp.rows; r++)
			for (int c = 0; c < capTemp.cols; c++)
			{
				//horizontal
				if (capTempMask.at<uchar>(r, c) == 0 && perimeter_mask.at<uchar>(r, c) == 0 && H_patternImg.at<uchar>(r, c) == 0)
				{
					//capTemp.at<uchar>(r, c) = H_patternImg.at<uchar>(r, c);
					//CMYB perimeterGrayscale
					//if (!perimeterB)
					//capTemp.at<uchar>(r, c) = 255;


					capTemp.at<uchar>(r, c) = perimeterB ? 0 : 255;
					colorImgOut.at<cv::Vec3b>(r, c) = Vec3b(perimeterY ? 0 : 255, perimeterM ? 0 : 255, perimeterC ? 0 : 255);
				}

				//top
				if (capTempMask.at<uchar>(r, c) == 0 && top_mask.at<uchar>(r, c) == 255 && Z_patternImg.at<uchar>(r, c) == 0)
				{
					/*	capTemp.at<uchar>(r, c) = Z_patternImg.at<uchar>(r, c);

						if (!up_downB)
						capTemp.at<uchar>(r, c) = 255;*/


					capTemp.at<uchar>(r, c) = up_downB ? 0 : 255;
					colorImgOut.at<cv::Vec3b>(r, c) = Vec3b(up_downY ? 0 : 255, up_downM ? 0 : 255, up_downC ? 0 : 255);

				}

				//bottom lighter
				if (capDownMask.at<uchar>(r, c) == 255 && capTempMask.at<uchar>(r, c) == 0 && Bottom_patternImg.at<uchar>(r, c) == 0)
				{
					/*	capTemp.at<uchar>(r, c) = Bottom_patternImg.at<uchar>(r, c);
						if (!bottomB)
						capTemp.at<uchar>(r, c) = 255;*/
					capTemp.at<uchar>(r, c) = bottomB ? 0 : 255;
					//BGR
					colorImgOut.at<cv::Vec3b>(r, c) = Vec3b(bottomY ? 0 : 255, bottomM ? 0 : 255, bottomC ? 0 : 255);
				}

			}



	}


	//static void pastePatternToShell_and_grayscale_four_diff_Patterns(cv::Mat &capTemp, cv::Mat &colorImgOut,
	//	cv::Mat &capTempMask, cv::Mat &perimeter_mask, cv::Mat &top_mask,
	//	cv::Mat &capDownMask, cv::Mat capDownSureMask,
	//	cv::Mat &H_patternImg, cv::Mat &Z_patternImg, cv::Mat &Bottom_patternImg,
	//	//horizonPixelValue, verticalPixelValue, bottomPixelValue, insidePixelValue
	//	int horizonPixelValue, cv::Vec3b verticalPixelValue, int bottomPixelValue, cv::Vec3b insidePixelValue,
	//	bool perimeterB, bool perimeterC, bool perimeterM, bool perimeterY,
	//	bool up_downB, bool up_downC, bool up_downM, bool up_downY,
	//	bool bottomB, bool bottomC, bool bottomM, bool bottomY, bool inv = true)
	//{
	//	if (perimeter_mask.type() != CV_8UC1)
	//		cv::cvtColor(perimeter_mask, perimeter_mask, CV_BGR2GRAY);
	//	if (top_mask.type() != CV_8UC1)
	//		cv::cvtColor(top_mask, top_mask, CV_BGR2GRAY);
	//	if (capDownMask.type() != CV_8UC1)
	//		cv::cvtColor(capDownMask, capDownMask, CV_BGR2GRAY);

	//	for (int r = 0; r < capTemp.rows; r++)
	//		for (int c = 0; c < capTemp.cols; c++)
	//		{


	//			if (capTempMask.at<uchar>(r, c) == 0 && top_mask.at<uchar>(r, c) == 255 && Z_patternImg.at<uchar>(r, c) == 0)
	//			{

	//				//TOP
	//				capTemp.at<uchar>(r, c) = 255;
	//				colorImgOut.at<cv::Vec3b>(r, c) = verticalPixelValue;

	//			}
	//			if (capTempMask.at<uchar>(r, c) == 0 && perimeter_mask.at<uchar>(r, c) == 0 && H_patternImg.at<uchar>(r, c) == 0)
	//			{
	//				//SHELL
	//				capTemp.at<uchar>(r, c) = horizonPixelValue;
	//				colorImgOut.at<cv::Vec3b>(r, c) = Vec3b(255, 255, 255);
	//				
	//			}

	//			if ((capDownMask.at<uchar>(r, c) == 255 || capDownSureMask.at<uchar>(r, c) == 255) && capTempMask.at<uchar>(r, c) == 0 && Bottom_patternImg.at<uchar>(r, c) == 0)
	//			{

	//				
	//				//Bottom
	//				capTemp.at<uchar>(r, c) = bottomPixelValue;
	//				colorImgOut.at<cv::Vec3b>(r, c) = Vec3b(255,255,255);
	//			}
	//			if (capTempMask.at<uchar>(r, c) == 0 && perimeter_mask.at<uchar>(r, c) != 0 && top_mask.at<uchar>(r, c) != 0 && capDownMask.at<uchar>(r, c) != 255)
	//			{
	//				//Inside
	//				capTemp.at<uchar>(r, c) = 255;
	//				colorImgOut.at<cv::Vec3b>(r, c) = insidePixelValue;
	//				
	//			}

	//		}
	//}


	static void pastePatternToShell_and_grayscale_four_diff_Patterns(cv::Mat &capTemp, cv::Mat &colorImgOut,
		cv::Mat &capTempMask, cv::Mat &perimeter_mask, cv::Mat &top_mask, cv::Mat &topSureMask,
		cv::Mat &capDownMask, cv::Mat capDownSureMask,
		cv::Mat &H_patternImg, cv::Mat &Z_patternImg, cv::Mat &Bottom_patternImg,
		//horizonPixelValue, verticalPixelValue, bottomPixelValue, insidePixelValue
		cv::Vec4b horizonBGRK_Value, cv::Vec4b topBGRK_Value, cv::Vec4b bottomBGRK_Value, cv::Vec4b insideRGBK_Value,
		bool perimeterB, bool perimeterC, bool perimeterM, bool perimeterY,
		bool up_downB, bool up_downC, bool up_downM, bool up_downY,
		bool bottomB, bool bottomC, bool bottomM, bool bottomY, bool inv = true)
	{
		if (perimeter_mask.type() != CV_8UC1)
			cv::cvtColor(perimeter_mask, perimeter_mask, CV_BGR2GRAY);
		if (top_mask.type() != CV_8UC1)
			cv::cvtColor(top_mask, top_mask, CV_BGR2GRAY);
		if (capDownMask.type() != CV_8UC1)
			cv::cvtColor(capDownMask, capDownMask, CV_BGR2GRAY);

		for (int r = 0; r < capTemp.rows; r++)
			for (int c = 0; c < capTemp.cols; c++)
			{


				if (capTempMask.at<uchar>(r, c) == 0 && (top_mask.at<uchar>(r, c) == 255 || topSureMask.at<uchar>(r, c) == 255) /*&& Z_patternImg.at<uchar>(r, c) == 0*/)
				{

					//TOP
					capTemp.at<uchar>(r, c) = topBGRK_Value[3];
					colorImgOut.at<cv::Vec3b>(r, c) = Vec3b(topBGRK_Value[0], topBGRK_Value[1], topBGRK_Value[2]);
				}

				if (capTempMask.at<uchar>(r, c) == 0 && perimeter_mask.at<uchar>(r, c) == 0 /*&& H_patternImg.at<uchar>(r, c) == 0*/)
				{
					//SHELL

					capTemp.at<uchar>(r, c) = horizonBGRK_Value[3];
					colorImgOut.at<cv::Vec3b>(r, c) = Vec3b(horizonBGRK_Value[0], horizonBGRK_Value[1], horizonBGRK_Value[2]);

				}
				if ((capDownMask.at<uchar>(r, c) == 255 || capDownSureMask.at<uchar>(r, c) == 255) && capTempMask.at<uchar>(r, c) == 0 /*&& Bottom_patternImg.at<uchar>(r, c) == 0*/)
				{


					//Bottom
					capTemp.at<uchar>(r, c) = bottomBGRK_Value[3];
					colorImgOut.at<cv::Vec3b>(r, c) = Vec3b(bottomBGRK_Value[0], bottomBGRK_Value[1], bottomBGRK_Value[2]);
				}
				if (capTempMask.at<uchar>(r, c) == 0 && perimeter_mask.at<uchar>(r, c) != 0 && top_mask.at<uchar>(r, c) != 255 && capDownMask.at<uchar>(r, c) != 255)
				{
					//Inside
					capTemp.at<uchar>(r, c) = insideRGBK_Value[3];
					colorImgOut.at<cv::Vec3b>(r, c) = Vec3b(insideRGBK_Value[0], insideRGBK_Value[1], insideRGBK_Value[2]);

				}

			}



	}



	static void pastePatternToShell_and_grayscale_diffColor(cv::Mat &capTemp, cv::Mat &colorImgOut,
		cv::Mat &capTempMask, cv::Mat &perimeter_mask, cv::Mat &top_mask,
		cv::Mat &capDownMask, cv::Mat capDownSureMask,
		cv::Mat &H_patternImg, cv::Mat &Z_patternImg, cv::Mat &Bottom_patternImg,
		int horizonPixelValue, int verticalPixelValue, int bottomPixelValue, int insidePixelValue,
		bool perimeterB, bool perimeterC, bool perimeterM, bool perimeterY,
		bool up_downB, bool up_downC, bool up_downM, bool up_downY,
		bool bottomB, bool bottomC, bool bottomM, bool bottomY, bool inv = true)
	{
		if (perimeter_mask.type() != CV_8UC1)
			cv::cvtColor(perimeter_mask, perimeter_mask, CV_BGR2GRAY);
		if (top_mask.type() != CV_8UC1)
			cv::cvtColor(top_mask, top_mask, CV_BGR2GRAY);
		if (capDownMask.type() != CV_8UC1)
			cv::cvtColor(capDownMask, capDownMask, CV_BGR2GRAY);

		for (int r = 0; r < capTemp.rows; r++)
			for (int c = 0; c < capTemp.cols; c++)
			{


				if (capTempMask.at<uchar>(r, c) == 0 && top_mask.at<uchar>(r, c) == 255 && Z_patternImg.at<uchar>(r, c) == 0)
				{

					capTemp.at<uchar>(r, c) = verticalPixelValue;
					//capTemp.at<uchar>(r, c) = 255;
					colorImgOut.at<cv::Vec3b>(r, c) = Vec3b(verticalPixelValue, verticalPixelValue, verticalPixelValue);

				}
				if (capTempMask.at<uchar>(r, c) == 0 && perimeter_mask.at<uchar>(r, c) == 0 && H_patternImg.at<uchar>(r, c) == 0)
				{

					capTemp.at<uchar>(r, c) = horizonPixelValue;
					//capTemp.at<uchar>(r, c) = 255;
					colorImgOut.at<cv::Vec3b>(r, c) = Vec3b(horizonPixelValue, horizonPixelValue, horizonPixelValue);
				}

				if ((capDownMask.at<uchar>(r, c) == 255 || capDownSureMask.at<uchar>(r, c) == 255) && capTempMask.at<uchar>(r, c) == 0 && Bottom_patternImg.at<uchar>(r, c) == 0)
				{

					capTemp.at<uchar>(r, c) = bottomPixelValue;
					//BGR
					//capTemp.at<uchar>(r, c) = 255;
					colorImgOut.at<cv::Vec3b>(r, c) = Vec3b(bottomPixelValue, bottomPixelValue, bottomPixelValue);
				}
				if (capTempMask.at<uchar>(r, c) == 0 && perimeter_mask.at<uchar>(r, c) != 0 && top_mask.at<uchar>(r, c) != 255 && capDownMask.at<uchar>(r, c) != 255)
				{
					capTemp.at<uchar>(r, c) = insidePixelValue;
					//capTemp.at<uchar>(r, c) = 255;
					colorImgOut.at<cv::Vec3b>(r, c) = Vec3b(insidePixelValue, insidePixelValue, insidePixelValue);
				}

			}



	}




	static void bottomTopMergeColorImage(cv::Mat &bottomMask, cv::Mat &TopMask, cv::Mat &bottomSureMask, cv::Mat &topSureMask, cv::Mat &capMask,
		cv::Mat &bottom_color, cv::Mat &top_color, cv::Mat &outlineColor
		, cv::Mat &output, cv::Mat &secondBinderMask2)
	{
		if (bottomMask.type() != CV_8UC1)
			cv::cvtColor(bottomMask, bottomMask, CV_BGR2GRAY);
		if (TopMask.type() != CV_8UC1)
			cv::cvtColor(TopMask, TopMask, CV_BGR2GRAY);
		if (capMask.type() != CV_8UC1)
			cv::cvtColor(capMask, capMask, CV_BGR2GRAY);
		if (secondBinderMask2.type() != CV_8UC1)
			cv::cvtColor(secondBinderMask2, secondBinderMask2, CV_BGR2GRAY);

		for (int r = 0; r < capMask.rows; r++)
			for (int c = 0; c < capMask.cols; c++)
			{
				if ((bottomMask.at<uchar>(r, c) == 255 || bottomSureMask.at<uchar>(r, c) == 255) && capMask.at<uchar>(r, c) == 0)
				{
					output.at<cv::Vec3b>(r, c) = bottom_color.at<cv::Vec3b>(r, c);
					secondBinderMask2.at<uchar>(r, c) = 0;
				}
				else if ((TopMask.at<uchar>(r, c) == 255 || topSureMask.at<uchar>(r, c) == 255) && capMask.at<uchar>(r, c) == 0)
				{
					output.at<cv::Vec3b>(r, c) = top_color.at<cv::Vec3b>(r, c);
					secondBinderMask2.at<uchar>(r, c) = 0;
				}
				//captemp==0 
				if (bottomMask.at<uchar>(r, c) == 0 && TopMask.at<uchar>(r, c) == 0 && capMask.at<uchar>(r, c) == 0)
				{
					output.at<cv::Vec3b>(r, c) = outlineColor.at<cv::Vec3b>(r, c);
					secondBinderMask2.at<uchar>(r, c) = 0;
				}

			}

	}

	static void bottomTopMergeColorImage_Multi(QVector<cv::Mat> &bottomSureMaskV, QVector<cv::Mat> &topSureMaskV, QVector<cv::Mat> &outlineMaskMiniV, cv::Mat &capMask,
		cv::Mat &bottom_color, cv::Mat &top_color, cv::Mat &outlineColor, QVector<cv::Mat> &outlineColorMiniV
		, cv::Mat &output, cv::Mat &secondBinderMask2)
	{
		for (int x = 0; x < bottomSureMaskV.size(); x++)
		{
			if (bottomSureMaskV[x].type() != CV_8UC1)
				cv::cvtColor(bottomSureMaskV[x], bottomSureMaskV[x], CV_BGR2GRAY);
			if (topSureMaskV[x].type() != CV_8UC1)
				cv::cvtColor(topSureMaskV[x], topSureMaskV[x], CV_BGR2GRAY);
		}
		for (int x = 0; x < outlineMaskMiniV.size(); x++)
		{
			if (outlineMaskMiniV[x].type() != CV_8UC1)
				cv::cvtColor(outlineMaskMiniV[x], outlineMaskMiniV[x], CV_BGR2GRAY);

		}
		if (capMask.type() != CV_8UC1)
			cv::cvtColor(capMask, capMask, CV_BGR2GRAY);
		if (secondBinderMask2.type() != CV_8UC1)
			cv::cvtColor(secondBinderMask2, secondBinderMask2, CV_BGR2GRAY);

		for (int r = 0; r < capMask.rows; r++)
			for (int c = 0; c < capMask.cols; c++)
			{
				int recordX = -1;
				int recordTopOrBottom = -1;
				bool getColor = false;
				for (int x = 0; x < bottomSureMaskV.size(); x++)
				{
					if (bottomSureMaskV[x].at<uchar>(r, c) == 255 && capMask.at<uchar>(r, c) == 0)
					{
						output.at<cv::Vec3b>(r, c) = bottom_color.at<cv::Vec3b>(r, c);
						if (outlineMaskMiniV[x].at<uchar>(r, c) == 0)
							output.at<cv::Vec3b>(r, c) = outlineColorMiniV[x].at<cv::Vec3b>(r, c);
						secondBinderMask2.at<uchar>(r, c) = 0;


						getColor = true;
						break;
					}
					else if (topSureMaskV[x].at<uchar>(r, c) == 255 && capMask.at<uchar>(r, c) == 0)
					{
						output.at<cv::Vec3b>(r, c) = top_color.at<cv::Vec3b>(r, c);
						if (outlineMaskMiniV[x].at<uchar>(r, c) == 0)
							output.at<cv::Vec3b>(r, c) = outlineColorMiniV[x].at<cv::Vec3b>(r, c);

						secondBinderMask2.at<uchar>(r, c) = 0;


						getColor = true;
						break;
					}
				}
				if (!getColor)
				{
					if (capMask.at<uchar>(r, c) == 0)
					{
						output.at<cv::Vec3b>(r, c) = outlineColor.at<cv::Vec3b>(r, c);
						secondBinderMask2.at<uchar>(r, c) = 0;
					}

				}




				/*int recordX = 0;
				for (int x = 0; x < bottomSureMaskV.size(); x++)
				{
				if (bottomSureMaskV[x].at<uchar>(r, c) == 255 && topSureMaskV[0].at<uchar>(r,c)==0 &&capMask.at<uchar>(r, c) == 0)
				{
				output.at<cv::Vec3b>(r, c) = bottom_color.at<cv::Vec3b>(r, c);
				secondBinderMask2.at<uchar>(r, c) = 0;
				recordX = x;
				break;
				}
				else if (topSureMaskV[x].at<uchar>(r, c) == 255 && bottomSureMaskV[0].at<uchar>(r, c) == 0 && capMask.at<uchar>(r, c) == 0)
				{
				output.at<cv::Vec3b>(r, c) = top_color.at<cv::Vec3b>(r, c);
				secondBinderMask2.at<uchar>(r, c) = 0;
				recordX = x;
				break;
				}
				}*/

				//captemp==0 
				/*if (bottomSureMaskV[recordX].at<uchar>(r, c) == 0 && topSureMaskV[recordX].at<uchar>(r, c) == 0 && capMask.at<uchar>(r, c) == 0)
				{
				output.at<cv::Vec3b>(r, c) = outlineColor.at<cv::Vec3b>(r, c);
				secondBinderMask2.at<uchar>(r, c) = 0;
				}*/

			}

	}


	static void GrayScalePerimeter(cv::Mat &capTempOut, cv::Mat &colorImgOut, cv::Mat &capTempMask, cv::Mat &perimeter_mask, cv::Mat &up_down_mask, bool perimeterB, bool perimeterC, bool perimeterM, bool perimeterY, bool up_downB, bool up_downC, bool up_downM, bool up_downY)
	{
		if (perimeter_mask.type() != CV_8UC1)
			cv::cvtColor(perimeter_mask, perimeter_mask, CV_BGR2GRAY);
		if (up_down_mask.type() != CV_8UC1)
			cv::cvtColor(up_down_mask, up_down_mask, CV_BGR2GRAY);
		for (int r = 0; r < capTempMask.rows; r++)
			for (int c = 0; c < capTempMask.cols; c++)
			{
				if (capTempMask.at<uchar>(r, c) == 0 && perimeter_mask.at<uchar>(r, c) == 0)
				{
					if (!perimeterB)
						capTempOut.at<uchar>(r, c) = 255;
					//BGR
					//if (!perimeterC)
					//	colorImgOut.at<cv::Vec3b>(r, c).val[2] = 255;//
					//else
					//	colorImgOut.at<cv::Vec3b>(r, c).val[2] = 0;
					//if (!perimeterM)
					//	colorImgOut.at<cv::Vec3b>(r, c).val[1] = 255;
					//else
					//	colorImgOut.at<cv::Vec3b>(r, c).val[1] = 0;
					//if (!perimeterY)
					//	colorImgOut.at<cv::Vec3b>(r, c).val[0] = 255;
					//else
					//	colorImgOut.at<cv::Vec3b>(r, c).val[0] = 0;				

					colorImgOut.at<cv::Vec3b>(r, c) = Vec3b(perimeterY ? 0 : 255, perimeterM ? 0 : 255, perimeterC ? 0 : 255);

				}

				if (capTempMask.at<uchar>(r, c) == 0 && up_down_mask.at<uchar>(r, c) == 0)
				{
					if (!up_downB)
						capTempOut.at<uchar>(r, c) = 255;
					/*if (!up_downC)
						colorImgOut.at<cv::Vec3b>(r, c).val[2] = 255;
						else
						colorImgOut.at<cv::Vec3b>(r, c).val[2] = 0;
						if (!up_downM)
						colorImgOut.at<cv::Vec3b>(r, c).val[1] = 255;
						else
						colorImgOut.at<cv::Vec3b>(r, c).val[1] = 0;
						if (!up_downY)
						colorImgOut.at<cv::Vec3b>(r, c).val[0] = 255;
						else
						colorImgOut.at<cv::Vec3b>(r, c).val[0] = 0;*/

					colorImgOut.at<cv::Vec3b>(r, c) = Vec3b(up_downY ? 0 : 255, up_downM ? 0 : 255, up_downC ? 0 : 255);

				}

			}


		//return capTemp;
	}

	static void BottomLighter(cv::Mat &capMask, cv::Mat &capDownMask, cv::Mat &capTempIn, cv::Mat &colorIn, bool bottomB, bool bottomC, bool bottomM, bool bottomY)
	{
		if (capMask.type() != CV_8UC1)
			cv::cvtColor(capMask, capMask, CV_BGR2GRAY);
		if (capDownMask.type() != CV_8UC1)
			cv::cvtColor(capDownMask, capDownMask, CV_BGR2GRAY);
		for (int r = 0; r < capMask.rows; r++)
		{
			for (int c = 0; c < capMask.cols; c++)
			{
				if (capDownMask.at<uchar>(r, c) == 255 && capMask.at<uchar>(r, c) == 0)
				{

					if (!bottomB)
						capTempIn.at<uchar>(r, c) = 255;
					//BGR
					if (!bottomC)
						colorIn.at<cv::Vec3b>(r, c).val[2] = 255;//
					else
						colorIn.at<cv::Vec3b>(r, c).val[2] = 0;

					if (!bottomM)
						colorIn.at<cv::Vec3b>(r, c).val[1] = 255;
					else
						colorIn.at<cv::Vec3b>(r, c).val[1] = 0;

					if (!bottomY)
						colorIn.at<cv::Vec3b>(r, c).val[0] = 255;
					else
						colorIn.at<cv::Vec3b>(r, c).val[0] = 0;


				}
			}
		}
	}

	static void BottomLighter_binder(cv::Mat &capMask, cv::Mat &capDownMask, cv::Mat &capTempIn, int percentage)
	{
		if (capMask.type() != CV_8UC1)
			cv::cvtColor(capMask, capMask, CV_BGR2GRAY);
		if (capDownMask.type() != CV_8UC1)
			cv::cvtColor(capDownMask, capDownMask, CV_BGR2GRAY);
		int percentValue = 255 * (100 - percentage) / 100.;
		for (int r = 0; r < capMask.rows; r++)
		{
			for (int c = 0; c < capMask.cols; c++)
			{
				if (capDownMask.at<uchar>(r, c) == 255 && capMask.at<uchar>(r, c) == 0)
				{


					capTempIn.at<uchar>(r, c) = percentValue;



				}
			}
		}
	}


	static bool createPatternPercent(cv::Mat &src, int percent)
	{
		int midR = src.rows / 2;
		int midC = src.cols / 2;

		int maxrectlength = src.cols;//===groove size
		int dots = (maxrectlength*maxrectlength) * (percent / 100.);
		double steps = (maxrectlength*maxrectlength) / (double)dots;


		int nc = 0;
		if (src.isContinuous())
		{
			nc = src.rows*src.cols;
		}
		uchar* data = src.ptr<uchar>(0);
		double j = 0;
		for (int i = 0; i < nc; j += steps)
		{
			i = std::ceil(j);
			data[i] = 0;
		}

		return true;
	}

	static cv::Mat getInnerMask(cv::Mat *capTemp, cv::Mat *innerOutlineMask)
	{
		cv::Mat  innerGrayMask(innerOutlineMask->size(), CV_8UC1);
		innerGrayMask.setTo(cv::Scalar(255));
		cv::cvtColor(*innerOutlineMask, *innerOutlineMask, CV_BGR2GRAY);
		for (int r = 0; r < capTemp->rows; r++)
			for (int c = 0; c < capTemp->cols; c++)
			{
				if (capTemp->at<uchar>(r, c) == 0 && innerOutlineMask->at<uchar>(r, c) == 255)
				{
					innerGrayMask.at<uchar>(r, c) = 0;//become white except mask area				
				}
			}
		return innerGrayMask;
	}
	static void compareWithHexagonImage(cv::Mat *innerOutlineMask, cv::Mat *hexagonImage)
	{
		for (int r = 0; r < innerOutlineMask->rows; r++)
			for (int c = 0; c < innerOutlineMask->cols; c++)
			{
				if (hexagonImage->at<uchar>(r, c) == 254)
				{
					innerOutlineMask->at<uchar>(r, c) = 255;//become white except mask area				
				}
				//else innerOutlineMask->at<uchar>(r, c) = 255;
			}
	}
	static void combineGrayImageAndColorImage(cv::Mat *colorImage, cv::Mat *grayImage)
	{
		//cv::Mat temp(colorImage->size(), colorImage->type());
		//temp.setTo(cv::Scalar(255));
		cvtColor(*grayImage, *grayImage, CV_GRAY2BGR);

		for (int r = 0; r < colorImage->rows; r++)
			for (int c = 0; c < colorImage->cols; c++)
			{
				if (colorImage->at<cv::Vec3b>(r, c) == cv::Vec3b(255, 255, 255))
				{
					colorImage->at<cv::Vec3b>(r, c) = grayImage->at<cv::Vec3b>(r, c);//become white except mask area			
					//qDebug()<< grayImage->at<cv::Vec3b>(r, c).val[0];
				}
				//else innerOutlineMask->at<uchar>(r, c) = 255;
			}
		//addWeighted(*colorImage, 0.0, temp, 0.0, 0.0, *colorImage);
	}

	static void newUselessbar(cv::Mat &src, float uselessprint, pixelFormat pixformat)
	{

		int uselessstart = 59;
		int mm_pixel = 118 * uselessprint;
		int rect_s1 = uselessstart;
		int rect_s2 = rect_s1 + mm_pixel;
		int rect_s3 = rect_s2 + mm_pixel;
		int rect_e3 = rect_s3 + mm_pixel;
		int srcrows = src.rows;
		int bRect1 = rect_e3 + 15;
		int bRect2 = bRect1 + 59;
		switch (src.type())
		{
		case CV_8UC3:
		{
			if (pixformat == pixelFormat::RGB)
			{
				cv::rectangle(src, cv::Point(rect_s1, 0), cv::Point(rect_s2, srcrows), cv::Scalar(255, 0, 255), CV_FILLED);
				cv::rectangle(src, cv::Point(rect_s2, 0), cv::Point(rect_s3, srcrows), cv::Scalar(255, 255, 0), CV_FILLED);
				cv::rectangle(src, cv::Point(rect_s3, 0), cv::Point(rect_e3, srcrows), cv::Scalar(0, 255, 255), CV_FILLED);
			}
			else if (pixformat == pixelFormat::CMY)
			{
				cv::rectangle(src, cv::Point(rect_s1, 0), cv::Point(rect_s2, srcrows), cv::Scalar(0, 255, 0), CV_FILLED);
				cv::rectangle(src, cv::Point(rect_s2, 0), cv::Point(rect_s3, srcrows), cv::Scalar(255, 0, 0), CV_FILLED);
				cv::rectangle(src, cv::Point(rect_s3, 0), cv::Point(rect_e3, srcrows), cv::Scalar(0, 0, 255), CV_FILLED);
			}
		}break;
		case CV_8UC1:
		{
			cv::rectangle(src, cv::Point(bRect1, 0), cv::Point(bRect2, srcrows), cv::Scalar(0), CV_FILLED);


		}break;
		}
		//****thomas Test*****
		//int uselessstart = 59;
		//int mm_pixel = 118 * uselessprint;
		//int mRrect1 = uselessstart;
		//int cRrect1 = mRrect1 + mm_pixel;
		//int yRrect1 = cRrect1 + mm_pixel;
		//int yRrect2 = yRrect1 + mm_pixel;

		//int mRrect2 = yRrect2+59;
		//int cRrect2 = mRrect2 + 12;
		//int yRrect3 = cRrect2 + 12;
		//int yRrect3end = yRrect3 + 12;

		//int srcrows = src.rows;

		//int bRect1 = yRrect3end + 15;
		//int bRect1end = bRect1 + 236;
		//switch (src.type())
		//{
		//case CV_8UC3:
		//{
		//				if (pixformat == pixelFormat::RGB)
		//				{
		//					cv::rectangle(src, cv::Point(mRrect1, 0), cv::Point(cRrect1, srcrows), cv::Scalar(255, 0, 255), CV_FILLED);
		//					cv::rectangle(src, cv::Point(cRrect1, 0), cv::Point(yRrect1, srcrows), cv::Scalar(255, 255, 0), CV_FILLED);
		//					cv::rectangle(src, cv::Point(yRrect1, 0), cv::Point(yRrect2, srcrows), cv::Scalar(0, 255, 255), CV_FILLED);

		//					cv::rectangle(src, cv::Point(mRrect2, 0), cv::Point(cRrect2, srcrows), cv::Scalar(255, 0, 255), CV_FILLED);
		//					cv::rectangle(src, cv::Point(cRrect2, 0), cv::Point(yRrect3, srcrows), cv::Scalar(255, 255, 0), CV_FILLED);
		//					cv::rectangle(src, cv::Point(yRrect3, 0), cv::Point(yRrect3end, srcrows), cv::Scalar(0, 255, 255), CV_FILLED);
		//				}
		//				else if (pixformat == pixelFormat::CMY)
		//				{
		//					cv::rectangle(src, cv::Point(mRrect1, 0), cv::Point(cRrect1, srcrows), cv::Scalar(0, 255, 0), CV_FILLED);
		//					cv::rectangle(src, cv::Point(cRrect1, 0), cv::Point(yRrect1, srcrows), cv::Scalar(0, 0, 255), CV_FILLED);
		//					cv::rectangle(src, cv::Point(yRrect1, 0), cv::Point(yRrect2, srcrows), cv::Scalar(255, 0, 0), CV_FILLED);

		//					cv::rectangle(src, cv::Point(mRrect2, 0), cv::Point(cRrect2, srcrows), cv::Scalar(0, 255, 0), CV_FILLED);
		//					cv::rectangle(src, cv::Point(cRrect2, 0), cv::Point(yRrect3, srcrows), cv::Scalar(0, 0, 255), CV_FILLED);
		//					cv::rectangle(src, cv::Point(yRrect3, 0), cv::Point(yRrect3end, srcrows), cv::Scalar(255, 0, 0), CV_FILLED);

		//				}
		//}break;
		//case CV_8UC1:
		//{
		//				//cv::rectangle(src, cv::Point(bRect1, 0), cv::Point(bRect2, srcrows), cv::Scalar(0), CV_FILLED);
		//				cv::rectangle(src, cv::Point(bRect1, 0), cv::Point(bRect1end, srcrows), cv::Scalar(0), CV_FILLED);


		//}break;
		//}

	}
	void sacrificePattern(cv::Mat &outPatternImg, int rowPixels, int cols)
	{
		switch (outPatternImg.type())
		{
		case CV_8UC3:
		{
			cv::Mat outColorPattern(cv::Size(cols, rowPixels * 3), CV_8UC3, cv::Scalar(255, 255, 255));
			cv::rectangle(outColorPattern, cv::Point(0, 0), cv::Point(cols, rowPixels), cv::Scalar(255, 0, 255), CV_FILLED);
			cv::rectangle(outColorPattern, cv::Point(0, rowPixels), cv::Point(cols, rowPixels * 2), cv::Scalar(255, 255, 0), CV_FILLED);
			cv::rectangle(outColorPattern, cv::Point(0, rowPixels * 2), cv::Point(cols, rowPixels * 3), cv::Scalar(0, 255, 255), CV_FILLED);
			outColorPattern.copyTo(outPatternImg);
		}break;
		case CV_8UC1:
		{
			cv::Mat outBinderPattern(cv::Size(cols, rowPixels), CV_8UC1, cv::Scalar(255));
			cv::rectangle(outBinderPattern, cv::Point(0, 0), cv::Point(cols, rowPixels), cv::Scalar(0), CV_FILLED);
			outBinderPattern.copyTo(outPatternImg);
		}break;
		}

	}
	static void genSacrificeBar(cv::Mat &inColor, cv::Mat &inBinder, std::vector<int> rowVector, int unitpixels)
	{
		//create sacrificeBarPattern		
		int size_rows = inColor.rows;
		int size_cols = inColor.cols;

		cv::Mat patternColor(0, 0, CV_8UC3);
		cv::Mat patternBinder(0, 0, CV_8UC1);
		sacrificePattern(patternColor, unitpixels, size_cols);
		sacrificePattern(patternBinder, unitpixels, size_cols);


		for (int i = 0; i < rowVector.size(); i++)
		{
			patternColor.copyTo(inColor(cv::Rect(0, rowVector.at(i), size_cols, unitpixels * 3)));
			patternBinder.copyTo(inBinder(cv::Rect(0, rowVector.at(i) + unitpixels * 3, size_cols, unitpixels)));
		}


		/*patternColor.copyTo(inColor(cv::Rect(0, rowOne, size_cols, unitpixels * 3)));
		patternColor.copyTo(inColor(cv::Rect(0, rowTwo, size_cols, unitpixels * 3)));
		patternColor.copyTo(inColor(cv::Rect(0, rowThree, size_cols, unitpixels * 3)));


		patternBinder.copyTo(inBinder(cv::Rect(0, rowOne + unitpixels * 3, size_cols, unitpixels)));
		patternBinder.copyTo(inBinder(cv::Rect(0, rowTwo + unitpixels * 3, size_cols, unitpixels)));
		patternBinder.copyTo(inBinder(cv::Rect(0, rowThree + unitpixels * 3, size_cols, unitpixels)));*/



		/*for (int i = 0; i < size_rows; i += 1)
		{

		if ((i) < size_rows)
		{
		patternColor.copyTo(inColor(cv::Rect(0, rowOne, size_cols, unitpixels * 3)));

		}

		}*/




	}



	/*SPITTOON*/
	static void newUselessbar_custom(cv::Mat &src, float uselessprint, pixelFormat pixformat, float spittoon_m, float spittoon_c, float spittoon_y, float spittoon_b, float dpi,int start_position = 59)
	{

		int uselessstart = start_position;
		int cm_pixel = 118 * uselessprint;
		float mm_pixel = dpi / 25.4;

		int m_height_pixel = mm_pixel * spittoon_m;
		int c_height_pixel = mm_pixel * spittoon_c;
		int y_height_pixel = mm_pixel * spittoon_y;
		int b_height_pixel = mm_pixel * spittoon_b;

		int rect_M_s = uselessstart;
		int rect_C_s = rect_M_s + m_height_pixel;
		int rect_Y_s = rect_C_s + c_height_pixel;
		int rect_Y_e = rect_Y_s + y_height_pixel;
		int srcrows = src.rows;
		int bRect_s = rect_Y_e + 15;
		int bRect_e = bRect_s + b_height_pixel;
		switch (src.type())
		{
		case CV_8UC3:
		{
			if (pixformat == pixelFormat::RGB)
			{
				if (rect_M_s != rect_C_s)
					cv::rectangle(src, cv::Point(rect_M_s, 0), cv::Point(rect_C_s, srcrows), cv::Scalar(255, 0, 255), CV_FILLED);
				if (rect_C_s != rect_Y_s)
					cv::rectangle(src, cv::Point(rect_C_s, 0), cv::Point(rect_Y_s, srcrows), cv::Scalar(255, 255, 0), CV_FILLED);
				if (rect_Y_s != rect_Y_e)
					cv::rectangle(src, cv::Point(rect_Y_s, 0), cv::Point(rect_Y_e, srcrows), cv::Scalar(0, 255, 255), CV_FILLED);
			}
			else if (pixformat == pixelFormat::CMY)
			{
				cv::rectangle(src, cv::Point(rect_M_s, 0), cv::Point(rect_C_s, srcrows), cv::Scalar(0, 255, 0), CV_FILLED);
				cv::rectangle(src, cv::Point(rect_C_s, 0), cv::Point(rect_Y_s, srcrows), cv::Scalar(255, 0, 0), CV_FILLED);
				cv::rectangle(src, cv::Point(rect_Y_s, 0), cv::Point(rect_Y_e, srcrows), cv::Scalar(0, 0, 255), CV_FILLED);
			}
		}break;
		case CV_8UC1:
		{
			cv::rectangle(src, cv::Point(bRect_s, 0), cv::Point(bRect_e, srcrows), cv::Scalar(0), CV_FILLED);


		}break;
		}


	}

	/*SPITTOON*/
	static void newUselessbar_custom_DM(cv::Mat &src, float uselessprint, pixelFormat pixformat, float spittoon_m, float spittoon_c, float spittoon_y, float spittoon_b, float dpi,int start_position = 59)
	{

		int uselessstart = start_position;
		int cm_pixel = 118 * uselessprint;
		float mm_pixel = dpi / 25.4;

		int m_height_pixel = mm_pixel * spittoon_m;
		int c_height_pixel = mm_pixel * spittoon_c;
		int y_height_pixel = mm_pixel * spittoon_y;
		int b_height_pixel = mm_pixel * spittoon_b;

		int rect_M_s = uselessstart;
		int rect_C_s = rect_M_s + m_height_pixel;
		int rect_Y_s = rect_C_s + c_height_pixel;
		int rect_Y_e = rect_Y_s + y_height_pixel;
		int srcrows = src.rows;
		int halfsrcrows = src.rows / 2;
		int bRect_s = rect_Y_e + 15;
		int bRect_e = bRect_s + b_height_pixel;
		switch (src.type())
		{
		case CV_8UC3:
		{
			if (pixformat == pixelFormat::RGB)
			{
				if (rect_M_s != rect_C_s){
					cv::rectangle(src, cv::Point(rect_M_s, 0), cv::Point(rect_C_s, halfsrcrows), cv::Scalar(255, 0, 255), CV_FILLED);
					cv::rectangle(src, cv::Point(rect_C_s + 1, halfsrcrows), cv::Point(rect_Y_s, srcrows), cv::Scalar(255, 0, 255), CV_FILLED);
				}
				/*if (rect_C_s != rect_Y_s)
					cv::rectangle(src, cv::Point(rect_C_s, 0), cv::Point(rect_Y_s, srcrows), cv::Scalar(255, 255, 0), CV_FILLED);
					if (rect_Y_s != rect_Y_e)
					cv::rectangle(src, cv::Point(rect_Y_s, 0), cv::Point(rect_Y_e, srcrows), cv::Scalar(0, 255, 255), CV_FILLED);*/
			}
			else if (pixformat == pixelFormat::CMY)
			{
				cv::rectangle(src, cv::Point(rect_M_s, 0), cv::Point(rect_C_s, srcrows), cv::Scalar(0, 255, 0), CV_FILLED);
				cv::rectangle(src, cv::Point(rect_C_s, 0), cv::Point(rect_Y_s, srcrows), cv::Scalar(255, 0, 0), CV_FILLED);
				cv::rectangle(src, cv::Point(rect_Y_s, 0), cv::Point(rect_Y_e, srcrows), cv::Scalar(0, 0, 255), CV_FILLED);
			}
		}break;
		case CV_8UC1:
		{
			//cv::rectangle(src, cv::Point(bRect_s, 0), cv::Point(bRect_e, srcrows), cv::Scalar(0), CV_FILLED);


		}break;
		}


	}


	static void newUselessbarBlank(cv::Mat &src, float uselessprint, pixelFormat pixformat, float spittoon_m, float spittoon_c, float spittoon_y, float spittoon_b, float dpi)
	{
		int uselessstart = 59;
		int mm_pixel = 118 * uselessprint;
		int mRrect1 = uselessstart;
		int cRrect1 = mRrect1 + mm_pixel;
		int yRrect1 = cRrect1 + mm_pixel;
		int yRrect2 = yRrect1 + 0.5 * mm_pixel;
		int srccols = src.cols;
		int bRect1 = yRrect2 + 15;
		int bRect2 = bRect1 + 59;
		switch (src.type())
		{
		case CV_8UC3:
		{
			if (pixformat == pixelFormat::RGB){
				cv::rectangle(src, cv::Point(0, mRrect1), cv::Point(srccols, cRrect1), cv::Scalar(255, 0, 255), CV_FILLED);
				cv::rectangle(src, cv::Point(0, cRrect1), cv::Point(srccols, yRrect1), cv::Scalar(255, 255, 0), CV_FILLED);
				cv::rectangle(src, cv::Point(0, yRrect1), cv::Point(srccols, yRrect2), cv::Scalar(0, 255, 255), CV_FILLED);
			}
			else if (pixformat == pixelFormat::CMY){
				cv::rectangle(src, cv::Point(0, mRrect1), cv::Point(srccols, cRrect1), cv::Scalar(0, 255, 0), CV_FILLED);
				cv::rectangle(src, cv::Point(0, cRrect1), cv::Point(srccols, yRrect1), cv::Scalar(0, 0, 255), CV_FILLED);
				cv::rectangle(src, cv::Point(0, yRrect1), cv::Point(srccols, yRrect2), cv::Scalar(255, 0, 0), CV_FILLED);

			}

		}break;
		case CV_8UC1:
		{
			cv::rectangle(src, cv::Point(0, bRect1), cv::Point(srccols, bRect2), cv::Scalar(0), CV_FILLED);

		}break;
		}

	}
	static void generateUselessbarDin(cv::Mat &src, float uselessprint, bool Din)
	{
		int greenTest = 0;
		int temptemp = 118 * uselessprint;
		int uselessstart = 59;
		//bb3
		int startfrom = 1547;
		//bb2
		//int startfrom = 2125;		
		int startfrom2 = startfrom + 12 * 3;
		switch (src.type())
		{
		case CV_8UC3:
		{
			/*for (int r = 0; r < 117; r++)
			for (int c = 0; c < src.cols; c++)
			{



			src.at<cv::Vec3b>(r, c).val[0] = 0;
			src.at<cv::Vec3b>(r, c).val[1] = 0;
			src.at<cv::Vec3b>(r, c).val[2] = 0;



			}*/


			/*for (int r = 0; r < 118; r++)
			for (int c = 0; c < src.cols; c++)
			{

			if (r<39)
			{
			src.at<cv::Vec3b>(r, c).val[0] = 0;
			src.at<cv::Vec3b>(r, c).val[1] = 255;
			src.at<cv::Vec3b>(r, c).val[2] = 255;
			}
			else if (r<39 * 2)
			{
			src.at<cv::Vec3b>(r, c).val[0] = 255;
			src.at<cv::Vec3b>(r, c).val[1] = 0;
			src.at<cv::Vec3b>(r, c).val[2] = 255;
			}
			else if (r < 39 * 3)
			{
			src.at<cv::Vec3b>(r, c).val[0] = 255;
			src.at<cv::Vec3b>(r, c).val[1] = 255;
			src.at<cv::Vec3b>(r, c).val[2] = 0;
			}


			}*/
			//間格犧牲條
			/*for (int r = 78; r < 273; r++)
			for (int c = 0; c < src.cols; c++)
			{

			if (r<39 * 3 && r>39 * 2)
			{
			src.at<cv::Vec3b>(r, c).val[0] = 255;
			src.at<cv::Vec3b>(r, c).val[1] = 255;
			src.at<cv::Vec3b>(r, c).val[2] = 0;
			}
			else if (r<39 * 5 && r>39 * 4)
			{
			src.at<cv::Vec3b>(r, c).val[0] = 255;
			src.at<cv::Vec3b>(r, c).val[1] = 0;
			src.at<cv::Vec3b>(r, c).val[2] = 255;
			}
			else if (r<39 * 7 && r>39 * 6)
			{
			src.at<cv::Vec3b>(r, c).val[0] = 0;
			src.at<cv::Vec3b>(r, c).val[1] = 255;
			src.at<cv::Vec3b>(r, c).val[2] = 255;
			}


			}*/

			for (int r = uselessstart; r < uselessstart + temptemp * 6; r++)
				for (int c = 0; c < src.cols; c++)
				{

					if (r < uselessstart + temptemp)
					{
						//m
						src.at<cv::Vec3b>(r, c).val[0] = 255;
						src.at<cv::Vec3b>(r, c).val[1] = 0;
						src.at<cv::Vec3b>(r, c).val[2] = 255;

					}
					else if (r<uselessstart + temptemp * 2 && r>uselessstart + temptemp)
					{
						//c
						src.at<cv::Vec3b>(r, c).val[0] = 255;
						src.at<cv::Vec3b>(r, c).val[1] = 255;
						src.at<cv::Vec3b>(r, c).val[2] = 0;
					}
					else if (r<uselessstart + temptemp * 2.5 && r>uselessstart + temptemp * 2)
					{
						//y
						src.at<cv::Vec3b>(r, c).val[0] = 0;
						src.at<cv::Vec3b>(r, c).val[1] = 255;
						src.at<cv::Vec3b>(r, c).val[2] = 255;
					}

					/*src.at<cv::Vec3b>(r, c).val[0] = 0;
					src.at<cv::Vec3b>(r, c).val[1] = 0;
					src.at<cv::Vec3b>(r, c).val[2] = 0;*/


				}
			//*****20160715
			if (Din){
				/*int temp2 = 12;

				for (int r = startfrom; r < temp2 * 3 + startfrom; r++)
				for (int c = 0; c < src.cols; c++)
				{

				if (r < temp2 + startfrom)
				{
				src.at<cv::Vec3b>(r, c).val[0] = 255;
				src.at<cv::Vec3b>(r, c).val[1] = 255;
				src.at<cv::Vec3b>(r, c).val[2] = 0;
				}
				else if (r<temp2 * 2 + startfrom && r>temp2 + startfrom)
				{
				src.at<cv::Vec3b>(r, c).val[0] = 255;
				src.at<cv::Vec3b>(r, c).val[1] = 0;
				src.at<cv::Vec3b>(r, c).val[2] = 255;
				}
				else if (r<temp2 * 3 + startfrom && r>temp2 * 2 + startfrom)
				{
				src.at<cv::Vec3b>(r, c).val[0] = 0;
				src.at<cv::Vec3b>(r, c).val[1] = 255;
				src.at<cv::Vec3b>(r, c).val[2] = 255;
				}


				}*/
			}


			break;
		}
		case CV_8UC1:
		{
			int binderStart = uselessstart + temptemp * 2.5 + greenTest + 15;
			//for (int r = temptemp * 0; r < temptemp * 3; r++)
			//for (int r = uselessstart + temptemp * 3 + greenTest + 15; r < uselessstart + temptemp * 36 + greenTest; r++)
			for (int r = binderStart; r < binderStart + 59; r++)//test 18, 59, 118, 236
				for (int c = 0; c < src.cols; c++)
				{

					//if (r < 39 * 3)//***20160422
					/*if (r < 39)
					{*/
					src.at<unsigned char>(r, c) = 0;

					//}

				}
			//******20160715
			if (Din){
				//int temp2 = 12;
				////int startfrom2 = 1748 + 12 * 3;


				//for (int r = startfrom2; r < temp2 * 2 + startfrom2; r++)
				//for (int c = 0; c < src.cols; c++)
				//{

				//	//if (r < 39 * 3)//***20160422
				//	/*if (r < 39)
				//	{*/
				//	src.at<unsigned char>(r, c) = 0;

				//	//}

				//}
			}
			//************
			break;
		}

		}


	}

	static void generateUselessbarCol(cv::Mat &src, float uselessprint, bool Din)
	{
		int greenTest = 0;
		int temptemp = 118 * uselessprint;
		int uselessstart = 59;
		//bb3
		int startfrom = 1547;
		//bb2
		//int startfrom = 2125;		
		int startfrom2 = startfrom + 12 * 3;
		switch (src.type())
		{
		case CV_8UC3:
		{


			for (int c = uselessstart; c < uselessstart + temptemp * 6; c++)
				for (int r = 0; r < src.rows; r++)
				{

					if (c < uselessstart + temptemp)
					{
						//m
						src.at<cv::Vec3b>(r, c).val[0] = 255;
						src.at<cv::Vec3b>(r, c).val[1] = 0;
						src.at<cv::Vec3b>(r, c).val[2] = 255;

					}
					else if (c<uselessstart + temptemp * 2 && c>uselessstart + temptemp)
					{
						//c
						src.at<cv::Vec3b>(r, c).val[0] = 255;
						src.at<cv::Vec3b>(r, c).val[1] = 255;
						src.at<cv::Vec3b>(r, c).val[2] = 0;
					}
					else if (c<uselessstart + temptemp * 2.5 && c>uselessstart + temptemp * 2)
					{
						//y
						src.at<cv::Vec3b>(r, c).val[0] = 0;
						src.at<cv::Vec3b>(r, c).val[1] = 255;
						src.at<cv::Vec3b>(r, c).val[2] = 255;
					}

					/*src.at<cv::Vec3b>(r, c).val[0] = 0;
					src.at<cv::Vec3b>(r, c).val[1] = 0;
					src.at<cv::Vec3b>(r, c).val[2] = 0;*/


				}



			break;
		}
		case CV_8UC1:
		{
			int binderStart = uselessstart + temptemp * 2.5 + greenTest + 15;
			//for (int r = temptemp * 0; r < temptemp * 3; r++)
			//for (int r = uselessstart + temptemp * 3 + greenTest + 15; r < uselessstart + temptemp * 36 + greenTest; r++)
			for (int c = binderStart; c < binderStart + 59; c++)//test 18, 59, 118, 236
				for (int r = 0; r < src.rows; r++)
				{

					//if (r < 39 * 3)//***20160422
					/*if (r < 39)
					{*/
					src.at<unsigned char>(r, c) = 0;

					//}

				}

			break;
		}

		}


	}

	static void createBlankPages(bool doubleprint, double widthcm, double total_printlengthcm, float uselessprint, int times, std::vector<cv::Mat>&outlineImg, std::vector<cv::Mat> &capImg, pixelFormat _pixelFormat, float spittoon_m, float spittoon_c, float spittoon_y, float spittoon_b, float dpi)
	{
		int widthpixel = (widthcm / DSP_inchmm) * dpi;
		int lengthpixel = (total_printlengthcm / DSP_inchmm) * dpi;
		cv::Mat out(widthpixel, lengthpixel, CV_8UC3);
		cv::Mat out2(widthpixel, lengthpixel, CV_8UC1, Scalar(255));
		cv::Mat out3(widthpixel, lengthpixel, CV_8UC3);
		cv::Mat out4(widthpixel, lengthpixel, CV_8UC1, Scalar(255));

		cv::Mat outt(lengthpixel, widthpixel, CV_8UC3);
		cv::Mat outt2(lengthpixel, widthpixel, CV_8UC1, Scalar(255));
		cv::Mat outt3(lengthpixel, widthpixel, CV_8UC3);
		cv::Mat outt4(lengthpixel, widthpixel, CV_8UC1, Scalar(255));
		if (_pixelFormat == pixelFormat::RGB)
		{
			out.setTo(cv::Scalar(255, 255, 255));
			out3.setTo(cv::Scalar(255, 255, 255));
		}
		else if (_pixelFormat == pixelFormat::CMY)
		{
			out.setTo(cv::Scalar(0, 0, 0));
			out3.setTo(cv::Scalar(0, 0, 0));
		}

		out2.setTo(cv::Scalar(255));
		newUselessbar_custom(out, uselessprint, _pixelFormat, spittoon_m, spittoon_c, spittoon_y, spittoon_b, dpi);
		newUselessbar_custom(out2, uselessprint, _pixelFormat, spittoon_m, spittoon_c, spittoon_y, spittoon_b, dpi);

		SKT::rotateImage(out, outt);
		SKT::rotateImage(out2, outt2);

		SKT::rotateImage(out3, outt3);
		SKT::rotateImage(out4, outt4);

		//cutImage(out2, out);
		for (int i = 0; i < times; i++)
		{
			if (i % 2 == 1 && doubleprint)
			{
				outlineImg.push_back(outt3);
				capImg.push_back(outt4);
			}
			else
			{
				outlineImg.push_back(outt);
				capImg.push_back(outt2);
			}
		}


	}

	static void createTestPrintPages(double widthcm, double plus_printlength, double x_groove_length, float uselessprint, int times, std::vector<cv::Mat>&outlineImg, std::vector<cv::Mat> &capImg)
	{
		int widthpixel = (widthcm / DSP_inchmm) * 300;
		int lengthpixel = ((/*plus_printlength + */x_groove_length) / DSP_inchmm) * 300;

		int colorRows = 20;
		int colorRowsPixel = (colorRows / DSP_inchmm) * 300;

		int blankRows = 20;
		int blankRowsPixel = (blankRows / DSP_inchmm) * 300;

		int startQ1P1 = (plus_printlength / DSP_inchmm) * 300;
		int startQ1P2 = startQ1P1 + colorRowsPixel;

		int startQ2P1 = startQ1P2 + blankRowsPixel;
		int startQ2P2 = startQ2P1 + colorRowsPixel;

		int startQ3P1 = startQ2P2 + blankRowsPixel;
		int startQ3P2 = startQ3P1 + colorRowsPixel;

		cv::Mat out(lengthpixel, widthpixel, CV_8UC3);
		cv::Mat out2(lengthpixel, widthpixel, CV_8UC1);
		out.setTo(cv::Scalar(255, 255, 255));
		out2.setTo(cv::Scalar(255));

		cv::rectangle(out, cv::Point(0, startQ1P1), cv::Point(widthpixel, startQ1P2), cv::Scalar(255, 255, 0), CV_FILLED);
		cv::rectangle(out, cv::Point(0, startQ2P1), cv::Point(widthpixel, startQ2P2), cv::Scalar(0, 255, 255), CV_FILLED);
		cv::rectangle(out, cv::Point(0, startQ3P1), cv::Point(widthpixel, startQ3P2), cv::Scalar(255, 0, 255), CV_FILLED);


		generateUselessbarDin(out, uselessprint, false);
		generateUselessbarDin(out2, uselessprint, false);

		cv::rectangle(out2, cv::Point(0, startQ1P2), cv::Point(widthpixel, startQ2P1), cv::Scalar(0), CV_FILLED);
		cv::rectangle(out2, cv::Point(0, startQ2P2), cv::Point(widthpixel, startQ3P1), cv::Scalar(0), CV_FILLED);

		//lengthpixel = 4204;
		//widthpixel = 2622;
		cv::Mat outwhite(lengthpixel, widthpixel, CV_8UC3);
		cv::Mat out2white(lengthpixel, widthpixel, CV_8UC1);
		outwhite.setTo(cv::Scalar(255, 255, 255));
		out2white.setTo(cv::Scalar(255));


		//cutImage(out2, out);
		for (int i = 0; i < 1; i++)
		{
			outlineImg.push_back(out);
			capImg.push_back(out2);
		}

		/*for (int i = 0; i < 1; i++)
		{
		outlineImg.push_back(outwhite);
		capImg.push_back(out2white);
		}*/


	}

	static void generateUselessbar(cv::Mat &src, float uselessprint)
	{

		int temptemp = 118 * uselessprint;
		switch (src.type())
		{
		case CV_8UC3:
		{
			/*for (int r = 0; r < 117; r++)
			for (int c = 0; c < src.cols; c++)
			{



			src.at<cv::Vec3b>(r, c).val[0] = 0;
			src.at<cv::Vec3b>(r, c).val[1] = 0;
			src.at<cv::Vec3b>(r, c).val[2] = 0;



			}*/


			/*for (int r = 0; r < 118; r++)
			for (int c = 0; c < src.cols; c++)
			{

			if (r<39)
			{
			src.at<cv::Vec3b>(r, c).val[0] = 0;
			src.at<cv::Vec3b>(r, c).val[1] = 255;
			src.at<cv::Vec3b>(r, c).val[2] = 255;
			}
			else if (r<39 * 2)
			{
			src.at<cv::Vec3b>(r, c).val[0] = 255;
			src.at<cv::Vec3b>(r, c).val[1] = 0;
			src.at<cv::Vec3b>(r, c).val[2] = 255;
			}
			else if (r < 39 * 3)
			{
			src.at<cv::Vec3b>(r, c).val[0] = 255;
			src.at<cv::Vec3b>(r, c).val[1] = 255;
			src.at<cv::Vec3b>(r, c).val[2] = 0;
			}


			}*/
			//間格犧牲條
			/*for (int r = 78; r < 273; r++)
			for (int c = 0; c < src.cols; c++)
			{

			if (r<39 * 3 && r>39 * 2)
			{
			src.at<cv::Vec3b>(r, c).val[0] = 255;
			src.at<cv::Vec3b>(r, c).val[1] = 255;
			src.at<cv::Vec3b>(r, c).val[2] = 0;
			}
			else if (r<39 * 5 && r>39 * 4)
			{
			src.at<cv::Vec3b>(r, c).val[0] = 255;
			src.at<cv::Vec3b>(r, c).val[1] = 0;
			src.at<cv::Vec3b>(r, c).val[2] = 255;
			}
			else if (r<39 * 7 && r>39 * 6)
			{
			src.at<cv::Vec3b>(r, c).val[0] = 0;
			src.at<cv::Vec3b>(r, c).val[1] = 255;
			src.at<cv::Vec3b>(r, c).val[2] = 255;
			}


			}*/

			for (int r = 0; r < temptemp * 3; r++)
				for (int c = 0; c < src.cols; c++)
				{

					//if (r<temptemp)
					//{
					//	src.at<cv::Vec3b>(r, c).val[0] = 255;
					//	src.at<cv::Vec3b>(r, c).val[1] = 255;
					//	src.at<cv::Vec3b>(r, c).val[2] = 0;
					//}
					//else if (r<temptemp * 2 && r>temptemp)
					//{
					//	src.at<cv::Vec3b>(r, c).val[0] = 255;
					//	src.at<cv::Vec3b>(r, c).val[1] = 0;
					//	src.at<cv::Vec3b>(r, c).val[2] = 255;
					//}
					//else if (r<temptemp * 3 && r>temptemp * 2)
					//{
					//	src.at<cv::Vec3b>(r, c).val[0] = 0;
					//	src.at<cv::Vec3b>(r, c).val[1] = 255;
					//	src.at<cv::Vec3b>(r, c).val[2] = 255;
					//}

					src.at<cv::Vec3b>(r, c).val[0] = 0;
					src.at<cv::Vec3b>(r, c).val[1] = 0;
					src.at<cv::Vec3b>(r, c).val[2] = 0;


				}





			break;
		}
		case CV_8UC1:
		{
			for (int r = temptemp * 0; r < temptemp * 3; r++)
				for (int c = 0; c < src.cols; c++)
				{

					//if (r < 39 * 3)//***20160422
					/*if (r < 39)
					{*/
					src.at<unsigned char>(r, c) = 0;

					//}

				}
			break;
		}

		}


	}

	static cv::Mat combinePoint05mmToPoint01cm(cv::Mat *point01cm, cv::Mat &point05mm, cv::Mat &point05mmBalckMask)
	{
		cv::cvtColor(point05mmBalckMask, point05mmBalckMask, CV_BGR2GRAY);
		for (int r = 0; r < point01cm->rows; r++)
			for (int c = 0; c < point01cm->cols; c++)
			{
				if (point05mmBalckMask.at<uchar>(r, c) == 0)
				{
					point01cm->at<cv::Vec3b>(r, c).val[0] = point05mm.at<cv::Vec3b>(r, c).val[0];
					point01cm->at<cv::Vec3b>(r, c).val[1] = point05mm.at<cv::Vec3b>(r, c).val[1];
					point01cm->at<cv::Vec3b>(r, c).val[2] = point05mm.at<cv::Vec3b>(r, c).val[2];
				}
			}
		return *point01cm;
	}
	static cv::Mat combineOutlineWithWidthtoImage(cv::Mat *point01cm, cv::Mat &outline, cv::Mat &outlineMask)
	{
		cv::cvtColor(outlineMask, outlineMask, CV_BGR2GRAY);
		for (int r = 0; r < point01cm->rows; r++)
			for (int c = 0; c < point01cm->cols; c++)
			{
				if (outlineMask.at<uchar>(r, c) == 0)
				{
					point01cm->at<cv::Vec3b>(r, c).val[0] = outline.at<cv::Vec3b>(r, c).val[0];
					point01cm->at<cv::Vec3b>(r, c).val[1] = outline.at<cv::Vec3b>(r, c).val[1];
					point01cm->at<cv::Vec3b>(r, c).val[2] = outline.at<cv::Vec3b>(r, c).val[2];
				}
			}
		return *point01cm;
	}

	static cv::Mat upDownCombine(cv::Mat& upBlackMask, cv::Mat& upcolorMask, cv::Mat& downBlackMask, cv::Mat& downcolorMask)
	{
		cv::cvtColor(upBlackMask, upBlackMask, CV_BGR2GRAY);
		cv::cvtColor(downBlackMask, downBlackMask, CV_BGR2GRAY);
		cv::Mat out(upcolorMask.rows, upcolorMask.cols, CV_8UC3);


		for (int c = 0; c < upcolorMask.cols; c++)
		{
			unsigned char* downcolor = (unsigned char*)downcolorMask.col(c).data;//Mat::data, pointer to data，data的pointer
			unsigned char* downblack = (unsigned char*)downBlackMask.col(c).data;//Mat::data, pointer to data，data的pointer
			unsigned char* upcolor = (unsigned char*)upcolorMask.col(c).data;
			unsigned char* upblack = (unsigned char*)upBlackMask.col(c).data;
			unsigned char* outc = (unsigned char*)out.col(c).data;


			int colorcols = (int)(upcolorMask.step / upcolorMask.elemSize1());//
			int blackcols = (int)(downBlackMask.step / downBlackMask.elemSize1());//
			for (int r = 0; r < upcolorMask.rows; r++)
			{
				if (upblack[r*blackcols] < 20)
				{
					//out.at<cv::Vec3b>(r, c) = downcolorMask.at<cv::Vec3b>(r, c);
					//out.at<cv::Vec3b>(r, c).val[1] = downcolorMask.at<cv::Vec3b>(r, c).val[1];
					//out.at<cv::Vec3b>(r, c).val[2] = downcolorMask.at<cv::Vec3b>(r, c).val[2];
					outc[r*colorcols + 0] = downcolor[r*colorcols + 0];
					outc[r*colorcols + 1] = downcolor[r*colorcols + 1];
					outc[r*colorcols + 2] = downcolor[r*colorcols + 2];
				}
				else if (downblack[r*blackcols])
				{
					//out.at<cv::Vec3b>(r, c) = upcolorMask.at<cv::Vec3b>(r, c);
					/*out.at<cv::Vec3b>(r, c).val[1] = upcolorMask.at<cv::Vec3b>(r, c).val[1];
					out.at<cv::Vec3b>(r, c).val[2] = upcolorMask.at<cv::Vec3b>(r, c).val[2];*/
					outc[r*colorcols + 0] = upcolor[r*colorcols + 0];
					outc[r*colorcols + 1] = upcolor[r*colorcols + 1];
					outc[r*colorcols + 2] = upcolor[r*colorcols + 2];
				}

			}
		}
		return out;
	}

	static cv::Mat outlinePoint01CMCombine(cv::Mat& outline, cv::Mat& outlineMask, cv::Mat& updown, cv::Mat &binderMask)
	{
		cv::cvtColor(outlineMask, outlineMask, CV_BGR2GRAY);

		for (int r = 0; r < outline.rows; r++)
			for (int c = 0; c < outline.cols; c++)
			{
				//if (binderMask.at<uchar>(r, c) == 0 && outline.at<cv::Vec3b>(r, c).val[0]!=255){
				////	qDebug() << outlineMask.at<uchar>(r, c);
				////	qDebug() << binderMask.at<uchar>(r, c);
				//	qDebug() <<" outline.at<cv::Vec3b>(r, c).val[0]"<< outline.at<cv::Vec3b>(r, c).val[0];//
				//}
				if (outlineMask.at<uchar>(r, c) == 0 && binderMask.at<uchar>(r, c) == 0)
				{
					updown.at<cv::Vec3b>(r, c).val[0] = outline.at<cv::Vec3b>(r, c).val[0];
					updown.at<cv::Vec3b>(r, c).val[1] = outline.at<cv::Vec3b>(r, c).val[1];
					updown.at<cv::Vec3b>(r, c).val[2] = outline.at<cv::Vec3b>(r, c).val[2];
				}


			}
		return updown;
	}
	static cv::Mat outlineAndBinderMask(cv::Mat& outline, cv::Mat& outlineMask, cv::Mat& binderMask, cv::Mat &downColorFirst, cv::Mat &downBlackMaskFirst, cv::Mat &downColorSecond, cv::Mat &downBlackMaskSecond, cv::Mat &reverse_binder_outline)
	{
		cv::cvtColor(outlineMask, outlineMask, CV_BGR2GRAY);
		cv::cvtColor(downBlackMaskFirst, downBlackMaskFirst, CV_BGR2GRAY);
		cv::cvtColor(downBlackMaskSecond, downBlackMaskSecond, CV_BGR2GRAY);
		cv::cvtColor(reverse_binder_outline, reverse_binder_outline, CV_BGR2GRAY);

		/*for (int r = 0; r < outline.rows; r++)
		for (int c = 0; c < outline.cols; c++)
		{
		if (binderMask.at<uchar>(r, c) == 255)
		{
		outline.at<cv::Vec3b>(r, c).val[0] = 255;
		outline.at<cv::Vec3b>(r, c).val[1] = 255;
		outline.at<cv::Vec3b>(r, c).val[2] = 255;
		outlineMask.at<uchar>(r, c) = 255;
		}
		}


		for (int r = 0; r < outline.rows; r++)
		for (int c = 0; c < outline.cols; c++)
		{
		if (downBlackMaskFirst.at<uchar>(r, c) == 255 && downBlackMaskSecond.at<uchar>(r, c) == 0)
		{
		downColorFirst.at<cv::Vec3b>(r, c).val[0] = downColorSecond.at<cv::Vec3b>(r, c).val[0];
		downColorFirst.at<cv::Vec3b>(r, c).val[1] = downColorSecond.at<cv::Vec3b>(r, c).val[1];
		downColorFirst.at<cv::Vec3b>(r, c).val[2] = downColorSecond.at<cv::Vec3b>(r, c).val[2];
		downBlackMaskFirst.at<uchar>(r, c) = 0;
		}
		}




		for (int r = 0; r < outline.rows; r++)
		for (int c = 0; c < outline.cols; c++)
		{
		if (downBlackMaskFirst.at<uchar>(r, c) == 255)
		{
		downColorFirst.at<cv::Vec3b>(r, c).val[0] = outline.at<cv::Vec3b>(r, c).val[0];
		downColorFirst.at<cv::Vec3b>(r, c).val[1] = outline.at<cv::Vec3b>(r, c).val[1];
		downColorFirst.at<cv::Vec3b>(r, c).val[2] = outline.at<cv::Vec3b>(r, c).val[2];

		}
		}*/


		////////////////////////////////////////////////////////////////////////////////////////////////////////
		/*for (int r = 0; r < outline.rows; r++)
		for (int c = 0; c < outline.cols; c++)
		{
		if (binderMask.at<uchar>(r, c) == 255)
		{
		outline.at<cv::Vec3b>(r, c).val[0] = 255;
		outline.at<cv::Vec3b>(r, c).val[1] = 255;
		outline.at<cv::Vec3b>(r, c).val[2] = 255;
		outlineMask.at<uchar>(r, c) = 255;
		}
		}


		for (int r = 0; r < outline.rows; r++)
		for (int c = 0; c < outline.cols; c++)
		{
		if (downBlackMaskFirst.at<uchar>(r, c) == 255 && outlineMask.at<uchar>(r, c) == 0)
		{
		downColorFirst.at<cv::Vec3b>(r, c).val[0] = outline.at<cv::Vec3b>(r, c).val[0];
		downColorFirst.at<cv::Vec3b>(r, c).val[1] = outline.at<cv::Vec3b>(r, c).val[1];
		downColorFirst.at<cv::Vec3b>(r, c).val[2] = outline.at<cv::Vec3b>(r, c).val[2];
		downBlackMaskFirst.at<uchar>(r, c) = 0;

		}
		}
		reverse_binder_outline = downBlackMaskFirst.clone();

		for (int r = 0; r < outline.rows; r++)
		for (int c = 0; c < outline.cols; c++)
		{
		if (downBlackMaskFirst.at<uchar>(r, c) == 255 && downBlackMaskSecond.at<uchar>(r, c) == 0)
		{
		downColorFirst.at<cv::Vec3b>(r, c).val[0] = downColorSecond.at<cv::Vec3b>(r, c).val[0];
		downColorFirst.at<cv::Vec3b>(r, c).val[1] = downColorSecond.at<cv::Vec3b>(r, c).val[1];
		downColorFirst.at<cv::Vec3b>(r, c).val[2] = downColorSecond.at<cv::Vec3b>(r, c).val[2];
		reverse_binder_outline.at<uchar>(r, c) = 0;
		}
		}*/
		/*
			===========================================================================================
			*/
		for (int r = 0; r < outline.rows; r++)
			for (int c = 0; c < outline.cols; c++)
			{
				if (binderMask.at<uchar>(r, c) == 255)
				{
					outline.at<cv::Vec3b>(r, c).val[0] = 255;
					outline.at<cv::Vec3b>(r, c).val[1] = 255;
					outline.at<cv::Vec3b>(r, c).val[2] = 255;
					outlineMask.at<uchar>(r, c) = 255;
				}
			}


		for (int r = 0; r < outline.rows; r++)
			for (int c = 0; c < outline.cols; c++)
			{
				if (downBlackMaskFirst.at<uchar>(r, c) == 255 && outlineMask.at<uchar>(r, c) == 0)
				{
					downColorFirst.at<cv::Vec3b>(r, c).val[0] = outline.at<cv::Vec3b>(r, c).val[0];
					downColorFirst.at<cv::Vec3b>(r, c).val[1] = outline.at<cv::Vec3b>(r, c).val[1];
					downColorFirst.at<cv::Vec3b>(r, c).val[2] = outline.at<cv::Vec3b>(r, c).val[2];
					downBlackMaskFirst.at<uchar>(r, c) = 0;

				}
			}
		reverse_binder_outline = downBlackMaskFirst.clone();

		for (int r = 0; r < outline.rows; r++)
			for (int c = 0; c < outline.cols; c++)
			{
				if (downBlackMaskFirst.at<uchar>(r, c) == 255 && downBlackMaskSecond.at<uchar>(r, c) == 0)
				{
					downColorFirst.at<cv::Vec3b>(r, c).val[0] = downColorSecond.at<cv::Vec3b>(r, c).val[0];
					downColorFirst.at<cv::Vec3b>(r, c).val[1] = downColorSecond.at<cv::Vec3b>(r, c).val[1];
					downColorFirst.at<cv::Vec3b>(r, c).val[2] = downColorSecond.at<cv::Vec3b>(r, c).val[2];
					reverse_binder_outline.at<uchar>(r, c) = 0;
				}
			}

		for (int r = 0; r < outline.rows; r++)
			for (int c = 0; c < outline.cols; c++)
			{
				if (binderMask.at<uchar>(r, c) == 255)
				{
					downColorFirst.at<cv::Vec3b>(r, c).val[0] = 255;
					downColorFirst.at<cv::Vec3b>(r, c).val[1] = 255;
					downColorFirst.at<cv::Vec3b>(r, c).val[2] = 255;
					reverse_binder_outline.at<uchar>(r, c) = 255;
				}
			}

		return downColorFirst;
	}
	static cv::Mat GenerateBinderMask(cv::Mat& outlineMask, cv::Mat& binderMask, cv::Mat &downBlackMaskFirst, cv::Mat &downBlackMaskSecond, cv::Mat &reverse_binder_outline)
	{
		cv::cvtColor(outlineMask, outlineMask, CV_BGR2GRAY);
		cv::cvtColor(downBlackMaskFirst, downBlackMaskFirst, CV_BGR2GRAY);
		cv::cvtColor(downBlackMaskSecond, downBlackMaskSecond, CV_BGR2GRAY);
		cv::cvtColor(reverse_binder_outline, reverse_binder_outline, CV_BGR2GRAY);


		////////////////////////////////////////////////////////////////////////////////////////////////////////
		/*
		===========================================================================================
		*/
		for (int r = 0; r < outlineMask.rows; r++)
			for (int c = 0; c < outlineMask.cols; c++)
			{
				if (binderMask.at<uchar>(r, c) == 255)
				{
					outlineMask.at<uchar>(r, c) = 255;
				}
			}


		for (int r = 0; r < outlineMask.rows; r++)
			for (int c = 0; c < outlineMask.cols; c++)
			{
				if (downBlackMaskFirst.at<uchar>(r, c) == 255 && outlineMask.at<uchar>(r, c) == 0)
				{
					downBlackMaskFirst.at<uchar>(r, c) = 0;
				}
			}
		reverse_binder_outline = downBlackMaskFirst.clone();

		for (int r = 0; r < outlineMask.rows; r++)
			for (int c = 0; c < outlineMask.cols; c++)
			{
				if (downBlackMaskFirst.at<uchar>(r, c) == 255 && downBlackMaskSecond.at<uchar>(r, c) == 0)
				{
					reverse_binder_outline.at<uchar>(r, c) = 0;
				}
			}

		for (int r = 0; r < outlineMask.rows; r++)
			for (int c = 0; c < outlineMask.cols; c++)
			{
				if (binderMask.at<uchar>(r, c) == 255)
				{
					reverse_binder_outline.at<uchar>(r, c) = 255;
				}
			}

		return reverse_binder_outline;
	}
	static cv::Mat outlinePoint01CMCombineVersion2(cv::Mat& outline, cv::Mat& outlineMask, cv::Mat& updown, cv::Mat &binderMask, cv::Mat &updownmask)
	{
		cv::cvtColor(outlineMask, outlineMask, CV_BGR2GRAY);
		cv::cvtColor(updownmask, updownmask, CV_BGR2GRAY);

		for (int r = 0; r < outline.rows; r++)
			for (int c = 0; c < outline.cols; c++)
			{
				if (outlineMask.at<uchar>(r, c) == 0 && binderMask.at<uchar>(r, c) == 0 && updownmask.at<uchar>(r, c) == 255)
				{
					updown.at<cv::Vec3b>(r, c).val[0] = outline.at<cv::Vec3b>(r, c).val[0];
					updown.at<cv::Vec3b>(r, c).val[1] = outline.at<cv::Vec3b>(r, c).val[1];
					updown.at<cv::Vec3b>(r, c).val[2] = outline.at<cv::Vec3b>(r, c).val[2];
				}


			}
		return updown;
	}

	static cv::Scalar meanCMY(cv::Mat &img)
	{		// Get rgb
		//std::vector<cv::Mat> rgb;
		cv::Vec3b temp(255, 255, 255);
		int test0 = 0;
		int test1 = 0;
		int test2 = 0;
		for (int i = 0; i < img.rows; i++) {
			for (int j = 0; j < img.cols; j++) {
				img.at<cv::Vec3b>(i, j) = temp - img.at<cv::Vec3b>(i, j);
				/*if (img.at<cv::Vec3b>(i, j)[0]>0)
					test0 = img.at<cv::Vec3b>(i, j)[0];
					if (img.at<cv::Vec3b>(i, j)[1]>0)
					test1 = img.at<cv::Vec3b>(i, j)[1];
					if (img.at<cv::Vec3b>(i, j)[2] > 0)
					test2 = img.at<cv::Vec3b>(i, j)[2];*/

			}
		}
		cv::Scalar out = cv::mean(img);
		return out;

	}
	static double countBinderArea(cv::Mat &img)
	{
		cv::Mat temp;
		img.copyTo(temp);
		cv::bitwise_not(temp, temp);
		return cv::countNonZero(temp);
	}


	static void cutImage(cv::Mat &M1, cv::Mat &M2)//white_kick
	{
		double sum = 0;

		int recordLastLine = 0;
		//if (M1.channels() == 1)
		//{
		//	
		//	for (int i = 0; i < M1.rows; i++)
		//	{
		//		const uchar* Mi = M1.ptr<uchar>(i);
		//		for (int j = 0; j < M1.cols; j++)
		//		{
		//			if (Mi[j] == 0 && j>recordLastLine)
		//			{
		//				recordLastLine = j;
		//				//break;
		//			}
		//		}
		//	}
		//	if (recordLastLine < 944)recordLastLine = 944;
		//	else if (recordLastLine < (M1.cols - 10))recordLastLine += 10;
		//	//cout << "recordLastLine" << recordLastLine << endl;

		//	//cv::Mat ROI = M1(cv::Rect(0, 0, M1.cols, recordLastLine));
		//	M1 = M1(cv::Rect(0, 0, recordLastLine, M1.rows));
		//	M2 = M2(cv::Rect(0, 0, recordLastLine, M2.rows));
		//}
		if (M1.channels() == 1)
		{

			for (int r = 0; r < M1.rows; r++)
			{
				const uchar* Mi = M1.ptr<uchar>(r);
				for (int c = 0; c < M1.cols; c++)
				{
					//if (c >M1.cols)c = M1.cols;
					if (Mi[c] == 0 && c>recordLastLine)
						//if (M1.at<uchar>(r, c) == 0 && c>recordLastLine)
					{
						recordLastLine = c;
						//break;
					}
				}
			}
			if (recordLastLine < 944)recordLastLine = 944;
			else if (recordLastLine < (M1.cols - 10))recordLastLine += 10;
			//cout << "recordLastLine" << recordLastLine << endl;

			//cv::Mat ROI = M1(cv::Rect(0, 0, M1.cols, recordLastLine));
			M1 = M1(cv::Rect(0, 0, recordLastLine, M1.rows));
			M2 = M2(cv::Rect(0, 0, recordLastLine, M2.rows));
		}
	}

	static void rotateImage(cv::Mat &M1, cv::Mat &dstM1)
	{
		switch (M1.type())
		{
		case CV_8UC3:
		{
			for (int r = 0; r < M1.rows; r++)
				for (int c = 0; c < M1.cols; c++)
				{
					//std::cout << M1.cols - c << std::endl;
					dstM1.at<cv::Vec3b>(c, M1.rows - r - 1) = M1.at<cv::Vec3b>(r, c);
				}
			break;
		}
		case CV_8UC1:
		{
			//cv::cvtColor(dstM1, dstM1, CV_BGR2GRAY);
			for (int r = 0; r < M1.rows; r++)
				for (int c = 0; c < M1.cols; c++)
				{
					//std::cout << M1.cols - c << std::endl;
					dstM1.at<uchar>(c, M1.rows - r - 1) = M1.at<uchar>(r, c);
				}
			break;

		}
		}

	}

	static cv::Mat rotateImageV2(cv::Mat &M1)
	{
		cv::Mat temp(M1.cols, M1.rows, M1.type());
		switch (M1.type())
		{
		case CV_8UC3:
		{
			for (int r = 0; r < M1.rows; r++)
				for (int c = 0; c < M1.cols; c++)
				{
					//std::cout << M1.cols - c << std::endl;
					temp.at<cv::Vec3b>(c, M1.rows - r - 1) = M1.at<cv::Vec3b>(r, c);
				}
			return temp;
			break;
		}
		case CV_8UC1:
		{
			//cv::cvtColor(dstM1, dstM1, CV_BGR2GRAY);
			for (int r = 0; r < M1.rows; r++)
				for (int c = 0; c < M1.cols; c++)
				{
					//std::cout << M1.cols - c << std::endl;
					temp.at<uchar>(c, M1.rows - r - 1) = M1.at<uchar>(r, c);
				}
			return temp;
			break;

		}
		}
		return temp;
	}
	static void mirrorImage(cv::Mat &M1, cv::Mat &dstM1)
	{


		switch (M1.type())
		{
		case CV_8UC3:
		{
			for (int r = 0; r < M1.rows; r++)
				for (int c = 0; c < M1.cols; c++)
				{
					//dstM1.at<cv::Vec3b>(r, M1.cols - c - 1) = M1.at<cv::Vec3b>(r, c);
					//if (M1.at<cv::Vec3b>(r, c) != cv::Vec3b(0, 0, 0))
					//dstM1.at<cv::Vec3b>(M1.rows - r - 1, c) = cv::Vec3b(255, 0, 0);
					dstM1.at<cv::Vec3b>(M1.rows - r - 1, c) = M1.at<cv::Vec3b>(r, c);

				}
			break;
		}
		case CV_8UC1:
		{
			for (int r = 0; r < M1.rows; r++)
				for (int c = 0; c < M1.cols; c++)
				{
					//dstM1.at<uchar>(r, M1.cols - c - 1) = M1.at<uchar>(r, c);
					//dstM1.at<uchar>(M1.rows-r-1, c) = M1.at<uchar>(r, c);
					if (M1.at<uchar>(r, c) == 0)
						//dstM1.at<uchar>(M1.rows - r - 1, c) = 128;//M1.at<uchar>(r, c);
						dstM1.at<uchar>(M1.rows - r - 1, c) = 0;//M1.at<uchar>(r, c);
				}
			break;

		}
		}

	}


	static void brighterImage(cv::Mat &M1)
	{
		float alpha = 1.0;
		float beta = 65;
		/*for (int y = 0; y < M1.rows; y++)
		{
		for (int x = 0; x < M1.cols; x++)
		{
		for (int c = 0; c < 3; c++)
		{
		M1.at<cv::Vec3b>(y, x)[c] =
		cv::saturate_cast<uchar>(alpha*(M1.at<cv::Vec3b>(y, x)[c]) + beta);
		}
		}
		}*/
		M1.convertTo(M1, -1, alpha, beta);
	}
	static cv::Mat yCbCrBrighterTest(cv::Mat &M1)
	{
		//YUV
		cv::Mat new_image = cv::Mat::zeros(M1.size(), M1.type());
		cvtColor(M1, new_image, cv::COLOR_BGR2YCrCb);
		std::vector<cv::Mat> yuvV;
		cv::split(new_image, yuvV);
		//for (int y = 0; y < new_image.rows; y++)
		//{
		//	for (int x = 0; x < new_image.cols; x++)
		//	{
		//		int temp = yuvV[0].at<uchar>(y, x);
		//		if (yuvV[0].at<uchar>(y, x)>0 && yuvV[0].at<uchar>(y, x) <= 55)
		//		{
		//			yuvV[0].at<uchar>(y, x) += int((temp)* 100 / 56.);
		//		}
		//		else if (yuvV[0].at<uchar>(y, x)>55 && yuvV[0].at<uchar>(y, x) <= 133)
		//		{
		//			yuvV[0].at<uchar>(y, x) += int(100 + (temp - 56) * 22 / 77.);//71+ [(temp-56)/(133-56)]*(122-71)
		//		}
		//		else if (yuvV[0].at<uchar>(y, x)>133 && yuvV[0].at<uchar>(y, x) <= 255)
		//		{
		//			yuvV[0].at<uchar>(y, x) += int(122 + (temp - 133) * (-122) / 122.);//102 + [(temp-133)/(255-133)]*(-102)
		//		}
		//	}
		//}

		//int yuvcols = (int)(yuvV[0].cols);//
		for (int c = 0; c < new_image.cols; c++)
		{
			int yuvcols = (int)(yuvV[0].step / yuvV[0].elemSize1());//

			for (int r = 0; r < new_image.rows; r++)
			{
				unsigned char* yuvV0data = (unsigned char*)yuvV[0].col(c).data;//Mat::data, pointer to data，data的pointer
				int temp = yuvV0data[r*yuvcols];
				/*if (yuvV0data[r*yuvcols]>0 && yuvV0data[r*yuvcols]<55)
				{
				yuvV0data[r*yuvcols] += int((temp)* 100 / 56.);
				}
				else if (yuvV0data[r*yuvcols]>55 && yuvV0data[r*yuvcols] <= 133)
				{
				yuvV0data[r*yuvcols] += int(100 + (temp - 56) * 22 / 77.);
				}
				else if (yuvV0data[r*yuvcols] > 133 && yuvV0data[r*yuvcols] <= 255)
				{
				yuvV0data[r*yuvcols] += int(122 + (temp - 133) * (-122) / 122.);
				}*/

				if (yuvV0data[r*yuvcols] > 0 && yuvV0data[r*yuvcols] <= 55)
				{
					yuvV0data[r*yuvcols] = int((temp)* 156 / 55);
				}
				else if (yuvV0data[r*yuvcols] > 55 && yuvV0data[r*yuvcols] <= 133)
				{
					yuvV0data[r*yuvcols] = int(156 + (temp - 56)*(235 - 156) / (133 - 56));
				}
				else if (yuvV0data[r*yuvcols] > 133 && yuvV0data[r*yuvcols] <= 255)
				{
					yuvV0data[r*yuvcols] = int(235 + (temp - 134)*(255 - 235) / (255 - 134));
				}
			}
		}


		cv::Mat dest(M1.size(), M1.type());
		cv::merge(yuvV, dest);
		cvtColor(dest, dest, cv::COLOR_YCrCb2BGR);
		return dest;
	}

	static int Text_OneCM_ToPix(double cm_num, double pix_num)
	{
		int  OneCM_ToPix;
		OneCM_ToPix = (int)(pix_num / cm_num + 0.5);

		return OneCM_ToPix;
	}

	static void TextPageSet(Mat &src, int line_num, int overlap_pix, int ColorBlock_length)
	{
		int line_counter = 0;
		int line_average = 0;
		int overlapping = 25;

		int width = src.size().width;
		int length_ = src.size().height;

		line_average = width / line_num;


		/*switch (src.type())
		{
		case CV_8UC3:
		{
		rectangle(src, Point(0, 236 + ColorBlock_length * 12), Point(width, 236 + ColorBlock_length * 13), Scalar(0, 255, 255), -1);
		rectangle(src, Point(0, 236 + ColorBlock_length * 14), Point(width, 236 + ColorBlock_length * 15), Scalar(255, 0, 255), -1);
		rectangle(src, Point(0, 236 + ColorBlock_length * 16), Point(width, 236 + ColorBlock_length * 17), Scalar(255, 255, 0), -1);



		}break;
		case CV_8UC1:
		{

		}break;
		}*/





		//line
		for (line_counter = 1; line_counter < line_num; line_counter++)
		{
			switch (src.type())
			{
			case CV_8UC3:
			{
				line(src, Point(line_average*line_counter, 0), Point(line_average*line_counter, 236), Scalar(0, 0, 0), 2);
				line(src, Point(line_average*line_counter, 236 + ColorBlock_length * 10), Point(line_average*line_counter, 236 + ColorBlock_length * 11), Scalar(0, 0, 0), 2);



			}break;
			case CV_8UC1:
			{

			}break;
			}
		}
		std::vector<std::string> str_num;
		//number
		for (line_counter = 0; line_counter < line_num; line_counter++)
		{
			str_num.push_back(std::to_string(line_counter));
			switch (src.type())
			{
			case CV_8UC3:
			{
				putText(src, std::to_string(line_counter + 1), Point((int)(line_average*(line_counter + 0.29)), 180), 0, 3, Scalar(0, 0, 0), 6);
			}break;
			case CV_8UC1:
			{

			}break;
			}
		}

		//color blocks
		for (line_counter = 0; line_counter < line_num; line_counter++)
		{
			if (line_counter % 2 == 0)
			{
				switch (src.type())
				{
				case CV_8UC3:
				{
					rectangle(src, Point(line_average*line_counter - overlap_pix, 236), Point(line_average*(line_counter + 1) + overlap_pix, 236 + ColorBlock_length + overlapping), Scalar(0, 255, 255), -1);

					rectangle(src, Point(line_average*line_counter - overlap_pix, 236 + ColorBlock_length * 4), Point(line_average*(line_counter + 1) + overlap_pix, 236 + ColorBlock_length * 5 + overlapping), Scalar(255, 0, 255), -1);

					rectangle(src, Point(line_average*line_counter - overlap_pix, 236 + ColorBlock_length * 8), Point(line_average*(line_counter + 1) + overlap_pix, 236 + ColorBlock_length * 9 + overlapping), Scalar(255, 255, 0), -1);


				}break;
				case CV_8UC1:
				{
					rectangle(src, Point(line_average*line_counter - overlap_pix, 236 + ColorBlock_length * 2), Point(line_average*(line_counter + 1) + overlap_pix, 236 + ColorBlock_length * 3 + overlapping), Scalar(0), -1);
					rectangle(src, Point(line_average*line_counter - overlap_pix, 236 + ColorBlock_length * 6), Point(line_average*(line_counter + 1) + overlap_pix, 236 + ColorBlock_length * 7 + overlapping), Scalar(0), -1);
				}break;
				}

			}
			else
			{
				switch (src.type())
				{
				case CV_8UC3:
				{
					rectangle(src, Point(line_average*line_counter - overlap_pix, 236 + ColorBlock_length - overlapping), Point(line_average*(line_counter + 1) + overlap_pix, 236 + ColorBlock_length * 2), Scalar(0, 255, 255), -1);
					//rectangle(src, Point(line_average*line_counter - overlap_pix, 236 + ColorBlock_length*2), Point(line_average*(line_counter + 1) + overlap_pix, 236 + ColorBlock_length*3), Scalar(255, 255, 255), -1);
					rectangle(src, Point(line_average*line_counter - overlap_pix, 236 + ColorBlock_length * 5 - overlapping), Point(line_average*(line_counter + 1) + overlap_pix, 236 + ColorBlock_length * 6), Scalar(255, 0, 255), -1);
					//	rectangle(src, Point(line_average*line_counter - overlap_pix, 236 + ColorBlock_length*4), Point(line_average*(line_counter + 1) + overlap_pix, 236 + ColorBlock_length*5), Scalar(255, 255, 255), -1);
					rectangle(src, Point(line_average*line_counter - overlap_pix, 236 + ColorBlock_length * 9 - overlapping), Point(line_average*(line_counter + 1) + overlap_pix, 236 + ColorBlock_length * 10), Scalar(255, 255, 0), -1);
				}break;
				case CV_8UC1:
				{
					rectangle(src, Point(line_average*line_counter - overlap_pix, 236 + ColorBlock_length * 3 - overlapping), Point(line_average*(line_counter + 1) + overlap_pix, 236 + ColorBlock_length * 4), Scalar(0), -1);
					rectangle(src, Point(line_average*line_counter - overlap_pix, 236 + ColorBlock_length * 7 - overlapping), Point(line_average*(line_counter + 1) + overlap_pix, 236 + ColorBlock_length * 8), Scalar(0), -1);
				}break;
				}
			}
		}

		switch (src.type())
		{
		case CV_8UC3:
		{
			rectangle(src, Point(0, 236 + ColorBlock_length * 11), Point(length_, 236 + (int)(ColorBlock_length * 11.5)), Scalar(0, 0, 0), -1);
		}break;
		case CV_8UC1:
		{
		}break;
		}


	}
	static void printBedSize(Mat &src, int startPrintPixel, int secondPrintPixel)
	{
		int width = src.size().width;
		int lengthh = src.size().height;
		int widthHalf = width / 2.;
		int lengthHalf = lengthh / 2.;
		int dpiCM = 59 * 2;
		int dpiMM = 12;
		switch (src.type())
		{
		case CV_8UC3:
		{
			rectangle(src, Point(3, startPrintPixel), Point(width - 3, lengthh - 73), Scalar(0, 0, 0), 5);
			cv::line(src, Point(widthHalf - dpiCM, startPrintPixel - dpiMM), Point(widthHalf + dpiCM, startPrintPixel - dpiMM), Scalar(0, 0, 0), 1);
			cv::line(src, Point(widthHalf - dpiCM, startPrintPixel - dpiMM * 2), Point(widthHalf + dpiCM, startPrintPixel - dpiMM * 2), Scalar(0, 0, 0), 1);
			cv::line(src, Point(widthHalf - dpiCM, startPrintPixel - dpiMM * 3), Point(widthHalf + dpiCM, startPrintPixel - dpiMM * 3), Scalar(0, 0, 0), 1);
			cv::line(src, Point(widthHalf - dpiCM, startPrintPixel - dpiMM * 4), Point(widthHalf + dpiCM, startPrintPixel - dpiMM * 4), Scalar(0, 0, 0), 1);
			cv::line(src, Point(widthHalf - dpiCM, startPrintPixel - dpiMM * 5), Point(widthHalf + dpiCM, startPrintPixel - dpiMM * 5), Scalar(0, 0, 0), 1);
			/*cv::line(src, Point(widthHalf - dpiCM, startPrintPixel - dpiMM * 6), Point(widthHalf + dpiCM, startPrintPixel - dpiMM * 6), Scalar(0, 0, 0), 1);
			cv::line(src, Point(widthHalf - dpiCM, startPrintPixel - dpiMM * 7), Point(widthHalf + dpiCM, startPrintPixel - dpiMM * 7), Scalar(0, 0, 0), 1);*/

			cv::line(src, Point(widthHalf - dpiCM, secondPrintPixel + dpiMM), Point(widthHalf + dpiCM, secondPrintPixel + dpiMM), Scalar(0, 0, 0), 1);
			cv::line(src, Point(widthHalf - dpiCM, secondPrintPixel + dpiMM * 2), Point(widthHalf + dpiCM, secondPrintPixel + dpiMM * 2), Scalar(0, 0, 0), 1);
			cv::line(src, Point(widthHalf - dpiCM, secondPrintPixel + dpiMM * 3), Point(widthHalf + dpiCM, secondPrintPixel + dpiMM * 3), Scalar(0, 0, 0), 1);
			cv::line(src, Point(widthHalf - dpiCM, secondPrintPixel + dpiMM * 4), Point(widthHalf + dpiCM, secondPrintPixel + dpiMM * 4), Scalar(0, 0, 0), 1);
			cv::line(src, Point(widthHalf - dpiCM, secondPrintPixel + dpiMM * 5), Point(widthHalf + dpiCM, secondPrintPixel + dpiMM * 5), Scalar(0, 0, 0), 1);
			//cv::line(src, Point(widthHalf - dpiCM, startPrintPixel - dpiMM * 5), Point(widthHalf + dpiCM, startPrintPixel - dpiMM * 6), Scalar(0, 0, 0), 1);



		}break;
		case CV_8UC1:
		{
			//rectangle(src, Point(line_average*line_counter - overlap_pix, 236 + ColorBlock_length * 2), Point(line_average*(line_counter + 1) + overlap_pix, 236 + ColorBlock_length * 3), Scalar(0), -1);
			//rectangle(src, Point(line_average*line_counter - overlap_pix, 236 + ColorBlock_length * 6), Point(line_average*(line_counter + 1) + overlap_pix, 236 + ColorBlock_length * 7), Scalar(0), -1);
		}break;
		}
	}
	static void createTestPrintPagesK(double widthcm, double plus_printlength, double x_groove_length, float uselessprint, int times, std::vector<cv::Mat>&outlineImg, std::vector<cv::Mat> &capImg, bool plus_spitton)
	{
		int widthpixel = (widthcm / DSP_inchmm) * 300;
		int lengthpixel;// = ((plus_printlength + x_groove_length) / DSP_inchmm) * 300;
		lengthpixel = ((plus_spitton ? (plus_printlength + x_groove_length) : x_groove_length) / DSP_inchmm) * 300;

		int startPixel2 = (plus_printlength / DSP_inchmm) * 300;
		int endPixel2 = ((plus_printlength + x_groove_length) / DSP_inchmm) * 300;

		cv::Mat outColor(lengthpixel + 70, widthpixel, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat outBinder(lengthpixel + 70, widthpixel, CV_8UC1, Scalar(255));
		if (plus_spitton)
		{
			printBedSize(outColor, startPixel2, endPixel2);
			printBedSize(outBinder, startPixel2, endPixel2);
		}
		else
		{
			TextPageSet(outColor, 11, 10, Text_OneCM_ToPix(2.54, 300.0));
			TextPageSet(outBinder, 11, 10, Text_OneCM_ToPix(2.54, 300.0));

		}


		//cutImage(out2, out);
		for (int i = 0; i < 1; i++)
		{
			outlineImg.push_back(outColor);
			capImg.push_back(outBinder);
		}

		/*for (int i = 0; i < 1; i++)
		{
		outlineImg.push_back(outwhite);
		capImg.push_back(out2white);
		}*/


	}
	static void createStupidChipImage(double widthcm, double plus_printlength, double x_groove_length, float uselessprint, int times, std::vector<cv::Mat>&outlineImg, std::vector<cv::Mat> &capImg)
	{
		int widthpixel = (widthcm / DSP_inchmm) * 300;
		int lengthpixel = ((/*plus_printlength + */x_groove_length) / DSP_inchmm) * 300;



		cv::Mat outColor(lengthpixel, widthpixel, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat outBinder(lengthpixel, widthpixel, CV_8UC1, Scalar(255));

		TextPageSet(outColor, 11, 10, Text_OneCM_ToPix(2.54, 300.0));
		TextPageSet(outBinder, 11, 10, Text_OneCM_ToPix(2.54, 300.0));

		//cutImage(out2, out);
		for (int i = 0; i < 1; i++)
		{
			outlineImg.push_back(outColor);
			capImg.push_back(outBinder);
		}

	}


	static double imageComplexity(Mat &src)
	{
		double srcTotalPixel = src.cols * src.rows * 255.;// 3819648825.;
		double fullpages = 3819648825;
		double blackRatio = 0;
		if (src.type() == CV_8UC1)
		{
			double whiteData = cv::sum(src)[0];
			blackRatio = (srcTotalPixel - whiteData) * 100 / fullpages;
		}


		return blackRatio;
	}

	static bool imageOverLapping(cv::Mat meshImg1, cv::Mat meshImg2)
	{
		//cv::Mat src1 = meshImg1.clone();
		cv::Mat src2 = meshImg2.clone();
		//bitwise_not(src1, src1);
		bitwise_not(src2, src2);
		cv::Mat output;
		bitwise_and(meshImg1, src2, output);
		double s = cv::sum(output)[0];
		if (s == 0)return false;
		else return true;
	}
	static std::set<int>  checkImageOverlap(vector<std::map<int, cv::Mat>> &inputFloorMeshesImage)
	{
		std::set<int> meshIntersectId;
		std::map<int, set<int>> meshpair;
		for (int i = 0; i < inputFloorMeshesImage.size(); i++)
		{
			std::map<int, cv::Mat> *tempFloor = &(inputFloorMeshesImage[i]);
			for (std::map<int, cv::Mat>::iterator it = tempFloor->begin(); it != tempFloor->end(); ++it)
			{
				bitwise_not(it->second, it->second);
				std::map<int, cv::Mat>::iterator it2 = it;
				it2++;
				std::set<int> *getset;
				if (meshpair.find(it->first) != meshpair.end())
					getset = &(meshpair.at(it->first));
				else getset = new std::set<int>;
				for (; it2 != tempFloor->end(); ++it2)
				{
					if (getset)
						if (getset->find(it2->first) != getset->end())
							continue;
					bool result = imageOverLapping(it->second, it2->second);
					if (result)
					{
						//meshIntersectId.insert(std::pair<int, int>(it->first, it2->first));
						meshIntersectId.insert(it->first);
						meshIntersectId.insert(it2->first);

						getset->insert(it2->first);
					}
				}
				meshpair.insert(std::pair<int, set<int>>(it->first, *getset));
			}

		}

		return meshIntersectId;
		/*std::multimap<int, int> mymultimap;
		for (int i = 0; i < inputFloorMeshesImage.size(); i++)
		{
		vector<cv::Mat> *tempFloor = &(inputFloorMeshesImage[i]);
		for (int j = 0; j < tempFloor->size(); j++)
		{
		for (int x = j+1; x < tempFloor->size(); x++)
		{
		bool result = imageOverLapping((*tempFloor)[j], (*tempFloor)[x]);
		if (result)
		{
		mymultimap.insert(std::pair<char, int>(j, x));
		}
		}

		}
		}
		int x = 0;*/
	}



	static void findSmallestDistanceXorYAxis(cv::Mat M1, cv::Mat M2, int *offset, unsigned int imageAxis)
	{
		int white = 255;
		std::set<int> recordOffset;
		switch (imageAxis)
		{
		case 0: //x
		{
			for (int r = 0; r < M1.rows; r++)
			{
				const uchar* Mi = M1.ptr<uchar>(r);
				const uchar* M2i = M2.ptr<uchar>(r);
				int recordLastLine = 0;
				int recordMinline = M1.cols;
				bool getM1 = false;
				bool getM2 = false;
				for (int c = 0; c < M1.cols; c++)
				{
					if (Mi[c] == white && c>recordLastLine)
					{
						recordLastLine = c;
						getM1 = true;
					}
					if (M2i[c] == white && c < recordMinline)
					{
						getM2 = true;
						recordMinline = c;
					}
				}
				recordOffset.insert(recordMinline - recordLastLine);

				int temp = recordMinline - recordLastLine;
				if (getM2 && getM1 && temp > 0)
					recordOffset.insert(temp);
			}


		}break;
		case 1:
		{
			for (int c = 0; c < M1.cols; c++)
			{
				int recordLastLine = 0;
				int recordMinline = M1.rows;
				bool getM1 = false;
				bool getM2 = false;
				for (int r = 0; r < M1.rows; r++)
				{
					if (M1.at<uchar>(r, c) == white && r >recordLastLine)
					{
						recordLastLine = r;
						getM1 = true;
					}
					if (M2.at<uchar>(r, c) == white && r < recordMinline)
					{
						getM2 = true;
						recordMinline = r;
					}
				}
				int temp = recordMinline - recordLastLine;
				if (getM2 && getM1 && temp > 0)
					recordOffset.insert(temp);
				//recordOffset.insert(recordMinline - recordLastLine);
			}

		}break;
		case 2://image y for z
		{

			for (int c = 0; c < M1.cols; c++)
			{
				int recordfirstLine = M1.rows;
				int recordMaxline = 0;
				bool getM1 = false;
				bool getM2 = false;
				for (int r = 0; r < M1.rows; r++)
				{
					if (M1.at<uchar>(r, c) == white && r < recordfirstLine)
					{
						recordfirstLine = r;
						getM1 = true;

					}
					if (M2.at<uchar>(r, c) == white && r > recordMaxline)
					{
						recordMaxline = r;
						getM2 = true;
					}
				}
				int temp = -(recordMaxline - recordfirstLine);
				if (getM2 && getM1 && temp > 0)
					recordOffset.insert(temp);

			}


		}
		}


		//*offset = *recordOffset.upper_bound(0);
		if (recordOffset.size() > 0)
			*offset = *recordOffset.begin();
		else
			*offset = M1.rows;

	}


	//static int smallestOffsetfromImage(vector<std::map<int, cv::Mat>> &inputFloorMeshesImage)
	static int smallestOffsetfromM1M2(vector<cv::Mat > m1Image, vector<cv::Mat> m2Image, unsigned int axis, vector<cv::Mat> cbombineImage)
	{
		std::set<int> meshSmallestOffset;
		for (int i = 0; i < m1Image.size(); i++)
		{
			int floorSmallestOffset;
			findSmallestDistanceXorYAxis(m1Image[i], m2Image[i], &floorSmallestOffset, axis);

			meshSmallestOffset.insert(floorSmallestOffset);
		}
		int maxoffset;
		switch (axis)
		{
		case 0:
			maxoffset = m1Image[0].cols;
			break;
		case 1:
		case 2:
			maxoffset = m1Image[0].rows;
			break;
		}

		/*if ((*meshSmallestOffset.begin()) < 0 && (*meshSmallestOffset.upper_bound(-1)) != maxoffset)
			return *meshSmallestOffset.upper_bound(-1);
			else if ((*meshSmallestOffset.begin()) < 0)
			return 0;
			else*/
		if ((*meshSmallestOffset.begin()) != maxoffset)
			return *meshSmallestOffset.begin();
		else return 0;
	}
	//static bool coll
	static int smallestOffsetfromDepthImage(vector<cv::Mat > m1Image, vector<cv::Mat> m2Image, unsigned int axis)
	{
		cv::Mat m1front = m1Image.at(0);//other mesh
		cv::Mat m1back = m1Image.at(1);
		cv::Mat m2front = m2Image.at(0);//selected
		cv::Mat m2back = m2Image.at(1);
		int white = 255;

		int recordminDistance = 255;
		bool hasSpaceforSelectedMesh = true;
		for (int c = 0; c < m1front.cols; c++)
		{

			for (int r = 0; r < m1front.rows; r++)
			{
				int m2thick = m2front.at<uchar>(r, c) - m2back.at<uchar>(r, c);
				int temp = m1back.at<uchar>(r, c) - m2thick;

				if (temp < 0)
				{
					hasSpaceforSelectedMesh = false;
					break;
				}


			}
		}
		if (hasSpaceforSelectedMesh)
		{
			for (int c = 0; c < m1front.cols; c++)
			{

				for (int r = 0; r < m1front.rows; r++)
				{
					int temp = m1back.at<uchar>(r, c) - m2front.at<uchar>(r, c);

					if (temp < recordminDistance)
					{
						recordminDistance = temp;

					}


				}
			}
			return -recordminDistance * 200 / 255.;
		}
		else
		{

			for (int c = 0; c < m1front.cols; c++)
			{

				for (int r = 0; r < m1front.rows; r++)
				{

					int temp = m2back.at<uchar>(r, c) - m1front.at<uchar>(r, c);

					if (/*temp>0 &&*/ temp < recordminDistance)
					{
						recordminDistance = temp;

					}

				}
			}
			return recordminDistance * 200 / 255.;
		}

	}
	//static bool coll
	static bool m1m2CollideFromSixDepthImage(vector<cv::Mat > selectImage, vector<cv::Mat> compareImage)
	{
		//===select
		cv::Mat selectTop = selectImage.at(0);//===z m1
		cv::Mat selectBottom = selectImage.at(1);

		cv::Mat selectRight = selectImage.at(2);//===max x
		cv::Mat selectLeft = selectImage.at(3);//=== min x

		cv::Mat selectMaxY = selectImage.at(4);//===max y
		cv::Mat selectMinY = selectImage.at(5);//===min y

		//===compare
		cv::Mat compareTop = compareImage.at(0);
		cv::Mat compareBottom = compareImage.at(1);

		cv::Mat compareRight = compareImage.at(2);
		cv::Mat compareLeft = compareImage.at(3);

		cv::Mat compareMaxY = compareImage.at(4);
		cv::Mat compareMinY = compareImage.at(5);

		//b.min.Z()<max.Z() && b.max.Z()>min.Z();

		bool selectMinZlowerThanCompareMaxZ = false;
		bool selectMaxZgreaterThanCompareMinZ = false;

		bool selectMinXlowerThanCompareMaxX = false;
		bool selectMaxXgreaterThanCompareMinX = false;

		bool selectMinYlowerThanCompareMaxY = false;
		bool selectMaxYgreaterThanCompareMinY = false;


#pragma region zzz
		for (int c = 0; c < selectTop.cols; c++)
		{
			if (selectMinZlowerThanCompareMaxZ)break;
			for (int r = 0; r < selectTop.rows; r++)
			{
				int temp = selectBottom.at<uchar>(r, c) - compareTop.at<uchar>(r, c);

				if (temp < 0)
				{
					selectMinZlowerThanCompareMaxZ = true;
					break;
				}
			}
		}
		for (int c = 0; c < selectTop.cols; c++)
		{
			if (selectMaxZgreaterThanCompareMinZ)break;
			for (int r = 0; r < selectTop.rows; r++)
			{
				int temp = selectTop.at<uchar>(r, c) - compareBottom.at<uchar>(r, c);

				if (temp > 0)
				{
					selectMaxZgreaterThanCompareMinZ = true;
					break;
				}
			}
		}
#pragma endregion zzz
#pragma region xxx
		//x
		for (int c = 0; c < selectRight.cols; c++)
		{
			if (selectMinXlowerThanCompareMaxX)break;
			for (int r = 0; r < selectRight.rows; r++)
			{
				int temp = selectLeft.at<uchar>(r, c) - compareRight.at<uchar>(r, c);

				if (temp < 0)
				{
					selectMinXlowerThanCompareMaxX = true;
					break;
				}
			}
		}
		for (int c = 0; c < selectRight.cols; c++)
		{
			if (selectMaxXgreaterThanCompareMinX)break;
			for (int r = 0; r < selectRight.rows; r++)
			{
				int temp = selectRight.at<uchar>(r, c) - compareLeft.at<uchar>(r, c);

				if (temp > 0)
				{
					selectMaxXgreaterThanCompareMinX = true;
					break;
				}
			}
		}
#pragma endregion xxx
		//Y
		for (int c = 0; c < selectMaxY.cols; c++)
		{
			if (selectMinYlowerThanCompareMaxY)break;
			for (int r = 0; r < selectMaxY.rows; r++)
			{
				int temp = selectMinY.at<uchar>(r, c) - selectMaxY.at<uchar>(r, c);

				if (temp < 0)
				{
					selectMinYlowerThanCompareMaxY = true;
					break;
				}
			}
		}
		for (int c = 0; c < selectMaxY.cols; c++)
		{
			if (selectMaxYgreaterThanCompareMinY)break;
			for (int r = 0; r < selectMaxY.rows; r++)
			{
				int temp = compareMaxY.at<uchar>(r, c) - compareMinY.at<uchar>(r, c);

				if (temp > 0)
				{
					selectMaxYgreaterThanCompareMinY = true;
					break;
				}
			}
		}


		if (selectMinZlowerThanCompareMaxZ &&  selectMaxZgreaterThanCompareMinZ && selectMinXlowerThanCompareMaxX && selectMaxXgreaterThanCompareMinX &&
			selectMinYlowerThanCompareMaxY && selectMaxYgreaterThanCompareMinY)
			return true;
		else return false;
	}

	static int moveMajorToM2Top(vector<cv::Mat > m1Image, vector<cv::Mat> m2Image, unsigned int axis)
	{
		cv::Mat majorFront = m1Image.at(0);//other mesh
		cv::Mat majorBack = m1Image.at(1);
		cv::Mat m2front = m2Image.at(0);//selected
		cv::Mat m2back = m2Image.at(1);
		int white = 255;

		int recordminDistance = 255;
		bool hasSpaceforSelectedMesh = true;

		for (int c = 0; c < majorFront.cols; c++)
		{
			for (int r = 0; r < majorFront.rows; r++)
			{
				int temp = majorBack.at<uchar>(r, c) - m2front.at<uchar>(r, c);
				if (temp < recordminDistance)
				{
					recordminDistance = temp;
				}

			}
		}
		return recordminDistance * 200 / 255.;

	}

	//static bool searchEmptySpaceInDepthImage(vector<cv::Mat > selectImage, vector<cv::Mat> grooveImage, int &x, int &y, int &z)
	//{
	//	//===select
	//	cv::Mat selectTop = selectImage.at(0);//===z m1
	//	cv::Mat selectBottom = selectImage.at(1);
	//	cv::Mat selectBetween;
	//	double minSelBot, maxSelBot, minbBetween, maxbBetween, minSelTop, maxSelTop;
	//	//top distance to min value in bot
	//	minMaxLoc(selectBottom, &minSelBot, &maxSelBot);
	//
	//	subtract(selectTop, selectBottom, selectBetween);
	//	minMaxLoc(selectBetween, &minbBetween, &maxbBetween);
	//	minMaxLoc(selectTop, &minSelTop, &maxSelTop);
	//
	//
	//	cv::Mat selectTopProject = selectTop - minSelBot;
	//	//cv::imwrite("D://selectTopProject.png", selectTopProject);	
	//	
	//	
	//	//xxx = cv::imwrite("D://selectBetween.png", selectBetween);*/
	//	//
	//	//cv::Mat selectTopProject_32f(selectBottom.size(), CV_32F, Scalar(0));
	//	//selectTopProject.convertTo(selectTopProject_32f, CV_32F);
	//
	//
	//	//cv::imwrite("./flipss.png", selectBetween);
	//	//cv::imwrite("./flipss2.png", selectTopProject);
	//
	//	//===compare
	//	cv::Mat baseTop = grooveImage.at(0);
	//	cv::Mat basebottom = grooveImage.at(1);
	//	//double minBaseTop, maxBaseTop;
	//	//minMaxLoc(baseTop, &minBaseTop, &maxBaseTop);
	//
	//	//
	//	int select_rows = selectBottom.rows;
	//	int select_cols = selectBottom.cols;
	//
	//	/*int minSelBot, maxSelBot;
	//	minMaxLoc(selectBottom, &minSelBot, &maxSelBot);
	//	int minBaseTop, maxBaseTop;
	//	minMaxLoc(baseTop, &minBaseTop, &maxBaseTop);
	//	int minBaseTop;*/
	//
	//	int base_c = 500;
	//	int base_r = 500;
	//
	//	int second_base_c = 500;
	//	int second_base_r = 500;
	//
	//	int maxZOffset = 0;
	//	int second_maxZOffset = 0;
	//	bool breakBool = true;
	//	bool getFirstSolution = false;
	//
	//	for (int c = 0; c < baseTop.cols && !getFirstSolution; c++)
	//	{
	//		if (c + select_cols+1 <= baseTop.cols)
	//		for (int r = 0; r < baseTop.rows && !getFirstSolution; r++)
	//		{
	//			//for every pixel in baseTop
	//			if (r + select_rows+1 <= baseTop.rows)
	//			{
	//				cv::Mat ROI = baseTop(cv::Rect(c, r, select_cols, select_rows));
	//				cv::Mat baseBottomROI = basebottom(cv::Rect(c, r, select_cols, select_rows));				
	//				breakBool = true;
	//				//bool allwhite = true;
	//				int baseoffsetBottomValue = 255;
	//				int minoffset = 255;
	//				int temp2;
	//				int selectTOPValue;
	//				//int recordcs, recordrs, recordc;
	//				//opencv way to find avalible space 
	//				/*cv::Mat result(selectBottom.size(),CV_32F,Scalar(0));
	//				cv::Mat baseBottomROI_32f(selectBottom.size(), CV_32F, Scalar(0));
	//				baseBottomROI.convertTo(baseBottomROI_32f, CV_32F);
	//				result = baseBottomROI_32f - selectTopProject_32f;
	//				double minoffsetMat;
	//				minMaxLoc(result, &minoffsetMat);
	//				if (minoffsetMat<0)breakBool = false;*/
	//
	//				//find avalible space 
	//				for (int cs = 0; cs < selectBottom.cols && breakBool; cs++)
	//				{
	//					for (int rs = 0; rs < selectBottom.rows && breakBool; rs++)
	//					{
	//						if (selectBetween.at<uchar>(rs, cs) != 0)
	//						{
	//							temp2 = basebottom.at<uchar>(rs + r, cs + c) - (selectTopProject.at<uchar>(rs, cs)+ 3);//
	//							if (temp2 <= 0)						
	//								breakBool = false;
	//
	//							else if (temp2 < minoffset )
	//							{
	//								minoffset = temp2;
	//								baseoffsetBottomValue = basebottom.at<uchar>(rs + r, cs + c);
	//								selectTOPValue = selectTop.at<uchar>(rs, cs);
	//							
	//							}
	//						}
	//					}
	//				}
	//				if (breakBool == true)
	//				{
	//					
	//					if (c <= base_c)
	//					{
	//						base_c = c;
	//						base_r = r;
	//						
	//						//maxZOffset = minSelBot - 1;//backup
	//						//getFirstSolution = true;
	//						
	//						if (baseoffsetBottomValue == 255)
	//						{
	//							maxZOffset = minSelBot - 1;//backup
	//							getFirstSolution = true;
	//						}
	//						else
	//						{							
	//							maxZOffset = selectTOPValue - (baseoffsetBottomValue - 2);
	//							getFirstSolution = true;
	//						}
	//					}
	//				}
	//				else if (!getFirstSolution)
	//				{
	//					cv::Mat offsetMat = selectBottom - ROI;
	//					double minoffsetMat;
	//					minMaxLoc(offsetMat, &minoffsetMat);
	//					if (minoffsetMat > second_maxZOffset)
	//					{						
	//						second_maxZOffset = minoffsetMat;
	//						second_base_c = c;
	//						second_base_r = r;
	//					}
	//					else if (minoffsetMat == second_maxZOffset && second_base_c > c)
	//					{
	//						second_maxZOffset = minoffsetMat;
	//						second_base_c = c;
	//						second_base_r = r;
	//					}
	//				}
	//
	//
	//			}
	//
	//		}
	//	}
	//
	//	if (getFirstSolution)
	//	{
	//		x = base_c;
	//		y = base_r;
	//		z = maxZOffset;
	//	}
	//	else
	//	{
	//		x = second_base_c;
	//		y = second_base_r;
	//		z = second_maxZOffset - 1;
	//	}
	//	return true;
	//}
	static bool searchEmptySpaceInDepthImage(vector<cv::Mat > selectImage, vector<cv::Mat> grooveImage, int &x, int &y, int &z)
	{
		//===select
		cv::Mat selectTop = selectImage.at(0);//===z m1
		cv::Mat selectBottom = selectImage.at(1);
		cv::Mat selectBetween;
		double minSelBot, maxSelBot, minbBetween, maxbBetween, minSelTop, maxSelTop;
		//top distance to min value in bot
		minMaxLoc(selectBottom, &minSelBot, &maxSelBot);

		subtract(selectTop, selectBottom, selectBetween);
		minMaxLoc(selectBetween, &minbBetween, &maxbBetween);
		minMaxLoc(selectTop, &minSelTop, &maxSelTop);


		cv::Mat selectTopProject = selectTop - minSelBot;
		//cv::imwrite("D://selectTopProject.png", selectTopProject);	


		//xxx = cv::imwrite("D://selectBetween.png", selectBetween);*/
		//
		//cv::Mat selectTopProject_32f(selectBottom.size(), CV_32F, Scalar(0));
		//selectTopProject.convertTo(selectTopProject_32f, CV_32F);


		//cv::imwrite("./flipss.png", selectBetween);
		//cv::imwrite("./flipss2.png", selectTopProject);

		//===compare
		cv::Mat baseTop = grooveImage.at(0);
		cv::Mat basebottom = grooveImage.at(1);
		double minBaseTop, maxBaseTop;
		minMaxLoc(baseTop, &minBaseTop, &maxBaseTop);

		//
		int select_rows = selectBottom.rows;
		int select_cols = selectBottom.cols;

		/*int minSelBot, maxSelBot;
		minMaxLoc(selectBottom, &minSelBot, &maxSelBot);
		int minBaseTop, maxBaseTop;
		minMaxLoc(baseTop, &minBaseTop, &maxBaseTop);
		int minBaseTop;*/
		int bigValue = 2000;
		int base_c = bigValue;
		int base_r = bigValue;

		int second_base_c = bigValue;
		int second_base_r = bigValue;

		int maxZOffset = 0;
		int second_maxZOffset = 0;
		bool breakBool = true;
		bool getFirstSolution = false;

		for (int c = 0; c < baseTop.cols && !getFirstSolution; c++)
		{
			if (c + select_cols <= baseTop.cols)
				for (int r = 0; r < baseTop.rows && !getFirstSolution; r++)
				{
					//for every pixel in baseTop
					if (r + select_rows <= baseTop.rows)
					{
						cv::Mat ROI = baseTop(cv::Rect(c, r, select_cols, select_rows));
						//cv::Mat baseBottomROI = basebottom(cv::Rect(c, r, select_cols, select_rows));
						breakBool = true;
						//bool allwhite = true;
						int baseoffsetBottomValue = 255;
						int minoffset = 255;
						int temp2 = 0, baseBottomValueTemp = 0;
						int baseBottomMaxValue = 0;
						int selectTOPValue;
						int minValueTemp = 255;
						//int recordcs, recordrs, recordc;
						//opencv way to find avalible space 
						/*cv::Mat result(selectBottom.size(),CV_32F,Scalar(0));
						cv::Mat baseBottomROI_32f(selectBottom.size(), CV_32F, Scalar(0));
						baseBottomROI.convertTo(baseBottomROI_32f, CV_32F);
						result = baseBottomROI_32f - selectTopProject_32f;
						double minoffsetMat;
						minMaxLoc(result, &minoffsetMat);
						if (minoffsetMat<0)breakBool = false;*/

						//find avalible space 
						for (int cs = 0; cs < selectBottom.cols && breakBool; cs++)
						{
							for (int rs = 0; rs < selectBottom.rows && breakBool; rs++)
							{
								if (selectBetween.at<uchar>(rs, cs) != 0)
								{
									temp2 = (basebottom.at<uchar>(rs + r, cs + c)) - (selectTopProject.at<uchar>(rs, cs));

									baseBottomValueTemp = basebottom.at<uchar>(rs + r, cs + c);

									/*if (baseBottomValueTemp < minValueTemp)
										minValueTemp = baseBottomValueTemp;*/

									//int x = basebottom.at<uchar>(rs + r, cs + c);
									//int y = (selectTopProject.at<uchar>(rs, cs) + 3);//
									if (temp2 <= 0)
										breakBool = false;
									/*else if (temp2 < minoffset)//======A: for bottom space
									{
									minoffset = temp2;
									baseoffsetBottomValue = basebottom.at<uchar>(rs + r, cs + c);
									selectTOPValue = selectTop.at<uchar>(rs, cs);

									}*/
								}
							}
						}
						if (breakBool == true)
						{

							if (c <= base_c)
							{
								base_c = c;
								base_r = r;

								/*if (minValueTemp == 255)
									maxZOffset = minSelBot-0.1;
									else*/
								maxZOffset = minSelBot;// -1;//backup

								getFirstSolution = true;

								//======A: for bottom space 		
								/*if (baseoffsetBottomValue == 255)
								{
								maxZOffset = minSelBot - 1;//backup
								getFirstSolution = true;
								}
								else
								{
								maxZOffset = selectTOPValue - (baseoffsetBottomValue - 3);
								getFirstSolution = true;
								}*/
							}
						}
						else if (!getFirstSolution)
						{
							cv::Mat offsetMat = selectBottom - ROI;
							double minoffsetMat;
							minMaxLoc(offsetMat, &minoffsetMat);
							if (minoffsetMat > second_maxZOffset)
							{
								second_maxZOffset = minoffsetMat;
								second_base_c = c;
								second_base_r = r;
							}
							else if (minoffsetMat == second_maxZOffset && second_base_c > c)
							{
								second_maxZOffset = minoffsetMat;
								second_base_c = c;
								second_base_r = r;
							}
						}


					}

				}
		}

		if (getFirstSolution)
		{
			x = base_c;
			y = base_r;
			z = maxZOffset;
		}
		else
		{
			x = second_base_c;
			y = second_base_r;
			z = second_maxZOffset;// -2;
		}
		return true;
	}
	static bool searchEmptySpaceInDepthImage_SLS(vector<cv::Mat > selectImage, vector<cv::Mat> grooveImage, int &x, int &y, int &z)
	{
		//===select
		cv::Mat selectTop = selectImage.at(0);//===z m1
		cv::Mat selectBottom = selectImage.at(1);
		cv::Mat selectBetween;
		double minSelBot, maxSelBot;
		//top distance to min value in bot
		minMaxLoc(selectBottom, &minSelBot, &maxSelBot);
		subtract(selectTop, selectBottom, selectBetween);
		cv::Mat selectTopProject = selectTop - minSelBot;
		//
		cv::Mat selectTopProject_32f(selectBottom.size(), CV_32F, Scalar(0));
		selectTopProject.convertTo(selectTopProject_32f, CV_32F);

		//===compare
		cv::Mat baseTop = grooveImage.at(0);
		cv::Mat basebottom = grooveImage.at(1);


		//
		int select_rows = selectBottom.rows;
		int select_cols = selectBottom.cols;

		int bigValue = 2000;
		int base_c = bigValue;
		int base_r = bigValue;

		int second_base_c = bigValue;
		int second_base_r = bigValue;

		int maxZOffset = 0;
		int second_maxZOffset = 0;
		bool breakBool = true;
		bool getFirstSolution = false;

		int midR = baseTop.rows / 2;
		int midC = baseTop.cols / 2;

		int maxrectlength = 200;//===groove size

		//====ccw path parameter====================================
		int index, xx, yy, temp_x, temp_y, centerX, centerY;
		int tempMaxSizeX, tempMaxSizeY, tempMinSizeX, tempMinSizeY;
		//int arrayy[SIZE][SIZE] = { 0 };
		int dynamicSize = 3;
		int offset = (dynamicSize - 1) / 2.;
		index = 0;
		xx = midC;
		yy = midR - 1;
		centerX = midC;
		centerY = midR;
		int dir_x[4] = { 0, 1, 0, -1 };
		int dir_y[4] = { 1, 0, -1, 0 };
		//=========================================================

		for (int i = 0; i < maxrectlength*maxrectlength && !getFirstSolution; i++)
		{

			temp_x = xx + dir_x[index];
			temp_y = yy + dir_y[index];
			if (i == dynamicSize*dynamicSize)
			{
				dynamicSize += 2;
				offset = (dynamicSize - 1) / 2.;
			}
			tempMaxSizeX = centerX + offset;
			tempMaxSizeY = centerY + offset;
			tempMinSizeX = centerX - offset;
			tempMinSizeY = centerY - offset;
			if (temp_x < 0 || temp_x > tempMaxSizeX || temp_x < tempMinSizeX ||
				temp_y < 0 || temp_y > tempMaxSizeY || temp_y < tempMinSizeY)
			{
				index = (index + 1) % 4;
			}
			//往下一個座標前進
			xx = xx + dir_x[index];
			yy = yy + dir_y[index];
			//qDebug() << "searchEmptySpaceInDepthImage_SLS %1, %2" << xx << yy;
			int c = xx;
			int r = yy;

			if (xx + select_cols < baseTop.cols && yy + select_rows < baseTop.rows)
			{
				//===start compare================================//
				cv::Mat ROI = baseTop(cv::Rect(c, r, select_cols, select_rows));
				cv::Mat baseBottomROI = basebottom(cv::Rect(c, r, select_cols, select_rows));
				breakBool = true;
				int temp2;
				int minoffset = 255;
				int baseoffsetBottomValue = 255;
				int selectTOPValue;
				for (int cs = 0; cs < selectBottom.cols && breakBool; cs++)
				{
					for (int rs = 0; rs < selectBottom.rows && breakBool; rs++)
					{
						if (selectBetween.at<uchar>(rs, cs) != 0)
						{
							int temp = basebottom.at<uchar>(rs + r, cs + c) - selectTopProject.at<uchar>(rs, cs);
							if (temp - 2 < 0)
								breakBool = false;
							/*temp2 = basebottom.at<uchar>(rs + r, cs + c) - (selectTopProject.at<uchar>(rs, cs) + 3);

							if (temp2 <= 0)
							breakBool = false;

							else if (temp2 < minoffset)
							{
							minoffset = temp2;
							baseoffsetBottomValue = basebottom.at<uchar>(rs + r, cs + c);
							selectTOPValue = selectTop.at<uchar>(rs, cs);

							}*/
						}
					}
				}
				//===end compare================================//
				if (breakBool == true)
				{
					if (c < base_c)
					{
						base_c = c;
						base_r = r;
						maxZOffset = minSelBot - 1;
						getFirstSolution = true;
					}
					//if (c <= base_c)
					//{
					//	base_c = c;
					//	base_r = r;

					//	//maxZOffset = minSelBot - 1;//backup
					//	//getFirstSolution = true;

					//	if (baseoffsetBottomValue == 255)
					//	{
					//		maxZOffset = minSelBot - 1;//backup
					//		getFirstSolution = true;
					//	}
					//	else
					//	{
					//		maxZOffset = selectTOPValue - (baseoffsetBottomValue - 3);
					//		getFirstSolution = true;

					//	}
					//}


				}
				else if (!getFirstSolution)
				{
					cv::Mat offsetMat = selectBottom - ROI;
					double minoffsetMat;
					minMaxLoc(offsetMat, &minoffsetMat);
					if (minoffsetMat > second_maxZOffset)
					{
						second_maxZOffset = minoffsetMat;
						second_base_c = c;
						second_base_r = r;
					}
				}
			}

		}







		//for (int rectlength = 1; rectlength < maxrectlength && !getFirstSolution; rectlength++)
		//{
		//	int datasize = rectlength*(rectlength - 2) * 2;
		//	for (int c = midC - rectlength; c < midC + rectlength + 1 && !getFirstSolution; c++)
		//	{
		//		int r = midR - rectlength;//
		//		if (c + select_cols < baseTop.cols)
		//		if (c == midC - rectlength || c == midC + rectlength)
		//		{
		//			for (r = midR - rectlength; r < midR + rectlength + 1 && !getFirstSolution; r++)
		//			{
		//				if (r + select_rows < baseTop.rows){
		//					//===start compare================================//
		//					cv::Mat ROI = baseTop(cv::Rect(c, r, select_cols, select_rows));
		//					cv::Mat baseBottomROI = basebottom(cv::Rect(c, r, select_cols, select_rows));
		//					breakBool = true;
		//					for (int cs = 0; cs < selectBottom.cols && breakBool; cs++)
		//					{
		//						for (int rs = 0; rs < selectBottom.rows && breakBool; rs++)
		//						{
		//							if (selectBetween.at<uchar>(rs, cs) != 0)
		//							{
		//								int temp = basebottom.at<uchar>(rs + r, cs + c) - selectTopProject.at<uchar>(rs, cs);
		//								if (temp - 2 < 0)
		//									breakBool = false;
		//							}
		//						}
		//					}
		//					//===end compare================================//
		//					if (breakBool == true)
		//					{
		//						if (c < base_c)
		//						{
		//							base_c = c;
		//							base_r = r;
		//							maxZOffset = minSelBot - 1;
		//							getFirstSolution = true;
		//						}
		//					}
		//					else if (!getFirstSolution)
		//					{
		//						cv::Mat offsetMat = selectBottom - ROI;
		//						double minoffsetMat;
		//						minMaxLoc(offsetMat, &minoffsetMat);
		//						if (minoffsetMat > second_maxZOffset)
		//						{
		//							second_maxZOffset = minoffsetMat;
		//							second_base_c = c;
		//							second_base_r = r;
		//						}
		//					}
		//					//qDebug() << "searchEmptySpaceInDepthImage_SLS %1, %2" << c << r;
		//				}
		//			}



		//		}
		//		else
		//		{
		//			for (r = midR - rectlength; r < midR + rectlength + 1 && !getFirstSolution; r += rectlength * 2)
		//			{
		//				if (r + select_rows < baseTop.rows){
		//					//===start compare================================//
		//					cv::Mat ROI = baseTop(cv::Rect(c, r, select_cols, select_rows));
		//					cv::Mat baseBottomROI = basebottom(cv::Rect(c, r, select_cols, select_rows));
		//					breakBool = true;
		//					for (int cs = 0; cs < selectBottom.cols && breakBool; cs++)
		//					{
		//						for (int rs = 0; rs < selectBottom.rows && breakBool; rs++)
		//						{
		//							if (selectBetween.at<uchar>(rs, cs) != 0)
		//							{
		//								int temp = basebottom.at<uchar>(rs + r, cs + c) - selectTopProject.at<uchar>(rs, cs);
		//								if (temp - 2 < 0)
		//									breakBool = false;
		//							}
		//						}
		//					}
		//					//===end compare================================//

		//					if (breakBool == true)
		//					{
		//						if (c < base_c)
		//						{
		//							base_c = c;
		//							base_r = r;
		//							maxZOffset = minSelBot - 1;
		//							getFirstSolution = true;
		//						}
		//					}
		//					else if (!getFirstSolution)
		//					{
		//						cv::Mat offsetMat = selectBottom - ROI;
		//						double minoffsetMat;
		//						minMaxLoc(offsetMat, &minoffsetMat);
		//						if (minoffsetMat > second_maxZOffset)
		//						{
		//							second_maxZOffset = minoffsetMat;
		//							second_base_c = c;
		//							second_base_r = r;
		//						}
		//					}
		//					//qDebug() << "searchEmptySpaceInDepthImage_SLS %1, %2" << c << r;
		//				}
		//			}


		//		}
		//	}
		//}

		if (getFirstSolution)
		{
			x = base_c;
			y = base_r;
			z = maxZOffset;
		}
		else
		{
			x = second_base_c;
			y = second_base_r;
			z = second_maxZOffset - 1;
		}




		//for (int c = 0; c < baseTop.cols && !getFirstSolution; c++)
		//{
		//	if (c + select_cols < baseTop.cols)
		//	for (int r = 0; r < baseTop.rows && !getFirstSolution; r++)
		//	{
		//		//for every pixel in baseTop
		//		if (r + select_rows < baseTop.rows)
		//		{
		//			cv::Mat ROI = baseTop(cv::Rect(c, r, select_cols, select_rows));
		//			cv::Mat baseBottomROI = basebottom(cv::Rect(c, r, select_cols, select_rows));
		//			breakBool = true;

		//			//opencv way to find avalible space 
		//			/*cv::Mat result(selectBottom.size(),CV_32F,Scalar(0));
		//			cv::Mat baseBottomROI_32f(selectBottom.size(), CV_32F, Scalar(0));
		//			baseBottomROI.convertTo(baseBottomROI_32f, CV_32F);
		//			result = baseBottomROI_32f - selectTopProject_32f;
		//			double minoffsetMat;
		//			minMaxLoc(result, &minoffsetMat);
		//			if (minoffsetMat<0)breakBool = false;*/

		//			//find avalible space 
		//			for (int cs = 0; cs < selectBottom.cols && breakBool; cs++)
		//			{
		//				for (int rs = 0; rs < selectBottom.rows && breakBool; rs++)
		//				{
		//					if (selectBetween.at<uchar>(rs, cs) != 0)
		//					{
		//						int temp = basebottom.at<uchar>(rs + r, cs + c) - selectTopProject.at<uchar>(rs, cs);
		//						if (temp - 2 < 0)
		//							breakBool = false;
		//					}
		//				}
		//			}
		//			if (breakBool == true)
		//			{
		//				if (c < base_c)
		//				{
		//					base_c = c;
		//					base_r = r;
		//					maxZOffset = minSelBot - 1;
		//					getFirstSolution = true;
		//				}
		//			}
		//			else if (!getFirstSolution)
		//			{
		//				cv::Mat offsetMat = selectBottom - ROI;
		//				double minoffsetMat;
		//				minMaxLoc(offsetMat, &minoffsetMat);
		//				if (minoffsetMat > second_maxZOffset)
		//				{
		//					second_maxZOffset = minoffsetMat;
		//					second_base_c = c;
		//					second_base_r = r;
		//				}
		//			}


		//		}

		//	}
		//}

		//if (getFirstSolution)
		//{
		//	x = base_c;
		//	y = base_r;
		//	z = maxZOffset;
		//}
		//else
		//{
		//	x = second_base_c;
		//	y = second_base_r;
		//	z = second_maxZOffset - 1;
		//}
		return true;
	}


	static bool GeneratePointsFromGrayImage(vector<cv::Mat > selectImage, vector<vcg::Point3f> &genPoints, float xMinPos, float offset, int steps, float ratio = 1., int DPI = 25)
	{
		//===select
		cv::Mat selectLeft = selectImage.at(0);//===z m1
		cv::flip(selectLeft, selectLeft, 1);
		//cv::Mat selectBottom = selectImage.at(1);
		//cv::Mat dst(selectLeft.size(), selectLeft.type());
		//bilateralFilter(selectLeft, dst, 5, 30, 30);
		//selectLeft = dst;
		//bool x = cv::imwrite("D://bilateralFilter.png", selectLeft);


		int select_rows = selectLeft.rows;
		int select_cols = selectLeft.cols;


		bool getFirstSolution = false;
		float halfGrooveBigX = DSP_grooveBigX / 2.;

		float halfwidth = selectLeft.cols / 2.;
		float halfheight = selectLeft.rows / 2.;


		for (int c = 0; c < selectLeft.cols && !getFirstSolution; c += steps)
		{
			for (int r = 0; r < selectLeft.rows && !getFirstSolution; r += steps)
			{
				if (selectLeft.at<uchar>(r, c) != 255)
				{
					//transfer gray to points position
					int pixelValue = selectLeft.at<uchar>(r, c);
					vcg::Point3f temp;
					temp.X() = -halfGrooveBigX + pixelValue * DSP_grooveBigX / 255. - offset;
					//temp.X() = xMinPos - offset;
					temp.Y() = SKT::pixelRatioToMm(halfwidth - c, ratio);// (halfwidth - c)*25.4 / DPI;;
					temp.Z() = SKT::pixelRatioToMm(halfheight - r, ratio);// *25.4 / DPI;
					genPoints.push_back(temp);

				}

			}
		}


		return true;
	}
	static bool GeneratePointsFromGrayImage_float(vector<cv::Mat > selectImage, vector<vcg::Point3f> &genPoints, float xMinPos, float offset, int steps, float ratio = 1., int DPI = 25)
	{
		//===select
		cv::Mat selectLeft = selectImage.at(0);//===z m1
		cv::flip(selectLeft, selectLeft, 1);


		int select_rows = selectLeft.rows;
		int select_cols = selectLeft.cols;



		float halfGrooveBigX = DSP_grooveBigX / 2.;

		float halfwidth = selectLeft.cols / 2.;
		float halfheight = selectLeft.rows / 2.;


		for (int c = 0; c < selectLeft.cols; c += steps)
		{
			for (int r = 0; r < selectLeft.rows; r += steps)
			{
				if (selectLeft.at<float>(r, c) != 1)
				{
					//transfer gray to points position
					float pixelValue = selectLeft.at<float>(r, c);
					//qDebug() << "pixelValue" << r << "_" << c << "_" << selectLeft.at<float>(r, c);
					vcg::Point3f temp;
					//temp.X() = -halfGrooveBigX + pixelValue * DSP_grooveBigX - offset;
					temp.X() = xMinPos - offset;
					temp.Y() = SKT::pixelRatioToMm(halfwidth - c, ratio);// (halfwidth - c)*25.4 / DPI;;
					temp.Z() = SKT::pixelRatioToMm(halfheight - r, ratio);// *25.4 / DPI;
					genPoints.push_back(temp);

				}

			}
		}


		return true;
	}
	static bool GeneratePointsFromGrayImage2(vector<cv::Mat > selectImage, vector<vcg::Point3f> &genPoints, float xMinPos, int steps, int DPI = 25)
	{
		//===select
		cv::Mat selectLeft = selectImage.at(0);//===z m1
		cv::flip(selectLeft, selectLeft, 1);



		int select_rows = selectLeft.rows;
		int select_cols = selectLeft.cols;


		bool getFirstSolution = false;
		float halfGrooveBigX = DSP_grooveBigX / 2.;

		float halfwidth = selectLeft.cols / 2.;
		float halfheight = selectLeft.rows / 2.;


		for (int c = 0; c < selectLeft.cols && !getFirstSolution; c += 1)
		{
			for (int r = 0; r < selectLeft.rows && !getFirstSolution; r += 1)
			{
				if (selectLeft.at<uchar>(r, c) != 255)
				{
					//transfer gray to points position
					int pixelValue = selectLeft.at<uchar>(r, c);
					vcg::Point3f temp;
					temp.X() = -halfGrooveBigX + pixelValue * DSP_grooveBigX / 255.;
					//temp.X() = xMinPos - offset;
					temp.Y() = (halfwidth - c)*25.4 / DPI;;
					temp.Z() = (halfheight - r)*25.4 / DPI;
					genPoints.push_back(temp);

				}

			}
		}


		return true;
	}
	static bool GeneratePointsFromGrayImage3(vector<cv::Mat > selectImage, vector<vcg::Point3f> &genPoints, float xMinPos, int steps, int DPI = 25)
	{
		//===select
		cv::Mat selectLeft = selectImage.at(0);//===z m1
		cv::flip(selectLeft, selectLeft, 1);



		int select_rows = selectLeft.rows;
		int select_cols = selectLeft.cols;


		bool getFirstSolution = false;
		float halfGrooveBigX = DSP_grooveBigX / 2.;

		float halfwidth = selectLeft.cols / 2.;
		float halfheight = selectLeft.rows / 2.;


		for (int c = 0; c < selectLeft.cols && !getFirstSolution; c += 1)
		{
			for (int r = 0; r < selectLeft.rows && !getFirstSolution; r += 1)
			{
				if (selectLeft.at<uchar>(r, c) != 255)
				{
					//transfer gray to points position
					int pixelValue = selectLeft.at<uchar>(r, c);
					vcg::Point3f temp;
					temp.X() = -halfGrooveBigX + pixelValue * DSP_grooveBigX / 255.;
					//temp.X() = xMinPos - offset;
					temp.Y() = (halfwidth - c)*25.4 / DPI;;
					temp.Z() = (halfheight - r)*25.4 / DPI;
					genPoints.push_back(temp);

				}

			}
		}


		return true;
	}
	static bool GeneratePointsFromContour(vector<cv::Mat > selectImage, vector<vcg::Point3f> &genPoints, float xMinPos, float offset)
	{
		//===select
		cv::Mat selectLeft = selectImage.at(0);//===z m1
		//cv::flip(selectLeft, selectLeft, 1);
		threshold(selectLeft, selectLeft, 254, 255, THRESH_BINARY);
		//blur(selectLeft, selectLeft, Size(3, 3));
		Mat edge;
		Canny(selectLeft, edge, 50, 150, 3);
		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;

		findContours(edge, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);



		//cv::Mat selectBottom = selectImage.at(1);

		//
		int select_rows = selectLeft.rows;
		int select_cols = selectLeft.cols;

		int base_c = 500;
		int base_r = 500;

		int second_base_c = 500;
		int second_base_r = 500;

		int maxZOffset = 0;
		int second_maxZOffset = 0;
		bool breakBool = true;
		bool getFirstSolution = false;

		for (int i = 0; i < contours.size(); i++)
		{
			vector<Point> *po = &contours[i];
			for (int j = 0; j < po->size(); j += 3)
			{
				vcg::Point3f temp;
				//temp.X() = -175 + pixelValue * 350 / 255 - offset;
				temp.X() = xMinPos - offset;
				temp.Y() = (109.5 - (*po)[j].x)*25.4 / 25;;
				temp.Z() = (99 - (*po)[j].y)*25.4 / 25;
				genPoints.push_back(temp);
			}
		}





		//for (int c = 0; c < selectLeft.cols && !getFirstSolution; c += 3)
		//{
		//	for (int r = 0; r < selectLeft.rows && !getFirstSolution; r += 3)
		//	{
		//		if (selectLeft.at<uchar>(r, c) != 255)
		//		{
		//			//transfer gray to points position
		//			//int pixelValue =  selectLeft.at<uchar>(r, c);
		//			vcg::Point3f temp;
		//			//temp.X() = -175 + pixelValue * 350 / 255 - offset;
		//			temp.X() = xMinPos - offset;
		//			temp.Y() = (109.5 - c)*25.4 / 25;;
		//			temp.Z() = (99 - r)*25.4 / 25;
		//			genPoints.push_back(temp);

		//		}

		//	}
		//}


		return true;
	}
	static bool mergePrePrinting(cv::Mat &srcColor, cv::Mat &prePrint, int percent = 0)
	{
		switch (srcColor.type())
		{
		case CV_8UC3:
		{
			int color_Percent = 255 * percent / 100.;
			for (int c = 0; c < srcColor.cols; c++)
			{
				for (int r = 0; r < srcColor.rows; r++)
				{
					//if (srcColor.at<cv::Vec3b>(r, c).val[0] == 0 && srcColor.at<cv::Vec3b>(r, c).val[1] ==0 && srcColor.at<cv::Vec3b>(r, c).val[2]==0)
					//if (srcColor.at<cv::Vec3b>(r, c) == Vec3b(0, 0, 0))
					if (prePrint.at<uchar>(r, c) == 255)
					{
						srcColor.at<Vec3b>(r, c) = Vec3b(color_Percent, color_Percent, color_Percent);// prePrint.at<Vec3b>(r, c);
					}

				}
			}

		}break;
		case CV_8UC1:
		{
			for (int c = 0; c < srcColor.cols; c++)
			{
				for (int r = 0; r < srcColor.rows; r++)
				{
					if (prePrint.at<uchar>(r, c) == 255)
					{
						srcColor.at<uchar>(r, c) = 0;
					}

				}
			}


		}break;
		}
		return true;
	}
	static cv::Mat reduc1(cv::Mat &srcImg)
	{
		cv::Mat copySrcI(srcImg.size(), CV_8UC3, Scalar(255, 255, 255));// = srcImg;
		srcImg.copyTo(copySrcI);

		unsigned int r_start = 255 - floor(256 * 0.54);
		unsigned int g_start = 255 - floor(256. * 0.64);
		unsigned int b_start = 255 - floor(256. * 0.93);

		unsigned int r_range = (256 * 0.54);
		unsigned int g_range = (256. * 0.64);
		unsigned int b_range = (256. * 0.93);

		for (int r = 0; r < copySrcI.rows; r++)
			for (int c = 0; c < copySrcI.cols; c++)
			{
				copySrcI.at<cv::Vec3b>(r, c).val[0] = 255 - (r_start + srcImg.at<cv::Vec3b>(r, c).val[2] * r_range / 256.);
				copySrcI.at<cv::Vec3b>(r, c).val[1] = 255 - (g_start + srcImg.at<cv::Vec3b>(r, c).val[1] * g_range / 256.);
				copySrcI.at<cv::Vec3b>(r, c).val[2] = 255 - (b_start + srcImg.at<cv::Vec3b>(r, c).val[0] * b_range / 256.);
			}
		return copySrcI;
	}

	static void BinderPercent(cv::Mat &binder, int ratioPercent)
	{
		double inv_ratio = (100 - ratioPercent) / 100.;
		int white = 0;

		uchar *binderr;
		for (int r = 0; r < binder.rows; r++)
		{
			binderr = binder.ptr<uchar>(r);
			for (int c = 0; c < binder.cols; c++)
			{
				if (binderr[c] == 0)
					binderr[c] = (uchar)(255 * inv_ratio);

			}
		}
	}

	static void combineBinderImage(cv::Mat &shellbinder1, cv::Mat &binder2)
	{
		uchar *shellBinderr;
		uchar *binderr2;
		for (int r = 0; r < shellbinder1.rows; r++)
		{
			shellBinderr = shellbinder1.ptr<uchar>(r);
			binderr2 = binder2.ptr<uchar>(r);
			/*for (int c = 0; c < shellbinder1.cols; c++)
			{
				if (shellBinderr[c] == 255 && binderr2[c] < 255)
					shellBinderr[c] = binderr2[c];


			}*/
			for (int c = 0; c < shellbinder1.cols; c++)
			{
				if (shellBinderr[c] == 0 && binderr2[c] == 255)
					shellBinderr[c] = binderr2[c];


			}
		}

	}

	static void createColorImage(cv::Mat &src1, cv::Mat &mask, bool c_bool, bool m_bool, bool y_bool)
	{
		if (mask.type() != CV_8UC1)
			cv::cvtColor(mask, mask, CV_BGR2GRAY);
		uchar *mask_r;

		if (src1.type() == CV_8UC3)
		{
			for (int r = 0; r < src1.rows; r++)
			{
				mask_r = mask.ptr<uchar>(r);
				for (int c = 0; c < src1.cols; c++)
				{
					if (mask_r[c] == 0)
					{
						if (!c_bool)
							src1.at<cv::Vec3b>(r, c).val[0] = 0;
						if (!m_bool)
							src1.at<cv::Vec3b>(r, c).val[1] = 0;

						if (!y_bool)
							src1.at<cv::Vec3b>(r, c).val[2] = 0;
					}
				}

			}
		}



	}

	struct MeshesImage{
		vector<int> meshInImage;

		vector<cv::Mat> image;
	};

	static bool mergeImage(cv::Mat &img1, cv::Mat &img2, int xOffsetPixel, int yOffsetPixel)
	{
		/*bitwise_not(mesh1, mesh1);
		bitwise_not(mesh2, mesh2);*/

		//Mat nonZeroCoordinatesM1, nonZeroCoordinatesM2;
		vector<Point> nonZeroCoordinatesM1, nonZeroCoordinatesM2;
		findNonZero(img1, nonZeroCoordinatesM1);
		findNonZero(img2, nonZeroCoordinatesM2);

		//cv::Rect testRectM1 = boundingRect(nonZeroCoordinatesM1);
		cv::Rect testRectM2 = boundingRect(nonZeroCoordinatesM2);
		cv::Mat translateM2(img2.size(), img2.type(), Scalar(0));

		if (testRectM2.x - xOffsetPixel >= 0 && xOffsetPixel >= 0)
			for (int i = 0; i < nonZeroCoordinatesM2.size(); i++)
			{
				Point temp = nonZeroCoordinatesM2[i];
				//********************************************need to modify 1832.............
				//***........................................................
				translateM2.at<uchar>(temp.y + yOffsetPixel, temp.x - xOffsetPixel) = 255;
				img1.at<uchar>(temp.y + yOffsetPixel, temp.x - xOffsetPixel) = 255;
			}
		translateM2.copyTo(img2);

		// Copy images in correct position		
		return true;
	}

	static void mergeOutlineImages(QVector<cv::Mat > outlineColorA, QVector<cv::Mat> &outlineMaskA, cv::Mat &output, QVector<cv::Mat> &outlineColorVV)
	{
		for (int i = 0; i < outlineMaskA.size(); i++)
		{
			if (outlineMaskA[i].type() != CV_8UC1)
				cv::cvtColor(outlineMaskA[i], outlineMaskA[i], CV_BGR2GRAY);
		}
		outlineColorA[0].copyTo(output);
		for (int i = 0; i < outlineColorVV.size(); i++)
		{

			outlineColorA[0].copyTo(outlineColorVV[i]);
		}
		//for (int i = 0; i < outlineColorA.size()-1; i++){
		for (int r = 0; r < outlineMaskA[0].rows; r++){
			for (int c = 0; c < outlineMaskA[0].cols; c++)
			{

				for (int i = 0; i < outlineColorA.size() - 1; i++)
				{
					if (outlineMaskA[i].at<uchar>(r, c) == 255 && outlineMaskA[i + 1].at<uchar>(r, c) == 0)
					{
						output.at<cv::Vec3b>(r, c) = outlineColorA[i + 1].at<cv::Vec3b>(r, c);

						for (int j = 1; j < outlineColorVV.size(); j++)
						{
							if (j>=(i+1))
								outlineColorVV[j].at<cv::Vec3b>(r, c) = outlineColorA[i + 1].at<cv::Vec3b>(r, c);
						}
						break;
					}
				}

			}
		}

		/*for (int i = 0; i < outlineColorVV.size() - 1; i++){
			for (int r = 0; r < outlineMaskA[0].rows; r++){
				for (int c = 0; c < outlineMaskA[0].cols; c++)
				{
					if (outlineMaskA[i].at<uchar>(r, c) == 255 && outlineMaskA[i + 1].at<uchar>(r, c) == 0)
					{
						outlineColorVV[i].at<cv::Vec3b>(r, c) = outlineColorA[i + 1].at<cv::Vec3b>(r, c);

						break;
					}

				}
			}
		}*/






		//}





	}


	class InkBalanceException : public std::exception
	{
	public:
		InkBalanceException(const char* str) : std::exception(str) {}
	};

	class InkBalance
	{
		unsigned inkSpotSpan;
		int cmyRepeatRuler[3];
		std::mt19937 randGenerator;
	public:
		void setInkSpotSpan(const unsigned &d){ inkSpotSpan = d; }
		void setInkPatternRepeatTimes(int c, int m, int y)
		{
			cmyRepeatRuler[0] = c;
			cmyRepeatRuler[1] = cmyRepeatRuler[0] + m;
			cmyRepeatRuler[2] = cmyRepeatRuler[1] + y;
		}
	private:
		cv::Vec3b* getInkSpot()
		{
			static cv::Vec3b c = { 0, 255, 255 };
			static cv::Vec3b m = { 255, 0, 255 };
			static cv::Vec3b y = { 255, 255, 0 };

			int n = randGenerator() % cmyRepeatRuler[2];
			if (n < cmyRepeatRuler[0]) return &c;
			if (n < cmyRepeatRuler[1]) return &m;
			return &y;
		}

	public:
		void operate(
			int inputCount,
			cv::Mat *colorOutlineImageSet, cv::Mat *binderImageSet, cv::Mat *monoOutlineImageSet,
			std::vector<cv::Mat> &outOutlineImgSet, std::vector<cv::Mat> &outBinderImgSet)
		{
#ifdef _DEBUG
			for (int i = 0; i < inputCount; ++i)
				if (binderImageSet[i].channels() != 1)
					throw InkBalanceException("InkBalance operate: binderImage must be read in CV_LOAD_IMAGE_GRAYSCALE mode");
			for (int i = 0; i < inputCount; ++i)
				if (monoOutlineImageSet[i].channels() != 1)
					throw InkBalanceException("InkBalance operate: monoOutlineImage must be read in CV_LOAD_IMAGE_GRAYSCALE mode");
#endif

			outOutlineImgSet.clear();
			outBinderImgSet.clear();

			for (int i = 0; i < inputCount; ++i)
			{
				outOutlineImgSet.push_back(colorOutlineImageSet[i]);
				outBinderImgSet.push_back(binderImageSet[i]);

				cv::Mat &colorOutline = outOutlineImgSet.back();
				cv::Mat &binder = outBinderImgSet.back();
				cv::Mat &monoOutline = monoOutlineImageSet[i];

				for (int r = 0, cstart = 0; r < binder.rows; r += inkSpotSpan, cstart = (cstart + 1) % inkSpotSpan)
				{
					for (int c = cstart; c < binder.cols; c += inkSpotSpan)
					{
						if (binder.at<unsigned char>(r, c) == 0) //is black
							if (monoOutline.at<unsigned char>(r, c) > 0) //is white
							{
								binder.at<unsigned char>(r, c) = 255;
								colorOutline.at<cv::Vec3b>(r, c) = *getInkSpot();
							}
					}
				}
			}
		}
		//
		void operate2(cv::Mat &colorOutlineImage, cv::Mat &binder, cv::Mat monoOutline, bool insertdot, bool lessInk = false)
		{

			cv::cvtColor(monoOutline, monoOutline, CV_BGR2GRAY);
#ifdef _DEBUG
			if (binder.channels() != 1)
				throw InkBalanceException("InkBalance operate: binderImage must be read in CV_LOAD_IMAGE_GRAYSCALE mode");

			if (monoOutline.channels() != 1)
				throw InkBalanceException("InkBalance operate: monoOutlineImage must be read in CV_LOAD_IMAGE_GRAYSCALE mode");
#endif


			/*for (int r = 0; r < binder.rows; r++)
			for (int c = 0; c < binder.cols; c++)
			{
			if (!(binder.at<uchar>(r, c) == 0 && monoOutline.at<uchar>(r, c) == 0))
			{
			colorOutlineImage.at<cv::Vec3b>(r, c).val[0] = 255;
			colorOutlineImage.at<cv::Vec3b>(r, c).val[1] = 255;
			colorOutlineImage.at<cv::Vec3b>(r, c).val[2] = 255;

			}
			}*/
			cv::parallelTestWithParallel_for_(colorOutlineImage, binder, monoOutline);


			//////////插點//////////
			if (insertdot)
				for (int r = 0, cstart = 0; r < binder.rows; r += inkSpotSpan, cstart = (cstart + 1) % inkSpotSpan)
				{
					for (int c = cstart; c < binder.cols; c += inkSpotSpan)
					{
						if (binder.at<unsigned char>(r, c) == 0) //is black
							if (monoOutline.at<unsigned char>(r, c) == 255) //is white
							{
								binder.at<unsigned char>(r, c) = 255;
								colorOutlineImage.at<cv::Vec3b>(r, c) = *getInkSpot();
							}

					}

				}
			///////////////////////////////////////////////////////////////////////////
			if (lessInk)
				for (int r = 0; r < binder.rows; r++)//有顏色的地方binder較少
					for (int c = 0; c < binder.cols; c++)
					{
						if (binder.at<unsigned char>(r, c) == 0 && (monoOutline.at<unsigned char>(r, c) == 0))
						{

							int totalColorValue = colorOutlineImage.at<cv::Vec3b>(r, c).val[0] * 0.299 + colorOutlineImage.at<cv::Vec3b>(r, c).val[1] * 0.587 + colorOutlineImage.at<cv::Vec3b>(r, c).val[2] * 0.114;
							//int totalColorValue = (colorOutlineImage.at<cv::Vec3b>(r, c).val[0]  + colorOutlineImage.at<cv::Vec3b>(r, c).val[1]  + colorOutlineImage.at<cv::Vec3b>(r, c).val[2] )/3.+128;
							//if (totalColorValue>255)totalColorValue = 255;
							//qDebug() << colorOutlineImage.at<cv::Vec3b>(r, c).val[0] * 0.114 << colorOutlineImage.at<cv::Vec3b>(r, c).val[1] * 0.587 << colorOutlineImage.at<cv::Vec3b>(r, c).val[2] * 0.299;
							if (totalColorValue < 50)binder.at<unsigned char>(r, c) = 255 - 0;
							else binder.at<unsigned char>(r, c) = 255 - totalColorValue;

						}
					}


		}
		void addCMYDots(cv::Mat &colorOutlineImage, cv::Mat &binder, cv::Mat monoOutline)
		{
			inkSpotSpan = 1;
			int count = 0;
			int count2 = 0;
			cv::Vec3b cmy[4];
			if (monoOutline.type() != CV_8UC1)
				cv::cvtColor(monoOutline, monoOutline, CV_BGR2GRAY);
			cmy[0] = { 0, 255, 255 };
			cmy[1] = { 255, 0, 255 };
			cmy[2] = { 255, 255, 0 };
			cmy[3] = { 50, 50, 50 };

			for (int c = 0; c < binder.cols; c++)
			{
				for (int r = 0; r < binder.rows; r++)
				{
					if (binder.at<unsigned char>(r, c) == 0 && monoOutline.at<unsigned char>(r, c) == 255)
					{
						count++;
						//if (count % 3 == 0)
						{
							count2++;
							//binder.at<unsigned char>(r, c) = 75;//255:spit none
							//colorOutlineImage.at<cv::Vec3b>(r, c) = cmy[count2 % 3];
							colorOutlineImage.at<cv::Vec3b>(r, c) = cmy[3];
						}
					}
				}
			}


			//for (int r = 0, cstart = 0; r < binder.rows; r ++)
			//{
			//	//for (int c = cstart; c < binder.cols; c += inkSpotSpan)
			//	for (int c = 0; c < binder.cols; c++)
			//	{
			//		if (binder.at<unsigned char>(r, c) == 0 && monoOutline.at<unsigned char>(r, c) == 255) 				
			//		{
			//			count++;
			//			/*if (count % 3 == 0)
			//			{*/
			//				count2++;
			//				//binder.at<unsigned char>(r, c) = 255;
			//				colorOutlineImage.at<cv::Vec3b>(r, c) = cmy[0];
			//			//}
			//		}

			//	}

			//}

		}

	public:
		InkBalance() : inkSpotSpan(10), randGenerator(time(NULL))
		{
			cmyRepeatRuler[0] = 1;
			cmyRepeatRuler[1] = 2;
			cmyRepeatRuler[2] = 3;
		}
	};

	//class SliceImageProcess
	//{
	//public :
	//	SliceImageProcess();
	//	void loadPattern(char *);
	//private:
	//	cv::Mat pattern;

	//};


	typedef struct { cmsUInt8Number r, g, b, a; }   Scanline_rgb1;
	typedef struct { cmsUInt16Number r, g, b, a; }  Scanline_rgb2;
	typedef struct { cmsUInt8Number r, g, b; }      Scanline_rgb8;
	typedef struct { cmsUInt16Number r, g, b; }     Scanline_rgb0;

	class ColorTransform
	{
	public:
		ColorTransform(){}
		static	void TitlePerformance(const char* Txt)
		{
			printf("%-45s: ", Txt); fflush(stdout);
		}
		static string type2str(int type) {
			string r;

			uchar depth = type & CV_MAT_DEPTH_MASK;
			uchar chans = 1 + (type >> CV_CN_SHIFT);

			switch (depth) {
			case CV_8U:  r = "8U"; break;
			case CV_8S:  r = "8S"; break;
			case CV_16U: r = "16U"; break;
			case CV_16S: r = "16S"; break;
			case CV_32S: r = "32S"; break;
			case CV_32F: r = "32F"; break;
			case CV_64F: r = "64F"; break;
			default:     r = "User"; break;
			}

			r += "C";
			r += (chans + '0');

			return r;
		}

		// Die, a fatal unexpected error is detected!
		void Die(const char* Reason, ...)
		{
			va_list args;
			va_start(args, Reason);
			//vsprintf(ReasonToFailBuffer, Reason, args);
			va_end(args);
			//printf("\n%s\n", ReasonToFailBuffer);
			fflush(stdout);
			exit(1);
		}

		static	cmsContext DbgThread(void)
		{
			static cmsUInt32Number n = 1;

			return (cmsContext)(void*)(n++ % 0xff0);
		}
		static	void PrintPerformance(cmsUInt32Number Bytes, cmsUInt32Number SizeOfPixel, cmsFloat64Number diff)
		{
			cmsFloat64Number seconds = (cmsFloat64Number)diff / CLOCKS_PER_SEC;
			cmsFloat64Number mpix_sec = Bytes / (1024.0*1024.0*seconds*SizeOfPixel);

			printf("%g MPixel/sec.\n", mpix_sec);
			fflush(stdout);
		}

		// Open an ICCprofile, from a file, or a standard internal sRGB
		static cmsHPROFILE OpenStockProfile(const char* File)
		{
			if (!File)
				return cmsCreate_sRGBProfile();

			if (stricmp(File, "*srgb") == 0)
				return cmsCreate_sRGBProfile();

			if (stricmp(File, "*null") == 0)
				return cmsCreateNULLProfile();

			return  cmsOpenProfileFromFile(File, "r");
		}


		//		static bool applyColorFromRGBtoCMY(Mat &srcImg, Mat &desImg, const char *szSrcProfile, const char *szDstProfile, int nIntent)
		//		{
		//			//unsigned long i, j, k, sn, sphoto, dn, photo, space;
		//			//CIccCmm cmm;
		//			//unsigned char *sptr, *dptr;
		//			//bool bSuccess = true;
		//			//bool bConvert = false;
		//
		//			//if (cmm.AddXform(szSrcProfile, nIntent < 0 ? icUnknownIntent : (icRenderingIntent)nIntent/*, icInterpTetrahedral*/)) {
		//			//	printf("Invalid Profile:  %s\n", szSrcProfile);
		//			//	return false;
		//			//}
		//
		//			//if (szDstProfile && *szDstProfile && cmm.AddXform(szDstProfile/*, icUnknownIntent, icInterpTetrahedral*/)) {
		//			//	printf("Invalid Profile:  %s\n", szDstProfile);
		//			//	return false;
		//			//}
		//
		//			//if (cmm.Begin() != icCmmStatOk) {
		//			//	printf("Invalid Profile:\n  %s\n  %s'\n", szSrcProfile, szDstProfile);
		//			//	return false;
		//			//}
		//
		//			//sn = 3;
		//			//dn = 3;
		//			//
		//
		//			//photo = cmm.GetDestSpace();
		//
		//			CIccCmm cmmi;
		//			
		//#pragma omp parallel private(cmmi) num_threads(4) 
		//			{				
		//				cmmi.AddXform(szSrcProfile, nIntent < 0 ? icUnknownIntent : (icRenderingIntent)nIntent/*, icInterpTetrahedral*/);
		//				cmmi.AddXform(szDstProfile/*, icUnknownIntent, icInterpTetrahedral*/);
		//				cmmi.Begin();				
		//
		////#pragma omp for
		//				for (int r = 0; r < srcImg.rows; r++)
		//				{
		//					icFloatNumber Pixel[16];
		//					cv::Vec3b* srcPixel = srcImg.ptr<cv::Vec3b>(r); // point to first pixel in row
		//					cv::Vec3b* desPixel = desImg.ptr<cv::Vec3b>(r); // point to first pixel in row
		//					for (int c = 0; c < srcImg.cols; c++)
		//					{
		//						Pixel[0] = (icFloatNumber)srcPixel[c][2] / 255.0f;
		//						Pixel[1] = (icFloatNumber)srcPixel[c][1] / 255.0f;
		//						Pixel[2] = (icFloatNumber)srcPixel[c][0] / 255.0f;
		//
		//						cmmi.Apply(Pixel, Pixel);
		//
		//						//for (k = 0; k < dn; k++) {
		//						desPixel[c][0] = (unsigned char)(UnitClip(Pixel[0]) * 255.0 + 0.5);
		//						desPixel[c][1] = (unsigned char)(UnitClip(Pixel[1]) * 255.0 + 0.5);
		//						desPixel[c][2] = (unsigned char)(UnitClip(Pixel[2]) * 255.0 + 0.5);
		//
		//						//}
		//						//cout <<(int) desPixel[r][0]<<" " << (int)desPixel[r][1]<<" " <<(int) desPixel[r][2] << endl;
		//					}
		//
		//				}
		//			}
		//
		//			return 1;
		//
		//		}
		//
		static bool applyColorFromRGBtoCMYK_LCMS(Mat &srcImg, Mat &desImg, const char *szSrcProfile, const char *szDstProfile, int nIntent)
		{
			clock_t atime;
			cmsFloat64Number diff;
			cmsHPROFILE hIn, hOut, hProof;
			cmsHTRANSFORM xform;
			DWORD wInput, wOutput;
			int OutputColorSpace;
			int bps = 1;
			DWORD dwFlags = 0;
			int nPlanes;

			hProof = NULL;
			hOut = NULL;


			hIn = OpenStockProfile(szSrcProfile);
			hOut = OpenStockProfile(szDstProfile);

			//xform = cmsCreateProofingTransform(hIn, wInput, hOut, wOutput, hProof,	Intent, ProofingIntent, dwFlags);

			xform = cmsCreateTransform(hIn, TYPE_RGBA_8, hOut, TYPE_CMYK_8, nIntent, dwFlags);
			string ty = type2str(srcImg.type());
			printf("Matrix: %s %dx%d \n", ty.c_str(), srcImg.cols, srcImg.rows);

			atime = clock();


			int nc = 0;
			if (srcImg.isContinuous())
			{
				nc = srcImg.rows*srcImg.cols;// *img1.channels();
			}
			Vec3b* data = srcImg.ptr<Vec3b>(0);
			Vec3b* desData = desImg.ptr<Vec3b>(0);
#pragma omp parallel for
			for (int i = 0; i < nc; i++)
			{
				cmsUInt8Number RGB[3];
				cmsUInt8Number Out[4];
				RGB[0] = data[i][2];
				RGB[1] = data[i][1];
				RGB[2] = data[i][0];
				cmsDoTransform(xform, RGB, Out, 1);
				desData[i][0] = Out[0];
				desData[i][1] = Out[1];
				desData[i][2] = Out[2];
			}
			/*diff = clock() - atime;

			int Mb = srcImg.cols*srcImg.rows*sizeof(cmsUInt8Number)* 3;
			PrintPerformance(Mb, sizeof(Scanline_rgb2), diff);
			cout << "diff:" << diff;*/

			cmsDeleteTransform(xform);
			cmsCloseProfile(hIn);
			cmsCloseProfile(hOut);

			return 1;

		}

		static bool applyColorFromRGBtoRGB_LCMS_RGBProfile(Mat &srcImg, Mat &desImg, const char *szSrcProfile, const char *szDstProfile, int nIntent)
		{
			clock_t atime;
			cmsFloat64Number diff;
			cmsHPROFILE hIn, hOut, hProof;
			cmsHTRANSFORM xform;
			DWORD wInput, wOutput;
			int OutputColorSpace;
			int bps = 1;
			DWORD dwFlags = 0;
			int nPlanes;

			hProof = NULL;
			hOut = NULL;


			hIn = OpenStockProfile(szSrcProfile);
			hOut = OpenStockProfile(szDstProfile);

			//xform = cmsCreateProofingTransform(hIn, TYPE_RGBA_8, hOut, TYPE_CMYK_8, hIn, nIntent, nIntent, dwFlags);
			//xform = cmsCreateProofingTransform(hIn, TYPE_RGBA_8, hOut, TYPE_CMYK_8, hIn, 0, 0, dwFlags);
			if (cmsGetColorSpace(hOut) != cmsSigCmykData);
			xform = cmsCreateTransform(hIn, TYPE_RGBA_8, hOut, TYPE_RGB_8, nIntent, dwFlags);
			string ty = type2str(srcImg.type());
			printf("Matrix: %s %dx%d \n", ty.c_str(), srcImg.cols, srcImg.rows);

			atime = clock();

			//*************************************
			int nc = 0;
			if (srcImg.isContinuous())
			{
				nc = srcImg.rows*srcImg.cols;// *img1.channels();
			}
			Vec3b* data = srcImg.ptr<Vec3b>(0);
			Vec3b* desData = desImg.ptr<Vec3b>(0);
#pragma omp parallel for
			for (int i = 0; i < nc; i++)
			{
				cmsUInt8Number RGB[3];
				cmsUInt8Number Out[3];
				RGB[0] = data[i][2];
				RGB[1] = data[i][1];
				RGB[2] = data[i][0];
				cmsDoTransform(xform, RGB, Out, 1);
				desData[i][0] = Out[2];
				desData[i][1] = Out[1];
				desData[i][2] = Out[0];
			}
			diff = clock() - atime;

			int Mb = srcImg.cols*srcImg.rows*sizeof(cmsUInt8Number) * 3;
			PrintPerformance(Mb, sizeof(Scanline_rgb2), diff);
			cout << "diff:" << diff;

			cmsDeleteTransform(xform);
			cmsCloseProfile(hIn);
			cmsCloseProfile(hOut);

			return 1;

		}
		/*static icFloatNumber UnitClip(icFloatNumber v)
		{
		if (v<0.0)
		return 0.0;
		if (v>1.0)
		return 1.0;
		return v;
		}*/

	};


}



#endif