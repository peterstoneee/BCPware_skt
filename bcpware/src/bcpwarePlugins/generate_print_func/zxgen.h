#ifndef ZXGEN_H
#define ZXGEN_H

#include "sirius_print_job.h"
#include <opencv2/opencv.hpp>
#include <vector>
#include <cstdlib>
#include <vector>
#include <map>
#include <list>
#include <cmath>
#include <thread>
#include <exception>

using namespace std;


struct Entry1DLut { float valfrom; float valto; };
static unsigned char cyan1DLut(const unsigned char &red)
{
	//lut should be sorted manually
	/*static Entry1DLut lut[] = {
	{ 0.0f, 255.0f - 96.9f }, { 56.355f, 255.0f - 96.9f }, { 131.6565f, 255.0f - 234.6f },
	{ 166.821f, 255.0f - 249.9f }, { 255.0f, 255.0f - 255.0f }
	};*/
	/*static Entry1DLut lut[] = {
		{ 0.0f, 255.0f - 96.9f }, { 56.355f, 255.0f - 96.9f }, { 131.6565f, 255.0f - 234.6f }, { 255.0f, 255.0f - 255.0f }
		};*/
	//For Chou Tom-How //@@@
	//For Chou Tom-How 2 //@@@
	static Entry1DLut lut[] = {
		{ 0.0f, 150.0f }, { 56.355f, 140.0f }, { 131.6565f, 10.0f }, { 255.0f, 255.0f - 255.0f }
	}; //@@@




	static int lut256[256] = {
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
	};
	static float fval;
	static Entry1DLut *ptr;
	static Entry1DLut *const pend = lut + sizeof(lut) / sizeof(Entry1DLut);
	static float firstvfrom;
	static float secondvfrom;
	static float firstvto;
	static float secondvto;

	if (lut256[red] != -1) return lut256[red];

	fval = red;
	for (ptr = lut; ptr != pend; ++ptr)
	{
		if (ptr->valfrom >= fval)
			break;
	}

	if (ptr->valfrom == fval)
	{
		lut256[red] = roundf(ptr->valto);
		return lut256[red];
	}
	secondvfrom = ptr->valfrom;
	secondvto = ptr->valto;
	//The situation that ptr point to the first element of lut should not happen, so "--ptr" is ok
	--ptr;
	firstvfrom = ptr->valfrom;
	firstvto = ptr->valto;

	lut256[red] = roundf(firstvto + (secondvto - firstvto)*(fval - firstvfrom) / (secondvfrom - firstvfrom));
	return lut256[red];
}
static unsigned char magenta1DLut(const unsigned char &green)
{
	//lut should be sorted manually
	/*static Entry1DLut lut[] = {
	{ 0.0f, 255.0f - 96.9f }, { 56.355f, 255.0f - 96.9f }, { 131.6565f, 255.0f - 234.6f },
	{ 166.821f, 255.0f - 249.9f }, { 255.0f, 255.0f - 255.0f }
	};*/
	/*static Entry1DLut lut[] = {
		{ 0.0f, 255.0f - 96.9f }, { 56.355f, 255.0f - 96.9f }, { 131.6565f, 255.0f - 234.6f }, { 255.0f, 255.0f - 255.0f }
		};*/
	//For Chou Tom-How 2 //@@@
	static Entry1DLut lut[] = {
		{ 0.0f, 140.0f }, { 131.6565f, 255.0f - 234.6f }, { 255.0f, 255.0f - 255.0f }
	}; //@@@


	static int lut256[256] = {
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
	};
	static float fval;
	static Entry1DLut *ptr;
	static Entry1DLut *const pend = lut + sizeof(lut) / sizeof(Entry1DLut);
	static float firstvfrom;
	static float secondvfrom;
	static float firstvto;
	static float secondvto;

	if (lut256[green] != -1) return lut256[green];

	fval = green;
	for (ptr = lut; ptr != pend; ++ptr)
	{
		if (ptr->valfrom >= fval)
			break;
	}

	if (ptr->valfrom == fval)
	{
		lut256[green] = roundf(ptr->valto);
		return lut256[green];
	}
	secondvfrom = ptr->valfrom;
	secondvto = ptr->valto;
	//The situation that ptr point to the first element of lut should not happen, so "--ptr" is ok
	--ptr;
	firstvfrom = ptr->valfrom;
	firstvto = ptr->valto;

	lut256[green] = roundf(firstvto + (secondvto - firstvto)*(fval - firstvfrom) / (secondvfrom - firstvfrom));
	return lut256[green];
}
static unsigned char yellow1DLut(const unsigned char &blue)
{//lut should be sorted manually
	/*static Entry1DLut lut[] = {
	{ 0.0f, 255.0f - 0.0f }, { 56.355f, 255.0f - 96.9f }, { 131.6565f, 255.0f - 234.6f },
	{ 166.821f, 255.0f - 249.9f }, { 255.0f, 255.0f - 255.0f }
	};*/
	/*static Entry1DLut lut[] = {
		{ 0.0f, 255.0f - 0.0f }, { 56.355f, 255.0f - 96.9f }, { 131.6565f, 255.0f - 234.6f }, { 255.0f, 255.0f - 255.0f }
		};*/
	//For Chou Tom-How //@@@
	static Entry1DLut lut[] = {
		{ 0.0f, 150.0f }, { 56.355f, 130.0f }, { 131.6565f, 255.0f - 234.6f }, { 255.0f, 255.0f - 255.0f }
	}; //@@@



	static int lut256[256] = {
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
	};
	static float fval;
	static Entry1DLut *ptr;
	static Entry1DLut *const pend = lut + sizeof(lut) / sizeof(Entry1DLut);
	static float firstvfrom;
	static float secondvfrom;
	static float firstvto;
	static float secondvto;

	if (lut256[blue] != -1) return lut256[blue];

	fval = blue;
	for (ptr = lut; ptr != pend; ++ptr)
	{
		if (ptr->valfrom >= fval)
			break;
	}

	if (ptr->valfrom == fval)
	{
		lut256[blue] = roundf(ptr->valto);
		return lut256[blue];
	}
	secondvfrom = ptr->valfrom;
	secondvto = ptr->valto;
	//The situation that ptr point to the first element of lut should not happen, so "--ptr" is ok
	--ptr;
	firstvfrom = ptr->valfrom;
	firstvto = ptr->valto;

	lut256[blue] = roundf(firstvto + (secondvto - firstvto)*(fval - firstvfrom) / (secondvfrom - firstvfrom));
	return lut256[blue];
}



class ZxGenException : public std::exception
{
public:
	ZxGenException(const char* str) : std::exception(str) {}
};

