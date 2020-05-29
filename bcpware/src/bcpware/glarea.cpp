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


#include "../common/interfaces.h"
#include "glarea.h"
#include "mainwindow.h"

#include <QFileDialog>
#include <QClipboard>
#include <QOpenGLTexture>

#include <wrap/gl/picking.h>
#include <wrap/qt/trackball.h>
#include <wrap/qt/col_qt_convert.h>
#include <wrap/qt/shot_qt.h>
#include <wrap/qt/checkGLError.h>
#include <wrap/qt/gl_label.h>
#include <wrap/io_trimesh/export_ply.h>
#include <wrap/io_trimesh/import_ply.h>
#include <gl/freeglut.h>
#include "gl_label_SKT.h"

//#include <wrap/gl/space.h>
//#include <wrap/gl/math.h>
//#include <vcg/space/color4.h>
//#include <wrap/gl/glu_tessellator_cap.h>



using namespace std;
using namespace vcg;

//GLfloat GLArea::cone_mat[4] = { 0.f, .5f, 1.f, 1.f };
GLUquadricObj *GLArea::cone;// = gluNewQuadric();
GLUquadricObj *GLArea::base;// = gluNewQuadric();

GLdouble GLArea::equ[] = { 0, 0, -1, 0 };
int GLArea::getPickId = 0;
#define viewDistance 5000

GLArea::GLArea(MultiViewer_Container *mvcont, RichParameterSet *current)
: QGLWidget(), interrbutshow(false), timerID(0), counttime(0), _grid_chip_gap_(grid_chip_gap_)
{
	hasToRefreshGlList = false;
	drawLists.resize(2, 0);
	this->setParent(mvcont);
	parentmultiview = mvcont;
	this->updateCustomSettingValues(*current);
	animMode = AnimNone;
	iRenderer = 0; //Shader support
	iEdit = 0;
	currentEditor = 0;
	suspendedEditor = false;
	lastModelEdited = 0;
	cfps = 0;
	lastTime = 0;
	hasToPick = false;
	hasToSelect = false;
	selectionSwitch = true;
	hasToGetPickPos = false;
	hasToUpdateTexture = false;
	helpVisible = false;
	takeSnapTile = false;
	activeDefaultTrackball = true;
	infoAreaVisible = true;
	trackBallVisible = false;//***showtrackball
	currentShader = NULL;
	lastFilterRef = NULL;
	//lastEditRef = NULL;
	setAttribute(Qt::WA_DeleteOnClose, true);
	fov = fovDefault();
	clipRatioFar = 5;
	clipRatioNear = clipRatioNearDefault();
	nearPlane = .2f;
	farPlane = 5.0f;
	drawMeshFlag = true;
	contextMenuFlag = true;
	VBOupdatePart[0] = true;
	VBOupdatePart[1] = false;
	VBOupdatePart[2] = false;
	grid_chip_gap_ = true;

	id = mvcont->getNextViewerId();

	updateMeshSetVisibilities();
	updateRasterSetVisibilities();
	setAutoFillBackground(false);

	//Ratser support
	_isRaster = false;
	opacity = 0.5;
	zoom = false;
	targetTex = 0;

	connect(this->md(), SIGNAL(currentMeshChanged(int)), this, SLOT(manageCurrentMeshChange()), Qt::QueuedConnection);
	connect(this->md(), SIGNAL(meshDocumentModified()), this, SLOT(updateAllPerMeshDecorators()), Qt::QueuedConnection);
	connect(this->md(), SIGNAL(meshSetChanged()), this, SLOT(updateMeshSetVisibilities()));
	connect(this->md(), SIGNAL(rasterSetChanged()), this, SLOT(updateRasterSetVisibilities()));
	connect(this->md(), SIGNAL(documentUpdated()), this, SLOT(completeUpdateRequested()));
	connect(this, SIGNAL(updateLayerTable()), this->mw(), SIGNAL(updateLayerTable()));
	connect(md(), SIGNAL(meshAdded(int, RenderMode)), this, SLOT(meshAdded(int, RenderMode)));
	connect(md(), SIGNAL(meshRemoved(int)), this, SLOT(meshRemoved(int)));

	foreach(MeshModel* mesh, md()->meshList)//初始化rendermodemap
	{
		rendermodemap[mesh->id()] = RenderMode();
	}

	/*getting the meshlab MainWindow from parent, which is QWorkspace.
	*note as soon as the GLArea is added as Window to the QWorkspace the parent of GLArea is a QWidget,
	*which takes care about the window frame (its parent is the QWorkspace again).
	*/
	MainWindow* mainwindow = this->mw();
	//connecting the MainWindow Slots to GLArea signal (simple passthrough)
	if (mainwindow != NULL) {
		connect(this, SIGNAL(updateMainWindowMenus()), mainwindow, SLOT(updateMenus()));
		connect(mainwindow, SIGNAL(dispatchCustomSettings(RichParameterSet&)), this, SLOT(updateCustomSettingValues(RichParameterSet&)));
	}
	else {
		qDebug("The parent of the GLArea parent is not a pointer to the  MainWindow.");
	}

	//***20150408
	//if (vd == top || vd == front)trackball.current_mode = NULL;
	//printArea = new Box3m(vcg::Point3f(-1, -1, -1),vcg::Point3f(1, 1, 1));
	//printArea = printBox(15, 20, 17);
	//printArea.Translate(vcg::Point3f(2, 0, 0));

	lockTrackmouse = false;
	templockTrackmouse = false;
	showDisplayInfo = false;
	openEditMoving = false;
	md()->groove.setGrooveHit(false);
	//md()->groove.setLWH(21, 20, 21); //(x, z, y)
	//md()->groove.setLWH(22, 20, 22); //(x, z, y)
	updateRendermodemapSiganl = false;

	//***20160302

	//render_slice_height = 0;
	/*equ[0] = 0;
	equ[1] = 0;
	equ[2] = -1;
	equ[3] = md()->bbox().min.Z();*/
	//***20160322
	timerID = startTimer(30);

	this->addAction(new QAction("test_action", this));
	connect(this, SIGNAL(matrix_changed(MeshModel *, Matrix44m)), mw(), SLOT(matrix_changed(MeshModel*, Matrix44m)));
	connect(this, SIGNAL(matrix_changed_2(QSet<int>, const Matrix44m)), mw(), SLOT(matrix_changed_2(QSet<int>, const Matrix44m)));
	vboid = 0;
	/*sortDepthColor.push_back(Point3f(rand() / (RAND_MAX + 1.0), rand() / (RAND_MAX + 1.0), rand() / (RAND_MAX + 1.0)));
	sortDepthColor.push_back(Point3f(rand() / (RAND_MAX + 1.0), rand() / (RAND_MAX + 1.0), rand() / (RAND_MAX + 1.0)));
	sortDepthColor.push_back(Point3f(rand() / (RAND_MAX + 1.0), rand() / (RAND_MAX + 1.0), rand() / (RAND_MAX + 1.0)));
	sortDepthColor.push_back(Point3f(rand() / (RAND_MAX + 1.0), rand() / (RAND_MAX + 1.0), rand() / (RAND_MAX + 1.0)));
	sortDepthColor.push_back(Point3f(rand() / (RAND_MAX + 1.0), rand() / (RAND_MAX + 1.0), rand() / (RAND_MAX + 1.0)));*/
	sortDepthColor.push_back(Point3f(249 / 255., 12 / 255., 12 / 255.));
	sortDepthColor.push_back(Point3f(249 / 255., 159 / 255., 11 / 255.));
	sortDepthColor.push_back(Point3f(248 / 255., 249 / 255., 11 / 255.));
	sortDepthColor.push_back(Point3f(114 / 255., 253 / 255., 7 / 255.));
	sortDepthColor.push_back(Point3f(2 / 255., 175 / 255., 250 / 255.));
	sortDepthColor.push_back(Point3f(19 / 255., 3 / 255., 250 / 255.));
	sortDepthColor.push_back(Point3f(196 / 255., 2 / 255., 249 / 255.));

	//rubberBand = new QRubberBand(QRubberBand::Line, this);
	
}
void GLArea::timerEvent(QTimerEvent * event)
{
	if (timerID == event->timerId())
		mvc()->updateAllViewer();
}
GLArea::~GLArea()
{
	rendermodemap.clear();
}

/*
This member returns the information of the Mesh in terms of VC,VQ,FC,FQ,WT
where:
VC = VertColor,VQ = VertQuality,FC = FaceColor,FQ = FaceQuality,WT = WedgTexCoord
*/
QString GLArea::GetMeshInfoString()
{
	QString info;
	if (mm()->hasDataMask(MeshModel::MM_VERTQUALITY)) { info.append("VQ "); }
	if (mm()->hasDataMask(MeshModel::MM_VERTCOLOR)) { info.append("VC "); }
	if (mm()->hasDataMask(MeshModel::MM_VERTRADIUS)) { info.append("VR "); }
	if (mm()->hasDataMask(MeshModel::MM_VERTTEXCOORD)) { info.append("VT "); }
	if (mm()->hasDataMask(MeshModel::MM_VERTCURV)) { info.append("VK "); }
	if (mm()->hasDataMask(MeshModel::MM_VERTCURVDIR)) { info.append("VD "); }
	if (mm()->hasDataMask(MeshModel::MM_FACECOLOR)) { info.append("FC "); }
	if (mm()->hasDataMask(MeshModel::MM_FACEQUALITY)) { info.append("FQ "); }
	if (mm()->hasDataMask(MeshModel::MM_WEDGTEXCOORD)) { info.append("WT "); }
	if (mm()->hasDataMask(MeshModel::MM_CAMERA)) { info.append("MC "); }
	if (mm()->hasDataMask(MeshModel::MM_POLYGONAL)) { info.append("MP "); }

	return info;
}


void GLArea::Logf(int Level, const char * f, ...)
{
	if (this->md() == 0) return;

	char buf[4096];
	va_list marker;
	va_start(marker, f);

	vsprintf(buf, f, marker);
	va_end(marker);
	this->md()->Log.Log(Level, buf);
}

QSize GLArea::minimumSizeHint() const { return QSize(400, 300); }
QSize GLArea::sizeHint() const { return QSize(400, 300); }


void GLArea::initializeGL()
{

	glShadeModel(GL_SMOOTH);
	glPixelStorei(GL_PACK_ROW_LENGTH, 0);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	static float diffuseColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glEnable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseColor);
	/*trackball.center=Point3f(0, 0, 0);
	trackball.radius= 1;
	trackball_light.center = Point3f(0, 0, 0);
	trackball_light.radius = 1;*/
	//***20150410//設定trackball
	trackball.center = md()->groove.Center();
	trackball.radius = md()->groove.Diag() / 4.0;//***diag除2半徑除2
	trackball_light.center = md()->groove.Center();
	trackball_light.radius = md()->groove.Diag() / 4.0;
	//*******

	m_program = new QOpenGLShaderProgram(this);
	m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
	m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
	m_program->link();




	GLenum err = glewInit();
	if (err != GLEW_OK) {
		assert(0);
	}

}

void GLArea::pasteTile()
{
	QString outfile;

	glPushAttrib(GL_ENABLE_BIT);
	//QImage tileBuffer = grabFrameBuffer(true).mirrored(false, true);
	//===20151104
	QImage tileBuffer = grabFrameBuffer(false).mirrored(false, true);
	if (ss.tiledSave)
	{
		outfile = QString("%1/%2_%3-%4.png")
			.arg(ss.outdir)
			.arg(ss.basename)
			.arg(tileCol, 2, 10, QChar('0'))
			.arg(tileRow, 2, 10, QChar('0'));

		//tileBuffer.rgbSwapped();
		tileBuffer.mirrored(false, true).save(outfile, "PNG");
		//***20151104

	}
	else
	{
		if (snapBuffer.isNull())
			snapBuffer = QImage(tileBuffer.width() * ss.resolution, tileBuffer.height() * ss.resolution, tileBuffer.format());

		uchar *snapPtr = snapBuffer.bits() + (tileBuffer.bytesPerLine() * tileCol) + ((totalCols * tileRow) * tileBuffer.byteCount());
		uchar *tilePtr = tileBuffer.bits();

		for (int y = 0; y < tileBuffer.height(); y++)
		{
			memcpy((void*)snapPtr, (void*)tilePtr, tileBuffer.bytesPerLine());
			snapPtr += tileBuffer.bytesPerLine() * totalCols;
			tilePtr += tileBuffer.bytesPerLine();
		}
	}
	tileCol++;

	if (tileCol >= totalCols)
	{
		tileCol = 0;
		tileRow++;

		if (tileRow >= totalRows)
		{
			if (ss.snapAllLayers)
			{
				outfile = QString("%1/%2%3_L%4.png")
					.arg(ss.outdir).arg(ss.basename)
					.arg(ss.counter, 2, 10, QChar('0'))
					.arg(currSnapLayer, 2, 10, QChar('0'));
			}
			else {
				outfile = QString("%1/%2%3.png")
					.arg(ss.outdir).arg(ss.basename)
					.arg(ss.counter++, 2, 10, QChar('0'));
			}

			if (!ss.tiledSave)
			{
				bool ret = (snapBuffer.mirrored(false, true)).save(outfile, "PNG");
				if (ret)
				{
					this->Logf(GLLogStream::SYSTEM, "Snapshot saved to %s", outfile.toLocal8Bit().constData());
					if (ss.addToRasters)
					{
						mw()->importRaster(outfile);

						RasterModel *rastm = md()->rm();
						rastm->shot = shotFromTrackball().first;
						float ratio = (float)rastm->currentPlane->image.height() / (float)rastm->shot.Intrinsics.ViewportPx[1];
						rastm->shot.Intrinsics.ViewportPx[0] = rastm->currentPlane->image.width();
						rastm->shot.Intrinsics.ViewportPx[1] = rastm->currentPlane->image.height();
						rastm->shot.Intrinsics.PixelSizeMm[1] /= ratio;
						rastm->shot.Intrinsics.PixelSizeMm[0] /= ratio;
						rastm->shot.Intrinsics.CenterPx[0] = (int)((float)rastm->shot.Intrinsics.ViewportPx[0] / 2.0);
						rastm->shot.Intrinsics.CenterPx[1] = (int)((float)rastm->shot.Intrinsics.ViewportPx[1] / 2.0);
					}
				}
				else
				{
					Logf(GLLogStream::WARNING, "Error saving %s", outfile.toLocal8Bit().constData());
				}
			}
			takeSnapTile = false;
			snapBuffer = QImage();

		}
	}
	update();
	glPopAttrib();
}



void GLArea::drawGradient()//設置背景顏色
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	/* glBegin(GL_TRIANGLE_STRIP);
	glColor(glas.backgroundTopColor);  	glVertex2f(-1, 1);
	glColor(glas.backgroundBotColor);	glVertex2f(-1,-1);
	glColor(glas.backgroundTopColor);	glVertex2f( 1, 1);
	glColor(glas.backgroundBotColor);	glVertex2f( 1,-1);
	glEnd();*/

	if (vd != topTag) {
		//glColor3f(0.3, 0.3, 0.3);
		glColor3ub(237, 237, 237);
		glBegin(GL_TRIANGLE_STRIP);
		glVertex2f(-1, 1);
		glVertex2f(-1, -1);
		glVertex2f(1, 1);
		glVertex2f(1, -1);
		glEnd();
	}
	else
	{
		glColor3f(1, 0, 0);
		glBegin(GL_LINE_LOOP);
		glVertex2f(-1, 1);
		glVertex2f(-1, -1);
		glVertex2f(1, -1);
		glVertex2f(1, 1);
		glEnd();
	}

	glPopAttrib();
	glPopMatrix(); // restore modelview
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void GLArea::drawLight()
{
	// ============== LIGHT TRACKBALL ==============
	// Apply the trackball for the light direction
	glPushMatrix();
	trackball_light.GetView();
	trackball_light.Apply();

	static float lightPosF[] = { 0.0, 0.0, 1.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosF);
	static float lightPosB[] = { 0.0, 0.0, -1.0, 0.0 };
	glLightfv(GL_LIGHT1, GL_POSITION, lightPosB);

	if (!(isDefaultTrackBall()))
	{
		glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
		glColor3f(1, 1, 0);
		glDisable(GL_LIGHTING);
		const unsigned int lineNum = 3;
		glBegin(GL_LINES);
		for (unsigned int i = 0; i <= lineNum; ++i)
		for (unsigned int j = 0; j <= lineNum; ++j) {
			glVertex3f(-1.0f + i*2.0 / lineNum, -1.0f + j*2.0 / lineNum, -2);
			glVertex3f(-1.0f + i*2.0 / lineNum, -1.0f + j*2.0 / lineNum, 2);
		}
		glEnd();
		glPopAttrib();
	}
	glPopMatrix();
	if (!isDefaultTrackBall())
		trackball_light.DrawPostApply();

}
int GLArea::RenderForSelection_bak(int pickX, int pickY)//render選取的範圍
{
	//int sz = int(md()->meshList.size()) * 5;
	int sz = int(md()->meshList.size()) * 5 + 5;
	GLuint *selectBuf = new GLuint[sz];
	glSelectBuffer(sz, selectBuf);
	glRenderMode(GL_SELECT);
	glInitNames();

	/* Because LoadName() won't work with no names on the stack */
	glPushName(-1);
	double mp[16];

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glPushAttrib(GL_TRANSFORM_BIT);
	glMatrixMode(GL_PROJECTION);
	glGetDoublev(GL_PROJECTION_MATRIX, mp);
	glPushMatrix();
	glLoadIdentity();
	gluPickMatrix(pickX, pickY, 4, 4, viewport);
	glMultMatrixd(mp);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	foreach(MeshModel * mp, this->md()->meshList)
	{
		//***20150904 不要選到outline
		//if (!(mp->label().contains("_temp_outlines", Qt::CaseSensitive) || mp->label().contains("_temp_ol_mesh", Qt::CaseSensitive)))
		//if (!(mp->label().contains(md()->p_setting.getOlMeshName(), Qt::CaseSensitive) || mp->label().contains(md()->p_setting.getCapMeshName(), Qt::CaseSensitive)))
		if (!md()->isSlice_item(*mp))
		{
			glLoadName(mp->id());
			/*QMap<int, RenderMode>::iterator it = rendermodemap.find(mp->id());
			if (it != rendermodemap.end())
			mp->render(it.value().drawMode, vcg::GLW::CMNone, vcg::GLW::TMNone);*/
			mp->render(vcg::GLW::DMSELECTION, vcg::GLW::CMNone, vcg::GLW::TMNone);
			//mp->render(vcg::GLW::DMSmooth, vcg::GLW::CMNone, vcg::GLW::TMNone);
		}
	}

	/*if (drawLists[0] > 0)
		glCallList(drawLists[0]);*/


	//***20160302***//
	//glLoadName(1000);
	//createqobj();
	//glutSolidSphere(10, 50, 50);
	//******//

	long hits;
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	hits = glRenderMode(GL_RENDER);
	glPopAttrib();

	std::vector< std::pair<double, unsigned int> > H;
	for (long ii = 0; ii < hits; ii++) {
		H.push_back(std::pair<double, unsigned int>(selectBuf[ii * 4 + 1] / 4294967295.0, selectBuf[ii * 4 + 3]));
		//4294967295.0 = 0xffffffff
		//***GLuint 32bits, unsigned int 16bits, long 32bits

	}
	std::sort(H.begin(), H.end());
	delete[] selectBuf;
	if (hits == 0) return -1;
	return H.front().second;
}
#if 1
int GLArea::RenderForSelection(int pickX, int pickY)//render選取的範圍
{

	int xxx = this->width();
	float x = (2.0f * pickX) / this->width() - 1.0f;
	float y = (2.0f * pickY) / this->height() - 1.0;
	float z = -1.0f;
	float z2 = 1.0f;
	trackball.GetView();
	//vcg::Point3f point(x, y, 0.1);
	vcg::Point4f ray_clip(x, y, z, 1.0);

	Matrix44m proj = trackball.camera.proj;
	Matrix44m modelm = trackball.camera.model;

	vcg::Point4f ray_eye = vcg::Inverse(proj) * ray_clip;
	Point4f temp = (Inverse(modelm) * ray_eye);
	vcg::Point3f orig_wor = Point3m(temp.X(), temp.Y(), temp.Z());
	//ray_wor.normalized();


	vcg::Point4f ray_clip2(x, y, z2, 1.0);
	vcg::Point4f ray_eye2 = vcg::Inverse(proj) * ray_clip2;
	Point4f temp2 = (Inverse(modelm) * ray_eye2);
	vcg::Point3f ray_wor2 = Point3m(temp2.X(), temp2.Y(), temp2.Z());
	//ray_wor2.normalized();
	Point3f dir = ray_wor2 - orig_wor;
	dir.normalized();


	/*GLdouble modelMatrix[16];
	GLdouble projMatrix[16];
	GLint viewport[4];
	GLdouble objx, objy, objz, objx2, objy2, objz2;

	glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
	glGetIntegerv(GL_VIEWPORT, viewport);

	gluUnProject(pickX, pickY, 0, modelMatrix, projMatrix, viewport, &objx, &objy, &objz);
	gluUnProject(pickX, pickY, 1.0, modelMatrix, projMatrix, viewport, &objx2, &objy2, &objz2);

	Point3m test(objx, objy, objz);
	Point3m test2(objx2, objy2, objz2);*/

	/*pre Test */
	QList<MeshModel *> record_bbox;
	foreach(MeshModel *mm, md()->meshList)
	{
		Box3f tmep = mm->cm.bbox;
		SKT::Ray ray(orig_wor, dir);
		if (SKT::testIntersection_cube(tmep, ray))
		{
			record_bbox.push_back(mm);
		}
	}
	/*ray and triangle*/
	int id = -1;
	float smalltt = 100000;
	foreach(MeshModel *mm, record_bbox)
	{
		CMeshO::FaceIterator fi;
		fi = mm->cm.face.begin();
		fi->SetS();
		for (; fi != mm->cm.face.end(); ++fi)
		{
			if (!(*fi).IsD() && (*fi).IsS() && !(*fi).IsV());
			Point3m v1 = fi->V(0)->P();
			Point3m v2 = fi->V(1)->P();
			Point3m v3 = fi->V(2)->P();
			float tt = -1;
			if (SKT::testIntersection(v1, v2, v3, orig_wor, dir, tt))
			{
				if ((tt) < smalltt)
				{
					id = mm->id();
					smalltt = tt;
					//camerDepth.insert(tt, id);
				}
				break;

			}
		}

	}


	return id;
}

#else

