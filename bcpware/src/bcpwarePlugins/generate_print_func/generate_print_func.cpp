
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
#define GEN_SURFACE0

//#ifdef _DEBUG
//#define DEBU_GENSLICEFUNC
//#endif
//
//#ifdef DEBU_GENSLICEFUNC
//#define LOGFILENAME "genSlice.log"
//#endif
//
//#ifdef DEBU_GENSLICEFUNC
//static FILE *_log_fptr = fopen(LOGFILENAME, "wb");
//static int _log_funclv = 0;
//#endif

//check list
//1.lossless or auto stencil
//2.brighter
//3.frank look up table // zx gen L931
//4.binder is white or not//SET BINDER TO WHITE
//5.testswitch print middle pages
//	5.1 middle pages //search//int testPrintPageCount = 100;
//6. image quality
//7.white kick
//8.inner gray algorithm
//9.rgb or cmy format


//#include <QtOpenGL>
#include "generate_print_func.h"
#include <wrap/glw/glw.h>
#include <QImage>
#include <QDialog>
#include "slice_function.h"
#include "convertQI_CV.h"
#include "image_process.h"
#include <thread>
#include <time.h>
#include <QImageWriter>
#include <algorithm>  
#include <vcg/complex/algorithms/create/ball_pivoting.h>
#include <SKTlib\skt_function.h>
#include <QVector>
#include "packingFunc.h"
#include "meshPackingTest.h"

#include <vcg/complex/algorithms/create/extended_marching_cubes.h>
#include <vcg/complex/algorithms/create/marching_cubes.h>
#include "gmpfrac.h"
#include "intercept.h"

//#include "IntersectionTest.h"




// Constructor usually performs only two simple tasks of filling the two lists
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

using namespace glw;
using namespace vcg::intercept;

//bool MySearchCallback(int id, void* arg)
//{
//	//printf("Hit data rect %d\n", id);
//	qDebug()<<"Hit data rect "<< id;
//	return true; // keep going
//}

QVector<int> GenSliceJob::recordID;
static void createBBox(Point3m p1, Point3m p2, Point3m p3, float *bmin, float *bmax)
{
	float sx1 = p1.X();
	float sx2 = p2.X();
	float sx3 = p3.X();

	float sy1 = p1.Y();
	float sy2 = p2.Y();
	float sy3 = p3.Y();

	float sz1 = p1.Z();
	float sz2 = p2.Z();
	float sz3 = p3.Z();



	bmax[0] = sx1 > sx2 ? (sx1 > sx3 ? sx1 : sx3) : (sx2 > sx3 ? sx2 : sx3);
	bmax[1] = sy1 > sy2 ? (sy1 > sy3 ? sy1 : sy3) : (sy2 > sy3 ? sy2 : sy3);
	bmax[2] = sz1 > sz2 ? (sz1 > sz3 ? sz1 : sz3) : (sz2 > sz3 ? sz2 : sz3);

	bmin[0] = sx1 < sx2 ? (sx1 < sx3 ? sx1 : sx3) : (sx2 < sx3 ? sx2 : sx3);
	bmin[1] = sy1 < sy2 ? (sy1 < sy3 ? sy1 : sy3) : (sy2 < sy3 ? sy2 : sy3);
	bmin[2] = sz1 < sz2 ? (sz1 < sz3 ? sz1 : sz3) : (sz2 < sz3 ? sz2 : sz3);

}

GenSliceJob::GenSliceJob()
{
	typeList << FP_GPU_EXAMPLE << FP_GENERATE_MONO << FP_PRINT_FLOW << FP_PRINT_FLOW_2 << FP_PRINT_JR
		<< FP_ESTIMATE_COLOR << FP_PRINT_FLOW_3 << FP_ESTIMATE_COLOR2 << FP_PRINT_TEST_PAGE << FP_ESTIMATE_SURFACE_COLOR << FP_PRINT_INTERSECT_TRIANGLE << FP_MESH_INTERSECT_TEST
		<< FP_TEST_AUTOPACK_V2 << FP_LANDING_BY_IMAGE_Z_AXIS << FP_LANDING_BY_IMAGE_X_AXIS << FP_LANDING_BY_IMAGE_Y_AXIS
		<< FP_GET_DEPTH_IMAGE << FP_GET_DEPTH_LANDING_TEST2/* << FP_GET_DEPTH_X_LANDING_TEST2 << FP_GET_DEPTH_Y_LANDING_TEST2*/ << FP_PACKING_VERSION_3 << FP_TEST_SEARCH_SPACE_ALL_IMAGE
		<< FP_TEST_SEARCH_SPACE_ALL_IMAGE2 << FP_TEST_GENERATE_POINTS_FROM_GRAY_IMAGE << FP_TEST_GENERATE_POINTS_FROM_GRAY_IMAGE_FLOAT << FP_MESH_INTERSECT_TEST2
		<< FP_MESH_DELAUNAY_TRIANGULATION << FP_COUNT_BINDER_AREA << FP_PRINT_FOUR_IN_ONE << FP_PRINT_DM_SLICER << FP_PRINT_DM_SLICER_ALPHA
		<< FP_PRINT_FLOW_2_V3 << FP_TEST_WITNESS_BAR << FP_SAVE_DEFAULT_SNAP_SHOT << FP_DM_ESTIMATE_COLOR << FP_TEST_BEZIER_CURVE;

	foreach(FilterIDType tt, types())
	{
		actionList << new QAction(filterName(tt), this);
		//if (tt == FP_GPU_EXAMPLE)  actionList.last()->setIcon(QIcon(":/myImage/generate_slice_img.jpg"));
		//if (tt == FP_GENERATE_MONO)  actionList.last()->setIcon(QIcon(":/myImage/generate_slice_img.jpg"));
	}
	//======
	//pd = new QProgressDialog("Task in progress.", "Cancel", 0, 100000);
	connect(this, SIGNAL(updatepd(int)), this, SLOT(setpd(int)), Qt::DirectConnection);//test useless

	//==============================
	equ[0] = 0;
	equ[1] = 0;
	equ[2] = -1;
	equ[3] = 0;
	equ2[0] = -1;
	equ2[1] = 0;
	equ2[2] = 0;
	equ2[3] = 0;
	ollX = DSP_grooveBigX;
	ollY = DSP_grooveY;
	ollZ = DSP_grooveZ;

	testtimer = new QTimer(this);
	connect(testtimer, SIGNAL(timeout()), this, SLOT(testprintf()));
	drawLists.resize(2, 0);

	//th1 = new QThread();
}

// ST() must return the very short string describing each filtering action
// (this string is used also to define the menu entry)
QString GenSliceJob::filterName(FilterIDType filterId) const
{
	switch (filterId) {
	case FP_GPU_EXAMPLE:  return QString("Generate_Slice_Pic");
	case FP_GENERATE_MONO: return QString("Generate_mono");
	case FP_PRINT_FLOW: return QString("Print_Flow");
	case FP_PRINT_FLOW_2:return QString("Print_Flow_2");
	case FP_PRINT_JR:return QString("FP_PRINT_JR");
	case FP_ESTIMATE_COLOR:return QString("FP_ESTIMATE_COLOR");
	case FP_ESTIMATE_SURFACE_COLOR:return QString("FP_ESTIMATE_SURFACE_COLOR");
	case FP_PRINT_FLOW_3:return QString("FP_PRINT_FLOW_3");
	case FP_ESTIMATE_COLOR2:return QString("FP_ESTIMATE_COLOR2");
	case FP_PRINT_TEST_PAGE:return QString("FP_PRINT_TEST_PAGE");
	case FP_PRINT_INTERSECT_TRIANGLE: return QString("FP_PRINT_INTERSECT_TRIANGLE");
	case FP_MESH_INTERSECT_TEST: return QString("FP_MESH_INTERSECT_TEST");
	case FP_TEST_AUTOPACK_V2: return QString("FP_TEST_AUTOPACK_V2");
	case FP_LANDING_BY_IMAGE_Z_AXIS: return QString("FP_LANDING_BY_IMAGE_Z_AXIS");
	case FP_LANDING_BY_IMAGE_X_AXIS: return QString("FP_LANDING_BY_IMAGE_X_AXIS");
	case FP_LANDING_BY_IMAGE_Y_AXIS: return QString("FP_LANDING_BY_IMAGE_Y_AXIS");
	case FP_GET_DEPTH_IMAGE: return QString("FP_GET_DEPTH_IMAGE");
	case FP_GET_DEPTH_LANDING_TEST2: return QString("FP_GET_DEPTH_LANDING_TEST2");
	case FP_PACKING_VERSION_3: return QString("FP_PACKING_VERSION_3");
	case FP_TEST_SEARCH_SPACE_ALL_IMAGE: return QString("FP_TEST_SEARCH_SPACE_ALL_IMAGE");
	case FP_TEST_SEARCH_SPACE_ALL_IMAGE2: return QString("FP_TEST_SEARCH_SPACE_ALL_IMAGE2");
	case FP_TEST_GENERATE_POINTS_FROM_GRAY_IMAGE: return QString("FP_TEST_GENERATE_POINTS_FROM_GRAY_IMAGE");
	case FP_TEST_GENERATE_POINTS_FROM_GRAY_IMAGE_FLOAT: return QString("FP_TEST_GENERATE_POINTS_FROM_GRAY_IMAGE_FLOAT");
	case FP_MESH_INTERSECT_TEST2: return QString("FP_MESH_INTERSECT_TEST2");
	case FP_MESH_DELAUNAY_TRIANGULATION: return QString("FP_MESH_DELAUNAY_TRIANGULATION");
	case FP_COUNT_BINDER_AREA: return QString("FP_COUNT_BINDER_AREA");
	case FP_PRINT_FOUR_IN_ONE: return QString("FP_PRINT_FOUR_IN_ONE");
	case FP_PRINT_DM_SLICER: return QString("FP_PRINT_DM_SLICER");
	case FP_PRINT_DM_SLICER_ALPHA: return QString("FP_PRINT_DM_SLICER_ALPHA");
	case FP_PRINT_FLOW_2_V3:return QString("FP_PRINT_FLOW_2_V3");;
	case FP_SAVE_DEFAULT_SNAP_SHOT:return QString("FP_SAVE_DEFAULT_SNAP_SHOT");
	case FP_TEST_WITNESS_BAR:return QString("FP_TEST_WITNESS_BAR");
	case FP_DM_ESTIMATE_COLOR:return QString("FP_DM_ESTIMATE_COLOR");
	case FP_TEST_BEZIER_CURVE:return QString("FP_TEST_BEZIER_CURVE");
	default: assert(0);
	}
	return QString();
}

// Info() must return the longer string describing each filtering action
// (this string is used in the About plugin dialog)
QString GenSliceJob::filterInfo(FilterIDType filterId) const
{
	switch (filterId) {
	case FP_GPU_EXAMPLE:  return QString("create color outlines and binder images(bmp,png,jpg)");
	case FP_GENERATE_MONO:  return QString("mono");
	case FP_PRINT_FLOW:return QString("print_flow");
	case FP_PRINT_FLOW_2:return QString("print_flow_2");
	case FP_PRINT_JR:return QString("FP_PRINT_JR");
	case FP_ESTIMATE_COLOR:return QString("Estimate_Color_Usage");
	case FP_ESTIMATE_SURFACE_COLOR:return QString("Estimate_Surface_Color_Usage");
	case FP_PRINT_FLOW_3:return QString("FP_PRINT_FLOW_3");
	case FP_ESTIMATE_COLOR2:return QString("FP_ESTIMATE_COLOR2");
	case FP_PRINT_TEST_PAGE:return QString("FP_PRINT_TEST_PAGE");
	case FP_PRINT_INTERSECT_TRIANGLE:return QString("FP_PRINT_INTERSECT_TRIANGLE");
	case FP_MESH_INTERSECT_TEST: return QString("FP_MESH_INTERSECT_TEST");
	case FP_TEST_AUTOPACK_V2: return QString("FP_TEST_AUTOPACK_V2");
	case FP_LANDING_BY_IMAGE_Z_AXIS: return QString("FP_LANDING_BY_IMAGE_Z_AXIS");
	case FP_LANDING_BY_IMAGE_X_AXIS: return QString("FP_LANDING_BY_IMAGE_X_AXIS");
	case FP_LANDING_BY_IMAGE_Y_AXIS: return QString("FP_LANDING_BY_IMAGE_Y_AXIS");
	case FP_GET_DEPTH_IMAGE: return QString("FP_GET_DEPTH_IMAGE");
	case FP_GET_DEPTH_LANDING_TEST2: return QString("FP_GET_DEPTH_LANDING_TEST2");
	case FP_PACKING_VERSION_3: return QString("FP_PACKING_VERSION_3");
	case FP_TEST_SEARCH_SPACE_ALL_IMAGE: return QString("FP_TEST_SEARCH_SPACE_ALL_IMAGE");
	case FP_TEST_SEARCH_SPACE_ALL_IMAGE2: return QString("FP_TEST_SEARCH_SPACE_ALL_IMAGE2");
	case FP_TEST_GENERATE_POINTS_FROM_GRAY_IMAGE: return QString("FP_TEST_GENERATE_POINTS_FROM_GRAY_IMAGE");
	case FP_TEST_GENERATE_POINTS_FROM_GRAY_IMAGE_FLOAT: return QString("FP_TEST_GENERATE_POINTS_FROM_GRAY_IMAGE_FLOAT");
	case FP_MESH_INTERSECT_TEST2: return QString("FP_MESH_INTERSECT_TEST2");
	case FP_MESH_DELAUNAY_TRIANGULATION: return QString("FP_MESH_DELAUNAY_TRIANGULATION");
	case FP_COUNT_BINDER_AREA: return QString("FP_COUNT_BINDER_AREA");
	case FP_PRINT_FOUR_IN_ONE: return QString("FP_PRINT_FOUR_IN_ONE");
	case FP_PRINT_DM_SLICER: return QString("FP_PRINT_DM_SLICER");
	case FP_PRINT_DM_SLICER_ALPHA: return QString("FP_PRINT_DM_SLICER_ALPHA");
	case FP_PRINT_FLOW_2_V3:return QString("FP_PRINT_FLOW_2_V3");;
	case FP_SAVE_DEFAULT_SNAP_SHOT:return QString("FP_SAVE_DEFAULT_SNAP_SHOT");;
	case FP_TEST_WITNESS_BAR:return QString("FP_TEST_WITNESS_BAR");
	case FP_DM_ESTIMATE_COLOR:return QString("FP_DM_ESTIMATE_COLOR"); 
	case FP_TEST_BEZIER_CURVE:return QString("FP_TEST_BEZIER_CURVE");
	default: assert(0);
	}
	return QString("Unknown Filter");
}

// The FilterClass describes in which generic class of filters it fits.
// This choice affect the submenu in which each filter will be placed
// More than a single class can be choosen.
GenSliceJob::FilterClass GenSliceJob::getClass(QAction *a)
{
	switch (ID(a))
	{
	case FP_GPU_EXAMPLE:  return MeshFilterInterface::Texture;
	case FP_GENERATE_MONO:  return MeshFilterInterface::Texture;
	case FP_PRINT_FLOW: return MeshFilterInterface::Texture;
	case FP_PRINT_FLOW_2: return MeshFilterInterface::Texture;
	case FP_PRINT_FLOW_3: return MeshFilterInterface::Texture;
	case FP_PRINT_JR: return MeshFilterInterface::Texture;
	case FP_ESTIMATE_COLOR:return MeshFilterInterface::Texture;
	case FP_ESTIMATE_COLOR2:return MeshFilterInterface::Texture;
	case FP_ESTIMATE_SURFACE_COLOR:return MeshFilterInterface::Texture;
	case FP_PRINT_TEST_PAGE:return MeshFilterInterface::Generic;
	case FP_PRINT_INTERSECT_TRIANGLE:return MeshFilterInterface::Generic;
	case FP_MESH_INTERSECT_TEST: return MeshFilterInterface::MovePos;
	case FP_TEST_AUTOPACK_V2: return  MeshFilterInterface::MovePos;
	case FP_LANDING_BY_IMAGE_Z_AXIS: return MeshFilterInterface::MovePos;
	case FP_LANDING_BY_IMAGE_X_AXIS: return MeshFilterInterface::MovePos;
	case FP_LANDING_BY_IMAGE_Y_AXIS: return MeshFilterInterface::MovePos;
	case FP_GET_DEPTH_IMAGE: return  MeshFilterInterface::Generic;
	case FP_GET_DEPTH_LANDING_TEST2: return  MeshFilterInterface::MovePos;
	case FP_PACKING_VERSION_3: return  MeshFilterInterface::MovePos;
	case FP_TEST_SEARCH_SPACE_ALL_IMAGE: return  MeshFilterInterface::MovePos;
	case FP_TEST_SEARCH_SPACE_ALL_IMAGE2: return  MeshFilterInterface::MovePos;
	case FP_TEST_GENERATE_POINTS_FROM_GRAY_IMAGE:	return  MeshFilterInterface::MeshCreation;
	case FP_TEST_GENERATE_POINTS_FROM_GRAY_IMAGE_FLOAT: return  MeshFilterInterface::MeshCreation;
	case FP_MESH_INTERSECT_TEST2: return MeshFilterInterface::Generic;
	case FP_MESH_DELAUNAY_TRIANGULATION:  return  MeshFilterInterface::MeshCreation;
	case FP_COUNT_BINDER_AREA: return MeshFilterInterface::Generic;
	case FP_PRINT_FOUR_IN_ONE: return MeshFilterInterface::Generic;
	case FP_PRINT_DM_SLICER: return MeshFilterInterface::Generic;
	case FP_PRINT_DM_SLICER_ALPHA: return MeshFilterInterface::Generic;
	case FP_PRINT_FLOW_2_V3:return MeshFilterInterface::Generic;
	case FP_SAVE_DEFAULT_SNAP_SHOT:return MeshFilterInterface::Generic;
	case FP_TEST_WITNESS_BAR:return MeshFilterInterface::MeshCreation;;
	case FP_DM_ESTIMATE_COLOR:return MeshFilterInterface::Generic;
	case FP_TEST_BEZIER_CURVE:return MeshFilterInterface::Generic;;



	default: assert(0);
	}
	return MeshFilterInterface::Generic;
}

// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parameter you need to define,
// - the name of the parameter,
// - the string shown in the dialog
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
void GenSliceJob::initParameterSet(QAction * action, MeshDocument &md, RichParameterSet & parlst)
{


	switch (ID(action))
	{
#pragma region FP_GPU_EXAMPLE
	case FP_GPU_EXAMPLE:
	{
		//parlst.addParam(new RichColor    ("ImageBackgroundColor", QColor(255, 50, 50),                 "Image Background Color", "The color used as image background."        ));
		float x = md.groove.DimX();
		float y = md.groove.DimY();
		qDebug() << "md.groove.DimX()" << x << y;
		float dpi_x = x / DSP_inchmm * 200;///dpi是以英吋為單位
		float dpi_y = dpi_x * y / x;
		qDebug() << "md.groove.DimX()" << dpi_x << dpi_y;

		//parlst.addParam(new RichInt("ImageWidth", int(dpi_x), "Image Width", "The width in pixels of the produced image."));
		//parlst.addParam(new RichInt("ImageHeight", int(dpi_y), "Image Height", "The height in pixels of the produced image."));
		parlst.addParam(new RichInt("print_dpi", 300, "dpi", ""));
		//parlst.addParam(new RichSaveFile ("ImageFileName",        "gpu_generated_image.png", "*.png", "Image File Name",        "The file name used to save the image."      ));
		//parlst.addParam(new RichSaveFile( "CapImageFileName", "cap_edge.png", "*.png", "Cap Outline Image File Name", "The file name used to save the image."));						   						   
		//QStringList sliceheight = QStringList() << "5mm"<<"0.1mm" << "0.07mm";
		//parlst.addParam(new RichEnum("SliceHeight", 0, sliceheight, tr("sliceHeight"), tr("")));


		parlst.addParam(new RichFloat("slice_height", 0.5, "height_cm", ""));

		parlst.addParam(new RichBool("OL_Image", false, "save_Outline_Image", ""));
		parlst.addParam(new RichBool("Cap_Image", false, "save_cap_Image", ""));
		parlst.addParam(new RichBool("OL_CAP_Image", false, "Combine", ""));
		parlst.addParam(new RichBool("BLK_OUT_LINE", false, "BLK_OUT_LINE", ""));
		parlst.addParam(new RichBool("FaceColor", false, "FaceColor", ""));

		parlst.addParam(new RichFloat("useless_print", 1, "useless_print_des", ""));//犧牲條寬度
		parlst.addParam(new RichBool("generate_zx", 0, "generate_zx_or_not", ""));//
		parlst.addParam(new RichBool("generate_final_picture", 0, "generate_final_picture", ""));//
		parlst.addParam(new RichBool("mono_bool", 0, "mono_bool", ""));//***20160322mono_bool

		parlst.addParam(new RichInt("per_zx_pages", 10, "number of zx pages", ""));//
		parlst.addParam(new RichInt("start_page", 0, "start print pages"));

		int boxy_dim = qCeil((md.bbox().max.Z() - md.bbox().min.Z()) / 0.005) + 1;
		parlst.addParam(new RichInt("end_page", boxy_dim, "end print pages"));
		parlst.addParam(new RichInt("send_page_num", 10, "send_page_num"));

		break;
	}
#pragma endregion FP_GPU_EXAMPLE
#pragma region FP_GENERATE_MONO
	case FP_GENERATE_MONO:
	{
		//parlst.addParam(new RichColor    ("ImageBackgroundColor", QColor(255, 50, 50),                 "Image Background Color", "The color used as image background."        ));
		float x = md.groove.DimX();
		float y = md.groove.DimY();
		qDebug() << "md.groove.DimX()" << x << y;
		float dpi_x = x / DSP_inchmm * 200;///dpi是以英吋為單位
		float dpi_y = dpi_x * y / x;
		qDebug() << "md.groove.DimX()" << dpi_x << dpi_y;

		//parlst.addParam(new RichInt("ImageWidth", int(dpi_x), "Image Width", "The width in pixels of the produced image."));
		//parlst.addParam(new RichInt("ImageHeight", int(dpi_y), "Image Height", "The height in pixels of the produced image."));
		parlst.addParam(new RichInt("print_dpi", 300, "dpi", ""));
		//parlst.addParam(new RichSaveFile ("ImageFileName",        "gpu_generated_image.png", "*.png", "Image File Name",        "The file name used to save the image."      ));
		//parlst.addParam(new RichSaveFile( "CapImageFileName", "cap_edge.png", "*.png", "Cap Outline Image File Name", "The file name used to save the image."));						   						   
		//QStringList sliceheight = QStringList() << "5mm"<<"0.1mm" << "0.07mm";
		//parlst.addParam(new RichEnum("SliceHeight", 0, sliceheight, tr("sliceHeight"), tr("")));


		parlst.addParam(new RichFloat("slice_height", 0.5, "height_cm", ""));

		parlst.addParam(new RichBool("OL_Image", false, "save_Outline_Image", ""));
		parlst.addParam(new RichBool("Cap_Image", false, "save_cap_Image", ""));
		parlst.addParam(new RichBool("OL_CAP_Image", false, "Combine", ""));
		parlst.addParam(new RichBool("FaceColor", false, "FaceColor", ""));
		parlst.addParam(new RichBool("BLK_OUT_LINE", false, "BLK_OUT_LINE", ""));



		parlst.addParam(new RichFloat("useless_print", 1, "useless_print_des", ""));//犧牲條寬度
		parlst.addParam(new RichBool("generate_zx", 0, "generate_zx_or_not", ""));//
		parlst.addParam(new RichBool("generate_final_picture", 0, "generate_final_picture", ""));//

		parlst.addParam(new RichInt("per_zx_pages", 10, "number of zx pages", ""));//
		parlst.addParam(new RichInt("start_page", 0, "start print pages"));

		int boxy_dim = qCeil((md.bbox().max.Z() - md.bbox().min.Z()) / 0.005) + 1;
		parlst.addParam(new RichInt("end_page", boxy_dim, "end print pages"));
		parlst.addParam(new RichInt("send_page_num", 10, "send_page_num"));
		break;
	}
#pragma endregion FP_GENERATE_MONO
	default: break;
		//assert(0);
	}
}
void GenSliceJob::init_texture(MeshDocument & md)
{
	/*QImage img;
	bool res = img.load(":/myImage/dummy.png");
	qDebug() << "res=" << res;
	if (res)
	{
	qDebug() << "res==true";
	glGenTextures(1, &texName);
	glBindTexture(GL_TEXTURE_2D, texName);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
	GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
	GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(),
	img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
	img.bits());
	}*/

	if (true) // ALL the texture must to be re-loaded, so we clear all the TMId vectors
	{
		foreach(MeshModel *mp, md.meshList)
		{
			if (!mp->glw.TMId.empty())
			{
				glDeleteTextures(1, &(mp->glw.TMId[0]));
				mp->glw.TMId.clear();
			}
		}
		foreach(MeshModel *mp, md.meshList)
		{

			if (!mp->glw.TMIdd[2].empty())
			{
				mp->glw.TMIdd[2].clear();
			}

		}
	}
	size_t totalTextureNum = 0, toBeUpdatedNum = 0;
	foreach(MeshModel *mp, md.meshList)//***計算所有mesh材質的數量
	{
		totalTextureNum += mp->cm.textures.size();
		if (!mp->cm.textures.empty() && mp->glw.TMId.empty()) toBeUpdatedNum++;
	}
	qDebug() << "totalTextureNum" << totalTextureNum;
	if (toBeUpdatedNum == 0) return;
	int singleMaxTextureSizeMpx = 85;
	int singleMaxTextureSize = 8192;
	qDebug() << "plaa1";
	glEnable(GL_TEXTURE_2D);
	GLint MaxTextureSize;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &MaxTextureSize);
	qDebug() << "MaxTextureSize" << MaxTextureSize;
	if (singleMaxTextureSize < MaxTextureSize)
	{
		//qDebug(0, "There are too many textures (%i), reducing max texture size from %i to %i", totalTextureNum, MaxTextureSize, singleMaxTextureSize);
		qDebug() << "plaa2";
		MaxTextureSize = singleMaxTextureSize;
	}

	foreach(MeshModel *mp, md.meshList)
	{
		if (!mp->cm.textures.empty() && mp->glw.TMId.empty())
		{
			QString unexistingtext = "In mesh file <i>" + mp->fullName() + "</i> : Failure loading textures:<br>";
			bool sometextfailed = false;
			for (unsigned int i = 0; i < mp->cm.textures.size(); ++i)
			{
				QImage img, imgScaled, imgGL;
				//***20150508 將匯入材質路徑設為mesh所在資料夾位置，直接讀材質路徑的話會錯。
				QFileInfo fi(mp->fullName());
				QDir fid = fi.dir();
				QString texpath;
				QFileInfo texfileinfo(mp->cm.textures[i].c_str());
				if (texfileinfo.isRelative())
					texpath = fid.absolutePath() + "/" + mp->cm.textures[i].c_str();
				else texpath = mp->cm.textures[i].c_str();

				bool res = img.load(texpath);
				img = img.mirrored();
				//qDebug() << "absoluteFilePath" << fi.path();
				//qDebug() << "mp->cm.textures[i].c_str()" << mp->cm.textures[i].c_str();

				sometextfailed = sometextfailed || !res;
				//qDebug() << "res=" << res;
				/*if (!res)
				{
				// Note that sometimes (in collada) the texture names could have been encoded with a url-like style (e.g. replacing spaces with '%20') so making some other attempt could be harmless
				QString ConvertedName = QString(mp->cm.textures[i].c_str()).replace(QString("%20"), QString(" "));
				res = img.load(ConvertedName);
				if (!res)
				{
				qDebug(0, "Failure of loading texture %s", mp->cm.textures[i].c_str());
				unexistingtext += "<font color=red>" + QString(mp->cm.textures[i].c_str()) + "</font><br>";
				}
				else
				qDebug(0, "Warning, texture loading was successful only after replacing %%20 with spaces;\n Loaded texture %s instead of %s", qPrintable(ConvertedName), mp->cm.textures[i].c_str());

				}*/

				if (!res)//***為什麼有錯誤
					res = img.load(":/images/dummy.png");
				if (res)
				{
					// image has to be scaled to a 2^n size. We choose the first 2^N >= picture size.

					int bestW = RoundUpToTheNextHighestPowerOf2(img.width());
					int bestH = RoundUpToTheNextHighestPowerOf2(img.height());
					while (bestW >= MaxTextureSize) bestW /= 2;
					while (bestH >= MaxTextureSize) bestH /= 2;


					imgScaled = img.scaled(bestW, bestH, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
					img = imgScaled;

					//img = img.mirrored(true, false);
					//imgGL = convertToGLFormat(imgScaled);
					mp->glw.TMId.push_back(0);//***先給一個值					
					//****20150402這邊要如何修改***不用改					
					glGenTextures(1, (GLuint*)&(mp->glw.TMId.back()));//***把值拿來用，0會被改調
					glBindTexture(GL_TEXTURE_2D, mp->glw.TMId.back());
					//qDebug() <<"back2"<< mp->glw.TMId.back();
					//qDebug("  	will be loaded as GL texture id %i  ( %i x %i )", mp->glw.TMId.back(), img.width(), img.height());
					//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, img.bits());

					int x = img.format();
					if (x <3 )
					{
						QImage convertImg = img.convertToFormat(QImage::Format_RGB32);
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, convertImg.width(), convertImg.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, convertImg.bits());
					}
					else glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, img.bits());
					//gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.width(), img.height(), GL_RGBA, GL_UNSIGNED_BYTE, img.bits());


				}
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
					GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
					GL_LINEAR);

				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			}
			mp->glw.viewerNum = 2;//設定viewer是哪一個
			std::vector<unsigned int> copymp(mp->glw.TMId);//複製TMId vector
			//qDebug() << "mp->glw.viewerNum;" << mp->glw.viewerNum;
			mp->glw.TMIdd[2] = copymp;
			//for (unsigned i = 0; i < copymp.size(); i++)
			//qDebug() << "TMIdd[2]" << mp->glw.TMIdd[2].at(i);



			//if (sometextfailed)
			//QMessageBox::warning(this, "Texture files has not been correctly loaded", unexistingtext);

		}
	}
	glDisable(GL_TEXTURE_2D);

}

// The Real Core Function doing the actual mesh processing.
// Move Vertex of a random quantity
bool GenSliceJob::applyFilter(QAction * a, MeshDocument & md, RichParameterSet & par, vcg::CallBackPos * cb, GLArea *gla, vcg::SendBackData *sb)
{
	bool switchInitMeshVBO = false;

	switch (ID(a))
	{

#pragma region FP_GPU_EXAMPLE
	case FP_GPU_EXAMPLE:
	{
		//qDebug() <<"deplace1" ;

		CMeshO & mesh = md.mm()->cm;
		if ((mesh.vn < 3) || (mesh.fn < 1)) return false;

		const unsigned char * p0 = (const unsigned char *)(&(mesh.vert[0].P()));
		const unsigned char * p1 = (const unsigned char *)(&(mesh.vert[1].P()));
		const void *          pbase = p0;
		GLsizei               pstride = GLsizei(p1 - p0);

		const unsigned char * n0 = (const unsigned char *)(&(mesh.vert[0].N()));
		const unsigned char * n1 = (const unsigned char *)(&(mesh.vert[1].N()));
		const void *          nbase = n0;
		GLsizei               nstride = GLsizei(n1 - n0);

		glContext->makeCurrent();
		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			/* Problem: glewInit failed, something is seriously wrong. */
			//fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
			//qDebug("Error: %s\n", glewGetErrorString(err));
		}

		glPushAttrib(GL_ALL_ATTRIB_BITS);

		Context ctx;
		ctx.acquire();
		if (switchInitMeshVBO)
			initMeshVBO(md);

		const GLsizeiptr psize = GLsizeiptr(GLsizei(mesh.vn) * pstride);
		BufferHandle hPositionBuffer = createBuffer(ctx, psize, pbase);

		const GLsizeiptr nsize = GLsizeiptr(GLsizei(mesh.vn) * nstride);
		BufferHandle hNormalBuffer = createBuffer(ctx, nsize, nbase);

		const GLsizeiptr isize = GLsizeiptr(mesh.fn * 3 * sizeof(GLuint));
		BufferHandle hIndexBuffer = createBuffer(ctx, isize);
		{
			BoundIndexBufferHandle indexBuffer = ctx.bindIndexBuffer(hIndexBuffer);

			const CMeshO::VertexType * vbase = &(mesh.vert[0]);
			GLuint *  indices = (GLuint *)indexBuffer->map(GL_WRITE_ONLY);
			for (size_t i = 0; i < mesh.face.size(); ++i)
			{
				const CMeshO::FaceType & f = mesh.face[i];
				if (f.IsD()) continue;
				for (int v = 0; v < 3; ++v)
				{
					*indices++ = GLuint(vcg::tri::Index(mesh, f.cV(v)));
				}
			}
			indexBuffer->unmap();

			ctx.unbindIndexBuffer();
		}
		///***設定儲存image的長寬
		/* const GLsizei width = GLsizei(par.getInt("ImageWidth"));
		const GLsizei height = GLsizei(par.getInt("ImageHeight"));*/

		//***20150907
		const GLsizei print_dpi = GLsizei(par.getInt("print_dpi"));
		float x = md.groove.DimX();
		float y = md.groove.DimY();
		const GLsizei width = (int)(x / DSP_inchmm * print_dpi);
		const GLsizei height = (int)(width * y / x);
		qDebug() << "width,height" << width << height;
		//const GLsizei height = (int)y / DSP_inchmm * print_dpi;
		//par.setValue("ImageWidth", IntValue(width));
		//par.setValue("ImageHeight", IntValue(height));

		//*******

		//qDebug() << "width" << "height" << width << height;
		//RenderbufferHandle hDepth = createRenderbuffer(ctx, GL_DEPTH_COMPONENT24, width, height);
		RenderbufferHandle hDepth = createRenderbuffer(ctx, GL_DEPTH24_STENCIL8, width, height);
		Texture2DHandle    hColor = createTexture2D(ctx, GL_RGB8, width, height, GL_RGB, GL_UNSIGNED_BYTE);
		//FramebufferHandle  hFramebuffer = createFramebuffer(ctx, renderbufferTarget(hDepth), texture2DTarget(hColor));
		FramebufferHandle  hFramebuffer = createFramebufferWithDepthStencil(ctx, renderbufferTarget(hDepth), renderbufferTarget(hDepth), texture2DTarget(hColor));



		//const QColor       backgroundColor  = par.getColor("ImageBackgroundColor");
		const vcg::Point3f lightDirectionVS = vcg::Point3f(0.0f, 0.0f, -1.0f).Normalize();

		glEnable(GL_DEPTH_TEST);
		//glClearColor(GLfloat(backgroundColor.red())/255.0f, GLfloat(backgroundColor.green())/255.0f, GLfloat(backgroundColor.blue())/255.0f, 0.0f);
		Construction_Groove<float> tempgroove = md.groove;

		glViewport(0, 0, width, height);//***window的長寬
		//glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
		//glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -100, 100);
		//glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, 0, 0, 1, 0);

		const vcg::Point3f center = mesh.bbox.Center();
		const float        scale = 1.0f / mesh.bbox.Diag();

		//glScalef(scale, scale, scale);
		//glTranslatef(-center[0], -center[1], -center[2]);

		//***儲存image的基本資訊
		QImage image(int(width), int(height), QImage::Format_RGB888);
		QImage cap_image(int(width), int(height), QImage::Format_RGB888);

		//***20150505***slice_func_設置高度
		float box_bottom = md.bbox().min.Z();
		float unit = par.getFloat("slice_height");

		/*float unit;
		switch (par.getEnum("SliceHeight"))
		{
		case 0:
		unit = 0.5;
		break;
		case 1:
		unit = 0.01;
		break;
		case 2:
		unit = 0.007;
		break;
		default:
		break;
		}*/

		int boxy_dim = qCeil((md.bbox().max.Z() - md.bbox().min.Z()) / unit) + 1;
		init_texture(md);

		//*******************						  
		qDebug() << "sample_filter_place1";

		//***20150507 standard path******************
		//QString temppath = QStandardPaths::locate(QStandardPaths::TempLocation, "", QStandardPaths::LocateDirectory);
		//QString temppath = "D:/temptemp";//backup
		QString temppath = getRoamingDir();
		QDir dir(temppath);//backup
		if (dir.exists("xyzimage"))
		{
			dir.setPath(temppath + "/xyzimage");
			dir.removeRecursively();
			dir.setPath(temppath);
			dir.mkpath("xyzimage");
		}
		else
		{
			dir.setPath(temppath);
			dir.mkpath("xyzimage");
		}

		dir.setPath(temppath + "/xyzimage");

		//---zx_file------------------------------------------
		QString zxtemp = "D:/temptemp";
		QDir zx_dir(zxtemp);
		if (zx_dir.exists("zx_file"))
		{
			zx_dir.setPath(zxtemp + "/zx_file");
			//zx_dir.removeRecursively();
			zx_dir.setPath(zxtemp);
			zx_dir.mkpath("zx_file");
		}
		else
		{
			dir.setPath(temppath);
			dir.mkpath("zx_file");
		}

		zx_dir.setPath(zxtemp + "/zx_file");

		//qDebug() <<"TempLocation" <<dir.absolutePath();
		//=============================						   
		// progress = new QProgressDialog("Task in progress...", "Cancel", 0, boxy_dim,a->parentWidget());
		//connect(this,SIGNAL(update(int)),this,SLOT(updatepd(int)));
		//progress->setWindowModality(Qt::WindowModal);
		//progress->setWindowModality(Qt::NonModal);
		//progress->setModal(false);						  
		//progress->show();						   
		//==============================
		//**********************
		ctx.bindReadDrawFramebuffer(hFramebuffer);
		GLW_CHECK_GL_READ_DRAW_FRAMEBUFFER_STATUS;
#if 1

		const bool outputOL_Image = par.getBool("OL_Image");
		const bool save_cap_Image = par.getBool("Cap_Image");
		const bool ol_and_cap = par.getBool("OL_CAP_Image");
		const bool faceColor = par.getBool("FaceColor");
		glClearColor(1, 1, 1, 1);
		QString olMName = md.p_setting.getOlMeshName();
		QString cMName = md.p_setting.getCapMeshName();

		//***opencvMat clear
		capCTemp.clear();
		outlineCTemp.clear();

		/*glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
		glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -100, 100);
		glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, 0, 0, 1, 0);						   */


		int zx_pages = par.getInt("per_zx_pages");
		int start_page = par.getInt("start_page");
		int send_page_num = par.getInt("send_page_num");
		//QString zx_file_name = zx_dir.absolutePath() + "/";//option 1
		//string zx_sn = zx_file_name.toUtf8().constData();
		//---option 2
		//QString zx_file_name = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/";//---20151228
		QString zx_file_name = getRoamingDir();

		/*QDateTime timefilename = QDateTime::currentDateTime();
		zx_file_name.append(timefilename.toString(QString("hh:mm:ss.dd.MM.yyyy")));
		zx_file_name.append(".zx");*/

		string zx_sn = zx_file_name.toUtf8().constData();
		//zx_sn += makeDefaultZxName();//***20151224**backup
		zx_sn += "output.zx";
		qDebug() << "sample_filtergpuzx_sn" << zx_file_name;
		ZxGen zGG(unsigned int(width), unsigned int(height), boxy_dim, md.groove.DimX(), 21, zx_sn, false);//20161219_backup
		//ZxGen zGG(unsigned int(width), unsigned int(height)+500, boxy_dim, md.groove.DimX(), zx_sn);
		qDebug() << "sample_filtergpu" << md.groove.DimX();
		//for (int i = 0, j = 1; i < boxy_dim; i++, j++)
		//boxy_dim+1因為讓最後一層顏色多一層
		//FILE *dbgff;
		//dbgff = fopen("D:\\debug_open_1.txt", "w");//@@@

		cv::Mat look_down_color_CV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat look_down_Black_Mask_layer(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat look_down_Black_MaskCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat look_up_Black_MaskCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat look_up_color_CV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat captemp(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat capDownMask(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat black_Mask(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat black_Mask2(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat outlineBlackMaskCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat lookDownBlackTriangleMask(height, width, CV_8UC3, Scalar(255, 255, 255));

		cv::Mat prePrintobjectC(height, width, CV_8UC3, Scalar(255, 255, 255));

		cv::Mat downColorFirstCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat downBlackMaskFirstCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat downColorSecondCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat downBlackMaskSecondCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat prePrintobjectB(height, width, CV_8UC1, Scalar(255));

		cv::Mat capDownMask_bottom_face(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat capDownMask_top_face(height, width, CV_8UC3, Scalar(255, 255, 255));


		cv::Mat color_Bottom_face(height, width, CV_8UC3, Scalar(255, 255, 255));



		cv::Mat color_Top_face(height, width, CV_8UC3, Scalar(255, 255, 255));



		cv::Mat originalOutline(height, width, CV_8UC3, Scalar(255, 255, 255));


		cv::Mat outlineColorCv(height, width, CV_8UC3, Scalar(255, 255, 255));


		cv::Mat capDownSureMask(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat capTopSureMask(height, width, CV_8UC3, Scalar(255, 255, 255));


		if (cb != NULL)		(*cb)(0, "processing...");
		glContext->makeCurrent();
		for (int i = start_page, j = start_page + 1; i < boxy_dim; i++, j++)
			//for (int i = 0; i < 1; i++)
		{
			if (cb && (i % 2) == 0)
				cb((i * 100) / boxy_dim, "slice process");
			glContext->makeCurrent();
			QTime time;
			time.start();
			//xyz::slice_roution(md, box_bottom + unit*i, faceColor);//***backup******************切層演算							  
			xyz::slice_roution(md, box_bottom + unit*i, true);//****20160301*****************切層演算							  
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);

			//vcg::glColor(vcg::Color4b::LightGray);
			//_buffPass->enable();

			//***第一張outline圖*****************************
			QVector<MeshModel *> vmm;
			md.getMeshByContainString("_temp_outlines", vmm);
			md.getMeshByContainString(olMName, vmm);
			QVector<MeshModel *> cmm;
			md.getMeshByContainString(cMName, cmm);//capImage
			int xx = 0;

			QString img_file_name = md.p_setting.getoutlineName();


			if (!par.getBool("mono_bool"))
			{
				foreach(MeshModel *mdmm, md.meshList)
				{
					/*if (vcg::tri::HasPerWedgeTexCoord(mdmm->cm))
					{
					mdmm->render(GLW::DMFlat, GLW::CMNone, GLW::TMPerWedgeMulti);
					}*/
					//mdmm->render(GLW::DMSmooth, GLW::CMNone, GLW::TMPerWedgeMulti);//backup
					//mdmm->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
					//mdmm->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);//backup
					//mdmm->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMPerWedgeMulti);
					//glEnable(GL_LIGHTING);
					//****20151231********************************************************************
					foreach(MeshModel *mp, vmm)//畫輪廓圖。
					{
						mp->glw.viewerNum = 2;
						if (vmm.size() > 0)
						{
							//qDebug() << "picrender";
							//qDebug("meshname" + (*mp).label().toLatin1());
							//mp->render(vcg::GLW::DMSmooth, vcg::GLW::CMPerVert, vcg::GLW::TMPerWedgeMulti);						
							//mp->render(vcg::GLW::DMTexWire, vcg::GLW::CMPerVert, vcg::GLW::TMNone);//backup
							mp->render(vcg::GLW::DMTexWire, mp->rmm.colorMode, mp->rmm.textureMode);//backup
							//mp->render(vcg::GLW::DMTexWire, vcg::GLW::CMBlack, mp->rmm.textureMode);//backup

							//mp->render(vcg::GLW::DMOutline_Test, vcg::GLW::CMNone, vcg::GLW::TMNone);
						}
					}

					float cut_z = box_bottom + unit*i;
					if (mdmm->rmm.colorMode != GLW::CMNone || mdmm->rmm.textureMode != GLW::TMNone)//如果無色就不須做
						if (cut_z - mdmm->cm.bbox.min.Z() < 0 || cut_z - mdmm->cm.bbox.max.Z() > 0)//在物體box的上下需要設定限制，才不會超出物體大小。
						{
							glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
							glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., 0.0001, 0);
							glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, box_bottom + unit*i, 0, 0, -30, 0, 1, 0);

							glPushMatrix();
							if (vcg::tri::HasPerWedgeTexCoord(mdmm->cm) && !mdmm->cm.textures.empty())
								mdmm->render(GLW::DMSmooth, GLW::CMNone, GLW::TMPerWedgeMulti);
							else if (vcg::tri::HasPerFaceColor(mdmm->cm))
								mdmm->render(GLW::DMFlat, GLW::CMPerFace, GLW::TMNone);
							else if (vcg::tri::HasPerVertexColor(mdmm->cm))
								mdmm->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
							glPopMatrix();

							/*glMatrixMode(GL_PROJECTION);
							glPopMatrix();
							glMatrixMode(GL_MODELVIEW);
							glPopMatrix();*/

						}
					/*else if (cut_z - mdmm->cm.bbox.min.Z() < 0)
					{
					}*/
						else//物體梯田情況會有0.1層顏色的厚度。
						{
							glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
							//glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., 0.01, 0);//backup
							//glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., 0.005, -0.005);
							glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -0.05, 0.05);
							//***20161112
							glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, box_bottom + unit*i, 0, 0, -30, 0, 1, 0);

							glPushMatrix();
							if (vcg::tri::HasPerWedgeTexCoord(mdmm->cm) && !mdmm->cm.textures.empty())
								mdmm->render(GLW::DMSmooth, GLW::CMNone, GLW::TMPerWedgeMulti);
							else if (vcg::tri::HasPerFaceColor(mdmm->cm))
								mdmm->render(GLW::DMFlat, GLW::CMPerFace, GLW::TMNone);
							else if (vcg::tri::HasPerVertexColor(mdmm->cm))
								mdmm->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
							glPopMatrix();
						}

				}
				//
				glMatrixMode(GL_PROJECTION);
				glPopMatrix();
				glMatrixMode(GL_MODELVIEW);
				glPopMatrix();

				glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
				glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -100, 100);
				glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, 0, 0, 1, 0);
				cmystrips(md, par.getFloat("useless_print") / 3.);


				//***20160113
				//foreach(MeshModel *mp, cmm)//binder圖
				//{
				// mp->render(vcg::GLW::DMCapEdge, vcg::GLW::CMBlack, vcg::GLW::TMNone);
				//}
				//******************************************************



				//foreach(MeshModel *mp, cmm)//扣掉白色的輪廓20160301
				//{

				// mp->render(vcg::GLW::DMCapEdge, vcg::GLW::CMTestSig, vcg::GLW::TMNone);
				//}


				//***第一張圖片編號
				//***20150921***增加bool box

				//***20150508***BMP QImage設為Format_RGBA8888, readpixel要設為GL_RGBA
				glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image.bits());

				if (par.getBool("generate_zx"))
				{
					originalOutline = QImageToCvMat(image.mirrored());
					//outlineCTemp.push_back(QImageToCvMat(image.mirrored())); //zxzxzxzxzxzxzxz
				}
				if (outputOL_Image)//彩色輪廓
				{
					img_file_name.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
					image.mirrored().save(dir.absolutePath() + "/" + img_file_name, "png");
				}

			}
			//***20160122***產生黑色輪廓編號_mask
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);
			//********************************
			foreach(MeshModel *mp, vmm)//outline
			{
				mp->glw.viewerNum = 2;
				if (vmm.size() > 0)
				{
					//mp->render(vcg::GLW::DMTexWire, mp->rmm.colorMode, mp->rmm.textureMode);//backup
					mp->render(vcg::GLW::DMTexWire, vcg::GLW::CMBlack, vcg::GLW::TMNone);//backup

					//mp->render(vcg::GLW::DMOutline_Test, vcg::GLW::CMNone, vcg::GLW::TMNone);
				}
			}

			foreach(MeshModel *mdmm, md.meshList)
			{
				glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
				glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -0.05, 0.05);
				//***20161112
				glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, -11, 0, 0, -30, 0, 1, 0);

				glPushMatrix();
				mdmm->render(GLW::DMFlat, GLW::CMBlack, GLW::TMNone);
				glPopMatrix();

				glMatrixMode(GL_PROJECTION);
				glPopMatrix();
				glMatrixMode(GL_MODELVIEW);
				glPopMatrix();

			}
			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -100, 100);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, 0, 0, 1, 0);
			//*********************************
			binderstrips(md, par.getFloat("useless_print"));
			//image.fill(QColor::w);

			//foreach(MeshModel *mp, cmm)//扣掉白色的輪廓
			//{

			// mp->render(vcg::GLW::DMCapEdge, vcg::GLW::CMTestSig, vcg::GLW::TMNone);
			//}

			foreach(MeshModel *mdmm, md.meshList)
			{
				//****20151231********************************************************************
				float cut_z = box_bottom + unit*i;
				if (cut_z - mdmm->cm.bbox.min.Z() < 0 || cut_z - mdmm->cm.bbox.max.Z() > 0)//在物體box的上下需要設定限制，才不會超出物體大小。
				{
					glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
					glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., 0.0001, 0);
					glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, box_bottom + unit*i, 0, 0, -30, 0, 1, 0);

					mdmm->render(GLW::DMFlat, GLW::CMBlack, GLW::TMNone);
					glPopMatrix();

				}
				else//物體梯田情況會有0.1層顏色的厚度。
				{
					glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
					//glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., 0.01, 0);//backup
					//glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., 0.005, -0.005);
					glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -0.05, 0.05);
					//***20161112
					glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, box_bottom + unit*i, 0, 0, -30, 0, 1, 0);

					glPushMatrix();
					mdmm->render(GLW::DMFlat, GLW::CMBlack, GLW::TMNone);
					glPopMatrix();
				}

			}
			//
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();




			QString blk_img_file_name = "black_outline";
			glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image.bits());
			black_Mask = QImageToCvMat(image.mirrored());
			if (par.getBool("BLK_OUT_LINE"))
			{
				blk_img_file_name.append(QString("blk_%1.png").arg(i, 4, 10, QChar('0')));
				image.mirrored().save(dir.absolutePath() + "/" + blk_img_file_name, "png");
			}


			//===========================================================================================
			equ[3] = box_bottom + unit*i;
			//***第二張圖***產生膠水圖**********//
			//if (!ol_and_cap)
			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -100, 100);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, -20, 0, 1, 0);
			//*********************************
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);

			binderstrips(md, par.getFloat("useless_print"));

			//************************************************//
			//****** Rendering the mesh's clip edge ****//
			GLuint MY_CLIP_PLANE = GL_CLIP_PLANE0;
			glEnable(MY_CLIP_PLANE);
			glClipPlane(MY_CLIP_PLANE, equ);

			glEnable(GL_STENCIL_TEST);
			glEnable(GL_CULL_FACE);
			glClear(GL_STENCIL_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

			//// first pass: increment stencil buffer value on back faces
			glStencilFunc(GL_ALWAYS, 0, 0);
			glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
			glCullFace(GL_FRONT); // render back faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			//glRotatef(90, 1, 0, 0);
			foreach(MeshModel * mp, md.meshList)
			{
				mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
			}

			glPopMatrix();
			glPopAttrib();
			//second pass: decrement stencil buffer value on front faces
			glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
			glCullFace(GL_BACK); // render front faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			//glRotatef(90, 1, 0, 0);
			foreach(MeshModel * mp, md.meshList)
			{
				mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
			}
			glPopMatrix();
			glPopAttrib();
			//-----------------------------
			// drawing clip planes masked by stencil buffer content
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glEnable(GL_DEPTH_TEST);
			glDisable(MY_CLIP_PLANE);
			glStencilFunc(GL_NOTEQUAL, 0, ~0);
			// stencil test will pass only when stencil buffer value = 0; 
			// (~0 = 0x11...11)

			glColor4f(1, 1, 1, 1);
			fillsquare();
			glDisable(GL_STENCIL_TEST);
			glEnable(MY_CLIP_PLANE); // enabling clip plane again
			glDisable(GL_LIGHTING);

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			//********************************************************************************************//



			glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, cap_image.bits());//***20150724
			//glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, cap_image.bits());							   
			QString img_file_name2 = md.p_setting.getcapImageName();
			glDisable(MY_CLIP_PLANE);//存完圖
			glDisable(GL_CULL_FACE);
			//****
			if (par.getBool("generate_zx"))
			{
				cv::Mat temp;
				cv::cvtColor(QImageToCvMat(cap_image.mirrored()), temp, CV_RGB2GRAY);
				capCTemp.push_back(temp); //zx,backup
				//***resizeImage***//
				//capCTemp.push_back(SKT::resizeIamge(&temp));
			}

			if (save_cap_Image){
				//cap_image = SKT::resizeIamge(QImageToCvMat(cap_image));
				img_file_name2.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				cap_image.mirrored().save(dir.absolutePath() + "/" + img_file_name2, "png");
			}
			//qDebug() << "img_file_name2" << img_file_name2;
			//****


			//***20160112***image_process加上插點pattern要在這處理
			if (par.getBool("generate_zx"))
			{
				if (par.getBool("mono_bool"))
				{
					image.fill(Qt::white);
					originalOutline = QImageToCvMat(image);
				}
				SKT::InkBalance imageProcess;
				imageProcess.operate2(originalOutline, capCTemp.back(), black_Mask, true);

				//cv::Mat getRealOutlineDst(originalOutline.size(), originalOutline.type());
				//SKT::getrealOutline(capCTemp.back(), originalOutline, black_Mask);
				outlineCTemp.push_back(originalOutline);//backup
				//outlineCTemp.push_back(SKT::resizeIamge(&originalOutline));


			}
			if (par.getBool(("generate_final_picture")))
			{
				QImage ttt(cvMatToQImage(originalOutline));
				QString tttt = "tttt";
				tttt.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				ttt.save(dir.absolutePath() + "/" + tttt, "png");
			}
			//***
			//*****
			if (par.getBool("generate_zx") && j % send_page_num == 0)
			{
				zGG.sendData(outlineCTemp, capCTemp);
				capCTemp.clear();
				outlineCTemp.clear();

				/*fprintf(dbgff, "b_name %i\n", time.elapsed());
				fflush(dbgff);*/
				//USBlink();
			}
			else if (par.getBool("generate_zx") && j % send_page_num != 0 && j == boxy_dim)
			{
				zGG.sendData(outlineCTemp, capCTemp);
				capCTemp.clear();
				outlineCTemp.clear();
				time.elapsed();
			}


			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();




			/* if (par.getBool("generate_zx") && j % zx_pages == 0)
			{
			QString zx_file_name = zx_dir.absolutePath() + "/uuu";
			zx_file_name.append(QString("_%1.zx").arg(j / zx_pages, 4, 10, QChar('0')));
			string zx_sn = zx_file_name.toUtf8().constData();

			generateZX(width, height, zx_pages, md.groove.DimX(), zx_sn);

			}
			else if (par.getBool("generate_zx") && j == boxy_dim && j % zx_pages > 0)
			{
			if (j / zx_pages > 0)
			{
			QString zx_file_name = zx_dir.absolutePath() + "/uuu";
			zx_file_name.append(QString("_%1.zx").arg((j / zx_pages) + 1, 4, 10, QChar('0')));
			string zx_sn = zx_file_name.toUtf8().constData();

			generateZX(width, height, j%zx_pages, md.groove.DimX(), zx_sn);
			capCTemp.clear();
			outlineCTemp.clear();
			}
			else
			{
			QString zx_file_name = zx_dir.absolutePath() + "/uuu";
			zx_file_name.append(QString("_%1.zx").arg((j / zx_pages), 4, 10, QChar('0')));
			string zx_sn = zx_file_name.toUtf8().constData();

			generateZX(width, height, j%zx_pages, md.groove.DimX(), zx_sn);
			capCTemp.clear();
			outlineCTemp.clear();
			}

			}*/
		}

		zGG.close();
		/*glMatrixMode(GL_MODELVIEW);
		glPopMatrix();*/

		qDebug() << "capCTemp.size()" << capCTemp.size();

#endif	

		ctx.unbindReadDrawFramebuffer();
		ctx.release();
		glPopAttrib();
		glContext->doneCurrent();

		//image.rgbSwapped().mirrored().save(par.getSaveFileName("ImageFileName"));
		//image.mirrored().save(par.getSaveFileName("ImageFileName"));
		//cap_image.mirrored().save(par.getSaveFileName("CapImageFileName"));
		//image.save(par.getSaveFileName("ImageFileName"));


		foreach(MeshModel *bm, md.meshList)
		{
			if ((bm->label().contains(olMName, Qt::CaseSensitive) || bm->label().contains(cMName, Qt::CaseSensitive)))
			{
				md.delMesh(bm);
			}
		}

		//if (par.getBool("generate_zx")){

		// QString ppath = "cd /d " + getRoamingDir();

		// QStringList arguments;
		// //arguments << "/k" << "cd /d d:/temptemp/zx_file" << "&" << "usbsend.exe" << "output.zx";						   
		// arguments << "/k" << ppath << "&&" << "usbsend.exe" << "output.zx" << "&&" << "exit";
		// //QProcess::startDetached("cmd", arguments);
		//}
		qDebug() << " done _applyClick ";
		break;
	}

#pragma endregion FP_GPU_EXAMPLE

#pragma region FP_GENERATE_MONO
	case FP_GENERATE_MONO:
	{


		//							 CMeshO & mesh = md.mm()->cm;
		//							 if ((mesh.vn < 3) || (mesh.fn < 1)) return false;
		//
		//							 const unsigned char * p0 = (const unsigned char *)(&(mesh.vert[0].P()));
		//							 const unsigned char * p1 = (const unsigned char *)(&(mesh.vert[1].P()));
		//							 const void *          pbase = p0;
		//							 GLsizei               pstride = GLsizei(p1 - p0);
		//
		//							 const unsigned char * n0 = (const unsigned char *)(&(mesh.vert[0].N()));
		//							 const unsigned char * n1 = (const unsigned char *)(&(mesh.vert[1].N()));
		//							 const void *          nbase = n0;
		//							 GLsizei               nstride = GLsizei(n1 - n0);
		//
		//							 glContext->makeCurrent();
		//							 GLenum err = glewInit();
		//							 if (GLEW_OK != err)
		//							 {
		//								 /* Problem: glewInit failed, something is seriously wrong. */
		//								 //fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		//								 //qDebug("Error: %s\n", glewGetErrorString(err));
		//							 }
		//
		//							 glPushAttrib(GL_ALL_ATTRIB_BITS);
		//
		//							 Context ctx;
		//							 ctx.acquire();
		//
		//							 const GLsizeiptr psize = GLsizeiptr(GLsizei(mesh.vn) * pstride);
		//							 BufferHandle hPositionBuffer = createBuffer(ctx, psize, pbase);
		//
		//							 const GLsizeiptr nsize = GLsizeiptr(GLsizei(mesh.vn) * nstride);
		//							 BufferHandle hNormalBuffer = createBuffer(ctx, nsize, nbase);
		//
		//							 const GLsizeiptr isize = GLsizeiptr(mesh.fn * 3 * sizeof(GLuint));
		//							 BufferHandle hIndexBuffer = createBuffer(ctx, isize);
		//							 {
		//								 BoundIndexBufferHandle indexBuffer = ctx.bindIndexBuffer(hIndexBuffer);
		//
		//								 const CMeshO::VertexType * vbase = &(mesh.vert[0]);
		//								 GLuint *  indices = (GLuint *)indexBuffer->map(GL_WRITE_ONLY);
		//								 for (size_t i = 0; i < mesh.face.size(); ++i)
		//								 {
		//									 const CMeshO::FaceType & f = mesh.face[i];
		//									 if (f.IsD()) continue;
		//									 for (int v = 0; v < 3; ++v)
		//									 {
		//										 *indices++ = GLuint(vcg::tri::Index(mesh, f.cV(v)));
		//									 }
		//								 }
		//								 indexBuffer->unmap();
		//
		//								 ctx.unbindIndexBuffer();
		//							 }
		//							 ///***設定儲存image的長寬
		//							 /* const GLsizei width = GLsizei(par.getInt("ImageWidth"));
		//							 const GLsizei height = GLsizei(par.getInt("ImageHeight"));*/
		//
		//							 //***20150907
		//							 const GLsizei print_dpi = GLsizei(par.getInt("print_dpi"));
		//							 float x = md.groove.DimX();
		//							 float y = md.groove.DimY();
		//							 const GLsizei width = (int)(x / DSP_inchmm * print_dpi);
		//							 const GLsizei height = (int)(width * y / x);
		//							 qDebug() << "width,height" << width << height;
		//						
		//
		//							 //qDebug() << "width" << "height" << width << height;
		//							 //RenderbufferHandle hDepth = createRenderbuffer(ctx, GL_DEPTH_COMPONENT24, width, height);
		//							 RenderbufferHandle hDepth = createRenderbuffer(ctx, GL_DEPTH24_STENCIL8, width, height);
		//							 Texture2DHandle    hColor = createTexture2D(ctx, GL_RGB8, width, height, GL_RGB, GL_UNSIGNED_BYTE);
		//							 //FramebufferHandle  hFramebuffer = createFramebuffer(ctx, renderbufferTarget(hDepth), texture2DTarget(hColor));
		//							 FramebufferHandle  hFramebuffer = createFramebufferWithDepthStencil(ctx, renderbufferTarget(hDepth), renderbufferTarget(hDepth), texture2DTarget(hColor));
		//
		//
		//
		//							 //const QColor       backgroundColor  = par.getColor("ImageBackgroundColor");
		//							 const vcg::Point3f lightDirectionVS = vcg::Point3f(0.0f, 0.0f, -1.0f).Normalize();
		//
		//							 glEnable(GL_DEPTH_TEST);
		//							 //glClearColor(GLfloat(backgroundColor.red())/255.0f, GLfloat(backgroundColor.green())/255.0f, GLfloat(backgroundColor.blue())/255.0f, 0.0f);
		//							 Construction_Groove<float> tempgroove = md.groove;
		//
		//							 glViewport(0, 0, width, height);//***window的長寬
		//							
		//
		//							 const vcg::Point3f center = mesh.bbox.Center();
		//							 const float        scale = 1.0f / mesh.bbox.Diag();
		//
		//							 //glScalef(scale, scale, scale);
		//							 //glTranslatef(-center[0], -center[1], -center[2]);
		//
		//							 //***儲存image的基本資訊
		//							 QImage image(int(width), int(height), QImage::Format_RGB888);
		//							 QImage cap_image(int(width), int(height), QImage::Format_RGB888);
		//
		//							 //***20150505***slice_func_設置高度
		//							 float box_bottom = md.bbox().min.Z();
		//							 float unit = par.getFloat("slice_height");
		//
		//							
		//							 int boxy_dim = qCeil((md.bbox().max.Z() - md.bbox().min.Z()) / unit) + 1;
		//							 //init_texture(md);
		//
		//							 //*******************
		//
		//							 qDebug() << "sample_filter_place1";
		//
		//							 //***20150507 standard path******************
		//							 //QString temppath = QStandardPaths::locate(QStandardPaths::TempLocation, "", QStandardPaths::LocateDirectory);
		//							 //QString temppath = "D:/temptemp";//backup
		//							 QString temppath = getRoamingDir();
		//							 QDir dir(temppath);//backup
		//							 if (dir.exists("xyzimage"))
		//							 {
		//								 dir.setPath(temppath + "/xyzimage");
		//								 dir.removeRecursively();
		//								 dir.setPath(temppath);
		//								 dir.mkpath("xyzimage");
		//							 }
		//							 else
		//							 {
		//								 dir.setPath(temppath);
		//								 dir.mkpath("xyzimage");
		//							 }
		//
		//							 dir.setPath(temppath + "/xyzimage");
		//
		//							 //---zx_file------------------------------------------
		//							 QString zxtemp = "D:/temptemp";
		//							 QDir zx_dir(zxtemp);
		//							 if (zx_dir.exists("zx_file"))
		//							 {
		//								 zx_dir.setPath(zxtemp + "/zx_file");
		//								 //zx_dir.removeRecursively();
		//								 zx_dir.setPath(zxtemp);
		//								 zx_dir.mkpath("zx_file");
		//							 }
		//							 else
		//							 {
		//								 dir.setPath(temppath);
		//								 dir.mkpath("zx_file");
		//							 }
		//
		//							 zx_dir.setPath(zxtemp + "/zx_file");
		//
		//							
		//							 ctx.bindReadDrawFramebuffer(hFramebuffer);
		//							 GLW_CHECK_GL_READ_DRAW_FRAMEBUFFER_STATUS;
		//#if 1
		//
		//							 const bool outputOL_Image = par.getBool("OL_Image");
		//							 const bool save_cap_Image = par.getBool("Cap_Image");
		//							 const bool ol_and_cap = par.getBool("OL_CAP_Image");
		//							 const bool faceColor = par.getBool("FaceColor");
		//							 glClearColor(1, 1, 1, 1);
		//							 QString olMName = md.p_setting.getOlMeshName();
		//							 QString cMName = md.p_setting.getCapMeshName();
		//
		//							 //***opencvMat clear
		//							 capCTemp.clear();
		//							 outlineCTemp.clear();
		//
		//							 int zx_pages = par.getInt("per_zx_pages");
		//							 int start_page = par.getInt("start_page");
		//							 int send_page_num = par.getInt("send_page_num");
		//							 //QString zx_file_name = zx_dir.absolutePath() + "/";//option 1
		//							 //string zx_sn = zx_file_name.toUtf8().constData();
		//							 //---option 2
		//							 //QString zx_file_name = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/";//---20151228
		//							 QString zx_file_name = getRoamingDir();
		//
		//							
		//
		//							 string zx_sn = zx_file_name.toUtf8().constData();
		//							 //zx_sn += makeDefaultZxName();//***20151224**backup
		//							 zx_sn += "output.zx";
		//							 qDebug() << "sample_filtergpuzx_sn" << zx_file_name;
		//							 ZxGen zGG(unsigned int(width), unsigned int(height), boxy_dim, md.groove.DimX(), zx_sn);//20161219_backup
		//							 //ZxGen zGG(unsigned int(width), unsigned int(height)+500, boxy_dim, md.groove.DimX(), zx_sn);
		//							 qDebug() << "sample_filtergpu" << md.groove.DimX();
		//							 //for (int i = 0, j = 1; i < boxy_dim; i++, j++)
		//							 //boxy_dim+1因為讓最後一層顏色多一層
		//							 for (int i = start_page, j = start_page + 1; i < boxy_dim; i++, j++)
		//								 //for (int i = 0; i < 1; i++)
		//							 {
		//								 //xyz::slice_roution(md, box_bottom + unit*i, faceColor);//***backup******************切層演算							  
		//								 xyz::slice_roution(md, box_bottom + unit*i, true);//****20160301*****************切層演算							  
		//								 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		//								 glClearColor(1, 1, 1, 1);
		//
		//								 //vcg::glColor(vcg::Color4b::LightGray);
		//								 //_buffPass->enable();
		//
		//								 //***第一張outline圖*****************************
		//								 QVector<MeshModel *> vmm;
		//								 md.getMeshByContainString("_temp_outlines", vmm);
		//								 md.getMeshByContainString(olMName, vmm);
		//								 QVector<MeshModel *> cmm;
		//								 md.getMeshByContainString(cMName, cmm);//capImage
		//								 int xx = 0;								 
		//								 
		//
		//								 glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
		//								 glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -100, 100);
		//								 glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, 0, 0, 1, 0);
		//								 cmystrips(md, par.getFloat("useless_print") / 3.);
		//
		//								 QString img_file_name = md.p_setting.getoutlineName();
		//
		//
		//								 //***第一張圖片編號
		//								 //***20150921***增加bool box
		//
		//								 //***20150508***BMP QImage設為Format_RGBA8888, readpixel要設為GL_RGBA
		//								 glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image.bits());
		//
		//								 if (par.getBool("generate_zx"))
		//								 {
		//									 originalOutline = QImageToCvMat(image.mirrored());
		//									 //outlineCTemp.push_back(QImageToCvMat(image.mirrored())); //zxzxzxzxzxzxzxz
		//								 }
		//								 
		//
		//
		//								 //===========================================================================================
		//								 equ[3] = box_bottom + unit*i;
		//								 //***第二張圖***產生膠水圖**********//
		//								 //if (!ol_and_cap)
		//								 glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
		//								 glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -100, 100);
		//								 glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, -20, 0, 1, 0);
		//								 //*********************************
		//								 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		//								 glClearColor(1, 1, 1, 1);
		//
		//								 binderstrips(md, par.getFloat("useless_print"));
		//
		//								 //************************************************//
		//								 //****** Rendering the mesh's clip edge ****//
		//								 GLuint MY_CLIP_PLANE = GL_CLIP_PLANE0;
		//								 glEnable(MY_CLIP_PLANE);
		//								 glClipPlane(MY_CLIP_PLANE, equ);
		//
		//								 glEnable(GL_STENCIL_TEST);
		//								 glEnable(GL_CULL_FACE);
		//								 glClear(GL_STENCIL_BUFFER_BIT);
		//								 glDisable(GL_DEPTH_TEST);
		//								 glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		//
		//								 //// first pass: increment stencil buffer value on back faces
		//								 glStencilFunc(GL_ALWAYS, 0, 0);
		//								 glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
		//								 glCullFace(GL_FRONT); // render back faces only
		//								 glPushAttrib(GL_ALL_ATTRIB_BITS);
		//								 glEnable(GL_LIGHTING);
		//								 glPushMatrix();
		//
		//								 //glRotatef(90, 1, 0, 0);
		//								 foreach(MeshModel * mp, md.meshList)
		//								 {
		//									 mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
		//								 }
		//
		//								 glPopMatrix();
		//								 glPopAttrib();
		//								 //second pass: decrement stencil buffer value on front faces
		//								 glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
		//								 glCullFace(GL_BACK); // render front faces only
		//								 glPushAttrib(GL_ALL_ATTRIB_BITS);
		//								 glEnable(GL_LIGHTING);
		//								 glPushMatrix();
		//
		//								 //glRotatef(90, 1, 0, 0);
		//								 foreach(MeshModel * mp, md.meshList)
		//								 {
		//									 mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
		//								 }
		//								 glPopMatrix();
		//								 glPopAttrib();
		//								 //-----------------------------
		//								 // drawing clip planes masked by stencil buffer content
		//								 glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		//								 glEnable(GL_DEPTH_TEST);
		//								 glDisable(MY_CLIP_PLANE);
		//								 glStencilFunc(GL_NOTEQUAL, 0, ~0);
		//								 // stencil test will pass only when stencil buffer value = 0; 
		//								 // (~0 = 0x11...11)
		//
		//								 glColor4f(1, 1, 1, 1);
		//								 fillsquare();
		//								 glDisable(GL_STENCIL_TEST);
		//								 glEnable(MY_CLIP_PLANE); // enabling clip plane again
		//								 glDisable(GL_LIGHTING);
		//
		//								 glMatrixMode(GL_MODELVIEW);
		//								 glPopMatrix();
		//								 glMatrixMode(GL_PROJECTION);
		//								 glPopMatrix();
		//								 //********************************************************************************************//
		//
		//
		//
		//								 glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, cap_image.bits());//***20150724
		//								 //glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, cap_image.bits());							   
		//								 QString img_file_name2 = md.p_setting.getcapImageName();
		//								 glDisable(MY_CLIP_PLANE);//存完圖
		//								 glDisable(GL_CULL_FACE);
		//								 //****
		//								 if (par.getBool("generate_zx"))
		//								 {
		//									 cv::Mat temp;
		//									 cv::cvtColor(QImageToCvMat(cap_image.mirrored()), temp, CV_RGB2GRAY);
		//									 capCTemp.push_back(temp); //zx,backup
		//									 //***resizeImage***//
		//									 //capCTemp.push_back(SKT::resizeIamge(&temp));
		//								 }
		//
		//								 if (save_cap_Image){
		//									 img_file_name2.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
		//									 cap_image.mirrored().save(dir.absolutePath() + "/" + img_file_name2, "png");
		//								 }
		//								 
		//
		//								 //***20160112***image_process加上pattern要在這處理
		//								 if (par.getBool("generate_zx"))
		//								 {
		//									
		//									 outlineCTemp.push_back(originalOutline);//backup
		//									 //outlineCTemp.push_back(SKT::resizeIamge(&originalOutline));
		//								 }
		//								 
		//
		//								 //***
		//								 //*****
		//								 if (par.getBool("generate_zx") && j % send_page_num == 0)
		//								 {
		//									 zGG.sendData(outlineCTemp, capCTemp);
		//									 capCTemp.clear();
		//									 outlineCTemp.clear();
		//								 }
		//								 else if (par.getBool("generate_zx") && j % send_page_num != 0 && j == boxy_dim)
		//								 {
		//									 zGG.sendData(outlineCTemp, capCTemp);
		//									 capCTemp.clear();
		//									 outlineCTemp.clear();
		//
		//								 }
		//
		//
		//								 glMatrixMode(GL_MODELVIEW);
		//								 glPopMatrix();
		//								 glMatrixMode(GL_PROJECTION);
		//								 glPopMatrix();
		//
		//
		//
		//
		//							 }
		//
		//							 zGG.close();
		//							 /*glMatrixMode(GL_MODELVIEW);
		//							 glPopMatrix();*/
		//
		//							 qDebug() << "capCTemp.size()" << capCTemp.size();
		//
		//#endif	
		//
		//							 ctx.unbindReadDrawFramebuffer();
		//							 ctx.release();
		//							 glPopAttrib();
		//							 glContext->doneCurrent();
		//
		//							
		//
		//							 foreach(MeshModel *bm, md.meshList)
		//							 {
		//								 if ((bm->label().contains(olMName, Qt::CaseSensitive) || bm->label().contains(cMName, Qt::CaseSensitive)))
		//								 {
		//									 md.delMesh(bm);
		//								 }
		//							 }
		//
		//							 if (par.getBool("generate_zx")){
		//
		//								 QString ppath = "cd /d " + getRoamingDir();
		//
		//								 QStringList arguments;
		//								 //arguments << "/k" << "cd /d d:/temptemp/zx_file" << "&" << "usbsend.exe" << "output.zx";						   
		//								 arguments << "/k" << ppath << "&&" << "usbsend.exe" << "output.zx" << "&&" << "exit";
		//								 //QProcess::startDetached("cmd", arguments);
		//							 }
		//
		//							 qDebug() << " done _applyClick ";



		break;
	}
#pragma endregion FP_GENERATE_MONO
#pragma region FP_PRINT_FLOW
	case FP_PRINT_FLOW:
	{
		/*QProgressDialog dialog;
		dialog.setLabelText(QString("Progressing using thread(s)..."));
		dialog.setRange(0, 100);*/
		// Create a QFutureWatcher and connect signals and slots.
		/*QObject::connect(this, SIGNAL(test_update_pages(int)), &dialog, SLOT(setValue(int)));	*/
		QObject::connect(this, SIGNAL(test_update_pages(int)), &thread1, SLOT(page_count(int)));
		//gl.moveToThread( QApplication::instance()->thread() );
		// send it event which will be posted from main thread
		//QCoreApplication::postEvent( &gl, new QEvent( QEvent::User ) );
		//dialog.exec();
		//thread1.start();
		//QObject::connect(&t, SIGNAL(finished()), &a, SLOT(quit()));
		md.jobname.clear();
		//***************************


		CMeshO & mesh = md.mm()->cm;
		if ((mesh.vn < 3) || (mesh.fn < 1)) return false;

		const unsigned char * p0 = (const unsigned char *)(&(mesh.vert[0].P()));
		const unsigned char * p1 = (const unsigned char *)(&(mesh.vert[1].P()));
		const void *          pbase = p0;
		GLsizei               pstride = GLsizei(p1 - p0);

		const unsigned char * n0 = (const unsigned char *)(&(mesh.vert[0].N()));
		const unsigned char * n1 = (const unsigned char *)(&(mesh.vert[1].N()));
		const void *          nbase = n0;
		GLsizei               nstride = GLsizei(n1 - n0);

		/*glContext->doneCurrent();
		glContext->moveToThread(th1);*/

		glContext->makeCurrent();


		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			/* Problem: glewInit failed, something is seriously wrong. */
			//fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
			//qDebug("Error: %s\n", glewGetErrorString(err));
		}

		glPushAttrib(GL_ALL_ATTRIB_BITS);

		Context ctx;
		ctx.acquire();
		if (switchInitMeshVBO)
			initMeshVBO(md);

		const GLsizeiptr psize = GLsizeiptr(GLsizei(mesh.vn) * pstride);
		BufferHandle hPositionBuffer = createBuffer(ctx, psize, pbase);

		const GLsizeiptr nsize = GLsizeiptr(GLsizei(mesh.vn) * nstride);
		BufferHandle hNormalBuffer = createBuffer(ctx, nsize, nbase);

		const GLsizeiptr isize = GLsizeiptr(mesh.fn * 3 * sizeof(GLuint));
		BufferHandle hIndexBuffer = createBuffer(ctx, isize);
		{
			BoundIndexBufferHandle indexBuffer = ctx.bindIndexBuffer(hIndexBuffer);

			const CMeshO::VertexType * vbase = &(mesh.vert[0]);
			GLuint *  indices = (GLuint *)indexBuffer->map(GL_WRITE_ONLY);
			for (size_t i = 0; i < mesh.face.size(); ++i)
			{
				const CMeshO::FaceType & f = mesh.face[i];
				if (f.IsD()) continue;
				for (int v = 0; v < 3; ++v)
				{
					*indices++ = GLuint(vcg::tri::Index(mesh, f.cV(v)));
				}
			}
			indexBuffer->unmap();

			ctx.unbindIndexBuffer();
		}

		//***20150907
		const GLsizei print_dpi = GLsizei(par.getInt("print_dpi"));
		float x = md.groove.DimX();
		float y = md.groove.DimY();
		const GLsizei width = (int)(x / DSP_inchmm * print_dpi);
		const GLsizei height = (int)(width * y / x);

		//qDebug() << "width" << "height" << width << height;
		//RenderbufferHandle hDepth = createRenderbuffer(ctx, GL_DEPTH_COMPONENT24, width, height);
		RenderbufferHandle hDepth = createRenderbuffer(ctx, GL_DEPTH24_STENCIL8, width, height);
		Texture2DHandle    hColor = createTexture2D(ctx, GL_RGB8, width, height, GL_RGB, GL_UNSIGNED_BYTE);
		//FramebufferHandle  hFramebuffer = createFramebuffer(ctx, renderbufferTarget(hDepth), texture2DTarget(hColor));
		FramebufferHandle  hFramebuffer = createFramebufferWithDepthStencil(ctx, renderbufferTarget(hDepth), renderbufferTarget(hDepth), texture2DTarget(hColor));




		const vcg::Point3f lightDirectionVS = vcg::Point3f(0.0f, 0.0f, -1.0f).Normalize();

		glEnable(GL_DEPTH_TEST);
		//glClearColor(GLfloat(backgroundColor.red())/255.0f, GLfloat(backgroundColor.green())/255.0f, GLfloat(backgroundColor.blue())/255.0f, 0.0f);
		Construction_Groove<float> tempgroove = md.groove;

		glViewport(0, 0, width, height);//***window的長寬				  

		const vcg::Point3f center = mesh.bbox.Center();
		const float        scale = 1.0f / mesh.bbox.Diag();

		//glScalef(scale, scale, scale);
		//glTranslatef(-center[0], -center[1], -center[2]);

		//***儲存image的基本資訊
		QImage image(int(width), int(height), QImage::Format_RGB888);
		QImage cap_image(int(width), int(height), QImage::Format_RGB888);

		//***20150505***slice_func_設置高度
		float box_bottom = md.bbox().min.Z();
		float unit = par.getFloat("slice_height");



		int boxy_dim = qCeil((md.bbox().max.Z() - md.bbox().min.Z()) / unit) + 1;
		init_texture(md);

		//*******************					  

		//***20150507 standard path******************
		//QString temppath = QStandardPaths::locate(QStandardPaths::TempLocation, "", QStandardPaths::LocateDirectory);
		//QString temppath = "D:/temptemp";//backup
		QString temppath = getRoamingDir();
		QDir dir(temppath);//backup
		if (dir.exists("xyzimage"))
		{
			dir.setPath(temppath + "/xyzimage");
			dir.removeRecursively();
			dir.setPath(temppath);
			dir.mkpath("xyzimage");
		}
		else
		{
			dir.setPath(temppath);
			dir.mkpath("xyzimage");
		}

		dir.setPath(temppath + "/xyzimage");

		//---zx_file------------------------------------------
		//QString zxtemp = "D:/temptemp";
		QString zxtemp = getRoamingDir();
		QDir zx_dir(zxtemp);
		if (zx_dir.exists("zx_file"))
		{
			zx_dir.setPath(zxtemp + "/zx_file");
			//zx_dir.removeRecursively();
			zx_dir.setPath(zxtemp);
			zx_dir.mkpath("zx_file");
		}
		else
		{
			dir.setPath(temppath);
			dir.mkpath("zx_file");
		}

		zx_dir.setPath(zxtemp + "/zx_file");

		//==============================
		//**********************
		ctx.bindReadDrawFramebuffer(hFramebuffer);
		GLW_CHECK_GL_READ_DRAW_FRAMEBUFFER_STATUS;
#if 1

		const bool outputOL_Image = par.getBool("OL_Image");
		const bool save_cap_Image = par.getBool("Cap_Image");

		const bool faceColor = par.getBool("FaceColor");
		glClearColor(1, 1, 1, 1);
		QString olMName = md.p_setting.getOlMeshName();
		QString cMName = md.p_setting.getCapMeshName();

		//***opencvMat clear
		capCTemp.clear();
		outlineCTemp.clear();


		int zx_job_pages = par.getInt("per_zx_job");
		int start_page = par.getInt("start_page");
		int send_page_num = par.getInt("send_page_num");
		int plus_pixel = par.getInt("plus_pixel");
		int PLUS_MM = par.getInt("PLUS_MM");
		bool start_print_b = par.getBool("PRINT_START");
		float useless_print = par.getFloat("USELESS_PRINT");
		float meCorrectPercent = 0.97;

		int widthrecord, heightrecord;
		QString blk_img_file_name = "black_outline";
		QString zx_sn = makeOutputname(zx_dir.absolutePath(), 0);

		md.jobname.push_back(zx_sn);

		//zggg = new ZxGen(unsigned int(width), unsigned int(height), zx_pages, md.groove.DimX(),21, zx_sn.toStdString());//20161219_backup						 
		//zggg = new ZxGen(unsigned int(width), 2580, zx_job_pages, md.groove.DimX(), 21, zx_sn.toStdString());//20161219_backup						 
		zggg = new ZxGen(unsigned int(width), unsigned int(height*meCorrectPercent + plus_pixel), zx_job_pages, md.groove.DimX(), md.groove.DimY() + PLUS_MM, zx_sn.toStdString(), start_print_b);//20161219_backup						 
		//zggg = new ZxGen(unsigned int(width), unsigned int(4713), zx_job_pages, md.groove.DimX(), md.groove.DimY() + PLUS_MM, zx_sn.toStdString(), start_print_b);//20161219_backup						 

		FILE *dbgff;
		dbgff = fopen("D:\\debug_open_1.txt", "w");//@@@
		QTime time;
		//clock_t start, stop;
		time.start();

		//***20160622_test_progress bar initializing progress bar status
		if (cb != NULL)
		{
			glContext->doneCurrent();
			(*cb)(0, "processing...");
			glContext->makeCurrent();
		}

		cv::Mat look_down_color_CV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat look_down_Black_Mask_layer(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat look_down_Black_MaskCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat look_up_Black_MaskCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat look_up_color_CV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat captemp(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat capDownMask(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat black_Mask(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat black_Mask2(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat outlineBlackMaskCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat lookDownBlackTriangleMask(height, width, CV_8UC3, Scalar(255, 255, 255));

		cv::Mat prePrintobjectC(height, width, CV_8UC3, Scalar(255, 255, 255));

		cv::Mat downColorFirstCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat downBlackMaskFirstCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat downColorSecondCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat downBlackMaskSecondCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat prePrintobjectB(height, width, CV_8UC1, Scalar(255));

		cv::Mat capDownMask_bottom_face(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat capDownMask_top_face(height, width, CV_8UC3, Scalar(255, 255, 255));


		cv::Mat color_Bottom_face(height, width, CV_8UC3, Scalar(255, 255, 255));



		cv::Mat color_Top_face(height, width, CV_8UC3, Scalar(255, 255, 255));



		cv::Mat originalOutline(height, width, CV_8UC3, Scalar(255, 255, 255));


		cv::Mat outlineColorCv(height, width, CV_8UC3, Scalar(255, 255, 255));


		cv::Mat capDownSureMask(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat capTopSureMask(height, width, CV_8UC3, Scalar(255, 255, 255));
		for (int i = start_page, j = start_page + 1; i < boxy_dim; i++, j++)
		{
			if (cb && (i % 1) == 0)
			{
				glContext->doneCurrent();
				bool break_test = cb((i * 100) / boxy_dim, "slice process");
				if (!break_test)
				{
					glContext->doneCurrent();
					return false;
					break;
				}

			}
			glContext->makeCurrent();
			//xyz::slice_roution(md, box_bottom + unit*i, faceColor);//***backup******************切層演算							  
			time.restart();
			xyz::slice_roution(md, box_bottom + unit*i, faceColor);//****20160301*****************切層演算							  

			/*fprintf(dbgff, "slice_func %i, %i\n", time.elapsed(), i);
			fflush(dbgff);*/

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);
			//time.restart();
			//***第一張outline圖*****************************
			QVector<MeshModel *> vmm;
			md.getMeshByContainString("_temp_outlines", vmm);
			md.getMeshByContainString(olMName, vmm);
			QVector<MeshModel *> cmm;
			md.getMeshByContainString(cMName, cmm);//capImage
			int xx = 0;

			QString img_file_name = md.p_setting.getoutlineName();
			float cut_z = box_bottom + unit*i;
			foreach(MeshModel *mdmm, md.meshList)
			{
				//****20151231********************************************************************

				if (mdmm->getMeshSort() == MeshModel::meshsort::print_item)
					if (mdmm->rmm.colorMode != GLW::CMNone || mdmm->rmm.textureMode != GLW::TMNone)//如果無色就不須做
						if (cut_z - mdmm->cm.bbox.min.Z() < 0 || cut_z - mdmm->cm.bbox.max.Z() > 0)//在物體box的上下需要設定限制，才不會超出物體大小。								  
						{
							glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
							glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., 0.0001, 0);
							glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, cut_z, 0, 0, -30, 0, 1, 0);

							glPushMatrix();
							if (vcg::tri::HasPerWedgeTexCoord(mdmm->cm) && !mdmm->cm.textures.empty())
								mdmm->render(GLW::DMSmooth, GLW::CMNone, GLW::TMPerWedgeMulti);
							else if (vcg::tri::HasPerFaceColor(mdmm->cm))
								mdmm->render(GLW::DMFlat, GLW::CMPerFace, GLW::TMNone);
							else if (vcg::tri::HasPerVertexColor(mdmm->cm))
								mdmm->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
							glPopMatrix();


						}

						else//物體梯田情況會有0.1層顏色的厚度。
						{
							glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
							//glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., 0.01, 0);//backup
							//glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., 0.005, -0.005);
							glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -0.05, 0.05);
							//***20161112
							glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, cut_z, 0, 0, -30, 0, 1, 0);

							glPushMatrix();
							if (vcg::tri::HasPerWedgeTexCoord(mdmm->cm) && !mdmm->cm.textures.empty())
								mdmm->render(GLW::DMSmooth, GLW::CMNone, GLW::TMPerWedgeMulti);
							else if (vcg::tri::HasPerFaceColor(mdmm->cm))
								mdmm->render(GLW::DMFlat, GLW::CMPerFace, GLW::TMNone);
							else if (vcg::tri::HasPerVertexColor(mdmm->cm))
								mdmm->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
							glPopMatrix();
						}

			}
			//
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();

			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -100, 100);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, 0, 0, 1, 0);
			//cmystrips(md, par.getFloat("useless_print") / 3.);//***useless bar
			foreach(MeshModel *mp, vmm)//畫輪廓圖。
			{
				mp->glw.viewerNum = 2;
				if (vmm.size() > 0)
				{
					//qDebug("meshname" + (*mp).label().toLatin1());
					//mp->render(vcg::GLW::DMSmooth, vcg::GLW::CMPerVert, vcg::GLW::TMPerWedgeMulti);						
					//mp->render(vcg::GLW::DMTexWire, vcg::GLW::CMPerVert, vcg::GLW::TMNone);//backup
					mp->render(vcg::GLW::DMTexWire, mp->rmm.colorMode, mp->rmm.textureMode);//backup
					//mp->render(vcg::GLW::DMTexWire, vcg::GLW::CMBlack, mp->rmm.textureMode);//backup

					//mp->render(vcg::GLW::DMOutline_Test, vcg::GLW::CMNone, vcg::GLW::TMNone);
				}
			}

			//***第一張圖片編號
			//***20150921***增加bool box

			//***20150508***BMP QImage設為Format_RGBA8888, readpixel要設為GL_RGBA
			glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image.bits());

			if (par.getBool("generate_zx"))
			{
				originalOutline = QImageToCvMat(image.mirrored());
				//outlineCTemp.push_back(QImageToCvMat(image.mirrored())); //zxzxzxzxzxzxzxz
			}
			if (outputOL_Image)//彩色輪廓
			{
				img_file_name.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				image.mirrored().save(dir.absolutePath() + "/" + img_file_name, "png");
			}


			//***20160122***產生黑色輪廓編號_mask
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);

			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -100, 100);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, 0, 0, 1, 0);
			//*********************************
			//binderstrips(md, par.getFloat("useless_print"));//******uselessbar
			//image.fill(QColor::w);
			foreach(MeshModel *mp, vmm)//outline
			{
				mp->glw.viewerNum = 2;
				if (vmm.size() > 0)
				{
					//mp->render(vcg::GLW::DMTexWire, mp->rmm.colorMode, mp->rmm.textureMode);//backup
					mp->render(vcg::GLW::DMTexWire, vcg::GLW::CMBlack, vcg::GLW::TMNone);//backup

					//mp->render(vcg::GLW::DMOutline_Test, vcg::GLW::CMNone, vcg::GLW::TMNone);
				}
			}

			foreach(MeshModel *mdmm, md.meshList)
			{
				//****20151231********************************************************************								  
				if (mdmm->getMeshSort() == MeshModel::meshsort::print_item)
					if (cut_z - mdmm->cm.bbox.min.Z() < 0 || cut_z - mdmm->cm.bbox.max.Z() > 0)//在物體box的上下需要設定限制，才不會超出物體大小。
					{
						glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
						glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., 0.0001, 0);
						glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, cut_z, 0, 0, -30, 0, 1, 0);

						mdmm->render(GLW::DMFlat, GLW::CMBlack, GLW::TMNone);
						glPopMatrix();

					}
					else//物體梯田情況會有0.1層顏色的厚度。
					{
						glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
						glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -0.05, 0.05);
						//***20161112
						glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, cut_z, 0, 0, -30, 0, 1, 0);

						glPushMatrix();
						mdmm->render(GLW::DMFlat, GLW::CMBlack, GLW::TMNone);
						glPopMatrix();
					}

			}
			//fFaceColor
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();


			glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image.bits());
			black_Mask = QImageToCvMat(image.mirrored());
			if (par.getBool("BLK_OUT_LINE"))
			{
				blk_img_file_name.append(QString("blk_%1.png").arg(i, 4, 10, QChar('0')));
				image.mirrored().save(dir.absolutePath() + "/" + blk_img_file_name, "png");
			}

			/*fprintf(dbgff, "render_process %i, %i\n", time.elapsed(), i);
			fflush(dbgff);*/
			//===========================================================================================
			equ[3] = box_bottom + unit*i;
			//***第二張圖***產生膠水圖**********//
			//time.restart();
			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -100, 100);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, -20, 0, 1, 0);
			//*********************************
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);

			//binderstrips(md, par.getFloat("useless_print"));//***uselessbar

			//************************************************//
			//****** Rendering the mesh's clip edge ****//
			GLuint MY_CLIP_PLANE = GL_CLIP_PLANE0;
			glEnable(MY_CLIP_PLANE);
			glClipPlane(MY_CLIP_PLANE, equ);

			glEnable(GL_STENCIL_TEST);
			glEnable(GL_CULL_FACE);
			glClear(GL_STENCIL_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

			//// first pass: increment stencil buffer value on back faces
			glStencilFunc(GL_ALWAYS, 0, 0);
			glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
			glCullFace(GL_FRONT); // render back faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			//glRotatef(90, 1, 0, 0);
			foreach(MeshModel * mp, md.meshList)
			{
				if (mp->getMeshSort() == MeshModel::meshsort::print_item)
					//mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
					mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
			}

			glPopMatrix();
			glPopAttrib();
			//second pass: decrement stencil buffer value on front faces
			glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
			glCullFace(GL_BACK); // render front faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			//glRotatef(90, 1, 0, 0);
			foreach(MeshModel * mp, md.meshList)
			{
				if (mp->getMeshSort() == MeshModel::meshsort::print_item)
					//mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
					mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
			}
			glPopMatrix();
			glPopAttrib();
			//-----------------------------
			// drawing clip planes masked by stencil buffer content
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glEnable(GL_DEPTH_TEST);
			glDisable(MY_CLIP_PLANE);
			glStencilFunc(GL_NOTEQUAL, 0, ~0);
			// stencil test will pass only when stencil buffer value = 0; 
			// (~0 = 0x11...11)

			glColor4f(1, 1, 1, 1);
			fillsquare();
			glDisable(GL_STENCIL_TEST);
			glEnable(MY_CLIP_PLANE); // enabling clip plane again
			glDisable(GL_LIGHTING);

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			//********************************************************************************************//


			glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, cap_image.bits());//***20150724
			//glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, cap_image.bits());							   
			QString img_file_name2 = md.p_setting.getcapImageName();
			glDisable(MY_CLIP_PLANE);//存完圖
			glDisable(GL_CULL_FACE);
			//****

			/*fprintf(dbgff, "cap_image_process %i, %i\n", time.elapsed(), i);
			fflush(dbgff);*/

			cv::Mat captemp;
			if (par.getBool("generate_zx"))
			{
				//cv::Mat temp;
				cv::cvtColor(QImageToCvMat(cap_image.mirrored()), captemp, CV_RGB2GRAY);
				//capCTemp.push_back(temp); //zx,backup
				//***resizeImage***//
				//captemp = SKT::resizeIamge(&captemp, plus_pixel);
				//SKT::generateUselessbar(captemp);
			}
			//***20160112***image_process加上插點pattern
			if (par.getBool("generate_zx"))
			{
				if (par.getBool("mono_bool"))
				{
					image.fill(Qt::white);
					originalOutline = QImageToCvMat(image);
				}
				SKT::InkBalance imageProcess;

				cv::Mat temp, temp2;
				//temp = SKT::resizeIamge(&originalOutline,plus_pixel);


				//imageProcess.operate2(temp, captemp, SKT::resizeIamge(&black_Mask, plus_pixel));
				imageProcess.operate2(originalOutline, captemp, black_Mask, true);
				//SKT::generateUselessbar(temp);
				originalOutline = SKT::resizeXAxis(&originalOutline, meCorrectPercent);//****0.97
				originalOutline = SKT::resizeIamge(&originalOutline, plus_pixel);
				//SKT::generateUselessbar(originalOutline, useless_print);
				//SKT::generateUselessbar(originalOutline, useless_print);//20160715_bakcup
				bool longDin = par.getBool("longpage_orshortpage");
				SKT::generateUselessbarDin(originalOutline, useless_print, longDin);
				outlineCTemp.push_back(originalOutline);//**********PUSH_BACK_OUTLINE********************************

				///////cap_temp_change_add_pattern///////////////////

				//////////////////////////////////
				captemp = SKT::resizeXAxis(&captemp, meCorrectPercent);//****0.97
				captemp = SKT::resizeIamge(&captemp, plus_pixel);
				//SKT::generateUselessbar(captemp, useless_print);//20160715_backup
				SKT::generateUselessbarDin(captemp, useless_print, longDin);//20160715_backup
				capCTemp.push_back(captemp);//***********PUSH_BACK_CAPIMAGE******************************************


			}
			if (save_cap_Image){
				//cap_image = SKT::resizeIamge(QImageToCvMat(cap_image));
				QImage cap_Test = cvMatToQImage(captemp);
				img_file_name2.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				cap_Test.save(dir.absolutePath() + "/" + img_file_name2, "png");
			}



			if (par.getBool(("generate_final_picture")))
			{
				if (i < 50)
				{
					QImage ttt(cvMatToQImage(outlineCTemp.back()));
					//QImage ttt(cvMatToQImage(originalOutline));
					QString tttt = "tttt";
					tttt.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
					ttt.save(dir.absolutePath() + "/" + tttt, "png");
					//////////////black_mask///////////////////////////////
					//QImage black_mask_QI(cvMatToQImage(black_Mask));
					////QImage ttt(cvMatToQImage(originalOutline));
					//QString tttt2 = "black_mask_QI";
					//tttt2.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
					//black_mask_QI.save(dir.absolutePath() + "/" + tttt2, "png");


				}
			}




			if (par.getBool("generate_zx") && j % send_page_num == 0)//幾頁傳到已產生的zxfile一次
			{
				//if (zggg)
				//{
				// zggg = new ZxGen(outlineCTemp.begin()->cols, outlineCTemp.begin()->rows, zx_job_pages, md.groove.DimX(), md.groove.DimY() + PLUS_MM, zx_sn.toStdString());//20161219_backup						 
				// 
				//}
				//time.restart();
				zggg->sendData(outlineCTemp, capCTemp);

				capCTemp.clear();
				outlineCTemp.clear();


				if (j == boxy_dim)
				{
					zggg->close();
					zggg->~ZxGen();
				}

				fprintf(dbgff, "zx_gen %i, %i\n", time.elapsed(), i);
				fflush(dbgff);

			}
			else if (par.getBool("generate_zx") && j % send_page_num != 0 && j == boxy_dim)//超過一個zxfile, 頁數不超過1個job
			{
				//if (zggg)
				//{
				// zggg = new ZxGen(outlineCTemp.begin()->cols, outlineCTemp.begin()->rows, zx_job_pages, md.groove.DimX(), md.groove.DimY() + PLUS_MM, zx_sn.toStdString());//20161219_backup						 
				//}
				//time.restart();
				zggg->sendData(outlineCTemp, capCTemp);
				capCTemp.clear();
				outlineCTemp.clear();
				//time.elapsed();
				zggg->close();
				zggg->~ZxGen();

				fprintf(dbgff, "zx_gen %i, %i\n", time.elapsed(), i);
				fflush(dbgff);
			}
			QString tempt = makeOutputname(zx_dir.absolutePath(), j);
			if (j % zx_job_pages == 0)//一個job，產生下一個job
			{
				md.jobname.push_back(tempt);
				zggg->close();
				zggg->~ZxGen();
				zggg = new ZxGen(unsigned int(width), unsigned int(((height*meCorrectPercent) + plus_pixel)), zx_job_pages, md.groove.DimX(), md.groove.DimY() + PLUS_MM, tempt.toStdString(), start_print_b);//20161219_backup
				//zggg = new ZxGen(unsigned int(width), unsigned int(4713), zx_job_pages, md.groove.DimX(), md.groove.DimY() + PLUS_MM, tempt.toStdString(), start_print_b);//20161219_backup

			}



			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();


			//*****test_thread
			emit test_update_pages(i);
			thread1.start();


			/*fprintf(dbgff, "b_name %i\n", i);
			fflush(dbgff);*/

		}
		//thread1.exit();



#endif	

		ctx.unbindReadDrawFramebuffer();
		ctx.release();
		glPopAttrib();
		glContext->doneCurrent();

		//image.rgbSwapped().mirrored().save(par.getSaveFileName("ImageFileName"));
		//image.mirrored().save(par.getSaveFileName("ImageFileName"));
		//cap_image.mirrored().save(par.getSaveFileName("CapImageFileName"));
		//image.save(par.getSaveFileName("ImageFileName"));


		foreach(MeshModel *bm, md.meshList)
		{
			if ((bm->label().contains(olMName, Qt::CaseSensitive) || bm->label().contains(cMName, Qt::CaseSensitive)))
				//if (bm->label().contains(olMName, Qt::CaseSensitive) || bm->label().contains(cMName, Qt::CaseSensitive)))
				//if (bm->getMeshSort()==MeshModel::meshsort::)
			{
				md.delMesh(bm);
			}
		}

		/*if (par.getBool("generate_zx")){

		QString ppath = "cd /d " + getRoamingDir();
		QStringList arguments;
		arguments << "/k" << ppath << "&&" << "usbsend.exe" << "output.zx" << "&&" << "exit";

		}*/



		qDebug() << " done _applyClick ";
		break;
	}
#pragma endregion FP_PRINT_FLOW
#pragma region FP_PRINT_FLOW_2

#pragma endregion FP_PRINT_FLOW_2
#pragma region FP_PRINT_WITH_WHITE_KICK

	case FP_PRINT_FLOW_2:
	{
		WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("FP_PRINT_WITH_WHITE_KICK"));


		md.jobname.clear();
		//***************************


		/*CMeshO & mesh = md.mm()->cm;
		if ((mesh.vn < 3) || (mesh.fn < 1)) return false;

		const unsigned char * p0 = (const unsigned char *)(&(mesh.vert[0].P()));
		const unsigned char * p1 = (const unsigned char *)(&(mesh.vert[1].P()));
		const void *          pbase = p0;
		GLsizei               pstride = GLsizei(p1 - p0);

		const unsigned char * n0 = (const unsigned char *)(&(mesh.vert[0].N()));
		const unsigned char * n1 = (const unsigned char *)(&(mesh.vert[1].N()));
		const void *          nbase = n0;
		GLsizei               nstride = GLsizei(n1 - n0);*/

		/*glContext->doneCurrent();
		glContext->moveToThread(th1);*/

		glContext->makeCurrent();


		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			/* Problem: glewInit failed, something is seriously wrong. */
			//fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
			//qDebug("Error: %s\n", glewGetErrorString(err));
		}

		glPushAttrib(GL_ALL_ATTRIB_BITS);

		Context ctx;
		ctx.acquire();

		if (switchInitMeshVBO)
			initMeshVBO(md);
		/*const GLsizeiptr psize = GLsizeiptr(GLsizei(mesh.vn) * pstride);
		BufferHandle hPositionBuffer = createBuffer(ctx, psize, pbase);

		const GLsizeiptr nsize = GLsizeiptr(GLsizei(mesh.vn) * nstride);
		BufferHandle hNormalBuffer = createBuffer(ctx, nsize, nbase);

		const GLsizeiptr isize = GLsizeiptr(mesh.fn * 3 * sizeof(GLuint));
		BufferHandle hIndexBuffer = createBuffer(ctx, isize);
		{
		BoundIndexBufferHandle indexBuffer = ctx.bindIndexBuffer(hIndexBuffer);

		const CMeshO::VertexType * vbase = &(mesh.vert[0]);
		GLuint *  indices = (GLuint *)indexBuffer->map(GL_WRITE_ONLY);
		for (size_t i = 0; i < mesh.face.size(); ++i)
		{
		const CMeshO::FaceType & f = mesh.face[i];
		if (f.IsD()) continue;
		for (int v = 0; v < 3; ++v)
		{
		*indices++ = GLuint(vcg::tri::Index(mesh, f.cV(v)));
		}
		}
		indexBuffer->unmap();

		ctx.unbindIndexBuffer();
		}*/

		//***20150907
		const GLsizei print_dpi = GLsizei(par.getInt("print_dpi"));
		float x = md.groove.DimX();
		float y = md.groove.DimY();
		const GLsizei width = (int)(x / DSP_inchmm * print_dpi);
		const GLsizei height = (int)(width * y / x);

		//qDebug() << "width" << "height" << width << height;
		//RenderbufferHandle hDepth = createRenderbuffer(ctx, GL_DEPTH_COMPONENT24, width, height);
		RenderbufferHandle hDepth = createRenderbuffer(ctx, GL_DEPTH24_STENCIL8, width, height);
		Texture2DHandle    hColor = createTexture2D(ctx, GL_RGB8, width, height, GL_RGB, GL_UNSIGNED_BYTE);
		//FramebufferHandle  hFramebuffer = createFramebuffer(ctx, renderbufferTarget(hDepth), texture2DTarget(hColor));
		FramebufferHandle  hFramebuffer = createFramebufferWithDepthStencil(ctx, renderbufferTarget(hDepth), renderbufferTarget(hDepth), texture2DTarget(hColor));




		const vcg::Point3f lightDirectionVS = vcg::Point3f(0.0f, 0.0f, -1.0f).Normalize();

		glEnable(GL_DEPTH_TEST);
		//glClearColor(GLfloat(backgroundColor.red())/255.0f, GLfloat(backgroundColor.green())/255.0f, GLfloat(backgroundColor.blue())/255.0f, 0.0f);
		Construction_Groove<float> tempgroove = md.groove;

		glViewport(0, 0, width, height);//***window的長寬				  

		/*const vcg::Point3f center = mesh.bbox.Center();
		const float        scale = 1.0f / mesh.bbox.Diag();*/

		//glScalef(scale, scale, scale);
		//glTranslatef(-center[0], -center[1], -center[2]);

		//***儲存image的基本資訊
		QImage image(int(width), int(height), QImage::Format_RGB888);
		QImage image2(int(width), int(height), QImage::Format_RGB888);
		QImage slice0Point05mmImage(int(width), int(height), QImage::Format_RGB888);
		QImage cap_image(int(width), int(height), QImage::Format_RGB888);
		QImage cap_prePrting_image(int(width), int(height), QImage::Format_RGB888);
		//prePrintobjectC = *new Mat(height,width,CV_8UC3,Scalar(255, 255, 255));
		cv::Mat look_down_color_CV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat look_down_Black_Mask_layer(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat look_down_Black_MaskCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat look_up_Black_MaskCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat look_up_color_CV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat captemp(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat capDownMask(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat black_Mask(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat black_Mask2(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat outlineBlackMaskCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat lookDownBlackTriangleMask(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat outlineColorCv(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat prePrintobjectC(height, width, CV_8UC3, Scalar(255, 255, 255));

		cv::Mat downColorFirstCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat downBlackMaskFirstCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat downColorSecondCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat downBlackMaskSecondCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat prePrintobjectB(height, width, CV_8UC1, Scalar(255));

		cv::Mat originalOutline(height, width, CV_8UC3, Scalar(255, 255, 255));

		//
		/*cv::Mat originalOutline;
		cv::Mat look_down_color_CV;
		cv::Mat look_up_color_CV;
		cv::Mat outlineCV;
		cv::Mat look_down_Black_MaskCV;
		cv::Mat look_up_Black_MaskCV;
		cv::Mat black_Mask;
		cv::Mat black_Mask2;
		cv::Mat innerColorMask;
		cv::Mat upMergeDownCV;
		cv::Mat outlineBlackMaskCV;
		cv::Mat outlineColorCv;
		cv::Mat lookDownBlackTriangleMask;
		cv::Mat prePrintobjectB;
		cv::Mat prePrintobjectC;*/


		//***20150505***slice_func_設置高度
		float box_bottom = md.bbox().min.Z();
		float unit = par.getFloat("slice_height");



		int boxy_dim = qCeil((md.bbox().max.Z() - md.bbox().min.Z()) / unit) + 1;
		//int boxy_dim = qCeil((md.bbox().max.Z() - md.bbox().min.Z()) / unit);//for one page
		//int(mdd->bbox().DimZ() / pJI.layerThickness.value) + 1;
		//Log("boxy_dim %i", boxy_dim);

		init_texture(md);

		//*******************					  

		//***20150507 standard path******************
		//QString temppath = QStandardPaths::locate(QStandardPaths::TempLocation, "", QStandardPaths::LocateDirectory);
		//QString temppath = "D:/temptemp";//backup



		QString temppath = PicaApplication::getRoamingDir();
		QDir dir(temppath);//backup
		if (dir.exists("xyzimage"))
		{
			dir.setPath(temppath + "/xyzimage");
			dir.removeRecursively();
			dir.setPath(temppath);
			dir.mkpath("xyzimage");
		}
		else
		{
			dir.setPath(temppath);
			dir.mkpath("xyzimage");
		}

		dir.setPath(temppath + "/xyzimage");

		//---zx_file------------------------------------------
		//QString zxtemp = "D:/temptemp";
		QString zxtemp = PicaApplication::getRoamingDir();
		QDir zx_dir(zxtemp);
		if (zx_dir.exists("zx_file"))
		{
			zx_dir.setPath(zxtemp + "/zx_file");
			//zx_dir.removeRecursively();
			zx_dir.setPath(zxtemp);
			zx_dir.mkpath("zx_file");
		}
		else
		{
			dir.setPath(temppath);
			dir.mkpath("zx_file");
		}

		zx_dir.setPath(zxtemp + "/zx_file");

		//==============================
		//**********************
		ctx.bindReadDrawFramebuffer(hFramebuffer);
		GLW_CHECK_GL_READ_DRAW_FRAMEBUFFER_STATUS;
#if 1

		const bool outputOL_Image = par.getBool("OL_Image");
		const bool save_cap_Image = par.getBool("Cap_Image");

		const bool faceColor = par.getBool("FaceColor");
		glClearColor(1, 1, 1, 1);
		QString olMName = md.p_setting.getOlMeshName();
		QString cMName = md.p_setting.getCapMeshName();

		//***opencvMat clear
		capCTemp.clear();
		outlineCTemp.clear();


		int zx_job_pages = par.getInt("per_zx_job");
		//int start_page = par.getInt("start_page");
		int send_page_num = par.getInt("send_page_num");
		int plus_pixel = par.getInt("plus_pixel");
		float PLUS_MM = par.getFloat("PLUS_MM");
		bool start_print_b = par.getBool("PRINT_START");
		float useless_print = par.getFloat("USELESS_PRINT");
		float spittoon_M = par.getFloat("SPITTOON_M");
		float spittoon_C = par.getFloat("SPITTOON_C");
		float spittoon_Y = par.getFloat("SPITTOON_Y");
		float spittoon_B = par.getFloat("SPITTOON_B");
		float spittoon_fucking_second_Layer_CMY = par.getFloat("SPITTOON_SECOND_CMY");;
		float spittoon_fucking_second_Layer_B = par.getFloat("SPITTOON_SECOND_B");;

		bool add_pattern = par.getBool("ADD_PATTERN");
		string desProfilePath = par.getString("COLOR_PROFILE").toStdString();
		QFileInfo tempProfilePath(QString::fromStdString(desProfilePath));
		bool doubleprint = false;// par.getBool("Stiff_Print");
		bool dilateBinder = par.getBool("DILATE_BINDER");
		int dilateBinderValue = par.getInt("DILATE_BINDER_VALUE");
		bool dynamicValueSwitch = par.getBool("DYNAMIC_WIPE");
		bool colorBinding = par.getBool("COLOR_BINDING");
		bool doublePrint_V2 = par.getBool("STIFF_PRIN_V2");
		int doublePrint_V2Percent = par.getInt("STIFF_PRINT_VALUE");

		int doublePrint_begin_twenty_layer_V2Percent = par.getInt("PP_BOTTOM_LIGHTER_PERCENTAGE");
		float doublePrint_begin_twenty_layer_V2Thick = par.getFloat("PP_BOTTOM_LIGHTER_THICK");
		//double pp_upperMask = 0.5;//Bottom Ink lighter mm


		int doublePrint_shell_Percent = par.getInt("SHELL_PERCENT");
		bool CMYB_black_mode = par.getBool("CMYB_BLACK");
		int CMYB_black_percentage = par.getInt("CMYB_BLACK_VALUE");
		bool usePatternPercent = false;
		double colorReferenceThick = 0.1;

		/*int spitton_Mode = par.getBool("SPITTON_MODE");
		if (spitton_Mode == 0)
		{
		spittoon_M = 0.5;
		spittoon_C = 0.5;
		spittoon_Y = 0.5;
		spittoon_B = 5;
		}
		else if (spitton_Mode == 1)
		{
		spittoon_M = 3;
		spittoon_C = 3;
		spittoon_Y = 3;
		spittoon_B = 5;
		}*/

		bool prePrintBool = par.getBool("WITNESS_BAR");;//witness bar
		int prePrintBool_Percent = par.getInt("WITNESS_BAR_PERCENT");;//witness bar

		bool fourPage_switch = false;


		bool continuePrinting = par.getBool(("CONTINUE_PRINTING"));
		int start_page = par.getInt("CONTINUE_PRINTING_PAGE");
		int end_page = par.getInt("CONTINUE_PRINTING_END_PAGE");

		if (!tempProfilePath.exists()){
			QMessageBox mBox;
			mBox.setIcon(QMessageBox::Critical);
			mBox.setWindowTitle(tr("PartPro350"));
			mBox.setText(tr("Color Profile not exist\n Stop Printing"));
			mBox.setStandardButtons(QMessageBox::Ok);
			mBox.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
			mBox.exec();
		}
		assert(tempProfilePath.exists());
		//float meCorrectPercent = 0.97;
		float meCorrectPercent = 1;

		//********control profile on && DSP_TEST_MODE
		bool profileOn = true;
		bool saveFinalImage = false;
		bool savedebugImage = false;// par.getBool(("generate_final_picture"));
		//bool line_or_quads = par.getBool("LINE_OR_RECT");;//true line, false quads
		float horizonLineWidth = par.getFloat("HORIZONE_LINE_WIDTH");

		SKT::pixelFormat gen_pixelFormat;
		ZxGen_SKT::pixelFormat genZxPixelFormat;
		ZxGen_SKT::zxInitMode zxCheckMode;
		if (profileOn)
		{
			gen_pixelFormat = SKT::pixelFormat::CMY;
			genZxPixelFormat = ZxGen_SKT::pixelFormat::CMY;
		}
		else
		{
			gen_pixelFormat = SKT::pixelFormat::RGB;
			genZxPixelFormat = ZxGen_SKT::pixelFormat::RGB;
		}
		if (DSP_TEST_MODE)
		{
			zxCheckMode = ZxGen_SKT::zxInitMode::ViewCheckMode;
		}
		else
		{
			zxCheckMode = ZxGen_SKT::zxInitMode::RealPrintMode;

		}
		//ZxGen_SKT::zxInitMode zxCheckMode(ZxGen_SKT::zxInitMode::ViewCheckMode);



		int widthrecord, heightrecord;
		QString blk_img_file_name = "black_outline";

		QString zxFilePath = par.getString("ZX_File_Path");
		QString zx_sn = makeOutputname(zx_dir.absolutePath(), 0);




		md.jobname.push_back(zx_sn);

		//zggg = new ZxGen(unsigned int(width), unsigned int(height), zx_pages, md.groove.DimX(),21, zx_sn.toStdString());//20161219_backup						 
		//zggg = new ZxGen(unsigned int(width), 2580, zx_job_pages, md.groove.DimX(), 21, zx_sn.toStdString());//20161219_backup						 
		//zx_skt = new ZxGen(unsigned int(width), unsigned int(height*meCorrectPercent + plus_pixel), zx_job_pages, md.groove.DimX(), md.groove.DimY() + PLUS_MM, zx_sn.toStdString(), start_print_b);//20161219_backup						 
		if (zxFilePath.isEmpty())
			zx_skt = new ZxGen_SKT(300, zx_job_pages, zx_sn.toStdString(), start_print_b, genZxPixelFormat, zxCheckMode);//20161219_backup						 
		else
			zx_skt = new ZxGen_SKT(300, zx_job_pages, zxFilePath.toStdString(), start_print_b, genZxPixelFormat, zxCheckMode);//20161219_backup						 
		std::vector<cv::Mat> blankTemp;
		std::vector<cv::Mat> blankcapCTemp;


		//if (start_page == 0)
		//{
		WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("newUselessbar_customateBlankPages"));
		if (!zxCheckMode == ZxGen_SKT::zxInitMode::ViewCheckMode  && continuePrinting == false && fourPage_switch)
		{

			SKT::createBlankPages(doubleprint, tempgroove.grooveheight, md.p_setting.get_plus_print_length(), useless_print, DSP_Blank_pages, blankTemp, blankcapCTemp, gen_pixelFormat, spittoon_M, spittoon_C, spittoon_Y, spittoon_B, print_dpi);
			////WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("sendData2"));
			zx_skt->sendData2(blankTemp, blankcapCTemp);
		}
		//}
		//Log("ZxGen width %i height %i ", int(width), int(height*meCorrectPercent + plus_pixel));
		//FILE *dbgff;
		//dbgff = fopen("D:\\debug_open_1.txt", "w");//@@@
		QTime time;
		//clock_t start, stop;
		time.start();

		//***20160622_test_progress bar initializing progress bar status
		/*if (cb != NULL)
		{
		glContext->doneCurrent();
		(*cb)(0, "processing...");
		glContext->makeCurrent();
		}*/

		QDir patternPath(PicaApplication::getRoamingDir());

		if (patternPath.exists("pattern"))
		{
			patternPath.setPath(PicaApplication::getRoamingDir() + "/pattern");
		}
		else
		{
			patternPath.setPath(PicaApplication::getRoamingDir());
			patternPath.mkpath("pattern");
			patternPath.setPath(PicaApplication::getRoamingDir() + "/pattern");
			//patternPath.setPath( "./pattern");

		}

		cv::Mat patternfilter(cv::Size(5, 5), CV_8UC1, Scalar(255));
		if (usePatternPercent)
		{
			SKT::createPatternPercent(patternfilter, 36);

		}


		cv::Mat patternImg;
		patternImg = cv::imread(patternPath.absolutePath().toStdString() + "/hexagon_4.png", CV_LOAD_IMAGE_GRAYSCALE);
		string srcProfilePath = PicaApplication::getRoamingDir().toStdString() + "/ColorProfile/sRGB2014.icc";
		//string desProfilePath = PicaApplication::getRoamingDir().toStdString() + "/ColorProfile/ECI2002CMYKEyeOneL_siriusCMY_70_70_70.icm";
		//string desProfilePath = PicaApplication::getRoamingDir().toStdString() + "/ColorProfile/ECI2002CMYKEyeOneL_siriusCMY_73_76_91_large_paperGray_1ti_coveron.icm";

		//testtimer->start(100);

		//***testLog
		//Log("Closed %i holes and added %i new faces", 12, 12);
		//boxy_dim--;
		int testPrintPageCount;// = 10;
		bool testswitch;// = false;

		int generateNOI = 25;
		int quickWipe = 1;
		WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("for loop start"));
		if (start_page > boxy_dim && continuePrinting){
			zx_skt->close();
			zx_skt->~ZxGen_SKT();
			return false;
		}

		if (continuePrinting)
		{
			testPrintPageCount = end_page - start_page;
			boxy_dim = start_page + testPrintPageCount;
			testswitch = true;
		}
		else
		{
			start_page = 0;
			testswitch = false;
		}

		//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("gen log"));
		for (int i = start_page, j = start_page + 1; i < boxy_dim; i++, j++)

			//for (int i = start_page, j = start_page + 1; i < start_page + testPrintPageCount; i++, j++)
		{
			time.restart();
			resetImage(look_down_Black_Mask_layer, width, height);//
			resetImage(look_down_color_CV, width, height);
			resetImage(look_down_Black_MaskCV, width, height);//
			resetImage(look_up_Black_MaskCV, width, height);//
			resetImage(look_up_color_CV, width, height);
			resetImage(black_Mask, width, height);
			resetImage(black_Mask2, width, height);
			resetImage(lookDownBlackTriangleMask, width, height);//
			resetImage(outlineBlackMaskCV, width, height);//
			resetImage(prePrintobjectC, width, height);


			resetImage(downColorFirstCV, width, height);
			resetImage(downBlackMaskFirstCV, width, height);
			resetImage(downColorSecondCV, width, height);
			resetImage(downBlackMaskSecondCV, width, height);

			resetImage(outlineColorCv, width, height);
			resetImage(captemp, width, height);

			resetImage(capDownMask, width, height);

			md.wipeFlag.push_back(true);
			if (cb && (i % 1) == 0)
			{
				glContext->doneCurrent();
				bool break_test = false;
				if (start_print_b)
				{
					break_test = cb((i * 100) / boxy_dim, "start_printing_slice_process");
					/*if (i % 10 == 0)
					quickWipe = 2;
					else
					quickWipe = 4;*/
					if (i > 1 && dynamicValueSwitch)
					{
						int temp = i - 1 + 5;
						sb(temp, quickWipe);
					}
				}
				else
				{
					break_test = cb((i * 100) / boxy_dim, "slice_process");
					/*if (i % 10 == 0)
					quickWipe = 1;
					else
					quickWipe = 6;*/
					if (i > 1 && dynamicValueSwitch)
						sb(i - 1 + 5, quickWipe);
					//break_test = cb((i , "slice_process");
				}
				if (!break_test)//callback2 return cancel, need to close zx_SKT
				{
					zx_skt->close();
					zx_skt->~ZxGen_SKT();

					glContext->doneCurrent();
					return false;
					break;
				}

			}
			glContext->makeCurrent();
			//xyz::slice_roution(md, box_bottom + unit*i, faceColor);//***backup******************切層演算							  
			//time.restart();
			//Log("Closed %i holes and added %i new faces, %s,%d ", 12, 12, __FUNCTION__, __LINE__);
			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("slice start"));
			xyz::slice_roution(md, box_bottom + unit*i, faceColor);//****20160301*****************切層演算		
			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("slice end"));


			float currnentHeight = box_bottom + unit*i;


			/*fprintf(dbgff, "slice_func %i, %i\n", time.elapsed(), i);
			fflush(dbgff);*/


			//time.restart();
			//***第一張outline圖*****************************
			QVector<MeshModel *> vmm;
			md.getMeshByContainString("_temp_outlines", vmm);
			md.getMeshByContainString(olMName, vmm);
			QVector<MeshModel *> cmm;
			md.getMeshByContainString(cMName, cmm);//capImage
			int xx = 0;
			QString img_file_name = md.p_setting.getoutlineName();
			float cut_z = box_bottom + unit*i;


			glPixelStorei(GL_PACK_ALIGNMENT, 1);//OpenGL PixelStori Setting

			//***start*** test color 0.05 ortho mask***
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			//glClearColor(1, 1, 1, 1);
			//foreach(MeshModel *mdmm, md.meshList)
			//{
			//	//****20151231********************************************************************
			//	if (mdmm->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mdmm->cm.bbox.min.Z() && currnentHeight <= mdmm->cm.bbox.max.Z()))
			//	if (mdmm->rmm.colorMode != GLW::CMNone || mdmm->rmm.textureMode != GLW::TMNone)//如果無色就不須做
			//	if (cut_z - mdmm->cm.bbox.min.Z() < 0 || cut_z - mdmm->cm.bbox.max.Z()>0)//在物體box的上下需要設定限制，才不會超出物體大小。								  
			//	{
			//		glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			//		glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., 0.0001, 0);
			//		glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, cut_z, 0, 0, -DSP_Look_Distination, 0, 1, 0);
			//		glPushMatrix();
			//		mdmm->render(GLW::DMSmooth, mdmm->rmm.colorMode, mdmm->rmm.textureMode);
			//		glPopMatrix();
			//	}
			//	else//物體梯田情況會有0.1層顏色的厚度。
			//	{
			//		glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			//		
			//		glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -DSP_Color_thick, DSP_Color_thick);
			//	
			//		//***20161112
			//		glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
			//		gluLookAt(0, 0, cut_z, 0, 0, -DSP_Look_Distination, 0, 1, 0);//look down		
			//		glPushAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
			//		glShadeModel(GL_SMOOTH);
			//		glDisable(GL_COLOR_MATERIAL);
			//		//glEnable(GL_DEPTH_TEST);
			//		glEnable(GL_LIGHTING);
			//		glEnable(GL_LIGHT0);
			//		glEnable(GL_NORMALIZE);
			//		glDisable(GL_LIGHT1);
			//		float ambient[] = { 0.0, 0.0, 0.0, 1 };
			//		float diffuse[] = { 1, 1, 1, 1 };
			//		float specular[] = { 0, 0, 0, 1 };
			//		GLfloat spec[] = { 1, 0, 0 };
			//		glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
			//		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
			//		glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
			//		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST); //Smooth polygons
			//		glPushMatrix();										
			//		mdmm->render(GLW::DMSmooth, mdmm->rmm.colorMode, mdmm->rmm.textureMode);
			//		glPopMatrix();
			//		glPopAttrib();
			//	}
			//}							
			//glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, look_down_Black_Mask_layer.data);
			//cv::flip(look_down_Black_Mask_layer, look_down_Black_Mask_layer, 0);
			//if (savedebugImage){
			//	image = cvMatToQImage(look_down_Black_Mask_layer);
			//	QString look_down_Black_Mask_layer = "look_down_Black_Mask_layer";
			//	look_down_Black_Mask_layer.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
			//	image.save(dir.absolutePath() + "/" + look_down_Black_Mask_layer, "png");
			//}
			//***end*** test color 0.05 ortho mask***


			//***start***look down first black mask 0.05***
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);
			foreach(MeshModel *mdmm, md.meshList)
			{
				//****20151231********************************************************************

				if (mdmm->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mdmm->cm.bbox.min.Z() && currnentHeight <= mdmm->cm.bbox.max.Z()))
					if (mdmm->rmm.colorMode != GLW::CMNone || mdmm->rmm.textureMode != GLW::TMNone)//如果無色就不須做
						if (cut_z - mdmm->cm.bbox.min.Z() < 0 || cut_z - mdmm->cm.bbox.max.Z() > 0)//在物體box的上下需要設定限制，才不會超出物體大小。								  
						{
							glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
							glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., 0.0001, 0);
							glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, cut_z, 0, 0, -DSP_Look_Distination, 0, 1, 0);

							glPushMatrix();
							/*if (vcg::tri::HasPerWedgeTexCoord(mdmm->cm) && !mdmm->cm.textures.empty())
							mdmm->render(GLW::DMSmooth, GLW::CMNone, GLW::TMPerWedgeMulti);
							else if (vcg::tri::HasPerFaceColor(mdmm->cm))
							mdmm->render(GLW::DMSmooth, GLW::CMPerFace, GLW::TMNone);
							else if (vcg::tri::HasPerVertexColor(mdmm->cm))
							mdmm->render(GLW::DMSmooth, GLW::CMPerVert, GLW::TMNone);*/
							//mdmm->render(GLW::DMSmooth, mdmm->rmm.colorMode, mdmm->rmm.textureMode);
							mdmm->render(GLW::DMSmooth, mdmm->rmm.colorMode, mdmm->rmm.textureMode);

							glPopMatrix();


						}

						else
						{

							glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
							glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., colorReferenceThick, -colorReferenceThick);
							glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();

							gluLookAt(0, 0, cut_z, 0, 0, -DSP_Look_Distination, 0, 1, 0);//look down

							glPushAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
							glShadeModel(GL_SMOOTH);
							glEnable(GL_COLOR_MATERIAL);
							glDisable(GL_LIGHTING);
							glPushMatrix();
							mdmm->render(GLW::DMSmooth, GLW::CMBlack, GLW::TMNone);
							glPopMatrix();
							glPopAttrib();


						}

			}
			//***20150508***BMP QImage設為Format_RGBA8888, readpixel要設為GL_RGBA
			glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, downBlackMaskFirstCV.data);
			cv::flip(downBlackMaskFirstCV, downBlackMaskFirstCV, 0);
			//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("gen log"));
			/*glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image.bits());
			image = image.mirrored(false, true);
			look_down_Black_MaskCV = QImageToCvMat(image);*/
			if (savedebugImage){
				image = cvMatToQImage(downBlackMaskFirstCV);
				QString downBlackMaskFirstCVS = "downBlackMaskFirstCVS";
				downBlackMaskFirstCVS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				image.save(dir.absolutePath() + "/" + downBlackMaskFirstCVS, "png");
			}
			//***end***look down first black mask 0.05***

			//***start***look down first color mask 0.05***
			glMatrixMode(GL_PROJECTION); glPopMatrix();
			glMatrixMode(GL_MODELVIEW); glPopMatrix();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);
			foreach(MeshModel *mdmm, md.meshList)
			{
				if (mdmm->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mdmm->cm.bbox.min.Z() && currnentHeight <= mdmm->cm.bbox.max.Z()))
					if (mdmm->rmm.colorMode != GLW::CMNone || mdmm->rmm.textureMode != GLW::TMNone)//如果無色就不須做
						if (cut_z - mdmm->cm.bbox.min.Z() < 0 || cut_z - mdmm->cm.bbox.max.Z() > 0)//在物體box的上下需要設定限制，才不會超出物體大小。								  
						{
							glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
							glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., 0.0001, 0);
							glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, cut_z, 0, 0, -DSP_Look_Distination, 0, 1, 0);

							glPushMatrix();
							mdmm->render(GLW::DMSmooth, mdmm->rmm.colorMode, mdmm->rmm.textureMode);
							glPopMatrix();
						}
						else
						{
							glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
							//glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -DSP_Color_thick, DSP_Color_thick);
							glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -colorReferenceThick, colorReferenceThick);
							glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();

							gluLookAt(0, 0, cut_z, 0, 0, -DSP_Look_Distination, 0, 1, 0);//look down

							glPushAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
							glShadeModel(GL_SMOOTH);
							glEnable(GL_COLOR_MATERIAL);
							glDisable(GL_LIGHTING);
							glPushMatrix();
							mdmm->render(GLW::DMSmooth, mdmm->rmm.colorMode, mdmm->rmm.textureMode);
							glPopMatrix();
							glPopAttrib();
						}
			}
			//===testing							
			glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, downColorFirstCV.data);
			//glPixelStorei(GL_PACK_ROW_LENGTH, look_down_color_CV.step / look_down_color_CV.elemSize());
			cv::flip(downColorFirstCV, downColorFirstCV, 0);

			//glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image.bits());
			//image = image.mirrored(false, true);
			//look_down_color_CV = QImageToCvMat(image);
			if (savedebugImage)
			{
				image = cvMatToQImage(downColorFirstCV);
				QString downColorFirstCVS = "downColorFirstCVS";
				downColorFirstCVS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				image.save(dir.absolutePath() + "/" + downColorFirstCVS, "png");

				/*QImage image2 = cvMatToQImage(look_down_color_CV);
				QString lookDownColorkMask = "lookDownColorkMask";
				lookDownColorkMask.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				image2.save(dir.absolutePath() + "/" + lookDownColorkMask, "png");*/

			}
			/////////////////===end look down first color mask 0.05=====////////////////////////////

			//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("gen log"));
			/*======start look down second black mask 0.5================*/
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);
			foreach(MeshModel *mdmm, md.meshList)
			{
				//****20151231********************************************************************

				if (mdmm->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mdmm->cm.bbox.min.Z() && currnentHeight <= mdmm->cm.bbox.max.Z()))
					if (mdmm->rmm.colorMode != GLW::CMNone || mdmm->rmm.textureMode != GLW::TMNone)//如果無色就不須做
						if (cut_z - mdmm->cm.bbox.min.Z() < 0 || cut_z - mdmm->cm.bbox.max.Z() > 0)//在物體box的上下需要設定限制，才不會超出物體大小。								  
						{
							glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
							glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., 0.0001, 0);
							glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, cut_z, 0, 0, -DSP_Look_Distination, 0, 1, 0);

							glPushMatrix();
							/*if (vcg::tri::HasPerWedgeTexCoord(mdmm->cm) && !mdmm->cm.textures.empty())
							mdmm->render(GLW::DMSmooth, GLW::CMNone, GLW::TMPerWedgeMulti);
							else if (vcg::tri::HasPerFaceColor(mdmm->cm))
							mdmm->render(GLW::DMSmooth, GLW::CMPerFace, GLW::TMNone);
							else if (vcg::tri::HasPerVertexColor(mdmm->cm))
							mdmm->render(GLW::DMSmooth, GLW::CMPerVert, GLW::TMNone);*/
							//mdmm->render(GLW::DMSmooth, mdmm->rmm.colorMode, mdmm->rmm.textureMode);
							mdmm->render(GLW::DMSmooth, mdmm->rmm.colorMode, mdmm->rmm.textureMode);

							glPopMatrix();


						}

						else//物體梯田情況會有0.1層顏色的厚度。
						{
							glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
							glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., 0.5, -0.5);
							glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();

							gluLookAt(0, 0, cut_z, 0, 0, -DSP_Look_Distination, 0, 1, 0);//look down

							glPushAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
							glShadeModel(GL_SMOOTH);
							glEnable(GL_COLOR_MATERIAL);
							glDisable(GL_LIGHTING);
							glPushMatrix();
							mdmm->render(GLW::DMSmooth, GLW::CMBlack, GLW::TMNone);
							glPopMatrix();
							glPopAttrib();
						}

			}
			//***20150508***BMP QImage設為Format_RGBA8888, readpixel要設為GL_RGBA
			glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, downBlackMaskSecondCV.data);
			cv::flip(downBlackMaskSecondCV, downBlackMaskSecondCV, 0);

			if (savedebugImage){
				image = cvMatToQImage(downBlackMaskSecondCV);
				QString downBlackMaskSecondCVS = "downBlackMaskSecondCVS";
				downBlackMaskSecondCVS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				image.save(dir.absolutePath() + "/" + downBlackMaskSecondCVS, "png");
			}
			/////////////////end look down second black mask 0.5////////////////////////////////////////

			//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("gen log"));
			/***start look down second color mask 0.5****/
			glMatrixMode(GL_PROJECTION); glPopMatrix();
			glMatrixMode(GL_MODELVIEW); glPopMatrix();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);
			foreach(MeshModel *mdmm, md.meshList)
			{
				if (mdmm->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mdmm->cm.bbox.min.Z() && currnentHeight <= mdmm->cm.bbox.max.Z()))
					if (mdmm->rmm.colorMode != GLW::CMNone || mdmm->rmm.textureMode != GLW::TMNone)//如果無色就不須做
						if (cut_z - mdmm->cm.bbox.min.Z() < 0 || cut_z - mdmm->cm.bbox.max.Z() > 0)//在物體box的上下需要設定限制，才不會超出物體大小。								  
						{
							glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
							glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., 0.0001, 0);
							glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, cut_z, 0, 0, -DSP_Look_Distination, 0, 1, 0);

							glPushMatrix();
							mdmm->render(GLW::DMSmooth, mdmm->rmm.colorMode, mdmm->rmm.textureMode);
							glPopMatrix();
						}
						else
						{
							glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();

							glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -0.5, 0.5);
							glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();

							gluLookAt(0, 0, cut_z, 0, 0, -DSP_Look_Distination, 0, 1, 0);//look down

							glPushAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
							glShadeModel(GL_SMOOTH);
							glEnable(GL_COLOR_MATERIAL);
							glDisable(GL_LIGHTING);
							glPushMatrix();
							mdmm->render(GLW::DMSmooth, mdmm->rmm.colorMode, mdmm->rmm.textureMode);
							glPopMatrix();
							glPopAttrib();
						}
			}
			//===testing							
			glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, downColorSecondCV.data);
			//glPixelStorei(GL_PACK_ROW_LENGTH, look_down_color_CV.step / look_down_color_CV.elemSize());
			cv::flip(downColorSecondCV, downColorSecondCV, 0);


			if (savedebugImage)
			{
				image = cvMatToQImage(downColorSecondCV);
				QString downColorSecondCVS = "downColorSecondCVS";
				downColorSecondCVS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				image.save(dir.absolutePath() + "/" + downColorSecondCVS, "png");
			}
			//////////////end look down second color mask 0.5////////////////////////////////

			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("upMergeDownCV"));
			//upMergeDownCV = SKT::upDownCombine(look_up_Black_MaskCV, look_up_color_CV, look_down_Black_MaskCV, look_down_color_CV);
			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("upMergeDownCV"));


			/*if (savedebugImage)
			{
			QImage upMergeDownQT = cvMatToQImage(upMergeDownCV);
			QString upMergeDownQTS = "upMergeDownQTS";
			upMergeDownQTS.append(QString("abb_%1.png").arg(i, 4, 10, QChar('0')));
			upMergeDownQT.save(dir.absolutePath() + "/" + upMergeDownQTS, "png");
			}*/


			/*==============start outline black Mask============================*/
			glMatrixMode(GL_PROJECTION); glPopMatrix();
			glMatrixMode(GL_MODELVIEW); glPopMatrix();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);
			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -1000, 1000);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
			gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, 0, 0, 1, 0);
			foreach(MeshModel *mp, vmm)//outline
			{
				mp->glw.viewerNum = 2;
				mp->outlineThick = horizonLineWidth;
				if (vmm.size() > 0)
				{
					//if (line_or_quads)
					//	mp->render(vcg::GLW::DMTexWire, vcg::GLW::CMBlack, vcg::GLW::TMNone);//backup
					//else
					mp->render(vcg::GLW::DMTexWireQuad, vcg::GLW::CMBlack, vcg::GLW::TMNone);//backup
				}
			}
			glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, outlineBlackMaskCV.data);
			cv::flip(outlineBlackMaskCV, outlineBlackMaskCV, 0);
			/*glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image.bits());
			image = image.mirrored(false, true);
			outlineBlackMaskCV = QImageToCvMat(image);*/

			if (savedebugImage){
				image = cvMatToQImage(outlineBlackMaskCV);
				QString outlineBlackMask = "outlineBlackMask";
				outlineBlackMask.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				image.save(dir.absolutePath() + "/" + outlineBlackMask, "png");
			}
			///////////////////////end outlineBlackMask//////////////////////////

			////***start***lookDownBlackTriangleMask***
			//glMatrixMode(GL_PROJECTION); glPopMatrix();
			//glMatrixMode(GL_MODELVIEW); glPopMatrix();
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			//glClearColor(1, 1, 1, 1);
			//foreach(MeshModel *mdmm, md.meshList)
			//{
			//	//****20151231********************************************************************
			//	if (mdmm->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mdmm->cm.bbox.min.Z() && currnentHeight <= mdmm->cm.bbox.max.Z()))
			//	if (cut_z - mdmm->cm.bbox.min.Z() < 0 || cut_z - mdmm->cm.bbox.max.Z()>0)//在物體box的上下需要設定限制，才不會超出物體大小。								  
			//	{
			//		glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			//		glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., 0.0001, 0);
			//		glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, cut_z, 0, 0, -DSP_Look_Distination, 0, 1, 0);
			//		glPushMatrix();
			//		mdmm->render(GLW::DMSmooth, GLW::CMBlack, GLW::TMNone);
			//		glPopMatrix();
			//	}
			//	else
			//	{
			//		glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			//		glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -DSP_Color_thick, DSP_Color_thick);
			//		glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
			//		gluLookAt(0, 0, cut_z, 0, 0, -DSP_Look_Distination, 0, 1, 0);//look down
			//		glPushAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
			//		glShadeModel(GL_SMOOTH);
			//		glEnable(GL_COLOR_MATERIAL);
			//		glDisable(GL_LIGHTING);
			//		glPushMatrix();
			//		mdmm->render(GLW::DMSmooth, GLW::CMBlack, GLW::TMNone);
			//		glPopMatrix();
			//		glPopAttrib();
			//	}
			//}								
			//glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, lookDownBlackTriangleMask.data);
			//cv::flip(lookDownBlackTriangleMask, lookDownBlackTriangleMask, 0);
			//if (savedebugImage)
			//{
			//	image = cvMatToQImage(lookDownBlackTriangleMask);
			//	QString lookDownColorkMask = "lookDownBlackTriangleMask";
			//	lookDownColorkMask.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
			//	image.save(dir.absolutePath() + "/" + lookDownColorkMask, "png");
			//}
			////***end***lookDownBlackTriangleMask***

			/*==============start outline color ============================*/
			glMatrixMode(GL_PROJECTION); glPopMatrix();
			glMatrixMode(GL_MODELVIEW); glPopMatrix();
			glDisable(GL_LIGHTING);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);
			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -1000, 1000);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
			gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, 0, 0, 1, 0);
			//cmystrips(md, par.getFloat("useless_print") / 3.);//***useless bar

			foreach(MeshModel *mp, vmm)//畫輪廓圖。
			{
				mp->glw.viewerNum = 2;
				mp->outlineThick = horizonLineWidth;
				if (vmm.size() > 0)
				{
					//if (line_or_quads)
					//	mp->render(vcg::GLW::DMTexWire, mp->rmm.colorMode, mp->rmm.textureMode);//backup
					//else
					mp->render(vcg::GLW::DMTexWireQuad, mp->rmm.colorMode, mp->rmm.textureMode);//backup

					//mp->render(vcg::GLW::DMTexWire, mp->rmm.colorMode, vcg::GLW::TMNone);//backup
					//mp->render(vcg::GLW::DMTexWire, vcg::GLW::CMPerVert, mp->rmm.textureMode);//backup
					//mp->render(vcg::GLW::DMTexWire, vcg::GLW::CMPerVert, GLW::TMPerVert);//backup		
					//mp->render(vcg::GLW::DMTexWire, GLW::CMNone, mp->rmm.textureMode);//backup									 
				}
			}

			/*glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image.bits());
			image = image.mirrored(false, true);
			outlineColorCv = QImageToCvMat(image);*/
			glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, outlineColorCv.data);
			cv::flip(outlineColorCv, outlineColorCv, 0);

			if (savedebugImage){
				image = cvMatToQImage(outlineColorCv);
				QString outlineColor = "outlineColor";
				outlineColor.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				image.save(dir.absolutePath() + "/" + outlineColor, "png");
			}
			//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("gen log"));
			///////////////////////end outline color//////////////////////////

			//***第一張圖片
			//if (par.getBool("generate_zx"))
			//{
			//	originalOutline = QImageToCvMat(image.mirrored());
			//	//outlineCTemp.push_back(QImageToCvMat(image.mirrored())); //zxzxzxzxzxzxzxz
			//}
			//if (outputOL_Image)//彩色輪廓
			//{
			//	img_file_name.append(QString("abb_%1.png").arg(i, 4, 10, QChar('0')));
			//	image.mirrored().save(dir.absolutePath() + "/" + img_file_name, "png");
			//}


			/***20160122***outline and binder thick black mask******/
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);

			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			//glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -100, 100);
			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -1000, 1000);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, 0, 0, 1, 0);
			//*********************************
			//binderstrips(md, par.getFloat("useless_print"));//******uselessbar
			//image.fill(QColor::w);
			foreach(MeshModel *mp, vmm)//outline
			{
				mp->glw.viewerNum = 2;
				mp->outlineThick = horizonLineWidth;
				if (vmm.size() > 0)
				{
					//if (line_or_quads)
					//	mp->render(vcg::GLW::DMTexWire, vcg::GLW::CMBlack, vcg::GLW::TMNone);//backup
					//else
					mp->render(vcg::GLW::DMTexWireQuad, vcg::GLW::CMBlack, vcg::GLW::TMNone);//backup

					//mp->render(vcg::GLW::DMOutline_Test, vcg::GLW::CMNone, vcg::GLW::TMNone);
				}
			}
			foreach(MeshModel *mdmm, md.meshList)
			{
				//****20151231********************************************************************								  
				if (mdmm->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mdmm->cm.bbox.min.Z() && currnentHeight <= mdmm->cm.bbox.max.Z()))
					if (cut_z - mdmm->cm.bbox.min.Z() < 0 || cut_z - mdmm->cm.bbox.max.Z() > 0)//在物體box的上下需要設定限制，才不會超出物體大小。
					{
						glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
						glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., 0.0001, 0);
						glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, cut_z, 0, 0, -DSP_Look_Distination, 0, 1, 0);

						mdmm->render(GLW::DMFlat, GLW::CMBlack, GLW::TMNone);
						glPopMatrix();

					}
					else
					{
						glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
						//glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -0.6, 0.6);
						glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -DSP_Binder_thick, DSP_Binder_thick);
						//glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -0.2, 0.2);
						//***20161112
						glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, cut_z, 0, 0, -DSP_Look_Distination, 0, 1, 0);

						glPushMatrix();
						mdmm->render(GLW::DMFlat, GLW::CMBlack, GLW::TMNone);
						glPopMatrix();
					}

			}
			//fFaceColor
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			/*glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image.bits());
			black_Mask = QImageToCvMat(image.mirrored());*/
			glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, black_Mask.data);
			cv::flip(black_Mask, black_Mask, 0);
			//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("gen log"));
			//***20160122***DSP_Color_Replace_Binder_thick_mask

			/******outline and color replace binder thick black mask******/
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);

			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			//glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -100, 100);
			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -1000, 1000);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, 0, 0, 1, 0);
			//*********************************
			//binderstrips(md, par.getFloat("useless_print"));//******uselessbar
			//image.fill(QColor::w);
			foreach(MeshModel *mp, vmm)//outline
			{
				mp->glw.viewerNum = 2;
				mp->outlineThick = horizonLineWidth;
				if (vmm.size() > 0)
				{
					//if (line_or_quads)
					//	mp->render(vcg::GLW::DMTexWire, vcg::GLW::CMBlack, vcg::GLW::TMNone);//backup
					//else
					mp->render(vcg::GLW::DMTexWireQuad, vcg::GLW::CMBlack, vcg::GLW::TMNone);//backup

					//mp->render(vcg::GLW::DMOutline_Test, vcg::GLW::CMNone, vcg::GLW::TMNone);
				}
			}
			foreach(MeshModel *mdmm, md.meshList)
			{
				//****20151231********************************************************************								  
				if (mdmm->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mdmm->cm.bbox.min.Z() && currnentHeight <= mdmm->cm.bbox.max.Z()))
					if (cut_z - mdmm->cm.bbox.min.Z() < 0 || cut_z - mdmm->cm.bbox.max.Z() > 0)//在物體box的上下需要設定限制，才不會超出物體大小。
					{
						glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
						glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., 0.0001, 0);
						glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, cut_z, 0, 0, -DSP_Look_Distination, 0, 1, 0);

						mdmm->render(GLW::DMFlat, GLW::CMBlack, GLW::TMNone);
						glPopMatrix();

					}
					else
					{
						glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
						//glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -0.6, 0.6);
						glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -DSP_Color_Replace_Binder_thick, DSP_Color_Replace_Binder_thick);
						//glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -0.2, 0.2);
						//***20161112
						glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, cut_z, 0, 0, -DSP_Look_Distination, 0, 1, 0);

						glPushMatrix();
						mdmm->render(GLW::DMFlat, GLW::CMBlack, GLW::TMNone);
						glPopMatrix();
					}

			}
			//fFaceColor
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			/*glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image.bits());
			black_Mask2 = QImageToCvMat(image.mirrored());*/
			glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, black_Mask2.data);
			cv::flip(black_Mask2, black_Mask2, 0);
			//////////////end outline and color replace binder thick black mask////////////

			//if (par.getBool("BLK_OUT_LINE"))
			/*if (1)
			{
			QString tttt2 = "black_mask_QI";
			tttt2.append(QString("blk_%1.png").arg(i, 4, 10, QChar('0')));
			image.mirrored().save(dir.absolutePath() + "/" + tttt2, "png");
			}*/

			/*fprintf(dbgff, "render_process %i, %i\n", time.elapsed(), i);
			fflush(dbgff);*/
			//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T(""));
			//===========================================================================================
			equ[3] = box_bottom + unit*i;
			//***第二張圖***產生膠水圖**********//
			//time.restart();
			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -1000, 1000);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, -DSP_Look_Distination, 0, 1, 0);
			//*********************************
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);

			//binderstrips(md, par.getFloat("useless_print"));//***uselessbar

			//************************************************//
			//****** Rendering the mesh's clip edge ****//
			GLuint MY_CLIP_PLANE = GL_CLIP_PLANE0;
			glEnable(MY_CLIP_PLANE);
			glClipPlane(MY_CLIP_PLANE, equ);

			glEnable(GL_STENCIL_TEST);
			glEnable(GL_CULL_FACE);
			glClear(GL_STENCIL_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

			//// first pass: increment stencil buffer value on back faces
			glStencilFunc(GL_ALWAYS, 0, 0);
			glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
			glCullFace(GL_FRONT); // render back faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			//glRotatef(90, 1, 0, 0);
			foreach(MeshModel * mp, md.meshList)
			{
				if (mp->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mp->cm.bbox.min.Z() && currnentHeight <= mp->cm.bbox.max.Z()))
					//mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
					mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
			}

			glPopMatrix();
			glPopAttrib();
			//second pass: decrement stencil buffer value on front faces
			glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
			glCullFace(GL_BACK); // render front faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			//glRotatef(90, 1, 0, 0);
			foreach(MeshModel * mp, md.meshList)
			{
				if (mp->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mp->cm.bbox.min.Z() && currnentHeight <= mp->cm.bbox.max.Z()))
					//mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
					mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
			}
			glPopMatrix();
			glPopAttrib();
			//-----------------------------
			// drawing clip planes masked by stencil buffer content
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glEnable(GL_DEPTH_TEST);
			glDisable(MY_CLIP_PLANE);
			glStencilFunc(GL_NOTEQUAL, 0, ~0);
			// stencil test will pass only when stencil buffer value = 0; 
			// (~0 = 0x11...11)

			glColor4f(1, 1, 1, 1);
			fillsquare();
			glDisable(GL_STENCIL_TEST);
			glEnable(MY_CLIP_PLANE); // enabling clip plane again
			glDisable(GL_LIGHTING);

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			//********************************************************************************************//


			//glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, cap_image.bits());//***20150724
			glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, captemp.data);
			cv::flip(captemp, captemp, 0);

			QString img_file_name2 = md.p_setting.getcapImageName();
			glDisable(MY_CLIP_PLANE);//存完圖
			glDisable(GL_CULL_FACE);
			//****
			//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("gen log"));
			//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T(""));
			if (par.getBool("generate_zx"))
			{
				cv::cvtColor(captemp, captemp, CV_BGR2GRAY);
			}
			if (savedebugImage){
				QImage testttt = cvMatToQImage(captemp);
				QString capimagest = "captemp_image";
				capimagest.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				testttt.save(dir.absolutePath() + "/" + capimagest, "png");
			}


			/*===========================================================================================
			***Binder down Mask  image
			==============================================================================================*/


			equ[3] = box_bottom + unit*i - doublePrint_begin_twenty_layer_V2Thick;

			//time.restart();
			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -1000, 1000);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, -DSP_Look_Distination, 0, 1, 0);
			//*********************************
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);

			//binderstrips(md, par.getFloat("useless_print"));//***uselessbar

			//************************************************//
			//****** Rendering the mesh's clip edge ****//
			GLuint MY_CLIP_PLANE3 = GL_CLIP_PLANE0;
			glEnable(MY_CLIP_PLANE3);
			glClipPlane(MY_CLIP_PLANE3, equ);

			glEnable(GL_STENCIL_TEST);
			glEnable(GL_CULL_FACE);
			glClear(GL_STENCIL_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

			//// first pass: increment stencil buffer value on back faces
			glStencilFunc(GL_ALWAYS, 0, 0);
			glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
			glCullFace(GL_FRONT); // render back faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			//glRotatef(90, 1, 0, 0);
			foreach(MeshModel * mp, md.meshList)
			{
				if (mp->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mp->cm.bbox.min.Z()) && (currnentHeight <= mp->cm.bbox.max.Z()))
					//mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
					mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
			}

			glPopMatrix();
			glPopAttrib();
			//second pass: decrement stencil buffer value on front faces
			glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
			glCullFace(GL_BACK); // render front faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			//glRotatef(90, 1, 0, 0);
			foreach(MeshModel * mp, md.meshList)
			{
				if (mp->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mp->cm.bbox.min.Z() && currnentHeight <= mp->cm.bbox.max.Z()))
					//mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
					mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
			}
			glPopMatrix();
			glPopAttrib();

			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glEnable(GL_DEPTH_TEST);
			glDisable(MY_CLIP_PLANE3);
			glStencilFunc(GL_NOTEQUAL, 0, ~0);


			glColor4f(1, 1, 1, 1);
			fillsquare();
			glDisable(GL_STENCIL_TEST);
			glEnable(MY_CLIP_PLANE3); // enabling clip plane again
			glDisable(GL_LIGHTING);

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			//********************************************************************************************//


			//glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, cap_image.bits());//***20150724
			glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, capDownMask.data);
			cv::flip(capDownMask, capDownMask, 0);

			//QString img_file_name2 = md.p_setting.getcapImageName();
			glDisable(MY_CLIP_PLANE3);
			glDisable(GL_CULL_FACE);
			//****								
			//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T(""));
			/*if (par.getBool("generate_zx"))
			{*/
			cv::cvtColor(capDownMask, capDownMask, CV_BGR2GRAY);
			//}
			if (false){
				QImage capDownMaskkQ = cvMatToQImage(capDownMask);
				QString capDownMaskst = "capDownMask";
				capDownMaskst.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				capDownMaskkQ.save(dir.absolutePath() + "/" + capDownMaskst, "png");
			}
			/*===========================================================================================
			***END  Binder down Mask  image
			==============================================================================================*/




			//===*****PrePrintingObject**********************************************************************************************************===//
			if (prePrintBool)
			{
				//***第二張圖***產生膠水圖**********//
				//time.restart();
				glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
				glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -1000, 1000);
				glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, -DSP_Look_Distination, 0, 1, 0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
				glClearColor(1, 1, 1, 1);
				//****** Rendering the mesh's clip edge ****//
				GLuint MY_CLIP_PLANE2 = GL_CLIP_PLANE0;
				glEnable(MY_CLIP_PLANE2);
				glClipPlane(MY_CLIP_PLANE2, equ);

				glEnable(GL_STENCIL_TEST);
				glEnable(GL_CULL_FACE);
				glClear(GL_STENCIL_BUFFER_BIT);
				glDisable(GL_DEPTH_TEST);
				glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

				//// first pass: increment stencil buffer value on back faces
				glStencilFunc(GL_ALWAYS, 0, 0);
				glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
				glCullFace(GL_FRONT); // render back faces only
				glPushAttrib(GL_ALL_ATTRIB_BITS);
				glEnable(GL_LIGHTING);
				glPushMatrix();

				foreach(MeshModel * mp, md.meshList)
				{
					if (mp->getMeshSort() == MeshModel::meshsort::pre_print_item && (currnentHeight >= mp->cm.bbox.min.Z() && currnentHeight <= mp->cm.bbox.max.Z()))
						mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
				}

				glPopMatrix();
				glPopAttrib();
				//second pass: decrement stencil buffer value on front faces
				glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
				glCullFace(GL_BACK); // render front faces only
				glPushAttrib(GL_ALL_ATTRIB_BITS);
				glEnable(GL_LIGHTING);
				glPushMatrix();

				//glRotatef(90, 1, 0, 0);
				foreach(MeshModel * mp, md.meshList)
				{
					if (mp->getMeshSort() == MeshModel::meshsort::pre_print_item && (currnentHeight >= mp->cm.bbox.min.Z() && currnentHeight <= mp->cm.bbox.max.Z()))
						mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
				}
				glPopMatrix();
				glPopAttrib();
				//-----------------------------
				// drawing clip planes masked by stencil buffer content
				glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
				glEnable(GL_DEPTH_TEST);
				glDisable(MY_CLIP_PLANE2);
				glStencilFunc(GL_NOTEQUAL, 0, ~0);
				// stencil test will pass only when stencil buffer value = 0; 
				// (~0 = 0x11...11)

				glColor4f(1, 1, 1, 1);
				fillsquare();
				glDisable(GL_STENCIL_TEST);
				glEnable(MY_CLIP_PLANE2); // enabling clip plane again
				glDisable(GL_LIGHTING);

				glMatrixMode(GL_MODELVIEW);
				glPopMatrix();
				glMatrixMode(GL_PROJECTION);
				glPopMatrix();
				//********************************************************************************************//

				//glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, cap_prePrting_image.bits());//***20150724



				glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, prePrintobjectC.data);
				cv::flip(prePrintobjectC, prePrintobjectC, 0);
				bitwise_not(prePrintobjectC, prePrintobjectC);
				cv::cvtColor(prePrintobjectC, prePrintobjectB, CV_BGR2GRAY);


				glDisable(MY_CLIP_PLANE2);//存完圖
				glDisable(GL_CULL_FACE);

				if (savedebugImage){
					//if (true){
					QImage ttt(cvMatToQImage(prePrintobjectB));
					QString tttt = "prePrintobjectB";
					tttt.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
					ttt.save(dir.absolutePath() + "/" + tttt, "png");

				}
				/*prePrintobjectC = QImageToCvMat(cap_prePrting_image.mirrored());
				bitwise_not(prePrintobjectC, prePrintobjectC);
				cv::cvtColor(QImageToCvMat(cap_prePrting_image.mirrored()), prePrintobjectB, CV_RGB2GRAY);*/
			}
			//****
			//===*******************************************************************************************===//
			/*fprintf(dbgff, "cap_prePrting_image %i, %i\n", time.elapsed(), i);
			fflush(dbgff);*/
			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T(""));


			//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T(""));


			//originalOutline = SKT::outlinePoint01CMCombine(outlineColorCv, outlineBlackMaskCV, upMergeDownCV, captemp);
			//originalOutline = SKT::outlinePoint01CMCombineVersion2(outlineColorCv, outlineBlackMaskCV, upMergeDownCV, captemp, lookDownBlackTriangleMask);
			//originalOutline = SKT::outlineAndBinderMask(outlineColorCv, outlineBlackMaskCV, captemp, downColorFirstCV, downBlackMaskFirstCV, downColorSecondCV, downBlackMaskSecondCV/*, lookDownBlackTriangleMask*/);


			/*if (savedebugImage){
			QImage tempOutlineqi = cvMatToQImage(outlineColorCv);

			QString tempOutlineSs= "tempOutlineSs";
			tempOutlineSs.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
			tempOutlineqi.save(dir.absolutePath() + "/" + tempOutlineSs, "png");
			}

			if (savedebugImage){
			QImage tempOutlineMaskqi = cvMatToQImage(outlineBlackMaskCV);
			QString tempOutlineMaskS = "tempOutlineMaskSs";
			tempOutlineMaskS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
			tempOutlineMaskqi.save(dir.absolutePath() + "/" + tempOutlineMaskS, "png");
			}*/
			//========================================================//
			cv::Mat reverse_binder_outline(downBlackMaskFirstCV.size(), downBlackMaskFirstCV.type(), Scalar(255, 255, 255));
			originalOutline = SKT::outlineAndBinderMask(outlineColorCv, outlineBlackMaskCV, captemp, downColorFirstCV, downBlackMaskFirstCV, downColorSecondCV, downBlackMaskSecondCV, reverse_binder_outline).clone();
			QImage testttt = cvMatToQImage(originalOutline);
			if (savedebugImage){
				QString originalOutlineS = "originalOutlineS";
				originalOutlineS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				testttt.save(dir.absolutePath() + "/" + originalOutlineS, "png");
			}
			QImage reverse_binder = cvMatToQImage(reverse_binder_outline);
			if (savedebugImage){
				QString reverse_binder_outlineS = "reverse_binder_outlineO";
				reverse_binder_outlineS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				reverse_binder.save(dir.absolutePath() + "/" + reverse_binder_outlineS, "png");
			}

			/*if (savedebugImage){
			image = cvMatToQImage(downBlackMaskFirstCV);
			QString downBlackMaskFirstCVS2 = "downBlackMaskFirstCVS2";
			downBlackMaskFirstCVS2.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
			image.save(dir.absolutePath() + "/" + downBlackMaskFirstCVS2, "png");
			}*/



			//***20160112***image_process加上插點pattern
			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("imagePorcess"));
			if (par.getBool("generate_zx"))
			{
				/*if (par.getBool("mono_bool"))
				{
				image.fill(Qt::white);
				originalOutline = QImageToCvMat(image);
				}*/
				SKT::InkBalance imageProcess;

				cv::Mat temp, temp2;
				//temp = SKT::resizeIamge(&originalOutline,plus_pixel);


				//imageProcess.operate2(temp, captemp, SKT::resizeIamge(&black_Mask, plus_pixel));
				//=====================black_Mask for no color area==================================================
				imageProcess.operate2(originalOutline, captemp, black_Mask, false);
				//SKT::brighterImage(originalOutline);
				//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("yuvBrighterTest"));
				//originalOutline = SKT::yCbCrBrighterTest(originalOutline);

				if (par.getBool("mono_bool"))
					originalOutline = cv::Scalar(255, 255, 255);


				//if (par.getBool(("generate_final_picture")))
				//{
				//	////picasso/////////////////////////////////////////////////
				//	if (i < start_page + generateNOI)
				//	{
				//		//QImage ttt(cvMatToQImage(outlineCTemp.back()));
				//		QImage ttt(cvMatToQImage(originalOutline));
				//		QString tttt = "tttt";
				//		tttt.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				//		ttt.save(dir.absolutePath() + "/" + tttt, "png");
				//		qDebug() << "dir.absolutePath()" << dir.absolutePath();
				//	}
				//}



				WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("profileStart"));


				Mat desImg(originalOutline.size(), originalOutline.type(), Scalar(0, 0, 0));
				//if (profileOn && SKT::ColorTransform::applyColorFromRGBtoCMY(originalOutline, desImg, srcProfilePath.c_str(), desProfilePath.c_str(), 0))
				/*RGB to RGB and turn desImg to CMY
				, then copy to originalOutline,
				ink reduction*/
				cmsHPROFILE htemp = SKT::ColorTransform::OpenStockProfile(desProfilePath.c_str());
				cmsColorSpaceSignature sigTemp = cmsGetColorSpace(htemp);
				cmsCloseProfile(htemp);
				if (sigTemp == cmsSigCmykData)
				{
					if (profileOn && SKT::ColorTransform::applyColorFromRGBtoCMYK_LCMS(originalOutline, desImg, srcProfilePath.c_str(), desProfilePath.c_str(), 0))
					{
						qDebug() << " color profile success" << endl;
						desImg.copyTo(originalOutline);

						/*QImage testttt = cvMatToQImage(desImg);
						QString originalOutlineS = "profile";
						originalOutlineS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
						testttt.save(dir.absolutePath() + "/" + originalOutlineS, "png");*/
					}
				}
				else if (sigTemp == cmsSigRgbData)
				{
					if (profileOn && SKT::ColorTransform::applyColorFromRGBtoRGB_LCMS_RGBProfile(originalOutline, desImg, srcProfilePath.c_str(), desProfilePath.c_str(), 0))
					{
						qDebug() << " RGB color profile success" << endl;
						desImg.copyTo(originalOutline);
						Mat testcolor = SKT::reduc1(originalOutline);
						testcolor.copyTo(originalOutline);
					}
				}
				WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("profileEnd"));

				if (savedebugImage)
				{

					////picasso/////////////////////////////////////////////////
					if (i < start_page + generateNOI)
					{

						//QImage ttt(cvMatToQImage(outlineCTemp.back()));
						QImage ttt(cvMatToQImage(originalOutline));
						QString tttt = "profile";
						tttt.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
						ttt.save(dir.absolutePath() + "/" + tttt, "png");
						qDebug() << "dir.absolutePath()" << dir.absolutePath();
					}
				}
				//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("yuvBrighterTest"));
				//SKT::generateUselessbar(temp);

				//bool innerGrayBool = false;

				///////cap_temp_change_add_pattern///////////////////
				//if (innerGrayBool) //remove 
				cv::Mat innerColorMask = SKT::erodeImage(&black_Mask2, 8);///*****add innerGrayImage(1.
				black_Mask = SKT::erodeImage(&black_Mask, 51);
				cv::Mat getInnerGrayMask;
				if (colorBinding)getInnerGrayMask = SKT::getInnerMask(&captemp, &innerColorMask);///*****add innerGrayImage(4.								


				//captemp = SKT::binderAddPattern(&captemp,&black_Mask);
				//if(((i-60)%100)>100)
				QString papa = patternPath.absolutePath();

				////WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("rect_test1"));
				if ((i % 100) > 20 && add_pattern)
					//if (true)
				{

					if (usePatternPercent)
						captemp = SKT::binderAddPatternPercent(&captemp, &black_Mask, patternfilter);
					else captemp = SKT::binderAddHexagon(&captemp, &black_Mask, patternImg);

					if (colorBinding)SKT::compareWithHexagonImage(&getInnerGrayMask, &captemp);///*****add innerGrayImage(2.

				}

				if (colorBinding)
				{
					//QImage ttt(cvMatToQImage(outlineCTemp.back()));
					if (savedebugImage){
						QImage t4(cvMatToQImage(originalOutline));
						QString tt4 = "before_originalOutline";
						tt4.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
						t4.save(dir.absolutePath() + "/" + tt4, "png");

						QImage ttt(cvMatToQImage(black_Mask2));
						QString tttt = "black_Mask2";
						tttt.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
						ttt.save(dir.absolutePath() + "/" + tttt, "png");

						QImage ttt2(cvMatToQImage(captemp));
						QString tttt2 = "captemp";
						tttt2.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
						ttt2.save(dir.absolutePath() + "/" + tttt2, "png");
					}


					imageProcess.addCMYDots(originalOutline, captemp, innerColorMask);

					if (savedebugImage){
						QImage ttt3(cvMatToQImage(originalOutline));
						QString tttt3 = "after_originalOutline";
						tttt3.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
						ttt3.save(dir.absolutePath() + "/" + tttt3, "png");

						QImage ttt5(cvMatToQImage(captemp));
						QString tttt5 = "after_captemp";
						tttt5.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
						ttt5.save(dir.absolutePath() + "/" + tttt5, "png");
					}
				}


				//Combine With Original Color Image
				//if (innerGrayBool)SKT::combineGrayImageAndColorImage(&originalOutline, &getInnerGrayMask);///*****add innerGrayImage(3.

				//xy_axis now is fine
				//originalOutline = SKT::resizeXAxis(&originalOutline, meCorrectPercent);//****0.97
				if (prePrintBool)
				{
					SKT::mergePrePrinting(originalOutline, prePrintobjectB, prePrintBool_Percent);
					SKT::mergePrePrinting(captemp, prePrintobjectB);


				}
				if (CMYB_black_mode)
				{
					captemp.copyTo(originalOutline);
					cv::cvtColor(originalOutline, originalOutline, cv::COLOR_GRAY2BGR);
					cv::subtract(cv::Scalar::all(255), originalOutline, originalOutline);
					originalOutline = originalOutline*CMYB_black_percentage / 100.;


				}
				originalOutline = SKT::resizeIamgecols(&originalOutline, plus_pixel, gen_pixelFormat);//ADD LENGTH ON COLOR IMAGE									
				SKT::newUselessbar_custom(originalOutline, useless_print, gen_pixelFormat, spittoon_M, spittoon_C, spittoon_Y, spittoon_B, print_dpi);




				captemp = SKT::resizeIamgecols(&captemp, plus_pixel, gen_pixelFormat);
				capDownMask = SKT::resizeIamgecols(&capDownMask, plus_pixel, gen_pixelFormat);
				//SKT::newUselessbar(captemp, useless_print, gen_pixelFormat);//20160715_backup																			
				if (!doubleprint)
				{
					SKT::cutImage(captemp, originalOutline);//white_kick							

					capDownMask = capDownMask(cv::Rect(0, 0, captemp.cols, captemp.rows));
				}

				/*if (true){
				QImage testttt = cvMatToQImage(captemp);
				QString originalOutlineS = "captempp";
				originalOutlineS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				testttt.save(dir.absolutePath() + "/" + originalOutlineS, "png");
				}
				if (true){
				QImage testttt = cvMatToQImage(capDownMask);
				QString originalOutlineS = "capDownMask";
				originalOutlineS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				testttt.save(dir.absolutePath() + "/" + originalOutlineS, "png");
				}
				*/


				if (CMYB_black_mode)
				{
					SKT::BinderPercent(captemp, CMYB_black_percentage);

				}
				/*
				copyCapTemp
				*/
				cv::Mat copyCaptemp;
				copyCaptemp = captemp.clone();

				SKT::newUselessbar_custom(captemp, useless_print, gen_pixelFormat, spittoon_M, spittoon_C, spittoon_Y, spittoon_B, print_dpi);
				//SKT::newUselessbar_custom(capDownMask, useless_print, gen_pixelFormat, spittoon_M, spittoon_C, spittoon_Y, spittoon_B, print_dpi);

				if (dilateBinder)
					captemp = SKT::dilateImage(&captemp, dilateBinderValue);

				//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T(""));
				cv::Mat finalcolor(originalOutline.cols, originalOutline.rows, originalOutline.type(), Scalar(255, 255, 255));//finalcolor.setTo(cv::Scalar(255, 255, 255));
				SKT::rotateImage(originalOutline, finalcolor);

				/*
				rotate capTemp
				*/
				cv::Mat finalBinder(captemp.cols, captemp.rows, captemp.type(), Scalar(255));//finalBinder.setTo(cv::Scalar(255));
				cv::Mat capDownMaskFinal(captemp.cols, captemp.rows, captemp.type(), Scalar(255));//finalBinder.setTo(cv::Scalar(255));
				cv::Mat copyCaptempMaskFinal(captemp.cols, captemp.rows, captemp.type(), Scalar(255));//finalBinder.setTo(cv::Scalar(255));

				SKT::rotateImage(captemp, finalBinder);
				SKT::rotateImage(capDownMask, capDownMaskFinal);
				SKT::rotateImage(copyCaptemp, copyCaptempMaskFinal);


				/*if (true){
				QImage testttt = cvMatToQImage(originalOutline);
				QString originalOutlineS = "finalcolor";
				originalOutlineS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				testttt.save(dir.absolutePath() + "/" + originalOutlineS, "png");
				}*/


				outlineCTemp.push_back(finalcolor);//**********PUSH_BACK_OUTLINE********************************
				if (zxCheckMode == ZxGen_SKT::zxInitMode::ViewCheckMode && !doubleprint)
					finalBinder.setTo(cv::Scalar(255));//SET BINDER TO WHITE
				//if (dialiteBinder)finalBinder = SKT::dilateImage(&finalBinder, 1);
				capCTemp.push_back(finalBinder);//***********PUSH_BACK_CAPIMAGE******************************************

				cv::Mat backbinder(finalBinder.size(), finalBinder.type(), cv::Scalar(255));
				cv::Mat mirrorColor(cv::Size(finalcolor.rows, finalcolor.cols), finalcolor.type(), cv::Scalar(0, 0, 0));
				cv::Mat stiffColor(cv::Size(finalcolor.rows, finalcolor.cols), finalcolor.type(), cv::Scalar(0, 0, 0));
				cv::Mat stiffColor2(cv::Size(finalcolor.cols, finalcolor.rows), finalcolor.type(), cv::Scalar(0, 0, 0));


				//if (doubleprint)
				//{
				//	cv::Mat finalBackBinder(captemp.cols, captemp.rows, captemp.type(), Scalar(255));//finalBinder.setTo(cv::Scalar(255));

				//	reverse_binder_outline = SKT::resizeIamgecols(&reverse_binder_outline, plus_pixel, gen_pixelFormat);
				//	//SKT::newUselessbar(captemp, useless_print, gen_pixelFormat);//20160715_backup										
				//	SKT::newUselessbar_custom(reverse_binder_outline, useless_print, gen_pixelFormat, spittoon_M, spittoon_C, spittoon_Y, spittoon_B, print_dpi);
				//	SKT::rotateImage(reverse_binder_outline, finalBackBinder);


				//	SKT::mirrorImage(finalBackBinder, backbinder);
				//	cv::Mat erodeBinder = SKT::dilateImage(&backbinder, 1);

				//	outlineCTemp.push_back(stiffColor2);
				//	capCTemp.push_back(erodeBinder);

				//	if (savedebugImage){
				//		QImage cap_Test = cvMatToQImage(finalBackBinder);
				//		img_file_name2.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				//		cap_Test.save(dir.absolutePath() + "/" + img_file_name2, "png");
				//	}

				//	/*SKT::mirrorImage(finalBinder, backbinder);
				//	cv::Mat erodeBinder = SKT::dilateImage(&backbinder, 1);
				//	outlineCTemp.push_back(stiffColor2);
				//	capCTemp.push_back(erodeBinder);*/
				//}
				if (doublePrint_V2)
				{

					SKT::newUselessbar_custom(stiffColor, useless_print, gen_pixelFormat, spittoon_fucking_second_Layer_CMY, spittoon_fucking_second_Layer_CMY, spittoon_fucking_second_Layer_CMY, spittoon_fucking_second_Layer_B, print_dpi);
					SKT::rotateImage(stiffColor, stiffColor2);

					copyCaptempMaskFinal.copyTo(backbinder);
					cv::Mat erodeBlackBinder = SKT::dilateImage(&backbinder, 1);

					SKT::BottomLighter_binder(copyCaptempMaskFinal, capDownMaskFinal, erodeBlackBinder, doublePrint_begin_twenty_layer_V2Percent);

					/*if (true){
					QImage testttt = cvMatToQImage(erodeBlackBinder);
					QString originalOutlineS = "erodeBlackBinder";
					originalOutlineS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
					testttt.save(dir.absolutePath() + "/" + originalOutlineS, "png");
					}*/

					/*===============================================
					remain x percent binder in the second printing
					================================================*/
					SKT::BinderPercent(reverse_binder_outline, doublePrint_shell_Percent);
					reverse_binder_outline = SKT::resizeIamgecols(&reverse_binder_outline, plus_pixel, gen_pixelFormat);
					reverse_binder_outline = reverse_binder_outline(cv::Rect(0, 0, backbinder.rows, finalBinder.cols));
					cv::Mat secondOutlineBinder(reverse_binder_outline.cols, reverse_binder_outline.rows, reverse_binder_outline.type(), Scalar(255));//finalBinder.setTo(cv::Scalar(255));
					SKT::newUselessbar_custom(reverse_binder_outline, useless_print, gen_pixelFormat, spittoon_fucking_second_Layer_CMY, spittoon_fucking_second_Layer_CMY, spittoon_fucking_second_Layer_CMY, spittoon_fucking_second_Layer_B, print_dpi);
					SKT::rotateImage(reverse_binder_outline, secondOutlineBinder);

					//combine secondOutlineBinder and erodeBlackBinder to secondOutlineBinder
					SKT::combineBinderImage(secondOutlineBinder, erodeBlackBinder);

					outlineCTemp.push_back(stiffColor2);
					//capCTemp.push_back(erodeBlackBinder);
					capCTemp.push_back(secondOutlineBinder);

					if (saveFinalImage){
						int count_outline_Temp = 0;
						foreach(cv::Mat img, outlineCTemp)
						{

							QImage cap_Test = cvMatToQImage(img);
							QString tttt3 = "outline";
							tttt3.append(QString("_%1_%2.png").arg(i, 4, 10, QChar('0')).arg(count_outline_Temp, 4, 10, QChar('0')));
							cap_Test.save(dir.absolutePath() + "/" + tttt3, "png");
							count_outline_Temp++;
							break;
						}

						int count_cap_Temp = 0;
						foreach(cv::Mat img, capCTemp)
						{

							QImage cap_Test = cvMatToQImage(img);
							QString tttt3 = "captemp";
							tttt3.append(QString("_%1_%2.png").arg(i, 4, 10, QChar('0')).arg(count_cap_Temp, 4, 10, QChar('0')));
							cap_Test.save(dir.absolutePath() + "/" + tttt3, "png");
							count_cap_Temp++;
						}
					}
				}
				/*	int count_outline_Temp = 0;
				foreach(cv::Mat img, outlineCTemp)
				{

				QImage cap_Test = cvMatToQImage(img);
				QString tttt3 = "outline";
				tttt3.append(QString("_%1_%2.png").arg(i, 4, 10, QChar('0')).arg(count_outline_Temp, 4, 10, QChar('0')));
				cap_Test.save(dir.absolutePath() + "/" + tttt3, "png");
				count_outline_Temp++;
				}*/


				//if (doubleprint)//brian
				//{
				//	SKT::mirrorImage(finalcolor, mirrorColor);
				//	cv::Mat mirrorColor2 = SKT::erodeImage(&mirrorColor, 1);
				//	outlineCTemp.push_back(mirrorColor2);
				//	capCTemp.push_back(backbinder);
				//}
				double binderRatio = SKT::imageComplexity(finalBinder);
				if (!doubleprint)
				{
					if (binderRatio > 18)
					{
						quickWipe = 4;
					}
					else if (binderRatio > 9)
					{
						quickWipe = 4;
					}
					else
						quickWipe = 6;
				}
				else
				{
					if (binderRatio > 18)
					{
						quickWipe = 8;
					}
					else if (binderRatio > 9)
					{
						quickWipe = 8;
					}
					else
						quickWipe = 12;

				}


			}


			if (save_cap_Image){

				QImage cap_Test = cvMatToQImage(captemp);
				img_file_name2.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				cap_Test.save(dir.absolutePath() + "/" + img_file_name2, "png");
			}


			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("generate_final_picture"));
			/*
			gen_Final_Picture
			*/
			if (savedebugImage)
				//if (false)
			{
#if 1
				////picasso/////////////////////////////////////////////////
				//if (i < start_page + generateNOI)
									{

										//QImage ttt(cvMatToQImage(outlineCTemp.back()));
										QImage ttt(cvMatToQImage(originalOutline));
										QString tttt = "tttt_originalOutline";
										tttt.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
										ttt.save(dir.absolutePath() + "/" + tttt, "png");
										qDebug() << "dir.absolutePath()" << dir.absolutePath();


										//////////////black_mask///////////////////////////////
										//QImage black_mask_QI(cvMatToQImage(black_Mask));
										////QImage ttt(cvMatToQImage(originalOutline));
										//QString tttt2 = "black_mask_QI";
										//tttt2.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
										//black_mask_QI.save(dir.absolutePath() + "/" + tttt2, "png");
										//////////captemp//////////
										QImage binder_image(cvMatToQImage(captemp));
										//QImage ttt(cvMatToQImage(originalOutline));
										QString binder_image_text = "binder_image";
										binder_image_text.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
										binder_image.save(dir.absolutePath() + "/" + binder_image_text, "png");
										///////////////////////////
									}
#else
				//**picasso_junior****************************
				cv::Mat test = SKT::resizeXYAxis(&originalOutline, 2240, 2240);
				test = SKT::resizeIamge(&test, 10);
				QImage jr_c(cvMatToQImage(test));
				//QImage ttt(cvMatToQImage(originalOutline));
				QString tttt = "tttt";
				tttt.append(QString("_%1.jpg").arg(i, 4, 10, QChar('0')));
				//jr_c.save(dir.absolutePath() + "/" + tttt, "jpg");
				//////////////black_mask///////////////////////////////

				//////////captemp//////////
				cv::Mat test2 = SKT::resizeXYAxis(&captemp, 2240, 2240);
				test2 = SKT::resizeIamge(&test2, 10);
				QImage jr_b(cvMatToQImage(test2));
				QString binder_image_text = "binder_image";
				binder_image_text.append(QString("_%1.jpg").arg(i, 4, 10, QChar('0')));
				//jr_b.save(dir.absolutePath() + "/" + binder_image_text, "jpg");
				///////////////////////////
				QImage jr_f(jr_c.width(), jr_c.height(), QImage::Format_RGBA8888);
				for (int i = 0; i<jr_c.width(); i++)
					for (int j = 0; j<jr_c.height(); j++)
					{
						QRgb value = qRgba(qRed(jr_c.pixel(i, j)), qGreen(jr_c.pixel(i, j)), qBlue(jr_c.pixel(i, j)), 255 - qRed(jr_b.pixel(i, j)));
						jr_f.setPixel(i, j, value);

						QRgb value2 = jr_f.pixel(i, j);
						if (qRed(value2) < 255){
							//qDebug() << "qRGBA" << qRed(value2) << qGreen(value2) << qBlue(value2) << qAlpha(value2);
						}
						//qDebug() << "qRGBAaa" <<  qAlpha(value2);
						//qDebug()<<"qRGBA"<<qRed(value2)<<qGreen(value2)<<qBlue(value2)<<qAlpha(value2);

					}
				QString jrFI = "";
				jrFI.append(QString("%1.tif").arg(i, 4, 10, QChar('0')));
				QString savedir = dir.absolutePath() + "/" + jrFI;
				QImageWriter writer(savedir);
				bool fffff = writer.canWrite();
				QString fffdasdfasdf = writer.errorString();
				writer.setCompression(1);
				writer.write(jr_f);

				//***end_picasso_junior
#endif

			}

			if (doubleprint && savedebugImage){
				QString backimgname = "backImage";
				//QImage backbinderqq = cvMatToQImage(outlineCTemp.back());
				QImage backbinderqq = cvMatToQImage(capCTemp.back());
				backimgname.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				backbinderqq.save(dir.absolutePath() + "/" + backimgname, "png");
			}

			if (par.getBool("generate_zx") && j % send_page_num == 0)//幾頁傳到已產生的zxfile一次
			{
				//if (zggg)
				//{
				// zggg = new ZxGen(outlineCTemp.begin()->cols, outlineCTemp.begin()->rows, zx_job_pages, md.groove.DimX(), md.groove.DimY() + PLUS_MM, zx_sn.toStdString());//20161219_backup						 
				// 
				//}
				//time.restart();
				//zggg->sendData(outlineCTemp, capCTemp);
				bool sendResult = zx_skt->sendData2(outlineCTemp, capCTemp);
				wchar_t *vOut = sendResult ? L"true" : L"false";
				WRITELOG(logger, framework::Diagnostics::LogLevel::Info, (vOut));

				capCTemp.clear();
				outlineCTemp.clear();


				if (j == boxy_dim)
				{
					zx_skt->close();
					zx_skt->~ZxGen_SKT();
					Log("end ZX %i", j);
				}
				else
					if (i == start_page + testPrintPageCount - 1 && testswitch)
					{
						zx_skt->close();
						zx_skt->~ZxGen_SKT();
					}

				//fprintf(dbgff, "zx_gen %i, %i\n", time.elapsed(), i);
				//fflush(dbgff);

			}
			else if (par.getBool("generate_zx") && j % send_page_num != 0 && j == boxy_dim)//超過一個zxfile, 頁數不超過1個job
			{
				//if (zggg)
				//{
				// zggg = new ZxGen(outlineCTemp.begin()->cols, outlineCTemp.begin()->rows, zx_job_pages, md.groove.DimX(), md.groove.DimY() + PLUS_MM, zx_sn.toStdString());//20161219_backup						 
				//}
				//time.restart();
				zx_skt->sendData2(outlineCTemp, capCTemp);
				capCTemp.clear();
				outlineCTemp.clear();
				//time.elapsed();
				zx_skt->close();
				zx_skt->~ZxGen_SKT();
				Log("2end ZX  %i", j);

				//fprintf(dbgff, "zx_gen %i, %i\n", time.elapsed(), i);
				//fflush(dbgff);
			}
			QString tempt = makeOutputname(zx_dir.absolutePath(), j);
			if (j % zx_job_pages == 0)//一個job，產生下一個job
			{
				md.jobname.push_back(tempt);
				zx_skt->close();
				zx_skt->~ZxGen_SKT();
				//zx_skt = new ZxGen_SKT(unsigned int(width), unsigned int(((height*meCorrectPercent) + plus_pixel)), zx_job_pages, md.groove.DimX(), md.groove.DimY() + PLUS_MM, tempt.toStdString(), start_print_b);//20161219_backup
				zx_skt = new ZxGen_SKT(300, zx_job_pages, zx_sn.toStdString(), start_print_b);//20161219_backup
				//zggg = new ZxGen(unsigned int(width), unsigned int(4713), zx_job_pages, md.groove.DimX(), md.groove.DimY() + PLUS_MM, tempt.toStdString(), start_print_b);//20161219_backup

			}

			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("senddata end"));
			char* numpages = new char[20];
			string temp = std::to_string(i).append("_pages");
			strcpy(numpages, temp.c_str());
			wchar_t *wmsg = new wchar_t[strlen(numpages) + 1]; //memory allocation
			mbstowcs(wmsg, numpages, strlen(numpages) + 1);
			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, wmsg);
			delete[]wmsg;
			delete[]numpages;


			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();


			//*****test_thread
			/*emit test_update_pages(i);
			thread1.start();*/


			/*fprintf(dbgff, "b_name %i\n", i);
			fflush(dbgff);*/

			//fprintf(dbgff, "cap_prePrting_image %i, %i\n", time.elapsed(), i);
			//fflush(dbgff);
			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("generate_final_picture"));
		}
		WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("for loop end"));
		//thread1.exit();


#endif	

		ctx.unbindReadDrawFramebuffer();
		ctx.release();
		glPopAttrib();
		glContext->doneCurrent();

		//image.rgbSwapped().mirrored().save(par.getSaveFileName("ImageFileName"));
		//image.mirrored().save(par.getSaveFileName("ImageFileName"));
		//cap_image.mirrored().save(par.getSaveFileName("CapImageFileName"));
		//image.save(par.getSaveFileName("ImageFileName"));


		foreach(MeshModel *bm, md.meshList)
		{
			if ((bm->label().contains(olMName, Qt::CaseSensitive) || bm->label().contains(cMName, Qt::CaseSensitive)))
				//if (bm->label().contains(olMName, Qt::CaseSensitive) || bm->label().contains(cMName, Qt::CaseSensitive)))
				//if (bm->getMeshSort()==MeshModel::meshsort::)
			{
				md.delMesh(bm);
			}
		}

		/*if (par.getBool("generate_zx")){

		QString ppath = "cd /d " + getRoamingDir();
		QStringList arguments;
		arguments << "/k" << ppath << "&&" << "usbsend.exe" << "output.zx" << "&&" << "exit";

		}*/



		qDebug() << " done _applyClick ";
		break;
	}

#pragma endregion FP_PRINT_WITH_WHITE_KICK
#pragma region FP_PRINT_FOUR_IN_ONE
	case FP_PRINT_FOUR_IN_ONE:
	{
		WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("FP_PRINT_WITH_WHITE_KICK"));


		md.jobname.clear();
		//***************************		

		glContext->makeCurrent();


		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			/* Problem: glewInit failed, something is seriously wrong. */
			//fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
			//qDebug("Error: %s\n", glewGetErrorString(err));
		}

		glPushAttrib(GL_ALL_ATTRIB_BITS);

		Context ctx;
		ctx.acquire();

		if (switchInitMeshVBO)
			initMeshVBO(md);

		//***20150907
		const GLsizei print_dpi = GLsizei(par.getInt("print_dpi"));
		float x = md.groove.DimX();
		float y = md.groove.DimY();
		const GLsizei width = (int)(x / DSP_inchmm * print_dpi);
		const GLsizei height = (int)(width * y / x);

		//qDebug() << "width" << "height" << width << height;
		//RenderbufferHandle hDepth = createRenderbuffer(ctx, GL_DEPTH_COMPONENT24, width, height);
		RenderbufferHandle hDepth = createRenderbuffer(ctx, GL_DEPTH24_STENCIL8, width, height);
		Texture2DHandle    hColor = createTexture2D(ctx, GL_RGB8, width, height, GL_RGB, GL_UNSIGNED_BYTE);
		//FramebufferHandle  hFramebuffer = createFramebuffer(ctx, renderbufferTarget(hDepth), texture2DTarget(hColor));
		FramebufferHandle  hFramebuffer = createFramebufferWithDepthStencil(ctx, renderbufferTarget(hDepth), renderbufferTarget(hDepth), texture2DTarget(hColor));




		const vcg::Point3f lightDirectionVS = vcg::Point3f(0.0f, 0.0f, -1.0f).Normalize();

		glEnable(GL_DEPTH_TEST);
		//glClearColor(GLfloat(backgroundColor.red())/255.0f, GLfloat(backgroundColor.green())/255.0f, GLfloat(backgroundColor.blue())/255.0f, 0.0f);
		Construction_Groove<float> tempgroove = md.groove;

		glViewport(0, 0, width, height);//***window的長寬				  

		/*const vcg::Point3f center = mesh.bbox.Center();
		const float        scale = 1.0f / mesh.bbox.Diag();*/

		//glScalef(scale, scale, scale);
		//glTranslatef(-center[0], -center[1], -center[2]);

		//***儲存image的基本資訊
		QImage image(int(width), int(height), QImage::Format_RGB888);
		QImage image2(int(width), int(height), QImage::Format_RGB888);
		QImage slice0Point05mmImage(int(width), int(height), QImage::Format_RGB888);
		QImage cap_image(int(width), int(height), QImage::Format_RGB888);
		QImage cap_prePrting_image(int(width), int(height), QImage::Format_RGB888);
		//prePrintobjectC = *new Mat(height,width,CV_8UC3,Scalar(255, 255, 255));

		cv::Mat look_down_color_CV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat look_down_Black_Mask_layer(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat look_down_Black_MaskCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat look_up_Black_MaskCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat look_up_color_CV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat captemp(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat capDownMask(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat black_Mask(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat black_Mask2(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat outlineBlackMaskCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat lookDownBlackTriangleMask(height, width, CV_8UC3, Scalar(255, 255, 255));

		cv::Mat prePrintobjectC(height, width, CV_8UC3, Scalar(255, 255, 255));

		cv::Mat downColorFirstCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat downBlackMaskFirstCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat downColorSecondCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat downBlackMaskSecondCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat prePrintobjectB(height, width, CV_8UC1, Scalar(255));

		cv::Mat capDownMask_bottom_face(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat capDownMask_top_face(height, width, CV_8UC3, Scalar(255, 255, 255));


		cv::Mat color_Bottom_face(height, width, CV_8UC3, Scalar(255, 255, 255));



		cv::Mat color_Top_face(height, width, CV_8UC3, Scalar(255, 255, 255));



		cv::Mat originalOutline(height, width, CV_8UC3, Scalar(255, 255, 255));


		cv::Mat outlineColorCv(height, width, CV_8UC3, Scalar(255, 255, 255));


		cv::Mat capDownSureMask(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat capTopSureMask(height, width, CV_8UC3, Scalar(255, 255, 255));
		/*look_down_color_CV = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		look_down_Black_Mask_layer = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		look_down_Black_MaskCV = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		look_up_Black_MaskCV = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		look_up_color_CV = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		captemp = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		black_Mask = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		black_Mask2 = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		outlineBlackMaskCV = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		lookDownBlackTriangleMask = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		outlineColorCv = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		prePrintobjectC = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));

		downColorFirstCV = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		downBlackMaskFirstCV = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		downColorSecondCV = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		downBlackMaskSecondCV = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));*/

		//***20150505***slice_func_設置高度
		float box_bottom = md.bbox().min.Z();
		float unit = par.getFloat("slice_height");



		int boxy_dim = qCeil((md.bbox().max.Z() - md.bbox().min.Z()) / unit) + 1;
		//int boxy_dim = qCeil((md.bbox().max.Z() - md.bbox().min.Z()) / unit);//for one page
		//int(mdd->bbox().DimZ() / pJI.layerThickness.value) + 1;
		//Log("boxy_dim %i", boxy_dim);

		init_texture(md);


		QString temppath = PicaApplication::getRoamingDir();
		QDir dir(temppath);//backup
		if (dir.exists("xyzimage"))
		{
			dir.setPath(temppath + "/xyzimage");
			dir.removeRecursively();
			dir.setPath(temppath);
			dir.mkpath("xyzimage");
		}
		else
		{
			dir.setPath(temppath);
			dir.mkpath("xyzimage");
		}

		dir.setPath(temppath + "/xyzimage");

		//---zx_file------------------------------------------
		//QString zxtemp = "D:/temptemp";
		QString zxtemp = PicaApplication::getRoamingDir();
		QDir zx_dir(zxtemp);
		if (zx_dir.exists("zx_file"))
		{
			zx_dir.setPath(zxtemp + "/zx_file");
			//zx_dir.removeRecursively();
			zx_dir.setPath(zxtemp);
			zx_dir.mkpath("zx_file");
		}
		else
		{
			dir.setPath(temppath);
			dir.mkpath("zx_file");
		}

		zx_dir.setPath(zxtemp + "/zx_file");

		//==============================
		//**********************
		ctx.bindReadDrawFramebuffer(hFramebuffer);
		GLW_CHECK_GL_READ_DRAW_FRAMEBUFFER_STATUS;
#if 1

		const bool outputOL_Image = par.getBool("OL_Image");
		const bool save_cap_Image = par.getBool("Cap_Image");

		const bool faceColor = par.getBool("FaceColor");
		glClearColor(1, 1, 1, 1);
		QString olMName = md.p_setting.getOlMeshName();
		QString cMName = md.p_setting.getCapMeshName();

		//***opencvMat clear
		capCTemp.clear();
		outlineCTemp.clear();


		int zx_job_pages = par.getInt("per_zx_job");
		//int start_page = par.getInt("start_page");
		int send_page_num = par.getInt("send_page_num");
		int plus_pixel = par.getInt("plus_pixel");
		float PLUS_MM = par.getFloat("PLUS_MM");
		bool start_print_b = par.getBool("PRINT_START");
		float useless_print = par.getFloat("USELESS_PRINT");
		float spittoon_M = par.getFloat("SPITTOON_M");
		float spittoon_C = par.getFloat("SPITTOON_C");
		float spittoon_Y = par.getFloat("SPITTOON_Y");
		float spittoon_B = par.getFloat("SPITTOON_B");
		bool add_pattern = par.getBool("ADD_PATTERN");
		string desProfilePath = par.getString("COLOR_PROFILE").toStdString();
		QFileInfo tempProfilePath(QString::fromStdString(desProfilePath));
		bool doubleprint = par.getBool("Stiff_Print");
		bool dilateBinder = par.getBool("DILATE_BINDER");
		int dilateBinderValue = par.getInt("DILATE_BINDER_VALUE");
		bool dynamicValueSwitch = par.getBool("DYNAMIC_WIPE");
		bool colorBinding = par.getBool("COLOR_BINDING");
		bool doublePrint_V2 = par.getBool("STIFF_PRIN_V2");
		int doublePrint_V2Percent = par.getInt("STIFF_PRINT_VALUE");
		bool CMYB_black_mode = par.getBool("CMYB_BLACK");
		int CMYB_black_percentage = par.getInt("CMYB_BLACK_VALUE");

		bool Channel_control_mode = par.getBool("CHANNEL_CONTROL");
		bool C_Channel_onOFF = par.getBool("C_CHANNEL");
		bool M_Channel_onOFF = par.getBool("M_CHANNEL");
		bool Y_Channel_onOFF = par.getBool("Y_CHANNEL");
		bool B_Channel_onOFF = par.getBool("B_CHANNEL");


		/*int spitton_Mode = par.getBool("SPITTON_MODE");
		if (spitton_Mode == 0)
		{
		spittoon_M = 0.5;
		spittoon_C = 0.5;
		spittoon_Y = 0.5;
		spittoon_B = 5;
		}
		else if (spitton_Mode == 1)
		{
		spittoon_M = 3;
		spittoon_C = 3;
		spittoon_Y = 3;
		spittoon_B = 5;
		}*/

		bool prePrintBool = true;//witness bar

		bool fourPage_switch = false;


		bool continuePrinting = par.getBool(("CONTINUE_PRINTING"));
		int start_page = par.getInt("CONTINUE_PRINTING_PAGE");
		int end_page = par.getInt("CONTINUE_PRINTING_END_PAGE");


		assert(tempProfilePath.exists());
		//float meCorrectPercent = 0.97;
		float meCorrectPercent = 1;

		//********control profile on && DSP_TEST_MODE
		bool profileOn = true;
		bool savedebugImage = false;// par.getBool(("generate_final_picture"));
		bool line_or_quads = par.getBool("LINE_OR_RECT");;//true line, false quads





		SKT::pixelFormat gen_pixelFormat;
		ZxGen_SKT::pixelFormat genZxPixelFormat;
		ZxGen_SKT::zxInitMode zxCheckMode;
		if (profileOn)
		{
			gen_pixelFormat = SKT::pixelFormat::CMY;
			genZxPixelFormat = ZxGen_SKT::pixelFormat::CMY;
		}
		else
		{
			gen_pixelFormat = SKT::pixelFormat::RGB;
			genZxPixelFormat = ZxGen_SKT::pixelFormat::RGB;
		}
		if (DSP_TEST_MODE)
		{
			zxCheckMode = ZxGen_SKT::zxInitMode::ViewCheckMode;
		}
		else
		{
			zxCheckMode = ZxGen_SKT::zxInitMode::RealPrintMode;

		}
		//ZxGen_SKT::zxInitMode zxCheckMode(ZxGen_SKT::zxInitMode::ViewCheckMode);



		int widthrecord, heightrecord;
		QString blk_img_file_name = "black_outline";

		QString zxFilePath = par.getString("ZX_File_Path");
		QString zx_sn = makeOutputname(zx_dir.absolutePath(), 0);




		md.jobname.push_back(zx_sn);

		if (zxFilePath.isEmpty())
			zx_skt = new ZxGen_SKT(300, zx_job_pages, zx_sn.toStdString(), start_print_b, genZxPixelFormat, zxCheckMode);//20161219_backup						 
		else
			zx_skt = new ZxGen_SKT(300, zx_job_pages, zxFilePath.toStdString(), start_print_b, genZxPixelFormat, zxCheckMode);//20161219_backup						 
		std::vector<cv::Mat> blankTemp;
		std::vector<cv::Mat> blankcapCTemp;


		//if (start_page == 0)
		//{
		WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("newUselessbar_customateBlankPages"));
		if (!zxCheckMode == ZxGen_SKT::zxInitMode::ViewCheckMode  && continuePrinting == false && fourPage_switch)
		{

			SKT::createBlankPages(doubleprint, tempgroove.grooveheight, md.p_setting.get_plus_print_length(), useless_print, DSP_Blank_pages, blankTemp, blankcapCTemp, gen_pixelFormat, spittoon_M, spittoon_C, spittoon_Y, spittoon_B, print_dpi);
			////WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("sendData2"));
			zx_skt->sendData2(blankTemp, blankcapCTemp);
		}
		//}
		//Log("ZxGen width %i height %i ", int(width), int(height*meCorrectPercent + plus_pixel));
		//FILE *dbgff;
		//dbgff = fopen("D:\\debug_open_1.txt", "w");//@@@
		QTime time;
		//clock_t start, stop;
		time.start();



		QDir patternPath(PicaApplication::getRoamingDir());

		if (patternPath.exists("pattern"))
		{
			patternPath.setPath(PicaApplication::getRoamingDir() + "/pattern");
		}
		else
		{
			patternPath.setPath(PicaApplication::getRoamingDir());
			patternPath.mkpath("pattern");
			patternPath.setPath(PicaApplication::getRoamingDir() + "/pattern");
			//patternPath.setPath( "./pattern");

		}
		cv::Mat patternImg;
		patternImg = cv::imread(patternPath.absolutePath().toStdString() + "/hexagon_4.png", CV_LOAD_IMAGE_GRAYSCALE);
		string srcProfilePath = PicaApplication::getRoamingDir().toStdString() + "/ColorProfile/sRGB2014.icc";
		//string desProfilePath = PicaApplication::getRoamingDir().toStdString() + "/ColorProfile/ECI2002CMYKEyeOneL_siriusCMY_70_70_70.icm";
		//string desProfilePath = PicaApplication::getRoamingDir().toStdString() + "/ColorProfile/ECI2002CMYKEyeOneL_siriusCMY_73_76_91_large_paperGray_1ti_coveron.icm";

		//testtimer->start(100);

		//***testLog
		//Log("Closed %i holes and added %i new faces", 12, 12);
		//boxy_dim--;
		int testPrintPageCount;// = 10;
		bool testswitch;// = false;

		int generateNOI = 25;
		int quickWipe = 1;
		WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("for loop start"));
		if (start_page > boxy_dim && continuePrinting){
			zx_skt->close();
			zx_skt->~ZxGen_SKT();
			return false;
		}

		if (continuePrinting)
		{
			testPrintPageCount = end_page - start_page;
			boxy_dim = start_page + testPrintPageCount;
			testswitch = true;
		}
		else
		{
			start_page = 0;
			testswitch = false;
		}

		//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("gen log"));
		for (int i = start_page, j = start_page + 1; i < boxy_dim; i++, j++)

			//for (int i = start_page, j = start_page + 1; i < start_page+testPrintPageCount; i++, j++)
		{
			time.restart();
			resetImage(look_down_Black_Mask_layer, width, height);//
			resetImage(look_down_color_CV, width, height);
			resetImage(look_down_Black_MaskCV, width, height);//
			resetImage(look_up_Black_MaskCV, width, height);//
			resetImage(look_up_color_CV, width, height);
			resetImage(black_Mask, width, height);
			resetImage(black_Mask2, width, height);
			resetImage(lookDownBlackTriangleMask, width, height);//
			resetImage(outlineBlackMaskCV, width, height);//
			resetImage(prePrintobjectC, width, height);


			resetImage(downColorFirstCV, width, height);
			resetImage(downBlackMaskFirstCV, width, height);
			resetImage(downColorSecondCV, width, height);
			resetImage(downBlackMaskSecondCV, width, height);



			resetImage(outlineColorCv, width, height);
			resetImage(captemp, width, height);

			md.wipeFlag.push_back(true);
			if (cb && (i % 1) == 0)
			{
				glContext->doneCurrent();
				bool break_test = false;
				if (start_print_b)
				{
					break_test = cb((i * 100) / boxy_dim, "start_printing_slice_process");
					/*if (i % 10 == 0)
					quickWipe = 2;
					else
					quickWipe = 4;*/
					if (i > 1 && dynamicValueSwitch)
					{
						int temp = i - 1 + 5;
						sb(temp, quickWipe);
					}
				}
				else
				{
					break_test = cb((i * 100) / boxy_dim, "slice_process");
					/*if (i % 10 == 0)
					quickWipe = 1;
					else
					quickWipe = 6;*/
					if (i > 1 && dynamicValueSwitch)
						sb(i - 1 + 5, quickWipe);
					//break_test = cb((i , "slice_process");
				}
				if (!break_test)//callback2 return cancel, need to close zx_SKT
				{
					zx_skt->close();
					zx_skt->~ZxGen_SKT();

					glContext->doneCurrent();
					return false;
					break;
				}

			}
			glContext->makeCurrent();
			float currnentHeight = box_bottom + unit*i;



			//time.restart();
			//********************************		
			QString img_file_name = md.p_setting.getoutlineName();
			float cut_z = box_bottom + unit*i;


			glPixelStorei(GL_PACK_ALIGNMENT, 1);//OpenGL PixelStori Setting							

			//===========================================================================================
			equ[3] = box_bottom + unit*i;
			//***第二張圖***產生膠水圖**********//
			//time.restart();
			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -1000, 1000);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, -DSP_Look_Distination, 0, 1, 0);
			//*********************************
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);

			//binderstrips(md, par.getFloat("useless_print"));//***uselessbar

			//************************************************//
			//****** Rendering the mesh's clip edge ****//
			GLuint MY_CLIP_PLANE = GL_CLIP_PLANE0;
			glEnable(MY_CLIP_PLANE);
			glClipPlane(MY_CLIP_PLANE, equ);

			glEnable(GL_STENCIL_TEST);
			glEnable(GL_CULL_FACE);
			glClear(GL_STENCIL_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

			//// first pass: increment stencil buffer value on back faces
			glStencilFunc(GL_ALWAYS, 0, 0);
			glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
			glCullFace(GL_FRONT); // render back faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			//glRotatef(90, 1, 0, 0);
			foreach(MeshModel * mp, md.meshList)
			{
				if (mp->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mp->cm.bbox.min.Z() && currnentHeight <= mp->cm.bbox.max.Z()))
					//mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
					mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
			}

			glPopMatrix();
			glPopAttrib();
			//second pass: decrement stencil buffer value on front faces
			glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
			glCullFace(GL_BACK); // render front faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			//glRotatef(90, 1, 0, 0);
			foreach(MeshModel * mp, md.meshList)
			{
				if (mp->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mp->cm.bbox.min.Z() && currnentHeight <= mp->cm.bbox.max.Z()))
					//mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
					mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
			}
			glPopMatrix();
			glPopAttrib();
			//-----------------------------
			// drawing clip planes masked by stencil buffer content
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glEnable(GL_DEPTH_TEST);
			glDisable(MY_CLIP_PLANE);
			glStencilFunc(GL_NOTEQUAL, 0, ~0);
			// stencil test will pass only when stencil buffer value = 0; 
			// (~0 = 0x11...11)

			glColor4f(1, 1, 1, 1);
			fillsquare();
			glDisable(GL_STENCIL_TEST);
			glEnable(MY_CLIP_PLANE); // enabling clip plane again
			glDisable(GL_LIGHTING);

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			//********************************************************************************************//


			//glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, cap_image.bits());//***20150724
			glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, captemp.data);
			cv::flip(captemp, captemp, 0);

			QString img_file_name2 = md.p_setting.getcapImageName();
			glDisable(MY_CLIP_PLANE);//存完圖
			glDisable(GL_CULL_FACE);
			//****
			//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("gen log"));
			//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T(""));
			//captemp.copyTo(originalOutline);
			if (par.getBool("generate_zx"))
			{
				cv::cvtColor(captemp, captemp, CV_BGR2GRAY);
			}
			if (savedebugImage){
				QImage testttt = cvMatToQImage(captemp);
				QString capimagest = "captemp_image";
				capimagest.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				testttt.save(dir.absolutePath() + "/" + capimagest, "png");
			}


			//===*****PrePrintingObject**********************************************************************************************************===//
			if (prePrintBool)
			{
				//***第二張圖***產生膠水圖**********//
				//time.restart();
				glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
				glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -1000, 1000);
				glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, -DSP_Look_Distination, 0, 1, 0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
				glClearColor(1, 1, 1, 1);
				//****** Rendering the mesh's clip edge ****//
				GLuint MY_CLIP_PLANE2 = GL_CLIP_PLANE0;
				glEnable(MY_CLIP_PLANE2);
				glClipPlane(MY_CLIP_PLANE2, equ);

				glEnable(GL_STENCIL_TEST);
				glEnable(GL_CULL_FACE);
				glClear(GL_STENCIL_BUFFER_BIT);
				glDisable(GL_DEPTH_TEST);
				glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

				//// first pass: increment stencil buffer value on back faces
				glStencilFunc(GL_ALWAYS, 0, 0);
				glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
				glCullFace(GL_FRONT); // render back faces only
				glPushAttrib(GL_ALL_ATTRIB_BITS);
				glEnable(GL_LIGHTING);
				glPushMatrix();

				foreach(MeshModel * mp, md.meshList)
				{
					if (mp->getMeshSort() == MeshModel::meshsort::pre_print_item && (currnentHeight >= mp->cm.bbox.min.Z() && currnentHeight <= mp->cm.bbox.max.Z()))
						mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
				}

				glPopMatrix();
				glPopAttrib();
				//second pass: decrement stencil buffer value on front faces
				glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
				glCullFace(GL_BACK); // render front faces only
				glPushAttrib(GL_ALL_ATTRIB_BITS);
				glEnable(GL_LIGHTING);
				glPushMatrix();

				//glRotatef(90, 1, 0, 0);
				foreach(MeshModel * mp, md.meshList)
				{
					if (mp->getMeshSort() == MeshModel::meshsort::pre_print_item && (currnentHeight >= mp->cm.bbox.min.Z() && currnentHeight <= mp->cm.bbox.max.Z()))
						mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
				}
				glPopMatrix();
				glPopAttrib();
				//-----------------------------
				// drawing clip planes masked by stencil buffer content
				glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
				glEnable(GL_DEPTH_TEST);
				glDisable(MY_CLIP_PLANE2);
				glStencilFunc(GL_NOTEQUAL, 0, ~0);
				// stencil test will pass only when stencil buffer value = 0; 
				// (~0 = 0x11...11)

				glColor4f(1, 1, 1, 1);
				fillsquare();
				glDisable(GL_STENCIL_TEST);
				glEnable(MY_CLIP_PLANE2); // enabling clip plane again
				glDisable(GL_LIGHTING);

				glMatrixMode(GL_MODELVIEW);
				glPopMatrix();
				glMatrixMode(GL_PROJECTION);
				glPopMatrix();
				//********************************************************************************************//

				//glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, cap_prePrting_image.bits());//***20150724



				glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, prePrintobjectC.data);
				cv::flip(prePrintobjectC, prePrintobjectC, 0);
				bitwise_not(prePrintobjectC, prePrintobjectC);
				cv::cvtColor(prePrintobjectC, prePrintobjectB, CV_BGR2GRAY);


				glDisable(MY_CLIP_PLANE2);//存完圖
				glDisable(GL_CULL_FACE);

				if (savedebugImage){
					//if (true){
					QImage ttt(cvMatToQImage(prePrintobjectB));
					QString tttt = "prePrintobjectB";
					tttt.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
					ttt.save(dir.absolutePath() + "/" + tttt, "png");

				}
				/*prePrintobjectC = QImageToCvMat(cap_prePrting_image.mirrored());
				bitwise_not(prePrintobjectC, prePrintobjectC);
				cv::cvtColor(QImageToCvMat(cap_prePrting_image.mirrored()), prePrintobjectB, CV_RGB2GRAY);*/
			}
			//****
			//===*******************************************************************************************===//
			/*fprintf(dbgff, "cap_prePrting_image %i, %i\n", time.elapsed(), i);
			fflush(dbgff);*/
			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T(""));


			//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T(""));				
			//========================================================//				
			QImage testttt = cvMatToQImage(originalOutline);
			if (false){
				QString originalOutlineS = "originalOutlineS";
				originalOutlineS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				testttt.save(dir.absolutePath() + "/" + originalOutlineS, "png");
			}



			//***20160112***image_process加上插點pattern
			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("imagePorcess"));
			if (par.getBool("generate_zx"))
			{

				if (Channel_control_mode)
				{
					captemp.copyTo(originalOutline);
					cv::cvtColor(originalOutline, originalOutline, cv::COLOR_GRAY2BGR);
					cv::subtract(cv::Scalar::all(255), originalOutline, originalOutline);
					SKT::createColorImage(originalOutline, captemp, C_Channel_onOFF, M_Channel_onOFF, Y_Channel_onOFF);


				}
				originalOutline = SKT::resizeIamgecols(&originalOutline, plus_pixel, gen_pixelFormat);//ADD LENGTH ON COLOR IMAGE									
				SKT::newUselessbar_custom(originalOutline, useless_print, gen_pixelFormat, spittoon_M, spittoon_C, spittoon_Y, spittoon_B, print_dpi);

				//////////////////////////////////


				captemp = SKT::resizeIamgecols(&captemp, plus_pixel, gen_pixelFormat);
				//SKT::newUselessbar(captemp, useless_print, gen_pixelFormat);//20160715_backup																			
				if (!doubleprint)
					SKT::cutImage(captemp, originalOutline);//white_kick

				/* if (CMYB_black_mode)
				{
				SKT::BinderPercent(captemp, CMYB_black_percentage);
				}*/
				SKT::newUselessbar_custom(captemp, useless_print, gen_pixelFormat, spittoon_M, spittoon_C, spittoon_Y, spittoon_B, print_dpi);

				if (dilateBinder)
					captemp = SKT::dilateImage(&captemp, dilateBinderValue);

				//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T(""));
				cv::Mat finalcolor(originalOutline.cols, originalOutline.rows, originalOutline.type(), Scalar(255, 255, 255));//finalcolor.setTo(cv::Scalar(255, 255, 255));
				SKT::rotateImage(originalOutline, finalcolor);
				cv::Mat finalBinder(captemp.cols, captemp.rows, captemp.type(), Scalar(255));//finalBinder.setTo(cv::Scalar(255));
				SKT::rotateImage(captemp, finalBinder);


				outlineCTemp.push_back(finalcolor);//**********PUSH_BACK_OUTLINE********************************
				if (zxCheckMode == ZxGen_SKT::zxInitMode::ViewCheckMode && !doubleprint)
					finalBinder.setTo(cv::Scalar(255));//SET BINDER TO WHITE
				if (!B_Channel_onOFF)
					finalBinder.setTo(cv::Scalar(255));//SET BINDER TO WHITE

				capCTemp.push_back(finalBinder);//***********PUSH_BACK_CAPIMAGE******************************************
				cv::Mat backbinder(finalBinder.size(), finalBinder.type(), cv::Scalar(255));
				cv::Mat mirrorColor(cv::Size(finalcolor.rows, finalcolor.cols), finalcolor.type(), cv::Scalar(0, 0, 0));
				cv::Mat stiffColor(cv::Size(finalcolor.rows, finalcolor.cols), finalcolor.type(), cv::Scalar(0, 0, 0));
				cv::Mat stiffColor2(cv::Size(finalcolor.cols, finalcolor.rows), finalcolor.type(), cv::Scalar(0, 0, 0));


				if (doublePrint_V2)
				{

					SKT::newUselessbar_custom(stiffColor, useless_print, gen_pixelFormat, spittoon_M, spittoon_C, spittoon_Y, spittoon_B, print_dpi);
					SKT::rotateImage(stiffColor, stiffColor2);

					finalBinder.copyTo(backbinder);
					cv::Mat erodeBlackBinder = SKT::dilateImage(&backbinder, 1);
					SKT::BinderPercent(erodeBlackBinder, doublePrint_V2Percent);

					outlineCTemp.push_back(stiffColor2);
					capCTemp.push_back(erodeBlackBinder);


					if (false){
						QImage cap_Test = cvMatToQImage(backbinder);
						img_file_name2.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
						cap_Test.save(dir.absolutePath() + "/" + img_file_name2, "png");
					}
				}

			}


			if (save_cap_Image){

				QImage cap_Test = cvMatToQImage(captemp);
				img_file_name2.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				cap_Test.save(dir.absolutePath() + "/" + img_file_name2, "png");
			}

			/*int count_outline_Temp = 0;
			foreach(cv::Mat img, outlineCTemp)
			{

			QImage cap_Test = cvMatToQImage(img);
			QString tttt3 = "outline";
			tttt3.append(QString("_%1_%2.png").arg(i, 4, 10, QChar('0')).arg(count_outline_Temp, 4, 10, QChar('0')));
			cap_Test.save(dir.absolutePath() + "/" + tttt3, "png");
			count_outline_Temp++;
			}

			int count_cap_Temp = 0;
			foreach(cv::Mat img, capCTemp)
			{

			QImage cap_Test = cvMatToQImage(img);
			QString tttt3 = "captemp";
			tttt3.append(QString("_%1_%2.png").arg(i, 4, 10, QChar('0')).arg(count_cap_Temp, 4, 10, QChar('0')));
			cap_Test.save(dir.absolutePath() + "/" + tttt3, "png");
			count_cap_Temp++;
			}*/





			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("generate_final_picture"));
			if (savedebugImage)
				//if (true)
			{
#if 1

				//QImage ttt(cvMatToQImage(outlineCTemp.back()));
				QImage ttt(cvMatToQImage(originalOutline));
				QString tttt = "tttt_originalOutline";
				tttt.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				ttt.save(dir.absolutePath() + "/" + tttt, "png");
				qDebug() << "dir.absolutePath()" << dir.absolutePath();


				//////////////black_mask///////////////////////////////
				//QImage black_mask_QI(cvMatToQImage(black_Mask));
				////QImage ttt(cvMatToQImage(originalOutline));
				//QString tttt2 = "black_mask_QI";
				//tttt2.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				//black_mask_QI.save(dir.absolutePath() + "/" + tttt2, "png");
				//////////captemp//////////
				QImage binder_image(cvMatToQImage(captemp));
				//QImage ttt(cvMatToQImage(originalOutline));
				QString binder_image_text = "binder_image";
				binder_image_text.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				binder_image.save(dir.absolutePath() + "/" + binder_image_text, "png");
				///////////////////////////

#else
				//**picasso_junior****************************
				cv::Mat test = SKT::resizeXYAxis(&originalOutline, 2240, 2240);
				test = SKT::resizeIamge(&test, 10);
				QImage jr_c(cvMatToQImage(test));
				//QImage ttt(cvMatToQImage(originalOutline));
				QString tttt = "tttt";
				tttt.append(QString("_%1.jpg").arg(i, 4, 10, QChar('0')));
				//jr_c.save(dir.absolutePath() + "/" + tttt, "jpg");
				//////////////black_mask///////////////////////////////

				//////////captemp//////////
				cv::Mat test2 = SKT::resizeXYAxis(&captemp, 2240, 2240);
				test2 = SKT::resizeIamge(&test2, 10);
				QImage jr_b(cvMatToQImage(test2));
				QString binder_image_text = "binder_image";
				binder_image_text.append(QString("_%1.jpg").arg(i, 4, 10, QChar('0')));
				//jr_b.save(dir.absolutePath() + "/" + binder_image_text, "jpg");
				///////////////////////////
				QImage jr_f(jr_c.width(), jr_c.height(), QImage::Format_RGBA8888);
				for (int i = 0; i < jr_c.width(); i++)
					for (int j = 0; j < jr_c.height(); j++)
					{
						QRgb value = qRgba(qRed(jr_c.pixel(i, j)), qGreen(jr_c.pixel(i, j)), qBlue(jr_c.pixel(i, j)), 255 - qRed(jr_b.pixel(i, j)));
						jr_f.setPixel(i, j, value);

						QRgb value2 = jr_f.pixel(i, j);
						if (qRed(value2) < 255){
							//qDebug() << "qRGBA" << qRed(value2) << qGreen(value2) << qBlue(value2) << qAlpha(value2);
						}
						//qDebug() << "qRGBAaa" <<  qAlpha(value2);
						//qDebug()<<"qRGBA"<<qRed(value2)<<qGreen(value2)<<qBlue(value2)<<qAlpha(value2);

					}
				QString jrFI = "";
				jrFI.append(QString("%1.tif").arg(i, 4, 10, QChar('0')));
				QString savedir = dir.absolutePath() + "/" + jrFI;
				QImageWriter writer(savedir);
				bool fffff = writer.canWrite();
				QString fffdasdfasdf = writer.errorString();
				writer.setCompression(1);
				writer.write(jr_f);

				//***end_picasso_junior
#endif

			}

			if (par.getBool("generate_zx") && j % send_page_num == 0)//幾頁傳到已產生的zxfile一次
			{
				//if (zggg)
				//{
				// zggg = new ZxGen(outlineCTemp.begin()->cols, outlineCTemp.begin()->rows, zx_job_pages, md.groove.DimX(), md.groove.DimY() + PLUS_MM, zx_sn.toStdString());//20161219_backup						 
				// 
				//}
				//time.restart();
				//zggg->sendData(outlineCTemp, capCTemp);
				bool sendResult = zx_skt->sendData2(outlineCTemp, capCTemp);
				wchar_t *vOut = sendResult ? L"true" : L"false";
				WRITELOG(logger, framework::Diagnostics::LogLevel::Info, (vOut));

				capCTemp.clear();
				outlineCTemp.clear();


				if (j == boxy_dim)
				{
					zx_skt->close();
					zx_skt->~ZxGen_SKT();
					Log("end ZX %i", j);
				}
				else
					if (i == start_page + testPrintPageCount - 1 && testswitch)
					{
						zx_skt->close();
						zx_skt->~ZxGen_SKT();
					}

				//fprintf(dbgff, "zx_gen %i, %i\n", time.elapsed(), i);
				//fflush(dbgff);

			}
			else if (par.getBool("generate_zx") && j % send_page_num != 0 && j == boxy_dim)//超過一個zxfile, 頁數不超過1個job
			{
				//if (zggg)
				//{
				// zggg = new ZxGen(outlineCTemp.begin()->cols, outlineCTemp.begin()->rows, zx_job_pages, md.groove.DimX(), md.groove.DimY() + PLUS_MM, zx_sn.toStdString());//20161219_backup						 
				//}
				//time.restart();
				zx_skt->sendData2(outlineCTemp, capCTemp);
				capCTemp.clear();
				outlineCTemp.clear();
				//time.elapsed();
				zx_skt->close();
				zx_skt->~ZxGen_SKT();
				Log("2end ZX  %i", j);

				//fprintf(dbgff, "zx_gen %i, %i\n", time.elapsed(), i);
				//fflush(dbgff);
			}
			QString tempt = makeOutputname(zx_dir.absolutePath(), j);
			if (j % zx_job_pages == 0)//一個job，產生下一個job
			{
				md.jobname.push_back(tempt);
				zx_skt->close();
				zx_skt->~ZxGen_SKT();
				//zx_skt = new ZxGen_SKT(unsigned int(width), unsigned int(((height*meCorrectPercent) + plus_pixel)), zx_job_pages, md.groove.DimX(), md.groove.DimY() + PLUS_MM, tempt.toStdString(), start_print_b);//20161219_backup
				zx_skt = new ZxGen_SKT(300, zx_job_pages, zx_sn.toStdString(), start_print_b);//20161219_backup
				//zggg = new ZxGen(unsigned int(width), unsigned int(4713), zx_job_pages, md.groove.DimX(), md.groove.DimY() + PLUS_MM, tempt.toStdString(), start_print_b);//20161219_backup

			}

			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("senddata end"));
			char* numpages = new char[20];
			string temp = std::to_string(i).append("_pages");
			strcpy(numpages, temp.c_str());
			wchar_t *wmsg = new wchar_t[strlen(numpages) + 1]; //memory allocation
			mbstowcs(wmsg, numpages, strlen(numpages) + 1);
			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, wmsg);
			delete[]wmsg;
			delete[]numpages;


			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();


			//*****test_thread
			/*emit test_update_pages(i);
			thread1.start();*/


			/*fprintf(dbgff, "b_name %i\n", i);
			fflush(dbgff);*/

			//fprintf(dbgff, "cap_prePrting_image %i, %i\n", time.elapsed(), i);
			//fflush(dbgff);
			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("generate_final_picture"));
		}
		WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("for loop end"));
		//thread1.exit();


#endif	

		ctx.unbindReadDrawFramebuffer();
		ctx.release();
		glPopAttrib();
		glContext->doneCurrent();

		//image.rgbSwapped().mirrored().save(par.getSaveFileName("ImageFileName"));
		//image.mirrored().save(par.getSaveFileName("ImageFileName"));
		//cap_image.mirrored().save(par.getSaveFileName("CapImageFileName"));
		//image.save(par.getSaveFileName("ImageFileName"));


		foreach(MeshModel *bm, md.meshList)
		{
			if ((bm->label().contains(olMName, Qt::CaseSensitive) || bm->label().contains(cMName, Qt::CaseSensitive)))
				//if (bm->label().contains(olMName, Qt::CaseSensitive) || bm->label().contains(cMName, Qt::CaseSensitive)))
				//if (bm->getMeshSort()==MeshModel::meshsort::)
			{
				md.delMesh(bm);
			}
		}

		/*if (par.getBool("generate_zx")){

		QString ppath = "cd /d " + getRoamingDir();
		QStringList arguments;
		arguments << "/k" << ppath << "&&" << "usbsend.exe" << "output.zx" << "&&" << "exit";

		}*/



		qDebug() << " done _applyClick ";
		break;
	}
#pragma endregion FP_PRINT_FOUR_IN_ONE
#pragma region FP_PRINT_DM_SLICER
	case FP_PRINT_DM_SLICER:
	{
		WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("FP_PRINT_DM_SLICER"));


		md.jobname.clear();
		//***************************													

		glContext->makeCurrent();


		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("glew_error"));
			/* Problem: glewInit failed, something is seriously wrong. */
			//fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
			//qDebug("Error: %s\n", glewGetErrorString(err));
		}

		glPushAttrib(GL_ALL_ATTRIB_BITS);

		Context ctx;
		ctx.acquire();

		if (switchInitMeshVBO)
			initMeshVBO(md);
		//***20150907
		const GLsizei print_dpi = GLsizei(par.getInt("print_dpi"));
		float x = md.groove.DimX();
		float y = md.groove.DimY();
		const GLsizei width = (int)(x / DSP_inchmm * print_dpi);
		const GLsizei height = (int)(width * y / x);

		//qDebug() << "width" << "height" << width << height;
		//RenderbufferHandle hDepth = createRenderbuffer(ctx, GL_DEPTH_COMPONENT24, width, height);
		RenderbufferHandle hDepth = createRenderbuffer(ctx, GL_DEPTH24_STENCIL8, width, height);
		Texture2DHandle    hColor = createTexture2D(ctx, GL_RGB8, width, height, GL_RGB, GL_UNSIGNED_BYTE);
		//FramebufferHandle  hFramebuffer = createFramebuffer(ctx, renderbufferTarget(hDepth), texture2DTarget(hColor));
		FramebufferHandle  hFramebuffer = createFramebufferWithDepthStencil(ctx, renderbufferTarget(hDepth), renderbufferTarget(hDepth), texture2DTarget(hColor));




		const vcg::Point3f lightDirectionVS = vcg::Point3f(0.0f, 0.0f, -1.0f).Normalize();

		glEnable(GL_DEPTH_TEST);
		//glClearColor(GLfloat(backgroundColor.red())/255.0f, GLfloat(backgroundColor.green())/255.0f, GLfloat(backgroundColor.blue())/255.0f, 0.0f);
		Construction_Groove<float> tempgroove = md.groove;

		glViewport(0, 0, width, height);//***window的長寬				  

		/*const vcg::Point3f center = mesh.bbox.Center();
		const float        scale = 1.0f / mesh.bbox.Diag();*/

		//glScalef(scale, scale, scale);
		//glTranslatef(-center[0], -center[1], -center[2]);

		//***儲存image的基本資訊
		QImage image(int(width), int(height), QImage::Format_RGB888);
		QImage image2(int(width), int(height), QImage::Format_RGB888);
		QImage slice0Point05mmImage(int(width), int(height), QImage::Format_RGB888);
		QImage cap_image(int(width), int(height), QImage::Format_RGB888);
		QImage cap_prePrting_image(int(width), int(height), QImage::Format_RGB888);
		//prePrintobjectC = *new Mat(height,width,CV_8UC3,Scalar(255, 255, 255));
		/*look_down_color_CV = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		look_down_Black_Mask_layer = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		look_down_Black_MaskCV = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		look_up_Black_MaskCV = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		look_up_color_CV = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		captemp = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		capDownMask = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		black_Mask = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		black_Mask2 = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		outlineBlackMaskCV = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		lookDownBlackTriangleMask = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		outlineColorCv = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		prePrintobjectC = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));

		downColorFirstCV = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		downBlackMaskFirstCV = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		downColorSecondCV = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		downBlackMaskSecondCV = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));

		up_downBlackMaskCV = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));*/

		cv::Mat look_down_color_CV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat look_down_Black_Mask_layer(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat look_down_Black_MaskCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat look_up_Black_MaskCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat look_up_color_CV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat captemp(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat capDownMask(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat black_Mask(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat black_Mask2(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat outlineBlackMaskCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat lookDownBlackTriangleMask(height, width, CV_8UC3, Scalar(255, 255, 255));

		cv::Mat prePrintobjectC(height, width, CV_8UC3, Scalar(255, 255, 255));

		cv::Mat downColorFirstCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat downBlackMaskFirstCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat downColorSecondCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat downBlackMaskSecondCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat prePrintobjectB(height, width, CV_8UC1, Scalar(255));

		cv::Mat capDownMask_bottom_face(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat capDownMask_top_face(height, width, CV_8UC3, Scalar(255, 255, 255));


		cv::Mat color_Bottom_face(height, width, CV_8UC3, Scalar(255, 255, 255));



		cv::Mat color_Top_face(height, width, CV_8UC3, Scalar(255, 255, 255));



		cv::Mat originalOutline(height, width, CV_8UC3, Scalar(255, 255, 255));


		cv::Mat outlineColorCv(height, width, CV_8UC3, Scalar(255, 255, 255));


		cv::Mat capDownSureMask(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat capTopSureMask(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat up_downBlackMaskCV(height, width, CV_8UC3, Scalar(255, 255, 255));



		//***20150505***slice_func_設置高度
		float box_bottom = md.bbox().min.Z();
		float unit = par.getFloat("slice_height");



		int boxy_dim = qCeil((md.bbox().max.Z() - md.bbox().min.Z()) / unit) + 1;

		init_texture(md);

		//*******************					  

		//***20150507 standard path******************
		//QString temppath = QStandardPaths::locate(QStandardPaths::TempLocation, "", QStandardPaths::LocateDirectory);
		//QString temppath = "D:/temptemp";//backup



		QString temppath = PicaApplication::getRoamingDir();
		QDir dir(temppath);//backup
		if (dir.exists("xyzimage"))
		{
			dir.setPath(temppath + "/xyzimage");
			dir.removeRecursively();
			dir.setPath(temppath);
			dir.mkpath("xyzimage");
		}
		else
		{
			dir.setPath(temppath);
			dir.mkpath("xyzimage");
		}

		dir.setPath(temppath + "/xyzimage");

		//---zx_file------------------------------------------
		//QString zxtemp = "D:/temptemp";
		QString zxtemp = PicaApplication::getRoamingDir();
		QDir zx_dir(zxtemp);
		if (zx_dir.exists("zx_file"))
		{
			zx_dir.setPath(zxtemp + "/zx_file");
			//zx_dir.removeRecursively();
			zx_dir.setPath(zxtemp);
			zx_dir.mkpath("zx_file");
		}
		else
		{
			dir.setPath(temppath);
			dir.mkpath("zx_file");
		}

		zx_dir.setPath(zxtemp + "/zx_file");

		//==============================
		//**********************
		ctx.bindReadDrawFramebuffer(hFramebuffer);
		GLW_CHECK_GL_READ_DRAW_FRAMEBUFFER_STATUS;
#if 1

		const bool outputOL_Image = par.getBool("OL_Image");
		const bool save_cap_Image = par.getBool("Cap_Image");

		const bool faceColor = par.getBool("FaceColor");
		glClearColor(1, 1, 1, 1);
		QString olMName = md.p_setting.getOlMeshName();
		QString cMName = md.p_setting.getCapMeshName();

		//***opencvMat clear
		capCTemp.clear();
		outlineCTemp.clear();


		int zx_job_pages = par.getInt("per_zx_job");
		//int start_page = par.getInt("start_page");
		int send_page_num = par.getInt("send_page_num");
		int plus_pixel = par.getInt("plus_pixel");
		float PLUS_MM = par.getFloat("PLUS_MM");
		bool start_print_b = par.getBool("PRINT_START");
		float useless_print = par.getFloat("USELESS_PRINT");
		float spittoon_M = par.getFloat("SPITTOON_M");
		float spittoon_C = par.getFloat("SPITTOON_C");
		float spittoon_Y = par.getFloat("SPITTOON_Y");
		float spittoon_B = par.getFloat("SPITTOON_B");
		bool add_pattern = par.getBool("ADD_PATTERN");
		string desProfilePath = par.getString("COLOR_PROFILE").toStdString();
		//QFileInfo tempProfilePath(QString::fromStdString(desProfilePath));
		bool doubleprint = par.getBool("Stiff_Print");
		bool dilateBinder = par.getBool("DILATE_BINDER");
		int dilateBinderValue = par.getInt("DILATE_BINDER_VALUE");
		bool dynamicValueSwitch = par.getBool("DYNAMIC_WIPE");
		bool colorBinding = par.getBool("COLOR_BINDING");
		bool doublePrint_V2 = par.getBool("STIFF_PRIN_V2");
		int doublePrint_V2Percent = par.getInt("STIFF_PRINT_VALUE");

		int doublePrint_shell_Percent = par.getInt("SHELL_PERCENT");
		bool CMYB_black_mode = par.getBool("CMYB_BLACK");
		int CMYB_black_percentage = par.getInt("CMYB_BLACK_VALUE");
		bool usePatternPercent = true;

		/*
		DM_Parameter
		*/
		int printDirectionMode = 0;
		double vertical_Thick = par.getFloat("DM_SHELL_VERTICAL_THICKNESS");;//Vertical Thick
		bool vertical_ThickC = par.getBool("DM_SHELL_VERTICAL_THICKNESS_C");;
		bool vertical_ThickM = par.getBool("DM_SHELL_VERTICAL_THICKNESS_M");;
		bool vertical_ThickY = par.getBool("DM_SHELL_VERTICAL_THICKNESS_Y");;
		bool vertical_ThickB = par.getBool("DM_SHELL_VERTICAL_THICKNESS_B");;

		double horizontal_Thick = par.getFloat("DM_SHELL_HORIZONTAL_THICKNESS");//Horizontal Thick
		bool horizontal_ThickC = par.getBool("DM_SHELL_HORIZONTAL_THICKNESS_C");;
		bool horizontal_ThickM = par.getBool("DM_SHELL_HORIZONTAL_THICKNESS_M");;
		bool horizontal_ThickY = par.getBool("DM_SHELL_HORIZONTAL_THICKNESS_Y");;
		bool horizontal_ThickB = par.getBool("DM_SHELL_HORIZONTAL_THICKNESS_B");;

		double bottom_lighter_Thick = par.getFloat("DM_SHELL_BOTTOM_LIGHTER");//Horizontal Thick
		bool bottom_lighterC = par.getBool("DM_SHELL_BOTTOM_LIGHTER_C");;
		bool bottom_lighterM = par.getBool("DM_SHELL_BOTTOM_LIGHTER_M");;
		bool bottom_lighterY = par.getBool("DM_SHELL_BOTTOM_LIGHTER_Y");;
		bool bottom_lighterB = par.getBool("DM_SHELL_BOTTOM_LIGHTER_B");;

		int magicSquarePatternSize = par.getInt("DM_INNER_CELL_SIZE");;
		int fillPatternPercent = par.getInt("DM_INNER_PERCENTAGE");;
		/*==================================================================================================================
		====================================================================================================================*/

		//int shellPercent = 100;
		//int erodeIteration = ( DM_binderThick*(print_dpi / DSP_inchmm) ) - 5;



		/*int spitton_Mode = par.getBool("SPITTON_MODE");
		if (spitton_Mode == 0)
		{
		spittoon_M = 0.5;
		spittoon_C = 0.5;
		spittoon_Y = 0.5;
		spittoon_B = 5;
		}
		else if (spitton_Mode == 1)
		{
		spittoon_M = 3;
		spittoon_C = 3;
		spittoon_Y = 3;
		spittoon_B = 5;
		}*/

		bool prePrintBool = true;//witness bar

		bool fourPage_switch = false;


		bool continuePrinting = par.getBool(("CONTINUE_PRINTING"));
		int start_page = par.getInt("CONTINUE_PRINTING_PAGE");
		int end_page = par.getInt("CONTINUE_PRINTING_END_PAGE");


		//assert(tempProfilePath.exists());
		//float meCorrectPercent = 0.97;
		float meCorrectPercent = 1;

		//********control profile on && DSP_TEST_MODE
		bool profileOn = false;
		bool savedebugImage = false;// par.getBool(("generate_final_picture"));
		bool line_or_quads = false;// par.getBool("LINE_OR_RECT");;//true line, false quads


		SKT::pixelFormat gen_pixelFormat;
		ZxGen_SKT::pixelFormat genZxPixelFormat;
		ZxGen_SKT::zxInitMode zxCheckMode;
		if (profileOn)
		{
			gen_pixelFormat = SKT::pixelFormat::CMY;
			genZxPixelFormat = ZxGen_SKT::pixelFormat::CMY;
		}
		else
		{
			gen_pixelFormat = SKT::pixelFormat::RGB;
			genZxPixelFormat = ZxGen_SKT::pixelFormat::RGB;
		}
		if (DSP_TEST_MODE)
		{
			zxCheckMode = ZxGen_SKT::zxInitMode::ViewCheckMode;
		}
		else
		{
			zxCheckMode = ZxGen_SKT::zxInitMode::RealPrintMode;

		}


		int widthrecord, heightrecord;
		QString blk_img_file_name = "black_outline";

		QString zxFilePath = par.getString("ZX_File_Path");
		QString zx_sn = makeOutputname(zx_dir.absolutePath(), 0);


		md.jobname.push_back(zx_sn);

		//zggg = new ZxGen(unsigned int(width), unsigned int(height), zx_pages, md.groove.DimX(),21, zx_sn.toStdString());//20161219_backup						 
		//zggg = new ZxGen(unsigned int(width), 2580, zx_job_pages, md.groove.DimX(), 21, zx_sn.toStdString());//20161219_backup						 
		//zx_skt = new ZxGen(unsigned int(width), unsigned int(height*meCorrectPercent + plus_pixel), zx_job_pages, md.groove.DimX(), md.groove.DimY() + PLUS_MM, zx_sn.toStdString(), start_print_b);//20161219_backup						 
		if (zxFilePath.isEmpty())
			zx_skt = new ZxGen_SKT(print_dpi, zx_job_pages, zx_sn.toStdString(), start_print_b, genZxPixelFormat, zxCheckMode);//20161219_backup						 
		else
			zx_skt = new ZxGen_SKT(print_dpi, zx_job_pages, zxFilePath.toStdString(), start_print_b, genZxPixelFormat, zxCheckMode);//20161219_backup						 
		std::vector<cv::Mat> blankTemp;
		std::vector<cv::Mat> blankcapCTemp;


		//if (start_page == 0)
		//{
		WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("newUselessbar_customateBlankPages"));
		if (!zxCheckMode == ZxGen_SKT::zxInitMode::ViewCheckMode  && continuePrinting == false && fourPage_switch)
		{

			SKT::createBlankPages(doubleprint, tempgroove.grooveheight, md.p_setting.get_plus_print_length(), useless_print, DSP_Blank_pages, blankTemp, blankcapCTemp, gen_pixelFormat, spittoon_M, spittoon_C, spittoon_Y, spittoon_B, print_dpi);
			////WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("sendData2"));
			zx_skt->sendData2(blankTemp, blankcapCTemp);
		}
		//}
		//Log("ZxGen width %i height %i ", int(width), int(height*meCorrectPercent + plus_pixel));
		//FILE *dbgff;
		//dbgff = fopen("D:\\debug_open_1.txt", "w");//@@@
		QTime time;
		//clock_t start, stop;
		time.start();



		QDir patternPath(PicaApplication::getRoamingDir());

		if (patternPath.exists("pattern"))
		{
			patternPath.setPath(PicaApplication::getRoamingDir() + "/pattern");
		}
		else
		{
			patternPath.setPath(PicaApplication::getRoamingDir());
			patternPath.mkpath("pattern");
			patternPath.setPath(PicaApplication::getRoamingDir() + "/pattern");
			//patternPath.setPath( "./pattern");

		}



		cv::Mat patternfilter_magicSquare(cv::Size(magicSquarePatternSize, magicSquarePatternSize), CV_8UC1, Scalar(255));
		if (usePatternPercent)
		{
			//SKT::createPatternPercent(patternfilter, 36);

			SKT::MagicSquarePattern(patternfilter_magicSquare, fillPatternPercent, magicSquarePatternSize);


		}


		cv::Mat patternImg;
		patternImg = cv::imread(patternPath.absolutePath().toStdString() + "/hexagon_4.png", CV_LOAD_IMAGE_GRAYSCALE);
		string srcProfilePath = PicaApplication::getRoamingDir().toStdString() + "/ColorProfile/sRGB2014.icc";

		//***testLog
		//Log("Closed %i holes and added %i new faces", 12, 12);
		//boxy_dim--;
		int testPrintPageCount;// = 10;
		bool testswitch;// = false;

		int generateNOI = 25;
		int quickWipe = 1;
		WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("for loop start"));
		if (start_page > boxy_dim && continuePrinting){
			zx_skt->close();
			zx_skt->~ZxGen_SKT();
			return false;
		}

		if (continuePrinting)
		{
			testPrintPageCount = end_page - start_page;
			boxy_dim = start_page + testPrintPageCount;
			testswitch = true;
		}
		else
		{
			start_page = 0;
			testswitch = false;
		}

		//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("gen log"));
		//for (int i = start_page, j = start_page + 1; i < boxy_dim; i++, j++)

		for (int i = start_page, j = start_page + 1; i < start_page + testPrintPageCount; i++, j++)
		{
			time.restart();
			resetImage(look_down_Black_Mask_layer, width, height);//
			resetImage(look_down_color_CV, width, height);
			resetImage(look_down_Black_MaskCV, width, height);//
			resetImage(look_up_Black_MaskCV, width, height);//
			resetImage(look_up_color_CV, width, height);
			resetImage(black_Mask, width, height);
			resetImage(black_Mask2, width, height);
			resetImage(lookDownBlackTriangleMask, width, height);//
			resetImage(outlineBlackMaskCV, width, height);//
			resetImage(prePrintobjectC, width, height);


			resetImage(downColorFirstCV, width, height);
			resetImage(downBlackMaskFirstCV, width, height);
			resetImage(downColorSecondCV, width, height);
			resetImage(downBlackMaskSecondCV, width, height);
			resetImage(up_downBlackMaskCV, width, height);



			resetImage(outlineColorCv, width, height);
			resetImage(captemp, width, height);

			resetImage(capDownMask, width, height);

			md.wipeFlag.push_back(true);
			if (cb && (i % 1) == 0)
			{
				glContext->doneCurrent();
				bool break_test = false;
				if (start_print_b)
				{
					break_test = cb((i * 100) / boxy_dim, "start_printing_slice_process");
					/*if (i % 10 == 0)
					quickWipe = 2;
					else
					quickWipe = 4;*/
					if (i > 1 && dynamicValueSwitch)
					{
						int temp = i - 1 + 5;
						sb(temp, quickWipe);
					}
				}
				else
				{
					break_test = cb((i * 100) / boxy_dim, "slice_process");
					/*if (i % 10 == 0)
					quickWipe = 1;
					else
					quickWipe = 6;*/
					if (i > 1 && dynamicValueSwitch)
						sb(i - 1 + 5, quickWipe);
					//break_test = cb((i , "slice_process");
				}
				if (!break_test)//callback2 return cancel, need to close zx_SKT
				{
					zx_skt->close();
					zx_skt->~ZxGen_SKT();

					glContext->doneCurrent();
					return false;
					break;
				}

			}
			glContext->makeCurrent();
			//xyz::slice_roution(md, box_bottom + unit*i, faceColor);//***backup******************切層演算							  
			//time.restart();
			//Log("Closed %i holes and added %i new faces, %s,%d ", 12, 12, __FUNCTION__, __LINE__);
			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("slice start"));
			xyz::slice_roution(md, box_bottom + unit*i, faceColor);//****20160301*****************切層演算		
			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("slice end"));


			float currnentHeight = box_bottom + unit*i;


			//time.restart();
			//***第一張outline圖*****************************
			QVector<MeshModel *> vmm;
			md.getMeshByContainString("_temp_outlines", vmm);
			md.getMeshByContainString(olMName, vmm);
			QVector<MeshModel *> cmm;
			md.getMeshByContainString(cMName, cmm);//capImage
			int xx = 0;
			QString img_file_name = md.p_setting.getoutlineName();
			float cut_z = box_bottom + unit*i;


			glPixelStorei(GL_PACK_ALIGNMENT, 1);//OpenGL PixelStori Setting


			/*==============start outline black Mask============================*/
			glMatrixMode(GL_PROJECTION); glPopMatrix();
			glMatrixMode(GL_MODELVIEW); glPopMatrix();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);
			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -1000, 1000);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
			gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, 0, 0, 1, 0);
			foreach(MeshModel *mp, vmm)//outline
			{
				mp->glw.viewerNum = 2;
				mp->outlineThick = horizontal_Thick;
				if (vmm.size() > 0)
				{
					if (line_or_quads)
						mp->render(vcg::GLW::DMTexWire, vcg::GLW::CMBlack, vcg::GLW::TMNone);//backup
					else
						mp->render(vcg::GLW::DMTexWireQuad, vcg::GLW::CMBlack, vcg::GLW::TMNone);//backup
				}
			}
			glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, outlineBlackMaskCV.data);
			cv::flip(outlineBlackMaskCV, outlineBlackMaskCV, 0);

			if (savedebugImage){
				image = cvMatToQImage(outlineBlackMaskCV);
				QString outlineBlackMask = "outlineBlackMask";
				outlineBlackMask.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				image.save(dir.absolutePath() + "/" + outlineBlackMask, "png");
			}
			///////////////////////end outlineBlackMask//////////////////////////
			/*==============start TOP-DOWN black Mask============================*/
			glMatrixMode(GL_PROJECTION); glPopMatrix();
			glMatrixMode(GL_MODELVIEW); glPopMatrix();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);
			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -1000, 1000);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
			gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, 0, 0, 1, 0);

			foreach(MeshModel *mdmm, md.meshList)
			{
				//****20151231********************************************************************								  
				if (mdmm->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mdmm->cm.bbox.min.Z() && currnentHeight <= mdmm->cm.bbox.max.Z()))
					if (cut_z - mdmm->cm.bbox.min.Z() < 0 || cut_z - mdmm->cm.bbox.max.Z() > 0)
					{
						glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
						glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., 0.0001, 0);
						glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, cut_z, 0, 0, -DSP_Look_Distination, 0, 1, 0);

						mdmm->render(GLW::DMFlat, GLW::CMBlack, GLW::TMNone);
						glPopMatrix();

					}
					else
					{
						glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
						glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -vertical_Thick, vertical_Thick);
						glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, cut_z, 0, 0, -DSP_Look_Distination, 0, 1, 0);

						glPushMatrix();
						mdmm->render(GLW::DMFlat, GLW::CMBlack, GLW::TMNone);
						glPopMatrix();
					}

			}

			glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, up_downBlackMaskCV.data);
			cv::flip(up_downBlackMaskCV, up_downBlackMaskCV, 0);

			if (savedebugImage){
				image = cvMatToQImage(up_downBlackMaskCV);
				QString up_downBlackMask = "up_downBlackMaskCV";
				up_downBlackMask.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				image.save(dir.absolutePath() + "/" + up_downBlackMask, "png");
			}
			/*
			//////////////////////end TOP-DOWN Mask//////////////////////////
			*/



			////***start***lookDownBlackTriangleMask***
			//glMatrixMode(GL_PROJECTION); glPopMatrix();
			//glMatrixMode(GL_MODELVIEW); glPopMatrix();
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			//glClearColor(1, 1, 1, 1);
			//foreach(MeshModel *mdmm, md.meshList)
			//{
			//	//****20151231********************************************************************
			//	if (mdmm->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mdmm->cm.bbox.min.Z() && currnentHeight <= mdmm->cm.bbox.max.Z()))
			//	if (cut_z - mdmm->cm.bbox.min.Z() < 0 || cut_z - mdmm->cm.bbox.max.Z()>0)//在物體box的上下需要設定限制，才不會超出物體大小。								  
			//	{
			//		glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			//		glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., 0.0001, 0);
			//		glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, cut_z, 0, 0, -DSP_Look_Distination, 0, 1, 0);
			//		glPushMatrix();
			//		mdmm->render(GLW::DMSmooth, GLW::CMBlack, GLW::TMNone);
			//		glPopMatrix();
			//	}
			//	else
			//	{
			//		glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			//		glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -DSP_Color_thick, DSP_Color_thick);
			//		glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
			//		gluLookAt(0, 0, cut_z, 0, 0, -DSP_Look_Distination, 0, 1, 0);//look down
			//		glPushAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
			//		glShadeModel(GL_SMOOTH);
			//		glEnable(GL_COLOR_MATERIAL);
			//		glDisable(GL_LIGHTING);
			//		glPushMatrix();
			//		mdmm->render(GLW::DMSmooth, GLW::CMBlack, GLW::TMNone);
			//		glPopMatrix();
			//		glPopAttrib();
			//	}
			//}								
			//glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, lookDownBlackTriangleMask.data);
			//cv::flip(lookDownBlackTriangleMask, lookDownBlackTriangleMask, 0);
			//if (savedebugImage)
			//{
			//	image = cvMatToQImage(lookDownBlackTriangleMask);
			//	QString lookDownColorkMask = "lookDownBlackTriangleMask";
			//	lookDownColorkMask.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
			//	image.save(dir.absolutePath() + "/" + lookDownColorkMask, "png");
			//}
			////***end***lookDownBlackTriangleMask***


			/***20160122***outline and binder thick black mask******/
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);

			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			//glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -100, 100);
			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -1000, 1000);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, 0, 0, 1, 0);
			//*********************************
			//binderstrips(md, par.getFloat("useless_print"));//******uselessbar
			//image.fill(QColor::w);
			foreach(MeshModel *mp, vmm)//outline
			{
				mp->outlineThick = horizontal_Thick;
				mp->glw.viewerNum = 2;
				if (vmm.size() > 0)
				{
					if (line_or_quads)
						mp->render(vcg::GLW::DMTexWire, vcg::GLW::CMBlack, vcg::GLW::TMNone);//backup
					else
						mp->render(vcg::GLW::DMTexWireQuad, vcg::GLW::CMBlack, vcg::GLW::TMNone);//backup

					//mp->render(vcg::GLW::DMOutline_Test, vcg::GLW::CMNone, vcg::GLW::TMNone);
				}
			}
			foreach(MeshModel *mdmm, md.meshList)
			{
				//****20151231********************************************************************								  
				if (mdmm->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mdmm->cm.bbox.min.Z() && currnentHeight <= mdmm->cm.bbox.max.Z()))
					if (cut_z - mdmm->cm.bbox.min.Z() < 0 || cut_z - mdmm->cm.bbox.max.Z() > 0)
					{
						glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
						glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., 0.0001, 0);
						glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, cut_z, 0, 0, -DSP_Look_Distination, 0, 1, 0);

						mdmm->render(GLW::DMFlat, GLW::CMBlack, GLW::TMNone);
						glPopMatrix();

					}
					else
					{
						glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
						//glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -0.6, 0.6);
						glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -vertical_Thick, vertical_Thick);
						//glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -0.2, 0.2);
						//***20161112
						glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, cut_z, 0, 0, -DSP_Look_Distination, 0, 1, 0);

						glPushMatrix();
						mdmm->render(GLW::DMFlat, GLW::CMBlack, GLW::TMNone);
						glPopMatrix();
					}

			}
			//fFaceColor
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, black_Mask.data);
			cv::flip(black_Mask, black_Mask, 0);
			//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("gen log"));
			//***20160122***DSP_Color_Replace_Binder_thick_mask


			/*===========================================================================================
			***Binder Information image
			==============================================================================================*/
			equ[3] = box_bottom + unit*i;

			//time.restart();
			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -1000, 1000);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, -DSP_Look_Distination, 0, 1, 0);
			//*********************************
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);

			//binderstrips(md, par.getFloat("useless_print"));//***uselessbar

			//************************************************//
			//****** Rendering the mesh's clip edge ****//
			GLuint MY_CLIP_PLANE = GL_CLIP_PLANE0;
			glEnable(MY_CLIP_PLANE);
			glClipPlane(MY_CLIP_PLANE, equ);

			glEnable(GL_STENCIL_TEST);
			glEnable(GL_CULL_FACE);
			glClear(GL_STENCIL_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

			//// first pass: increment stencil buffer value on back faces
			glStencilFunc(GL_ALWAYS, 0, 0);
			glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
			glCullFace(GL_FRONT); // render back faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			//glRotatef(90, 1, 0, 0);
			foreach(MeshModel * mp, md.meshList)
			{
				if (mp->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mp->cm.bbox.min.Z() && currnentHeight <= mp->cm.bbox.max.Z()))
					//mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
					mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
			}

			glPopMatrix();
			glPopAttrib();
			//second pass: decrement stencil buffer value on front faces
			glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
			glCullFace(GL_BACK); // render front faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			//glRotatef(90, 1, 0, 0);
			foreach(MeshModel * mp, md.meshList)
			{
				if (mp->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mp->cm.bbox.min.Z() && currnentHeight <= mp->cm.bbox.max.Z()))
					//mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
					mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
			}
			glPopMatrix();
			glPopAttrib();
			//-----------------------------
			// drawing clip planes masked by stencil buffer content
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glEnable(GL_DEPTH_TEST);
			glDisable(MY_CLIP_PLANE);
			glStencilFunc(GL_NOTEQUAL, 0, ~0);
			// stencil test will pass only when stencil buffer value = 0; 
			// (~0 = 0x11...11)

			glColor4f(1, 1, 1, 1);
			fillsquare();
			glDisable(GL_STENCIL_TEST);
			glEnable(MY_CLIP_PLANE); // enabling clip plane again
			glDisable(GL_LIGHTING);

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			//********************************************************************************************//


			//glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, cap_image.bits());//***20150724
			glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, captemp.data);
			cv::flip(captemp, captemp, 0);

			QString img_file_name2 = md.p_setting.getcapImageName();
			glDisable(MY_CLIP_PLANE);
			glDisable(GL_CULL_FACE);
			//****								
			//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T(""));
			/*if (par.getBool("generate_zx"))
			{*/
			cv::cvtColor(captemp, captemp, CV_BGR2GRAY);
			cv::Mat copyCaptemp;
			copyCaptemp = captemp.clone();
			//}
			//if (savedebugImage){
			if (false){
				QImage testttt = cvMatToQImage(captemp);
				QString capimagest = "captemp_image";
				capimagest.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				testttt.save(dir.absolutePath() + "/" + capimagest, "png");
			}


			/*===========================================================================================
			***Binder down Mask  image
			==============================================================================================*/
			equ[3] = box_bottom + unit*i - bottom_lighter_Thick;

			//time.restart();
			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -1000, 1000);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, -DSP_Look_Distination, 0, 1, 0);
			//*********************************
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);

			//binderstrips(md, par.getFloat("useless_print"));//***uselessbar

			//************************************************//
			//****** Rendering the mesh's clip edge ****//
			GLuint MY_CLIP_PLANE3 = GL_CLIP_PLANE0;
			glEnable(MY_CLIP_PLANE3);
			glClipPlane(MY_CLIP_PLANE3, equ);

			glEnable(GL_STENCIL_TEST);
			glEnable(GL_CULL_FACE);
			glClear(GL_STENCIL_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

			//// first pass: increment stencil buffer value on back faces
			glStencilFunc(GL_ALWAYS, 0, 0);
			glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
			glCullFace(GL_FRONT); // render back faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			//glRotatef(90, 1, 0, 0);
			foreach(MeshModel * mp, md.meshList)
			{
				if (mp->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mp->cm.bbox.min.Z()) && (currnentHeight <= mp->cm.bbox.max.Z()))
					//mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
					mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
			}

			glPopMatrix();
			glPopAttrib();
			//second pass: decrement stencil buffer value on front faces
			glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
			glCullFace(GL_BACK); // render front faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			//glRotatef(90, 1, 0, 0);
			foreach(MeshModel * mp, md.meshList)
			{
				if (mp->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mp->cm.bbox.min.Z() && currnentHeight <= mp->cm.bbox.max.Z()))
					//mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
					mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
			}
			glPopMatrix();
			glPopAttrib();

			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glEnable(GL_DEPTH_TEST);
			glDisable(MY_CLIP_PLANE3);
			glStencilFunc(GL_NOTEQUAL, 0, ~0);


			glColor4f(1, 1, 1, 1);
			fillsquare();
			glDisable(GL_STENCIL_TEST);
			glEnable(MY_CLIP_PLANE3); // enabling clip plane again
			glDisable(GL_LIGHTING);

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			//********************************************************************************************//


			//glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, cap_image.bits());//***20150724
			glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, capDownMask.data);
			cv::flip(capDownMask, capDownMask, 0);

			//QString img_file_name2 = md.p_setting.getcapImageName();
			glDisable(MY_CLIP_PLANE3);
			glDisable(GL_CULL_FACE);
			//****								
			//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T(""));
			/*if (par.getBool("generate_zx"))
			{*/
			cv::cvtColor(capDownMask, capDownMask, CV_BGR2GRAY);
			//}
			if (false){
				QImage capDownMaskkQ = cvMatToQImage(capDownMask);
				QString capDownMaskst = "capDownMask";
				capDownMaskst.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				capDownMaskkQ.save(dir.absolutePath() + "/" + capDownMaskst, "png");
			}
			/*===========================================================================================
			***END  Binder down Mask  image
			==============================================================================================*/


			/*=============================================================================================
			prePrint object
			===============================================================================================*/
			if (prePrintBool)
			{
				//time.restart();
				glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
				glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -1000, 1000);
				glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, -DSP_Look_Distination, 0, 1, 0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
				glClearColor(1, 1, 1, 1);
				//****** Rendering the mesh's clip edge ****//
				GLuint MY_CLIP_PLANE2 = GL_CLIP_PLANE0;
				glEnable(MY_CLIP_PLANE2);
				glClipPlane(MY_CLIP_PLANE2, equ);

				glEnable(GL_STENCIL_TEST);
				glEnable(GL_CULL_FACE);
				glClear(GL_STENCIL_BUFFER_BIT);
				glDisable(GL_DEPTH_TEST);
				glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

				//// first pass: increment stencil buffer value on back faces
				glStencilFunc(GL_ALWAYS, 0, 0);
				glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
				glCullFace(GL_FRONT); // render back faces only
				glPushAttrib(GL_ALL_ATTRIB_BITS);
				glEnable(GL_LIGHTING);
				glPushMatrix();

				foreach(MeshModel * mp, md.meshList)
				{
					if (mp->getMeshSort() == MeshModel::meshsort::pre_print_item && (currnentHeight >= mp->cm.bbox.min.Z() && currnentHeight <= mp->cm.bbox.max.Z()))
						mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
				}

				glPopMatrix();
				glPopAttrib();
				//second pass: decrement stencil buffer value on front faces
				glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
				glCullFace(GL_BACK); // render front faces only
				glPushAttrib(GL_ALL_ATTRIB_BITS);
				glEnable(GL_LIGHTING);
				glPushMatrix();

				//glRotatef(90, 1, 0, 0);
				foreach(MeshModel * mp, md.meshList)
				{
					if (mp->getMeshSort() == MeshModel::meshsort::pre_print_item && (currnentHeight >= mp->cm.bbox.min.Z() && currnentHeight <= mp->cm.bbox.max.Z()))
						mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
				}
				glPopMatrix();
				glPopAttrib();
				//-----------------------------
				// drawing clip planes masked by stencil buffer content
				glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
				glEnable(GL_DEPTH_TEST);
				glDisable(MY_CLIP_PLANE2);
				glStencilFunc(GL_NOTEQUAL, 0, ~0);
				// stencil test will pass only when stencil buffer value = 0; 
				// (~0 = 0x11...11)

				glColor4f(1, 1, 1, 1);
				fillsquare();
				glDisable(GL_STENCIL_TEST);
				glEnable(MY_CLIP_PLANE2); // enabling clip plane again
				glDisable(GL_LIGHTING);

				glMatrixMode(GL_MODELVIEW);
				glPopMatrix();
				glMatrixMode(GL_PROJECTION);
				glPopMatrix();

				glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, prePrintobjectC.data);
				cv::flip(prePrintobjectC, prePrintobjectC, 0);
				bitwise_not(prePrintobjectC, prePrintobjectC);
				cv::cvtColor(prePrintobjectC, prePrintobjectB, CV_BGR2GRAY);


				glDisable(MY_CLIP_PLANE2);//存完圖
				glDisable(GL_CULL_FACE);

				if (savedebugImage){
					//if (true){
					QImage ttt(cvMatToQImage(prePrintobjectB));
					QString tttt = "prePrintobjectB";
					tttt.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
					ttt.save(dir.absolutePath() + "/" + tttt, "png");

				}
			}
			/*=============================================================================================================================
			=============================================================================================================================*/

			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T(""));


			//cv::Mat reverse_binder_outline(downBlackMaskFirstCV.size(), downBlackMaskFirstCV.type(), Scalar(255, 255, 255));
			//originalOutline = SKT::outlineAndBinderMask(outlineColorCv, outlineBlackMaskCV, captemp, downColorFirstCV, downBlackMaskFirstCV, downColorSecondCV, downBlackMaskSecondCV, reverse_binder_outline).clone();								


			//***20160112***image_process加上插點pattern
			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("imagePorcess"));
			if (par.getBool("generate_zx"))
			{
				SKT::InkBalance imageProcess;

				cv::Mat temp, temp2;

				///////cap_temp_change_add_pattern///////////////////																		
				//black_Mask = SKT::erodeImage(&black_Mask, erodeIteration);

				if (savedebugImage){
					QImage erode_black_Mask(cvMatToQImage(black_Mask));
					QString erode_black_MaskS = "erode_black_Mask_DM";
					erode_black_MaskS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
					erode_black_Mask.save(dir.absolutePath() + "/" + erode_black_MaskS, "png");
				}
				/*
				perimeter  thicker
				*/
				cv::cvtColor(outlineBlackMaskCV, outlineBlackMaskCV, CV_BGR2GRAY);
				//outlineBlackMaskCV = SKT::erodeImage(&outlineBlackMaskCV, erodeIteration);


				////WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("rect_test1"));
				//if ((i % 100) > 20 && add_pattern)

				if (savedebugImage){
					QImage ttt(cvMatToQImage(black_Mask));
					QString black_MaskS = "black_Mask";
					black_MaskS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
					ttt.save(dir.absolutePath() + "/" + black_MaskS, "png");
				}


				captemp = SKT::pastePatternToImage(captemp, &black_Mask, patternfilter_magicSquare, magicSquarePatternSize);
				if (savedebugImage){
					QImage pastePattern(cvMatToQImage(captemp));
					QString pastePatternS = "pastePattern_DM";
					pastePatternS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
					pastePattern.save(dir.absolutePath() + "/" + pastePatternS, "png");
				}

				cv::cvtColor(captemp, originalOutline, CV_GRAY2RGB);
				/*
				Grayscale outline, use channel control method,
				Mask : perimeter, updown
				*/
				SKT::GrayScalePerimeter(captemp, originalOutline, copyCaptemp, outlineBlackMaskCV, up_downBlackMaskCV,
					horizontal_ThickB, horizontal_ThickC, horizontal_ThickM, horizontal_ThickY,
					vertical_ThickB, vertical_ThickC, vertical_ThickM, vertical_ThickY);
				if (savedebugImage){
					QImage copyCaptempkQ = cvMatToQImage(captemp);
					QString copyCaptempkQst = "GrayScalePerimeter_Captemp";
					copyCaptempkQst.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
					copyCaptempkQ.save(dir.absolutePath() + "/" + copyCaptempkQst, "png");
				}
				if (savedebugImage){
					QImage copyCaptempkQ = cvMatToQImage(originalOutline);
					QString copyCaptempkQst = "GrayScalePerimeter_originalOutline";
					copyCaptempkQst.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
					copyCaptempkQ.save(dir.absolutePath() + "/" + copyCaptempkQst, "png");
				}


				SKT::BottomLighter(copyCaptemp, capDownMask, captemp, originalOutline, bottom_lighterB, bottom_lighterC, bottom_lighterM, bottom_lighterY);
				if (savedebugImage){
					QImage ttt(cvMatToQImage(originalOutline));
					QString originalOutlineS = "originalOutline_DM";
					originalOutlineS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
					ttt.save(dir.absolutePath() + "/" + originalOutlineS, "png");
				}
				if (savedebugImage){
					QImage ttt(cvMatToQImage(captemp));
					QString captempS = "captemp_DM";
					captempS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
					ttt.save(dir.absolutePath() + "/" + captempS, "png");
				}





				/*if (prePrintBool)
				{
				SKT::mergePrePrinting(originalOutline, prePrintobjectB);
				SKT::mergePrePrinting(captemp, prePrintobjectB);
				}*/
				/*if (CMYB_black_mode)
				{
				captemp.copyTo(originalOutline);
				cv::cvtColor(originalOutline, originalOutline, cv::COLOR_GRAY2BGR);
				cv::subtract(cv::Scalar::all(255), originalOutline, originalOutline);
				originalOutline = originalOutline*CMYB_black_percentage / 100.;
				}*/



				if (savedebugImage){
					QImage ttt(cvMatToQImage(originalOutline));
					QString originalOutlineS = "originalOutline";
					originalOutlineS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
					ttt.save(dir.absolutePath() + "/" + originalOutlineS, "png");
				}




				originalOutline = SKT::resizeIamgecols(&originalOutline, plus_pixel, gen_pixelFormat);//ADD LENGTH ON COLOR IMAGE									
				SKT::newUselessbar_custom(originalOutline, useless_print, gen_pixelFormat, spittoon_M, spittoon_C, spittoon_Y, spittoon_B, print_dpi);

				//////////////////////////////////
				//xy_axis now is fine
				//captemp = SKT::resizeXAxis(&captemp, meCorrectPercent);//****0.97


				captemp = SKT::resizeIamgecols(&captemp, plus_pixel, gen_pixelFormat);
				//SKT::newUselessbar(captemp, useless_print, gen_pixelFormat);//20160715_backup																			
				//if (!doubleprint)
				//SKT::cutImage(captemp, originalOutline);//white_kick
				if (printDirectionMode == 0)
					//SKT::cutImage(captemp, originalOutline);//white_kick

					/*if (CMYB_black_mode)
					{
					SKT::BinderPercent(captemp, CMYB_black_percentage);

					}*/
					SKT::newUselessbar_custom(captemp, useless_print, gen_pixelFormat, spittoon_M, spittoon_C, spittoon_Y, spittoon_B, print_dpi);

				if (dilateBinder)
					captemp = SKT::dilateImage(&captemp, dilateBinderValue);

				//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T(""));
				cv::Mat finalcolor(originalOutline.cols, originalOutline.rows, originalOutline.type(), Scalar(255, 255, 255));//finalcolor.setTo(cv::Scalar(255, 255, 255));
				SKT::rotateImage(originalOutline, finalcolor);
				cv::Mat finalBinder(captemp.cols, captemp.rows, captemp.type(), Scalar(255));//finalBinder.setTo(cv::Scalar(255));
				SKT::rotateImage(captemp, finalBinder);

				//outlineCTemp.push_back(finalcolor);//**********PUSH_BACK_OUTLINE********************************
				if (zxCheckMode == ZxGen_SKT::zxInitMode::ViewCheckMode && !doubleprint)
					finalBinder.setTo(cv::Scalar(255));//SET BINDER TO WHITE
				//if (dialiteBinder)finalBinder = SKT::dilateImage(&finalBinder, 1);
				//capCTemp.push_back(finalBinder);//***********PUSH_BACK_CAPIMAGE******************************************
				cv::Mat backbinder(finalBinder.size(), finalBinder.type(), cv::Scalar(255));

				cv::Mat mirrorColor(cv::Size(finalcolor.cols, finalcolor.rows), finalcolor.type(), cv::Scalar(0, 0, 0));
				cv::Mat mirrorBinder(cv::Size(finalBinder.cols, finalBinder.rows), finalBinder.type(), cv::Scalar(255));


				//cv::Mat stiffColor(cv::Size(finalcolor.rows, finalcolor.cols), finalcolor.type(), cv::Scalar(0, 0, 0));
				//cv::Mat stiffColor2(cv::Size(finalcolor.cols, finalcolor.rows), finalcolor.type(), cv::Scalar(0, 0, 0));


				//if (doubleprint)
				//{
				//	cv::Mat finalBackBinder(captemp.cols, captemp.rows, captemp.type(), Scalar(255));//finalBinder.setTo(cv::Scalar(255));
				//	reverse_binder_outline = SKT::resizeIamgecols(&reverse_binder_outline, plus_pixel, gen_pixelFormat);
				//	//SKT::newUselessbar(captemp, useless_print, gen_pixelFormat);//20160715_backup										
				//	SKT::newUselessbar_custom(reverse_binder_outline, useless_print, gen_pixelFormat, spittoon_M, spittoon_C, spittoon_Y, spittoon_B, print_dpi);
				//	SKT::rotateImage(reverse_binder_outline, finalBackBinder);
				//	SKT::mirrorImage(finalBackBinder, backbinder);
				//	cv::Mat erodeBinder = SKT::dilateImage(&backbinder, 1);
				//	outlineCTemp.push_back(stiffColor2);
				//	capCTemp.push_back(erodeBinder);
				//	if (savedebugImage){
				//		QImage cap_Test = cvMatToQImage(finalBackBinder);
				//		img_file_name2.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				//		cap_Test.save(dir.absolutePath() + "/" + img_file_name2, "png");
				//	}										
				//}
				//if (doublePrint_V2)
				//{
				//	SKT::newUselessbar_custom(stiffColor, useless_print, gen_pixelFormat, spittoon_M, spittoon_C, spittoon_Y, spittoon_B, print_dpi);
				//	SKT::rotateImage(stiffColor, stiffColor2);
				//	finalBinder.copyTo(backbinder);
				//	cv::Mat erodeBlackBinder = SKT::dilateImage(&backbinder, 1);
				//	/*=======================================
				//	reduce binder in the first 20 layers
				//	==========================================*/
				//	if (i < 20)
				//		SKT::BinderPercent(erodeBlackBinder, doublePrint_begin_twenty_layer_V2Percent);
				//	else
				//		SKT::BinderPercent(erodeBlackBinder, doublePrint_V2Percent);
				//	/*===============================================
				//	remain x percent binder in the second printing
				//	================================================*/
				//	SKT::BinderPercent(reverse_binder_outline, doublePrint_shell_Percent);
				//	reverse_binder_outline = SKT::resizeIamgecols(&reverse_binder_outline, plus_pixel, gen_pixelFormat);
				//	reverse_binder_outline = reverse_binder_outline(cv::Rect(0, 0, backbinder.rows, finalBinder.cols));
				//	cv::Mat secondOutlineBinder(reverse_binder_outline.cols, reverse_binder_outline.rows, reverse_binder_outline.type(), Scalar(255));//finalBinder.setTo(cv::Scalar(255));
				//	SKT::newUselessbar_custom(reverse_binder_outline, useless_print, gen_pixelFormat, spittoon_M, spittoon_C, spittoon_Y, spittoon_B, print_dpi);
				//	SKT::rotateImage(reverse_binder_outline, secondOutlineBinder);
				//	//combine secondOutlineBinder and erodeBlackBinder to secondOutlineBinder
				//	SKT::combineBinderImage(secondOutlineBinder, erodeBlackBinder, i);
				//	if (savedebugImage){
				//		QImage cap_Test = cvMatToQImage(erodeBlackBinder);
				//		QString tttt3 = "erodeBlackBinder";
				//		tttt3.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				//		cap_Test.save(dir.absolutePath() + "/" + tttt3, "png");
				//	}
				//	if (savedebugImage){
				//		QImage cap_Test = cvMatToQImage(secondOutlineBinder);
				//		QString tttt3 = "secondOutlineBinder";
				//		tttt3.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				//		cap_Test.save(dir.absolutePath() + "/" + tttt3, "png");
				//	}
				//	/*outlineCTemp.push_back(mirrorColor);
				//	capCTemp.push_back(erodeBlackBinder);*/
				//	outlineCTemp.push_back(stiffColor2);
				//	//capCTemp.push_back(erodeBlackBinder);
				//	capCTemp.push_back(secondOutlineBinder);
				//	int count_outline_Temp = 0;
				//	foreach(cv::Mat img, outlineCTemp)
				//	{
				//	QImage cap_Test = cvMatToQImage(img);
				//	QString tttt3 = "outline";
				//	tttt3.append(QString("_%1_%2.png").arg(i, 4, 10, QChar('0')).arg(count_outline_Temp, 4, 10, QChar('0')));
				//	cap_Test.save(dir.absolutePath() + "/" + tttt3, "png");
				//	count_outline_Temp++;
				//	}
				//	int count_cap_Temp = 0;
				//	foreach(cv::Mat img, capCTemp)
				//	{
				//		QImage cap_Test = cvMatToQImage(img);
				//		QString tttt3 = "captemp";
				//		tttt3.append(QString("_%1_%2.png").arg(i, 4, 10, QChar('0')).arg(count_cap_Temp, 4, 10, QChar('0')));
				//		cap_Test.save(dir.absolutePath() + "/" + tttt3, "png");
				//		count_cap_Temp++;
				//	}
				//}

				SKT::mirrorImage(finalcolor, mirrorColor);
				SKT::mirrorImage(finalBinder, mirrorBinder);
				bool printTwice = par.getBool("DM_TWICE_IN_ONE_LAYER");
				if (!printTwice)
				{
					outlineCTemp.push_back(finalcolor);
					capCTemp.push_back(finalBinder);
				}
				else
				{
					outlineCTemp.push_back(finalcolor);
					outlineCTemp.push_back(finalcolor);
					capCTemp.push_back(finalBinder);
					capCTemp.push_back(finalBinder);
				}

				/* int count_outline_Temp = 0;
				foreach(cv::Mat img, outlineCTemp)
				{

				QImage cap_Test = cvMatToQImage(img);
				QString tttt3 = "outline";
				tttt3.append(QString("_%1_%2.png").arg(i, 4, 10, QChar('0')).arg(count_outline_Temp, 4, 10, QChar('0')));
				cap_Test.save(dir.absolutePath() + "/" + tttt3, "png");
				count_outline_Temp++;
				}

				int count_cap_Temp = 0;
				foreach(cv::Mat img, capCTemp)
				{

				QImage cap_Test = cvMatToQImage(img);
				QString tttt3 = "captemp";
				tttt3.append(QString("_%1_%2.png").arg(i, 4, 10, QChar('0')).arg(count_cap_Temp, 4, 10, QChar('0')));
				cap_Test.save(dir.absolutePath() + "/" + tttt3, "png");
				count_cap_Temp++;
				}*/







				//switch (printDirectionMode)
				//{
				//case 0:// positive
				//{
				//  outlineCTemp.push_back(finalcolor);
				//  capCTemp.push_back(finalBinder);
				//}
				// break;
				//case 1:// negative
				//{
				//  outlineCTemp.push_back(finalcolor.setTo(cv::Scalar(255, 255, 255)));
				//  outlineCTemp.push_back(mirrorColor);
				//  capCTemp.push_back(finalBinder.setTo(cv::Scalar(255)));
				//  capCTemp.push_back(mirrorBinder);
				//}
				// break;
				//case 2:// positive + negative
				//{
				//  outlineCTemp.push_back(finalcolor);
				//  outlineCTemp.push_back(mirrorColor);
				//  capCTemp.push_back(finalBinder);
				//  capCTemp.push_back(mirrorBinder);
				//}

				// break;

				//}

				double binderRatio = SKT::imageComplexity(finalBinder);


			}


			if (save_cap_Image){

				QImage cap_Test = cvMatToQImage(captemp);
				img_file_name2.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				cap_Test.save(dir.absolutePath() + "/" + img_file_name2, "png");
			}


			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("generate_final_picture"));
			if (savedebugImage)
				//if (true)
			{
#if 1
				////picasso/////////////////////////////////////////////////
				//if (i < start_page + generateNOI)
									   {

										   //QImage ttt(cvMatToQImage(outlineCTemp.back()));
										   QImage ttt(cvMatToQImage(originalOutline));
										   QString tttt = "tttt_originalOutline";
										   tttt.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
										   ttt.save(dir.absolutePath() + "/" + tttt, "png");
										   qDebug() << "dir.absolutePath()" << dir.absolutePath();


										   //////////////black_mask///////////////////////////////
										   //QImage black_mask_QI(cvMatToQImage(black_Mask));
										   ////QImage ttt(cvMatToQImage(originalOutline));
										   //QString tttt2 = "black_mask_QI";
										   //tttt2.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
										   //black_mask_QI.save(dir.absolutePath() + "/" + tttt2, "png");
										   //////////captemp//////////
										   QImage binder_image(cvMatToQImage(captemp));
										   //QImage ttt(cvMatToQImage(originalOutline));
										   QString binder_image_text = "binder_image";
										   binder_image_text.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
										   binder_image.save(dir.absolutePath() + "/" + binder_image_text, "png");
										   ///////////////////////////
									   }

#endif

			}

			if (doubleprint && savedebugImage){
				QString backimgname = "backImage";
				//QImage backbinderqq = cvMatToQImage(outlineCTemp.back());
				QImage backbinderqq = cvMatToQImage(capCTemp.back());
				backimgname.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				backbinderqq.save(dir.absolutePath() + "/" + backimgname, "png");
			}

			/* int count_outline_Temp = 0;
			foreach(cv::Mat img, outlineCTemp)
			{
			QImage cap_Test = cvMatToQImage(img);
			QString tttt3 = "outline";
			tttt3.append(QString("_%1_%2.png").arg(i, 4, 10, QChar('0')).arg(count_outline_Temp, 4, 10, QChar('0')));
			cap_Test.save(dir.absolutePath() + "/" + tttt3, "png");
			count_outline_Temp++;
			}
			int count_cap_Temp = 0;
			foreach(cv::Mat img, capCTemp)
			{
			QImage cap_Test = cvMatToQImage(img);
			QString tttt3 = "captemp";
			tttt3.append(QString("_%1_%2.png").arg(i, 4, 10, QChar('0')).arg(count_cap_Temp, 4, 10, QChar('0')));
			cap_Test.save(dir.absolutePath() + "/" + tttt3, "png");
			count_cap_Temp++;
			}*/





			if (par.getBool("generate_zx") && j % send_page_num == 0)//幾頁傳到已產生的zxfile一次
			{
				bool sendResult = zx_skt->sendData2(outlineCTemp, capCTemp);
				wchar_t *vOut = sendResult ? L"true" : L"false";
				WRITELOG(logger, framework::Diagnostics::LogLevel::Info, (vOut));

				capCTemp.clear();
				outlineCTemp.clear();


				if (j == boxy_dim)
				{
					zx_skt->close();
					zx_skt->~ZxGen_SKT();
					Log("end ZX %i", j);
				}
				else
					if (i == start_page + testPrintPageCount - 1 && testswitch)
					{
						zx_skt->close();
						zx_skt->~ZxGen_SKT();
					}

				//fprintf(dbgff, "zx_gen %i, %i\n", time.elapsed(), i);
				//fflush(dbgff);

			}
			else if (par.getBool("generate_zx") && j % send_page_num != 0 && j == boxy_dim)//
			{

				zx_skt->sendData2(outlineCTemp, capCTemp);
				capCTemp.clear();
				outlineCTemp.clear();
				//time.elapsed();
				zx_skt->close();
				zx_skt->~ZxGen_SKT();
				Log("2end ZX  %i", j);

				//fprintf(dbgff, "zx_gen %i, %i\n", time.elapsed(), i);
				//fflush(dbgff);
			}
			QString tempt = makeOutputname(zx_dir.absolutePath(), j);
			if (j % zx_job_pages == 0)//一個job，產生下一個job
			{
				md.jobname.push_back(tempt);
				zx_skt->close();
				zx_skt->~ZxGen_SKT();
				//zx_skt = new ZxGen_SKT(unsigned int(width), unsigned int(((height*meCorrectPercent) + plus_pixel)), zx_job_pages, md.groove.DimX(), md.groove.DimY() + PLUS_MM, tempt.toStdString(), start_print_b);//20161219_backup
				zx_skt = new ZxGen_SKT(300, zx_job_pages, zx_sn.toStdString(), start_print_b);//20161219_backup
				//zggg = new ZxGen(unsigned int(width), unsigned int(4713), zx_job_pages, md.groove.DimX(), md.groove.DimY() + PLUS_MM, tempt.toStdString(), start_print_b);//20161219_backup

			}

			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("senddata end"));
			char* numpages = new char[20];
			string temp = std::to_string(i).append("_pages");
			strcpy(numpages, temp.c_str());
			wchar_t *wmsg = new wchar_t[strlen(numpages) + 1]; //memory allocation
			mbstowcs(wmsg, numpages, strlen(numpages) + 1);
			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, wmsg);
			delete[]wmsg;
			delete[]numpages;


			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();



			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("generate_final_picture"));
		}
		WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("for loop end"));
		//thread1.exit();


#endif	

		ctx.unbindReadDrawFramebuffer();
		ctx.release();
		glPopAttrib();
		glContext->doneCurrent();

		//image.rgbSwapped().mirrored().save(par.getSaveFileName("ImageFileName"));
		//image.mirrored().save(par.getSaveFileName("ImageFileName"));
		//cap_image.mirrored().save(par.getSaveFileName("CapImageFileName"));
		//image.save(par.getSaveFileName("ImageFileName"));


		foreach(MeshModel *bm, md.meshList)
		{
			if ((bm->label().contains(olMName, Qt::CaseSensitive) || bm->label().contains(cMName, Qt::CaseSensitive)))
				//if (bm->label().contains(olMName, Qt::CaseSensitive) || bm->label().contains(cMName, Qt::CaseSensitive)))
				//if (bm->getMeshSort()==MeshModel::meshsort::)
			{
				md.delMesh(bm);
			}
		}

		/*if (par.getBool("generate_zx")){

		QString ppath = "cd /d " + getRoamingDir();
		QStringList arguments;
		arguments << "/k" << ppath << "&&" << "usbsend.exe" << "output.zx" << "&&" << "exit";

		}*/



		qDebug() << " done _applyClick ";
		break;
	}

#pragma endregion FP_PRINT_DM_SLICER

#pragma region FP_PRINT_DM_SLICER_ALPHA
	case FP_PRINT_DM_SLICER_ALPHA:
	{
		WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("FP_PRINT_DM_SLICER"));


		md.jobname.clear();
		//***************************													

		glContext->makeCurrent();


		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("glew_error"));
		}

		glPushAttrib(GL_ALL_ATTRIB_BITS);

		Context ctx;
		ctx.acquire();
		if (switchInitMeshVBO)
			initMeshVBO(md);

		//***20150907
		const GLsizei print_dpi = GLsizei(par.getInt("print_dpi"));
		float x = md.groove.DimX();
		float y = md.groove.DimY();
		const GLsizei width = (int)(x / DSP_inchmm * print_dpi);
		const GLsizei height = (int)(width * y / x);

		//qDebug() << "width" << "height" << width << height;
		//RenderbufferHandle hDepth = createRenderbuffer(ctx, GL_DEPTH_COMPONENT24, width, height);
		RenderbufferHandle hDepth = createRenderbuffer(ctx, GL_DEPTH24_STENCIL8, width, height);
		Texture2DHandle    hColor = createTexture2D(ctx, GL_RGB8, width, height, GL_RGB, GL_UNSIGNED_BYTE);
		//FramebufferHandle  hFramebuffer = createFramebuffer(ctx, renderbufferTarget(hDepth), texture2DTarget(hColor));
		FramebufferHandle  hFramebuffer = createFramebufferWithDepthStencil(ctx, renderbufferTarget(hDepth), renderbufferTarget(hDepth), texture2DTarget(hColor));




		const vcg::Point3f lightDirectionVS = vcg::Point3f(0.0f, 0.0f, -1.0f).Normalize();

		glEnable(GL_DEPTH_TEST);
		//glClearColor(GLfloat(backgroundColor.red())/255.0f, GLfloat(backgroundColor.green())/255.0f, GLfloat(backgroundColor.blue())/255.0f, 0.0f);
		Construction_Groove<float> tempgroove = md.groove;

		glViewport(0, 0, width, height);//***window的長寬				  

		/*const vcg::Point3f center = mesh.bbox.Center();
		const float        scale = 1.0f / mesh.bbox.Diag();*/

		//glScalef(scale, scale, scale);
		//glTranslatef(-center[0], -center[1], -center[2]);

		//***儲存image的基本資訊
		QImage image(int(width), int(height), QImage::Format_RGB888);

		/*captemp = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		capDownMask = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		black_Mask = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));

		outlineBlackMaskCV = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		prePrintobjectC = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));

		up_downBlackMaskCV = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));

		originalOutline = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		finalcolor = *new cv::Mat(height, width, originalOutline.type(), Scalar(255, 255, 255));
		finalBinder = *new cv::Mat(height, width, captemp.type(), Scalar(255));
		backbinder = *new cv::Mat(finalBinder.size(), finalBinder.type(), cv::Scalar(255));*/


		cv::Mat captemp(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat copyCaptemp(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat capDownMask(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat capTopMask(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat black_Mask(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat outlineBlackMaskCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat prePrintobjectC(height, width, CV_8UC3, Scalar(255, 255, 255));
		//cv::Mat up_downBlackMaskCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat originalOutline(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat finalcolor(height, width, originalOutline.type(), Scalar(255, 255, 255));
		cv::Mat finalBinder(height, width, captemp.type(), Scalar(255));
		cv::Mat backbinder(finalBinder.size(), finalBinder.type(), cv::Scalar(255));

		cv::Mat capDownSureMask(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat capTopSureMask(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat prePrintobjectB(height, width, CV_8UC3, Scalar(255, 255, 255));




		//***20150505***slice_func_設置高度
		float box_bottom = md.bbox().min.Z();
		//float unit = par.getFloat("slice_height");
		float unit = par.getFloat("DM_MONO_LAYER_HEIGHT");



		int boxy_dim = qCeil((md.bbox().max.Z() - md.bbox().min.Z()) / unit) + 1;

		init_texture(md);


		QString temppath = PicaApplication::getRoamingDir();
		QDir dir(temppath);//backup
		if (dir.exists("xyzimage"))
		{
			dir.setPath(temppath + "/xyzimage");
			dir.removeRecursively();
			dir.setPath(temppath);
			dir.mkpath("xyzimage");
		}
		else
		{
			dir.setPath(temppath);
			dir.mkpath("xyzimage");
		}

		dir.setPath(temppath + "/xyzimage");

		//---zx_file------------------------------------------
		//QString zxtemp = "D:/temptemp";
		QString zxtemp = PicaApplication::getRoamingDir();
		QDir zx_dir(zxtemp);
		if (zx_dir.exists("zx_file"))
		{
			zx_dir.setPath(zxtemp + "/zx_file");
			zx_dir.removeRecursively();
			zx_dir.setPath(zxtemp);
			zx_dir.mkpath("zx_file");
		}
		else
		{
			dir.setPath(temppath);
			dir.mkpath("zx_file");
		}

		zx_dir.setPath(zxtemp + "/zx_file");

		//==============================
		//**********************
		ctx.bindReadDrawFramebuffer(hFramebuffer);
		GLW_CHECK_GL_READ_DRAW_FRAMEBUFFER_STATUS;
#if 1

		const bool outputOL_Image = par.getBool("OL_Image");
		const bool save_cap_Image = par.getBool("Cap_Image");

		const bool faceColor = par.getBool("FaceColor");
		glClearColor(1, 1, 1, 1);
		QString olMName = md.p_setting.getOlMeshName();
		QString cMName = md.p_setting.getCapMeshName();

		//***opencvMat clear
		capCTemp.clear();
		outlineCTemp.clear();


		int zx_job_pages = par.getInt("per_zx_job");
		//int start_page = par.getInt("start_page");
		int send_page_num = par.getInt("send_page_num");
		int plus_pixel = par.getInt("plus_pixel");
		float PLUS_MM = par.getFloat("PLUS_MM");
		bool start_print_b = par.getBool("PRINT_START");
		float useless_print = par.getFloat("USELESS_PRINT");
		float spittoon_M = par.getFloat("SPITTOON_M");
		float spittoon_C = par.getFloat("SPITTOON_C");
		float spittoon_Y = par.getFloat("SPITTOON_Y");
		float spittoon_B = par.getFloat("SPITTOON_B");
		bool add_pattern = par.getBool("ADD_PATTERN");
		string desProfilePath = par.getString("COLOR_PROFILE").toStdString();
		QFileInfo tempProfilePath(QString::fromStdString(desProfilePath));
		bool doubleprint = par.getBool("Stiff_Print");
		bool dilateBinder = par.getBool("DILATE_BINDER");
		int dilateBinderValue = par.getInt("DILATE_BINDER_VALUE");
		bool dynamicValueSwitch = par.getBool("DYNAMIC_WIPE");
		bool colorBinding = par.getBool("COLOR_BINDING");
		bool doublePrint_V2 = par.getBool("STIFF_PRIN_V2");
		int doublePrint_V2Percent = par.getInt("STIFF_PRINT_VALUE");

		int doublePrint_shell_Percent = par.getInt("SHELL_PERCENT");
		bool CMYB_black_mode = par.getBool("CMYB_BLACK");
		int CMYB_black_percentage = par.getInt("CMYB_BLACK_VALUE");
		bool usePatternPercent = true;
		bool reversePrint = false;
		int spitton_start_position = par.getInt("SPITTON_START_POSITION");

		/*
		DM_Parameter
		*/
		int printDirectionMode = 0;

		double horizontal_Thick = par.getFloat("DM_SHELL_HORIZONTAL_THICKNESS");//Horizontal Thick
		bool horizontal_ThickC = par.getBool("DM_SHELL_HORIZONTAL_THICKNESS_C");
		bool horizontal_ThickM = par.getBool("DM_SHELL_HORIZONTAL_THICKNESS_M");
		bool horizontal_ThickY = par.getBool("DM_SHELL_HORIZONTAL_THICKNESS_Y");
		bool horizontal_ThickB = par.getBool("DM_SHELL_HORIZONTAL_THICKNESS_B");
		int horizon_R_Value = 255 - par.getInt("DM_HORIZONTAL_C_Value");
		int horizon_G_Value = 255 - par.getInt("DM_HORIZONTAL_M_Value");
		int horizon_B_Value = 255 - par.getInt("DM_HORIZONTAL_Y_Value");
		int horizon_K_Value = 255 - par.getInt("DM_HORIZONTAL_K_Value");
		int horizonCellSize = par.getInt("DM_HORIZONTAL_CELL_SIZE");
		int horizonPatternPercent = par.getInt("DM_HORIZONTAL_PERCENTAGE");
		int horizontal_Pixel_Value = par.getInt("DM_HORIZONTAL_PIXEL_VALUE");

		double vertical_Thick = par.getFloat("DM_SHELL_VERTICAL_THICKNESS");//Vertical Thick
		if (vertical_Thick == 0) vertical_Thick = 0.001;
		bool vertical_ThickC = par.getBool("DM_SHELL_VERTICAL_THICKNESS_C");
		bool vertical_ThickM = par.getBool("DM_SHELL_VERTICAL_THICKNESS_M");
		bool vertical_ThickY = par.getBool("DM_SHELL_VERTICAL_THICKNESS_Y");
		bool vertical_ThickB = par.getBool("DM_SHELL_VERTICAL_THICKNESS_B");
		int top_R_Value = 255 - par.getInt("DM_SHELL_TOP_C_Value");
		int top_G_Value = 255 - par.getInt("DM_SHELL_TOP_M_Value");
		int top_B_Value = 255 - par.getInt("DM_SHELL_TOP_Y_Value");
		int top_K_Value = 255 - par.getInt("DM_SHELL_TOP_K_Value");
		int verticalCellSize = par.getInt("DM_VERTICAL_CELL_SIZE");
		int verticalPatternPercent = par.getInt("DM_VERTICAL_PERCENTAGE");
		int vertical_Pixel_Value = par.getInt("DM_VERTICAL_PIXEL_VALUE");

		double top_Thickness = par.getFloat("DM_SHELL_VERTICAL_THICKNESS");//Vertical Thick

		double bottom_lighter_Thick = par.getFloat("DM_SHELL_BOTTOM_LIGHTER");//Horizontal Thick
		bool bottom_lighterC = par.getBool("DM_SHELL_BOTTOM_LIGHTER_C");
		bool bottom_lighterM = par.getBool("DM_SHELL_BOTTOM_LIGHTER_M");
		bool bottom_lighterY = par.getBool("DM_SHELL_BOTTOM_LIGHTER_Y");
		bool bottom_lighterB = par.getBool("DM_SHELL_BOTTOM_LIGHTER_B");
		int bottom_R_Value = 255 - par.getInt("DM_SHELL_BOTTOM_C_Value");
		int bottom_G_Value = 255 - par.getInt("DM_SHELL_BOTTOM_M_Value");
		int bottom_B_Value = 255 - par.getInt("DM_SHELL_BOTTOM_Y_Value");
		int bottom_K_Value = 255 - par.getInt("DM_SHELL_BOTTOM_K_Value");
		int bottomCellSize = par.getInt("DM_BOTTOM_CELL_SIZE");
		int bottomPatternPercent = par.getInt("DM_BOTTOM_PERCENTAGE");
		int bottom_Pixel_Value = par.getInt("DM_BOTTOM_PIXEL_VALUE");

		bool innerC = par.getBool("DM_INNER_C");
		bool innerM = par.getBool("DM_INNER_M");
		bool innerY = par.getBool("DM_INNER_Y");
		bool innerB = par.getBool("DM_INNER_B");
		int inner_R_Value = 255 - par.getInt("DM_INNER_C_Value");
		int inner_G_Value = 255 - par.getInt("DM_INNER_M_Value");
		int inner_B_Value = 255 - par.getInt("DM_INNER_Y_Value");
		int inner_K_Value = 255 - par.getInt("DM_INNER_K_Value");
		int innerCellSize = par.getInt("DM_INNER_CELL_SIZE");
		int innerPatternPercent = par.getInt("DM_INNER_PERCENTAGE");
		int innerPixelValue = par.getInt("DM_INNER_PIXEL_VALUE");

		int dmPixelNum = par.getInt("DM_DITHER_PIXEL_NUM");// all the same value

		int dmSlicingMode = par.getEnum("DM_SLICING_MODE");

		int dmSPitMode = par.getEnum("DM_SPIT_SWITCH");

		double bottom_Thick_sure = 0.2;//objects gap must larger than this
		double top_Thick_sure = 0.2;
		/*==================================================================================================================
		====================================================================================================================*/

		//int shellPercent = 100;
		//int erodeIteration = ( DM_binderThick*(print_dpi / DSP_inchmm) ) - 5;


		bool prePrintBool = true;//witness bar

		bool fourPage_switch = false;


		bool continuePrinting = par.getBool(("CONTINUE_PRINTING"));
		int start_page = par.getInt("CONTINUE_PRINTING_PAGE");
		int end_page = par.getInt("CONTINUE_PRINTING_END_PAGE");


		assert(tempProfilePath.exists());
		//float meCorrectPercent = 0.97;
		float meCorrectPercent = 1;

		//********control profile on && DSP_TEST_MODE
		bool profileOn = false;
		bool savedebugImage =  par.getBool("DM_RESULT_IMG");;// par.getBool(("generate_final_picture"));
		bool line_or_quads = false;// par.getBool("LINE_OR_RECT");;//true line, false quads
		bool savefinalImage = par.getBool("DM_RESULT_IMG");
		bool saveReverseImage = false;
		bool draw_circle_at_edge_point = true;


		SKT::pixelFormat gen_pixelFormat;
		ZxGen_SKT::pixelFormat genZxPixelFormat;
		ZxGen_SKT::zxInitMode zxCheckMode;
		if (profileOn)
		{
			gen_pixelFormat = SKT::pixelFormat::CMY;
			genZxPixelFormat = ZxGen_SKT::pixelFormat::CMY;
		}
		else
		{
			gen_pixelFormat = SKT::pixelFormat::RGB;
			genZxPixelFormat = ZxGen_SKT::pixelFormat::RGB;
		}

		zxCheckMode = ZxGen_SKT::zxInitMode::ViewCheckMode;
		/*if (DSP_TEST_MODE)
		{
		zxCheckMode = ZxGen_SKT::zxInitMode::ViewCheckMode;
		}
		else
		{
		zxCheckMode = ZxGen_SKT::zxInitMode::RealPrintMode;

		}*/


		int widthrecord, heightrecord;
		QString blk_img_file_name = "black_outline";

		QString zxFilePath = par.getString("ZX_File_Path");
		QFileInfo zxtempFileFinfo(zxFilePath);
		QString zx_sn = makeOutputname2(zx_dir.absolutePath(), zxtempFileFinfo.completeBaseName());
		QString zx_sn2 = makeOutputname(zx_dir.absolutePath(), 0);
		QString icm_fileName = par.getString("DM_ICM_FOR_DITHER");

		md.jobname.push_back(zx_sn);

		//zggg = new ZxGen(unsigned int(width), unsigned int(height), zx_pages, md.groove.DimX(),21, zx_sn.toStdString());//20161219_backup						 
		//zggg = new ZxGen(unsigned int(width), 2580, zx_job_pages, md.groove.DimX(), 21, zx_sn.toStdString());//20161219_backup						 
		//zx_skt = new ZxGen(unsigned int(width), unsigned int(height*meCorrectPercent + plus_pixel), zx_job_pages, md.groove.DimX(), md.groove.DimY() + PLUS_MM, zx_sn.toStdString(), start_print_b);//20161219_backup						 

		//zx_skt = new ZxGen_SKT(print_dpi, zx_job_pages, zx_sn.toStdString(), start_print_b, genZxPixelFormat, zxCheckMode);//20161219_backup			
		if (zxFilePath.isEmpty())
			zx_skt = new ZxGen_SKT(print_dpi, zx_job_pages, zx_sn2.toStdString(), start_print_b, genZxPixelFormat, zxCheckMode, icm_fileName.toStdWString());//		
		else
			zx_skt = new ZxGen_SKT(print_dpi, zx_job_pages, zx_sn.toStdString(), start_print_b, genZxPixelFormat, zxCheckMode, icm_fileName.toStdWString());//		
		
		
		//if (zxFilePath.isEmpty())
		//	zx_skt = new ZxGen_SKT(print_dpi, zx_job_pages, zx_sn.toStdString(), start_print_b, genZxPixelFormat, zxCheckMode);//20161219_backup						 
		//else
		//	zx_skt = new ZxGen_SKT(print_dpi, zx_job_pages, zxFilePath.toStdString(), start_print_b, genZxPixelFormat, zxCheckMode, icm_fileName.toStdWString());//20161219_backup		

		std::vector<cv::Mat> blankTemp;
		std::vector<cv::Mat> blankcapCTemp;


		//if (start_page == 0)
		//{
		WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("newUselessbar_customateBlankPages"));
		if (!zxCheckMode == ZxGen_SKT::zxInitMode::ViewCheckMode  && continuePrinting == false && fourPage_switch)
		{

			SKT::createBlankPages(doubleprint, tempgroove.grooveheight, md.p_setting.get_plus_print_length(), useless_print, DSP_Blank_pages, blankTemp, blankcapCTemp, gen_pixelFormat, spittoon_M, spittoon_C, spittoon_Y, spittoon_B, print_dpi);
			////WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("sendData2"));
			zx_skt->sendData2(blankTemp, blankcapCTemp);
		}
		//}
		//Log("ZxGen width %i height %i ", int(width), int(height*meCorrectPercent + plus_pixel));
		//FILE *dbgff;
		//dbgff = fopen("D:\\debug_open_1.txt", "w");//@@@
		QTime time;
		//clock_t start, stop;
		time.start();



		QDir patternPath(PicaApplication::getRoamingDir());

		if (patternPath.exists("pattern"))
		{
			patternPath.setPath(PicaApplication::getRoamingDir() + "/pattern");
		}
		else
		{
			patternPath.setPath(PicaApplication::getRoamingDir());
			patternPath.mkpath("pattern");
			patternPath.setPath(PicaApplication::getRoamingDir() + "/pattern");
			//patternPath.setPath( "./pattern");

		}

		/*
		create Pattern Imgages for different usage
		inner, Horizontal, Vertical shell
		*/


		cv::Mat horizonMagicSquareCell(cv::Size(horizonCellSize, horizonCellSize), CV_8UC1, Scalar(255));
		cv::Mat verticalMagicSquareCell(cv::Size(verticalCellSize, verticalCellSize), CV_8UC1, Scalar(255));
		cv::Mat bottomMagicSquareCell(cv::Size(bottomCellSize, bottomCellSize), CV_8UC1, Scalar(255));
		cv::Mat innerMagicSquareCell(cv::Size(innerCellSize, innerCellSize), CV_8UC1, Scalar(255));

		if (usePatternPercent)
		{
			//SKT::createPatternPercent(patternfilter, 36);

			SKT::MagicSquarePattern(horizonMagicSquareCell, horizonPatternPercent, horizonCellSize);
			SKT::MagicSquarePattern(verticalMagicSquareCell, verticalPatternPercent, verticalCellSize);
			SKT::MagicSquarePattern(bottomMagicSquareCell, bottomPatternPercent, bottomCellSize);
			SKT::MagicSquarePattern(innerMagicSquareCell, innerPatternPercent, innerCellSize);

		}

		cv::Mat horizontalMagicSquarePattern(cv::Size(width, height), CV_8U, Scalar(255));
		cv::Mat verticalMagicSquarePattern(cv::Size(width, height), CV_8U, Scalar(255));
		cv::Mat bottomMagicSquarePattern(cv::Size(width, height), CV_8U, Scalar(255));
		cv::Mat innerMagicSquarePattern(cv::Size(width, height), CV_8U, Scalar(255));

		SKT::createPatternImage(horizonMagicSquareCell, horizontalMagicSquarePattern);
		SKT::createPatternImage(verticalMagicSquareCell, verticalMagicSquarePattern);
		SKT::createPatternImage(bottomMagicSquareCell, bottomMagicSquarePattern);
		SKT::createPatternImage(innerMagicSquareCell, innerMagicSquarePattern);


		if (savedebugImage){
			QImage pastePattern(cvMatToQImage(innerMagicSquarePattern));
			QString pastePatternS = "innerMagicSquarePattern";
			pastePatternS.append(QString(".png"));
			pastePattern.save(dir.absolutePath() + "/" + pastePatternS, "png");
		}
		if (savedebugImage){
			QImage pastePattern(cvMatToQImage(horizontalMagicSquarePattern));
			QString pastePatternS = "magicSquareCell_for_verticalShell";
			pastePatternS.append(QString(".png"));
			pastePattern.save(dir.absolutePath() + "/" + pastePatternS, "png");
		}
		if (savedebugImage){
			QImage pastePattern(cvMatToQImage(verticalMagicSquarePattern));
			QString pastePatternS = "magicSquareCell_for_horizontalShell";
			pastePatternS.append(QString(".png"));
			pastePattern.save(dir.absolutePath() + "/" + pastePatternS, "png");
		}
		if (savedebugImage){
			QImage pastePattern(cvMatToQImage(bottomMagicSquarePattern));
			QString pastePatternS = "magicSquareCell_for_bottom";
			pastePatternS.append(QString(".png"));
			pastePattern.save(dir.absolutePath() + "/" + pastePatternS, "png");
		}





		//***testLog
		//Log("Closed %i holes and added %i new faces", 12, 12);
		//boxy_dim--;
		int testPrintPageCount;// = 10;
		bool testswitch;// = false;

		int generateNOI = 25;
		int quickWipe = 1;
		WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("for loop start"));
		if (start_page > boxy_dim && continuePrinting){
			zx_skt->close();
			zx_skt->~ZxGen_SKT();
			return false;
		}

		if (continuePrinting)
		{
			testPrintPageCount = end_page - start_page;
			boxy_dim = start_page + testPrintPageCount;
			testswitch = true;
		}
		else
		{
			start_page = 0;
			testPrintPageCount = boxy_dim;
			testswitch = false;
		}


		cv::Mat temp(width, height, CV_8UC3, Scalar(255, 255, 255));
		temp = SKT::resizeIamgecols(&finalcolor, plus_pixel, gen_pixelFormat);

		cv::Mat temp2 = SKT::rotateImageV2(temp);

		cv::Mat testforReverseColorFirst(temp2.size(), CV_8UC3, cv::Scalar(255, 255, 255));
		cv::Mat testforReverseBinderFirst(temp2.size(), CV_8UC1, cv::Scalar(255));

		cv::Mat backColor(temp2.size(), CV_8UC3, cv::Scalar(255, 255, 255));
		cv::Mat backBinder(temp2.size(), CV_8UC1, cv::Scalar(255));
		//if (doubleprint){
		if (reversePrint)
		{
			//create back print image
			std::vector<int> collectRowPosition;
			collectRowPosition.push_back(70);
			collectRowPosition.push_back(4080);
			collectRowPosition.push_back(5600);


			SKT::genSacrificeBar(backColor, backBinder, collectRowPosition, 13);



			std::vector<int> collectRowPosition2;
			collectRowPosition2.push_back(115);
			collectRowPosition2.push_back(1635);
			collectRowPosition2.push_back(5645);

			SKT::genSacrificeBar(testforReverseColorFirst, testforReverseBinderFirst, collectRowPosition2, 13);


			if (saveReverseImage){
				QImage pastePattern(cvMatToQImage(backColor));
				QString pastePatternS = "backColor";
				pastePatternS.append(QString("_%1.png").arg(1, 4, 10, QChar('0')));
				pastePattern.save(dir.absolutePath() + "/" + pastePatternS, "png");
			}

			if (saveReverseImage){
				QImage pastePattern(cvMatToQImage(backBinder));
				QString pastePatternS = "backBinder";
				pastePatternS.append(QString("_%1.png").arg(1, 4, 10, QChar('0')));
				pastePattern.save(dir.absolutePath() + "/" + pastePatternS, "png");
			}

			if (saveReverseImage){
				QImage pastePattern(cvMatToQImage(testforReverseColorFirst));
				QString pastePatternS = "testforReverseColorFirst";
				pastePatternS.append(QString("_%1.png").arg(1, 4, 10, QChar('0')));
				pastePattern.save(dir.absolutePath() + "/" + pastePatternS, "png");
			}

			if (saveReverseImage){
				QImage pastePattern(cvMatToQImage(testforReverseBinderFirst));
				QString pastePatternS = "testforReverseBinderFirst";
				pastePatternS.append(QString("_%1.png").arg(1, 4, 10, QChar('0')));
				pastePattern.save(dir.absolutePath() + "/" + pastePatternS, "png");
			}



		}


		//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("gen log"));
		//for (int i = start_page, j = start_page + 1; i < boxy_dim; i++, j++)
		for (int i = start_page, j = start_page + 1; i < start_page + testPrintPageCount; i++, j++)
		{
			time.restart();





			//resetImage(look_down_Black_Mask_layer, width, height);//
			//resetImage(look_down_color_CV, width, height);
			//resetImage(look_down_Black_MaskCV, width, height);//
			//resetImage(look_up_Black_MaskCV, width, height);//
			//resetImage(look_up_color_CV, width, height);
			resetImage(black_Mask, width, height);
			//resetImage(black_Mask2, width, height);
			//resetImage(lookDownBlackTriangleMask, width, height);//
			resetImage(outlineBlackMaskCV, width, height);//
			resetImage(prePrintobjectC, width, height);


			//resetImage(downColorFirstCV, width, height);
			//resetImage(downBlackMaskFirstCV, width, height);
			//resetImage(downColorSecondCV, width, height);
			//resetImage(downBlackMaskSecondCV, width, height);
			//resetImage(up_downBlackMaskCV, width, height);



			//resetImage(outlineColorCv, width, height);
			resetImage(captemp, width, height);

			resetImage(capDownMask, width, height);
			resetImage(capTopMask, width, height);

			resetImage(capDownSureMask, width, height);
			resetImage(capTopSureMask, width, height);


			md.wipeFlag.push_back(true);
			if (cb && (i % 1) == 0)
			{
				glContext->doneCurrent();
				bool break_test = false;
				if (start_print_b)
				{
					break_test = cb((i * 100) / boxy_dim, "start_printing_slice_process");
					/*if (i % 10 == 0)
					quickWipe = 2;
					else
					quickWipe = 4;*/
					if (i > 1 && dynamicValueSwitch)
					{
						int temp = i - 1 + 5;
						sb(temp, quickWipe);
					}
				}
				else
				{
					break_test = cb((i * 100) / boxy_dim, "slice_process");

					if (i > 1 && dynamicValueSwitch)
						sb(i - 1 + 5, quickWipe);
				}
				if (!break_test)//callback2 return cancel, need to close zx_SKT
				{
					zx_skt->close();
					zx_skt->~ZxGen_SKT();

					glContext->doneCurrent();
					return false;
					break;
				}

			}
			glContext->makeCurrent();
			//xyz::slice_roution(md, box_bottom + unit*i, faceColor);//***backup******************切層演算							  
			//time.restart();
			//Log("Closed %i holes and added %i new faces, %s,%d ", 12, 12, __FUNCTION__, __LINE__);
			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("slice start"));
			xyz::slice_roution(md, box_bottom + unit*i, faceColor);//****20160301*****************切層演算		
			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("slice end"));


			float currnentHeight = box_bottom + unit*i;


			//time.restart();
			//***第一張outline圖*****************************
			QVector<MeshModel *> vmm;
			md.getMeshByContainString("_temp_outlines", vmm);
			md.getMeshByContainString(olMName, vmm);
			QVector<MeshModel *> cmm;
			md.getMeshByContainString(cMName, cmm);//capImage
			int xx = 0;
			QString img_file_name = md.p_setting.getoutlineName();
			float cut_z = box_bottom + unit*i;


			glPixelStorei(GL_PACK_ALIGNMENT, 1);//OpenGL PixelStori Setting


			/*==============start outline black Mask============================*/
			glMatrixMode(GL_PROJECTION); glPopMatrix();
			glMatrixMode(GL_MODELVIEW); glPopMatrix();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);
			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -1000, 1000);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
			gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, 0, 0, 1, 0);
			foreach(MeshModel *mp, vmm)//outline
			{
				mp->glw.viewerNum = 2;
				mp->outlineThick = horizontal_Thick;
				mp->drawCircleInQuad = draw_circle_at_edge_point;
				if (vmm.size() > 0)
				{
					if (line_or_quads)
						mp->render(vcg::GLW::DMTexWire, vcg::GLW::CMBlack, vcg::GLW::TMNone);//backup
					else
						mp->render(vcg::GLW::DMTexWireQuad, vcg::GLW::CMBlack, vcg::GLW::TMNone);//backup
				}
			}
			glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, outlineBlackMaskCV.data);
			cv::flip(outlineBlackMaskCV, outlineBlackMaskCV, 0);

			if (savedebugImage){
				image = cvMatToQImage(outlineBlackMaskCV);
				QString outlineBlackMask = "outlineBlackMask";
				outlineBlackMask.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				image.save(dir.absolutePath() + "/" + outlineBlackMask, "png");
			}
			///////////////////////end outlineBlackMask//////////////////////////
			/*==============start TOP-DOWN black Mask============================*/
			//glMatrixMode(GL_PROJECTION); glPopMatrix();
			//glMatrixMode(GL_MODELVIEW); glPopMatrix();
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			//glClearColor(1, 1, 1, 1);
			//glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			//glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -1000, 1000);
			//glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
			//gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, 0, 0, 1, 0);

			//foreach(MeshModel *mdmm, md.meshList)
			//{
			//	//****20151231********************************************************************								  
			//	if (mdmm->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mdmm->cm.bbox.min.Z() && currnentHeight <= mdmm->cm.bbox.max.Z()))
			//		if (cut_z - mdmm->cm.bbox.min.Z() < 0 || cut_z - mdmm->cm.bbox.max.Z() > 0)
			//		{
			//			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			//			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., 0.0001, 0);
			//			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, cut_z, 0, 0, -DSP_Look_Distination, 0, 1, 0);

			//			mdmm->render(GLW::DMFlat, GLW::CMBlack, GLW::TMNone);
			//			glPopMatrix();

			//		}
			//		else
			//		{
			//			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			//			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -vertical_Thick, vertical_Thick);
			//			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, cut_z, 0, 0, -DSP_Look_Distination, 0, 1, 0);

			//			glPushMatrix();
			//			mdmm->render(GLW::DMFlat, GLW::CMBlack, GLW::TMNone);
			//			glPopMatrix();
			//		}

			//}

			//glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, up_downBlackMaskCV.data);
			//cv::flip(up_downBlackMaskCV, up_downBlackMaskCV, 0);

			//if (savedebugImage){
			//	image = cvMatToQImage(up_downBlackMaskCV);
			//	QString up_downBlackMask = "up_downBlackMaskCV";
			//	up_downBlackMask.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
			//	image.save(dir.absolutePath() + "/" + up_downBlackMask, "png");
			//}
			/*
			//////////////////////end TOP-DOWN Mask//////////////////////////
			*/






			/***20160122***outline and binder thick black mask******/
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);

			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			//glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -100, 100);
			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -1000, 1000);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, 0, 0, 1, 0);
			//*********************************
			//binderstrips(md, par.getFloat("useless_print"));//******uselessbar
			//image.fill(QColor::w);
			foreach(MeshModel *mp, vmm)//outline
			{
				mp->outlineThick = horizontal_Thick;
				mp->glw.viewerNum = 2;
				mp->drawCircleInQuad = draw_circle_at_edge_point;
				if (vmm.size() > 0)
				{
					if (line_or_quads)
						mp->render(vcg::GLW::DMTexWire, vcg::GLW::CMBlack, vcg::GLW::TMNone);//backup
					else
						mp->render(vcg::GLW::DMTexWireQuad, vcg::GLW::CMBlack, vcg::GLW::TMNone);//backup

					//mp->render(vcg::GLW::DMOutline_Test, vcg::GLW::CMNone, vcg::GLW::TMNone);
				}
			}
			foreach(MeshModel *mdmm, md.meshList)
			{
				//****20151231********************************************************************								  
				if (mdmm->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mdmm->cm.bbox.min.Z() && currnentHeight <= mdmm->cm.bbox.max.Z()))
					if (cut_z - mdmm->cm.bbox.min.Z() < 0 || cut_z - mdmm->cm.bbox.max.Z() > 0)
					{
						glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
						glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., 0.0001, 0);
						glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, cut_z, 0, 0, -DSP_Look_Distination, 0, 1, 0);

						mdmm->render(GLW::DMFlat, GLW::CMBlack, GLW::TMNone);
						glPopMatrix();

					}
					else
					{
						glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
						//glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -0.6, 0.6);
						glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -vertical_Thick, vertical_Thick);
						//glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -0.2, 0.2);
						//***20161112
						glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, cut_z, 0, 0, -DSP_Look_Distination, 0, 1, 0);

						glPushMatrix();
						mdmm->render(GLW::DMFlat, GLW::CMBlack, GLW::TMNone);
						glPopMatrix();
					}

			}
			//fFaceColor
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, black_Mask.data);
			cv::flip(black_Mask, black_Mask, 0);
			//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("gen log"));
			//***20160122***DSP_Color_Replace_Binder_thick_mask
			if (savedebugImage){
				QImage testttt = cvMatToQImage(black_Mask);
				QString black_Maskst = "black_Mask";
				black_Maskst.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				testttt.save(dir.absolutePath() + "/" + black_Maskst, "png");
			}

			/*===========================================================================================
			***Binder Information image
			==============================================================================================*/
			equ[3] = box_bottom + unit*i;

			//time.restart();
			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -1000, 1000);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, -DSP_Look_Distination, 0, 1, 0);
			//*********************************
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);

			//binderstrips(md, par.getFloat("useless_print"));//***uselessbar

			//************************************************//
			//****** Rendering the mesh's clip edge ****//
			GLuint MY_CLIP_PLANE = GL_CLIP_PLANE0;
			glEnable(MY_CLIP_PLANE);
			glClipPlane(MY_CLIP_PLANE, equ);

			glEnable(GL_STENCIL_TEST);
			glEnable(GL_CULL_FACE);
			glClear(GL_STENCIL_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

			//// first pass: increment stencil buffer value on back faces
			glStencilFunc(GL_ALWAYS, 0, 0);
			glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
			glCullFace(GL_FRONT); // render back faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			//glRotatef(90, 1, 0, 0);
			foreach(MeshModel * mp, md.meshList)
			{
				if (mp->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mp->cm.bbox.min.Z() && currnentHeight <= mp->cm.bbox.max.Z()))
					//mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
					mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
			}

			glPopMatrix();
			glPopAttrib();
			//second pass: decrement stencil buffer value on front faces
			glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
			glCullFace(GL_BACK); // render front faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			//glRotatef(90, 1, 0, 0);
			foreach(MeshModel * mp, md.meshList)
			{
				if (mp->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mp->cm.bbox.min.Z() && currnentHeight <= mp->cm.bbox.max.Z()))
					//mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
					mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
			}
			glPopMatrix();
			glPopAttrib();
			//-----------------------------
			// drawing clip planes masked by stencil buffer content
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glEnable(GL_DEPTH_TEST);
			glDisable(MY_CLIP_PLANE);
			glStencilFunc(GL_NOTEQUAL, 0, ~0);
			// stencil test will pass only when stencil buffer value = 0; 
			// (~0 = 0x11...11)

			glColor4f(1, 1, 1, 1);
			fillsquare();
			glDisable(GL_STENCIL_TEST);
			glEnable(MY_CLIP_PLANE); // enabling clip plane again
			glDisable(GL_LIGHTING);

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			//********************************************************************************************//


			//glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, cap_image.bits());//***20150724
			glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, captemp.data);
			cv::flip(captemp, captemp, 0);

			QString img_file_name2 = md.p_setting.getcapImageName();
			glDisable(MY_CLIP_PLANE);
			glDisable(GL_CULL_FACE);
			//****								
			//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T(""));
			/*if (par.getBool("generate_zx"))
			{*/
			//cv::Mat tempMat(captemp.size(),CV_8UC1);
			cv::cvtColor(captemp, captemp, CV_BGR2GRAY);
			//captemp = tempMat.clone();

			copyCaptemp = captemp.clone();
			//}
			//if (savedebugImage){
			if (savedebugImage){
				QImage testttt = cvMatToQImage(captemp);
				QString capimagest = "captemp_image";
				capimagest.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				testttt.save(dir.absolutePath() + "/" + capimagest, "png");
			}

			/*===========================================================================================
			***create mask to determine top face
			==============================================================================================*/

			equ[3] = box_bottom + unit*i + top_Thickness;

			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -1000, 1000);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, -DSP_Look_Distination, 0, 1, 0);
			//*********************************
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);

			//************************************************//
			//****** Rendering the mesh's clip edge ****//
			GLuint MY_CLIP_PLANE5 = GL_CLIP_PLANE0;
			glEnable(MY_CLIP_PLANE5);
			glClipPlane(MY_CLIP_PLANE5, equ);

			glEnable(GL_STENCIL_TEST);
			glEnable(GL_CULL_FACE);
			glClear(GL_STENCIL_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

			//// first pass: increment stencil buffer value on back faces
			glStencilFunc(GL_ALWAYS, 0, 0);
			glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
			glCullFace(GL_FRONT); // render back faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			//glRotatef(90, 1, 0, 0);
			foreach(MeshModel * mp, md.meshList)
			{
				if (mp->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mp->cm.bbox.min.Z()) && (currnentHeight <= mp->cm.bbox.max.Z()))
					//mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
					mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
			}

			glPopMatrix();
			glPopAttrib();
			//second pass: decrement stencil buffer value on front faces
			glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
			glCullFace(GL_BACK); // render front faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			//glRotatef(90, 1, 0, 0);
			foreach(MeshModel * mp, md.meshList)
			{
				if (mp->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mp->cm.bbox.min.Z() && currnentHeight <= mp->cm.bbox.max.Z()))
					//mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
					mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
			}
			glPopMatrix();
			glPopAttrib();

			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glEnable(GL_DEPTH_TEST);
			glDisable(MY_CLIP_PLANE5);
			glStencilFunc(GL_NOTEQUAL, 0, ~0);


			glColor4f(1, 1, 1, 1);
			fillsquare();
			glDisable(GL_STENCIL_TEST);
			glEnable(MY_CLIP_PLANE5); // enabling clip plane again
			glDisable(GL_LIGHTING);

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();

			glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, capTopMask.data);
			cv::flip(capTopMask, capTopMask, 0);


			glDisable(MY_CLIP_PLANE5);
			glDisable(GL_CULL_FACE);

			cv::cvtColor(capTopMask, capTopMask, CV_BGR2GRAY);

			if (savedebugImage){
				QImage capTopMaskQ = cvMatToQImage(capTopMask);
				QString capTopMaskQst = "capTopMask";
				capTopMaskQst.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				capTopMaskQ.save(dir.absolutePath() + "/" + capTopMaskQst, "png");
			}
			/*=============================================================================================*/

			/*===========================================================================================
			***create mask to determine 0.1mm top face
			==============================================================================================*/

			equ[3] = box_bottom + unit*i + top_Thick_sure;

			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -1000, 1000);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, -DSP_Look_Distination, 0, 1, 0);
			//*********************************
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);

			//************************************************//
			//****** Rendering the mesh's clip edge ****//
			GLuint MY_CLIP_PLANE6 = GL_CLIP_PLANE0;
			glEnable(MY_CLIP_PLANE6);
			glClipPlane(MY_CLIP_PLANE6, equ);

			glEnable(GL_STENCIL_TEST);
			glEnable(GL_CULL_FACE);
			glClear(GL_STENCIL_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

			//// first pass: increment stencil buffer value on back faces
			glStencilFunc(GL_ALWAYS, 0, 0);
			glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
			glCullFace(GL_FRONT); // render back faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			//glRotatef(90, 1, 0, 0);
			foreach(MeshModel * mp, md.meshList)
			{
				if (mp->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mp->cm.bbox.min.Z()) && (currnentHeight <= mp->cm.bbox.max.Z()))
					//mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
					mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
			}

			glPopMatrix();
			glPopAttrib();
			//second pass: decrement stencil buffer value on front faces
			glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
			glCullFace(GL_BACK); // render front faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			//glRotatef(90, 1, 0, 0);
			foreach(MeshModel * mp, md.meshList)
			{
				if (mp->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mp->cm.bbox.min.Z() && currnentHeight <= mp->cm.bbox.max.Z()))
					//mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
					mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
			}
			glPopMatrix();
			glPopAttrib();

			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glEnable(GL_DEPTH_TEST);
			glDisable(MY_CLIP_PLANE6);
			glStencilFunc(GL_NOTEQUAL, 0, ~0);


			glColor4f(1, 1, 1, 1);
			fillsquare();
			glDisable(GL_STENCIL_TEST);
			glEnable(MY_CLIP_PLANE6); // enabling clip plane again
			glDisable(GL_LIGHTING);

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();

			glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, capTopSureMask.data);
			cv::flip(capTopSureMask, capTopSureMask, 0);


			glDisable(MY_CLIP_PLANE6);
			glDisable(GL_CULL_FACE);

			cv::cvtColor(capTopSureMask, capTopSureMask, CV_BGR2GRAY);

			if (savedebugImage){
				QImage capTopSureMaskQ = cvMatToQImage(capTopSureMask);
				QString capTopSureMaskQst = "capTopSureMask";
				capTopSureMaskQst.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				capTopSureMaskQ.save(dir.absolutePath() + "/" + capTopSureMaskQst, "png");
			}
			/*=============================================================================================*/




			/*===========================================================================================
			***Binder down Mask  image, bottom lighter
			==============================================================================================*/
			equ[3] = box_bottom + unit*i - bottom_lighter_Thick;

			//time.restart();
			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -1000, 1000);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, -DSP_Look_Distination, 0, 1, 0);
			//*********************************
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);

			//binderstrips(md, par.getFloat("useless_print"));//***uselessbar

			//************************************************//
			//****** Rendering the mesh's clip edge ****//
			GLuint MY_CLIP_PLANE3 = GL_CLIP_PLANE0;
			glEnable(MY_CLIP_PLANE3);
			glClipPlane(MY_CLIP_PLANE3, equ);

			glEnable(GL_STENCIL_TEST);
			glEnable(GL_CULL_FACE);
			glClear(GL_STENCIL_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

			//// first pass: increment stencil buffer value on back faces
			glStencilFunc(GL_ALWAYS, 0, 0);
			glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
			glCullFace(GL_FRONT); // render back faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			//glRotatef(90, 1, 0, 0);
			foreach(MeshModel * mp, md.meshList)
			{
				if (mp->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mp->cm.bbox.min.Z()) && (currnentHeight <= mp->cm.bbox.max.Z()))
					//mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
					mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
			}

			glPopMatrix();
			glPopAttrib();
			//second pass: decrement stencil buffer value on front faces
			glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
			glCullFace(GL_BACK); // render front faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			//glRotatef(90, 1, 0, 0);
			foreach(MeshModel * mp, md.meshList)
			{
				if (mp->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mp->cm.bbox.min.Z() && currnentHeight <= mp->cm.bbox.max.Z()))
					//mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
					mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
			}
			glPopMatrix();
			glPopAttrib();

			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glEnable(GL_DEPTH_TEST);
			glDisable(MY_CLIP_PLANE3);
			glStencilFunc(GL_NOTEQUAL, 0, ~0);


			glColor4f(1, 1, 1, 1);
			fillsquare();
			glDisable(GL_STENCIL_TEST);
			glEnable(MY_CLIP_PLANE3); // enabling clip plane again
			glDisable(GL_LIGHTING);

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			//********************************************************************************************//


			//glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, cap_image.bits());//***20150724
			glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, capDownMask.data);
			cv::flip(capDownMask, capDownMask, 0);

			//QString img_file_name2 = md.p_setting.getcapImageName();
			glDisable(MY_CLIP_PLANE3);
			glDisable(GL_CULL_FACE);
			//****								
			//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T(""));
			/*if (par.getBool("generate_zx"))
			{*/
			cv::cvtColor(capDownMask, capDownMask, CV_BGR2GRAY);
			//}
			if (savedebugImage){
				QImage capDownMaskkQ = cvMatToQImage(capDownMask);
				QString capDownMaskst = "capDownMask";
				capDownMaskst.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				capDownMaskkQ.save(dir.absolutePath() + "/" + capDownMaskst, "png");
			}
			/*===========================================================================================
			***END  Binder down Mask  image
			==============================================================================================*/

			/*===========================================================================================
			***Binder down Mask 0.1mm image, bottom lighter
			==============================================================================================*/
			equ[3] = box_bottom + unit*i - bottom_Thick_sure;

			//time.restart();
			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -1000, 1000);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, -DSP_Look_Distination, 0, 1, 0);
			//*********************************
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);

			//binderstrips(md, par.getFloat("useless_print"));//***uselessbar

			//************************************************//
			//****** Rendering the mesh's clip edge ****//
			GLuint MY_CLIP_PLANE4 = GL_CLIP_PLANE0;
			glEnable(MY_CLIP_PLANE4);
			glClipPlane(MY_CLIP_PLANE4, equ);

			glEnable(GL_STENCIL_TEST);
			glEnable(GL_CULL_FACE);
			glClear(GL_STENCIL_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

			//// first pass: increment stencil buffer value on back faces
			glStencilFunc(GL_ALWAYS, 0, 0);
			glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
			glCullFace(GL_FRONT); // render back faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			//glRotatef(90, 1, 0, 0);
			foreach(MeshModel * mp, md.meshList)
			{
				if (mp->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mp->cm.bbox.min.Z()) && (currnentHeight <= mp->cm.bbox.max.Z()))
					//mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
					mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
			}

			glPopMatrix();
			glPopAttrib();
			//second pass: decrement stencil buffer value on front faces
			glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
			glCullFace(GL_BACK); // render front faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			//glRotatef(90, 1, 0, 0);
			foreach(MeshModel * mp, md.meshList)
			{
				if (mp->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mp->cm.bbox.min.Z() && currnentHeight <= mp->cm.bbox.max.Z()))
					//mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
					mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
			}
			glPopMatrix();
			glPopAttrib();

			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glEnable(GL_DEPTH_TEST);
			glDisable(MY_CLIP_PLANE4);
			glStencilFunc(GL_NOTEQUAL, 0, ~0);


			glColor4f(1, 1, 1, 1);
			fillsquare();
			glDisable(GL_STENCIL_TEST);
			glEnable(MY_CLIP_PLANE4); // enabling clip plane again
			glDisable(GL_LIGHTING);

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			//********************************************************************************************//


			//glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, cap_image.bits());//***20150724
			glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, capDownSureMask.data);
			cv::flip(capDownSureMask, capDownSureMask, 0);

			//QString img_file_name2 = md.p_setting.getcapImageName();
			glDisable(MY_CLIP_PLANE4);
			glDisable(GL_CULL_FACE);
			//****								
			//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T(""));
			/*if (par.getBool("generate_zx"))
			{*/
			cv::cvtColor(capDownSureMask, capDownSureMask, CV_BGR2GRAY);
			//}
			if (savedebugImage){
				QImage capDownSureMaskIMG = cvMatToQImage(capDownSureMask);
				QString capDownSureMaskS = "capDownSureMask";
				capDownSureMaskS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				capDownSureMaskIMG.save(dir.absolutePath() + "/" + capDownSureMaskS, "png");
			}
			/*===========================================================================================
			***END  Binder down Mask  image
			==============================================================================================*/








			/*=============================================================================================
			prePrint object
			===============================================================================================*/
			if (prePrintBool)
			{
				//time.restart();
				glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
				glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -1000, 1000);
				glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, -DSP_Look_Distination, 0, 1, 0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
				glClearColor(1, 1, 1, 1);
				//****** Rendering the mesh's clip edge ****//
				GLuint MY_CLIP_PLANE2 = GL_CLIP_PLANE0;
				glEnable(MY_CLIP_PLANE2);
				glClipPlane(MY_CLIP_PLANE2, equ);

				glEnable(GL_STENCIL_TEST);
				glEnable(GL_CULL_FACE);
				glClear(GL_STENCIL_BUFFER_BIT);
				glDisable(GL_DEPTH_TEST);
				glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

				//// first pass: increment stencil buffer value on back faces
				glStencilFunc(GL_ALWAYS, 0, 0);
				glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
				glCullFace(GL_FRONT); // render back faces only
				glPushAttrib(GL_ALL_ATTRIB_BITS);
				glEnable(GL_LIGHTING);
				glPushMatrix();

				foreach(MeshModel * mp, md.meshList)
				{
					if (mp->getMeshSort() == MeshModel::meshsort::pre_print_item && (currnentHeight >= mp->cm.bbox.min.Z() && currnentHeight <= mp->cm.bbox.max.Z()))
						mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
				}

				glPopMatrix();
				glPopAttrib();
				//second pass: decrement stencil buffer value on front faces
				glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
				glCullFace(GL_BACK); // render front faces only
				glPushAttrib(GL_ALL_ATTRIB_BITS);
				glEnable(GL_LIGHTING);
				glPushMatrix();

				//glRotatef(90, 1, 0, 0);
				foreach(MeshModel * mp, md.meshList)
				{
					if (mp->getMeshSort() == MeshModel::meshsort::pre_print_item && (currnentHeight >= mp->cm.bbox.min.Z() && currnentHeight <= mp->cm.bbox.max.Z()))
						mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
				}
				glPopMatrix();
				glPopAttrib();
				//-----------------------------
				// drawing clip planes masked by stencil buffer content
				glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
				glEnable(GL_DEPTH_TEST);
				glDisable(MY_CLIP_PLANE2);
				glStencilFunc(GL_NOTEQUAL, 0, ~0);
				// stencil test will pass only when stencil buffer value = 0; 
				// (~0 = 0x11...11)

				glColor4f(1, 1, 1, 1);
				fillsquare();
				glDisable(GL_STENCIL_TEST);
				glEnable(MY_CLIP_PLANE2); // enabling clip plane again
				glDisable(GL_LIGHTING);

				glMatrixMode(GL_MODELVIEW);
				glPopMatrix();
				glMatrixMode(GL_PROJECTION);
				glPopMatrix();

				glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, prePrintobjectC.data);
				cv::flip(prePrintobjectC, prePrintobjectC, 0);
				bitwise_not(prePrintobjectC, prePrintobjectC);
				cv::cvtColor(prePrintobjectC, prePrintobjectB, CV_BGR2GRAY);


				glDisable(MY_CLIP_PLANE2);//存完圖
				glDisable(GL_CULL_FACE);

				if (savedebugImage){
					//if (true){
					QImage ttt(cvMatToQImage(prePrintobjectB));
					QString tttt = "prePrintobjectB";
					tttt.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
					ttt.save(dir.absolutePath() + "/" + tttt, "png");

				}
			}
			/*=============================================================================================================================
			=============================================================================================================================*/

			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T(""));


			//cv::Mat reverse_binder_outline(downBlackMaskFirstCV.size(), downBlackMaskFirstCV.type(), Scalar(255, 255, 255));
			//originalOutline = SKT::outlineAndBinderMask(outlineColorCv, outlineBlackMaskCV, captemp, downColorFirstCV, downBlackMaskFirstCV, downColorSecondCV, downBlackMaskSecondCV, reverse_binder_outline).clone();								


			//***20160112***image_process加上插點pattern
			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("imagePorcess"));
			if (par.getBool("generate_zx"))
			{
				SKT::InkBalance imageProcess;

				//cv::Mat temp, temp2;

				///////cap_temp_change_add_pattern///////////////////																		
				//black_Mask = SKT::erodeImage(&black_Mask, erodeIteration);

				/*if (savedebugImage){
				QImage erode_black_Mask(cvMatToQImage(black_Mask));
				QString erode_black_MaskS = "erode_black_Mask_DM";
				erode_black_MaskS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				erode_black_Mask.save(dir.absolutePath() + "/" + erode_black_MaskS, "png");
				}*/
				/*
				perimeter  thicker
				*/
				cv::cvtColor(outlineBlackMaskCV, outlineBlackMaskCV, CV_BGR2GRAY);
				//outlineBlackMaskCV = SKT::erodeImage(&outlineBlackMaskCV, erodeIteration);


				////WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("rect_test1"));
				//if ((i % 100) > 20 && add_pattern)

				/*if (savedebugImage){
				QImage ttt(cvMatToQImage(black_Mask));
				QString black_MaskS = "black_Mask";
				black_MaskS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				ttt.save(dir.absolutePath() + "/" + black_MaskS, "png");
				}*/
				/*bool innerPatternC = true;
				bool innerPatternM = true;
				bool innerPatternY = true;
				bool innerPatternB = true;*/

				/*
				Copy captemp to colorImage
				*/
				//cv::cvtColor(captemp, originalOutline, CV_GRAY2RGB);
				//originalOutline = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
				resetImage(originalOutline, width, height);
				originalOutline = Scalar(255, 255, 255);
				captemp = Scalar(255, 255, 255);

				/*
				paste magic Square Pattern to inner
				*/
				//captemp = SKT::pastePatternToInner_and_grayscale(captemp, originalOutline, copyCaptemp, &black_Mask, innerMagicSquarePattern,innerC, innerM, innerY, innerB);




				/*
				Grayscale captemp
				*/
				//SKT::grayscaleInnerPattern(captemp, originalOutline, innerPatternC, innerPatternM, innerPatternY, innerPatternB);

				if (savedebugImage){
					QImage pastePattern(cvMatToQImage(captemp));
					QString pastePatternS = "pastePattern_DM";
					pastePatternS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
					pastePattern.save(dir.absolutePath() + "/" + pastePatternS, "png");
				}
				if (savedebugImage){
					QImage pastePattern(cvMatToQImage(originalOutline));
					QString pastePatternS = "PatternColor_DM";
					pastePatternS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
					pastePattern.save(dir.absolutePath() + "/" + pastePatternS, "png");
				}



				/*
				Grayscale outline, use channel control method,
				Mask : perimeter, updown
				*/
				switch (dmSlicingMode)
				{
				case 0:
				{
					SKT::pastePatternToShell_and_grayscale(captemp, originalOutline,
						copyCaptemp, outlineBlackMaskCV, capTopMask,
						capDownMask,
						horizontalMagicSquarePattern, verticalMagicSquarePattern, bottomMagicSquarePattern,
						horizontal_ThickB, horizontal_ThickC, horizontal_ThickM, horizontal_ThickY,
						vertical_ThickB, vertical_ThickC, vertical_ThickM, vertical_ThickY,
						bottom_lighterB, bottom_lighterC, bottom_lighterM, bottom_lighterY);


					if (!reversePrint)
						SKT::cutImage(captemp, originalOutline);//white_kick	


					for (int r = 0; r < captemp.rows; r++)
						for (int c = 0; c < captemp.cols; c++)
						{
							if (captemp.at<uchar>(r, c) == 0)
							{
								captemp.at<uchar>(r, c) = dmPixelNum;
							}
							if (originalOutline.at<cv::Vec3b>(r, c)[2] == 0)
							{
								originalOutline.at<cv::Vec3b>(r, c)[2] = dmPixelNum;
							}
							if (originalOutline.at<cv::Vec3b>(r, c)[1] == 0)
							{
								originalOutline.at<cv::Vec3b>(r, c)[1] = dmPixelNum;
							}
							if (originalOutline.at<cv::Vec3b>(r, c)[0] == 0)
							{
								originalOutline.at<cv::Vec3b>(r, c)[0] = dmPixelNum;
							}
						}

				}break;
				case 1:
				{
					SKT::pastePatternToShell_and_grayscale_diffColor(captemp, originalOutline,
						copyCaptemp, outlineBlackMaskCV, capTopMask,
						capDownMask, capDownSureMask,
						horizontalMagicSquarePattern, verticalMagicSquarePattern, bottomMagicSquarePattern,
						horizontal_Pixel_Value, vertical_Pixel_Value, bottom_Pixel_Value, innerPixelValue,
						horizontal_ThickB, horizontal_ThickC, horizontal_ThickM, horizontal_ThickY,
						vertical_ThickB, vertical_ThickC, vertical_ThickM, vertical_ThickY,
						bottom_lighterB, bottom_lighterC, bottom_lighterM, bottom_lighterY);
				}break;
				case 2:
				{
					SKT::pastePatternToShell_and_grayscale_four_diff_Patterns(captemp, originalOutline,
						copyCaptemp, outlineBlackMaskCV, capTopMask, capTopSureMask,
						capDownMask, capDownSureMask,
						horizontalMagicSquarePattern, verticalMagicSquarePattern, bottomMagicSquarePattern,
						//horizonPixelValue, verticalPixelValue, bottomPixelValue, insidePixelValue
						//127, Vec3b(255, 255, 127), 127, Vec3b(255, 255, 127),
						//bgrk
						cv::Vec4b(horizon_B_Value, horizon_G_Value, horizon_R_Value, horizon_K_Value),
						cv::Vec4b(top_B_Value, top_G_Value, top_R_Value, top_K_Value),
						cv::Vec4b(bottom_B_Value, bottom_G_Value, bottom_R_Value, bottom_K_Value),
						cv::Vec4b(inner_B_Value, inner_G_Value, inner_R_Value, inner_K_Value),
						horizontal_ThickB, horizontal_ThickC, horizontal_ThickM, horizontal_ThickY,
						vertical_ThickB, vertical_ThickC, vertical_ThickM, vertical_ThickY,
						bottom_lighterB, bottom_lighterC, bottom_lighterM, bottom_lighterY);

				}break;
				}

#if 0
				SKT::pastePatternToShell_and_grayscale_diffColor(captemp, originalOutline,
					copyCaptemp, outlineBlackMaskCV, up_downBlackMaskCV,
					capDownMask, capDownSureMask,
					horizontalMagicSquarePattern, verticalMagicSquarePattern, bottomMagicSquarePattern,
					horizontal_ThickB, horizontal_ThickC, horizontal_ThickM, horizontal_ThickY,
					vertical_ThickB, vertical_ThickC, vertical_ThickM, vertical_ThickY,
					bottom_lighterB, bottom_lighterC, bottom_lighterM, bottom_lighterY);

				//if (!reversePrint)
				//	SKT::cutImage(captemp, originalOutline);//white_kick	
#endif
#if 0
				SKT::pastePatternToShell_and_grayscale(captemp, originalOutline,
					copyCaptemp, outlineBlackMaskCV, up_downBlackMaskCV,
					capDownMask,
					horizontalMagicSquarePattern, verticalMagicSquarePattern, bottomMagicSquarePattern,
					horizontal_ThickB, horizontal_ThickC, horizontal_ThickM, horizontal_ThickY,
					vertical_ThickB, vertical_ThickC, vertical_ThickM, vertical_ThickY,
					bottom_lighterB, bottom_lighterC, bottom_lighterM, bottom_lighterY);


				/*if (prePrintBool)
				{
				SKT::mergePrePrinting(originalOutline, prePrintobjectB);
				SKT::mergePrePrinting(captemp, prePrintobjectB);
				}*/

				if (!reversePrint)
					SKT::cutImage(captemp, originalOutline);//white_kick	


				for (int r = 0; r < captemp.rows; r++)
					for (int c = 0; c < captemp.cols; c++)
					{
						if (captemp.at<uchar>(r, c) == 0)
						{
							captemp.at<uchar>(r, c) = dmPixelNum;
						}
						if (originalOutline.at<cv::Vec3b>(r, c)[2] == 0)
						{
							originalOutline.at<cv::Vec3b>(r, c)[2] = dmPixelNum;
						}
						if (originalOutline.at<cv::Vec3b>(r, c)[1] == 0)
						{
							originalOutline.at<cv::Vec3b>(r, c)[1] = dmPixelNum;
						}
						if (originalOutline.at<cv::Vec3b>(r, c)[0] == 0)
						{
							originalOutline.at<cv::Vec3b>(r, c)[0] = dmPixelNum;
						}
					}

#endif				



				//switch (dmSlicingMode)
				//{
				//case 0:
				//case 1:
				//{
				//	originalOutline = SKT::resizeIamgecols(&originalOutline, plus_pixel, gen_pixelFormat);//ADD LENGTH ON COLOR IMAGE	
				//	SKT::newUselessbar_custom(originalOutline, useless_print, gen_pixelFormat, spittoon_M, spittoon_C, spittoon_Y, spittoon_B, print_dpi);

				//	captemp = SKT::resizeIamgecols(&captemp, plus_pixel, gen_pixelFormat);
				//	SKT::newUselessbar_custom(captemp, useless_print, gen_pixelFormat, spittoon_M, spittoon_C, spittoon_Y, spittoon_B, print_dpi);

				//}break;
				//case 2:
				//{
				//	originalOutline = SKT::resizeIamgecols(&originalOutline, plus_pixel, gen_pixelFormat);//ADD LENGTH ON COLOR IMAGE									
				//	SKT::newUselessbar_custom_DM(originalOutline, useless_print, gen_pixelFormat, spittoon_M, spittoon_C, spittoon_Y, spittoon_B, print_dpi);

				//	captemp = SKT::resizeIamgecols(&captemp, plus_pixel, gen_pixelFormat);
				//	SKT::newUselessbar_custom_DM(captemp, useless_print, gen_pixelFormat, spittoon_M, spittoon_C, spittoon_Y, spittoon_B, print_dpi);

				//}
				//break;
				//}

				switch (dmSPitMode)
				{
				case 0:
				{
					originalOutline = SKT::resizeIamgecols(&originalOutline, plus_pixel, gen_pixelFormat);//ADD LENGTH ON COLOR IMAGE									
					SKT::newUselessbar_custom_DM(originalOutline, useless_print, gen_pixelFormat, spittoon_M, spittoon_C, spittoon_Y, spittoon_B, print_dpi, spitton_start_position);

					captemp = SKT::resizeIamgecols(&captemp, plus_pixel, gen_pixelFormat);
					SKT::newUselessbar_custom_DM(captemp, useless_print, gen_pixelFormat, spittoon_M, spittoon_C, spittoon_Y, spittoon_B, print_dpi, spitton_start_position);

				}break;
				case 1:
				{
					originalOutline = SKT::resizeIamgecols(&originalOutline, plus_pixel, gen_pixelFormat);//ADD LENGTH ON COLOR IMAGE	
					SKT::newUselessbar_custom(originalOutline, useless_print, gen_pixelFormat, spittoon_M, spittoon_C, spittoon_Y, spittoon_B, print_dpi, spitton_start_position);

					captemp = SKT::resizeIamgecols(&captemp, plus_pixel, gen_pixelFormat);
					SKT::newUselessbar_custom(captemp, useless_print, gen_pixelFormat, spittoon_M, spittoon_C, spittoon_Y, spittoon_B, print_dpi, spitton_start_position);
				}break;

				}




				//originalOutline = SKT::resizeIamgecols(&originalOutline, plus_pixel, gen_pixelFormat);//ADD LENGTH ON COLOR IMAGE									
				////SKT::newUselessbar_custom_DM(originalOutline, useless_print, gen_pixelFormat, spittoon_M, spittoon_C, spittoon_Y, spittoon_B, print_dpi);
				//SKT::newUselessbar_custom(originalOutline, useless_print, gen_pixelFormat, spittoon_M, spittoon_C, spittoon_Y, spittoon_B, print_dpi);

				//captemp = SKT::resizeIamgecols(&captemp, plus_pixel, gen_pixelFormat);
				////SKT::newUselessbar_custom_DM(captemp, useless_print, gen_pixelFormat, spittoon_M, spittoon_C, spittoon_Y, spittoon_B, print_dpi);
				//SKT::newUselessbar_custom(captemp, useless_print, gen_pixelFormat, spittoon_M, spittoon_C, spittoon_Y, spittoon_B, print_dpi);

				/*if (dilateBinder)
				captemp = SKT::dilateImage(&captemp, dilateBinderValue);*/

				//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T(""));
				//finalcolor = *new cv::Mat(originalOutline.cols, originalOutline.rows, originalOutline.type(), Scalar(255, 255, 255));//finalcolor.setTo(cv::Scalar(255, 255, 255));				
				//cv::resize(finalcolor, finalcolor, Size(originalOutline.rows, originalOutline.cols));
				resetImage(finalcolor, originalOutline.rows, originalOutline.cols);
				SKT::rotateImage(originalOutline, finalcolor);
				//finalBinder = *new cv::Mat(captemp.cols, captemp.rows, captemp.type(), Scalar(255));//finalBinder.setTo(cv::Scalar(255));				
				cv::resize(finalBinder, finalBinder, Size(captemp.rows, captemp.cols));
				if (finalBinder.type() == CV_8UC3)
					cvtColor(finalBinder, finalBinder, CV_BGR2GRAY);
				//cv::resize(finalBinder, finalBinder, Size(captemp.rows, captemp.cols));
				SKT::rotateImage(captemp, finalBinder);


				//if (zxCheckMode == ZxGen_SKT::zxInitMode::ViewCheckMode && !doubleprint)
				//	finalBinder.setTo(cv::Scalar(255));//SET BINDER TO WHITE


				/*backbinder = *new cv::Mat(finalBinder.size(), finalBinder.type(), cv::Scalar(255));
				mirrorColor = *new cv::Mat(cv::Size(finalcolor.cols, finalcolor.rows), finalcolor.type(), cv::Scalar(0, 0, 0));
				mirrorBinder = *new cv::Mat(cv::Size(finalBinder.cols, finalBinder.rows), finalBinder.type(), cv::Scalar(255));*/




				//if (doubleprint)
				//{
				//	cv::Mat finalBackBinder(captemp.cols, captemp.rows, captemp.type(), Scalar(255));//finalBinder.setTo(cv::Scalar(255));
				//	reverse_binder_outline = SKT::resizeIamgecols(&reverse_binder_outline, plus_pixel, gen_pixelFormat);
				//	//SKT::newUselessbar(captemp, useless_print, gen_pixelFormat);//20160715_backup										
				//	SKT::newUselessbar_custom(reverse_binder_outline, useless_print, gen_pixelFormat, spittoon_M, spittoon_C, spittoon_Y, spittoon_B, print_dpi);
				//	SKT::rotateImage(reverse_binder_outline, finalBackBinder);
				//	SKT::mirrorImage(finalBackBinder, backbinder);
				//	cv::Mat erodeBinder = SKT::dilateImage(&backbinder, 1);
				//	outlineCTemp.push_back(stiffColor2);
				//	capCTemp.push_back(erodeBinder);
				//	if (savedebugImage){
				//		QImage cap_Test = cvMatToQImage(finalBackBinder);
				//		img_file_name2.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				//		cap_Test.save(dir.absolutePath() + "/" + img_file_name2, "png");
				//	}										
				//}
				//if (doublePrint_V2)
				//{
				//	SKT::newUselessbar_custom(stiffColor, useless_print, gen_pixelFormat, spittoon_M, spittoon_C, spittoon_Y, spittoon_B, print_dpi);
				//	SKT::rotateImage(stiffColor, stiffColor2);
				//	finalBinder.copyTo(backbinder);
				//	cv::Mat erodeBlackBinder = SKT::dilateImage(&backbinder, 1);
				//	/*=======================================
				//	reduce binder in the first 20 layers
				//	==========================================*/
				//	if (i < 20)
				//		SKT::BinderPercent(erodeBlackBinder, doublePrint_begin_twenty_layer_V2Percent);
				//	else
				//		SKT::BinderPercent(erodeBlackBinder, doublePrint_V2Percent);
				//	/*===============================================
				//	remain x percent binder in the second printing
				//	================================================*/
				//	SKT::BinderPercent(reverse_binder_outline, doublePrint_shell_Percent);
				//	reverse_binder_outline = SKT::resizeIamgecols(&reverse_binder_outline, plus_pixel, gen_pixelFormat);
				//	reverse_binder_outline = reverse_binder_outline(cv::Rect(0, 0, backbinder.rows, finalBinder.cols));
				//	cv::Mat secondOutlineBinder(reverse_binder_outline.cols, reverse_binder_outline.rows, reverse_binder_outline.type(), Scalar(255));//finalBinder.setTo(cv::Scalar(255));
				//	SKT::newUselessbar_custom(reverse_binder_outline, useless_print, gen_pixelFormat, spittoon_M, spittoon_C, spittoon_Y, spittoon_B, print_dpi);
				//	SKT::rotateImage(reverse_binder_outline, secondOutlineBinder);
				//	//combine secondOutlineBinder and erodeBlackBinder to secondOutlineBinder
				//	SKT::combineBinderImage(secondOutlineBinder, erodeBlackBinder, i);
				//	if (savedebugImage){
				//		QImage cap_Test = cvMatToQImage(erodeBlackBinder);
				//		QString tttt3 = "erodeBlackBinder";
				//		tttt3.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				//		cap_Test.save(dir.absolutePath() + "/" + tttt3, "png");
				//	}
				//	if (savedebugImage){
				//		QImage cap_Test = cvMatToQImage(secondOutlineBinder);
				//		QString tttt3 = "secondOutlineBinder";
				//		tttt3.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				//		cap_Test.save(dir.absolutePath() + "/" + tttt3, "png");
				//	}
				//	/*outlineCTemp.push_back(mirrorColor);
				//	capCTemp.push_back(erodeBlackBinder);*/
				//	outlineCTemp.push_back(stiffColor2);
				//	//capCTemp.push_back(erodeBlackBinder);
				//	capCTemp.push_back(secondOutlineBinder);
				//	int count_outline_Temp = 0;
				//	foreach(cv::Mat img, outlineCTemp)
				//	{
				//	QImage cap_Test = cvMatToQImage(img);
				//	QString tttt3 = "outline";
				//	tttt3.append(QString("_%1_%2.png").arg(i, 4, 10, QChar('0')).arg(count_outline_Temp, 4, 10, QChar('0')));
				//	cap_Test.save(dir.absolutePath() + "/" + tttt3, "png");
				//	count_outline_Temp++;
				//	}
				//	int count_cap_Temp = 0;
				//	foreach(cv::Mat img, capCTemp)
				//	{
				//		QImage cap_Test = cvMatToQImage(img);
				//		QString tttt3 = "captemp";
				//		tttt3.append(QString("_%1_%2.png").arg(i, 4, 10, QChar('0')).arg(count_cap_Temp, 4, 10, QChar('0')));
				//		cap_Test.save(dir.absolutePath() + "/" + tttt3, "png");
				//		count_cap_Temp++;
				//	}
				//}

				/*SKT::mirrorImage(finalcolor, mirrorColor);
				SKT::mirrorImage(finalBinder, mirrorBinder);*/
				bool printTwice = par.getBool("DM_TWICE_IN_ONE_LAYER");
				/*
				ink out any Test
				*/
				//cv::Mat tempwhiteColor(finalcolor.size(), finalcolor.type(),Scalar(255, 255, 255));
				//cv::Mat tempwhiteBinder(finalBinder.size(), finalBinder.type(), Scalar(255));
				//for (int r = 0; r < finalBinder.rows; r++)
				//	for (int c = 0; c < finalBinder.cols; c++)
				//	{						
				//		/*finalcolor.at<cv::Vec3b>(r, c).val[0] = 255;
				//		finalcolor.at<cv::Vec3b>(r, c).val[1] = 255;*/
				//		if (finalBinder.at<uchar>(r, c) == 0)
				//			finalBinder.at<uchar>(r, c) = 128;
				//	}
				//if (!printTwice)
				//{
				//	outlineCTemp.push_back(finalcolor);					
				//	capCTemp.push_back(finalBinder);
				//}
				//else
				//{
				//	outlineCTemp.push_back(finalcolor);
				//	outlineCTemp.push_back(finalcolor);
				//	capCTemp.push_back(finalBinder);
				//	capCTemp.push_back(finalBinder);
				//}
				/*
				==========================================
				*/
				if (!reversePrint)
				{
					if (!printTwice)
					{
						outlineCTemp.push_back(finalcolor);
						capCTemp.push_back(finalBinder);

					}
					else
					{
						outlineCTemp.push_back(finalcolor);
						outlineCTemp.push_back(finalcolor);
						capCTemp.push_back(finalBinder);
						capCTemp.push_back(finalBinder);
					}
				}
				else
				{
					if (!printTwice)
					{
						outlineCTemp.push_back(finalcolor);
						outlineCTemp.push_back(backColor);
						capCTemp.push_back(finalBinder);
						capCTemp.push_back(backBinder);

						/*outlineCTemp.push_back(testforReverseColorFirst);
						outlineCTemp.push_back(backColor);
						capCTemp.push_back(testforReverseBinderFirst);
						capCTemp.push_back(backBinder);*/

					}
					else
					{
						outlineCTemp.push_back(finalcolor);
						outlineCTemp.push_back(backColor);
						outlineCTemp.push_back(finalcolor);
						outlineCTemp.push_back(backColor);
						capCTemp.push_back(finalBinder);
						capCTemp.push_back(backBinder);
						capCTemp.push_back(finalBinder);
						capCTemp.push_back(backBinder);
					}

				}
				if (savefinalImage){
					int count_outline_Temp = 0;
					foreach(cv::Mat img, outlineCTemp)
					{

						QImage cap_Test = cvMatToQImage(img);
						QString tttt3 = "outline";
						tttt3.append(QString("_%1_%2.png").arg(i, 4, 10, QChar('0')).arg(count_outline_Temp, 4, 10, QChar('0')));
						cap_Test.save(dir.absolutePath() + "/" + tttt3, "png");
						count_outline_Temp++;
					}

					int count_cap_Temp = 0;
					foreach(cv::Mat img, capCTemp)
					{

						QImage cap_Test = cvMatToQImage(img);
						QString tttt3 = "captemp";
						tttt3.append(QString("_%1_%2.png").arg(i, 4, 10, QChar('0')).arg(count_cap_Temp, 4, 10, QChar('0')));
						cap_Test.save(dir.absolutePath() + "/" + tttt3, "png");
						count_cap_Temp++;
					}
				}

				//switch (printDirectionMode)
				//{
				//case 0:// positive
				//{
				//  outlineCTemp.push_back(finalcolor);
				//  capCTemp.push_back(finalBinder);
				//}
				// break;
				//case 1:// negative
				//{
				//  outlineCTemp.push_back(finalcolor.setTo(cv::Scalar(255, 255, 255)));
				//  outlineCTemp.push_back(mirrorColor);
				//  capCTemp.push_back(finalBinder.setTo(cv::Scalar(255)));
				//  capCTemp.push_back(mirrorBinder);
				//}
				// break;
				//case 2:// positive + negative
				//{
				//  outlineCTemp.push_back(finalcolor);
				//  outlineCTemp.push_back(mirrorColor);
				//  capCTemp.push_back(finalBinder);
				//  capCTemp.push_back(mirrorBinder);
				//}

				// break;

				//}

				double binderRatio = SKT::imageComplexity(finalBinder);


			}





			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("generate_final_picture"));


			//if (doubleprint && savedebugImage){
			//	QString backimgname = "backImage";
			//	//QImage backbinderqq = cvMatToQImage(outlineCTemp.back());
			//	QImage backbinderqq = cvMatToQImage(capCTemp.back());
			//	backimgname.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
			//	backbinderqq.save(dir.absolutePath() + "/" + backimgname, "png");
			//}

			/* int count_outline_Temp = 0;
			foreach(cv::Mat img, outlineCTemp)
			{
			QImage cap_Test = cvMatToQImage(img);
			QString tttt3 = "outline";
			tttt3.append(QString("_%1_%2.png").arg(i, 4, 10, QChar('0')).arg(count_outline_Temp, 4, 10, QChar('0')));
			cap_Test.save(dir.absolutePath() + "/" + tttt3, "png");
			count_outline_Temp++;
			}
			int count_cap_Temp = 0;
			foreach(cv::Mat img, capCTemp)
			{
			QImage cap_Test = cvMatToQImage(img);
			QString tttt3 = "captemp";
			tttt3.append(QString("_%1_%2.png").arg(i, 4, 10, QChar('0')).arg(count_cap_Temp, 4, 10, QChar('0')));
			cap_Test.save(dir.absolutePath() + "/" + tttt3, "png");
			count_cap_Temp++;
			}*/





			if (par.getBool("generate_zx") && j % send_page_num == 0)//幾頁傳到已產生的zxfile一次
			{
				bool sendResult = zx_skt->sendData2(outlineCTemp, capCTemp);
				wchar_t *vOut = sendResult ? L"true" : L"false";
				WRITELOG(logger, framework::Diagnostics::LogLevel::Info, (vOut));

				capCTemp.clear();
				outlineCTemp.clear();


				if (j == boxy_dim)
				{
					zx_skt->close();
					zx_skt->~ZxGen_SKT();
					Log("end ZX %i", j);
				}
				else
					if (i == start_page + testPrintPageCount - 1 && testswitch)
					{
						zx_skt->close();
						zx_skt->~ZxGen_SKT();
					}

				//fprintf(dbgff, "zx_gen %i, %i\n", time.elapsed(), i);
				//fflush(dbgff);

			}
			else if (par.getBool("generate_zx") && j % send_page_num != 0 && j == boxy_dim)//
			{

				zx_skt->sendData2(outlineCTemp, capCTemp);
				capCTemp.clear();
				outlineCTemp.clear();
				//time.elapsed();
				zx_skt->close();
				zx_skt->~ZxGen_SKT();
				Log("2end ZX  %i", j);

				//fprintf(dbgff, "zx_gen %i, %i\n", time.elapsed(), i);
				//fflush(dbgff);
			}
			QString tempt = makeOutputname(zx_dir.absolutePath(), j);
			if (j % zx_job_pages == 0)//一個job，產生下一個job
			{
				md.jobname.push_back(tempt);
				zx_skt->close();
				zx_skt->~ZxGen_SKT();
				//zx_skt = new ZxGen_SKT(unsigned int(width), unsigned int(((height*meCorrectPercent) + plus_pixel)), zx_job_pages, md.groove.DimX(), md.groove.DimY() + PLUS_MM, tempt.toStdString(), start_print_b);//20161219_backup
				zx_skt = new ZxGen_SKT(300, zx_job_pages, zx_sn.toStdString(), start_print_b);//20161219_backup
				//zggg = new ZxGen(unsigned int(width), unsigned int(4713), zx_job_pages, md.groove.DimX(), md.groove.DimY() + PLUS_MM, tempt.toStdString(), start_print_b);//20161219_backup

			}

			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("senddata end"));
			char* numpages = new char[20];
			string temp = std::to_string(i).append("_pages");
			strcpy(numpages, temp.c_str());
			wchar_t *wmsg = new wchar_t[strlen(numpages) + 1]; //memory allocation
			mbstowcs(wmsg, numpages, strlen(numpages) + 1);
			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, wmsg);
			delete[]wmsg;
			delete[]numpages;


			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();



			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("generate_final_picture"));
		}
		WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("for loop end"));
		//thread1.exit();


#endif	

		ctx.unbindReadDrawFramebuffer();
		ctx.release();
		glPopAttrib();
		glContext->doneCurrent();

		//image.rgbSwapped().mirrored().save(par.getSaveFileName("ImageFileName"));
		//image.mirrored().save(par.getSaveFileName("ImageFileName"));
		//cap_image.mirrored().save(par.getSaveFileName("CapImageFileName"));
		//image.save(par.getSaveFileName("ImageFileName"));


		foreach(MeshModel *bm, md.meshList)
		{
			if ((bm->label().contains(olMName, Qt::CaseSensitive) || bm->label().contains(cMName, Qt::CaseSensitive)))
				//if (bm->label().contains(olMName, Qt::CaseSensitive) || bm->label().contains(cMName, Qt::CaseSensitive)))
				//if (bm->getMeshSort()==MeshModel::meshsort::)
			{
				md.delMesh(bm);
			}
		}

		/*if (par.getBool("generate_zx")){

		QString ppath = "cd /d " + getRoamingDir();
		QStringList arguments;
		arguments << "/k" << ppath << "&&" << "usbsend.exe" << "output.zx" << "&&" << "exit";

		}*/





		qDebug() << " done _applyClick ";
		break;
	}

#pragma endregion FP_PRINT_DM_SLICER_ALPHA

#pragma region ESTIMATE_COLOR2
	case FP_ESTIMATE_COLOR2:
	{
		genDrawList(md);
		QObject::connect(this, SIGNAL(test_update_pages(int)), &thread1, SLOT(page_count(int)));

		md.jobname.clear();
		//***************************


		/*glContext->doneCurrent();
		glContext->moveToThread(th1);*/

		glContext->makeCurrent();


		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			/* Problem: glewInit failed, something is seriously wrong. */
			//fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
			//qDebug("Error: %s\n", glewGetErrorString(err));
		}

		glPushAttrib(GL_ALL_ATTRIB_BITS);

		Context ctx;
		ctx.acquire();

		if (switchInitMeshVBO)
			initMeshVBO(md);
		/*const GLsizeiptr psize = GLsizeiptr(GLsizei(mesh.vn) * pstride);
		BufferHandle hPositionBuffer = createBuffer(ctx, psize, pbase);

		const GLsizeiptr nsize = GLsizeiptr(GLsizei(mesh.vn) * nstride);
		BufferHandle hNormalBuffer = createBuffer(ctx, nsize, nbase);

		const GLsizeiptr isize = GLsizeiptr(mesh.fn * 3 * sizeof(GLuint));
		BufferHandle hIndexBuffer = createBuffer(ctx, isize);
		{
		BoundIndexBufferHandle indexBuffer = ctx.bindIndexBuffer(hIndexBuffer);

		const CMeshO::VertexType * vbase = &(mesh.vert[0]);
		GLuint *  indices = (GLuint *)indexBuffer->map(GL_WRITE_ONLY);
		for (size_t i = 0; i < mesh.face.size(); ++i)
		{
		const CMeshO::FaceType & f = mesh.face[i];
		if (f.IsD()) continue;
		for (int v = 0; v < 3; ++v)
		{
		*indices++ = GLuint(vcg::tri::Index(mesh, f.cV(v)));
		}
		}
		indexBuffer->unmap();

		ctx.unbindIndexBuffer();
		}*/

		//***20150907
		const GLsizei print_dpi = 40;// GLsizei(par.getInt("print_dpi"));
		float x = md.groove.DimX();
		float y = md.groove.DimY();
		const GLsizei width = (int)(x / DSP_inchmm * print_dpi);//827
		const GLsizei height = (int)(width * y / x);//520

		//qDebug() << "width" << "height" << width << height;
		//RenderbufferHandle hDepth = createRenderbuffer(ctx, GL_DEPTH_COMPONENT24, width, height);
		RenderbufferHandle hDepth = createRenderbuffer(ctx, GL_DEPTH24_STENCIL8, width, height);
		Texture2DHandle    hColor = createTexture2D(ctx, GL_RGB8, width, height, GL_RGB, GL_UNSIGNED_BYTE);
		//FramebufferHandle  hFramebuffer = createFramebuffer(ctx, renderbufferTarget(hDepth), texture2DTarget(hColor));
		FramebufferHandle  hFramebuffer = createFramebufferWithDepthStencil(ctx, renderbufferTarget(hDepth), renderbufferTarget(hDepth), texture2DTarget(hColor));




		const vcg::Point3f lightDirectionVS = vcg::Point3f(0.0f, 0.0f, -1.0f).Normalize();

		glEnable(GL_DEPTH_TEST);
		//glClearColor(GLfloat(backgroundColor.red())/255.0f, GLfloat(backgroundColor.green())/255.0f, GLfloat(backgroundColor.blue())/255.0f, 0.0f);
		Construction_Groove<float> tempgroove = md.groove;

		glViewport(0, 0, width, height);//***window的長寬				  


		//glScalef(scale, scale, scale);
		//glTranslatef(-center[0], -center[1], -center[2]);

		//***儲存image的基本資訊
		QImage image(int(width), int(height), QImage::Format_RGB888);
		QImage image2(int(width), int(height), QImage::Format_RGB888);
		QImage slice0Point05mmImage(int(width), int(height), QImage::Format_RGB888);
		QImage cap_image(int(width), int(height), QImage::Format_RGB888);

		//***20150505***slice_func_設置高度
		float box_bottom = md.bbox().min.Z();
#if 1								
		float unit = 1;// par.getFloat("slice_height");
		int xyImageThick = 10;//***0.1mm * 10 layer
#else
		float unit = 0.01;
		int xyImageThick = 1;
#endif

		bool startprintestimate = par.getBool("Start_Printing_Estimate");
		//int boxy_dim = qCeil((md.bbox().max.Z() - md.bbox().min.Z()) / unit) + 1;
		//int boxy_dim = qCeil((md.bbox().max.Z() - md.bbox().min.Z()) / unit);//for one page
		//int(mdd->bbox().DimZ() / pJI.layerThickness.value) + 1;
		int boxy_dim;
		if (startprintestimate)
			boxy_dim = qCeil((md.bbox().max.Z() - md.bbox().min.Z()) / unit);//for one page
		else
			boxy_dim = qCeil((md.selBBox().max.Z() - md.selBBox().min.Z()) / unit);//for one page

		Log("boxy_dim %i", boxy_dim);

		init_texture(md);

		//*******************					  

		//***20150507 standard path******************
		//QString temppath = QStandardPaths::locate(QStandardPaths::TempLocation, "", QStandardPaths::LocateDirectory);
		//QString temppath = "D:/temptemp";//backup
		QString temppath = PicaApplication::getRoamingDir();
		QDir dir(temppath);//backup
		if (dir.exists("xyzimage"))
		{
			dir.setPath(temppath + "/xyzimage");
			dir.removeRecursively();
			dir.setPath(temppath);
			dir.mkpath("xyzimage");
		}
		else
		{
			dir.setPath(temppath);
			dir.mkpath("xyzimage");
		}

		dir.setPath(temppath + "/xyzimage");

		//---zx_file------------------------------------------
		//QString zxtemp = "D:/temptemp";
		QString zxtemp = PicaApplication::getRoamingDir();
		QDir zx_dir(zxtemp);
		if (zx_dir.exists("zx_file"))
		{
			zx_dir.setPath(zxtemp + "/zx_file");
			//zx_dir.removeRecursively();
			zx_dir.setPath(zxtemp);
			zx_dir.mkpath("zx_file");
		}
		else
		{
			dir.setPath(temppath);
			dir.mkpath("zx_file");
		}

		zx_dir.setPath(zxtemp + "/zx_file");

		//==============================
		//**********************
		ctx.bindReadDrawFramebuffer(hFramebuffer);
		GLW_CHECK_GL_READ_DRAW_FRAMEBUFFER_STATUS;
#if 1

		const bool outputOL_Image = false;// par.getBool("OL_Image");
		const bool save_cap_Image = false;// par.getBool("Cap_Image");

		const bool faceColor = par.getBool("FaceColor");
		glClearColor(1, 1, 1, 1);
		QString olMName = md.p_setting.getOlMeshName();
		QString cMName = md.p_setting.getCapMeshName();

		//***opencvMat clear
		capCTemp.clear();
		outlineCTemp.clear();


		int zx_job_pages = par.getInt("per_zx_job");
		int start_page = 1;// par.getInt("start_page");
		int send_page_num = par.getInt("send_page_num");
		int plus_pixel = par.getInt("plus_pixel");
		float PLUS_MM = par.getFloat("PLUS_MM");
		bool start_print_b = par.getBool("PRINT_START");
		float useless_print = par.getFloat("USELESS_PRINT");
		bool line_or_quads = par.getBool("LINE_OR_RECT");;//true line, false quads

		//int spittonmode = par.getInt("SPITTON_MODE");


		bool add_pattern = false;// par.getBool("ADD_PATTERN");
		//float meCorrectPercent = 0.97;
		float meCorrectPercent = 1;
		bool savedebugImage = false;


		int widthrecord, heightrecord;
		QString blk_img_file_name = "black_outline";
		QString zx_sn = makeOutputname(zx_dir.absolutePath(), 0);

		md.jobname.push_back(zx_sn);

		QTime time;

		time.start();

		//***20160622_test_progress bar initializing progress bar status
		if (cb != NULL)
		{
			glContext->doneCurrent();
			(*cb)(0, "processing...");
			glContext->makeCurrent();
		}

		QDir patternPath(PicaApplication::getRoamingDir());

		if (patternPath.exists("pattern"))
		{
			patternPath.setPath(PicaApplication::getRoamingDir() + "/pattern");
		}
		else
		{
			patternPath.setPath(PicaApplication::getRoamingDir());
			patternPath.mkpath("pattern");
			patternPath.setPath(PicaApplication::getRoamingDir() + "/pattern");
			//patternPath.setPath( "./pattern");

		}


		//***testLog
		Log("Closed %i holes and added %i new faces", 12, 12);
		//boxy_dim--;
		cv::Scalar totalMeanVal(0, 0, 0, 0);
		double meanSumValueC = 0;
		double meanSumValueM = 0;
		double meanSumValueY = 0;

		double meanSumValueCside = 0;
		double meanSumValueMside = 0;
		double meanSumValueYside = 0;

		int printHeadNozzle = 14080;//***640*11*2 = 14080
		double printHeadLength = DSP_grooveX;//***640*11*2 = 14080
		int imageWidth = 2600;//***22.2/DSP_inchmm*300 = 2622
		int grooveMaxlayer = 2000;//***20cm

		int tenPixelNozel = printHeadNozzle * 10 / imageWidth;
		double inkDropVolumn = 1.2e-9;//ml

		//double dropsize = 1200 * 1e-12;
		double midJobDropDots = 14080000;

		double finalCusage = 0;
		double finalMusage = 0;
		double finalYusage = 0;
		double finalBusage = 0;
		if (startprintestimate)
		{
			md.multiSelectID.clear();

			foreach(MeshModel *sm, md.meshList)
			{
				//if (sm->getMeshSort() == MeshModel::meshsort::print_item)
				if (!sm->label().contains(md.p_setting.getOlMeshName(), Qt::CaseInsensitive) && !sm->label().contains(md.p_setting.getCapMeshName(), Qt::CaseInsensitive))
					md.multiSelectID.insert(sm->id());
			}
		}

		cv::Mat look_down_color_CV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat look_down_Black_Mask_layer(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat look_down_Black_MaskCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat look_up_Black_MaskCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat look_up_color_CV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat captemp(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat capDownMask(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat black_Mask(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat black_Mask2(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat outlineBlackMaskCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat lookDownBlackTriangleMask(height, width, CV_8UC3, Scalar(255, 255, 255));

		cv::Mat prePrintobjectC(height, width, CV_8UC3, Scalar(255, 255, 255));

		cv::Mat downColorFirstCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat downBlackMaskFirstCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat downColorSecondCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat downBlackMaskSecondCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat prePrintobjectB(height, width, CV_8UC1, Scalar(255));

		cv::Mat upMergeDownCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat capDownMask_top_face(height, width, CV_8UC3, Scalar(255, 255, 255));


		cv::Mat color_Bottom_face(height, width, CV_8UC3, Scalar(255, 255, 255));



		cv::Mat color_Top_face(height, width, CV_8UC3, Scalar(255, 255, 255));



		cv::Mat originalOutline(height, width, CV_8UC3, Scalar(255, 255, 255));


		cv::Mat outlineColorCv(height, width, CV_8UC3, Scalar(255, 255, 255));


		cv::Mat capDownSureMask(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat capTopSureMask(height, width, CV_8UC3, Scalar(255, 255, 255));


		for (int i = start_page, j = start_page + 1; i < boxy_dim; i++, j++)
		{
			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("estimateStart"));
			if (cb && (i % 1) == 0)
			{
				glContext->doneCurrent();
				bool break_test = cb((i * 100) / boxy_dim, "slice process");
				if (!break_test)
				{
					ctx.unbindReadDrawFramebuffer();
					ctx.release();
					//glPopAttrib();
					glDeleteLists(drawList, 2);
					glContext->doneCurrent();
					return false;
					break;
				}

			}
			glContext->makeCurrent();
			//xyz::slice_roution(md, box_bottom + unit*i, faceColor);//***backup******************切層演算							  
			time.restart();

			xyz::slice_roution_selected(md, box_bottom + unit*i, faceColor);//****20160301*****************切層演算							  

			/*fprintf(dbgff, "slice_func %i, %i\n", time.elapsed(), i);
			fflush(dbgff);*/


			//time.restart();
			//***第一張outline圖*****************************
			QVector<MeshModel *> vmm;
			//md.getMeshByContainString("_temp_outlines", vmm);
			md.getMeshByContainString(olMName, vmm);
			//QVector<MeshModel *> cmm;
			//md.getMeshByContainString(cMName, cmm);//capImage
			int xx = 0;
			QString img_file_name = md.p_setting.getoutlineName();
			float cut_z = box_bottom + unit*i;



			//***start***look up color mask***
			//glMatrixMode(GL_PROJECTION); glPopMatrix();
			//glMatrixMode(GL_MODELVIEW); glPopMatrix();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);

			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -DSP_Color_thick, DSP_Color_thick);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();

			gluLookAt(0, 0, cut_z, 0, 0, DSP_Look_Distination, 0, 1, 0);//look up

			glPushAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
			glShadeModel(GL_SMOOTH);
			glEnable(GL_COLOR_MATERIAL);
			glDisable(GL_LIGHTING);

			glPushMatrix();
			if (drawLists[0] > 0)
				glCallList(drawLists[0]);
			//foreach(int id, md.multiSelectID)
			//{
			// MeshModel *mdmm = md.getMesh(id);									 
			// if (mdmm->getMeshSort() == MeshModel::meshsort::print_item)
			// if (mdmm->rmm.colorMode != GLW::CMNone || mdmm->rmm.textureMode != GLW::TMNone)//如果無色就不須做									   
			// {
			//  mdmm->render(GLW::DMSmooth, mdmm->rmm.colorMode, mdmm->rmm.textureMode);
			// }
			//}
			glPopMatrix();
			glPopAttrib();


			glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image.bits());
			image = image.mirrored(true, true);
			look_up_color_CV = QImageToCvMat(image);
			if (savedebugImage){
				QString lookUpColorMask = "lookUpColorMask";
				lookUpColorMask.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				image.save(dir.absolutePath() + "/" + lookUpColorMask, "png");
			}
			//***end***look up color mask***
			upMergeDownCV = look_up_color_CV;// SKT::upDownCombine(look_up_Black_MaskCV, look_up_color_CV, look_down_Black_MaskCV, look_down_color_CV);
			//QImage upMergeDownQT = cvMatToQImage(upMergeDownCV);
			/*QString upMergeDownQTS = "upMergeDownQTS";
			upMergeDownQTS.append(QString("abb_%1.png").arg(i, 4, 10, QChar('0')));
			upMergeDownQT.save(dir.absolutePath() + "/" + upMergeDownQTS, "png");*/



			//***start***outlineBlackMask***
			glMatrixMode(GL_PROJECTION); glPopMatrix();
			glMatrixMode(GL_MODELVIEW); glPopMatrix();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);
			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -1000, 1000);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
			gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, 0, 0, 1, 0);
			foreach(MeshModel *mp, vmm)//outline
			{
				mp->glw.viewerNum = 2;
				if (vmm.size() > 0)
				{
					if (line_or_quads)
						mp->render(vcg::GLW::DMTexWire, vcg::GLW::CMBlack, vcg::GLW::TMNone);//backup
					else
						mp->render(vcg::GLW::DMTexWireQuad, vcg::GLW::CMBlack, vcg::GLW::TMNone);//backup


					//mp->render(vcg::GLW::DMTexWire, vcg::GLW::CMBlack, vcg::GLW::TMNone);//backup
					//mp->render(vcg::GLW::DMTexWireOne, vcg::GLW::CMBlack, vcg::GLW::TMNone);//backup
				}
			}
			glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image.bits());
			image = image.mirrored(false, true);
			outlineBlackMaskCV = QImageToCvMat(image);
			if (savedebugImage){
				QString outlineBlackMask = "outlineBlackMask";
				outlineBlackMask.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				image.save(dir.absolutePath() + "/" + outlineBlackMask, "png");
			}
			//***end***outlineBlackMask***


			glMatrixMode(GL_PROJECTION); glPopMatrix();
			glMatrixMode(GL_MODELVIEW); glPopMatrix();
			glDisable(GL_LIGHTING);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);
			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -1000, 1000);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
			gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, 0, 0, 1, 0);
			//cmystrips(md, par.getFloat("useless_print") / 3.);//***useless bar

			foreach(MeshModel *mp, vmm)//畫輪廓圖。
			{
				mp->glw.viewerNum = 2;
				if (vmm.size() > 0)
				{
					//mp->render(vcg::GLW::DMTexWire, mp->rmm.colorMode, mp->rmm.textureMode);//backup
					//mp->render(vcg::GLW::DMTexWireOne, mp->rmm.colorMode, mp->rmm.textureMode);//backup

					if (line_or_quads)
						mp->render(vcg::GLW::DMTexWire, mp->rmm.colorMode, mp->rmm.textureMode);//backup
					else
						mp->render(vcg::GLW::DMTexWireQuad, mp->rmm.colorMode, mp->rmm.textureMode);//backup

				}
			}

			glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image.bits());
			image = image.mirrored(false, true);
			outlineColorCv = QImageToCvMat(image);
			if (savedebugImage){
				QString outlineColor = "outlineColor";
				outlineColor.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				image.save(dir.absolutePath() + "/" + outlineColor, "png");
			}



			//***第一張圖片
			//if (par.getBool("generate_zx"))
			//{
			//	originalOutline = QImageToCvMat(image.mirrored());
			//	//outlineCTemp.push_back(QImageToCvMat(image.mirrored())); //zxzxzxzxzxzxzxz
			//}
			//if (outputOL_Image)//彩色輪廓
			//{
			//	img_file_name.append(QString("abb_%1.png").arg(i, 4, 10, QChar('0')));
			//	image.mirrored().save(dir.absolutePath() + "/" + img_file_name, "png");
			//}


			//***20160122***產生黑色輪廓編號_mask
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);

			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -1000, 1000);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, 0, 0, 1, 0);

			foreach(MeshModel *mp, vmm)//outline
			{
				mp->glw.viewerNum = 2;
				if (vmm.size() > 0)
				{
					//mp->render(vcg::GLW::DMTexWireOne, vcg::GLW::CMBlack, vcg::GLW::TMNone);//backup
					if (line_or_quads)
						mp->render(vcg::GLW::DMTexWire, vcg::GLW::CMBlack, vcg::GLW::TMNone);//backup
					else
						mp->render(vcg::GLW::DMTexWireQuad, vcg::GLW::CMBlack, vcg::GLW::TMNone);//backup

				}
			}

			//****20151231********************************************************************								  

			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -DSP_Color_thick, DSP_Color_thick);

			//***20161112
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, cut_z, 0, 0, -DSP_Look_Distination, 0, 1, 0);

			glPushMatrix();
			if (drawLists[0] > 0)
				glCallList(drawLists[1]);
			/* if (drawLists[0] > 0)
			glCallList(drawLists[2]);*/
			/*foreach(int id, md.multiSelectID)
			{
			MeshModel *mdmm = md.getMesh(id);
			if (mdmm->getMeshSort() == MeshModel::meshsort::print_item)
			{
			mdmm->render(GLW::DMFlat, GLW::CMBlack, GLW::TMNone);
			}
			}*/
			glPopMatrix();



			//fFaceColor
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();



			glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image.bits());
			black_Mask = QImageToCvMat(image.mirrored());
			//if (par.getBool("BLK_OUT_LINE"))
			if (savedebugImage)
			{
				QString tttt2 = "black_mask_QI";
				tttt2.append(QString("blk_%1.png").arg(i, 4, 10, QChar('0')));
				image.mirrored().save(dir.absolutePath() + "/" + tttt2, "png");
			}

			/*fprintf(dbgff, "render_process %i, %i\n", time.elapsed(), i);
			fflush(dbgff);*/
			//===========================================================================================
			equ[3] = box_bottom + unit*i;
			//***第二張圖***產生膠水圖**********//
			//time.restart();
			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -1000, 1000);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, -DSP_Look_Distination, 0, 1, 0);
			//*********************************
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);

			//binderstrips(md, par.getFloat("useless_print"));//***uselessbar

			//************************************************//
			//****** Rendering the mesh's clip edge ****//
			GLuint MY_CLIP_PLANE = GL_CLIP_PLANE0;
			glEnable(MY_CLIP_PLANE);
			glClipPlane(MY_CLIP_PLANE, equ);

			glEnable(GL_STENCIL_TEST);
			glEnable(GL_CULL_FACE);
			glClear(GL_STENCIL_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

			//// first pass: increment stencil buffer value on back faces
			glStencilFunc(GL_ALWAYS, 0, 0);
			glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
			glCullFace(GL_FRONT); // render back faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			//glRotatef(90, 1, 0, 0);
			if (drawLists[0] > 0)
				glCallList(drawLists[1]);
			//foreach(int id, md.multiSelectID)
			//{
			// MeshModel *mp = md.getMesh(id);
			// if (mp->getMeshSort() == MeshModel::meshsort::print_item)
			//  //mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
			//  mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
			//}

			glPopMatrix();
			glPopAttrib();
			//second pass: decrement stencil buffer value on front faces
			glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
			glCullFace(GL_BACK); // render front faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			//glRotatef(90, 1, 0, 0);
			if (drawLists[0] > 0)
				glCallList(drawLists[1]);
			//foreach(int id, md.multiSelectID)
			//{
			// MeshModel *mp = md.getMesh(id);
			// if (mp->getMeshSort() == MeshModel::meshsort::print_item)
			//  //mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
			//  mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
			//}
			glPopMatrix();
			glPopAttrib();
			//-----------------------------
			// drawing clip planes masked by stencil buffer content
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glEnable(GL_DEPTH_TEST);
			glDisable(MY_CLIP_PLANE);
			glStencilFunc(GL_NOTEQUAL, 0, ~0);
			// stencil test will pass only when stencil buffer value = 0; 
			// (~0 = 0x11...11)

			glColor4f(1, 1, 1, 1);
			fillsquare();
			glDisable(GL_STENCIL_TEST);
			glEnable(MY_CLIP_PLANE); // enabling clip plane again
			glDisable(GL_LIGHTING);

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			//********************************************************************************************//


			glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, cap_image.bits());//***20150724
			//glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, cap_image.bits());							   
			QString img_file_name2 = md.p_setting.getcapImageName();
			glDisable(MY_CLIP_PLANE);//存完圖
			glDisable(GL_CULL_FACE);
			//****

			/*fprintf(dbgff, "cap_image_process %i, %i\n", time.elapsed(), i);
			fflush(dbgff);*/

			cv::Mat captemp;
			cv::cvtColor(QImageToCvMat(cap_image.mirrored()), captemp, CV_RGB2GRAY);


			originalOutline = SKT::outlinePoint01CMCombine(outlineColorCv, outlineBlackMaskCV, upMergeDownCV, captemp);
			QImage testttt = cvMatToQImage(originalOutline);
			if (savedebugImage){
				QString originalOutlineSs = "originalOutlineSs";
				originalOutlineSs.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				testttt.save(dir.absolutePath() + "/" + originalOutlineSs, "png");
			}


			//***20160112***image_process							
			SKT::InkBalance imageProcess;
			cv::Mat temp, temp2;
			imageProcess.operate2(originalOutline, captemp, black_Mask, false);
			if (par.getBool("mono_bool"))originalOutline = cv::Scalar(255, 255, 255);

			//originalOutline = SKT::resizeIamge(&originalOutline, plus_pixel);
			//cv::Scalar meanVal = SKT::meanCMY(originalOutline);
			totalMeanVal += SKT::meanCMY(originalOutline);
			QImage outputColorImage = cvMatToQImage(originalOutline);
			if (savedebugImage){
				QString originalOutlineS = "originalOutlineS";
				originalOutlineS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				outputColorImage.save(dir.absolutePath() + "/" + originalOutlineS, "png");
			}
			outlineCTemp.push_back(originalOutline);//**********PUSH_BACK_OUTLINE********************************

			///////cap_temp_change_add_pattern///////////////////
			black_Mask = SKT::erodeImage(&black_Mask, 51);
			capCTemp.push_back(captemp);//***********PUSH_BACK_CAPIMAGE******************************************


			if (save_cap_Image)
			{
				QImage cap_Test = cvMatToQImage(captemp);
				img_file_name2.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				cap_Test.save(dir.absolutePath() + "/" + img_file_name2, "png");
			}



			if (savedebugImage)// par.getBool(("generate_final_picture")))
			{

				////picasso/////////////////////////////////////////////////
				if (i < 50)
				{

					QImage ttt(cvMatToQImage(outlineCTemp.back()));
					//QImage ttt(cvMatToQImage(originalOutline));
					QString tttt = "tttt";
					tttt.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
					ttt.save(dir.absolutePath() + "/" + tttt, "png");
					qDebug() << "dir.absolutePath()" << dir.absolutePath();
					//ttt.save(PicaApplication::getRoamingDir() + "/xyzimage/" + tttt, "png");

					//////////////black_mask///////////////////////////////
					//QImage black_mask_QI(cvMatToQImage(black_Mask));
					////QImage ttt(cvMatToQImage(originalOutline));
					//QString tttt2 = "black_mask_QI";
					//tttt2.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
					//black_mask_QI.save(dir.absolutePath() + "/" + tttt2, "png");
					//////////captemp//////////
					QImage binder_image(cvMatToQImage(captemp));
					//QImage ttt(cvMatToQImage(originalOutline));
					QString binder_image_text = "binder_image";
					binder_image_text.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
					binder_image.save(dir.absolutePath() + "/" + binder_image_text, "png");
					///////////////////////////
				}


			}
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();

		}
		WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("estimateEnd"));

		qDebug() << "totalMeanVal" << totalMeanVal[0];//Yellow
		qDebug() << "totalMeanVal" << totalMeanVal[1];//Magenta
		qDebug() << "totalMeanVal" << totalMeanVal[2];//Cyan
		// float uselessPrintWidth = par.getFloat("USELESS_PRINT") * 10;//cm to mm
		float uselessPrintWidthC = par.getFloat("SPITTOON_C");
		float uselessPrintWidthM = par.getFloat("SPITTOON_M");
		float uselessPrintWidthY = par.getFloat("SPITTOON_Y");
		float secondCMY = par.getFloat("SPITTOON_SECOND_CMY");

		int DSP_Midjob_Frequence = par.getInt("MIDJOB_FREQUENCY");
		int DSP_Big_Midjob_Frequence_ = 128;



		//float spitton_c = par.getFloat("SPITTOON_M") ;//
		//float spitton_m = par.getFloat("SPITTOON_C") ;//
		//float spitton_y = par.getFloat("SPITTOON_Y") ;//
		//float spitton_b = par.getFloat("SPITTOON_B") ;//
		//uselessBinderUsage
		//nozzle *							  
		bool doublePrint_V2 = par.getBool("STIFF_PRIN_V2");

		double uselessPrint_UsageC = 0;
		double uselessPrint_UsageM = 0;
		double uselessPrint_UsageY = 0;
		if (doublePrint_V2)
		{
			uselessPrint_UsageC = (DSP_grooveY * 1600 / 25.4) * (uselessPrintWidthC * 1600 / 25.4)*boxy_dim * 10 * inkDropVolumn;
			uselessPrint_UsageC += (DSP_grooveY * 1600 / 25.4) * (secondCMY * 1600 / 25.4)*boxy_dim * 10 * inkDropVolumn;

			uselessPrint_UsageM = (DSP_grooveY * 1600 / 25.4) * (uselessPrintWidthM * 1600 / 25.4)*boxy_dim * 10 * inkDropVolumn;
			uselessPrint_UsageM += (DSP_grooveY * 1600 / 25.4) * (secondCMY * 1600 / 25.4)*boxy_dim * 10 * inkDropVolumn;

			uselessPrint_UsageY = (DSP_grooveY * 1600 / 25.4) * (uselessPrintWidthY * 1600 / 25.4)*boxy_dim * 10 * inkDropVolumn;
			uselessPrint_UsageY += (DSP_grooveY * 1600 / 25.4) * (secondCMY * 1600 / 25.4)*boxy_dim * 10 * inkDropVolumn;



		}
		else
		{
			uselessPrint_UsageC = (DSP_grooveY * 1600 / 25.4) * (uselessPrintWidthC * 1600 / 25.4)*boxy_dim * 10 * inkDropVolumn;;
			uselessPrint_UsageM = (DSP_grooveY * 1600 / 25.4) * (uselessPrintWidthM * 1600 / 25.4)*boxy_dim * 10 * inkDropVolumn;;
			uselessPrint_UsageY = (DSP_grooveY * 1600 / 25.4) * (uselessPrintWidthY * 1600 / 25.4)*boxy_dim * 10 * inkDropVolumn;;

		}

		//double uselessPrint_MUsage = (DSP_grooveY * 1600 / 25.4) * (spitton_m * 1600 / 25.4)*boxy_dim * 10 * inkDropVolumn;;
		//double uselessPrint_YUsage = (DSP_grooveY * 1600 / 25.4) * (spitton_y * 1600 / 25.4)*boxy_dim * 10 * inkDropVolumn;;





		jobServiceCount prejob(200, 200, 200, 200, 200);
		jobServiceCount midjob(0, 0, 0, 0, 0);
		jobServiceCount postjob(200, 200, 200, 200, 200);

		//int midJobFrequency = 4;
		int nMidJob = (boxy_dim / DSP_Midjob_Frequence) * 10;;
		int nMidJobFordouble = 0;//
		int nMidJobFordouble_fuck_slow_wipe = 0;
		if (doublePrint_V2)
		{
			nMidJobFordouble = 2 * (boxy_dim / DSP_Midjob_Frequence) * 10;//10 times larger than slice mode
			nMidJobFordouble_fuck_slow_wipe = 2 * (boxy_dim / DSP_Big_Midjob_Frequence_) * 10;
		}
		else
		{
			nMidJobFordouble = (boxy_dim / DSP_Midjob_Frequence) * 10;//10 times larger than slice mode
			nMidJobFordouble_fuck_slow_wipe = (boxy_dim * 10 / DSP_Big_Midjob_Frequence_);
		}

		fuck_maint_c.fuck_useles_print = uselessPrint_UsageC;
		fuck_maint_c.fuck_spitton = prejob.maintUsage(jobServiceCount::inktype::c) +
			midjob.maintUsage(jobServiceCount::inktype::c) * nMidJobFordouble +
			postjob.maintUsage(jobServiceCount::inktype::c);
		fuck_maint_c.fuck_wiper = nMidJobFordouble * DSP_WIPER_C + DSP_WIPER_C * 2 + nMidJobFordouble_fuck_slow_wipe * DSP_SLOW_WIPER_C;//(DSP_WIPER_C * 2) prjob and postjob
		fuck_maint_c.fuck_printUsage = (printHeadNozzle * printHeadNozzle *inkDropVolumn * xyImageThick * (totalMeanVal[2] / 256.));


		fuck_maint_m.fuck_useles_print = uselessPrint_UsageM;
		fuck_maint_m.fuck_spitton = prejob.maintUsage(jobServiceCount::inktype::m) + midjob.maintUsage(jobServiceCount::inktype::m) * nMidJobFordouble + postjob.maintUsage(jobServiceCount::inktype::m);
		fuck_maint_m.fuck_wiper = nMidJobFordouble * DSP_WIPER_M + DSP_WIPER_M * 2 + nMidJobFordouble_fuck_slow_wipe * DSP_SLOW_WIPER_M;
		fuck_maint_m.fuck_printUsage = (printHeadNozzle * printHeadNozzle *inkDropVolumn * xyImageThick * (totalMeanVal[1] / 256.));

		fuck_maint_y.fuck_useles_print = uselessPrint_UsageY;
		fuck_maint_y.fuck_spitton = prejob.maintUsage(jobServiceCount::inktype::y) + midjob.maintUsage(jobServiceCount::inktype::y) * nMidJobFordouble + postjob.maintUsage(jobServiceCount::inktype::y);
		fuck_maint_y.fuck_wiper = nMidJobFordouble * DSP_WIPER_Y + DSP_WIPER_Y * 2 + nMidJobFordouble_fuck_slow_wipe * DSP_SLOW_WIPER_Y;;
		fuck_maint_y.fuck_printUsage = (printHeadNozzle * printHeadNozzle *inkDropVolumn * xyImageThick * (totalMeanVal[0] / 256.));

		//assume layer_height is 0.1mm
		double maintC = uselessPrint_UsageC +
			prejob.maintUsage(jobServiceCount::inktype::c) + DSP_WIPER_C +
			midjob.maintUsage(jobServiceCount::inktype::c) * nMidJobFordouble + nMidJobFordouble * DSP_WIPER_C +
			+nMidJobFordouble_fuck_slow_wipe * DSP_SLOW_WIPER_C +
			postjob.maintUsage(jobServiceCount::inktype::c) + DSP_WIPER_C;
		double maintM = uselessPrint_UsageM +
			prejob.maintUsage(jobServiceCount::inktype::m) + DSP_WIPER_M +
			midjob.maintUsage(jobServiceCount::inktype::m) * nMidJobFordouble + nMidJobFordouble * DSP_WIPER_M +
			nMidJobFordouble_fuck_slow_wipe * DSP_SLOW_WIPER_M +
			postjob.maintUsage(jobServiceCount::inktype::m) + DSP_WIPER_M;
		double maintY = uselessPrint_UsageY +
			prejob.maintUsage(jobServiceCount::inktype::y) + DSP_WIPER_Y +
			midjob.maintUsage(jobServiceCount::inktype::y) * nMidJobFordouble + nMidJobFordouble * DSP_WIPER_Y +
			nMidJobFordouble_fuck_slow_wipe * DSP_SLOW_WIPER_Y +
			postjob.maintUsage(jobServiceCount::inktype::y) + DSP_WIPER_Y;

		/*double WIPERC =  DSP_WIPER_C + nMidJobFordouble * DSP_WIPER_C +nMidJobFordouble_fuck_slow_wipe * DSP_SLOW_WIPER_C +
			 + DSP_WIPER_C;
		double WIPERC = DSP_WIPER_C + nMidJobFordouble * DSP_WIPER_C + nMidJobFordouble_fuck_slow_wipe * DSP_SLOW_WIPER_C +
			+DSP_WIPER_C;
		double WIPERC = DSP_WIPER_C + nMidJobFordouble * DSP_WIPER_C + nMidJobFordouble_fuck_slow_wipe * DSP_SLOW_WIPER_C +
			+DSP_WIPER_C;*/


		finalCusage = maintC + (printHeadNozzle * printHeadNozzle *inkDropVolumn * xyImageThick * (totalMeanVal[2] / 256.));
		finalMusage = maintM + (printHeadNozzle * printHeadNozzle *inkDropVolumn * xyImageThick * (totalMeanVal[1] / 256.));
		finalYusage = maintY + (printHeadNozzle * printHeadNozzle *inkDropVolumn * xyImageThick * (totalMeanVal[0] / 256.));

		par.addParam(new RichFloat("CUsage", finalCusage, "", ""));
		par.addParam(new RichFloat("MUsage", finalMusage, "", ""));
		par.addParam(new RichFloat("YUsage", finalYusage, "", ""));

		par.addParam(new RichFloat("MaintCUsage", maintC, "", ""));
		par.addParam(new RichFloat("MaintMUsage", maintM, "", ""));
		par.addParam(new RichFloat("MaintYUsage", maintY, "", ""));

		par.addParam(new RichFloat("MaintC_Spitton", fuck_maint_c.fuck_spitton, "", ""));
		par.addParam(new RichFloat("MaintM_Spitton", fuck_maint_m.fuck_spitton, "", ""));
		par.addParam(new RichFloat("MaintY_Spitton", fuck_maint_y.fuck_spitton, "", ""));

		par.addParam(new RichFloat("MaintC_Wiper", fuck_maint_c.fuck_wiper, "", ""));
		par.addParam(new RichFloat("MaintM_Wiper", fuck_maint_m.fuck_wiper, "", ""));
		par.addParam(new RichFloat("MaintY_Wiper", fuck_maint_y.fuck_wiper, "", ""));

		par.addParam(new RichFloat("MaintC_uselessPrint", fuck_maint_c.fuck_useles_print, "", ""));
		par.addParam(new RichFloat("MaintM_uselessPrint", fuck_maint_m.fuck_useles_print, "", ""));
		par.addParam(new RichFloat("MaintY_uselessPrint", fuck_maint_y.fuck_useles_print, "", ""));

		par.addParam(new RichFloat("MaintC_PrintUsage", fuck_maint_c.fuck_printUsage, "", ""));
		par.addParam(new RichFloat("MaintM_PrintUsage", fuck_maint_m.fuck_printUsage, "", ""));
		par.addParam(new RichFloat("MaintY_PrintUsage", fuck_maint_y.fuck_printUsage, "", ""));


#endif	

		ctx.unbindReadDrawFramebuffer();
		ctx.release();
		glPopAttrib();
		glDeleteLists(drawList, 2);
		glContext->doneCurrent();

		//image.rgbSwapped().mirrored().save(par.getSaveFileName("ImageFileName"));
		//image.mirrored().save(par.getSaveFileName("ImageFileName"));
		//cap_image.mirrored().save(par.getSaveFileName("CapImageFileName"));
		//image.save(par.getSaveFileName("ImageFileName"));


		foreach(MeshModel *bm, md.meshList)
		{
			if ((bm->label().contains(olMName, Qt::CaseSensitive) || bm->label().contains(cMName, Qt::CaseSensitive)))
				//if (bm->label().contains(olMName, Qt::CaseSensitive) || bm->label().contains(cMName, Qt::CaseSensitive)))
				//if (bm->getMeshSort()==MeshModel::meshsort::)
			{
				md.delMesh(bm);
			}
		}

		/*if (par.getBool("generate_zx")){

		QString ppath = "cd /d " + getRoamingDir();
		QStringList arguments;
		arguments << "/k" << ppath << "&&" << "usbsend.exe" << "output.zx" << "&&" << "exit";

		}*/


		WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("ESTIMATE_COLOR2"));
		qDebug() << " done _applyClick ";
		break;
	}
#pragma endregion ESTIMATE_COLOR2
#pragma region FP_ESTIMATE_SURFACE_COLOR
	case FP_ESTIMATE_SURFACE_COLOR:
	{
		//QObject::connect(this, SIGNAL(test_update_pages(int)), &thread1, SLOT(page_count(int)));

		md.jobname.clear();
		//***************************

		WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("FP_ESTIMATE_SURFACE_COLOR"));

		glContext->makeCurrent();


		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			/* Problem: glewInit failed, something is seriously wrong. */
			//fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
			//qDebug("Error: %s\n", glewGetErrorString(err));
		}

		glPushAttrib(GL_ALL_ATTRIB_BITS);

		Context ctx;
		ctx.acquire();

		if (switchInitMeshVBO)
			initMeshVBO(md);

		//***20150907
		const GLsizei print_dpi = GLsizei(par.getInt("print_dpi"));
		float x = md.groove.DimX();
		float y = md.groove.DimY();
		const GLsizei width = 827; //(int)(x / DSP_inchmm * print_dpi);
		const GLsizei height = 520;// (int)(width * y / x);

		//qDebug() << "width" << "height" << width << height;
		//RenderbufferHandle hDepth = createRenderbuffer(ctx, GL_DEPTH_COMPONENT24, width, height);
		RenderbufferHandle hDepth = createRenderbuffer(ctx, GL_DEPTH24_STENCIL8, width, height);
		Texture2DHandle    hColor = createTexture2D(ctx, GL_RGB8, width, height, GL_RGB, GL_UNSIGNED_BYTE);
		//FramebufferHandle  hFramebuffer = createFramebuffer(ctx, renderbufferTarget(hDepth), texture2DTarget(hColor));
		FramebufferHandle  hFramebuffer = createFramebufferWithDepthStencil(ctx, renderbufferTarget(hDepth), renderbufferTarget(hDepth), texture2DTarget(hColor));




		const vcg::Point3f lightDirectionVS = vcg::Point3f(0.0f, 0.0f, -1.0f).Normalize();

		glEnable(GL_DEPTH_TEST);
		//glClearColor(GLfloat(backgroundColor.red())/255.0f, GLfloat(backgroundColor.green())/255.0f, GLfloat(backgroundColor.blue())/255.0f, 0.0f);
		Construction_Groove<float> tempgroove = md.groove;

		glViewport(0, 0, width, height);//***window的長寬	


		//***儲存image的基本資訊
		QImage image(int(width), int(height), QImage::Format_RGB888);
		QImage image2(int(width), int(height), QImage::Format_RGB888);
		QImage slice0Point05mmImage(int(width), int(height), QImage::Format_RGB888);
		QImage cap_image(int(width), int(height), QImage::Format_RGB888);

		//***20150505***slice_func_設置高度
		float box_bottom = md.bbox().min.Z();
#if 1								
		float unit = 1;// par.getFloat("slice_height");
		int xyImageThick = 10;//***0.1mm * 10 layer
#else
		float unit = 0.01;
		int xyImageThick = 1;
#endif

		bool startprintestimate = par.getBool("Start_Printing_Estimate");
		int boxy_dim;
		if (startprintestimate)
			boxy_dim = qCeil((md.bbox().max.Z() - md.bbox().min.Z()) / unit);//for one page
		else
			boxy_dim = qCeil((md.selBBox().max.Z() - md.selBBox().min.Z()) / unit);//for one page

		Log("boxy_dim %i", boxy_dim);

		init_texture(md);

		//*******************					  
		WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("FP_ESTIMATE_SURFACE_COLOR"));
		//***20150507 standard path******************
		//QString temppath = QStandardPaths::locate(QStandardPaths::TempLocation, "", QStandardPaths::LocateDirectory);
		//QString temppath = "D:/temptemp";//backup
		QString temppath = PicaApplication::getRoamingDir();
		QDir dir(temppath);//backup
		if (dir.exists("xyzimage"))
		{
			dir.setPath(temppath + "/xyzimage");
			dir.removeRecursively();
			dir.setPath(temppath);
			dir.mkpath("xyzimage");
		}
		else
		{
			dir.setPath(temppath);
			dir.mkpath("xyzimage");
		}

		dir.setPath(temppath + "/xyzimage");

		//---zx_file------------------------------------------
		//QString zxtemp = "D:/temptemp";
		QString zxtemp = PicaApplication::getRoamingDir();
		QDir zx_dir(zxtemp);
		if (zx_dir.exists("zx_file"))
		{
			zx_dir.setPath(zxtemp + "/zx_file");
			//zx_dir.removeRecursively();
			zx_dir.setPath(zxtemp);
			zx_dir.mkpath("zx_file");
		}
		else
		{
			dir.setPath(temppath);
			dir.mkpath("zx_file");
		}

		zx_dir.setPath(zxtemp + "/zx_file");
		WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("FP_ESTIMATE_SURFACE_COLOR"));
		//==============================
		//**********************
		ctx.bindReadDrawFramebuffer(hFramebuffer);
		GLW_CHECK_GL_READ_DRAW_FRAMEBUFFER_STATUS;
#if 1

		const bool outputOL_Image = false;// par.getBool("OL_Image");
		const bool save_cap_Image = false;// par.getBool("Cap_Image");

		const bool faceColor = par.getBool("FaceColor");
		glClearColor(1, 1, 1, 1);
		QString olMName = md.p_setting.getOlMeshName();
		QString cMName = md.p_setting.getCapMeshName();

		//***opencvMat clear
		capCTemp.clear();
		outlineCTemp.clear();


		int zx_job_pages = par.getInt("per_zx_job");
		int start_page = 1;// par.getInt("start_page");
		int send_page_num = par.getInt("send_page_num");
		int plus_pixel = par.getInt("plus_pixel");
		float PLUS_MM = par.getFloat("PLUS_MM");
		bool start_print_b = par.getBool("PRINT_START");
		float useless_print = par.getFloat("USELESS_PRINT");


		bool add_pattern = false;// par.getBool("ADD_PATTERN");
		//float meCorrectPercent = 0.97;
		float meCorrectPercent = 1;
		bool savedebugImage = false;


		int widthrecord, heightrecord;
		QString blk_img_file_name = "black_outline";
		QString zx_sn = makeOutputname(zx_dir.absolutePath(), 0);

		md.jobname.push_back(zx_sn);

		QTime time;

		time.start();

		//***20160622_test_progress bar initializing progress bar status
		if (cb != NULL)
		{
			glContext->doneCurrent();
			(*cb)(0, "processing...");
			glContext->makeCurrent();
		}

		QDir patternPath(PicaApplication::getRoamingDir());

		if (patternPath.exists("pattern"))
		{
			patternPath.setPath(PicaApplication::getRoamingDir() + "/pattern");
		}
		else
		{
			patternPath.setPath(PicaApplication::getRoamingDir());
			patternPath.mkpath("pattern");
			patternPath.setPath(PicaApplication::getRoamingDir() + "/pattern");
			//patternPath.setPath( "./pattern");

		}


		//***testLog
		Log("Closed %i holes and added %i new faces", 12, 12);
		//boxy_dim--;
		cv::Scalar totalMeanVal(0, 0, 0, 0);
		double meanSumValueC = 0;
		double meanSumValueM = 0;
		double meanSumValueY = 0;

		double meanSumValueCside = 0;
		double meanSumValueMside = 0;
		double meanSumValueYside = 0;

		int printHeadNozzle = 14000;//***640*11*2 = 14080
		double printHeadLength = DSP_grooveX;//***640*11*2 = 14080
		int imageWidth = 2600;//***22.2/DSP_inchmm*300 = 2622
		int grooveMaxlayer = 2000;//***20cm

		int tenPixelNozel = printHeadNozzle * 10 / imageWidth;
		double inkDropVolumn = 1.5e-9;//ml
		int oneSquareDpi = (1600 * 1600) / (2.54*2.54);
		//double dropsize = 1200 * 1e-12;
		double midJobDropDots = 14080000;

		double finalCusage = 0;
		double finalMusage = 0;
		double finalYusage = 0;
		double finalBusage = 0;
		if (startprintestimate)
		{
			md.multiSelectID.clear();

			foreach(MeshModel *sm, md.meshList)
			{
				//if (sm->getMeshSort() == MeshModel::meshsort::print_item)
				if (!sm->label().contains(md.p_setting.getOlMeshName(), Qt::CaseInsensitive) && !sm->label().contains(md.p_setting.getCapMeshName(), Qt::CaseInsensitive))
					md.multiSelectID.insert(sm->id());
			}
		}
		double TotalinkUsage_C = 0;
		double TotalinkUsage_M = 0;
		double TotalinkUsage_Y = 0;
		WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("FP_ESTIMATE_SURFACE_COLOR"));
		//for (int i = start_page, j = start_page + 1; i < boxy_dim; i++, j++)
		for (int i = 0; i < 1; i++)
		{
			if (cb && (i % 1) == 0)
			{
				glContext->doneCurrent();
				bool break_test = cb((i * 100) / boxy_dim, "slice process");
				if (!break_test)
				{
					glContext->doneCurrent();
					return false;
					break;
				}

			}
			glContext->makeCurrent();
			//xyz::slice_roution(md, box_bottom + unit*i, faceColor);//***backup******************切層演算							  
			time.restart();

			//xyz::slice_roution_selected(md, box_bottom + unit*i, faceColor);//****20160301*****************切層演算							  											  
			//***第一張outline圖*****************************
			QVector<MeshModel *> vmm;
			//md.getMeshByContainString("_temp_outlines", vmm);
			md.getMeshByContainString(olMName, vmm);
			//QVector<MeshModel *> cmm;
			//md.getMeshByContainString(cMName, cmm);//capImage
			int xx = 0;
			QString img_file_name = md.p_setting.getoutlineName();
			float cut_z = box_bottom + unit*i;

			double area = 0;


			CMeshO::FaceIterator fi;
			int x = md.multiSelectID.size();
			foreach(int i, md.multiSelectID)
			{
				MeshModel &ma = *md.getMesh(i);
				for (fi = ma.cm.face.begin(); fi != ma.cm.face.end(); ++fi)
				{
					vcg::Point3f v0 = fi->V(1)->P() - fi->V(0)->P();
					vcg::Point3f v1 = fi->V(2)->P() - fi->V(0)->P();
					//
					area = (1. / 2) / 100. * sqrt(SKT::Length<float>(v0)*SKT::Length<float>(v0)*SKT::Length<float>(v1)*SKT::Length<float>(v1)-(v0*v1)*(v0*v1));
					if (tri::HasPerVertexColor(ma.cm) || tri::HasPerWedgeTexCoord(ma.cm))
					{
						//if ((255 - int(fi->V(0)->C().X()))>0)
						//{

						/*TotalinkUsage_C += oneSquareDpi*area*inkDropVolumn*((255 - int(fi->V(0)->C().X())) / 255.)*10;
						TotalinkUsage_M += oneSquareDpi*area*inkDropVolumn*((255 - int(fi->V(1)->C().Y())) / 255.)*10;
						TotalinkUsage_Y += oneSquareDpi*area*inkDropVolumn*((255 - int(fi->V(2)->C().Z())) / 255.)*10;*/
						TotalinkUsage_C += oneSquareDpi*area*inkDropVolumn*(1.) * 10;
						TotalinkUsage_M += oneSquareDpi*area*inkDropVolumn*(1) * 10;
						TotalinkUsage_Y += oneSquareDpi*area*inkDropVolumn*(1) * 10;
						//}

					}

				}
			}


		}


		float uselessPrintWidth = par.getFloat("USELESS_PRINT");
		//uselessBinderUsage
		//nozzle *
		//double uselessPrintUsage = printHeadNozzle * (printHeadNozzle*uselessPrintWidth / printHeadLength) * boxy_dim * inkDropVolumn;
		double uselessPrintUsage = printHeadNozzle * (printHeadNozzle*uselessPrintWidth / printHeadLength) * boxy_dim * inkDropVolumn;


		jobServiceCount prejob(400, 800, 200, 800, 400);
		jobServiceCount midjob(400, 800, 200, 800, 400);
		jobServiceCount postjob(2400, 2800, 2200, 2800, 2400);
		int DSP_Midjob_Frequence = par.getInt("MIDJOB_FREQUENCY");
		//int midJobFrequency = 4;
		int nMidJob = (boxy_dim / DSP_Midjob_Frequence) * 10;//10 times larger than slice mode


		//assume layer_height is 0.1mm
		finalCusage = uselessPrintUsage +
			prejob.maintUsage(jobServiceCount::inktype::c) +
			midjob.maintUsage(jobServiceCount::inktype::c) * nMidJob + nMidJob * DSP_wipe_midJob +
			postjob.maintUsage(jobServiceCount::inktype::c) + DSP_wipe_postJob
			+ TotalinkUsage_C;// (printHeadNozzle * printHeadNozzle *inkDropVolumn * xyImageThick * (totalMeanVal[2] / 256.));
		finalMusage = uselessPrintUsage +
			prejob.maintUsage(jobServiceCount::inktype::m) +
			midjob.maintUsage(jobServiceCount::inktype::m) * nMidJob + nMidJob * DSP_wipe_midJob +
			postjob.maintUsage(jobServiceCount::inktype::m) + DSP_wipe_postJob
			+ TotalinkUsage_M;// (printHeadNozzle * printHeadNozzle *inkDropVolumn * xyImageThick * (totalMeanVal[1] / 256.));
		finalYusage = uselessPrintUsage +
			prejob.maintUsage(jobServiceCount::inktype::y) +
			midjob.maintUsage(jobServiceCount::inktype::y) * nMidJob + nMidJob * DSP_wipe_midJob +
			postjob.maintUsage(jobServiceCount::inktype::y) + DSP_wipe_postJob
			+ TotalinkUsage_Y;// (printHeadNozzle * printHeadNozzle *inkDropVolumn * xyImageThick * (totalMeanVal[0] / 256.));


		par.addParam(new RichFloat("CUsage", finalCusage, "", ""));
		par.addParam(new RichFloat("MUsage", finalMusage, "", ""));
		par.addParam(new RichFloat("YUsage", finalYusage, "", ""));

		WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("FP_ESTIMATE_SURFACE_COLOR"));
#endif	

		ctx.unbindReadDrawFramebuffer();
		ctx.release();
		glPopAttrib();
		glContext->doneCurrent();
		foreach(MeshModel *bm, md.meshList)
		{
			if ((bm->label().contains(olMName, Qt::CaseSensitive) || bm->label().contains(cMName, Qt::CaseSensitive)))
				//if (bm->label().contains(olMName, Qt::CaseSensitive) || bm->label().contains(cMName, Qt::CaseSensitive)))
				//if (bm->getMeshSort()==MeshModel::meshsort::)
			{
				md.delMesh(bm);
			}
		}



		WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("FP_ESTIMATE_SURFACE_COLOR"));

		qDebug() << " done _applyClick ";
		break;



	}break;
#pragma endregion FP_ESTIMATE_SURFACE_COLOR
#pragma region FP_PRINT_JR
	case FP_PRINT_JR:
	{

		QObject::connect(this, SIGNAL(test_update_pages(int)), &thread1, SLOT(page_count(int)));

		md.jobname.clear();
		//***************************


		CMeshO & mesh = md.mm()->cm;
		if ((mesh.vn < 3) || (mesh.fn < 1)) return false;

		const unsigned char * p0 = (const unsigned char *)(&(mesh.vert[0].P()));
		const unsigned char * p1 = (const unsigned char *)(&(mesh.vert[1].P()));
		const void *          pbase = p0;
		GLsizei               pstride = GLsizei(p1 - p0);

		const unsigned char * n0 = (const unsigned char *)(&(mesh.vert[0].N()));
		const unsigned char * n1 = (const unsigned char *)(&(mesh.vert[1].N()));
		const void *          nbase = n0;
		GLsizei               nstride = GLsizei(n1 - n0);

		/*glContext->doneCurrent();
		glContext->moveToThread(th1);*/

		glContext->makeCurrent();


		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			/* Problem: glewInit failed, something is seriously wrong. */
			//fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
			//qDebug("Error: %s\n", glewGetErrorString(err));
		}

		glPushAttrib(GL_ALL_ATTRIB_BITS);

		Context ctx;
		ctx.acquire();
		if (switchInitMeshVBO)
			initMeshVBO(md);

		const GLsizeiptr psize = GLsizeiptr(GLsizei(mesh.vn) * pstride);
		BufferHandle hPositionBuffer = createBuffer(ctx, psize, pbase);

		const GLsizeiptr nsize = GLsizeiptr(GLsizei(mesh.vn) * nstride);
		BufferHandle hNormalBuffer = createBuffer(ctx, nsize, nbase);

		const GLsizeiptr isize = GLsizeiptr(mesh.fn * 3 * sizeof(GLuint));
		BufferHandle hIndexBuffer = createBuffer(ctx, isize);
		{
			BoundIndexBufferHandle indexBuffer = ctx.bindIndexBuffer(hIndexBuffer);

			const CMeshO::VertexType * vbase = &(mesh.vert[0]);
			GLuint *  indices = (GLuint *)indexBuffer->map(GL_WRITE_ONLY);
			for (size_t i = 0; i < mesh.face.size(); ++i)
			{
				const CMeshO::FaceType & f = mesh.face[i];
				if (f.IsD()) continue;
				for (int v = 0; v < 3; ++v)
				{
					*indices++ = GLuint(vcg::tri::Index(mesh, f.cV(v)));
				}
			}
			indexBuffer->unmap();

			ctx.unbindIndexBuffer();
		}

		//***20150907
		const GLsizei print_dpi = GLsizei(par.getInt("print_dpi"));
		float x = md.groove.DimX();
		float y = md.groove.DimY();
		const GLsizei width = (int)(x / DSP_inchmm * print_dpi);
		const GLsizei height = (int)(width * y / x);

		//qDebug() << "width" << "height" << width << height;
		//RenderbufferHandle hDepth = createRenderbuffer(ctx, GL_DEPTH_COMPONENT24, width, height);
		RenderbufferHandle hDepth = createRenderbuffer(ctx, GL_DEPTH24_STENCIL8, width, height);
		Texture2DHandle    hColor = createTexture2D(ctx, GL_RGB8, width, height, GL_RGB, GL_UNSIGNED_BYTE);
		//FramebufferHandle  hFramebuffer = createFramebuffer(ctx, renderbufferTarget(hDepth), texture2DTarget(hColor));
		FramebufferHandle  hFramebuffer = createFramebufferWithDepthStencil(ctx, renderbufferTarget(hDepth), renderbufferTarget(hDepth), texture2DTarget(hColor));




		const vcg::Point3f lightDirectionVS = vcg::Point3f(0.0f, 0.0f, -1.0f).Normalize();

		glEnable(GL_DEPTH_TEST);
		//glClearColor(GLfloat(backgroundColor.red())/255.0f, GLfloat(backgroundColor.green())/255.0f, GLfloat(backgroundColor.blue())/255.0f, 0.0f);
		Construction_Groove<float> tempgroove = md.groove;

		glViewport(0, 0, width, height);//***window的長寬				  

		const vcg::Point3f center = mesh.bbox.Center();
		const float        scale = 1.0f / mesh.bbox.Diag();

		//glScalef(scale, scale, scale);
		//glTranslatef(-center[0], -center[1], -center[2]);

		//***儲存image的基本資訊
		QImage image(int(width), int(height), QImage::Format_RGB888);
		QImage image2(int(width), int(height), QImage::Format_RGB888);
		QImage cap_image(int(width), int(height), QImage::Format_RGB888);

		//***20150505***slice_func_設置高度
		float box_bottom = md.bbox().min.Z();
		float unit = par.getFloat("slice_height");



		int boxy_dim = qCeil((md.bbox().max.Z() - md.bbox().min.Z()) / unit) + 1;
		init_texture(md);

		//*******************					  

		//***20150507 standard path******************
		//QString temppath = QStandardPaths::locate(QStandardPaths::TempLocation, "", QStandardPaths::LocateDirectory);
		//QString temppath = "D:/temptemp";//backup
		QString temppath = PicaApplication::getRoamingDir();
		QDir dir(temppath);//backup
		if (dir.exists("xyzimage"))
		{
			dir.setPath(temppath + "/xyzimage");
			dir.removeRecursively();
			dir.setPath(temppath);
			dir.mkpath("xyzimage");
		}
		else
		{
			dir.setPath(temppath);
			dir.mkpath("xyzimage");
		}

		dir.setPath(temppath + "/xyzimage");

		//---zx_file------------------------------------------
		//QString zxtemp = "D:/temptemp";
		QString zxtemp = PicaApplication::getRoamingDir();
		QDir zx_dir(zxtemp);
		if (zx_dir.exists("zx_file"))
		{
			zx_dir.setPath(zxtemp + "/zx_file");
			//zx_dir.removeRecursively();
			zx_dir.setPath(zxtemp);
			zx_dir.mkpath("zx_file");
		}
		else
		{
			dir.setPath(temppath);
			dir.mkpath("zx_file");
		}

		zx_dir.setPath(zxtemp + "/zx_file");

		//==============================
		//**********************
		ctx.bindReadDrawFramebuffer(hFramebuffer);
		GLW_CHECK_GL_READ_DRAW_FRAMEBUFFER_STATUS;
#if 1

		const bool outputOL_Image = par.getBool("OL_Image");
		const bool save_cap_Image = par.getBool("Cap_Image");

		const bool faceColor = par.getBool("FaceColor");
		glClearColor(1, 1, 1, 1);
		QString olMName = md.p_setting.getOlMeshName();
		QString cMName = md.p_setting.getCapMeshName();

		//***opencvMat clear
		capCTemp.clear();
		outlineCTemp.clear();


		int zx_job_pages = par.getInt("per_zx_job");
		int start_page = par.getInt("start_page");
		int send_page_num = par.getInt("send_page_num");
		int plus_pixel = par.getInt("plus_pixel");
		float PLUS_MM = par.getFloat("PLUS_MM");
		bool start_print_b = par.getBool("PRINT_START");
		float useless_print = par.getFloat("USELESS_PRINT");
		//float meCorrectPercent = 0.97;
		float meCorrectPercent = 1;

		int widthrecord, heightrecord;
		QString blk_img_file_name = "black_outline";
		QString zx_sn = makeOutputname(zx_dir.absolutePath(), 0);

		md.jobname.push_back(zx_sn);

		//zggg = new ZxGen(unsigned int(width), unsigned int(height), zx_pages, md.groove.DimX(),21, zx_sn.toStdString());//20161219_backup						 
		//zggg = new ZxGen(unsigned int(width), 2580, zx_job_pages, md.groove.DimX(), 21, zx_sn.toStdString());//20161219_backup						 
		//zggg = new ZxGen(unsigned int(width), unsigned int(height*meCorrectPercent + plus_pixel), zx_job_pages, md.groove.DimX(), md.groove.DimY() + PLUS_MM, zx_sn.toStdString(), start_print_b);//20161219_backup						 
		//zggg = new ZxGen(unsigned int(width), unsigned int(4713), zx_job_pages, md.groove.DimX(), md.groove.DimY() + PLUS_MM, zx_sn.toStdString(), start_print_b);//20161219_backup						 

		FILE *dbgff;
		dbgff = fopen("D:\\debug_open_1.txt", "w");//@@@
		QTime time;
		//clock_t start, stop;
		time.start();

		//***20160622_test_progress bar initializing progress bar status
		if (cb != NULL)
		{
			glContext->doneCurrent();
			(*cb)(0, "processing...");
			glContext->makeCurrent();
		}

		QDir patternPath(PicaApplication::getRoamingDir());

		if (patternPath.exists("pattern"))
		{
		}
		else
		{
			patternPath.setPath(PicaApplication::getRoamingDir());
			patternPath.mkpath("pattern");
		}
		patternPath.setPath(PicaApplication::getRoamingDir() + "/pattern");

		cv::Mat patternImg;
		patternImg = cv::imread(patternPath.absolutePath().toStdString() + "/hexagon_4.png", CV_LOAD_IMAGE_GRAYSCALE);
		//boxy_dim--;

		cv::Mat look_down_color_CV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat look_down_Black_Mask_layer(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat look_down_Black_MaskCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat look_up_Black_MaskCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat look_up_color_CV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat captemp(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat capDownMask(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat black_Mask(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat black_Mask2(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat outlineBlackMaskCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat lookDownBlackTriangleMask(height, width, CV_8UC3, Scalar(255, 255, 255));

		cv::Mat prePrintobjectC(height, width, CV_8UC3, Scalar(255, 255, 255));

		cv::Mat downColorFirstCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat downBlackMaskFirstCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat downColorSecondCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat downBlackMaskSecondCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat prePrintobjectB(height, width, CV_8UC1, Scalar(255));

		cv::Mat capDownMask_bottom_face(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat capDownMask_top_face(height, width, CV_8UC3, Scalar(255, 255, 255));


		cv::Mat color_Bottom_face(height, width, CV_8UC3, Scalar(255, 255, 255));



		cv::Mat color_Top_face(height, width, CV_8UC3, Scalar(255, 255, 255));



		cv::Mat originalOutline(height, width, CV_8UC3, Scalar(255, 255, 255));


		cv::Mat outlineColorCv(height, width, CV_8UC3, Scalar(255, 255, 255));


		cv::Mat capDownSureMask(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat capTopSureMask(height, width, CV_8UC3, Scalar(255, 255, 255));

		for (int i = start_page, j = start_page + 1; i < boxy_dim; i++, j++)
		{
			if (cb && (i % 1) == 0)
			{
				glContext->doneCurrent();
				bool break_test = cb((i * 100) / boxy_dim, "slice process");
				if (!break_test)
				{
					glContext->doneCurrent();
					return false;
					break;
				}

			}
			glContext->makeCurrent();
			//xyz::slice_roution(md, box_bottom + unit*i, faceColor);//***backup******************切層演算							  
			time.restart();
			xyz::slice_roution(md, box_bottom + unit*i, faceColor);//****20160301*****************切層演算							  

			/*fprintf(dbgff, "slice_func %i, %i\n", time.elapsed(), i);
			fflush(dbgff);*/

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);
			//time.restart();
			//***第一張outline圖*****************************
			QVector<MeshModel *> vmm;
			md.getMeshByContainString("_temp_outlines", vmm);
			md.getMeshByContainString(olMName, vmm);
			QVector<MeshModel *> cmm;
			md.getMeshByContainString(cMName, cmm);//capImage
			int xx = 0;

			QString img_file_name = md.p_setting.getoutlineName();
			float cut_z = box_bottom + unit*i;

			foreach(MeshModel *mdmm, md.meshList)
			{
				//****20151231********************************************************************

				if (mdmm->getMeshSort() == MeshModel::meshsort::print_item)
					if (mdmm->rmm.colorMode != GLW::CMNone || mdmm->rmm.textureMode != GLW::TMNone)//如果無色就不須做
						if (cut_z - mdmm->cm.bbox.min.Z() < 0 || cut_z - mdmm->cm.bbox.max.Z() > 0)//在物體box的上下需要設定限制，才不會超出物體大小。								  
						{
							glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
							glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., 0.0001, 0);
							glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, cut_z, 0, 0, -30, 0, 1, 0);

							glPushMatrix();
							/*if (vcg::tri::HasPerWedgeTexCoord(mdmm->cm) && !mdmm->cm.textures.empty())
							mdmm->render(GLW::DMSmooth, GLW::CMNone, GLW::TMPerWedgeMulti);
							else if (vcg::tri::HasPerFaceColor(mdmm->cm))
							mdmm->render(GLW::DMSmooth, GLW::CMPerFace, GLW::TMNone);
							else if (vcg::tri::HasPerVertexColor(mdmm->cm))
							mdmm->render(GLW::DMSmooth, GLW::CMPerVert, GLW::TMNone);*/
							//mdmm->render(GLW::DMSmooth, mdmm->rmm.colorMode, mdmm->rmm.textureMode);
							mdmm->render(GLW::DMSmooth, mdmm->rmm.colorMode, mdmm->rmm.textureMode);

							glPopMatrix();


						}

						else//物體梯田情況會有0.1層顏色的厚度。
						{
							glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
							//glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., 0.01, 0);//backup
							//glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., 0.005, -0.005);
							glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -0.1, 0.1);
							//***20161112
							glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, cut_z, 0, 0, -30, 0, 1, 0);

							glPushMatrix();
							/*if (vcg::tri::HasPerWedgeTexCoord(mdmm->cm) && !mdmm->cm.textures.empty())
							mdmm->render(GLW::DMSmooth, GLW::CMNone, GLW::TMPerWedgeMulti);
							else if (vcg::tri::HasPerFaceColor(mdmm->cm))
							mdmm->render(GLW::DMSmooth, GLW::CMPerFace, GLW::TMNone);
							else if (vcg::tri::HasPerVertexColor(mdmm->cm))
							mdmm->render(GLW::DMSmooth, GLW::CMPerVert, GLW::TMNone);*/
							//mdmm->render(GLW::DMFlat, mdmm->rmm.colorMode, mdmm->rmm.textureMode);
							mdmm->render(GLW::DMSmooth, mdmm->rmm.colorMode, mdmm->rmm.textureMode);
							glPopMatrix();
						}

			}
			//***20150508***BMP QImage設為Format_RGBA8888, readpixel要設為GL_RGBA
			glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image.bits());
			//===
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();

			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -1000, 1000);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, 0, 0, 1, 0);
			//cmystrips(md, par.getFloat("useless_print") / 3.);//***useless bar
			foreach(MeshModel *mp, vmm)//畫輪廓圖。
			{
				mp->glw.viewerNum = 2;
				if (vmm.size() > 0)
				{
					mp->render(vcg::GLW::DMTexWire, mp->rmm.colorMode, mp->rmm.textureMode);//backup									 
					//mp->render(vcg::GLW::DMTexWire, GLW::CMNone, mp->rmm.textureMode);//backup									 
				}
			}

			glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image2.bits());
			combineImage(image2, image);

			//***第一張圖片
			if (par.getBool("generate_zx"))
			{
				originalOutline = QImageToCvMat(image.mirrored());
				//outlineCTemp.push_back(QImageToCvMat(image.mirrored())); //zxzxzxzxzxzxzxz
			}
			if (outputOL_Image)//彩色輪廓
			{
				img_file_name.append(QString("abb_%1.png").arg(i, 4, 10, QChar('0')));
				image.mirrored().save(dir.absolutePath() + "/" + img_file_name, "png");
			}


			//***20160122***產生黑色輪廓編號_mask
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);

			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -100, 100);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, 0, 0, 1, 0);
			//*********************************
			//binderstrips(md, par.getFloat("useless_print"));//******uselessbar
			//image.fill(QColor::w);
			foreach(MeshModel *mp, vmm)//outline
			{
				mp->glw.viewerNum = 2;
				if (vmm.size() > 0)
				{
					//mp->render(vcg::GLW::DMTexWire, mp->rmm.colorMode, mp->rmm.textureMode);//backup
					mp->render(vcg::GLW::DMTexWire, vcg::GLW::CMBlack, vcg::GLW::TMNone);//backup

					//mp->render(vcg::GLW::DMOutline_Test, vcg::GLW::CMNone, vcg::GLW::TMNone);
				}
			}
			foreach(MeshModel *mdmm, md.meshList)
			{
				//****20151231********************************************************************								  
				if (mdmm->getMeshSort() == MeshModel::meshsort::print_item)
					if (cut_z - mdmm->cm.bbox.min.Z() < 0 || cut_z - mdmm->cm.bbox.max.Z() > 0)//在物體box的上下需要設定限制，才不會超出物體大小。
					{
						glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
						glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., 0.0001, 0);
						glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, cut_z, 0, 0, -30, 0, 1, 0);

						mdmm->render(GLW::DMFlat, GLW::CMBlack, GLW::TMNone);
						glPopMatrix();

					}
					else//物體梯田情況會有0.1層顏色的厚度。
					{
						glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
						glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -0.6, 0.6);
						//glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -0.2, 0.2);
						//***20161112
						glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, cut_z, 0, 0, -30, 0, 1, 0);

						glPushMatrix();
						mdmm->render(GLW::DMFlat, GLW::CMBlack, GLW::TMNone);
						glPopMatrix();
					}

			}
			//fFaceColor
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();



			glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image.bits());
			black_Mask = QImageToCvMat(image.mirrored());
			if (par.getBool("BLK_OUT_LINE"))
			{
				blk_img_file_name.append(QString("blk_%1.png").arg(i, 4, 10, QChar('0')));
				image.mirrored().save(dir.absolutePath() + "/" + blk_img_file_name, "png");
			}

			/*fprintf(dbgff, "render_process %i, %i\n", time.elapsed(), i);
			fflush(dbgff);*/
			//===========================================================================================
			equ[3] = box_bottom + unit*i;
			//***第二張圖***產生膠水圖**********//
			//time.restart();
			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -100, 100);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, -20, 0, 1, 0);
			//*********************************
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);

			//binderstrips(md, par.getFloat("useless_print"));//***uselessbar

			//************************************************//
			//****** Rendering the mesh's clip edge ****//
			GLuint MY_CLIP_PLANE = GL_CLIP_PLANE0;
			glEnable(MY_CLIP_PLANE);
			glClipPlane(MY_CLIP_PLANE, equ);

			glEnable(GL_STENCIL_TEST);
			glEnable(GL_CULL_FACE);
			glClear(GL_STENCIL_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

			//// first pass: increment stencil buffer value on back faces
			glStencilFunc(GL_ALWAYS, 0, 0);
			glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
			glCullFace(GL_FRONT); // render back faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			//glRotatef(90, 1, 0, 0);
			foreach(MeshModel * mp, md.meshList)
			{
				if (mp->getMeshSort() == MeshModel::meshsort::print_item)
					//mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
					mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
			}

			glPopMatrix();
			glPopAttrib();
			//second pass: decrement stencil buffer value on front faces
			glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
			glCullFace(GL_BACK); // render front faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			//glRotatef(90, 1, 0, 0);
			foreach(MeshModel * mp, md.meshList)
			{
				if (mp->getMeshSort() == MeshModel::meshsort::print_item)
					//mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
					mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
			}
			glPopMatrix();
			glPopAttrib();
			//-----------------------------
			// drawing clip planes masked by stencil buffer content
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glEnable(GL_DEPTH_TEST);
			glDisable(MY_CLIP_PLANE);
			glStencilFunc(GL_NOTEQUAL, 0, ~0);
			// stencil test will pass only when stencil buffer value = 0; 
			// (~0 = 0x11...11)

			glColor4f(1, 1, 1, 1);
			fillsquare();
			glDisable(GL_STENCIL_TEST);
			glEnable(MY_CLIP_PLANE); // enabling clip plane again
			glDisable(GL_LIGHTING);

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			//********************************************************************************************//


			glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, cap_image.bits());//***20150724
			//glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, cap_image.bits());							   
			QString img_file_name2 = md.p_setting.getcapImageName();
			glDisable(MY_CLIP_PLANE);//存完圖
			glDisable(GL_CULL_FACE);
			//****

			/*fprintf(dbgff, "cap_image_process %i, %i\n", time.elapsed(), i);
			fflush(dbgff);*/

			cv::Mat captemp;
			if (par.getBool("generate_zx"))
			{
				cv::cvtColor(QImageToCvMat(cap_image.mirrored()), captemp, CV_RGB2GRAY);
			}
			//***20160112***image_process加上插點pattern
			if (par.getBool("generate_zx"))
			{

				SKT::InkBalance imageProcess;

				cv::Mat temp, temp2;

				imageProcess.operate2(originalOutline, captemp, black_Mask, false, true);
				if (par.getBool("mono_bool"))originalOutline = cv::Scalar(255, 255, 255);
				originalOutline = SKT::resizeIamge(&originalOutline, plus_pixel);
				bool longDin = false;
				SKT::generateUselessbarDin(originalOutline, useless_print, longDin);
				//outlineCTemp.push_back(originalOutline);//**********PUSH_BACK_OUTLINE********************************

				///////cap_temp_change_add_pattern///////////////////
				black_Mask = SKT::erodeImage(&black_Mask, 51);
				//captemp = SKT::binderAddPattern(&captemp,&black_Mask);
				//if(((i-60)%100)>100)
				if ((i % 100) > 20)
					captemp = SKT::binderAddHexagon(&captemp, &black_Mask, patternImg);
				//////////////////////////////////								
				captemp = SKT::resizeIamge(&captemp, plus_pixel);
				//SKT::generateUselessbar(captemp, useless_print);//20160715_backup
				SKT::generateUselessbarDin(captemp, useless_print, longDin);//20160715_backup								  
				//capCTemp.push_back(captemp);//***********PUSH_BACK_CAPIMAGE******************************************

			}
			if (save_cap_Image){
				//cap_image = SKT::resizeIamge(QImageToCvMat(cap_image));
				QImage cap_Test = cvMatToQImage(captemp);
				img_file_name2.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				//cap_Test.save(dir.absolutePath() + "/" + img_file_name2, "png");
			}



			if (par.getBool(("generate_final_picture")))
			{
#if 0				

#else
				//**picasso_junior****************************
				cv::Mat test = SKT::resizeXYAxis(&originalOutline, 2240, 2240);
				test = SKT::resizeIamge(&test, 10);
				QImage jr_c(cvMatToQImage(test));
				//QImage ttt(cvMatToQImage(originalOutline));
				QString tttt = "tttt";
				tttt.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				jr_c.save(dir.absolutePath() + "/" + tttt, "png");
				//////////////black_mask///////////////////////////////

				//////////captemp//////////
				cv::Mat test2 = SKT::resizeXYAxis(&captemp, 2240, 2240);
				test2 = SKT::resizeIamge(&test2, 10);
				QImage jr_b(cvMatToQImage(test2));

				QString binder_image_text = "binder_image";
				binder_image_text.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				jr_b.save(dir.absolutePath() + "/" + binder_image_text, "png");
				///////////////////////////
				QImage jr_f(jr_c.width(), jr_c.height(), QImage::Format_RGBA8888);
				for (int i = 0; i < jr_c.width(); i++)
					for (int j = 0; j < jr_c.height(); j++)
					{

						QRgb value = qRgba(qRed(jr_c.pixel(i, j)), qGreen(jr_c.pixel(i, j)), qBlue(jr_c.pixel(i, j)), 255 - qRed(jr_b.pixel(i, j)));
						//QRgb value = qRgba(qRed(jr_c.pixel(i, j)), qGreen(jr_c.pixel(i, j)), qBlue(jr_c.pixel(i, j)), 255);
						jr_f.setPixel(i, j, value);

						//if (i>2)
						//if (qGreen(jr_c.pixel(i, j)==0) && qRed(jr_c.pixel(i, j))==0 && qBlue(jr_c.pixel(i, j))==0)
						//qDebug() << "qRed(jr_c.pixel(i, j)" << qRed(jr_c.pixel(i, j)) << qGreen(jr_c.pixel(i, j)) << qBlue(jr_c.pixel(i, j));
						//QRgb value2 = jr_f.pixel(i, j);						

					}
				QString jrFI = "";
				jrFI.append(QString("%1.tif").arg(i, 4, 10, QChar('0')));
				QString savedir = dir.absolutePath() + "/" + jrFI;
				QImageWriter writer(savedir);
				bool fffff = writer.canWrite();
				QString fffdasdfasdf = writer.errorString();
				writer.setCompression(1);
				writer.write(jr_f);
				//***end_picasso_junior
#endif

			}






			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();


			//*****test_thread
			emit test_update_pages(i);
			thread1.start();


			/*fprintf(dbgff, "b_name %i\n", i);
			fflush(dbgff);*/

		}
		//thread1.exit();



#endif	

		ctx.unbindReadDrawFramebuffer();
		ctx.release();
		glPopAttrib();
		glContext->doneCurrent();

		//image.rgbSwapped().mirrored().save(par.getSaveFileName("ImageFileName"));
		//image.mirrored().save(par.getSaveFileName("ImageFileName"));
		//cap_image.mirrored().save(par.getSaveFileName("CapImageFileName"));
		//image.save(par.getSaveFileName("ImageFileName"));


		foreach(MeshModel *bm, md.meshList)
		{
			if ((bm->label().contains(olMName, Qt::CaseSensitive) || bm->label().contains(cMName, Qt::CaseSensitive)))
				//if (bm->label().contains(olMName, Qt::CaseSensitive) || bm->label().contains(cMName, Qt::CaseSensitive)))
				//if (bm->getMeshSort()==MeshModel::meshsort::)
			{
				md.delMesh(bm);
			}
		}

		/*if (par.getBool("generate_zx")){

		QString ppath = "cd /d " + getRoamingDir();
		QStringList arguments;
		arguments << "/k" << ppath << "&&" << "usbsend.exe" << "output.zx" << "&&" << "exit";

		}*/



		qDebug() << " done _applyClick ";
		break;



	}break;
#pragma endregion FP_PRINT_JR
#pragma region FP_ESTIMATE_COLOR
	case FP_ESTIMATE_COLOR:
	{
		/*QProgressDialog dialog;
		dialog.setLabelText(QString("Progressing using thread(s)..."));
		dialog.setRange(0, 100);*/
		// Create a QFutureWatcher and connect signals and slots.
		/*QObject::connect(this, SIGNAL(test_update_pages(int)), &dialog, SLOT(setValue(int)));	*/
		QObject::connect(this, SIGNAL(test_update_pages(int)), &thread1, SLOT(page_count(int)));

		md.jobname.clear();
		//***************************


		CMeshO & mesh = md.mm()->cm;
		if ((mesh.vn < 3) || (mesh.fn < 1)) return false;

		const unsigned char * p0 = (const unsigned char *)(&(mesh.vert[0].P()));
		const unsigned char * p1 = (const unsigned char *)(&(mesh.vert[1].P()));
		const void *          pbase = p0;
		GLsizei               pstride = GLsizei(p1 - p0);

		const unsigned char * n0 = (const unsigned char *)(&(mesh.vert[0].N()));
		const unsigned char * n1 = (const unsigned char *)(&(mesh.vert[1].N()));
		const void *          nbase = n0;
		GLsizei               nstride = GLsizei(n1 - n0);

		/*glContext->doneCurrent();
		glContext->moveToThread(th1);*/

		glContext->makeCurrent();


		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			/* Problem: glewInit failed, something is seriously wrong. */
			//fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
			//qDebug("Error: %s\n", glewGetErrorString(err));
		}

		glPushAttrib(GL_ALL_ATTRIB_BITS);

		Context ctx;
		ctx.acquire();
		if (switchInitMeshVBO)
			initMeshVBO(md);

		const GLsizeiptr psize = GLsizeiptr(GLsizei(mesh.vn) * pstride);
		BufferHandle hPositionBuffer = createBuffer(ctx, psize, pbase);

		const GLsizeiptr nsize = GLsizeiptr(GLsizei(mesh.vn) * nstride);
		BufferHandle hNormalBuffer = createBuffer(ctx, nsize, nbase);

		const GLsizeiptr isize = GLsizeiptr(mesh.fn * 3 * sizeof(GLuint));
		BufferHandle hIndexBuffer = createBuffer(ctx, isize);
		{
			BoundIndexBufferHandle indexBuffer = ctx.bindIndexBuffer(hIndexBuffer);

			const CMeshO::VertexType * vbase = &(mesh.vert[0]);
			GLuint *  indices = (GLuint *)indexBuffer->map(GL_WRITE_ONLY);
			for (size_t i = 0; i < mesh.face.size(); ++i)
			{
				const CMeshO::FaceType & f = mesh.face[i];
				if (f.IsD()) continue;
				for (int v = 0; v < 3; ++v)
				{
					*indices++ = GLuint(vcg::tri::Index(mesh, f.cV(v)));
				}
			}
			indexBuffer->unmap();

			ctx.unbindIndexBuffer();
		}

		//***20150907
		const GLsizei print_dpi = GLsizei(par.getInt("print_dpi"));
		float x = md.groove.DimX();
		float y = md.groove.DimY();
		const GLsizei width = 400;// (int)(x / DSP_inchmm * print_dpi);
		const GLsizei height = 400;// (int)(width * y / x);

		//qDebug() << "width" << "height" << width << height;
		//RenderbufferHandle hDepth = createRenderbuffer(ctx, GL_DEPTH_COMPONENT24, width, height);
		RenderbufferHandle hDepth = createRenderbuffer(ctx, GL_DEPTH24_STENCIL8, width, height);
		Texture2DHandle    hColor = createTexture2D(ctx, GL_RGB8, width, height, GL_RGB, GL_UNSIGNED_BYTE);
		//FramebufferHandle  hFramebuffer = createFramebuffer(ctx, renderbufferTarget(hDepth), texture2DTarget(hColor));
		FramebufferHandle  hFramebuffer = createFramebufferWithDepthStencil(ctx, renderbufferTarget(hDepth), renderbufferTarget(hDepth), texture2DTarget(hColor));




		const vcg::Point3f lightDirectionVS = vcg::Point3f(0.0f, 0.0f, -1.0f).Normalize();

		glEnable(GL_DEPTH_TEST);
		//glClearColor(GLfloat(backgroundColor.red())/255.0f, GLfloat(backgroundColor.green())/255.0f, GLfloat(backgroundColor.blue())/255.0f, 0.0f);
		Construction_Groove<float> tempgroove = md.groove;

		glViewport(0, 0, width, height);//***window的長寬				  

		const vcg::Point3f center = mesh.bbox.Center();
		const float        scale = 1.0f / mesh.bbox.Diag();

		//glScalef(scale, scale, scale);
		//glTranslatef(-center[0], -center[1], -center[2]);

		//***儲存image的基本資訊
		std::vector<QImage *> estimate_image;
		std::vector<cv::Mat *> estimate_imageCV;

		//QImage *estimate_image[6];
		for (int i = 0; i < 6; i++)
			estimate_image.push_back(new QImage(int(width), int(height), QImage::Format_RGB888));

		QImage image(int(width), int(height), QImage::Format_RGB888);
		QImage image2(int(width), int(height), QImage::Format_RGB888);
		QImage cap_image(int(width), int(height), QImage::Format_RGB888);

		//***20150505***slice_func_設置高度
		float box_bottom = md.bbox().min.Z();
		float unit = 0.1;// par.getFloat("slice_height");



		int boxy_dim = qCeil((md.bbox().max.Z() - md.bbox().min.Z()) / unit) + 1;
		//int(mdd->bbox().DimZ() / pJI.layerThickness.value) + 1;
		Log("boxy_dim %i", boxy_dim);

		init_texture(md);

		//*******************					  

		//***20150507 standard path******************
		//QString temppath = QStandardPaths::locate(QStandardPaths::TempLocation, "", QStandardPaths::LocateDirectory);
		//QString temppath = "D:/temptemp";//backup
		QString temppath = PicaApplication::getRoamingDir();
		QDir dir(temppath);//backup
		if (dir.exists("xyzimage"))
		{
			dir.setPath(temppath + "/xyzimage");
			dir.removeRecursively();
			dir.setPath(temppath);
			dir.mkpath("xyzimage");
		}
		else
		{
			dir.setPath(temppath);
			dir.mkpath("xyzimage");
		}

		dir.setPath(temppath + "/xyzimage");

		//---zx_file------------------------------------------
		//QString zxtemp = "D:/temptemp";
		QString zxtemp = PicaApplication::getRoamingDir();
		QDir zx_dir(zxtemp);
		if (zx_dir.exists("zx_file"))
		{
			zx_dir.setPath(zxtemp + "/zx_file");
			//zx_dir.removeRecursively();
			zx_dir.setPath(zxtemp);
			zx_dir.mkpath("zx_file");
		}
		else
		{
			dir.setPath(temppath);
			dir.mkpath("zx_file");
		}

		zx_dir.setPath(zxtemp + "/zx_file");

		//==============================
		//**********************
		ctx.bindReadDrawFramebuffer(hFramebuffer);
		GLW_CHECK_GL_READ_DRAW_FRAMEBUFFER_STATUS;
#if 1

		const bool outputOL_Image = par.getBool("OL_Image");
		const bool save_cap_Image = par.getBool("Cap_Image");

		const bool faceColor = par.getBool("FaceColor");
		glClearColor(1, 1, 1, 1);
		QString olMName = md.p_setting.getOlMeshName();
		QString cMName = md.p_setting.getCapMeshName();

		//***opencvMat clear
		capCTemp.clear();
		outlineCTemp.clear();


		int zx_job_pages = par.getInt("per_zx_job");
		int start_page = par.getInt("start_page");
		int send_page_num = par.getInt("send_page_num");
		int plus_pixel = par.getInt("plus_pixel");
		float PLUS_MM = par.getFloat("PLUS_MM");
		bool start_print_b = par.getBool("PRINT_START");
		float useless_print = par.getFloat("USELESS_PRINT");
		float meCorrectPercent = 1;

		int widthrecord, heightrecord;
		QString blk_img_file_name = "black_outline";
		QString zx_sn = makeOutputname(zx_dir.absolutePath(), 0);

		md.jobname.push_back(zx_sn);

		//zggg = new ZxGen(unsigned int(width), unsigned int(height*meCorrectPercent + plus_pixel), zx_job_pages, md.groove.DimX(), md.groove.DimY() + PLUS_MM, zx_sn.toStdString(), start_print_b);//20161219_backup						 


		Log("ZxGen width %i height %i ", int(width), int(height*meCorrectPercent + plus_pixel));
		QTime time;
		//clock_t start, stop;
		time.start();

		//***20160622_test_progress bar initializing progress bar status
		if (cb != NULL)
		{
			glContext->doneCurrent();
			(*cb)(0, "processing...");
			glContext->makeCurrent();
		}

		QDir patternPath(PicaApplication::getRoamingDir());

		if (patternPath.exists("pattern"))
		{
		}
		else
		{
			patternPath.setPath(PicaApplication::getRoamingDir());
			patternPath.mkpath("pattern");
		}
		patternPath.setPath(PicaApplication::getRoamingDir() + "/pattern");



		float uselessPrintWidth = par.getFloat("USELESS_PRINT");


		double meanSumValueC = 0;
		double meanSumValueM = 0;
		double meanSumValueY = 0;

		double meanSumValueCside = 0;
		double meanSumValueMside = 0;
		double meanSumValueYside = 0;

		int printHeadNozzle = 14000;//***640*11*2 = 14080
		double printHeadLength = 22.2;//***640*11*2 = 14080
		int imageWidth = 2600;//***22.2/DSP_inchmm*300 = 2622
		int grooveMaxlayer = 2000;//***20cm
		int xyImageThick = 10;//***0.1mm * 10 layer
		int tenPixelNozel = printHeadNozzle * 10 / imageWidth;
		double inkDropVolumn = 1.5e-9;//ml


		jobServiceCount prejob(400, 800, 200, 800, 400);
		jobServiceCount midjob(400, 800, 200, 800, 400);
		jobServiceCount postjob(2400, 2800, 2200, 2800, 2400);

		//double dropsize = 1200 * 1e-12;
		double midJobDropDots = 14080000;




		int midJobFrequency = 8;



		double finalCusage = 0;
		double finalMusage = 0;
		double finalYusage = 0;

		foreach(MeshModel *mdmm, md.meshList)
		{
			if (mdmm->getMeshSort() == MeshModel::meshsort::print_item)
				if (mdmm->rmm.colorMode != GLW::CMNone || mdmm->rmm.textureMode != GLW::TMNone)//如果無色就不須做
				{
					for (int i = 0; i < 6; i++)
					{
						/*if (cb && (i % 1) == 0)
						{
						glContext->doneCurrent();
						bool break_test = cb((i * 100) / boxy_dim, "slice process");
						if (!break_test)
						{
						glContext->doneCurrent();
						return false;
						break;
						}

						}
						glContext->makeCurrent();
						*/
						time.restart();
						switch (i)
						{
						case 0://up
							glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
							glClearColor(1, 1, 1, 1);
							glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
							glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -100, 100);
							glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, 30, 0, 0, -30, 0, 1, 0);
							break;
						case 1://down
							glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
							glClearColor(1, 1, 1, 1);
							glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
							glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -100, 100);
							glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, -30, 0, 0, 30, 0, 1, 0);
							break;
							//side view...
						case 2:
							glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
							glClearColor(1, 1, 1, 1);
							glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
							glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -100, 100);
							glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(30, 0, 0, -30, 0, 0, 0, 1, 0);
							break;
						case 3:
							glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
							glClearColor(1, 1, 1, 1);
							glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
							glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -100, 100);
							glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(-30, 0, 0, 30, 0, 0, 0, 1, 0);
							break;

						case 4:
							glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
							glClearColor(1, 1, 1, 1);
							glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
							glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -100, 100);
							glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 30, 0, 0, -30, 0, 0, 0, 1);
							break;
						case 5:
							glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
							glClearColor(1, 1, 1, 1);
							glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
							glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -100, 100);
							glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, -30, 0, 0, 30, 0, 0, 0, 1);
							break;


						}


						//***20161112
						glPushMatrix();
						mdmm->render(GLW::DMSmooth, mdmm->rmm.colorMode, mdmm->rmm.textureMode);
						glPopMatrix();


						//***20150508***BMP QImage設為Format_RGBA8888, readpixel要設為GL_RGBA
						//glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image.bits());								
						glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, estimate_image[i]->bits());
						if (estimate_image[i])
						{
							cv::Scalar meanVal = SKT::meanCMY(QImageToCvMat(*estimate_image[i]));
							switch (i)
							{
							case 0:
							case 1:
							{
								meanSumValueC += meanVal.val[2];
								meanSumValueM += meanVal.val[1];
								meanSumValueY += meanVal.val[0];
							}
							break;
							case 2:
							case 3:
							case 4:
							case 5:
							{
								meanSumValueCside += meanVal.val[2];
								meanSumValueMside += meanVal.val[1];
								meanSumValueYside += meanVal.val[0];
							}
							break;

							}
						}




						glMatrixMode(GL_PROJECTION);
						glPopMatrix();
						glMatrixMode(GL_MODELVIEW);
						glPopMatrix();

						QString estiamte_image = "estiamte_image";
						//if (outputOL_Image)//彩色輪廓
						if (true)//彩色輪廓
						{
							estiamte_image.append(QString("abb_%1.png").arg(i, 4, 10, QChar('0')));
							estimate_image[i]->mirrored().save(dir.absolutePath() + "/" + estiamte_image, "png");
						}
					}




					//===========================================================================================

					//***20160112***image_process加上插點pattern
					if (par.getBool("generate_zx"))
					{
						//SKT::InkBalance imageProcess;
						//cv::Mat temp, temp2;					
						//
						//if (par.getBool("mono_bool"))originalOutline = cv::Scalar(255, 255, 255);
						////xy_axis now is fine
						////originalOutline = SKT::resizeXAxis(&originalOutline, meCorrectPercent);//****0.97
						//originalOutline = SKT::resizeIamge(&originalOutline, plus_pixel);
						//
						//bool longDin = false;
						//SKT::generateUselessbarDin(originalOutline, useless_print, longDin);
						//outlineCTemp.push_back(originalOutline);//**********PUSH_BACK_OUTLINE********************************

						/////////cap_temp_change_add_pattern///////////////////
						//black_Mask = SKT::erodeImage(&black_Mask, 51);								

					}

					if (par.getBool(("generate_final_picture")))
					{
						////picasso/////////////////////////////////////////////////
						//if (i<50)
						//{
						//	QImage ttt(cvMatToQImage(outlineCTemp.back()));
						//	//QImage ttt(cvMatToQImage(originalOutline));
						//	QString tttt = "tttt";
						//	tttt.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
						//	ttt.save(dir.absolutePath() + "/" + tttt, "png");
						//	qDebug() << "dir.absolutePath()" << dir.absolutePath();										
						//	///////////////////////////
						//}
					}

					//*****test_thread
					/*emit test_update_pages(i);
					thread1.start();*/

				}


		}
		Log("meanSumValueC  %f", meanSumValueC);
		Log("meanSumValueM  %f", meanSumValueM);
		Log("meanSumValueY  %f", meanSumValueY);

		Log("meanSumValueCside  %f", meanSumValueCside);
		Log("meanSumValueMside  %f", meanSumValueMside);
		Log("meanSumValueYside  %f", meanSumValueYside);

		//assume layer_height is 0.01cm
		double topDownColorCEstimate = printHeadNozzle * printHeadNozzle * xyImageThick * (meanSumValueC / 256.) * inkDropVolumn;
		double topDownColorMEstimate = printHeadNozzle * printHeadNozzle * xyImageThick * (meanSumValueM / 256.) * inkDropVolumn;
		double topDownColorYEstimate = printHeadNozzle * printHeadNozzle * xyImageThick * (meanSumValueY / 256.) * inkDropVolumn;

		double sideColorCEstimate = printHeadNozzle * tenPixelNozel * grooveMaxlayer * (meanSumValueCside / 256.) * inkDropVolumn;
		double sideColorMEstimate = printHeadNozzle * tenPixelNozel * grooveMaxlayer * (meanSumValueMside / 256.) * inkDropVolumn;
		double sideColorYEstimate = printHeadNozzle * tenPixelNozel * grooveMaxlayer * (meanSumValueYside / 256.) * inkDropVolumn;

		//uselessBinderUsage
		double uselessPrintUsage = printHeadNozzle * (printHeadNozzle*uselessPrintWidth / printHeadLength) * boxy_dim * inkDropVolumn;

		finalCusage = topDownColorCEstimate + sideColorCEstimate + uselessPrintUsage + prejob.maintUsage(jobServiceCount::inktype::c);
		finalMusage = topDownColorMEstimate + sideColorMEstimate + uselessPrintUsage + prejob.maintUsage(jobServiceCount::inktype::m);
		finalYusage = topDownColorYEstimate + sideColorYEstimate + uselessPrintUsage + prejob.maintUsage(jobServiceCount::inktype::y);


		Log("finalCusage  %f", finalCusage);
		Log("finalMusage  %f", finalMusage);
		Log("finalYusage  %f", finalYusage);




		par.addParam(new RichFloat("CUsage", finalCusage, "", ""));
		par.addParam(new RichFloat("MUsage", finalMusage, "", ""));
		par.addParam(new RichFloat("YUsage", finalYusage, "", ""));





#endif	

		ctx.unbindReadDrawFramebuffer();
		ctx.release();
		glPopAttrib();
		glContext->doneCurrent();

		foreach(MeshModel *bm, md.meshList)
		{
			if ((bm->label().contains(olMName, Qt::CaseSensitive) || bm->label().contains(cMName, Qt::CaseSensitive)))
				//if (bm->label().contains(olMName, Qt::CaseSensitive) || bm->label().contains(cMName, Qt::CaseSensitive)))
				//if (bm->getMeshSort()==MeshModel::meshsort::)
			{
				md.delMesh(bm);
			}
		}



		qDebug() << " done _applyClick ";
		break;
	}
#pragma endregion FP_ESTIMATE_COLOR

#pragma region FP_PRINT_TEST_PAGE
	case FP_PRINT_TEST_PAGE:
	{

		//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("test_page_start"));
		md.jobname.clear();
		//***************************

		/*glContext->doneCurrent();
		glContext->moveToThread(th1);*/

		glContext->makeCurrent();

		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			/* Problem: glewInit failed, something is seriously wrong. */
			//fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
			//qDebug("Error: %s\n", glewGetErrorString(err));
		}

		glPushAttrib(GL_ALL_ATTRIB_BITS);

		Context ctx;
		ctx.acquire();



		//***20150907							
		float x = md.groove.DimX();
		float y = md.groove.DimY();
		const GLsizei width = (int)(x / DSP_inchmm * DSP_Look_Distination);
		const GLsizei height = (int)(width * y / x);


		//const vcg::Point3f lightDirectionVS = vcg::Point3f(0.0f, 0.0f, -1.0f).Normalize();

		//glEnable(GL_DEPTH_TEST);
		//glClearColor(GLfloat(backgroundColor.red())/255.0f, GLfloat(backgroundColor.green())/255.0f, GLfloat(backgroundColor.blue())/255.0f, 0.0f);
		Construction_Groove<float> tempgroove = md.groove;

		//glViewport(0, 0, width, height);//***window的長寬				  



		//glScalef(scale, scale, scale);
		//glTranslatef(-center[0], -center[1], -center[2]);

		//***儲存image的基本資訊
		/*QImage image(int(width), int(height), QImage::Format_RGB888);
		QImage image2(int(width), int(height), QImage::Format_RGB888);
		QImage slice0Point05mmImage(int(width), int(height), QImage::Format_RGB888);
		QImage cap_image(int(width), int(height), QImage::Format_RGB888);*/

		//***20150505***slice_func_設置高度
		float box_bottom = md.bbox().min.Z();
		float unit = par.getFloat("slice_height");



		int boxy_dim = qCeil((md.bbox().max.Z() - md.bbox().min.Z()) / unit) + 1;


		//init_texture(md);

		//*******************					  

		//***20150507 standard path******************
		//QString temppath = QStandardPaths::locate(QStandardPaths::TempLocation, "", QStandardPaths::LocateDirectory);
		//QString temppath = "D:/temptemp";//backup
		QString temppath = PicaApplication::getRoamingDir();
		QDir dir(temppath);//backup
		if (dir.exists("xyzimage"))
		{
			dir.setPath(temppath + "/xyzimage");
			dir.removeRecursively();
			dir.setPath(temppath);
			dir.mkpath("xyzimage");
		}
		else
		{
			dir.setPath(temppath);
			dir.mkpath("xyzimage");
		}

		dir.setPath(temppath + "/xyzimage");

		//---zx_file------------------------------------------
		//QString zxtemp = "D:/temptemp";
		QString zxtemp = PicaApplication::getRoamingDir();
		QDir zx_dir(zxtemp);
		if (zx_dir.exists("zx_file"))
		{
			zx_dir.setPath(zxtemp + "/zx_file");
			//zx_dir.removeRecursively();
			zx_dir.setPath(zxtemp);
			zx_dir.mkpath("zx_file");
		}
		else
		{
			dir.setPath(temppath);
			dir.mkpath("zx_file");
		}

		zx_dir.setPath(zxtemp + "/zx_file");

		//==============================
		//**********************
		GLW_CHECK_GL_READ_DRAW_FRAMEBUFFER_STATUS;


		const bool outputOL_Image = par.getBool("OL_Image");
		const bool save_cap_Image = par.getBool("Cap_Image");

		const bool faceColor = par.getBool("FaceColor");
		glClearColor(1, 1, 1, 1);
		QString olMName = md.p_setting.getOlMeshName();
		QString cMName = md.p_setting.getCapMeshName();

		//***opencvMat clear
		capCTemp.clear();
		outlineCTemp.clear();


		int zx_job_pages = par.getInt("per_zx_job");
		int start_page = par.getInt("start_page");
		int send_page_num = par.getInt("send_page_num");
		int plus_pixel = par.getInt("plus_pixel");
		float plus_MM = par.getFloat("PLUS_MM");
		bool start_print_b = par.getBool("PRINT_START");
		float useless_print = par.getFloat("USELESS_PRINT");
		bool add_pattern = par.getBool("ADD_PATTERN");
		bool plus_Spitton = par.getBool("PLUS_SPITTON");

		//float meCorrectPercent = 0.97;
		float meCorrectPercent = 1;



		int widthrecord, heightrecord;
		QString blk_img_file_name = "black_outline";
		QString zx_sn = makeOutputname(zx_dir.absolutePath(), 0);

		md.jobname.push_back(zx_sn);


		//SKT::pixelFormat gen_pixelFormat(SKT::pixelFormat::CMY);
		ZxGen_SKT::pixelFormat genZxPixelFormat(ZxGen_SKT::pixelFormat::RGB);
		//ZxGen_SKT::zxInitMode zxCheckMode(ZxGen_SKT::zxInitMode::ViewCheckMode);
		ZxGen_SKT::zxInitMode zxCheckMode(ZxGen_SKT::zxInitMode::RealPrintMode);



		zx_skt = new ZxGen_SKT(300, zx_job_pages, zx_sn.toStdString(), start_print_b, genZxPixelFormat, zxCheckMode);//20161219_backup						 

		std::vector<cv::Mat> blankTemp;
		std::vector<cv::Mat> blankcapCTemp;
		blankTemp.clear();
		blankcapCTemp.clear();


		//SKT::createTestPrintPages(tempgroove.grooveheight, 20/*md.p_setting.get_plus_print_length()*/ ,tempgroove.groovelength, useless_print, 5, blankTemp, blankcapCTemp);

		SKT::createTestPrintPagesK(tempgroove.grooveheight, plus_MM, tempgroove.groovelength, useless_print, 5, blankTemp, blankcapCTemp, plus_Spitton);



		QString img_file_name2 = "printtestpage";
		QImage cap_Test = cvMatToQImage(blankTemp[0]);
		img_file_name2.append(QString("_%1.png").arg(0, 4, 10, QChar('0')));
		cap_Test.save(dir.absolutePath() + "/" + img_file_name2, "png");



		zx_skt->sendData3(blankTemp, blankcapCTemp);




		//zx_skt->sendData3(blankTemp, blankcapCTemp);



		//Sleep(10000);



		QTime time;
		//clock_t start, stop;
		time.start();


		bool savedebugImage = false;
		int testPrintPageCount = 100;
		bool testswitch = false;
		int generateNOI = 50;


		glContext->doneCurrent();


		qDebug() << " done _applyClick ";
		zx_skt->close();
		zx_skt->~ZxGen_SKT();
		//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("test_page_end"));
		break;
	}
#pragma endregion FP_PRINT_TEST_PAGE
#pragma region FP_PRINT_INTERSECT_TRIANGLE
	case FP_PRINT_INTERSECT_TRIANGLE:
	{
		CMeshO::FaceIterator fi, fia;
		//std::vector<CFaceO*>::iterator fib;
		md.multiSelectID.clear();
		int count = 0;

		/*foreach(MeshModel *mdmm, md.meshList)
		{
		if (!mdmm->getMeshSort() == MeshModel::meshsort::print_item)continue;
		int m1id = mdmm->id();
		bool flag = false;
		foreach(MeshModel *mdmm2, md.meshList)
		{
		int mdmm2ID = mdmm2->id();
		if (mdmm2ID>m1id && mdmm2->getMeshSort() == MeshModel::meshsort::print_item)
		if (mdmm2->cm.bbox.Collide(mdmm->cm.bbox) || mdmm2->cm.bbox.isInside(mdmm->cm.bbox) || mdmm->cm.bbox.isInside(mdmm2->cm.bbox))
		{



		}

		}



		}*/

		/*foreach(MeshModel *mdmm, md.meshList)
		{
		if (!mdmm->getMeshSort() == MeshModel::meshsort::print_item)continue;
		int m1id = mdmm->id();
		bool flag = false;
		for (fi = mdmm->cm.face.begin(); fi != mdmm->cm.face.end(); ++fi)
		{
		if (flag){
		flag = false;
		break;
		}
		foreach(MeshModel *mdmm2, md.meshList)
		{

		int mdmm2ID = mdmm2->id();
		if (mdmm2ID > m1id && mdmm2->getMeshSort() == MeshModel::meshsort::print_item && (mdmm2->cm.bbox.Collide(mdmm->cm.bbox) || mdmm2->cm.bbox.isInside(mdmm->cm.bbox) || mdmm->cm.bbox.isInside(mdmm2->cm.bbox)))
		{
		for (fia = mdmm2->cm.face.begin(); fia != mdmm2->cm.face.end(); ++fia)
		{
		//if (fi == fia)continue;
		bool x = vcg::tri::Clean<CMeshO>::TestFaceFaceIntersection(&*fi, &*fia);
		count++;
		if (x)
		{
		//qDebug() << "face intersection" << x;
		md.multiSelectID.insert(m1id);
		md.multiSelectID.insert(mdmm2->id());
		//fia->V(0)->C() = vcg::Color4b(255, 0, 0, 0);
		//fia->C() = vcg::Color4b(255, 0, 0, 0);
		flag = true;
		break;
		}
		}
		}

		}
		}
		}*/
		//CMeshO::FaceIterator fi, fia;
		foreach(MeshModel *mdmm, md.meshList)
		{
			int count = 0;
			if (!mdmm->getMeshSort() == MeshModel::meshsort::print_item)continue;
			else{
				mdmm->Enable(vcg::tri::io::Mask::IOM_FACECOLOR);
				mdmm->cm.face.EnableFFAdjacency();
				//mdmm->cm.face.Enable();
				//vcg::
				//RequireFFAdjacency(mdmm->cm);
			}

			size_t sefaces = faceconnect(*mdmm);


			for (fi = mdmm->cm.face.begin(); fi != mdmm->cm.face.end(); ++fi)
			{
				count++;
				vcg::Point3f x = fi->N();
				/*if (fi->IsS())
				{
				fi->C() = vcg::Color4b(255, 0, 0, 0);
				}*/
				/*if (x.dot(vcg::Point3f(0, 0, 1)) < 0.)
				{*/
				fi->C() = vcg::Color4b(255, 0, 0, 0);
				/*vcg::Point3f tempsave = fi->V(0)->P();
				fi->V(0) = fi->V(2);
				fi->V(2)->P() = tempsave;*/


				for (int i = 0; i < fi->VN(); ++i) {
					CMeshO::FaceType *ff = fi->FFp(i);
				}

				CMeshO::FaceType *zz = fi->FFp(0);
				//CMeshO::EdgeType *ee 
				int ee = fi->FFi(0);
				fi->IsF(0);
				fi->V0(0);
				CEdgeO::EdgeType *A_edge = fi->FEp(0);
				//B_edge->P0(0);

				//yy->V(1)->IsV();
				vcg::face::SwapEdge<CMeshO::FaceType, false>((*fi), 0);
				if (HasPerWedgeTexCoord(mdmm->cm))
					std::swap((*fi).WT(0), (*fi).WT(1));

				mdmm->UpdateBoxAndNormals();
				mdmm->clearDataMask(MeshModel::MM_FACEFACETOPO);

				//}

				//for (int j = 0; j < 3; j++)
				//{

				//	if (vcg::face::IsBorder(*fi, j))
				//	{

				//		vcg::face::Pos<CMeshO::FaceType> hei(&*fi, j, fi->V(j));
				//		glColor3f(1, 0, 0);
				//		glEnable(GL_LINE_SMOOTH);
				//		glLineWidth(5);
				//		glBegin(GL_LINES);
				//		glVertex(hei.V()->P());
				//		hei.FlipV();
				//		glVertex(hei.V()->P());
				//		//glVertex(fi->V(j)->P());
				//		//glVertex(fi->V(++j % 3)->P());
				//		glEnd();
				//	}
				//}

			}
			//tri::UpdateNormal<CMeshO>::PerVertex(mdmm->cm);
		}

	}break;
#pragma endregion FP_PRINT_INTERSECT_TRIANGLE
#pragma region FP_MESH_INTERSECT_TEST
	case FP_MESH_INTERSECT_TEST:
	{
		WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("FP_MESH_INTERSECT_TEST"));


		md.jobname.clear();
		//***************************

		glContext->makeCurrent();


		GLenum err = glewInit();
		if (GLEW_OK != err)
		{

		}

		glPushAttrib(GL_ALL_ATTRIB_BITS);

		Context ctx;
		ctx.acquire();

		if (switchInitMeshVBO)
			initMeshVBO(md);
		QSet<MeshModel *> overlappingMeshes;

		for (QList<MeshModel *>::const_iterator ii = md.meshList.constBegin(); ii != md.meshList.constEnd(); ii++)
		{
			QList<MeshModel *>::const_iterator i2 = ii;
			i2++;
			for (; i2 != md.meshList.constEnd(); i2++)
			{
				if (md.isPrint_item(**ii) && md.isPrint_item(**i2))
					if ((*ii)->cm.bbox.Collide((*i2)->cm.bbox))
					{
						overlappingMeshes.insert((*ii));
						overlappingMeshes.insert((*i2));
					}

			}

		}
		if (overlappingMeshes.size() == 0)return true;
		Box3m FullBBox;
		Matrix44m iden;
		iden.SetIdentity();
		foreach(MeshModel *mp, overlappingMeshes)
		{
			FullBBox.Add(iden, mp->cm.bbox);
		}




		//***20150907
		const GLsizei print_dpi = 25;// GLsizei(par.getInt("print_dpi"));
		float x = FullBBox.DimX();
		float y = FullBBox.DimY();
		const GLsizei width = (int)(x / DSP_inchmm * print_dpi);
		const GLsizei height = (int)(width * y / x);


		RenderbufferHandle hDepth = createRenderbuffer(ctx, GL_DEPTH24_STENCIL8, width, height);
		Texture2DHandle    hColor = createTexture2D(ctx, GL_RGB8, width, height, GL_RGB, GL_UNSIGNED_BYTE);
		//FramebufferHandle  hFramebuffer = createFramebuffer(ctx, renderbufferTarget(hDepth), texture2DTarget(hColor));
		FramebufferHandle  hFramebuffer = createFramebufferWithDepthStencil(ctx, renderbufferTarget(hDepth), renderbufferTarget(hDepth), texture2DTarget(hColor));




		const vcg::Point3f lightDirectionVS = vcg::Point3f(0.0f, 0.0f, -1.0f).Normalize();

		glEnable(GL_DEPTH_TEST);
		//glClearColor(GLfloat(backgroundColor.red())/255.0f, GLfloat(backgroundColor.green())/255.0f, GLfloat(backgroundColor.blue())/255.0f, 0.0f);


		glViewport(0, 0, width, height);//***window的長寬				  

		//const vcg::Point3f center = mesh.bbox.Center();
		//const float        scale = 1.0f / mesh.bbox.Diag();

		//glScalef(scale, scale, scale);
		//glTranslatef(-center[0], -center[1], -center[2]);

		//***儲存image的基本資訊
		QImage image(int(width), int(height), QImage::Format_RGB888);
		QImage image2(int(width), int(height), QImage::Format_RGB888);
		QImage slice0Point05mmImage(int(width), int(height), QImage::Format_RGB888);
		QImage cap_image(int(width), int(height), QImage::Format_RGB888);

		//***20150505***slice_func_設置高度

		init_texture(md);

		//*******************

		QString temppath = PicaApplication::getRoamingDir();
		QDir dir(temppath);//backup
		if (dir.exists("xyzimage"))
		{
			dir.setPath(temppath + "/xyzimage");
			dir.removeRecursively();
			dir.setPath(temppath);
			dir.mkpath("xyzimage");
		}
		else
		{
			dir.setPath(temppath);
			dir.mkpath("xyzimage");
		}

		dir.setPath(temppath + "/xyzimage");

		//==============================
		//**********************
		ctx.bindReadDrawFramebuffer(hFramebuffer);
		GLW_CHECK_GL_READ_DRAW_FRAMEBUFFER_STATUS;
#if 1


		glClearColor(1, 1, 1, 1);
		QString olMName = md.p_setting.getOlMeshName();
		QString cMName = md.p_setting.getCapMeshName();

		//***opencvMat clear
		capCTemp.clear();
		outlineCTemp.clear();


		float meCorrectPercent = 1;


		int widthrecord, heightrecord;
		QString blk_img_file_name = "black_outline";


		std::vector<cv::Mat> blankTemp;
		std::vector<cv::Mat> blankcapCTemp;

		QTime time;
		//clock_t start, stop;
		time.start();

		QDir patternPath(PicaApplication::getRoamingDir());




		bool testswitch = false;
		int generateNOI = 20;
		int quickWipe = 1;
		bool savedebugImage = false;// par.getBool(("generate_final_picture"));


		float unit = 2;
		int boxy_dim = qCeil((FullBBox.max.Z() - FullBBox.min.Z()) / unit) + 1;



		float box_bottom = FullBBox.min.Z();
		vector<std::map<int, cv::Mat>> savefloorMeshImage;
		int start_page = 0;// par.getInt("start_page");
		//Construction_Groove<float> tempgroove(FullBBox.DimX(), FullBBox.DimZ(), FullBBox.DimY());// = md.groove;
		float orthox = FullBBox.DimX() / 2.;
		float orthoy = FullBBox.DimY() / 2.;

		for (int i = start_page, j = start_page + 1; i < boxy_dim; i++, j++)
			//for (int i = start_page, j = start_page + 1; i < start_page+testPrintPageCount; i++, j++)
		{
			md.wipeFlag.push_back(true);
			glContext->makeCurrent();
			time.restart();

			/*  glMatrixMode(GL_PROJECTION); glPopMatrix();
			glMatrixMode(GL_MODELVIEW); glPopMatrix();
			glDisable(GL_LIGHTING);*/
			//===========================================================================================
			equ[3] = box_bottom + unit*i;
			//***第二張圖***產生膠水圖**********//
			//time.restart();
			//vector<cv::Mat> temp_capImage;
			std::map<int, cv::Mat> temp_capImage;
			//for (int count = 0; count < md.meshList.size(); count++)
			int countj = 0;
			//foreach(MeshModel *floorMesh, md.meshList)
			foreach(MeshModel *floorMesh, overlappingMeshes)
			{
				if (floorMesh->getMeshSort() != MeshModel::meshsort::print_item)continue;

				glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
				//glOrtho(FullBBox.min.X(), FullBBox.max.X(), FullBBox.min.Y(), FullBBox.max.Y(), -1000, 1000);
				glOrtho(-orthox, orthox, -orthoy, orthoy, -1000, 1000);
				glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
				vcg::Point3f x = FullBBox.Center();
				gluLookAt(FullBBox.Center().X(), FullBBox.Center().Y(), DSP_Look_Distination, FullBBox.Center().X(), FullBBox.Center().Y(), -DSP_Look_Distination, 0, 1, 0);
				//gluLookAt(-80, 0, DSP_Look_Distination,-80, 0, -DSP_Look_Distination, 0, 1, 0);
				//*********************************
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
				glClearColor(1, 1, 1, 1);

				//binderstrips(md, par.getFloat("useless_print"));//***uselessbar

				//************************************************//
				//****** Rendering the mesh's clip edge ****//
				GLuint MY_CLIP_PLANE = GL_CLIP_PLANE0;
				glEnable(MY_CLIP_PLANE);
				glClipPlane(MY_CLIP_PLANE, equ);

				glEnable(GL_STENCIL_TEST);
				glEnable(GL_CULL_FACE);
				glClear(GL_STENCIL_BUFFER_BIT);
				glDisable(GL_DEPTH_TEST);
				glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

				//// first pass: increment stencil buffer value on back faces
				glStencilFunc(GL_ALWAYS, 0, 0);
				glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
				glCullFace(GL_FRONT); // render back faces only
				glPushAttrib(GL_ALL_ATTRIB_BITS);
				glEnable(GL_LIGHTING);
				glPushMatrix();

				//glRotatef(90, 1, 0, 0);
				//foreach(MeshModel * mp, md.meshList)
				//{
				// if (mp->getMeshSort() == MeshModel::meshsort::print_item)
				//  //mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
				//  mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
				//}										  
				floorMesh->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);

				glPopMatrix();
				glPopAttrib();
				//second pass: decrement stencil buffer value on front faces
				glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
				glCullFace(GL_BACK); // render front faces only
				glPushAttrib(GL_ALL_ATTRIB_BITS);
				glEnable(GL_LIGHTING);
				glPushMatrix();

				//glRotatef(90, 1, 0, 0);
				//foreach(MeshModel * mp, md.meshList)
				//{
				// if (mp->getMeshSort() == MeshModel::meshsort::print_item)
				//  //mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
				//  mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
				//}

				floorMesh->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);

				glPopMatrix();
				glPopAttrib();
				//-----------------------------
				// drawing clip planes masked by stencil buffer content
				glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
				glEnable(GL_DEPTH_TEST);
				glDisable(MY_CLIP_PLANE);
				glStencilFunc(GL_NOTEQUAL, 0, ~0);
				// stencil test will pass only when stencil buffer value = 0; 
				// (~0 = 0x11...11)

				glColor4f(1, 1, 1, 1);
				fillsquare();
				glDisable(GL_STENCIL_TEST);
				glEnable(MY_CLIP_PLANE); // enabling clip plane again
				glDisable(GL_LIGHTING);

				glMatrixMode(GL_MODELVIEW);
				glPopMatrix();
				glMatrixMode(GL_PROJECTION);
				glPopMatrix();
				//********************************************************************************************//


				glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, cap_image.bits());//***20150724
				//glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, cap_image.bits());							  
				QString img_file_name2 = md.p_setting.getcapImageName();
				glDisable(MY_CLIP_PLANE);//存完圖
				glDisable(GL_CULL_FACE);
				//****
				//(md.logger, framework::Diagnostics::LogLevel::Info, _T(""));
				cv::Mat captemp;
				//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T(""));

				cv::cvtColor(QImageToCvMat(cap_image.mirrored()), captemp, CV_RGB2GRAY);
				temp_capImage.insert(std::pair<int, cv::Mat>(floorMesh->id(), captemp));

				if (savedebugImage){
					QImage testttt = cvMatToQImage(captemp);
					QString capimagest = "captemp_image";
					capimagest.append(QString("_%1_%2.png").arg(i, 4, 10, QChar('0')).arg(countj, 4, 10, QChar('0')));
					testttt.save(dir.absolutePath() + "/" + capimagest, "png");
					countj++;
				}

				//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T(""));

				glMatrixMode(GL_MODELVIEW);
				glPopMatrix();
				glMatrixMode(GL_PROJECTION);
				glPopMatrix();


			}
			savefloorMeshImage.push_back(temp_capImage);

		}
		//std::multimap<int, int> * tempIntersectMeshesID = md.getIntersectMeshesID();								  
		md.setIntersectMeshesID2(SKT::checkImageOverlap(savefloorMeshImage));

#endif	

		ctx.unbindReadDrawFramebuffer();
		ctx.release();
		glPopAttrib();
		glContext->doneCurrent();




		foreach(MeshModel *bm, md.meshList)
		{
			if ((bm->label().contains(olMName, Qt::CaseSensitive) || bm->label().contains(cMName, Qt::CaseSensitive)))
				//if (bm->label().contains(olMName, Qt::CaseSensitive) || bm->label().contains(cMName, Qt::CaseSensitive)))
				//if (bm->getMeshSort()==MeshModel::meshsort::)
			{
				md.delMesh(bm);
			}
		}
		qDebug() << " done _applyClick ";


	}break;
#pragma endregion FP_MESH_INTERSECT_TEST
#pragma region FP_TEST_AUTOPACK_V2
	case FP_TEST_AUTOPACK_V2:
	{
		//get Mesh box min
		QMap<int, float> mesh_boxXmin_map;//
		foreach(MeshModel *mp, md.meshList)
		{
			if (md.isPrint_item(*mp))
			{
				mesh_boxXmin_map.insert(mp->id(), mp->cm.bbox.min.X());
			}
		}

		vector<MeshModel *> m_record;
		int i = 0;
		foreach(MeshModel *mp, md.meshList)
		{
			if (md.isPrint_item(*mp))
			{
				m_record.push_back(mp);

			}
		}
		//vector<std::map<int, cv::Mat>> layerImage;

		std::map<int, std::vector<cv::Mat> *> testSaveImage;


		//***bubble sort the bounding box
		QVector<MeshModel *> meshVector = QVector<MeshModel *>::fromList(md.meshList);
		SKT::sortMesh(meshVector);


		foreach(MeshModel *mm, meshVector)
		{
			qDebug() << mm->cm.bbox.min.X() << " : " << mm->cm.bbox.min.Y() << " : " << mm->cm.bbox.min.Z();
		}

		//getMeshLayerImage(md.meshList, testSaveImage);
		getMeshLayerImagePlaneX(md.meshList, testSaveImage);



		/////////////////////////////////////////////////////////////////////////
		//***XAxis
		//////////////////////////////////////////////////////////////////////////
		//std::vector<cv::Mat> combineImage;								
		//for (int x = 0; x < (*(testSaveImage.at(meshVector[0]->id()))).size(); x++)
		//{
		//	int firstMesh = meshVector[0]->id();
		//	cv::Mat temp;
		//	(*(testSaveImage.at(firstMesh)))[x].copyTo(temp);
		//	combineImage.push_back(temp);
		//}	
		//for (int i = 0; i < meshVector.size()-1; i++)
		//{
		//	int imageAxis = 1;
		//	int secondmesh = meshVector[i + 1]->id();
		//	int moveOffsetInt = SKT::smallestOffsetfromM1M2(combineImage, *(testSaveImage.at(secondmesh)), imageAxis, combineImage);
		//	if (moveOffsetInt > 4)
		//		moveOffsetInt -= 4;
		//	else moveOffsetInt = 0;
		//	//else moveOffsetInt -= 3;
		//	QString temppath = PicaApplication::getRoamingDir();
		//	QDir dir(temppath);//backup
		//	dir.setPath(temppath + "/xyzimage");
		//	for (int j = 0; j < combineImage.size(); j++)
		//	{
		//		SKT::mergeImage(combineImage.at(j), (*(testSaveImage.at(secondmesh))).at(j), 0, moveOffsetInt);
		//		if (true){
		//			QImage testttt = cvMatToQImage(combineImage.at(j));
		//			QString capimagest = "captemp_image";
		//			capimagest.append(QString("_%1_combine.png").arg(j, 4, 10, QChar('0')));
		//			testttt.save(dir.absolutePath() + "/" + capimagest, "png");
		//			QImage testttt2 = cvMatToQImage((*(testSaveImage.at(secondmesh))).at(j));
		//			QString capimagest2 = "moved_secondImage";
		//			capimagest2.append(QString("_%1_.png").arg(j, 4, 10, QChar('0')));
		//			testttt2.save(dir.absolutePath() + "/" + capimagest2, "png");
		//		}
		//	}
		//	double moveOffset = SKT::pixelToMm(moveOffsetInt, 25);
		//	vcg::Point3f Offset;
		//	if (imageAxis == int(genPF_X_Axis))
		//		Offset = vcg::Point3f(-moveOffset, 0, 0);
		//	else if (imageAxis == int(genPF_Y_Axis))
		//		Offset = vcg::Point3f(0, moveOffset, 0);
		//	else if (imageAxis == int(genPF_Z_Axis))
		//		Offset = vcg::Point3f(0, 0, -moveOffset);
		//	Matrix44m tranlate_to;
		//	tranlate_to.SetTranslate(Offset);
		//	meshVector[i + 1]->cm.Tr = tranlate_to *meshVector[i + 1]->cm.Tr;
		//	//*********end*************
		//	tri::UpdatePosition<CMeshO>::Matrix(meshVector[i + 1]->cm, tranlate_to, true);
		//	tri::UpdateBounding<CMeshO>::Box(meshVector[i + 1]->cm);
		//}							

		///////////////////////////////////////////////////////////////////////////
		////***YAxis
		////////////////////////////////////////////////////////////////////////////
		//std::vector<cv::Mat> combineImage2;								
		//for (int x = 0; x < (*(testSaveImage.at(meshVector[0]->id()))).size(); x++)
		//{
		//	int firstMesh = meshVector[0]->id();
		//	cv::Mat temp;
		//	(*(testSaveImage.at(firstMesh)))[x].copyTo(temp);
		//	combineImage2.push_back(temp);
		//}
		//for (int i = 0; i < meshVector.size() - 1; i++)
		//{
		//	int imageAxis = 0;
		//	int secondmesh = meshVector[i + 1]->id();
		//	int moveOffsetInt = SKT::smallestOffsetfromM1M2(combineImage2, *(testSaveImage.at(secondmesh)), imageAxis, combineImage2);
		//	if (moveOffsetInt > 4)
		//		moveOffsetInt -= 4;
		//	else moveOffsetInt = 0;
		//	//else moveOffsetInt -= 3;
		//	QString temppath = PicaApplication::getRoamingDir();
		//	QDir dir(temppath);//backup
		//	dir.setPath(temppath + "/xyzimage");
		//	for (int j = 0; j < combineImage2.size(); j++)
		//	{
		//		SKT::mergeImage(combineImage2.at(j), (*(testSaveImage.at(secondmesh))).at(j), moveOffsetInt, 0);
		//		if (true){
		//			QImage testttt = cvMatToQImage(combineImage2.at(j));
		//			//QImage testttt = cvMatToQImage((*(testSaveImage.at(meshVector[0]->id())))[j]);
		//			QString capimagest = "captemp_image";
		//			capimagest.append(QString("_%1_combine2.png").arg(j, 4, 10, QChar('0')));
		//			testttt.save(dir.absolutePath() + "/" + capimagest, "png");
		//			QImage testttt2 = cvMatToQImage((*(testSaveImage.at(secondmesh))).at(j));
		//			QString capimagest2 = "moved_secondImage";
		//			capimagest2.append(QString("_%1_.png").arg(j, 4, 10, QChar('0')));
		//			testttt2.save(dir.absolutePath() + "/" + capimagest2, "png");
		//		}
		//	}
		//	double moveOffset = SKT::pixelToMm(moveOffsetInt, 25);
		//	vcg::Point3f Offset;
		//	if (imageAxis == int(genPF_X_Axis))
		//		Offset = vcg::Point3f(-moveOffset, 0, 0);
		//	else if (imageAxis == int(genPF_Y_Axis))
		//		Offset = vcg::Point3f(0, moveOffset, 0);
		//	else if (imageAxis == int(genPF_Z_Axis))
		//		Offset = vcg::Point3f(0, 0, -moveOffset);
		//	Matrix44m tranlate_to;
		//	tranlate_to.SetTranslate(Offset);
		//	meshVector[i + 1]->cm.Tr = tranlate_to *meshVector[i + 1]->cm.Tr;
		//	//*********end*************
		//	tri::UpdatePosition<CMeshO>::Matrix(meshVector[i + 1]->cm, tranlate_to, true);
		//	tri::UpdateBounding<CMeshO>::Box(meshVector[i + 1]->cm);
		//}


		/////////////////////////////////////////////////////////////////////////
		//***ZAxis
		//////////////////////////////////////////////////////////////////////////
		std::vector<cv::Mat> combineImage2;
		//combineImage2.assign((*(testSaveImage.at(meshVector[0]->id()))).begin(), (*(testSaveImage.at(meshVector[0]->id()))).end());
		for (int x = 0; x < (*(testSaveImage.at(meshVector[0]->id()))).size(); x++)
		{
			int firstMesh = meshVector[0]->id();
			cv::Mat temp;
			(*(testSaveImage.at(firstMesh)))[x].copyTo(temp);
			combineImage2.push_back(temp);
		}


		for (int i = 0; i < meshVector.size() - 1; i++)
		{
			int imageAxis = 2;
			int secondmesh = meshVector[i + 1]->id();
			int moveOffsetInt = SKT::smallestOffsetfromM1M2(combineImage2, *(testSaveImage.at(secondmesh)), imageAxis, combineImage2);
			if (moveOffsetInt > 3)
				moveOffsetInt -= 3;
			else moveOffsetInt = 0;
			//else moveOffsetInt -= 3;


			QString temppath = PicaApplication::getRoamingDir();
			QDir dir(temppath);//backup
			dir.setPath(temppath + "/xyzimage");

			for (int j = 0; j < combineImage2.size(); j++)
			{
				SKT::mergeImage(combineImage2.at(j), (*(testSaveImage.at(secondmesh))).at(j), 0, moveOffsetInt);

				if (true){
					QImage testttt = cvMatToQImage(combineImage2.at(j));
					//QImage testttt = cvMatToQImage((*(testSaveImage.at(meshVector[0]->id())))[j]);
					QString capimagest = "captemp_image";
					capimagest.append(QString("_%1_combine2.png").arg(j, 4, 10, QChar('0')));
					testttt.save(dir.absolutePath() + "/" + capimagest, "png");

					/*QImage testttt2 = cvMatToQImage((*(testSaveImage.at(secondmesh))).at(j));
					QString capimagest2 = "moved_secondImage";
					capimagest2.append(QString("_%1_.png").arg(j, 4, 10, QChar('0')));
					testttt2.save(dir.absolutePath() + "/" + capimagest2, "png");*/
				}

			}
			double moveOffset = SKT::pixelToMm(moveOffsetInt, 25);

			vcg::Point3f Offset;
			if (imageAxis == int(genPF_X_Axis))
				Offset = vcg::Point3f(-moveOffset, 0, 0);
			else if (imageAxis == int(genPF_Y_Axis))
				Offset = vcg::Point3f(0, moveOffset, 0);
			else if (imageAxis == int(genPF_Z_Axis))
				Offset = vcg::Point3f(0, 0, -moveOffset);
			Matrix44m tranlate_to;
			tranlate_to.SetTranslate(Offset);
			meshVector[i + 1]->cm.Tr = tranlate_to *meshVector[i + 1]->cm.Tr;
			//*********end*************
			tri::UpdatePosition<CMeshO>::Matrix(meshVector[i + 1]->cm, tranlate_to, true);
			tri::UpdateBounding<CMeshO>::Box(meshVector[i + 1]->cm);

		}






		//int imageAxis = 0;
		//std::vector<cv::Mat> combineImage;
		//combineImage.assign((*(testSaveImage.at(0))).begin(), (*(testSaveImage.at(0))).end());
		//int moveOffsetInt = SKT::smallestOffsetfromM1M2(*(testSaveImage.at(0)), *(testSaveImage.at(3)), imageAxis, combineImage);
		//if (moveOffsetInt < 0)
		//	moveOffsetInt += 3;
		//else moveOffsetInt -= 5;
		//
		//QString temppath = PicaApplication::getRoamingDir();
		//QDir dir(temppath);//backup
		//dir.setPath(temppath + "/xyzimage");

		//for (int i = 0; i < combineImage.size(); i++)
		//{
		//	SKT::mergeImage(combineImage.at(i), (*(testSaveImage.at(3))).at(i), moveOffsetInt,0);

		//	if (true){
		//		QImage testttt = cvMatToQImage(combineImage.at(i));
		//		QString capimagest = "captemp_image";
		//		capimagest.append(QString("_%1_combine.png").arg(i, 4, 10, QChar('0')));
		//		testttt.save(dir.absolutePath() + "/" + capimagest, "png");
		//		
		//	}

		//}

		////SKT::mergeImage();
		//
		//
		//
		//double moveOffset = SKT::pixelToMm(moveOffsetInt, 25);

		//vcg::Point3f xOffset;
		//if (imageAxis==0)
		//	xOffset = vcg::Point3f (- moveOffset, 0, 0);
		//else
		//	xOffset = vcg::Point3f(0, 0, -moveOffset);
		//Matrix44m tranlate_to;
		//tranlate_to.SetTranslate(xOffset);

		////***20150827**************
		////Matrix44m tempt;
		////tempt = rm.Tr;		

		//m_record[1]->cm.Tr = tranlate_to *m_record[1]->cm.Tr;
		////*********end*************
		//tri::UpdatePosition<CMeshO>::Matrix(m_record[1]->cm, tranlate_to, true);
		//tri::UpdateBounding<CMeshO>::Box(m_record[1]->cm);











	}break;
#pragma endregion FP_TEST_AUTOPACK_V2
#pragma region FP_LANDING_BY_IMAGE_Z_AXIS
	case FP_LANDING_BY_IMAGE_Z_AXIS:
	{

		vector<PAIR> sortZAxis = SKT::sortAxis<float>(md, 2, GPF_Z_coordinate);

		QList<int>tempidList;
		for (int i = 0; i < sortZAxis.size(); i++)
		{
			temp.push_back(sortZAxis[i].first);
		}


		//get collapsed mesh's boxes
		//
		QSet<int> tempsave;
		for (int i = 0; i < sortZAxis.size(); i++)
		{
			qDebug() << "mesh_z_coordinate[i].second" << sortZAxis[i].second;
			tempsave.insert(sortZAxis[i].first);

			Box3m multiselectedBox;
			QList<MeshModel *> dispiteSelelctMesh;// = md.despiteSelectedMesh();
			QList<MeshModel *> selectedMesh;
			/*foreach(int mpid, md.multiSelectID)
			{*/
			selectedMesh.push_back(md.getMesh(sortZAxis[i].first));
			multiselectedBox.Add((md.getMesh(sortZAxis[i].first))->cm.bbox);
			//}
			//***get box and set min z to bottom
			double storeSelectMeshZ = multiselectedBox.min.Z();
			multiselectedBox.min[2] = -(md.groove.groovewidth) / 2;//z bottom
			//***get collapsed mesh's boxes								
			Box3m allBoxes(multiselectedBox), dispiteMeshBox;
			foreach(MeshModel *mp, md.meshList)
			{
				if (md.isPrint_item(*mp) && multiselectedBox.Collide(mp->cm.bbox) && mp->cm.bbox.min.Z() < sortZAxis[i].second && mp->id() != sortZAxis[i].first    /*&& md.multiSelectID.find(mp->id()) == md.multiSelectID.end()*/)
				{
					dispiteSelelctMesh.push_back(mp);
					allBoxes.Add(mp->cm.bbox);
					dispiteMeshBox.Add(mp->cm.bbox);
				}
			}
			allBoxes.min[0] = multiselectedBox.min.X();
			allBoxes.min[1] = multiselectedBox.min.Y();
			allBoxes.max[0] = multiselectedBox.max.X();
			allBoxes.max[1] = multiselectedBox.max.Y();
			allBoxes.max[2] = multiselectedBox.max.Z();
			if (dispiteMeshBox.min.Z() >= storeSelectMeshZ && dispiteSelelctMesh.size() > 0)
				return 0;
			else if (dispiteSelelctMesh.size() > 0)
			{
				std::vector<cv::Mat> testSaveImage, testSaveImage2;


				getMultiMeshLayerImage(dispiteSelelctMesh, testSaveImage, allBoxes);
				getMultiMeshLayerImage(selectedMesh, testSaveImage2, allBoxes);

				/////////////////////////////////////////////////////////////////////////
				//***ZAxis
				//////////////////////////////////////////////////////////////////////////
				std::vector<cv::Mat> combineImage2;


				int imageAxis = 2;
				int moveOffsetInt = SKT::smallestOffsetfromM1M2(testSaveImage, testSaveImage2, imageAxis, combineImage2);
				//if (moveOffsetInt >0)
				moveOffsetInt -= 3;
				//else if (moveOffsetInt < 0)moveOffsetInt -= 3;

				QString temppath = PicaApplication::getRoamingDir();
				QDir dir(temppath);//backup
				dir.setPath(temppath + "/xyzimage");

				for (int j = 0; j < testSaveImage.size(); j++)
				{
					if (true){
						QImage testttt = cvMatToQImage(testSaveImage.at(j));
						QString capimagest = "captemp_image";
						capimagest.append(QString("_%1_combine2.png").arg(j, 4, 10, QChar('0')));
						testttt.save(dir.absolutePath() + "/" + capimagest, "png");
					}
				}
				double moveOffset = SKT::pixelToMm(moveOffsetInt, 25);
				//if (moveOffsetInt < 0)moveOffset *= -1;

				vcg::Point3f Offset;
				if (imageAxis == int(genPF_X_Axis))
					Offset = vcg::Point3f(-moveOffset, 0, 0);
				else if (imageAxis == int(genPF_Y_Axis))
					Offset = vcg::Point3f(0, moveOffset, 0);
				else if (imageAxis == int(genPF_Z_Axis))
				{
					Offset = vcg::Point3f(0, 0, -moveOffset);
				}
				Matrix44m tranlate_to;
				tranlate_to.SetTranslate(Offset);

				/*foreach(int mpid, md.multiSelectID)
				{*/
				MeshModel *tmpmp = md.getMesh(sortZAxis[i].first);
				SKT::mesh_translate(tmpmp->cm, Offset);
				/*tmpmp->cm.Tr = tranlate_to *tmpmp->cm.Tr;
				tri::UpdatePosition<CMeshO>::Matrix(tmpmp->cm, tranlate_to, true);
				tri::UpdateBounding<CMeshO>::Box(tmpmp->cm);*/

				//}
			}
			else if (dispiteSelelctMesh.size() == 0)
			{
				vcg::Point3f Offset(0, 0, multiselectedBox.min[2] - storeSelectMeshZ);
				/*foreach(int mpid, md.multiSelectID)
				{*/
				MeshModel *tmpmp = md.getMesh(sortZAxis[i].first);
				SKT::mesh_translate(tmpmp->cm, Offset);
				//}
			}
		}










	}break;
#pragma endregion FP_LANDING_BY_IMAGE_Z_AXIS
#pragma region FP_LANDING_BY_IMAGE_X_AXIS
	case FP_LANDING_BY_IMAGE_X_AXIS:
	{

		vector<PAIR> sortXAxis = SKT::sortAxis<float>(md, 2, GPF_X_coordinate);

		/*QList<int>tempidList;
		for (int i = 0; i < sortXAxis.size(); i++)
		{
		temp.push_back(sortXAxis[i].first);
		}*/


		//get collapsed mesh's boxes
		//
		QSet<int> tempsave;
		for (int i = 0; i < sortXAxis.size(); i++)
		{
			qDebug() << "mesh_X_coordinate[i].second" << sortXAxis[i].second;
			tempsave.insert(sortXAxis[i].first);

			Box3m multiselectedBox;
			QList<MeshModel *> dispiteSelelctMesh;// = md.despiteSelectedMesh();
			QList<MeshModel *> selectedMesh;
			/*foreach(int mpid, md.multiSelectID)
			{*/
			selectedMesh.push_back(md.getMesh(sortXAxis[i].first));
			multiselectedBox.Add((md.getMesh(sortXAxis[i].first))->cm.bbox);
			//}
			//***get box and set min z to bottom
			double storeSelectMeshX = multiselectedBox.min.X();
			multiselectedBox.min[0] = -(md.groove.groovelength) / 2;//z bottom
			//***get collapsed mesh's boxes								
			Box3m allBoxes(multiselectedBox), dispiteMeshBox;
			foreach(MeshModel *mp, md.meshList)
			{
				if (md.isPrint_item(*mp) && multiselectedBox.Collide(mp->cm.bbox) && mp->cm.bbox.min.X() < sortXAxis[i].second && mp->id() != sortXAxis[i].first    /*&& md.multiSelectID.find(mp->id()) == md.multiSelectID.end()*/)
				{
					dispiteSelelctMesh.push_back(mp);
					allBoxes.Add(mp->cm.bbox);
					dispiteMeshBox.Add(mp->cm.bbox);
				}
			}
			allBoxes.min[1] = multiselectedBox.min.Y();
			allBoxes.min[2] = multiselectedBox.min.Z();
			allBoxes.max[0] = multiselectedBox.max.X();
			allBoxes.max[1] = multiselectedBox.max.Y();
			allBoxes.max[2] = multiselectedBox.max.Z();
			if (dispiteMeshBox.min.X() >= storeSelectMeshX && dispiteSelelctMesh.size() > 0)
				return 0;
			else if (dispiteSelelctMesh.size() > 0)
			{
				std::vector<cv::Mat> testSaveImage, testSaveImage2;


				getMultiMeshLayerImageZ(dispiteSelelctMesh, testSaveImage, allBoxes);
				getMultiMeshLayerImageZ(selectedMesh, testSaveImage2, allBoxes);

				/////////////////////////////////////////////////////////////////////////
				//***ZAxis
				//////////////////////////////////////////////////////////////////////////
				std::vector<cv::Mat> combineImage2;


				int imageAxis = GPF_X_coordinate;
				int moveOffsetInt = SKT::smallestOffsetfromM1M2(testSaveImage, testSaveImage2, GPF_X_coordinate, combineImage2);
				//if (moveOffsetInt > 3)
				moveOffsetInt -= 3;
				//else if (moveOffsetInt < 0)moveOffsetInt -= 3;

				QString temppath = PicaApplication::getRoamingDir();
				QDir dir(temppath);//backup
				dir.setPath(temppath + "/xyzimage");

				for (int j = 0; j < testSaveImage2.size(); j++)
				{
					if (true){
						QImage testttt = cvMatToQImage(testSaveImage2.at(j));
						QString capimagest = "captemp_image";
						capimagest.append(QString("_%1_combine2.png").arg(j, 4, 10, QChar('0')));
						testttt.save(dir.absolutePath() + "/" + capimagest, "png");
					}
				}
				double moveOffset = SKT::pixelToMm(moveOffsetInt, 25);
				//if (moveOffsetInt < 0)moveOffset *= -1;

				vcg::Point3f Offset;
				if (imageAxis == int(genPF_X_Axis))
					Offset = vcg::Point3f(-moveOffset, 0, 0);
				else if (imageAxis == int(genPF_Y_Axis))
					Offset = vcg::Point3f(0, moveOffset, 0);
				else if (imageAxis == int(genPF_Z_Axis))
				{
					Offset = vcg::Point3f(0, 0, -moveOffset);
				}
				Matrix44m tranlate_to;
				tranlate_to.SetTranslate(Offset);

				/*foreach(int mpid, md.multiSelectID)
				{*/
				MeshModel *tmpmp = md.getMesh(sortXAxis[i].first);
				SKT::mesh_translate(tmpmp->cm, Offset);

			}
			else if (dispiteSelelctMesh.size() == 0)
			{
				vcg::Point3f Offset(multiselectedBox.min[0] - storeSelectMeshX, 0, 0);
				/*foreach(int mpid, md.multiSelectID)
				{*/
				MeshModel *tmpmp = md.getMesh(sortXAxis[i].first);
				SKT::mesh_translate(tmpmp->cm, Offset);
				//}
			}
		}










	}break;
#pragma endregion FP_LANDING_BY_IMAGE_X_AXIS
#pragma region FP_LANDING_BY_IMAGE_Y_AXIS
	case FP_LANDING_BY_IMAGE_Y_AXIS:
	{

		vector<PAIR> sortYAxis = SKT::sortAxis<float>(md, 1, GPF_Y_coordinate);
		std::reverse(sortYAxis.begin(), sortYAxis.end());
		/*QList<int>tempidList;
		for (int i = 0; i < sortXAxis.size(); i++)
		{
		temp.push_back(sortXAxis[i].first);
		}*/


		//get collapsed mesh's boxes
		//
		QSet<int> tempsave;
		for (int i = 0; i < sortYAxis.size(); i++)
		{
			qDebug() << "mesh_X_coordinate[i].second" << sortYAxis[i].second;
			tempsave.insert(sortYAxis[i].first);

			Box3m multiselectedBox;
			QList<MeshModel *> dispiteSelelctMesh;// = md.despiteSelectedMesh();
			QList<MeshModel *> selectedMesh;
			/*foreach(int mpid, md.multiSelectID)
			{*/
			selectedMesh.push_back(md.getMesh(sortYAxis[i].first));
			multiselectedBox.Add((md.getMesh(sortYAxis[i].first))->cm.bbox);
			//}
			//***get box and set min Y to 
			double storeSelectMeshY = multiselectedBox.max.Y();
			multiselectedBox.max[1] = (md.groove.grooveheight) / 2;
			//***get collapsed mesh's boxes								
			Box3m allBoxes(multiselectedBox), dispiteMeshBox;
			foreach(MeshModel *mp, md.meshList)
			{
				if (md.isPrint_item(*mp) && multiselectedBox.Collide(mp->cm.bbox) && mp->cm.bbox.max.Y() > sortYAxis[i].second && mp->id() != sortYAxis[i].first    /*&& md.multiSelectID.find(mp->id()) == md.multiSelectID.end()*/)
				{
					dispiteSelelctMesh.push_back(mp);
					allBoxes.Add(mp->cm.bbox);
					dispiteMeshBox.Add(mp->cm.bbox);
				}
			}
			allBoxes.min[0] = multiselectedBox.min.X();
			allBoxes.min[1] = multiselectedBox.min.Y();
			allBoxes.min[2] = multiselectedBox.min.Z();
			allBoxes.max[0] = multiselectedBox.max.X();
			allBoxes.max[2] = multiselectedBox.max.Z();

			if (dispiteMeshBox.max.Y() <= storeSelectMeshY && dispiteSelelctMesh.size() > 0)
				return 0;
			else if (dispiteSelelctMesh.size() > 0)
			{
				std::vector<cv::Mat> testSaveImage, testSaveImage2;

				getMultiMeshLayerImageZ(dispiteSelelctMesh, testSaveImage, allBoxes);
				getMultiMeshLayerImageZ(selectedMesh, testSaveImage2, allBoxes);

				/////////////////////////////////////////////////////////////////////////
				//***ZAxis
				//////////////////////////////////////////////////////////////////////////
				std::vector<cv::Mat> combineImage2;


				int imageAxis = GPF_Y_coordinate;
				//***********************************************other**********select
				int moveOffsetInt = SKT::smallestOffsetfromM1M2(testSaveImage, testSaveImage2, GPF_Y_coordinate, combineImage2);
				//if (moveOffsetInt > 3)
				moveOffsetInt -= 3;
				//else if (moveOffsetInt < 0)moveOffsetInt -= 3;

				QString temppath = PicaApplication::getRoamingDir();
				QDir dir(temppath);//backup
				dir.setPath(temppath + "/xyzimage");

				for (int j = 0; j < testSaveImage2.size(); j++)
				{
					if (false){
						QImage testttt = cvMatToQImage(testSaveImage2.at(j));
						QString capimagest = "captemp_image";
						capimagest.append(QString("_%1_combine2.png").arg(j, 4, 10, QChar('0')));
						testttt.save(dir.absolutePath() + "/" + capimagest, "png");
					}
				}
				double moveOffset = SKT::pixelToMm(moveOffsetInt, 25);
				//if (moveOffsetInt < 0)moveOffset *= -1;

				vcg::Point3f Offset;
				if (imageAxis == int(genPF_X_Axis))
					Offset = vcg::Point3f(-moveOffset, 0, 0);
				else if (imageAxis == int(genPF_Y_Axis))
					Offset = vcg::Point3f(0, moveOffset, 0);
				else if (imageAxis == int(genPF_Z_Axis))
				{
					Offset = vcg::Point3f(0, 0, -moveOffset);
				}
				Matrix44m tranlate_to;
				tranlate_to.SetTranslate(Offset);

				/*foreach(int mpid, md.multiSelectID)
				{*/
				MeshModel *tmpmp = md.getMesh(sortYAxis[i].first);
				SKT::mesh_translate(tmpmp->cm, Offset);

			}
			else if (dispiteSelelctMesh.size() == 0)
			{
				vcg::Point3f Offset(0, multiselectedBox.max[1] - storeSelectMeshY, 0);
				/*foreach(int mpid, md.multiSelectID)
				{*/
				MeshModel *tmpmp = md.getMesh(sortYAxis[i].first);
				SKT::mesh_translate(tmpmp->cm, Offset);
				//}
			}
		}










	}break;
#pragma endregion FP_LANDING_BY_IMAGE_Y_AXIS
#pragma region FP_GET_DEPTH_IMAGE
	case FP_GET_DEPTH_IMAGE:
	{

		vector<PAIR> sortZAxis = SKT::sortAxis<float>(md, 2, GPF_Z_coordinate);

		/*QList<int>tempidList;
		for (int i = 0; i < sortZAxis.size(); i++)
		{
		temp.push_back(sortZAxis[i].first);
		}
		*/

		int sortZAxisSize = sortZAxis.size();
		/* for (int i = 0; i < sortZAxisSize; i++)
		{*/
		while (!sortZAxis.empty())
		{
			int i = 0;
			qDebug() << "mesh_z_coordinate[i].second" << sortZAxis[i].second;
			//tempsave.insert(sortZAxis[i].first);

			Box3m multiselectedBoxEachOne;
			QList<MeshModel *> dispiteSelelctMesh;
			QList<MeshModel *> selectedMesh;
			/*foreach(int mpid, md.multiSelectID)
			{*/
			selectedMesh.push_back(md.getMesh(sortZAxis[i].first));
			multiselectedBoxEachOne.Add((md.getMesh(sortZAxis[i].first))->cm.bbox);
			multiselectedBoxEachOne.max.Z() = md.groove.max.Z();

			//}
			//***get box and set min z to bottom
			double storeSelectMeshZ = multiselectedBoxEachOne.min.Z();
			multiselectedBoxEachOne.min[2] = -(md.groove.groovewidth) / 2;//z bottom
			//***get collapsed mesh's boxes								
			Box3m allBoxes(multiselectedBoxEachOne), dispiteMeshBox;
			QSet<int> tempsave;
			for (int x = 0; x < sortZAxis.size(); x++)
			{
				tempsave.insert(sortZAxis[x].first);
			}
			foreach(MeshModel *mp, md.meshList)
			{
				if (md.isPrint_item(*mp) && multiselectedBoxEachOne.Collide(mp->cm.bbox) /*&& mp->cm.bbox.min.Z() <= sortZAxis[i].second*/ && tempsave.find(mp->id()) == tempsave.end() /*mp->id() != sortZAxis[i].first*/
					/* && md.multiSelectID.find(mp->id()) == md.multiSelectID.end()*/)
				{
					dispiteSelelctMesh.push_back(mp);
					allBoxes.Add(mp->cm.bbox);
					dispiteMeshBox.Add(mp->cm.bbox);
				}
			}
			allBoxes.min[0] = multiselectedBoxEachOne.min.X();
			allBoxes.min[1] = multiselectedBoxEachOne.min.Y();
			allBoxes.max[0] = multiselectedBoxEachOne.max.X();
			allBoxes.max[1] = multiselectedBoxEachOne.max.Y();
			allBoxes.max[2] = multiselectedBoxEachOne.max.Z();
			/* if (dispiteMeshBox.min.Z() > storeSelectMeshZ && dispiteSelelctMesh.size() > 0)
			continue;
			else*/


			if (dispiteSelelctMesh.size() > 0)
			{
				std::vector<cv::Mat> testSaveImage, testSaveImage2;
				QVector<bool> genNum(6, true);


				testDepthBuffer(selectedMesh, testSaveImage, allBoxes, genNum, md.groove);
				testDepthBuffer(dispiteSelelctMesh, testSaveImage2, allBoxes, genNum, md.groove);

				/////////////////////////////////////////////////////////////////////////
				//***ZAxis
				//////////////////////////////////////////////////////////////////////////
				std::vector<cv::Mat> combineImage2;


				int imageAxis = 2;
				//int moveOffsetInt = SKT::smallestOffsetfromM1M2(testSaveImage, testSaveImage2, imageAxis, combineImage2);
				int moveOffsetInt = SKT::smallestOffsetfromDepthImage(testSaveImage2, testSaveImage, imageAxis);

				//if (moveOffsetInt >0)
				moveOffsetInt -= 0;
				//else if (moveOffsetInt < 0)moveOffsetInt -= 3;

				QString temppath = PicaApplication::getRoamingDir();
				QDir dir(temppath);//backup
				dir.setPath(temppath + "/xyzimage");

				for (int j = 0; j < testSaveImage.size(); j++)
				{
					if (true){
						QImage testttt = cvMatToQImage(testSaveImage.at(j));
						QString capimagest = "selectSaveImage";
						capimagest.append(QString("_%1_combine2.png").arg(j, 4, 10, QChar('0')));
						testttt.save(dir.absolutePath() + "/" + capimagest, "png");
					}
				}
				double moveOffset = SKT::pixelToMm(moveOffsetInt, 25);
				//if (moveOffsetInt < 0)moveOffset *= -1;

				vcg::Point3f Offset;
				if (imageAxis == int(genPF_X_Axis))
					Offset = vcg::Point3f(-moveOffset, 0, 0);
				else if (imageAxis == int(genPF_Y_Axis))
					Offset = vcg::Point3f(0, moveOffset, 0);
				else if (imageAxis == int(genPF_Z_Axis))
				{
					Offset = vcg::Point3f(0, 0, -moveOffset);
				}
				Matrix44m tranlate_to;
				tranlate_to.SetTranslate(Offset);

				/*foreach(int mpid, md.multiSelectID)
				{*/
				MeshModel *tmpmp = md.getMesh(sortZAxis[i].first);
				SKT::mesh_translate(tmpmp->cm, Offset);
				/*tmpmp->cm.Tr = tranlate_to *tmpmp->cm.Tr;
				tri::UpdatePosition<CMeshO>::Matrix(tmpmp->cm, tranlate_to, true);
				tri::UpdateBounding<CMeshO>::Box(tmpmp->cm);*/

				//}
			}
			else if (dispiteSelelctMesh.size() == 0)
			{
				vcg::Point3f Offset(0, 0, multiselectedBoxEachOne.min[2] - storeSelectMeshZ);
				/*foreach(int mpid, md.multiSelectID)
				{*/
				MeshModel *tmpmp = md.getMesh(sortZAxis[i].first);
				SKT::mesh_translate(tmpmp->cm, Offset);
				//}
			}
			if (sortZAxis.size() > 0)
			{
				sortZAxis.erase(sortZAxis.begin());
			}
		}
		//}










	}break;
#pragma endregion FP_GET_DEPTH_IMAGE
#pragma region FP_GET_DEPTH_LANDING_TEST2
	case FP_GET_DEPTH_LANDING_TEST2://READY FUNC
	{

		bool all_or_not = par.getBool("all_or_not");
		bool collision_detected = par.getBool("collision_detected");
		int a;
		//
		if (all_or_not)a = 1;
		else a = 0;

		switch (a)
		{
		case 1://all
		{
			if (collision_detected)
			{
				foreach(MeshModel *bm, md.meshList)
				{
					md.multiSelectID.insert(bm->id());
				}
				vector<PAIR> sortZAxis = SKT::sortAxis<float>(md, 2, GPF_Z_coordinate);
				float bt_gap = 2;

				for (int i = 0; i < sortZAxis.size(); i++)
				{
					MeshModel *bm = md.getMesh(sortZAxis[i].first);
					Point3m translatePath;
					translatePath.X() = 0;
					translatePath.Y() = 0;
					translatePath.Z() = md.groove.min.Z() - bm->cm.bbox.min.Z();
					testDepthRecursive_func(*bm, md, bt_gap, true, translatePath);
				}

			}
			else
			{
				Matrix44m landToGround;
				Point3m translatePath;
				foreach(MeshModel *bm, md.meshList)
				{
					landToGround.SetIdentity();
					translatePath.X() = 0;
					translatePath.Y() = 0;
					translatePath.Z() = md.groove.min.Z() - bm->cm.bbox.min.Z();
					landToGround.SetTranslate(translatePath);
					bm->cm.Tr = landToGround * bm->cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(bm->cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(bm->cm);

				}
			}

		}break;
		case 0://selected Mesh
		{
			if (collision_detected)
			{
				vector<PAIR> sortZAxis = SKT::sortAxis<float>(md, 2, GPF_Z_coordinate);
				float bt_gap = 2;

				for (int i = 0; i < sortZAxis.size(); i++)
				{
					MeshModel *bm = md.getMesh(sortZAxis[i].first);
					Point3m translatePath;
					translatePath.X() = 0;
					translatePath.Y() = 0;
					translatePath.Z() = md.groove.min.Z() - bm->cm.bbox.min.Z();
					testDepthRecursive_func(*bm, md, bt_gap, true, translatePath);
				}
			}
			else
			{
				foreach(int i, md.multiSelectID)
				{
					MeshModel *bm = md.getMesh(i);
					Matrix44m landToGround;
					landToGround.SetIdentity();
					Point3m translatePath;
					translatePath.X() = 0;
					translatePath.Y() = 0;
					translatePath.Z() = md.groove.min.Z() - bm->cm.bbox.min.Z();
					landToGround.SetTranslate(translatePath);
					bm->cm.Tr = landToGround * bm->cm.Tr;
					tri::UpdatePosition<CMeshO>::Matrix(bm->cm, landToGround, true);
					tri::UpdateBounding<CMeshO>::Box(bm->cm);
				}
			}
		}
		break;

		}


		//======================
		/*vector<PAIR> sortZAxis = SKT::sortAxis<float>(md, 2, GPF_Z_coordinate);
		float bt_gap = 2;

		for (int i = 0; i < sortZAxis.size(); i++)
		{
		MeshModel *bm = md.getMesh(sortZAxis[i].first);
		Point3m translatePath;
		translatePath.X() = 0;
		translatePath.Y() = 0;
		translatePath.Z() = md.groove.min.Z() - bm->cm.bbox.min.Z();
		testDepthRecursive_func(*bm, md, bt_gap, true, translatePath);
		}*/




	}break;
#pragma endregion FP_GET_DEPTH_LANDING_TEST2
#pragma region FP_PACKING_VERSION_3
	case FP_PACKING_VERSION_3://pending
	{
		float t_gap = md.p_setting.getTopGap();
		float b_gap = md.p_setting.getBottom_Gap();
		float r_gap = md.p_setting.getRight_Gap();
		float l_gap = md.p_setting.getLeft_Gap();
		float bt_gap = md.p_setting.getBetweenGap();

		bool auto_rotate = false;// par.getBool("auto_rotate_or_not");
		QMap<int, float> mesh_area_map, mesh_volumn_map;//***把meshmodel編號與最大面積傳入qmap

		//***20150908合為一個
		foreach(MeshModel *bm, md.meshList)
		{
			/*if (!(bm->label().contains(s1, Qt::CaseSensitive) || bm->label().contains(s2, Qt::CaseSensitive)))
			{*/
			if (bm->getMeshSort() == MeshModel::meshsort::print_item)
			{
				mesh_area_map.insert(bm->id(), bm->cm.bbox.Total_SurfaceArea());
				mesh_volumn_map.insert(bm->id(), bm->cm.bbox.Volume());
			}
			//}								

		}
		//*************************

		Box3m sinkBBox, sinkBackup;//***建構槽
		sinkBBox.min = md.groove.min;// vcg::Point3f(-15 / 2., -20 / 2., -17 / 2.);
		sinkBBox.max = md.groove.max;// vcg::Point3f(15 / 2., 20 / 2., 17 / 2.);

		sinkBackup.min = md.groove.min;
		sinkBackup.max = md.groove.max;

		//***增加gap

		float gap = 1;
		sinkBBox.min[0] = sinkBBox.min[0] + l_gap;
		sinkBBox.min[1] = sinkBBox.min[1] + b_gap;
		sinkBBox.max[0] = sinkBBox.max[0] - r_gap;
		sinkBBox.max[1] = sinkBBox.max[1] - t_gap;
		sinkBackup.min[0] = sinkBackup.min[0] + l_gap;
		sinkBackup.min[1] = sinkBackup.min[1] + b_gap;
		sinkBackup.max[0] = sinkBackup.max[0] - r_gap;
		sinkBackup.max[1] = sinkBackup.max[1] - t_gap;

		if (mesh_area_map.size() > 0)
			xyz::box_packing_algrithm_left(sinkBBox, mesh_area_map, mesh_volumn_map, md, sinkBackup, bt_gap, auto_rotate);
		//xyz::LAFF_First_Placement(sinkBBox, sinkBackup, mesh_area_map, mesh_volumn_map, md, bt_gap, auto_rotate);
		else return false;


	}break;
#pragma endregion FP_PACKING_VERSION_3

#pragma region FP_TEST_SEARCH_SPACE_ALL_IMAGE
	case FP_TEST_SEARCH_SPACE_ALL_IMAGE2:
	case FP_TEST_SEARCH_SPACE_ALL_IMAGE:
	{
		bool auto_rotate = par.getBool("auto_rotate_or_not");

		if (md.multiSelectID.size() == 0)
		{
			foreach(MeshModel *mm, md.meshList)
			{

				md.multiSelectID.insert(mm->id());
			}
		}

		//sort id with box volumn
		QVector<int> sortID;
		foreach(int id, md.multiSelectID)
		{
			sortID.push_back(id);
		}

		bool flag = true;
		for (int i = 0; i < sortID.size() - 1 && flag; i++)
		{
			flag = false;
			for (int j = 0; j < sortID.size() - i - 1; j++)
			{
				MeshModel *am = md.getMesh(sortID[j]);
				MeshModel *bm = md.getMesh(sortID[j + 1]);
				if (bm->cm.bbox.Volume() > am->cm.bbox.Volume())
				{
					int temp = sortID[j];
					sortID[j] = sortID[j + 1];
					sortID[j + 1] = temp;
					flag = true;
				}
			}
		}

		//GET GROOVE X DEPTH IMAGE
		//SET AS BLACK FIRST
		//THEN GET BOTTOM  DEPTH IMAGE
		//COMPARE WITH THE BLACK IMAGE(FIND THE SMALLEST X AND Y)


		QList<MeshModel *> dispiteSelelctMesh = md.despiteSelectedMesh();
		/*int grooveHalfXPixel = md.groove.DimX() * 25 / (DSP_inchmm * 2);
		int grooveHalfYPixel = md.groove.DimY() * 25 / (DSP_inchmm * 2);*/


		QVector<bool> genNum(6, true);
		genNum[2] = false;
		genNum[3] = false;
		genNum[4] = false;
		genNum[5] = false;

		//foreach(int id, md.multiSelectID)
		//foreach(int id, sortID)
		int tempDPI = 25;
		float pixelRatio = 1;// pixel : 1mm

		Box3m littlebig(md.groove);
		littlebig.Offset(vcg::Point3f(1, 1, 1));
		float gap_offset = md.p_setting.getBetweenGap();;
		Box3m grooveAddOffset(md.groove);
		grooveAddOffset.Offset(Point3m(gap_offset, gap_offset, gap_offset));

		for (int i = 0; i < sortID.size(); i++)
		{
			int id = sortID[i];
			bool breakfileter = cb((i * 100) / sortID.size(), "Auto_Packing");

			if (breakfileter)
				glContext->makeCurrent();
			else
			{
				glContext->doneCurrent();
				return false;

			}






			//move to right back top corner
			MeshModel *bm = md.getMesh(id);


			//auto rotate
			if (auto_rotate)
			{
				int mindim = bm->cm.bbox.MinDim();
				Point3m rotate_xaxis(1.0, 0.0, 0.0);//***x旋轉軸
				Point3m rotate_yaxis(0.0, 1.0, 0.0);
				Point3m rotate_zaxis(0.0, 0.0, 1.0);//***z旋轉軸

				switch (mindim)//***轉至高度最低 disable auto_rotate
				{
				case 0://***mindim為x軸
					SKT::mesh_rotate<CMeshO, Scalarm>(bm->cm, 90.0, rotate_yaxis);
					break;
				case 1://***mindim為y軸
					SKT::mesh_rotate<CMeshO, Scalarm>(bm->cm, 90.0, rotate_xaxis);
					break;
				case 2://***mindim為z軸
					break;
				}

				int maxdim;
				if (md.groove.DimX() >= md.groove.DimY())maxdim = 0;
				else maxdim = 1;
				switch (maxdim)
				{
				case 0:
				{
					if (bm->cm.bbox.MaxDim() == 1)SKT::mesh_rotate<CMeshO, Scalarm>(bm->cm, 90.0, rotate_zaxis);
				}
				break;
				case 1:
				{
					if (bm->cm.bbox.MaxDim() == 0)SKT::mesh_rotate<CMeshO, Scalarm>(bm->cm, 90.0, rotate_zaxis);
				}
				break;
				case 2:
					break;
				}
			}



			int bmMaxDim = bm->cm.bbox.MaxDim();
			int bmMinDim = bm->cm.bbox.MinDim();
			int bmMidDim = bm->cm.bbox.MidDim();

			int grooveMaxDim = md.groove.MaxDim();
			int grooveMinDim = md.groove.MinDim();
			int grooveMidDim = md.groove.MidDim();



			std::vector<cv::Mat> baseSaveImage;
			std::vector<cv::Mat> selectSaveImage;

			testDepthBuffer(dispiteSelelctMesh, baseSaveImage, grooveAddOffset, genNum, grooveAddOffset, false, false, 1, pixelRatio);


			/**************scale object*/
			float scaleNumX = (bm->cm.bbox.DimX() + gap_offset * 2) / bm->cm.bbox.DimX();
			float scaleNumY = (bm->cm.bbox.DimY() + gap_offset * 2) / bm->cm.bbox.DimY();
			float scaleNumZ = (bm->cm.bbox.DimZ() + gap_offset * 2) / bm->cm.bbox.DimZ();
			Matrix44m scaleMatrix, wholeTransformMatrix;
			scaleMatrix.SetScale(*new Point3m(scaleNumX, scaleNumY, scaleNumZ));

			Matrix44m scale_Translate_BackTo_CenterMatrix;
			Matrix44m scale_Translate_GoBackTo_OriginalMatrix;

			Point3m meshCenter = bm->cm.bbox.Center();

			scale_Translate_BackTo_CenterMatrix.SetTranslate(-(meshCenter));//***back to center
			scale_Translate_GoBackTo_OriginalMatrix.SetTranslate(meshCenter);//***original to position
			wholeTransformMatrix = scale_Translate_GoBackTo_OriginalMatrix*scaleMatrix*scale_Translate_BackTo_CenterMatrix;

			tri::UpdatePosition<CMeshO>::Matrix(bm->cm, wholeTransformMatrix, true);
			tri::UpdateBounding<CMeshO>::Box(bm->cm);
			/****************end scale object*/

			/**********move to groove top back right***********/
			//if (bm->cm.bbox.DimValue(bmMaxDim) > md.groove.DimValue(bmMaxDim) || bm->cm.bbox.DimValue(bmMinDim) > md.groove.DimValue(grooveMinDim) || bm->cm.bbox.DimValue(bmMidDim) > md.groove.DimValue(grooveMidDim))
			if (bm->cm.bbox.DimX() > md.groove.DimX() || bm->cm.bbox.DimY() > md.groove.DimY() || bm->cm.bbox.DimZ() > md.groove.DimZ())
			{
				//===move out from groove												
				double x_offset_right = md.groove.max.X() - bm->cm.bbox.min.X();
				double y_offset_back = md.groove.max.Y() - bm->cm.bbox.min.Y();
				double z_offset_top = md.groove.max.Z() - bm->cm.bbox.min.Z();
				vcg::Point3f Offset_top_back_right = vcg::Point3f(x_offset_right, y_offset_back, z_offset_top);
				SKT::mesh_translate(bm->cm, Offset_top_back_right);

				///**************scale object*/
				Matrix44m scaleMatrix2, wholeTransformMatrix2;
				float recoverX = 1. / scaleNumX;
				float recoverY = 1. / scaleNumY;
				float recoverZ = 1. / scaleNumZ;
				scaleMatrix2.SetScale(*new Point3m(recoverX, recoverY, recoverZ));

				Matrix44m scale_Translate_BackTo_CenterMatrix2;
				Matrix44m scale_Translate_GoBackTo_OriginalMatrix2;

				meshCenter = bm->cm.bbox.Center();
				scale_Translate_BackTo_CenterMatrix2.SetTranslate(-(meshCenter));//***back to center
				scale_Translate_GoBackTo_OriginalMatrix2.SetTranslate(meshCenter);//***original to position
				wholeTransformMatrix2 = scale_Translate_GoBackTo_OriginalMatrix2*scaleMatrix2*scale_Translate_BackTo_CenterMatrix2;

				tri::UpdatePosition<CMeshO>::Matrix(bm->cm, wholeTransformMatrix2, true);
				tri::UpdateBounding<CMeshO>::Box(bm->cm);
				/****************end   scale object*/


				continue;
			}
			else{
				double x_offset_right = md.groove.max.X() - bm->cm.bbox.max.X();
				double y_offset_back = md.groove.max.Y() - bm->cm.bbox.max.Y();
				double z_offset_top = md.groove.max.Z() - bm->cm.bbox.max.Z();
				vcg::Point3f Offset_top_back_right = vcg::Point3f(x_offset_right, y_offset_back, z_offset_top);
				SKT::mesh_translate(bm->cm, Offset_top_back_right);
			}
			/*****************************************************/


			QList<MeshModel *> selectMesh;
			Box3m mmmBox(bm->cm.bbox);


			selectMesh.push_back(bm);
			testDepthBuffer(selectMesh, selectSaveImage, mmmBox, genNum, grooveAddOffset, false, false, 1, pixelRatio);

			///**************scale object*/
			Matrix44m scaleMatrix2, wholeTransformMatrix2;
			float recoverX = 1. / scaleNumX;
			float recoverY = 1. / scaleNumY;
			float recoverZ = 1. / scaleNumZ;
			scaleMatrix2.SetScale(*new Point3m(recoverX, recoverY, recoverZ));

			Matrix44m scale_Translate_BackTo_CenterMatrix2;
			Matrix44m scale_Translate_GoBackTo_OriginalMatrix2;

			meshCenter = bm->cm.bbox.Center();
			scale_Translate_BackTo_CenterMatrix2.SetTranslate(-(meshCenter));//***back to center
			scale_Translate_GoBackTo_OriginalMatrix2.SetTranslate(meshCenter);//***original to position
			wholeTransformMatrix2 = scale_Translate_GoBackTo_OriginalMatrix2*scaleMatrix2*scale_Translate_BackTo_CenterMatrix2;

			tri::UpdatePosition<CMeshO>::Matrix(bm->cm, wholeTransformMatrix2, true);
			tri::UpdateBounding<CMeshO>::Box(bm->cm);
			/****************end   scale object*/




			int xinGroove, yinGroove, zinImageValue;

			QTime time;
			//clock_t start, stop;
			time.start();
			SKT::searchEmptySpaceInDepthImage(selectSaveImage, baseSaveImage, xinGroove, yinGroove, zinImageValue);
			//SKT::searchEmptySpaceInDepthImage_SLS(selectSaveImage, baseSaveImage, xinGroove, yinGroove, zinImageValue);
			qDebug() << time.elapsed();


			double tempHalfimageX = baseSaveImage[0].size().width / 2.;
			double tempHalfimageY = baseSaveImage[0].size().height / 2.;

			double x_pos = (xinGroove - tempHalfimageX) + gap_offset;
			double y_pos = (tempHalfimageY - yinGroove) - gap_offset;

			x_pos = SKT::pixelRatioToMm(x_pos, pixelRatio);
			y_pos = SKT::pixelRatioToMm(y_pos, pixelRatio);

			if ((y_pos - bm->cm.bbox.DimY()) < md.groove.min.Y())
				y_pos = md.groove.min.Y() + bm->cm.bbox.DimY();

			if (x_pos + bm->cm.bbox.DimX() > md.groove.max.X())
				x_pos = md.groove.max.X() - bm->cm.bbox.DimX();

			if (abs(y_pos - (grooveAddOffset.DimY() / 2. - 1)) < 0.01)
				y_pos = grooveAddOffset.DimY() / 2.;
			if (zinImageValue == -1)
				zinImageValue = 0;
			double z_pos = zinImageValue * grooveAddOffset.DimZ() / 255;

			if (abs(z_pos - (bm->cm.bbox.min.Z() + md.groove.DimZ() / 2.)) < 1)
				z_pos = (bm->cm.bbox.min.Z() + md.groove.DimZ() / 2.);

			double x_offset = x_pos - bm->cm.bbox.min.X();
			double y_offset = y_pos - bm->cm.bbox.max.Y();
			double z_offset = -z_pos;
			vcg::Point3f offset = vcg::Point3f(x_offset, y_offset, z_offset);
			SKT::mesh_translate(bm->cm, offset);
			if (!littlebig.isInside(bm->cm.bbox))
			{
				//===move out from groove												
				double x_offset_right = md.groove.max.X() - bm->cm.bbox.min.X();
				double y_offset_back = md.groove.max.Y() - bm->cm.bbox.min.Y();
				double z_offset_top = md.groove.max.Z() - bm->cm.bbox.min.Z();
				vcg::Point3f Offset_top_back_right = vcg::Point3f(x_offset_right, y_offset_back, z_offset_top);
				SKT::mesh_translate(bm->cm, Offset_top_back_right);
			}
			dispiteSelelctMesh.push_back(bm);
		}


	}break;
#pragma endregion FP_TEST_SEARCH_SPACE_ALL_IMAGE

	//bak
	//#pragma region FP_TEST_SEARCH_SPACE_ALL_IMAGE
	//	case FP_TEST_SEARCH_SPACE_ALL_IMAGE2:
	//	case FP_TEST_SEARCH_SPACE_ALL_IMAGE:
	//	{
	//										   bool auto_rotate = par.getBool("auto_rotate_or_not");
	//										   if (md.multiSelectID.size() == 0)
	//										   {
	//											   foreach(MeshModel *mm, md.meshList)
	//											   {
	//
	//												   md.multiSelectID.insert(mm->id());
	//											   }
	//										   }
	//
	//										   //sort id with box volumn
	//										   QVector<int> sortID;
	//										   foreach(int id, md.multiSelectID)
	//										   {
	//											   sortID.push_back(id);
	//										   }
	//
	//										   bool flag = true;
	//										   for (int i = 0; i < sortID.size() - 1 && flag; i++)
	//										   {
	//											   flag = false;
	//											   for (int j = 0; j < sortID.size() - i - 1; j++)
	//											   {
	//												   MeshModel *am = md.getMesh(sortID[j]);
	//												   MeshModel *bm = md.getMesh(sortID[j + 1]);
	//												   if (bm->cm.bbox.Volume() > am->cm.bbox.Volume())
	//												   {
	//													   int temp = sortID[j];
	//													   sortID[j] = sortID[j + 1];
	//													   sortID[j + 1] = temp;
	//													   flag = true;
	//												   }
	//											   }
	//										   }
	//
	//										   //GET GROOVE X DEPTH IMAGE
	//										   //SET AS BLACK FIRST
	//										   //THEN GET BOTTOM  DEPTH IMAGE
	//										   //COMPARE WITH THE BLACK IMAGE(FIND THE SMALLEST X AND Y)
	//
	//
	//										   QList<MeshModel *> dispiteSelelctMesh = md.despiteSelectedMesh();
	//										   /*int grooveHalfXPixel = md.groove.DimX() * 25 / (DSP_inchmm * 2);
	//										   int grooveHalfYPixel = md.groove.DimY() * 25 / (DSP_inchmm * 2);*/
	//
	//
	//										   QVector<bool> genNum(6, true);
	//										   genNum[2] = false;
	//										   genNum[3] = false;
	//										   genNum[4] = false;
	//										   genNum[5] = false;
	//
	//										   //foreach(int id, md.multiSelectID)
	//										   //foreach(int id, sortID)
	//										   int tempDPI = 25;
	//										   //float pixelRatio = 0.1;
	//										   float pixelRatio = 2;
	//										   Box3m littlebig(md.groove);
	//										   littlebig.Offset(vcg::Point3f(1, 1, 1));
	//										   int gap_offset = 0;
	//										   for (int i = 0; i < sortID.size(); i++)
	//										   {
	//											   int id = sortID[i];
	//											   cb((i * 100) / sortID.size(), "Auto_Packing");
	//											   //move to right back top corner
	//											   MeshModel *bm = md.getMesh(id);
	//
	//
	//											   //auto rotate
	//											   if (auto_rotate)
	//											   {
	//												   int mindim = bm->cm.bbox.MinDim();
	//												   Point3m rotate_xaxis(1.0, 0.0, 0.0);//***x旋轉軸
	//												   Point3m rotate_yaxis(0.0, 1.0, 0.0);
	//												   Point3m rotate_zaxis(0.0, 0.0, 1.0);//***z旋轉軸
	//
	//												   switch (mindim)//***轉至高度最低 disable auto_rotate
	//												   {
	//												   case 0://***mindim為x軸
	//													   SKT::mesh_rotate<CMeshO, Scalarm>(bm->cm, 90.0, rotate_yaxis);
	//													   break;
	//												   case 1://***mindim為y軸
	//													   SKT::mesh_rotate<CMeshO, Scalarm>(bm->cm, 90.0, rotate_xaxis);
	//													   break;
	//												   case 2://***mindim為z軸
	//													   break;
	//												   }
	//
	//												   int maxdim;
	//												   if (md.groove.DimX() >= md.groove.DimY())maxdim = 0;
	//												   else maxdim = 1;
	//												   switch (maxdim)
	//												   {
	//												   case 0:
	//												   {
	//															 if (bm->cm.bbox.MaxDim() == 1)SKT::mesh_rotate<CMeshO, Scalarm>(bm->cm, 90.0, rotate_zaxis);
	//												   }
	//													   break;
	//												   case 1:
	//												   {
	//															 if (bm->cm.bbox.MaxDim() == 0)SKT::mesh_rotate<CMeshO, Scalarm>(bm->cm, 90.0, rotate_zaxis);
	//												   }
	//													   break;
	//												   case 2:
	//													   break;
	//												   }
	//											   }
	//
	//
	//
	//											   int bmMaxDim = bm->cm.bbox.MaxDim();
	//											   int bmMinDim = bm->cm.bbox.MinDim();
	//											   int bmMidDim = bm->cm.bbox.MidDim();
	//
	//											   int grooveMaxDim = md.groove.MaxDim();
	//											   int grooveMinDim = md.groove.MinDim();
	//											   int grooveMidDim = md.groove.MidDim();
	//
	//											   //if (bm->cm.bbox.DimValue(bmMaxDim) > md.groove.DimValue(bmMaxDim) || bm->cm.bbox.DimValue(bmMinDim) > md.groove.DimValue(grooveMinDim) || bm->cm.bbox.DimValue(bmMidDim) > md.groove.DimValue(grooveMidDim))
	//											   if (bm->cm.bbox.DimX() > md.groove.DimX() || bm->cm.bbox.DimY() > md.groove.DimY() || bm->cm.bbox.DimZ() > md.groove.DimZ())
	//											   {
	//												   //===move out from groove												
	//												   double x_offset_right = md.groove.max.X() - bm->cm.bbox.min.X();
	//												   double y_offset_back = md.groove.max.Y() - bm->cm.bbox.min.Y();
	//												   double z_offset_top = md.groove.max.Z() - bm->cm.bbox.min.Z();
	//												   vcg::Point3f Offset_top_back_right = vcg::Point3f(x_offset_right, y_offset_back, z_offset_top);
	//												   SKT::mesh_translate(bm->cm, Offset_top_back_right);
	//												   continue;
	//											   }
	//											   else{
	//												   double x_offset_right = md.groove.max.X() - bm->cm.bbox.max.X();
	//												   double y_offset_back = md.groove.max.Y() - bm->cm.bbox.max.Y();
	//												   double z_offset_top = md.groove.max.Z() - bm->cm.bbox.max.Z();
	//												   vcg::Point3f Offset_top_back_right = vcg::Point3f(x_offset_right, y_offset_back, z_offset_top);
	//												   SKT::mesh_translate(bm->cm, Offset_top_back_right);
	//											   }
	//
	//											   std::vector<cv::Mat> baseSaveImage;
	//											   std::vector<cv::Mat> selectSaveImage;
	//											   /*Box3m mdBox(md.groove);
	//											   mdBox.min.X() -= 1;
	//											   mdBox.min.Y() -= 1;
	//											   mdBox.min.Z() -= 1;*/
	//
	//											   /*Box3m littleZbig(md.groove);
	//											   littleZbig.Offset(vcg::Point3f(0, 0, 1));*/
	//											   testDepthBuffer(dispiteSelelctMesh, baseSaveImage, md.groove, genNum, true, false, 1, pixelRatio);
	//
	//											   QList<MeshModel *> selectMesh;
	//											   Box3m mmmBox(bm->cm.bbox);
	//											   selectMesh.push_back(bm);
	//											   testDepthBuffer(selectMesh, selectSaveImage, mmmBox, genNum, false, false, 1, pixelRatio);
	//
	//											   int xinGroove, yinGroove, zinImageValue;
	//
	//											   QTime time;
	//											   //clock_t start, stop;
	//											   time.start();
	//											   SKT::searchEmptySpaceInDepthImage(selectSaveImage, baseSaveImage, xinGroove, yinGroove, zinImageValue);
	//											   //SKT::searchEmptySpaceInDepthImage_SLS(selectSaveImage, baseSaveImage, xinGroove, yinGroove, zinImageValue);
	//											   qDebug() << time.elapsed();
	//
	//
	//											   double tempHalfimageX = baseSaveImage[0].size().width / 2.;
	//											   double tempHalfimageY = baseSaveImage[0].size().height / 2.;
	//
	//											   double x_pos = (xinGroove - tempHalfimageX) + gap_offset;
	//											   double y_pos = (tempHalfimageY - yinGroove) - gap_offset;
	//
	//											   x_pos = SKT::pixelRatioToMm(x_pos, pixelRatio);
	//											   y_pos = SKT::pixelRatioToMm(y_pos, pixelRatio);
	//											   if ((y_pos - bm->cm.bbox.DimY()) < md.groove.min.Y())
	//												   y_pos = md.groove.min.Y() + bm->cm.bbox.DimY();
	//											   if (abs(x_pos - (-(DSP_grooveBigX / 2. - 1))) < 0.01)
	//												   x_pos = -DSP_grooveBigX / 2.;
	//
	//											   if (abs(y_pos - (DSP_grooveY / 2. - 1)) < 0.01)
	//												   y_pos = DSP_grooveY / 2.;
	//											   if (zinImageValue == -1)
	//												   zinImageValue = 0;
	//											   double z_pos = (zinImageValue * DSP_grooveZ / 255);
	//											   double x_offset = x_pos - bm->cm.bbox.min.X();
	//											   double y_offset = y_pos - bm->cm.bbox.max.Y();
	//											   double z_offset = -z_pos;
	//											   vcg::Point3f offset = vcg::Point3f(x_offset, y_offset, z_offset);
	//											   SKT::mesh_translate(bm->cm, offset);
	//											   if (!littlebig.isInside(bm->cm.bbox))
	//											   {
	//												   //===move out from groove												
	//												   double x_offset_right = md.groove.max.X() - bm->cm.bbox.min.X();
	//												   double y_offset_back = md.groove.max.Y() - bm->cm.bbox.min.Y();
	//												   double z_offset_top = md.groove.max.Z() - bm->cm.bbox.min.Z();
	//												   vcg::Point3f Offset_top_back_right = vcg::Point3f(x_offset_right, y_offset_back, z_offset_top);
	//												   SKT::mesh_translate(bm->cm, Offset_top_back_right);
	//											   }
	//											   dispiteSelelctMesh.push_back(bm);
	//										   }
	//
	//
	//	}break;
	//#pragma endregion FP_TEST_SEARCH_SPACE_ALL_IMAGE


#pragma region FP_TEST_GENERATE_POINTS_FROM_GRAY_IMAGE

	case FP_TEST_GENERATE_POINTS_FROM_GRAY_IMAGE://pending
	{
		WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("FP_TEST_GENERATE_POINTS_FROM_GRAY_IMAGE"));
		float witnessWidth = par.getFloat("WITNESS_WIDTH");

		if (md.multiSelectID.size() == 0)
		{
			foreach(MeshModel *mm, md.meshList)
			{
				if (mm->getMeshSort() == MeshModel::meshsort::print_item)
					md.multiSelectID.insert(mm->id());
				else return false;
			}
		}

		md.pointsInFrontOfMesh.clear();
		QList<MeshModel *> dispiteSelelctMesh = md.despiteSelectedMesh();
		int grooveHalfXPixel = md.groove.DimX() * 25 / (DSP_inchmm * 2);
		int grooveHalfYPixel = md.groove.DimY() * 25 / (DSP_inchmm * 2);


		QVector<bool> genNum(6, true);
		genNum[0] = false;
		genNum[1] = false;
		genNum[2] = false;
		genNum[3] = true;
		genNum[4] = false;
		genNum[5] = false;
		int inDPI = 50;
		//foreach(int id, md.multiSelectID)
		foreach(int id, md.multiSelectID)
		{
			md.pointsInFrontOfMesh.clear();
			//move to right back top corner
			MeshModel *bm = md.getMesh(id);


			std::vector<cv::Mat> baseSaveImage;
			std::vector<cv::Mat> selectSaveImage;


			QList<MeshModel *> selectMesh;
			Box3m mmmBox(bm->cm.bbox);
			selectMesh.push_back(bm);
			int pixelRatio = 5;
			//testDepthBuffer(selectMesh, selectSaveImage, md.groove, genNum, false, false,inDPI, pixelRatio);
			testDepthBuffer(selectMesh, selectSaveImage, md.groove, genNum, md.groove, false, false, 1, pixelRatio);
			//testDepthBuffer(dispiteSelelctMesh, baseSaveImage, md.groove, genNum, true, false, 1, pixelRatio);
			int xinGroove, yinGroove, zinGroove;

			//QTime time;
			////clock_t start, stop;
			//time.start();
			float stepss = 3;
			std::vector<vcg::Point3f> pointCloud;
			std::vector<vcg::Point3f> pointCloud2;
			int witnessOffset = 5;
			SKT::GeneratePointsFromGrayImage(selectSaveImage, md.pointsInFrontOfMesh, bm->cm.bbox.min.X(), witnessOffset, stepss, pixelRatio, inDPI);
			//SKT::GeneratePointsFromGrayImage2(selectSaveImage, pointCloud, bm->cm.bbox.min.X(), stepss, inDPI);
			//SKT::GeneratePointsFromContour(selectSaveImage, pointCloud2, bm->cm.bbox.min.X(), 10);
			//qDebug() << time.elapsed();

			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("FP_TEST_GENERATE_POINTS_FROM_GRAY_IMAGE"));
			//=========first method============//
			//float boxsize = 1 / (inDPI / 25.4);
			float boxsize = 1. / pixelRatio;
			MeshModel *alShadow = md.addNewMesh("a", "alShadow", false, RenderMode(GLW::DMFlat));//backup														
			//MeshModel *alShadow2 = md.addNewMesh("a2", "alShadow2", false, RenderMode(GLW::DMFlat));//backup														

			//===create Model R_Tree==//
			CMeshO::FaceIterator fi;
			fi = bm->cm.face.begin();
			//fi->SetS();
			int i2 = 0, nhits2;
			RTree<int, float, 3, float> tree3;

			for (fi = bm->cm.face.begin(); fi != bm->cm.face.end(); ++fi)
			{
				/*if (!(*fi).IsD() && (*fi).IsS() && !(*fi).IsV())
				visitStack.push_back(&*fi);*/
				vcg::Point3f  v0 = fi->V(0)->P();

				float sx1 = fi->V(0)->P().X();
				float sx2 = fi->V(1)->P().X();
				float sx3 = fi->V(2)->P().X();

				float sy1 = fi->V(0)->P().Y();
				float sy2 = fi->V(1)->P().Y();
				float sy3 = fi->V(2)->P().Y();

				float sz1 = fi->V(0)->P().Z();
				float sz2 = fi->V(1)->P().Z();
				float sz3 = fi->V(2)->P().Z();

				float bmin[3];
				float bmax[3];

				bmax[0] = sx1 > sx2 ? (sx1 > sx3 ? sx1 : sx3) : (sx2 > sx3 ? sx2 : sx3);
				bmax[1] = sy1 > sy2 ? (sy1 > sy3 ? sy1 : sy3) : (sy2 > sy3 ? sy2 : sy3);
				bmax[2] = sz1 > sz2 ? (sz1 > sz3 ? sz1 : sz3) : (sz2 > sz3 ? sz2 : sz3);

				bmin[0] = sx1 < sx2 ? (sx1 < sx3 ? sx1 : sx3) : (sx2 < sx3 ? sx2 : sx3);
				bmin[1] = sy1 < sy2 ? (sy1 < sy3 ? sy1 : sy3) : (sy2 < sy3 ? sy2 : sy3);
				bmin[2] = sz1 < sz2 ? (sz1 < sz3 ? sz1 : sz3) : (sz2 < sz3 ? sz2 : sz3);

				tree3.Insert(bmin, bmax, i2);
				i2++;

			}
			//===end create Model R_Tree==//
			//FILE *dbgff = fopen("D:/ouputPC.txt", "w");//@@@;
			for (int x = 0; x < md.pointsInFrontOfMesh.size(); x++)
			{
				cb((x * 100) / md.pointsInFrontOfMesh.size(), "GENERATE_POINTS_FROM_GRAY_IMAGE");
				vcg::Point3f pos = md.pointsInFrontOfMesh[x];
				/*
				output Point Cloud
				*/
				QString tempPC = QString("%1 %2 %3\n").arg(pos[0], 0, 'f', 3).arg(pos[1], 0, 'f', 3).arg(pos[2], 0, 'f', 3);
				//fprintf(dbgff, tempPC.toStdString().c_str()); //@@@
				//fflush(dbgff);

				Box3m b(Point3m(1.5, boxsize, boxsize)*(-stepss / 2) + pos, Point3m(boxsize, boxsize, boxsize)*(stepss / 2) + pos);

				bool cbreak = false;
				/* for (int x = 0; x < pointCloud.size(); x++)
				{
				vcg::Point3f pos2 = pointCloud[x];

				if (b.IsIn(pos2))
				cbreak = false;
				}
				if (!cbreak)
				continue;*/
				float bmin[3] = { b.min[0], b.min[1], b.min[2] };
				float bmax[3] = { b.max[0], b.max[1], b.max[2] };
				cbreak = tree3.Search(bmin, bmax, MySearchCallback, NULL);
				if (cbreak)continue;//Test Intersection

				MeshModel* m = md.addNewMesh("", "temp");
				tri::Box<CMeshO>(m->cm, b);
				m->updateDataMask(MeshModel::MM_POLYGONAL);
				//m->UpdateBoxAndNormals();
				vcg::tri::Append<CMeshO, CMeshO>::Mesh(alShadow->cm, m->cm);
				md.delMesh(m);
				/*
				//===build points Mesh=====///
				CMeshO::VertexIterator vii;
				vii = tri::Allocator<CMeshO>::AddVertices(alShadow->cm, 1);
				CMeshO::VertexPointer  ptVec[1];
				ptVec[0] = &*vii;
				vii->P() = CMeshO::CoordType(md.pointsInFrontOfMesh[x].X(), md.pointsInFrontOfMesh[x].Y(), md.pointsInFrontOfMesh[x].Z());
				alShadow->cm.vert.back().ImportData(*ptVec[0]);
				*/
			}
			//dbgff->close();
			tri::Clean<CMeshO>::RemoveDuplicateVertex(alShadow->cm);
			alShadow->setMeshSort(MeshModel::meshsort::pre_print_item);
			alShadow->Enable(vcg::tri::io::Mask::IOM_VERTCOLOR);
			alShadow->updateDataMask(MeshModel::MM_VERTCOLOR);
			alShadow->UpdateBoxAndNormals();
			alShadow->Enable(tri::io::Mask::IOM_VERTCOLOR);
			alShadow->updateDataMask(MeshModel::MM_VERTCOLOR);
			Color4b newColor = Color4b(0, 0, 0, 255);
			tri::UpdateColor<CMeshO>::PerVertexConstant(alShadow->cm, newColor);
			tri::UpdateNormal<CMeshO>::PerVertex(alShadow->cm);
			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("FP_TEST_GENERATE_POINTS_FROM_GRAY_IMAGE"));
			////==========end first method=======================================================================

			//=========second method============//
			//MeshModel *alShadow = md.addNewMesh("a", "alShadow", false, RenderMode(GLW::DMFlat));//backup														
			//for (int x = 0; x < md.pointsInFrontOfMesh.size(); x++)
			//{
			// //===build points Mesh=====///
			// CMeshO::VertexIterator vii;
			// vii = tri::Allocator<CMeshO>::AddVertices(alShadow->cm, 1);
			// CMeshO::VertexPointer  ptVec[1];
			// ptVec[0] = &*vii;
			// vii->P() = CMeshO::CoordType(md.pointsInFrontOfMesh[x].X(), md.pointsInFrontOfMesh[x].Y(), md.pointsInFrontOfMesh[x].Z());
			// alShadow->cm.vert.back().ImportData(*ptVec[0]);
			//}
			//for (int x = 0; x < pointCloud2.size(); x++)
			//{														 
			// CMeshO::VertexIterator vii;
			// vii = tri::Allocator<CMeshO>::AddVertices(alShadow->cm, 1);
			// CMeshO::VertexPointer  ptVec[1];
			// ptVec[0] = &*vii;
			// vii->P() = CMeshO::CoordType(pointCloud2[x].X() + 3, pointCloud2[x].Y(), pointCloud2[x].Z());
			// alShadow->cm.vert.back().ImportData(*ptVec[0]);
			//}
			//for (int x = 0; x < md.pointsInFrontOfMesh.size(); x++)
			//{														 
			// CMeshO::VertexIterator vii;
			// vii = tri::Allocator<CMeshO>::AddVertices(alShadow->cm, 1);
			// CMeshO::VertexPointer  ptVec[1];
			// ptVec[0] = &*vii;
			// vii->P() = CMeshO::CoordType(md.pointsInFrontOfMesh[x].X() + 6, md.pointsInFrontOfMesh[x].Y(), md.pointsInFrontOfMesh[x].Z());
			// alShadow->cm.vert.back().ImportData(*ptVec[0]);
			//}
			////tri::Clean<CMeshO>::RemoveDuplicateVertex(alShadow->cm);
			//alShadow->setMeshSort(MeshModel::meshsort::pre_print_item);
			//alShadow->Enable(vcg::tri::io::Mask::IOM_VERTCOLOR);
			//alShadow->updateDataMask(MeshModel::MM_VERTCOLOR);
			//alShadow->UpdateBoxAndNormals();
			//alShadow->Enable(tri::io::Mask::IOM_VERTCOLOR);
			//alShadow->updateDataMask(MeshModel::MM_VERTCOLOR);
			//Color4b newColor = Color4b(255, 0, 0, 255);
			//tri::UpdateColor<CMeshO>::PerVertexConstant(alShadow->cm, newColor);
			//tri::UpdateNormal<CMeshO>::PerVertex(alShadow->cm);
			////===compute normal
			//tri::PointCloudNormal<CMeshO>::Param p;
			//p.fittingAdjNum = 20;
			//p.smoothingIterNum = 0;
			////p.viewPoint = vcg::Point3f(-200, 0, -10);
			//p.useViewPoint = false;
			//tri::PointCloudNormal<CMeshO>::Compute(alShadow->cm, p, cb);
			////===ballpivoting
			//float Radius = 5;
			//float Clustering = 10 / 100.0f;
			//float CreaseThr = math::ToRad(90.);
			//bool DeleteFaces = false;
			//if (DeleteFaces) 
			//{
			// alShadow->cm.fn = 0;
			// alShadow->cm.face.resize(0);
			//}
			//alShadow->updateDataMask(MeshModel::MM_VERTFACETOPO);
			//int startingFn = alShadow->cm.fn;
			//tri::BallPivoting<CMeshO> pivot(alShadow->cm, Radius, Clustering, CreaseThr);
			//// the main processing
			//pivot.BuildMesh(cb);
			//alShadow->clearDataMask(MeshModel::MM_FACEFACETOPO);
			////Log("Reconstructed surface. Added %i faces", alShadow->cm.fn - startingFn);
			////==========end second method=======================================================================

			// //=========third method============//
			// //create two mesh 
			// // const Scalarm d = par.getFloat("Delta");
			// // const Point3m delta(d, d, d);
			// // const int subFreq = par.getInt("SubDelta");


			// vcg::Point3f pos = md.pointsInFrontOfMesh[0];
			// MeshModel* collectMesh = md.addNewMesh("", "temp");
			// int sz = 2;
			// float tempsize = 0.6;
			// float boxsize = 0.5;// 1 / (inDPI / 25.4);
			// Box3m b(Point3m(tempsize, boxsize, boxsize)*(-sz / 2.) + pos, Point3m(tempsize, boxsize, boxsize)*(sz / 2.) + pos);
			// //Box3m b(Point3m(witnessWidth, boxsize, boxsize)*(-stepss / 2.) + pos, Point3m(boxsize, boxsize, boxsize)*(stepss / 2.) + pos);
			// tri::Box<CMeshO>(collectMesh->cm, b);
			///* m1->UpdateBoxAndNormals();
			// m1->clearDataMask(MeshModel::MM_FACEFACETOPO);*/
			// //tri::UpdateNormal<CMeshO>::PerFace(collectMesh->cm);

			// collectMesh->updateDataMask(MeshModel::MM_POLYGONAL);
			// vcg::tri::UpdateBounding<CMeshO>::Box(collectMesh->cm);
			// vcg::tri::UpdateNormal<CMeshO>::PerFaceFromCurrentVertexNormal(collectMesh->cm);
			// collectMesh->updateDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACENORMAL | MeshModel::MM_FACEQUALITY);
			// collectMesh->updateDataMask(MeshModel::MM_FACENORMAL | MeshModel::MM_FACEQUALITY);


			// typedef Intercept<mpq_class, Scalarm> intercept;
			// const Scalarm d = stepss / (inDPI / 25.4);// par.getFloat("Delta");
			// const Point3m delta(d, d, d);
			// const int subFreq = 32;// par.getInt("SubDelta");
			// InterceptVolume<intercept> v = InterceptSet3<intercept>(collectMesh->cm, delta, subFreq);
			// md.delMesh(collectMesh);

			// MeshModel *alShadow = md.addNewMesh("a", "alShadow", false, RenderMode(GLW::DMFlat));//backup														
			// for (int x = 1; x < md.pointsInFrontOfMesh.size(); x++)														 
			// //for (int x = 1; x <10; x++)
			// {
			//	 cb((x * 100) / md.pointsInFrontOfMesh.size(), "GENERATE_POINTS_FROM_GRAY_IMAGE");
			//	 pos = md.pointsInFrontOfMesh[x];
			//	 MeshModel* combineMesh = md.addNewMesh("", "temp");
			//
			// Box3m b2(Point3m(tempsize, boxsize, boxsize)*(-sz / 2.) + pos, Point3m(tempsize, boxsize, boxsize)*(sz / 2.) + pos);
			//	 //Box3m b2(Point3m(witnessWidth, boxsize, boxsize)*(-stepss / 2.) + pos, Point3m(boxsize, boxsize, boxsize)*(stepss / 2.) + pos);
			//	 tri::Box<CMeshO>(combineMesh->cm, b2);
			//	 //tri::UpdateNormal<CMeshO>::PerFace(combineMesh->cm);

			//	/* m2->UpdateBoxAndNormals();
			//	 m2->clearDataMask(MeshModel::MM_FACEFACETOPO);*/

			//	 combineMesh->updateDataMask(MeshModel::MM_POLYGONAL);
			//	 vcg::tri::UpdateBounding<CMeshO>::Box(combineMesh->cm);
			//	 vcg::tri::UpdateNormal<CMeshO>::PerFaceFromCurrentVertexNormal(combineMesh->cm);
			//	 combineMesh->updateDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACENORMAL | MeshModel::MM_FACEQUALITY);
			//	 combineMesh->updateDataMask(MeshModel::MM_FACENORMAL | MeshModel::MM_FACEQUALITY);


			//	 InterceptVolume<intercept> tmp = InterceptSet3<intercept>(combineMesh->cm, delta, subFreq);
			//	 md.delMesh(combineMesh);
			//	 v |= tmp;
			//	 //vcg::tri::Append<CMeshO, CMeshO>::Mesh(alShadow->cm, m->cm);

			// }
			//
			// typedef vcg::intercept::Walker<CMeshO, intercept> MyWalker;
			// typedef vcg::tri::MarchingCubes<CMeshO, MyWalker> MyMarchingCubes;
			// MyWalker walker;
			// MyMarchingCubes mc(alShadow->cm, walker);
			// walker.BuildMesh<MyMarchingCubes>(alShadow->cm, v, mc, cb);

			// tri::Clean<CMeshO>::FlipNormalOutside(alShadow->cm);
			// alShadow->UpdateBoxAndNormals();
			// alShadow->clearDataMask(MeshModel::MM_FACEFACETOPO);

			// vcg::tri::UpdateBounding<CMeshO>::Box(alShadow->cm);
			// vcg::tri::UpdateNormal<CMeshO>::PerFaceFromCurrentVertexNormal(alShadow->cm);

			// alShadow->UpdateBoxAndNormals();
			// alShadow->Enable(vcg::tri::io::Mask::IOM_VERTCOLOR);
			// alShadow->setMeshSort(MeshModel::meshsort::pre_print_item);
			// alShadow->updateDataMask(MeshModel::MM_VERTCOLOR);														 
			// alShadow->Enable(tri::io::Mask::IOM_VERTCOLOR);
			// alShadow->updateDataMask(MeshModel::MM_VERTCOLOR);
			// Color4b newColor = Color4b(128, 0, 0, 255);
			// tri::UpdateColor<CMeshO>::PerVertexConstant(alShadow->cm, newColor);




			// // tri::Clean<CMeshO>::RemoveDuplicateVertex(alShadow->cm);
			// // alShadow->setMeshSort(MeshModel::meshsort::pre_print_item);
			// // alShadow->Enable(vcg::tri::io::Mask::IOM_VERTCOLOR);
			// // alShadow->updateDataMask(MeshModel::MM_VERTCOLOR);
			// // alShadow->UpdateBoxAndNormals();
			// // alShadow->Enable(tri::io::Mask::IOM_VERTCOLOR);
			// // alShadow->updateDataMask(MeshModel::MM_VERTCOLOR);
			// // Color4b newColor = Color4b(0, 0, 0, 255);
			// // tri::UpdateColor<CMeshO>::PerVertexConstant(alShadow->cm, newColor);
			//// // tri::UpdateNormal<CMeshO>::PerVertex(alShadow->cm);
			// //=========end third method============//





		}




	}break;
#pragma endregion FP_TEST_GENERATE_POINTS_FROM_GRAY_IMAGE
#pragma region FP_TEST_GENERATE_POINTS_FROM_GRAY_IMAGE_FLOAT

	case FP_TEST_GENERATE_POINTS_FROM_GRAY_IMAGE_FLOAT://pending
	{
		WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("FP_TEST_GENERATE_POINTS_FROM_GRAY_IMAGE"));
		float witnessWidth = par.getFloat("WITNESS_WIDTH");

		if (md.multiSelectID.size() == 0)
		{
			foreach(MeshModel *mm, md.meshList)
			{
				if (mm->getMeshSort() == MeshModel::meshsort::print_item)
					md.multiSelectID.insert(mm->id());
				else return false;
			}
		}

		md.pointsInFrontOfMesh.clear();
		QList<MeshModel *> dispiteSelelctMesh = md.despiteSelectedMesh();
		int grooveHalfXPixel = md.groove.DimX() * 25 / (DSP_inchmm * 2);
		int grooveHalfYPixel = md.groove.DimY() * 25 / (DSP_inchmm * 2);


		QVector<bool> genNum(6, true);
		genNum[0] = false;
		genNum[1] = false;
		genNum[2] = false;
		genNum[3] = true;
		genNum[4] = false;
		genNum[5] = false;
		int inDPI = 50;
		//foreach(int id, md.multiSelectID)
		foreach(int id, md.multiSelectID)
		{

			md.pointsInFrontOfMesh.clear();
			//move to right back top corner
			MeshModel *bm = md.getMesh(id);


			std::vector<cv::Mat> baseSaveImage;
			std::vector<cv::Mat> selectSaveImage;


			QList<MeshModel *> selectMesh;
			Box3m mmmBox(bm->cm.bbox);
			selectMesh.push_back(bm);
			int pixelRatio = 1;//?pixel 1mm

			testDepthBuffer_float(selectMesh, selectSaveImage, md.groove, genNum, false, false, 1, pixelRatio);

			int xinGroove, yinGroove, zinGroove;


			float stepss = 3;
			float halfStepess = stepss / 2.;
			std::vector<vcg::Point3f> pointCloud;

			std::vector<vcg::Point3f> pointCloud2;
			int witnessOffset = 5;
			SKT::GeneratePointsFromGrayImage_float(selectSaveImage, md.pointsInFrontOfMesh, bm->cm.bbox.min.X(), witnessOffset, stepss, pixelRatio, inDPI);
			//SKT::GeneratePointsFromGrayImage2(selectSaveImage, pointCloud, bm->cm.bbox.min.X(), stepss, inDPI);
			//SKT::GeneratePointsFromContour(selectSaveImage, pointCloud2, bm->cm.bbox.min.X(), 10);
			//qDebug() << time.elapsed();

			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("FP_TEST_GENERATE_POINTS_FROM_GRAY_IMAGE"));
			//=========first method============//
			//float boxsize = 1 / (inDPI / 25.4);
			float boxsize = 1. / pixelRatio;
			MeshModel *alShadow = md.addNewMesh("a", "alShadow", false, RenderMode(GLW::DMFlat));//backup														
			//MeshModel *alShadow2 = md.addNewMesh("a2", "alShadow2", false, RenderMode(GLW::DMFlat));//backup														

#if 0
			//===create Model R_Tree==//
			CMeshO::FaceIterator fi;
			fi = bm->cm.face.begin();
			//fi->SetS();
			int i2 = 0, nhits2;
			/*create R Tree*/
			RTree<int, float, 3, float> tree3;
			for (fi = bm->cm.face.begin(); fi != bm->cm.face.end(); ++fi)
			{
				/*if (!(*fi).IsD() && (*fi).IsS() && !(*fi).IsV())
				visitStack.push_back(&*fi);*/
				vcg::Point3f  v0 = fi->V(0)->P();

				float sx1 = fi->V(0)->P().X();
				float sx2 = fi->V(1)->P().X();
				float sx3 = fi->V(2)->P().X();

				float sy1 = fi->V(0)->P().Y();
				float sy2 = fi->V(1)->P().Y();
				float sy3 = fi->V(2)->P().Y();

				float sz1 = fi->V(0)->P().Z();
				float sz2 = fi->V(1)->P().Z();
				float sz3 = fi->V(2)->P().Z();

				float bmin[3];
				float bmax[3];

				bmax[0] = sx1 > sx2 ? (sx1 > sx3 ? sx1 : sx3) : (sx2 > sx3 ? sx2 : sx3);
				bmax[1] = sy1 > sy2 ? (sy1 > sy3 ? sy1 : sy3) : (sy2 > sy3 ? sy2 : sy3);
				bmax[2] = sz1 > sz2 ? (sz1 > sz3 ? sz1 : sz3) : (sz2 > sz3 ? sz2 : sz3);

				bmin[0] = sx1 < sx2 ? (sx1 < sx3 ? sx1 : sx3) : (sx2 < sx3 ? sx2 : sx3);
				bmin[1] = sy1 < sy2 ? (sy1 < sy3 ? sy1 : sy3) : (sy2 < sy3 ? sy2 : sy3);
				bmin[2] = sz1 < sz2 ? (sz1 < sz3 ? sz1 : sz3) : (sz2 < sz3 ? sz2 : sz3);

				tree3.Insert(bmin, bmax, i2);
				i2++;

			}
			//===end create Model R_Tree==//
#endif
			//FILE *dbgff = fopen("D:/ouputPC.txt", "w");//@@@;

			for (int x = 0; x < md.pointsInFrontOfMesh.size(); x++)
			{
				cb((x * 100) / md.pointsInFrontOfMesh.size(), "GENERATE_POINTS_FROM_GRAY_IMAGE");
				vcg::Point3f pos = md.pointsInFrontOfMesh[x];
				/*
				output Point Cloud
				QString tempPC = QString("%1 %2 %3\n").arg(pos[0], 0, 'f', 3).arg(pos[1], 0, 'f', 3).arg(pos[2], 0, 'f', 3);
				fprintf(dbgff, tempPC.toStdString().c_str()); //@@@
				fflush(dbgff);
				*/
				Box3m b;

				if ((pos.Z() - bm->cm.bbox.min.Z()) < stepss)
				{
					pos.Z() = bm->cm.bbox.min.Z() + halfStepess;
					b.Import(Box3m(Point3m(2, boxsize, boxsize)*(-halfStepess) + pos, Point3m(boxsize, boxsize, boxsize)*(halfStepess)+pos));
					b.max.Z() += (pos.Z() - bm->cm.bbox.min.Z());
				}
				else if ((pos.Z() + stepss) > bm->cm.bbox.max.Z())
				{
					pos.Z() = bm->cm.bbox.max.Z() - halfStepess;
					b.Import(Box3m(Point3m(2, boxsize, boxsize)*(-halfStepess) + pos, Point3m(boxsize, boxsize, boxsize)*(halfStepess)+pos));

				}
				else
				{
					b.Import(Box3m(Point3m(2, boxsize, boxsize)*(-halfStepess) + pos, Point3m(boxsize, boxsize, boxsize)*(halfStepess)+pos));
					//(Point3m(2, boxsize, boxsize)*(-halfStepess) + pos, Point3m(boxsize, boxsize, boxsize)*(halfStepess / 2) + pos);
				}
				//Box3m b(Point3m(2, boxsize, boxsize)*(-halfStepess) + pos, Point3m(boxsize, boxsize, boxsize)*(halfStepess / 2) + pos);


				bool cbreak = false;
				/* for (int x = 0; x < pointCloud.size(); x++)
				{
				vcg::Point3f pos2 = pointCloud[x];

				if (b.IsIn(pos2))
				cbreak = false;
				}
				if (!cbreak)
				continue;*/
				/*test intersect*/
				/*
				float bmin[3] = { b.min[0], b.min[1], b.min[2] };
				float bmax[3] = { b.max[0], b.max[1], b.max[2] };
				cbreak = tree3.Search(bmin, bmax, MySearchCallback, NULL);
				if (cbreak)continue;//Test Intersection
				*/
				MeshModel* m = md.addNewMesh("", "temp");
				tri::Box<CMeshO>(m->cm, b);
				m->updateDataMask(MeshModel::MM_POLYGONAL);
				//m->UpdateBoxAndNormals();
				vcg::tri::Append<CMeshO, CMeshO>::Mesh(alShadow->cm, m->cm);
				md.delMesh(m);
				/*
				//===build points Mesh=====///
				CMeshO::VertexIterator vii;
				vii = tri::Allocator<CMeshO>::AddVertices(alShadow->cm, 1);
				CMeshO::VertexPointer  ptVec[1];
				ptVec[0] = &*vii;
				vii->P() = CMeshO::CoordType(md.pointsInFrontOfMesh[x].X(), md.pointsInFrontOfMesh[x].Y(), md.pointsInFrontOfMesh[x].Z());
				alShadow->cm.vert.back().ImportData(*ptVec[0]);
				*/
			}
			//dbgff->close();
			tri::Clean<CMeshO>::RemoveDuplicateVertex(alShadow->cm);
			alShadow->setMeshSort(MeshModel::meshsort::pre_print_item);
			alShadow->Enable(vcg::tri::io::Mask::IOM_VERTCOLOR);
			alShadow->updateDataMask(MeshModel::MM_VERTCOLOR);
			alShadow->UpdateBoxAndNormals();
			alShadow->Enable(tri::io::Mask::IOM_VERTCOLOR);
			alShadow->updateDataMask(MeshModel::MM_VERTCOLOR);
			alShadow->glw.SetHint(GLW::Hint::HNUseVBO);
			Color4b newColor = Color4b(0, 0, 0, 255);
			tri::UpdateColor<CMeshO>::PerVertexConstant(alShadow->cm, newColor);
			tri::UpdateNormal<CMeshO>::PerVertex(alShadow->cm);
			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("FP_TEST_GENERATE_POINTS_FROM_GRAY_IMAGE"));
			////==========end first method=======================================================================


		}




	}break;
#pragma endregion FP_TEST_GENERATE_POINTS_FROM_GRAY_IMAGE_FLOAT
#pragma region FP_TEST_WITNESS_BAR
	case FP_TEST_WITNESS_BAR:
	{
		foreach(int id, md.multiSelectID)
		{
			MeshModel *mdmm = md.getMesh(id);


			MeshModel *alShadow = md.addNewMesh("a", "alShadow", false, RenderMode(GLW::DMFlat));
			//MeshModel* m = md.addNewMesh("", "temp");
			Point3m minbb(mdmm->cm.bbox.max.X() + 5, mdmm->cm.bbox.min.Y(), mdmm->cm.bbox.min.Z());
			Point3m maxbb(mdmm->cm.bbox.max.X() + 10, mdmm->cm.bbox.max.Y(), mdmm->cm.bbox.max.Z());
			Box3m b(minbb, maxbb);
			tri::Box<CMeshO>(alShadow->cm, b);

			/*m->updateDataMask(MeshModel::MM_POLYGONAL);
			vcg::tri::Append<CMeshO, CMeshO>::Mesh(alShadow->cm, m->cm);*/
			alShadow->setMeshSort(MeshModel::meshsort::pre_print_item);
			alShadow->Enable(vcg::tri::io::Mask::IOM_VERTCOLOR);
			alShadow->updateDataMask(MeshModel::MM_VERTCOLOR);
			alShadow->UpdateBoxAndNormals();
			alShadow->Enable(tri::io::Mask::IOM_VERTCOLOR);
			alShadow->updateDataMask(MeshModel::MM_VERTCOLOR);
			alShadow->glw.SetHint(GLW::Hint::HNUseVBO);
			Color4b newColor = Color4b(0, 0, 0, 255);
			tri::UpdateColor<CMeshO>::PerVertexConstant(alShadow->cm, newColor);
			tri::UpdateNormal<CMeshO>::PerVertex(alShadow->cm);
		}

	}break;
#pragma endregion FP_TEST_WITNESS_BAR
#pragma region FP_MESH_INTERSECT_TEST2
	case FP_MESH_INTERSECT_TEST2://
	{
		//								   WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("FP_MESH_INTERSECT_TEST"));
		//
		//
		//								   md.jobname.clear();
		//								   //***************************
		//
		//								   GLenum err = glewInit();
		//								   if (GLEW_OK != err)
		//								   {
		//
		//								   }
		//
		//								
		//
		//								   Context ctx;
		//								   ctx.acquire();
		//
		//
		//								   QSet<MeshModel *> overlappingMeshes;
		//
		//								   for (QList<MeshModel *>::const_iterator ii = md.meshList.constBegin(); ii != md.meshList.constEnd(); ii++)
		//								   {
		//									   QList<MeshModel *>::const_iterator i2 = ii;
		//									   i2++;
		//									   for (; i2 != md.meshList.constEnd(); i2++)
		//									   {
		//										   if (md.isPrint_item(**ii) && md.isPrint_item(**i2))
		//										   if ((*ii)->cm.bbox.Collide((*i2)->cm.bbox))
		//										   {
		//											   overlappingMeshes.insert((*ii));
		//											   overlappingMeshes.insert((*i2));
		//										   }
		//
		//									   }
		//
		//								   }
		//								   if (overlappingMeshes.size() == 0)return true;
		//								   Box3m FullBBox;
		//								   Matrix44m iden;
		//								   iden.SetIdentity();
		//								   foreach(MeshModel *mp, overlappingMeshes)
		//								   {
		//									   FullBBox.Add(iden, mp->cm.bbox);
		//								   }
		//
		//								   QString temppath = PicaApplication::getRoamingDir();
		//								   QDir dir(temppath);//backup
		//								   if (dir.exists("xyzimage"))
		//								   {
		//									   dir.setPath(temppath + "/xyzimage");
		//									   dir.removeRecursively();
		//									   dir.setPath(temppath);
		//									   dir.mkpath("xyzimage");
		//								   }
		//								   else
		//								   {
		//									   dir.setPath(temppath);
		//									   dir.mkpath("xyzimage");
		//								   }
		//
		//								   dir.setPath(temppath + "/xyzimage");
		//								   //**********************
		//								
		//#if 1
		//								 
		//								   QString olMName = md.p_setting.getOlMeshName();
		//								   QString cMName = md.p_setting.getCapMeshName();
		//
		//								   //***opencvMat clear
		//								   float meCorrectPercent = 1;
		//
		//
		//								   int widthrecord, heightrecord;
		//								   QString blk_img_file_name = "black_outline";
		//
		//
		//								   std::vector<cv::Mat> blankTemp;
		//								   std::vector<cv::Mat> blankcapCTemp;
		//
		//								   QTime time;
		//								   //clock_t start, stop;
		//								   time.start();
		//
		//								   QDir patternPath(PicaApplication::getRoamingDir());
		//
		//								   bool testswitch = false;
		//								   int generateNOI = 20;
		//								   int quickWipe = 1;
		//								   bool savedebugImage = false;// par.getBool(("generate_final_picture"));
		//
		//								   std::map<int, std::vector<cv::Mat>> meshCubeImage;
		//								   std::vector<cv::Mat> selectSaveImage;
		//								   QVector<bool> genNum(6, true);
		//								   genNum[2] = true;
		//								   genNum[3] = true;
		//								   genNum[4] = true;
		//								   genNum[5] = true;
		//								   foreach(MeshModel *mp, overlappingMeshes)
		//								   {
		//									   QList<MeshModel *> overlappingMesh;
		//									   std::vector<cv::Mat> baseSaveImage;
		//									   overlappingMesh.push_back(mp);
		//									   testDepthBuffer(overlappingMesh, baseSaveImage, md.groove, genNum);
		//									   meshCubeImage.insert(std::pair<int, std::vector<cv::Mat>>(mp->id(), baseSaveImage));
		//								   }
		//								   ;
		//								   
		//								   //std::multimap<int, int> * tempIntersectMeshesID = md.getIntersectMeshesID();								  
		//								   md.setIntersectMeshesID2(SKT::checkImageOverlap2(meshCubeImage));
		//
		//#endif	 
		//								 
		//
		//								   foreach(MeshModel *bm, md.meshList)
		//								   {
		//									   if ((bm->label().contains(olMName, Qt::CaseSensitive) || bm->label().contains(cMName, Qt::CaseSensitive)))
		//										   //if (bm->label().contains(olMName, Qt::CaseSensitive) || bm->label().contains(cMName, Qt::CaseSensitive)))
		//										   //if (bm->getMeshSort()==MeshModel::meshsort::)
		//									   {
		//										   md.delMesh(bm);
		//									   }
		//								   }
		//								   qDebug() << " done _applyClick ";

		//================================start test six image point==================
		//QVector<bool> genNum(6, true);
		//genNum[0] = true;
		//genNum[1] = true;
		//genNum[2] = true;
		//genNum[3] = true;
		//genNum[4] = true;
		//genNum[5] = true;
		//int inDPI = 50;
		//foreach(int id, md.multiSelectID)
		//{
		// md.pointsInFrontOfMesh.clear();
		// //move to right back top corner
		// MeshModel *bm = md.getMesh(id);
		// std::vector<cv::Mat> baseSaveImage;
		// std::vector<cv::Mat> selectSaveImage;
		// QList<MeshModel *> selectMesh;
		// Box3m mmmBox(bm->cm.bbox);
		// selectMesh.push_back(bm);
		// testDepthBuffer(selectMesh, selectSaveImage, md.groove, genNum, false, inDPI);
		// int xinGroove, yinGroove, zinGroove;
		// //QTime time;
		// ////clock_t start, stop;
		// //time.start();
		// float stepss = 3;
		// std::vector<vcg::Point3f> pointCloud;
		// std::vector<vcg::Point3f> pointCloud2;
		// int witnessOffset = 10;
		// SKT::GeneratePointsFromGrayImage(selectSaveImage, md.pointsInFrontOfMesh, bm->cm.bbox.min.X(), witnessOffset, stepss, inDPI);
		//}
		//================================end test six image point==================

		//bool test = xyz::();


		WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("FP_MESH_INTERSECT_TEST"));


		md.jobname.clear();
		//***************************

		glContext->makeCurrent();


		GLenum err = glewInit();
		if (GLEW_OK != err)
		{

		}





		QMap<int, QVector<int>> overLapMeshMap;

		foreach(MeshModel *mm, md.meshList)
		{
			QVector<int> temp;
			foreach(MeshModel *mm2, md.meshList)
			{
				if (mm->id() != mm2->id())
				{
					if (md.isPrint_item(*mm) && md.isPrint_item(*mm2))
						if (mm->cm.bbox.Collide(mm2->cm.bbox))
						{
							temp.push_back(mm2->id());
						}
				}
			}
			if (temp.size() > 0)
				overLapMeshMap.insert(mm->id(), temp);
		}


		QMapIterator<int, QVector<int>> i(overLapMeshMap);
		std::set<int> intersectMeshes;
		while (i.hasNext()) {
			i.next();

			MeshModel *mm = md.getMesh(i.key());
			CMeshO::FaceIterator fi;
			RTree<int, float, 3, float> treeMM;
			int treeIdCount = 0;
			for (fi = mm->cm.face.begin(); fi != mm->cm.face.end(); ++fi)
			{
				float bmin[3];
				float bmax[3];
				createBBox(fi->V(0)->P(), fi->V(1)->P(), fi->V(2)->P(), bmin, bmax);
				/* qDebug() << treeIdCount <<" "<< fi->V(0)->P().X() << fi->V(0)->P().Y() << fi->V(0)->P().Z();
				qDebug() << treeIdCount <<" "<< fi->V(1)->P().X() << fi->V(1)->P().Y() << fi->V(1)->P().Z();
				qDebug() << treeIdCount <<" "<< fi->V(2)->P().X() << fi->V(2)->P().Y() << fi->V(2)->P().Z();*/
				treeMM.Insert(bmin, bmax, treeIdCount);
				treeIdCount++;
			}

			QVector<int> tempVector = i.value();
			foreach(int overlapMesh, tempVector)
			{
				if (intersectMeshes.find(overlapMesh) != intersectMeshes.end())
					continue;
				int cbreak = 0;
				MeshModel *mm2 = md.getMesh(overlapMesh);
				CMeshO::FaceIterator fi2;
				bool b1 = true;
				for (fi2 = mm2->cm.face.begin(); fi2 != mm2->cm.face.end() && b1; ++fi2)
				{
					float b2min[3];
					float b2max[3];

					createBBox(fi2->V(0)->P(), fi2->V(1)->P(), fi2->V(2)->P(), b2min, b2max);
					cbreak = treeMM.Search(b2min, b2max, GenSliceJob::MySearchCallback, NULL);


					//treeIdCount++;
					if (cbreak > 0)
					{

						bool realIntersect = false;

						foreach(int id, recordID)
						{
							CMeshO::FaceIterator fi = mm->cm.face.begin();
							advance(fi, id);
							realIntersect = vcg::IntersectionTriangleTriangle(fi->V(0)->P(), fi->V(1)->P(), fi->V(2)->P(), fi2->V(0)->P(), fi2->V(1)->P(), fi2->V(2)->P());
							if (realIntersect)
							{

								break;
							}

						}

						//advance(fi, cbreak);
						/* qDebug() << cbreak << " " << fi->V(0)->P().X() << fi->V(0)->P().Y() << fi->V(0)->P().Z();
						qDebug() << cbreak << " " << fi->V(1)->P().X() << fi->V(1)->P().Y() << fi->V(1)->P().Z();
						qDebug() << cbreak << " " << fi->V(2)->P().X() << fi->V(2)->P().Y() << fi->V(2)->P().Z();*/

						if (realIntersect)
						{
							intersectMeshes.insert(overlapMesh);
							intersectMeshes.insert(i.key());
							b1 = false;
						}

						recordID.clear();
					}

				}
			}
		}
		md.setIntersectMeshesID2(intersectMeshes);





	}break;
#pragma endregion FP_MESH_INTERSECT_TEST2
#pragma region FP_MESH_DELAUNAY_TRIANGULATION
	case FP_MESH_DELAUNAY_TRIANGULATION:
	{
		MeshModel *alCubeVolumn = md.addNewMesh("a", "alShadow", false, RenderMode(GLW::DMFlat));//backup														
		//MeshModel *alShadow2 = md.addNewMesh("a2", "alShadow2", false, RenderMode(GLW::DMFlat));//backup														
		/*test volumn Space*/
		float cellSize = 2;
		float half_cellSize = cellSize / 2.;

		Box3m  tempbox = md.mm()->cm.bbox;
		Box3m  tempbox2 = md.mm()->cm.bbox;
		//tempbox.min = Point3m(round(tempbox.min.X() - 0.5), round(tempbox.min.Y() - 0.5), round(tempbox.min.Z()-0.5));
		//tempbox.max = Point3m(round(tempbox.max.X() + 0.5), round(tempbox.max.Y() + 0.5), round(tempbox.max.Z()+0.5));
		//tempbox.min = Point3m(round(tempbox.min.X()) - 0.5, round(tempbox.min.Y()) - 0.5, round(tempbox.min.Z()) - 0.5);
		//tempbox.max = Point3m(round(tempbox.max.X()) + 0.5, round(tempbox.max.Y()) + 0.5, round(tempbox.max.Z()) + 0.5);

		tempbox.min = Point3m((tempbox.min.X()) - 0.5, (tempbox.min.Y()) - 0.5, (tempbox.min.Z()) - 0.5);
		tempbox.max = Point3m((tempbox.max.X()) + 0.5, (tempbox.max.Y()) + 0.5, (tempbox.max.Z()) + 0.5);
		float lineLength = sqrt(tempbox.DimX()*tempbox.DimX() + tempbox.DimY()*tempbox.DimY() + tempbox.DimZ()*tempbox.DimZ());

		//tempbox to fabs
		int zc = 0;
		int yc = 0;
		int xc = 0;
		float z_, y_, x_, countbb;

		MeshModel* m = md.getMeshByLabel("TruncOctTex.obj");
		/*tri::UpdatePosition<CMeshO>::Scale(m->cm, );
		m->UpdateBoxAndNormals();*/
		int total_size = (tempbox.DimX() / half_cellSize)*(tempbox.DimY() / half_cellSize)*(tempbox.DimZ() / half_cellSize);

		/*  Point3m v1(0, 0, 0);
		Point3m v2(0, 50, 0);
		Point3m v3(30, 0, 0);
		Box3m voxel(Point3m(-0.1, 0, 0), Point3m(1, 1, 1));

		bool triIntersectCube = xyz::triangleIntersectCube_v2(v1, v2, v3, voxel);*/



		for (z_ = tempbox.min.Z(), countbb = 0, zc = 0; z_ < tempbox.max.Z() + cellSize; z_ += half_cellSize, zc++)
		{
			for (y_ = tempbox.min.Y(), yc = 0; y_ < tempbox.max.Y() + cellSize; y_ += half_cellSize, yc++)
			{

				for (x_ = tempbox.min.X(), xc = 0; x_ < (tempbox.max.X() + cellSize); x_ += half_cellSize, xc++, countbb++)
				{
					cb((countbb * 100) / total_size, "Generate Volumn Metric");
					//qDebug() <<"xyz"<< x_<<": "<<y_<<": "<<z_;
					if ((zc % 2 == 0 && yc % 2 == 0 && xc % 2 == 0) || (zc % 2 == 1 && yc % 2 == 1 && xc % 2 == 1))
					{
						Point3m tempMin(x_, y_, z_), tempMin2(x_, y_ + 1, z_ + 1);
						Point3m tempMax = tempMin + Point3m(1, 1, 1);
						Box3m rayb(tempMin, tempMax);
						Box3m voxel(tempMin, tempMax);


						float centerTest[3] = { voxel.Center().X(), voxel.Center().Y(), voxel.Center().Z() };
						float hSizeTest[3] = { voxel.max.X() - centerTest[0], voxel.max.Y() - centerTest[1], voxel.max.Z() - centerTest[2] };



#ifndef GEN_SURFACE
						/*volume*/

						Point3m dir(lineLength, lineLength, lineLength);

						CMeshO::FaceIterator fi;
						fi = md.mm()->cm.face.begin();
						fi->SetS();
						bool insideMesh = false;
						int intersections = 0;


						// 
						//MeshModel* m = md.addNewMesh("", "temp");
						//tri::Box<CMeshO>(m->cm, b);
						//m->updateDataMask(MeshModel::MM_POLYGONAL);
						////m->UpdateBoxAndNormals();
						//vcg::tri::Append<CMeshO, CMeshO>::Mesh(alCubeVolumn->cm, m->cm);
						//md.delMesh(m);

						for (; fi != md.mm()->cm.face.end(); ++fi)
						{
							if (!(*fi).IsD() && (*fi).IsS() && !(*fi).IsV());
							Point3m v1 = fi->V(0)->P();
							Point3m v2 = fi->V(1)->P();
							Point3m v3 = fi->V(2)->P();
							if (testIntersection(v1, v2, v3, tempMin, dir))
							{
								intersections++;
							}
						}
						//if (intersections % 2 == 1) {												  
						// MeshModel* m = md.addNewMesh("", "temp");
						// tri::Box<CMeshO>(m->cm, b);
						///* m->cm.face.EnableFFAdjacency();
						// vcg::tri::UpdateTopology<CMeshO>::FaceFace(m->cm);
						// tri::Sphere<CMeshO>(m->cm, 2);
						// m->updateDataMask(MeshModel::MM_POLYGONAL);
						// tri::UpdatePosition<CMeshO>::Translate(m->cm, tempMin);*/
						// //m->UpdateBoxAndNormals();
						// vcg::tri::Append<CMeshO, CMeshO>::Mesh(alCubeVolumn->cm, m->cm);
						// md.delMesh(m);
						//}		


						if (intersections % 2 == 1) {

							//Point3m abc = m->cm.bbox.Center();
							Point3m tempo = tempMin - m->cm.bbox.Center();
							/*  m->cm.face.EnableFFAdjacency();
							m->updateDataMask(MeshModel::MM_POLYGONAL);		*/
							tri::UpdatePosition<CMeshO>::Translate(m->cm, tempo);
							m->UpdateBoxAndNormals();
							vcg::tri::Append<CMeshO, CMeshO>::Mesh(alCubeVolumn->cm, m->cm);

						}




#else


						Point3m dir(half_cellSize, half_cellSize, half_cellSize), dir2(half_cellSize, -half_cellSize, -half_cellSize);

						CMeshO::FaceIterator fi;
						fi = md.mm()->cm.face.begin();
						fi->SetS();
						bool insideMesh = false;
						int intersections = 0;


						// 
						//MeshModel* m = md.addNewMesh("", "temp");
						//tri::Box<CMeshO>(m->cm, b);
						//m->updateDataMask(MeshModel::MM_POLYGONAL);
						////m->UpdateBoxAndNormals();
						//vcg::tri::Append<CMeshO, CMeshO>::Mesh(alCubeVolumn->cm, m->cm);
						//md.delMesh(m);

						for (; fi != md.mm()->cm.face.end(); ++fi)
						{
							if (!(*fi).IsD() && (*fi).IsS() && !(*fi).IsV());
							Point3m v1 = fi->V(0)->P();
							Point3m v2 = fi->V(1)->P();
							Point3m v3 = fi->V(2)->P();
							///*face min max*/
							float sx1 = fi->V(0)->P().X();
							float sx2 = fi->V(1)->P().X();
							float sx3 = fi->V(2)->P().X();

							float sy1 = fi->V(0)->P().Y();
							float sy2 = fi->V(1)->P().Y();
							float sy3 = fi->V(2)->P().Y();

							float sz1 = fi->V(0)->P().Z();
							float sz2 = fi->V(1)->P().Z();
							float sz3 = fi->V(2)->P().Z();

							Point3m facemin;
							Point3m facemax;

							/*facemax[0] = sx1 > sx2 ? (sx1 > sx3 ? sx1 : sx3) : (sx2 > sx3 ? sx2 : sx3);
							facemax[1] = sy1 > sy2 ? (sy1 > sy3 ? sy1 : sy3) : (sy2 > sy3 ? sy2 : sy3);
							facemax[2] = sz1 > sz2 ? (sz1 > sz3 ? sz1 : sz3) : (sz2 > sz3 ? sz2 : sz3);

							facemin[0] = sx1 < sx2 ? (sx1 < sx3 ? sx1 : sx3) : (sx2 < sx3 ? sx2 : sx3);
							facemin[1] = sy1 < sy2 ? (sy1 < sy3 ? sy1 : sy3) : (sy2 < sy3 ? sy2 : sy3);
							facemin[2] = sz1 < sz2 ? (sz1 < sz3 ? sz1 : sz3) : (sz2 < sz3 ? sz2 : sz3);

							Box3m faceb(facemin, facemax);

							if (!(rayb.Collide(faceb)))continue;*/

							//bool triIntersectCube = xyz::triangleIntersectCube(v1, v2, v3, voxel);
							bool triIntersectCube = xyz::triangleIntersectCube_v2(v1, v2, v3, voxel);



							//if (rayb.Collide(faceb))//(testIntersection(v1, v2, v3, tempMin, dir) || testIntersection(v1, v2, v3, tempMin2, dir2))
							//{
							// //intersections++;
							// Point3m tempo = tempMin - m->cm.bbox.Center();
							// tri::UpdatePosition<CMeshO>::Translate(m->cm, tempo);
							// m->UpdateBoxAndNormals();
							// vcg::tri::Append<CMeshO, CMeshO>::Mesh(alCubeVolumn->cm, m->cm);
							//}

							if (triIntersectCube)
							{
								//intersections++;
								Point3m tempo = tempMin - m->cm.bbox.Center();
								tri::UpdatePosition<CMeshO>::Translate(m->cm, tempo);
								m->UpdateBoxAndNormals();
								vcg::tri::Append<CMeshO, CMeshO>::Mesh(alCubeVolumn->cm, m->cm);
							}



						}



#endif // !GEN_SURFACE
					}



				}

			}
		}



		tri::Clean<CMeshO>::RemoveDuplicateVertex(alCubeVolumn->cm);
		alCubeVolumn->setMeshSort(MeshModel::meshsort::print_item);
		alCubeVolumn->glw.SetHint(GLW::Hint::HNUseVBO);

		alCubeVolumn->Enable(vcg::tri::io::Mask::IOM_VERTCOLOR);
		alCubeVolumn->updateDataMask(MeshModel::MM_VERTCOLOR);
		alCubeVolumn->UpdateBoxAndNormals();
		alCubeVolumn->Enable(tri::io::Mask::IOM_VERTCOLOR);
		alCubeVolumn->updateDataMask(MeshModel::MM_VERTCOLOR);
		Color4b newColor = Color4b(0, 0, 0, 255);
		tri::UpdateColor<CMeshO>::PerVertexConstant(alCubeVolumn->cm, newColor);
		tri::UpdateNormal<CMeshO>::PerVertex(alCubeVolumn->cm);

	}break;
#pragma endregion FP_MESH_DELAUNAY_TRIANGULATION

#pragma region FP_COUNT_BINDER_AREA

	case FP_COUNT_BINDER_AREA:
	{
		WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("FP_PRINT_WITH_WHITE_KICK"));


		md.jobname.clear();

		glContext->makeCurrent();


		GLenum err = glewInit();
		if (GLEW_OK != err)
		{

		}

		glPushAttrib(GL_ALL_ATTRIB_BITS);

		Context ctx;
		ctx.acquire();
		if (switchInitMeshVBO)
			initMeshVBO(md);

		//***20150907
		const GLsizei print_dpi = 100;// GLsizei(par.getInt("print_dpi"));
		float x = md.groove.DimX();
		float y = md.groove.DimY();
		const GLsizei width = (int)(x / DSP_inchmm * print_dpi);
		const GLsizei height = (int)(width * y / x);


		RenderbufferHandle hDepth = createRenderbuffer(ctx, GL_DEPTH24_STENCIL8, width, height);
		Texture2DHandle    hColor = createTexture2D(ctx, GL_RGB8, width, height, GL_RGB, GL_UNSIGNED_BYTE);
		//FramebufferHandle  hFramebuffer = createFramebuffer(ctx, renderbufferTarget(hDepth), texture2DTarget(hColor));
		FramebufferHandle  hFramebuffer = createFramebufferWithDepthStencil(ctx, renderbufferTarget(hDepth), renderbufferTarget(hDepth), texture2DTarget(hColor));




		const vcg::Point3f lightDirectionVS = vcg::Point3f(0.0f, 0.0f, -1.0f).Normalize();

		glEnable(GL_DEPTH_TEST);
		//glClearColor(GLfloat(backgroundColor.red())/255.0f, GLfloat(backgroundColor.green())/255.0f, GLfloat(backgroundColor.blue())/255.0f, 0.0f);
		Construction_Groove<float> tempgroove = md.groove;

		glViewport(0, 0, width, height);//***window的長寬				  



		//***儲存image的基本資訊
		QImage image(int(width), int(height), QImage::Format_RGB888);
		QImage image2(int(width), int(height), QImage::Format_RGB888);
		QImage slice0Point05mmImage(int(width), int(height), QImage::Format_RGB888);
		QImage cap_image(int(width), int(height), QImage::Format_RGB888);
		QImage cap_prePrting_image(int(width), int(height), QImage::Format_RGB888);
		//prePrintobjectC = *new Mat(height,width,CV_8UC3,Scalar(255, 255, 255));
		/*look_down_color_CV = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		look_down_Black_Mask_layer = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		look_down_Black_MaskCV = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		look_up_Black_MaskCV = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		look_up_color_CV = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		captemp = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		black_Mask = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		black_Mask2 = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		outlineBlackMaskCV = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		lookDownBlackTriangleMask = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		outlineColorCv = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		prePrintobjectC = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));

		downColorFirstCV = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		downBlackMaskFirstCV = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		downColorSecondCV = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		downBlackMaskSecondCV = *new Mat(height, width, CV_8UC3, Scalar(255, 255, 255));*/

		cv::Mat look_down_color_CV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat look_down_Black_Mask_layer(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat look_down_Black_MaskCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat look_up_Black_MaskCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat look_up_color_CV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat captemp(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat capDownMask(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat black_Mask(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat black_Mask2(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat outlineBlackMaskCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat lookDownBlackTriangleMask(height, width, CV_8UC3, Scalar(255, 255, 255));

		cv::Mat prePrintobjectC(height, width, CV_8UC3, Scalar(255, 255, 255));

		cv::Mat downColorFirstCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat downBlackMaskFirstCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat downColorSecondCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat downBlackMaskSecondCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat prePrintobjectB(height, width, CV_8UC1, Scalar(255));

		cv::Mat capDownMask_bottom_face(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat capDownMask_top_face(height, width, CV_8UC3, Scalar(255, 255, 255));


		cv::Mat color_Bottom_face(height, width, CV_8UC3, Scalar(255, 255, 255));



		cv::Mat color_Top_face(height, width, CV_8UC3, Scalar(255, 255, 255));



		cv::Mat originalOutline(height, width, CV_8UC3, Scalar(255, 255, 255));


		cv::Mat outlineColorCv(height, width, CV_8UC3, Scalar(255, 255, 255));


		cv::Mat capDownSureMask(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat capTopSureMask(height, width, CV_8UC3, Scalar(255, 255, 255));




		//***20150505***slice_func_設置高度
		float box_bottom = md.bbox().min.Z();
		float unit = par.getFloat("slice_height");



		int boxy_dim = qCeil((md.bbox().max.Z() - md.bbox().min.Z()) / unit) + 1;
		//int boxy_dim = qCeil((md.bbox().max.Z() - md.bbox().min.Z()) / unit);//for one page
		//int(mdd->bbox().DimZ() / pJI.layerThickness.value) + 1;
		//Log("boxy_dim %i", boxy_dim);

		init_texture(md);

		//*******************		


		QString temppath = PicaApplication::getRoamingDir();
		QDir dir(temppath);//backup
		if (dir.exists("xyzimage"))
		{
			dir.setPath(temppath + "/xyzimage");
			dir.removeRecursively();
			dir.setPath(temppath);
			dir.mkpath("xyzimage");
		}
		else
		{
			dir.setPath(temppath);
			dir.mkpath("xyzimage");
		}

		dir.setPath(temppath + "/xyzimage");

		//---zx_file------------------------------------------
		//QString zxtemp = "D:/temptemp";
		QString zxtemp = PicaApplication::getRoamingDir();
		QDir zx_dir(zxtemp);
		if (zx_dir.exists("zx_file"))
		{
			zx_dir.setPath(zxtemp + "/zx_file");
			//zx_dir.removeRecursively();
			zx_dir.setPath(zxtemp);
			zx_dir.mkpath("zx_file");
		}
		else
		{
			dir.setPath(temppath);
			dir.mkpath("zx_file");
		}

		zx_dir.setPath(zxtemp + "/zx_file");

		//==============================
		//**********************
		ctx.bindReadDrawFramebuffer(hFramebuffer);
		GLW_CHECK_GL_READ_DRAW_FRAMEBUFFER_STATUS;
#if 1

		const bool outputOL_Image = par.getBool("OL_Image");
		const bool save_cap_Image = par.getBool("Cap_Image");

		const bool faceColor = par.getBool("FaceColor");
		glClearColor(1, 1, 1, 1);
		QString olMName = md.p_setting.getOlMeshName();
		QString cMName = md.p_setting.getCapMeshName();

		//***opencvMat clear
		capCTemp.clear();
		outlineCTemp.clear();


		int zx_job_pages = par.getInt("per_zx_job");
		//int start_page = par.getInt("start_page");
		int send_page_num = par.getInt("send_page_num");
		int plus_pixel = par.getInt("plus_pixel");
		float PLUS_MM = par.getFloat("PLUS_MM");
		bool start_print_b = par.getBool("PRINT_START");
		float useless_print = par.getFloat("USELESS_PRINT");
		/*float spittoon_M = par.getFloat("SPITTOON_M");
		float spittoon_C = par.getFloat("SPITTOON_C");
		float spittoon_Y = par.getFloat("SPITTOON_Y");
		float spittoon_B = par.getFloat("SPITTOON_B");
		bool add_pattern = par.getBool("ADD_PATTERN");
		string desProfilePath = par.getString("COLOR_PROFILE").toStdString();
		QFileInfo tempProfilePath(QString::fromStdString(desProfilePath));
		bool doubleprint = par.getBool("Stiff_Print");
		bool dilateBinder = par.getBool("DILATE_BINDER");
		int dilateBinderValue = par.getInt("DILATE_BINDER_VALUE");
		bool dynamicValueSwitch = par.getBool("DYNAMIC_WIPE");
		bool colorBinding = par.getBool("COLOR_BINDING");*/


		bool prePrintBool = true;//witness bar




		bool continuePrinting = par.getBool(("CONTINUE_PRINTING"));
		int start_page = par.getInt("CONTINUE_PRINTING_PAGE");
		int end_page = par.getInt("CONTINUE_PRINTING_END_PAGE");


		//assert(tempProfilePath.exists());
		//float meCorrectPercent = 0.97;
		float meCorrectPercent = 1;

		//********control profile on && DSP_TEST_MODE

		bool savedebugImage = false;// par.getBool(("generate_final_picture"));
		bool line_or_quads = par.getBool("LINE_OR_RECT");;//true line, false quads

		SKT::pixelFormat gen_pixelFormat;
		ZxGen_SKT::pixelFormat genZxPixelFormat;
		ZxGen_SKT::zxInitMode zxCheckMode;





		int widthrecord, heightrecord;
		QString blk_img_file_name = "black_outline";

		QString zxFilePath = par.getString("ZX_File_Path");
		QString zx_sn = makeOutputname(zx_dir.absolutePath(), 0);




		md.jobname.push_back(zx_sn);


		std::vector<cv::Mat> blankTemp;
		std::vector<cv::Mat> blankcapCTemp;


		//if (start_page == 0)
		//{
		WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("createBlankPages"));


		QTime time;
		//clock_t start, stop;
		time.start();

		QDir patternPath(PicaApplication::getRoamingDir());

		if (patternPath.exists("pattern"))
		{
			patternPath.setPath(PicaApplication::getRoamingDir() + "/pattern");
		}
		else
		{
			patternPath.setPath(PicaApplication::getRoamingDir());
			patternPath.mkpath("pattern");
			patternPath.setPath(PicaApplication::getRoamingDir() + "/pattern");
			//patternPath.setPath( "./pattern");

		}
		cv::Mat patternImg;
		patternImg = cv::imread(patternPath.absolutePath().toStdString() + "/hexagon_4.png", CV_LOAD_IMAGE_GRAYSCALE);
		string srcProfilePath = PicaApplication::getRoamingDir().toStdString() + "/ColorProfile/sRGB2014.icc";


		int testPrintPageCount;// = 10;
		bool testswitch;// = false;


		int quickWipe = 1;
		WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("for loop start"));


		if (continuePrinting)
		{
			testPrintPageCount = end_page - start_page;
			boxy_dim = start_page + testPrintPageCount;
			testswitch = true;
		}
		else
		{
			start_page = 0;
			testswitch = false;
		}
		double totalDataPoint = 0;
		//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("gen log"));
		for (int i = start_page, j = start_page + 1; i < boxy_dim; i++, j++)
			//for (int i = start_page, j = start_page + 1; i < start_page+testPrintPageCount; i++, j++)
		{
			time.restart();
			//resetImage(look_down_Black_Mask_layer, width, height);//
			//resetImage(look_down_color_CV, width, height);
			//resetImage(look_down_Black_MaskCV, width, height);//
			//resetImage(look_up_Black_MaskCV, width, height);//
			//resetImage(look_up_color_CV, width, height);
			//resetImage(black_Mask, width, height);
			//resetImage(black_Mask2, width, height);
			//resetImage(lookDownBlackTriangleMask, width, height);//
			//resetImage(outlineBlackMaskCV, width, height);//
			//resetImage(prePrintobjectC, width, height);


			//resetImage(downColorFirstCV, width, height);
			//resetImage(downBlackMaskFirstCV, width, height);
			//resetImage(downColorSecondCV, width, height);
			//resetImage(downBlackMaskSecondCV, width, height);



			resetImage(outlineColorCv, width, height);
			resetImage(captemp, width, height);

			md.wipeFlag.push_back(true);
			if (cb && (i % 1) == 0)
			{
				glContext->doneCurrent();
				bool break_test = false;

				break_test = cb((i * 100) / boxy_dim, "slice_process");

			}
			glContext->makeCurrent();

			float currnentHeight = box_bottom + unit*i;

			//time.restart();
			//***第一張outline圖*****************************

			int xx = 0;
			QString img_file_name = md.p_setting.getoutlineName();
			float cut_z = box_bottom + unit*i;


			glPixelStorei(GL_PACK_ALIGNMENT, 1);//OpenGL PixelStori Setting



			//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T(""));
			//===========================================================================================
			equ[3] = box_bottom + unit*i;
			//***第二張圖***產生膠水圖**********//
			//time.restart();
			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -1000, 1000);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, -DSP_Look_Distination, 0, 1, 0);
			//*********************************
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);

			//binderstrips(md, par.getFloat("useless_print"));//***uselessbar

			//************************************************//
			//****** Rendering the mesh's clip edge ****//
			GLuint MY_CLIP_PLANE = GL_CLIP_PLANE0;
			glEnable(MY_CLIP_PLANE);
			glClipPlane(MY_CLIP_PLANE, equ);

			glEnable(GL_STENCIL_TEST);
			glEnable(GL_CULL_FACE);
			glClear(GL_STENCIL_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

			//// first pass: increment stencil buffer value on back faces
			glStencilFunc(GL_ALWAYS, 0, 0);
			glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
			glCullFace(GL_FRONT); // render back faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			//glRotatef(90, 1, 0, 0);
			foreach(MeshModel * mp, md.meshList)
			{
				if (mp->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mp->cm.bbox.min.Z() && currnentHeight <= mp->cm.bbox.max.Z()))
					//mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
					mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
			}

			glPopMatrix();
			glPopAttrib();
			//second pass: decrement stencil buffer value on front faces
			glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
			glCullFace(GL_BACK); // render front faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			//glRotatef(90, 1, 0, 0);
			foreach(MeshModel * mp, md.meshList)
			{
				if (mp->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mp->cm.bbox.min.Z() && currnentHeight <= mp->cm.bbox.max.Z()))
					//mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
					mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
			}
			glPopMatrix();
			glPopAttrib();
			//-----------------------------
			// drawing clip planes masked by stencil buffer content
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glEnable(GL_DEPTH_TEST);
			glDisable(MY_CLIP_PLANE);
			glStencilFunc(GL_NOTEQUAL, 0, ~0);
			// stencil test will pass only when stencil buffer value = 0; 
			// (~0 = 0x11...11)

			glColor4f(1, 1, 1, 1);
			fillsquare();
			glDisable(GL_STENCIL_TEST);
			glEnable(MY_CLIP_PLANE); // enabling clip plane again
			glDisable(GL_LIGHTING);

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			//********************************************************************************************//


			//glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, cap_image.bits());//***20150724
			glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, captemp.data);
			cv::flip(captemp, captemp, 0);

			QString img_file_name2 = md.p_setting.getcapImageName();
			glDisable(MY_CLIP_PLANE);//存完圖
			glDisable(GL_CULL_FACE);


			cv::cvtColor(captemp, captemp, CV_BGR2GRAY);

			if (savedebugImage){
				QImage testttt = cvMatToQImage(captemp);
				QString capimagest = "captemp_image";
				capimagest.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				testttt.save(dir.absolutePath() + "/" + capimagest, "png");
			}



			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();

			totalDataPoint += SKT::countBinderArea(captemp);

			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("generate_final_picture"));
		}
		WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("for loop end"));
		//thread1.exit();


#endif	

		ctx.unbindReadDrawFramebuffer();
		ctx.release();
		glPopAttrib();
		glContext->doneCurrent();





		QString fileName = temppath + "/countBinderArea.txt";





		QFile file(fileName);

		if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
		{

			std::cerr << "Cannot open file for writing: "

				<< qPrintable(file.errorString()) << std::endl;

		}

		QTextStream out(&file);
		out.setRealNumberNotation(QTextStream::FixedNotation);

		out << "\n" << totalDataPoint << " : print data \n"
			<< width*height << " : one layer data\n"
			<< width*height*(boxy_dim + 1) << ": total layers' data  \n"
			<< totalDataPoint / (width*height*(boxy_dim + 1)) << " : percent \n"
			<< boxy_dim + 1 << " Layers \n";



		file.close();


		foreach(MeshModel *bm, md.meshList)
		{
			if ((bm->label().contains(olMName, Qt::CaseSensitive) || bm->label().contains(cMName, Qt::CaseSensitive)))
				//if (bm->label().contains(olMName, Qt::CaseSensitive) || bm->label().contains(cMName, Qt::CaseSensitive)))
				//if (bm->getMeshSort()==MeshModel::meshsort::)
			{
				md.delMesh(bm);
			}
		}

		break;
	}

#pragma endregion FP_COUNT_BINDER_AREA
#pragma region FP_PRINT_FLOW_2_V3

	case FP_PRINT_FLOW_2_V3:
	{
		WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("FP_PRINT_WITH_WHITE_KICK"));
		testlog("FP_PRINT_FLOW_2_V3");

		md.jobname.clear();


		glContext->makeCurrent();


		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			/* Problem: glewInit failed, something is seriously wrong. */
			//fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
			//qDebug("Error: %s\n", glewGetErrorString(err));
		}

		glPushAttrib(GL_ALL_ATTRIB_BITS);

		Context ctx;
		ctx.acquire();
		if (switchInitMeshVBO)
			initMeshVBO(md);


		//***20150907
		const GLsizei print_dpi = GLsizei(par.getInt("print_dpi"));
		float x = md.groove.DimX();
		float y = md.groove.DimY();
		const GLsizei width = (int)(x / DSP_inchmm * print_dpi);
		const GLsizei height = (int)(width * y / x);

		//qDebug() << "width" << "height" << width << height;
		//RenderbufferHandle hDepth = createRenderbuffer(ctx, GL_DEPTH_COMPONENT24, width, height);
		RenderbufferHandle hDepth = createRenderbuffer(ctx, GL_DEPTH24_STENCIL8, width, height);
		Texture2DHandle    hColor = createTexture2D(ctx, GL_RGB8, width, height, GL_RGB, GL_UNSIGNED_BYTE);
		//FramebufferHandle  hFramebuffer = createFramebuffer(ctx, renderbufferTarget(hDepth), texture2DTarget(hColor));
		FramebufferHandle  hFramebuffer = createFramebufferWithDepthStencil(ctx, renderbufferTarget(hDepth), renderbufferTarget(hDepth), texture2DTarget(hColor));




		const vcg::Point3f lightDirectionVS = vcg::Point3f(0.0f, 0.0f, -1.0f).Normalize();

		glEnable(GL_DEPTH_TEST);
		//glClearColor(GLfloat(backgroundColor.red())/255.0f, GLfloat(backgroundColor.green())/255.0f, GLfloat(backgroundColor.blue())/255.0f, 0.0f);
		Construction_Groove<float> tempgroove = md.groove;

		glViewport(0, 0, width, height);//***window的長寬	

		//***儲存image的基本資訊
		QImage image(int(width), int(height), QImage::Format_RGB888);
		QImage image2(int(width), int(height), QImage::Format_RGB888);
		QImage slice0Point05mmImage(int(width), int(height), QImage::Format_RGB888);
		QImage cap_image(int(width), int(height), QImage::Format_RGB888);
		QImage cap_prePrting_image(int(width), int(height), QImage::Format_RGB888);
		//prePrintobjectC = *new Mat(height,width,CV_8UC3,Scalar(255, 255, 255));
		cv::Mat look_down_color_CV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat look_down_Black_Mask_layer(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat look_down_Black_MaskCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat look_up_Black_MaskCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat look_up_color_CV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat captemp(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat capDownMask(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat black_Mask(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat black_Mask2(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat outlineBlackMaskCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat lookDownBlackTriangleMask(height, width, CV_8UC3, Scalar(255, 255, 255));

		cv::Mat prePrintobjectC(height, width, CV_8UC3, Scalar(255, 255, 255));

		cv::Mat downColorFirstCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat downBlackMaskFirstCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat downColorSecondCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat downBlackMaskSecondCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat prePrintobjectB(height, width, CV_8UC1, Scalar(255));

		cv::Mat capDownMask_bottom_face(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat capDownMask_top_face(height, width, CV_8UC3, Scalar(255, 255, 255));


		cv::Mat color_Bottom_face(height, width, CV_8UC3, Scalar(255, 255, 255));



		cv::Mat color_Top_face(height, width, CV_8UC3, Scalar(255, 255, 255));



		cv::Mat originalOutline(height, width, CV_8UC3, Scalar(255, 255, 255));


		cv::Mat outlineColorCv(height, width, CV_8UC3, Scalar(255, 255, 255));


		cv::Mat capDownSureMask(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat capTopSureMask(height, width, CV_8UC3, Scalar(255, 255, 255));











		//***20150505***slice_func_設置高度
		float box_bottom = md.bbox().min.Z();
		float unit = par.getFloat("slice_height");



		int boxy_dim = qCeil((md.bbox().max.Z() - md.bbox().min.Z()) / unit) + 1;
		//int boxy_dim = qCeil((md.bbox().max.Z() - md.bbox().min.Z()) / unit);//for one page
		//int(mdd->bbox().DimZ() / pJI.layerThickness.value) + 1;
		//Log("boxy_dim %i", boxy_dim);

		init_texture(md);

		//***20150507 standard path******************
		//QString temppath = QStandardPaths::locate(QStandardPaths::TempLocation, "", QStandardPaths::LocateDirectory);
		//QString temppath = "D:/temptemp";//backup



		QString temppath = PicaApplication::getRoamingDir();
		QDir dir(temppath);//backup
		if (dir.exists("xyzimage"))
		{
			dir.setPath(temppath + "/xyzimage");
			dir.removeRecursively();
			dir.setPath(temppath);
			dir.mkpath("xyzimage");
		}
		else
		{
			dir.setPath(temppath);
			dir.mkpath("xyzimage");
		}

		dir.setPath(temppath + "/xyzimage");

		//---zx_file------------------------------------------
		//QString zxtemp = "D:/temptemp";
		QString zxtemp = PicaApplication::getRoamingDir();
		QDir zx_dir(zxtemp);
		if (zx_dir.exists("zx_file"))
		{
			zx_dir.setPath(zxtemp + "/zx_file");
			zx_dir.removeRecursively();
			zx_dir.setPath(zxtemp);
			zx_dir.mkpath("zx_file");
		}
		else
		{
			dir.setPath(temppath);
			dir.mkpath("zx_file");
		}

		zx_dir.setPath(zxtemp + "/zx_file");

		//==============================
		//**********************
		ctx.bindReadDrawFramebuffer(hFramebuffer);
		GLW_CHECK_GL_READ_DRAW_FRAMEBUFFER_STATUS;
#if 1

		const bool outputOL_Image = par.getBool("OL_Image");
		const bool save_cap_Image = par.getBool("Cap_Image");

		const bool faceColor = par.getBool("FaceColor");
		glClearColor(1, 1, 1, 1);
		QString olMName = md.p_setting.getOlMeshName();
		QString cMName = md.p_setting.getCapMeshName();

		//***opencvMat clear
		capCTemp.clear();
		outlineCTemp.clear();


		int zx_job_pages = par.getInt("per_zx_job");
		//int start_page = par.getInt("start_page");
		int send_page_num = par.getInt("send_page_num");
		int plus_pixel = par.getInt("plus_pixel");
		float PLUS_MM = par.getFloat("PLUS_MM");
		bool start_print_b = par.getBool("PRINT_START");
		float useless_print = par.getFloat("USELESS_PRINT");
		float spittoon_M = par.getFloat("SPITTOON_M");
		float spittoon_C = par.getFloat("SPITTOON_C");
		float spittoon_Y = par.getFloat("SPITTOON_Y");
		float spittoon_B = par.getFloat("SPITTOON_B");
		float spittoon_fucking_second_Layer_CMY = par.getFloat("SPITTOON_SECOND_CMY");;
		float spittoon_fucking_second_Layer_B = par.getFloat("SPITTOON_SECOND_B");;

		bool add_pattern = par.getBool("ADD_PATTERN");
		string desProfilePath = par.getString("COLOR_PROFILE").toStdString();
		QFileInfo tempProfilePath(QString::fromStdString(desProfilePath));
		bool doubleprint = false;// par.getBool("Stiff_Print");
		bool dilateBinder = par.getBool("DILATE_BINDER");
		int dilateBinderValue = par.getInt("DILATE_BINDER_VALUE");
		bool dynamicValueSwitch = par.getBool("DYNAMIC_WIPE");
		bool colorBinding = par.getBool("COLOR_BINDING");
		bool doublePrint_V2 = par.getBool("STIFF_PRIN_V2");
		int doublePrint_V2Percent = par.getInt("STIFF_PRINT_VALUE");

		int doublePrint_begin_twenty_layer_V2Percent = par.getInt("PP_BOTTOM_LIGHTER_PERCENTAGE");
		float doublePrint_begin_twenty_layer_V2Thick = par.getFloat("PP_BOTTOM_LIGHTER_THICK");
		//double pp_upperMask = 0.5;//Bottom Ink lighter mm


		int doublePrint_shell_Percent = par.getInt("SHELL_PERCENT");
		bool CMYB_black_mode = par.getBool("CMYB_BLACK");
		int CMYB_black_percentage = par.getInt("CMYB_BLACK_VALUE");
		bool usePatternPercent = false;
		double colorReferenceThick = 0.1;



		bool prePrintBool = par.getBool("WITNESS_BAR");;//witness bar
		int prePrintBool_Percent = par.getInt("WITNESS_BAR_PERCENT");;//witness bar

		bool fourPage_switch = false;


		bool continuePrinting = par.getBool(("CONTINUE_PRINTING"));
		int start_page = par.getInt("CONTINUE_PRINTING_PAGE");
		int end_page = par.getInt("CONTINUE_PRINTING_END_PAGE");

		/*===========shell Thick===============================*/
		float horizontal_Thick = par.getFloat("HORIZONE_LINE_WIDTH");
		float bottom_top_shell_thick = par.getFloat("VERTICAL_LINE_WIDTH");

		//double bottom_Thick_sure = 0.1;//objects gap must larger than this
		//double top_Thick_sure = 0.1;
		//bottom_top_shell_thick = 0.1;
		double minimumUnit = 0.1;

		int offsetTimes = 1;
		if (minimumUnit > 0)
			offsetTimes = (bottom_top_shell_thick / minimumUnit);
		QVector<cv::Mat > capDownSureMaskV;

		QVector<cv::Mat > capTopSureMaskV;
		for (int i = 0; i < offsetTimes; i++){
			capDownSureMaskV.push_back(cv::Mat(height, width, CV_8UC3, Scalar(255, 255, 255)));
			capTopSureMaskV.push_back(cv::Mat(height, width, CV_8UC3, Scalar(255, 255, 255)));
		}


		int horizonDivision = offsetTimes;
		QVector<cv::Mat > outlineColorCvA;// (height, width, CV_8UC3, Scalar(255, 255, 255));
		QVector<cv::Mat > outlineMaskA;// (height, width, CV_8UC3, Scalar(255, 255, 255));
		QVector<cv::Mat> outlineOutputVV;
		for (int i = 0; i < horizonDivision; i++){
			outlineColorCvA.push_back(cv::Mat(height, width, CV_8UC3, Scalar(255, 255, 255)));
			outlineMaskA.push_back(cv::Mat(height, width, CV_8UC3, Scalar(255, 255, 255)));
			outlineOutputVV.push_back(cv::Mat(height, width, CV_8UC3, Scalar(255, 255, 255)));
		}


		//QVector<cv::Mat> color_Bottom_faceV;// (height, width, CV_8UC3, Scalar(255, 255, 255));
		//for (int i = 0; i < offsetTimes; i++){
		//	color_Bottom_faceV.push_back(*new cv::Mat(height, width, CV_8UC3, Scalar(255, 255, 255)));
		//}
		//QVector<cv::Mat> color_Top_faceV;// (height, width, CV_8UC3, Scalar(255, 255, 255));
		//for (int i = 0; i < offsetTimes; i++){
		//	color_Top_faceV.push_back(*new cv::Mat(height, width, CV_8UC3, Scalar(255, 255, 255)));
		//}


		qWarning() << "tempProfilePath : 3" << tempProfilePath.filePath();
		if (!tempProfilePath.exists()){
			QMessageBox mBox;
			mBox.setIcon(QMessageBox::Critical);
			mBox.setWindowTitle(tr("PartPro350"));
			mBox.setText(tr("Color Profile not exist\n Stop Printing"));
			mBox.setStandardButtons(QMessageBox::Ok);
			mBox.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
			mBox.exec();
			return false;
		}
		assert(tempProfilePath.exists());
		//float meCorrectPercent = 0.97;
		float meCorrectPercent = 1;

		//********control profile on && DSP_TEST_MODE
		bool profileOn = par.getBool("PROFILE_ON");// true;
		bool saveFinalImage = par.getBool("DM_RESULT_IMG");;
		bool savedebugImage = false;// par.getBool(("generate_final_picture"));


		bool savedebugImage2 = false;
		bool savedebugImage4 = false;// par.getBool("DM_RESULT_IMG");;

		SKT::pixelFormat gen_pixelFormat;
		ZxGen_SKT::pixelFormat genZxPixelFormat;
		ZxGen_SKT::zxInitMode zxCheckMode;
		if (profileOn)
		{
			gen_pixelFormat = SKT::pixelFormat::CMY;
			genZxPixelFormat = ZxGen_SKT::pixelFormat::CMY;
		}
		else
		{
			gen_pixelFormat = SKT::pixelFormat::RGB;
			genZxPixelFormat = ZxGen_SKT::pixelFormat::RGB;
		}
		if (DSP_TEST_MODE)
		{
			zxCheckMode = ZxGen_SKT::zxInitMode::ViewCheckMode;
		}
		else
		{
			zxCheckMode = ZxGen_SKT::zxInitMode::RealPrintMode;

		}
		//ZxGen_SKT::zxInitMode zxCheckMode(ZxGen_SKT::zxInitMode::ViewCheckMode);



		int widthrecord, heightrecord;
		QString blk_img_file_name = "black_outline";

		QString zxFilePath = par.getString("ZX_File_Path");
		QFileInfo zxtempFileFinfo(zxFilePath);
		QString zx_sn = makeOutputname2(zx_dir.absolutePath(), zxtempFileFinfo.baseName());
		QString zx_sn2 = makeOutputname(zx_dir.absolutePath(), 0);


		md.jobname.push_back(zx_sn);

		//zggg = new ZxGen(unsigned int(width), unsigned int(height), zx_pages, md.groove.DimX(),21, zx_sn.toStdString());//20161219_backup						 
		//zggg = new ZxGen(unsigned int(width), 2580, zx_job_pages, md.groove.DimX(), 21, zx_sn.toStdString());//20161219_backup						 
		//zx_skt = new ZxGen(unsigned int(width), unsigned int(height*meCorrectPercent + plus_pixel), zx_job_pages, md.groove.DimX(), md.groove.DimY() + PLUS_MM, zx_sn.toStdString(), start_print_b);//20161219_backup						 

		//zx_skt = new ZxGen_SKT(300, zx_job_pages, zx_sn.toStdString(), start_print_b, genZxPixelFormat, zxCheckMode);//20161219_backup		

		if (zxFilePath.isEmpty())
			zx_skt = new ZxGen_SKT(300, zx_job_pages, zx_sn2.toStdString(), start_print_b, genZxPixelFormat, zxCheckMode);//20161219_backup						 
		else
			zx_skt = new ZxGen_SKT(300, zx_job_pages, zx_sn.toStdString(), start_print_b, genZxPixelFormat, zxCheckMode);//20161219_backup						 
		std::vector<cv::Mat> blankTemp;
		std::vector<cv::Mat> blankcapCTemp;


		//if (start_page == 0)
		//{
		WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("newUselessbar_customateBlankPages"));
		if (!(zxCheckMode == ZxGen_SKT::zxInitMode::ViewCheckMode)  && continuePrinting == false && fourPage_switch)
		{

			SKT::createBlankPages(doubleprint, tempgroove.grooveheight, md.p_setting.get_plus_print_length(), useless_print, DSP_Blank_pages, blankTemp, blankcapCTemp, gen_pixelFormat, spittoon_M, spittoon_C, spittoon_Y, spittoon_B, print_dpi);
			////WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("sendData2"));
			if (par.getBool("generate_zx"))
				zx_skt->sendData2(blankTemp, blankcapCTemp);
		}
		//}
		//Log("ZxGen width %i height %i ", int(width), int(height*meCorrectPercent + plus_pixel));
		//FILE *dbgff;
		//dbgff = fopen("D:\\debug_open_1.txt", "w");//@@@
		QTime time;
		//clock_t start, stop;
		time.start();

		//***20160622_test_progress bar initializing progress bar status
		/*if (cb != NULL)
		{
		glContext->doneCurrent();
		(*cb)(0, "processing...");
		glContext->makeCurrent();
		}*/

		QDir patternPath(PicaApplication::getRoamingDir());

		if (patternPath.exists("pattern"))
		{
			patternPath.setPath(PicaApplication::getRoamingDir() + "/pattern");
		}
		else
		{
			patternPath.setPath(PicaApplication::getRoamingDir());
			patternPath.mkpath("pattern");
			patternPath.setPath(PicaApplication::getRoamingDir() + "/pattern");
			//patternPath.setPath( "./pattern");

		}

		cv::Mat patternfilter(cv::Size(5, 5), CV_8UC1, Scalar(255));
		if (usePatternPercent)
		{
			SKT::createPatternPercent(patternfilter, 36);

		}


		cv::Mat patternImg;
		cv::Mat hexBoard(height, width, CV_8UC1, Scalar(255));
		patternImg = cv::imread(patternPath.absolutePath().toStdString() + "/hexagon_4.png", CV_LOAD_IMAGE_GRAYSCALE);
		//create pattern Board
		SKT::createHexPatternImage(patternImg, hexBoard);



		string srcProfilePath = PicaApplication::getRoamingDir().toStdString() + "/ColorProfile/sRGB2014.icc";
		//string desProfilePath = PicaApplication::getRoamingDir().toStdString() + "/ColorProfile/ECI2002CMYKEyeOneL_siriusCMY_70_70_70.icm";
		//string desProfilePath = PicaApplication::getRoamingDir().toStdString() + "/ColorProfile/ECI2002CMYKEyeOneL_siriusCMY_73_76_91_large_paperGray_1ti_coveron.icm";

		//testtimer->start(100);

		//***testLog
		//Log("Closed %i holes and added %i new faces", 12, 12);
		//boxy_dim--;
		int testPrintPageCount;// = 10;
		bool testswitch;// = false;

		int generateNOI = 25;
		int quickWipe = 1;
		WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("for loop start"));
		if (start_page > boxy_dim && continuePrinting){
			zx_skt->close();
			zx_skt->~ZxGen_SKT();
			return false;
		}

		if (continuePrinting)
		{
			testPrintPageCount = end_page - start_page;
			boxy_dim = start_page + testPrintPageCount;
			testswitch = true;
		}
		else
		{
			start_page = 0;
			testPrintPageCount = boxy_dim;
			testswitch = false;
		}

		/*===========test param==================*/
		double horizontal_Thick_for_Binder_mask = 5;
		/*=============================================*/



		for (int i = start_page, j = start_page + 1; i < start_page + testPrintPageCount; i++, j++)
			//for (int i = start_page, j = start_page + 1; i < boxy_dim; i++, j++)
		{
			int determin_firstPage = i - start_page;
			time.restart();
			//resetImage(look_down_Black_Mask_layer, width, height);//
			//resetImage(look_down_color_CV, width, height);
			//resetImage(look_down_Black_MaskCV, width, height);//
			//resetImage(look_up_Black_MaskCV, width, height);//
			//resetImage(look_up_color_CV, width, height);
			resetImage(black_Mask, width, height);
			resetImage(black_Mask2, width, height);
			//resetImage(lookDownBlackTriangleMask, width, height);//
			resetImage(outlineBlackMaskCV, width, height);//
			resetImage(prePrintobjectC, width, height);


			resetImage(downColorFirstCV, width, height);
			resetImage(downBlackMaskFirstCV, width, height);
			resetImage(downColorSecondCV, width, height);
			resetImage(downBlackMaskSecondCV, width, height);

			for (int olcv = 0; olcv < outlineColorCvA.size(); olcv++)
			{
				resetImage(outlineColorCvA[olcv], width, height);
				resetImage(outlineMaskA[olcv], width, height);
				resetImage(outlineOutputVV[olcv], width, height);
			}


			resetImage(outlineColorCv, width, height);


			/*for (int olma = 0; olma < outlineMaskA.size(); olma++)
			{
			resetImage(outlineMaskA[olma], width, height);
			}*/




			/*for (int ia = 0; ia < capDownSureMaskV.size(); ia++)
			{
			resetImage(capDownSureMaskV[ia], width, height);
			}

			for (int ib = 0; ib < capTopSureMaskV.size(); ib++)
			{
			resetImage(capTopSureMaskV[ib], width, height);
			}*/

			/*for (int ia = 0; ia < color_Bottom_faceV.size(); ia++)
			{
			resetImage(color_Bottom_faceV[ia], width, height);
			}

			for (int ib = 0; ib < color_Top_faceV.size(); ib++)
			{
			resetImage(color_Top_faceV[ib], width, height);
			}*/


			/*resetImage(outlineMaskA[0], width, height);
			resetImage(outlineMaskA[1], width, height);
			resetImage(outlineMaskA[2], width, height);
			resetImage(outlineMaskA[3], width, height);
			resetImage(outlineMaskA[4], width, height);*/


			resetImage(captemp, width, height);

			resetImage(capDownMask, width, height);
			resetImage(capDownMask_bottom_face, width, height);
			resetImage(capDownMask_top_face, width, height);
			resetImage(originalOutline, width, height);

			resetImage(color_Bottom_face, width, height);
			resetImage(color_Top_face, width, height);

			resetImage(capDownSureMask, width, height);
			resetImage(capTopSureMask, width, height);

			md.wipeFlag.push_back(true);
			if (cb && (i % 1) == 0)
			{
				glContext->doneCurrent();
				bool break_test = false;
				if (start_print_b)
				{
					break_test = cb((i * 100) / boxy_dim, "start_printing_slice_process");

					if (i > 1 && dynamicValueSwitch)
					{
						int temp = i - 1 + 5;
						sb(temp, quickWipe);
					}
				}
				else
				{
					break_test = cb((i * 100) / boxy_dim, "slice_process");

					if (i > 1 && dynamicValueSwitch)
						sb(i - 1 + 5, quickWipe);

				}
				if (!break_test)//callback2 return cancel, need to close zx_SKT
				{
					zx_skt->close();
					zx_skt->~ZxGen_SKT();

					glContext->doneCurrent();
					return false;
					break;
				}

			}
			glContext->makeCurrent();
			//xyz::slice_roution(md, box_bottom + unit*i, faceColor);//***backup******************切層演算							  
			//time.restart();

			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("slice start"));
			xyz::slice_roution(md, box_bottom + unit*i, faceColor);//****20160301*****************切層演算		
			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("slice end"));


			float currnentHeight = box_bottom + unit*i;


			/*fprintf(dbgff, "slice_func %i, %i\n", time.elapsed(), i);
			fflush(dbgff);*/


			//time.restart();
			//********************************
			QVector<MeshModel *> vmm;
			md.getMeshByContainString("_temp_outlines", vmm);
			md.getMeshByContainString(olMName, vmm);
			QVector<MeshModel *> cmm;
			md.getMeshByContainString(cMName, cmm);//capImage
			int xx = 0;
			QString img_file_name = md.p_setting.getoutlineName();
			float cut_z = box_bottom + unit*i;


			glPixelStorei(GL_PACK_ALIGNMENT, 1);//OpenGL PixelStori Setting


			/*==============================================*/
			//create bottom-face img every 0.1, default 0.8mm
			/*==============================================*/
			//for (int xj = 1, imgi = 0; imgi < color_Bottom_faceV.size(); xj++, imgi++)
			//{
			//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			//	glClearColor(1, 1, 1, 1);
			//	foreach(MeshModel *mdmm, md.meshList)
			//	{
			//		if (mdmm->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mdmm->cm.bbox.min.Z() && currnentHeight <= mdmm->cm.bbox.max.Z()))
			//			if (mdmm->rmm.colorMode != GLW::CMNone || mdmm->rmm.textureMode != GLW::TMNone)
			//			{
			//				glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			//				glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., 0, minimumUnit*xj);
			//				glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
			//				gluLookAt(0, 0, cut_z, 0, 0, -DSP_Look_Distination, 0, 1, 0);//look down
			//				glPushAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
			//				glShadeModel(GL_SMOOTH);
			//				glEnable(GL_COLOR_MATERIAL);
			//				glDisable(GL_LIGHTING);
			//				glPushMatrix();
			//				mdmm->render(GLW::DMSmooth, mdmm->rmm.colorMode, mdmm->rmm.textureMode);
			//				glPopMatrix();
			//				glPopAttrib();
			//			}
			//	}
			//	glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, color_Bottom_faceV[imgi].data);
			//	cv::flip(color_Bottom_faceV[imgi], color_Bottom_faceV[imgi], 0);
			//	if (savedebugImage2){
			//		image = cvMatToQImage(color_Bottom_faceV[imgi]);
			//		QString color_Bottom_faceS = "color_Bottom_faceV";
			//		color_Bottom_faceS.append(QString("_%1_%2.png").arg(i, 4, 10, QChar('0')).arg(xj));
			//		image.save(dir.absolutePath() + "/" + color_Bottom_faceS, "png");
			//	}
			//}
			/*==============================================================*/


			/*==============================================*/
			//create bottom-face img setting 0.8 
			/*==============================================*/
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);
			foreach(MeshModel *mdmm, md.meshList)
			{
				if (mdmm->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mdmm->cm.bbox.min.Z() && currnentHeight <= mdmm->cm.bbox.max.Z()))
					if (mdmm->rmm.colorMode != GLW::CMNone || mdmm->rmm.textureMode != GLW::TMNone)
					{

						glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
						glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., 0, bottom_top_shell_thick);
						glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();

						gluLookAt(0, 0, cut_z, 0, 0, -DSP_Look_Distination, 0, 1, 0);//look down

						glPushAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
						glShadeModel(GL_SMOOTH);
						glEnable(GL_COLOR_MATERIAL);
						glDisable(GL_LIGHTING);
						glPushMatrix();
						mdmm->render(GLW::DMSmooth, mdmm->rmm.colorMode, mdmm->rmm.textureMode);
						glPopMatrix();
						glPopAttrib();
					}
			}

			glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, color_Bottom_face.data);
			cv::flip(color_Bottom_face, color_Bottom_face, 0);

			if (savedebugImage4){
				image = cvMatToQImage(color_Bottom_face);
				QString color_Bottom_faceS = "color_Bottom_face";
				color_Bottom_faceS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				image.save(dir.absolutePath() + "/" + color_Bottom_faceS, "png");
			}
			/*==============================================================*/

			/*=============================================*/
			//create top-face img top thick Setting 0.8
			/*==============================================*/
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);
			foreach(MeshModel *mdmm, md.meshList)
			{
				if (mdmm->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mdmm->cm.bbox.min.Z() && currnentHeight <= mdmm->cm.bbox.max.Z()))
					if (mdmm->rmm.colorMode != GLW::CMNone || mdmm->rmm.textureMode != GLW::TMNone)
					{

						glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
						glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -bottom_top_shell_thick, 0);
						glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();

						gluLookAt(0, 0, cut_z, 0, 0, -DSP_Look_Distination, 0, 1, 0);//look down

						glPushAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
						glShadeModel(GL_SMOOTH);
						glEnable(GL_COLOR_MATERIAL);
						glDisable(GL_LIGHTING);
						glPushMatrix();
						mdmm->render(GLW::DMSmooth, mdmm->rmm.colorMode, mdmm->rmm.textureMode);
						glPopMatrix();
						glPopAttrib();
					}

			}
			glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, color_Top_face.data);
			cv::flip(color_Top_face, color_Top_face, 0);

			if (savedebugImage4){
				image = cvMatToQImage(color_Top_face);
				QString color_Top_faceS = "color_Top_face";
				color_Top_faceS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				image.save(dir.absolutePath() + "/" + color_Top_faceS, "png");
			}
			/*==============================================================*/

			/*=============================================*/
			//create color top-face Vector img every 0.1, default 0.8mm
			/*==============================================*/
			//for (int xj = 1, imgi = 0; imgi < color_Top_faceV.size(); xj++, imgi++){
			//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			//	glClearColor(1, 1, 1, 1);
			//	foreach(MeshModel *mdmm, md.meshList)
			//	{
			//		if (mdmm->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mdmm->cm.bbox.min.Z() && currnentHeight <= mdmm->cm.bbox.max.Z()))
			//			if (mdmm->rmm.colorMode != GLW::CMNone || mdmm->rmm.textureMode != GLW::TMNone)
			//			{

			//				glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			//				glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -minimumUnit*xj, 0);
			//				glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();

			//				gluLookAt(0, 0, cut_z, 0, 0, -DSP_Look_Distination, 0, 1, 0);//look down

			//				glPushAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
			//				glShadeModel(GL_SMOOTH);
			//				glEnable(GL_COLOR_MATERIAL);
			//				glDisable(GL_LIGHTING);
			//				glPushMatrix();
			//				mdmm->render(GLW::DMSmooth, mdmm->rmm.colorMode, mdmm->rmm.textureMode);
			//				glPopMatrix();
			//				glPopAttrib();
			//			}

			//	}
			//	glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, color_Top_faceV[imgi].data);
			//	cv::flip(color_Top_faceV[imgi], color_Top_faceV[imgi], 0);

			//	if (savedebugImage2){
			//		image = cvMatToQImage(color_Top_faceV[imgi]);
			//		QString color_Top_faceS = "color_Top_faceV";
			//		color_Top_faceS.append(QString("_%1_%2.png").arg(i, 4, 10, QChar('0')).arg(xj));
			//		image.save(dir.absolutePath() + "/" + color_Top_faceS, "png");
			//	}
			//}
			/*==============================================================*/



			/*==============start outline Mask ============================*/

			double horizontal_Thick_d = horizontal_Thick / horizonDivision;
			for (int xj = 1, imgi = 0; imgi < horizonDivision; xj++, imgi++){
				glMatrixMode(GL_PROJECTION); glPopMatrix();
				glMatrixMode(GL_MODELVIEW); glPopMatrix();
				glDisable(GL_LIGHTING);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
				glClearColor(1, 1, 1, 1);
				glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
				glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -1000, 1000);
				glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
				gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, 0, 0, 1, 0);


				foreach(MeshModel *mp, vmm)//畫輪廓圖。
				{
					mp->glw.viewerNum = 2;
					//mp->outlineThick = horizontal_Thick;
					mp->outlineThick = xj * horizontal_Thick_d;
					if (vmm.size() > 0)
					{


						mp->render(vcg::GLW::DMTexWireQuad, vcg::GLW::CMBlack, vcg::GLW::TMNone);//backup

					}
				}

				glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, outlineMaskA[imgi].data);
				cv::flip(outlineMaskA[imgi], outlineMaskA[imgi], 0);

				if (savedebugImage4){
					image = cvMatToQImage(outlineMaskA[imgi]);
					QString outlineMaskAS = "outlineMaskA";
					outlineMaskAS.append(QString("_%1_%2.png").arg(i, 4, 10, QChar('0')).arg(xj, 4, 10, QChar('0')));
					image.save(dir.absolutePath() + "/" + outlineMaskAS, "png");
				}
			}
			///////////////////////end outline Mask//////////////////////////



			/*==============start outline color ============================*/
			for (int xj = 1, imgi = 0; imgi < horizonDivision; xj++, imgi++){
				glMatrixMode(GL_PROJECTION); glPopMatrix();
				glMatrixMode(GL_MODELVIEW); glPopMatrix();
				glDisable(GL_LIGHTING);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
				glClearColor(1, 1, 1, 1);
				glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
				glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -1000, 1000);
				glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
				gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, 0, 0, 1, 0);
				//cmystrips(md, par.getFloat("useless_print") / 3.);//***useless bar


				foreach(MeshModel *mp, vmm)//畫輪廓圖。
				{
					mp->glw.viewerNum = 2;
					//mp->outlineThick = horizontal_Thick;
					mp->outlineThick = xj * horizontal_Thick_d;
					if (vmm.size() > 0)
					{
						//if (line_or_quads)
						//	mp->render(vcg::GLW::DMTexWire, mp->rmm.colorMode, mp->rmm.textureMode);//backup
						//else
						mp->render(vcg::GLW::DMTexWireQuad, mp->rmm.colorMode, mp->rmm.textureMode);//backup

					}
				}

				/*glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, outlineColorCv.data);
				cv::flip(outlineColorCv, outlineColorCv, 0);*/

				glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, outlineColorCvA[imgi].data);
				cv::flip(outlineColorCvA[imgi], outlineColorCvA[imgi], 0);

				if (savedebugImage2){
					image = cvMatToQImage(outlineColorCvA[imgi]);
					QString outlineColorA = "outlineColoraaa";
					outlineColorA.append(QString("_%1_%2.png").arg(i, 4, 10, QChar('0')).arg(xj, 4, 10, QChar('0')));
					image.save(dir.absolutePath() + "/" + outlineColorA, "png");
				}
			}

			cv::Mat outlineoutput(height, width, CV_8UC3, Scalar(255, 255, 255));


			SKT::mergeOutlineImages(outlineColorCvA, outlineMaskA, outlineoutput, outlineOutputVV);
			for (int a = 0; a < outlineOutputVV.size(); a++)
			{
				if (savedebugImage4){
					image = cvMatToQImage(outlineOutputVV[a]);
					QString outlineOutputVVA = "outlineOutputVV";
					outlineOutputVVA.append(QString("_%1_%2.png").arg(i, 4, 10, QChar('0')).arg(a, 4, 10, QChar('0')));
					image.save(dir.absolutePath() + "/" + outlineOutputVVA, "png");
				}
			}
			if (savedebugImage4){
				image = cvMatToQImage(outlineoutput);
				QString outlineoutputS = "outlineoutputtytyty";
				outlineoutputS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				image.save(dir.absolutePath() + "/" + outlineoutputS, "png");
			}
			outlineColorCv = outlineoutput;
			///////////////////////end outline color//////////////////////////


			/***20160122***outline and binder thick black mask******/
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);

			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			//glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -100, 100);
			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -1000, 1000);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, 0, 0, 1, 0);
			//*********************************
			//binderstrips(md, par.getFloat("useless_print"));//******uselessbar
			//image.fill(QColor::w);
			foreach(MeshModel *mp, vmm)//outline
			{
				mp->glw.viewerNum = 2;
				mp->outlineThick = horizontal_Thick_for_Binder_mask;
				if (vmm.size() > 0)
				{
					//if (line_or_quads)
					//	mp->render(vcg::GLW::DMTexWire, vcg::GLW::CMBlack, vcg::GLW::TMNone);//backup
					//else
					mp->render(vcg::GLW::DMTexWireQuad, vcg::GLW::CMBlack, vcg::GLW::TMNone);//backup

					//mp->render(vcg::GLW::DMOutline_Test, vcg::GLW::CMNone, vcg::GLW::TMNone);
				}
			}
			foreach(MeshModel *mdmm, md.meshList)
			{
				//****20151231********************************************************************								  
				if (mdmm->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mdmm->cm.bbox.min.Z() && currnentHeight <= mdmm->cm.bbox.max.Z()))
					if (cut_z - mdmm->cm.bbox.min.Z() < 0 || cut_z - mdmm->cm.bbox.max.Z() > 0)//在物體box的上下需要設定限制，才不會超出物體大小。
					{
						glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
						glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., 0.0001, 0);
						glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, cut_z, 0, 0, -DSP_Look_Distination, 0, 1, 0);
						mdmm->render(GLW::DMFlat, GLW::CMBlack, GLW::TMNone);
						glPopMatrix();
					}
					else
					{
						glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
						glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -DSP_Binder_thick, DSP_Binder_thick);
						glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, cut_z, 0, 0, -DSP_Look_Distination, 0, 1, 0);
						glPushMatrix();
						mdmm->render(GLW::DMFlat, GLW::CMBlack, GLW::TMNone);
						glPopMatrix();
					}

			}
			//fFaceColor
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, black_Mask.data);
			cv::flip(black_Mask, black_Mask, 0);
			if (savedebugImage){
				image = cvMatToQImage(black_Mask);
				QString black_MaskS = "black_MaskS";
				black_MaskS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				image.save(dir.absolutePath() + "/" + black_MaskS, "png");
			}
			/*=====================================================================================================*/

			//===========================================================================================
			/*===========================================================================================
			***create cap temp image
			==============================================================================================*/
			equ[3] = box_bottom + unit*i;
			//******cap image**********//
			//time.restart();
			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -1000, 1000);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, -DSP_Look_Distination, 0, 1, 0);
			//*********************************
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);

			//binderstrips(md, par.getFloat("useless_print"));//***uselessbar

			//************************************************//
			//****** Rendering the mesh's clip edge ****//
			GLuint MY_CLIP_PLANE = GL_CLIP_PLANE0;
			glEnable(MY_CLIP_PLANE);
			glClipPlane(MY_CLIP_PLANE, equ);

			glEnable(GL_STENCIL_TEST);
			glEnable(GL_CULL_FACE);
			glClear(GL_STENCIL_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

			//// first pass: increment stencil buffer value on back faces
			glStencilFunc(GL_ALWAYS, 0, 0);
			glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
			glCullFace(GL_FRONT); // render back faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			//glRotatef(90, 1, 0, 0);
			foreach(MeshModel * mp, md.meshList)
			{
				if (mp->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mp->cm.bbox.min.Z() && currnentHeight <= mp->cm.bbox.max.Z()))
					//mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
					mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
			}

			glPopMatrix();
			glPopAttrib();
			//second pass: decrement stencil buffer value on front faces
			glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
			glCullFace(GL_BACK); // render front faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			//glRotatef(90, 1, 0, 0);
			foreach(MeshModel * mp, md.meshList)
			{
				if (mp->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mp->cm.bbox.min.Z() && currnentHeight <= mp->cm.bbox.max.Z()))
					//mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
					mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
			}
			glPopMatrix();
			glPopAttrib();
			//-----------------------------
			// drawing clip planes masked by stencil buffer content
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glEnable(GL_DEPTH_TEST);
			glDisable(MY_CLIP_PLANE);
			glStencilFunc(GL_NOTEQUAL, 0, ~0);
			// stencil test will pass only when stencil buffer value = 0; 
			// (~0 = 0x11...11)

			glColor4f(1, 1, 1, 1);
			fillsquare();
			glDisable(GL_STENCIL_TEST);
			glEnable(MY_CLIP_PLANE); // enabling clip plane again
			glDisable(GL_LIGHTING);

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			//********************************************************************************************//			
			glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, captemp.data);
			cv::flip(captemp, captemp, 0);

			QString img_file_name2 = md.p_setting.getcapImageName();
			glDisable(MY_CLIP_PLANE);//存完圖
			glDisable(GL_CULL_FACE);
			//****
			//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("gen log"));
			//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T(""));
			if (par.getBool("generate_zx"))
			{
				cv::cvtColor(captemp, captemp, CV_BGR2GRAY);
			}
			if (savedebugImage2){
				QImage testttt = cvMatToQImage(captemp);
				QString capimagest = "captemp_image_actual place";
				capimagest.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				testttt.save(dir.absolutePath() + "/" + capimagest, "png");
			}
			cv::Mat captempMask;
			captemp.copyTo(captempMask);
			/*===========================================================================================
			***create mask to determine top face
			==============================================================================================*/

			equ[3] = box_bottom + unit*i + bottom_top_shell_thick;

			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -1000, 1000);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, -DSP_Look_Distination, 0, 1, 0);
			//*********************************
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);

			//************************************************//
			//****** Rendering the mesh's clip edge ****//
			GLuint MY_CLIP_PLANE4 = GL_CLIP_PLANE0;
			glEnable(MY_CLIP_PLANE4);
			glClipPlane(MY_CLIP_PLANE4, equ);

			glEnable(GL_STENCIL_TEST);
			glEnable(GL_CULL_FACE);
			glClear(GL_STENCIL_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

			//// first pass: increment stencil buffer value on back faces
			glStencilFunc(GL_ALWAYS, 0, 0);
			glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
			glCullFace(GL_FRONT); // render back faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			//glRotatef(90, 1, 0, 0);
			foreach(MeshModel * mp, md.meshList)
			{
				if (mp->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mp->cm.bbox.min.Z()) && (currnentHeight <= mp->cm.bbox.max.Z()))
					//mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
					mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
			}

			glPopMatrix();
			glPopAttrib();
			//second pass: decrement stencil buffer value on front faces
			glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
			glCullFace(GL_BACK); // render front faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			//glRotatef(90, 1, 0, 0);
			foreach(MeshModel * mp, md.meshList)
			{
				if (mp->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mp->cm.bbox.min.Z() && currnentHeight <= mp->cm.bbox.max.Z()))
					//mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
					mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
			}
			glPopMatrix();
			glPopAttrib();

			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glEnable(GL_DEPTH_TEST);
			glDisable(MY_CLIP_PLANE4);
			glStencilFunc(GL_NOTEQUAL, 0, ~0);


			glColor4f(1, 1, 1, 1);
			fillsquare();
			glDisable(GL_STENCIL_TEST);
			glEnable(MY_CLIP_PLANE4); // enabling clip plane again
			glDisable(GL_LIGHTING);

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();

			glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, capDownMask_top_face.data);
			cv::flip(capDownMask_top_face, capDownMask_top_face, 0);


			glDisable(MY_CLIP_PLANE4);
			glDisable(GL_CULL_FACE);

			cv::cvtColor(capDownMask_top_face, capDownMask_top_face, CV_BGR2GRAY);

			if (savedebugImage){
				QImage capDownMask_top_facekQ = cvMatToQImage(capDownMask_top_face);
				QString capDownMask_top_facest = "capDownMask_top_face";
				capDownMask_top_facest.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				capDownMask_top_facekQ.save(dir.absolutePath() + "/" + capDownMask_top_facest, "png");
			}
			/*=============================================================================================*/

			/*===========================================================================================
			***only create vector at first, then remove last one and prepend one on the front
			==============================================================================================*/
			int genImgNum = 0;
			if (determin_firstPage == 0)
			{
				genImgNum = offsetTimes;
			}
			else
			{
				genImgNum = 1;
				capTopSureMaskV.removeFirst();
				capTopSureMaskV.push_back(cv::Mat(height, width, CV_8UC3, Scalar(255, 255, 255)));
				//capTopSureMaskV.squeeze();

				capDownSureMaskV.removeLast();
				capDownSureMaskV.prepend(cv::Mat(height, width, CV_8UC3, Scalar(255, 255, 255)));
				//capDownSureMaskV.squeeze();
			}


			/*===========================================================================================
			***create mask to determine top face, every 0.1mm, to gap top thick default 0.8mm
			==============================================================================================*/

			for (int ic = 1, imbNum = 0; imbNum < genImgNum; ic++, imbNum++){
				if (genImgNum == 1)
					equ[3] = box_bottom + unit*i + minimumUnit*offsetTimes;
				else
					equ[3] = box_bottom + unit*i + minimumUnit*ic;

				glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
				glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -1000, 1000);
				glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, -DSP_Look_Distination, 0, 1, 0);
				//*********************************
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
				glClearColor(1, 1, 1, 1);

				//************************************************//
				//****** Rendering the mesh's clip edge ****//
				GLuint MY_CLIP_PLANE6 = GL_CLIP_PLANE0;
				glEnable(MY_CLIP_PLANE6);
				glClipPlane(MY_CLIP_PLANE6, equ);

				glEnable(GL_STENCIL_TEST);
				glEnable(GL_CULL_FACE);
				glClear(GL_STENCIL_BUFFER_BIT);
				glDisable(GL_DEPTH_TEST);
				glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

				//// first pass: increment stencil buffer value on back faces
				glStencilFunc(GL_ALWAYS, 0, 0);
				glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
				glCullFace(GL_FRONT); // render back faces only
				glPushAttrib(GL_ALL_ATTRIB_BITS);
				glEnable(GL_LIGHTING);
				glPushMatrix();

				//glRotatef(90, 1, 0, 0);
				foreach(MeshModel * mp, md.meshList)
				{
					if (mp->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mp->cm.bbox.min.Z()) && (currnentHeight <= mp->cm.bbox.max.Z()))
						//mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
						mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
				}

				glPopMatrix();
				glPopAttrib();
				//second pass: decrement stencil buffer value on front faces
				glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
				glCullFace(GL_BACK); // render front faces only
				glPushAttrib(GL_ALL_ATTRIB_BITS);
				glEnable(GL_LIGHTING);
				glPushMatrix();

				//glRotatef(90, 1, 0, 0);
				foreach(MeshModel * mp, md.meshList)
				{
					if (mp->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mp->cm.bbox.min.Z() && currnentHeight <= mp->cm.bbox.max.Z()))
						//mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
						mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
				}
				glPopMatrix();
				glPopAttrib();

				glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
				glEnable(GL_DEPTH_TEST);
				glDisable(MY_CLIP_PLANE6);
				glStencilFunc(GL_NOTEQUAL, 0, ~0);


				glColor4f(1, 1, 1, 1);
				fillsquare();
				glDisable(GL_STENCIL_TEST);
				glEnable(MY_CLIP_PLANE6); // enabling clip plane again
				glDisable(GL_LIGHTING);

				glMatrixMode(GL_MODELVIEW);
				glPopMatrix();
				glMatrixMode(GL_PROJECTION);
				glPopMatrix();

				if (genImgNum == 1){
					glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, capTopSureMaskV.last().data);
					cv::flip(capTopSureMaskV.last(), capTopSureMaskV.last(), 0);


					glDisable(MY_CLIP_PLANE6);
					glDisable(GL_CULL_FACE);

					cv::cvtColor(capTopSureMaskV.last(), capTopSureMaskV.last(), CV_BGR2GRAY);

					if (savedebugImage4){
						QImage capTopSureMaskQ = cvMatToQImage(capTopSureMaskV.last());
						QString capTopSureMaskQst = "capTopSureMask";
						capTopSureMaskQst.append(QString("_%1_%2.png").arg(i, 4, 10, QChar('0')).arg(offsetTimes));
						capTopSureMaskQ.save(dir.absolutePath() + "/" + capTopSureMaskQst, "png");
					}
				}
				else
				{
					glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, capTopSureMaskV[imbNum].data);
					cv::flip(capTopSureMaskV[imbNum], capTopSureMaskV[imbNum], 0);


					glDisable(MY_CLIP_PLANE6);
					glDisable(GL_CULL_FACE);

					cv::cvtColor(capTopSureMaskV[imbNum], capTopSureMaskV[imbNum], CV_BGR2GRAY);

					if (savedebugImage4){
						QImage capTopSureMaskQ = cvMatToQImage(capTopSureMaskV[imbNum]);
						QString capTopSureMaskQst = "capTopSureMask";
						capTopSureMaskQst.append(QString("_%1_%2.png").arg(i, 4, 10, QChar('0')).arg(imbNum));
						capTopSureMaskQ.save(dir.absolutePath() + "/" + capTopSureMaskQst, "png");
					}
				}


				/*glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, capTopSureMask.data);
				cv::flip(capTopSureMask, capTopSureMask, 0);


				glDisable(MY_CLIP_PLANE6);
				glDisable(GL_CULL_FACE);

				cv::cvtColor(capTopSureMask, capTopSureMask, CV_BGR2GRAY);

				if (savedebugImage){
				QImage capTopSureMaskQ = cvMatToQImage(capTopSureMask);
				QString capTopSureMaskQst = "capTopSureMask";
				capTopSureMaskQst.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				capTopSureMaskQ.save(dir.absolutePath() + "/" + capTopSureMaskQst, "png");
				}*/
			}
			/*=============================================================================================*/






			/*===========================================================================================
			***create mask to determine bottom face
			==============================================================================================*/

			equ[3] = box_bottom + unit*i - bottom_top_shell_thick;

			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -1000, 1000);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, -DSP_Look_Distination, 0, 1, 0);
			//*********************************
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);

			//************************************************//
			//****** Rendering the mesh's clip edge ****//
			GLuint MY_CLIP_PLANE5 = GL_CLIP_PLANE0;
			glEnable(MY_CLIP_PLANE5);
			glClipPlane(MY_CLIP_PLANE5, equ);

			glEnable(GL_STENCIL_TEST);
			glEnable(GL_CULL_FACE);
			glClear(GL_STENCIL_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

			//// first pass: increment stencil buffer value on back faces
			glStencilFunc(GL_ALWAYS, 0, 0);
			glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
			glCullFace(GL_FRONT); // render back faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			//glRotatef(90, 1, 0, 0);
			foreach(MeshModel * mp, md.meshList)
			{
				if (mp->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mp->cm.bbox.min.Z()) && (currnentHeight <= mp->cm.bbox.max.Z()))
					//mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
					mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
			}

			glPopMatrix();
			glPopAttrib();
			//second pass: decrement stencil buffer value on front faces
			glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
			glCullFace(GL_BACK); // render front faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			//glRotatef(90, 1, 0, 0);
			foreach(MeshModel * mp, md.meshList)
			{
				if (mp->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mp->cm.bbox.min.Z() && currnentHeight <= mp->cm.bbox.max.Z()))
					//mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
					mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
			}
			glPopMatrix();
			glPopAttrib();

			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glEnable(GL_DEPTH_TEST);
			glDisable(MY_CLIP_PLANE5);
			glStencilFunc(GL_NOTEQUAL, 0, ~0);


			glColor4f(1, 1, 1, 1);
			fillsquare();
			glDisable(GL_STENCIL_TEST);
			glEnable(MY_CLIP_PLANE5); // enabling clip plane again
			glDisable(GL_LIGHTING);

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();

			glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, capDownMask_bottom_face.data);
			cv::flip(capDownMask_bottom_face, capDownMask_bottom_face, 0);


			glDisable(MY_CLIP_PLANE5);
			glDisable(GL_CULL_FACE);

			cv::cvtColor(capDownMask_bottom_face, capDownMask_bottom_face, CV_BGR2GRAY);

			if (savedebugImage){
				QImage capDownMaskkQ = cvMatToQImage(capDownMask_bottom_face);
				QString capDownMaskst = "capDownMask_bottom_face";
				capDownMaskst.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				capDownMaskkQ.save(dir.absolutePath() + "/" + capDownMaskst, "png");
			}
			/*=============================================================================================*/


			/*===========================================================================================
			***create mask to determine bottom face, every 0.1mm to bottom thick, default 0.8mm
			==============================================================================================*/

			for (int ic = 1, imbNum = 0; imbNum < genImgNum; ic++, imbNum++){
				equ[3] = box_bottom + unit*i - minimumUnit*ic;

				//time.restart();
				glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
				glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -1000, 1000);
				glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, -DSP_Look_Distination, 0, 1, 0);
				//*********************************
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
				glClearColor(1, 1, 1, 1);

				//binderstrips(md, par.getFloat("useless_print"));//***uselessbar

				//************************************************//
				//****** Rendering the mesh's clip edge ****//
				GLuint MY_CLIP_PLANE7 = GL_CLIP_PLANE0;
				glEnable(MY_CLIP_PLANE7);
				glClipPlane(MY_CLIP_PLANE7, equ);

				glEnable(GL_STENCIL_TEST);
				glEnable(GL_CULL_FACE);
				glClear(GL_STENCIL_BUFFER_BIT);
				glDisable(GL_DEPTH_TEST);
				glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

				//// first pass: increment stencil buffer value on back faces
				glStencilFunc(GL_ALWAYS, 0, 0);
				glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
				glCullFace(GL_FRONT); // render back faces only
				glPushAttrib(GL_ALL_ATTRIB_BITS);
				glEnable(GL_LIGHTING);
				glPushMatrix();

				//glRotatef(90, 1, 0, 0);
				foreach(MeshModel * mp, md.meshList)
				{
					if (mp->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mp->cm.bbox.min.Z()) && (currnentHeight <= mp->cm.bbox.max.Z()))
						//mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
						mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
				}

				glPopMatrix();
				glPopAttrib();
				//second pass: decrement stencil buffer value on front faces
				glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
				glCullFace(GL_BACK); // render front faces only
				glPushAttrib(GL_ALL_ATTRIB_BITS);
				glEnable(GL_LIGHTING);
				glPushMatrix();

				//glRotatef(90, 1, 0, 0);
				foreach(MeshModel * mp, md.meshList)
				{
					if (mp->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mp->cm.bbox.min.Z() && currnentHeight <= mp->cm.bbox.max.Z()))
						//mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
						mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
				}
				glPopMatrix();
				glPopAttrib();

				glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
				glEnable(GL_DEPTH_TEST);
				glDisable(MY_CLIP_PLANE7);
				glStencilFunc(GL_NOTEQUAL, 0, ~0);


				glColor4f(1, 1, 1, 1);
				fillsquare();
				glDisable(GL_STENCIL_TEST);
				glEnable(MY_CLIP_PLANE7); // enabling clip plane again
				glDisable(GL_LIGHTING);

				glMatrixMode(GL_MODELVIEW);
				glPopMatrix();
				glMatrixMode(GL_PROJECTION);
				glPopMatrix();
				//********************************************************************************************//



				glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, capDownSureMaskV[imbNum].data);
				cv::flip(capDownSureMaskV[imbNum], capDownSureMaskV[imbNum], 0);


				glDisable(MY_CLIP_PLANE7);
				glDisable(GL_CULL_FACE);

				cv::cvtColor(capDownSureMaskV[imbNum], capDownSureMaskV[imbNum], CV_BGR2GRAY);
				//}
				if (savedebugImage4){
					QImage capDownSureMaskIMG = cvMatToQImage(capDownSureMaskV[imbNum]);
					QString capDownSureMaskS = "capDownSureMask";
					capDownSureMaskS.append(QString("_%1_%2.png").arg(i, 4, 10, QChar('0')).arg(imbNum));
					capDownSureMaskIMG.save(dir.absolutePath() + "/" + capDownSureMaskS, "png");
				}

				//glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, capDownSureMask.data);
				//cv::flip(capDownSureMask, capDownSureMask, 0);

				//
				//glDisable(MY_CLIP_PLANE7);
				//glDisable(GL_CULL_FACE);
				//
				//cv::cvtColor(capDownSureMask, capDownSureMask, CV_BGR2GRAY);
				////}
				//if (savedebugImage){
				//	QImage capDownSureMaskIMG = cvMatToQImage(capDownSureMask);
				//	QString capDownSureMaskS = "capDownSureMask";
				//	capDownSureMaskS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				//	capDownSureMaskIMG.save(dir.absolutePath() + "/" + capDownSureMaskS, "png");
				//}
			}
			/*===========================================================================================
			***END  Binder down Mask  image
			==============================================================================================*/

			/*for (int y = 0; y < capDownSureMaskV.size(); y++)
			{
			QImage capDownSureMaskIMG = cvMatToQImage(capDownSureMaskV[y]);
			QString capDownSureMaskS = "capDownSureMaskVVV";
			capDownSureMaskS.append(QString("_%1_%2.png").arg(i, 4, 10, QChar('0')).arg(y));
			capDownSureMaskIMG.save(dir.absolutePath() + "/" + capDownSureMaskS, "png");
			}

			for (int y = 0; y < capTopSureMaskV.size(); y++)
			{
			QImage capTopSureMaskVIMG = cvMatToQImage(capTopSureMaskV[y]);
			QString capTopSureMaskS = "capTopSureMaskVVVV";
			capTopSureMaskS.append(QString("_%1_%2.png").arg(i, 4, 10, QChar('0')).arg(y));
			capTopSureMaskVIMG.save(dir.absolutePath() + "/" + capTopSureMaskS, "png");
			}*/







			/*===========================================================================================
			***Binder down Mask  image
			==============================================================================================*/

			equ[3] = box_bottom + unit*i - doublePrint_begin_twenty_layer_V2Thick;

			//time.restart();
			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -1000, 1000);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, -DSP_Look_Distination, 0, 1, 0);
			//*********************************
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);

			//binderstrips(md, par.getFloat("useless_print"));//***uselessbar

			//************************************************//
			//****** Rendering the mesh's clip edge ****//
			GLuint MY_CLIP_PLANE3 = GL_CLIP_PLANE0;
			glEnable(MY_CLIP_PLANE3);
			glClipPlane(MY_CLIP_PLANE3, equ);

			glEnable(GL_STENCIL_TEST);
			glEnable(GL_CULL_FACE);
			glClear(GL_STENCIL_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

			//// first pass: increment stencil buffer value on back faces
			glStencilFunc(GL_ALWAYS, 0, 0);
			glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
			glCullFace(GL_FRONT); // render back faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			//glRotatef(90, 1, 0, 0);
			foreach(MeshModel * mp, md.meshList)
			{
				if (mp->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mp->cm.bbox.min.Z()) && (currnentHeight <= mp->cm.bbox.max.Z()))
					//mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
					mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
			}

			glPopMatrix();
			glPopAttrib();
			//second pass: decrement stencil buffer value on front faces
			glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
			glCullFace(GL_BACK); // render front faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			//glRotatef(90, 1, 0, 0);
			foreach(MeshModel * mp, md.meshList)
			{
				if (mp->getMeshSort() == MeshModel::meshsort::print_item && (currnentHeight >= mp->cm.bbox.min.Z() && currnentHeight <= mp->cm.bbox.max.Z()))
					//mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
					mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
			}
			glPopMatrix();
			glPopAttrib();

			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glEnable(GL_DEPTH_TEST);
			glDisable(MY_CLIP_PLANE3);
			glStencilFunc(GL_NOTEQUAL, 0, ~0);


			glColor4f(1, 1, 1, 1);
			fillsquare();
			glDisable(GL_STENCIL_TEST);
			glEnable(MY_CLIP_PLANE3); // enabling clip plane again
			glDisable(GL_LIGHTING);

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			//********************************************************************************************//


			//glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, cap_image.bits());//***20150724
			glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, capDownMask.data);
			cv::flip(capDownMask, capDownMask, 0);

			//QString img_file_name2 = md.p_setting.getcapImageName();
			glDisable(MY_CLIP_PLANE3);
			glDisable(GL_CULL_FACE);
			//****								
			//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T(""));
			/*if (par.getBool("generate_zx"))
			{*/
			cv::cvtColor(capDownMask, capDownMask, CV_BGR2GRAY);
			//}
			if (savedebugImage){
				QImage capDownMaskkQ = cvMatToQImage(capDownMask);
				QString capDownMaskst = "capDownMask";
				capDownMaskst.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				capDownMaskkQ.save(dir.absolutePath() + "/" + capDownMaskst, "png");
			}
			/*===========================================================================================
			***END  Binder down Mask  image
			==============================================================================================*/




			//===*****PrePrintingObject**********************************************************************************************************===//
			if (prePrintBool)
			{
				//***第二張圖***產生膠水圖**********//
				//time.restart();
				glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
				glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -1000, 1000);
				glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, -DSP_Look_Distination, 0, 1, 0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
				glClearColor(1, 1, 1, 1);
				//****** Rendering the mesh's clip edge ****//
				GLuint MY_CLIP_PLANE2 = GL_CLIP_PLANE0;
				glEnable(MY_CLIP_PLANE2);
				glClipPlane(MY_CLIP_PLANE2, equ);

				glEnable(GL_STENCIL_TEST);
				glEnable(GL_CULL_FACE);
				glClear(GL_STENCIL_BUFFER_BIT);
				glDisable(GL_DEPTH_TEST);
				glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

				//// first pass: increment stencil buffer value on back faces
				glStencilFunc(GL_ALWAYS, 0, 0);
				glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
				glCullFace(GL_FRONT); // render back faces only
				glPushAttrib(GL_ALL_ATTRIB_BITS);
				glEnable(GL_LIGHTING);
				glPushMatrix();

				foreach(MeshModel * mp, md.meshList)
				{
					if (mp->getMeshSort() == MeshModel::meshsort::pre_print_item && (currnentHeight >= mp->cm.bbox.min.Z() && currnentHeight <= mp->cm.bbox.max.Z()))
						mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
				}

				glPopMatrix();
				glPopAttrib();
				//second pass: decrement stencil buffer value on front faces
				glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
				glCullFace(GL_BACK); // render front faces only
				glPushAttrib(GL_ALL_ATTRIB_BITS);
				glEnable(GL_LIGHTING);
				glPushMatrix();

				//glRotatef(90, 1, 0, 0);
				foreach(MeshModel * mp, md.meshList)
				{
					if (mp->getMeshSort() == MeshModel::meshsort::pre_print_item && (currnentHeight >= mp->cm.bbox.min.Z() && currnentHeight <= mp->cm.bbox.max.Z()))
						mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
				}
				glPopMatrix();
				glPopAttrib();
				//-----------------------------
				// drawing clip planes masked by stencil buffer content
				glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
				glEnable(GL_DEPTH_TEST);
				glDisable(MY_CLIP_PLANE2);
				glStencilFunc(GL_NOTEQUAL, 0, ~0);
				// stencil test will pass only when stencil buffer value = 0; 
				// (~0 = 0x11...11)

				glColor4f(1, 1, 1, 1);
				fillsquare();
				glDisable(GL_STENCIL_TEST);
				glEnable(MY_CLIP_PLANE2); // enabling clip plane again
				glDisable(GL_LIGHTING);

				glMatrixMode(GL_MODELVIEW);
				glPopMatrix();
				glMatrixMode(GL_PROJECTION);
				glPopMatrix();
				//********************************************************************************************//
				glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, prePrintobjectC.data);
				cv::flip(prePrintobjectC, prePrintobjectC, 0);
				bitwise_not(prePrintobjectC, prePrintobjectC);
				cv::cvtColor(prePrintobjectC, prePrintobjectB, CV_BGR2GRAY);


				glDisable(MY_CLIP_PLANE2);//存完圖
				glDisable(GL_CULL_FACE);

				//if (true){
				//	//if (true){
				//	QImage ttt(cvMatToQImage(prePrintobjectB));
				//	QString tttt = "prePrintobjectB";
				//	tttt.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				//	ttt.save(dir.absolutePath() + "/" + tttt, "png");
				//}

			}
			//****
			//===*******************************************************************************************===//


			//resetImage(originalOutline, width, height);
			originalOutline = Scalar(255, 255, 255);
			//cv::Mat outputOutlineImage(color_Top_face.size(), color_Top_face.type(), Scalar(255, 255, 255));;
			cv::Mat reverse_binder_outline(capDownMask_bottom_face.size(), CV_8UC1, Scalar(255));



			/*SKT::bottomTopMergeColorImage(capDownMask_bottom_face, capDownMask_top_face, capDownSureMask, capTopSureMask, captempMask,
			color_Bottom_face, color_Top_face, outlineColorCv, originalOutline, reverse_binder_outline);*/
			//=======current========//
			SKT::bottomTopMergeColorImage_Multi(capDownSureMaskV, capTopSureMaskV, outlineMaskA, captempMask,
				color_Bottom_face, color_Top_face, outlineColorCv, outlineOutputVV, originalOutline, reverse_binder_outline);



			if (savedebugImage4){
				QImage outputOutlineImageQt = cvMatToQImage(originalOutline);
				QString outputOutlineImageS = "outputOutlineImageAfterMergeColoImage";
				outputOutlineImageS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				outputOutlineImageQt.save(dir.absolutePath() + "/" + outputOutlineImageS, "png");
			}


			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T(""));

			//========================================================//

			//originalOutline = SKT::outlineAndBinderMask(outlineColorCv, outlineBlackMaskCV, captemp, downColorFirstCV, downBlackMaskFirstCV, downColorSecondCV, downBlackMaskSecondCV, reverse_binder_outline).clone();

			if (savedebugImage){
				QImage testttt = cvMatToQImage(originalOutline);
				QString originalOutlineS = "originalOutlineS";
				originalOutlineS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				testttt.save(dir.absolutePath() + "/" + originalOutlineS, "png");
			}
			QImage reverse_binder = cvMatToQImage(reverse_binder_outline);
			if (savedebugImage){
				QString reverse_binder_outlineS = "reverse_binder_outlineO";
				reverse_binder_outlineS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				reverse_binder.save(dir.absolutePath() + "/" + reverse_binder_outlineS, "png");
			}




			//***20160112***image_process加上插點pattern
			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("imagePorcess"));
			if (par.getBool("generate_zx"))
			{
				SKT::InkBalance imageProcess;

				cv::Mat temp, temp2;
				//temp = SKT::resizeIamge(&originalOutline,plus_pixel);


				//imageProcess.operate2(temp, captemp, SKT::resizeIamge(&black_Mask, plus_pixel));
				//=====================black_Mask for no color area==================================================
				//imageProcess.operate2(originalOutline, captemp, black_Mask, false);


				if (par.getBool("mono_bool"))
					originalOutline = cv::Scalar(255, 255, 255);

				WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("profileStart"));


				Mat desImg(originalOutline.size(), originalOutline.type(), Scalar(0, 0, 0));

				/*RGB to RGB and turn desImg to CMY
				, then copy to originalOutline,
				ink reduction*/
				cmsHPROFILE htemp = SKT::ColorTransform::OpenStockProfile(desProfilePath.c_str());
				cmsColorSpaceSignature sigTemp = cmsGetColorSpace(htemp);
				cmsCloseProfile(htemp);
				if (sigTemp == cmsSigCmykData)
				{
					if (profileOn && SKT::ColorTransform::applyColorFromRGBtoCMYK_LCMS(originalOutline, desImg, srcProfilePath.c_str(), desProfilePath.c_str(), 0))
					{
						qDebug() << " color profile success" << endl;
						desImg.copyTo(originalOutline);

					}
				}
				else if (sigTemp == cmsSigRgbData)
				{
					if (profileOn && SKT::ColorTransform::applyColorFromRGBtoRGB_LCMS_RGBProfile(originalOutline, desImg, srcProfilePath.c_str(), desProfilePath.c_str(), 0))
					{
						qDebug() << " RGB color profile success" << endl;
						desImg.copyTo(originalOutline);
						Mat testcolor = SKT::reduc1(originalOutline);
						testcolor.copyTo(originalOutline);
					}
				}



				///////cap_temp_change_add_pattern///////////////////		
				QString papa = patternPath.absolutePath();

				////WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("rect_test1"));
				if ((i % 100) > 20 && add_pattern)
					//if ( add_pattern)
				{

					//captemp = SKT::binderAddHexagon(&captemp, &black_Mask, patternImg);
					captemp = SKT::pasteHexBoardtoImg(captempMask, hexBoard, black_Mask, captemp);
					if (false){
						QImage pastePattern(cvMatToQImage(captemp));
						QString pastePatternS = "pastePattern_DM";
						pastePatternS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
						pastePattern.save(dir.absolutePath() + "/" + pastePatternS, "png");
					}
				}


				/*if (colorBinding)
				{
				//QImage ttt(cvMatToQImage(outlineCTemp.back()));
				if (savedebugImage){
				QImage t4(cvMatToQImage(originalOutline));
				QString tt4 = "before_originalOutline";
				tt4.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				t4.save(dir.absolutePath() + "/" + tt4, "png");

				QImage ttt(cvMatToQImage(black_Mask2));
				QString tttt = "black_Mask2";
				tttt.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				ttt.save(dir.absolutePath() + "/" + tttt, "png");

				QImage ttt2(cvMatToQImage(captemp));
				QString tttt2 = "captemp";
				tttt2.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				ttt2.save(dir.absolutePath() + "/" + tttt2, "png");
				}


				imageProcess.addCMYDots(originalOutline, captemp, innerColorMask);

				if (savedebugImage){
				QImage ttt3(cvMatToQImage(originalOutline));
				QString tttt3 = "after_originalOutline";
				tttt3.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				ttt3.save(dir.absolutePath() + "/" + tttt3, "png");

				QImage ttt5(cvMatToQImage(captemp));
				QString tttt5 = "after_captemp";
				tttt5.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				ttt5.save(dir.absolutePath() + "/" + tttt5, "png");
				}
				}*/


				//Combine With Original Color Image
				//if (innerGrayBool)SKT::combineGrayImageAndColorImage(&originalOutline, &getInnerGrayMask);///*****add innerGrayImage(3.

				if (prePrintBool)
				{
					SKT::mergePrePrinting(originalOutline, prePrintobjectB, prePrintBool_Percent);
					SKT::mergePrePrinting(captemp, prePrintobjectB);


				}


				/*QImage testttt = cvMatToQImage(originalOutline);
				if (true){
				QString originalOutlineS = "originalOutlineS";
				originalOutlineS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				testttt.save(dir.absolutePath() + "/" + originalOutlineS, "png");
				}
				QImage reverse_binder = cvMatToQImage(reverse_binder_outline);
				if (true){
				QString reverse_binder_outlineS = "reverse_binder_outlineO";
				reverse_binder_outlineS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				reverse_binder.save(dir.absolutePath() + "/" + reverse_binder_outlineS, "png");
				}*/


				originalOutline = SKT::resizeIamgecols(&originalOutline, plus_pixel, gen_pixelFormat);//ADD LENGTH ON COLOR IMAGE									
				SKT::newUselessbar_custom(originalOutline, useless_print, gen_pixelFormat, spittoon_M, spittoon_C, spittoon_Y, spittoon_B, print_dpi);




				captemp = SKT::resizeIamgecols(&captemp, plus_pixel, gen_pixelFormat);
				capDownMask = SKT::resizeIamgecols(&capDownMask, plus_pixel, gen_pixelFormat);
				//SKT::newUselessbar(captemp, useless_print, gen_pixelFormat);//20160715_backup																			

				if (!doubleprint)
				{
					SKT::cutImage(captemp, originalOutline);//white_kick							

					capDownMask = capDownMask(cv::Rect(0, 0, captemp.cols, captemp.rows));
				}

				/*if (true){
				QImage testttt = cvMatToQImage(captemp);
				QString originalOutlineS = "captempp";
				originalOutlineS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				testttt.save(dir.absolutePath() + "/" + originalOutlineS, "png");
				}
				if (true){
				QImage testttt = cvMatToQImage(capDownMask);
				QString originalOutlineS = "capDownMask";
				originalOutlineS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				testttt.save(dir.absolutePath() + "/" + originalOutlineS, "png");
				}
				*/



				/*
				copyCapTemp
				*/
				cv::Mat copyCaptemp;
				captemp.copyTo(copyCaptemp);

				//copyCaptemp = captemp.clone();

				SKT::newUselessbar_custom(captemp, useless_print, gen_pixelFormat, spittoon_M, spittoon_C, spittoon_Y, spittoon_B, print_dpi);
				//SKT::newUselessbar_custom(capDownMask, useless_print, gen_pixelFormat, spittoon_M, spittoon_C, spittoon_Y, spittoon_B, print_dpi);

				/*if (dilateBinder)
				captemp = SKT::dilateImage(&captemp, dilateBinderValue);*/

				//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T(""));
				cv::Mat finalcolor(originalOutline.cols, originalOutline.rows, originalOutline.type(), Scalar(255, 255, 255));//finalcolor.setTo(cv::Scalar(255, 255, 255));
				SKT::rotateImage(originalOutline, finalcolor);

				/*
				rotate capTemp
				*/
				cv::Mat finalBinder(captemp.cols, captemp.rows, captemp.type(), Scalar(255));//finalBinder.setTo(cv::Scalar(255));
				cv::Mat capDownMaskFinal(captemp.cols, captemp.rows, captemp.type(), Scalar(255));//finalBinder.setTo(cv::Scalar(255));
				cv::Mat copyCaptempMaskFinal(captemp.cols, captemp.rows, captemp.type(), Scalar(255));//finalBinder.setTo(cv::Scalar(255));

				SKT::rotateImage(captemp, finalBinder);
				SKT::rotateImage(capDownMask, capDownMaskFinal);
				SKT::rotateImage(copyCaptemp, copyCaptempMaskFinal);


				/*if (true){
				QImage testttt = cvMatToQImage(originalOutline);
				QString originalOutlineS = "finalcolor";
				originalOutlineS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				testttt.save(dir.absolutePath() + "/" + originalOutlineS, "png");
				}*/


				outlineCTemp.push_back(finalcolor);//**********PUSH_BACK_OUTLINE********************************
				if (zxCheckMode == ZxGen_SKT::zxInitMode::ViewCheckMode && !doubleprint)
					finalBinder.setTo(cv::Scalar(255));//SET BINDER TO WHITE
				//if (dialiteBinder)finalBinder = SKT::dilateImage(&finalBinder, 1);
				capCTemp.push_back(finalBinder);//***********PUSH_BACK_CAPIMAGE*********************************

				cv::Mat backbinder(finalBinder.size(), finalBinder.type(), cv::Scalar(255));

				cv::Mat stiffColor(cv::Size(finalcolor.rows, finalcolor.cols), finalcolor.type(), cv::Scalar(0, 0, 0));
				cv::Mat stiffColor2(cv::Size(finalcolor.cols, finalcolor.rows), finalcolor.type(), cv::Scalar(0, 0, 0));
				if (!profileOn){
					stiffColor = Scalar(255, 255, 255);
					stiffColor2 = Scalar(255, 255, 255);
				}

				if (doublePrint_V2)
				{

					SKT::newUselessbar_custom(stiffColor, useless_print, gen_pixelFormat, spittoon_fucking_second_Layer_CMY, spittoon_fucking_second_Layer_CMY, spittoon_fucking_second_Layer_CMY, spittoon_fucking_second_Layer_B, print_dpi);
					SKT::rotateImage(stiffColor, stiffColor2);

					copyCaptempMaskFinal.copyTo(backbinder);
					cv::Mat erodeBlackBinder = SKT::dilateImage(&backbinder, dilateBinderValue);

					SKT::BottomLighter_binder(copyCaptempMaskFinal, capDownMaskFinal, erodeBlackBinder, doublePrint_begin_twenty_layer_V2Percent);

					/*===============================================
					remain x percent binder in the second printing
					================================================*/
					SKT::BinderPercent(reverse_binder_outline, doublePrint_shell_Percent);

					reverse_binder_outline = SKT::resizeIamgecols(&reverse_binder_outline, plus_pixel, gen_pixelFormat);
					reverse_binder_outline = reverse_binder_outline(cv::Rect(0, 0, backbinder.rows, finalBinder.cols));

					cv::Mat secondOutlineBinder(reverse_binder_outline.cols, reverse_binder_outline.rows, reverse_binder_outline.type(), Scalar(255));//finalBinder.setTo(cv::Scalar(255));
					//add uselessbar and rotateImage
					SKT::newUselessbar_custom(reverse_binder_outline, useless_print, gen_pixelFormat, spittoon_fucking_second_Layer_CMY, spittoon_fucking_second_Layer_CMY, spittoon_fucking_second_Layer_CMY, spittoon_fucking_second_Layer_B, print_dpi);
					SKT::rotateImage(reverse_binder_outline, secondOutlineBinder);

					//combine secondOutlineBinder and erodeBlackBinder to secondOutlineBinder
					SKT::combineBinderImage(secondOutlineBinder, erodeBlackBinder);

					outlineCTemp.push_back(stiffColor2);
					//capCTemp.push_back(erodeBlackBinder);
					capCTemp.push_back(secondOutlineBinder);


				}
				if (saveFinalImage){
					int count_outline_Temp = 0;
					foreach(cv::Mat img, outlineCTemp)
					{

						QImage cap_Test = cvMatToQImage(img);
						QString tttt3 = "outline";
						tttt3.append(QString("_%1_%2.png").arg(i, 4, 10, QChar('0')).arg(count_outline_Temp, 4, 10, QChar('0')));
						cap_Test.save(dir.absolutePath() + "/" + tttt3, "png");
						count_outline_Temp++;
						break;
					}

					int count_cap_Temp = 0;
					foreach(cv::Mat img, capCTemp)
					{

						QImage cap_Test = cvMatToQImage(img);
						QString tttt3 = "captemp";
						tttt3.append(QString("_%1_%2.png").arg(i, 4, 10, QChar('0')).arg(count_cap_Temp, 4, 10, QChar('0')));
						cap_Test.save(dir.absolutePath() + "/" + tttt3, "png");
						count_cap_Temp++;
					}
				}

				double binderRatio = SKT::imageComplexity(finalBinder);
				if (!doubleprint)
				{
					if (binderRatio > 18)
					{
						quickWipe = 4;
					}
					else if (binderRatio > 9)
					{
						quickWipe = 4;
					}
					else
						quickWipe = 6;
				}
				else
				{
					if (binderRatio > 18)
					{
						quickWipe = 8;
					}
					else if (binderRatio > 9)
					{
						quickWipe = 8;
					}
					else
						quickWipe = 12;

				}


			}


			if (save_cap_Image){

				QImage cap_Test = cvMatToQImage(captemp);
				img_file_name2.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				cap_Test.save(dir.absolutePath() + "/" + img_file_name2, "png");
			}


			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("generate_final_picture"));
			/*
			gen_Final_Picture
			*/
			if (savedebugImage)
				//if (false)
			{
#if 1
				////picasso/////////////////////////////////////////////////
				//if (i < start_page + generateNOI)
									{

										//QImage ttt(cvMatToQImage(outlineCTemp.back()));
										QImage ttt(cvMatToQImage(originalOutline));
										QString tttt = "tttt_originalOutline";
										tttt.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
										ttt.save(dir.absolutePath() + "/" + tttt, "png");
										qDebug() << "dir.absolutePath()" << dir.absolutePath();


										//////////////black_mask///////////////////////////////
										//QImage black_mask_QI(cvMatToQImage(black_Mask));
										////QImage ttt(cvMatToQImage(originalOutline));
										//QString tttt2 = "black_mask_QI";
										//tttt2.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
										//black_mask_QI.save(dir.absolutePath() + "/" + tttt2, "png");
										//////////captemp//////////
										QImage binder_image(cvMatToQImage(captemp));
										//QImage ttt(cvMatToQImage(originalOutline));
										QString binder_image_text = "binder_image";
										binder_image_text.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
										binder_image.save(dir.absolutePath() + "/" + binder_image_text, "png");
										///////////////////////////
									}
#else
				//**picasso_junior****************************
				cv::Mat test = SKT::resizeXYAxis(&originalOutline, 2240, 2240);
				test = SKT::resizeIamge(&test, 10);
				QImage jr_c(cvMatToQImage(test));
				//QImage ttt(cvMatToQImage(originalOutline));
				QString tttt = "tttt";
				tttt.append(QString("_%1.jpg").arg(i, 4, 10, QChar('0')));
				//jr_c.save(dir.absolutePath() + "/" + tttt, "jpg");
				//////////////black_mask///////////////////////////////

				//////////captemp//////////
				cv::Mat test2 = SKT::resizeXYAxis(&captemp, 2240, 2240);
				test2 = SKT::resizeIamge(&test2, 10);
				QImage jr_b(cvMatToQImage(test2));
				QString binder_image_text = "binder_image";
				binder_image_text.append(QString("_%1.jpg").arg(i, 4, 10, QChar('0')));
				//jr_b.save(dir.absolutePath() + "/" + binder_image_text, "jpg");
				///////////////////////////
				QImage jr_f(jr_c.width(), jr_c.height(), QImage::Format_RGBA8888);
				for (int i = 0; i < jr_c.width(); i++)
					for (int j = 0; j < jr_c.height(); j++)
					{
						QRgb value = qRgba(qRed(jr_c.pixel(i, j)), qGreen(jr_c.pixel(i, j)), qBlue(jr_c.pixel(i, j)), 255 - qRed(jr_b.pixel(i, j)));
						jr_f.setPixel(i, j, value);

						QRgb value2 = jr_f.pixel(i, j);
						if (qRed(value2) < 255){
							//qDebug() << "qRGBA" << qRed(value2) << qGreen(value2) << qBlue(value2) << qAlpha(value2);
						}
						//qDebug() << "qRGBAaa" <<  qAlpha(value2);
						//qDebug()<<"qRGBA"<<qRed(value2)<<qGreen(value2)<<qBlue(value2)<<qAlpha(value2);

					}
				QString jrFI = "";
				jrFI.append(QString("%1.tif").arg(i, 4, 10, QChar('0')));
				QString savedir = dir.absolutePath() + "/" + jrFI;
				QImageWriter writer(savedir);
				bool fffff = writer.canWrite();
				QString fffdasdfasdf = writer.errorString();
				writer.setCompression(1);
				writer.write(jr_f);

				//***end_picasso_junior
#endif

			}

			if (doubleprint && savedebugImage){
				QString backimgname = "backImage";
				//QImage backbinderqq = cvMatToQImage(outlineCTemp.back());
				QImage backbinderqq = cvMatToQImage(capCTemp.back());
				backimgname.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
				backbinderqq.save(dir.absolutePath() + "/" + backimgname, "png");
			}

			if (par.getBool("generate_zx") && j % send_page_num == 0)//幾頁傳到已產生的zxfile一次
			{
				//if (zggg)
				//{
				// zggg = new ZxGen(outlineCTemp.begin()->cols, outlineCTemp.begin()->rows, zx_job_pages, md.groove.DimX(), md.groove.DimY() + PLUS_MM, zx_sn.toStdString());//20161219_backup						 
				// 
				//}
				//time.restart();
				//zggg->sendData(outlineCTemp, capCTemp);
				bool sendResult = zx_skt->sendData2(outlineCTemp, capCTemp);
				wchar_t *vOut = sendResult ? L"true" : L"false";
				//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, (vOut));

				capCTemp.clear();
				outlineCTemp.clear();


				if (j == boxy_dim)
				{
					zx_skt->close();
					zx_skt->~ZxGen_SKT();
					Log("end ZX %i", j);
				}
				else
					if (i == start_page + testPrintPageCount - 1 && testswitch)
					{
						zx_skt->close();
						zx_skt->~ZxGen_SKT();
					}

				//fprintf(dbgff, "zx_gen %i, %i\n", time.elapsed(), i);
				//fflush(dbgff);

			}
			else if (par.getBool("generate_zx") && j % send_page_num != 0 && j == boxy_dim)//超過一個zxfile, 頁數不超過1個job
			{
				//if (zggg)
				//{
				// zggg = new ZxGen(outlineCTemp.begin()->cols, outlineCTemp.begin()->rows, zx_job_pages, md.groove.DimX(), md.groove.DimY() + PLUS_MM, zx_sn.toStdString());//20161219_backup						 
				//}
				//time.restart();
				zx_skt->sendData2(outlineCTemp, capCTemp);
				capCTemp.clear();
				outlineCTemp.clear();
				//time.elapsed();
				zx_skt->close();
				zx_skt->~ZxGen_SKT();
				Log("2end ZX  %i", j);

				//fprintf(dbgff, "zx_gen %i, %i\n", time.elapsed(), i);
				//fflush(dbgff);
			}
			QString tempt = makeOutputname(zx_dir.absolutePath(), j);
			if (j % zx_job_pages == 0)//一個job，產生下一個job
			{
				md.jobname.push_back(tempt);
				zx_skt->close();
				zx_skt->~ZxGen_SKT();
				//zx_skt = new ZxGen_SKT(unsigned int(width), unsigned int(((height*meCorrectPercent) + plus_pixel)), zx_job_pages, md.groove.DimX(), md.groove.DimY() + PLUS_MM, tempt.toStdString(), start_print_b);//20161219_backup
				zx_skt = new ZxGen_SKT(300, zx_job_pages, zx_sn.toStdString(), start_print_b);//20161219_backup
				//zggg = new ZxGen(unsigned int(width), unsigned int(4713), zx_job_pages, md.groove.DimX(), md.groove.DimY() + PLUS_MM, tempt.toStdString(), start_print_b);//20161219_backup

			}

			//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("senddata end"));
			char* numpages = new char[20];
			string temp = std::to_string(i).append("_pages");
			strcpy(numpages, temp.c_str());
			wchar_t *wmsg = new wchar_t[strlen(numpages) + 1]; //memory allocation
			mbstowcs(wmsg, numpages, strlen(numpages) + 1);
			//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, wmsg);
			delete[]wmsg;
			delete[]numpages;


			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();


			//*****test_thread
			/*emit test_update_pages(i);
			thread1.start();*/


			/*fprintf(dbgff, "b_name %i\n", i);
			fflush(dbgff);*/

			//fprintf(dbgff, "cap_prePrting_image %i, %i\n", time.elapsed(), i);
			//fflush(dbgff);
			//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("generate_final_picture"));
		}
		//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("for loop end"));
		//thread1.exit();


#endif	

		ctx.unbindReadDrawFramebuffer();
		ctx.release();
		glPopAttrib();
		glContext->doneCurrent();

		//image.rgbSwapped().mirrored().save(par.getSaveFileName("ImageFileName"));
		//image.mirrored().save(par.getSaveFileName("ImageFileName"));
		//cap_image.mirrored().save(par.getSaveFileName("CapImageFileName"));
		//image.save(par.getSaveFileName("ImageFileName"));


		foreach(MeshModel *bm, md.meshList)
		{
			if ((bm->label().contains(olMName, Qt::CaseSensitive) || bm->label().contains(cMName, Qt::CaseSensitive)))
				//if (bm->label().contains(olMName, Qt::CaseSensitive) || bm->label().contains(cMName, Qt::CaseSensitive)))
				//if (bm->getMeshSort()==MeshModel::meshsort::)
			{
				md.delMesh(bm);
			}
		}

		/*if (par.getBool("generate_zx")){

		QString ppath = "cd /d " + getRoamingDir();
		QStringList arguments;
		arguments << "/k" << ppath << "&&" << "usbsend.exe" << "output.zx" << "&&" << "exit";

		}*/



		qDebug() << " done _applyClick ";
		break;
	}

#pragma endregion FP_PRINT_FLOW_2_V3

#pragma region FP_SAVE_DEFAULT_SNAP_SHOT

	case FP_SAVE_DEFAULT_SNAP_SHOT:
	{
		//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("FP_PRINT_WITH_WHITE_KICK"));
		testlog("FP_PRINT_FLOW_2_V3");

		md.jobname.clear();


		glContext->makeCurrent();


		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			/* Problem: glewInit failed, something is seriously wrong. */
			//fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
			//qDebug("Error: %s\n", glewGetErrorString(err));
		}

		glPushAttrib(GL_ALL_ATTRIB_BITS);

		Context ctx;
		ctx.acquire();

		if (switchInitMeshVBO)
			initMeshVBO(md);

		//***20150907
		const GLsizei print_dpi = 100;// GLsizei(par.getInt("print_dpi"));
		float x = md.groove.DimX();
		float y = md.groove.DimY();
		const GLsizei width = (int)(x / DSP_inchmm * print_dpi);
		const GLsizei height = (int)(width * y / x);

		//qDebug() << "width" << "height" << width << height;
		//RenderbufferHandle hDepth = createRenderbuffer(ctx, GL_DEPTH_COMPONENT24, width, height);
		RenderbufferHandle hDepth = createRenderbuffer(ctx, GL_DEPTH24_STENCIL8, width, height);
		Texture2DHandle    hColor = createTexture2D(ctx, GL_RGB8, width, height, GL_RGB, GL_UNSIGNED_BYTE);
		//FramebufferHandle  hFramebuffer = createFramebuffer(ctx, renderbufferTarget(hDepth), texture2DTarget(hColor));
		FramebufferHandle  hFramebuffer = createFramebufferWithDepthStencil(ctx, renderbufferTarget(hDepth), renderbufferTarget(hDepth), texture2DTarget(hColor));




		const vcg::Point3f lightDirectionVS = vcg::Point3f(0.0f, 0.0f, -1.0f).Normalize();

		glEnable(GL_DEPTH_TEST);
		//glClearColor(GLfloat(backgroundColor.red())/255.0f, GLfloat(backgroundColor.green())/255.0f, GLfloat(backgroundColor.blue())/255.0f, 0.0f);
		Construction_Groove<float> tempgroove = md.groove;

		glViewport(0, 0, width, height);//***window的長寬	

		//***儲存image的基本資訊
		QImage image(int(width), int(height), QImage::Format_RGB888);
		QImage image2(int(width), int(height), QImage::Format_RGB888);
		QImage slice0Point05mmImage(int(width), int(height), QImage::Format_RGB888);
		QImage cap_image(int(width), int(height), QImage::Format_RGB888);
		QImage cap_prePrting_image(int(width), int(height), QImage::Format_RGB888);
		//prePrintobjectC = *new Mat(height,width,CV_8UC3,Scalar(255, 255, 255));
		cv::Mat look_down_color_CV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat look_down_Black_Mask_layer(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat look_down_Black_MaskCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat look_up_Black_MaskCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat look_up_color_CV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat captemp(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat capDownMask(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat black_Mask(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat black_Mask2(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat outlineBlackMaskCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat lookDownBlackTriangleMask(height, width, CV_8UC3, Scalar(255, 255, 255));

		cv::Mat prePrintobjectC(height, width, CV_8UC3, Scalar(255, 255, 255));

		cv::Mat downColorFirstCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat downBlackMaskFirstCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat downColorSecondCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat downBlackMaskSecondCV(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat prePrintobjectB(height, width, CV_8UC1, Scalar(255));

		cv::Mat capDownMask_bottom_face(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat capDownMask_top_face(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat color_Bottom_face(height, width, CV_8UC3, Scalar(255, 255, 255));
		cv::Mat color_Top_face(height, width, CV_8UC3, Scalar(255, 255, 255));

		cv::Mat originalOutline(height, width, CV_8UC3, Scalar(255, 255, 255));


		cv::Mat outlineColorCv(height, width, CV_8UC3, Scalar(255, 255, 255));

		vector<cv::Mat > outlineColorCvA;// (height, width, CV_8UC3, Scalar(255, 255, 255));
		for (int i = 0; i < 4; i++){
			outlineColorCvA.push_back(*new cv::Mat(height, width, CV_8UC3, Scalar(255, 255, 255)));
			//outlineColorCvA[i].create(height, width, CV_8UC3);
			//outlineColorCvA[i] = Scalar(255, 255, 255);
		}
		vector<cv::Mat > outlineMaskA;// (height, width, CV_8UC3, Scalar(255, 255, 255));
		for (int i = 0; i < 4; i++){
			outlineMaskA.push_back(*new cv::Mat(height, width, CV_8UC3, Scalar(255, 255, 255)));

			//outlineMaskA[i].create(height, width, CV_8UC3);
			//outlineColorCvA[i] = Scalar(255, 255, 255);
		}






		//***20150505***slice_func_設置高度
		float box_bottom = md.bbox().min.Z();
		float unit = par.getFloat("slice_height");



		int boxy_dim = qCeil((md.bbox().max.Z() - md.bbox().min.Z()) / unit) + 1;
		//int boxy_dim = qCeil((md.bbox().max.Z() - md.bbox().min.Z()) / unit);//for one page
		//int(mdd->bbox().DimZ() / pJI.layerThickness.value) + 1;
		//Log("boxy_dim %i", boxy_dim);

		init_texture(md);

		//***20150507 standard path******************
		//QString temppath = QStandardPaths::locate(QStandardPaths::TempLocation, "", QStandardPaths::LocateDirectory);
		//QString temppath = "D:/temptemp";//backup



		QString temppath = PicaApplication::getRoamingDir();
		//QDir dir(temppath);//backup
		//if (dir.exists("xyzimage"))
		//{
		//	dir.setPath(temppath + "/xyzimage");
		//	dir.removeRecursively();
		//	dir.setPath(temppath);
		//	dir.mkpath("xyzimage");
		//}
		//else
		//{
		//	dir.setPath(temppath);
		//	dir.mkpath("xyzimage");
		//}

		//dir.setPath(temppath + "/xyzimage");

		//---zx_file------------------------------------------
		//QString zxtemp = "D:/temptemp";
		QString zxtemp = PicaApplication::getRoamingDir();
		QDir zx_dir(zxtemp);
		if (zx_dir.exists("zx_file"))
		{
			zx_dir.setPath(zxtemp + "/zx_file");
			/*zx_dir.removeRecursively();
			zx_dir.setPath(zxtemp);
			zx_dir.mkpath("zx_file");*/
		}
		else
		{
			zx_dir.setPath(zxtemp);
			zx_dir.mkpath("zx_file");
		}

		zx_dir.setPath(zxtemp + "/zx_file");

		//==============================
		//**********************
		ctx.bindReadDrawFramebuffer(hFramebuffer);
		GLW_CHECK_GL_READ_DRAW_FRAMEBUFFER_STATUS;
#if 1

		const bool outputOL_Image = par.getBool("OL_Image");
		const bool save_cap_Image = par.getBool("Cap_Image");

		const bool faceColor = par.getBool("FaceColor");
		glClearColor(1, 1, 1, 1);
		QString olMName = md.p_setting.getOlMeshName();
		QString cMName = md.p_setting.getCapMeshName();

		//***opencvMat clear
		capCTemp.clear();
		outlineCTemp.clear();


		int zx_job_pages = par.getInt("per_zx_job");
		//int start_page = par.getInt("start_page");
		int send_page_num = par.getInt("send_page_num");
		int plus_pixel = par.getInt("plus_pixel");
		float PLUS_MM = par.getFloat("PLUS_MM");
		bool start_print_b = par.getBool("PRINT_START");
		float useless_print = par.getFloat("USELESS_PRINT");
		float spittoon_M = par.getFloat("SPITTOON_M");
		float spittoon_C = par.getFloat("SPITTOON_C");
		float spittoon_Y = par.getFloat("SPITTOON_Y");
		float spittoon_B = par.getFloat("SPITTOON_B");
		float spittoon_fucking_second_Layer_CMY = par.getFloat("SPITTOON_SECOND_CMY");;
		float spittoon_fucking_second_Layer_B = par.getFloat("SPITTOON_SECOND_B");;

		bool add_pattern = par.getBool("ADD_PATTERN");
		string desProfilePath = par.getString("COLOR_PROFILE").toStdString();
		QFileInfo tempProfilePath(QString::fromStdString(desProfilePath));
		bool doubleprint = false;// par.getBool("Stiff_Print");
		bool dilateBinder = par.getBool("DILATE_BINDER");
		int dilateBinderValue = par.getInt("DILATE_BINDER_VALUE");
		bool dynamicValueSwitch = par.getBool("DYNAMIC_WIPE");
		bool colorBinding = par.getBool("COLOR_BINDING");
		bool doublePrint_V2 = par.getBool("STIFF_PRIN_V2");
		int doublePrint_V2Percent = par.getInt("STIFF_PRINT_VALUE");

		int doublePrint_begin_twenty_layer_V2Percent = par.getInt("PP_BOTTOM_LIGHTER_PERCENTAGE");
		float doublePrint_begin_twenty_layer_V2Thick = par.getFloat("PP_BOTTOM_LIGHTER_THICK");
		//double pp_upperMask = 0.5;//Bottom Ink lighter mm


		int doublePrint_shell_Percent = par.getInt("SHELL_PERCENT");
		bool CMYB_black_mode = par.getBool("CMYB_BLACK");
		int CMYB_black_percentage = par.getInt("CMYB_BLACK_VALUE");
		bool usePatternPercent = false;
		double colorReferenceThick = 0.1;


		bool prePrintBool = par.getBool("WITNESS_BAR");;//witness bar
		int prePrintBool_Percent = par.getInt("WITNESS_BAR_PERCENT");;//witness bar

		bool fourPage_switch = false;


		bool continuePrinting = par.getBool(("CONTINUE_PRINTING"));
		int start_page = par.getInt("CONTINUE_PRINTING_PAGE");
		int end_page = par.getInt("CONTINUE_PRINTING_END_PAGE");

		/*===========shell Thick===============================*/
		float horizontal_Thick = par.getFloat("HORIZONE_LINE_WIDTH");
		float bottom_top_shell_thick = par.getFloat("VERTICAL_LINE_WIDTH");

		if (!tempProfilePath.exists()){
			QMessageBox mBox;
			mBox.setIcon(QMessageBox::Critical);
			mBox.setWindowTitle(tr("PartPro350"));
			mBox.setText(tr("Color Profile not exist\n Stop Printing"));
			mBox.setStandardButtons(QMessageBox::Ok);
			mBox.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
			mBox.exec();
		}
		assert(tempProfilePath.exists());
		//float meCorrectPercent = 0.97;
		float meCorrectPercent = 1;

		//********control profile on && DSP_TEST_MODE
		bool profileOn = true;
		bool saveFinalImage = par.getBool("DM_RESULT_IMG");;
		bool savedebugImage = false;// par.getBool(("generate_final_picture"));


		bool savedebugImage2 = false;

		SKT::pixelFormat gen_pixelFormat;
		ZxGen_SKT::pixelFormat genZxPixelFormat;
		ZxGen_SKT::zxInitMode zxCheckMode;
		if (profileOn)
		{
			gen_pixelFormat = SKT::pixelFormat::CMY;
			genZxPixelFormat = ZxGen_SKT::pixelFormat::CMY;
		}
		else
		{
			gen_pixelFormat = SKT::pixelFormat::RGB;
			genZxPixelFormat = ZxGen_SKT::pixelFormat::RGB;
		}
		if (DSP_TEST_MODE)
		{
			zxCheckMode = ZxGen_SKT::zxInitMode::ViewCheckMode;
		}
		else
		{
			zxCheckMode = ZxGen_SKT::zxInitMode::RealPrintMode;

		}
		//ZxGen_SKT::zxInitMode zxCheckMode(ZxGen_SKT::zxInitMode::ViewCheckMode);



		int widthrecord, heightrecord;
		QString blk_img_file_name = "black_outline";

		QString zxFilePath = par.getString("ZX_File_Path");
		QFileInfo zxtempFileFinfo(zxFilePath);
		QString zx_sn = makeOutputname2(zx_dir.absolutePath(), zxtempFileFinfo.completeBaseName());
		QString zx_sn2 = makeOutputname(zx_dir.absolutePath(), 0);
		QString snapShotFileName = zx_dir.absolutePath() + "/" + zxtempFileFinfo.completeBaseName() + ".png";
		QString thumnailPath = par.getString("THUMBNAIL_FILE_PATH");
		md.jobname.push_back(zx_sn);



		QDir patternPath(PicaApplication::getRoamingDir());

		if (patternPath.exists("pattern"))
		{
			patternPath.setPath(PicaApplication::getRoamingDir() + "/pattern");
		}
		else
		{
			patternPath.setPath(PicaApplication::getRoamingDir());
			patternPath.mkpath("pattern");
			patternPath.setPath(PicaApplication::getRoamingDir() + "/pattern");
			//patternPath.setPath( "./pattern");

		}

		cv::Mat patternfilter(cv::Size(5, 5), CV_8UC1, Scalar(255));
		if (usePatternPercent)
		{
			SKT::createPatternPercent(patternfilter, 36);

		}


		cv::Mat patternImg;
		cv::Mat hexBoard(height, width, CV_8UC1, Scalar(255));
		patternImg = cv::imread(patternPath.absolutePath().toStdString() + "/hexagon_4.png", CV_LOAD_IMAGE_GRAYSCALE);
		//create pattern Board
		SKT::createHexPatternImage(patternImg, hexBoard);



		string srcProfilePath = PicaApplication::getRoamingDir().toStdString() + "/ColorProfile/sRGB2014.icc";
		//string desProfilePath = PicaApplication::getRoamingDir().toStdString() + "/ColorProfile/ECI2002CMYKEyeOneL_siriusCMY_70_70_70.icm";
		//string desProfilePath = PicaApplication::getRoamingDir().toStdString() + "/ColorProfile/ECI2002CMYKEyeOneL_siriusCMY_73_76_91_large_paperGray_1ti_coveron.icm";

		//testtimer->start(100);

		//***testLog
		//Log("Closed %i holes and added %i new faces", 12, 12);
		//boxy_dim--;
		int testPrintPageCount;// = 10;
		bool testswitch;// = false;

		int generateNOI = 25;
		int quickWipe = 1;
		//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("for loop start"));




		/*===========test param==================*/
		double horizontal_Thick_for_Binder_mask = 5;
		/*=============================================*/
		double horizonDivision = 4.;



		for (int i = 0; i < 1; i++)
		{
			//cv::Mat capDownMask_bottom_face(height, width, CV_8UC3, Scalar(255, 255, 255));
			resetImage(color_Bottom_face, width, height);


			glContext->makeCurrent();

			float currnentHeight = box_bottom + unit*i;

			QString img_file_name = md.p_setting.getoutlineName();



			glPixelStorei(GL_PACK_ALIGNMENT, 1);//OpenGL PixelStori Setting

			//create bottom-face img 0.5
			/*==============================================*/
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);
			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-350, 350, -220, 220, 0, 1500);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
			//gluLookAt(-350, -220, 300, 0, 0, -200, 0, 0, 1);//look down
			gluLookAt(-350, -280, 100, 0, 0, 0, 0, 0, 1);//look down
			//gluLookAt(0, -220, 0, 0, 0, 0, 0, 1, 0);//look down
			//gluLookAt(0, 0, cut_z, 0, 0, -DSP_Look_Distination, 0, 1, 0);//look down
			glEnable(GL_LIGHTING);
			glEnable(GL_LIGHT0);
			foreach(MeshModel *mdmm, md.meshList)
			{
				glPushAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
				glShadeModel(GL_SMOOTH);
				if (mdmm->rmm.colorMode != GLW::CMNone || mdmm->getMeshSort() == MeshModel::meshsort::decorate_item)
				{
					glEnable(GL_COLOR_MATERIAL);
					glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
				}
				else
				{
					glEnable(GL_COLOR_MATERIAL);
					glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
					glColor(Color4b::White);
				}

				//glDisable(GL_LIGHTING);
				glPushMatrix();
				mdmm->render(GLW::DMFlat, mdmm->rmm.colorMode, mdmm->rmm.textureMode);
				glPopMatrix();
				glPopAttrib();

			}
			/*glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();*/
			glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, color_Bottom_face.data);
			cv::flip(color_Bottom_face, color_Bottom_face, 0);

			if (true){
				image = cvMatToQImage(color_Bottom_face);				
				image.save(snapShotFileName, "png");
				image.save(thumnailPath);
			}
			/*==============================================================*/

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();

			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("generate_final_picture"));
		}
		//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("for loop end"));
		//thread1.exit();


#endif	

		ctx.unbindReadDrawFramebuffer();
		ctx.release();
		glPopAttrib();
		glContext->doneCurrent();

		//image.rgbSwapped().mirrored().save(par.getSaveFileName("ImageFileName"));
		//image.mirrored().save(par.getSaveFileName("ImageFileName"));
		//cap_image.mirrored().save(par.getSaveFileName("CapImageFileName"));
		//image.save(par.getSaveFileName("ImageFileName"));


		foreach(MeshModel *bm, md.meshList)
		{
			if ((bm->label().contains(olMName, Qt::CaseSensitive) || bm->label().contains(cMName, Qt::CaseSensitive)))
				//if (bm->label().contains(olMName, Qt::CaseSensitive) || bm->label().contains(cMName, Qt::CaseSensitive)))
				//if (bm->getMeshSort()==MeshModel::meshsort::)
			{
				md.delMesh(bm);
			}
		}

		/*if (par.getBool("generate_zx")){

		QString ppath = "cd /d " + getRoamingDir();
		QStringList arguments;
		arguments << "/k" << ppath << "&&" << "usbsend.exe" << "output.zx" << "&&" << "exit";

		}*/



		qDebug() << " done _applyClick ";
		break;
	}

#pragma endregion FP_SAVE_DEFAULT_SNAP_SHOT

#pragma region FP_DM_ESTIMATE_COLOR

	case FP_DM_ESTIMATE_COLOR:
	{
	}break;

#pragma endregion FP_DM_ESTIMATE_COLOR
#pragma region FP_TEST_BEZIER_CURVE

	case FP_TEST_BEZIER_CURVE:
	{
		Point2m p0(0, 0);
		Point2m p1(3, 8);
		Point2m p2(10, 5);
		for (int x = 0; x < 10; x++)
		{

		}

	}break;

#pragma endregion FP_TEST_BEZIER_CURVE

	}
	return true;
}
void GenSliceJob::testthreadfunc()
{
	double dSum = 0;
	for (int i = 0; i < 10000; ++i)
		for (int j = 0; j < 10000; ++j)
			dSum += i*j;
	qDebug() << "threaddd" << dSum;
}

void GenSliceJob::testprintf()
{
	qDebug() << "timeout";
}
void GenSliceJob::setpd(int per)
{
	qDebug() << "stdpd" << per;
	progress->setValue(per);
	progress->update();
}
void GenSliceJob::generateZX(float width, float height, int pages, float grooveDimx, string full_filename)
{
	ZxGen zGG(unsigned int(width), unsigned int(height), pages, grooveDimx, 29, full_filename, false);
	zGG.sendData(outlineCTemp, capCTemp);
	zGG.close();

}


void GenSliceJob::setpd2(int per)
{
	qDebug() << "stdpd" << per;
	progress->setValue(per);
	QThread::currentThread()->wait();
}
void GenSliceJob::cmystrips(MeshDocument & md, float cmywidth)
{
	Construction_Groove<float> tempgroove = md.groove;
	float x1 = -tempgroove.groovelength / 2.;
	float x2 = tempgroove.groovelength / 2.;
	float y1 = tempgroove.grooveheight / 2.;
	float z1 = -tempgroove.groovewidth / 2. - 1;


	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();
	glBegin(GL_QUADS);

	for (int i = 0; i < 3; i++)
	{
		switch (i)
		{
		case 0:
			glColor3ub(0, 255, 255);
			//glColor3ub(0, 0, 255);
			//glColor3ub(0, 0, 0);
			break;
		case 1:
			glColor3ub(255, 0, 255);
			//glColor3ub(255, 0, 0);
			break;
		case 2:
			glColor3ub(255, 255, 0);
			break;
		case 3:
			//glColor3ub(0, 0, 0);
			break;

		}
		glVertex3f(x1, y1 - cmywidth*i, z1);
		glVertex3f(x1, y1 - cmywidth*(i + 1), z1);
		glVertex3f(x2, y1 - cmywidth*(i + 1), z1);
		glVertex3f(x2, y1 - cmywidth*i, z1);

	}

	glEnd();
	//glBegin();
	//glEnd();


	glPopMatrix();
	glPopAttrib();
}
QString GenSliceJob::getRoamingDir()
{
	//QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Temp", "application");
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, PicaApplication::getRoamingDir(), "application");
	QString location = QFileInfo(settings.fileName()).absolutePath() + "/";

	return location;

}
QString GenSliceJob::makeOutputname(QString dir, int numpages)
{
	QDateTime dateTime = QDateTime::currentDateTime();

	QString temp = dir + "/output_" + dateTime.toString("yyyy_M_d_H_m_s")/*QString::number(numpages) */ + ".zx";
	return temp;
}
QString GenSliceJob::makeOutputname2(QString dir, QString name)
{
	QString temp = dir + "/" + name + ".zx";
	return temp;
}

void GenSliceJob::binderstrips(MeshDocument & md, float blackwidth)
{
	Construction_Groove<float> tempgroove = md.groove;
	float x1 = -tempgroove.groovelength / 2.;
	float x2 = tempgroove.groovelength / 2.;
	float y1 = tempgroove.grooveheight / 2.;
	float z1 = -tempgroove.groovewidth / 2. - 1;
	//blackwidth;

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();
	glBegin(GL_QUADS);

	for (int i = 0; i < 1; i++)
	{
		glColor3ub(0, 0, 0);
		glVertex3f(x1, y1 - blackwidth*i, z1);
		glVertex3f(x1, y1 - blackwidth*(i + 1), z1);
		glVertex3f(x2, y1 - blackwidth*(i + 1), z1);
		glVertex3f(x2, y1 - blackwidth*i, z1);

	}

	glEnd();
	//glBegin();
	//glEnd();


	glPopMatrix();
	glPopAttrib();
}

void GenSliceJob::generateOneHundredZX(float width, float height)
{

}

bool GenSliceJob::getMeshLayerImage(QList<MeshModel *> mdlist, std::map<int, std::vector<cv::Mat> *> &testSaveImage)
{


	CMeshO & mesh = mdlist.at(0)->cm;
	if ((mesh.vn < 3) || (mesh.fn < 1)) return false;

	const unsigned char * p0 = (const unsigned char *)(&(mesh.vert[0].P()));
	const unsigned char * p1 = (const unsigned char *)(&(mesh.vert[1].P()));
	const void *          pbase = p0;
	GLsizei               pstride = GLsizei(p1 - p0);

	const unsigned char * n0 = (const unsigned char *)(&(mesh.vert[0].N()));
	const unsigned char * n1 = (const unsigned char *)(&(mesh.vert[1].N()));
	const void *          nbase = n0;
	GLsizei               nstride = GLsizei(n1 - n0);

	glContext->makeCurrent();


	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
	}

	glPushAttrib(GL_ALL_ATTRIB_BITS);

	Context ctx;
	ctx.acquire();

	const GLsizeiptr psize = GLsizeiptr(GLsizei(mesh.vn) * pstride);
	BufferHandle hPositionBuffer = createBuffer(ctx, psize, pbase);

	const GLsizeiptr nsize = GLsizeiptr(GLsizei(mesh.vn) * nstride);
	BufferHandle hNormalBuffer = createBuffer(ctx, nsize, nbase);

	const GLsizeiptr isize = GLsizeiptr(mesh.fn * 3 * sizeof(GLuint));
	BufferHandle hIndexBuffer = createBuffer(ctx, isize);
	{
		BoundIndexBufferHandle indexBuffer = ctx.bindIndexBuffer(hIndexBuffer);

		const CMeshO::VertexType * vbase = &(mesh.vert[0]);
		GLuint *  indices = (GLuint *)indexBuffer->map(GL_WRITE_ONLY);
		for (size_t i = 0; i < mesh.face.size(); ++i)
		{
			const CMeshO::FaceType & f = mesh.face[i];
			if (f.IsD()) continue;
			for (int v = 0; v < 3; ++v)
			{
				*indices++ = GLuint(vcg::tri::Index(mesh, f.cV(v)));
			}
		}
		indexBuffer->unmap();

		ctx.unbindIndexBuffer();
	}



	if (mdlist.size() == 0)return true;
	Box3m FullBBox;
	Matrix44m iden;
	iden.SetIdentity();
	foreach(MeshModel *mp, mdlist)
	{
		FullBBox.Add(iden, mp->cm.bbox);
	}




	//***20150907
	const GLsizei print_dpi = 25;// GLsizei(par.getInt("print_dpi"));
	float x = FullBBox.DimX();
	float y = FullBBox.DimY();
	const GLsizei width = (int)(x / DSP_inchmm * print_dpi);
	const GLsizei height = (int)(width * y / x);


	RenderbufferHandle hDepth = createRenderbuffer(ctx, GL_DEPTH24_STENCIL8, width, height);
	Texture2DHandle    hColor = createTexture2D(ctx, GL_RGB8, width, height, GL_RGB, GL_UNSIGNED_BYTE);
	//FramebufferHandle  hFramebuffer = createFramebuffer(ctx, renderbufferTarget(hDepth), texture2DTarget(hColor));
	FramebufferHandle  hFramebuffer = createFramebufferWithDepthStencil(ctx, renderbufferTarget(hDepth), renderbufferTarget(hDepth), texture2DTarget(hColor));




	const vcg::Point3f lightDirectionVS = vcg::Point3f(0.0f, 0.0f, -1.0f).Normalize();

	glEnable(GL_DEPTH_TEST);
	//glClearColor(GLfloat(backgroundColor.red())/255.0f, GLfloat(backgroundColor.green())/255.0f, GLfloat(backgroundColor.blue())/255.0f, 0.0f);


	glViewport(0, 0, width, height);//***window的長寬				  

	//const vcg::Point3f center = mesh.bbox.Center();
	//const float        scale = 1.0f / mesh.bbox.Diag();

	//glScalef(scale, scale, scale);
	//glTranslatef(-center[0], -center[1], -center[2]);

	//***儲存image的基本資訊
	QImage image(int(width), int(height), QImage::Format_RGB888);
	QImage image2(int(width), int(height), QImage::Format_RGB888);
	QImage slice0Point05mmImage(int(width), int(height), QImage::Format_RGB888);
	QImage cap_image(int(width), int(height), QImage::Format_RGB888);
	//*******************

	QString temppath = PicaApplication::getRoamingDir();
	QDir dir(temppath);//backup
	if (dir.exists("xyzimage"))
	{
		dir.setPath(temppath + "/xyzimage");
		dir.removeRecursively();
		dir.setPath(temppath);
		dir.mkpath("xyzimage");
	}
	else
	{
		dir.setPath(temppath);
		dir.mkpath("xyzimage");
	}

	dir.setPath(temppath + "/xyzimage");

	//==============================
	//**********************
	ctx.bindReadDrawFramebuffer(hFramebuffer);
	GLW_CHECK_GL_READ_DRAW_FRAMEBUFFER_STATUS;
#if 1


	glClearColor(1, 1, 1, 1);
	//***opencvMat clear
	capCTemp.clear();
	outlineCTemp.clear();

	//float meCorrectPercent = 0.97;
	float meCorrectPercent = 1;


	int widthrecord, heightrecord;
	QString blk_img_file_name = "black_outline";


	std::vector<cv::Mat> blankTemp;
	std::vector<cv::Mat> blankcapCTemp;

	QTime time;
	//clock_t start, stop;
	time.start();

	QDir patternPath(PicaApplication::getRoamingDir());




	bool testswitch = false;
	int generateNOI = 20;
	int quickWipe = 1;
	bool savedebugImage = false;// par.getBool(("generate_final_picture"));


	float unit = 2;
	int boxy_dim = qCeil((FullBBox.max.Z() - FullBBox.min.Z()) / unit) + 1;



	float box_bottom = FullBBox.min.Z();
	int start_page = 0;// par.getInt("start_page");
	//Construction_Groove<float> tempgroove(FullBBox.DimX(), FullBBox.DimZ(), FullBBox.DimY());// = md.groove;
	float orthox = FullBBox.DimX() / 2.;
	float orthoy = FullBBox.DimY() / 2.;

	//std::map<int, std::vector<cv::Mat> *> testSaveImage;
	for (int i = start_page, j = start_page + 1; i < boxy_dim; i++, j++)
	{
		glContext->makeCurrent();
		time.restart();

		/*  glMatrixMode(GL_PROJECTION); glPopMatrix();
		glMatrixMode(GL_MODELVIEW); glPopMatrix();
		glDisable(GL_LIGHTING);*/
		//===========================================================================================
		equ[3] = box_bottom + unit*i;
		//***第二張圖***產生膠水圖**********//
		//time.restart();	

		//for (int count = 0; count < md.meshList.size(); count++)
		int countj = 0;
		//foreach(MeshModel *floorMesh, md.meshList)
		foreach(MeshModel *floorMesh, mdlist)
		{
			vector<cv::Mat> *meshlayerImage;
			if (testSaveImage.find(floorMesh->id()) == testSaveImage.end())
			{
				meshlayerImage = new vector<cv::Mat>;
				testSaveImage.insert(pair<int, vector<cv::Mat> *>(floorMesh->id(), meshlayerImage));
			}
			else
			{
				meshlayerImage = testSaveImage.at(floorMesh->id());
			}

			if (floorMesh->getMeshSort() != MeshModel::meshsort::print_item)continue;

			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			//glOrtho(FullBBox.min.X(), FullBBox.max.X(), FullBBox.min.Y(), FullBBox.max.Y(), -1000, 1000);
			glOrtho(-orthox, orthox, -orthoy, orthoy, -1000, 1000);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
			vcg::Point3f x = FullBBox.Center();
			gluLookAt(FullBBox.Center().X(), FullBBox.Center().Y(), DSP_Look_Distination, FullBBox.Center().X(), FullBBox.Center().Y(), -DSP_Look_Distination, 0, 1, 0);
			//gluLookAt(-80, 0, DSP_Look_Distination,-80, 0, -DSP_Look_Distination, 0, 1, 0);
			//*********************************
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);

			//binderstrips(md, par.getFloat("useless_print"));//***uselessbar

			//************************************************//
			//****** Rendering the mesh's clip edge ****//
			GLuint MY_CLIP_PLANE = GL_CLIP_PLANE0;
			glEnable(MY_CLIP_PLANE);
			glClipPlane(MY_CLIP_PLANE, equ);

			glEnable(GL_STENCIL_TEST);
			glEnable(GL_CULL_FACE);
			glClear(GL_STENCIL_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

			//// first pass: increment stencil buffer value on back faces
			glStencilFunc(GL_ALWAYS, 0, 0);
			glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
			glCullFace(GL_FRONT); // render back faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			//glRotatef(90, 1, 0, 0);
			//foreach(MeshModel * mp, md.meshList)
			//{
			// if (mp->getMeshSort() == MeshModel::meshsort::print_item)
			//  //mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
			//  mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
			//}										  
			floorMesh->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);

			glPopMatrix();
			glPopAttrib();
			//second pass: decrement stencil buffer value on front faces
			glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
			glCullFace(GL_BACK); // render front faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			//glRotatef(90, 1, 0, 0);
			//foreach(MeshModel * mp, md.meshList)
			//{
			// if (mp->getMeshSort() == MeshModel::meshsort::print_item)
			//  //mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
			//  mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
			//}

			floorMesh->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);

			glPopMatrix();
			glPopAttrib();
			//-----------------------------
			// drawing clip planes masked by stencil buffer content
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glEnable(GL_DEPTH_TEST);
			glDisable(MY_CLIP_PLANE);
			glStencilFunc(GL_NOTEQUAL, 0, ~0);
			// stencil test will pass only when stencil buffer value = 0; 
			// (~0 = 0x11...11)

			glColor4f(1, 1, 1, 1);
			fillsquare();
			glDisable(GL_STENCIL_TEST);
			glEnable(MY_CLIP_PLANE); // enabling clip plane again
			glDisable(GL_LIGHTING);

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			//********************************************************************************************//


			glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, cap_image.bits());//***20150724
			//glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, cap_image.bits());							  
			//QString img_file_name2 = md.p_setting.getcapImageName();
			glDisable(MY_CLIP_PLANE);//存完圖
			glDisable(GL_CULL_FACE);
			//****
			//(md.logger, framework::Diagnostics::LogLevel::Info, _T(""));
			cv::Mat captemp;
			//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T(""));

			cv::cvtColor(QImageToCvMat(cap_image.mirrored()), captemp, CV_RGB2GRAY);
			bitwise_not(captemp, captemp);
			meshlayerImage->push_back(captemp);

			if (savedebugImage){
				QImage testttt = cvMatToQImage(captemp);
				QString capimagest = "captemp_image";
				capimagest.append(QString("_%1_%2.png").arg(i, 4, 10, QChar('0')).arg(countj, 4, 10, QChar('0')));
				testttt.save(dir.absolutePath() + "/" + capimagest, "png");
				countj++;
			}

			//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T(""));

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();


		}

	}
	//std::multimap<int, int> * tempIntersectMeshesID = md.getIntersectMeshesID();								  
	//md.setIntersectMeshesID2(SKT::checkImageOverlap(savefloorMeshImage));

#endif	

	ctx.unbindReadDrawFramebuffer();
	ctx.release();
	glPopAttrib();
	glContext->doneCurrent();




	//foreach(MeshModel *bm, md.meshList)
	//{
	//	if ((bm->label().contains(olMName, Qt::CaseSensitive) || bm->label().contains(cMName, Qt::CaseSensitive)))
	//		//if (bm->label().contains(olMName, Qt::CaseSensitive) || bm->label().contains(cMName, Qt::CaseSensitive)))
	//		//if (bm->getMeshSort()==MeshModel::meshsort::)
	//	{
	//		md.delMesh(bm);
	//	}
	//}
	qDebug() << " done _applyClick ";
	return true;
}

bool GenSliceJob::getMeshLayerImagePlaneX(QList<MeshModel *> mdlist, std::map<int, std::vector<cv::Mat> *> &testSaveImage)
{


	CMeshO & mesh = mdlist.at(0)->cm;
	if ((mesh.vn < 3) || (mesh.fn < 1)) return false;

	const unsigned char * p0 = (const unsigned char *)(&(mesh.vert[0].P()));
	const unsigned char * p1 = (const unsigned char *)(&(mesh.vert[1].P()));
	const void *          pbase = p0;
	GLsizei               pstride = GLsizei(p1 - p0);

	const unsigned char * n0 = (const unsigned char *)(&(mesh.vert[0].N()));
	const unsigned char * n1 = (const unsigned char *)(&(mesh.vert[1].N()));
	const void *          nbase = n0;
	GLsizei               nstride = GLsizei(n1 - n0);

	glContext->makeCurrent();


	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
	}

	glPushAttrib(GL_ALL_ATTRIB_BITS);

	Context ctx;
	ctx.acquire();

	const GLsizeiptr psize = GLsizeiptr(GLsizei(mesh.vn) * pstride);
	BufferHandle hPositionBuffer = createBuffer(ctx, psize, pbase);

	const GLsizeiptr nsize = GLsizeiptr(GLsizei(mesh.vn) * nstride);
	BufferHandle hNormalBuffer = createBuffer(ctx, nsize, nbase);

	const GLsizeiptr isize = GLsizeiptr(mesh.fn * 3 * sizeof(GLuint));
	BufferHandle hIndexBuffer = createBuffer(ctx, isize);
	{
		BoundIndexBufferHandle indexBuffer = ctx.bindIndexBuffer(hIndexBuffer);

		const CMeshO::VertexType * vbase = &(mesh.vert[0]);
		GLuint *  indices = (GLuint *)indexBuffer->map(GL_WRITE_ONLY);
		for (size_t i = 0; i < mesh.face.size(); ++i)
		{
			const CMeshO::FaceType & f = mesh.face[i];
			if (f.IsD()) continue;
			for (int v = 0; v < 3; ++v)
			{
				*indices++ = GLuint(vcg::tri::Index(mesh, f.cV(v)));
			}
		}
		indexBuffer->unmap();

		ctx.unbindIndexBuffer();
	}



	if (mdlist.size() == 0)return true;
	Box3m FullBBox;
	Matrix44m iden;
	iden.SetIdentity();
	foreach(MeshModel *mp, mdlist)
	{
		FullBBox.Add(iden, mp->cm.bbox);
	}




	//***20150907
	const GLsizei print_dpi = 25;// GLsizei(par.getInt("print_dpi"));	
	float y = FullBBox.DimY();
	float z = FullBBox.DimZ();
	const GLsizei width = (int)(y / DSP_inchmm * print_dpi);
	const GLsizei height = (int)(width * z / y);


	RenderbufferHandle hDepth = createRenderbuffer(ctx, GL_DEPTH24_STENCIL8, width, height);
	Texture2DHandle    hColor = createTexture2D(ctx, GL_RGB8, width, height, GL_RGB, GL_UNSIGNED_BYTE);
	//FramebufferHandle  hFramebuffer = createFramebuffer(ctx, renderbufferTarget(hDepth), texture2DTarget(hColor));
	FramebufferHandle  hFramebuffer = createFramebufferWithDepthStencil(ctx, renderbufferTarget(hDepth), renderbufferTarget(hDepth), texture2DTarget(hColor));




	const vcg::Point3f lightDirectionVS = vcg::Point3f(0.0f, 0.0f, -1.0f).Normalize();

	glEnable(GL_DEPTH_TEST);
	//glClearColor(GLfloat(backgroundColor.red())/255.0f, GLfloat(backgroundColor.green())/255.0f, GLfloat(backgroundColor.blue())/255.0f, 0.0f);


	glViewport(0, 0, width, height);//***window的長寬				  

	//const vcg::Point3f center = mesh.bbox.Center();
	//const float        scale = 1.0f / mesh.bbox.Diag();

	//glScalef(scale, scale, scale);
	//glTranslatef(-center[0], -center[1], -center[2]);

	//***儲存image的基本資訊
	QImage image(int(width), int(height), QImage::Format_RGB888);
	QImage image2(int(width), int(height), QImage::Format_RGB888);
	QImage slice0Point05mmImage(int(width), int(height), QImage::Format_RGB888);
	QImage cap_image(int(width), int(height), QImage::Format_RGB888);
	//*******************

	QString temppath = PicaApplication::getRoamingDir();
	QDir dir(temppath);//backup
	if (dir.exists("xyzimage"))
	{
		dir.setPath(temppath + "/xyzimage");
		dir.removeRecursively();
		dir.setPath(temppath);
		dir.mkpath("xyzimage");
	}
	else
	{
		dir.setPath(temppath);
		dir.mkpath("xyzimage");
	}

	dir.setPath(temppath + "/xyzimage");

	//==============================
	//**********************
	ctx.bindReadDrawFramebuffer(hFramebuffer);
	GLW_CHECK_GL_READ_DRAW_FRAMEBUFFER_STATUS;
#if 1


	glClearColor(1, 1, 1, 1);
	//***opencvMat clear
	capCTemp.clear();
	outlineCTemp.clear();

	//float meCorrectPercent = 0.97;
	float meCorrectPercent = 1;


	int widthrecord, heightrecord;
	QString blk_img_file_name = "black_outline";


	std::vector<cv::Mat> blankTemp;
	std::vector<cv::Mat> blankcapCTemp;

	QTime time;
	//clock_t start, stop;
	time.start();

	QDir patternPath(PicaApplication::getRoamingDir());

	bool savedebugImage = true;// par.getBool(("generate_final_picture"));


	float unit = 2;
	int boxy_dim = qCeil((FullBBox.max.X() - FullBBox.min.X()) / unit) + 1;

	float box_Left = FullBBox.min.X();
	int start_page = 0;// par.getInt("start_page");
	//Construction_Groove<float> tempgroove(FullBBox.DimX(), FullBBox.DimZ(), FullBBox.DimY());// = md.groove;
	float orthoWidth = FullBBox.DimY() / 2.;
	float orthoHeight = FullBBox.DimZ() / 2.;

	//std::map<int, std::vector<cv::Mat> *> testSaveImage;
	for (int i = start_page, j = start_page + 1; i < boxy_dim; i++, j++)
	{
		glContext->makeCurrent();
		time.restart();

		/*  glMatrixMode(GL_PROJECTION); glPopMatrix();
		glMatrixMode(GL_MODELVIEW); glPopMatrix();
		glDisable(GL_LIGHTING);*/
		//===========================================================================================
		equ2[3] = box_Left + unit*i;
		//***第二張圖***產生膠水圖**********//
		//time.restart();	

		//for (int count = 0; count < md.meshList.size(); count++)
		int countj = 0;
		//foreach(MeshModel *floorMesh, md.meshList)
		foreach(MeshModel *floorMesh, mdlist)
		{
			vector<cv::Mat> *meshlayerImage;
			if (testSaveImage.find(floorMesh->id()) == testSaveImage.end())
			{
				meshlayerImage = new vector<cv::Mat>;
				testSaveImage.insert(pair<int, vector<cv::Mat> *>(floorMesh->id(), meshlayerImage));
			}
			else
			{
				meshlayerImage = testSaveImage.at(floorMesh->id());
			}

			if (floorMesh->getMeshSort() != MeshModel::meshsort::print_item)continue;

			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			//glOrtho(FullBBox.min.X(), FullBBox.max.X(), FullBBox.min.Y(), FullBBox.max.Y(), -1000, 1000);
			glOrtho(-orthoWidth, orthoWidth, -orthoHeight, orthoHeight, -1000, 1000);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
			vcg::Point3f x = FullBBox.Center();
			gluLookAt(DSP_Look_Distination, FullBBox.Center().Y(), FullBBox.Center().Z(), -DSP_Look_Distination, FullBBox.Center().Y(), FullBBox.Center().Z(), 0, 0, 1);
			//gluLookAt(-80, 0, DSP_Look_Distination,-80, 0, -DSP_Look_Distination, 0, 1, 0);
			//*********************************
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1, 1, 1, 1);

			//binderstrips(md, par.getFloat("useless_print"));//***uselessbar
			/*float lightposition[4] = { 400, 0, 0, 0 };
			glEnable(GL_LIGHTING);
			glEnable(GL_LIGHT0);
			glLightfv(GL_LIGHT0, GL_POSITION, lightposition);*/
			//glDisable(GL_LIGHTING);
			//floorMesh->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
			//************************************************//			
			//****** Rendering the mesh's clip edge ****//
			GLuint MY_CLIP_PLANE = GL_CLIP_PLANE0;
			glEnable(MY_CLIP_PLANE);
			glClipPlane(MY_CLIP_PLANE, equ2);

			glEnable(GL_STENCIL_TEST);
			glEnable(GL_CULL_FACE);
			glClear(GL_STENCIL_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

			//// first pass: increment stencil buffer value on back faces
			glStencilFunc(GL_ALWAYS, 0, 0);
			glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
			glCullFace(GL_FRONT); // render back faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			floorMesh->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);

			glPopMatrix();
			glPopAttrib();
			//second pass: decrement stencil buffer value on front faces
			glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
			glCullFace(GL_BACK); // render front faces only
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glEnable(GL_LIGHTING);
			glPushMatrix();

			floorMesh->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);

			glPopMatrix();
			glPopAttrib();
			//-----------------------------
			// drawing clip planes masked by stencil buffer content
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glEnable(GL_DEPTH_TEST);
			glDisable(MY_CLIP_PLANE);
			glStencilFunc(GL_NOTEQUAL, 0, ~0);
			// stencil test will pass only when stencil buffer value = 0; 
			// (~0 = 0x11...11)

			glColor4f(1, 1, 1, 1);

			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glDisable(GL_LIGHTING);
			glColor3f(0, 0, 0);
			glPushMatrix();

			glNormal3f(-1, 0, 0);
			glBegin(GL_QUADS);
			glVertex3f(equ2[3], -ollY, -ollZ);
			glVertex3f(equ2[3], ollY, -ollZ);
			glVertex3f(equ2[3], ollY, ollZ);
			glVertex3f(equ2[3], -ollY, ollZ);
			glEnd();

			//glRecti(-5, -5, 5, 5);

			glPopMatrix();
			glPopAttrib();
			glDisable(GL_STENCIL_TEST);
			glEnable(MY_CLIP_PLANE); // enabling clip plane again
			glDisable(GL_LIGHTING);

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			//********************************************************************************************//


			glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, cap_image.bits());//***20150724
			//glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, cap_image.bits());							  
			//QString img_file_name2 = md.p_setting.getcapImageName();
			glDisable(MY_CLIP_PLANE);//存完圖
			glDisable(GL_CULL_FACE);
			//****
			//(md.logger, framework::Diagnostics::LogLevel::Info, _T(""));
			cv::Mat captemp;
			//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T(""));

			cv::cvtColor(QImageToCvMat(cap_image.mirrored()), captemp, CV_RGB2GRAY);
			bitwise_not(captemp, captemp);
			meshlayerImage->push_back(captemp);

			if (savedebugImage){
				QImage testttt = cvMatToQImage(captemp);
				QString capimagest = "captemp_image";
				capimagest.append(QString("_%1_%2.png").arg(i, 4, 10, QChar('0')).arg(countj, 4, 10, QChar('0')));
				testttt.save(dir.absolutePath() + "/" + capimagest, "png");
				countj++;
			}

			//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T(""));

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();


		}

	}
	//std::multimap<int, int> * tempIntersectMeshesID = md.getIntersectMeshesID();								  
	//md.setIntersectMeshesID2(SKT::checkImageOverlap(savefloorMeshImage));

#endif	

	ctx.unbindReadDrawFramebuffer();
	ctx.release();
	glPopAttrib();
	glContext->doneCurrent();




	//foreach(MeshModel *bm, md.meshList)
	//{
	//	if ((bm->label().contains(olMName, Qt::CaseSensitive) || bm->label().contains(cMName, Qt::CaseSensitive)))
	//		//if (bm->label().contains(olMName, Qt::CaseSensitive) || bm->label().contains(cMName, Qt::CaseSensitive)))
	//		//if (bm->getMeshSort()==MeshModel::meshsort::)
	//	{
	//		md.delMesh(bm);
	//	}
	//}
	qDebug() << " done _applyClick ";
	return true;
}
bool GenSliceJob::getMultiMeshLayerImage(QList<MeshModel *> mdlist, std::vector<cv::Mat> &testSaveImage, Box3m boxAll)
{


	CMeshO & mesh = mdlist.at(0)->cm;
	if ((mesh.vn < 3) || (mesh.fn < 1)) return false;

	const unsigned char * p0 = (const unsigned char *)(&(mesh.vert[0].P()));
	const unsigned char * p1 = (const unsigned char *)(&(mesh.vert[1].P()));
	const void *          pbase = p0;
	GLsizei               pstride = GLsizei(p1 - p0);

	const unsigned char * n0 = (const unsigned char *)(&(mesh.vert[0].N()));
	const unsigned char * n1 = (const unsigned char *)(&(mesh.vert[1].N()));
	const void *          nbase = n0;
	GLsizei               nstride = GLsizei(n1 - n0);

	glContext->makeCurrent();


	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
	}

	glPushAttrib(GL_ALL_ATTRIB_BITS);

	Context ctx;
	ctx.acquire();

	const GLsizeiptr psize = GLsizeiptr(GLsizei(mesh.vn) * pstride);
	BufferHandle hPositionBuffer = createBuffer(ctx, psize, pbase);

	const GLsizeiptr nsize = GLsizeiptr(GLsizei(mesh.vn) * nstride);
	BufferHandle hNormalBuffer = createBuffer(ctx, nsize, nbase);

	const GLsizeiptr isize = GLsizeiptr(mesh.fn * 3 * sizeof(GLuint));
	BufferHandle hIndexBuffer = createBuffer(ctx, isize);
	{
		BoundIndexBufferHandle indexBuffer = ctx.bindIndexBuffer(hIndexBuffer);

		const CMeshO::VertexType * vbase = &(mesh.vert[0]);
		GLuint *  indices = (GLuint *)indexBuffer->map(GL_WRITE_ONLY);
		for (size_t i = 0; i < mesh.face.size(); ++i)
		{
			const CMeshO::FaceType & f = mesh.face[i];
			if (f.IsD()) continue;
			for (int v = 0; v < 3; ++v)
			{
				*indices++ = GLuint(vcg::tri::Index(mesh, f.cV(v)));
			}
		}
		indexBuffer->unmap();

		ctx.unbindIndexBuffer();
	}



	if (mdlist.size() == 0)return true;
	Box3m FullBBox;
	Matrix44m iden;
	iden.SetIdentity();
	foreach(MeshModel *mp, mdlist)
	{
		FullBBox.Add(iden, mp->cm.bbox);
	}




	//***20150907
	const GLsizei print_dpi = 25;// GLsizei(par.getInt("print_dpi"));	
	float y = boxAll.DimY();
	float z = boxAll.DimZ();
	const GLsizei width = (int)(y / DSP_inchmm * print_dpi);
	const GLsizei height = (int)(width * z / y);


	RenderbufferHandle hDepth = createRenderbuffer(ctx, GL_DEPTH24_STENCIL8, width, height);
	Texture2DHandle    hColor = createTexture2D(ctx, GL_RGB8, width, height, GL_RGB, GL_UNSIGNED_BYTE);
	//FramebufferHandle  hFramebuffer = createFramebuffer(ctx, renderbufferTarget(hDepth), texture2DTarget(hColor));
	FramebufferHandle  hFramebuffer = createFramebufferWithDepthStencil(ctx, renderbufferTarget(hDepth), renderbufferTarget(hDepth), texture2DTarget(hColor));




	const vcg::Point3f lightDirectionVS = vcg::Point3f(0.0f, 0.0f, -1.0f).Normalize();

	glEnable(GL_DEPTH_TEST);

	glViewport(0, 0, width, height);//***window的長寬				  	

	//***儲存image的基本資訊
	QImage image(int(width), int(height), QImage::Format_RGB888);
	QImage image2(int(width), int(height), QImage::Format_RGB888);
	QImage slice0Point05mmImage(int(width), int(height), QImage::Format_RGB888);
	QImage cap_image(int(width), int(height), QImage::Format_RGB888);
	//*******************

	QString temppath = PicaApplication::getRoamingDir();
	QDir dir(temppath);//backup
	if (dir.exists("xyzimage"))
	{
		dir.setPath(temppath + "/xyzimage");
		dir.removeRecursively();
		dir.setPath(temppath);
		dir.mkpath("xyzimage");
	}
	else
	{
		dir.setPath(temppath);
		dir.mkpath("xyzimage");
	}

	dir.setPath(temppath + "/xyzimage");

	//==============================
	//**********************
	ctx.bindReadDrawFramebuffer(hFramebuffer);
	GLW_CHECK_GL_READ_DRAW_FRAMEBUFFER_STATUS;
#if 1


	glClearColor(1, 1, 1, 1);
	//***opencvMat clear
	capCTemp.clear();
	outlineCTemp.clear();

	//float meCorrectPercent = 0.97;
	float meCorrectPercent = 1;


	int widthrecord, heightrecord;
	QString blk_img_file_name = "black_outline";


	std::vector<cv::Mat> blankTemp;
	std::vector<cv::Mat> blankcapCTemp;

	QTime time;
	//clock_t start, stop;
	time.start();

	QDir patternPath(PicaApplication::getRoamingDir());

	bool savedebugImage = true;// par.getBool(("generate_final_picture"));


	float unit = 2;
	int boxy_dim = qCeil((boxAll.max.X() - boxAll.min.X()) / unit) + 1;

	float box_Left = boxAll.min.X();
	int start_page = 0;// par.getInt("start_page");	
	float orthoWidth = boxAll.DimY() / 2.;
	float orthoHeight = boxAll.DimZ() / 2.;


	for (int i = start_page, j = start_page + 1; i < boxy_dim; i++, j++)
	{
		glContext->makeCurrent();
		time.restart();


		//===========================================================================================
		equ2[3] = box_Left + unit*i;

		//time.restart();
		int countj = 0;

		glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
		//glOrtho(FullBBox.min.X(), FullBBox.max.X(), FullBBox.min.Y(), FullBBox.max.Y(), -1000, 1000);
		glOrtho(-orthoWidth, orthoWidth, -orthoHeight, orthoHeight, -1000, 1000);
		glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
		vcg::Point3f x = boxAll.Center();
		gluLookAt(DSP_Look_Distination, boxAll.Center().Y(), boxAll.Center().Z(), -DSP_Look_Distination, boxAll.Center().Y(), boxAll.Center().Z(), 0, 0, 1);
		//gluLookAt(-80, 0, DSP_Look_Distination,-80, 0, -DSP_Look_Distination, 0, 1, 0);
		//*********************************
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearColor(1, 1, 1, 1);

		//binderstrips(md, par.getFloat("useless_print"));//***uselessbar		
		//************************************************//			
		//****** Rendering the mesh's clip edge ****//
		GLuint MY_CLIP_PLANE = GL_CLIP_PLANE0;
		glEnable(MY_CLIP_PLANE);
		glClipPlane(MY_CLIP_PLANE, equ2);

		glEnable(GL_STENCIL_TEST);
		glEnable(GL_CULL_FACE);
		glClear(GL_STENCIL_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

		//// first pass: increment stencil buffer value on back faces
		glStencilFunc(GL_ALWAYS, 0, 0);
		glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
		glCullFace(GL_FRONT); // render back faces only
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glEnable(GL_LIGHTING);
		glPushMatrix();

		foreach(MeshModel *floorMesh, mdlist)
		{
			floorMesh->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
		}

		glPopMatrix();
		glPopAttrib();
		//second pass: decrement stencil buffer value on front faces
		glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
		glCullFace(GL_BACK); // render front faces only
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glEnable(GL_LIGHTING);
		glPushMatrix();
		foreach(MeshModel *floorMesh, mdlist)
		{
			floorMesh->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
		}

		glPopMatrix();
		glPopAttrib();
		//-----------------------------
		// drawing clip planes masked by stencil buffer content
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		glDisable(MY_CLIP_PLANE);
		glStencilFunc(GL_NOTEQUAL, 0, ~0);
		// stencil test will pass only when stencil buffer value = 0; 
		// (~0 = 0x11...11)

		glColor4f(1, 1, 1, 1);

		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glDisable(GL_LIGHTING);
		glColor3f(0, 0, 0);
		glPushMatrix();

		glNormal3f(-1, 0, 0);
		glBegin(GL_QUADS);
		glVertex3f(equ2[3], -ollY, -ollZ);
		glVertex3f(equ2[3], ollY, -ollZ);
		glVertex3f(equ2[3], ollY, ollZ);
		glVertex3f(equ2[3], -ollY, ollZ);
		glEnd();

		//glRecti(-5, -5, 5, 5);

		glPopMatrix();
		glPopAttrib();
		glDisable(GL_STENCIL_TEST);
		glEnable(MY_CLIP_PLANE); // enabling clip plane again
		glDisable(GL_LIGHTING);

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		//********************************************************************************************//


		glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, cap_image.bits());//***20150724
		//glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, cap_image.bits());							  
		//QString img_file_name2 = md.p_setting.getcapImageName();
		glDisable(MY_CLIP_PLANE);//存完圖
		glDisable(GL_CULL_FACE);
		//****
		//(md.logger, framework::Diagnostics::LogLevel::Info, _T(""));
		cv::Mat captemp;
		//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T(""));

		cv::cvtColor(QImageToCvMat(cap_image.mirrored()), captemp, CV_RGB2GRAY);
		bitwise_not(captemp, captemp);
		testSaveImage.push_back(captemp);
		if (savedebugImage){
			QImage testttt = cvMatToQImage(captemp);
			QString capimagest = "captemp_image";
			capimagest.append(QString("_%1_%2.png").arg(i, 4, 10, QChar('0')).arg(countj, 4, 10, QChar('0')));
			testttt.save(dir.absolutePath() + "/" + capimagest, "png");
			countj++;
		}

		//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T(""));

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();


	}
	//std::multimap<int, int> * tempIntersectMeshesID = md.getIntersectMeshesID();								  
	//md.setIntersectMeshesID2(SKT::checkImageOverlap(savefloorMeshImage));

#endif	

	ctx.unbindReadDrawFramebuffer();
	ctx.release();
	glPopAttrib();
	glContext->doneCurrent();

	qDebug() << " done _applyClick ";
	return true;
}
bool GenSliceJob::getMultiMeshLayerImageZ(QList<MeshModel *> mdlist, std::vector<cv::Mat> &testSaveImage, Box3m boxAll)
{


	CMeshO & mesh = mdlist.at(0)->cm;
	if ((mesh.vn < 3) || (mesh.fn < 1)) return false;

	const unsigned char * p0 = (const unsigned char *)(&(mesh.vert[0].P()));
	const unsigned char * p1 = (const unsigned char *)(&(mesh.vert[1].P()));
	const void *          pbase = p0;
	GLsizei               pstride = GLsizei(p1 - p0);

	const unsigned char * n0 = (const unsigned char *)(&(mesh.vert[0].N()));
	const unsigned char * n1 = (const unsigned char *)(&(mesh.vert[1].N()));
	const void *          nbase = n0;
	GLsizei               nstride = GLsizei(n1 - n0);

	glContext->makeCurrent();


	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
	}

	glPushAttrib(GL_ALL_ATTRIB_BITS);

	Context ctx;
	ctx.acquire();

	const GLsizeiptr psize = GLsizeiptr(GLsizei(mesh.vn) * pstride);
	BufferHandle hPositionBuffer = createBuffer(ctx, psize, pbase);

	const GLsizeiptr nsize = GLsizeiptr(GLsizei(mesh.vn) * nstride);
	BufferHandle hNormalBuffer = createBuffer(ctx, nsize, nbase);

	const GLsizeiptr isize = GLsizeiptr(mesh.fn * 3 * sizeof(GLuint));
	BufferHandle hIndexBuffer = createBuffer(ctx, isize);
	{
		BoundIndexBufferHandle indexBuffer = ctx.bindIndexBuffer(hIndexBuffer);

		const CMeshO::VertexType * vbase = &(mesh.vert[0]);
		GLuint *  indices = (GLuint *)indexBuffer->map(GL_WRITE_ONLY);
		for (size_t i = 0; i < mesh.face.size(); ++i)
		{
			const CMeshO::FaceType & f = mesh.face[i];
			if (f.IsD()) continue;
			for (int v = 0; v < 3; ++v)
			{
				*indices++ = GLuint(vcg::tri::Index(mesh, f.cV(v)));
			}
		}
		indexBuffer->unmap();

		ctx.unbindIndexBuffer();
	}



	if (mdlist.size() == 0)return true;

	//***20150907
	const GLsizei print_dpi = 25;// GLsizei(par.getInt("print_dpi"));	
	float x = boxAll.DimX();
	float y = boxAll.DimY();
	const GLsizei width = (int)(x / DSP_inchmm * print_dpi);
	const GLsizei height = (int)(width * y / x);


	RenderbufferHandle hDepth = createRenderbuffer(ctx, GL_DEPTH24_STENCIL8, width, height);
	Texture2DHandle    hColor = createTexture2D(ctx, GL_RGB8, width, height, GL_RGB, GL_UNSIGNED_BYTE);
	//FramebufferHandle  hFramebuffer = createFramebuffer(ctx, renderbufferTarget(hDepth), texture2DTarget(hColor));
	FramebufferHandle  hFramebuffer = createFramebufferWithDepthStencil(ctx, renderbufferTarget(hDepth), renderbufferTarget(hDepth), texture2DTarget(hColor));




	const vcg::Point3f lightDirectionVS = vcg::Point3f(0.0f, 0.0f, -1.0f).Normalize();

	glEnable(GL_DEPTH_TEST);

	glViewport(0, 0, width, height);//***window的長寬				  	

	//***儲存image的基本資訊
	QImage image(int(width), int(height), QImage::Format_RGB888);
	QImage image2(int(width), int(height), QImage::Format_RGB888);
	QImage slice0Point05mmImage(int(width), int(height), QImage::Format_RGB888);
	QImage cap_image(int(width), int(height), QImage::Format_RGB888);
	//*******************

	QString temppath = PicaApplication::getRoamingDir();
	QDir dir(temppath);//backup
	if (dir.exists("xyzimage"))
	{
		dir.setPath(temppath + "/xyzimage");
		dir.removeRecursively();
		dir.setPath(temppath);
		dir.mkpath("xyzimage");
	}
	else
	{
		dir.setPath(temppath);
		dir.mkpath("xyzimage");
	}

	dir.setPath(temppath + "/xyzimage");

	//==============================
	//**********************
	ctx.bindReadDrawFramebuffer(hFramebuffer);
	GLW_CHECK_GL_READ_DRAW_FRAMEBUFFER_STATUS;
#if 1


	glClearColor(1, 1, 1, 1);
	//***opencvMat clear
	capCTemp.clear();
	outlineCTemp.clear();

	//float meCorrectPercent = 0.97;
	float meCorrectPercent = 1;


	int widthrecord, heightrecord;
	QString blk_img_file_name = "black_outline";


	std::vector<cv::Mat> blankTemp;
	std::vector<cv::Mat> blankcapCTemp;

	QTime time;
	//clock_t start, stop;
	time.start();

	QDir patternPath(PicaApplication::getRoamingDir());

	bool savedebugImage = false;// par.getBool(("generate_final_picture"));


	float unit = 2;
	int boxy_dim = qCeil((boxAll.max.Z() - boxAll.min.Z()) / unit) + 1;

	float box_bottom = boxAll.min.Z();
	int start_page = 0;// par.getInt("start_page");	
	float orthoWidth = boxAll.DimX() / 2.;
	float orthoHeight = boxAll.DimY() / 2.;


	for (int i = start_page, j = start_page + 1; i < boxy_dim; i++, j++)
	{
		glContext->makeCurrent();
		time.restart();


		//===========================================================================================
		equ[3] = box_bottom + unit*i;

		//time.restart();
		int countj = 0;

		glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
		//glOrtho(FullBBox.min.X(), FullBBox.max.X(), FullBBox.min.Y(), FullBBox.max.Y(), -1000, 1000);
		glOrtho(-orthoWidth, orthoWidth, -orthoHeight, orthoHeight, -1000, 1000);
		glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
		vcg::Point3f x = boxAll.Center();
		gluLookAt(boxAll.Center().X(), boxAll.Center().Y(), DSP_Look_Distination, boxAll.Center().X(), boxAll.Center().Y(), -DSP_Look_Distination, 0, 1, 0);
		//gluLookAt(-80, 0, DSP_Look_Distination,-80, 0, -DSP_Look_Distination, 0, 1, 0);
		//*********************************
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearColor(1, 1, 1, 1);

		//binderstrips(md, par.getFloat("useless_print"));//***uselessbar		
		//************************************************//			
		//****** Rendering the mesh's clip edge ****//
		GLuint MY_CLIP_PLANE = GL_CLIP_PLANE0;
		glEnable(MY_CLIP_PLANE);
		glClipPlane(MY_CLIP_PLANE, equ);

		glEnable(GL_STENCIL_TEST);
		glEnable(GL_CULL_FACE);
		glClear(GL_STENCIL_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

		//// first pass: increment stencil buffer value on back faces
		glStencilFunc(GL_ALWAYS, 0, 0);
		glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
		glCullFace(GL_FRONT); // render back faces only
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glEnable(GL_LIGHTING);
		glPushMatrix();

		foreach(MeshModel *floorMesh, mdlist)
		{
			floorMesh->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
		}

		glPopMatrix();
		glPopAttrib();
		//second pass: decrement stencil buffer value on front faces
		glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
		glCullFace(GL_BACK); // render front faces only
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glEnable(GL_LIGHTING);
		glPushMatrix();
		foreach(MeshModel *floorMesh, mdlist)
		{
			floorMesh->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
		}

		glPopMatrix();
		glPopAttrib();
		//-----------------------------
		// drawing clip planes masked by stencil buffer content
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		glDisable(MY_CLIP_PLANE);
		glStencilFunc(GL_NOTEQUAL, 0, ~0);
		// stencil test will pass only when stencil buffer value = 0; 
		// (~0 = 0x11...11)

		glColor4f(1, 1, 1, 1);

		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glDisable(GL_LIGHTING);
		glColor3f(0, 0, 0);
		glPushMatrix();

		glNormal3f(0, 0, -1);
		glBegin(GL_QUADS);
		glVertex3f(-ollX, -ollY, equ[3]);
		glVertex3f(ollX, -ollY, equ[3]);
		glVertex3f(ollX, ollY, equ[3]);
		glVertex3f(-ollX, ollY, equ[3]);
		glEnd();

		//glRecti(-5, -5, 5, 5);

		glPopMatrix();
		glPopAttrib();
		glDisable(GL_STENCIL_TEST);
		glEnable(MY_CLIP_PLANE); // enabling clip plane again
		glDisable(GL_LIGHTING);

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		//********************************************************************************************//


		glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, cap_image.bits());//***20150724
		//glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, cap_image.bits());							  
		//QString img_file_name2 = md.p_setting.getcapImageName();
		glDisable(MY_CLIP_PLANE);//存完圖
		glDisable(GL_CULL_FACE);
		//****
		//(md.logger, framework::Diagnostics::LogLevel::Info, _T(""));
		cv::Mat captemp;
		//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T(""));

		cv::cvtColor(QImageToCvMat(cap_image.mirrored()), captemp, CV_RGB2GRAY);
		bitwise_not(captemp, captemp);
		testSaveImage.push_back(captemp);
		if (savedebugImage){
			QImage testttt = cvMatToQImage(captemp);
			QString capimagest = "captemp_image";
			capimagest.append(QString("_%1_%2.png").arg(i, 4, 10, QChar('0')).arg(countj, 4, 10, QChar('0')));
			testttt.save(dir.absolutePath() + "/" + capimagest, "png");
			countj++;
		}

		//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T(""));

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();


	}
	//std::multimap<int, int> * tempIntersectMeshesID = md.getIntersectMeshesID();								  
	//md.setIntersectMeshesID2(SKT::checkImageOverlap(savefloorMeshImage));

#endif	

	ctx.unbindReadDrawFramebuffer();
	ctx.release();
	glPopAttrib();
	glContext->doneCurrent();

	qDebug() << " done _applyClick ";
	return true;
}

//bool GenSliceJob::testDepthBuffer(QList<MeshModel *> mdlist, std::vector<cv::Mat> &testSaveImage, Box3m boxAll, QVector<bool> &genbool, bool sscale,bool copyToBig, int DPI)
//{
//	/*CMeshO & mesh = mdlist.at(0)->cm;
//	if ((mesh.vn < 3) || (mesh.fn < 1)) return false;
//
//	const unsigned char * p0 = (const unsigned char *)(&(mesh.vert[0].P()));
//	const unsigned char * p1 = (const unsigned char *)(&(mesh.vert[1].P()));
//	const void *          pbase = p0;
//	GLsizei               pstride = GLsizei(p1 - p0);
//
//	const unsigned char * n0 = (const unsigned char *)(&(mesh.vert[0].N()));
//	const unsigned char * n1 = (const unsigned char *)(&(mesh.vert[1].N()));
//	const void *          nbase = n0;
//	GLsizei               nstride = GLsizei(n1 - n0);*/
//
//	QString blk_img_file_name = "black_outline";
//	QString temppath = PicaApplication::getRoamingDir();
//	QDir dir(temppath);//backup
//	if (dir.exists("xyzimage"))
//	{
//		dir.setPath(temppath + "/xyzimage");
//		dir.removeRecursively();
//		dir.setPath(temppath);
//		dir.mkpath("xyzimage");
//	}
//	else
//	{
//		dir.setPath(temppath);
//		dir.mkpath("xyzimage");
//	}
//
//	dir.setPath(temppath + "/xyzimage");
//
//	//===for top down
//	float orthoWidth = boxAll.DimX() / 2.;
//	float orthoHeight = boxAll.DimY() / 2.;
//	//===for left right
//	float orthoWidth2 = boxAll.DimY() / 2.;
//	float orthoHeight2 = boxAll.DimZ() / 2.;
//	//===for front back
//	float orthoWidth3 = boxAll.DimX() / 2.;
//	float orthoHeight3 = boxAll.DimZ() / 2.;
//
//	//groove size
//	float groovez = 200;
//	float groovex = 350;
//	float groovey = 222;
//
//	glContext->makeCurrent();
//
//
//	GLenum err = glewInit();
//	if (GLEW_OK != err)
//	{
//	}
//
//	Context ctx;
//	ctx.acquire();
//
//	/*const GLsizeiptr psize = GLsizeiptr(GLsizei(mesh.vn) * pstride);
//	BufferHandle hPositionBuffer = createBuffer(ctx, psize, pbase);
//
//	const GLsizeiptr nsize = GLsizeiptr(GLsizei(mesh.vn) * nstride);
//	BufferHandle hNormalBuffer = createBuffer(ctx, nsize, nbase);
//
//	const GLsizeiptr isize = GLsizeiptr(mesh.fn * 3 * sizeof(GLuint));
//	BufferHandle hIndexBuffer = createBuffer(ctx, isize);
//	{
//	BoundIndexBufferHandle indexBuffer = ctx.bindIndexBuffer(hIndexBuffer);
//
//	const CMeshO::VertexType * vbase = &(mesh.vert[0]);
//	GLuint *  indices = (GLuint *)indexBuffer->map(GL_WRITE_ONLY);
//	for (size_t i = 0; i < mesh.face.size(); ++i)
//	{
//	const CMeshO::FaceType & f = mesh.face[i];
//	if (f.IsD()) continue;
//	for (int v = 0; v < 3; ++v)
//	{
//	*indices++ = GLuint(vcg::tri::Index(mesh, f.cV(v)));
//	}
//	}
//	indexBuffer->unmap();
//
//	ctx.unbindIndexBuffer();
//	}*/
//
//
//	const GLsizei print_dpi = DPI;
//	GLsizei maxx_width;
//	GLsizei maxx_height;
//	if (mdlist.size() == 0)
//	{
//		maxx_width = (int)(ceil)(boxAll.DimX() / DSP_inchmm * print_dpi);
//		//maxx_height = (int)(ceil)(maxx_width * boxAll.DimY() / boxAll.DimX());
//		maxx_height = (int)(ceil)(boxAll.DimY() / DSP_inchmm * print_dpi);
//		cv::Mat top(maxx_height, maxx_width, CV_8UC1, Scalar(0));
//		cv::Mat bottom(maxx_height, maxx_width, CV_8UC1, Scalar(255));
//		testSaveImage.push_back(top);
//		testSaveImage.push_back(bottom);
//		return true;
//	}
//
//	//***20150907
//	for (int i = 0; i < 6; i++)
//	{
//		if (!genbool[i])
//			continue;
//		GLsizei width;//
//		GLsizei height;// = (int)(width * boxAll.DimY() / boxAll.DimX());
//		switch (i)
//		{
//		case 0:
//		case 1:
//		{
//				  width = (int)(ceil)(boxAll.DimX() / DSP_inchmm * print_dpi);
//				  //height = (int)(ceil)(width * boxAll.DimY() / boxAll.DimX());
//				  height = (int)(ceil)(boxAll.DimY() / DSP_inchmm * print_dpi);
//		}break;
//		case 2:
//		case 3:
//		{
//				  width = (int)(ceil)(boxAll.DimY() / DSP_inchmm * print_dpi);
//				  height = (int)(ceil)(boxAll.DimZ() / DSP_inchmm * print_dpi);
//				  //height = (int)(ceil)(width * boxAll.DimZ() / boxAll.DimY());
//		}break;
//		case 4:
//		case 5:
//		{
//				  width = (int)(ceil)(boxAll.DimX() / DSP_inchmm * print_dpi);
//				  height = (int)(ceil)(boxAll.DimZ() / DSP_inchmm * print_dpi);
//				  //height = (int)(ceil)(width * boxAll.DimZ() / boxAll.DimX());
//		}break;
//
//
//		}
//
//
//		RenderbufferHandle hDepth = createRenderbuffer(ctx, GL_DEPTH24_STENCIL8, width, height);
//		RenderbufferHandle hDepth1 = createRenderbuffer(ctx, GL_DEPTH24_STENCIL8, width, height);
//		Texture2DHandle    hColor = createTexture2D(ctx, GL_RGB8, width, height, GL_RGB, GL_UNSIGNED_BYTE);
//		FramebufferHandle  hFramebuffer = createFramebufferWithDepthStencil(ctx, renderbufferTarget(hDepth), renderbufferTarget(hDepth), texture2DTarget(hColor));
//
//		//const vcg::Point3f lightDirectionVS = vcg::Point3f(0.0f, 0.0f, -1.0f).Normalize();
//
//		glEnable(GL_DEPTH_TEST);
//
//		glViewport(0, 0, width, height);//***window的長寬				  			
//		ctx.bindReadDrawFramebuffer(hFramebuffer);
//		GLW_CHECK_GL_READ_DRAW_FRAMEBUFFER_STATUS;
//#if 1
//
//
//		glContext->makeCurrent();
//
//		//===========================================================================================
//		//time.restart();
//		int countj = 0;
//
//
//		switch (i)
//		{
//		case 0:
//		{
//				  glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
//				  glOrtho(-orthoWidth, orthoWidth, -orthoHeight, orthoHeight, 0, groovez);
//				  glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
//				  vcg::Point3f x = boxAll.Center();
//				  gluLookAt(boxAll.Center().X(), boxAll.Center().Y(), groovez / 2., boxAll.Center().X(), boxAll.Center().Y(), -DSP_Look_Distination, 0, 1, 0);
//		}
//			break;
//		case 1:
//		{
//				  glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
//				  glOrtho(-orthoWidth, orthoWidth, -orthoHeight, orthoHeight, 0, groovez);
//				  glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
//				  vcg::Point3f x = boxAll.Center();
//				  gluLookAt(boxAll.Center().X(), boxAll.Center().Y(), -groovez / 2., boxAll.Center().X(), boxAll.Center().Y(), DSP_Look_Distination, 0, 1, 0);
//		}
//			break;
//		case 2:
//		{
//				  glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
//				  glOrtho(-orthoWidth2, orthoWidth2, -orthoHeight2, orthoHeight2, 0, groovex);
//				  glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
//				  vcg::Point3f x = boxAll.Center();
//				  gluLookAt(groovex / 2., boxAll.Center().Y(), boxAll.Center().Z(), -DSP_Look_Distination, boxAll.Center().Y(), boxAll.Center().Z(), 0, 0, 1);
//
//		}
//			break;
//		case 3:
//		{
//				  glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
//				  glOrtho(-orthoWidth2, orthoWidth2, -orthoHeight2, orthoHeight2, 0, groovex);
//				  glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
//				  vcg::Point3f x = boxAll.Center();
//				  gluLookAt(-groovex / 2., boxAll.Center().Y(), boxAll.Center().Z(), DSP_Look_Distination, boxAll.Center().Y(), boxAll.Center().Z(), 0, 0, 1);
//
//		}
//			break;
//		case 4:
//		{
//				  glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
//				  glOrtho(-orthoWidth3, orthoWidth3, -orthoHeight3, orthoHeight3, 0, groovey);
//				  glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
//				  vcg::Point3f x = boxAll.Center();
//				  gluLookAt(boxAll.Center().X(), groovey / 2., boxAll.Center().Z(), boxAll.Center().X(), -DSP_Look_Distination, boxAll.Center().Z(), 0, 0, 1);
//
//
//		}
//			break;
//		case 5:
//		{
//
//				  glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
//				  glOrtho(-orthoWidth3, orthoWidth3, -orthoHeight3, orthoHeight3, 0, groovey);
//				  glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
//				  vcg::Point3f x = boxAll.Center();
//				  gluLookAt(boxAll.Center().X(), -groovey / 2., boxAll.Center().Z(), boxAll.Center().X(), DSP_Look_Distination, boxAll.Center().Z(), 0, 0, 1);
//
//		}
//			break;
//
//
//		}
//
//		//*********************************
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
//		glClearColor(1, 1, 1, 1);
//
//		//binderstrips(md, par.getFloat("useless_print"));//***uselessbar		
//		//************************************************//			
//		//****** Rendering the mesh's clip edge ****//
//		glEnable(GL_CULL_FACE);
//		glEnable(GL_DEPTH_TEST);
//		glCullFace(GL_BACK);
//		glPushAttrib(GL_ALL_ATTRIB_BITS);
//		glEnable(GL_LIGHTING);
//		glPushMatrix();
//
//		foreach(MeshModel *floorMesh, mdlist)
//		{
//			floorMesh->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
//		}
//
//		glPopMatrix();
//		glPopAttrib();
//
//
//		glMatrixMode(GL_MODELVIEW);
//		glPopMatrix();
//		glMatrixMode(GL_PROJECTION);
//		glPopMatrix();
//		//********************************************************************************************//
//
//		cv::Mat captemp(height, width, CV_8UC1, Scalar(255));
//		glPixelStorei(GL_PACK_ALIGNMENT, 1);
//		//set length of one complete row in destination data (doesn't need to equal img.cols)
//		glPixelStorei(GL_PACK_ROW_LENGTH, captemp.step / captemp.elemSize());
//		glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, captemp.data);//***20150724		
//
//		cv::flip(captemp, captemp, 0);
//		cv::Mat bigImage, captemp2;//for erode 
//		int plus_pixel = 2;
//		int plus_hPixel = plus_pixel / 2;
//		int plus_h2Pixel = plus_pixel / 2;
//		int bigImageRow, bigImageCol;
//		if (captemp.rows + plus_pixel >= maxx_height || captemp.cols + plus_pixel > maxx_height)
//		{
//			plus_pixel = 0;
//			plus_hPixel = 0;
//
//		}
//
//		if (copyToBig)
//			bigImage = *new Mat(captemp.rows + plus_pixel, captemp.cols + plus_pixel, captemp.type(), Scalar(255));
//		else
//			bigImage = captemp;
//		if (i % 2 == 1)
//		{
//			cv::flip(captemp, captemp, 1);
//			if (copyToBig)
//				captemp.copyTo(bigImage.rowRange(plus_hPixel, plus_hPixel + captemp.rows).colRange(plus_hPixel, plus_hPixel + captemp.cols));
//			if (sscale)
//			{
//				captemp2 = SKT::erodeImage(&bigImage, plus_h2Pixel);
//				for (int c = 0; c < bigImage.cols; c++)
//				{
//					for (int r = 0; r < bigImage.rows; r++)
//					{
//						if (captemp2.at<uchar>(r, c) != 255 && bigImage.at<uchar>(r, c) == 255)
//							bigImage.at<uchar>(r, c) = captemp2.at<uchar>(r, c);
//					}
//				}
//			}
//			else captemp2 = bigImage;
//
//
//		}
//		if (i % 2 == 0)
//		{
//			/*for (int c = 0; c < captemp.cols; c++)
//			{
//			for (int r = 0; r < captemp.rows; r++)
//			{
//			captemp.at<uchar>(r, c) = 255 - captemp.at<uchar>(r, c);
//			}
//			}*/
//			captemp = 255 - captemp;
//			if (copyToBig)
//			{
//				bigImage.setTo(0);
//				captemp.copyTo(bigImage.rowRange(plus_hPixel, plus_hPixel + captemp.rows).colRange(plus_hPixel, plus_hPixel + captemp.cols));
//			}
//			if (sscale)
//			{
//				captemp2 = SKT::dilateImage(&bigImage, plus_h2Pixel);
//				for (int c = 0; c < bigImage.cols; c++)
//				{
//					for (int r = 0; r < bigImage.rows; r++)
//					{
//						if (captemp2.at<uchar>(r, c) != 0 && bigImage.at<uchar>(r, c) == 0)
//							bigImage.at<uchar>(r, c) = captemp2.at<uchar>(r, c);
//					}
//				}
//			}
//			else captemp2 = bigImage;
//		}
//
//		testSaveImage.push_back(bigImage);
//		if (false){
//			QImage testttt = cvMatToQImage(captemp2);
//			QString capimagest = "captemp_image";
//			capimagest.append(QString("_%1_%2.png").arg(i, 4, 10, QChar('0')).arg(countj, 4, 10, QChar('0')));
//			testttt.save(dir.absolutePath() + "/" + capimagest, "png");
//			countj++;
//		}
//
//		//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T(""));
//
//		glMatrixMode(GL_MODELVIEW);
//		glPopMatrix();
//		glMatrixMode(GL_PROJECTION);
//		glPopMatrix();
//
//
//	}
//	//std::multimap<int, int> * tempIntersectMeshesID = md.getIntersectMeshesID();								  
//	//md.setIntersectMeshesID2(SKT::checkImageOverlap(savefloorMeshImage));
//
//#endif	
//
//	ctx.unbindReadDrawFramebuffer();
//	ctx.release();
//	glPopAttrib();
//	glContext->doneCurrent();
//
//	qDebug() << " done _applyClick ";
//	return true;
//}

//bool GenSliceJob::testDepthBuffer(QList<MeshModel *> mdlist, std::vector<cv::Mat> &testSaveImage, Box3m boxAll, QVector<bool> &genbool, bool sscale, bool copyToBig, int DPI, int pixelRatio)
//{
//	QString blk_img_file_name = "black_outline";
//	QString temppath = PicaApplication::getRoamingDir();
//	QDir dir(temppath);//backup
//	if (dir.exists("xyzimage"))
//	{
//		dir.setPath(temppath + "/xyzimage");
//		dir.removeRecursively();
//		dir.setPath(temppath);
//		dir.mkpath("xyzimage");
//	}
//	else
//	{
//		dir.setPath(temppath);
//		dir.mkpath("xyzimage");
//	}
//
//	dir.setPath(temppath + "/xyzimage");
//
//	//===for top down
//	float orthoWidth = boxAll.DimX() / 2.;
//	float orthoHeight = boxAll.DimY() / 2.;
//	//===for left right
//	float orthoWidth2 = boxAll.DimY() / 2.;
//	float orthoHeight2 = boxAll.DimZ() / 2.;
//	//===for front back
//	float orthoWidth3 = boxAll.DimX() / 2.;
//	float orthoHeight3 = boxAll.DimZ() / 2.;
//
//	//groove size
//	float groovez = DSP_grooveZ;
//	float groovex = DSP_grooveBigX;
//	float groovey = DSP_grooveY;
//
//	glContext->makeCurrent();
//
//	/*foreach(MeshModel *tmm, mdlist)
//	{
//	if (tmm->getMeshSort() == MeshModel::meshsort::print_item)
//	{
//	if ((tmm->glw.curr_hints & GLW::Hint::HNUseVBO) != 0)
//	{
//	for (int i = 0; i < 4; ++i)
//	if (glIsBuffer(GLuint(tmm->glw.array_buffers[i])))
//	{
//	glDeleteBuffers(1, (GLuint *)(tmm->glw.array_buffers + i));
//	tmm->glw.array_buffers[i] = 0;
//	}
//	tmm->glw.initVBOUpdate(GLW::DMSmooth, tmm->rmm.colorMode, tmm->rmm.textureMode);
//	}
//	}
//	}*/
//
//
//
//	GLenum err = glewInit();
//	if (GLEW_OK != err)
//	{
//	}
//
//	Context ctx;
//	ctx.acquire();
//
//	/*const GLsizeiptr psize = GLsizeiptr(GLsizei(mesh.vn) * pstride);
//	BufferHandle hPositionBuffer = createBuffer(ctx, psize, pbase);
//
//	const GLsizeiptr nsize = GLsizeiptr(GLsizei(mesh.vn) * nstride);
//	BufferHandle hNormalBuffer = createBuffer(ctx, nsize, nbase);
//
//	const GLsizeiptr isize = GLsizeiptr(mesh.fn * 3 * sizeof(GLuint));
//	BufferHandle hIndexBuffer = createBuffer(ctx, isize);
//	{
//	BoundIndexBufferHandle indexBuffer = ctx.bindIndexBuffer(hIndexBuffer);
//
//	const CMeshO::VertexType * vbase = &(mesh.vert[0]);
//	GLuint *  indices = (GLuint *)indexBuffer->map(GL_WRITE_ONLY);
//	for (size_t i = 0; i < mesh.face.size(); ++i)
//	{
//	const CMeshO::FaceType & f = mesh.face[i];
//	if (f.IsD()) continue;
//	for (int v = 0; v < 3; ++v)
//	{
//	*indices++ = GLuint(vcg::tri::Index(mesh, f.cV(v)));
//	}
//	}
//	indexBuffer->unmap();
//
//	ctx.unbindIndexBuffer();
//	}*/
//
//
//	const GLsizei print_dpi = DPI;
//	GLsizei maxx_width;
//	GLsizei maxx_height;
//	if (mdlist.size() == 0)
//	{
//		maxx_width = (int)(boxAll.DimX())*pixelRatio;
//		//maxx_height = (int)(ceil)(maxx_width * boxAll.DimY() / boxAll.DimX());
//		maxx_height = (int)(boxAll.DimY())*pixelRatio;
//		cv::Mat top(maxx_height, maxx_width, CV_8UC1, Scalar(0));
//		cv::Mat bottom(maxx_height, maxx_width, CV_8UC1, Scalar(255));
//		testSaveImage.push_back(top);
//		testSaveImage.push_back(bottom);
//		return true;
//	}
//
//	//***20150907
//	for (int i = 0; i < 6; i++)
//	{
//		if (!genbool[i])
//			continue;
//		GLsizei width;//
//		GLsizei height;// = (int)(width * boxAll.DimY() / boxAll.DimX());
//		switch (i)
//		{
//		case 0:
//		case 1:
//		{
//				  width = (int)(boxAll.DimX())*pixelRatio;
//				  //height = (int)(ceil)(width * boxAll.DimY() / boxAll.DimX());
//				  height = (int)(boxAll.DimY())*pixelRatio;
//		}break;
//		case 2:
//		case 3:
//		{
//				  width = (int)(boxAll.DimY())*pixelRatio;
//				  height = (int)(boxAll.DimZ())*pixelRatio;
//				  //height = (int)(ceil)(width * boxAll.DimZ() / boxAll.DimY());
//		}break;
//		case 4:
//		case 5:
//		{
//				  width = (int)(boxAll.DimX())*pixelRatio;
//				  height = (int)(boxAll.DimZ())*pixelRatio;
//				  //height = (int)(ceil)(width * boxAll.DimZ() / boxAll.DimX());
//		}break;
//
//
//		}
//
//
//		RenderbufferHandle hDepth = createRenderbuffer(ctx, GL_DEPTH24_STENCIL8, width, height);
//		RenderbufferHandle hDepth1 = createRenderbuffer(ctx, GL_DEPTH24_STENCIL8, width, height);
//		Texture2DHandle    hColor = createTexture2D(ctx, GL_RGB8, width, height, GL_RGB, GL_UNSIGNED_BYTE);
//		FramebufferHandle  hFramebuffer = createFramebufferWithDepthStencil(ctx, renderbufferTarget(hDepth), renderbufferTarget(hDepth), texture2DTarget(hColor));
//
//		//const vcg::Point3f lightDirectionVS = vcg::Point3f(0.0f, 0.0f, -1.0f).Normalize();
//
//		glEnable(GL_DEPTH_TEST);
//
//		glViewport(0, 0, width, height);//***window的長寬				  			
//		ctx.bindReadDrawFramebuffer(hFramebuffer);
//		GLW_CHECK_GL_READ_DRAW_FRAMEBUFFER_STATUS;
//#if 1
//
//
//		//glContext->makeCurrent();
//
//		//===========================================================================================
//		//time.restart();
//		int countj = 0;
//
//
//		switch (i)
//		{
//		case 0:
//		{
//				  glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
//				  glOrtho(-orthoWidth, orthoWidth, -orthoHeight, orthoHeight, 0, groovez);
//				  glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
//				  vcg::Point3f x = boxAll.Center();
//				  gluLookAt(boxAll.Center().X(), boxAll.Center().Y(), groovez / 2., boxAll.Center().X(), boxAll.Center().Y(), -DSP_Look_Distination, 0, 1, 0);
//		}
//			break;
//		case 1:
//		{
//				  glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
//				  glOrtho(-orthoWidth, orthoWidth, -orthoHeight, orthoHeight, 0, groovez);
//				  glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
//				  vcg::Point3f x = boxAll.Center();
//				  gluLookAt(boxAll.Center().X(), boxAll.Center().Y(), -groovez / 2., boxAll.Center().X(), boxAll.Center().Y(), DSP_Look_Distination, 0, 1, 0);
//		}
//			break;
//		case 2:
//		{
//				  glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
//				  glOrtho(-orthoWidth2, orthoWidth2, -orthoHeight2, orthoHeight2, 0, groovex);
//				  glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
//				  vcg::Point3f x = boxAll.Center();
//				  gluLookAt(groovex / 2., boxAll.Center().Y(), boxAll.Center().Z(), -DSP_Look_Distination, boxAll.Center().Y(), boxAll.Center().Z(), 0, 0, 1);
//
//		}
//			break;
//		case 3:
//		{
//				  glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
//				  glOrtho(-orthoWidth2, orthoWidth2, -orthoHeight2, orthoHeight2, 0, groovex);
//				  glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
//				  vcg::Point3f x = boxAll.Center();
//				  gluLookAt(-groovex / 2., boxAll.Center().Y(), boxAll.Center().Z(), DSP_Look_Distination, boxAll.Center().Y(), boxAll.Center().Z(), 0, 0, 1);
//
//		}
//			break;
//		case 4:
//		{
//				  glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
//				  glOrtho(-orthoWidth3, orthoWidth3, -orthoHeight3, orthoHeight3, 0, groovey);
//				  glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
//				  vcg::Point3f x = boxAll.Center();
//				  gluLookAt(boxAll.Center().X(), groovey / 2., boxAll.Center().Z(), boxAll.Center().X(), -DSP_Look_Distination, boxAll.Center().Z(), 0, 0, 1);
//
//
//		}
//			break;
//		case 5:
//		{
//
//				  glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
//				  glOrtho(-orthoWidth3, orthoWidth3, -orthoHeight3, orthoHeight3, 0, groovey);
//				  glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
//				  vcg::Point3f x = boxAll.Center();
//				  gluLookAt(boxAll.Center().X(), -groovey / 2., boxAll.Center().Z(), boxAll.Center().X(), DSP_Look_Distination, boxAll.Center().Z(), 0, 0, 1);
//
//		}
//			break;
//
//
//		}
//
//		//*********************************
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
//		glClearColor(1, 1, 1, 1);
//
//		//binderstrips(md, par.getFloat("useless_print"));//***uselessbar		
//		//************************************************//			
//		//****** Rendering the mesh's clip edge ****//
//		glEnable(GL_CULL_FACE);
//		glEnable(GL_DEPTH_TEST);
//		glCullFace(GL_BACK);
//		glPushAttrib(GL_ALL_ATTRIB_BITS);
//		glEnable(GL_LIGHTING);
//		glPushMatrix();
//
//		foreach(MeshModel *floorMesh, mdlist)
//		{
//			floorMesh->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
//		}
//
//		glPopMatrix();
//		glPopAttrib();
//
//
//		glMatrixMode(GL_MODELVIEW);
//		glPopMatrix();
//		glMatrixMode(GL_PROJECTION);
//		glPopMatrix();
//		//********************************************************************************************//
//
//		cv::Mat captemp(height, width, CV_8UC1, Scalar(255));
//		glPixelStorei(GL_PACK_ALIGNMENT, 1);
//		//set length of one complete row in destination data (doesn't need to equal img.cols)
//		glPixelStorei(GL_PACK_ROW_LENGTH, captemp.step / captemp.elemSize());
//		glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, captemp.data);//***20150724		
//
//		//cv::Mat captemp(height, width, CV_32FC1, Scalar(1));		
//		//glPixelStorei(GL_PACK_ALIGNMENT, 1);
//		////set length of one complete row in destination data (doesn't need to equal img.cols)
//		//glPixelStorei(GL_PACK_ROW_LENGTH, captemp.step / captemp.elemSize());
//		//glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, captemp.data);//***20150724		
//
//		//for (int c = 0; c < captemp.cols ; c++)
//		//{
//		//	
//		//	for (int r = 0; r < captemp.rows ; r++)
//		//	{
//		//		if (captemp.at<float>(r, c)!=1)
//		//			qDebug() << "captemp_" <<r<<"_"<<c<<"_"<< captemp.at<float>(r, c);
//		//		//float test = captemp.at<float>(r, c);
//		//	}
//		//}
//
//
//		cv::flip(captemp, captemp, 0);
//		cv::Mat bigImage, captemp2;//for erode 
//		int plus_pixel = 2;
//		int plus_hPixel = plus_pixel / 2;
//		int plus_h2Pixel = plus_pixel / 2;
//		int bigImageRow, bigImageCol;
//		if (captemp.rows + plus_pixel >= maxx_height || captemp.cols + plus_pixel > maxx_height)
//		{
//			plus_pixel = 0;
//			plus_hPixel = 0;
//
//		}
//
//		if (copyToBig)
//			bigImage = *new Mat(captemp.rows + plus_pixel, captemp.cols + plus_pixel, captemp.type(), Scalar(255));
//		else
//			bigImage = captemp;
//		if (i % 2 == 1)
//		{
//			cv::flip(captemp, captemp, 1);
//			if (copyToBig)
//				captemp.copyTo(bigImage.rowRange(plus_hPixel, plus_hPixel + captemp.rows).colRange(plus_hPixel, plus_hPixel + captemp.cols));
//			if (sscale)
//			{
//				captemp2 = SKT::erodeImage(&bigImage, plus_h2Pixel);
//				for (int c = 0; c < bigImage.cols; c++)
//				{
//					for (int r = 0; r < bigImage.rows; r++)
//					{
//						if (captemp2.at<uchar>(r, c) != 255 && bigImage.at<uchar>(r, c) == 255)
//							bigImage.at<uchar>(r, c) = captemp2.at<uchar>(r, c);
//					}
//				}
//			}
//			else captemp2 = bigImage;
//
//
//		}
//		if (i % 2 == 0)
//		{
//			/*for (int c = 0; c < captemp.cols; c++)
//			{
//			for (int r = 0; r < captemp.rows; r++)
//			{
//			captemp.at<uchar>(r, c) = 255 - captemp.at<uchar>(r, c);
//			}
//			}*/
//			captemp = 255 - captemp;
//			if (copyToBig)
//			{
//				bigImage.setTo(0);
//				captemp.copyTo(bigImage.rowRange(plus_hPixel, plus_hPixel + captemp.rows).colRange(plus_hPixel, plus_hPixel + captemp.cols));
//			}
//			if (sscale)
//			{
//				captemp2 = SKT::dilateImage(&bigImage, plus_h2Pixel);
//				for (int c = 0; c < bigImage.cols; c++)
//				{
//					for (int r = 0; r < bigImage.rows; r++)
//					{
//						if (captemp2.at<uchar>(r, c) != 0 && bigImage.at<uchar>(r, c) == 0)
//							bigImage.at<uchar>(r, c) = captemp2.at<uchar>(r, c);
//					}
//				}
//			}
//			else captemp2 = bigImage;
//		}
//
//		testSaveImage.push_back(bigImage);
//		if (true){
//			QImage testttt = cvMatToQImage(captemp2);
//			QString capimagest = "captemp_image";
//			capimagest.append(QString("_%1_%2.png").arg(i, 4, 10, QChar('0')).arg(countj, 4, 10, QChar('0')));
//			testttt.save(dir.absolutePath() + "/" + capimagest, "png");
//			countj++;
//		}
//
//		//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T(""));
//
//		glMatrixMode(GL_MODELVIEW);
//		glPopMatrix();
//		glMatrixMode(GL_PROJECTION);
//		glPopMatrix();
//
//
//	}
//	//std::multimap<int, int> * tempIntersectMeshesID = md.getIntersectMeshesID();								  
//	//md.setIntersectMeshesID2(SKT::checkImageOverlap(savefloorMeshImage));
//
//#endif	
//
//	ctx.unbindReadDrawFramebuffer();
//	ctx.release();
//	glPopAttrib();
//	glContext->doneCurrent();
//
//	qDebug() << " done _applyClick ";
//	return true;
//}

bool GenSliceJob::testDepthBuffer(QList<MeshModel *> mdlist, std::vector<cv::Mat> &testSaveImage, Box3m boxAll, QVector<bool> &genbool, Box3m grooveAddOffset, bool sscale, bool copyToBig, int DPI, float pixelRatio)
{
	QString blk_img_file_name = "black_outline";
	QString temppath = PicaApplication::getRoamingDir();
	QDir dir(temppath);//backup
	if (dir.exists("xyzimage"))
	{
		dir.setPath(temppath + "/xyzimage");
		dir.removeRecursively();
		dir.setPath(temppath);
		dir.mkpath("xyzimage");
	}
	else
	{
		dir.setPath(temppath);
		dir.mkpath("xyzimage");
	}

	dir.setPath(temppath + "/xyzimage");

	//===for top down
	float orthoWidth = boxAll.DimX() / 2.;
	float orthoHeight = boxAll.DimY() / 2.;
	//===for left right
	float orthoWidth2 = boxAll.DimY() / 2.;
	float orthoHeight2 = boxAll.DimZ() / 2.;
	//===for front back
	float orthoWidth3 = boxAll.DimX() / 2.;
	float orthoHeight3 = boxAll.DimZ() / 2.;

	//groove size
	float groovez = grooveAddOffset.DimZ();// DSP_grooveZ;
	float groovex = grooveAddOffset.DimX();// DSP_grooveBigX;
	float groovey = grooveAddOffset.DimY();// DSP_grooveY;

	glContext->makeCurrent();

	/*foreach(MeshModel *tmm, mdlist)
	{
	if (tmm->getMeshSort() == MeshModel::meshsort::print_item)
	{
	if ((tmm->glw.curr_hints & GLW::Hint::HNUseVBO) != 0)
	{
	for (int i = 0; i < 4; ++i)
	if (glIsBuffer(GLuint(tmm->glw.array_buffers[i])))
	{
	glDeleteBuffers(1, (GLuint *)(tmm->glw.array_buffers + i));
	tmm->glw.array_buffers[i] = 0;
	}
	tmm->glw.initVBOUpdate(GLW::DMSmooth, tmm->rmm.colorMode, tmm->rmm.textureMode);
	}
	}
	}*/



	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
	}

	Context ctx;
	ctx.acquire();

	/*const GLsizeiptr psize = GLsizeiptr(GLsizei(mesh.vn) * pstride);
	BufferHandle hPositionBuffer = createBuffer(ctx, psize, pbase);

	const GLsizeiptr nsize = GLsizeiptr(GLsizei(mesh.vn) * nstride);
	BufferHandle hNormalBuffer = createBuffer(ctx, nsize, nbase);

	const GLsizeiptr isize = GLsizeiptr(mesh.fn * 3 * sizeof(GLuint));
	BufferHandle hIndexBuffer = createBuffer(ctx, isize);
	{
	BoundIndexBufferHandle indexBuffer = ctx.bindIndexBuffer(hIndexBuffer);

	const CMeshO::VertexType * vbase = &(mesh.vert[0]);
	GLuint *  indices = (GLuint *)indexBuffer->map(GL_WRITE_ONLY);
	for (size_t i = 0; i < mesh.face.size(); ++i)
	{
	const CMeshO::FaceType & f = mesh.face[i];
	if (f.IsD()) continue;
	for (int v = 0; v < 3; ++v)
	{
	*indices++ = GLuint(vcg::tri::Index(mesh, f.cV(v)));
	}
	}
	indexBuffer->unmap();

	ctx.unbindIndexBuffer();
	}*/


	const GLsizei print_dpi = DPI;
	GLsizei maxx_width = 0;
	GLsizei maxx_height = 0;
	if (mdlist.size() == 0)
	{
		maxx_width = (int)(boxAll.DimX())*pixelRatio;
		//maxx_height = (int)(ceil)(maxx_width * boxAll.DimY() / boxAll.DimX());
		maxx_height = (int)(boxAll.DimY())*pixelRatio;
		cv::Mat top(maxx_height, maxx_width, CV_8UC1, Scalar(0));
		cv::Mat bottom(maxx_height, maxx_width, CV_8UC1, Scalar(255));
		testSaveImage.push_back(top);
		testSaveImage.push_back(bottom);
		return true;
	}

	//***20150907
	for (int i = 0; i < 6; i++)
	{
		if (!genbool[i])
			continue;
		GLsizei width;//
		GLsizei height;// = (int)(width * boxAll.DimY() / boxAll.DimX());
		switch (i)
		{
		case 0:
		case 1:
		{
			width = (int)(boxAll.DimX())*pixelRatio;
			//height = (int)(ceil)(width * boxAll.DimY() / boxAll.DimX());
			height = (int)(boxAll.DimY())*pixelRatio;
		}break;
		case 2:
		case 3:
		{
			width = (int)(boxAll.DimY())*pixelRatio;
			height = (int)(boxAll.DimZ())*pixelRatio;
			//height = (int)(ceil)(width * boxAll.DimZ() / boxAll.DimY());
		}break;
		case 4:
		case 5:
		{
			width = (int)(boxAll.DimX())*pixelRatio;
			height = (int)(boxAll.DimZ())*pixelRatio;
			//height = (int)(ceil)(width * boxAll.DimZ() / boxAll.DimX());
		}break;


		}


		RenderbufferHandle hDepth = createRenderbuffer(ctx, GL_DEPTH24_STENCIL8, width, height);
		RenderbufferHandle hDepth1 = createRenderbuffer(ctx, GL_DEPTH24_STENCIL8, width, height);
		Texture2DHandle    hColor = createTexture2D(ctx, GL_RGB8, width, height, GL_RGB, GL_UNSIGNED_BYTE);
		FramebufferHandle  hFramebuffer = createFramebufferWithDepthStencil(ctx, renderbufferTarget(hDepth), renderbufferTarget(hDepth), texture2DTarget(hColor));

		//const vcg::Point3f lightDirectionVS = vcg::Point3f(0.0f, 0.0f, -1.0f).Normalize();

		glEnable(GL_DEPTH_TEST);

		glViewport(0, 0, width, height);//***window的長寬				  			
		ctx.bindReadDrawFramebuffer(hFramebuffer);
		GLW_CHECK_GL_READ_DRAW_FRAMEBUFFER_STATUS;
#if 1


		//glContext->makeCurrent();

		//===========================================================================================
		//time.restart();
		int countj = 0;


		switch (i)
		{
		case 0:
		{
			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-orthoWidth, orthoWidth, -orthoHeight, orthoHeight, 0, groovez);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
			vcg::Point3f x = boxAll.Center();
			gluLookAt(boxAll.Center().X(), boxAll.Center().Y(), groovez / 2., boxAll.Center().X(), boxAll.Center().Y(), -DSP_Look_Distination, 0, 1, 0);
		}
		break;
		case 1:
		{
			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-orthoWidth, orthoWidth, -orthoHeight, orthoHeight, 0, groovez);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
			vcg::Point3f x = boxAll.Center();
			gluLookAt(boxAll.Center().X(), boxAll.Center().Y(), -groovez / 2., boxAll.Center().X(), boxAll.Center().Y(), DSP_Look_Distination, 0, 1, 0);
		}
		break;
		case 2:
		{
			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-orthoWidth2, orthoWidth2, -orthoHeight2, orthoHeight2, 0, groovex);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
			vcg::Point3f x = boxAll.Center();
			gluLookAt(groovex / 2., boxAll.Center().Y(), boxAll.Center().Z(), -DSP_Look_Distination, boxAll.Center().Y(), boxAll.Center().Z(), 0, 0, 1);

		}
		break;
		case 3:
		{
			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-orthoWidth2, orthoWidth2, -orthoHeight2, orthoHeight2, 0, groovex);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
			vcg::Point3f x = boxAll.Center();
			gluLookAt(-groovex / 2., boxAll.Center().Y(), boxAll.Center().Z(), DSP_Look_Distination, boxAll.Center().Y(), boxAll.Center().Z(), 0, 0, 1);

		}
		break;
		case 4:
		{
			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-orthoWidth3, orthoWidth3, -orthoHeight3, orthoHeight3, 0, groovey);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
			vcg::Point3f x = boxAll.Center();
			gluLookAt(boxAll.Center().X(), groovey / 2., boxAll.Center().Z(), boxAll.Center().X(), -DSP_Look_Distination, boxAll.Center().Z(), 0, 0, 1);


		}
		break;
		case 5:
		{

			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-orthoWidth3, orthoWidth3, -orthoHeight3, orthoHeight3, 0, groovey);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
			vcg::Point3f x = boxAll.Center();
			gluLookAt(boxAll.Center().X(), -groovey / 2., boxAll.Center().Z(), boxAll.Center().X(), DSP_Look_Distination, boxAll.Center().Z(), 0, 0, 1);

		}
		break;


		}

		//*********************************
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearColor(1, 1, 1, 1);

		//binderstrips(md, par.getFloat("useless_print"));//***uselessbar		
		//************************************************//			
		//****** Rendering the mesh's clip edge ****//
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glCullFace(GL_BACK);
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glEnable(GL_LIGHTING);
		glPushMatrix();

		foreach(MeshModel *floorMesh, mdlist)
		{
			floorMesh->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
		}

		glPopMatrix();
		glPopAttrib();


		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		//********************************************************************************************//

		cv::Mat captemp(height, width, CV_8UC1, Scalar(255));
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		//set length of one complete row in destination data (doesn't need to equal img.cols)
		glPixelStorei(GL_PACK_ROW_LENGTH, captemp.step / captemp.elemSize());
		glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, captemp.data);//***20150724		

		//cv::Mat captemp(height, width, CV_32FC1, Scalar(1));		
		//glPixelStorei(GL_PACK_ALIGNMENT, 1);
		////set length of one complete row in destination data (doesn't need to equal img.cols)
		//glPixelStorei(GL_PACK_ROW_LENGTH, captemp.step / captemp.elemSize());
		//glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, captemp.data);//***20150724		

		//for (int c = 0; c < captemp.cols ; c++)
		//{
		//	
		//	for (int r = 0; r < captemp.rows ; r++)
		//	{
		//		if (captemp.at<float>(r, c)!=1)
		//			qDebug() << "captemp_" <<r<<"_"<<c<<"_"<< captemp.at<float>(r, c);
		//		//float test = captemp.at<float>(r, c);
		//	}
		//}


		cv::flip(captemp, captemp, 0);
		cv::Mat bigImage, captemp2;//for erode 
		int plus_pixel = 2;
		int plus_hPixel = plus_pixel / 2;
		int plus_h2Pixel = plus_pixel / 2;
		int bigImageRow, bigImageCol;
		if (captemp.rows + plus_pixel >= maxx_height || captemp.cols + plus_pixel > maxx_height)
		{
			plus_pixel = 0;
			plus_hPixel = 0;

		}

		if (copyToBig)
			bigImage = *new Mat(captemp.rows + plus_pixel, captemp.cols + plus_pixel, captemp.type(), Scalar(255));
		else
			bigImage = captemp;
		if (i % 2 == 1)
		{
			cv::flip(captemp, captemp, 1);
			if (copyToBig)
				captemp.copyTo(bigImage.rowRange(plus_hPixel, plus_hPixel + captemp.rows).colRange(plus_hPixel, plus_hPixel + captemp.cols));
			if (sscale)
			{
				captemp2 = SKT::erodeImage(&bigImage, plus_h2Pixel);
				for (int c = 0; c < bigImage.cols; c++)
				{
					for (int r = 0; r < bigImage.rows; r++)
					{
						if (captemp2.at<uchar>(r, c) != 255 && bigImage.at<uchar>(r, c) == 255)
							bigImage.at<uchar>(r, c) = captemp2.at<uchar>(r, c);
					}
				}
			}
			else captemp2 = bigImage;


		}
		if (i % 2 == 0)
		{
			/*for (int c = 0; c < captemp.cols; c++)
			{
			for (int r = 0; r < captemp.rows; r++)
			{
			captemp.at<uchar>(r, c) = 255 - captemp.at<uchar>(r, c);
			}
			}*/
			captemp = 255 - captemp;
			if (copyToBig)
			{
				bigImage.setTo(0);
				captemp.copyTo(bigImage.rowRange(plus_hPixel, plus_hPixel + captemp.rows).colRange(plus_hPixel, plus_hPixel + captemp.cols));
			}
			if (sscale)
			{
				captemp2 = SKT::dilateImage(&bigImage, plus_h2Pixel);
				for (int c = 0; c < bigImage.cols; c++)
				{
					for (int r = 0; r < bigImage.rows; r++)
					{
						if (captemp2.at<uchar>(r, c) != 0 && bigImage.at<uchar>(r, c) == 0)
							bigImage.at<uchar>(r, c) = captemp2.at<uchar>(r, c);
					}
				}
			}
			else captemp2 = bigImage;
		}

		testSaveImage.push_back(bigImage);
		if (false){
			QImage testttt = cvMatToQImage(captemp2);
			QString capimagest = "captemp_image_depth";
			capimagest.append(QString("_%1_%2.png").arg(i, 4, 10, QChar('0')).arg(countj, 4, 10, QChar('0')));
			testttt.save(dir.absolutePath() + "/" + capimagest, "png");
			countj++;
		}

		//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T(""));

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();


	}
	//std::multimap<int, int> * tempIntersectMeshesID = md.getIntersectMeshesID();								  
	//md.setIntersectMeshesID2(SKT::checkImageOverlap(savefloorMeshImage));

#endif	

	ctx.unbindReadDrawFramebuffer();
	ctx.release();
	glPopAttrib();
	glContext->doneCurrent();

	qDebug() << " done _applyClick ";
	return true;
}
bool GenSliceJob::testDepthBuffer_float(QList<MeshModel *> mdlist, std::vector<cv::Mat> &testSaveImage, Box3m boxAll, QVector<bool> &genbool, bool sscale, bool copyToBig, int DPI, int pixelRatio)
{
	QString blk_img_file_name = "black_outline";
	QString temppath = PicaApplication::getRoamingDir();
	QDir dir(temppath);//backup
	if (dir.exists("xyzimage"))
	{
		dir.setPath(temppath + "/xyzimage");
		dir.removeRecursively();
		dir.setPath(temppath);
		dir.mkpath("xyzimage");
	}
	else
	{
		dir.setPath(temppath);
		dir.mkpath("xyzimage");
	}

	dir.setPath(temppath + "/xyzimage");

	//===for top down
	float orthoWidth = boxAll.DimX() / 2.;
	float orthoHeight = boxAll.DimY() / 2.;
	//===for left right
	float orthoWidth2 = boxAll.DimY() / 2.;
	float orthoHeight2 = boxAll.DimZ() / 2.;
	//===for front back
	float orthoWidth3 = boxAll.DimX() / 2.;
	float orthoHeight3 = boxAll.DimZ() / 2.;

	//groove size
	float groovez = DSP_grooveZ;
	float groovex = DSP_grooveBigX;
	float groovey = DSP_grooveY;

	glContext->makeCurrent();

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
	}

	Context ctx;
	ctx.acquire();

	const GLsizei print_dpi = DPI;
	GLsizei maxx_width = 0;
	GLsizei maxx_height = 0;
	if (mdlist.size() == 0)
	{
		maxx_width = (int)(boxAll.DimX())*pixelRatio;
		//maxx_height = (int)(ceil)(maxx_width * boxAll.DimY() / boxAll.DimX());
		maxx_height = (int)(boxAll.DimY())*pixelRatio;
		cv::Mat top(maxx_height, maxx_width, CV_8UC1, Scalar(0));
		cv::Mat bottom(maxx_height, maxx_width, CV_8UC1, Scalar(255));
		testSaveImage.push_back(top);
		testSaveImage.push_back(bottom);
		return true;
	}

	//***20150907
	for (int i = 0; i < 6; i++)
	{
		if (!genbool[i])
			continue;
		GLsizei width;//
		GLsizei height;// = (int)(width * boxAll.DimY() / boxAll.DimX());
		switch (i)
		{
		case 0:
		case 1:
		{
			width = (int)(boxAll.DimX())*pixelRatio;
			//height = (int)(ceil)(width * boxAll.DimY() / boxAll.DimX());
			height = (int)(boxAll.DimY())*pixelRatio;
		}break;
		case 2:
		case 3:
		{
			width = (int)(boxAll.DimY())*pixelRatio;
			height = (int)(boxAll.DimZ())*pixelRatio;
			//height = (int)(ceil)(width * boxAll.DimZ() / boxAll.DimY());
		}break;
		case 4:
		case 5:
		{
			width = (int)(boxAll.DimX())*pixelRatio;
			height = (int)(boxAll.DimZ())*pixelRatio;
			//height = (int)(ceil)(width * boxAll.DimZ() / boxAll.DimX());
		}break;


		}


		RenderbufferHandle hDepth = createRenderbuffer(ctx, GL_DEPTH24_STENCIL8, width, height);
		RenderbufferHandle hDepth1 = createRenderbuffer(ctx, GL_DEPTH24_STENCIL8, width, height);
		Texture2DHandle    hColor = createTexture2D(ctx, GL_RGB8, width, height, GL_RGB, GL_UNSIGNED_BYTE);
		FramebufferHandle  hFramebuffer = createFramebufferWithDepthStencil(ctx, renderbufferTarget(hDepth), renderbufferTarget(hDepth), texture2DTarget(hColor));

		//const vcg::Point3f lightDirectionVS = vcg::Point3f(0.0f, 0.0f, -1.0f).Normalize();

		glEnable(GL_DEPTH_TEST);

		glViewport(0, 0, width, height);//***window的長寬				  			
		ctx.bindReadDrawFramebuffer(hFramebuffer);
		GLW_CHECK_GL_READ_DRAW_FRAMEBUFFER_STATUS;
#if 1


		//glContext->makeCurrent();

		//===========================================================================================
		//time.restart();
		int countj = 0;


		switch (i)
		{
		case 0:
		{
			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-orthoWidth, orthoWidth, -orthoHeight, orthoHeight, 0, groovez);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
			vcg::Point3f x = boxAll.Center();
			gluLookAt(boxAll.Center().X(), boxAll.Center().Y(), groovez / 2., boxAll.Center().X(), boxAll.Center().Y(), -DSP_Look_Distination, 0, 1, 0);
		}
		break;
		case 1:
		{
			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-orthoWidth, orthoWidth, -orthoHeight, orthoHeight, 0, groovez);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
			vcg::Point3f x = boxAll.Center();
			gluLookAt(boxAll.Center().X(), boxAll.Center().Y(), -groovez / 2., boxAll.Center().X(), boxAll.Center().Y(), DSP_Look_Distination, 0, 1, 0);
		}
		break;
		case 2:
		{
			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-orthoWidth2, orthoWidth2, -orthoHeight2, orthoHeight2, 0, groovex);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
			vcg::Point3f x = boxAll.Center();
			gluLookAt(groovex / 2., boxAll.Center().Y(), boxAll.Center().Z(), -DSP_Look_Distination, boxAll.Center().Y(), boxAll.Center().Z(), 0, 0, 1);

		}
		break;
		case 3:
		{
			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-orthoWidth2, orthoWidth2, -orthoHeight2, orthoHeight2, 0, groovex);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
			vcg::Point3f x = boxAll.Center();
			gluLookAt(-groovex / 2., boxAll.Center().Y(), boxAll.Center().Z(), DSP_Look_Distination, boxAll.Center().Y(), boxAll.Center().Z(), 0, 0, 1);

		}
		break;
		case 4:
		{
			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-orthoWidth3, orthoWidth3, -orthoHeight3, orthoHeight3, 0, groovey);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
			vcg::Point3f x = boxAll.Center();
			gluLookAt(boxAll.Center().X(), groovey / 2., boxAll.Center().Z(), boxAll.Center().X(), -DSP_Look_Distination, boxAll.Center().Z(), 0, 0, 1);


		}
		break;
		case 5:
		{

			glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
			glOrtho(-orthoWidth3, orthoWidth3, -orthoHeight3, orthoHeight3, 0, groovey);
			glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
			vcg::Point3f x = boxAll.Center();
			gluLookAt(boxAll.Center().X(), -groovey / 2., boxAll.Center().Z(), boxAll.Center().X(), DSP_Look_Distination, boxAll.Center().Z(), 0, 0, 1);

		}
		break;


		}

		//*********************************
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearColor(1, 1, 1, 1);

		//binderstrips(md, par.getFloat("useless_print"));//***uselessbar		
		//************************************************//			
		//****** Rendering the mesh's clip edge ****//
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glCullFace(GL_BACK);
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glEnable(GL_LIGHTING);
		glPushMatrix();

		foreach(MeshModel *floorMesh, mdlist)
		{
			floorMesh->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);
		}

		glPopMatrix();
		glPopAttrib();


		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		//********************************************************************************************//

		//cv::Mat captemp(height, width, CV_8UC1, Scalar(255));
		//glPixelStorei(GL_PACK_ALIGNMENT, 1);
		////set length of one complete row in destination data (doesn't need to equal img.cols)
		//glPixelStorei(GL_PACK_ROW_LENGTH, captemp.step / captemp.elemSize());
		//glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, captemp.data);//***20150724		

		cv::Mat captemp(height, width, CV_32FC1, Scalar(1));
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		//set length of one complete row in destination data (doesn't need to equal img.cols)
		glPixelStorei(GL_PACK_ROW_LENGTH, captemp.step / captemp.elemSize());
		glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, captemp.data);//***20150724		

		//for (int c = 0; c < captemp.cols ; c++)
		//{
		//	
		//	for (int r = 0; r < captemp.rows ; r++)
		//	{
		//		if (captemp.at<float>(r, c)!=1)
		//			qDebug() << "captemp_" <<r<<"_"<<c<<"_"<< captemp.at<float>(r, c);
		//		//float test = captemp.at<float>(r, c);
		//	}
		//}


		cv::flip(captemp, captemp, 0);
		cv::Mat bigImage, captemp2;//for erode 



		if (i % 2 == 1)
		{
			cv::flip(captemp, captemp, 1);
		}
		if (i % 2 == 0)
		{
			captemp = 1 - captemp;
		}

		testSaveImage.push_back(captemp);
		captemp.convertTo(captemp2, CV_8U, 255.0);

		/*for (int c = 0; c < captemp2.cols; c++)
		{

		for (int r = 0; r < captemp2.rows; r++)
		{
		if (captemp2.at<uchar>(r, c) != 255)
		qDebug() << "captemp2_" << r << "_" << c << "_" << captemp2.at<uchar>(r, c);

		}
		}*/

		if (false){
			QImage testttt = cvMatToQImage(captemp2);
			QString capimagest = "captemp_image";
			capimagest.append(QString("_%1_%2.png").arg(i, 4, 10, QChar('0')).arg(countj, 4, 10, QChar('0')));
			testttt.save(dir.absolutePath() + "/" + capimagest, "png");
			countj++;
		}

		//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T(""));

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();


	}
	//std::multimap<int, int> * tempIntersectMeshesID = md.getIntersectMeshesID();								  
	//md.setIntersectMeshesID2(SKT::checkImageOverlap(savefloorMeshImage));

#endif	

	ctx.unbindReadDrawFramebuffer();
	ctx.release();
	glPopAttrib();
	glContext->doneCurrent();

	qDebug() << " done _applyClick ";
	return true;
}

void GenSliceJob::fillsquare(void) {
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_LIGHTING);
	glColor3f(0, 0, 0);
	glPushMatrix();

	glNormal3f(0, 0, -1);
	glBegin(GL_QUADS);
	glVertex3f(-ollX, -ollY, equ[3]);
	glVertex3f(ollX, -ollY, equ[3]);
	glVertex3f(ollX, ollY, equ[3]);
	glVertex3f(-ollX, ollY, equ[3]);
	glEnd();


	//glRecti(-5, -5, 5, 5);

	glPopMatrix();
	glPopAttrib();
}
void GenSliceJob::combineImage(QImage &image_line, QImage& image_ortho)
{
	for (int i = 0; i < image_line.width(); i++)
		for (int j = 0; j < image_line.height(); j++)
		{
			QRgb value = image_line.pixel(i, j);
			QRgb value2 = image_ortho.pixel(i, j);
			if (qRed(value) != 255 || qGreen(value) != 255 || qBlue(value) != 255)
				image_ortho.setPixel(i, j, value);

		}
}
size_t GenSliceJob::faceconnect(MeshModel &m)
{
	// it also assumes that the FF adjacency is well computed.
	assert(HasFFAdjacency(m.cm));
	UpdateFlags<CMeshO>::FaceClearV(m.cm);

	std::deque<CMeshO::FacePointer> visitStack;
	size_t selCnt = 0;
	CMeshO::FaceIterator fi;
	fi = m.cm.face.begin();
	fi->SetS();
	for (fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi)
	{
		if (!(*fi).IsD() && (*fi).IsS() && !(*fi).IsV())
			visitStack.push_back(&*fi);
	}

	while (!visitStack.empty())
	{
		CMeshO::FacePointer fp = visitStack.front();
		visitStack.pop_front();
		assert(!fp->IsV());//if face is visited,break;
		fp->SetV();
		for (int i = 0; i < fp->VN(); ++i)
		{
			CMeshO::FacePointer ff = fp->FFp(i);
			if (!ff->IsS())
			{
				ff->SetS();
				++selCnt;
				visitStack.push_back(ff);
				assert(!ff->IsV());
			}
		}
	}

	return selCnt;

}
void GenSliceJob::genDrawList(MeshDocument &md)
{
	glContext->makeCurrent();
	glDeleteLists(drawList, 2);
	drawList = glGenLists(2);
	if (md.meshList.size() > 0 && drawList > 0)
	{

#pragma region listTest2
		glNewList(drawList, GL_COMPILE);
		foreach(int id, md.multiSelectID)
		{
			MeshModel *mdmm = md.getMesh(id);
			//****20151231********************************************************************

			if (mdmm->getMeshSort() == MeshModel::meshsort::print_item)
				if (mdmm->rmm.colorMode != GLW::CMNone || mdmm->rmm.textureMode != GLW::TMNone)//如果無色就不須做									   
				{
					mdmm->render(GLW::DMSmooth, mdmm->rmm.colorMode, mdmm->rmm.textureMode);
				}
		}

		glEndList();
#pragma endregion listTest2
		glNewList(drawList + 1, GL_COMPILE);
		foreach(int id, md.multiSelectID)
		{
			MeshModel *mp = md.getMesh(id);
			if (mp->getMeshSort() == MeshModel::meshsort::print_item)
				//mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
				mp->render(GLW::DMFlat, GLW::CMBlack, GLW::TMNone);
		}
		glEndList();

		/*glNewList(drawList + 2, GL_COMPILE);
		foreach(int id, md.multiSelectID)
		{
		MeshModel *mdmm = md.getMesh(id);
		if (mdmm->getMeshSort() == MeshModel::meshsort::print_item)
		{
		mdmm->render(GLW::DMFlat, GLW::CMBlack, GLW::TMNone);
		}
		}
		glEndList();*/

		drawLists.at(0) = drawList;
		drawLists.at(1) = drawList + 1;

	}

	glContext->doneCurrent();

}
bool GenSliceJob::testDepthRecursive_func(MeshModel &mmm, MeshDocument &md, float gap, bool x, vcg::Point3f movePosition)
{
	x = true;
	Matrix44m landToGround;
	landToGround.SetIdentity();
	vcg::Point3f pp(0, 0, 0);
	if (movePosition != pp)
	{
		landToGround.SetTranslate(movePosition);
		mmm.cm.Tr = landToGround * mmm.cm.Tr;
		tri::UpdatePosition<CMeshO>::Matrix(mmm.cm, landToGround, true);
		tri::UpdateBounding<CMeshO>::Box(mmm.cm);
	}
	landToGround.SetIdentity();
	vcg::Point3f translatePath;
	translatePath.SetZero();
	QVector<bool> genNum(6, true);

	foreach(MeshModel *bm, md.meshList)
	{
		QString myString = bm->fullName();
		const char* myChar = myString.toStdString().c_str();

		QList<MeshModel *> selectMesh;
		selectMesh.push_back(&mmm);
		QList<MeshModel *> compareMesh;

		Box3m mmmBox(mmm.cm.bbox);
		std::vector<cv::Mat> testSaveImage, testSaveImage2;
		bool twoMeshCollide = false;
		if (bm->getMeshSort() == MeshModel::meshsort::print_item && bm->id() != mmm.id() && mmm.cm.bbox.Collide(bm->cm.bbox))
		{
			compareMesh.push_back(bm);

			testDepthBuffer(selectMesh, testSaveImage, mmmBox, genNum, md.groove);
			testDepthBuffer(compareMesh, testSaveImage2, mmmBox, genNum, md.groove);
			twoMeshCollide = SKT::m1m2CollideFromSixDepthImage(testSaveImage, testSaveImage2);
		}
		if (twoMeshCollide)
		{
			int imageAxis = 2;
			int moveOffsetInt = SKT::moveMajorToM2Top(testSaveImage, testSaveImage2, imageAxis);
			moveOffsetInt -= 3;

			double moveOffset = SKT::pixelRatioToMm(moveOffsetInt, 1);
			vcg::Point3f Offset;
			if (imageAxis == int(genPF_X_Axis))
				Offset = vcg::Point3f(-moveOffset, 0, 0);
			else if (imageAxis == int(genPF_Y_Axis))
				Offset = vcg::Point3f(0, moveOffset, 0);
			else if (imageAxis == int(genPF_Z_Axis))
			{
				Offset = vcg::Point3f(0, 0, -moveOffset);
			}
			Matrix44m tranlate_to;
			tranlate_to.SetTranslate(Offset);

			SKT::mesh_translate(mmm.cm, Offset);
			x = testDepthRecursive_func(mmm, md, gap, x);
			break;
		}


		/*if (bm->getMeshSort() == MeshModel::meshsort::print_item)
		if (bm->id() != mmm.id() && mmm.cm.bbox.Collide(bm->cm.bbox) && bm->cm.bbox.min.Z() >= md.groove.min.Z())
		{
		translatePath.Z() = bm->cm.bbox.max.Z() - mmm.cm.bbox.min.Z() + gap;
		landToGround.SetTranslate(translatePath);
		mmm.cm.Tr = landToGround * mmm.cm.Tr;
		tri::UpdatePosition<CMeshO>::Matrix(mmm.cm, landToGround, true);
		tri::UpdateBounding<CMeshO>::Box(mmm.cm);
		x = testDepthRecursive_func(mmm, md, gap, x);
		break;
		}*/
	}
	return x;



}

void GenSliceJob::resetImage(cv::Mat &inputImage, int width, int height)
{

	if (inputImage.size() != Size(width, height))
	{
		cv::resize(inputImage, inputImage, Size(width, height));
		qDebug("resetImage resize");
	}
	if (inputImage.type() != CV_8UC3)
	{
		cv::cvtColor(inputImage, inputImage, CV_GRAY2BGR);
		qDebug("resetImage cvtColor");
	}
}

bool GenSliceJob::testIntersection(Point3m v1, Point3m v2, Point3m v3, Point3m orig, Point3m dir)
{
	Point3m e1, e2, q, r, s;
	e1 = v2 - v1;
	e2 = v3 - v1;

	q = dir^e2;

	//dir.normalized();

	float EPSILON = 0.000001f;
	float a = e1.dot(q);
#if 0
	if (a < EPSILON)
	{

		return false;
	}
	s = orig - v1;
	float u = s.dot(q);
	if (u < 0.0 || u > a)
	{

		return false;
	}
	r = s^ e1;
	float v = (dir.dot(r));
	if (v < 0.0f || v + u > a)
	{
		return false;
	}
#else
	if (a < EPSILON && a > -EPSILON)
	{

		return false;
	}

	float f = 1.0f / a;
	s = orig - v1;
	// NORMALIZE(tvec);
	float u = f * (s.dot(q));

	if (u <0.0f || u > 1.0f)
	{
		return false;
	}
	r = s^ e1;
	// NORMALIZE(qvec);
	float v = f* (dir.dot(r));
	if (v < 0.0f || u + v > 1.0f)
	{
		return false;
	}

#endif
	float t = f * (e2.dot(r));
	if (t < 0 || t > 1)
		return false;
	return true;
}

//bool GenSliceJob::testIntersection(Point3m v1, Point3m v2, Point3m v3, Point3m orig, Point3m dir)
//{
//	// Compute the plane supporting the triangle (p1, p2, p3)
//	//     normal: n
//	//     offset: d
//	//
//	// A point P lies on the supporting plane iff n.dot(P) + d = 0
//	//
//#define EPSILON 0.000001f
//	Point3m e21, e31, e32, triN, q, r, s;
//	e21 = v2 - v1;
//	e31 = v3 - v1;
//	triN = e21^e31;
//
//	triN = triN.normalized();
//	float d = -triN.dot(v1);
//
//	// A point P belongs to the line from P1 to P2 iff
//	//     P = P1 + t * (P2 - P1)
//	//
//	// Find the intersection point P(t) between the line and
//	// the plane supporting the triangle:
//	//     n.dot(P) + d = 0
//	//                  = n.dot(P1 + t (P2 - P1)) + d
//	//                  = n.dot(P1) + t n.dot(P2 - P1) + d
//	//
//	//     t = -(n.dot(P1) + d) / n.dot(P2 - P1)
//	//
//	Point3m P21 = dir;
//	float nDotP21 = triN.dot(P21);
//
//	if (fabs(nDotP21) < EPSILON)
//		return false;
//	float t = -(triN.dot(orig) + d) / nDotP21;
//	Point3m P = orig + dir*t;
//
//	if (t<0 || t > 1)
//		return false;
//
//	// Plane bounding the inside half-space of edge (p1, p2): 
//	//     normal: n21 = n x (p2 - p1)
//	//     offset: d21 = -n21.dot(p1)
//	//
//	// A point P is in the inside half-space iff n21.dot(P) + d21 > 0
//	//
//
//	Point3m n21, n32, n13;
//	n21 = triN^e21;
//	float d21 = -n21.dot(v1);
//
//	if (n21.dot(P) + d21 <= 0)
//		return false;
//
//	// Edge (p2, p3)
//	e32 = v3 - v2;
//	n32 = triN ^ e32;
//	float d32 = -n32.dot(v2);
//
//	if (n32.dot(P) + d32 <= 0)
//		return false;
//
//	// Edge (p3, p1)
//	n13 = triN ^ -e31;
//	float d13 = -n13.dot(v3);
//	if (n13.dot(P) + d13 <= 0)
//		return false;
//
//}
void GenSliceJob::releaseMat()
{
	/*look_down_color_CV.release();
	look_down_Black_Mask_layer.release();
	look_down_Black_MaskCV.release();
	look_up_Black_MaskCV.release();
	look_up_color_CV.release();
	captemp.release();
	capDownMask.release();
	black_Mask.release();
	black_Mask2.release();
	outlineBlackMaskCV.release();
	lookDownBlackTriangleMask.release();
	outlineColorCv.release();
	prePrintobjectC.release();

	downColorFirstCV.release();
	downBlackMaskFirstCV.release();
	downColorSecondCV.release();
	downBlackMaskSecondCV.release();

	up_downBlackMaskCV.release();

	originalOutline.release();
	finalcolor.release();
	finalBinder.release();
	backbinder.release();
	mirrorColor.release();
	mirrorBinder.release();*/

	/*cv::resize(captemp, captemp, Size(1, 1));
	cv::resize(capDownMask, capDownMask, Size(1, 1));
	cv::resize(black_Mask, black_Mask, Size(1, 1));
	cv::resize(outlineBlackMaskCV, outlineBlackMaskCV, Size(1, 1));
	cv::resize(prePrintobjectC, prePrintobjectC, Size(1, 1));
	cv::resize(up_downBlackMaskCV, up_downBlackMaskCV, Size(1, 1));
	cv::resize(originalOutline, originalOutline, Size(1, 1));
	cv::resize(finalcolor, finalcolor, Size(1, 1));
	cv::resize(finalBinder, finalBinder, Size(1, 1));
	cv::resize(backbinder, backbinder, Size(1, 1));*/

	/*captemp.deallocate();
	capDownMask.deallocate();;
	black_Mask.deallocate();;
	outlineBlackMaskCV.deallocate();
	prePrintobjectC.deallocate();
	up_downBlackMaskCV.deallocate();
	originalOutline.deallocate();
	finalcolor.deallocate();
	finalBinder.deallocate();
	backbinder.deallocate();*/


}

void GenSliceJob::initMeshVBO(MeshDocument &md)
{
	foreach(int id, md.multiSelectID)
	{
		//CMeshO::VertexIterator vid;
		//vid = tmm->cm.vert.begin();
		//bool x = vid->HasTexCoord();
		MeshModel *tmm = md.getMesh(id);

		if (tmm->getMeshSort() == MeshModel::meshsort::print_item || tmm->getMeshSort() == MeshModel::meshsort::pre_print_item)
		{
			//tmm->rmm.colorMode = GLW::CMNone;
			if (/* tmm->glw.TMIdd[0].size() < 2 &&*/ ((tmm->glw.curr_hints & GLW::Hint::HNUseVBO) != 0))
			{
				/*if (tmm->rmm.textureMode == vcg::GLW::TMPerWedgeMulti || tmm->rmm.textureMode == vcg::GLW::TMPerWedge)
				{*/
				tmm->updateDataMask(MeshModel::MM_FACEFACETOPO);
				tmm->updateDataMask(MeshModel::MM_VERTFACETOPO);
				tri::UpdateNormal<CMeshO>::PerVertexFromCurrentFaceNormal(tmm->cm);
				tri::UpdateNormal<CMeshO>::PerVertex(tmm->cm);
				tri::UpdateNormal<CMeshO>::NormalizePerVertex(tmm->cm);
				vcg::tri::Allocator<CMeshO>::CompactFaceVector(tmm->cm);
				vcg::tri::Allocator<CMeshO>::CompactVertexVector(tmm->cm);
				vcg::tri::UpdateTopology<CMeshO>::FaceFace(tmm->cm);
				vcg::tri::UpdateTopology<CMeshO>::VertexFace(tmm->cm);

				//}
				tmm->glw.initVBOUpdate_multi_texture(GLW::DMSmooth, tmm->rmm.colorMode, tmm->rmm.textureMode);
			}


		}
	}
	foreach(MeshModel *mmm, md.meshList)
	{
		if ((mmm->getMeshSort() == MeshModel::meshsort::print_item || mmm->getMeshSort() == MeshModel::meshsort::pre_print_item)/*&& mmm->glw.TMIdd[0].size() < 2*/)
		{
			if (mmm->glw.multi_array_buffers.size() <= 0)
			{
				/*if (mmm->rmm.textureMode == vcg::GLW::TMPerWedgeMulti || mmm->rmm.textureMode == vcg::GLW::TMPerWedge)
				{*/
				mmm->updateDataMask(MeshModel::MM_FACEFACETOPO);
				mmm->updateDataMask(MeshModel::MM_VERTFACETOPO);
				tri::UpdateNormal<CMeshO>::PerVertexFromCurrentFaceNormal(mmm->cm);
				tri::UpdateNormal<CMeshO>::PerVertex(mmm->cm);
				vcg::tri::Allocator<CMeshO>::CompactFaceVector(mmm->cm);
				vcg::tri::Allocator<CMeshO>::CompactVertexVector(mmm->cm);
				vcg::tri::UpdateTopology<CMeshO>::FaceFace(mmm->cm);
				vcg::tri::UpdateTopology<CMeshO>::VertexFace(mmm->cm);
				//}
				mmm->glw.initVBOUpdate_multi_texture(GLW::DMSmooth, mmm->rmm.colorMode, mmm->rmm.textureMode);
			}
			else if (!glIsBufferARB(mmm->glw.multi_array_buffers[0].array_buffers[0]))
			{
				mmm->updateDataMask(MeshModel::MM_FACEFACETOPO);
				mmm->updateDataMask(MeshModel::MM_VERTFACETOPO);
				tri::UpdateNormal<CMeshO>::PerVertexFromCurrentFaceNormal(mmm->cm);
				tri::UpdateNormal<CMeshO>::PerVertex(mmm->cm);
				vcg::tri::Allocator<CMeshO>::CompactFaceVector(mmm->cm);
				vcg::tri::Allocator<CMeshO>::CompactVertexVector(mmm->cm);
				vcg::tri::UpdateTopology<CMeshO>::FaceFace(mmm->cm);
				vcg::tri::UpdateTopology<CMeshO>::VertexFace(mmm->cm);
				//}
				mmm->glw.initVBOUpdate_multi_texture(GLW::DMSmooth, mmm->rmm.colorMode, mmm->rmm.textureMode);
			}
		}
	}
}
int GenSliceJob::RoundUpToTheNextHighestPowerOf2(unsigned int v)

{
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;
	return v;
}
GenSliceJob::~GenSliceJob()
{
	releaseMat();
}
MESHLAB_PLUGIN_NAME_EXPORTER(GenSliceJob)