class ZxGen
{
private:
	//FILE *dbgf;//@@@
	double SIRIUS_A4_WIDTH_INCH;
	double SIRIUS_A4_HEIGHT_INCH;
	int SIRIUS_MAX_IMAGE_WIDTH_PIXELS;



public:
	ZxGen(unsigned int imgWidth, unsigned int imgHeight, unsigned int nPlanes, double headwidth, double headScanLength) :
		SIRIUS_A4_WIDTH_INCH(headwidth / DSP_inchmm), SIRIUS_A4_HEIGHT_INCH(headScanLength / DSP_inchmm), srcXRes(roundf(imgWidth / SIRIUS_A4_WIDTH_INCH)), srcYRes(roundf(imgHeight / SIRIUS_A4_HEIGHT_INCH)),
		nCuts(nPlanes), nCutProcessed(0), job(), width(imgWidth), height(imgHeight)
	{
		//SIRIUS_MAX_IMAGE_WIDTH_PIXELS = (int(SIRIUS_A4_WIDTH_INCH) + 1) * 800;
		SIRIUS_MAX_IMAGE_WIDTH_PIXELS = ceil(SIRIUS_A4_WIDTH_INCH * 800);
		initialize(true);
		//dbgf = fopen("D:\\debug.txt", "w");//@@@

		//fprintf(dbgf, "0\n");//@@@
		//fflush(dbgf);

	}
	ZxGen(unsigned int imgWidth, unsigned int imgHeight, unsigned int nPlanes, double headwidth, double headScanLength, string name, bool _printornot) :
		SIRIUS_A4_WIDTH_INCH(headwidth / DSP_inchmm), SIRIUS_A4_HEIGHT_INCH(headScanLength / DSP_inchmm), srcXRes(roundf(imgWidth / SIRIUS_A4_WIDTH_INCH)), srcYRes(roundf(imgHeight / SIRIUS_A4_HEIGHT_INCH)),
		nCuts(nPlanes), nCutProcessed(0), job(name), width(imgWidth), height(imgHeight), printornot(_printornot)
	{
		//SIRIUS_MAX_IMAGE_WIDTH_PIXELS = (int(SIRIUS_A4_WIDTH_INCH) + 1) * 800;
		SIRIUS_MAX_IMAGE_WIDTH_PIXELS = ceil(SIRIUS_A4_WIDTH_INCH * 800);
		initialize(true);
		//dbgf = fopen("D:\\debug.txt", "w");//@@@

		//fprintf(dbgf, "0\n");//@@@
		//fflush(dbgf);
		//fprintf(dbgf, "0\n %d,imgwidth = %d", srcXRes, imgWidth);//@@@
		//fflush(dbgf);

	}
	~ZxGen(){}


private:
	static void rgbOutlineBinder2cmykInterleaved_back_up(cv::Mat& glueImg, cv::Mat& colorImg, std::vector<uchar>& cmyk)
	{
		//FILE *dbgff;
		//dbgff = fopen("D:\\debug1.txt", "w");//@@@


		//fprintf(dbgff, "a-0\n"); //@@@
		//fflush(dbgff);
		cmyk.clear();

		// Get rgb
		std::vector<cv::Mat> rgb;
		//rgb.resize(5);
		//fprintf(dbgff, "a-3\n"); //@@@
		//fflush(dbgff);

		cv::split(colorImg, rgb);

		//fprintf(dbgff, "a-1\n"); //@@@
		//fflush(dbgff);

		// rgb to cmyk, combine binder and color info
		cmyk.reserve(colorImg.cols * colorImg.rows * 4);

		//fprintf(dbgff, "a-2\n"); //@@@
		//fflush(dbgff);

		for (int i = 0; i < colorImg.rows; ++i)
		{
			for (int j = 0; j < colorImg.cols; ++j)
			{
				cmyk.push_back(255u - rgb[2].at<uchar>(i, j));
				cmyk.push_back(255u - rgb[1].at<uchar>(i, j));
				cmyk.push_back(255u - rgb[0].at<uchar>(i, j));
				cmyk.push_back(255u - glueImg.at<unsigned char>(i, j));
			}
		}


	}