int GLArea::RenderForSelection(int pickX, int pickY)//render選取的範圍
{

	int xxx = this->width();
	float x = (2.0f * pickX) / this->width() - 1.0f;
	float y = (2.0f * pickY) / this->height() - 1.0;
	float z = -1.0f;
	float z2 = 1.0f;
	trackball.GetView();
	//vcg::Point3f point(x, y, 0.1);
	vcg::Point4f ray_clip(x, y, z, 1.0);

	Matrix44m proj = trackball.camera.proj;
	Matrix44m modelm = trackball.camera.model;

	vcg::Point4f ray_eye = vcg::Inverse(proj) * ray_clip;
	Point4f temp = (Inverse(modelm) * ray_eye);
	vcg::Point3f orig_wor = Point3m(temp.X(), temp.Y(), temp.Z());
	//ray_wor.normalized();


	vcg::Point4f ray_clip2(x, y, z2, 1.0);
	vcg::Point4f ray_eye2 = vcg::Inverse(proj) * ray_clip2;
	Point4f temp2 = (Inverse(modelm) * ray_eye2);
	vcg::Point3f ray_wor2 = Point3m(temp2.X(), temp2.Y(), temp2.Z());
	//ray_wor2.normalized();
	Point3f dir = ray_wor2 - orig_wor;
	dir.normalized();


	/*GLdouble modelMatrix[16];
	GLdouble projMatrix[16];
	GLint viewport[4];
	GLdouble objx, objy, objz, objx2, objy2, objz2;

	glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
	glGetIntegerv(GL_VIEWPORT, viewport);

	gluUnProject(pickX, pickY, 0, modelMatrix, projMatrix, viewport, &objx, &objy, &objz);
	gluUnProject(pickX, pickY, 1.0, modelMatrix, projMatrix, viewport, &objx2, &objy2, &objz2);

	Point3m test(objx, objy, objz);
	Point3m test2(objx2, objy2, objz2);*/

	/*pre Test */
	QList<MeshModel *> record_bbox;
	foreach(MeshModel *mm, md()->meshList)
	{
		Box3f tmep = mm->cm.bbox;
		SKT::Ray ray(orig_wor, dir);
		if (SKT::testIntersection_cube(tmep, ray))
		{
			record_bbox.push_back(mm);
		}
	}
	/*ray and triangle*/
	int id = -1;
	float smalltt = 100000;
	foreach(MeshModel *mm, record_bbox)
	{
		CMeshO::FaceIterator fi;
		fi = mm->cm.face.begin();
		fi->SetS();
		for (; fi != mm->cm.face.end(); ++fi)
		{
			if (!(*fi).IsD() && (*fi).IsS() && !(*fi).IsV());
			Point3m v1 = fi->V(0)->P();
			Point3m v2 = fi->V(1)->P();
			Point3m v3 = fi->V(2)->P();
			float tt = -1;

			CMeshO::VertexPointer vi = fi->V(0);
			Point3m normal = fi->N();
			vi->P() = v1 + normal * 2;
			vi = fi->V(1);
			vi->P() = v2 + normal * 2;
			vi = fi->V(2);
			vi->P() = v3 + normal * 2;
			// (*vi).P() = M*(*vi).cP();

			if (SKT::testIntersection(v1, v2, v3, orig_wor, dir, tt))
			{
				if ((tt) < smalltt)
				{
					id = mm->id();
					smalltt = tt;
					//camerDepth.insert(tt, id);
				}
				
				
				//break;

			}
			
		}

		mm->UpdateBoxAndNormals();
	}
	return id;
}
#endif
void GLArea::testSortObjectDepth()
{
	camerDepth.clear();

	int xxx = this->width();
	float x = 1;// (2.0f * pickX) / this->width() - 1.0f;
	float y = 1;// (2.0f * pickY) / this->height() - 1.0;
	float z = -1.0f;
	float z2 = 1.0f;
	trackball.GetView();
	//vcg::Point3f point(x, y, 0.1);
	vcg::Point4f ray_clip(x, y, z, 1.0);

	Matrix44m proj = trackball.camera.proj;
	Matrix44m modelm = trackball.camera.model;

	vcg::Point4f ray_eye = vcg::Inverse(proj) * ray_clip;
	Point4f temp = (Inverse(modelm) * ray_eye);
	vcg::Point3f orig_wor = Point3m(temp.X(), temp.Y(), temp.Z());
	//ray_wor.normalized();


	vcg::Point4f ray_clip2(x, y, z2, 1.0);
	vcg::Point4f ray_eye2 = vcg::Inverse(proj) * ray_clip2;
	Point4f temp2 = (Inverse(modelm) * ray_eye2);
	vcg::Point3f ray_wor2 = Point3m(temp2.X(), temp2.Y(), temp2.Z());
	//ray_wor2.normalized();
	Point3f dir = orig_wor - ray_wor2;
	dir.normalized();

	Plane3f clipPlane;
	clipPlane.Init(ray_wor2, dir);


	foreach(MeshModel *mm, md()->meshList)
	{
		float dis = vcg::SignedDistancePlanePoint(clipPlane, mm->cm.bbox.Center());
		camerDepth.insert(dis, mm->id());
	}







}
int GLArea::testGetSelection(int pickX, int pickY)
{
	int xxx = this->width();
	float x = (2.0f * pickX) / this->width() - 1.0f;
	float y = (2.0f * pickY) / this->height() - 1.0;
	float z = 1.0f;
	trackball.GetView();
	vcg::Point3f point(x, y, z);
	vcg::Point4f ray_clip(x, y, 1.0, 1.0);
	Matrix44m proj = trackball.camera.proj;
	Matrix44m modelm = trackball.camera.model;
	vcg::Point4f ray_eye = Inverse(proj) * ray_clip;

	vcg::Point3f ray_wor = Point3m((Inverse(modelm) * ray_eye).X(), (Inverse(modelm) * ray_eye).Y(), (Inverse(modelm) * ray_eye).Z());
	ray_wor.normalized();
	Point3m viewPoint = vcg::Inverse(modelm)* Point3m(0, 0, 0);
	//raywor.normalized();


	//double mp[16];

	//double res[3];
	//GLdouble mm[16], pm[16]; GLint vp[4];
	//glGetDoublev(GL_MODELVIEW_MATRIX, mm);
	//glGetDoublev(GL_PROJECTION_MATRIX, pm);
	//glGetIntegerv(GL_VIEWPORT, vp);
	//gluUnProject(pickX, pickY, 0.01, mm, pm, vp, &res[0], &res[1], &res[2]);//將window coordinate(x, y) 對應到object space,
	//
	//
	//Point3m pp = Point3m(Scalarm(res[0]), Scalarm(res[1]), Scalarm(res[2]));

	//Matrix44m original_Transform;
	////original_Transform.
	////vcg::Point4f rayeye = ;
	//trackball.GetView();
	//Point3m pickWPoint = trackball.camera.UnProject(Point3m(pickX, pickY, -1));
	//pickWPoint.normalized();
	//Point3m viewPoint = trackball.camera.ViewPoint();


	int id = -1;
	float smalltt = 5000;
	foreach(MeshModel *mm, md()->meshList)
	{
		CMeshO::FaceIterator fi;
		fi = mm->cm.face.begin();
		fi->SetS();
		for (; fi != mm->cm.face.end(); ++fi)
		{
			if (!(*fi).IsD() && (*fi).IsS() && !(*fi).IsV());
			Point3m v1 = fi->V(0)->P();
			Point3m v2 = fi->V(1)->P();
			Point3m v3 = fi->V(2)->P();
			float tt = -1;
			if (SKT::testIntersection(v1, v2, v3, viewPoint, ray_wor, tt))
			{
				if (tt < smalltt)
				{
					id = mm->id();
					smalltt = tt;
				}
				break;

			}
		}

	}

	return id;




	//return 0;
}
//void GLArea::paintEvent(QPaintEvent* /*event*/)//主要paintGL
//{
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
//	glClearColor(1, 1, 1, 1);
//	glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
//	glOrtho(-11.5, 11.5, -11.5, 11.5, -100, 100);
//	glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, 20, 0, 0, 0, 0, 1, 0);
//
//	setGrooveView();
//	mvc()->updateAllViewer();
//}

