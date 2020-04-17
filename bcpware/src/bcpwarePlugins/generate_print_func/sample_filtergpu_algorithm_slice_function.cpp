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




// Constructor usually performs only two simple tasks of filling the two lists
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

using namespace glw;

GenSliceJob::GenSliceJob()
{
	typeList << FP_GPU_EXAMPLE;

	foreach(FilterIDType tt, types())
	{
		actionList << new QAction(filterName(tt), this);
		if (tt == FP_GPU_EXAMPLE)  actionList.last()->setIcon(QIcon(":/myImage/generate_slice_img.jpg"));
	}
	//======
	//pd = new QProgressDialog("Task in progress.", "Cancel", 0, 100000);
	connect(this, SIGNAL(updatepd(int)), this, SLOT(setpd(int)), Qt::DirectConnection);

	//==============================

}

// ST() must return the very short string describing each filtering action
// (this string is used also to define the menu entry)
QString GenSliceJob::filterName(FilterIDType filterId) const
{
	switch (filterId) {
	case FP_GPU_EXAMPLE:  return QString("Generate_Slice_Pic");
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
						   parlst.addParam(new RichBool("FaceColor", false, "FaceColor", ""));

						   parlst.addParam(new RichFloat("useless_print", 1, "useless_print_des", ""));//犧牲條寬度
						   parlst.addParam(new RichBool("generate_zx", 0, "generate_zx_or_not", ""));//
						   parlst.addParam(new RichInt("per_zx_pages", 10, "number of zx pages", ""));//
						   parlst.addParam(new RichInt("start_page", 0, "start print pages"));

						   int boxy_dim = qCeil((md.bbox().max.Z() - md.bbox().min.Z()) / 0.005) + 1;
						   parlst.addParam(new RichInt("end_page", boxy_dim, "end print pages"));
						   parlst.addParam(new RichInt("send_page_num", 10, "send_page_num"));




						   break;
	}
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
bool GenSliceJob::applyFilter(QAction * a, MeshDocument & md, RichParameterSet & par, vcg::CallBackPos * /*cb*/, GLArea *gla)
{


	switch (ID(a))
	{
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
						   RenderbufferHandle hDepth = createRenderbuffer(ctx, GL_DEPTH_COMPONENT24, width, height);						  
						   Texture2DHandle    hColor = createTexture2D(ctx, GL_RGB8, width, height, GL_RGB, GL_UNSIGNED_BYTE);
						   FramebufferHandle  hFramebuffer = createFramebuffer(ctx, renderbufferTarget(hDepth), texture2DTarget(hColor));



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
						   ZxGen zGG(unsigned int(width), unsigned int(height), boxy_dim, md.groove.DimX(), zx_sn);//20161219_backup
						   //ZxGen zGG(unsigned int(width), unsigned int(height)+500, boxy_dim, md.groove.DimX(), zx_sn);
						   qDebug() << "sample_filtergpu" << md.groove.DimX();
						   //for (int i = 0, j = 1; i < boxy_dim; i++, j++)
						   //boxy_dim+1因為讓最後一層顏色多一層
						   for (int i = start_page, j = start_page + 1; i < boxy_dim; i++, j++)
							   //for (int i = 0; i < 1; i++)
						   {
							   //glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
							   //glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., 0.01, 0);
							   ////glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., 0.1, -0.1);
							   //glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, box_bottom + unit*i, 0, 0, -30, 0, 1, 0);

							   //qDebug() << "sample_filter_i" << box_bottom + unit*i;


							   xyz::slice_roution(md, box_bottom + unit*i, faceColor);//*********************切層演算							  
							   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
								   float cut_z = box_bottom + unit*i;
								   if (mdmm->rmm.colorMode != GLW::CMNone)//如果無色就不須做
								   if (cut_z - mdmm->cm.bbox.min.Z() < 0 || cut_z - mdmm->cm.bbox.max.Z()>0)//在物體box的上下需要設定ortho
								   {
									   glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
									   glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., 0.0001, 0);
									   glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, box_bottom + unit*i, 0, 0, -30, 0, 1, 0);

									   glPushMatrix();
									   if (vcg::tri::HasPerWedgeTexCoord(mdmm->cm) && !mdmm->cm.textures.empty())
										   mdmm->render(GLW::DMSmooth, GLW::CMNone, GLW::TMPerWedgeMulti);
									   else if (vcg::tri::HasPerVertexColor(mdmm->cm) || vcg::tri::HasPerFaceColor(mdmm->cm))
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
								   else
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
									   else if (vcg::tri::HasPerVertexColor(mdmm->cm) || vcg::tri::HasPerFaceColor(mdmm->cm))
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


							   foreach(MeshModel *mp, vmm)//outline
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
							   foreach(MeshModel *mp, cmm)//扣掉白色的輪廓
							   {

								   mp->render(vcg::GLW::DMCapEdge, vcg::GLW::CMTestSig, vcg::GLW::TMNone);
							   }



							   QString img_file_name = md.p_setting.getoutlineName();


							   //***第一張圖片編號
							   //***20150921***增加bool box

							   //***20150508***BMP QImage設為Format_RGBA8888, readpixel要設為GL_RGBA
							   glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image.bits());

							   if (par.getBool("generate_zx"))
							   {
								   originalOutline = QImageToCvMat(image.mirrored());
								   //outlineCTemp.push_back(QImageToCvMat(image.mirrored())); //zxzxzxzxzxzxzxz
							   }
							   if (outputOL_Image)
							   {
								   img_file_name.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
								   image.mirrored().save(dir.absolutePath() + "/" + img_file_name, "png");
							   }


							   //***20160122***產生黑色輪廓編號_mask
							   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

							   //********************************
							   foreach(MeshModel *mdmm, md.meshList)
							   {
								   glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
								   glOrtho(-tempgroove.groovelength / 2., tempgroove.groovelength / 2., -tempgroove.grooveheight / 2., tempgroove.grooveheight / 2., -0.05, 0.05);
								   //***20161112
								   glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, box_bottom + unit*i, 0, 0, -30, 0, 1, 0);

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
							   foreach(MeshModel *mp, cmm)//扣掉白色的輪廓
							   {

								   mp->render(vcg::GLW::DMCapEdge, vcg::GLW::CMTestSig, vcg::GLW::TMNone);
							   }
							   QString blk_img_file_name = "black_outline";
							   glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image.bits());
							   black_Mask = QImageToCvMat(image.mirrored());
							   if (outputOL_Image)
							   {
								   blk_img_file_name.append(QString("blk_%1.png").arg(i, 4, 10, QChar('0')));
								   image.mirrored().save(dir.absolutePath() + "/" + blk_img_file_name, "png");
							   }


							   //===========================================================================================

							   //***第二張圖**********
							   if (!ol_and_cap)glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
							   glClearColor(1, 1, 1, 1);
							   binderstrips(md, par.getFloat("useless_print"));

							   /*foreach(MeshModel *mp, vmm)//===當有顏色與材質時，也要有黑色輪廓當binder。
							   {
							   mp->glw.viewerNum = 2;
							   if (vmm.size() > 0)
							   {
							   //qDebug() << "picrender";
							   //qDebug("meshname" + (*mp).label().toLatin1());
							   if (mp->rmm.textureMode == GLW::TMPerVert || mp->rmm.colorMode == GLW::CMPerVert)
							   mp->render(vcg::GLW::DMTexWire, vcg::GLW::CMBlack, vcg::GLW::TMNone);
							   }
							   }*/

							   foreach(MeshModel *mp, cmm)//binder圖
							   {
								   //mp->render(vcg::GLW::DMCapEdge, vcg::GLW::CMNone, vcg::GLW::TMNone);
								   //mp->render(vcg::GLW::DMCapEdge, vcg::GLW::CMTestSig, vcg::GLW::TMNone);
								   mp->render(vcg::GLW::DMCapEdge, vcg::GLW::CMNone, vcg::GLW::TMNone);
							   }

							   glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, cap_image.bits());//***20150724
							   //glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, cap_image.bits());							   
							   QString img_file_name2 = md.p_setting.getcapImageName();

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
								   img_file_name2.append(QString("_%1.png").arg(i, 4, 10, QChar('0')));
								   cap_image.mirrored().save(dir.absolutePath() + "/" + img_file_name2, "png");
							   }
							   //qDebug() << "img_file_name2" << img_file_name2;
							   //****

							   //***20160112***image_process加上pattern要在這處理
							   if (par.getBool("generate_zx"))
							   {
								   SKT::InkBalance imageProcess;
								   imageProcess.operate2(originalOutline, capCTemp.back(), black_Mask);

								   //cv::Mat getRealOutlineDst(originalOutline.size(), originalOutline.type());
								   //SKT::getrealOutline(capCTemp.back(), originalOutline, black_Mask);
								   outlineCTemp.push_back(originalOutline);//backup
								   //outlineCTemp.push_back(SKT::resizeIamge(&originalOutline));

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
							   }
							   else if (par.getBool("generate_zx") && j % send_page_num != 0 && j == boxy_dim)
							   {
								   zGG.sendData(outlineCTemp, capCTemp);
								   capCTemp.clear();
								   outlineCTemp.clear();

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

						   if (par.getBool("generate_zx")){

							   QString ppath = "cd /d " + getRoamingDir();

							   QStringList arguments;
							   //arguments << "/k" << "cd /d d:/temptemp/zx_file" << "&" << "usbsend.exe" << "output.zx";						   
							   arguments << "/k" << ppath << "&&" << "usbsend.exe" << "output.zx" << "&&" << "exit";
							   //QProcess::startDetached("cmd", arguments);
						   }

						   qDebug() << " done _applyClick ";
						   break;
	}


	}
	return true;
}
void GenSliceJob::setpd(int per)
{
	qDebug() << "stdpd" << per;
	progress->setValue(per);
	progress->update();
}
void GenSliceJob::generateZX(float width, float height, int pages, float grooveDimx, string full_filename)
{
	ZxGen zGG(unsigned int(width), unsigned int(height), pages, grooveDimx, full_filename);
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
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Temp", "application");
	QString location = QFileInfo(settings.fileName()).absolutePath() + "/";
	return location;

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


MESHLAB_PLUGIN_NAME_EXPORTER(GenSliceJob)