	void rgbOutlineBinder2cmykInterleaved(cv::Mat& glueImg, cv::Mat& colorImg, std::vector<uchar>& cmyk)
	{
		//FILE *dbgff;
		//dbgff = fopen("D:\\debug1.txt", "w");//@@@

		unsigned nPixels = colorImg.cols * colorImg.rows;
		cmyk.resize(nPixels * 4);



		// Get rgb
		std::vector<cv::Mat> rgb;
		cv::split(colorImg, rgb);
		static auto func = [&rgb](int imgRows, int imgCols, char modeRGB, uchar *cmykdata)
		{
			if (modeRGB == 'r')
			{
				for (int i = 0; i < imgRows; ++i)
				{
					for (int j = 0; j < imgCols; ++j)
					{
						//BE CAREFUL!!!  !!!Choose right code block!!!!

						/*cmyk[(i*colorImg.cols + j) * 4] = 255u - rgb[2].at<uchar>(i, j);
						cmyk[(i*colorImg.cols + j) * 4 + 1] = 255u - rgb[1].at<uchar>(i, j);
						cmyk[(i*colorImg.cols + j) * 4 + 2] = 255u - rgb[0].at<uchar>(i, j);*/

						cmykdata[(i*imgCols + j) * 4] = cyan1DLut(rgb[2].at<uchar>(i, j));

					}
				}
			}
			else if (modeRGB == 'g')
			{
				for (int i = 0; i < imgRows; ++i)
				{
					for (int j = 0; j < imgCols; ++j)
					{
						//BE CAREFUL!!!  !!!Choose right code block!!!!

						/*cmyk[(i*colorImg.cols + j) * 4] = 255u - rgb[2].at<uchar>(i, j);
						cmyk[(i*colorImg.cols + j) * 4 + 1] = 255u - rgb[1].at<uchar>(i, j);
						cmyk[(i*colorImg.cols + j) * 4 + 2] = 255u - rgb[0].at<uchar>(i, j);*/

						cmykdata[(i*imgCols + j) * 4 + 1] = magenta1DLut(rgb[1].at<uchar>(i, j));

					}
				}
			}
			else if (modeRGB == 'b')
			{
				for (int i = 0; i < imgRows; ++i)
				{
					for (int j = 0; j < imgCols; ++j)
					{
						//BE CAREFUL!!!  !!!Choose right code block!!!!

						/*cmyk[(i*colorImg.cols + j) * 4] = 255u - rgb[2].at<uchar>(i, j);
						cmyk[(i*colorImg.cols + j) * 4 + 1] = 255u - rgb[1].at<uchar>(i, j);
						cmyk[(i*colorImg.cols + j) * 4 + 2] = 255u - rgb[0].at<uchar>(i, j);*/

						cmykdata[(i*imgCols + j) * 4 + 2] = yellow1DLut(rgb[0].at<uchar>(i, j));

					}
				}
			}

		};

		std::thread threadR(func, colorImg.rows, colorImg.cols, 'r', cmyk.data());
		std::thread threadG(func, colorImg.rows, colorImg.cols, 'g', cmyk.data());
		std::thread threadB(func, colorImg.rows, colorImg.cols, 'b', cmyk.data());

		for (int i = 0; i < colorImg.rows; ++i)
		{
			for (int j = 0; j < colorImg.cols; ++j)
				cmyk[(i*colorImg.cols + j) * 4 + 3] = 255u - glueImg.at<unsigned char>(i, j);
		}

		threadR.join();
		threadG.join();
		threadB.join();

		//for (int i = 0; i < colorImg.rows; ++i)
		//{
		//	for (int j = 0; j < colorImg.cols; ++j)
		//	{
		//		//BE CAREFUL!!!  !!!Choose right code block!!!!


		//		/*cmyk[(i*colorImg.cols + j) * 4] = 255u - rgb[2].at<uchar>(i, j);
		//		cmyk[(i*colorImg.cols + j) * 4 + 1] = 255u - rgb[1].at<uchar>(i, j);
		//		cmyk[(i*colorImg.cols + j) * 4 + 2] = 255u - rgb[0].at<uchar>(i, j);*/

		//		/*std::future<unsigned char> cyanfut = std::async(std::launch::async, cyan1DLut, rgb[2].at<uchar>(i, j));
		//		std::future<unsigned char> magentafut = std::async(std::launch::async, magenta1DLut, rgb[1].at<uchar>(i, j));
		//		std::future<unsigned char> yellowfut = std::async(std::launch::async, yellow1DLut, rgb[0].at<uchar>(i, j));
		//		cmyk[(i*colorImg.cols + j) * 4] = cyanfut.get();
		//		cmyk[(i*colorImg.cols + j) * 4 + 1] = magentafut.get();
		//		cmyk[(i*colorImg.cols + j) * 4 + 2] = yellowfut.get();*/

		//		//std::future<unsigned char> cyanfut = std::async(std::launch::async, cyan1DLut, rgb[2].at<uchar>(i, j));
		//		cmyk[(i*colorImg.cols + j) * 4] = cyan1DLut(rgb[2].at<uchar>(i, j));
		//		cmyk[(i*colorImg.cols + j) * 4 + 1] = magenta1DLut(rgb[1].at<uchar>(i, j));
		//		cmyk[(i*colorImg.cols + j) * 4 + 2] = yellow1DLut(rgb[0].at<uchar>(i, j));
		//		//cmyk[(i*colorImg.cols + j) * 4] = cyanfut.get();


		//	}
		//	//printf("row %d\n", i);
		//}


	}

private:
	unsigned int width;
	unsigned int height;
	unsigned int srcXRes;
	unsigned int srcYRes;
	unsigned int nCuts;
	unsigned int nCutProcessed;
	bool printornot;
	PrintJob job;

private:
	/*void initialize()
	{
	job.setPerformanceOption(IAPI_PERF_PAGE_XRES, 800);
	job.setPerformanceOption(IAPI_PERF_PAGE_YRES, 800);
	job.setPerformanceOption(IAPI_PERF_RAWMONO_FORMAT, 0);
	job.setPerformanceOption(IAPI_PERF_MAX_IMAGE_WIDTH_PIXELS, SIRIUS_MAX_IMAGE_WIDTH_PIXELS);
	job.outputall->startprinting = printornot;
	static const char* colorStreams[] = { "CMYK" };
	static const unsigned int nColorStreams = sizeof(colorStreams) / sizeof(char*);
	uint32_t zjarr[] = {
	ZJITEM_DMCOPIES, 1,
	ZJITEM_QUALITY, ZJITEM_QUALITY_BEST,
	ZJITEM_PAGECOUNT, nCuts,
	ZJITEM_RIP_ID, SIRIUS_RIP_ID,
	ZJITEM_RIP_VERSION, SIRIUS_RIP_VERSION,
	ZJITEM_RIP_TIME, SIRIUS_RIP_TIME,
	ZJITEM_JOB_TIME, makeTime()
	};
	unsigned int nZjitems = sizeof(zjarr) / (sizeof(uint32_t)* 2);
	job.open(IAPI_PRINT_MODE_ZX_800x800, nColorStreams, colorStreams, (void**)zjarr, nZjitems);
	}*/
	void initialize(bool k2)
	{
		job.setPerformanceOption(IAPI_PERF_PAGE_XRES, 800);
		job.setPerformanceOption(IAPI_PERF_PAGE_YRES, 800);
		job.setPerformanceOption(IAPI_PERF_RAWMONO_FORMAT, 0);
		job.setPerformanceOption(IAPI_PERF_MAX_IMAGE_WIDTH_PIXELS, SIRIUS_MAX_IMAGE_WIDTH_PIXELS);
		job.outputall->startprinting = printornot;
		static const char* colorStreams[] = { "CMYK" };
		static const unsigned int nColorStreams = sizeof(colorStreams) / sizeof(char*);
		uint32_t zjarr[] = {
			ZJITEM_DMCOPIES, 1,
			ZJITEM_QUALITY, ZJITEM_QUALITY_BEST,
			ZJITEM_PAGECOUNT, nCuts,
			ZJITEM_RIP_ID, SIRIUS_RIP_ID,
			ZJITEM_RIP_VERSION, SIRIUS_RIP_VERSION,
			ZJITEM_RIP_TIME, SIRIUS_RIP_TIME,
			ZJITEM_JOB_TIME, makeTime()
		};
		unsigned int nZjitems = sizeof(zjarr) / (sizeof(uint32_t) * 2);
		job.open(IAPI_PRINT_MODE_ZX_800x800, nColorStreams, colorStreams, (void**)zjarr, nZjitems);
		if (k2)
		{
			std::vector<unsigned char> vecbmt;
			if (!createBMT(vecbmt))
			{
#ifdef _DEBUG
				throw ZxGenException("ZxGen: fail create BMT");
#endif
			}
			unsigned char *bmtdata = vecbmt.data();
			unsigned int bmtsiz = vecbmt.size();
			job.embedMediaTransforms(&bmtdata, &bmtsiz);
		}
	}