void  GLArea::updateGLList()
{
	//	this->makeCurrent();
	//	glDeleteLists(drawList, 2);
	//	drawList = glGenLists(2);
	//	if (md()->meshList.size() > 0 && drawList > 0)
	//	{
	//#pragma region list1
	//		//glNewList(drawList, GL_COMPILE);
	//
	//		//foreach(MeshModel * mp, this->md()->meshList)
	//		//{
	//		//	//***20150512 傳入viewerID
	//		//	mp->glw.viewerNum = getId();
	//		//	QMap<int, RenderMode>::iterator it = rendermodemap.find(mp->id());
	//		//	if (it != rendermodemap.end())
	//		//	{
	//		//		RenderMode rm = it.value();
	//		//		setLightModel(rm);
	//
	//		//		if (mp->rmm.colorMode != GLW::CMNone || mp->getMeshSort() == MeshModel::meshsort::decorate_item)
	//		//		{
	//		//			glEnable(GL_COLOR_MATERIAL);
	//		//			glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	//		//		}
	//		//		else
	//		//		{
	//		//			glEnable(GL_COLOR_MATERIAL);
	//		//			glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	//		//			glColor(Color4b::White);
	//		//		}
	//
	//
	//		//		if (rm.backFaceCull)
	//		//			glEnable(GL_CULL_FACE);
	//		//		else
	//		//			glDisable(GL_CULL_FACE);
	//		//		mp->glw.SetHintParamf(GLW::HNPPointSize, glas.pointSize);
	//		//		mp->glw.SetHintParami(GLW::HNPPointDistanceAttenuation, glas.pointDistanceAttenuation ? 1 : 0);
	//		//		mp->glw.SetHintParami(GLW::HNPPointSmooth, glas.pointSmooth ? 1 : 0);
	//
	//
	//
	//		//		if (meshVisibilityMap[mp->id()] && mp->getMeshSort() == MeshModel::meshsort::print_item /*&& drawMeshFlag == true*/)
	//		//		{
	//		//			if (!md()->renderState().isEntityInRenderingState(id, MeshLabRenderState::MESH))
	//		//			{
	//
	//		//				//***20151001***點太多要改drawMode
	//		//				//if (mp->cm.VertexNumber() > mw()->getvtNumberLimit())								
	//		//				if (mw()->getMdNumLimit() || mw()->getpointCloudSwitch())							
	//		//				{							
	//		//					mp->render(GLW::DMPoints, mp->rmm.colorMode, mp->rmm.textureMode);						
	//		//				}
	//		//				else 
	//		//				{
	//
	//		//					mp->render(GLW::DMFlat, mp->rmm.colorMode, mp->rmm.textureMode);//currentused
	//
	//		//				}
	//		//				
	//		//			}
	//		//			else
	//		//			{
	//		//				md()->renderState().render(mp->id(), rm.drawMode, rm.colorMode, rm.textureMode);
	//
	//		//			}
	//
	//		//			QList<QAction *>& tmpset = iPerMeshDecoratorsListMap[mp->id()];
	//
	//		//		}
	//		//		else
	//		//		{
	//
	//		//			if (mp->getMeshSort() == MeshModel::meshsort::slice_item)
	//		//			{
	//		//				mp->render(rm.drawMode, GLW::CMBlack, mp->rmm.textureMode);//backup					
	//		//			}
	//
	//		//			if (mp->label().contains(md()->p_setting.getCapMeshName(), Qt::CaseSensitive))
	//		//			{
	//		//				mp->render(GLW::DMCapEdge, GLW::CMNone, GLW::TMNone);
	//
	//		//			}
	//		//			if (drawMeshFlag == false && mp->getMeshSort() == MeshModel::meshsort::print_item)
	//		//			{
	//		//				glBoxWire_SKT2(mp->cm.bbox, vcg::Color4f(0.2, 0.2, 0.2, 0), false);
	//		//			}
	//
	//
	//		//		}
	//		//		//if (mp->getMeshSort() == MeshModel::meshsort::decorate_item /*&& this->id == 2*/)
	//		//		//	mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
	//		//	}
	//		//}
	//		//glEndList();
	//#pragma endregion list1
	//#pragma region listTest2
	//		glNewList(drawList, GL_COMPILE);
	//
	//		foreach(MeshModel * mp, this->md()->meshList)
	//		{
	//			mp->glw.viewerNum = getId();
	//
	//			glEnable(GL_LIGHTING);
	//			glDisable(GL_LIGHT1);
	//
	//			glLightfv(GL_LIGHT0, GL_AMBIENT, Color4f::Construct(glas.baseLightAmbientColor).V());
	//			glLightfv(GL_LIGHT0, GL_DIFFUSE, Color4f::Construct(glas.baseLightDiffuseColor).V());
	//			glLightfv(GL_LIGHT0, GL_SPECULAR, Color4f::Construct(glas.baseLightSpecularColor).V());
	//
	//
	//			if (mp->rmm.colorMode != GLW::CMNone || mp->getMeshSort() == MeshModel::meshsort::decorate_item)
	//			{
	//				glEnable(GL_COLOR_MATERIAL);
	//				glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	//			}
	//			else
	//			{
	//				glEnable(GL_COLOR_MATERIAL);
	//				glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	//				glColor(Color4b::White);
	//			}
	//
	//
	//			//if (rm.backFaceCull)
	//				glEnable(GL_CULL_FACE);
	//			/*else
	//				glDisable(GL_CULL_FACE);*/
	//			mp->glw.SetHintParamf(GLW::HNPPointSize, glas.pointSize);
	//			mp->glw.SetHintParami(GLW::HNPPointDistanceAttenuation, glas.pointDistanceAttenuation ? 1 : 0);
	//			mp->glw.SetHintParami(GLW::HNPPointSmooth, glas.pointSmooth ? 1 : 0);
	//
	//
	//
	//			if (meshVisibilityMap[mp->id()] && mp->getMeshSort() == MeshModel::meshsort::print_item /*&& drawMeshFlag == true*/)
	//			{
	//				glLoadName(mp->id());
	//				if (!md()->renderState().isEntityInRenderingState(id, MeshLabRenderState::MESH))
	//				{
	//
	//					//***20151001***點太多要改drawMode
	//					//if (mp->cm.VertexNumber() > mw()->getvtNumberLimit())								
	//					if (mw()->getMdNumLimit() || mw()->getpointCloudSwitch())
	//					{
	//						mp->render(GLW::DMPoints, mp->rmm.colorMode, mp->rmm.textureMode);
	//					}
	//					else
	//					{
	//
	//						mp->render(GLW::DMFlat, mp->rmm.colorMode, mp->rmm.textureMode);//currentused
	//
	//					}
	//
	//				}
	//				/*else
	//				{
	//					md()->renderState().render(mp->id(), rm.drawMode, rm.colorMode, rm.textureMode);
	//
	//				}*/
	//
	//				//QList<QAction *>& tmpset = iPerMeshDecoratorsListMap[mp->id()];
	//
	//			}
	//			/*else
	//			{
	//
	//				if (mp->getMeshSort() == MeshModel::meshsort::slice_item)
	//				{
	//					mp->render(rm.drawMode, GLW::CMBlack, mp->rmm.textureMode);//backup					
	//				}
	//
	//				if (mp->label().contains(md()->p_setting.getCapMeshName(), Qt::CaseSensitive))
	//				{
	//					mp->render(GLW::DMCapEdge, GLW::CMNone, GLW::TMNone);
	//
	//				}
	//				if (drawMeshFlag == false && mp->getMeshSort() == MeshModel::meshsort::print_item)
	//				{
	//					glBoxWire_SKT2(mp->cm.bbox, vcg::Color4f(0.2, 0.2, 0.2, 0), false);
	//				}
	//
	//
	//			}*/
	//			//if (mp->getMeshSort() == MeshModel::meshsort::decorate_item /*&& this->id == 2*/)
	//			//	mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
	//
	//		}
	//		glEndList();
	//#pragma endregion listTest2
	//
	//
	//		//glNewList(drawList + 1, GL_COMPILE);
	//		//foreach(MeshModel * mp, this->md()->meshList)
	//		//{
	//		//	if (!md()->isSlice_item(*mp))
	//		//	{
	//		//		glLoadName(mp->id());
	//		//		//QMap<int, RenderMode>::iterator it = rendermodemap.find(mp->id());
	//		//		//if (it != rendermodemap.end())
	//		//		//	//mp->render(it.value().drawMode, vcg::GLW::CMNone, vcg::GLW::TMNone);
	//		//		mp->render(GLW::DMFlat, vcg::GLW::CMNone, vcg::GLW::TMNone);
	//		//	}
	//		//}
	//		//glEndList();
	//
	//		drawLists.at(0) = drawList;
	//		drawLists.at(1) = drawList + 1;
	//	}
	//
}
void GLArea::paintEvent(QPaintEvent* /*event*/)//主要paintGL
{

	/*static GLfloat fPitch = 0.0;
	if (bSpin)
	{
	fPitch += 3.0;

	}*/
	QTime time;
	time.start();
	if (mvc() == NULL) return;
	QPainter painter(this);

	painter.beginNativePainting();
	makeCurrent();

	if (!isValid()) return;

	/*QTime time;
	time.start();*/

	if (!this->md()->isBusy())
	{
		if (hasToUpdateTexture)//不需要每個frame都執行init texture
			//if (true)
			initTexture(hasToUpdateTexture);
		/*if (hasToRefreshGlList)
			updateGLList();*/
		//hasToRefreshGlList = false;
		hasToUpdateTexture = false;

		if (hasToRefreshVBO)
		{
			refreshVBO();
			hasToRefreshVBO = false;
		}
		if (hasToUpdatePartVBO)
		{
			updatePartVBO(VBOupdatePart[0], VBOupdatePart[1], VBOupdatePart[2]);
			VBOupdatePart[0] = true;
			VBOupdatePart[1] = false;
			VBOupdatePart[2] = false;
			hasToUpdatePartVBO = false;
		}



	}

	glClearColor(1.0, 1.0, 1.0, 0.0);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	setView();  // Set Modelview and Projection matrix設置modelView 與 projectionView
	if (vd != topTag)
	{

		if ((!takeSnapTile) || (takeSnapTile && !ss.transparentBackground))
			drawGradient();  // draws the background

		drawLight();


		glPushMatrix();
		//glRotatef(-fPitch, 0.0, 1.0, 0.0);
		// Finally apply the Trackball for the model	
		trackball.GetView();
		trackball.Apply();
		//***20150409//改由mousemove func鎖住
		//if(vd==top || vd==front)trackball.current_mode = NULL;//trackball 鎖住
		glPushMatrix();

		//grid();
		//createqobj();
		//glutSolidSphere(10, 50, 50);
		//md()->p_setting.gd_unit;
		grid3(md()->p_setting.gd_unit);
		vcg::glBoxWire_SKT3(md()->groove, md()->groove.getGrooveHit());//***20160720
		/*if (md()->pointsInFrontOfMesh.size() > 0)
			drawPointsfromGrayImage();*/
		//****
		//*********

		//glScale(d);
		//	glTranslate(-FullBBox.Center());
		//setLightModel();

		// Set proper colorMode
		/*if(rm.colorMode != GLW::CMNone)
		{
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
		}
		else glColor(Color4b::LightGray);*/

		/*if(rm.backFaceCull) glEnable(GL_CULL_FACE);
		else glDisable(GL_CULL_FACE);*/


		if (!this->md()->isBusy())//md為meshdocument
		{
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			if (iRenderer)//iRenderer為meshrenderInterface
			{
				//qDebug() << "render shader ";
				// virtual void Render(QAction *, MeshDocument &, QMap<int,RenderMode>&, QGLWidget *) = 0;
				//****20150504註解掉
				iRenderer->Render(currentShader, *this->md(), rendermodemap, this);
				/*foreach(MeshModel * mp, this->md()->meshList)
				{
				if (mp->visible)
				{
				QList<QAction *>& tmpset = iPerMeshDecoratorsListMap[mp->id()];
				for( QList<QAction *>::iterator it = tmpset.begin(); it != tmpset.end();++it)
				{
				MeshDecorateInterface * decorInterface = qobject_cast<MeshDecorateInterface *>((*it)->parent());
				decorInterface->decorateMesh(*it,*mp,this->glas.currentGlobalParamSet, this,&painter,md()->Log);
				}
				}
				}*/
			}
			else
			{
				if (hasToRefreshDepth && mw()->getRadarModeSwitch())
				{
					testSortObjectDepth();
					hasToRefreshDepth = false;
				}


				if (hasToSelect /*&& selectionSwitch*/)
				{
					QTime time2;
					time2.start();
					int newId = RenderForSelection(pointToPick[0], pointToPick[1]);
					//int newId2 = testGetSelection(pointToPick[0], pointToPick[1]);
					tempselectionms = time2.elapsed();
					//qDebug() <<"time2:"<< time2.elapsed();
					//qDebug()<<"RenderForSelection"<<time.elapsed();
					getPickId = newId;

					if (newId >= 0)
					{
						if (!hasToMultiSelect)
							md()->multiSelectID.clear();

						if (md()->multiSelectID.contains(newId))
						{
							md()->multiSelectID.remove(newId);
							//emit this->updateMainWindowMenus();
						}
						else
						{
							//Logf(0, "Selected new Mesh %i", newId);
							md()->multiSelectID.insert(newId);
							md()->setCurrentMesh(newId);
						}

						//***20150413畫BBox
						//glBoxWire(md()->getMesh(newId)->cm.bbox);
						//***

						//***transformwidgett1***refresh 
						//if (md()->multiSelectID.size() > 0)
						if (md()->mm() != NULL)
						{
							//qDebug() << "md()->meshList.size()" << md()->meshList.size();
							mw()->transformwidgett1->loadParameter();
							mw()->scale_widget_cus->loadParameter();
							mw()->rotate_widget_cus->loadParameter();
						}

						mw()->objList->updateTable();
						update();
					}

					else
					{
						/*md()->multiSelectID.clear();
						multi_pointToPick.clear();					*/
						//md()->setCurrentMesh(-1);
						//multi_pointToPick.clear();
					}
					//openTPreview();//***20160322
					//qDebug("multiSelectID %i", md()->multiSelectID.size());
					//***20150413
					hasToSelect = false;

					//***20160420*multi_select**************************************************************					
					/*if ((hasToMultiSelect &&newId>0))
					md()->multiSelectID.push_back(RenderForSelection(pointToPick[0], pointToPick[1]));*/
					hasToMultiSelect = false;
				}
				//else
				/*QMapIterator<double, int> rM(camerDepth);
				rM.toBack();
				while (rM.hasPrevious()) {
				i.previous();
				qDebug() << i.key() << ": " << i.value();
				}*/


				if (mw()->getRadarModeSwitch())
				{

					foreach(int id, camerDepth)
					{
						MeshModel *mp = md()->getMesh(id);
						QMap<int, RenderMode>::iterator it = rendermodemap.find(mp->id());
						if (it != rendermodemap.end())
						{
							RenderMode rm = it.value();
							setLightModel(rm);
						}
						glEnable(GL_COLOR_MATERIAL);
						glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

						if (/*meshVisibilityMap[mp->id()] &&*/ (mp->getMeshSort() == MeshModel::meshsort::print_item || mp->getMeshSort() == MeshModel::meshsort::pre_print_item) /*&& drawMeshFlag == true*/)
						{
							if (!md()->renderState().isEntityInRenderingState(id, MeshLabRenderState::MESH))
							{
								if (mw()->getMdNumLimit() || mw()->getpointCloudSwitch())
								{
									mp->render(GLW::DMPoints, mp->rmm.colorMode, mp->rmm.textureMode);
								}
								else
								{
									/*test radar*/
									//float ZTWIST = 0.00005;

									glEnable(GL_BLEND);
									glDisable(GL_CULL_FACE);
									glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
									glDepthMask(0);
									//glDepthRange(ZTWIST, 1.0f);
									glDepthRange(0, 1.0f);

									//glColor4f(0.2f, 1.0f, 0.4f, 0.2f);								
									glColor4f(sortDepthColor[id % 5].X(), sortDepthColor[id % 5].Y(), sortDepthColor[id % 5].Z(), 0.2);

									mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);

									glDepthMask(1);
									/*glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
									mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);*/

									//glDepthRange(0.0f, 1.0f - ZTWIST);
									////glDepthRange(0.0f, 1.0f - ZTWIST);
									glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);



									//glColor4f(0.1f, 1.0f, 0.2f, 0.6f);
									//mp->render(GLW::DMWire, GLW::CMNone, GLW::TMNone);//currentused		
									////Draw<DMWire, CMNone, TMNone>();
									//glDisable(GL_BLEND);
									//glDepthRange(0, 1.0f);


									if (md()->multiSelectID.contains(id))
										glBoxWire_SKT2(md()->getMesh(id)->cm.bbox, vcg::Color4f(1, 0, 1, 0.4), false, 1);
									glEnable(GL_CULL_FACE);
									glDisable(GL_BLEND);
								}

							}
						}
					}
					glEnable(GL_BLEND);

					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

					if (md()->multiSelectID.size() >= 1)
						glBoxWire_SKT2(md()->selBBox(), vcg::Color4f(0.5, 0, 0.5, 0.4), true);
					glDisable(GL_BLEND);


				}
				else{
					foreach(MeshModel * mp, this->md()->meshList)
					{

						QMap<int, RenderMode>::iterator it = rendermodemap.find(mp->id());
						if (it != rendermodemap.end())
						{
							RenderMode rm = it.value();
							setLightModel(rm);
						}
						//////////////////////////////////////////////
						//***20150512 傳入viewerID

						//glDisable(GL_LIGHTING);
						/*if (rm.colorMode != GLW::CMNone || mp->getMeshSort() == MeshModel::meshsort::decorate_item)
						{
						glEnable(GL_COLOR_MATERIAL);
						glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
						}*/
						if (mp->rmm.colorMode != GLW::CMNone || mp->getMeshSort() == MeshModel::meshsort::decorate_item)
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
						glEnable(GL_CULL_FACE);

						//Mesh visibility is read from the viewer visibility map, not from the mesh
						mp->glw.SetHintParamf(GLW::HNPPointSize, glas.pointSize);
						mp->glw.SetHintParami(GLW::HNPPointDistanceAttenuation, glas.pointDistanceAttenuation ? 1 : 0);
						mp->glw.SetHintParami(GLW::HNPPointSmooth, glas.pointSmooth ? 1 : 0);
						if (/*meshVisibilityMap[mp->id()] &&*/ (mp->getMeshSort() == MeshModel::meshsort::print_item || mp->getMeshSort() == MeshModel::meshsort::pre_print_item) /*&& drawMeshFlag == true*/)
						{
							if (!md()->renderState().isEntityInRenderingState(id, MeshLabRenderState::MESH))
							{
								if (mw()->getMdNumLimit() || mw()->getpointCloudSwitch())
								{
									mp->render(GLW::DMPoints, mp->rmm.colorMode, mp->rmm.textureMode);

								}
								else if (mp->glw.curr_hints &vcg::GLW::Hint::HNUseVBO)
								{
									//char* numpages = new char[20];
									////string temp = std::to_string(mp->id()).append("_meshID");
									//string temp = std::to_string(mp->glw.array_buffers[0]) + "," + std::to_string(mp->glw.array_buffers[1]) + "," + std::to_string(mp->glw.array_buffers[2]).append("_meshID");
									//strcpy(numpages, temp.c_str());
									//wchar_t *wmsg = new wchar_t[strlen(numpages) + 1]; //memory allocation
									//mbstowcs(wmsg, numpages, strlen(numpages) + 1);
									//
									//WRITELOG(md()->logger, framework::Diagnostics::LogLevel::Info, wmsg);
									//delete[]wmsg;
									//delete[]numpages;

									//mp->render(vcg::GLW::DMSmooth, vcg::GLW::CMNone, vcg::GLW::TMNone);
									//mp->render(GLW::DMSmooth, mp->rmm.colorMode, mp->rmm.textureMode);//currentused	
									mp->render(GLW::DMFlat, mp->rmm.colorMode, mp->rmm.textureMode);//currentused		
									//mp->render(GLW::DMRadar, mp->rmm.colorMode, mp->rmm.textureMode);//currentused	

									//WRITELOG(md()->logger, framework::Diagnostics::LogLevel::Info, _T("mp->rendervbo"));

								}
								else if (1)
								{
									mp->render(GLW::DMFlat, mp->rmm.colorMode, mp->rmm.textureMode);//currentused		
									//mp->render(GLW::DMRadar, mp->rmm.colorMode, mp->rmm.textureMode);//currentused	
								}
								else
								{
									////draw opaque object ...
									//glDepthMask(GL_FALSE);
									//glEnable(GL_BLEND);
									//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
									//glutSolidCube(3);
									////draw transparent object ...
									//glDisable(GL_BLEND);
									//glDepthMask(GL_TRUE);

									///*glPushAttrib(GL_ALL_ATTRIB_BITS);
									//glMatrixMode(GL_MODELVIEW);
									//glPushMatrix();
									//glBindBuffer((*it)->_target, (*it)->_bohandle);
									//setBufferPointer(boname);
									//glEnableClientState((*it)->_clientstatetag);
									//glBindBuffer((*it)->_target, 0);
									//glDisable(GL_TEXTURE_2D);
									//glDrawArrays(GL_TRIANGLES, 0, mp->cm.fn * 3);
									//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
									//glBindBuffer(GL_ARRAY_BUFFER, 0);
									//glPopMatrix();
									//glPopAttrib();*/

									/*test radar*/
									//float ZTWIST = 0.00005;
									//glEnable(GL_BLEND);
									//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
									//glDepthMask(0);
									//glDepthRange(ZTWIST, 1.0f);
									//

									//glColor4f(0.2f, 1.0f, 0.4f, 0.2f);								
									////glColor4f(sortDepthColor[sortdepth % 5].X(), sortDepthColor[sortdepth % 5].Y(), sortDepthColor[sortdepth % 5].Z(), 0.2f);

									//mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);

									/*glDepthMask(1);
									glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
									mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);*/

									//glDepthRange(0.0f, 1.0f - ZTWIST);
									////glDepthRange(0.0f, 1.0f - ZTWIST);
									//glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
									//glDisable(GL_BLEND);
									//glColor4f(0.1f, 1.0f, 0.2f, 0.6f);
									//mp->render(GLW::DMWire, GLW::CMNone, GLW::TMNone);							



									//glDisable(GL_BLEND);






								}
							}
							else
							{
								//md()->renderState().render(mp->id(), rm.drawMode, rm.colorMode, rm.textureMode);
								//qDebug() << "paint_place2" << rm.textureMode;
							}
							QList<QAction *>& tmpset = iPerMeshDecoratorsListMap[mp->id()];
							for (QList<QAction *>::iterator it = tmpset.begin(); it != tmpset.end(); ++it)
							{
								MeshDecorateInterface * decorInterface = qobject_cast<MeshDecorateInterface *>((*it)->parent());
								decorInterface->decorateMesh(*it, *mp, this->glas.currentGlobalParamSet, this, &painter, md()->Log);
								//***201507023***							
								/*QAction *temp = qobject_cast<QAction *>(*it);
								if (temp->text() == "Show Axis")
								decorInterface->decorateMesh(temp, *mp, this->glas.currentGlobalParamSet, this, &painter, md()->Log);*/
							}
						}
						else
						{
							//if (mp->label().contains(md()->p_setting.getOlMeshName(), Qt::CaseSensitive))
							if (mp->getMeshSort() == MeshModel::meshsort::slice_item)
							{
								//mp->render(GLW::DMTexWire, GLW::CMNone, GLW::TMNone);//backup
								//***20151113_tesst
								//mp->render(rm.drawMode, mp->rmm.colorMode, mp->rmm.textureMode);//backup

								//mp->render(rm.drawMode, GLW::CMBlack, mp->rmm.textureMode);//backup
								mp->render(GLW::DMTexWire, GLW::CMBlack, mp->rmm.textureMode);//backup
								//mp->render(GLW::DMOutline_test_triangle, GLW::CMNone, GLW::TMPerVert);
							}
							//mp->render(rm.drawMode, rm.colorMode, rm.textureMode);
							if (mp->label().contains(md()->p_setting.getCapMeshName(), Qt::CaseSensitive))
							{
								mp->render(GLW::DMCapEdge, GLW::CMNone, GLW::TMNone);
								//mp->render(GLW::DMCapEdge, GLW::CMNone, rm.textureMode);
							}
							if (drawMeshFlag == false && mp->getMeshSort() == MeshModel::meshsort::print_item)
							{
								glBoxWire_SKT2(mp->cm.bbox, vcg::Color4f(0.2, 0.2, 0.2, 0), false);
							}
							//qDebug() << "glArea_test";
						}
						if (mp->getMeshSort() == MeshModel::meshsort::decorate_item /*&& this->id == 2*/)
							mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);

						//qDebug() << "getId()=" << getId();
						//if (md()->hitSm() != NULL)
						//if (mp->gethitGroove())vcg::glBoxWire_SKT(mp->cm.bbox, mp->gethitGroove());//***20151014***test_hidden



						////////////////////////////////////////////

						////***20150512 傳入viewerID
						//mp->glw.viewerNum = getId();
						//QMap<int, RenderMode>::iterator it = rendermodemap.find(mp->id());
						//if (it != rendermodemap.end())
						//{
						//	RenderMode rm = it.value();
						//	setLightModel(rm);
						//	//glDisable(GL_LIGHTING);
						//	/*if (rm.colorMode != GLW::CMNone || mp->getMeshSort() == MeshModel::meshsort::decorate_item)
						//	{
						//	glEnable(GL_COLOR_MATERIAL);
						//	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
						//	}*/
						//	if (mp->rmm.colorMode != GLW::CMNone || mp->getMeshSort() == MeshModel::meshsort::decorate_item)
						//	{
						//		glEnable(GL_COLOR_MATERIAL);
						//		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
						//	}
						//	else
						//	{
						//		glEnable(GL_COLOR_MATERIAL);
						//		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
						//		glColor(Color4b::White);
						//	}
						//	if (rm.backFaceCull)
						//		glEnable(GL_CULL_FACE);
						//	else
						//		glDisable(GL_CULL_FACE);
						//	//Mesh visibility is read from the viewer visibility map, not from the mesh
						//	mp->glw.SetHintParamf(GLW::HNPPointSize, glas.pointSize);
						//	mp->glw.SetHintParami(GLW::HNPPointDistanceAttenuation, glas.pointDistanceAttenuation ? 1 : 0);
						//	mp->glw.SetHintParami(GLW::HNPPointSmooth, glas.pointSmooth ? 1 : 0);
						//	/*if (mm() != NULL &&
						//	(!(mm()->label().contains(md()->p_setting.getOlMeshName(), Qt::CaseSensitive) || mm()->label().contains(md()->p_setting.getCapMeshName(), Qt::CaseSensitive))))*/
						//	//if (meshVisibilityMap[mp->id()] && (!(mp->label().contains(md()->p_setting.getOlMeshName(), Qt::CaseSensitive) || mp->label().contains(md()->p_setting.getCapMeshName(), Qt::CaseSensitive))))
						//	if (meshVisibilityMap[mp->id()] && (mp->getMeshSort() == MeshModel::meshsort::print_item || mp->getMeshSort() == MeshModel::meshsort::pre_print_item) && drawMeshFlag == true)
						//	{
						//		if (!md()->renderState().isEntityInRenderingState(id, MeshLabRenderState::MESH))
						//		{
						//			//mp->render(rm.drawMode, rm.colorMode, rm.textureMode);
						//			//***20151001***點太多要改drawMode
						//			//if (mp->cm.VertexNumber() > mw()->getvtNumberLimit())								
						//			if (mw()->getMdNumLimit() || mw()->getpointCloudSwitch())
						//				//mp->render(GLW::DMPoints, rm.colorMode, rm.textureMode);//backup
						//			{
						//				//m_program->bind();
						//				mp->render(GLW::DMPoints, mp->rmm.colorMode, mp->rmm.textureMode);
						//				//m_program->release();
						//			}
						//			else if (1)
						//			{
						//				//rm.setColorMode(GLW::CMPerFace);
						//				//mp->render(rm.drawMode, rm.colorMode, rm.textureMode);//backup
						//				mp->render(GLW::DMFlat, mp->rmm.colorMode, mp->rmm.textureMode);//currentused
						//				//mp->render(GLW::DMFlat, GLW::CMNone, GLW::TMNone);//currentused
						//				//mp->render(GLW::DMFlat, mp->rmm.colorMode, mp->rmm.textureMode);									
						//				
						//			}
						//			else
						//			{	
						//				//draw opaque object ...
						//				glDepthMask(GL_FALSE);
						//				glEnable(GL_BLEND);
						//				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
						//				glutSolidCube(3);
						//				//draw transparent object ...
						//				glDisable(GL_BLEND);
						//				glDepthMask(GL_TRUE);
						//				/*glPushAttrib(GL_ALL_ATTRIB_BITS);
						//				glMatrixMode(GL_MODELVIEW);
						//				glPushMatrix();
						//				glBindBuffer((*it)->_target, (*it)->_bohandle);
						//				setBufferPointer(boname);
						//				glEnableClientState((*it)->_clientstatetag);
						//				glBindBuffer((*it)->_target, 0);
						//				glDisable(GL_TEXTURE_2D);
						//				glDrawArrays(GL_TRIANGLES, 0, mp->cm.fn * 3);
						//				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
						//				glBindBuffer(GL_ARRAY_BUFFER, 0);
						//				glPopMatrix();
						//				glPopAttrib();*/
						//			}
						//			//***20150402要改**已解決
						//			//mp->render(rm.drawMode, rm.colorMode, vcg::GLW::TextureMode::TMPerWedgeMulti);
						//			//qDebug()<<"paint_place1" << rm.textureMode;
						//		}
						//		else
						//		{
						//			md()->renderState().render(mp->id(), rm.drawMode, rm.colorMode, rm.textureMode);
						//			//qDebug() << "paint_place2" << rm.textureMode;
						//		}
						//		QList<QAction *>& tmpset = iPerMeshDecoratorsListMap[mp->id()];
						//		for (QList<QAction *>::iterator it = tmpset.begin(); it != tmpset.end(); ++it)
						//		{
						//			MeshDecorateInterface * decorInterface = qobject_cast<MeshDecorateInterface *>((*it)->parent());
						//			decorInterface->decorateMesh(*it, *mp, this->glas.currentGlobalParamSet, this, &painter, md()->Log);
						//			//***201507023***							
						//			/*QAction *temp = qobject_cast<QAction *>(*it);
						//			if (temp->text() == "Show Axis")
						//			decorInterface->decorateMesh(temp, *mp, this->glas.currentGlobalParamSet, this, &painter, md()->Log);*/
						//		}
						//	}
						//	else
						//	{
						//		//if (mp->label().contains(md()->p_setting.getOlMeshName(), Qt::CaseSensitive))
						//		if (mp->getMeshSort() == MeshModel::meshsort::slice_item)
						//		{
						//			//mp->render(GLW::DMTexWire, GLW::CMNone, GLW::TMNone);//backup
						//			//***20151113_tesst
						//			//mp->render(rm.drawMode, mp->rmm.colorMode, mp->rmm.textureMode);//backup
						//			mp->render(rm.drawMode, GLW::CMBlack, mp->rmm.textureMode);//backup
						//			//mp->render(GLW::DMOutline_test_triangle, GLW::CMNone, GLW::TMPerVert);
						//		}
						//		//mp->render(rm.drawMode, rm.colorMode, rm.textureMode);
						//		if (mp->label().contains(md()->p_setting.getCapMeshName(), Qt::CaseSensitive))
						//		{
						//			mp->render(GLW::DMCapEdge, GLW::CMNone, GLW::TMNone);
						//			//mp->render(GLW::DMCapEdge, GLW::CMNone, rm.textureMode);
						//		}
						//		if (drawMeshFlag == false && mp->getMeshSort() == MeshModel::meshsort::print_item)
						//		{
						//			glBoxWire_SKT2(mp->cm.bbox, vcg::Color4f(0.2, 0.2, 0.2, 0), false);
						//		}
						//		//qDebug() << "glArea_test";
						//	}
						//	if (mp->getMeshSort() == MeshModel::meshsort::decorate_item /*&& this->id == 2*/)
						//		mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
						//}
						////qDebug() << "getId()=" << getId();
						////if (md()->hitSm() != NULL)
						////if (mp->gethitGroove())vcg::glBoxWire_SKT(mp->cm.bbox, mp->gethitGroove());//***20151014***test_hidden
					}
					/*if (drawLists[0] > 0)
						glCallList(drawLists[0]);*/
					//qDebug() << "mp->render(GLW::DMFlat)" << time.elapsed();
					//if (vboid)
					//{

					//	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vboid);

					//	// enable vertex arrays
					//	glEnableClientState(GL_VERTEX_ARRAY);
					//	glEnableClientState(GL_NORMAL_ARRAY);


					//	// before draw, specify vertex and index arrays with their offsets
					//	//glVertexPointer(3, GL_FLOAT, 0, 0);

					//	glNormalPointer(GL_FLOAT, 0, (void*)sizeof(mw()->vbonormalptr));
					//	glVertexPointer(3, GL_FLOAT, 0, 0);

					//	/*glVertexPointer(3, GL_FLOAT, 0, mw()->vbodataptr);
					//	glNormalPointer(GL_FLOAT, 0, mw()->vbonormalptr);*/

					//	glDrawArrays(GL_TRIANGLES, 0, 36);

					//	glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
					//	glDisableClientState(GL_COLOR_ARRAY);
					//	glDisableClientState(GL_NORMAL_ARRAY);

					//	// it is good idea to release VBOs with ID 0 after use.
					//	// Once bound with 0, all pointers in gl*Pointer() behave as real
					//	// pointer, so, normal vertex array operations are re-activated
					//	//glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

					//}
					//qDebug() <<"getId()=" <<getId();

					//***20150511
					/*glPushAttrib(GL_ALL_ATTRIB_BITS);
					glEnable(GL_TEXTURE_2D);
					glDisable(GL_LIGHTING);
					glPushMatrix();
					glBindTexture(GL_TEXTURE_2D, mesh_texture[0]);
					glBegin(GL_QUADS);
					glTexCoord2f(0, 0); glVertex3f(-5, -5, 0);
					glTexCoord2f(1, 0 ); glVertex3f(5, -5, 0);
					glTexCoord2f(1, 1); glVertex3f(5, 5, 0);
					glTexCoord2f(0, 1); glVertex3f(-5, 5, 0);
					glEnd();
					glPopMatrix();
					glPopAttrib();*/
					/*testdrawtexture(-5, 8, 2, 0);
					testdrawtexture(-5, 6, 2, 1);
					testdrawtexture(-5, 4, 2, 2);
					testdrawtexture(-5, 2, 2, 3);
					testdrawtexture(-5, 0, 2, 4);
					testdrawtexture(-5, -2, 2, 5);*/
					//***20150409畫建構槽
					//vcg::glBoxWire(printArea);

					//vcg::glBoxWire(md()->groove);

					//***20150413 畫current mesh bounding_box
					//if (mm() != NULL &&
					//	(!(mm()->label().contains("_temp_outlines", Qt::CaseSensitive) || mm()->label().contains("_temp_ol_mesh", Qt::CaseSensitive)))
					//	)glBoxWire(mm()->cm.bbox);
					/*if (mm() != NULL &&
					(!(mm()->label().contains(md()->p_setting.getOlMeshName(), Qt::CaseSensitive) || mm()->label().contains(md()->p_setting.getCapMeshName(), Qt::CaseSensitive)))
					)glBoxWire(mm()->cm.bbox);*/

					/*if (mm() != NULL &&
					(mm()->getMeshSort() == MeshModel::meshsort::print_item)
					)glBoxWire_SKT2(mm()->cm.bbox);*/

					foreach(int i, md()->multiSelectID)
					{
						//if (i != -1 && md()->getMesh(i) != NULL )
						if (md()->multiSelectID.size() > 0)
							glBoxWire_SKT2(md()->getMesh(i)->cm.bbox, vcg::Color4f(1, 0, 1, 1), false, 1);

					}
					if (md()->intersectMeshesID2.size() > 0)
					for (std::set<int>::iterator it = md()->intersectMeshesID2.begin(); it != md()->intersectMeshesID2.end(); ++it)
					{
						//if (i != -1 && md()->getMesh(i) != NULL )
						if (md()->getMesh(*it) != 0)
						{
							//glBoxWire_SKT2(md()->getMesh(*it)->cm.bbox, vcg::Color4f(0.89, 0.43, 0.38, 1));
							glBoxWire_SKT2(md()->getMesh(*it)->cm.bbox, vcg::Color4f(0.89, 0.43, 0.38, 1), false, 5);
						}

					}

					if (md()->multiSelectID.size() >= 1)
						glBoxWire_SKT2(md()->selBBox(), vcg::Color4f(0.5, 0, 0.5, 0), true);
				}
			}
			if (iEdit) {//***20150623***decorate_plugins
				iEdit->setLog(&md()->Log);
				iEdit->Decorate(*mm(), this, &painter);
			}
			if (openEditMoving)
			{
				QAction *temp = mw()->genGroupbox.first()->currentAction;
				MeshEditInterFace_v2  *iEditI = qobject_cast<MeshEditInterFace_v2 *>(temp->parent());
				iEditI->updateModelProjectionMatrix(this);

			}

			// Draw the selection
			if (mm() != NULL)
			{
				QMap<int, RenderMode>::iterator it = rendermodemap.find(mm()->id());
				if ((it != rendermodemap.end()) && it.value().selectedFace)
					mm()->renderSelectedFace();
				if ((it != rendermodemap.end()) && it.value().selectedVert)
					mm()->renderSelectedVert();
			}

			glPopAttrib();
		} ///end if busy

		glPopMatrix(); // We restore the state to immediately after the trackball (and before the bbox scaling/translating)

		if (trackBallVisible && !takeSnapTile && !(iEdit && !suspendedEditor))
			trackball.DrawPostApply();

		// The picking of the surface position has to be done in object space,//選點
		// so after trackball transformation (and before the matrix associated to each mesh);
		if (hasToPick && hasToGetPickPos)
		{
			Point3f pp;
			hasToPick = false;
			if (Pick<Point3f>(pointToPick[0], pointToPick[1], pp))
			{
				emit transmitSurfacePos(nameToGetPickPos, pp);
				hasToGetPickPos = false;
			}
		}
		/*foreach(QAction * p, iPerDocDecoratorlist)
		{
		MeshDecorateInterface * decorInterface = qobject_cast<MeshDecorateInterface *>(p->parent());
		decorInterface->decorateDoc(p, *this->md(), this->glas.currentGlobalParamSet, this, &painter, md()->Log);
		}*/

		// we want to write scene-space the point picked with double-click in the log
		// we have to do it now, before leaving this transformation space
		// we hook to the same mechanism double-click will be managed later on to move trackball
		if (hasToPick && !hasToGetPickPos)
		{
			Point3f pp;
			if (Pick<Point3f>(pointToPick[0], pointToPick[1], pp))
			{
				// write picked point in the log
				//Logf(0, "Recentering on point [%f %f %f] [%d,%d]", pp[0], pp[1], pp[2], pointToPick[0], pointToPick[1]);
			}
		}
		//***20150804***draw_Axis
		//drawAxis(&painter, id);
		drawAxis(&painter, 1);


		glPopMatrix(); // We restore the state to immediately before the trackball
	}
	else if (!this->md()->isBusy())
	{
		setGrooveView();
	}
	//If it is a raster viewer draw the image as a texture
	if (isRaster())
		drawTarget();

	// Double click move picked point to center
	// It has to be done in the before trackball space (we MOVE the trackball itself...)
	if (hasToPick && !hasToGetPickPos)
	{
		Point3f pp;
		hasToPick = false;
		if (Pick<Point3f>(pointToPick[0], pointToPick[1], pp))
		{
			trackball.MouseUp(pointToPick[0], pointToPick[1], vcg::Trackball::BUTTON_NONE);
			trackball.Translate(-pp);
			trackball.Scale(1.25f);

			QCursor::setPos(mapToGlobal(QPoint(width() / 2 + 2, height() / 2 + 2)));
		}
	}

	// ...and take a snapshot
	if (takeSnapTile) pasteTile();

	// Finally display HELP if requested
	//if (isHelpVisible()) displayHelp();

	// Draw the log area background
	// on the bottom of the glArea
	if (infoAreaVisible)
	{
		glPushAttrib(GL_ENABLE_BIT);
		//glPushAttrib(GL_ALL_ATTRIB_BITS);
		glDisable(GL_DEPTH_TEST);
		//displayInfo(&painter);
		//if(getDisplayInfo())displayInfo_2(&painter);
		//if (getDisplayInfo())displayInfo(&painter);
		//if (getDisplayInfo())displayInfo_3(&painter);
		if (getDisplayInfo())displayInfo_4();//work function
		//displayRealTimeLog(&painter);//***顯示左上角的help
		updateFps(time.elapsed());
		glPopAttrib();
	}

	//Draw highlight if it is the current viewer
	if (mvc()->currentId == id)
		displayViewerHighlight();


	QString error = checkGLError::makeString("There are gl errors: ");
	if (!error.isEmpty()) {
		Logf(GLLogStream::WARNING, qPrintable(error));
	}




	//check if viewers are linked
	MainWindow *window = qobject_cast<MainWindow *>(QApplication::activeWindow());
	if (window && window->linkViewersAct->isChecked() && mvc()->currentId == id)
		mvc()->updateTrackballInViewers();
	painter.endNativePainting();
	//***20150409
	if (updateRendermodemapSiganl) { if (mvc()) { updaterendermodemap(); updateRendermodemapSiganl = false; } }
	//20160322
	//mvc()->updateAllViewer();//***backup

	//counttime++;
	//qDebug() << counttime ;
}
//***20150512_drawtexture
void GLArea::testdrawtexture(float x, float y, float length, unsigned int num)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glPushMatrix();
	//glBindTexture(GL_TEXTURE_2D, mesh_texture[num]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(x, y, 0);
	glTexCoord2f(1, 0); glVertex3f(x + length, y, 0);
	glTexCoord2f(1, 1); glVertex3f(x + length, y + length, 0);
	glTexCoord2f(0, 1); glVertex3f(x, y + length, 0);
	glEnd();
	glPopMatrix();
	glPopAttrib();
}
void GLArea::displayMatrix(QPainter *painter, QRect areaRect)
{
	painter->save();
	qFont.setFamily("Helvetica");
	qFont.setPixelSize(10);
	qFont.setStyleStrategy(QFont::PreferAntialias);
	painter->setFont(qFont);

	QString tableText;
	for (int i = 0; i < 4; i++)
		tableText += QString("\t%1\t%2\t%3\t%4\n")
		.arg(mm()->cm.Tr[i][0], 5, 'f', 2).arg(mm()->cm.Tr[i][1], 5, 'f', 2)
		.arg(mm()->cm.Tr[i][2], 5, 'f', 2).arg(mm()->cm.Tr[i][3], 5, 'f', 2);

	QTextOption TO;
	QTextOption::Tab ttt;
	ttt.type = QTextOption::DelimiterTab;
	ttt.delimiter = '.';
	const int columnSpacing = 40;
	ttt.position = columnSpacing;
	QList<QTextOption::Tab> TabList;
	for (int i = 0; i < 4; ++i) {
		TabList.push_back(ttt);
		ttt.position += columnSpacing;
	}
	TO.setTabs(TabList);
	painter->drawText(areaRect, tableText, TO);
	painter->restore();
}
void GLArea::displayRealTimeLog(QPainter *painter)
{
	painter->endNativePainting();
	painter->save();
	painter->setPen(Qt::white);
	Color4b logAreaColor = glas.logAreaColor;
	glas.logAreaColor[3] = 128;
	if (mvc()->currentId != id) logAreaColor /= 2.0;

	qFont.setStyleStrategy(QFont::PreferAntialias);
	qFont.setFamily("Helvetica");
	qFont.setPixelSize(11);
	painter->setFont(qFont);
	float margin = qFont.pixelSize();
	QFontMetrics metrics = QFontMetrics(font());
	int border = qMax(4, metrics.leading());
	qreal roundness = 10.0f;
	QTextDocument doc;
	doc.setDefaultFont(qFont);
	int startingpoint = border;
	//mQMultiMap<QString,std::pair<QString,QString> >::const_iterator it = md()->Log.RealTimeLogText.constBegin();it != md()->Log.RealTimeLogText.constEnd();++it)
	foreach(QString keyIt, md()->Log.RealTimeLogText.uniqueKeys())
	{
		QList< QPair<QString, QString> > valueList = md()->Log.RealTimeLogText.values(keyIt);
		QPair<QString, QString> itVal;
		// the map contains pairs of meshname, text
		// the meshname is used only to disambiguate when there are more than two boxes with the same title
		foreach(itVal, valueList)
		{
			QString HeadName = keyIt;
			if (md()->Log.RealTimeLogText.count(keyIt) > 1)
				HeadName += " - " + itVal.first;
			doc.clear();
			doc.setDocumentMargin(margin*0.75);
			QColor textColor = Qt::white;
			QColor headColor(200, 200, 200);
			doc.setHtml("<font color=\"" + headColor.name() + "\" size=\"+1\" ><p><i><b>" + HeadName + "</b></i></p></font>"
				"<font color=\"" + textColor.name() + "\"             >" + itVal.second + "</font>");
			QRect outrect(border, startingpoint, doc.size().width(), doc.size().height());
			QPainterPath path;
			painter->setBrush(QBrush(ColorConverter::ToQColor(logAreaColor), Qt::SolidPattern));
			painter->setPen(ColorConverter::ToQColor(logAreaColor));
			path.addRoundedRect(outrect, roundness, roundness);
			painter->drawPath(path);
			painter->save();
			painter->translate(border, startingpoint);
			doc.drawContents(painter);
			painter->restore();
			startingpoint = startingpoint + doc.size().height() + margin*.75;
		}
	}

	// After the rederaw we clear the RealTimeLog buffer!
	md()->Log.RealTimeLogText.clear();
	painter->restore();
	painter->beginNativePainting();
}
void GLArea::displayInfo_4()
{
	//glMatrixMode(GL_PROJECTION);
	//glPushMatrix(); // save
	//glLoadIdentity();// and clear
	//glMatrixMode(GL_MODELVIEW);
	//glPushMatrix();
	//glLoadIdentity();


	std::vector<QString> sv;
	if (/*this->md()->size() > 0 &&*/ this->md()->count_print_item() > 0 && mm() != NULL)
	{
		//sv.push_back(QString("FPS: %1\n").arg(cfps, 7, 'f', 1));
		//sv.push_back(QString("volume: %1\n").arg(this->md()->bbox().Volume(), 7, 'f', 1));
		//sv.push_back(QString("ms: %1\n").arg(tempselectionms));
		//sv.push_back(QString("Mesh: %1\n").arg(mm()->label()));
		////sv.push_back(QString("Vertices: %1\n").arg(mm()->cm.vn));
		//sv.push_back(QString("Vertices: %1 (%2)\n").arg(mm()->cm.vn).arg(this->md()->vn()));
		//sv.push_back(QString("Faces: %1\n").arg(mm()->cm.fn));
		//sv.push_back(QString("X: %1 \n").arg(mm()->cm.bbox.DimX(), 0, 'g', 3));
		//sv.push_back(QString("Y: %1 \n").arg(mm()->cm.bbox.DimY(), 0, 'g', 3));
		//sv.push_back(QString("Z: %1 \n").arg(mm()->cm.bbox.DimZ(), 0, 'g', 3));
		//sv.push_back(QString("Data Mask: %1 \n").arg(GetMeshInfoString()));
		//sv.push_back(QString(" drawMode %1  (%2) (%3)\n").arg(mm()->rmm.drawMode).arg(mm()->rmm.colorMode).arg(mm()->rmm.textureMode));
	}
	//SKT::GL_label_SKT::draw_label(sv, 22, -10);
	SKT::GL_label_SKT::draw_label(sv, -200, 200);
	//SKT::GL_label_SKT::draw_label(sv, -1, 0);
	//char *p1=0;//
	//SKT::GL_label_SKT::qstringToChar(col1Text, p1);
	//
	//


	//glDisable(GL_DEPTH_TEST); // also disable the depth test so renders on top

	//glColor3f(1, 1, 1);
	//glRasterPos2f(22, -10); // center of screen. (-1,0) is center left.
	//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	//char buf[300];
	//sprintf(buf, "Oh hello \n ddd");
	//const char * p = buf;
	////do glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p1); while (*(++p1));

	//SKT::GL_label_SKT::print_bitmap_string(GLUT_BITMAP_HELVETICA_18, p1);
	//glEnable(GL_DEPTH_TEST); // Turn depth testing back on

	//glMatrixMode(GL_PROJECTION);
	//glPopMatrix(); // revert back to the matrix I had before.
	//glMatrixMode(GL_MODELVIEW);
	//glPopMatrix();
}
void GLArea::displayInfo_3(QPainter *p)
{
	glLabel::Mode md;
	QPen textPen(QColor(255, 255, 255));
	//textPen.setWidthF(0.7);

	md.qFont.setBold(true);
	md.qFont.setPixelSize(20);
	p->setPen(textPen);
	float d = 10;
	QString col1Text, col0Text;
	if (this->md()->size() > 0)
	{
		//if (this->md()->size() == 1)
		if (true)
		{
			col1Text += QString("FPS: %1\n").arg(cfps, 7, 'f', 1);
			col1Text += QString("Mesh: %1\n").arg(mm()->label());
			col1Text += QString("Vertices: %1\n").arg(mm()->cm.vn);
			col1Text += QString("Faces: %1\n").arg(mm()->cm.fn);
			col1Text += QString("X: %1 \n").arg(mm()->cm.bbox.DimX(), 0, 'g', 3);
			col1Text += QString("Y: %1 \n").arg(mm()->cm.bbox.DimY(), 0, 'g', 3);
			col1Text += QString("Z: %1 \n").arg(mm()->cm.bbox.DimZ(), 0, 'g', 3);
		}
		else
		{
			col0Text += QString("FPS: %1\n").arg(cfps, 7, 'f', 1);
			col1Text += QString("Current Mesh: %1\n").arg(mm()->label());
			col1Text += QString("Vertices: %1 (%2)\n").arg(mm()->cm.vn).arg(this->md()->vn());
			col1Text += QString("Faces: %1 (%2)\n").arg(mm()->cm.fn).arg(this->md()->fn());
			col1Text += QString("X: %1 \n").arg(mm()->cm.bbox.DimX(), 0, 'g', 3);
			col1Text += QString("Y: %1 \n").arg(mm()->cm.bbox.DimY(), 0, 'g', 3);
			col1Text += QString("Z: %1 \n").arg(mm()->cm.bbox.DimZ(), 0, 'g', 3);
			//.arg(mm()->cm.bbox.DimY(), 0, 'g', 3).arg(mm()->cm.bbox.DimZ(), 0, 'g', 3);
		}

		col1Text += QString("Data Mask: %1").arg(GetMeshInfoString());

		if (int(fov) > 5) col0Text += QString("FOV: %1\n").arg(fov);
		else col0Text += QString("FOV: Ortho\n");
		if ((cfps > 0) && (cfps < 1999))
			col0Text += QString("FPS: %1\n").arg(cfps, 7, 'f', 1);
		if (clipRatioNear != clipRatioNearDefault())
			col0Text += QString("Clipping Near:%1\n").arg(clipRatioNear, 7, 'f', 2);


		QString widthfont = QString("Current Mesh: %1\n").arg(mm()->label());
		QString widthfont2 = QString("Current Mesh: \n");
		QString widthfont3 = "Current Mesh:   \n" + mm()->label();


		//if (mm()->cm.Tr != Matrix44m::Identity()) displayMatrix(painter, Column_2);
	}
	QRect Row_0(width() * 7 / 9, this->height() - 200, width() * 2 / 9, 400);
	//vcg::glLabel::render(p, vcg::Point3f(d, 0, 0), col1Text, md);
	p->drawText(Row_0, Qt::AlignLeft | Qt::TextWordWrap, col1Text);
	vcg::glLabel::render(p, vcg::Point3f(d, 1, 0), col1Text, md);
	//vcg::glLabel::render(p, vcg::Point3f(d, 2, 0), col1Text, md);
	//vcg::glLabel::render(p, vcg::Point3f(d, 0, 5), QString("c"), md);


}
void GLArea::displayInfo_2(QPainter *painter)
{
	if (mvc() == NULL)
		return;

	painter->endNativePainting();
	painter->save();
	painter->setRenderHint(QPainter::HighQualityAntialiasing);
	QPen textPen(QColor(255, 255, 255, 200));
	textPen.setWidthF(0.7);
	painter->setPen(textPen);
	qFont.setStyleStrategy(QFont::PreferAntialias);
	qFont.setFamily("Helvetica");
	qFont.setPixelSize(15);
	painter->setFont(qFont);


	QFontMetrics metrics = QFontMetrics(font());
	int border = qMax(4, metrics.leading());
	//int border = 10;
	int windowBorder = 10;

	//***info顯示位置
	//***QRect(x, y, width, height)	



	Color4b logAreaColor = glas.logAreaColor;
	//***20150910glas.logAreaColor[3] = 128;
	glas.logAreaColor[0] = 255;
	glas.logAreaColor[1] = 102;
	glas.logAreaColor[2] = 255;
	glas.logAreaColor[3] = 255;

	logAreaColor /= 2.0;

	qreal roundness = 20.0f;
	painter->setBrush(QBrush(ColorConverter::ToQColor(logAreaColor), Qt::SolidPattern));
	painter->setPen(QColor(51, 255, 255));

	QString col1Text, col0Text;

	if (this->md()->size() > 0)
	{
		//if (this->md()->size() == 1)
		if (true)
		{
			col1Text += QString("FPS: %1\n").arg(cfps, 7, 'f', 1);
			col1Text += QString("Mesh: %1\n").arg(mm()->label());
			col1Text += QString("Vertices: %1\n").arg(mm()->cm.vn);
			col1Text += QString("Faces: %1\n").arg(mm()->cm.fn);
			col1Text += QString("X: %1 \n").arg(mm()->cm.bbox.DimX(), 0, 'g', 3);
			col1Text += QString("Y: %1 \n").arg(mm()->cm.bbox.DimY(), 0, 'g', 3);
			col1Text += QString("Z: %1 \n").arg(mm()->cm.bbox.DimZ(), 0, 'g', 3);
		}
		else
		{
			col0Text += QString("FPS: %1\n").arg(cfps, 7, 'f', 1);
			col1Text += QString("Current Mesh: %1\n").arg(mm()->label());
			col1Text += QString("Vertices: %1 (%2)\n").arg(mm()->cm.vn).arg(this->md()->vn());
			col1Text += QString("Faces: %1 (%2)\n").arg(mm()->cm.fn).arg(this->md()->fn());
			col1Text += QString("X: %1 \n").arg(mm()->cm.bbox.DimX(), 0, 'g', 3);
			col1Text += QString("Y: %1 \n").arg(mm()->cm.bbox.DimY(), 0, 'g', 3);
			col1Text += QString("Z: %1 \n").arg(mm()->cm.bbox.DimZ(), 0, 'g', 3);
			//.arg(mm()->cm.bbox.DimY(), 0, 'g', 3).arg(mm()->cm.bbox.DimZ(), 0, 'g', 3);
		}
		QMap<int, RenderMode>::iterator it = rendermodemap.find(md()->mm()->id());
		if (it != rendermodemap.end())
		{
			RenderMode rm = it.value();
			if (rm.selectedFace || rm.selectedVert || mm()->cm.sfn > 0 || mm()->cm.svn > 0)
				col1Text += QString("Selection: v:%1 f:%2\n").arg(mm()->cm.svn).arg(mm()->cm.sfn);
		}
		col1Text += QString("Data Mask: %1").arg(GetMeshInfoString());

		if (int(fov) > 5) col0Text += QString("FOV: %1\n").arg(fov);
		else col0Text += QString("FOV: Ortho\n");
		if ((cfps > 0) && (cfps < 1999))
			col0Text += QString("FPS: %1\n").arg(cfps, 7, 'f', 1);
		if (clipRatioNear != clipRatioNearDefault())
			col0Text += QString("Clipping Near:%1\n").arg(clipRatioNear, 7, 'f', 2);


		QString widthfont = QString("Current Mesh: %1\n").arg(mm()->label());
		QString widthfont2 = QString("Current Mesh: \n");
		QString widthfont3 = "Current Mesh:   \n" + mm()->label();

		float barHeight = qFont.pixelSize() * 11;
		QRect texbound = metrics.boundingRect(col0Text);


		//QRect infoArea(width() * 7 / 9 - windowBorder, this->height() - barHeight - windowBorder, metrics.width(widthfont3)+60, barHeight);

		QRect infoArea(width() * 7 / 9 - windowBorder, this->height() - barHeight - windowBorder, width() * 2 / 9, barHeight);
		QRect Row_0(width() * 7 / 9 + border, this->height() - barHeight - windowBorder + border, width() * 2 / 9, barHeight);

		//painter->drawRoundedRect(infoArea, roundness, roundness);//畫方格
		//painter->drawText(Row_0, Qt::AlignLeft | Qt::TextWordWrap, col1Text);
		painter->drawText(Row_0, Qt::AlignLeft | Qt::TextWordWrap, col1Text);

		//if (mm()->cm.Tr != Matrix44m::Identity()) displayMatrix(painter, Column_2);
	}
	painter->restore();
	painter->beginNativePainting();
	//glPopAttrib();

}
void GLArea::displayInfo(QPainter *painter)
{
	if (mvc() == NULL)
		return;
	painter->endNativePainting();
	painter->save();
	painter->setRenderHint(QPainter::HighQualityAntialiasing);
	QPen textPen(QColor(255, 255, 255, 200));
	textPen.setWidthF(0.2f);
	painter->setPen(textPen);

	qFont.setStyleStrategy(QFont::PreferAntialias);
	qFont.setFamily("Helvetica");
	qFont.setPixelSize(12);
	painter->setFont(qFont);
	float barHeight = qFont.pixelSize() * 5;
	QFontMetrics metrics = QFontMetrics(font());
	int border = qMax(4, metrics.leading());

	QRect Column_0(width() / 10, this->height() - barHeight + border, width() / 2, this->height() - border);
	QRect Column_1(width() / 2, this->height() - barHeight + border, width() * 3 / 4, this->height() - border);
	QRect Column_2(width() * 3 / 4, this->height() - barHeight + border, width(), this->height() - border);

	Color4b logAreaColor = glas.logAreaColor;
	//***20150910glas.logAreaColor[3] = 128;
	glas.logAreaColor[3] = 0;

	//if (mvc()->currentId != id) logAreaColor /= 2.0;

	painter->fillRect(QRect(0, this->height() - barHeight, width(), this->height()), ColorConverter::ToQColor(logAreaColor));

	QString col1Text, col0Text;

	if (this->md()->size() > 0)
	{
		if (this->md()->size() == 1)
		{
			col1Text += QString("Mesh: %1\n").arg(mm()->label());
			col1Text += QString("Vertices: %1\n").arg(mm()->cm.vn);
			col1Text += QString("Faces: %1\n").arg(mm()->cm.fn);
		}
		else
		{
			col1Text += QString("Current Mesh: %1\n").arg(mm()->label());
			col1Text += QString("Vertices: %1 (%2)\n").arg(mm()->cm.vn).arg(this->md()->vn());
			col1Text += QString("Faces: %1 (%2)\n").arg(mm()->cm.fn).arg(this->md()->fn());
		}
		QMap<int, RenderMode>::iterator it = rendermodemap.find(md()->mm()->id());
		if (it != rendermodemap.end())
		{
			RenderMode rm = it.value();
			if (rm.selectedFace || rm.selectedVert || mm()->cm.sfn > 0 || mm()->cm.svn > 0)
				col1Text += QString("Selection: v:%1 f:%2\n").arg(mm()->cm.svn).arg(mm()->cm.sfn);
		}
		col1Text += GetMeshInfoString();

		if (int(fov) > 5) col0Text += QString("FOV: %1\n").arg(fov);
		else col0Text += QString("FOV: Ortho\n");
		if ((cfps > 0) && (cfps < 1999))
			col0Text += QString("FPS: %1\n").arg(cfps, 7, 'f', 1);
		if (clipRatioNear != clipRatioNearDefault())
			col0Text += QString("Clipping Near:%1\n").arg(clipRatioNear, 7, 'f', 2);
		painter->drawText(Column_1, Qt::AlignLeft | Qt::TextWordWrap, col1Text);
		painter->drawText(Column_0, Qt::AlignLeft | Qt::TextWordWrap, col0Text);
		if (mm()->cm.Tr != Matrix44m::Identity()) displayMatrix(painter, Column_2);
	}
	painter->restore();
	painter->beginNativePainting();
	//glPopAttrib();
}


