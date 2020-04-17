#ifndef ZXGEN_H
#define ZXGEN_H

#include "sirius_print_job.h"
#include <opencv2/opencv.hpp>
#include <vector>
#include <cmath>
#include <exception>

class ZxGenException : public std::exception
{
public:
	ZxGenException(const char* str) : std::exception(str) {}
};

class ZxGen
{
private:
	FILE *dbgf;//@@@
	double SIRIUS_A4_WIDTH_INCH;
	double SIRIUS_MAX_IMAGE_WIDTH_PIXELS;

public:
	ZxGen(unsigned int imgWidth, unsigned int imgHeight, unsigned int nPlanes,double headwidth) :
		 SIRIUS_A4_WIDTH_INCH(headwidth/2.54),srcXRes(roundf(imgWidth / SIRIUS_A4_WIDTH_INCH)), srcYRes(roundf(imgHeight / SIRIUS_A4_WIDTH_INCH)),
		nCuts(nPlanes), nCutProcessed(0), job(), width(imgWidth), height(imgHeight)
	{
		SIRIUS_MAX_IMAGE_WIDTH_PIXELS = (int(SIRIUS_A4_WIDTH_INCH) + 1) * 800;
		initialize();
		//dbgf = fopen("D:\\debug.txt", "w");//@@@

		//fprintf(dbgf, "0\n");//@@@
		//fflush(dbgf);

	}
	ZxGen(unsigned int imgWidth, unsigned int imgHeight, unsigned int nPlanes, double headwidth,string name) :
		SIRIUS_A4_WIDTH_INCH(headwidth / 2.54), srcXRes(roundf(imgWidth / SIRIUS_A4_WIDTH_INCH)), srcYRes(roundf(imgHeight / SIRIUS_A4_WIDTH_INCH)),
		nCuts(nPlanes), nCutProcessed(0), job(name), width(imgWidth), height(imgHeight)
	{
		SIRIUS_MAX_IMAGE_WIDTH_PIXELS = (int(SIRIUS_A4_WIDTH_INCH) + 1) * 800;
		initialize();
		//dbgf = fopen("D:\\debug.txt", "w");//@@@

		//fprintf(dbgf, "0\n");//@@@
		//fflush(dbgf);
		//fprintf(dbgf, "0\n %d,imgwidth = %d", srcXRes, imgWidth);//@@@
		//fflush(dbgf);

	}


private:
	static void rgbOutlineBinder2cmykInterleaved(cv::Mat& glueImg, cv::Mat& colorImg, std::vector<uchar>& cmyk)
	{
		//FILE *dbgff;
		//dbgff = fopen("D:\\debug1.txt", "w");//@@@

		
		
		cmyk.clear();

		// Get rgb
		std::vector<cv::Mat> rgb;
		//rgb.resize(5);

		
		cv::split(colorImg, rgb);

		

		// rgb to cmyk, combine binder and color info
		cmyk.reserve(colorImg.cols * colorImg.rows * 4);

		

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

private:
	unsigned int width;
	unsigned int height;
	unsigned int srcXRes;
	unsigned int srcYRes;
	unsigned int nCuts;
	unsigned int nCutProcessed;
	PrintJob job;

private:
	void initialize()
	{
		job.setPerformanceOption(IAPI_PERF_PAGE_XRES, 800);
		job.setPerformanceOption(IAPI_PERF_PAGE_YRES, 800);
		job.setPerformanceOption(IAPI_PERF_RAWMONO_FORMAT, 0);
		job.setPerformanceOption(IAPI_PERF_MAX_IMAGE_WIDTH_PIXELS, SIRIUS_MAX_IMAGE_WIDTH_PIXELS);
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

		

		nCutProcessed += outlineImageSet.size();
		static uint32_t zjarr2[] = {
			ZJITEM_QUALITY, ZJITEM_QUALITY_BEST
		};
		static unsigned int nZjitems2 = sizeof(zjarr2) / (sizeof(uint32_t)* 2);
		std::vector<unsigned char> tempVectorCmykInterleaved;
		for (int i = 0; i < outlineImageSet.size(); ++i)
		{
			//cv::Mat &tempOutlineImage = outlineImageSet[i];
			//cv::Mat &tempGlueImage = glueImageSet[i];
			
			rgbOutlineBinder2cmykInterleaved(glueImageSet[i], outlineImageSet[i], tempVectorCmykInterleaved);			
			job.setBWOptimize(IAPI_FALSE);
			job.setIPSTracking(IAPI_THROUGHPUT_BW);			

			job.setLosslessMode(IAPI_LOSSLESS_MODE_LOSSLESS);
			job.setLosslessModeLosslessValue(0);
			job.setVariableModeVariableValue(0);
			job.setLossyParameters(80, 400, 400);
			job.setVariableMode(IAPI_VARIABLE_DATA_MODE_NONE);

			

			job.newPage(width, height, srcXRes, srcYRes, (void**)zjarr2, nZjitems2);

			for (int lineidx = 0, lineOffset = 128; lineidx < outlineImageSet[i].rows; lineidx += lineOffset)
			{
				if (lineidx + lineOffset >= outlineImageSet[i].rows)
					lineOffset = outlineImageSet[i].rows - lineidx;
				unsigned char* dataSet[] = { tempVectorCmykInterleaved.data() + lineidx*outlineImageSet[i].cols * 4 };
				unsigned int nDataSet = sizeof(dataSet) / sizeof(unsigned char*);
				unsigned char* *pDataSet = dataSet;
				job.writeImageData(IAPI_DATA_FORMAT_INTERLEAVED, nDataSet, pDataSet, outlineImageSet[i].cols, lineOffset, outlineImageSet[i].cols);
			}
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

#endif //ZXGEN_H