	unsigned int makeTime()
	{
		tm epochTm;
		epochTm.tm_year = 70;
		epochTm.tm_mon = 0;
		epochTm.tm_mday = 1;
		epochTm.tm_hour = 0;
		epochTm.tm_min = 0;
		epochTm.tm_sec = 0;

		return difftime(time(NULL), _mkgmtime(&epochTm));
	}


public:
	std::string getZxName() { return job.getZxName(); }

	void sendData2(){}
	void sendData(std::vector<cv::Mat> &outlineImageSet, std::vector<cv::Mat> &glueImageSet)
	{
		//glueImage must read in CV_LOAD_IMAGE_GRAYSCALE mode
		//fprintf(dbgf, "1\n"); //@@@
		//fflush(dbgf);

		if (outlineImageSet.empty() || outlineImageSet.empty()) throw ZxGenException("ZxGen: parameter should not be empty vector");

		if (outlineImageSet.size() != outlineImageSet.size()) throw ZxGenException("ZxGen: size of outlineImageSet and glueImageSet should be equal");
		if (nCutProcessed + outlineImageSet.size() > nCuts) throw ZxGenException("ZxGen: input too many pages");
		for (cv::Mat &m : glueImageSet)
			if (m.channels() != 1) throw ZxGenException("ZxGen: glueImage must read in CV_LOAD_IMAGE_GRAYSCALE mode");

		//fprintf(dbgf, "2\n"); //@@@
		//fflush(dbgf);


		nCutProcessed += outlineImageSet.size();
		static uint32_t zjarr2[] = {
			ZJITEM_QUALITY, ZJITEM_QUALITY_BEST
		};
		static unsigned int nZjitems2 = sizeof(zjarr2) / (sizeof(uint32_t) * 2);
		std::vector<unsigned char> tempVectorCmykInterleaved;
		for (int i = 0; i < outlineImageSet.size(); ++i)
		{
			//fprintf(dbgf, "3\n"); //@@@
			//fflush(dbgf);

			//cv::Mat &tempOutlineImage = outlineImageSet[i];
			//cv::Mat &tempGlueImage = glueImageSet[i];

			rgbOutlineBinder2cmykInterleaved(glueImageSet[i], outlineImageSet[i], tempVectorCmykInterleaved);
			job.setBWOptimize(IAPI_FALSE);
			job.setIPSTracking(IAPI_THROUGHPUT_BW);
			//fprintf(dbgf, "3-1\n"); //@@@
			//fflush(dbgf);
			job.setLosslessMode(IAPI_LOSSLESS_MODE_LOSSLESS);
			job.setLosslessModeLosslessValue(0);
			job.setVariableModeVariableValue(0);
			job.setLossyParameters(80, 400, 400);
			job.setVariableMode(IAPI_VARIABLE_DATA_MODE_NONE);

			//fprintf(dbgf, "4\n"); //@@@
			//fflush(dbgf);


			job.newPage(width, height, srcXRes, srcYRes, (void**)zjarr2, nZjitems2);
			//fprintf(dbgf, "5\n"); //@@@
			//fflush(dbgf);

			/*for (int lineidx = 0, lineOffset = 128; lineidx < outlineImageSet[i].rows; lineidx += lineOffset)
			{
			if (lineidx + lineOffset >= outlineImageSet[i].rows)
			lineOffset = outlineImageSet[i].rows - lineidx;
			unsigned char* dataSet[] = { tempVectorCmykInterleaved.data() + lineidx*outlineImageSet[i].cols * 4 };
			unsigned int nDataSet = sizeof(dataSet) / sizeof(unsigned char*);
			unsigned char* *pDataSet = dataSet;
			job.writeImageData(IAPI_DATA_FORMAT_INTERLEAVED, nDataSet, pDataSet, outlineImageSet[i].cols, lineOffset, outlineImageSet[i].cols);
			}*/
			unsigned char* dataSet[] = { tempVectorCmykInterleaved.data() };
			job.writeImageData(IAPI_DATA_FORMAT_INTERLEAVED, 1, dataSet, outlineImageSet[i].cols, outlineImageSet[i].rows, outlineImageSet[i].cols);


			//fprintf(dbgf, "7-%d\n", i); //@@@
			//fflush(dbgf);
			job.endPage();
		}



		/*unsigned int vmajor, vminor;
		IAPI_GetVersion(&vmajor, &vminor);
		status->append(tr("Start generate print job file\n"));
		status->append(tr("IAPI version %1.%2\n").arg(vmajor).arg(vminor));
		status->update();*/
	}

	void close()
	{
		//if (nCutProcessed != nCuts) throw ZxGenException("ZxGen: number of input pages is not same as nCuts");
		job.close();
	}


};
class ZxGen_SKT
{
public:
	typedef enum { RGB, CMY }pixelFormat;
	enum zxInitMode{ ViewCheckMode, RealPrintMode };
	ZxGen_SKT(double _imageDPI, unsigned int nPlanes, string name, bool _printornot, pixelFormat _pixelFormat = pixelFormat::RGB, zxInitMode _zxInitMode = RealPrintMode, std::wstring _icmFileName = QString().toStdWString()) :
		nCuts(nPlanes), nCutProcessed(0), job(name), imageDPI(_imageDPI), printornot(_printornot), inputPixelFormat(_pixelFormat), zxzInitMode(_zxInitMode), icmName(_icmFileName)
	{
		SIRIUS_MAX_IMAGE_WIDTH_PIXELS = 7016;// ceil((printHeadUsageInch / DSP_inchmm) * 800);
		initialize(true);
	}
	ZxGen_SKT(unsigned int imgWidth, unsigned int imgHeight, unsigned int nPlanes, double headwidth, double headScanLength, string name, bool _printornot) :
		SIRIUS_A4_WIDTH_INCH(headwidth / DSP_inchmm), SIRIUS_A4_HEIGHT_INCH(headScanLength / DSP_inchmm), srcXRes(roundf(imgWidth / SIRIUS_A4_WIDTH_INCH)), srcYRes(roundf(imgHeight / SIRIUS_A4_HEIGHT_INCH)),
		nCuts(nPlanes), nCutProcessed(0), job(name), width(imgWidth), height(imgHeight), printornot(_printornot)
	{
		SIRIUS_MAX_IMAGE_WIDTH_PIXELS = ceil(SIRIUS_A4_WIDTH_INCH * 800);
		initialize(true);
	}
	~ZxGen_SKT(){}


private:
	static void rgbOutlineBinder2cmykInterleaved_back_up_noLUT_noMP(cv::Mat& glueImg, cv::Mat& colorImg, std::vector<uchar>& cmyk)
	{
		//FILE *dbgff;
		//dbgff = fopen("D:\\debug1.txt", "w");//@@@


		//fprintf(dbgff, "a-0\n"); //@@@
		//fflush(dbgff);
		cmyk.clear();

		// Get rgb
		std::vector<cv::Mat> rgb;
		//rgb.resize(5);
		//fprintf(dbgff, "a-3\n"); //@@@
		//fflush(dbgff);

		cv::split(colorImg, rgb);

		//fprintf(dbgff, "a-1\n"); //@@@
		//fflush(dbgff);

		// rgb to cmyk, combine binder and color info
		cmyk.reserve(colorImg.cols * colorImg.rows * 4);

		//fprintf(dbgff, "a-2\n"); //@@@
		//fflush(dbgff);

		for (int i = 0; i < colorImg.rows; ++i)
		{
			for (int j = 0; j < colorImg.cols; ++j)
			{
				cmyk.push_back(255u - rgb[2].at<uchar>(i, j));
				cmyk.push_back(255u - rgb[1].at<uchar>(i, j));
				cmyk.push_back(255u - rgb[0].at<uchar>(i, j));
				cmyk.push_back(255u - glueImg.at<unsigned char>(i, j));
			}
		}


	}
	static void cmykInterleaved_back_up_noLUT_noMP(cv::Mat& glueImg, cv::Mat& colorImg, std::vector<uchar>& cmyk)
	{
		//FILE *dbgff;
		//dbgff = fopen("D:\\debug1.txt", "w");//@@@		
		cmyk.clear();

		// Get rgb
		std::vector<cv::Mat> CMY;

		cv::split(colorImg, CMY);


		// rgb to cmyk, combine binder and color info
		cmyk.reserve(colorImg.cols * colorImg.rows * 4);



		for (int i = 0; i < colorImg.rows; ++i)
		{
			for (int j = 0; j < colorImg.cols; ++j)
			{
				cmyk.push_back(CMY[0].at<uchar>(i, j));
				cmyk.push_back(CMY[1].at<uchar>(i, j));
				cmyk.push_back(CMY[2].at<uchar>(i, j));
				cmyk.push_back(255u - glueImg.at<unsigned char>(i, j));
			}
		}


	}