void GLArea::displayViewerHighlight()
{
	// Enter in 2D screen Mode again
	glPushAttrib(GL_LINE_BIT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.0f, 1.0f, 1.0f, 0.3f);
	for (int width = 5; width > 0; width -= 2)
	{
		glLineWidth(width);
		glBegin(GL_LINE_LOOP);
		glVertex2f(-1.f, 1.f);     glVertex2f(1.f, 1.f); glVertex2f(1.f, -1.f);    glVertex2f(-1.f, -1.f);
		glEnd();
	}
	// Closing 2D
	glPopAttrib();
	glPopMatrix(); // restore modelview
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopAttrib();

}


void GLArea::displayHelp()
{
	static QString tableText;
	if (tableText.isEmpty())
	{
		QFile helpFile(":/images/onscreenHelp.txt");
		if (helpFile.open(QFile::ReadOnly))
			tableText = helpFile.readAll();
		else assert(0);
	}
	md()->Log.RealTimeLog("Quick Help", "", tableText);
}


void GLArea::saveSnapshot()
{
	// snap all layers
	currSnapLayer = 0;

	// number of subparts
	totalCols = totalRows = ss.resolution;
	tileRow = tileCol = 0;

	if (ss.snapAllLayers)
	{
		while (currSnapLayer < this->md()->meshList.size())
		{
			tileRow = tileCol = 0;
			qDebug("Snapping layer %i", currSnapLayer);
			this->md()->setCurrentMesh(currSnapLayer);
			foreach(MeshModel *mp, this->md()->meshList) {
				meshSetVisibility(mp, false);
			}
			meshSetVisibility(mm(), true);

			takeSnapTile = true;
			repaint();//
			currSnapLayer++;
		}

		//cleanup
		foreach(MeshModel *mp, this->md()->meshList) {
			meshSetVisibility(mp, true);
		}
		ss.counter++;
	}
	else
	{
		takeSnapTile = true;
		update();
	}
}

// Slot called when the current mesh has changed.
void GLArea::manageCurrentMeshChange()
{
	//if we have an edit tool open, notify it that the current layer has changed
	if (iEdit)
	{
		if (iEdit->isSingleMeshEdit())
			assert(lastModelEdited);  // if there is an editor that works on a single mesh
		// last model edited should always be set when start edit is called

		iEdit->LayerChanged(*this->md(), *lastModelEdited, this);

		//now update the last model edited
		//TODO this is not the best design....   iEdit should maybe keep track of the model on its own
		lastModelEdited = this->md()->mm();
	}
	emit this->updateMainWindowMenus();
	// if the layer has changed update also the decoration.
	//	updateAllPerMeshDecorators();
}

/// Execute a end/start pair for all the PerMesh decorator that are active in this glarea.
/// It is used when the document is changed or when some parameter changes
/// Note that it is rather inefficient. Such work should be done only once for each decorator.
void GLArea::updateAllPerMeshDecorators()
{
	for (QMap<int, QList<QAction *> >::iterator i = iPerMeshDecoratorsListMap.begin(); i != iPerMeshDecoratorsListMap.end(); ++i)
	{
		MeshModel *m = md()->getMesh(i.key());
		foreach(QAction *p, i.value())
		{
			MeshDecorateInterface * decorInterface = qobject_cast<MeshDecorateInterface *>(p->parent());
			decorInterface->endDecorate(p, *m, this->glas.currentGlobalParamSet, this);
			decorInterface->setLog(&md()->Log);
			decorInterface->startDecorate(p, *m, this->glas.currentGlobalParamSet, this);
		}
	}
}


void GLArea::setCurrentEditAction(QAction *editAction)
{
	assert(editAction);
	currentEditor = editAction;

	iEdit = actionToMeshEditMap.value(currentEditor);
	assert(iEdit);
	lastModelEdited = this->md()->mm();
	if (!iEdit->StartEdit(*this->md(), this))
	{
		//iEdit->EndEdit(*(this->md()->mm()), this);
		endEdit();
	}
	else
	{
		Logf(GLLogStream::SYSTEM, "Started Mode %s", qPrintable(currentEditor->text()));
		if (mm() != NULL)
			mm()->meshModified() = true;
		else assert(!iEdit->isSingleMeshEdit());
	}
}


bool GLArea::readyToClose()
{
	// Now do the actual closing of the glArea
	if (getCurrentEditAction()) endEdit();
	if (iRenderer)
		iRenderer->Finalize(currentShader, this->md(), this);

	// Now manage the closing of the decorator set;
	foreach(QAction* act, iPerDocDecoratorlist)
	{
		MeshDecorateInterface* mdec = qobject_cast<MeshDecorateInterface*>(act->parent());
		mdec->endDecorate(act, *md(), glas.currentGlobalParamSet, this);
		mdec->setLog(NULL);
	}
	iPerDocDecoratorlist.clear();
	QSet<QAction *> dectobeclose;
	for (QMap<int, QList<QAction *> >::iterator it = iPerMeshDecoratorsListMap.begin(); it != iPerMeshDecoratorsListMap.end(); ++it)
	{
		foreach(QAction* curract, it.value())
			dectobeclose.insert(curract);
	}

	for (QSet<QAction *>::iterator it = dectobeclose.begin(); it != dectobeclose.end(); ++it)
	{
		MeshDecorateInterface* mdec = qobject_cast<MeshDecorateInterface*>((*it)->parent());
		if (mdec != NULL)
		{
			mdec->endDecorate(*it, *md(), glas.currentGlobalParamSet, this);
			mdec->setLog(NULL);
		}
	}
	dectobeclose.clear();
	iPerMeshDecoratorsListMap.clear();
	if (targetTex) glDeleteTextures(1, &targetTex);
	emit glareaClosed();
	return true;
}

void GLArea::keyReleaseEvent(QKeyEvent * e)
{
	e->ignore();
	if (iEdit && !suspendedEditor)  iEdit->keyReleaseEvent(e, *mm(), this);
	else {
		if (e->key() == Qt::Key_Control) trackball.ButtonUp(QT2VCG(Qt::NoButton, Qt::ControlModifier));
		if (e->key() == Qt::Key_Shift) trackball.ButtonUp(QT2VCG(Qt::NoButton, Qt::ShiftModifier));
		if (e->key() == Qt::Key_Alt) trackball.ButtonUp(QT2VCG(Qt::NoButton, Qt::AltModifier));
	}
}

void GLArea::keyPressEvent(QKeyEvent * e)
{
	e->ignore();

	if (iEdit && !suspendedEditor)  iEdit->keyPressEvent(e, *mm(), this);
	else {
		if (e->key() == Qt::Key_Control) trackball.ButtonDown(QT2VCG(Qt::NoButton, Qt::ControlModifier));
		if (e->key() == Qt::Key_Shift) trackball.ButtonDown(QT2VCG(Qt::NoButton, Qt::ShiftModifier));
		if (e->key() == Qt::Key_Alt) trackball.ButtonDown(QT2VCG(Qt::NoButton, Qt::AltModifier));
		if (e->key() == Qt::Key_Delete)
		{
			//if (!(mm()->label().contains(psetting()->getCapMeshName(), Qt::CaseSensitive) || mm()->label().contains(psetting()->getCapMeshName(), Qt::CaseSensitive)))

			//***20160425
			//md()->delMeshes();
			mw()->delMeshes();
			//***20160425_backup
			//if (mm()->getMeshSort() != MeshModel::meshsort::decorate_item)
			//mw()->delCurrentMesh();


		}
		//***20150813
		//if (e->key() == Qt::Key_V) mw()->copyCurrentMesh ();		

		//***20170116 (comment out codes for QKeySequence::Copy and QKeySequence::Paste) lines 1549 - 1570 : by KPPH R&D-I-SW, Mark
		//if (e->matches(QKeySequence::Copy))
		//{
		//	if (md()->multiSelectID.size() == 1)
		//		mw()->setCopyMesh();//single_Mesh
		//	else	QMessageBox::warning(this, "One Mesh only", "Please select one mesh to copy");


		//}
		//if (e->matches(QKeySequence::Paste))
		//{

		//	if (md()->selCm() != NULL)
		//	{
		//		if (md()->multiSelectID.size() == 1){
		//			mw()->pastSelectMesh();//single_Mesh
		//			mw()->paste_occur();
		//		}
		//		else	QMessageBox::warning(this, "One Mesh only", "Please select one mesh to copy");
		//	}

		//}
		if (e->matches(QKeySequence::SelectAll))
		if (e->matches(QKeySequence::SelectAll))
			//if (QApplication::keyboardModifiers()&QKeySequence::SelectAll)
		{
			md()->multiSelectID.clear();

			foreach(MeshModel *sm, md()->meshList)
			{
				//if (sm->getMeshSort() == MeshModel::meshsort::print_item)
				if (!sm->label().contains(md()->p_setting.getOlMeshName(), Qt::CaseInsensitive) && !sm->label().contains(md()->p_setting.getCapMeshName(), Qt::CaseInsensitive))
					md()->multiSelectID.insert(sm->id());
			}

			emit this->updateMainWindowMenus();
		}
		//***20160302
		if (e->key() == Qt::Key_L)
		{
			equ[3] += 0.1;
			bSpin ^= 1;
			//qDebug() << "equ[3] " << equ[3];
		}else
		if (e->key() == Qt::Key_P)
		{
			equ[3] -= 0.1;
			//qDebug() << "equ[3] " << equ[3];

		}else
		if (e->key() == Qt::Key_O)
		{
			/*md()->setBusy(true);
			mw()->createSliceBar();
			md()->setBusy(false);*/

		}
		else if (e->key() == Qt::Key_Escape)
		{
			md()->multiSelectID.clear();
		}
		mvc()->updateAllViewer();

	}


}

