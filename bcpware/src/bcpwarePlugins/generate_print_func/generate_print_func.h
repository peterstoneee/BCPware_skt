/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/


/****************************************************************************
* My Extension to Meshlab
*
*  Copyright 2018 by
*
* This file is part of Meshlab.
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
/*
	
*/
#ifndef GNEERATE_PRINT_FUNC_H
#define GNEERATE_PRINT_FUNC_H

#include <QObject>
#include <QProgressDialog>
#include <QTimer>
#include <QWaitCondition>  
#include <QMutex> 
#include <common/interfaces.h>
#include <common/picaApplication.h>
#include<opencv2/opencv.hpp>
#include "zxgen.h"
#include "printthread.h"
#include "RTree.h"
//#include "gui_launcher.h"

//#include <QThread>
//class Thread2;

class GenSliceJob : public QObject, public MeshFilterInterface//, public QThread
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(MESH_FILTER_INTERFACE_IID)
	Q_INTERFACES(MeshFilterInterface)

public:
	enum { FP_GPU_EXAMPLE, 
		FP_GENERATE_FILE, 
		FP_GENERATE_MONO, 
		FP_PRINT_FLOW, 
		FP_PRINT_FLOW_2, 
		FP_PRINT_FLOW_3, 
		FP_PRINT_JR, 
		FP_ESTIMATE_COLOR, 
		FP_ESTIMATE_COLOR2,
		FP_ESTIMATE_SURFACE_COLOR,
		FP_PRINT_TEST_PAGE ,
		FP_PRINT_INTERSECT_TRIANGLE,
		FP_MESH_INTERSECT_TEST,
		FP_TEST_AUTOPACK_V2,
		FP_LANDING_BY_IMAGE_Z_AXIS,
		FP_LANDING_BY_IMAGE_X_AXIS,
		FP_LANDING_BY_IMAGE_Y_AXIS,
		FP_GET_DEPTH_IMAGE,
		FP_GET_DEPTH_LANDING_TEST2,
		/*FP_GET_DEPTH_X_LANDING_TEST2,
		FP_GET_DEPTH_Y_LANDING_TEST2,*/
		FP_PACKING_VERSION_3,
		FP_TEST_SEARCH_SPACE_ALL_IMAGE,
		FP_TEST_SEARCH_SPACE_ALL_IMAGE2,//NOT ASK USER TO ROTATE
		FP_TEST_GENERATE_POINTS_FROM_GRAY_IMAGE,
		FP_TEST_GENERATE_POINTS_FROM_GRAY_IMAGE_FLOAT,
		FP_TEST_WITNESS_BAR,
		FP_MESH_INTERSECT_TEST2,
		FP_MESH_DELAUNAY_TRIANGULATION,
		FP_COUNT_BINDER_AREA,
		FP_PRINT_FOUR_IN_ONE,
		FP_PRINT_DM_SLICER,
		FP_PRINT_DM_SLICER_ALPHA,
		FP_PRINT_FLOW_2_V3,
		FP_SAVE_DEFAULT_SNAP_SHOT,
		FP_DM_ESTIMATE_COLOR
		


	};

	enum{
		genPF_X_Axis,
		genPF_Y_Axis,
		genPF_Z_Axis

	};
	enum
	{
		gen_top_down_image,
		right_left_image,
		front_back_image
	};
	struct fuck_maint
	{
		double fuck_spitton;
		double fuck_wiper;
		double fuck_useles_print;
		double fuck_printUsage;
		double fuck_sum(){ return fuck_spitton + fuck_wiper + fuck_useles_print ; }
		fuck_maint()
		{
			fuck_spitton = 0;
			fuck_wiper = 0;
			fuck_printUsage = 0;
		}

	}fuck_maint_c, fuck_maint_m, fuck_maint_y;

    GenSliceJob();
	~GenSliceJob();
	virtual QString pluginName(void) const { return "ExtraSampleGPUPlugin"; }
	void initParameterSet(QAction *action, MeshDocument &md, RichParameterSet & parlst);

	QString filterName(FilterIDType filter) const;
	QString filterInfo(FilterIDType filter) const;
	bool applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & /*parent*/, vcg::CallBackPos * cb,GLArea *a = 0,vcg::SendBackData *sb = 0);
	FilterClass getClass(QAction *a);
	void init_texture(MeshDocument &md);
	GLuint texName;
	GLuint texture[10];