	void rgbOutlineBinder2cmykInterleaved_no_look_up_table(cv::Mat& glueImg, cv::Mat& colorImg, std::vector<uchar>& cmyk)
	{
		//FILE *dbgff;
		//dbgff = fopen("D:\\debug1.txt", "w");//@@@

		unsigned nPixels = colorImg.cols * colorImg.rows;
		cmyk.resize(nPixels * 4);



		// Get rgb
		std::vector<cv::Mat> rgb;
		cv::split(colorImg, rgb);
		cv::Mat glueImgcopy = glueImg;
		static auto func = [&rgb, &glueImgcopy](int imgRows, int imgCols, char modeRGB, uchar *cmykdata)
		{
			if (modeRGB == 'r')
			{
				for (int i = 0; i < imgRows; ++i)
				{
					for (int j = 0; j < imgCols; ++j)
					{
						cmykdata[(i*imgCols + j) * 4] = 255 - rgb[2].at<uchar>(i, j);

					}
				}
			}
			else if (modeRGB == 'g')
			{
				for (int i = 0; i < imgRows; ++i)
				{
					for (int j = 0; j < imgCols; ++j)
					{
						//BE CAREFUL!!!  !!!Choose right code block!!!!
						cmykdata[(i*imgCols + j) * 4 + 1] = 255 - rgb[1].at<uchar>(i, j); //@@@
					}
				}
			}
			else if (modeRGB == 'b')
			{
				for (int i = 0; i < imgRows; ++i)
				{
					for (int j = 0; j < imgCols; ++j)
					{
						//BE CAREFUL!!!  !!!Choose right code block!!!!
						cmykdata[(i*imgCols + j) * 4 + 2] = 255 - rgb[0].at<uchar>(i, j); //@@@

					}
				}
			}
			else if (modeRGB == 'k')
			{
				for (int i = 0; i < imgRows; ++i)
				{
					for (int j = 0; j < imgCols; ++j)
					{
						//BE CAREFUL!!!  !!!Choose right code block!!!!
						cmykdata[(i*imgCols + j) * 4 + 3] = 255 - glueImgcopy.at<unsigned char>(i, j); //@@@

					}
				}
			}

		};

		std::thread threadR(func, colorImg.rows, colorImg.cols, 'r', cmyk.data());
		std::thread threadG(func, colorImg.rows, colorImg.cols, 'g', cmyk.data());
		std::thread threadB(func, colorImg.rows, colorImg.cols, 'b', cmyk.data());
		std::thread threadk(func, colorImg.rows, colorImg.cols, 'k', cmyk.data());


		/*for (int i = 0; i < colorImg.rows; ++i)
		{
		for (int j = 0; j < colorImg.cols; ++j)
		cmyk[(i*colorImg.cols + j) * 4 + 3] = 255u - glueImg.at<unsigned char>(i, j);
		}*/

		threadR.join();
		threadG.join();
		threadB.join();
		threadk.join();

	}