void GLArea::mousePressEvent(QMouseEvent*e)
{
	e->accept();
	if (!this->hasFocus()) this->setFocus();	
	/*rubberband test*/
	/*origin = e->pos();
	rubberBand->setGeometry(QRect(origin, QSize()));
	rubberBand->show();*/

	if ((iEdit && !suspendedEditor))
	{

		iEdit->mousePressEvent(e, *mm(), this);
	}
	else
	{
		//***if(e->button()==Qt::RightButton)
		if ((e->modifiers() & Qt::ControlModifier) && (e->button() == Qt::LeftButton))
		{
			//multiSelectID.clear();
			hasToSelect = true;
			hasToMultiSelect = true;
			this->pointToPick = Point2i(QT2VCG_X(this, e), QT2VCG_Y(this, e));
			//this->multi_pointToPick.push_back(Point2i(QT2VCG_X(this, e), QT2VCG_Y(this, e)));

		}
		else if ((e->modifiers() & Qt::AltModifier) && (e->button() == Qt::LeftButton)) // Select a new current mesh鍵
		{
			qDebug() << "inALT func" << endl;
			//***20160525_test undo
			if (mm() != NULL) {
				old_Position = mm()->cm.bbox.Center();
				old_Matrix = mm()->cm.Tr;
			}
			//***20150909刪除ol
			//mw()->delOl("_temp_outlines", "_temp_ol_mesh");
			//mw()->delOl(md()->p_setting.getOlMeshName(), md()->p_setting.getCapMeshName());//20160308_backup

			//***20150413***如果有選到mesh, 則打開groupbox
			/*hasToSelect = true;
			qDebug() << "hasToSelect" << hasToSelect;
			this->pointToPick = Point2i(QT2VCG_X(this, e), QT2VCG_Y(this, e));*/
			//***20150602***把groupbox打開 
			/*mw()->ptranslate->setEnabled(true);
			mw()->pscale->setEnabled(true);
			mw()->protate->setEnabled(true);		*/
			//***20150728*******測試位移物體*********************			
			if (mw()->genGroupbox.first()->currentAction && mm() != NULL)
			{
				setxyOrz(true);
				QAction *temp = mw()->genGroupbox.first()->currentAction;
				MeshEditInterFace_v2  *iEditI = qobject_cast<MeshEditInterFace_v2 *>(temp->parent());
				iEditI->mousePressEvent(e, *mm(), this, md());
				//qDebug() << "glArea.cpp" << mw()->genGroupbox.first()->currentAction->text();
				openEditMoving = true;
			}
			//mw()->testdialog = new QDialog(this);

		}
		else if ((e->modifiers() & Qt::ShiftModifier) && (e->button() == Qt::LeftButton)) // Select a new current mesh鍵
		{
			//***20160525_test undo
			if (mm() != NULL) {
				old_Position = mm()->cm.bbox.Center();
				old_Matrix = mm()->cm.Tr;
			}
			//***20150728*******測試位移物體*********************			
			if (mw()->genGroupbox.first()->currentAction && mm() != NULL)
			{
				setxyOrz(false);
				QAction *temp = mw()->genGroupbox.first()->currentAction;
				MeshEditInterFace_v2  *iEditI = qobject_cast<MeshEditInterFace_v2 *>(temp->parent());
				iEditI->mousePressEvent(e, *mm(), this, md());
				//qDebug() << "glArea.cpp" << mw()->genGroupbox.first()->currentAction->text();
				openEditMoving = true;
			}
			//mw()->testdialog = new QDialog(this);

		}
		//else if (/*(e->modifiers() & Qt::Key_Meta) &&*/ (e->button() == Qt::RightButton))
		//{
		//	hasToSelect = true;
		//}

		else if ((e->button() == Qt::LeftButton))
		{
			//***20150413***如果有選到mesh, 則打開groupbox
			//md()->multiSelectID.clear();
			//multi_pointToPick.clear();					
			hasToSelect = true;
			//hasToMultiSelect = false;
			this->pointToPick = Point2i(QT2VCG_X(this, e), QT2VCG_Y(this, e));
			//***20150602***把groupbox打開 
		}


		//else
		//{
		//***20160308
		if ((e->modifiers() & Qt::ShiftModifier) &&
			/*(e->modifiers() & Qt::ControlModifier) &&*/
			/*(e->button() == Qt::LeftButton)*/(e->button() == Qt::RightButton))
		{
			//activeDefaultTrackball = false;
		}
		else
			activeDefaultTrackball = true;

		//***20150410 修改左邊兩個視窗為固定住為
		//if (isDefaultTrackBall() )
		if (isDefaultTrackBall() && !lockTrackmouse && (e->button() == Qt::RightButton))
		{
			if (QApplication::keyboardModifiers() & Qt::Key_Control)
				trackball.ButtonDown(QT2VCG(Qt::NoButton, Qt::ControlModifier));
			else
				trackball.ButtonUp(QT2VCG(Qt::NoButton, Qt::ControlModifier));

			if (QApplication::keyboardModifiers() & Qt::Key_Shift)
			{
				trackball.ButtonDown(QT2VCG(Qt::NoButton, Qt::ShiftModifier));
				//----20151228------------------					
				//mw()->delOl(md()->p_setting.getOlMeshName(), md()->p_setting.getCapMeshName());					
				//hasToSelect = true;
				//qDebug() << hasToSelect;
				//this->pointToPick = Point2i(QT2VCG_X(this, e), QT2VCG_Y(this, e));					
				////***20150728*******測試位移物體*********************
				//if (mw()->genGroupbox.first()->currentAction && mm() != NULL)
				//{
				//	QAction *temp = mw()->genGroupbox.first()->currentAction;
				//	MeshEditInterFace_v2  *iEditI = qobject_cast<MeshEditInterFace_v2 *>(temp->parent());
				//	iEditI->mousePressEvent(e, *mm(), this);
				//	qDebug() << "glArea.cpp" << mw()->genGroupbox.first()->currentAction->text();
				//	openEditMoving = true;
				//}

			}
			else
			{
				trackball.ButtonUp(QT2VCG(Qt::NoButton, Qt::ShiftModifier));

			}


			if (QApplication::keyboardModifiers() & Qt::Key_Alt)
				trackball.ButtonDown(QT2VCG(Qt::NoButton, Qt::AltModifier));
			else
				trackball.ButtonUp(QT2VCG(Qt::NoButton, Qt::AltModifier));

			trackball.MouseDown(QT2VCG_X(this, e), QT2VCG_Y(this, e), QT2VCG(e->button(), e->modifiers()));
		}
		else
			trackball_light.MouseDown(QT2VCG_X(this, e), QT2VCG_Y(this, e), QT2VCG(e->button(), Qt::NoModifier));
		//}
	}
	//getPickId = -1;
	//***20160322
	//update();
}

void GLArea::mouseMoveEvent(QMouseEvent*e)
{

	//***20150728*******************
	if (openEditMoving /*&& getPickId != -1*/)
	{
		drawMeshFlag = false;
		mw()->delOl(MeshModel::meshsort::slice_item);
		QAction *temp = mw()->genGroupbox.first()->currentAction;
		MeshEditInterFace_v2  *iEditI = qobject_cast<MeshEditInterFace_v2 *>(temp->parent());
		iEditI->mouseMoveEvent(e, *mm(), this, md());
		//***20160304
		if (mm()->getMeshSort() == MeshModel::meshsort::decorate_item)
		{
			equ[3] = mm()->cm.bbox.Center().Z();
			mw()->testslicefunc(*md(), equ[3]);
			//qDebug() << "equ[3] " << equ[3];
			if (equ[3] > md()->bbox().max.Z())equ[3] = md()->bbox().max.Z();
			if (equ[3] < md()->bbox().min.Z())equ[3] = md()->bbox().min.Z();
		}

		//20151006**偵測有無超出groove
		md()->groove.setGrooveHit(SKT::detect_hit_groove<Scalarm>(*md()));

	}

	/*rubberBand Test*/	
	//rubberBand->setGeometry(QRect(origin, e->pos()).normalized());
	//************************************************************
	if ((iEdit && !suspendedEditor))
	{
		iEdit->mouseMoveEvent(e, *mm(), this);

	}
	else {
		if (isDefaultTrackBall())
		{
			contextMenuFlag = false;
			drawMeshFlag = false;
			//***20151228***backup
			trackball.MouseMove(QT2VCG_X(this, e), QT2VCG_Y(this, e));

			setCursorTrack(trackball.current_mode);
			//***2015128
			/*QAction *temp = mw()->genGroupbox.first()->currentAction;
			MeshEditInterFace_v2  *iEditI = qobject_cast<MeshEditInterFace_v2 *>(temp->parent());
			iEditI->mouseMoveEvent(e, *mm(), this);*/
		}
		else trackball_light.MouseMove(QT2VCG_X(this, e), QT2VCG_Y(this, e));
	}

	//mm()->glw.Update();
	//***20160322backup
	//update();
}

// When mouse is released we set the correct mouse cursor
void GLArea::mouseReleaseEvent(QMouseEvent*e)
{
	if (openEditMoving)
	{
		//***20150728
		QAction *temp = mw()->genGroupbox.first()->currentAction;
		MeshEditInterFace_v2  *iEditI = qobject_cast<MeshEditInterFace_v2 *>(temp->parent());
		iEditI->mouseReleaseEvent(e, *mm(), this, md());
		md()->intersectMeshesID2.clear();

		//***20160525
		//emit matrix_changed(mm(), old_Matrix);


		//***20170210 --- added if statement to  push the translate command only when there is a selected mesh - by KPPH R&D-I-SW, Mark
		if (!md()->multiSelectID.isEmpty())
			emit matrix_changed_2(md()->multiSelectID, old_Matrix);

		qDebug() << "mouseReleaseEvent_matrix_changed";

		if (md()->multiSelectID.size() > 0)
		{
			mw()->transformwidgett1->loadParameter();
			mw()->updateMenus();
			//foreach(int id, md()->multiSelectID)
			//{
			//	MeshModel *tmm = md()->getMesh(id);
			//	//mw()->testFuncFunc();

			//	if (tmm->getMeshSort() == MeshModel::meshsort::print_item)
			//	{
			//		//tmm->rmm.colorMode = GLW::CMNone;
			//		if (tmm->glw.TMIdd[0].size() >= 0 && tmm->glw.TMIdd[0].size() < 2)
			//		{
			//			tmm->updateDataMask(MeshModel::MM_FACEFACETOPO);
			//			tmm->updateDataMask(MeshModel::MM_VERTFACETOPO);
			//			tri::UpdateNormal<CMeshO>::PerVertexFromCurrentFaceNormal(tmm->cm);
			//			//tri::UpdateNormal<CMeshO>::PerVertex(tmm->cm);
			//			vcg::tri::Allocator<CMeshO>::CompactFaceVector(tmm->cm);
			//			vcg::tri::Allocator<CMeshO>::CompactVertexVector(tmm->cm);
			//			vcg::tri::UpdateTopology<CMeshO>::FaceFace(tmm->cm);
			//			vcg::tri::UpdateTopology<CMeshO>::VertexFace(tmm->cm);

			//			tmm->glw.SetHint(GLW::Hint::HNUseVBO);
			//			tmm->glw.Update(GLW::DMSmooth, tmm->rmm.colorMode, tmm->rmm.textureMode);
			//		}
			//		else
			//		{
			//			return;
			//		}

			//	}

			//}
			//mw()->testFuncFunc();
			//setHastoUpdateVBO();
			setHastoUpdatePartVBO();
		}


		drawMeshFlag = true;
		reFreshGlListlist();
	}
	if (e->button() == Qt::RightButton && contextMenuFlag)
	{
		mw()->rightmouse_contextMenu->exec(e->globalPos());
	}
	else
	{
		contextMenuFlag = true;
	}

	//20151006**偵測有無超出groove
	md()->groove.setGrooveHit(SKT::detect_hit_groove<Scalarm>(*md()));//偵測有無超出groove
	//***************************
	openEditMoving = false;
	//clearFocus();
	activeDefaultTrackball = true;
	if ((iEdit && !suspendedEditor))
		iEdit->mouseReleaseEvent(e, *mm(), this);
	else {
		if (isDefaultTrackBall())
			trackball.MouseUp(QT2VCG_X(this, e), QT2VCG_Y(this, e), QT2VCG(e->button(), e->modifiers()));
		else
			trackball_light.MouseUp(QT2VCG_X(this, e), QT2VCG_Y(this, e), QT2VCG(e->button(), e->modifiers()));
		setCursorTrack(trackball.current_mode);
	}
	drawMeshFlag = true;
	setHasToRefreshDepth();
	//***20160304
	//equ[3] = md()->bbox().min.Z();
	//***20160322
	//update();
}

//Processing of tablet events, interesting only for painting plugins
void GLArea::tabletEvent(QTabletEvent*e)
{
	if (iEdit && !suspendedEditor) iEdit->tabletEvent(e, *mm(), this);
	else e->ignore();
}

void GLArea::wheelEvent(QWheelEvent*e)
{
	setFocus();
	if ((iEdit && !suspendedEditor))
	{
		iEdit->wheelEvent(e, *mm(), this);
	}
	//else if (!(id == 0 || id == 2))
	else if (true)
	{
		const int WHEEL_STEP = 120;
		float notch = e->delta() / float(WHEEL_STEP);
		switch (e->modifiers())
		{
			//        case Qt::ControlModifier+Qt::ShiftModifier     : clipRatioFar  = math::Clamp( clipRatioFar*powf(1.1f, notch),0.01f,50000.0f); break;
			/*case Qt::ControlModifier: clipRatioNear = math::Clamp(clipRatioNear*powf(1.1f, notch), 0.01f, 500.0f); break;
			case Qt::ShiftModifier: fov = math::Clamp(fov + 1.2f*notch, 5.0f, 90.0f); break;
			case Qt::AltModifier: glas.pointSize = math::Clamp(glas.pointSize*powf(1.2f, notch), 0.01f, 150.0f);
			foreach(MeshModel * mp, this->md()->meshList)
			mp->glw.SetHintParamf(GLW::HNPPointSize, glas.pointSize);
			break;*/
		default:
			if (isRaster())
				this->opacity = math::Clamp(opacity*powf(1.2f, notch), 0.1f, 1.0f);
			else
				trackball.MouseWheel(e->delta() / float(WHEEL_STEP));
			break;
		}
	}
	//***20160322update();
}

void GLArea::mouseDoubleClickEvent(QMouseEvent * e)
{
	//***20150410 double click 關掉 
	/*hasToPick=true;
	pointToPick=Point2i(QT2VCG_X(this,e), QT2VCG_Y(this,e));
	update();*/

	if (md()->multiSelectID.size() > 0)
	{
		//trackball.Reset();
		/*float newScale= 3.0f/this->md()->bbox().Diag();
		trackball.track.sca = newScale;
		trackball.track.tra.Import(-this->md()->bbox().Center());*/
		//***20150410 要固定trackball
		//float newScale = 3.0f / printArea.Diag();
		//trackball.track.sca = newScale;
		trackball.track.tra.Import(-md()->mm()->cm.bbox.Center());

		trackball.track.sca = 450. / md()->mm()->cm.bbox.Diag();
		


		//trackball.current_mode = trackball.modes[vcg::Trackball::Button::BUTTON_RIGHT | vcg::Trackball::Button::KEY_CTRL];
		//trackball.SetPosition(md()->mm()->cm.bbox.Center());
		//trackball.center = (md()->mm()->cm.bbox.Center());
	}
	//float test = md()->mm()->cm.bbox.Diag();
	update();

}

void GLArea::focusInEvent(QFocusEvent * e)
{
	QWidget::focusInEvent(e);
	emit currentViewerChanged(id);
}

void GLArea::setCursorTrack(vcg::TrackMode *tm)
{
	//static QMap<QString,QCursor> curMap;
	if (curMap.isEmpty())
	{
		curMap[QString("")] = QCursor(Qt::ArrowCursor);
		curMap["SphereMode"] = QCursor(QPixmap(":/images/cursors/plain_trackball.png"), 1, 1);
		curMap["PanMode"] = QCursor(QPixmap(":/images/cursors/plain_pan.png"), 1, 1);
		curMap["ScaleMode"] = QCursor(QPixmap(":/images/cursors/plain_zoom.png"), 1, 1);
		curMap["PickMode"] = QCursor(QPixmap(":/images/cursors/plain_pick.png"), 1, 1);
	}
	if (tm) setCursor(curMap[tm->Name()]);
	else
	if (hasToGetPickPos) setCursor(curMap["PickMode"]);
	else setCursor(curMap[""]);

}

void GLArea::setDrawMode(vcg::GLW::DrawMode mode)
{
	for (QMap<int, RenderMode>::iterator it = rendermodemap.begin(); it != rendermodemap.end(); ++it)
		it.value().drawMode = mode;
	update();
}

void GLArea::setDrawMode(RenderMode& rm, vcg::GLW::DrawMode mode)
{
	rm.drawMode = mode;
	update();
}

//
void GLArea::setColorMode(vcg::GLW::ColorMode mode)
{
	for (QMap<int, RenderMode>::iterator it = rendermodemap.begin(); it != rendermodemap.end(); ++it)
		it.value().colorMode = mode;
	update();
}

void GLArea::setColorMode(RenderMode& rm, vcg::GLW::ColorMode mode)
{
	rm.colorMode = mode;
	update();
}

void GLArea::setTextureMode(vcg::GLW::TextureMode mode)
{
	for (QMap<int, RenderMode>::iterator it = rendermodemap.begin(); it != rendermodemap.end(); ++it)
		it.value().textureMode = mode;
	update();
}

void GLArea::setTextureMode(RenderMode& rm, vcg::GLW::TextureMode mode)
{
	rm.textureMode = mode;
	update();
}

void GLArea::updateTexture()
{
	hasToUpdateTexture = true;
}
void GLArea::setHastoRefreshVBO()
{
	hasToRefreshVBO = true;
}
void GLArea::setHastoUpdatePartVBO()
{
	hasToUpdatePartVBO = true;
}
void GLArea::setHasToRefreshDepth()
{
	hasToRefreshDepth = true;
}
void GLArea::reFreshGlListlist()
{
	hasToRefreshGlList = false;
}

// compute the next highest power of 2 of 32-bit v
int GLArea::RoundUpToTheNextHighestPowerOf2(unsigned int v)
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
/** \brief Manage the loading/allocation of the textures of the meshes

It is called at every redraw for within the paint event with an active gl context.

It assumes that:
- there is a single shared gl wrapper for each mesh for all the contexts (eg same texture id for different glareas),

- No attempt of avoiding texture duplication if two models share the same texture file there are two texture id.

- the values stored in the glwrapper for the texture id (glw.TMId()) is an indicator if there is the need of
loading a texture (empty vector means load that texture).

*/


void GLArea::initTexture(bool reloadAllTexture)
{
	if (reloadAllTexture) // ALL the texture must to be re-loaded, so we clear all the TMId vectors
	{
		foreach(MeshModel *mp, this->md()->meshList)
		{
			if (!mp->glw.TMId.empty())
			{
				glDeleteTextures(1, &(mp->glw.TMId[0]));
				mp->glw.TMId.clear();
			}
		}
		//***20150512清掉tex
		foreach(MeshModel *mp, this->md()->meshList)
		{
			//qDebug() << "clear" << getId();
			switch (getId())
			{
			case 0:
			{
					  if (!mp->glw.TMIdd[0].empty())
					  {
						  mp->glw.TMIdd[0].clear();
					  }
					  break;
			}


			case 1:
			{
					  if (!mp->glw.TMIdd[1].empty())
					  {
						  mp->glw.TMIdd[1].clear();
					  }
					  break;
			}

			case 2:
			{
					  if (!mp->glw.TMIdd[2].empty())
					  {
						  mp->glw.TMIdd[2].clear();
					  }
					  break;
			}


			}


		}
	}
	//qDebug() << "glArea_inittexture";
	size_t totalTextureNum = 0, toBeUpdatedNum = 0;
	foreach(MeshModel *mp, this->md()->meshList)
	{
		totalTextureNum += mp->cm.textures.size();
		//qDebug() << "total texture: " << totalTextureNum;
		if (!mp->cm.textures.empty() && mp->glw.TMId.empty()) toBeUpdatedNum++;
	}
	//qDebug() << "totalTextureNum=" << totalTextureNum;
	if (toBeUpdatedNum == 0) return;

	int singleMaxTextureSizeMpx = int(glas.maxTextureMemory / totalTextureNum);

	int singleMaxTextureSize = RoundUpToTheNextHighestPowerOf2(int(sqrt(float(singleMaxTextureSizeMpx))*1024.0)) / 2;
	//qDebug() << "singleMaxTextureSize" << singleMaxTextureSize;
	glEnable(GL_TEXTURE_2D);
	GLint MaxTextureSize;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &MaxTextureSize);
	if (singleMaxTextureSize < MaxTextureSize)
	{
		this->Logf(0, "There are too many textures (%i), reducing max texture size from %i to %i", totalTextureNum, MaxTextureSize, singleMaxTextureSize);
		MaxTextureSize = singleMaxTextureSize;
	}
	int iii = 0;
	foreach(MeshModel *mp, this->md()->meshList)
	{
		if (!mp->cm.textures.empty() && mp->glw.TMId.empty())
		{
			QString unexistingtext = "In mesh file <i>" + mp->fullName() + "</i> : Failure loading textures:<br>";
			bool sometextfailed = false;
			for (unsigned int i = 0; i < mp->cm.textures.size(); ++i)
			{
				QImage img, imgScaled, imgGL;
				bool res;
				/*if (mp->is3mf == false)
				{*/
				//QFileInfo fi(mp->cm.textures[i].c_str());
				//***20150513修正
				QFileInfo fi(mp->fullName());
				QDir fid = fi.dir();
				QString imgpath;
				QFileInfo texfileinfo(mp->cm.textures[i].c_str());
				if (texfileinfo.isRelative())
					imgpath = fid.absolutePath() + "/" + mp->cm.textures[i].c_str();
				else imgpath = mp->cm.textures[i].c_str();
				qDebug() << "mm full name: " << mp->fullName();
				qDebug() << "image path: " << imgpath;
				qDebug() << "fid: " << fid.absolutePath();
				res = img.load(imgpath);
				/*}
				else*/
				//{
				//	QString temppath = PicaApplication::getRoamingDir();
				//	//QString imgpath = temppath + "temptexture/" + mp->cm.textures[i].c_str();
				//	QString imgpath = mp->cm.textures[i].c_str();
				//	qDebug() << "image path: " << imgpath;
				//	res = img.load(imgpath);
				//}



				//*******************
				//qDebug() << "glarea_fi.absoluteFilePath()"<<fi.absoluteFilePath();
				sometextfailed = sometextfailed || !res;
				if (!res)
				{
					// Note that sometimes (in collada) the texture names could have been encoded with a url-like style (e.g. replacing spaces with '%20') so making some other attempt could be harmless
					QString ConvertedName = QString(mp->cm.textures[i].c_str()).replace(QString("%20"), QString(" "));
					res = img.load(ConvertedName);
					if (!res)
					{
						this->Logf(0, "Failure of loading texture %s", mp->cm.textures[i].c_str());
						unexistingtext += "<font color=red>" + QString(mp->cm.textures[i].c_str()) + "</font><br>";
					}
					else
						this->Logf(0, "Warning, texture loading was successful only after replacing %%20 with spaces;\n Loaded texture %s instead of %s", qPrintable(ConvertedName), mp->cm.textures[i].c_str());
					/*mp->glw.TMId.push_back(0);
					glGenTextures( 1, (GLuint*)&(mp->glw.TMId.back()) );
					glBindTexture( GL_TEXTURE_2D, mp->glw.TMId.back() );
					glTexImage2D( GL_TEXTURE_2D, 0, 3, 0, 0, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );*/
				}
				if (!res && QString(mp->cm.textures[i].c_str()).endsWith("dds", Qt::CaseInsensitive))
				{
					qDebug("DDS binding!");
					int newTexId = bindTexture(QString(mp->cm.textures[i].c_str()));
					mp->glw.TMId.push_back(newTexId);
				}
				if (!res)
					res = img.load(":/images/dummy.png");
				if (res)
				{
					// image has to be scaled to a 2^n size. We choose the first 2^N >= picture size.
					int bestW = RoundUpToTheNextHighestPowerOf2(img.width());
					int bestH = RoundUpToTheNextHighestPowerOf2(img.height());
					while (bestW >= MaxTextureSize) bestW /= 2;
					while (bestH >= MaxTextureSize) bestH /= 2;

					//Logf(GLLogStream::SYSTEM,"Loading textures");
					//Logf(GLLogStream::SYSTEM,"	Texture[ %3i ] =  '%s' ( %6i x %6i ) -> ( %6i x %6i )",	i,mp->cm.textures[i].c_str(), img.width(), img.height(),bestW,bestH);
					imgScaled = img.scaled(bestW, bestH, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

					imgGL = convertToGLFormat(imgScaled);
					//mp->glw.TMId.push_back(0);//***先給一個值
					//****20150402這邊要如何修改***不用改					
					//glGenTextures( 1, (GLuint*)&(mp->glw.TMId.back()) );//***把值拿來用，0會被改調
					//               glBindTexture( GL_TEXTURE_2D, mp->glw.TMId.back() );
					//qDebug() << mp->glw.TMId.back();
					//               qDebug("      	will be loaded as GL texture id %i  ( %i x %i )",mp->glw.TMId.back() ,imgGL.width(), imgGL.height());
					//               glTexImage2D( GL_TEXTURE_2D, 0, 3, imgGL.width(), imgGL.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, imgGL.bits() );
					//               gluBuild2DMipmaps(GL_TEXTURE_2D, 3, imgGL.width(), imgGL.height(), GL_RGBA, GL_UNSIGNED_BYTE, imgGL.bits() );
					//imgGL = imgGL.rgbSwapped().mirrored();
					//mp->glw.TMId.push_back(bindTexture(imgGL, GL_TEXTURE_2D));
					//***20150511				
					imgGL = imgGL.rgbSwapped().mirrored();
					//mesh_texture[iii] = bindTexture(imgGL, GL_TEXTURE_2D);					
					mp->glw.TMId.push_back(bindTexture(imgGL, GL_TEXTURE_2D));
					//mp->glw.TMId.push_back(mesh_texture[iii]);					


					//qDebug() << "mesh_texture[iii]=" << mesh_texture[iii];
					//qDebug() << "iii =" << iii << "mp->glw.TMId.back()=" << mp->glw.TMId.back();
						iii++;
					//******************
				}

				/*if(glas.textureMagFilter == 0 ) 	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
				else	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
				if(glas.textureMinFilter == 0 ) 	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				else	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );*/

				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			}
			mp->glw.viewerNum = getId();//設定viewer是哪一個
			std::vector<unsigned int> copymp(mp->glw.TMId);//複製TMId vector
			switch (getId())
			{
			case 0:
			{
					  //qDebug() << "mp->glw.viewerNum;" << mp->glw.viewerNum;
					  mp->glw.TMIdd[0] = copymp;
					  /*for (unsigned i = 0; i < copymp.size(); i++)
					  qDebug() << "TMIdd[0]" << mp->glw.TMIdd[0].at(i);*/

			}break;
			case 1:
			{
					  //qDebug() << "mp->glw.viewerNum;" << mp->glw.viewerNum;
					  mp->glw.TMIdd[1] = copymp;
					  /*for (unsigned i = 0; i < copymp.size(); i++)
					  qDebug() << "TMIdd[1]" << mp->glw.TMIdd[1].at(i);*/

			}break;
			case 2:
			{
					  //qDebug() << "mp->glw.viewerNum;" << mp->glw.viewerNum;
					  mp->glw.TMIdd[2] = copymp;
					  /*for (unsigned i = 0; i < copymp.size(); i++)
					  qDebug() << "TMIdd[2]" << mp->glw.TMIdd[2].at(i);*/

			} break;
			}
			/*if (sometextfailed)
			QMessageBox::warning(this, "Texture files has not been correctly loaded", unexistingtext);*/

		}
	}
	glDisable(GL_TEXTURE_2D);

	//reFreshGlListlist();
	//mw()->testFuncFunc();
}


