
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

#include "generate_print_func.h"
//#include <QtOpenGL>
#include <wrap/glw/glw.h>
#include <QImage>
#include <QDialog>
#include "slice_function.h"
#include "convertQI_CV.h"
#include "image_process.h"
#include <thread>
#include <time.h>
#include <QImageWriter>



// Constructor usually performs only two simple tasks of filling the two lists
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

using namespace glw;

GenSliceJob::GenSliceJob()
{
	typeList << FP_GPU_EXAMPLE << FP_GENERATE_MONO << FP_PRINT_FLOW << FP_PRINT_FLOW_2 << FP_PRINT_JR << FP_ESTIMATE_COLOR;

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
	oll = 22;
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
	case FP_PRINT_JR: return MeshFilterInterface::Texture;
	case FP_ESTIMATE_COLOR:return MeshFilterInterface::Texture;
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
						   float dpi_x = x / 2.54 * 200;///dpi是以英吋為單位
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
							 float dpi_x = x / 2.54 * 200;///dpi是以英吋為單位
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
	default: assert(0);
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
	//qDebug() << "totalTextureNum" << totalTextureNum;
	//if (toBeUpdatedNum == 0) return;
	//int singleMaxTextureSizeMpx = 85;
	//int singleMaxTextureSize = 8192;
	//qDebug() << "plaa1";
	//glEnable(GL_TEXTURE_2D);
	//GLint MaxTextureSize;
	//glGetIntegerv(GL_MAX_TEXTURE_SIZE, &MaxTextureSize);
	//qDebug() << "MaxTextureSize" << MaxTextureSize;
	//if (singleMaxTextureSize < MaxTextureSize)
	//{		
	//	//qDebug(0, "There are too many textures (%i), reducing max texture size from %i to %i", totalTextureNum, MaxTextureSize, singleMaxTextureSize);
	//	qDebug() << "plaa2";
	//	MaxTextureSize = singleMaxTextureSize;
	//}

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
				QFileInfo fi(mp->fullName());//***mesh路徑
				QDir fid = fi.dir();//mesh資料夾位置	
				QString texpath = fid.absolutePath() + "/" + mp->cm.textures[i].c_str();

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
					int bestW = (img.width());
					int bestH = (img.height());
					/*while (bestW>MaxTextureSize) bestW /= 2;
					while (bestH>MaxTextureSize) bestH /= 2;*/


					//imgScaled = img.scaled(bestW, bestH, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
					//img = img.mirrored(true, false);
					//imgGL = convertToGLFormat(imgScaled);
					mp->glw.TMId.push_back(0);//***先給一個值					
					//****20150402這邊要如何修改***不用改					
					glGenTextures(1, (GLuint*)&(mp->glw.TMId.back()));//***把值拿來用，0會被改調
					glBindTexture(GL_TEXTURE_2D, mp->glw.TMId.back());
					//qDebug() <<"back2"<< mp->glw.TMId.back();
					//qDebug("  	will be loaded as GL texture id %i  ( %i x %i )", mp->glw.TMId.back(), img.width(), img.height());
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, img.bits());
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
bool GenSliceJob::applyFilter(QAction * a, MeshDocument & md, RichParameterSet & par, vcg::CallBackPos * cb, GLArea *gla)
{


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
						   const GLsizei width = (int)(x / 2.54 * print_dpi);
						   const GLsizei height = (int)(width * y / x);
						   qDebug() << "width,height" << width << height;
						   //const GLsizei height = (int)y / 2.54 * print_dpi;
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
						   FILE *dbgff;
						   dbgff = fopen("D:\\debug_open_1.txt", "w");//@@@

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
									   if (cut_z - mdmm->cm.bbox.min.Z() < 0 || cut_z - mdmm->cm.bbox.max.Z()>0)//在物體box的上下需要設定限制，才不會超出物體大小。
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
								   if (cut_z - mdmm->cm.bbox.min.Z() < 0 || cut_z - mdmm->cm.bbox.max.Z()>0)//在物體box的上下需要設定限制，才不會超出物體大小。
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

								   fprintf(dbgff, "b_name %i\n", time.elapsed());
								   fflush(dbgff);
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
							 //							 const GLsizei width = (int)(x / 2.54 * print_dpi);
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
						  const GLsizei width = (int)(x / 2.54 * print_dpi);
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
						  int plus_CM = par.getInt("plus_CM");
						  bool start_print_b = par.getBool("PRINT_START");
						  float useless_print = par.getFloat("USELESS_PRINT");
						  float meCorrectPercent = 0.97;

						  int widthrecord, heightrecord;
						  QString blk_img_file_name = "black_outline";
						  QString zx_sn = makeOutputname(zx_dir.absolutePath(), 0);

						  md.jobname.push_back(zx_sn);

						  //zggg = new ZxGen(unsigned int(width), unsigned int(height), zx_pages, md.groove.DimX(),21, zx_sn.toStdString());//20161219_backup						 
						  //zggg = new ZxGen(unsigned int(width), 2580, zx_job_pages, md.groove.DimX(), 21, zx_sn.toStdString());//20161219_backup						 
						  zggg = new ZxGen(unsigned int(width), unsigned int(height*meCorrectPercent + plus_pixel), zx_job_pages, md.groove.DimX(), md.groove.DimY() + plus_CM, zx_sn.toStdString(), start_print_b);//20161219_backup						 
						  //zggg = new ZxGen(unsigned int(width), unsigned int(4713), zx_job_pages, md.groove.DimX(), md.groove.DimY() + plus_CM, zx_sn.toStdString(), start_print_b);//20161219_backup						 

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
								  if (cut_z - mdmm->cm.bbox.min.Z() < 0 || cut_z - mdmm->cm.bbox.max.Z()>0)//在物體box的上下需要設定限制，才不會超出物體大小。								  
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
								  if (cut_z - mdmm->cm.bbox.min.Z() < 0 || cut_z - mdmm->cm.bbox.max.Z()>0)//在物體box的上下需要設定限制，才不會超出物體大小。
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
								  // zggg = new ZxGen(outlineCTemp.begin()->cols, outlineCTemp.begin()->rows, zx_job_pages, md.groove.DimX(), md.groove.DimY() + plus_CM, zx_sn.toStdString());//20161219_backup						 
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
								  // zggg = new ZxGen(outlineCTemp.begin()->cols, outlineCTemp.begin()->rows, zx_job_pages, md.groove.DimX(), md.groove.DimY() + plus_CM, zx_sn.toStdString());//20161219_backup						 
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
								  zggg = new ZxGen(unsigned int(width), unsigned int(((height*meCorrectPercent) + plus_pixel)), zx_job_pages, md.groove.DimX(), md.groove.DimY() + plus_CM, tempt.toStdString(), start_print_b);//20161219_backup
								  //zggg = new ZxGen(unsigned int(width), unsigned int(4713), zx_job_pages, md.groove.DimX(), md.groove.DimY() + plus_CM, tempt.toStdString(), start_print_b);//20161219_backup

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
	case FP_PRINT_FLOW_2:
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
							const GLsizei width = (int)(x / 2.54 * print_dpi);
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
							float plus_CM = par.getFloat("plus_CM");
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
							zggg = new ZxGen(unsigned int(width), unsigned int(height*meCorrectPercent + plus_pixel), zx_job_pages, md.groove.DimX(), md.groove.DimY() + plus_CM, zx_sn.toStdString(), start_print_b);//20161219_backup						 
							//zggg = new ZxGen(unsigned int(width), unsigned int(4713), zx_job_pages, md.groove.DimX(), md.groove.DimY() + plus_CM, zx_sn.toStdString(), start_print_b);//20161219_backup						 

							Log("ZxGen width %i height %i ", int(width), int(height*meCorrectPercent + plus_pixel));


							//FILE *dbgff;
							//dbgff = fopen("D:\\debug_open_1.txt", "w");//@@@
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

							//***testLog
							Log("Closed %i holes and added %i new faces", 12, 12);
							//boxy_dim--;
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
									if (cut_z - mdmm->cm.bbox.min.Z() < 0 || cut_z - mdmm->cm.bbox.max.Z()>0)//在物體box的上下需要設定限制，才不會超出物體大小。								  
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
										//glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -0.005, 0.005);
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
								glMatrixMode(GL_PROJECTION); glPopMatrix();
								glMatrixMode(GL_MODELVIEW); glPopMatrix();
								glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image.bits());
								QString point01cmCVS = "point01cmCVS";
								point01cmCVS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
								image.save(dir.absolutePath() + "/" + point01cmCVS, "png");
								point01cmCV = QImageToCvMat(image.mirrored());
								//END***generate point01cmCV slice Image

								glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
								glClearColor(1, 1, 1, 1);
								glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
									glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -100, 100);
								glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
									gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, 0, 0, 1, 0);
								//cmystrips(md, par.getFloat("useless_print") / 3.);//***useless bar

								foreach(MeshModel *mp, vmm)//畫輪廓圖。
								{
									mp->glw.viewerNum = 2;
									if (vmm.size() > 0)
									{
										mp->render(vcg::GLW::DMTexWire, mp->rmm.colorMode, mp->rmm.textureMode);//backup
										//mp->render(vcg::GLW::DMTexWire, vcg::GLW::CMPerVert, mp->rmm.textureMode);//backup
										//mp->render(vcg::GLW::DMTexWire, vcg::GLW::CMPerVert, GLW::TMPerVert);//backup		
										//mp->render(vcg::GLW::DMTexWire, GLW::CMNone, mp->rmm.textureMode);//backup									 
									}
								}

								glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image2.bits());
								QString outlineCVS = "outlineCVS";
								outlineCVS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
								image.save(dir.absolutePath() + "/" + outlineCVS, "png");
								outlineCV = QImageToCvMat(image2.mirrored());
								
								//START***generate point05mm slice picture**************************
								glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
								glClearColor(1, 1, 1, 1);
								glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
									glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., 0.005, -0.005);
								glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
									gluLookAt(0, 0, cut_z, 0, 0, -30, 0, 1, 0);

								foreach(MeshModel *mp, vmm)//畫輪廓圖。
								{
									mp->glw.viewerNum = 2;
									if (vmm.size() > 0)
									{
										mp->render(vcg::GLW::DMTexWire, mp->rmm.colorMode, mp->rmm.textureMode);//backup
																	 
									}
								}
								glMatrixMode(GL_PROJECTION); glPopMatrix();
								glMatrixMode(GL_MODELVIEW); glPopMatrix();
								glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image.bits());
								QString point05mmCVS = "point05mmCVS";
								point05mmCVS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
								image.save(dir.absolutePath() + "/" + point05mmCVS, "png");
								point05mmCV = QImageToCvMat(image.mirrored());
								//END***generate point05mm slice picture




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
									glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -100, 100);
								glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); 
									gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, 0, 0, 1, 0);
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
									if (cut_z - mdmm->cm.bbox.min.Z() < 0 || cut_z - mdmm->cm.bbox.max.Z()>0)//在物體box的上下需要設定限制，才不會超出物體大小。
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
										//glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -0.6, 0.6);
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


								//START***generate point05mm slice_BLACK_MASK**************************
								glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
								glClearColor(1, 1, 1, 1);
								glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
									glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., 0.005, -0.005);
								glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
									gluLookAt(0, 0, cut_z, 0, 0, -30, 0, 1, 0);

								foreach(MeshModel *mdmm, md.meshList)
								{
									if (mdmm->getMeshSort() == MeshModel::meshsort::print_item)
									{
										glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();										
											glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -0.005, 0.005);									
										glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); 
											gluLookAt(0, 0, cut_z, 0, 0, -30, 0, 1, 0);
										glPushMatrix();
										mdmm->render(GLW::DMFlat, GLW::CMBlack, GLW::TMNone);
										glPopMatrix();
									}
								}
								glMatrixMode(GL_PROJECTION); glPopMatrix();
								glMatrixMode(GL_MODELVIEW); glPopMatrix();
								glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image.bits());
								QString point05mm_Black_MaskCVS = "point05mm_Black_MaskCVS";
								point05mm_Black_MaskCVS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
								image.save(dir.absolutePath() + "/" + point05mm_Black_MaskCVS, "png");
								point05mm_Black_MaskCV = QImageToCvMat(image.mirrored());
								//END***generate point05mm slice_BLACK_MASK

								//START***generate outline_BLACK_MASK**************************
								glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
								glClearColor(1, 1, 1, 1);
								glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
								glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -100, 100);
								glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
								gluLookAt(0, 0, tempgroove.DimZ(), 0, 0, 0, 0, 1, 0);
								foreach(MeshModel *mp, vmm)//outline
								{
									mp->glw.viewerNum = 2;
									if (vmm.size() > 0)
									{
										mp->render(vcg::GLW::DMTexWire, vcg::GLW::CMBlack, vcg::GLW::TMNone);//backup
									}
								}
								glMatrixMode(GL_PROJECTION); glPopMatrix();
								glMatrixMode(GL_MODELVIEW); glPopMatrix();
								glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image.bits());
								QString outline_Black_MaskCVS = "outline_Black_MaskCVS";
								outline_Black_MaskCVS.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
								image.save(dir.absolutePath() + "/" + outline_Black_MaskCVS, "png");
								outline_Black_MaskCV = QImageToCvMat(image.mirrored());
								//END***generate outline_BLACK_MASK*********************************


								//if (par.getBool("BLK_OUT_LINE"))
								/*if (1)
								{
								QString tttt2 = "black_mask_QI";
								tttt2.append(QString("blk_%1.png").arg(i, 4, 10, QChar('0')));
								image.mirrored().save(dir.absolutePath() + "/" + tttt2, "png");
								}*/

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
									/*if (par.getBool("mono_bool"))
									{
									image.fill(Qt::white);
									originalOutline = QImageToCvMat(image);
									}*/
									SKT::InkBalance imageProcess;

									cv::Mat temp, temp2;
									//temp = SKT::resizeIamge(&originalOutline,plus_pixel);
									point01cmCV = SKT::combinePoint05mmToPoint01cm(&point01cmCV, point05mmCV, point05mm_Black_MaskCV);
									originalOutline = SKT::combineOutlineWithWidthtoImage(&point01cmCV, outlineCV, outline_Black_MaskCV);



									//imageProcess.operate2(temp, captemp, SKT::resizeIamge(&black_Mask, plus_pixel));
									imageProcess.operate2(originalOutline, captemp, black_Mask, false);
									//SKT::generateUselessbar(temp);
									if (par.getBool("mono_bool"))originalOutline = cv::Scalar(255, 255, 255);

									//xy_axis now is fine
									//originalOutline = SKT::resizeXAxis(&originalOutline, meCorrectPercent);//****0.97
									originalOutline = SKT::resizeIamge(&originalOutline, plus_pixel);
									//SKT::generateUselessbar(originalOutline, useless_print);
									//SKT::generateUselessbar(originalOutline, useless_print);//20160715_bakcup
									//bool longDin = par.getBool("longpage_orshortpage");
									bool longDin = false;
									SKT::generateUselessbarDin(originalOutline, useless_print, longDin);
									outlineCTemp.push_back(originalOutline);//**********PUSH_BACK_OUTLINE********************************

									///////cap_temp_change_add_pattern///////////////////
									black_Mask = SKT::erodeImage(&black_Mask, 51);
									//captemp = SKT::binderAddPattern(&captemp,&black_Mask);
									//if(((i-60)%100)>100)
									if ((i % 100) > 20)
										//if (1)
										captemp = SKT::binderAddHexagon(&captemp, &black_Mask, patternPath.absolutePath());
									//////////////////////////////////
									//xy_axis now is fine
									//captemp = SKT::resizeXAxis(&captemp, meCorrectPercent);//****0.97
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
#if 1
									////picasso/////////////////////////////////////////////////
									//if (i < 50)
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


								if (par.getBool("generate_zx") && j % send_page_num == 0)//幾頁傳到已產生的zxfile一次
								{
									//if (zggg)
									//{
									// zggg = new ZxGen(outlineCTemp.begin()->cols, outlineCTemp.begin()->rows, zx_job_pages, md.groove.DimX(), md.groove.DimY() + plus_CM, zx_sn.toStdString());//20161219_backup						 
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

									//fprintf(dbgff, "zx_gen %i, %i\n", time.elapsed(), i);
									//fflush(dbgff);

								}
								else if (par.getBool("generate_zx") && j % send_page_num != 0 && j == boxy_dim)//超過一個zxfile, 頁數不超過1個job
								{
									//if (zggg)
									//{
									// zggg = new ZxGen(outlineCTemp.begin()->cols, outlineCTemp.begin()->rows, zx_job_pages, md.groove.DimX(), md.groove.DimY() + plus_CM, zx_sn.toStdString());//20161219_backup						 
									//}
									//time.restart();
									zggg->sendData(outlineCTemp, capCTemp);
									capCTemp.clear();
									outlineCTemp.clear();
									//time.elapsed();
									zggg->close();
									zggg->~ZxGen();

									//fprintf(dbgff, "zx_gen %i, %i\n", time.elapsed(), i);
									//fflush(dbgff);
								}
								QString tempt = makeOutputname(zx_dir.absolutePath(), j);
								if (j % zx_job_pages == 0)//一個job，產生下一個job
								{
									md.jobname.push_back(tempt);
									zggg->close();
									zggg->~ZxGen();
									zggg = new ZxGen(unsigned int(width), unsigned int(((height*meCorrectPercent) + plus_pixel)), zx_job_pages, md.groove.DimX(), md.groove.DimY() + plus_CM, tempt.toStdString(), start_print_b);//20161219_backup
									//zggg = new ZxGen(unsigned int(width), unsigned int(4713), zx_job_pages, md.groove.DimX(), md.groove.DimY() + plus_CM, tempt.toStdString(), start_print_b);//20161219_backup

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
#pragma endregion FP_PRINT_FLOW_2
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
						const GLsizei width = (int)(x / 2.54 * print_dpi);
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
						float plus_CM = par.getFloat("plus_CM");
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
						//zggg = new ZxGen(unsigned int(width), unsigned int(height*meCorrectPercent + plus_pixel), zx_job_pages, md.groove.DimX(), md.groove.DimY() + plus_CM, zx_sn.toStdString(), start_print_b);//20161219_backup						 
						//zggg = new ZxGen(unsigned int(width), unsigned int(4713), zx_job_pages, md.groove.DimX(), md.groove.DimY() + plus_CM, zx_sn.toStdString(), start_print_b);//20161219_backup						 

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
						//boxy_dim--;
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
								if (cut_z - mdmm->cm.bbox.min.Z() < 0 || cut_z - mdmm->cm.bbox.max.Z()>0)//在物體box的上下需要設定限制，才不會超出物體大小。								  
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
							glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -100, 100);
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
								if (cut_z - mdmm->cm.bbox.min.Z() < 0 || cut_z - mdmm->cm.bbox.max.Z()>0)//在物體box的上下需要設定限制，才不會超出物體大小。
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
									captemp = SKT::binderAddHexagon(&captemp, &black_Mask, patternPath.absolutePath());
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
							  const GLsizei width = (int)(x / 2.54 * print_dpi);
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
							  std::vector<QImage *> estimate_image;
							  //QImage *estimate_image[6];
							  for (int i = 0; i < 6; i++)
								  estimate_image.push_back(new QImage(int(width), int(height), QImage::Format_RGB888));

							  QImage image(int(width), int(height), QImage::Format_RGB888);
							  QImage image2(int(width), int(height), QImage::Format_RGB888);
							  QImage cap_image(int(width), int(height), QImage::Format_RGB888);

							  //***20150505***slice_func_設置高度
							  float box_bottom = md.bbox().min.Z();
							  float unit = par.getFloat("slice_height");



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
							  float plus_CM = par.getFloat("plus_CM");
							  bool start_print_b = par.getBool("PRINT_START");
							  float useless_print = par.getFloat("USELESS_PRINT");
							  float meCorrectPercent = 1;

							  int widthrecord, heightrecord;
							  QString blk_img_file_name = "black_outline";
							  QString zx_sn = makeOutputname(zx_dir.absolutePath(), 0);

							  md.jobname.push_back(zx_sn);

							  zggg = new ZxGen(unsigned int(width), unsigned int(height*meCorrectPercent + plus_pixel), zx_job_pages, md.groove.DimX(), md.groove.DimY() + plus_CM, zx_sn.toStdString(), start_print_b);//20161219_backup						 
							  //zggg = new ZxGen(unsigned int(width), unsigned int(4713), zx_job_pages, md.groove.DimX(), md.groove.DimY() + plus_CM, zx_sn.toStdString(), start_print_b);//20161219_backup						 

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

							  //***testLog
							  Log("Closed %i holes and added %i new faces", 12, 12);
							  //for (int i = start_page, j = start_page + 1; i < boxy_dim; i++, j++)
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
										  case 0:
											  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
											  glClearColor(1, 1, 1, 1);
											  glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
											  glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -100, 100);
											  glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, 30, 0, 0, -30, 0, 1, 0);
											  break;
										  case 1:
											  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
											  glClearColor(1, 1, 1, 1);
											  glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
											  glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -100, 100);
											  glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, -30, 0, 0, 30, 0, 1, 0);
											  break;
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
							  //***end cube forloop*********************




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
	QString temp = dir + "/output_" + QString::number(numpages) + ".zx";
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

void GenSliceJob::fillsquare(void) {
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_LIGHTING);
	glColor3f(0, 0, 0);
	glPushMatrix();

	glNormal3f(0, 0, -1);
	glBegin(GL_QUADS);
	glVertex3f(-oll, -oll, equ[3]);
	glVertex3f(oll, -oll, equ[3]);
	glVertex3f(oll, oll, equ[3]);
	glVertex3f(-oll, oll, equ[3]);
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

MESHLAB_PLUGIN_NAME_EXPORTER(GenSliceJob)