	void rgbOutlineBinder2cmykInterleaved(cv::Mat& glueImg, cv::Mat& colorImg, std::vector<uchar>& cmyk)
	{
		//FILE *dbgff;
		//dbgff = fopen("D:\\debug1.txt", "w");//@@@

		unsigned nPixels = colorImg.cols * colorImg.rows;
		cmyk.resize(nPixels * 4);



		// Get rgb
		std::vector<cv::Mat> rgb;
		cv::split(colorImg, rgb);
		static auto func = [&rgb](int imgRows, int imgCols, char modeRGB, uchar *cmykdata)
		{
			if (modeRGB == 'r')
			{
				for (int i = 0; i < imgRows; ++i)
				{
					for (int j = 0; j < imgCols; ++j)
					{
						//BE CAREFUL!!!  !!!Choose right code block!!!!

						/*cmyk[(i*colorImg.cols + j) * 4] = 255u - rgb[2].at<uchar>(i, j);
						cmyk[(i*colorImg.cols + j) * 4 + 1] = 255u - rgb[1].at<uchar>(i, j);
						cmyk[(i*colorImg.cols + j) * 4 + 2] = 255u - rgb[0].at<uchar>(i, j);*/

						cmykdata[(i*imgCols + j) * 4] = cyan1DLut(rgb[2].at<uchar>(i, j));
						//cmykdata[(i*imgCols + j) * 4] = 255 - rgb[2].at<uchar>(i, j); //@@@

					}
				}
			}
			else if (modeRGB == 'g')
			{
				for (int i = 0; i < imgRows; ++i)
				{
					for (int j = 0; j < imgCols; ++j)
					{
						//BE CAREFUL!!!  !!!Choose right code block!!!!

						/*cmyk[(i*colorImg.cols + j) * 4] = 255u - rgb[2].at<uchar>(i, j);
						cmyk[(i*colorImg.cols + j) * 4 + 1] = 255u - rgb[1].at<uchar>(i, j);
						cmyk[(i*colorImg.cols + j) * 4 + 2] = 255u - rgb[0].at<uchar>(i, j);*/

						cmykdata[(i*imgCols + j) * 4 + 1] = magenta1DLut(rgb[1].at<uchar>(i, j));
						//cmykdata[(i*imgCols + j) * 4 + 1] = 255 - rgb[1].at<uchar>(i, j); //@@@

					}
				}
			}
			else if (modeRGB == 'b')
			{
				for (int i = 0; i < imgRows; ++i)
				{
					for (int j = 0; j < imgCols; ++j)
					{
						//BE CAREFUL!!!  !!!Choose right code block!!!!

						/*cmyk[(i*colorImg.cols + j) * 4] = 255u - rgb[2].at<uchar>(i, j);
						cmyk[(i*colorImg.cols + j) * 4 + 1] = 255u - rgb[1].at<uchar>(i, j);
						cmyk[(i*colorImg.cols + j) * 4 + 2] = 255u - rgb[0].at<uchar>(i, j);*/

						cmykdata[(i*imgCols + j) * 4 + 2] = yellow1DLut(rgb[0].at<uchar>(i, j));
						//cmykdata[(i*imgCols + j) * 4 + 2] = 255 - rgb[0].at<uchar>(i, j); //@@@
					}
				}
			}

		};

		std::thread threadR(func, colorImg.rows, colorImg.cols, 'r', cmyk.data());
		std::thread threadG(func, colorImg.rows, colorImg.cols, 'g', cmyk.data());
		std::thread threadB(func, colorImg.rows, colorImg.cols, 'b', cmyk.data());

		for (int i = 0; i < colorImg.rows; ++i)
		{
			for (int j = 0; j < colorImg.cols; ++j)
				cmyk[(i*colorImg.cols + j) * 4 + 3] = 255u - glueImg.at<unsigned char>(i, j);
		}

		threadR.join();
		threadG.join();
		threadB.join();

		//for (int i = 0; i < colorImg.rows; ++i)
		//{
		//	for (int j = 0; j < colorImg.cols; ++j)
		//	{
		//		//BE CAREFUL!!!  !!!Choose right code block!!!!


		//		/*cmyk[(i*colorImg.cols + j) * 4] = 255u - rgb[2].at<uchar>(i, j);
		//		cmyk[(i*colorImg.cols + j) * 4 + 1] = 255u - rgb[1].at<uchar>(i, j);
		//		cmyk[(i*colorImg.cols + j) * 4 + 2] = 255u - rgb[0].at<uchar>(i, j);*/

		//		/*std::future<unsigned char> cyanfut = std::async(std::launch::async, cyan1DLut, rgb[2].at<uchar>(i, j));
		//		std::future<unsigned char> magentafut = std::async(std::launch::async, magenta1DLut, rgb[1].at<uchar>(i, j));
		//		std::future<unsigned char> yellowfut = std::async(std::launch::async, yellow1DLut, rgb[0].at<uchar>(i, j));
		//		cmyk[(i*colorImg.cols + j) * 4] = cyanfut.get();
		//		cmyk[(i*colorImg.cols + j) * 4 + 1] = magentafut.get();
		//		cmyk[(i*colorImg.cols + j) * 4 + 2] = yellowfut.get();*/

		//		//std::future<unsigned char> cyanfut = std::async(std::launch::async, cyan1DLut, rgb[2].at<uchar>(i, j));
		//		cmyk[(i*colorImg.cols + j) * 4] = cyan1DLut(rgb[2].at<uchar>(i, j));
		//		cmyk[(i*colorImg.cols + j) * 4 + 1] = magenta1DLut(rgb[1].at<uchar>(i, j));
		//		cmyk[(i*colorImg.cols + j) * 4 + 2] = yellow1DLut(rgb[0].at<uchar>(i, j));
		//		//cmyk[(i*colorImg.cols + j) * 4] = cyanfut.get();


		//	}
		//	//printf("row %d\n", i);
		//}


	}

private:
	//FILE *dbgf;//@@@
	double SIRIUS_A4_WIDTH_INCH;
	double SIRIUS_A4_HEIGHT_INCH;
	int SIRIUS_MAX_IMAGE_WIDTH_PIXELS;
	unsigned int width;
	unsigned int height;
	unsigned int srcXRes;
	unsigned int srcYRes;
	unsigned int nCuts;
	unsigned int nCutProcessed;
	double imageDPI;
	double printHeadUsageInch;
	bool printornot;
	PrintJob job;
	pixelFormat inputPixelFormat;
	zxInitMode zxzInitMode;
	std::wstring icmName;

private:
	void initialize(bool k2)
	{
		job.setPerformanceOption(IAPI_PERF_PAGE_XRES, 800);
		job.setPerformanceOption(IAPI_PERF_PAGE_YRES, 800);
		job.setPerformanceOption(IAPI_PERF_RAWMONO_FORMAT, 0);
		job.setPerformanceOption(IAPI_PERF_MAX_IMAGE_WIDTH_PIXELS, SIRIUS_MAX_IMAGE_WIDTH_PIXELS);
		job.outputall->startprinting = printornot;
		static const char* colorStreams[] = { "CMYK" };
		static const unsigned int nColorStreams = sizeof(colorStreams) / sizeof(char*);
		uint32_t zjarr[] = {
			ZJITEM_DMCOPIES, 1,
			ZJITEM_QUALITY, ZJITEM_QUALITY_BEST,
			ZJITEM_PAGECOUNT, nCuts,
			ZJITEM_RIP_ID, SIRIUS_RIP_ID,
			ZJITEM_RIP_VERSION, SIRIUS_RIP_VERSION,
			ZJITEM_RIP_TIME, SIRIUS_RIP_TIME,
			ZJITEM_JOB_TIME, makeTime()
		};
		unsigned int nZjitems = sizeof(zjarr) / (sizeof(uint32_t) * 2);
		job.open(IAPI_PRINT_MODE_ZX_800x800, nColorStreams, colorStreams, (void**)zjarr, nZjitems);

		job.setLossyParameters(80, 400, 400);
		if (zxzInitMode == RealPrintMode)
		{
			job.setLosslessMode(IAPI_LOSSLESS_MODE_LOSSY);//lossy
			job.setIPSTracking(IAPI_THROUGHPUT_BW);
			job.setBWOptimize(IAPI_TRUE);
			//job.setLosslessMode(IAPI_LOSSLESS_MODE_LOSSLESS);
		}
		else if (zxzInitMode == ViewCheckMode)
		{
			job.setLosslessMode(IAPI_LOSSLESS_MODE_LOSSLESS);

		}
		/*
			testIAPI
			(const wchar_t *icmFile, const char **colorNames, unsigned char* *bmts, unsigned int *bmt_sizes)
			*/
		//std::vector<unsigned char *> bmts;
		//std::vector<unsigned int> bmtSizes;
		//bmts.resize(4);
		//bmtSizes.resize(4);

		//wstring icmFilename = {L"C:/Users/TB495076/Desktop/VC_WorkSpace/SKT_version1_VCS2_SVN2/bcpware/src/bcpwarePlugins/generate_print_func/ColorProfile/PlainPaper1600x1600color.icm" };
		//static const char* colorStreamscICM[] = { "CYAN","MAGENTA","YELLOW","BLACK" };
		////job.embedMediaTransformsfromfile(icmFilename.c_str(), colorStreamscICM, &bmts[0], &bmtSizes[0]);
#if 1
		if (k2 && icmName.size()>0)//test read dither matrix from color profile
		{
			std::vector<unsigned char> vecbmtt;

			unsigned char *bmtdata2[4];// = vecbmt.data();

			//bmtdata = new unsigned char*[4];

			unsigned int bmtsize2[4] = { 0, 0, 0, 0 };

			unsigned char *memblock;

			//const wchar_t *icmfile = L"C:/Users/TB495076/Desktop/VC_WorkSpace/SKT_version1_VCS2_SVN2/bcpware/src/distrib_de/60percentDitherv3.icm";
			//const wchar_t *icmfile = L"./60percentDitherv3.icm";
			//const wchar_t *icmfile = L"./PlainPaper1600x1600color.icm";
			const wchar_t *icmfile = icmName.c_str();

			/*string s = "./profiles/GlossyLabel_1600x1600_color.icm";
			std::wstring icmfile(s.begin(), s.end());*/


			unsigned int icmsize = sizeof(memblock) / sizeof(unsigned char);


			static const unsigned int colorPlanesICM = 4;// sizeof(colorStreams) / sizeof(char*);
			static const char* colorStreamscICM[] = { "CYAN", "MAGENTA", "YELLOW", "BLACK" };


			job.embedMediaTransformsfromfile(icmfile, colorPlanesICM, colorStreamscICM, bmtdata2, bmtsize2);
			job.embedMediaTransforms(bmtdata2, bmtsize2);


		}
		else
		{
			std::vector<unsigned char> vecbmt;
			if (!createBMT(vecbmt))
			{
#ifdef _DEBUG
				throw ZxGenException("ZxGen: fail create BMT");
#endif
			}
			unsigned char *bmtdata = vecbmt.data();
			unsigned int bmtsiz = vecbmt.size();
			//unsigned int bmtsiz = 57360;
			job.embedMediaTransforms(&bmtdata, &bmtsiz);

		}
#endif

		//job.setLosslessMode(IAPI_LOSSLESS_MODE_AUTOSTENCIL);
#if 0
		if (k2)
		{
			std::vector<unsigned char> vecbmt;
			if (!createBMT(vecbmt))
			{
#ifdef _DEBUG
				throw ZxGenException("ZxGen: fail create BMT");
#endif
			}
			unsigned char *bmtdata = vecbmt.data();
			unsigned int bmtsiz = vecbmt.size();
			//unsigned int bmtsiz = 57360;
			job.embedMediaTransforms(&bmtdata, &bmtsiz);

		}
#endif
	}