void GLArea::setLight(bool state)
{
	for (QMap<int, RenderMode>::iterator it = rendermodemap.begin(); it != rendermodemap.end(); ++it)
		it.value().lighting = state;
	update();
}

void GLArea::setLightMode(bool state, LightingModel lmode)
{
	for (QMap<int, RenderMode>::iterator it = rendermodemap.begin(); it != rendermodemap.end(); ++it)
	{
		switch (lmode)
		{
		case LDOUBLE:		it.value().doubleSideLighting = state;	break;
		case LFANCY:		it.value().fancyLighting = state; break;
		}
	}
	update();
}

void GLArea::setBackFaceCulling(bool enabled)
{
	for (QMap<int, RenderMode>::iterator it = rendermodemap.begin(); it != rendermodemap.end(); ++it)
		it.value().backFaceCull = enabled;
	update();
}

void GLArea::setSelectFaceRendering(bool enabled)
{
	if (md()->mm() != NULL)
	{
		QMap<int, RenderMode>::iterator it = rendermodemap.find(md()->mm()->id());
		if (it != rendermodemap.end())
			it.value().selectedFace = enabled;
	}
	update();
}
void GLArea::setSelectVertRendering(bool enabled)
{
	if (md()->mm() != NULL)
	{
		QMap<int, RenderMode>::iterator it = rendermodemap.find(md()->mm()->id());
		if (it != rendermodemap.end())
			it.value().selectedVert = enabled;
	}
	update();
}

void GLArea::setLightModel(RenderMode& rm)
{
	if (rm.lighting)
	{
		glEnable(GL_LIGHTING);

		if (rm.doubleSideLighting)
			glEnable(GL_LIGHT1);
		else
			glDisable(GL_LIGHT1);

		glLightfv(GL_LIGHT0, GL_AMBIENT, Color4f::Construct(glas.baseLightAmbientColor).V());
		glLightfv(GL_LIGHT0, GL_DIFFUSE, Color4f::Construct(glas.baseLightDiffuseColor).V());
		glLightfv(GL_LIGHT0, GL_SPECULAR, Color4f::Construct(glas.baseLightSpecularColor).V());

		glLightfv(GL_LIGHT1, GL_AMBIENT, Color4f::Construct(glas.baseLightAmbientColor).V());
		glLightfv(GL_LIGHT1, GL_DIFFUSE, Color4f::Construct(glas.baseLightDiffuseColor).V());
		glLightfv(GL_LIGHT1, GL_SPECULAR, Color4f::Construct(glas.baseLightSpecularColor).V());
		if (rm.fancyLighting)
		{
			glLightfv(GL_LIGHT0, GL_DIFFUSE, Color4f::Construct(glas.fancyFLightDiffuseColor).V());
			glLightfv(GL_LIGHT1, GL_DIFFUSE, Color4f::Construct(glas.fancyBLightDiffuseColor).V());
		}
	}
	else glDisable(GL_LIGHTING);
}

void GLArea::setView()
{
	//***20150824***
	if (int(fov) != 6)glViewport(0, 0, (GLsizei)QTLogicalToDevice(this, width()), (GLsizei)QTLogicalToDevice(this, height()));
	else
	{
		//qDebug() << "fov" << fov;
		glViewport(0, 0, md()->groove.DimX() / md()->groove.DimY() *height(), height());
	}
	//glViewport((GLsizei)QTLogicalToDevice(this, width())*0.33, 0, (GLsizei)QTLogicalToDevice(this, width())*0.67, (GLsizei)QTLogicalToDevice(this, height()));


	GLfloat fAspect = (GLfloat)width() / height();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	Matrix44f mtTr; mtTr.SetTranslate(trackball.center);
	Matrix44f mtSc; mtSc.SetScale(4.0f, 4.0f, 4.0f);
	Matrix44f mt = mtSc * mtTr * trackball.Matrix() *(-mtTr);
	//    Matrix44f mt =  trackball.Matrix();

	Box3m bb;
	bb.Add(Matrix44m::Construct(mt), this->md()->bbox());
	float cameraDist = this->getCameraDistance();
	/*******************test*/
	//Point3m viewWPoint = trackball.camera.ViewPoint();
	//trackball.track.sca *= 5;
	/********************edntest*/
	if (fov == 5)
		cameraDist = 3.0f; // small hack for orthographic projection where camera distance is rather meaningless...

	nearPlane = cameraDist*clipRatioNear;
	//farPlane = cameraDist + max(viewRatio(),float(-bb.min[2]));
	//***20150409
	farPlane = viewDistance;
	if (nearPlane <= cameraDist*.1f) nearPlane = cameraDist*.1f;

	//qDebug("tbcenter %f %f %f",trackball.center[0],trackball.center[1],trackball.center[2]);
	//qDebug("camera dist %f far  %f",cameraDist, farPlane);
	//qDebug("Bb %f %f %f - %f %f %f", bb.min[0], bb.min[1], bb.min[2], bb.max[0], bb.max[1], bb.max[2]);

	if (!takeSnapTile)
	{
		//***20150422加scale
		float ortho_scale = (md()->groove.DimX() + md()->groove.DimY() + md()->groove.DimZ()) / 6.0;
		//qDebug() << ortho_scale;
		//qDebug() << fov;
		if (int(fov) == 5)
		{
			//glOrtho(-viewRatio()*fAspect*printArea.DimX()*0.5, viewRatio()*fAspect*printArea.DimX()*0.5, -viewRatio()*printArea.DimY()*0.5, viewRatio()*printArea.DimY()*0.5, -100, 100);
			glOrtho(-viewRatio()*fAspect*ortho_scale, viewRatio()*fAspect*ortho_scale, -viewRatio()*ortho_scale, viewRatio()*ortho_scale, -viewDistance, viewDistance);

			//glOrtho(-15, 15, -15, 15, -100, 100);
			//qDebug(" glOrtho= %f  %f  %f", -viewRatio()*printArea.DimY()*0.5, -viewRatio()*fAspect*printArea.DimY()*0.5, printArea.DimX() );
		}
		else if (int(fov) == 6)
		{
			glOrtho(-md()->groove.groovelength / 2., md()->groove.groovelength / 2., -md()->groove.grooveheight / 2., md()->groove.grooveheight / 2., -viewDistance, viewDistance);
		}
		else
		{

			//gluPerspective(fov, fAspect, nearPlane, farPlane);
			gluPerspective(60, fAspect, 0.01, farPlane);
			//qDebug() << "fov" << fov;
			//qDebug("gluPerspective");
		}
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		//gluLookAt(0, -300, 0, 0, 0, 0, 0, 0, 1);
	}
	else
	{
		setTiledView(fov, viewRatio(), fAspect, -viewDistance, viewDistance, cameraDist);
		//qDebug("setTiledView");
	}

	//***20150428**註解掉modelview
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	//gluLookAt(0, 0, cameraDist, 0, 0, 0, 0, 1, 0);
	//gluLookAt(0, 0, -50, 0, 0, 0, 0, 1, 0);
}
void GLArea::setXYView()
{
	//glViewport(0, 0, (GLsizei)QTLogicalToDevice(this, width()) , (GLsizei)QTLogicalToDevice(this, height()) );
	glViewport((GLsizei)QTLogicalToDevice(this, width())*0.33, 0, (GLsizei)QTLogicalToDevice(this, width())*0.67, (GLsizei)QTLogicalToDevice(this, height()));


	GLfloat fAspect = (GLfloat)(width()*0.67) / height();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	Matrix44f mtTr; mtTr.SetTranslate(trackball.center);
	Matrix44f mtSc; mtSc.SetScale(4.0f, 4.0f, 4.0f);
	Matrix44f mt = mtSc * mtTr * trackball.Matrix() *(-mtTr);
	//    Matrix44f mt =  trackball.Matrix();

	Box3m bb;
	bb.Add(Matrix44m::Construct(mt), this->md()->bbox());
	float cameraDist = this->getCameraDistance();

	if (fov == 5) cameraDist = 3.0f; // small hack for orthographic projection where camera distance is rather meaningless...
	//***20150428
	nearPlane = cameraDist*clipRatioNear;
	//farPlane = cameraDist + max(viewRatio(), float(-bb.min[2]));

	if (nearPlane <= cameraDist*.1f) nearPlane = cameraDist*.1f;

	//qDebug("tbcenter %f %f %f",trackball.center[0],trackball.center[1],trackball.center[2]);
	//qDebug("camera dist %f far  %f",cameraDist, farPlane);
	//qDebug("Bb %f %f %f - %f %f %f", bb.min[0], bb.min[1], bb.min[2], bb.max[0], bb.max[1], bb.max[2]);

	if (!takeSnapTile)
	{
		if (fov == 5)
		{
			glOrtho(-viewRatio()*fAspect, viewRatio()*fAspect, -viewRatio(), viewRatio(), nearPlane, farPlane);
		}
		else		gluPerspective(fov, fAspect, nearPlane, farPlane);
	}
	else	setTiledView(fov, viewRatio(), fAspect, nearPlane, farPlane, cameraDist);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, cameraDist, 0, 0, 0, 0, 1, 0);
}

void GLArea::setTiledView(GLdouble fovY, float viewRatio, float fAspect, GLdouble zNear, GLdouble zFar, float cameraDist)
{
	if (fovY <= 5)
	{
		GLdouble fLeft = -viewRatio*fAspect;
		GLdouble fRight = viewRatio*fAspect;
		GLdouble fBottom = -viewRatio;
		GLdouble fTop = viewRatio;

		GLdouble tDimX = fabs(fRight - fLeft) / totalCols;
		GLdouble tDimY = fabs(fTop - fBottom) / totalRows;
		//***20150422加scale

		float ortho_scale = (md()->groove.DimX() + md()->groove.DimX() + md()->groove.DimX()) / 6.0;

		//glOrtho(fLeft   + tDimX * tileCol, fLeft   + tDimX * (tileCol+1),     /* left, right */
		//    fBottom + tDimY * tileRow, fBottom + tDimY * (tileRow+1),     /* bottom, top */
		//    cameraDist - 2.f*clipRatioNear, cameraDist+2.f*clipRatioFar);

		glOrtho(-1.75*fAspect*ortho_scale, 1.75*fAspect*ortho_scale, -1.75*ortho_scale, 1.75*ortho_scale, -100, 100);
		//***
		//glOrtho(-fAspect*md()->groove.DimX() / 2.0, fAspect*md()->groove.DimX() / 2.0, -md()->groove.DimY() / 2.0, md()->groove.DimY() / 2.0, -100, 100);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		//gluLookAt(0, 10, 0, 0, 0, 0, 0, 0, -1);
		//qDebug() << "lookat";
	}
	else
	{
		GLdouble fTop = zNear * tan(math::ToRad(fovY / 2.0));
		GLdouble fRight = fTop * fAspect;
		GLdouble fBottom = -fTop;
		GLdouble fLeft = -fRight;

		// tile Dimension
		GLdouble tDimX = fabs(fRight - fLeft) / totalCols;
		GLdouble tDimY = fabs(fTop - fBottom) / totalRows;

		glFrustum(fLeft + tDimX * tileCol, fLeft + tDimX * (tileCol + 1),
			fBottom + tDimY * tileRow, fBottom + tDimY * (tileRow + 1), zNear, zFar);
	}
}

void GLArea::updateFps(float deltaTime)
{
	static float fpsVector[10];
	static int j = 0;
	float averageFps = 0;
	if (deltaTime > 0) {
		fpsVector[j] = deltaTime;
		j = (j + 1) % 10;
	}
	for (int i = 0; i < 10; i++) averageFps += fpsVector[i];
	cfps = 1000.0f / (averageFps / 10);
	lastTime = deltaTime;
}

void GLArea::resetTrackBall()
{
	trackball.modes[Trackball::WHEEL] = new ScaleMode();
	if (int(fov) == 5 && vd == top)
	{
		createOrthoView("Top");
		update();
		//qDebug() << "Top";
	}
	else if ((int(fov) == 5 && vd == front))
	{
		createOrthoView("Front");
		update();
		//qDebug() << "Front" << fov;
	}
	else if (int(fov) == 5 && vd == bottom)
	{
		//qDebug() << "Front2" << fov;
		createOrthoView("Bottom");
		//qDebug() << "Front2" << fov;
		update();
		//qDebug() << "Front2" << fov;
	}
	else if (int(fov) == 6 && vd == topTag)
	{
		//createOrthoView("Front");
		createOrthoView("TopTag");
		trackball.modes[Trackball::WHEEL] = 0;
		update();
	}
	else if (int(fov) == 5 && vd == original)
	{
		createOrthoView("Original");
		update();
	}
	else
	{

		trackball.Reset();
		float newScale = 3.0f / this->md()->bbox().Diag();
		trackball.track.sca = newScale;
		trackball.track.tra.Import(-this->md()->bbox().Center());
		clipRatioNear = clipRatioNearDefault();
		fov = fovDefault();
		update();
		//qDebug() << "Front""Front" << fov;
	}

}

void GLArea::hideEvent(QHideEvent * /*event*/)
{
	trackball.current_button = 0;
}

void GLArea::sendViewPos(QString name)
{
#ifndef VCG_USE_EIGEN
	Point3f pos = trackball.track.InverseMatrix() *Inverse(trackball.camera.model) *Point3f(0, 0, 0);
#else
	Point3f pos = Eigen::Transform3f(trackball.track.InverseMatrix()) * Eigen::Transform3f(Inverse(trackball.camera.model)).translation();
#endif
	emit transmitViewPos(name, pos);
}

void GLArea::sendSurfacePos(QString name)
{
	qDebug("sendSurfacePos %s", qPrintable(name));
	nameToGetPickPos = name;
	hasToGetPickPos = true;
}

void GLArea::sendViewDir(QString name)
{
	Point3f dir = getViewDir();
	emit transmitViewDir(name, dir);
}

void GLArea::sendMeshShot(QString name)
{
	Shotm curShot = this->md()->mm()->cm.shot;
	emit transmitShot(name, curShot);
}

void GLArea::sendMeshMatrix(QString name)
{
	Matrix44m mat = this->md()->mm()->cm.Tr;
	emit transmitMatrix(name, mat);
}

void GLArea::sendViewerShot(QString name)
{
	Shotm curShot = shotFromTrackball().first;
	emit transmitShot(name, curShot);
}
void GLArea::sendRasterShot(QString name)
{
	Shotm curShot;
	if (this->md()->rm() != NULL)
		curShot = this->md()->rm()->shot;
	emit transmitShot(name, curShot);
}

void GLArea::sendCameraPos(QString name)
{
	Point3f dir = trackball.camera.ViewPoint();
	emit transmitCameraPos(name, dir);
}

Point3f GLArea::getViewDir()
{
	vcg::Matrix44f rotM;
	trackball.track.rot.ToMatrix(rotM);
	return vcg::Inverse(rotM)*vcg::Point3f(0, 0, 1);
}

void GLArea::updateCustomSettingValues(RichParameterSet& rps)
{
	glas.updateGlobalParameterSet(rps);
	updateMeshDocRelatedSetting(rps);

	this->update();
}
void GLArea::updateMeshDocRelatedSetting(RichParameterSet &mrps)
{
	md()->p_setting.gd_unit = (slice_program_setting::grid_unit)mrps.getEnum("DISPLAY_UNIT");
	int targetPrinter = mrps.getEnum("TARGET_PRINTER");
	switch (targetPrinter)
	{
	case 0:
		/*md()->groove.setLWH(DSP_grooveX, DSP_grooveZ, DSP_grooveY);
		break;*/
	case 1:
		md()->groove.setLWH(DSP_grooveBigX, DSP_grooveZ, DSP_grooveY);
		break;
	case 2:
		md()->groove.setLWH(20, 20, 20);
		break;

	}
	Label_Float temp = md()->p_setting.getbuildheight();
	temp.value = mrps.getFloat("SLIGHT_HEIGHT");
	md()->p_setting.setbuildheight(temp);

	md()->p_setting.setBetweenGap(mrps.getFloat("AUTO_PACKING_GAP"));

	md()->p_setting.setLeftGap(mrps.getFloat("GROOVE_GAP_L"));
	md()->p_setting.setRightGap(mrps.getFloat("GROOVE_GAP_R"));
	md()->p_setting.setTopGap(mrps.getFloat("GROOVE_GAP_T"));
	md()->p_setting.setBottomGap(mrps.getFloat("GROOVE_GAP_B"));

	md()->p_setting.set_useless_print(mrps.getFloat("SACRIFICE_WIDTH"));
	md()->p_setting.set_print_or_not(mrps.getBool("SYNCHRONIZE_PRINTING"));
	md()->p_setting.print_pagesize = (slice_program_setting::page_size)mrps.getEnum("PAGE_SIZE");
	md()->p_setting.set_add_pattern(mrps.getBool("ADD_PATTERN"));
	md()->p_setting.set_plus_print_length(mrps.getFloat("PLUS_PRINT_LENGTH"));
	md()->p_setting.set_start_print_page(mrps.getInt("START_PRINT_PAGE"));
	mw()->fastRenderSwitch = mrps.getBool("FAST_RENDERING");
	mw()->setFcNumberLimit(mrps.getInt("TOTAL_FACET"));
	if (mw()->getFCNumberLimit() < md()->fn() && mw()->fastRenderSwitch)
		mw()->setMdNumLimit(true);
	else
		mw()->setMdNumLimit(false);

	qDebug() << " mrps.getEnum(\"PAGE_SIZE\")" << mrps.getEnum("PAGE_SIZE");
}

void GLArea::initGlobalParameterSet(RichParameterSet * defaultGlobalParamSet)
{
	GLAreaSetting::initGlobalParameterSet(defaultGlobalParamSet);
}

//Don't alter the state of the other elements in the visibility map
void GLArea::updateMeshSetVisibilities()
{
	meshVisibilityMap.clear();
	foreach(MeshModel * mp, this->md()->meshList)
	{
		//Insert the new pair in the map; If the key is already in the map, its value will be overwritten
		meshVisibilityMap.insert(mp->id(), mp->visible);
	}

	if (mw()->getRadarModeSwitch())
	{
		setHasToRefreshDepth();
	}

}

//Don't alter the state of the other elements in the visibility map
void GLArea::updateRasterSetVisibilities()
{
	//Align rasterVisibilityMap state with rasterList state
	//Deleting from the map the visibility of the deleted rasters
	QMapIterator<int, bool> i(rasterVisibilityMap);
	while (i.hasNext()) {
		i.next();
		bool found = false;
		foreach(RasterModel * rp, this->md()->rasterList)
		{
			if (rp->id() == i.key())
			{
				found = true;
				break;
			}
		}
		if (!found)
			rasterVisibilityMap.remove(i.key());
	}

	foreach(RasterModel * rp, this->md()->rasterList)
	{
		//Insert the new pair in the map;If the key is already in the map, its value will be overwritten
		rasterVisibilityMap.insert(rp->id(), rp->visible);
	}
}

void GLArea::meshSetVisibility(MeshModel *mp, bool visibility)
{
	mp->visible = visibility;
	meshVisibilityMap[mp->id()] = visibility;
}

void GLArea::addRasterSetVisibility(int rasterId, bool visibility)
{
	rasterVisibilityMap.insert(rasterId, visibility);
}

// --------------- Raster view -------------------------------------
void GLArea::setIsRaster(bool viewMode) {
	_isRaster = viewMode;
}

// this slot is called when someone press the showraster button on the toolbar
void GLArea::showRaster(bool resetViewFlag)
{
	if (!this->isRaster())
	{
		lastViewBeforeRasterMode = this->viewToText();
		setIsRaster(true);
		loadRaster(md()->rm()->id());
	}
	else
	{
		this->setIsRaster(false);
		QDomDocument doc("StringDoc");
		doc.setContent(lastViewBeforeRasterMode);
		if (resetViewFlag) this->loadViewFromViewStateFile(doc);
		else this->update();
	}
}

void GLArea::loadRaster(int id)
{
	foreach(RasterModel *rm, this->md()->rasterList)
	if (rm->id() == id)
	{
		this->md()->setCurrentRaster(id);
		if (rm->currentPlane->image.isNull())
		{
			Logf(0, "Image file %s has not been correctly loaded, a fake image is going to be shown.", rm->currentPlane->fullPathFileName.toUtf8().constData());
			rm->currentPlane->image.load(":/images/dummy.png");
		}
		setTarget(rm->currentPlane->image);
		//load his shot or a default shot

		if (rm->shot.IsValid())
		{
			fov = rm->shot.GetFovFromFocal();

			float cameraDist = getCameraDistance();
			Matrix44f rotFrom;
			rm->shot.Extrinsics.Rot().ToMatrix(rotFrom);

			Point3f p1 = rotFrom*(vcg::Point3f::Construct(rm->shot.Extrinsics.Tra()));

			Point3f p2 = (Point3f(0, 0, cameraDist));

			trackball.track.sca = fabs(p2.Z() / p1.Z());
			loadShot(QPair<Shotm, float>(rm->shot, trackball.track.sca));
		}
		else
			createOrthoView("Front");
	}
}

void GLArea::drawTarget()
{
	if (!targetTex) return;

	if (this->md()->rm() == 0) return;
	QImage &curImg = this->md()->rm()->currentPlane->image;
	float imageRatio = float(curImg.width()) / float(curImg.height());
	float screenRatio = float(this->width()) / float(this->height());
	//set orthogonal view
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(-1.0f*screenRatio, 1.0f*screenRatio, -1, 1);

	glColor4f(1, 1, 1, opacity);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, targetTex);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);	//first point
	glVertex3f(-1.0f*imageRatio, -1.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);	//second point
	glVertex3f(1.0f*imageRatio, -1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);	//third point
	glVertex3f(1.0f*imageRatio, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);	//fourth point
	glVertex3f(-1.0f*imageRatio, 1.0f, 0.0f);
	glEnd();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	//restore view
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}


void GLArea::setTarget(QImage &image) {
	if (image.isNull())
		return;
	if (targetTex) {
		glDeleteTextures(1, &targetTex);
		targetTex = 0;
	}
	// create texture
	glGenTextures(1, &targetTex);
	QImage tximg = QGLWidget::convertToGLFormat(image);
	glBindTexture(GL_TEXTURE_2D, targetTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, tximg.width(), tximg.height(),
		0, GL_RGBA, GL_UNSIGNED_BYTE, tximg.bits());

	glBindTexture(GL_TEXTURE_2D, 0);
}

// --------------- Methods involving shots -------------------------------------

float GLArea::getCameraDistance()
{
	float cameraDist = viewRatio() / tanf(vcg::math::ToRad(fov*.5f));
	return cameraDist;
}

void GLArea::initializeShot(Shotm &shot)
{
	shot.Intrinsics.PixelSizeMm[0] = 0.036916077;/// Dimension in mm of a single pixel，1 pixel為幾公釐
	shot.Intrinsics.PixelSizeMm[1] = 0.036916077;

	shot.Intrinsics.DistorCenterPx[0] = width() / 2;
	shot.Intrinsics.DistorCenterPx[1] = height() / 2;
	shot.Intrinsics.CenterPx[0] = width() / 2;
	shot.Intrinsics.CenterPx[1] = height() / 2;
	shot.Intrinsics.ViewportPx[0] = width();
	shot.Intrinsics.ViewportPx[1] = height();

	double viewportYMm = shot.Intrinsics.PixelSizeMm[1] * shot.Intrinsics.ViewportPx[1];//Y方向viewport的長度mm
	shot.Intrinsics.FocalMm = viewportYMm / (2 * tanf(vcg::math::ToRad(this->fovDefault() / 2.0f))); //27.846098mm

	shot.Extrinsics.SetIdentity();
}

