#include <opencv2/opencv.hpp>
using namespace std;

namespace cv
{
	namespace test
	{
		class parallelTestBody : public ParallelLoopBody// 
		{
		public:
			parallelTestBody(Mat& _colorOutline, Mat &_binder, Mat &_monoOutline)//class constructor  
			{
				colorOutline = &_colorOutline;
				binder = &_binder;
				monoOutline = &_monoOutline;

			}
			parallelTestBody(Mat& _src)//class constructor  
			{
				src = &_src;
			}
			//void operator()(const Range& range) const//重载操作符（）  
			//{
			//	//register type *inputOutputBufferPTR = bufferToClip + range.start;
			//	Mat& srcMat = *src;
			//	int stepSrc = (int)(srcMat.step / srcMat.elemSize1());//获取每一行的元素总个数（相当于cols*channels，等同于step1)  
			//	for (int colIdx = range.start; colIdx < range.end; ++colIdx)
			//	{
			//		float* pData = (float*)srcMat.col(colIdx).data;	
			//		for (int i = 0; i < srcMat.rows; ++i)
			//			pData[i*stepSrc] = std::pow(pData[i*stepSrc], 3);
			//	}
			//}
			void operator()(const Range& range) const//
			{
				Mat& outline = *colorOutline;
				Mat& bindertemp = *binder;
				Mat& monotemp = *monoOutline;

				int colorcols = (int)(outline.step / outline.elemSize1());//
				int bindercols = (int)(bindertemp.step / bindertemp.elemSize1());
				int monocols = (int)(monotemp.step / monotemp.elemSize1());

				for (int colIdx = range.start; colIdx < range.end; ++colIdx)
				{
					unsigned char* pColorData = (unsigned char*)outline.col(colIdx).data;//Mat::data, pointer to data，data的pointer
					unsigned char* binderData = (unsigned char*)bindertemp.col(colIdx).data;
					unsigned char* monoData = (unsigned char*)monotemp.col(colIdx).data;

					for (int i = 0; i < outline.rows; ++i)//pData[i*stepSrc] = std::pow(pData[i*stepSrc], 3);
					{
						if (!(binderData[i*bindercols] == 0 && monoData[i*monocols] == 0))
						{
							pColorData[i*colorcols + 0] = 255;
							pColorData[i*colorcols + 1] = 255;
							pColorData[i*colorcols + 2] = 255;
						}
					}
				}


			}

		private:
			Mat* src;
			Mat* colorOutline;
			Mat* binder;
			Mat* monoOutline;
		};

		class parallelUpDownMergeBody : public ParallelLoopBody// 
		{
		public:
			parallelUpDownMergeBody(Mat &_out,Mat& _upBlack, Mat &_upColor, Mat &_downBlack, Mat &_downColor)//class constructor  
			{
				out = &_out;
				upBlackMask = &_upBlack;
				upColorMask = &_upColor;
				downBlackMask = &_downBlack;
				downColorMask = &_downColor;

			}			
			
			void operator()(const Range& range) const//
			{
				Mat& upBlack = *upBlackMask;
				Mat& upColor = *upColorMask;
				Mat& downBlack = *downBlackMask;
				Mat& downColor = *downColorMask;

				int blackCols = (int)(upBlack.step / upBlack.elemSize1());//
				int colorCols = (int)(upColor.step / upColor.elemSize1());
				

				for (int colIdx = range.start; colIdx < range.end; ++colIdx)
				{
					unsigned char* upBlackData = (unsigned char*)upBlack.col(colIdx).data;
					unsigned char* upColorData = (unsigned char*)upColor.col(colIdx).data;
					unsigned char* downBlackData = (unsigned char*)downBlack.col(colIdx).data;
					unsigned char* downColorData = (unsigned char*)downColor.col(colIdx).data;

					for (int i = 0; i < upColor.rows; ++i)//didn't work will crash
					{
						if (upBlackData[i*blackCols] < 20)
						{
							out[i*colorCols + 0] = downColorData[i*colorCols + 0];
							out[i*colorCols + 1] = downColorData[i*colorCols + 1];
							out[i*colorCols + 2] = downColorData[i*colorCols + 2];
						}
						/*else if (int(downBlackData[i*blackCols]))
						{
							out[i*colorCols + 0] = upColorData[i*colorCols + 0];
							out[i*colorCols + 1] = upColorData[i*colorCols + 1];
							out[i*colorCols + 2] = upColorData[i*colorCols + 2];
						}	*/					
					}
				}


			}

		private:
			

			Mat *upBlackMask;
			Mat *upColorMask;
			Mat *downBlackMask;
			Mat *downColorMask;
			Mat *out;

		};



		
	}//namesapce test  	

	void parallelTestWithParallel_for_(InputArray _src, InputArray _src2, InputArray _src3)//'parallel_for_' loop  
	{
		CV_Assert(_src.kind() == _InputArray::MAT);
		Mat src = _src.getMat();
		Mat src2 = _src2.getMat();
		Mat src3 = _src3.getMat();

		int totalCols = src.cols;
		typedef test::parallelTestBody parallelTestBody;
		parallel_for_(Range(0, totalCols), parallelTestBody(src, src2, src3));
	}

	void parallelUpDownMerge_for_(InputArray _out, InputArray _src,InputArray _src2, InputArray _src3, InputArray _src4)//'parallel_for_' loop  
	{
		CV_Assert(_src.kind() == _InputArray::MAT);
		Mat out = _out.getMat();
		Mat src1 = _src.getMat();
		Mat src2 = _src2.getMat();
		Mat src3 = _src3.getMat();
		Mat src4 = _src4.getMat();

		int totalCols = src1.cols;
		typedef test::parallelUpDownMergeBody parallelUpDownMergeBody;
		parallel_for_(Range(0, totalCols), parallelUpDownMergeBody(out, src1, src2, src3, src4));
	}

}//namespace cv 