	unsigned int makeTime()
	{
		tm epochTm;
		epochTm.tm_year = 70;
		epochTm.tm_mon = 0;
		epochTm.tm_mday = 1;
		epochTm.tm_hour = 0;
		epochTm.tm_min = 0;
		epochTm.tm_sec = 0;

		return difftime(time(NULL), _mkgmtime(&epochTm));
	}


public:
	std::string getZxName() { return job.getZxName(); }

	bool sendData2(std::vector<cv::Mat> &outlineImageSet, std::vector<cv::Mat> &glueImageSet)
	{
		if (outlineImageSet.empty() || outlineImageSet.empty()) throw ZxGenException("ZxGen: parameter should not be empty vector");

		if (outlineImageSet.size() != outlineImageSet.size()) throw ZxGenException("ZxGen: size of outlineImageSet and glueImageSet should be equal");
		if (nCutProcessed + outlineImageSet.size() > nCuts) throw ZxGenException("ZxGen: input too many pages");
		for (cv::Mat &m : glueImageSet)
			if (m.channels() != 1) throw ZxGenException("ZxGen: glueImage must read in CV_LOAD_IMAGE_GRAYSCALE mode");

		nCutProcessed += outlineImageSet.size();
		static uint32_t zjarr2[] = {
			ZJITEM_QUALITY, ZJITEM_QUALITY_BEST
		};
		static unsigned int nZjitems2 = sizeof(zjarr2) / (sizeof(uint32_t) * 2);
		std::vector<unsigned char> tempVectorCmykInterleaved;
		for (int i = 0; i < outlineImageSet.size(); ++i)
		{
			if (inputPixelFormat == pixelFormat::RGB)
			{
				//rgbOutlineBinder2cmykInterleaved(glueImageSet[i], outlineImageSet[i], tempVectorCmykInterleaved);
				rgbOutlineBinder2cmykInterleaved_no_look_up_table(glueImageSet[i], outlineImageSet[i], tempVectorCmykInterleaved);
				//rgbOutlineBinder2cmykInterleaved_back_up_noLUT_noMP(glueImageSet[i], outlineImageSet[i], tempVectorCmykInterleaved);
			}
			else if (inputPixelFormat == pixelFormat::CMY)
			{
				cmykInterleaved_back_up_noLUT_noMP(glueImageSet[i], outlineImageSet[i], tempVectorCmykInterleaved);
			}

			/*job.setBWOptimize(IAPI_FALSE);
			job.setIPSTracking(IAPI_THROUGHPUT_BW);
			job.setLosslessMode(IAPI_LOSSLESS_MODE_LOSSLESS);
			job.setLosslessModeLosslessValue(0);
			job.setVariableModeVariableValue(0);
			job.setLossyParameters(80, 400, 400);
			job.setVariableMode(IAPI_VARIABLE_DATA_MODE_NONE);*/

			//for (int y = 0; y < tempVectorCmykInterleaved.size(); y++)
			//{
			//if (tempVectorCmykInterleaved[y] > 0)
			//qDebug() <<"tempVectorCmykInterleaved[y] "<< tempVectorCmykInterleaved[y];
			//}

			job.newPage(outlineImageSet[i].cols, outlineImageSet[i].rows, imageDPI, imageDPI, (void**)zjarr2, nZjitems2);

			unsigned char* dataSet[] = { tempVectorCmykInterleaved.data() };
			job.writeImageData(IAPI_DATA_FORMAT_INTERLEAVED, 1, dataSet, outlineImageSet[i].cols, outlineImageSet[i].rows, outlineImageSet[i].cols);
			job.endPage();
		}
		if (job.outputall->sendSuccessful)
			return true;
		else return false;


	}
	void sendData(std::vector<cv::Mat> &outlineImageSet, std::vector<cv::Mat> &glueImageSet)
	{
		//glueImage must read in CV_LOAD_IMAGE_GRAYSCALE mode
		//fprintf(dbgf, "1\n"); //@@@
		//fflush(dbgf);

		if (outlineImageSet.empty() || outlineImageSet.empty()) throw ZxGenException("ZxGen: parameter should not be empty vector");

		if (outlineImageSet.size() != outlineImageSet.size()) throw ZxGenException("ZxGen: size of outlineImageSet and glueImageSet should be equal");
		if (nCutProcessed + outlineImageSet.size() > nCuts) throw ZxGenException("ZxGen: input too many pages");
		for (cv::Mat &m : glueImageSet)
			if (m.channels() != 1) throw ZxGenException("ZxGen: glueImage must read in CV_LOAD_IMAGE_GRAYSCALE mode");

		//fprintf(dbgf, "2\n"); //@@@
		//fflush(dbgf);


		nCutProcessed += outlineImageSet.size();
		static uint32_t zjarr2[] = {
			ZJITEM_QUALITY, ZJITEM_QUALITY_BEST
		};
		static unsigned int nZjitems2 = sizeof(zjarr2) / (sizeof(uint32_t) * 2);
		std::vector<unsigned char> tempVectorCmykInterleaved;
		for (int i = 0; i < outlineImageSet.size(); ++i)
		{
			//fprintf(dbgf, "3\n"); //@@@
			//fflush(dbgf);

			//cv::Mat &tempOutlineImage = outlineImageSet[i];
			//cv::Mat &tempGlueImage = glueImageSet[i];

			rgbOutlineBinder2cmykInterleaved(glueImageSet[i], outlineImageSet[i], tempVectorCmykInterleaved);
			job.setBWOptimize(IAPI_FALSE);
			job.setIPSTracking(IAPI_THROUGHPUT_BW);
			//fprintf(dbgf, "3-1\n"); //@@@
			//fflush(dbgf);
			job.setLosslessMode(IAPI_LOSSLESS_MODE_LOSSLESS);
			job.setLosslessModeLosslessValue(0);
			job.setVariableModeVariableValue(0);
			job.setLossyParameters(80, 400, 400);
			job.setVariableMode(IAPI_VARIABLE_DATA_MODE_NONE);

			//fprintf(dbgf, "4\n"); //@@@
			//fflush(dbgf);


			job.newPage(width, height, srcXRes, srcYRes, (void**)zjarr2, nZjitems2);
			//fprintf(dbgf, "5\n"); //@@@
			//fflush(dbgf);

			/*for (int lineidx = 0, lineOffset = 128; lineidx < outlineImageSet[i].rows; lineidx += lineOffset)
			{
			if (lineidx + lineOffset >= outlineImageSet[i].rows)
			lineOffset = outlineImageSet[i].rows - lineidx;
			unsigned char* dataSet[] = { tempVectorCmykInterleaved.data() + lineidx*outlineImageSet[i].cols * 4 };
			unsigned int nDataSet = sizeof(dataSet) / sizeof(unsigned char*);
			unsigned char* *pDataSet = dataSet;
			job.writeImageData(IAPI_DATA_FORMAT_INTERLEAVED, nDataSet, pDataSet, outlineImageSet[i].cols, lineOffset, outlineImageSet[i].cols);
			}*/
			unsigned char* dataSet[] = { tempVectorCmykInterleaved.data() };
			job.writeImageData(IAPI_DATA_FORMAT_INTERLEAVED, 1, dataSet, outlineImageSet[i].cols, outlineImageSet[i].rows, outlineImageSet[i].cols);


			//fprintf(dbgf, "7-%d\n", i); //@@@
			//fflush(dbgf);
			job.endPage();
		}



		/*unsigned int vmajor, vminor;
		IAPI_GetVersion(&vmajor, &vminor);
		status->append(tr("Start generate print job file\n"));
		status->append(tr("IAPI version %1.%2\n").arg(vmajor).arg(vminor));
		status->update();*/
	}
	void sendData3(std::vector<cv::Mat> &outlineImageSet, std::vector<cv::Mat> &glueImageSet)
	{
		if (outlineImageSet.empty() || outlineImageSet.empty()) throw ZxGenException("ZxGen: parameter should not be empty vector");

		if (outlineImageSet.size() != outlineImageSet.size()) throw ZxGenException("ZxGen: size of outlineImageSet and glueImageSet should be equal");
		if (nCutProcessed + outlineImageSet.size() > nCuts) throw ZxGenException("ZxGen: input too many pages");
		for (cv::Mat &m : glueImageSet)
			if (m.channels() != 1) throw ZxGenException("ZxGen: glueImage must read in CV_LOAD_IMAGE_GRAYSCALE mode");

		nCutProcessed += outlineImageSet.size();
		static uint32_t zjarr2[] = {
			ZJITEM_QUALITY, ZJITEM_QUALITY_BEST
		};
		static unsigned int nZjitems2 = sizeof(zjarr2) / (sizeof(uint32_t) * 2);
		std::vector<unsigned char> tempVectorCmykInterleaved;
		for (int i = 0; i < outlineImageSet.size(); ++i)
		{
			//rgbOutlineBinder2cmykInterleaved(glueImageSet[i], outlineImageSet[i], tempVectorCmykInterleaved);
			rgbOutlineBinder2cmykInterleaved_back_up_noLUT_noMP(glueImageSet[i], outlineImageSet[i], tempVectorCmykInterleaved);

			job.newPage(outlineImageSet[i].cols, outlineImageSet[i].rows, imageDPI, imageDPI, (void**)zjarr2, nZjitems2);

			unsigned char* dataSet[] = { tempVectorCmykInterleaved.data() };
			job.writeImageData(IAPI_DATA_FORMAT_INTERLEAVED, 1, dataSet, outlineImageSet[i].cols, outlineImageSet[i].rows, outlineImageSet[i].cols);
			job.endPage();

		}


	}
	void close()
	{
		//if (nCutProcessed != nCuts) throw ZxGenException("ZxGen: number of input pages is not same as nCuts");
		job.close();
	}


};
#endif //ZXGEN_H