bool GLArea::viewFromFile()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("Load Project"), "./", tr("Xml Files (*.xml)"));

	QFile qf(filename);
	QFileInfo qfInfo(filename);

	if (!qf.open(QIODevice::ReadOnly))
		return false;

	QString project_path = qfInfo.absoluteFilePath();

	QDomDocument doc("XmlDocument");    //It represents the XML document
	if (!doc.setContent(&qf))
		return false;

	QString type = doc.doctype().name();

	//TextAlign file project
	if (type == "RegProjectML")   loadShotFromTextAlignFile(doc);
	//View State file
	else if (type == "ViewState") loadViewFromViewStateFile(doc);

	qDebug("End file reading");
	qf.close();

	return true;
}

void GLArea::loadShotFromTextAlignFile(const QDomDocument &doc)
{
	QDomElement root = doc.documentElement();
	QDomNode node;
	Shotm shot;

	node = root.firstChild();

	//Devices
	while (!node.isNull()) {
		if (QString::compare(node.nodeName(), "Device") == 0)
		{
			QString type = node.attributes().namedItem("type").nodeValue();
			if (type == "GlImageWidget")
			{
				//Aligned Image
				if (QString::compare(node.attributes().namedItem("aligned").nodeValue(), "1") == 0) {
					QDomNode nodeb = node.firstChild();
					ReadShotFromOLDXML(shot, nodeb);
				}
			}
		}
		node = node.nextSibling();
	}

	//Adjust params for Meshlab settings

	//resize viewport
	int w = shot.Intrinsics.ViewportPx[0];
	int h = shot.Intrinsics.ViewportPx[1];

	shot.Intrinsics.DistorCenterPx[0] = w / 2;
	shot.Intrinsics.DistorCenterPx[1] = h / 2;
	shot.Intrinsics.CenterPx[0] = w / 2;
	shot.Intrinsics.CenterPx[1] = h / 2;
	shot.Intrinsics.ViewportPx[0] = w;
	shot.Intrinsics.ViewportPx[1] = h;

	// The shot loaded from TextAlign doesn't have a scale. Trackball needs it.
	// The scale factor is computed as the ratio between cameraDistance and the z coordinate of the translation
	// introduced by the shot.
	fov = shot.GetFovFromFocal();

	float cameraDist = getCameraDistance();

	Matrix44f rotFrom;
	shot.Extrinsics.Rot().ToMatrix(rotFrom);

	Point3f p1 = rotFrom*(vcg::Point3f::Construct(shot.Extrinsics.Tra()));

	Point3f p2 = (Point3f(0, 0, cameraDist));

	trackball.track.sca = fabs(p2.Z() / p1.Z());

	loadShot(QPair<Shotm, float>(shot, trackball.track.sca));

}

/*
ViewState file is an xml file format created by Meshlab with the action "copyToClipboard"
*/
void GLArea::loadViewFromViewStateFile(const QDomDocument &doc)
{
	Shotm shot;
	QDomElement root = doc.documentElement();
	QDomNode node = root.firstChild();

	while (!node.isNull())
	{
		if (QString::compare(node.nodeName(), "VCGCamera") == 0)
			ReadShotFromQDomNode(shot, node);
		else if (QString::compare(node.nodeName(), "CamParam") == 0)
			ReadShotFromOLDXML(shot, node);

		else if (QString::compare(node.nodeName(), "ViewSettings") == 0)
		{
			QDomNamedNodeMap attr = node.attributes();
			trackball.track.sca = attr.namedItem("TrackScale").nodeValue().section(' ', 0, 0).toFloat();
			nearPlane = attr.namedItem("NearPlane").nodeValue().section(' ', 0, 0).toFloat();
			farPlane = attr.namedItem("FarPlane").nodeValue().section(' ', 0, 0).toFloat();
			fov = shot.GetFovFromFocal();
			clipRatioNear = nearPlane / getCameraDistance();
			clipRatioFar = farPlane / getCameraDistance();

		}
		/*else if (QString::compare(node.nodeName(),"Render")==0)
		{
		QDomNamedNodeMap attr = node.attributes();
		rm.drawMode = (vcg::GLW::DrawMode) (attr.namedItem("DrawMode").nodeValue().section(' ',0,0).toInt());
		rm.colorMode = (vcg::GLW::ColorMode) (attr.namedItem("ColorMode").nodeValue().section(' ',0,0).toInt());
		rm.textureMode = (vcg::GLW::TextureMode) (attr.namedItem("TextureMode").nodeValue().section(' ',0,0).toInt());
		rm.lighting = (attr.namedItem("Lighting").nodeValue().section(' ',0,0).toInt() != 0);
		rm.backFaceCull = (attr.namedItem("BackFaceCull").nodeValue().section(' ',0,0).toInt() != 0);
		rm.doubleSideLighting = (attr.namedItem("DoubleSideLighting").nodeValue().section(' ',0,0).toInt() != 0);
		rm.fancyLighting = (attr.namedItem("FancyLighting").nodeValue().section(' ',0,0).toInt() != 0);
		rm.selectedFace = (attr.namedItem("SelectedFace").nodeValue().section(' ',0,0).toInt() != 0);
		rm.selectedVert = (attr.namedItem("SelectedVert").nodeValue().section(' ',0,0).toInt() != 0);
		}*/
		node = node.nextSibling();
	}

	loadShot(QPair<Shotm, float>(shot, trackball.track.sca));
}
QString GLArea::viewToText()
{
	QString docString;

	Shotm shot = shotFromTrackball().first;

	QDomDocument doc("ViewState");
	QDomElement root = doc.createElement("project");
	doc.appendChild(root);

	QDomElement shotElem = WriteShotToQDomNode(shot, doc);
	root.appendChild(shotElem);

	QDomElement settingsElem = doc.createElement("ViewSettings");
	settingsElem.setAttribute("TrackScale", trackball.track.sca);
	settingsElem.setAttribute("NearPlane", nearPlane);
	settingsElem.setAttribute("FarPlane", farPlane);
	root.appendChild(settingsElem);

	/*QDomElement renderElem = doc.createElement( "Render");
	renderElem.setAttribute("DrawMode",rm.drawMode);
	renderElem.setAttribute("ColorMode",rm.colorMode);
	renderElem.setAttribute("TextureMode",rm.textureMode);
	renderElem.setAttribute("Lighting",rm.lighting);
	renderElem.setAttribute("BackFaceCull",rm.backFaceCull);
	renderElem.setAttribute("DoubleSideLighting",rm.doubleSideLighting);
	renderElem.setAttribute("FancyLighting",rm.fancyLighting);
	renderElem.setAttribute("SelectedFace",rm.selectedFace);
	renderElem.setAttribute("SelectedVert",rm.selectedVert);
	root.appendChild(renderElem);*/

	return doc.toString();
}

void GLArea::viewToClipboard()
{
	QApplication::clipboard()->setText(this->viewToText());
}

void GLArea::viewFromClipboard()
{
	QClipboard *clipboard = QApplication::clipboard();
	QString shotString = clipboard->text();
	QDomDocument doc("StringDoc");
	doc.setContent(shotString);
	loadViewFromViewStateFile(doc);
}

QPair<Shotm, float> GLArea::shotFromTrackball()
{
	Shotm shot;
	initializeShot(shot);

	double viewportYMm = shot.Intrinsics.PixelSizeMm[1] * shot.Intrinsics.ViewportPx[1];
	shot.Intrinsics.FocalMm = viewportYMm / (2 * tanf(vcg::math::ToRad(fov / 2)));

	float cameraDist = getCameraDistance();

	//add the translation introduced by gluLookAt() (0,0,cameraDist), in order to have te same view---------------
	//T(gl)*S*R*T(t) => SR(gl+t) => S R (S^(-1)R^(-1)gl + t)
	//Add translation S^(-1) R^(-1)(gl)
	//Shotd doesn't introduce scaling
	//---------------------------------------------------------------------
	shot.Extrinsics.SetTra(shot.Extrinsics.Tra() + (Inverse(shot.Extrinsics.Rot())*Point3m(0, 0, cameraDist)));

	Shotm newShot = track2ShotCPU<Shotm::ScalarType>(shot, &trackball);

	return QPair<Shotm, float>(newShot, trackball.track.sca);
}
void GLArea::viewFromCurrentShot(QString kind)
{
	Shotm localShot;
	if (kind == "Mesh" && this->md()->mm())   localShot = this->md()->mm()->cm.shot;
	if (kind == "Raster" && this->md()->rm()) localShot = this->md()->rm()->shot;
	if (!localShot.IsValid())
	{
		this->Logf(GLLogStream::SYSTEM, "Unable to set Shot from current %s", qPrintable(kind));
		return;
	}

	loadShot(QPair<Shotm, float>(localShot, trackball.track.sca));
}


void GLArea::loadShot(const QPair<Shotm, float> &shotAndScale) {

	Shotm shot = shotAndScale.first;
	//qDebug() << "Front3" << shotAndScale.second;
	//qDebug() << "Front3" << fov;
	fov = shot.GetFovFromFocal();
	//***20150522***qDebug() << "Front3" << fov;
	float cameraDist = getCameraDistance();

	//reset trackball. The point of view must be set only by the shot
	trackball.Reset();
	trackball.track.sca = shotAndScale.second;//scale


	/*Point3f point = this->md()->bbox().Center();
	Point3f p1 = ((trackball.track.Matrix()*(point-trackball.center))- Point3f(0,0,cameraDist));*/
	shot2Track(shot, cameraDist, trackball);//更新trackball


	//Expressing the translation along Z with a scale factor k
	//Point3f p2 = ((trackball.track.Matrix()*(point-trackball.center))- Point3f(0,0,cameraDist));

	////k is the ratio between the distances along z of two correspondent points (before and after the traslation)
	////from the point of view
	//float k= abs(p2.Z()/p1.Z());

	//float sca= trackball.track.sca/k;
	//Point3f tra = trackball.track.tra;
	//
	//// Apply this formula:
	//// SR(t+p) -v = k[S'R'(t'+p) -v] forall p, R=R', k is a costant
	//// SR(t) -v = k[S'R(t') -v]
	//// t' = 1/k* S'^-1St + (k-1)/k S'^-1*R^-1v
	//Matrix44f s0 = Matrix44f().SetScale(trackball.track.sca,trackball.track.sca, trackball.track.sca);
	//Matrix44f s1 = Matrix44f().SetScale(sca, sca, sca);
	//Matrix44f r;
	//trackball.track.rot.ToMatrix(r);
	//Matrix44f rapM = Matrix44f().SetScale(1/k, 1/k, 1/k);
	//Matrix44f rapM2 = Matrix44f().SetScale(1-1/k, 1-1/k, 1-1/k);
	//Point3f t1 = rapM*Inverse(s1)*s0*tra + rapM2*Inverse(s1)*Inverse(r)*Point3f(0,0,cameraDist);

	//trackball.track.sca =sca;
	//trackball.track.tra =t1 /*+ tb.track.rot.Inverse().TB495076(glLookAt)*/ ;
	update();

}

void GLArea::createOrthoView(QString dir)//設置orthoView，傳入string ("top, bottom, left, right, front , back")
{
	Shotm view;
	initializeShot(view);

	//***20150824
	//fov =5;//set orthoView的訊號	

	double viewportYMm = view.Intrinsics.PixelSizeMm[1] * view.Intrinsics.ViewportPx[1];
	//每一個pixelsize多少釐米，乘以多少個pixel，算出來為Y方向總共有多少釐米
	view.Intrinsics.FocalMm = viewportYMm / (2 * tanf(vcg::math::ToRad(fov / 2))); //27.846098 equivalente a circa 60 gradi
	//算出camera到viewortPlane的距離 tan(30)=(viewportYMm/2)/FocalMm

	trackball.Reset();
	/*float newScale= 3.0f/this->md()->bbox().Diag();
	trackball.track.sca = newScale;
	trackball.track.tra.Import(-this->md()->bbox().Center());*/
	//***20150410 要固定trackball
	//float newScale = 3.0f / printArea.Diag();
	//trackball.track.sca = newScale;
	trackball.track.tra.Import(md()->groove.Center());
	//****************	
	Matrix44m rot, rot2;

	if (dir == tr("Back") || dir == tr("Back View"))
	{
		rot.SetRotateDeg(90, Point3m(1, 0, 0));
		rot2.SetRotateDeg(180, Point3m(0, 1, 0));
		rot = rot*rot2;
	}
	else if (dir == tr("Front") || dir == tr("Front View"))
	{
		rot.SetRotateDeg(90, Point3m(-1, 0, 0));
		vd = front;
		tempVD = front;
	}
	else if (dir == tr("Left") || dir == tr("Left View"))
	{
		rot.SetRotateDeg(90, Point3m(0, 1, 0));
		rot2.SetRotateDeg(-90, Point3m(1, 0, 0));
		rot = rot*rot2;
	}
	else if (dir == tr("Right") || dir == tr("Side View") || dir == tr("Right View"))
	{
		rot.SetRotateDeg(-90, Point3m(0, 1, 0));
		rot2.SetRotateDeg(-90, Point3m(1, 0, 0));
		rot = rot*rot2;
		//rot.SetRotateDeg(90, Point3m(0, -1, 0));
	}

	else if (dir == tr("Top") || dir == tr("Top View"))
	{
		rot.SetRotateDeg(0, Point3m(0, 1, 0));
		vd = top;
		tempVD = top;
	}
	else if (dir == tr("TopTag"))
	{
		rot.SetRotateDeg(0, Point3m(0, 1, 0));
		vd = topTag;
	}

	else if (dir == tr("Bottom") || dir == tr("Bottom View"))
		rot.SetRotateDeg(180, Point3m(0, 1, 0));

	else if (dir == tr("Original") || dir == tr("Original View"))
	{
		rot.SetRotateDeg(75, Point3m(-1, 0, 0));
		rot2.SetRotateDeg(35, Point3m(0, 0, 1));
		rot = rot*rot2;
		vd = original;
	}
	//else if (dir==tr(""))

	view.Extrinsics.SetRot(rot);//新的shot參數，名子叫view

	float cameraDist = getCameraDistance();

	//add the translation introduced by gluLookAt() (0,0,cameraDist), in order to have te same view---------------
	//T(gl)*S*R*T(t) => SR(gl+t) => S R (S^(-1)R^(-1)gl + t)
	//Add translation S^(-1) R^(-1)(gl)
	//Shotd doesn't introduce scaling
	//---------------------------------------------------------------------
	view.Extrinsics.SetTra(view.Extrinsics.Tra() + (Inverse(view.Extrinsics.Rot())*Point3m(0, 0, cameraDist)));

	Shotm shot = track2ShotCPU(view, &trackball);//*********


	QPair<Shotm, float> shotAndScale = QPair<Shotm, float>(shot, trackball.track.sca);
	//QPair<Shotm, float> shotAndScale = QPair<Shotm, float>(shot, 1);	
	loadShot(shotAndScale);//送shot物件與scale參數

	//trackball.ButtonDown

}

//MultiViewer_Container * GLArea::mvc()
//{
//	QObject * curParent = this->parent();
//	while(qobject_cast<MultiViewer_Container *>(curParent) == 0)
//	{
//		if (curParent != NULL)
//			curParent = curParent->parent();
//		else
//			return NULL;
//	}
//	return qobject_cast<MultiViewer_Container *>(curParent);
//}


MainWindow * GLArea::mw()
{
	QObject * curParent = this->parent();
	while (qobject_cast<MainWindow *>(curParent) == 0)
	{
		curParent = curParent->parent();
	}
	return qobject_cast<MainWindow *>(curParent);
}

bool GLArea::showInterruptButton() const
{
	return interrbutshow;
}

void GLArea::showInterruptButton(const bool& show)
{
	interrbutshow = show;
}

void GLArea::completeUpdateRequested()
{
	makeCurrent();
	if ((this->md() != NULL) && (this->md()->rm() != NULL))
		loadRaster(this->md()->rm()->id());
	//if (md()->mm() != NULL)
	//	trackball.center = md()->mm()->cm.bbox.Center();
	update();
	doneCurrent();
}

//RenderMode* GLArea::getCurrentRenderMode()
//{
//	if ((md() != NULL) && (md()->mm() != NULL))
//	{
//		QMap<int,RenderMode>::iterator it = rendermodemap.find(md()->mm()->id());
//		if (it != rendermodemap.end())
//			return &it.value();
//	}
//	return NULL;
//}

void GLArea::meshAdded(int index, RenderMode rm)
{

	rendermodemap[index] = rm;
	emit updateLayerTable();
}

void GLArea::meshRemoved(int index)
{
	rendermodemap.remove(index);
	reFreshGlListlist();
	emit updateLayerTable();
}
void GLArea::drawAxis(QPainter *p, int vi)
{
	//assert(glw != NULL);
	int x = md()->groove.DimX();
	//int x2 = x % 50;
	int y = md()->groove.DimY();
	int z = md()->groove.DimZ();

	int x_s = -x / 2. - 60;
	int x_e = -x / 2. - 30;


	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();
	glTranslatef(-1.1*md()->groove.DimX() / 2, -1.1*md()->groove.DimY() / 2, -1.1*md()->groove.DimZ() / 2);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POINT_SMOOTH);
	glLineWidth(3);
	glPointSize(5 * 1.5);
	glLabel::Mode md, md2, md3;
	Point3d o(0, 0, 0);
	Point3d a(30, 0, 0);
	Point3d b(0, 30, 0);
	Point3d c(0, 0, 30);

	// Get gl state values
	/*double mm[16], mp[16];
	GLint vp[4];
	glGetDoublev(GL_MODELVIEW_MATRIX, mm);
	glGetDoublev(GL_PROJECTION_MATRIX, mp);
	glGetIntegerv(GL_VIEWPORT, vp);
	float scalefactor = size*0.02f;
	*/
	/*double mm[16], mp[16];
	GLint vp[4];
	glGetDoublev(GL_MODELVIEW_MATRIX, mm);
	glGetDoublev(GL_PROJECTION_MATRIX, mp);
	glGetIntegerv(GL_VIEWPORT, vp);
	float slope_a = calcSlope(-a, a, 2 * size, 10, mm, mp, vp);
	float slope_b = calcSlope(-b, b, 2 * size, 10, mm, mp, vp);
	float slope_c = calcSlope(-c, c, 2 * size, 10, mm, mp, vp);
	float scalefactor = size*0.02f;*/

	//if (vi == viewerID::three){


	md.qFont.setBold(true);
	md.qFont.setPixelSize(20);
	md.color = vcg::Color4b(vcg::Color4b::Red);

	md2.qFont.setBold(true);
	md2.qFont.setPixelSize(20);
	md2.color = vcg::Color4b(vcg::Color4b::Green);

	md3.qFont.setBold(true);
	md3.qFont.setPixelSize(20);
	md3.color = vcg::Color4b(vcg::Color4b::Blue);

	

	//Point3f min = md()->groove.min;
	//Point3f max = md()->groove.max;
	

	switch (vi)
	{
	case 0://左下
	{

			   glBegin(GL_LINES);
			   glColor(Color4b::Red);
			   glVertex3f(0, 0, 0); glVertex(a);
			   glColor(Color4b::Green);
			   glVertex3f(0, 0, 0); glVertex(b);
			   glEnd();

			   md.qFont.setBold(true);
			   md.qFont.setPixelSize(15);
			   float d = 40;
			   vcg::glLabel::render(p, vcg::Point3f(d, 0, 0), QString("X"), md);
			   vcg::glLabel::render(p, vcg::Point3f(0, d, 0), QString("Y"), md);

	}
		break;
	case 1://main view
	{

			   glBegin(GL_LINES);
			   glColor(Color4b::Red);
			   glVertex3f(0, 0, 0); glVertex(a);
			   glColor(Color4b::Green);
			   glVertex3f(0, 0, 0); glVertex(b);
			   glColor(Color4b::Blue);
			   glVertex3f(0, 0, 0); glVertex(c);
			   glEnd();

			   float d = 40;
			   vcg::glLabel::render(p, vcg::Point3f(d, 0, 0), QString("X"), md);
			   vcg::glLabel::render(p, vcg::Point3f(0, d, 0), QString("Y"), md2);
			   vcg::glLabel::render(p, vcg::Point3f(0, 0, d), QString("Z"), md3);


	}
		break;
	case 2://左上
	{
			   glBegin(GL_LINES);
			   glColor(Color4b::Red);
			   glVertex3f(0, 0, 0); glVertex(a);
			   glColor(Color4b::Blue);
			   glVertex3f(0, 0, 0); glVertex(c);
			   glEnd();

			   md.qFont.setBold(true);
			   md.qFont.setPixelSize(15);
			   float d = 40;
			   vcg::glLabel::render(p, vcg::Point3f(d, 0, 0), QString("X"), md);
			   vcg::glLabel::render(p, vcg::Point3f(0, 0, d), QString("Z"), md);

			   //glutSolidSphere(1, 20, 20);
			   //createqobj();
	}
		break;
	}



	glGetError(); // Patch to buggy qt rendertext;
	glPopMatrix();

	if (!grid_chip_gap_)
	{
		glPushMatrix();
		glTranslatef(-x / 2., -y / 2., -z / 2.);
		glDisable(GL_LIGHTING);
		for (int i = 0; i < 11; i++)
		{
			vcg::glLabel::render(p, vcg::Point3f(-10, 11+20.18*i, 0), QString::number(i+1), md3);
		}
		glPopMatrix();

	}
	glPopAttrib();
	assert(!glGetError());

}

static int xol = DSP_grooveBigX;
static int yol = DSP_grooveY;
void GLArea::fillsquare(void) {
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_LIGHTING);
	glColor3f(0, 0, 0);
	glPushMatrix();

	glNormal3f(0, 0, -1);
	glBegin(GL_QUADS);
	glVertex3f(-xol, -yol, equ[3]);
	glVertex3f(xol, -yol, equ[3]);
	glVertex3f(xol, yol, equ[3]);
	glVertex3f(-xol, yol, equ[3]);
	glEnd();


	//glRecti(-5, -5, 5, 5);

	glPopMatrix();
	glPopAttrib();
}
void GLArea::drawglutobj(int i)
{
	//glutSolidSphere(2, 50, 50);
	switch (i)
	{
	case 0:

	{		  //glutSolidTeapot(2);
			  glPushAttrib(GL_ALL_ATTRIB_BITS);
			  glEnable(GL_LIGHTING);
			  glPushMatrix();

			  //glRotatef(90, 1, 0, 0);
			  foreach(MeshModel * mp, this->md()->meshList)
			  {
				  //mp->render(GLW::DMFlat, GLW::CMPerVert, GLW::TMNone);
				  mp->render(GLW::DMSmooth, mp->rmm.colorMode, mp->rmm.textureMode);//currentused		
			  }
			  //glutSolidSphere(2.5, 50, 50);
			  //glutSolidTorus(0.8, 3, 50, 50);			 

			  glPopMatrix();
			  glPopAttrib();
	}
		break;
	case 1:
	{
			  glPushMatrix();
			  //glTranslatef(0, 0, obj_z_tans);
			  glRotatef(90, 1, 0, 0);
			  glutSolidSphere(2, 50, 50);
			  glPopMatrix();

	}
		break;
	case 2:
	{		  //glutSolidTeapot(2);
			  glPushAttrib(GL_ALL_ATTRIB_BITS);
			  glEnable(GL_LIGHTING);
			  glPushMatrix();

			  //glRotatef(90, 1, 0, 0);
			  foreach(MeshModel * mp, this->md()->meshList)
			  {
				  if (mp->getMeshSort() == MeshModel::meshsort::slice_item)
					  mp->render(GLW::DMTexWire, GLW::CMPerVert, mp->rmm.textureMode);//backup  ;

				  if (mp != NULL &&
					  (mp->getMeshSort() == MeshModel::meshsort::print_item))
				  {
					  glColor3f(0, 1, 0);
					  glBoxWire(mp->cm.bbox);
				  }
			  }
			  //glutSolidSphere(2.5, 50, 50);
			  //glutSolidTorus(0.8, 3, 50, 50);			 

			  glPopMatrix();
			  glPopAttrib();
	}

		break;
	}

}
void GLArea::setGrooveView()//xy平面黑白view
{
	if ((!takeSnapTile) || (takeSnapTile && !ss.transparentBackground))
		drawGradient();  // draws the background

	/*glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearColor(1, 1, 1, 1);
	glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
	glOrtho(-11.5, 11.5, -11.5, 11.5, -100, 100);
	glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); gluLookAt(0, 0, 20, 0, 0, 0, 0, 1, 0);*/
	drawLight();

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();

	// Finally apply the Trackball for the model	
	trackball.GetView();
	trackball.Apply();

	//***20150409//改由mousemove func鎖住
	//if(vd==top || vd==front)trackball.current_mode = NULL;//trackball 鎖住
	glPushMatrix();

	//**************************************************************************//
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
	drawglutobj(0);
	//second pass: decrement stencil buffer value on front faces
	glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
	glCullFace(GL_BACK); // render front faces only
	drawglutobj(0);

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
	//********************************************************************************************//
	//glutSolidSphere(10, 50, 50);
	//createqobj();
	drawglutobj(2);
	//vcg::glBoxWire_SKT(md()->groove, md()->groove.getGrooveHit());

	glPopMatrix(); // We restore the state to immediately after the trackball (and before the bbox scaling/translating)
	glPopMatrix(); // We restore the state to immediately before the trackball
	glPopAttrib();


}