public :
	/*cv::Mat originalOutline;
	cv::Mat look_down_color_CV;
	cv::Mat look_up_color_CV;
	cv::Mat outlineCV;
	cv::Mat look_down_Black_Mask_layer;
	cv::Mat look_down_Black_MaskCV;
	cv::Mat look_up_Black_MaskCV;
	cv::Mat black_Mask;
	cv::Mat black_Mask2;
	cv::Mat innerColorMask;
	cv::Mat upMergeDownCV;
	cv::Mat outlineBlackMaskCV;
	cv::Mat outlineColorCv;
	cv::Mat up_downBlackMaskCV;

	cv::Mat finalcolor;
	cv::Mat finalBinder;
	cv::Mat backbinder;
	cv::Mat mirrorColor;
	cv::Mat mirrorBinder;

	cv::Mat copyCaptemp;

	cv::Mat lookDownBlackTriangleMask;

	cv::Mat downColorFirstCV;
	cv::Mat downBlackMaskFirstCV;
	cv::Mat downColorSecondCV;
	cv::Mat downBlackMaskSecondCV;

	cv::Mat prePrintobjectB;
	cv::Mat prePrintobjectC;
	cv::Mat captemp;
	cv::Mat capDownMask;*/

	

	void resetImage(cv::Mat &, int width, int height);
private:
	QTimer *testtimer;
	vcg::CallBackPos * cbb;
    std::vector<cv::Mat> outlineCTemp;
    std::vector<cv::Mat> capCTemp;
	
	//cv::Mat binderIm;
	//cv::Mat	 originalCapImage;
    //ZxGen *zG;
	void releaseMat();

signals:
	void updatepd(int per);
	void test_update_pages(int i);
	
public slots:
	void setpd(int per);
	void testprintf();
public:
	QWaitCondition cond;
	QMutex mutex;
	/*QThread *th1;
	Thread2 th2;*/
	QProgressDialog *progress;
	//QTimer *tt;
	void setpd2(int per);

	std::string makeDefaultZxName()
	{
		time_t rawtime;
		time(&rawtime);
		tm *pltime = localtime(&rawtime);
		char timestr[8 + 1 + 9 + 1];
		sprintf(timestr,
			"%.4d%.2d%.2d" "_" "%.2dh%.2dm%.2ds",
			pltime->tm_year + 1900, pltime->tm_mon + 1, pltime->tm_mday,
			pltime->tm_hour, pltime->tm_min, pltime->tm_sec);
		timestr[18] = '\0';
		//std::string retstr = "d:\\output";
		std::string retstr = "output";
		retstr += timestr;
		retstr += ".zx";

		return retstr;
	}
	static bool MySearchCallback(int id, void* arg)
	{
		//printf("Hit data rect %d\n", id);
		recordID.push_back(id);
		//qDebug() << "Hit data rect " << id;
		return true; // keep going
	}
	static QVector<int> recordID;
private:
	void cmystrips(MeshDocument & md, float);
	void binderstrips(MeshDocument & md, float);
	void drawquad(float z_offset);
	void generateZX(float width, float height, int pages,float,string );
	void generateOneHundredZX(float width, float height);
	QString getRoamingDir();
	QString makeOutputname(QString dir, int numpages );
	QString makeOutputname2(QString dir, QString );
	void fillsquare(void);
	void combineImage(QImage &, QImage &);

	size_t faceconnect(MeshModel &m);

	bool getMeshLayerImage(QList<MeshModel *> mdlist, std::map<int, std::vector<cv::Mat> *> &);
	bool getMeshLayerImagePlaneX(QList<MeshModel *> mdlist, std::map<int, std::vector<cv::Mat> *> &);
	bool getMultiMeshLayerImage(QList<MeshModel *> mdlist, std::vector<cv::Mat> &, Box3m);
	bool getMultiMeshLayerImageZ(QList<MeshModel *> mdlist, std::vector<cv::Mat> &, Box3m);

	bool testDepthBuffer(QList<MeshModel *> mdlist, std::vector<cv::Mat> &, Box3m, QVector<bool> &, Box3m, bool scale = true, bool copyToBig = false, int DPI = 25, float pixelRatio = 1);
	bool testDepthBuffer_float(QList<MeshModel *> mdlist, std::vector<cv::Mat> &, Box3m, QVector<bool> &, bool scale = true, bool copyToBig = false, int DPI = 25, int pixelRatio = 1);

	bool testDepthRecursive_func(MeshModel &mmm, MeshDocument &md, float gap, bool x, vcg::Point3f movePosition = {0, 0, 0});
	//bool testGenCubeStructure(MeshDocument &md);
	//bool createMeshRTree(MeshModel &mm,RTree<int, float, 3> tree);
	
	bool testIntersection(Point3m v1, Point3m v2, Point3m v3, Point3m orig, Point3m dir);
	//bool testIntersection2(Point3m v1, Point3m v2, Point3m v3, Point3m orig, Point3m dir);
	void initMeshVBO(MeshDocument &);

	GLdouble equ[4];// = { 0, 0, -1, 0 };
	GLdouble equ2[4];
	int ollX;// = 20;
	int ollY;// = 20;
	int ollZ;// = 20;
	void testthreadfunc();
    PrintThread thread1;
	ZxGen *zggg;
	ZxGen_SKT *zx_skt;

	GLuint drawList;
	vector<int> drawLists;
	void genDrawList(MeshDocument &);
    //gui_launcher<mywidget> gl;

	int RoundUpToTheNextHighestPowerOf2(unsigned int v);
	


};





#endif