void GLArea::setGrooveView2()//xy平面黑白view
{
	if ((!takeSnapTile) || (takeSnapTile && !ss.transparentBackground))
		drawGradient();  // draws the background

	drawLight();


	glPushMatrix();

	// Finally apply the Trackball for the model	
	trackball.GetView();
	trackball.Apply();
	//***20150409//改由mousemove func鎖住
	//if(vd==top || vd==front)trackball.current_mode = NULL;//trackball 鎖住
	glPushMatrix();



	if (!this->md()->isBusy())//md為meshdocument
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		if (iRenderer)//iRenderer為meshrenderInterface
		{
			//qDebug() << "render shader ";
			// virtual void Render(QAction *, MeshDocument &, QMap<int,RenderMode>&, QGLWidget *) = 0;
			//****20150504註解掉
			iRenderer->Render(currentShader, *this->md(), rendermodemap, this);

		}
		else
		{
			if (hasToSelect) // right mouse click you have to select a mesh，控制mouse函數那動作
			{
				int newId = RenderForSelection(pointToPick[0], pointToPick[1]);
				if (newId >= 0)
				{
					Logf(0, "Selected new Mesh %i", newId);
					md()->setCurrentMesh(newId);
					//***20150413畫BBox
					//glBoxWire(md()->getMesh(newId)->cm.bbox);
					//***
					update();
				}

				//***20150413
				hasToSelect = false;

			}
			//else
			foreach(MeshModel * mp, this->md()->meshList)
			{
				if (mp->label().contains(md()->p_setting.getOlMeshName(), Qt::CaseSensitive) || mp->label().contains(md()->p_setting.getCapMeshName(), Qt::CaseSensitive))
				{
					//***20150512 傳入viewerID
					mp->glw.viewerNum = getId();
					QMap<int, RenderMode>::iterator it = rendermodemap.find(mp->id());
					if (it != rendermodemap.end())
					{
						RenderMode rm = it.value();
						setLightModel(rm);
						if (rm.colorMode != GLW::CMNone)
						{
							glEnable(GL_COLOR_MATERIAL);
							glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
						}
						//
						else glColor(Color4b::White);
						if (rm.backFaceCull)
							glEnable(GL_CULL_FACE);
						else
							glDisable(GL_CULL_FACE);
						//Mesh visibility is read from the viewer visibility map, not from the mesh
						mp->glw.SetHintParamf(GLW::HNPPointSize, glas.pointSize);
						mp->glw.SetHintParami(GLW::HNPPointDistanceAttenuation, glas.pointDistanceAttenuation ? 1 : 0);
						mp->glw.SetHintParami(GLW::HNPPointSmooth, glas.pointSmooth ? 1 : 0);
						if (meshVisibilityMap[mp->id()])
						{
							if (!md()->renderState().isEntityInRenderingState(id, MeshLabRenderState::MESH))
							{
								//if (mp->label().contains(md()->p_setting.getOlMeshName(), Qt::CaseSensitive) || mp->label().contains(md()->p_setting.getCapMeshName(), Qt::CaseSensitive))
								if (mp->label().contains(md()->p_setting.getOlMeshName(), Qt::CaseSensitive))
									//mp->render(rm.drawMode, rm.colorMode, rm.textureMode);
									//***20151111***
								{
									//mp->render(rm.drawMode, mp->rmm.colorMode, mp->rmm.textureMode);
									//mp->render(rm.drawMode, mp->rmm.colorMode, mp->rmm.textureMode);//backup
									//mp->render(rm.drawMode, GLW::CMBlack, mp->rmm.textureMode);
									//if (mp->rmm.textureMode==GLW::TMNone)mp->render(rm.drawMode, mp->rmm.colorMode, mp->rmm.textureMode);
									//else{ qDebug() << "glarea_preview_render" << mp->rmm.textureMode; }
									if (mp->rmm.textureMode == GLW::TMNone && mp->rmm.colorMode == GLW::CMNone)mp->render(rm.drawMode, GLW::CMBlack, mp->rmm.textureMode);
									else { mp->render(rm.drawMode, mp->rmm.colorMode, mp->rmm.textureMode); qDebug() << "glarea_preview_render" << mp->rmm.textureMode; }
								}

								//mp->render(rm.drawMode, GLW::CMPerVert, GLW::TMNone);
								if (mp->label().contains(md()->p_setting.getCapMeshName(), Qt::CaseSensitive))
								{
									mp->render(GLW::DMCapEdge, GLW::CMNone, rm.textureMode);
									//mp->render(GLW::DMCapEdge, GLW::CMNone, rm.textureMode);
								}
								//***20150402要改**已解決
								//mp->render(rm.drawMode, rm.colorMode, vcg::GLW::TextureMode::TMPerWedgeMulti);
								//qDebug()<<"paint_place1" << rm.textureMode;
							}
							else
							{
								md()->renderState().render(mp->id(), rm.drawMode, rm.colorMode, rm.textureMode);
								//qDebug() << "paint_place2" << rm.textureMode;
							}

						}
					}
				}
				//qDebug() << "getId()=" << getId();
			}

			vcg::glBoxWire(md()->groove);
			//***20150413 畫current mesh bounding_box
			/*if (mm() != NULL &&
			(!(mm()->label().contains("_temp_outlines", Qt::CaseSensitive) || mm()->label().contains("_temp_ol_mesh", Qt::CaseSensitive)))
			)glBoxWire(mm()->cm.bbox);*/
			if (mm() != NULL &&
				(!(mm()->label().contains(md()->p_setting.getOlMeshName(), Qt::CaseSensitive) || mm()->label().contains(md()->p_setting.getCapMeshName(), Qt::CaseSensitive)))
				)glBoxWire(mm()->cm.bbox);


		}
		//if (iEdit) {//***20150623***decorate_plugins
		//	iEdit->setLog(&md()->Log);
		//	iEdit->Decorate(*mm(), this, &painter);
		//}
		if (openEditMoving)
		{
			QAction *temp = mw()->genGroupbox.first()->currentAction;
			MeshEditInterFace_v2  *iEditI = qobject_cast<MeshEditInterFace_v2 *>(temp->parent());
			iEditI->updateModelProjectionMatrix(this);

		}



		glPopAttrib();
	} ///end if busy

	glPopMatrix(); // We restore the state to immediately after the trackball (and before the bbox scaling/translating)

	if (trackBallVisible && !takeSnapTile && !(iEdit && !suspendedEditor))
		trackball.DrawPostApply();

	// The picking of the surface position has to be done in object space,//選點
	// so after trackball transformation (and before the matrix associated to each mesh);
	if (hasToPick && hasToGetPickPos)
	{
		Point3f pp;
		hasToPick = false;
		if (Pick<Point3f>(pointToPick[0], pointToPick[1], pp))
		{
			emit transmitSurfacePos(nameToGetPickPos, pp);
			hasToGetPickPos = false;
		}
	}
	/*foreach(QAction * p, iPerDocDecoratorlist)
	{
	MeshDecorateInterface * decorInterface = qobject_cast<MeshDecorateInterface *>(p->parent());
	decorInterface->decorateDoc(p, *this->md(), this->glas.currentGlobalParamSet, this, &painter, md()->Log);
	}*/

	// we want to write scene-space the point picked with double-click in the log
	// we have to do it now, before leaving this transformation space
	// we hook to the same mechanism double-click will be managed later on to move trackball
	if (hasToPick && !hasToGetPickPos)
	{
		Point3f pp;
		if (Pick<Point3f>(pointToPick[0], pointToPick[1], pp))
		{
			// write picked point in the log
			Logf(0, "Recentering on point [%f %f %f] [%d,%d]", pp[0], pp[1], pp[2], pointToPick[0], pointToPick[1]);
		}
	}

	glPopMatrix(); // We restore the state to immediately before the trackball
}

void GLArea::grid3(int unit)
{
	int x = md()->groove.DimX();
	//int x2 = x % 50;

	int y = md()->groove.DimY();

	int z = md()->groove.DimZ();

	//Point3f min = md()->groove.min;
	//Point3f max = md()->groove.max;
	int x_s = -x / 2.;
	int x_e = x / 2.;

	glPushMatrix();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glEnable(GL_LINE_SMOOTH);
	//glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	//glLineWidth(1);
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_LIGHTING);

	/*glBegin(GL_LINES);
	glColor3f(1, 0, 0);
	glLineWidth(5);
	glVertex3f(min.X(), min.Y(), min.Z()); glVertex3f(max.X(), min.Y(), min.Z());
	glVertex3f(min.X(), min.Y(), min.Z()); glVertex3f(min.X(), max.Y(), min.Z());
	glEnd();*/

	switch (unit)
	{
	case 0:
	{
			  if (this->id == 1 || this->id == 0)
			  {
				  //glColor3ub(0, 255, 127);
				  //vcg::glColor(vcg::Color4b::Gray);
				  /*
					line stippple
				  */
				  if (!grid_chip_gap_)
				  {
					  glEnable(GL_LINE_STIPPLE); 
					  glLineStipple(2, 0x0F0F);
					  glLineWidth(1.0f);
					  glBegin(GL_LINES);
					  for (int i = 0, j = 0; i <= 11; i ++)
					  {
						  
						  glColor4ub(0, 0, 0, 255);
						  glVertex3f(-x / 2., -y / 2. + 20.18 * i, -z / 2.); glVertex3f(x / 2., -y / 2. + 20.18*i, -z / 2.);

					  }
					  glEnd();
					  glDisable(GL_LINE_STIPPLE);
				  }
				  else
				  {


					  glBegin(GL_LINES);
					  for (int i = 0, j = 0; i <= y; i += 10)
					  {
						  if (i % 50 == 0)
							  glColor4ub(0, 0, 255, 255);
						  else
							  glColor4ub(112, 112, 112, 255);

						  glVertex3f(-x / 2., -y / 2. + i, -z / 2.); glVertex3f(x / 2., -y / 2. + i, -z / 2.);

					  }

					  for (int i = 0, j = 0; i <= y; i += 10)
					  {
						  if (i % 50 == 0)
							  glColor4ub(0, 0, 255, 255);

						  glVertex3f(-x / 2., -y / 2. + i, -z / 2.); glVertex3f(x / 2., -y / 2. + i, -z / 2.);

					  }

					  for (int i = 0; i <= x; i += 10)
						  //for (float i = 1; i < x; i += 10)
					  {
						  if (i % 50 == 0)
							  //vcg::glColor(vcg::Color4b::Blue);
							  glColor4ub(0, 0, 255, 255);
						  else
							  //vcg::glColor(vcg::Color4b::Gray);
							  glColor4ub(112, 112, 112, 255);
						  glVertex3f(-x / 2. + i, -y / 2., -z / 2.); glVertex3f(-x / 2. + i, y / 2., -z / 2.);
					  }
					  glEnd();
				  }

				  if (vd == front)
				  {
					  glBegin(GL_LINES);
					  for (int i = 0, j = 0; i <= z; i += 10)
					  {
						  if (i % 50 == 0)
							  glColor4ub(0, 0, 255, 50);
						  else
							  glColor4ub(112, 112, 112, 255);

						  glVertex3f(-x / 2., y / 2., -z / 2. + i); glVertex3f(x / 2., y / 2., -z / 2. + i);

					  }

					  for (int i = 0; i <= x; i += 10)
						  //for (float i = 1; i < x; i += 10)
					  {
						  if (i % 50 == 0)
							  //vcg::glColor(vcg::Color4b::Blue);
							  glColor4ub(0, 0, 255, 50);
						  else
							  //vcg::glColor(vcg::Color4b::Gray);
							  glColor4ub(112, 112, 112, 255);
						  glVertex3f(-x / 2. + i, y / 2., -z / 2.); glVertex3f(-x / 2. + i, y / 2., z / 2.);
					  }
					  glEnd();

				  }
				  /* glPointSize(5);
				   glBegin(GL_POINTS);
				   glColor4ub(0, 0, 255, 255);
				   glVertex3f(0, 0, 0);
				   glEnd();*/




			  }
			  else if (this->id == 2)
			  {
				  //glColor3ub(127, 127, 127);
				  vcg::glColor(vcg::Color4b::Gray);
				  glBegin(GL_LINES);
				  for (float i = 0; i < z + 1; i += 1)
				  {
					  glVertex3f(-x / 2., y / 2., -z / 2. + i); glVertex3f(x / 2., y / 2., -z / 2. + i);
				  }
				  for (float i = 0; i < x + 1; i += 1)
				  {
					  glVertex3f(-x / 2. + i, y / 2., -z / 2.); glVertex3f(-x / 2. + i, y / 2., z / 2.);
				  }
				  glEnd();


			  }




			  /*glPointSize(5);
			  glEnable(GL_POINT_SMOOTH);
			  glColor3f(1, 1, 0);
			  glBegin(GL_POINTS);
			  glVertex3f(md()->groove.Center().X(), md()->groove.Center().Y(), md()->groove.min.Z()+0.01);
			  glEnd();*/

	}
		break;
	case 1:
	{
			  if (this->id == 1 || this->id == 0)
			  {
				  //glColor3ub(0, 255, 127);
				  vcg::glColor(vcg::Color4b::Gray);
				  glBegin(GL_LINES);
				  //for (int i = 0; i < y + 1; i++)
				  //for (float i = 1; i < y; i += DSP_inchmm)
				  //{
				  // glVertex3f(-x / 2., -y / 2. + i, -z / 2.); glVertex3f(x / 2., -y / 2. + i, -z / 2.);
				  //}
				  ////for (int i = 0; i < x + 1; i++)
				  //for (float i = 1; i < x; i += DSP_inchmm)
				  //{
				  // glVertex3f(-x / 2. + i, -y / 2., -z / 2.); glVertex3f(-x / 2. + i, y / 2., -z / 2.);
				  //}

				  for (float i = 0; i < y / 2.; i += DSP_inchmm)
				  {
					  glVertex3f(-x / 2., i, -z / 2.); glVertex3f(x / 2., i, -z / 2.);
				  }

				  for (float i = 0; i > -(y / 2.); i -= DSP_inchmm)
				  {
					  glVertex3f(-x / 2., i, -z / 2.); glVertex3f(x / 2., i, -z / 2.);
				  }


				  for (float i = 0; i < x / 2.; i += DSP_inchmm)
				  {
					  glVertex3f(i, -(y / 2.), -z / 2.); glVertex3f(i, y / 2., -z / 2.);
				  }

				  for (float i = 0; i > -(x / 2); i -= DSP_inchmm)
				  {
					  glVertex3f(i, -y / 2., -z / 2.); glVertex3f(i, y / 2., -z / 2.);
				  }

				  glEnd();
			  }
			  else if (this->id == 2)
			  {
				  //glColor3ub(127, 127, 127);
				  vcg::glColor(vcg::Color4b::Gray);
				  glBegin(GL_LINES);
				  for (float i = 0; i < z + 1; i += DSP_inchmm)
				  {
					  glVertex3f(-x / 2., y / 2., -z / 2. + i); glVertex3f(x / 2., y / 2., -z / 2. + i);
				  }
				  for (float i = 0; i < x + 1; i += DSP_inchmm)
				  {
					  glVertex3f(-x / 2. + i, y / 2., -z / 2.); glVertex3f(-x / 2. + i, y / 2., z / 2.);
				  }
				  glEnd();
			  }
	}
		break;
	case 2:
	{
			  if (this->id == 1 || this->id == 0)
			  {
				  //glColor3ub(0, 255, 127);
				  vcg::glColor(vcg::Color4b::Gray);
				  glBegin(GL_LINES);
				  //for (int i = 0; i < y + 1; i++)
				  for (float i = 1; i < y; i += DSP_inchmm)
				  {
					  glVertex3f(-x / 2., -y / 2. + i, -z / 2.); glVertex3f(x / 2., -y / 2. + i, -z / 2.);
				  }
				  //for (int i = 0; i < x + 1; i++)
				  for (float i = 1; i < x; i += DSP_inchmm)
				  {
					  glVertex3f(-x / 2. + i, -y / 2., -z / 2.); glVertex3f(-x / 2. + i, y / 2., -z / 2.);
				  }
				  glEnd();
			  }
			  else if (this->id == 2)
			  {
				  //glColor3ub(127, 127, 127);
				  vcg::glColor(vcg::Color4b::Gray);
				  glBegin(GL_LINES);
				  for (float i = 0; i < z + 1; i += DSP_inchmm)
				  {
					  glVertex3f(-x / 2., y / 2., -z / 2. + i); glVertex3f(x / 2., y / 2., -z / 2. + i);
				  }
				  for (float i = 0; i < x + 1; i += DSP_inchmm)
				  {
					  glVertex3f(-x / 2. + i, y / 2., -z / 2.); glVertex3f(-x / 2. + i, y / 2., z / 2.);
				  }
				  glEnd();
			  }
	}
		break;
	default:
		break;

	}


	glPopAttrib();
	glPopMatrix();

}
void GLArea::grid()
{
	int x = md()->groove.DimX();
	int y = md()->groove.DimY();
	int z = md()->groove.DimZ();
	glPushMatrix();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	//glEnable(GL_LINE_SMOOTH);
	//glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	//glLineWidth(1);
	glDisable(GL_LIGHTING);

	if (this->id == 1 || this->id == 0)
	{
		//glColor3ub(0, 255, 127);
		vcg::glColor(vcg::Color4b::Gray);
		glBegin(GL_LINES);
		//for (int i = 0; i < y + 1; i++)
		for (int i = 1; i < y; i++)
		{
			glVertex3f(-x / 2., -y / 2. + i, -z / 2.); glVertex3f(x / 2., -y / 2. + i, -z / 2.);
		}
		//for (int i = 0; i < x + 1; i++)
		for (int i = 1; i < x; i++)
		{
			glVertex3f(-x / 2. + i, -y / 2., -z / 2.); glVertex3f(-x / 2. + i, y / 2., -z / 2.);
		}
		glEnd();
	}
	else if (this->id == 2)
	{
		//glColor3ub(127, 127, 127);
		vcg::glColor(vcg::Color4b::Gray);
		glBegin(GL_LINES);
		for (int i = 0; i < z + 1; i += 1)
		{
			glVertex3f(-x / 2., y / 2., -z / 2. + i); glVertex3f(x / 2., y / 2., -z / 2. + i);
		}
		for (int i = 0; i < x + 1; i += 1)
		{
			glVertex3f(-x / 2. + i, y / 2., -z / 2.); glVertex3f(-x / 2. + i, y / 2., z / 2.);
		}

		glEnd();

	}

	glPopAttrib();
	glPopMatrix();
}
bool GLArea::getDisplayInfo()
{
	return showDisplayInfo;
}
void GLArea::setDisplayInfo(bool x)
{
	showDisplayInfo = x;
}
void GLArea::grid2()
{
	int x = 9;
	int y = 9;
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();

	glDisable(GL_LIGHTING);

	//for (int i = 0; i < 9; i++)
	//{
	//	for (int j = 0; j < 9; j++)
	//	{

	//		SKT::GL_label_SKT::drawquad(i, j, 32 * i - 1, 32 * j - 1, 95);//x,y,r,g,b
	//		//glVertex3f(-x / 2. + i, -y / 2., -z / 2.); glVertex3f(-x / 2. + i, y / 2., -z / 2.);
	//	}

	//	//glVertex3f(-x / 2., -y / 2. + i, -z / 2.); glVertex3f(x / 2., -y / 2. + i, -z / 2.);


	//}



	glPopMatrix();
	glPopAttrib();
}
void GLArea::updaterendermodemap()
{
	mvc()->updateAllViewerRenderModeMap();
}

void GLArea::createqobj()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();
	GLfloat cone_mat[4] = { 0.f, .5f, 1.f, 1.f };

	cone = gluNewQuadric();
	base = gluNewQuadric();
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, cone_mat);
	gluQuadricOrientation(base, GLU_INSIDE);
	gluDisk(base, 0., 2., 64, 1);
	gluCylinder(cone, 2., 0., 6., 64, 64);
	gluDeleteQuadric(cone);
	gluDeleteQuadric(base);



	glPopMatrix();
	glPopAttrib();
}
void GLArea::openTPreview()
{
	if (mm() != NULL)
	if (mm()->getMeshSort() == MeshModel::meshsort::print_item && getPickId >= 0)
	{
		////***20160308

		//if (!mw()->TPreview->isVisible())
		//{
		//	mw()->testdialog = new QWidget(this);
		//	mw()->testdialog->setLayout(mw()->test_transform_layout());
		//	mw()->testdialog->setFocusPolicy(Qt::ClickFocus);
		//	QPoint pos = mw()->testdialog->pos();
		//	//if (pos.x() < 0)
		//	pos.setX(500);
		//	//if (pos.y() < 0)
		//	pos.setY(50);
		//	mw()->testdialog->move(pos);
		//	mw()->testdialog->show();

		//}
		//mw()->testdialog->setLayout(mw()->test_transform_layout());
		////mw()->testdialog->setModal(false);
		////mw()->testdialog->setWindowModality(Qt::NonModal);
		//mw()->testdialog->show();

		if (!mw()->TPreview->isVisible())
		{
			mw()->TPreview = new TransformPreview(this);
			mw()->TPreview->setLayout(mw()->test_transform_layout());
			mw()->TPreview->setFocusPolicy(Qt::ClickFocus);
			QPoint pos = mw()->TPreview->pos();
			//if (pos.x() < 0)
			pos.setX(1000);
			//if (pos.y() < 0)
			pos.setY(50);
			mw()->TPreview->move(pos);
			mw()->TPreview->show();

		}

	}
	else
	{
		/*if (mw()->testdialog!=NULL)
		mw()->testdialog->close();*/
		if (mw()->TPreview != NULL && mw()->TPreview->isVisible())
			mw()->TPreview->close();
	}

}
void GLArea::set_equ3(double value)
{
	equ[3] = value;
	update();

}
GLuint GLArea::createVBO(const void* data, int dataSize, GLenum target, GLenum usage)
{
	vboid = 0;  // 0 is reserved, glGenBuffersARB() will return non-zero id if success
	//vbotestptr = data;
	glGenBuffersARB(1, &vboid);                        // create a vbo
	glBindBufferARB(target, vboid);                    // activate vbo id to use
	glBufferDataARB(target, dataSize, data, usage); // upload data to video card

	// check data size in VBO is same as input array, if not return 0 and delete VBO
	int bufferSize = 0;
	glGetBufferParameterivARB(target, GL_BUFFER_SIZE_ARB, &bufferSize);
	if (dataSize != bufferSize)
	{
		glDeleteBuffersARB(1, &vboid);
		vboid = 0;
		//std::cout << "[createVBO()] Data size is mismatch with input array\n";
	}

	return vboid;      // return VBO id
}
void GLArea::drawPointsfromGrayImage()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_LIGHTING);

	QVector<vcg::Point3f> temp = QVector<vcg::Point3f>::fromStdVector(md()->pointsInFrontOfMesh);

	glBegin(GL_POINTS);

	glColor4ub(0, 0, 255, 255);

	//glColor4ub(112, 112, 112, 255);

	foreach(vcg::Point3f points, temp)
	{
		glVertex3f(points[0], points[1], points[2]);
	}



	glEnd();

}
void GLArea::refreshVBO()
{
	if (!this->md()->isBusy())
	{

		foreach(int id, md()->multiSelectID)
		{
			//CMeshO::VertexIterator vid;
			//vid = tmm->cm.vert.begin();
			//bool x = vid->HasTexCoord();
			MeshModel *tmm = md()->getMesh(id);

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
					tmm->glw.initVBOUpdate_multi_texture(GLW::DMFlat, tmm->rmm.colorMode, tmm->rmm.textureMode);
				}


			}
		}
		foreach(MeshModel *mmm, md()->meshList)
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
					mmm->glw.initVBOUpdate_multi_texture(GLW::DMFlat, mmm->rmm.colorMode, mmm->rmm.textureMode);
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
					mmm->glw.initVBOUpdate_multi_texture(GLW::DMFlat, mmm->rmm.colorMode, mmm->rmm.textureMode);
				}
			}
		}
	}


}
void GLArea::updatePartVBO(bool updateVert, bool updateNormal, bool updateColor)
{
	if (!this->md()->isBusy())
	{

		foreach(int id, md()->multiSelectID)
		{
			//CMeshO::VertexIterator vid;
			//vid = tmm->cm.vert.begin();
			//bool x = vid->HasTexCoord();
			MeshModel *tmm = md()->getMesh(id);

			if (tmm->getMeshSort() == MeshModel::meshsort::print_item || tmm->getMeshSort() == MeshModel::meshsort::pre_print_item)
			{
				//tmm->rmm.colorMode = GLW::CMNone;
				if (/*tmm->glw.TMIdd[0].size() < 2 &&*/ ((tmm->glw.curr_hints & GLW::Hint::HNUseVBO) != 0))
				{
					//tmm->glw.UpdateVBOData(updateVert,updateColor);
					tmm->glw.UpdateVBOData_multiArray(updateVert, updateNormal, updateColor, tmm->rmm.colorMode, tmm->rmm.drawMode);
				}
				else
				{
					continue;
				}

			}
		}
		/*foreach(MeshModel *mmm, md()->meshList)
		{
		if (mmm->getMeshSort() == MeshModel::meshsort::print_item  && mmm->glw.TMIdd[0].size() < 2)
		{
		if (!glIsBufferARB(mmm->glw.array_buffers[0]))
		{
		mmm->glw.UpdateVBOData();
		}
		}
		}*/
	}

}