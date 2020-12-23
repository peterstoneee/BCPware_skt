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

#define _UNICODE
#include "mainwindow.h"
#include "plugindialog.h"
#include "filterScriptDialog.h"
#include "customDialog.h"
#include "saveSnapshotDialog.h"
#include <io.h>  
//#include "ui_aboutDialog.h"
#include "savemaskexporter.h"
#include "alnParser.h"
#include <exception>
#include "xmlgeneratorgui.h"
#include "filterthread.h"
#include "process_wrapper.h"
#include "monitor.h"
#include <wrap/gl/glu_tessellator_cap.h>
#include <vcg/complex/algorithms/intersection.h>
#include "ui_printdialog.h"
#include <ShellApi.h>
#include "threemf_manip.h"
#include "genPrintInfo.h"
#include "ui_palette.h"
#include "ui_support.h"
#include "webService.h"
#include "../bcpwarePlugins/filter_box_packing/three_d_packing.h"
#include <QUuid>

#include <vcg/complex/algorithms/create/ball_pivoting.h>
#include <vcg/complex/algorithms/create/platonic.h>
//#include <SKTlib\skt_function.h>

//#include "usbprint_mp_1109.h"
//#include "chipcommunication.h"

#include "login.h"

#include <QToolBar>
#include <QToolTip>
#include <QStatusBar>
#include <QMenuBar>
#include <QProgressBar>
#include <QDesktopServices>
#include <QtPrintSupport>
#include <QPrintDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QtConcurrent>
#include <QFutureWatcher>
#include <QGraphicsView>
#include <QtGui>

#include<regex>




#include "../common/scriptinterface.h"
#include "../common/meshlabdocumentxml.h"
#include "../common/meshlabdocumentbundler.h"
#include "../common/mlapplication.h"
#include "../common/filterscript.h"
#include "meshcheck.h"
#include "widgetmonitor.h"
#include "zxparser.h"
#include "sktlib/zipIO.h"
#include "../common/NodeBVH.h"
#include "../common/pack3D.h"
#include "../common/binPacking.h"


#include <iostream>
#include <tchar.h>

using namespace std;
using namespace vcg;
QMap<int, int> MainWindow::dynamicWipe;
QMap<int, int> MainWindow::finalDynamicPage;


void MainWindow::updateRecentFileActions()
{
	bool activeDoc = (bool)!mdiarea->subWindowList().empty() && mdiarea->currentSubWindow();

	QSettings settings;
	QStringList files = settings.value("recentFileList").toStringList();

	int numRecentFiles = qMin(files.size(), (int)MAXRECENTFILES);

	for (int i = 0; i < numRecentFiles; ++i)
	{
		QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(files[i]).fileName());
		recentFileActs[i]->setText(text);
		recentFileActs[i]->setData(files[i]);
		recentFileActs[i]->setEnabled(activeDoc);
		//SYDNY 08/23/2017 recent file issue
		recentFileActs[i]->setVisible(true);
	}

	for (int j = numRecentFiles; j < MAXRECENTFILES; ++j)
	{
		recentFileActs[j]->setVisible(false);
	}

}

void MainWindow::updateRecentProjActions()
{
	//bool activeDoc = (bool) !mdiarea->subWindowList().empty() && mdiarea->currentSubWindow();

	QSettings settings;
	QStringList projs = settings.value("recentProjList").toStringList();

	int numRecentProjs = qMin(projs.size(), (int)MAXRECENTFILES);
	for (int i = 0; i < numRecentProjs; ++i)
	{
		QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(projs[i]).fileName());
		recentProjActs[i]->setText(text);
		recentProjActs[i]->setData(projs[i]);
		recentProjActs[i]->setEnabled(true);
		recentProjActs[i]->setVisible(true);
	}
	for (int j = numRecentProjs; j < MAXRECENTFILES; ++j)
		recentProjActs[j]->setVisible(false);
	//
}

// creates the standard plugin window
void MainWindow::createStdPluginWnd()
{
	//checks if a MeshlabStdDialog is already open and closes it
	if (stddialog != 0)
	{
		stddialog->close();
		delete stddialog;
	}
	stddialog = new MeshlabStdDialog(this);
	stddialog->setAllowedAreas(Qt::NoDockWidgetArea);
	//addDockWidget(Qt::RightDockWidgetArea,stddialog);

	//stddialog->setAttribute(Qt::WA_DeleteOnClose,true);
	stddialog->setFloating(true);
	stddialog->hide();
	connect(GLA(), SIGNAL(glareaClosed()), this, SLOT(updateStdDialog()));
	connect(GLA(), SIGNAL(glareaClosed()), stddialog, SLOT(closeClick()));
	//
}

void MainWindow::createXMLStdPluginWnd()
{
	//checks if a MeshlabStdDialog is already open and closes it
	if (xmldialog != 0) {
		xmldialog->close();
		delete xmldialog;
	}
	xmldialog = new MeshLabXMLStdDialog(this);
	connect(xmldialog, SIGNAL(filterParametersEvaluated(const QString&, const QMap<QString, QString>&)), meshDoc()->filterHistory, SLOT(addExecutedXMLFilter(const QString&, const QMap<QString, QString>&)));
	//connect(xmldialog,SIGNAL(dialogEvaluateExpression(const Expression&,Value**)),this,SLOT(evaluateExpression(const Expression&,Value**)),Qt::DirectConnection);
	xmldialog->setAllowedAreas(Qt::NoDockWidgetArea);
	//addDockWidget(Qt::RightDockWidgetArea,xmldialog);
	//stddialog->setAttribute(Qt::WA_DeleteOnClose,true);
	xmldialog->setFloating(true);
	xmldialog->hide();
	connect(GLA(), SIGNAL(glareaClosed()), this, SLOT(updateStdDialog()));
	connect(GLA(), SIGNAL(glareaClosed()), xmldialog, SLOT(closeClick()));
	//connect(GLA(),SIGNAL(glareaClosed()),xmldialog,SLOT(close()));
}


// When we switch the current model (and we change the active window)
// we have to close the stddialog.
// this one is called when user switch current window.
void MainWindow::updateStdDialog()
{
	if (stddialog != 0) {
		if (GLA() != 0) {
			if (stddialog->curModel != meshDoc()->mm()) {
				stddialog->curgla = 0; // invalidate the curgla member that is no more valid.
				stddialog->close();
			}
		}
	}
}

// When we switch the current model (and we change the active window)
// we have to close the stddialog.
// this one is called when user switch current window.
void MainWindow::updateXMLStdDialog()
{
	if (xmldialog != 0) {
		if (GLA() != 0) {
			if (xmldialog->curModel != meshDoc()->mm()) {
				xmldialog->resetPointers(); // invalidate the curgla member that is no more valid.
				xmldialog->close();
			}
		}
	}
}

void MainWindow::updateCustomSettings()
{
	mwsettings.updateGlobalParameterSet(currentGlobalParams);
	emit dispatchCustomSettings(currentGlobalParams);
}

void MainWindow::updateWindowMenu()
{
	qDebug() << "updateWindowMenu";
	windowsMenu->clear();
	windowsMenu->addAction(closeAllAct);
	windowsMenu->addSeparator();
	windowsMenu->addAction(windowsTileAct);
	windowsMenu->addAction(windowsCascadeAct);
	windowsMenu->addAction(windowsNextAct);
	windowsNextAct->setEnabled(mdiarea->subWindowList().size() > 1);

	windowsMenu->addSeparator();


	if ((mdiarea->subWindowList().size() > 0)) {
		// Split/Unsplit SUBmenu
		splitModeMenu = windowsMenu->addMenu(tr("&Split current view"));

		splitModeMenu->addAction(setSplitHAct);
		splitModeMenu->addAction(setSplitVAct);

		windowsMenu->addAction(setUnsplitAct);

		// Link act
		windowsMenu->addAction(linkViewersAct);

		// View From SUBmenu
		viewFromMenu = windowsMenu->addMenu(tr("&View from"));
		foreach(QAction *ac, viewFromGroupAct->actions())
			viewFromMenu->addAction(ac);

		// View From File act
		windowsMenu->addAction(viewFromFileAct);
		windowsMenu->addAction(viewFromMeshAct);
		windowsMenu->addAction(viewFromRasterAct);

		// Copy and paste shot acts
		windowsMenu->addAction(copyShotToClipboardAct);
		windowsMenu->addAction(pasteShotFromClipboardAct);

		//Enabling the actions
		MultiViewer_Container *mvc = currentViewContainer();
		if (mvc)
		{
			setUnsplitAct->setEnabled(mvc->viewerCounter() > 1);
			GLArea* current = mvc->currentView();
			if (current)
			{
				setSplitHAct->setEnabled(current->size().height() / 2 > current->minimumSizeHint().height());
				setSplitVAct->setEnabled(current->size().width() / 2 > current->minimumSizeHint().width());

				linkViewersAct->setEnabled(currentViewContainer()->viewerCounter() > 1);
				if (currentViewContainer()->viewerCounter() == 1)
					linkViewersAct->setChecked(false);

				windowsMenu->addSeparator();
			}
		}
	}

	QList<QMdiSubWindow*> windows = mdiarea->subWindowList();

	if (windows.size() > 0)
		windowsMenu->addSeparator();

	int i = 0;
	foreach(QWidget *w, windows)
	{
		QString text = tr("&%1. %2").arg(i + 1).arg(QFileInfo(w->windowTitle()).fileName());
		QAction *action = windowsMenu->addAction(text);
		action->setCheckable(true);
		action->setChecked(w == mdiarea->currentSubWindow());
		// Connect the signal to activate the selected window
		connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
		windowMapper->setMapping(action, w);
		++i;
	}
}

//void MainWindow::setColorNoneMode()
//{
//	GLA()->setColorMode(GLW::CMNone);
//}
//
//void MainWindow::setPerMeshColorMode()
//{
//	GLA()->setColorMode(GLW::CMPerMesh);
//}
//
//void MainWindow::setPerVertexColorMode()
//{
//	GLA()->setColorMode(GLW::CMPerVert);
//}
//
//void MainWindow::setPerFaceColorMode()
//{
//	GLA()->setColorMode(GLW::CMPerFace);
//}

void MainWindow::enableDocumentSensibleActionsContainer(const bool allowed)
{
	QAction* fileact = fileMenu->menuAction();
	if (fileact != NULL)
		fileact->setEnabled(allowed);
	if (mainToolBar != NULL)
		mainToolBar->setEnabled(allowed);
	if (searchToolBar != NULL)
		searchToolBar->setEnabled(allowed);
	QAction* filtact = filterMenu->menuAction();
	if (filtact != NULL)
		filtact->setEnabled(allowed);
	if (filterToolBar != NULL)
		filterToolBar->setEnabled(allowed);
	QAction* editact = editMenu->menuAction();
	if (editact != NULL)
		editact->setEnabled(allowed);
	if (editToolBar)
		editToolBar->setEnabled(allowed);
}



//menu create is not enabled only in case of not valid/existing meshdocument
void MainWindow::updateSubFiltersMenu(const bool createmenuenabled, const bool validmeshdoc)
{
	showFilterScriptAct->setEnabled(validmeshdoc);
	filterMenuSelect->setEnabled(validmeshdoc);
	updateMenuItems(filterMenuSelect, validmeshdoc);
	filterMenuClean->setEnabled(validmeshdoc);
	updateMenuItems(filterMenuClean, validmeshdoc);
	filterMenuCreate->setEnabled(createmenuenabled || validmeshdoc);
	updateMenuItems(filterMenuCreate, createmenuenabled || validmeshdoc);
	filterMenuRemeshing->setEnabled(validmeshdoc);
	updateMenuItems(filterMenuRemeshing, validmeshdoc);
	filterMenuPolygonal->setEnabled(validmeshdoc);
	updateMenuItems(filterMenuPolygonal, validmeshdoc);
	filterMenuColorize->setEnabled(validmeshdoc);
	updateMenuItems(filterMenuColorize, validmeshdoc);
	filterMenuSmoothing->setEnabled(validmeshdoc);
	updateMenuItems(filterMenuSmoothing, validmeshdoc);
	filterMenuQuality->setEnabled(validmeshdoc);
	updateMenuItems(filterMenuQuality, validmeshdoc);
	filterMenuNormal->setEnabled(validmeshdoc);
	updateMenuItems(filterMenuNormal, validmeshdoc);
	filterMenuMeshLayer->setEnabled(validmeshdoc);
	updateMenuItems(filterMenuMeshLayer, validmeshdoc);
	filterMenuRasterLayer->setEnabled(validmeshdoc);
	updateMenuItems(filterMenuRasterLayer, validmeshdoc);
	filterMenuRangeMap->setEnabled(validmeshdoc);
	updateMenuItems(filterMenuRangeMap, validmeshdoc);
	filterMenuPointSet->setEnabled(validmeshdoc);
	updateMenuItems(filterMenuPointSet, validmeshdoc);
	filterMenuSampling->setEnabled(validmeshdoc);
	updateMenuItems(filterMenuSampling, validmeshdoc);
	filterMenuTexture->setEnabled(validmeshdoc);
	updateMenuItems(filterMenuTexture, validmeshdoc);
	filterMenuCamera->setEnabled(validmeshdoc);
	updateMenuItems(filterMenuCamera, validmeshdoc);
	//***20150513 啟動filterMenu 下的所有項目
	updateMenuItems(filterMenu, validmeshdoc);

	//groove_settingAct->setEnabled(validmeshdoc);
}

void MainWindow::updateMenuItems(QMenu* menu, const bool enabled)
{
	foreach(QAction* act, menu->actions())
		act->setEnabled(enabled);
}

void MainWindow::switchOffDecorator(QAction* decorator)
{
	if (GLA() != NULL)
	{
		int res = GLA()->iCurPerMeshDecoratorList().removeAll(decorator);
		if (res == 0)
			GLA()->iPerDocDecoratorlist.removeAll(decorator);
		updateMenus();
		GLA()->update();
	}
}
QString MainWindow::operateString(QString ttt, int po)
{
	int temp = ttt.size() / po + 1;
	for (int i = 1; i < temp; i++)
	{
		ttt.insert(i * (po + 1), "\n");
	}
	return ttt;
}
//***20150527***v2
void MainWindow::updateMenus()
{
	//qDebug() << "bool fileMenu enabled : "<<fileMenu->isEnabled();
	//////////////////////////////////////////////////////////////////////////////////////////

	//HWND hwnd = ::FindWindowEx(NULL, NULL, NULL, DSP_DASHBOARD_WINDOW_NAME);
	//showPrintDashBoardAct->setChecked(IsWindow(hwnd));

	bool activeDoc = (bool)!mdiarea->subWindowList().empty() && mdiarea->currentSubWindow();//***subwindowlist不空and 有currentSubWindow
	bool notEmptyActiveDoc = activeDoc && !meshDoc()->meshList.empty();
	bool notEmptyActiveDocAndNotEmptySelection = notEmptyActiveDoc && !meshDoc()->getMultiSelectID()->empty();
	if (activeDoc)
		if (/*currentViewContainer() != NULL &&*/ /*this->meshDoc()->count_print_item() > 0 &&*/ meshDoc()->multiSelectID.size() > 0)
		{
			//QFileInfo temp(meshDoc()->mm()->fullName());		

			int oneLineString = 10;
			//originalName->setText(operateString(QString("%1").arg(temp.completeBaseName()), oneLineString));

			//buildHeight->setText(operateString(QString("%1").arg(meshDoc()->bbox().DimZ()), oneLineString));
			////dimension->setText(operateString(QString("%1  * %2 * %3").arg(meshDoc()->bbox().DimX(), 0, 'g', 3).arg(meshDoc()->bbox().DimY(), 0, 'g', 3).arg(meshDoc()->bbox().DimZ(), 0, 'g', 3), oneLineString));
			//dimension->setText((QString("%1 \n").arg(meshDoc()->bbox().DimX(), 0, 'g', 3).append("%1\n").arg(meshDoc()->bbox().DimY(), 0, 'g', 3).append("%1").arg(meshDoc()->bbox().DimZ(), 0, 'g', 3)));
			////dimension->setText(QString("45661 \n  9879"));
			//numOfvertices->setText(operateString(QString("%1 (%2)").arg(meshDoc()->mm()->cm.vn).arg(this->meshDoc()->vn()), oneLineString));
			//numOfFace->setText(operateString(QString("%1 (%2)").arg(meshDoc()->mm()->cm.fn).arg(this->meshDoc()->fn()), oneLineString));

			/*if (dockInformationWidget2->ui->singleOrAllTB->isChecked()) {
				QStringList tempSL;
				tempSL << operateString(QString("%1").arg(meshDoc()->meshList.count()), oneLineString)
				<< operateString(QString("%1").arg(meshDoc()->bbox().DimZ()), oneLineString)
				<< QString("%1(x)\n%2(y)\n%3(z)").arg(meshDoc()->bbox().DimX(), 0, 'f', 1).arg(meshDoc()->bbox().DimY(), 0, 'f', 1).arg(meshDoc()->bbox().DimZ(), 0, 'f', 1)
				<< operateString(QString("%1 ").arg(this->meshDoc()->vn()), oneLineString)
				<< operateString(QString("%1 ").arg(this->meshDoc()->fn()), oneLineString)
				<< operateString(QString("%1(x) * %2(y) * %3 (z) ").arg(meshDoc()->bbox().min.X(), 0, 'f', 1).arg(meshDoc()->bbox().min.Y(), 0, 'f', 1).arg(meshDoc()->bbox().min.Z(), 0, 'f', 1), oneLineString);

				dockInformationWidget2->updateUi(tempSL);

				}*/
			Point3f halfDim = meshDoc()->groove.Dim() / 2.;
			if (meshDoc()->p_setting.gd_unit == 0)
			{
				if (meshDoc()->multiSelectID.size() > 1)
				{
					QStringList tempSL;
					tempSL << operateString(QString("%1").arg(meshDoc()->multiSelectID.size()), oneLineString)
						<< operateString(QString("%1").arg(meshDoc()->selBBox().DimZ()), oneLineString)
						<< QString("%1(x)\n%2(y)\n%3(z) mm").arg(meshDoc()->selBBox().DimX(), 0, 'f', 1).arg(meshDoc()->selBBox().DimY(), 0, 'f', 1).arg(meshDoc()->selBBox().DimZ(), 0, 'f', 1)
						<< operateString(QString("%1 ").arg(meshDoc()->selvn()), oneLineString)
						<< operateString(QString("%1 ").arg(meshDoc()->selfn()), oneLineString)
						<< QString("%1(x)\n%2(y)\n%3 (z) mm ").arg(meshDoc()->selBBox().min.X() + halfDim.X(), 0, 'f', 1).arg(meshDoc()->selBBox().min.Y() + halfDim.Y(), 0, 'f', 1).arg(meshDoc()->selBBox().min.Z() + halfDim.Z(), 0, 'f', 1);
					//<< QString("%1(x)\n%2(y)\n%3 (z) ").arg((meshDoc()->selBBox().min.X() + meshDoc()->selBBox().max.X()) / 2, 0, 'f', 1).arg((meshDoc()->selBBox().min.Y() + meshDoc()->selBBox().max.Y()) / 2, 0, 'f', 1).arg(meshDoc()->selBBox().min.Z() + meshDoc()->groove.DimZ() / 2., 0, 'f', 1);

					dockInformationWidget2->updateUi(tempSL, false);
				}
				else if (meshDoc()->multiSelectID.size() == 1)
				{
					MeshModel *abc = meshDoc()->mm();
					QStringList tempSL;
					tempSL << operateString(QString("%1").arg(meshDoc()->mm()->label()), oneLineString)
						<< operateString(QString("%1").arg(meshDoc()->mm()->cm.bbox.DimZ()), oneLineString)
						<< QString("%1(x)\n%2(y)\n%3(z) mm").arg(meshDoc()->mm()->cm.bbox.DimX(), 0, 'f', 1).arg(meshDoc()->mm()->cm.bbox.DimY(), 0, 'f', 1).arg(meshDoc()->mm()->cm.bbox.DimZ(), 0, 'f', 1)
						<< operateString(QString("%1 ").arg(meshDoc()->mm()->cm.vn), oneLineString)
						<< operateString(QString("%1 ").arg(meshDoc()->mm()->cm.fn), oneLineString)
						<< QString("%1(x)\n%2(y)\n%3 (z) mm").arg(meshDoc()->mm()->cm.bbox.min.X() + halfDim.X(), 0, 'f', 1).arg(meshDoc()->mm()->cm.bbox.min.Y() + halfDim.Y(), 0, 'f', 1).arg(meshDoc()->mm()->cm.bbox.min.Z() + halfDim.Z(), 0, 'f', 1);
					//<< QString("%1(x)\n%2(y)\n%3 (z) ").arg((meshDoc()->selBBox().min.X() + meshDoc()->selBBox().max.X()) / 2, 0, 'f', 1).arg((meshDoc()->selBBox().min.Y() + meshDoc()->selBBox().max.Y()) / 2, 0, 'f', 1).arg(meshDoc()->selBBox().min.Z() + meshDoc()->groove.DimZ() / 2., 0, 'f', 1);

					dockInformationWidget2->updateUi(tempSL, true);
				}
			}
			else
			{
				if (meshDoc()->multiSelectID.size() > 1)
				{
					QStringList tempSL;
					tempSL << operateString(QString("%1").arg(meshDoc()->multiSelectID.size()), oneLineString)
						<< operateString(QString("%1").arg(meshDoc()->selBBox().DimZ() / DSP_inchmm), oneLineString)
						<< QString("%1(x)\n%2(y)\n%3(z) in").arg(meshDoc()->selBBox().DimX() / DSP_inchmm, 0, 'f', 2).arg(meshDoc()->selBBox().DimY() / DSP_inchmm, 0, 'f', 2).arg(meshDoc()->selBBox().DimZ() / DSP_inchmm, 0, 'f', 2)
						<< operateString(QString("%1 ").arg(meshDoc()->selvn()), oneLineString)
						<< operateString(QString("%1 ").arg(meshDoc()->selfn()), oneLineString)
						<< QString("%1(x)\n%2(y)\n%3 (z) in").arg((meshDoc()->selBBox().min.X() + halfDim.X()) / DSP_inchmm, 0, 'f', 2).arg((meshDoc()->selBBox().min.Y() + halfDim.Y()) / DSP_inchmm, 0, 'f', 2).arg((meshDoc()->selBBox().min.Z() + halfDim.Z()) / DSP_inchmm, 0, 'f', 1);
					//<< QString("%1(x)\n%2(y)\n%3 (z) ").arg((meshDoc()->selBBox().min.X() + meshDoc()->selBBox().max.X()) / 2, 0, 'f', 1).arg((meshDoc()->selBBox().min.Y() + meshDoc()->selBBox().max.Y()) / 2, 0, 'f', 1).arg(meshDoc()->selBBox().min.Z() + meshDoc()->groove.DimZ() / 2., 0, 'f', 1);

					dockInformationWidget2->updateUi(tempSL, false);
				}
				else if (meshDoc()->multiSelectID.size() == 1)
				{
					QStringList tempSL;
					tempSL << operateString(QString("%1").arg(meshDoc()->mm()->label()), oneLineString)
						<< operateString(QString("%1").arg(meshDoc()->mm()->cm.bbox.DimZ()), oneLineString)
						<< QString("%1(x)\n%2(y)\n%3(z) in").arg(meshDoc()->mm()->cm.bbox.DimX() / DSP_inchmm, 0, 'f', 2).arg(meshDoc()->mm()->cm.bbox.DimY() / DSP_inchmm, 0, 'f', 2).arg(meshDoc()->mm()->cm.bbox.DimZ() / DSP_inchmm, 0, 'f', 2)
						<< operateString(QString("%1 ").arg(meshDoc()->mm()->cm.vn), oneLineString)
						<< operateString(QString("%1 ").arg(meshDoc()->mm()->cm.fn), oneLineString)
						<< QString("%1(x)\n%2(y)\n%3 (z) in").arg((meshDoc()->mm()->cm.bbox.min.X() + halfDim.X()) / DSP_inchmm, 0, 'f', 2).arg((meshDoc()->mm()->cm.bbox.min.Y() + halfDim.Y()) / DSP_inchmm, 0, 'f', 2).arg((meshDoc()->mm()->cm.bbox.min.Z() + halfDim.Z()) / DSP_inchmm, 0, 'f', 2);
					//<< QString("%1(x)\n%2(y)\n%3 (z) ").arg((meshDoc()->selBBox().min.X() + meshDoc()->selBBox().max.X()) / 2, 0, 'f', 1).arg((meshDoc()->selBBox().min.Y() + meshDoc()->selBBox().max.Y()) / 2, 0, 'f', 1).arg(meshDoc()->selBBox().min.Z() + meshDoc()->groove.DimZ() / 2., 0, 'f', 1);

					dockInformationWidget2->updateUi(tempSL, true);
				}
			}


		}
		else
		{
			QStringList tempSL;
			tempSL << " "
				<< " "
				<< " "
				<< " "
				<< " "
				<< " ";
			dockInformationWidget2->updateUi(tempSL, false);
		}

	importMeshAct->setEnabled(activeDoc);
	ioToolBar->setEnabled((activeDoc));
	viewDToolBar->setEnabled((activeDoc));

	undoGroup->setActiveStack(activeDoc == false ? 0 : currentViewContainer()->undoStack);
	dockTabWidget->setEnabled(notEmptyActiveDoc);
	dockTabViewWidget->setEnabled(notEmptyActiveDoc);

	exportMeshAct->setEnabled(notEmptyActiveDocAndNotEmptySelection);
	exportMeshAsAct->setEnabled(notEmptyActiveDocAndNotEmptySelection);
	exportMeshAsAct2->setEnabled(notEmptyActiveDocAndNotEmptySelection);
	reloadMeshAct->setEnabled(notEmptyActiveDocAndNotEmptySelection);
	reloadAllMeshAct->setEnabled(notEmptyActiveDoc);
	genPrePrintingAct->setEnabled(notEmptyActiveDocAndNotEmptySelection);
	importRasterAct->setEnabled(activeDoc);

	topTagAction->setEnabled((activeDoc));
	genZXAct->setEnabled(notEmptyActiveDoc);
	testDMSlicerAct->setEnabled(notEmptyActiveDoc);
	//viewFromGroupActTB->setEnabled((activeDoc));	
	//viewCustomToolButton->setEnabled((activeDoc));	
	//lockViewAct->setEnabled((activeDoc));

	///context menu
	copyAct_right->setEnabled(notEmptyActiveDocAndNotEmptySelection);
	pasteAct_right->setEnabled(notEmptyActiveDocAndNotEmptySelection);
	landAct_right->setEnabled(notEmptyActiveDocAndNotEmptySelection);
	changeColorAC->setEnabled(notEmptyActiveDocAndNotEmptySelection);
	exportMeshAsAct2->setEnabled(notEmptyActiveDocAndNotEmptySelection);
	reloadMeshAct->setEnabled(notEmptyActiveDocAndNotEmptySelection);
	deleteAct_right->setEnabled(notEmptyActiveDocAndNotEmptySelection);
	detectOverlapping->setEnabled(notEmptyActiveDoc);
	pasteManyAct_right->setEnabled(notEmptyActiveDocAndNotEmptySelection);


	threeDPrintAct->setEnabled(notEmptyActiveDoc && !meshDoc()->isBusy());
	printfunctionMono->setEnabled(notEmptyActiveDoc && !meshDoc()->isBusy());
	viewMenu->setEnabled(activeDoc);
	editMenu->setEnabled(notEmptyActiveDoc);
	testFuncReadZX->setEnabled(activeDoc);

	printMenu->setEnabled(activeDoc);
	showObjectListAct->setEnabled(activeDoc);
	topTagAction->setEnabled(activeDoc);
	if (!activeDoc)
	{
		objList->myTreeWidget->clear();
		showObjectListAct->setChecked(false);
		objList->close();
	}
	if (objList->isVisible())
		objList->updateUI(activeDoc);

	if (activeDoc)
	{
		if (dockTabWidget->currentIndex() != 1)
			delOl(MeshModel::meshsort::slice_item);
	}


	//editToolBar menu
	editToolBar_pi->setEnabled(notEmptyActiveDoc);

	//*** 20170124 --- Modified the enable settings for edit menu and context menu. (lines 450 - 472)
	// Modified by (R&D-I-SW, Mark)
	unDoAct->setEnabled(activeDoc && currentViewContainer()->undoStack->canUndo());
	reDoAct->setEnabled(activeDoc && currentViewContainer()->undoStack->canRedo());
	copyAct->setEnabled(notEmptyActiveDocAndNotEmptySelection);
	//crash when close project
	pasteAct->setEnabled(activeDoc && !meshDoc()->copiedMeshList.isEmpty());
	deleteAct->setEnabled(notEmptyActiveDocAndNotEmptySelection);
	changeColorAC->setEnabled(notEmptyActiveDocAndNotEmptySelection);
	groove_settingAct->setEnabled(activeDoc);
	showFunctionMenuAct->setEnabled(notEmptyActiveDoc);
	selectAllAct->setEnabled(notEmptyActiveDoc);
	justify->setEnabled(notEmptyActiveDocAndNotEmptySelection);
	flipMenu->setEnabled(notEmptyActiveDocAndNotEmptySelection);
	autoPackingAct->setEnabled(notEmptyActiveDoc);
	toCenterAct->setEnabled(notEmptyActiveDocAndNotEmptySelection);
	landMenu->setEnabled(notEmptyActiveDocAndNotEmptySelection);

	//crash when close project
	//if (notEmptyActiveDoc == false && currentViewContainer() != NULL)
	//if (meshDoc()->copiedMeshList.isEmpty() && currentViewContainer()->undoStack->isClean())
	//	editMenu->setEnabled(false);
	//else editMenu->setEnabled(true);
	//else editMenu->setEnabled(notEmptyActiveDoc);

	//editMenu->setEnabled(notEmptyActiveDoc);

	reloadMeshAct->setEnabled(notEmptyActiveDocAndNotEmptySelection);
	//MeshFilterInterface *filter = *PM.stringFilterMap.find("Selected_Mesh_Landing");
	//filter->AC("Selected_Mesh_Landing")->setEnabled(notEmptyActiveDoc);
	landAct->setEnabled(notEmptyActiveDocAndNotEmptySelection);
	landAllAct->setEnabled(notEmptyActiveDoc);
	tmt->setEnabled(notEmptyActiveDocAndNotEmptySelection);
	printjobestimateAct->setEnabled(notEmptyActiveDoc && !meshDoc()->isBusy());


	//qDebug() << "copyAct" << copyAct->isEnabled();

	//settingToolBar
	//settingToolBar_pi->setEnabled(notEmptyActiveDoc);

	importFromProjectAct->setEnabled(notEmptyActiveDoc);
	saveProjectAsAct->setEnabled(activeDoc);
	saveProjectAct->setEnabled(activeDoc);
	closeProjectAct->setEnabled(activeDoc);

	saveSnapshotAct->setEnabled(activeDoc);

	updateRecentFileActions();//***
	updateRecentProjActions();//***
	filterMenu->setEnabled(!filterMenu->actions().isEmpty());
	if (!filterMenu->actions().isEmpty())
		updateSubFiltersMenu(GLA() != NULL, notEmptyActiveDoc);//***
	lastFilterAct->setEnabled(false);
	lastFilterAct->setText(QString("Apply filter"));

	//***updateMenuItems***
	//editMenu->setEnabled(!editMenu->actions().isEmpty());
	//updateMenuItems(editMenu, activeDoc);


	renderMenu->setEnabled(!editMenu->actions().isEmpty());
	updateMenuItems(renderMenu, activeDoc);
	fullScreenAct->setEnabled(activeDoc);
	trackBallMenu->setEnabled(activeDoc);
	logMenu->setEnabled(activeDoc);
	windowsMenu->setEnabled(activeDoc);
	preferencesMenu->setEnabled(activeDoc);

	//**20160516
	//renderToolBar->setEnabled(activeDoc);

	//showToolbarRenderAct->setChecked(renderToolBar->isVisible());
	showToolbarStandardAct->setChecked(mainToolBar->isVisible());
	if (activeDoc && GLA())
	{
		if (GLA()->getLastAppliedFilter() != NULL)
		{
			lastFilterAct->setText(QString("Apply filter ") + GLA()->getLastAppliedFilter()->text());
			lastFilterAct->setEnabled(true);
		}

		// Management of the editing toolbar
		// when you enter in a editing mode you can toggle between editing
		// and camera moving by esc;
		// you exit from editing mode by pressing again the editing button
		// When you are in a editing mode all the other editing are disabled.

		foreach(QAction *a, PM.editActionList)
		{
			a->setChecked(false);
			a->setEnabled(GLA()->getCurrentEditAction() == NULL);
		}

		suspendEditModeAct->setChecked(GLA()->suspendedEditor);
		suspendEditModeAct->setDisabled(GLA()->getCurrentEditAction() == NULL);

		if (GLA()->getCurrentEditAction())
		{
			GLA()->getCurrentEditAction()->setChecked(!GLA()->suspendedEditor);
			GLA()->getCurrentEditAction()->setEnabled(true);
		}

		showInfoPaneAct->setChecked(GLA()->infoAreaVisible);
		showTrackBallAct->setChecked(GLA()->isTrackBallVisible());
		RenderMode rendtmp;
		if (meshDoc()->meshList.size() > 0)
		{
			QMap<int, RenderMode>::iterator it = GLA()->rendermodemap.find(meshDoc()->meshList[0]->id());
			if (it == GLA()->rendermodemap.end())
				throw MeshLabException("1Something really bad happened. Mesh id has not been found in rendermodemap.");
			rendtmp = it.value();
		}
		bool checktext = (rendtmp.textureMode != GLW::TMNone);
		int ii = 0;
		while (ii < meshDoc()->meshList.size())
		{
			if (meshDoc()->meshList[ii] == NULL)
				return;
			QMap<int, RenderMode>::iterator it = GLA()->rendermodemap.find(meshDoc()->meshList[ii]->id());
			//qDebug() << meshDoc()->meshList[ii]->id() << endl;
			if (it == GLA()->rendermodemap.end())
				throw MeshLabException("2Something really bad happened. Mesh id has not been found in rendermodemap.");
			RenderMode& rm = it.value();
			if (rendtmp.drawMode != rm.drawMode)
				rendtmp.setDrawMode(vcg::GLW::DMNone);

			if (rendtmp.colorMode != rm.colorMode)
				rendtmp.setColorMode(vcg::GLW::CMNone);

			checktext &= (rm.textureMode != GLW::TMNone);

			rendtmp.setLighting(rendtmp.lighting && rm.lighting);
			rendtmp.setFancyLighting(rendtmp.fancyLighting && rm.fancyLighting);
			rendtmp.setDoubleFaceLighting(rendtmp.doubleSideLighting && rm.doubleSideLighting);
			rendtmp.setBackFaceCull(rendtmp.backFaceCull || rm.backFaceCull);
			rendtmp.setSelectedFaceRendering(rendtmp.selectedFace || rm.selectedFace);
			rendtmp.setSelectedVertRendering(rendtmp.selectedVert || rm.selectedVert);
			++ii;
		}

		foreach(QAction* ac, renderModeGroupAct->actions())
			ac->setChecked(false);

		switch (rendtmp.drawMode)
		{
		case GLW::DMBox:				renderBboxAct->setChecked(true);                break;
		case GLW::DMPoints:			renderModePointsAct->setChecked(true);      		break;
		case GLW::DMWire: 			renderModeWireAct->setChecked(true);      			break;
		case GLW::DMFlat:				renderModeFlatAct->setChecked(true);    				break;
		case GLW::DMSmooth:			renderModeSmoothAct->setChecked(true);  				break;
		case GLW::DMFlatWire:		renderModeFlatLinesAct->setChecked(true);				break;
		default: break;
		}

		foreach(QAction* ac, colorModeGroupAct->actions())
			ac->setChecked(false);

		switch (rendtmp.colorMode)
		{
		case GLW::CMNone:	colorModeNoneAct->setChecked(true);	      break;
		case GLW::CMPerMesh:	colorModePerMeshAct->setChecked(true);	      break;
		case GLW::CMPerVert:	colorModePerVertexAct->setChecked(true);  break;
		case GLW::CMPerFace:	colorModePerFaceAct->setChecked(true);    break;
		default: break;
		}

		backFaceCullAct->setChecked(rendtmp.backFaceCull);
		setLightAct->setIcon(rendtmp.lighting ? QIcon(":/images/lighton.png") : QIcon(":/images/lightoff.png"));
		setLightAct->setChecked(rendtmp.lighting);

		setFancyLightingAct->setChecked(rendtmp.fancyLighting);
		setDoubleLightingAct->setChecked(rendtmp.doubleSideLighting);
		setSelectFaceRenderingAct->setChecked(rendtmp.selectedFace);
		setSelectVertRenderingAct->setChecked(rendtmp.selectedVert);
		renderModeTextureWedgeAct->setChecked(checktext);

		// Decorator Menu Checking and unChecking
		// First uncheck and disable all the decorators
		foreach(QAction *a, PM.decoratorActionList)
		{
			a->setChecked(false);
			a->setEnabled(true);
		}
		// Check the decorator per Document of the current glarea
		foreach(QAction *a, GLA()->iPerDocDecoratorlist)
		{
			a->setChecked(true);
		}

		// Then check the decorator enabled for the current mesh.
		if (GLA()->mm())
			foreach(QAction *a, GLA()->iCurPerMeshDecoratorList())
			a->setChecked(true);
	} // if active
	else
	{
		foreach(QAction *a, PM.editActionList)
		{
			a->setEnabled(false);
		}
		foreach(QAction *a, PM.decoratorActionList)
			a->setEnabled(false);

	}

	if (GLA())
	{
		showLayerDlgAct->setChecked(layerDialog->isVisible());
		/*showRasterAct->setChecked(GLA()->isRaster());
		showRasterAct->setEnabled(meshDoc()->rm() != 0);*/
		//if(GLA()->layerDialog->isVisible())
		layerDialog->updateTable();
		layerDialog->updateLog(meshDoc()->Log);
		layerDialog->updateDecoratorParsView();
		objList->updateTable();

		//***20150413
		//qDebug("123456789");
	}
	else
	{
		foreach(QAction *a, PM.decoratorActionList)
		{
			a->setChecked(false);
			a->setEnabled(false);
		}
		if (layerDialog->isVisible())
		{
			layerDialog->updateTable();
			layerDialog->updateDecoratorParsView();
		}
	}
	//***20150527註解掉 與舊menu有關的會當掉
	/*if (searchMenu != NULL)
	searchMenu->searchLineWidth() = longestActionWidthInAllMenus();	*/

	//***20150604***先用爛方法傳值
	for (int i = 0; i < genGroupbox.size(); i++)
	{
		genGroupbox[i]->setEnabled(notEmptyActiveDoc);
		genGroupbox[i]->curgla = GLA();
		genGroupbox[i]->curmwi = this;
	}




	/* genGroupbox.last()->curgla = GLA();
	genGroupbox.last()->curmwi = this;*/
	//***20151006***測試有無與bounding box這邊會在關掉專案時會導致currentViewContainer()當掉
	if (currentViewContainer() != NULL)
	{
		transformwidgett1->curmwi = this;
		transformwidgett1->curmwi2 = this;
		transformwidgett1->meshDocument = meshDoc();
		rotate_widget_cus->curmwi = this;
		rotate_widget_cus->meshDocument = meshDoc();
		scale_widget_cus->curmwi = this;
		scale_widget_cus->meshDocument = meshDoc();
		meshDoc()->groove.setGrooveHit(SKT::detect_hit_groove<Scalarm>(*meshDoc()));

		//SYDNY 08/05/2017		
		//int number_Facets = meshDoc()->fn();		
		setpointCloudSwitch(pointCloudSwitchAct->isChecked());


		//if (fast_Rendering == true && facet_Limit < number_Facets)
		//{
		//	//setMdNumLimit(true);

		//	pointCloudSwitchAct->setDisabled(true);
		//}
		//else
		//{
		//	setMdNumLimit(false);
		//	pointCloudSwitchAct->setDisabled(false);
		//	
		//	if (meshDoc()->vn() > getvtNumberLimit() || pointCloudSwitchAct->isChecked())
		//	{
		//		setMdNumLimit(true);
		//	}
		//	else
		//	{
		//		setMdNumLimit(false);
		//	}
		//}


	}
	//***updatefilter process_running
	//pause_button->setEnabled(filter_process_running);
	//s_pushbutton->setEnabled(filter_process_running);

	//if (meshDoc()->copyMesh(currentViewContainer()->meshDoc.selCm()) == 0)
	//if (currentViewContainer()->meshDoc.copyMesh(currentViewContainer()->meshDoc.selCm()) == 0)
	//{
	//	pasteAct->setEnabled(false);
	//}
	//else
	//{
	//	pasteAct->setEnabled(true);
	//}

	if (notEmptyActiveDoc)
	{
		if (meshDoc()->mm() != NULL)
		{
			int unit = meshDoc()->p_setting.gd_unit;
			switch (unit)
			{
			case 0:
				crossUnitLabel->setText("Unit : mm");
				break;
			case 1:
				crossUnitLabel->setText("Unit : inch");
				break;
			}
		}
	}
}


void MainWindow::setSplit(QAction *qa)
{
	MultiViewer_Container *mvc = currentViewContainer();
	if (mvc)
	{
		GLArea *glwClone = new GLArea(mvc, &currentGlobalParams);
		if (qa->text() == tr("&Horizontally"))
			mvc->addView(glwClone, Qt::Vertical);
		else if (qa->text() == tr("&Vertically"))
			mvc->addView(glwClone, Qt::Horizontal);

		//The loading of the raster must be here
		if (GLA()->isRaster()) {
			glwClone->setIsRaster(true);
			if (this->meshDoc()->rm()->id() >= 0)
				glwClone->loadRaster(this->meshDoc()->rm()->id());
		}

		updateMenus();

		glwClone->resetTrackBall();
		glwClone->update();
	}

}

void MainWindow::setUnsplit()
{
	MultiViewer_Container *mvc = currentViewContainer();
	if (mvc)
	{
		assert(mvc->viewerCounter() > 1);

		mvc->removeView(mvc->currentView()->getId());

		updateMenus();
	}
}

//set the split/unsplit menu that appears right clicking on a splitter's handle
void MainWindow::setHandleMenu(QPoint point, Qt::Orientation orientation, QSplitter *origin) {
	MultiViewer_Container *mvc = currentViewContainer();
	int epsilon = 10;
	splitMenu->clear();
	unSplitMenu->clear();
	//the viewer to split/unsplit is chosen through picking

	//Vertical handle allows to split horizontally
	if (orientation == Qt::Vertical)
	{
		splitUpAct->setData(point);
		splitDownAct->setData(point);

		//check if the viewer on the top is splittable according to its size
		int pickingId = mvc->getViewerByPicking(QPoint(point.x(), point.y() - epsilon));
		if (pickingId >= 0)
			splitUpAct->setEnabled(mvc->getViewer(pickingId)->size().width() / 2 > mvc->getViewer(pickingId)->minimumSizeHint().width());

		//the viewer on top can be closed only if the splitter over the handle that originated the event has one child
		bool unSplittabilityUp = true;
		Splitter * upSplitter = qobject_cast<Splitter *>(origin->widget(0));
		if (upSplitter)
			unSplittabilityUp = !(upSplitter->count() > 1);
		unsplitUpAct->setEnabled(unSplittabilityUp);

		//check if the viewer below is splittable according to its size
		pickingId = mvc->getViewerByPicking(QPoint(point.x(), point.y() + epsilon));
		if (pickingId >= 0)
			splitDownAct->setEnabled(mvc->getViewer(pickingId)->size().width() / 2 > mvc->getViewer(pickingId)->minimumSizeHint().width());

		//the viewer below can be closed only if the splitter ounder the handle that orginated the event has one child
		bool unSplittabilityDown = true;
		Splitter * downSplitter = qobject_cast<Splitter *>(origin->widget(1));
		if (downSplitter)
			unSplittabilityDown = !(downSplitter->count() > 1);
		unsplitDownAct->setEnabled(unSplittabilityDown);

		splitMenu->addAction(splitUpAct);
		splitMenu->addAction(splitDownAct);

		unsplitUpAct->setData(point);
		unsplitDownAct->setData(point);

		unSplitMenu->addAction(unsplitUpAct);
		unSplitMenu->addAction(unsplitDownAct);
	}
	//Horizontal handle allows to split vertically
	else if (orientation == Qt::Horizontal)
	{
		splitRightAct->setData(point);
		splitLeftAct->setData(point);

		//check if the viewer on the right is splittable according to its size
		int pickingId = mvc->getViewerByPicking(QPoint(point.x() + epsilon, point.y()));
		if (pickingId >= 0)
			splitRightAct->setEnabled(mvc->getViewer(pickingId)->size().height() / 2 > mvc->getViewer(pickingId)->minimumSizeHint().height());

		//the viewer on the right can be closed only if the splitter on the right the handle that orginated the event has one child
		bool unSplittabilityRight = true;
		Splitter * rightSplitter = qobject_cast<Splitter *>(origin->widget(1));
		if (rightSplitter)
			unSplittabilityRight = !(rightSplitter->count() > 1);
		unsplitRightAct->setEnabled(unSplittabilityRight);

		//check if the viewer on the left is splittable according to its size
		pickingId = mvc->getViewerByPicking(QPoint(point.x() - epsilon, point.y()));
		if (pickingId >= 0)
			splitLeftAct->setEnabled(mvc->getViewer(pickingId)->size().height() / 2 > mvc->getViewer(pickingId)->minimumSizeHint().height());

		//the viewer on the left can be closed only if the splitter on the left of the handle that orginated the event has one child
		bool unSplittabilityLeft = true;
		Splitter * leftSplitter = qobject_cast<Splitter *>(origin->widget(0));
		if (leftSplitter)
			unSplittabilityLeft = !(leftSplitter->count() > 1);
		unsplitLeftAct->setEnabled(unSplittabilityLeft);

		splitMenu->addAction(splitRightAct);
		splitMenu->addAction(splitLeftAct);

		unsplitRightAct->setData(point);
		unsplitLeftAct->setData(point);

		unSplitMenu->addAction(unsplitRightAct);
		unSplitMenu->addAction(unsplitLeftAct);
	}

	handleMenu->popup(point);
}


void MainWindow::splitFromHandle(QAction *qa)
{
	MultiViewer_Container *mvc = currentViewContainer();
	QPoint point = qa->data().toPoint();
	int epsilon = 10;

	if (qa->text() == tr("&Right"))
		point.setX(point.x() + epsilon);
	else if (qa->text() == tr("&Left"))
		point.setX(point.x() - epsilon);
	else if (qa->text() == tr("&Up"))
		point.setY(point.y() - epsilon);
	else if (qa->text() == tr("&Down"))
		point.setY(point.y() + epsilon);

	int newCurrent = mvc->getViewerByPicking(point);
	mvc->updateCurrent(newCurrent);

	if (qa->text() == tr("&Right") || qa->text() == tr("&Left"))
		setSplit(new QAction(tr("&Horizontally"), this));
	else
		setSplit(new QAction(tr("&Vertically"), this));
}

void MainWindow::unsplitFromHandle(QAction * qa)
{
	MultiViewer_Container *mvc = currentViewContainer();

	QPoint point = qa->data().toPoint();
	int epsilon = 10;

	if (qa->text() == tr("&Right"))
		point.setX(point.x() + epsilon);
	else if (qa->text() == tr("&Left"))
		point.setX(point.x() - epsilon);
	else if (qa->text() == tr("&Up"))
		point.setY(point.y() - epsilon);
	else if (qa->text() == tr("&Down"))
		point.setY(point.y() + epsilon);

	int newCurrent = mvc->getViewerByPicking(point);
	mvc->updateCurrent(newCurrent);

	setUnsplit();
}

void MainWindow::linkViewers()
{
	MultiViewer_Container *mvc = currentViewContainer();
	mvc->updateTrackballInViewers();
}

void MainWindow::viewFrom(QAction *qa)
{
	setTopTagView(false);
	if (GLA())
		GLA()->createOrthoView(qa->text());

}

void MainWindow::lock_View(bool bobo)
{
	if (GLA())
	{
		GLA()->lockTrackmouse = bobo;//20160518
		GLA()->templockTrackmouse = bobo;
	}
}
void MainWindow::selection_View(bool bobo)
{
	if (GLA())
	{
		GLA()->selectionSwitch = bobo;

	}
}

void MainWindow::readViewFromFile()
{
	if (GLA()) GLA()->viewFromFile();
	updateMenus();
}


void MainWindow::viewFromCurrentMeshShot()
{
	if (GLA()) GLA()->viewFromCurrentShot("Mesh");
	updateMenus();
}

void MainWindow::viewFromCurrentRasterShot()
{
	if (GLA()) GLA()->viewFromCurrentShot("Raster");
	updateMenus();
}

void MainWindow::copyViewToClipBoard()
{
	if (GLA()) GLA()->viewToClipboard();
}

void MainWindow::pasteViewFromClipboard()
{
	if (GLA()) GLA()->viewFromClipboard();
	updateMenus();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
	//qDebug("dragEnterEvent: %s",event->format());
	event->accept();
	//event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent * event)//drag and drop action is completed.
{
	//qDebug("dropEvent: %s",event->format());
	const QMimeData * data = event->mimeData();
	if (data->hasUrls())
	{
		QList< QUrl > url_list = data->urls();
		bool layervis = false;
		if (layerDialog != NULL)
		{
			layervis = layerDialog->isVisible();
			showLayerDlg(false);
		}
		for (int i = 0, size = url_list.size(); i < size; i++)
		{
			QString path = url_list.at(i).toLocalFile();
			if ((event->keyboardModifiers() == Qt::ControlModifier) || (QApplication::keyboardModifiers() == Qt::ControlModifier))
			{
				this->newProject();
			}

			//if (path.endsWith("mlp", Qt::CaseInsensitive) || path.endsWith("aln", Qt::CaseInsensitive) || path.endsWith("out", Qt::CaseInsensitive) || path.endsWith("nvm", Qt::CaseInsensitive))
			if (path.endsWith("pip", Qt::CaseInsensitive))
				openProject2(path);
			else
			{
				meshCheckSwitch = true;
				importMesh(path);

			}
		}
		//***20160809 don't show layer
		//showLayerDlg(layervis || meshDoc()->meshList.size() > 1);
	}
}

void MainWindow::delCurrentMesh()
{
	//MeshDoc accessed through current container
	currentViewContainer()->meshDoc.delMesh(currentViewContainer()->meshDoc.mm());
	//***20150902***del後排序
	/*if (currentViewContainer()->meshDoc.meshList.size() > 0)
	{
	QMap<QString, MeshFilterInterface *>::iterator msi;
	RichParameterSet dummyParSet;
	executeFilter(PM.actionFilterMap.value("AutoPacking"), dummyParSet, false);
	}*/
	//******
	currentViewContainer()->updateAllViewer();
	GLA()->reFreshGlListlist();
	updateMenus();
}

//***20150813***ctrl+c
//void MainWindow::setCopyMesh()
//{
//	showLayerDlg(false);
//	currentViewContainer()->meshDoc.setSelectedCopy(currentViewContainer()->meshDoc.mm());
//	currentViewContainer()->updateAllViewer();
//	updateMenus();
//}

//*** 20170124 --- modified setCopyMeshes() function by (R&D-I-SW, Mark)
// This function will now copy the meshmodel to a new mesh list (copiedMeshList) to be used even original mesh is deleted
void MainWindow::setCopyMeshes()
{
	showLayerDlg(false);
	currentViewContainer()->meshDoc.copiedMeshList.clear();

	QList<int> list = currentViewContainer()->meshDoc.multiSelectID.toList();
	qSort(list.begin(), list.end(), qLess<int>());

	foreach(int i, list)
	{
		MeshModel *copyMesh = new MeshModel(meshDoc(), meshDoc()->getMesh(i)->fullName(), meshDoc()->getMesh(i)->label());
		copyMesh->updateDataMask(meshDoc()->getMesh(i)->dataMask());//複製mask;
		vcg::tri::Append<CMeshO, CMeshO >::MeshCopy(copyMesh->cm, meshDoc()->getMesh(i)->cm);


		//bool x = meshDoc()->getMesh(i)->cm->hasDataMask(MeshModel::MM_COLOR);
		/*if (meshDoc()->getMesh(i)->hasDataMask(MeshModel::MM_COLOR))
			copyMesh->cm.C() = meshDoc()->getMesh(i)->cm.C();*/

		copyMesh->cm.Tr = meshDoc()->getMesh(i)->cm.Tr;
		copyMesh->rmm = meshDoc()->getMesh(i)->rmm;
		copyMesh->glw.curr_hints = meshDoc()->getMesh(i)->glw.curr_hints;

		meshDoc()->copiedMeshList.push_back(copyMesh);
	}

	//currentViewContainer()->updateAllViewer();
	updateMenus();
}


//***20170131 --- added toCenter() function to avoid error during paste redo command.
// I already replaced other functions that used executeFilter(PM.actionFilterMap.value("MoveToCenter"), dummyParSet, false) to toCenter()
// added by KPPH R&D-I-SW, Mark
void MainWindow::toCenter()
{
	RichParameterSet dummyParSet;
	executeFilter(PM.actionFilterMap.value("MoveToCenter"), dummyParSet, false);
}

void MainWindow::pasteSelectMeshes()
{
	/*if (meshDoc()->multiSelectID.size() < 1 || meshDoc()->mm() == NULL)
		return;*/
	//***20170118 --- modified the code for pasteSelectMeshes to apply multiple copy and paste.
	//Modified by KPPH R&D-I-SW, Mark (lines 1011 - 1038)
	showLayerDlg(false);

	currentViewContainer()->meshDoc.multiSelectID.clear();

	// 20170124 -- change selectedCopyMeshes to copiedMeshList as the source of copied meshes.
	// modified by (R&D-I-SW, Mark)
	if (currentViewContainer()->meshDoc.copiedMeshList.isEmpty() == false)
	{
		int i = 0;
		foreach(MeshModel *cmp, meshDoc()->copiedMeshList)
		{

			currentViewContainer()->meshDoc.copyMesh(cmp);


			//testFuncFunc();

			RichParameterSet dummyParSet;
			executeFilter(PM.actionFilterMap.value("FP_TEST_SEARCH_SPACE_ALL_IMAGE2"), dummyParSet, false);
			qb->show();


			bool x = QCallBack(((i + 1) * 100) / meshDoc()->copiedMeshList.size(), "");
			if ((!filterResult) || (!x))
				break;
			//toCenter();
			//GLA()->update();
			i++;
		}
		qb->reset();
		MultiViewer_Container *tempmvc = currentViewContainer();
		GLArea* tempGLArea = NULL;
		for (int i = 0; i < 1; i++)
		{
			tempGLArea = tempmvc->getViewer(i);

			tempGLArea->updateTexture();

			tempGLArea->setHastoRefreshVBO();

		}

	}
	//setGLlistSwitch(true);
	//GLA()->reFreshGlListlist();

	//testFuncFunc();
	updateMenus();

	//***20170131 --- added paste_command to undostack
	// added by KPPH R&D-I-SW, Mark
	currentViewContainer()->undoStack->push(new paste_command(this, currentViewContainer()->meshDoc.copiedMeshList, false));
}

//***20150813***ctrl+v
//void MainWindow::pastSelectMesh()
//{
//	showLayerDlg(false);
//	currentViewContainer()->meshDoc.copyMesh(currentViewContainer()->meshDoc.selCm());
//
//	//***
//	MultiViewer_Container *tempmvc = currentViewContainer();
//	GLArea* tempGLArea = NULL;
//	//for (int i = 0; i < 3; i++)
//	for (int i = 0; i < 1; i++)
//	{
//		tempGLArea = tempmvc->getViewer(i);
//		tempGLArea->updateTexture();
//	}
/*MeshModel *model = (*meshDoc()).mm();
for (int i = 0; i < model->glw.TMIdd[1].size(); i++)
{
qDebug() << "copy_tmidd" << model->glw.TMIdd[1][i];
qDebug() << "sel_tmidd" << meshDoc()->selCm()->glw.TMIdd[1][i];

}*/
/*if (tri::HasPerWedgeTexCoord(model->cm))
qDebug()<<"maskSelectMesh";*/

//***
//currentViewContainer()->updateAllViewer();
//***做排序*******************
//RichParameterSet dummyParSet;
//executeFilter(PM.actionFilterMap.value("AutoPacking"), dummyParSet, false);//***20160607_test_hidden

//***20170113 --- if statement for executeFilter below was added by KPPH R&D-I-SW, Mark (lines 1043 - 1046)
//if (meshDoc()->selCm() > 0)
//{
//	executeFilter(PM.actionFilterMap.value("MoveToCenter"), dummyParSet, false);//***20160607_test_hidden
//}

//updateMenus();
//****************************

//}

//void MainWindow::delM



void MainWindow::delCurrentRaster()
{
	//MeshDoc accessed through current container
	currentViewContainer()->meshDoc.delRaster(currentViewContainer()->meshDoc.rm());
	updateMenus();
}

void MainWindow::endEdit()
{
	GLA()->endEdit();
}
void MainWindow::applyLastFilter()
{
	if (GLA() == 0) return;
	GLA()->getLastAppliedFilter()->activate(QAction::Trigger);
}
void MainWindow::showFilterScript()
{
	if (meshDoc()->filterHistory != NULL)
	{
		FilterScriptDialog dialog(this);

		dialog.setScript(meshDoc()->filterHistory);
		if (dialog.exec() == QDialog::Accepted)
		{
			runFilterScript();
			return;
		}
	}
}

void MainWindow::runFilterScript()
{
	if ((meshDoc() == NULL) || (meshDoc()->filterHistory == NULL))
		return;
	for (FilterScript::iterator ii = meshDoc()->filterHistory->filtparlist.begin(); ii != meshDoc()->filterHistory->filtparlist.end(); ++ii)
	{
		QString filtnm = (*ii)->filterName();
		int classes = 0;
		if (!(*ii)->isXMLFilter())
		{
			QAction *action = PM.actionFilterMap[filtnm];
			MeshFilterInterface *iFilter = qobject_cast<MeshFilterInterface *>(action->parent());

			int req = iFilter->getRequirements(action);
			if (meshDoc()->mm() != NULL)
				meshDoc()->mm()->updateDataMask(req);
			iFilter->setLog(&meshDoc()->Log);
			OldFilterNameParameterValuesPair* old = reinterpret_cast<OldFilterNameParameterValuesPair*>(*ii);
			RichParameterSet &parameterSet = old->pair.second;

			for (int i = 0; i < parameterSet.paramList.size(); i++)
			{
				//get a modifieable reference
				RichParameter* parameter = parameterSet.paramList[i];

				//if this is a mesh paramter and the index is valid
				if (parameter->val->isMesh())
				{
					RichMesh* md = reinterpret_cast<RichMesh*>(parameter);
					if (md->meshindex < meshDoc()->size() &&
						md->meshindex >= 0)
					{
						RichMesh* rmesh = new RichMesh(parameter->name, md->meshindex, meshDoc());
						parameterSet.paramList.replace(i, rmesh);
					}
					else
					{
						printf("Meshes loaded: %i, meshes asked for: %i \n", meshDoc()->size(), md->meshindex);
						printf("One of the filters in the script needs more meshes than you have loaded.\n");
						return;
					}
					delete parameter;
				}
			}
			//iFilter->applyFilter( action, *(meshDoc()->mm()), (*ii).second, QCallBack );
			QGLWidget wid;
			iFilter->glContext = new QGLContext(QGLFormat::defaultFormat(), wid.context()->device());
			bool created = iFilter->glContext->create(wid.context());
			if ((!created) || (!iFilter->glContext->isValid()))
				throw MeshLabException("A valid GLContext is required by the filter to work.\n");
			meshDoc()->setBusy(true);
			//WARNING!!!!!!!!!!!!
			/* to be changed */
			iFilter->applyFilter(action, *meshDoc(), old->pair.second, QCallBack);
			meshDoc()->setBusy(false);
			delete iFilter->glContext;
			classes = int(iFilter->getClass(action));
		}
		else
		{
			MeshLabXMLFilterContainer& cont = PM.stringXMLFilterMap[filtnm];
			MLXMLPluginInfo* info = cont.xmlInfo;
			MeshLabFilterInterface* cppfilt = cont.filterInterface;
			try
			{
				if (cppfilt != NULL)
				{
					cppfilt->setLog(&meshDoc()->Log);

					Env env;
					QScriptValue val = env.loadMLScriptEnv(*meshDoc(), PM);
					XMLFilterNameParameterValuesPair* xmlfilt = reinterpret_cast<XMLFilterNameParameterValuesPair*>(*ii);
					QMap<QString, QString>& parmap = xmlfilt->pair.second;
					for (QMap<QString, QString>::const_iterator it = parmap.constBegin(); it != parmap.constEnd(); ++it)
						env.insertExpressionBinding(it.key(), it.value());

					EnvWrap envwrap(env);
					MLXMLPluginInfo::XMLMapList params = info->filterParameters(filtnm);
					for (int i = 0; i < params.size(); ++i)
					{
						MLXMLPluginInfo::XMLMap& parinfo = params[i];

						//if this is a mesh parameter and the index is valid
						if (parinfo[MLXMLElNames::paramType] == MLXMLElNames::meshType)
						{
							QString& parnm = parinfo[MLXMLElNames::paramName];
							MeshModel* meshmdl = envwrap.evalMesh(parnm);
							if (meshmdl == NULL)
							{
								//parnm is associated with ,
								printf("Meshes loaded: %i, meshes asked for: %i \n", meshDoc()->size(), envwrap.evalInt(parnm));
								printf("One of the filters in the script needs more meshes than you have loaded.\n");
								return;
							}
						}
					}
					disconnect(meshDoc(), SIGNAL(documentUpdated()), GLA(), SLOT(completeUpdateRequested()));
					QGLWidget wid;
					cppfilt->glContext = new QGLContext(QGLFormat::defaultFormat(), wid.context()->device());
					bool created = cppfilt->glContext->create(wid.context());
					if ((!created) || (!cppfilt->glContext->isValid()))
						throw MeshLabException("A valid GLContext is required by the filter to work.\n");


					//WARNING!!!!!!!!!!!!
					/* IT SHOULD INVOKE executeFilter function. Unfortunately this function create a different thread for each invoked filter, and the MeshLab synchronization mechanisms are quite naive. Better to invoke the filters list in the same thread*/
					meshDoc()->setBusy(true);
					cppfilt->applyFilter(filtnm, *meshDoc(), envwrap, QCallBack);
					meshDoc()->setBusy(false);
					///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					delete cppfilt->glContext;
					GLA()->completeUpdateRequested();
					connect(meshDoc(), SIGNAL(documentUpdated()), GLA(), SLOT(completeUpdateRequested()));
					/* executeFilter(&cont,*env);*/
					QStringList filterClassesList = cont.xmlInfo->filterAttribute(filtnm, MLXMLElNames::filterClass).split(QRegExp("\\W+"), QString::SkipEmptyParts);
					classes = MeshLabFilterInterface::convertStringListToCategoryEnum(filterClassesList);
				}
				else
					throw MeshLabException("WARNING! The MeshLab Script System is able to manage just the C++ XML filters.");
			}
			catch (MeshLabException& e)
			{
				meshDoc()->Log.Log(GLLogStream::WARNING, e.what());
			}
		}
		if (meshDoc()->mm() != NULL)
		{
			if (classes & MeshFilterInterface::FaceColoring)
			{
				GLA()->setColorMode(vcg::GLW::CMPerFace);
				meshDoc()->mm()->updateDataMask(MeshModel::MM_FACECOLOR);
			}
			if (classes & MeshFilterInterface::VertexColoring) {
				GLA()->setColorMode(vcg::GLW::CMPerVert);
				meshDoc()->mm()->updateDataMask(MeshModel::MM_VERTCOLOR);
			}
			if (classes & MeshModel::MM_COLOR)
			{
				GLA()->setColorMode(vcg::GLW::CMPerMesh);
				meshDoc()->mm()->updateDataMask(MeshModel::MM_COLOR);
			}
			if (classes & MeshModel::MM_CAMERA)
				meshDoc()->mm()->updateDataMask(MeshModel::MM_CAMERA);
		}

		if (classes & MeshFilterInterface::Selection)
		{
			GLA()->setSelectFaceRendering(true);
			GLA()->setSelectVertRendering(true);
		}

		if (classes & MeshFilterInterface::MeshCreation)
			GLA()->resetTrackBall();
		/* to be changed */

		qb->reset();
		GLA()->update();
		GLA()->Logf(GLLogStream::SYSTEM, "Re-Applied filter %s", qPrintable((*ii)->filterName()));
	}
}

// Receives the action that wants to show a tooltip and display it
// on screen at the current mouse position.
// TODO: have the tooltip always display with fixed width at the right
//       hand side of the menu entry (not invasive)
void MainWindow::showTooltip(QAction* q)
{
	QString tip = q->toolTip();
	QToolTip::showText(QCursor::pos(), tip);
}

// /////////////////////////////////////////////////
// The Very Important Procedure of applying a filter
// /////////////////////////////////////////////////
// It is splitted in two part
// - startFilter that setup the dialogs and asks for parameters
// - executeFilter callback invoked when the params have been set up.


void MainWindow::startFilter()
{
	delOl(meshDoc()->p_setting.getOlMeshName(), meshDoc()->p_setting.getCapMeshName());

	if (currentViewContainer() == NULL) return;
	if (GLA() == NULL) return;

	// In order to avoid that a filter changes something assumed by the current editing tool,
	// before actually starting the filter we close the current editing tool (if any).
	if (GLA()) GLA()->endEdit();
	updateMenus();

	QStringList missingPreconditions;
	QAction *action = qobject_cast<QAction *>(sender());
	MeshFilterInterface *iFilter = qobject_cast<MeshFilterInterface *>(action->parent());
	if (meshDoc() == NULL)
		return;
	//OLD FILTER PHILOSOPHY
	if (iFilter != NULL)
	{
		//if(iFilter->getClass(action) == MeshFilterInterface::MeshCreation)
		//{
		//	qDebug("MeshCreation");
		//	GLA()->meshDoc->addNewMesh("",iFilter->filterName(action) );
		//}
		//else
		if (!iFilter->isFilterApplicable(action, (*meshDoc()->mm()), missingPreconditions))
		{
			QString enstr = missingPreconditions.join(",");
			QMessageBox::warning(0, tr("PreConditions' Failure"), QString("Warning the filter <font color=red>'" + iFilter->filterName(action) + "'</font> has not been applied.<br>"
				"Current mesh does not have <i>" + enstr + "</i>."));
			return;
		}

		if (currentViewContainer())
		{
			iFilter->setLog(currentViewContainer()->LogPtr());
			currentViewContainer()->LogPtr()->SetBookmark();
		}
		// just to be sure...
		createStdPluginWnd();

		if (xmldialog != NULL)
		{
			xmldialog->close();
			delete xmldialog;
			xmldialog = NULL;
		}
		// (2) Ask for filter parameters and eventally directly invoke the filter
		// showAutoDialog return true if a dialog have been created (and therefore the execution is demanded to the apply event)
		// if no dialog is created the filter must be executed immediately
		if (!stddialog->showAutoDialog(iFilter, meshDoc()->mm(), (meshDoc()), action, this, GLA()))
		{
			RichParameterSet dummyParSet;
			executeFilter(action, dummyParSet, false);
		}
	}
	else // NEW XML PHILOSOPHY
	{
		MeshLabFilterInterface *iXMLFilter = qobject_cast<MeshLabFilterInterface *>(action->parent());
		QString fname = action->text();
		MeshLabXMLFilterContainer& filt = PM.stringXMLFilterMap[fname];
		try
		{
			if ((filt.xmlInfo == NULL) || (filt.act == NULL))
				throw MeshLabException("An invalid MLXMLPluginInfo handle has been detected in startFilter function.");
			QString filterClasses = filt.xmlInfo->filterAttribute(fname, MLXMLElNames::filterClass);
			QStringList filterClassesList = filterClasses.split(QRegExp("\\W+"), QString::SkipEmptyParts);
			if (filterClassesList.contains("MeshCreation"))
			{
				qDebug("MeshCreation");
				meshDoc()->addNewMesh("", "untitled.ply");
			}
			else
			{
				QString preCond = filt.xmlInfo->filterAttribute(fname, MLXMLElNames::filterPreCond);
				QStringList preCondList = preCond.split(QRegExp("\\W+"), QString::SkipEmptyParts);
				int preCondMask = MeshLabFilterInterface::convertStringListToMeshElementEnum(preCondList);
				if (!MeshLabFilterInterface::arePreCondsValid(preCondMask, (*meshDoc()->mm()), missingPreconditions))
				{
					QString enstr = missingPreconditions.join(",");
					QMessageBox::warning(0, tr("PreConditions' Failure"), QString("Warning the filter <font color=red>'" + fname + "'</font> has not been applied.<br>"
						"Current mesh does not have <i>" + enstr + "</i>."));
					return;
				}
			}
			//INIT PARAMETERS WITH EXPRESSION : Both are defined inside the XML file



			//Inside the MapList there are QMap<QString,QString> containing info about parameters. In particular:
			// "type" - "Boolean","Real" etc
			// "name" - "parameter name"
			// "defaultExpression" - "defExpression"
			// "help" - "parameter help"
			// "typeGui" - "ABSPERC_GUI" "CHECKBOX_GUI" etc
			// "label" - "gui label"
			// Depending to the typeGui could be inside the map other info:
			// for example for ABSPERC_GUI there are also
			// "minExpr" - "minExpr"
			// "maxExpr" - "maxExpr"

			MLXMLPluginInfo::XMLMapList params = filt.xmlInfo->filterParametersExtendedInfo(fname);


			/*****IMPORTANT NOTE******/
			//the popContext will be called:
			//- or in the executeFilter if the filter will be executed
			//- or in the close Event of stdDialog window if the filter will NOT be executed
			//- or in the catch exception if something went wrong during parsing/scanning

			try
			{

				if (currentViewContainer())
				{
					if (iXMLFilter)
						iXMLFilter->setLog(currentViewContainer()->LogPtr());
					currentViewContainer()->LogPtr()->SetBookmark();
				}
				// just to be sure...
				createXMLStdPluginWnd();
				if (stddialog != NULL)
				{
					stddialog->close();
					delete stddialog;
					stddialog = NULL;
				}
				// (2) Ask for filter parameters and eventally directly invoke the filter
				// showAutoDialog return true if a dialog have been created (and therefore the execution is demanded to the apply event)
				// if no dialog is created the filter must be executed immediatel
				if (!xmldialog->showAutoDialog(filt, PM, meshDoc(), this, GLA()))
				{
					/*Mock Parameters (there are no ones in the filter indeed) for the filter history.The filters with parameters are inserted by the applyClick of the XMLStdParDialog.
					That is the only place where I can easily evaluate the parameter values without writing a long, boring and horrible if on the filter type for the correct evaluation of the expressions contained inside the XMLWidgets*/
					QMap<QString, QString> mock;
					meshDoc()->filterHistory->addExecutedXMLFilter(fname, mock);

					//executeFilter(&filt, mock, false);
					meshDoc()->Log.Logf(GLLogStream::SYSTEM, "OUT OF SCOPE\n");
				}
				//delete env;
			}
			catch (MeshLabException& e)
			{
				meshDoc()->Log.Logf(GLLogStream::SYSTEM, e.what());
			}
		}
		catch (ParsingException e)
		{
			meshDoc()->Log.Logf(GLLogStream::SYSTEM, e.what());
		}
	}

}

/*
callback function that actually start the chosen filter.
it is called once the parameters have been filled.
It can be called
from the automatic dialog
from the user defined dialog
*/
//void test_cast(QObject **returnedObject)
//{
//	MeshFilterInterface *iFilter = new MeshFilterInterface;
//	
//	*returnedObject = iFilter;
//}

void MainWindow::executeFilter(QAction *action, RichParameterSet &params, bool isPreview)
{

	QByteArray  tempname2 = action->text().toLatin1();
	const char *c_str3 = tempname2.data();
	wchar_t *wmsg2 = new wchar_t[strlen(c_str3) + 1]; //memory allocation
	mbstowcs(wmsg2, c_str3, strlen(c_str3) + 1);
	WRITELOG(logger, framework::Diagnostics::LogLevel::Info, wmsg2);


	WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("start executefilter"));
	delOl(MeshModel::meshsort::slice_item);
	filter_process_running = true;

	//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("executefilter"));
	//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("executefilter"));
	MeshFilterInterface  *iFilter = qobject_cast<MeshFilterInterface *>(action->parent());

	qb->show();

	iFilter->setLog(&meshDoc()->Log);
	WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("executefilter"));
	// Ask for filter requirements (eg a filter can need topology, border flags etc)
	// and statisfy them
	WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("executefilter"));
	qApp->setOverrideCursor(QCursor(Qt::WaitCursor));//***設置游標
	//MainWindow::globalStatusBar()->showMessage("Starting Filter...", 5000);//***設置狀態列
	int req = iFilter->getRequirements(action);//***目前filter沒有re-implement這個函式
	if (!meshDoc()->meshList.isEmpty())
		meshDoc()->mm()->updateDataMask(req);
	qApp->restoreOverrideCursor();

	QMdiSubWindow *tempmvcc = mdiarea->currentSubWindow();
	WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("executefilter"));
	// (3) save the current filter and its parameters in the history**儲存用過的filter在script裡
	if (!isPreview)
	{
		if (meshDoc()->filterHistory != NULL)
		{
			OldFilterNameParameterValuesPair* oldpair = new OldFilterNameParameterValuesPair();
			oldpair->pair = qMakePair(action->text(), params);
			meshDoc()->filterHistory->filtparlist.append(oldpair);
		}
		meshDoc()->Log.ClearBookmark();
	}
	else
		meshDoc()->Log.BackToBookmark();
	// (4) Apply the Filter
	bool ret;
	bool ret2 = false;
	WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("executefilter"));
	qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
	QTime tt; tt.start();
	meshDoc()->setBusy(true);
	updateMenus();
	RichParameterSet MergedEnvironment(params);
	MergedEnvironment.join(currentGlobalParams);
	params.join(currentGlobalParams);
	//GLA() is only the parent
	//***20150908***記憶體增加****罪魁禍首/////////////////////////
	QGLWidget* filterWidget = new QGLWidget(GLA());
	QGLFormat defForm = QGLFormat::defaultFormat();
	iFilter->glContext = new QGLContext(defForm, filterWidget->context()->device());
	iFilter->glContext->create(filterWidget->context());

	WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("executefilter"));
	QSet<MeshModel * > tempMM;
	foreach(MeshModel *tmm, meshDoc()->meshList)
	{
		if (tmm->getMeshSort() == MeshModel::meshsort::print_item || tmm->getMeshSort() == MeshModel::meshsort::pre_print_item)
		{
			int x = tmm->glw.curr_hints & GLW::Hint::HNUseVBO;
			if ((tmm->glw.curr_hints & GLW::Hint::HNUseVBO) != 0)
			{
				tmm->glw.ClearHint(GLW::Hint::HNUseVBO);
				tempMM.insert(tmm);
			}

		}
	}
	WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("executefilter"));
	//qDebug() << "QThread::currentThread()" << QThread::currentThread();	
	//filterThread = new QThread(this);
	//filterWidget = new QGLWidget(GLA());
	////QGLWidget* filterWidget = new QGLWidget();
	//QGLFormat defForm = QGLFormat::defaultFormat();
	//qobject_cast<QGLWidget*>(filterWidget)->doneCurrent();
	//qobject_cast<QGLWidget*>(filterWidget)->context()->moveToThread(filterThread);	
	//iFilter->glContext = new QGLContext(defForm, qobject_cast<QGLWidget*>(filterWidget)->context()->device());
	//iFilter->glContext->create(qobject_cast<QGLWidget*>(filterWidget)->context());
	//qDebug() << "filterThread" << filterThread->currentThreadId();


	try//***20150410*******applyFilter*******************
	{
		//***2010428
		// showAutoDialog return true if a dialog have been created (and therefore the execution is demanded to the apply event)		
		//****
		qDebug("applyFilter");
		//***20150908		
		//delOl("_temp_outlines", "_temp_ol_mesh");
		delOl(meshDoc()->p_setting.getOlMeshName(), meshDoc()->p_setting.getCapMeshName());

		//***
		showLayerDlg(false);//暫時解決當掉問題
		WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("executefilter"));

		if (currentViewContainer() == NULL)
		{
			ret = iFilter->applyFilter(action, *(meshDoc()), MergedEnvironment, QCallBack);//按下apply按鈕filter的反應        
		}
		//else if (action->text().contains("Auto Packing"))
		else if (action->text().contains("FP_TEST_SEARCH_SPACE_ALL_IMAGE2"))
		{
			MergedEnvironment.addParam(new RichBool("auto_rotate_or_not", false, "auto_rotate", ""));
			ret = iFilter->applyFilter(action, *(meshDoc()), MergedEnvironment, QCallBack, GLA());
		}
		//else if (action->text().contains("Auto Packing"))
		else if (action->text().contains("FP_TEST_SEARCH_SPACE_ALL_IMAGE") || action->text().contains("Auto Packing"))
		{

			QMap<int, Matrix44m> savematrix;
			QMap<int, Point3f> savePos;
			QSet<int> selmeshid;
			foreach(MeshModel * mp, meshDoc()->meshList)
			{
				savematrix.insert(mp->id(), mp->cm.Tr);
				savePos.insert(mp->id(), mp->cm.bbox.Center());
			}

			QMessageBox mBox;
			mBox.setIcon(QMessageBox::Question);
			mBox.setWindowTitle(tr("BCPware"));
			mBox.setText(tr("Packing With Auto Rotate?"));
			mBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			mBox.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
			mBox.setCursor(QCursor(Qt::ArrowCursor));
			QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
			mBox.exec();
			QApplication::restoreOverrideCursor();

			if (mBox.result() == QMessageBox::Yes)
			{
				QMap<int, Point3d> firstRotation;
				QMap<int, Point3d> secondRotation;

				/*foreach(MeshModel *tmm, meshDoc()->meshList)
				{*/
				foreach(int i, meshDoc()->multiSelectID)
				{
					MeshModel *tmm = meshDoc()->getMesh(i);
					int minDim = tmm->cm.bbox.MinDim();
					float dimX = tmm->cm.bbox.DimX();
					float dimY = tmm->cm.bbox.DimY();
					float dimZ = tmm->cm.bbox.DimZ();

					if (dimX == dimY && dimY == dimZ)
					{
						firstRotation.insert(tmm->id(), Point3d(0.00, 0.00, 0.00));
						secondRotation.insert(tmm->id(), Point3d(0.00, 0.00, 0.00));
					}
					else
					{
						switch (minDim)
						{
						case 0:
						{
							if (dimX == dimY)
							{
								firstRotation.insert(tmm->id(), Point3d(0.00, 0.00, 0.00));
								break;
							}
							else
							{
								firstRotation.insert(tmm->id(), Point3d(0.00, 90.00, 0.00));
								float temp = dimZ;
								dimZ = dimX;
								dimX = temp;
							}
							break;
						}
						case 1:
						{
							if (dimX == dimY)
							{
								firstRotation.insert(tmm->id(), Point3d(0.00, 0.00, 0.00));
								break;
							}
							else
							{
								firstRotation.insert(tmm->id(), Point3d(90.00, 0.00, 0.00));
								float temp = dimZ;
								dimZ = dimY;
								dimY = temp;
							}
							break;
						}
						default:
						{
							firstRotation.insert(tmm->id(), Point3d(0.00, 0.00, 0.00));
							break;
						}
						}
					}

					// get the current largest dim
					float maxTemp = 0.0;
					if (dimX > dimY) maxTemp = dimX;
					else maxTemp = dimY;
					if (maxTemp < dimZ) maxTemp = dimZ;

					int maxDim = 0;
					if (maxTemp == dimX) maxDim = 0;
					else if (maxTemp == dimY) maxDim = 1;
					else if (maxTemp == dimZ) maxDim = 2;

					switch (maxDim)
					{
					case 1:
						//case 0:
					{
						if (dimX == dimZ)
						{
							secondRotation.insert(tmm->id(), Point3d(0.00, 0.00, 0.00));
							break;
						}
						else
						{
							secondRotation.insert(tmm->id(), Point3d(0.00, 0.00, 90.00));
						}
						break;
					}
					case 2:
					{
						if (dimX == dimZ)
						{
							secondRotation.insert(tmm->id(), Point3d(0.00, 0.00, 0.00));
							break;
						}
						else
						{
							secondRotation.insert(tmm->id(), Point3d(90.00, 0.00, 0.00));
						}
						break;
					}
					default:
					{
						secondRotation.insert(tmm->id(), Point3d(0.00, 0.00, 0.00));
						break;
					}
					}
				}

				//MergedEnvironment.addParam(new RichBool("auto_rotate_or_not", true, "auto_rotate", ""));
				//ret = iFilter->applyFilter(action, *(meshDoc()), MergedEnvironment, QCallBack, GLA());
				//packing_rotate_occur(savePos, firstRotation, secondRotation);

				//SYDNY 10/10/2017-----------------------------------------
				QList<float> old_XValues;
				QList<float> old_YValues;
				QList<float> old_ZValues;

				/*foreach(MeshModel * mm, meshDoc()->meshList)
				{*/
				foreach(int i, meshDoc()->multiSelectID)
				{
					MeshModel *mm = meshDoc()->getMesh(i);
					float old_X = mm->cm.bbox.Center().X();
					float old_Y = mm->cm.bbox.Center().Y();
					float old_Z = mm->cm.bbox.Center().Z();

					//float old_RX = roundf(old_X * 100) / 100;
					//float old_RY = roundf(old_Y * 100) / 100;
					//float old_RZ = roundf(old_Z * 100) / 100;

					float old_RX = round(old_X * 10) / 10.0;
					float old_RY = round(old_X * 10) / 10.0;
					float old_RZ = round(old_X * 10) / 10.0;

					old_XValues.insert(mm->id(), old_RX);
					old_YValues.insert(mm->id(), old_RY);
					old_ZValues.insert(mm->id(), old_RZ);
				}

				float total_Old_X = 0;
				float total_Old_Y = 0;
				float total_Old_Z = 0;

				for (QList<float>::iterator i = old_XValues.begin(); i != old_XValues.end(); ++i)
				{
					total_Old_X += *i;
				}
				for (QList<float>::iterator i = old_YValues.begin(); i != old_YValues.end(); ++i)
				{
					total_Old_Y += *i;
				}
				for (QList<float>::iterator i = old_ZValues.begin(); i != old_ZValues.end(); ++i)
				{
					total_Old_Z += *i;
				}

				old_Total = total_Old_X + total_Old_Y + total_Old_Z;


				/*if (new_Total != old_Total)
				{*/
				MergedEnvironment.addParam(new RichBool("auto_rotate_or_not", true, "auto_rotate", ""));
				ret = iFilter->applyFilter(action, *(meshDoc()), MergedEnvironment, QCallBack, GLA());
				//}


				QList<float> new_XValues;
				QList<float> new_YValues;
				QList<float> new_ZValues;

				/*foreach(MeshModel * mm, meshDoc()->meshList)
				{*/
				foreach(int i, meshDoc()->multiSelectID)
				{
					MeshModel *mm = meshDoc()->getMesh(i);
					float new_X = mm->cm.bbox.Center().X();
					float new_Y = mm->cm.bbox.Center().Y();
					float new_Z = mm->cm.bbox.Center().Z();

					//float new_RX = roundf(new_X * 100) / 100;
					//float new_RY = roundf(new_Y * 100) / 100;
					//float new_RZ = roundf(new_Z * 100) / 100;

					float new_RX = round(new_X * 10) / 10.0;
					float new_RY = round(new_X * 10) / 10.0;
					float new_RZ = round(new_X * 10) / 10.0;

					new_XValues.insert(mm->id(), new_RX);
					new_YValues.insert(mm->id(), new_RY);
					new_ZValues.insert(mm->id(), new_RZ);
				}

				qDebug() << "X:" << new_XValues;
				qDebug() << "Y:" << new_YValues;
				qDebug() << "Z:" << new_ZValues;

				float total_New_X = 0;
				float total_New_Y = 0;
				float total_New_Z = 0;

				for (QList<float>::iterator i = new_XValues.begin(); i != new_XValues.end(); ++i)
				{
					total_New_X += *i;
				}
				for (QList<float>::iterator i = new_YValues.begin(); i != new_YValues.end(); ++i)
				{
					total_New_Y += *i;
				}
				for (QList<float>::iterator i = new_ZValues.begin(); i != new_ZValues.end(); ++i)
				{
					total_New_Z += *i;
				}


				new_Total = total_New_X + total_New_Y + total_New_Z;


				/*if (old_Total != new_Total)
				{*/
				packing_rotate_occur(meshDoc()->multiSelectID, savePos, firstRotation, secondRotation);

				//}
			}


			else if (mBox.result() == QMessageBox::No)	// don't close please!
			{
				//MergedEnvironment.addParam(new RichBool("auto_rotate_or_not", false, "auto_rotate", ""));
				//ret = iFilter->applyFilter(action, *(meshDoc()), MergedEnvironment, QCallBack, GLA());//按下apply按鈕filter的反應 
				//packing_occur(savePos);

				QList<float> old_XValues;
				QList<float> old_YValues;
				QList<float> old_ZValues;

				/*foreach(MeshModel * mm, meshDoc()->meshList)
				{*/
				foreach(int i, meshDoc()->multiSelectID)
				{
					MeshModel *mm = meshDoc()->getMesh(i);
					float old_X = mm->cm.bbox.Center().X();
					float old_Y = mm->cm.bbox.Center().Y();
					float old_Z = mm->cm.bbox.Center().Z();

					//float old_RX = roundf(old_X * 100) / 100;
					//float old_RY = roundf(old_Y * 100) / 100;
					//float old_RZ = roundf(old_Z * 100) / 100;

					float old_RX = round(old_X * 10) / 10.0;
					float old_RY = round(old_X * 10) / 10.0;
					float old_RZ = round(old_X * 10) / 10.0;

					old_XValues.insert(mm->id(), old_RX);
					old_YValues.insert(mm->id(), old_RY);
					old_ZValues.insert(mm->id(), old_RZ);
				}

				float total_Old_X = 0;
				float total_Old_Y = 0;
				float total_Old_Z = 0;

				for (QList<float>::iterator i = old_XValues.begin(); i != old_XValues.end(); ++i)
				{
					total_Old_X += *i;
				}
				for (QList<float>::iterator i = old_YValues.begin(); i != old_YValues.end(); ++i)
				{
					total_Old_Y += *i;
				}
				for (QList<float>::iterator i = old_ZValues.begin(); i != old_ZValues.end(); ++i)
				{
					total_Old_Z += *i;
				}

				old_Total2 = total_Old_X + total_Old_Y + total_Old_Z;

				/*if (new_Total2 != old_Total2 && old_Total2 != new_Total)
				{*/
				MergedEnvironment.addParam(new RichBool("auto_rotate_or_not", false, "auto_rotate", ""));
				ret = iFilter->applyFilter(action, *(meshDoc()), MergedEnvironment, QCallBack, GLA());//按下apply按鈕filter的反應 
				//}

				QList<float> new_XValues;
				QList<float> new_YValues;
				QList<float> new_ZValues;

				/*foreach(MeshModel * mm, meshDoc()->meshList)
				{*/
				foreach(int i, meshDoc()->multiSelectID)
				{
					MeshModel *mm = meshDoc()->getMesh(i);
					float new_X = mm->cm.bbox.Center().X();
					float new_Y = mm->cm.bbox.Center().Y();
					float new_Z = mm->cm.bbox.Center().Z();

					//float new_RX = roundf(new_X * 100) / 100;
					//float new_RY = roundf(new_Y * 100) / 100;
					//float new_RZ = roundf(new_Z * 100) / 100;

					float new_RX = round(new_X * 10) / 10.0;
					float new_RY = round(new_X * 10) / 10.0;
					float new_RZ = round(new_X * 10) / 10.0;

					new_XValues.insert(mm->id(), new_RX);
					new_YValues.insert(mm->id(), new_RY);
					new_ZValues.insert(mm->id(), new_RZ);
				}

				float total_New_X = 0;
				float total_New_Y = 0;
				float total_New_Z = 0;

				for (QList<float>::iterator i = new_XValues.begin(); i != new_XValues.end(); ++i)
				{
					total_New_X += *i;
				}
				for (QList<float>::iterator i = new_YValues.begin(); i != new_YValues.end(); ++i)
				{
					total_New_Y += *i;
				}
				for (QList<float>::iterator i = new_ZValues.begin(); i != new_ZValues.end(); ++i)
				{
					total_New_Z += *i;
				}


				new_Total2 = total_New_X + total_New_Y + total_New_Z;


				//if (new_Total2 != old_Total2)
				//{
				packing_occur(meshDoc()->multiSelectID, savePos);
				//setGLlistSwitch(true);
				//}
			}
		}
		else if (action->text().contains("MoveToCenter"))
		{
			Matrix44m old_t_matrix = meshDoc()->mm()->cm.Tr;
			ret = iFilter->applyFilter(action, *(meshDoc()), MergedEnvironment, QCallBack, GLA());//按下apply按鈕filter的反應  
			//matrix_changed_2(meshDoc()->multiSelectID, old_t_matrix);
		}
		//else if (action->text().contains("Landing_Tool") && params.getBool("all_or_not") == true)
		else if (action->text().contains("FP_GET_DEPTH_LANDING_TEST2") && params.getBool("all_or_not") == true)
		{
			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("executefilter"));
			Matrix44m old_t_matrix = meshDoc()->mm()->cm.Tr;
			ret = iFilter->applyFilter(action, *(meshDoc()), MergedEnvironment, QCallBack, GLA());//按下apply按鈕filter的反應 
			//matrix_changed_2(meshDoc()->multiSelectID, old_t_matrix);
		}
		else if (action->text().contains("FP_GET_DEPTH_LANDING_TEST2") && params.getBool("all_or_not") == false)
		{
			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("executefilter"));
			Matrix44m old_t_matrix = meshDoc()->mm()->cm.Tr;
			ret = iFilter->applyFilter(action, *(meshDoc()), MergedEnvironment, QCallBack, GLA());//按下apply按鈕filter的反應 
			//matrix_changed_2(meshDoc()->multiSelectID, old_t_matrix);			
		}
		else if (action->text().contains("count_hole"))
		{
			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("executefilter"));
			ret = iFilter->applyFilter(action, *(meshDoc()), params, QCallBack, GLA());//按下apply按鈕filter的反應 
		}
		else if (action->text().contains("FP_PRINT_FLOW_2_V3") || action->text().contains("FP_PRINT_JR"))
		{
			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("executefilter"));
			dynamicWipe.clear();
			finalDynamicPage.clear();
			ret = iFilter->applyFilter(action, *(meshDoc()), MergedEnvironment, QCallBack2, GLA(), QCallBackdata);
			ret2 = ret;
			if (!ret2){
				//print not complete
				QMessageBox::warning(this, tr("Error"), QString("Transfer not complete "));
			}


		}
		else if (action->text().contains("FP_SAVE_DEFAULT_SNAP_SHOT"))
		{
			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("executefilter"));

			ret = iFilter->applyFilter(action, *(meshDoc()), MergedEnvironment, QCallBack2, GLA(), QCallBackdata);



		}
		else if (action->text().contains("FP_ESTIMATE_COLOR"))
		{
			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("executefilter"));
			ret = iFilter->applyFilter(action, *(meshDoc()), params, QCallBack, GLA());//	
		}
		else if (action->text().contains("FP_ESTIMATE_COLOR2"))
		{
			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("executefilter"));
			ret = iFilter->applyFilter(action, *(meshDoc()), params, QCallBack, GLA());//
		}
		else if (action->text().contains("FP_ESTIMATE_SURFACE_COLOR"))
		{
			ret = iFilter->applyFilter(action, *(meshDoc()), params, QCallBack, GLA());//
		}
		else if (action->text().contains("FP_MESH_INTERSECT_TEST"))
		{
			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("FP_MESH_INTERSECT_TEST"));
			ret = iFilter->applyFilter(action, *(meshDoc()), params, QCallBack, GLA());//
			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("done FP_MESH_INTERSECT_TEST"));
		}
		else if (action->text().contains("FP_TEST_GENERATE_POINTS_FROM_GRAY_IMAGE"))
		{
			/*MergedEnvironment.addParam(new RichBool("auto_rotate_or_not", true, "auto_rotate", ""));
			ret = iFilter->applyFilter(action, *(meshDoc()), MergedEnvironment, QCallBack, GLA());*/
			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("FP_TEST_GENERATE_POINTS_FROM_GRAY_IMAGE"));
			ret = iFilter->applyFilter(action, *(meshDoc()), MergedEnvironment, QCallBack, GLA());//
		}
		else
		{
			ret = iFilter->applyFilter(action, *(meshDoc()), MergedEnvironment, QCallBack, GLA());//
		}
		//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("filter"));
		mdiarea->setActiveSubWindow(tempmvcc);
		MultiViewer_Container *tempmvc = currentViewContainer();
		meshDoc()->setBusy(false);

		qApp->restoreOverrideCursor();
		// (5) Apply post filter actions (e.g. recompute non updated stuff if needed)
		//fprintf(dbgff, "c-2-1\n"); //@@@
		//fflush(dbgff);
		if (ret)
		{
			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("executefilter"));
			meshDoc()->Log.Logf(GLLogStream::SYSTEM, "Applied filter %s in %i msec", qPrintable(action->text()), tt.elapsed());
			if (meshDoc()->mm() != NULL)
				meshDoc()->mm()->meshModified() = true;
			//MainWindow::globalStatusBar()->showMessage("Filter successfully completed...", 2000);
			if (GLA())
			{
				GLA()->setWindowModified(true);
				GLA()->setLastAppliedFilter(action);//
			}
			lastFilterAct->setText(QString("Apply filter ") + action->text());
			lastFilterAct->setEnabled(true);
			//fprintf(dbgff, "c-2-2\n"); //@@@
			//fflush(dbgff);
			filterResult = true;

		}
		else // filter has failed. show the message error.
		{
			MultiViewer_Container *tempmvc = currentViewContainer();
			MeshDocument *tmepmd = meshDoc();
			//QMessageBox::warning(this, tr("Filter Failure"), QString("Failure of filter <font color=red>: '%1'</font><br><br>").arg(action->text()) + iFilter->errorMsg()); // text
			meshDoc()->Log.Logf(GLLogStream::SYSTEM, "Filter failed: %s", qPrintable(iFilter->errorMsg()));
			//MainWindow::globalStatusBar()->showMessage("Filter failed...", 2000);
			filterResult = false;
		}
		if (ret2)
		{

			//if (dynamicWipe.size() > 0)
			{
				int jobWipe = countJobWipe(2);
				RichParameterSet tempforSave;
				tempforSave.addParam(new RichInt("Job_Wipe", jobWipe, "Job_Wipe", "Job_Wipe"));
				//tempforSave.addParam(new RichInt("Slice_Layer", dynamicWipe.lastKey(), "Slice_Layer", "Slice_Layer"));
				cmyusage.clear();
				PrinterJobInfo *pjii = new PrinterJobInfo(this, meshDoc(), &currentGlobalParams, &cmyusage);
				tempforSave.join(currentGlobalParams);
				tempforSave.join(params);
				GenPrintInfo::genZxInfo(*meshDoc(), tempforSave, *pjii);
				GenPrintInfo::count_total_Page();
			}

		}
		foreach(MeshModel *tmm, tempMM)
		{
			if (tmm->getMeshSort() == MeshModel::meshsort::print_item || tmm->getMeshSort() == MeshModel::meshsort::pre_print_item)
			{
				if ((tmm->glw.curr_hints & GLW::Hint::HNUseVBO) == 0)
				{
					tmm->glw.SetHint(GLW::Hint::HNUseVBO);

				}

			}
		}


		// at the end for filters that change the color, or selection set the appropriate rendering mode
		if (iFilter->getClass(action) & MeshFilterInterface::FaceColoring) {
			GLA()->setColorMode(vcg::GLW::CMPerFace);
			meshDoc()->mm()->updateDataMask(MeshModel::MM_FACECOLOR);
		}
		if (iFilter->getClass(action) & MeshFilterInterface::VertexColoring) {
			GLA()->setColorMode(vcg::GLW::CMPerVert);
			meshDoc()->mm()->updateDataMask(MeshModel::MM_VERTCOLOR);
		}
		if (iFilter->postCondition(action) & MeshModel::MM_COLOR)
		{
			GLA()->setColorMode(vcg::GLW::CMPerMesh);
			meshDoc()->mm()->updateDataMask(MeshModel::MM_COLOR);
		}

		//fprintf(dbgff, "c-2-2\n"); //@@@
		//fflush(dbgff);
		if (iFilter->postCondition(action) & MeshModel::MM_CAMERA)
			meshDoc()->mm()->updateDataMask(MeshModel::MM_CAMERA);

		if (iFilter->getClass(action) & MeshFilterInterface::Selection)
		{
			GLA()->setSelectVertRendering(true);
			GLA()->setSelectFaceRendering(true);
		}
		if (iFilter->getClass(action) & MeshFilterInterface::MeshCreation)
		{
			GLA()->resetTrackBall();
			GLA()->setHastoRefreshVBO();
		}

		if (iFilter->getClass(action) & MeshFilterInterface::Texture)
		{
			//fprintf(dbgff, "c-2-4\n"); //@@@
			//fflush(dbgff);
			//WARNING!!!!! HERE IT SHOULD BE A CHECK IF THE FILTER IS FOR MESH OR FOR DOCUMENT (IN THIS CASE I SHOULD ACTIVATE ALL THE TEXTURE MODE FOR EVERYONE...)
			//NOW WE HAVE JUST TEXTURE FILTERS WORKING ON SINGLE MESH//應該要啟動所有texture，但是現在只有對一個mesh起動			
			QMap<int, RenderMode>::iterator it = GLA()->rendermodemap.find(meshDoc()->mm()->id());
			if (it != GLA()->rendermodemap.end())
				it.value().setTextureMode(GLW::TMPerWedgeMulti);

			//GLA()->updateTexture();

			//**20150512//
			MultiViewer_Container *tempmvc = currentViewContainer();
			GLArea* tempGLArea = NULL;
			//for (int i = 0; i < 3; i++)
			for (int i = 0; i < 1; i++)
			{
				tempGLArea = tempmvc->getViewer(i);
				//qDebug("viewer=%i", (i));								
				tempGLArea->updateTexture();
			}
			//*************************
		}
		//***update vbo
		if (iFilter->getClass(action) & MeshFilterInterface::MovePos) {
			GLA()->setHastoUpdatePartVBO();
			GLA()->updateTexture();
			GLA()->VBOupdatePart[1] = true;

			//GLA()->setHastoRefreshVBO();
		}




		QByteArray  tempname = iFilter->filterName(action).toLatin1();
		const char *c_str2 = tempname.data();

		wchar_t *wmsg = new wchar_t[strlen(c_str2) + 1]; //memory allocation
		mbstowcs(wmsg, c_str2, strlen(c_str2) + 1);
		WRITELOG(logger, framework::Diagnostics::LogLevel::Info, wmsg);
		delete[]wmsg;
		//fprintf(dbgff, "c-3\n"); //@@@
		//fflush(dbgff);
		//***20150908***讓記憶體不會一直增加
		iFilter->glContext->~QGLContext();
		s_pushbutton->setVisible(false);
		delete filterWidget;//
		GLA()->reFreshGlListlist();
	}
	catch (std::bad_alloc& bdall)
	{
		meshDoc()->setBusy(false);
		qApp->restoreOverrideCursor();
		//QMessageBox::warning(this, tr("Filter Failure"), QString("Operating system was not able to allocate the requested memory.<br><b>Failure of filter <font color=red>: '%1'</font><br>We warmly suggest you to try a 64-bit version of MeshLab.<br>").arg(action->text()) + bdall.what()); // text
		//MainWindow::globalStatusBar()->showMessage("Filter failed...", 2000);
	}
	qb->reset();

	updateMenus();

	//***20150511//
	//GLArea* tempGLArea = NULL;
	//for (int i = 0; i < 3; i++)
	//{
	//	tempGLArea = currentViewContainer()->getViewer(i);
	//	qDebug("viewer=%i", (i));
	//	//stddialog->setVisible(false);
	//	tempGLArea->updateTexture();
	//	//currentViewContainer()->updateAllViewer();
	//	//tempGLArea->setTextureMode(GLW::TextureMode::TMPerWedgeMulti);
	//}

	/*foreach(MeshModel *tmm, meshDoc()->meshList)
	{


	if (tmm->getMeshSort() == MeshModel::meshsort::print_item)
	{


	tmm->glw.SetHint(GLW::Hint::HNUseVBO);
	}

	}*/


	//******************************************
	//GLA()->update(); //now there is the container
	MultiViewer_Container* mvc = currentViewContainer();
	if (mvc)
		mvc->updateAllViewer();
	filter_process_running = false;


}

void MainWindow::initDocumentMeshRenderState(MeshLabXMLFilterContainer* mfc)
{
	/* if (env.isNull())
	throw MeshLabException("Critical error in initDocumentMeshRenderState: Env object inside the QSharedPointer is NULL");*/
	if (meshDoc() == NULL)
		return;

	QString fname = mfc->act->text();
	QString ar = mfc->xmlInfo->filterAttribute(fname, MLXMLElNames::filterArity);

	if ((ar == MLXMLElNames::singleMeshArity) && (meshDoc()->mm() != NULL))
	{

		QTime tt;
		tt.start();
		meshDoc()->renderState().add(meshDoc()->mm()->id(), meshDoc()->mm()->cm);
		GLA()->Logf(0, "Elapsed time %d\n", tt.elapsed());
		return;
	}

	if (ar == MLXMLElNames::fixedArity)
	{
		Env env;
		QScriptValue val = env.loadMLScriptEnv(*meshDoc(), PM);
		EnvWrap envwrap(env);
		//I have to check which are the meshes requested as parameters by the filter. It's disgusting but there is not other way.
		MLXMLPluginInfo::XMLMapList params = mfc->xmlInfo->filterParameters(fname);
		for (int ii = 0; ii < params.size(); ++ii)
		{
			if (params[ii][MLXMLElNames::paramType] == MLXMLElNames::meshType)
			{
				try
				{
					MeshModel* tmp = envwrap.evalMesh(params[ii][MLXMLElNames::paramName]);
					if (tmp != NULL)
						meshDoc()->renderState().add(tmp->id(), tmp->cm);
				}
				catch (ExpressionHasNotThisTypeException&)
				{
					QString st = "parameter " + params[ii][MLXMLElNames::paramName] + "declared of type mesh contains a not mesh value.\n";
					meshDoc()->Log.Logf(GLLogStream::FILTER, qPrintable(st));
				}
			}
		}
		return;
	}

	//In this case I can only copy all the meshes in the document!
	if (ar == MLXMLElNames::variableArity)
	{
		for (int ii = 0; ii < meshDoc()->meshList.size(); ++ii)
			meshDoc()->renderState().add(meshDoc()->meshList[ii]->id(), meshDoc()->meshList[ii]->cm);
		return;
	}
}

void MainWindow::initDocumentRasterRenderState(MeshLabXMLFilterContainer* mfc)
{
	if (meshDoc() == NULL)
		return;
	QString fname = mfc->act->text();
	QString ar = mfc->xmlInfo->filterAttribute(fname, MLXMLElNames::filterRasterArity);

	if ((ar == MLXMLElNames::singleRasterArity) && (meshDoc()->rm() != NULL))
	{
		meshDoc()->renderState().add(meshDoc()->rm()->id(), *meshDoc()->rm());
		return;
	}

	if (ar == MLXMLElNames::fixedRasterArity)
	{
		// TO DO!!!!!! I have to add RasterType in order to understand which are the parameters working on Raster!!!

		//	//I have to check which are the meshes requested as parameters by the filter. It's disgusting but there is not other way.
		//	MLXMLPluginInfo::XMLMapList params = mfc->xmlInfo->filterParameters(fname);
		//	for(int ii = 0;ii < params.size();++ii)
		//	{
		//		if (params[ii][MLXMLElNames::paramType] == MLXMLElNames::meshType)
		//		{
		//			try
		//			{
		//				MeshModel* tmp = env.evalMesh(params[ii][MLXMLElNames::paramName]);
		//				if (tmp != NULL)
		//					meshDoc()->renderState().add(tmp->id(),tmp->cm);
		//			}
		//			catch (ExpressionHasNotThisTypeException& e)
		//			{
		//				QString st = "parameter " + params[ii][MLXMLElNames::paramName] + "declared of type mesh contains a not mesh value.\n";
		//				meshDoc()->Log.Logf(GLLogStream::FILTER,qPrintable(st));
		//			}
		//		}
		//	}
		return;
	}

	//In this case I can only copy all the meshes in the document!
	if (ar == MLXMLElNames::variableRasterArity)
	{
		for (int ii = 0; meshDoc()->rasterList.size(); ++ii)
			if (meshDoc()->rasterList[ii] != NULL)
				meshDoc()->renderState().add(meshDoc()->rasterList[ii]->id(), *meshDoc()->rasterList[ii]);
		return;
	}
}

void MainWindow::executeFilter(MeshLabXMLFilterContainer* mfc, const QMap<QString, QString>& parexpval, bool  ispreview)
{

	if (mfc == NULL)
		return;
	MeshLabFilterInterface    *iFilter = mfc->filterInterface;
	bool jscode = (mfc->xmlInfo->filterScriptCode(mfc->act->text()) != "");
	bool filtercpp = (iFilter != NULL) && (!jscode);

	QString fname = mfc->act->text();
	QString postCond = mfc->xmlInfo->filterAttribute(fname, MLXMLElNames::filterPostCond);
	QStringList postCondList = postCond.split(QRegExp("\\W+"), QString::SkipEmptyParts);
	int postCondMask = MeshLabFilterInterface::convertStringListToMeshElementEnum(postCondList);
	if (postCondMask != MeshModel::MM_NONE)
		initDocumentMeshRenderState(mfc);

	initDocumentRasterRenderState(mfc);

	if (!ispreview)
		meshDoc()->Log.ClearBookmark();
	else
		meshDoc()->Log.BackToBookmark();

	qb->show();
	if (filtercpp)
		iFilter->setLog(&meshDoc()->Log);

	//// Ask for filter requirements (eg a filter can need topology, border flags etc)
	//// and statisfy them
	qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
	//MainWindow::globalStatusBar()->showMessage("Starting Filter...", 5000);
	//int req=iFilter->getRequirements(action);
	meshDoc()->mm()->updateDataMask(postCondMask);
	qApp->restoreOverrideCursor();

	//// (3) save the current filter and its parameters in the history
	//if(!isPreview)
	//{
	//	meshDoc()->filterHistory.actionList.append(qMakePair(action->text(),params));
	//	meshDoc()->Log.ClearBookmark();
	//}
	//else
	//	meshDoc()->Log.BackToBookmark();
	//// (4) Apply the Filter



	bool ret = true;
	qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
	bool isinter = (mfc->xmlInfo->filterAttribute(fname, MLXMLElNames::filterIsInterruptible) == "true");

	if (!isinter) meshDoc()->setBusy(true);

	//RichParameterSet MergedEnvironment(params);
	//MergedEnvironment.join(currentGlobalParams);

	////GLA() is only the parent
	xmlfiltertimer.restart();

	/*if (filtercpp)
	{
	QGLWidget* filterWidget = new QGLWidget(GLA());
	QGLFormat defForm = QGLFormat::defaultFormat();
	iFilter->glContext = new QGLContext(defForm,filterWidget->context()->device());
	iFilter->glContext->create(filterWidget->context());
	}*/
	try
	{
		MLXMLPluginInfo::XMLMapList ml = mfc->xmlInfo->filterParametersExtendedInfo(fname);
		QString funcall = "Plugins." + mfc->xmlInfo->pluginAttribute(MLXMLElNames::pluginScriptName) + "." + mfc->xmlInfo->filterAttribute(fname, MLXMLElNames::filterScriptFunctName) + "(";
		if (mfc->xmlInfo->filterAttribute(fname, MLXMLElNames::filterArity) == MLXMLElNames::singleMeshArity && !jscode)
		{
			funcall = funcall + QString::number(meshDoc()->mm()->id());
			if (ml.size() != 0)
				funcall = funcall + ",";
		}
		for (int ii = 0; ii < ml.size(); ++ii)
		{
			funcall = funcall + parexpval[ml[ii][MLXMLElNames::paramName]];
			if (ii != ml.size() - 1)
				funcall = funcall + ",";
		}
		funcall = funcall + ");";
		if (meshDoc() != NULL)
			meshDoc()->xmlhistory << funcall;
		if (filtercpp)
		{
			enableDocumentSensibleActionsContainer(false);
			FilterThread* ft = new FilterThread(fname, parexpval, PM, *(meshDoc()));

			connect(ft, SIGNAL(finished()), this, SLOT(postFilterExecution()));
			connect(ft, SIGNAL(threadCB(const int, const QString&)), this, SLOT(updateProgressBar(const int, const QString&)));
			connect(xmldialog, SIGNAL(filterInterrupt(const bool)), PM.stringXMLFilterMap[fname].filterInterface, SLOT(setInterrupt(const bool)));

			ft->start();
		}
		else
		{
			QTime t;
			t.start();
			Env env;
			env.loadMLScriptEnv(*meshDoc(), PM);
			QScriptValue result = env.evaluate(funcall);
			scriptCodeExecuted(result, t.elapsed(), "");
			postFilterExecution();

		}
	}
	catch (MeshLabException& e)
	{
		meshDoc()->Log.Logf(GLLogStream::SYSTEM, e.what());
		ret = false;
	}

}

void MainWindow::postFilterExecution()
{
	emit filterExecuted();
	meshDoc()->renderState().clearState();
	qApp->restoreOverrideCursor();
	qb->reset();
	//foreach(QAction* act,filterMenu->actions())
	//    act->setEnabled(true);
	enableDocumentSensibleActionsContainer(true);
	updateMenus();
	GLA()->update(); //now there is the container
	MultiViewer_Container* mvc = currentViewContainer();
	if (mvc)
		mvc->updateAllViewer();

	FilterThread* obj = qobject_cast<FilterThread*>(QObject::sender());
	if (obj == NULL)
		return;
	QMap<QString, MeshLabXMLFilterContainer>::const_iterator mfc = PM.stringXMLFilterMap.find(obj->filterName());
	if (mfc == PM.stringXMLFilterMap.constEnd())
		return;
	if (mfc->filterInterface != NULL)
		mfc->filterInterface->setInterrupt(false);

	QString fname = mfc->act->text();
	meshDoc()->setBusy(false);



	//// (5) Apply post filter actions (e.g. recompute non updated stuff if needed)

	if (obj->succeed())
	{
		meshDoc()->Log.Logf(GLLogStream::SYSTEM, "Applied filter %s in %i msec\n", qPrintable(fname), xmlfiltertimer.elapsed());
		//MainWindow::globalStatusBar()->showMessage("Filter successfully completed...", 2000);
		if (GLA())
		{
			GLA()->setLastAppliedFilter(mfc->act);
			GLA()->setWindowModified(true);
		}
		lastFilterAct->setText(QString("Apply filter ") + fname);
		lastFilterAct->setEnabled(true);
	}
	else // filter has failed. show the message error.
	{
		MeshLabFilterInterface         *iFilter = mfc->filterInterface;
		QMessageBox::warning(this, tr("Filter Failure"), QString("Failure of filter: '%1'\n\n").arg(fname) + iFilter->errorMsg()); // text
		meshDoc()->Log.Logf(GLLogStream::SYSTEM, "Filter failed: %s", qPrintable(iFilter->errorMsg()));
		//MainWindow::globalStatusBar()->showMessage("Filter failed...", 2000);
	}
	// at the end for filters that change the color, or selection set the appropriate rendering mode
	QString filterClasses = mfc->xmlInfo->filterAttribute(fname, MLXMLElNames::filterClass);
	QStringList filterClassesList = filterClasses.split(QRegExp("\\W+"), QString::SkipEmptyParts);
	int mask = MeshLabFilterInterface::convertStringListToCategoryEnum(filterClassesList);
	if (mask & MeshFilterInterface::FaceColoring) {
		GLA()->setColorMode(vcg::GLW::CMPerFace);
		meshDoc()->mm()->updateDataMask(MeshModel::MM_FACECOLOR);
	}
	if (mask & MeshFilterInterface::VertexColoring) {
		GLA()->setColorMode(vcg::GLW::CMPerVert);
		meshDoc()->mm()->updateDataMask(MeshModel::MM_VERTCOLOR);
	}
	if (mask & MeshModel::MM_COLOR)
	{
		GLA()->setColorMode(vcg::GLW::CMPerMesh);
		meshDoc()->mm()->updateDataMask(MeshModel::MM_COLOR);
	}


	//if(iFilter->postCondition(action) & MeshModel::MM_CAMERA)
	//	meshDoc()->mm()->updateDataMask(MeshModel::MM_CAMERA);

	if (mask & MeshFilterInterface::Selection)
	{
		GLA()->setSelectVertRendering(true);
		GLA()->setSelectFaceRendering(true);
	}
	if (mask & MeshFilterInterface::MeshCreation)
		GLA()->resetTrackBall();

	if (mask & MeshFilterInterface::Texture)
	{
		//WARNING!!!!! HERE IT SHOULD BE A CHECK IF THE FILTER IS FOR MESH OR FOR DOCUMENT (IN THIS CASE I SHOULD ACTIVATE ALL THE TEXTURE MODE FOR EVERYONE...)
		//NOW WE HAVE JUST TEXTURE FILTERS WORKING ON SINGLE MESH
		QMap<int, RenderMode>::iterator it = GLA()->rendermodemap.find(meshDoc()->mm()->id());
		if (it != GLA()->rendermodemap.end())
			it.value().setTextureMode(GLW::TMPerWedgeMulti);
		GLA()->updateTexture();
	}
	/* QMap<QThread*,Env*>::iterator it = envtobedeleted.find(obj);
	if (it == envtobedeleted.end())
	{
	QString err;
	err.sprintf("FilterThread with address: %08p already does not exist.",obj);
	throw MeshLabException(err);
	}
	else
	{
	Env* tmpenv = it.value();
	envtobedeleted.erase(it);
	delete tmpenv;
	}*/
	delete obj;
}

void MainWindow::scriptCodeExecuted(const QScriptValue& val, const int time, const QString& output)
{
	if (val.isError())
	{
		meshDoc()->Log.Logf(GLLogStream::SYSTEM, "Interpreter Error: line %i: %s", val.property("lineNumber").toInt32(), qPrintable(val.toString()));
		layerDialog->updateLog(meshDoc()->Log);
	}
	else
	{
		meshDoc()->Log.Logf(GLLogStream::SYSTEM, "Code executed in %d millisecs.\nOutput:\n%s", time, qPrintable(output));
		GLA()->update();
	}
}

// Edit Mode Managment
// At any point there can be a single editing plugin active.
// When a plugin is active it intercept the mouse actions.
// Each active editing tools
//
//


void MainWindow::suspendEditMode()
{
	// return if no window is open
	if (!GLA()) return;

	// return if no editing action is currently ongoing
	if (!GLA()->getCurrentEditAction()) return;

	GLA()->suspendEditToggle();
	updateMenus();
	GLA()->update();
}
void MainWindow::applyEditMode()
{
	if (!GLA()) { //prevents crash without mesh
		QAction *action = qobject_cast<QAction *>(sender());
		action->setChecked(false);
		return;
	}

	QAction *action = qobject_cast<QAction *>(sender());

	if (GLA()->getCurrentEditAction()) //prevents multiple buttons pushed
	{
		if (action == GLA()->getCurrentEditAction()) // We have double pressed the same action and that means disable that actioon
		{
			if (GLA()->suspendedEditor)
			{
				suspendEditMode();
				return;
			}
			GLA()->endEdit();
			updateMenus();
			return;
		}
		assert(0); // it should be impossible to start an action without having ended the previous one.
		return;
	}

	//if this GLArea does not have an instance of this action's MeshEdit tool then give it one
	if (!GLA()->editorExistsForAction(action))
	{
		MeshEditInterfaceFactory *iEditFactory = qobject_cast<MeshEditInterfaceFactory *>(action->parent());
		MeshEditInterface *iEdit = iEditFactory->getMeshEditInterface(action);
		GLA()->addMeshEditor(action, iEdit);
	}
	GLA()->setCurrentEditAction(action);

	updateMenus();
}

void MainWindow::applyRenderMode()
{
	QAction *action = qobject_cast<QAction *>(sender());		// find the action which has sent the signal 找到誰發出訊號
	if ((GLA() != NULL) && (GLA()->getRenderer() != NULL))
	{
		GLA()->getRenderer()->Finalize(GLA()->getCurrentShaderAction(), meshDoc(), GLA());
		GLA()->setRenderer(NULL, NULL);
	}
	// Make the call to the plugin core
	MeshRenderInterface *iRenderTemp = qobject_cast<MeshRenderInterface *>(action->parent());
	bool initsupport = false;
	if (iRenderTemp != NULL)
	{
		//***20150504註解掉
		iRenderTemp->Init(action, *(meshDoc()), GLA()->rendermodemap, GLA());
		initsupport = iRenderTemp->isSupported();
		if (initsupport)
			GLA()->setRenderer(iRenderTemp, action);//***20150504要將renderinterface給值
		//     else
		//     {
		//         if (!initsupport)
		//{
		//	//qDebug() << "glew false";
		//             QString msg = "The selected shader is not supported by your graphic hardware!";
		//             GLA()->Logf(GLLogStream::SYSTEM,qPrintable(msg));
		//         }
		//         iRenderTemp->Finalize(action,meshDoc(),GLA());
		//     }
	}

	/*I clicked None in renderMenu */
	if ((action->parent() == this) || (!initsupport))
	{
		QString msg("No Shader.");
		GLA()->Logf(GLLogStream::SYSTEM, qPrintable(msg));
		GLA()->setRenderer(0, 0); //default opengl pipeline or vertex and fragment programs not supported
	}

}


void MainWindow::applyDecorateMode()
{
	//***20150804***因為iPerDocDecoratorlist沒有東西，所以先註解掉，強制塞東西進去
	//***if(GLA()->mm() == 0) return;
	QAction *action = qobject_cast<QAction *>(sender());		// find the action which has sent the signal

	MeshDecorateInterface *iDecorateTemp = qobject_cast<MeshDecorateInterface *>(action->parent());

	if (iDecorateTemp->getDecorationClass(action) == MeshDecorateInterface::PerDocument)
	{
		bool found = GLA()->iPerDocDecoratorlist.removeOne(action);
		if (false)
			//if (found)
		{
			iDecorateTemp->endDecorate(action, *meshDoc(), GLA()->glas.currentGlobalParamSet, GLA());
			iDecorateTemp->setLog(NULL);
			GLA()->Logf(GLLogStream::SYSTEM, "Disabled Decorate mode %s", qPrintable(action->text()));
		}
		else {
			iDecorateTemp->setLog(&meshDoc()->Log);
			bool ret = iDecorateTemp->startDecorate(action, *meshDoc(), &currentGlobalParams, GLA());
			if (ret) {
				GLA()->iPerDocDecoratorlist.push_back(action);
				GLA()->Logf(GLLogStream::SYSTEM, "Enabled Decorate mode %s", qPrintable(action->text()));
			}
			else GLA()->Logf(GLLogStream::SYSTEM, "Failed start of Decorate mode %s", qPrintable(action->text()));
		}
	}

	if (iDecorateTemp->getDecorationClass(action) == MeshDecorateInterface::PerMesh)
	{
		MeshModel &currentMeshModel = *(meshDoc()->mm());
		bool found = GLA()->iCurPerMeshDecoratorList().removeOne(action);
		if (found)
		{
			iDecorateTemp->endDecorate(action, currentMeshModel, GLA()->glas.currentGlobalParamSet, GLA());
			iDecorateTemp->setLog(NULL);
			GLA()->Logf(0, "Disabled Decorate mode %s", qPrintable(action->text()));
		}
		else {
			QString errorMessage;
			if (iDecorateTemp->isDecorationApplicable(action, currentMeshModel, errorMessage)) {
				iDecorateTemp->setLog(&meshDoc()->Log);
				bool ret = iDecorateTemp->startDecorate(action, currentMeshModel, &currentGlobalParams, GLA());
				if (ret) {
					GLA()->iCurPerMeshDecoratorList().push_back(action);
					GLA()->Logf(GLLogStream::SYSTEM, "Enabled Decorate mode %s", qPrintable(action->text()));
				}
				else GLA()->Logf(GLLogStream::SYSTEM, "Failed Decorate mode %s", qPrintable(action->text()));
			}
		}
	}

	layerDialog->updateDecoratorParsView();
	layerDialog->updateLog(meshDoc()->Log);
	layerDialog->update();
	GLA()->update();
}

//void MainWindow::setLight()
//{
//	RenderMode* rm = GLA()->getCurrentRenderMode();
//	if (rm != NULL)
//	{
//		GLA()->setLight(!rm->lighting);
//		updateMenus();
//	}
//};
//
//void MainWindow::setDoubleLighting()
//{
//	RenderMode* rm = GLA()->getCurrentRenderMode();
//	if (rm != NULL)
//		GLA()->setLightMode(!rm->doubleSideLighting,LDOUBLE);
//}
//
//void MainWindow::setFancyLighting()
//{
//	RenderMode* rm = GLA()->getCurrentRenderMode();
//	if (rm != NULL)
//		GLA()->setLightMode(!rm->fancyLighting,LFANCY);
//}
//
//void MainWindow::toggleBackFaceCulling()
//{
//	RenderMode* rm = GLA()->getCurrentRenderMode();
//	if (rm != NULL)
//		GLA()->setBackFaceCulling(!rm->backFaceCull);
//}
//
//void MainWindow::toggleSelectFaceRendering()
//{
//  RenderMode* rm = GLA()->getCurrentRenderMode();
//  if (rm != NULL)
//    GLA()->setSelectFaceRendering(!rm->selectedFace);
//}
//
//void MainWindow::toggleSelectVertRendering()
//{
//	RenderMode* rm = GLA()->getCurrentRenderMode();
//	if (rm != NULL)
//		GLA()->setSelectVertRendering(!rm->selectedVert);
//}

/*
Save project. It saves the info of all the layers and the layer themselves. So
*/
//void MainWindow::saveProject()
//{
//	if (meshDoc() == NULL)
//		return;
//	//if a mesh has been created by a create filter we must before to save it. Otherwise the project will refer to a mesh without file name path.
//	foreach(MeshModel * mp, meshDoc()->meshList)
//	{
//		//if (mp->getMeshSort() == MeshModel::meshsort::decorate_item) qDebug() << "dddddddddddddddddddddddddddddddddddddddddddd";
//		if (((mp != NULL) && (mp->fullName().isEmpty())))//***20160324
//		{
//			bool saved = exportMesh(tr(""), mp, false);
//			if (!saved)
//			{
//				QString msg = "Mesh layer " + mp->label() + " cannot be saved on a file.\nProject \"" + meshDoc()->docLabel() + "\" saving has been aborted.";
//				QMessageBox::warning(this, tr("Project Saving Aborted"), msg);
//				return;
//			}
//		}
//	}
//	//QFileDialog* saveDiag = new QFileDialog(this, tr("Save Project File"), lastUsedDirectory.path().append(""), tr("Picasso Project (*.pip);;Align Project (*.aln)"));
//	QFileDialog* saveDiag = new QFileDialog(this, tr("Save Project File"), lastUsedDirectory.path().append(""), tr("Picasso Project (*.pip);"));
//#if defined(Q_OS_WIN)
//	saveDiag->setOption(QFileDialog::DontUseNativeDialog);
//#endif
//	QCheckBox* saveAllFile = new QCheckBox(QString("Save All Files"), saveDiag);
//	saveAllFile->setCheckState(Qt::Unchecked);
//	QCheckBox* onlyVisibleLayers = new QCheckBox(QString("Only Visible Layers"), saveDiag);
//	onlyVisibleLayers->setCheckState(Qt::Unchecked);
//	QGridLayout* layout = qobject_cast<QGridLayout*>(saveDiag->layout());
//	if (layout != NULL)
//	{
//		layout->addWidget(saveAllFile, 4, 2);
//		layout->addWidget(onlyVisibleLayers, 4, 1);
//	}
//	saveDiag->setAcceptMode(QFileDialog::AcceptSave);
//	saveDiag->exec();
//	QStringList files = saveDiag->selectedFiles();
//	//qDebug() << "mainwidow_RunTime" << files;
//	if (files.size() != 1)
//		return;
//	QString fileName = files[0];
//	// this change of dir is needed for subsequent textures/materials loading
//	QFileInfo fi(fileName);
//	if (fi.isDir())
//		return;
//	if (fi.suffix().isEmpty())
//	{
//		QRegExp reg("\\.\\w+");
//		saveDiag->selectedNameFilter().indexOf(reg);
//		QString ext = reg.cap();
//		fileName.append(ext);
//		fi.setFile(fileName);
//	}
//	QDir::setCurrent(fi.absoluteDir().absolutePath());
//
//	/*********WARNING!!!!!! CHANGE IT!!! ALSO IN THE OPENPROJECT FUNCTION********/
//	meshDoc()->setDocLabel(fileName);
//	QMdiSubWindow* sub = mdiarea->currentSubWindow();
//	if (sub != NULL)
//	{
//		sub->setWindowTitle(meshDoc()->docLabel());
//		layerDialog->setWindowTitle(meshDoc()->docLabel());
//	}
//	/****************************************************************************/
//
//	//***
//	//testcopymesh
//	//createFolderSaveProject(*meshDoc(), fileName);
//	//****
//	bool ret;
//	qDebug("Saving aln file %s\n", qPrintable(fileName));
//	if (fileName.isEmpty()) return;
//	else
//	{
//		//save path away so we can use it again
//		QString path = fileName;
//		path.truncate(path.lastIndexOf("/"));
//		lastUsedDirectory.setPath(path);
//	}
//	if (QString(fi.suffix()).toLower() == "aln")
//	{
//		vector<string> meshNameVector;
//		vector<Matrix44m> transfVector;
//
//		foreach(MeshModel * mp, meshDoc()->meshList)
//		{
//			if ((!onlyVisibleLayers->isChecked()) || (mp->visible))
//			{
//				meshNameVector.push_back(qPrintable(mp->relativePathName()));
//				transfVector.push_back(mp->cm.Tr);
//			}
//		}
//		ret = ALNParser::SaveALN(qPrintable(fileName), meshNameVector, transfVector);
//	}
//	else
//		ret = MeshDocumentToXMLFile(*meshDoc(), fileName, onlyVisibleLayers->isChecked());//**************************save XML File
//
//	if (saveAllFile->isChecked())
//	{
//		for (int ii = 0; ii < meshDoc()->meshList.size(); ++ii)
//		{
//			MeshModel* mp = meshDoc()->meshList[ii];
//			if ((!onlyVisibleLayers->isChecked()) || (mp->visible))
//			{
//				ret |= exportMesh(mp->fullName(), mp, true);
//			}
//		}
//	}
//	if (!ret)
//		QMessageBox::critical(this, tr("Picasso Saving Error"), QString("Unable to save project file %1\n").arg(fileName));
//}

void MainWindow::saveProject()
{
	QFileInfo fileInfo = meshDoc()->getFileName();
	if (fileInfo.exists())
	{
		qb->show();
		ThreeMF_Manip *tmf = new ThreeMF_Manip();
		bool ret = tmf->save_3mf(meshDoc(), (const wchar_t*)fileInfo.absoluteFilePath().utf16(), QCallBack);

		if (ret == true)
		{
			QMessageBox::information(this, tr("Project Sucessfully Saved!"), fileInfo.absoluteFilePath());
			currentViewContainer()->undoStack->clear();
		}

		qb->reset();
	}
	else
		saveProjectAs();
}

void MainWindow::saveProject2()
{
	if (meshDoc() == NULL)
		return;
	//if a mesh has been created by a create filter we must before to save it. Otherwise the project will refer to a mesh without file name path.
	foreach(MeshModel * mp, meshDoc()->meshList)
	{
		//if (mp->getMeshSort() == MeshModel::meshsort::decorate_item) qDebug() << "dddddddddddddddddddddddddddddddddddddddddddd";
		if (((mp != NULL) && (mp->fullName().isEmpty())))//***20160324
		{
			bool saved = exportMesh(tr(""), mp, false);
			if (!saved)
			{
				QString msg = "Mesh layer " + mp->label() + " cannot be saved on a file.\nProject \"" + meshDoc()->docLabel() + "\" saving has been aborted.";
				QMessageBox::warning(this, tr("Project Saving Aborted"), msg);
				return;
			}
		}
	}
	//***20160615
	//SaveProject *saveprojectwidget = new SaveProject(this);
	////saveprojectwidget->exec();

	//QStringList *getData = saveprojectwidget->returndata();
	//bool ok_or_cancel = saveprojectwidget->result();
	//saveprojectwidget


	//	//QFileDialog* saveDiag = new QFileDialog(this, tr("Save Project File"), lastUsedDirectory.path().append(""), tr("Picasso Project (*.pip);;Align Project (*.aln)"));
	//	QFileDialog* saveDiag = new QFileDialog(this, tr("Save Project File"), lastUsedDirectory.path().append(""), tr("Picasso Project (*.pip);"));
	//#if defined(Q_OS_WIN)
	//	saveDiag->setOption(QFileDialog::DontUseNativeDialog);
	//#endif
	//	QCheckBox* saveAllFile = new QCheckBox(QString("Save All Files"), saveDiag);
	//	saveAllFile->setCheckState(Qt::Unchecked);
	//	QCheckBox* onlyVisibleLayers = new QCheckBox(QString("Only Visible Layers"), saveDiag);
	//	onlyVisibleLayers->setCheckState(Qt::Unchecked);
	//	QGridLayout* layout = qobject_cast<QGridLayout*>(saveDiag->layout());
	//	if (layout != NULL)
	//	{
	//		layout->addWidget(saveAllFile, 4, 2);
	//		layout->addWidget(onlyVisibleLayers, 4, 1);
	//	}
	//	saveDiag->setAcceptMode(QFileDialog::AcceptSave);
	//	saveDiag->exec();
	//	QStringList files = saveDiag->selectedFiles();
	//	//qDebug() << "mainwidow_RunTime" << files;
	//	if (files.size() != 1)
	//		return;
	//	QString fileName = files[0];
	//	// this change of dir is needed for subsequent textures/materials loading
	//	QFileInfo fi(fileName);
	//	if (fi.isDir())
	//		return;
	//	if (fi.suffix().isEmpty())
	//	{
	//		QRegExp reg("\\.\\w+");
	//		saveDiag->selectedNameFilter().indexOf(reg);
	//		QString ext = reg.cap();
	//		fileName.append(ext);
	//		fi.setFile(fileName);
	//	}
	//	QDir::setCurrent(fi.absoluteDir().absolutePath());

	/*********WARNING!!!!!! CHANGE IT!!! ALSO IN THE OPENPROJECT FUNCTION********/
	/*meshDoc()->setDocLabel(fileName);
	QMdiSubWindow* sub = mdiarea->currentSubWindow();
	if (sub != NULL)
	{
	sub->setWindowTitle(meshDoc()->docLabel());
	layerDialog->setWindowTitle(meshDoc()->docLabel());
	}*/
	/****************************************************************************/

	//***
	//***20160614***testcopymesh
	//if (ok_or_cancel && getData->size() == 2)
	//{
	//	createFolderSaveProject(*meshDoc(), *getData);
	//****
	//bool ret;
	//qDebug("Saving aln file %s\n", qPrintable(fileName));
	//if (getData->isEmpty()) return;
	//else
	//{
	//save path away so we can use it again
	/*QString path = fileName;
	path.truncate(path.lastIndexOf("/"));
	lastUsedDirectory.setPath(path);*/
	//}
	//SYDNY 02/24/2017 used section function to generate .pip file when user uses '/'
	//QString project_name = getData->at(0);
	//QString slash_remover = project_name.section('/', 1);
	//if (getData->at(0).contains('/'))
	//{
	//	QString filepath = getData->at(1) + "/" + getData->at(0) + "/" + slash_remover + ".pip";
	//	ret = MeshDocumentToXMLFile(*meshDoc(), filepath, true);
	//}
	//else
	//{
	//	QString filepath = getData->at(1) + "/" + getData->at(0) + "/" + getData->at(0) + ".pip";
	//	ret = MeshDocumentToXMLFile(*meshDoc(), filepath, true);
	//}

	/*ret = MeshDocumentToXMLFile(*meshDoc(), filepath, true);*///**************************save XML File

	/*if (saveAllFile->isChecked())
	{
	for (int ii = 0; ii < meshDoc()->meshList.size(); ++ii)
	{
	MeshModel* mp = meshDoc()->meshList[ii];
	if ((!onlyVisibleLayers->isChecked()) || (mp->visible))
	{
	ret |= exportMesh(mp->fullName(), mp, true);
	}
	}
	}*/
	/*if (!ret)
	QMessageBox::critical(this, tr("Picasso Saving Error"), QString("Unable to save project file %1\n").arg(fileName));*/
	//	QMessageBox saved;
	//	saved.setText("Project saved. Path: " + getData->at(1) + "/" + getData->at(0));
	//	saved.exec();
	//}

	//SYDNY 03/30/2017 saving .pip file using QFileDialog
	QFileDialog* saveDiag = new QFileDialog(this, tr("Save Project File"), lastUsedDirectory.path().append(""), tr("Palette Project (*.pip);"));
	saveDiag->setAcceptMode(QFileDialog::AcceptSave);
	saveDiag->exec();

	QStringList files = saveDiag->selectedFiles();
	qDebug() << files;
	if (files.size() != 1)
		return;
	{
		createFolderSaveProject2(*meshDoc(), files);

		bool ret;

		if (files.isEmpty())
			return;
		else
		{
			QString filepath = files[0].remove(".pip") + "/" + files[0].section('/', -1);
			ret = MeshDocumentToXMLFile(*meshDoc(), filepath, true);//**************************save XML File

			QMessageBox saved;
			saved.setText("Finished saving the Project.");
			saved.exec();
		}
	}
}

void MainWindow::saveProjectAs()
{
	if (meshDoc() == NULL)
		return;
	const QString DEFAULT_PROJECT_DIR_KEY("default_project_dir");
	QSettings MySettings;
	//QFileDialog* saveDiag = new QFileDialog(this, tr("Save Project File"), lastUsedDirectory.path().append(""), tr("Palette Project (*.3mf);"));
	QFileDialog* saveDiag = new QFileDialog(this, tr("Save Project as"), MySettings.value(DEFAULT_PROJECT_DIR_KEY).toString(), tr("3MF Project (*.3mf);"));
	saveDiag->setAcceptMode(QFileDialog::AcceptSave);
	int result = saveDiag->exec();

	if (result == QFileDialog::Accepted)
	{
		qb->show();
		QFileInfo f = saveDiag->selectedFiles().first();
		QString fileName = f.absoluteFilePath();

		if (f.suffix().toLower() != "3mf")
			fileName = fileName + ".3mf";

		QDir::setCurrent(f.absolutePath());

		ThreeMF_Manip *tmf = new ThreeMF_Manip();
		bool ret = tmf->save_3mf(meshDoc(), (const wchar_t*)fileName.utf16(), QCallBack);
		qb->reset();

		if (ret)
		{
			QMessageBox::information(this, tr("Project Sucessfully Saved!"), fileName);
			currentViewContainer()->undoStack->clear();
			meshDoc()->setFileName(QFileInfo(fileName).absoluteFilePath());
			mdiarea->currentSubWindow()->setWindowTitle(QFileInfo(fileName).fileName());
			meshDoc()->setDocLabel(QFileInfo(fileName).fileName());
			saveRecentProjectList(fileName);
		}
		else
		{
			QMessageBox::information(this, tr("Failed to Save Project"), tr("Can not save project %1").arg(fileName));
		}
	}
}

bool MainWindow::openProject2(QString fileName)
{
	const QString DEFAULT_PROJECT_DIR_KEY("default_project_dir");
	QSettings MySettings;

	if (fileName.isEmpty())
		fileName = QFileDialog::getOpenFileName(this, tr("Open Project"), MySettings.value(DEFAULT_PROJECT_DIR_KEY).toString(), "3MF Project (*.3mf);");

	if (fileName.isEmpty())
		return false;
	else
	{
		QDir CurrentDir;
		MySettings.setValue(DEFAULT_PROJECT_DIR_KEY, CurrentDir.absoluteFilePath(fileName));
	}

	QFileInfo fi(fileName);

	if ((fi.suffix().toLower() != "3mf"))
	{
		QMessageBox::critical(this, tr("BCPware Opening Error"), "Unknown project file extension");
		return false;
	}

	bool activeDoc = (bool)!mdiarea->subWindowList().empty() && mdiarea->currentSubWindow();
	bool activeEmpty = activeDoc && meshDoc()->meshList.empty();

	if (!activeEmpty)
		newProject(fileName);
	//QString temp2 = mdiarea->activeSubWindow()->windowTitle();
	QMdiSubWindow *tempmvcc = mdiarea->currentSubWindow();
	updateCustomSettings();
	meshDoc()->setBusy(true);
	qb->show();
	if (QString(fi.suffix()).toLower() == "3mf")
	{
		LPCWSTR pn = (const wchar_t*)fileName.utf16();
		ThreeMF_Manip *tmf = new ThreeMF_Manip();
		bool ret = tmf->open_3mf(this, meshDoc(), pn, QCallBack, false);
		foreach(MeshModel *mm, meshDoc()->meshList)
		{
			qDebug() << ">>>>" << mm->getMeshSort();
		}


		if (ret == false)
		{
			mdiarea->setActiveSubWindow(tempmvcc);
			QMessageBox::information(this, tr("BCPware Opening Error"), "Cancel Opening file");
			mdiarea->closeActiveSubWindow();
			//meshDoc()->setBusy(false);
			return false;
		}
		else
		{
			emit dispatchCustomSettings(currentGlobalParams);


			QFileInfo f = fileName;
			meshDoc()->setFileName(fileName);
			mdiarea->currentSubWindow()->setWindowTitle(f.fileName());
			meshDoc()->setDocLabel(f.fileName());

			for (int i = 0; i < meshDoc()->meshList.size(); i++)
			{
				if (meshDoc()->meshList[i]->hasDataMask(MeshModel::MM_WEDGTEXCOORD))
				{
					GLA()->updateTexture();
				}

				tri::UpdatePosition<CMeshO>::Matrix(this->meshDoc()->meshList[i]->cm, this->meshDoc()->meshList[i]->cm.Tr, true);
				tri::UpdateBounding<CMeshO>::Box(this->meshDoc()->meshList[i]->cm);
				tri::UpdateNormal<CMeshO>::PerFaceNormalized(this->meshDoc()->meshList[i]->cm);
				GLA()->updateRendermodemapSiganl = true;
			}


			if (this->GLA() == 0)  return false;
			this->currentViewContainer()->resetAllTrackBall();
			saveRecentProjectList(fileName);
			alreadyload = true;
			GLA()->reFreshGlListlist();

			GLA()->setHastoRefreshVBO();
			updateMenus();
		}
	}
	meshDoc()->setBusy(false);
	qb->reset();
	s_pushbutton->setVisible(false);


	//setGLlistSwitch(true);

	return true;
}

bool MainWindow::openProject(QString fileName)
{


	bool visiblelayer = layerDialog->isVisible();
	showLayerDlg(false);

	//***20151014
	const QString DEFAULT_PROJECT_DIR_KEY("default_project_dir");
	QSettings MySettings; // Will be using application informations for correct location of your settings
	//***

	if (fileName.isEmpty())
		//***20151005***fileName = QFileDialog::getOpenFileName(this, tr("Open Project File"), lastUsedDirectory.path(), "All Project Files (*.mlp *.aln *.out *.nvm);;MeshLab Project (*.mlp);;Align Project (*.aln);;Bundler Output (*.out);;VisualSFM Output (*.nvm)");
		//fileName = QFileDialog::getOpenFileName(this, tr("Open Project File"), lastUsedDirectory.path(), "All Project Files (*.mlp );;MeshLab Project (*.mlp);;Align Project (*.aln)");
		//***20160112
		//fileName = QFileDialog::getOpenFileName(this, tr("Open Project File"), MySettings.value(DEFAULT_DIR_KEY).toString(), "All Project Files (*.mlp );;MeshLab Project (*.mlp);;Align Project (*.aln)");//backup
		//fileName = QFileDialog::getOpenFileName(this, tr("Open Project File"), MySettings.value(DEFAULT_DIR_KEY).toString(), "All Project Files (*.pip );;Picasso Project (*.pip);");
		fileName = QFileDialog::getOpenFileName(this, tr("Open Project"), MySettings.value(DEFAULT_PROJECT_DIR_KEY).toString(), "Picasso Project (*.pip);");

	if (fileName.isEmpty()) return false;
	else {
		////***20151014
		QDir CurrentDir;
		MySettings.setValue(DEFAULT_PROJECT_DIR_KEY, CurrentDir.absoluteFilePath(fileName));
		//QMessageBox::information(this, "Info", "You selected the file '" + fileName + "'");
	}

	QFileInfo fi(fileName);
	lastUsedDirectory = fi.absoluteDir();

	//if ((fi.suffix().toLower() != "aln") && (fi.suffix().toLower() != "mlp") && (fi.suffix().toLower() != "out") && (fi.suffix().toLower() != "nvm"))
	if ((fi.suffix().toLower() != "pip"))
	{
		QMessageBox::critical(this, tr("Picasso Opening Error"), "Unknown project file extension");
		return false;
	}

	// Common Part: init a Doc if necessary, and
	bool activeDoc = (bool)!mdiarea->subWindowList().empty() && mdiarea->currentSubWindow();
	bool activeEmpty = activeDoc && meshDoc()->meshList.empty();

	if (!activeEmpty)
		newProject(fileName);

	meshDoc()->setFileName(fileName);
	mdiarea->currentSubWindow()->setWindowTitle(fileName);
	meshDoc()->setDocLabel(fileName);
	meshDoc()->setBusy(true);

	// this change of dir is needed for subsequent textures/materials loading
	QDir::setCurrent(fi.absoluteDir().absolutePath());
	qb->show();

	/*if (QString(fi.suffix()).toLower() == "aln")
	{
	vector<RangeMap> rmv;
	int retVal = ALNParser::ParseALN(rmv, qPrintable(fileName));
	if (retVal != ALNParser::NoError)
	{
	QMessageBox::critical(this, tr("Picasso Opening Error"), "Unable to open ALN file");
	return false;
	}

	bool openRes = true;
	vector<RangeMap>::iterator ir;
	for (ir = rmv.begin(); ir != rmv.end() && openRes; ++ir)
	{
	QString relativeToProj = fi.absoluteDir().absolutePath() + "/" + (*ir).filename.c_str();
	meshDoc()->addNewMesh(relativeToProj, relativeToProj);
	openRes = loadMeshWithStandardParams(relativeToProj, this->meshDoc()->mm());
	if (openRes)
	meshDoc()->mm()->cm.Tr.Import((*ir).trasformation);
	else
	meshDoc()->delMesh(meshDoc()->mm());
	}
	}*/

	//fprintf(dbgff, "a-1\n"); //@@@
	//fflush(dbgff);
	//fprintf(dbgff, "a-doc_sizef%i\n", meshDoc()->meshList.size()); //@@@
	//fflush(dbgff);
	if (QString(fi.suffix()).toLower() == "pip")
	{
		meshCheckSwitch = false;///***關掉meshcheck
		if (!MeshDocumentFromXML(*meshDoc(), fileName))
		{
			QMessageBox::critical(this, tr("Picasso Opening Error"), "Unable to open pip file");
			return false;
		}
		//fprintf(dbgff, "a-doc_size%i\n",meshDoc()->meshList.size()); //@@@
		//fflush(dbgff);

		for (int i = 0; i < meshDoc()->meshList.size(); i++)
		{
			QString fullPath = meshDoc()->meshList[i]->fullName();
			meshDoc()->setBusy(true);
			Matrix44m trm = this->meshDoc()->meshList[i]->cm.Tr; // save the matrix, because loadMeshClear it...	
			Color4b crm = this->meshDoc()->meshList[i]->cm.C();

			if (!loadMeshWithStandardParams(fullPath, this->meshDoc()->meshList[i]))
			{

				meshDoc()->delMesh(meshDoc()->meshList[i]);
				//fprintf(dbgff, "a-1-1\n"); //@@@
				//fflush(dbgff);
			}
			else
			{

				//fprintf(dbgff, "a-1-2\n"); //@@@
				//fflush(dbgff);
				this->meshDoc()->meshList[i]->cm.Tr = trm;


				//qDebug() <<" trm.GetColumn4(4).X()"<< trm.GetColumn4(4).X();
				//***20150828***更新meshDoc()
				tri::UpdatePosition<CMeshO>::Matrix(this->meshDoc()->meshList[i]->cm, this->meshDoc()->meshList[i]->cm.Tr, true);
				tri::UpdateBounding<CMeshO>::Box(this->meshDoc()->meshList[i]->cm);


				//SYDNY 05/16/2017 this will receive the color values from the .pip file
				this->meshDoc()->meshList[i]->cm.C() = crm;
				if (this->meshDoc()->meshList[i]->cm.C()[0] == 255 &&
					this->meshDoc()->meshList[i]->cm.C()[1] == 255 &&
					this->meshDoc()->meshList[i]->cm.C()[2] == 255)
				{
					GLA()->updateRendermodemapSiganl = true;
				}
				else
				{
					tri::UpdateColor<CMeshO>::PerVertexConstant(this->meshDoc()->meshList[i]->cm, this->meshDoc()->meshList[i]->cm.C());
					tri::UpdateNormal<CMeshO>::PerVertex(this->meshDoc()->meshList[i]->cm);
					this->meshDoc()->meshList[i]->Enable(tri::io::Mask::IOM_FACECOLOR);
					this->meshDoc()->meshList[i]->updateDataMask(MeshModel::MM_FACECOLOR);
					tri::UpdateColor<CMeshO>::PerFaceFromVertex(this->meshDoc()->meshList[i]->cm);
					this->meshDoc()->meshList[i]->rmm.colorMode = vcg::GLW::CMPerVert;
					this->meshDoc()->meshList[i]->rmm.textureMode = vcg::GLW::TMNone;
					GLA()->updateRendermodemapSiganl = true;
				}
			}
		}
	}


	if (QString(fi.suffix()).toLower() == "out") {

		QString cameras_filename = fileName;
		QString image_list_filename;
		QString model_filename;

		image_list_filename = QFileDialog::getOpenFileName(
			this, tr("Open image list file"),
			QFileInfo(fileName).absolutePath(),
			tr("Bundler images list file (*.txt)")
			);
		if (image_list_filename.isEmpty())
			return false;


		//model_filename = QFileDialog::getOpenFileName(
		//			this, tr("Open 3D model file"),
		//			QFileInfo(fileName).absolutePath(),
		//			tr("Bunler 3D model file (*.ply)")
		//			);
		//if(model_filename.isEmpty())
		//  return false;

		if (!MeshDocumentFromBundler(*meshDoc(), cameras_filename, image_list_filename, model_filename)) {
			QMessageBox::critical(this, tr("Meshlab Opening Error"), "Unable to open OUTs file");
			return false;
		}

		GLA()->setColorMode(GLW::CMPerVert);
		GLA()->setDrawMode(GLW::DMPoints);
		//else{
		//	for (int i=0; i<meshDoc()->meshList.size(); i++)
		//		{
		//		  QString fullPath = meshDoc()->meshList[i]->fullName();
		//		  meshDoc()->setBusy(true);
		//		  loadMeshWithStandardParams(fullPath,this->meshDoc()->meshList[i]);
		//		}
		//}


	}

	//////NVM

	if (QString(fi.suffix()).toLower() == "nvm") {

		QString cameras_filename = fileName;
		//QString image_list_filename;
		QString model_filename;

		/*image_list_filename = QFileDialog::getOpenFileName(
		this  ,  tr("Open image list file"),
		QFileInfo(fileName).absolutePath(),
		tr("Bundler images list file (*.txt)")
		);
		if(image_list_filename.isEmpty())
		return false;*/


		//model_filename = QFileDialog::getOpenFileName(
		//			this, tr("Open 3D model file"),
		//			QFileInfo(fileName).absolutePath(),
		//			tr("Bunler 3D model file (*.ply)")
		//			);
		//if(model_filename.isEmpty())
		//  return false;


		if (!MeshDocumentFromNvm(*meshDoc(), cameras_filename, model_filename)) {
			QMessageBox::critical(this, tr("picasso Opening Error"), "Unable to open NVMs file");
			return false;
		}
		GLA()->setColorMode(GLW::CMPerVert);
		GLA()->setDrawMode(GLW::DMPoints);

		//else{
		//	for (int i=0; i<meshDoc()->meshList.size(); i++)
		//		{
		//		  QString fullPath = meshDoc()->meshList[i]->fullName();
		//		  meshDoc()->setBusy(true);
		//		  loadMeshWithStandardParams(fullPath,this->meshDoc()->meshList[i]);
		//		}
		//}


	}

	//////NVM	

	meshDoc()->setBusy(false);
	if (this->GLA() == 0)  return false;
	this->currentViewContainer()->resetAllTrackBall();
	qb->reset();
	saveRecentProjectList(fileName);
	//***20150818註解掉showlayerdig
	//showLayerDlg(visiblelayer || (meshDoc()->meshList.size() > 1));
	meshCheckSwitch = true;
	showLayerDlg(false);

	alreadyload = true;

	return true;
}

bool MainWindow::appendProject(QString fileName)
{
	QStringList fileNameList;

	if (fileName.isEmpty())
		//fileNameList = QFileDialog::getOpenFileNames(this, tr("Append Project File"), lastUsedDirectory.path(), "All Project Files (*.mlp *.aln);;MeshLab Project (*.mlp);;Align Project (*.aln)");//backup
		//***20161112
		fileNameList = QFileDialog::getOpenFileNames(this, tr("Append Project File"), lastUsedDirectory.path(), "All Project Files (*.pip *.aln);;picasso Project (*.pip);;Align Project (*.aln)");
	else
		fileNameList.append(fileName);

	if (fileNameList.isEmpty()) return false;

	// Common Part: init a Doc if necessary, and
	bool activeDoc = (bool)!mdiarea->subWindowList().empty() && mdiarea->currentSubWindow();
	bool activeEmpty = activeDoc && meshDoc()->meshList.empty();

	if (activeEmpty)  // it is wrong to try appending to an empty project, even if it is possible
	{
		QMessageBox::critical(this, tr("Picasso Opening Error"), "Current project is empty, cannot append");
		return false;
	}

	meshDoc()->setBusy(true);

	// load all projects
	foreach(fileName, fileNameList)
	{
		QFileInfo fi(fileName);
		lastUsedDirectory = fi.absoluteDir();

		//if ((fi.suffix().toLower() != "aln") && (fi.suffix().toLower() != "mlp"))
		if ((fi.suffix().toLower() != "aln") && (fi.suffix().toLower() != "pip"))
		{
			QMessageBox::critical(this, tr("Picasso Opening Error"), "Unknown project file extension");
			return false;
		}

		// this change of dir is needed for subsequent textures/materials loading
		QDir::setCurrent(fi.absoluteDir().absolutePath());
		qb->show();

		meshCheckSwitch = false;//***20151221
		if (QString(fi.suffix()).toLower() == "aln")
		{
			vector<RangeMap> rmv;
			int retVal = ALNParser::ParseALN(rmv, qPrintable(fileName));
			if (retVal != ALNParser::NoError)
			{
				QMessageBox::critical(this, tr("Meshlab Opening Error"), "Unable to open ALN file");
				return false;
			}

			bool openRes = true;
			vector<RangeMap>::iterator ir;
			for (ir = rmv.begin(); ir != rmv.end() && openRes; ++ir)
			{
				QString relativeToProj = fi.absoluteDir().absolutePath() + "/" + (*ir).filename.c_str();
				meshDoc()->addNewMesh(relativeToProj, relativeToProj);

				openRes = loadMeshWithStandardParams(relativeToProj, this->meshDoc()->mm());
				if (openRes)
					meshDoc()->mm()->cm.Tr.Import((*ir).trasformation);
				else
					meshDoc()->delMesh(meshDoc()->mm());
			}
		}

		//if (QString(fi.suffix()).toLower() == "mlp")
		//***20161112
		if (QString(fi.suffix()).toLower() == "pip")
		{

			if (!MeshDocumentFromXML(*meshDoc(), fileName))
			{
				//QMessageBox::critical(this, tr("Meshlab Opening Error"), "Unable to open MLP file");
				QMessageBox::critical(this, tr("Picasso Opening Error"), "Unable to open pip file");
				return false;
			}
			for (int i = 0; i < meshDoc()->meshList.size(); i++)
			{
				QString fullPath = meshDoc()->meshList[i]->fullName();
				meshDoc()->setBusy(true);
				Matrix44m trm = this->meshDoc()->meshList[i]->cm.Tr; // save the matrix, because loadMeshClear it...
				if (!loadMeshWithStandardParams(fullPath, this->meshDoc()->meshList[i]))
					meshDoc()->delMesh(meshDoc()->meshList[i]);
				else
				{
					this->meshDoc()->meshList[i]->cm.Tr = trm;
					//***20161112***更新meshDoc()										
					tri::UpdatePosition<CMeshO>::Matrix(this->meshDoc()->meshList[i]->cm, this->meshDoc()->meshList[i]->cm.Tr, true);
					tri::UpdateBounding<CMeshO>::Box(this->meshDoc()->meshList[i]->cm);
				}

			}
		}
	}

	meshDoc()->setBusy(false);
	if (this->GLA() == 0)  return false;
	this->currentViewContainer()->resetAllTrackBall();
	qb->reset();
	saveRecentProjectList(fileName);
	meshCheckSwitch = true;//***20151221
	return true;
}
#pragma region Region_1
#if 1
GLArea* MainWindow::newProject(const QString& projName)
{
	MultiViewer_Container *mvcont = new MultiViewer_Container(mdiarea);//
	mvcont->setAttribute(Qt::WA_DeleteOnClose);
	mdiarea->addSubWindow(mvcont);
	mdiarea->setViewMode(QMdiArea::TabbedView);
	mdiarea->setTabsClosable(true);
	mdiarea->setTabsMovable(true);

	//connect(mvcont, SIGNAL(subWindowActivated()), this, SLOT(updateMenus()));
	connect(mvcont, SIGNAL(updateMainWindowMenus()), this, SLOT(updateMenus()));
	filterMenu->setEnabled(!filterMenu->actions().isEmpty());
	if (!filterMenu->actions().isEmpty())
		updateSubFiltersMenu(true, false);
	GLArea *gla = new GLArea(mvcont, &currentGlobalParams);
	mvcont->addView(gla, Qt::Horizontal);
	if (projName.isEmpty())
	{
		static int docCounter = 1;
		mvcont->meshDoc.setDocLabel(QString("Project_") + QString::number(docCounter));
		++docCounter;
	}
	else
		mvcont->meshDoc.setDocLabel(projName);
	mvcont->setWindowTitle(mvcont->meshDoc.docLabel());
	//if(mdiarea->isVisible())

	layerDialog->updateTable();
	layerDialog->updateDecoratorParsView();

	if (&mvcont->meshDoc != NULL)
		mvcont->showMaximized();


	//***20150409
	//setSplit(new QAction(tr("&Vertically"), mvcont));
	//setSplit(new QAction(tr("&Horizontally"), mvcont));

	//int size = mvcont->viewerCounter();
	//qDebug() << "***" << size;
	////設置orthoView為Front與Top
	////------------------------------------------------
	//GLArea* tempGLArea = mvcont->getViewer(0);//***viewer0左下
	//tempGLArea->setFov(5);
	//tempGLArea->vd = GLArea::viewDirection::front;
	//tempGLArea->lockTrackmouse = true;
	//tempGLArea->setDisplayInfo(false);
	//tempGLArea->resetTrackBall();

	//tempGLArea = mvcont->getViewer(2);//***viewer2左上
	//tempGLArea->setFov(5);
	//tempGLArea->vd = GLArea::viewDirection::bottom;
	//tempGLArea->lockTrackmouse = true;
	//tempGLArea->setDisplayInfo(false);
	//tempGLArea->resetTrackBall();

	//tempGLArea = mvcont->getViewer(1);//***viewer1	
	//tempGLArea->setFov(5);
	//tempGLArea->vd = GLArea::viewDirection::bottom;
	//tempGLArea->resetTrackBall();//***不知道為什麼fov的設置會跑掉//20150804***不知道什哪裡會出bug
	//tempGLArea->setDisplayInfo(true);

	//***20150824********useless**********************************
	//tempGLArea = mvcont->getViewer(1);//***viewer1	
	//tempGLArea->setFov(6);
	//tempGLArea->vd = GLArea::viewDirection::topTag;
	//tempGLArea->lockTrackmouse = true;
	//tempGLArea->resetTrackBall();//***不知道為什麼fov的設置會跑掉//20150804***不知道什哪裡會出bug
	//********************************************************************
	//mvcont->updateCurrent(1);//設置主畫面為viewer1
	//***********


	//***For one Viewer---------------------------------------------
	GLArea* tempGLArea = mvcont->getViewer(0);//***viewer0左下	//tempGLArea = mvcont->getViewer(0);//***viewer1	
	tempGLArea->setFov(5);
	//tempGLArea->vd = GLArea::viewDirection::top;
	tempGLArea->vd = GLArea::viewDirection::original;
	tempGLArea->resetTrackBall();//***不知道為什麼fov的設置會跑掉//20150804***不知道什哪裡會出bug
	tempGLArea->setDisplayInfo(true);

	/*QFrame  *pb1 = new QFrame(tempGLArea);
	QVBoxLayout *docklayout = new QVBoxLayout;
	docklayout->addWidget(new QPushButton());
	docklayout->addWidget(new QPushButton());
	docklayout->addWidget(new QPushButton());
	pb1->setLayout(docklayout);*/


	//***20150515設置建構槽大小
	//MeshDocument *tempmdc = meshDoc();
	//tempmdc->groove.setLWH(15, 17, 19);

	//createSliceBar();
	alreadyload = true;

	undoGroup->addStack(currentViewContainer()->undoStack);

	if (objList->isVisible())
		objList->updateUI(true);
	//new gla need to re connect
	connect(this, SIGNAL(slicePositionChanged(double)), GLA(), SLOT(set_equ3(double)));


	return gla;
}

#pragma endregion Region_1


#else
GLArea* MainWindow::newProject(const QString& projName)
{
	//=====================
	QGLWidget *widget = new QGLWidget(QGLFormat(QGL::SampleBuffers));
	widget->makeCurrent();


	scene.addText("Hello, world!!!!!!!!!");
	scene.addWidget(new QPushButton("hello button"));
	scene.addLine(QLineF(0, 0, 54, 54));


	MultiViewer_Container *mvcont = new MultiViewer_Container(mdiarea);//
	GLArea *gla = new GLArea(mvcont, &currentGlobalParams);
	gla->makeCurrent();
	//======================
	//view->setViewport(gla);
	view->setScene(&scene);


	mdiarea->addSubWindow(mvcont);//=====add_qGraphicView=============

	mvcont->addWidget(view);

	connect(mvcont, SIGNAL(updateMainWindowMenus()), this, SLOT(updateMenus()));
	filterMenu->setEnabled(!filterMenu->actions().isEmpty());
	if (!filterMenu->actions().isEmpty())
		updateSubFiltersMenu(true, false);

	//mvcont->addView(gla, Qt::Horizontal);

	mvcont->setWindowTitle(mvcont->meshDoc.docLabel());
	//if(mdiarea->isVisible())


	mvcont->showMaximized();

	//***For one Viewer---------------------------------------------




	//***20150515設置建構槽大小
	MeshDocument *tempmdc = meshDoc();
	//tempmdc->groove.setLWH(15, 17, 19);

	//createSliceBar();
	alreadyload = true;
	return gla;
}

#endif
//WARNING!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//Temporary disgusting inequality between open (slot) - importMesh (function)
//and importRaster (slot). It's not also difficult to remove the problem because
//addNewRaster add a raster on a document and open the file, instead addNewMesh add a new mesh layer
//without loading the model.

bool MainWindow::importRaster(const QString& fileImg)
{
	//QStringList filters;
	//filters.push_back("Images (*.jpg *.png *.xpm)");
	//filters.push_back("*.jpg");
	//filters.push_back("*.png");
	//filters.push_back("*.xpm");

	//QStringList fileNameList;
	//if (fileImg.isEmpty())
	//	fileNameList = QFileDialog::getOpenFileNames(this, tr("Open File"), lastUsedDirectory.path(), filters.join(";;"));
	//else
	//	fileNameList.push_back(fileImg);

	//foreach(QString fileName, fileNameList)
	//{
	//	QFileInfo fi(fileName);
	//	if (fi.suffix().toLower() == "png" || fi.suffix().toLower() == "xpm" || fi.suffix().toLower() == "jpg")
	//	{
	//		qb->show();

	//		if (!fi.exists()) 	{
	//			QString errorMsgFormat = "Unable to open file:\n\"%1\"\n\nError details: file %1 does not exist.";
	//			QMessageBox::critical(this, tr("Meshlab Opening Error"), errorMsgFormat.arg(fileName));
	//			return false;
	//		}
	//		if (!fi.isReadable()) 	{
	//			QString errorMsgFormat = "Unable to open file:\n\"%1\"\n\nError details: file %1 is not readable.";
	//			QMessageBox::critical(this, tr("Meshlab Opening Error"), errorMsgFormat.arg(fileName));
	//			return false;
	//		}

	//		this->meshDoc()->setBusy(true);
	//		RasterModel *rm = meshDoc()->addNewRaster();
	//		rm->setLabel(fileImg);
	//		rm->addPlane(new Plane(fileName, Plane::RGBA));
	//		meshDoc()->setBusy(false);
	//		showLayerDlg(true);

	//		/// Intrinsics extraction from EXIF
	//		///	If no CCD Width value is provided, the intrinsics are extracted using the Equivalent 35mm focal
	//		/// If no or invalid EXIF info is found, the Intrinsics are initialized as a "plausible" 35mm sensor, with 50mm focal

	//		::ResetJpgfile();
	//		FILE * pFile = fopen(qPrintable(fileName), "rb");

	//		int ret = ::ReadJpegSections(pFile, READ_METADATA);
	//		fclose(pFile);
	//		if (!ret || (ImageInfo.CCDWidth == 0.0f && ImageInfo.FocalLength35mmEquiv == 0.0f))
	//		{
	//			rm->shot.Intrinsics.ViewportPx = vcg::Point2i(rm->currentPlane->image.width(), rm->currentPlane->image.height());
	//			rm->shot.Intrinsics.CenterPx = Point2m(float(rm->currentPlane->image.width() / 2.0), float(rm->currentPlane->image.width() / 2.0));
	//			rm->shot.Intrinsics.PixelSizeMm[0] = 36.0f / (float)rm->currentPlane->image.width();
	//			rm->shot.Intrinsics.PixelSizeMm[1] = rm->shot.Intrinsics.PixelSizeMm[0];
	//			rm->shot.Intrinsics.FocalMm = 50.0f;
	//		}
	//		else if (ImageInfo.CCDWidth != 0)
	//		{
	//			rm->shot.Intrinsics.ViewportPx = vcg::Point2i(ImageInfo.Width, ImageInfo.Height);
	//			rm->shot.Intrinsics.CenterPx = Point2m(float(ImageInfo.Width / 2.0), float(ImageInfo.Height / 2.0));
	//			float ratio;
	//			if (ImageInfo.Width > ImageInfo.Height)
	//				ratio = (float)ImageInfo.Width / (float)ImageInfo.Height;
	//			else
	//				ratio = (float)ImageInfo.Height / (float)ImageInfo.Width;
	//			rm->shot.Intrinsics.PixelSizeMm[0] = ImageInfo.CCDWidth / (float)ImageInfo.Width;
	//			rm->shot.Intrinsics.PixelSizeMm[1] = ImageInfo.CCDWidth / ((float)ImageInfo.Height*ratio);
	//			rm->shot.Intrinsics.FocalMm = ImageInfo.FocalLength;
	//		}
	//		else
	//		{
	//			rm->shot.Intrinsics.ViewportPx = vcg::Point2i(ImageInfo.Width, ImageInfo.Height);
	//			rm->shot.Intrinsics.CenterPx = Point2m(float(ImageInfo.Width / 2.0), float(ImageInfo.Height / 2.0));
	//			float ratioFocal = ImageInfo.FocalLength / ImageInfo.FocalLength35mmEquiv;
	//			rm->shot.Intrinsics.PixelSizeMm[0] = (36.0f*ratioFocal) / (float)ImageInfo.Width;
	//			rm->shot.Intrinsics.PixelSizeMm[1] = (24.0f*ratioFocal) / (float)ImageInfo.Height;
	//			rm->shot.Intrinsics.FocalMm = ImageInfo.FocalLength;
	//		}

	//		// End of EXIF reading

	//		//			if(mdiarea->isVisible()) GLA()->mvc->showMaximized();
	//		updateMenus();
	//	}
	//	else
	//		return false;
	//}
	return true;
}

bool MainWindow::loadMesh(const QString& fileName, MeshIOInterface *pCurrentIOPlugin, MeshModel* mm, int& mask, RichParameterSet* prePar)
{
	if ((GLA() == NULL) || (mm == NULL))
		return false;

	QMap<int, RenderMode>::iterator it = GLA()->rendermodemap.find(mm->id());
	if (it == GLA()->rendermodemap.end())
		return false;
	RenderMode& rm = it.value();
	QFileInfo fi(fileName);
	QString extension = fi.suffix();
	if (!fi.exists())
	{
		QString errorMsgFormat = "Open File Error:\n\"%1\"\n\nError details: file %1 does not exist.";
		QMessageBox::critical(this, tr("Opening Error"), errorMsgFormat.arg(fileName));
		return false;
	}
	else
	{
		//FILE *pFile = fopen(QFile::encodeName(fileName).constData(), "r");
		int sizee = fileName.size() + 1;
		wchar_t *jpSentence = new wchar_t[sizee];
		//jpSentence->
		int y = fileName.toWCharArray(jpSentence);
		jpSentence[fileName.size()] = 0;
		//int x = _wopen(jpSentence, 0 ,L"w, ccs=UTF-8");
		FILE * pFile = _tfopen(jpSentence, _T("rt"));
		if (pFile == 0)
			//if (x == 0)
		{
			QTextCodec *codec = QTextCodec::codecForLocale();
			QByteArray b_array = codec->fromUnicode(fileName);
			QString fromlocale(b_array);

			QMessageBox::warning(this, tr("Open File Error"), QString("can't open file %1\n").arg(fromlocale)); // text+

			meshDoc()->setBusy(false);

			return false;
		}
		delete[]jpSentence;
		fclose(pFile);
		//_close(x);

	}

	if (!fi.isReadable())
	{
		QString errorMsgFormat = "Open File Error:\n\"%1\"\n\nError details: file %1 is not readable.";
		QMessageBox::critical(this, tr("Opening Error"), errorMsgFormat.arg(fileName));
		return false;
	}

	// this change of dir is needed for subsequent textures/materials loading
	QDir::setCurrent(fi.absoluteDir().absolutePath());

	// retrieving corresponding IO plugin
	if (pCurrentIOPlugin == 0)
	{
		QString errorMsgFormat = "Open File Error:\n\"%1\"\n\nError details: The \"%2\" file extension does not correspond to any supported format.";
		QMessageBox::critical(this, tr("Opening Error"), errorMsgFormat.arg(fileName, extension));
		return false;
	}
	meshDoc()->setBusy(true);
	pCurrentIOPlugin->setLog(&meshDoc()->Log);


	/*for (int i = 0; i < fileName.length(); i++)
	{
	bool isUnicode = fileName.at(i).unicode()>127;
	if (isUnicode)
	{
	QMessageBox::information(NULL, tr("File Name error"), tr("File Name contain unicode"));
	return false;
	}
	}*/



	//******************************ffffffffffffffffffffffffffffffffff*************************************************************************


	if (!pCurrentIOPlugin->open(extension, fileName, *mm, mask, *prePar, QCallBack, this /*gla*/))
	{
		//QMessageBox::warning(this, tr("Opening Failure"), QString("While opening: '%1'\n\n").arg(fileName) + pCurrentIOPlugin->errorMsg()); // text+
		pCurrentIOPlugin->clearErrorString();
		meshDoc()->setBusy(false);
		return false;
	}

	// After opening the mesh lets ask to the io plugin if this format
	// requires some optional, or userdriven post-opening processing.
	// and in that case ask for the required parameters and then
	// ask to the plugin to perform that processing
	//RichParameterSet par;
	//pCurrentIOPlugin->initOpenParameter(extension, *mm, par);
	//pCurrentIOPlugin->applyOpenParameter(extension, *mm, par);

	QString err = pCurrentIOPlugin->errorMsg();
	if (!err.isEmpty())
	{
		//QMessageBox::warning(this, tr("Opening Problems"), QString("While opening: '%1'\n\n").arg(fileName) + pCurrentIOPlugin->errorMsg());
		pCurrentIOPlugin->clearErrorString();
	}


	saveRecentFileList(fileName);
	//***20151005***點太多，用點顯示******
	if (!mm->cm.IsEmpty() && mm->cm.VertexNumber() > vtNumberLimit)
	{
		//rm.setDrawMode(GLW::DMPoints);
	}
	//***20151007***測試物體是否還會飛走
	mm->cm.Tr.SetIdentity();
	//*****
	/*if ((mask & vcg::tri::io::Mask::IOM_FACECOLOR) && (mask & vcg::tri::io::Mask::IOM_VERTCOLOR))
	{
	mm->rmm.setColorMode(GLW::CMPerVert);
	}else*/
	if (!mm->cm.textures.empty())
	{
		mm->rmm.setColorMode(GLW::CMNone);
	}
	else if (mask & vcg::tri::io::Mask::IOM_FACECOLOR)
	{
		//GLA()->setColorMode(GLW::CMPerFace);
		//***20160308		
		/*MultiViewer_Container *mvc = qobject_cast<MultiViewer_Container *>(mdiarea->currentSubWindow()->widget());
		GLArea* tempGLArea = NULL;
		for (int i = 0; i < 3; i++)
		{
		tempGLArea = mvc->getViewer(i);
		tempGLArea->setColorMode(GLW::ColorMode::CMPerFace);
		}*/
		mm->rmm.setColorMode(GLW::CMPerFace);
	}
	else if (mask & vcg::tri::io::Mask::IOM_VERTCOLOR)//***20160301
	{
		//***20150408 增加同步顏色
		//GLA()->setColorMode(GLW::CMPerVert);
		/*MultiViewer_Container *mvc = qobject_cast<MultiViewer_Container *>(mdiarea->currentSubWindow()->widget());
		GLArea* tempGLArea = NULL;
		for (int i = 0; i < 3; i++)
		{
		tempGLArea = mvc->getViewer(i);
		tempGLArea->setColorMode(GLW::ColorMode::CMPerVert);
		}*/
		//***20151205
		//mm->rmm.colorMode = GLW::CMPerVert;
		mm->rmm.setColorMode(GLW::CMPerVert);
	}


	renderModeTextureWedgeAct->setChecked(false);
	//renderModeTextureWedgeAct->setEnabled(false);


	//if (!meshDoc()->mm()->cm.textures.empty())
	if (!mm->cm.textures.empty())
	{
		renderModeTextureWedgeAct->setChecked(true);
		//renderModeTextureWedgeAct->setEnabled(true);
		if (tri::HasPerVertexTexCoord(mm->cm))
		{
			GLA()->setTextureMode(rm, GLW::TMPerVert);
			mm->rmm.setTextureMode(GLW::TMPerVert);
			//mm->rmm.setColorMode(GLW::CMNone);
		}
		if (tri::HasPerWedgeTexCoord(mm->cm))
		{
			GLA()->setTextureMode(rm, GLW::TMPerWedgeMulti);
			mm->rmm.setTextureMode(GLW::TMPerWedgeMulti);
			//mm->rmm.setColorMode(GLW::CMNone);
		}
		//********20150402增加同步Texture
		//GLA()->updateTexture();		
		GLArea* tempGLArea = NULL;
		for (int i = 0; i < 1; i++)
			//for (int i = 0; i < 3; i++)
		{
			tempGLArea = currentViewContainer()->getViewer(i);
			//qDebug("viewer=%i", (i % 3));
			tempGLArea->updateTexture();
			//currentViewContainer()->updateAllViewer();
			tempGLArea->setTextureMode(GLW::TextureMode::TMPerWedgeMulti);//20160329要註解掉
			//tempGLArea->setColorMode(rm, GLW::CMNone);
		}

	}


	// In case of polygonal meshes the normal should be updated accordingly
	if (mask & vcg::tri::io::Mask::IOM_BITPOLYGONAL)
	{
		mm->updateDataMask(MeshModel::MM_POLYGONAL); // just to be sure. Hopefully it should be done in the plugin...
		int degNum = tri::Clean<CMeshO>::RemoveDegenerateFace(mm->cm);
		if (degNum)
			GLA()->Logf(0, "Warning model contains %i degenerate faces. Removed them.", degNum);
		mm->updateDataMask(MeshModel::MM_FACEFACETOPO);
		vcg::tri::UpdateNormal<CMeshO>::PerBitQuadFaceNormalized(mm->cm);
		vcg::tri::UpdateNormal<CMeshO>::PerVertexFromCurrentFaceNormal(mm->cm);
	} // standard case
	else
	{
		vcg::tri::UpdateNormal<CMeshO>::PerFaceNormalized(mm->cm);
		if (!(mask & vcg::tri::io::Mask::IOM_VERTNORMAL))
			vcg::tri::UpdateNormal<CMeshO>::PerVertexAngleWeighted(mm->cm);
	}
	vcg::tri::UpdateBounding<CMeshO>::Box(mm->cm);					// updates bounding box
	if (mm->cm.fn == 0 && mm->cm.en == 0) {
		GLA()->setDrawMode(rm, vcg::GLW::DMPoints);
		if (!(mask & vcg::tri::io::Mask::IOM_VERTNORMAL))
			GLA()->setLight(false);
		else
			mm->updateDataMask(MeshModel::MM_VERTNORMAL);
	}
	if (mm->cm.fn == 0 && mm->cm.en > 0) {
		GLA()->setDrawMode(rm, vcg::GLW::DMWire);
		if (!(mask & vcg::tri::io::Mask::IOM_VERTNORMAL))
			GLA()->setLight(false);
		else
			mm->updateDataMask(MeshModel::MM_VERTNORMAL);
	}
	else
		mm->updateDataMask(MeshModel::MM_VERTNORMAL);

	//updateMenus();

	if (!mm->hasDataMask(MeshModel::MM_VERTCOLOR) || !!mm->hasDataMask(MeshModel::MM_FACECOLOR))
		tri::Clean<CMeshO>::RemoveDuplicateVertex(mm->cm);//***some mesh color will disappear because removing vertex

	int delVertNum = vcg::tri::Clean<CMeshO>::RemoveDegenerateVertex(mm->cm);
	int delFaceNum = vcg::tri::Clean<CMeshO>::RemoveDegenerateFace(mm->cm);

	/*if (delVertNum > 0 || delFaceNum > 0)
		QMessageBox::warning(this, "MeshLab Warning", QString("Warning mesh contains %1 vertices with NAN coords and %2 degenerated faces.\nCorrected.").arg(delVertNum).arg(delFaceNum));*/
	//meshDoc()->setBusy(false);

	qDebug() << "filename: " << fileName;

	//***20150512 close filter dialog
	if (stddialog != NULL)stddialog->setVisible(false);
#if 0
	//***20150828
	if (meshCheckSwitch)
	{
		MeshDocument *md = meshDoc();
		MultiViewer_Container* testmdi = currentViewContainer();
		//mm()->
		MeshCheck meshcheckdialog(&testmdi->meshDoc, currentGlobalParams, mm, md->groove, this);//***openMeshCheckDialog
		meshcheckdialog.exec();
		if (meshcheckdialog.result())
		{
			//20151006**偵測有無超出groove
			meshDoc()->groove.setGrooveHit(SKT ::detect_hit_groove<Scalarm>(*meshDoc()));
			//***********************
			return true;
		}
		else return false;
	}
#else

	//test vbo
	//if (/*mm->cm.textures.size() < 2 &&*/ mm->rmm.colorMode != vcg::GLW::ColorMode::CMPerFace)
	{
		mm->glw.SetHint(vcg::GLW::Hint::HNUseVBO);
		GLA()->setHastoRefreshVBO();
	}
	//mm->glw.Update();
	//mm->rmm.setColorMode(vcg::GLW::CMNone);

	RichParameterSet tt;
	executeFilter(PM.actionFilterMap.value("count_hole"), tt, false);
	int x = tt.getInt("holeNum");
	meshDoc()->setBusy(true);

	if (tt.getInt("holeNum") > 0)
	{
		QMessageBox mb(QMessageBox::NoIcon, "Mesh contain Hole!", "Mesh contain Hole! \nPrint may not successful !\nStill want to load? ", QMessageBox::NoButton, this);
		QPushButton *ok = mb.addButton("OK", QMessageBox::AcceptRole);
		mb.addButton(QMessageBox::Cancel);
		//mb.setInformativeText(tr( "Press button to retry connect printer\n") );
		mb.exec();

		if (mb.clickedButton() == ok)
		{

		}
		else return 0;
	}


	if (meshCheckSwitch)
	{
		bool stopp = true;
		while (stopp)
		{
			MeshDocument *md = meshDoc();

			float largest = mm->cm.bbox.DimX();
			if (mm->cm.bbox.DimY() >= largest)
				largest = mm->cm.bbox.DimY();
			if (mm->cm.bbox.DimZ() >= largest)
				largest = mm->cm.bbox.DimZ();
			bool tooSmall = true;

			QMessageBox sizeCheck;
			QPushButton *doNotImport = sizeCheck.addButton(tr("Do Not Import"), QMessageBox::ActionRole);
			QPushButton *importAnyway = sizeCheck.addButton(tr("Import Anyway"), QMessageBox::ActionRole);
			QPushButton *changeUnit = sizeCheck.addButton(tr("Change Unit"), QMessageBox::ActionRole);
			QPushButton *autoResize = sizeCheck.addButton(tr("Auto Resize"), QMessageBox::ActionRole);
			sizeCheck.setDefaultButton(autoResize);

			if (largest < 10)
			{
				sizeCheck.setWindowTitle("Object is too small");
				sizeCheck.setText("The object you are importing is tiny");
				sizeCheck.setInformativeText(QString("Current size: %1 x %2 x %3 mm^3")
					.arg(mm->cm.bbox.DimX()).arg(mm->cm.bbox.DimY()).arg(mm->cm.bbox.DimZ()));
				tooSmall = true;
				sizeCheck.exec();
			}
			else if (mm->cm.bbox.DimX() > DSP_grooveBigX || mm->cm.bbox.DimY() > DSP_grooveY || mm->cm.bbox.DimZ() > DSP_grooveZ)
			{
				sizeCheck.setWindowTitle("Object is too large");
				sizeCheck.setText("The object you are importing doesn't fit within the print boundaries");
				sizeCheck.setInformativeText(QString("Current size: %1 x %2 x %3 mm^3")
					.arg(mm->cm.bbox.DimX()).arg(mm->cm.bbox.DimY()).arg(mm->cm.bbox.DimZ()));
				tooSmall = false;
				sizeCheck.exec();
			}
			else
			{
				ScaleN = Point3f(1.0, 1.0, 1.0);
				meshDoc()->scaleMesh(mm, ScaleN);

				if (isReset == false)
					currentViewContainer()->undoStack->push(new import_command(this, fileName, ScaleN, mm->id(), false));
				else
					resetScaleN.insert(mm->id(), ScaleN);

				return true;
			}

			if (sizeCheck.clickedButton() == autoResize)
			{
				if (tooSmall)
				{
					float resize = 50.0 / largest;
					ScaleN = Point3f(resize, resize, resize);
					meshDoc()->scaleMesh(mm, ScaleN);
				}
				else
				{
					do
					{
						qDebug() << mm->cm.bbox.DimX();
						qDebug() << mm->cm.bbox.DimY();
						if (largest == mm->cm.bbox.DimX())
						{
							float resize = (DSP_grooveBigX - 10) / largest;
							ScaleN = Point3f(resize, resize, resize);
							meshDoc()->scaleMesh(mm, ScaleN);
						}
						else if (largest == mm->cm.bbox.DimY())
						{
							float resize = (DSP_grooveY - 10) / largest;
							ScaleN = Point3f(resize, resize, resize);
							meshDoc()->scaleMesh(mm, ScaleN);
						}
						else if (largest == mm->cm.bbox.DimZ())
						{
							float resize = (DSP_grooveZ - 10) / largest;
							ScaleN = Point3f(resize, resize, resize);
							meshDoc()->scaleMesh(mm, ScaleN);
						}

					} while (mm->cm.bbox.DimZ() > DSP_grooveZ || mm->cm.bbox.DimX() > DSP_grooveX || mm->cm.bbox.DimY() > DSP_grooveY);
				}

				if (isReset == false)
					currentViewContainer()->undoStack->push(new import_command(this, fileName, ScaleN, mm->id(), false));
				else
					resetScaleN.insert(mm->id(), ScaleN);

				return true;
			}
			else if (sizeCheck.clickedButton() == changeUnit)
			{
				meshCheck3 meshcheckdialog(mm, md->groove, GLA(), this);//***openMeshCheckDialog
				meshcheckdialog.exec();
				if (meshcheckdialog.result())
				{
					meshDoc()->groove.setGrooveHit(SKT::detect_hit_groove<Scalarm>(*meshDoc()));
					ScaleN = meshcheckdialog.scaleN;

					if (isReset == false)
						currentViewContainer()->undoStack->push(new import_command(this, fileName, ScaleN, mm->id(), false));
					else
						resetScaleN.insert(mm->id(), ScaleN);

					return true;
				}

				//SYDNY
				else if (meshcheckdialog.close())
				{
					/*md->delMesh(mm);
					firstImport = false;
					importMesh(fileName);
					return true;*/
				}

				//else return false;
			}
			else if (sizeCheck.clickedButton() == importAnyway)
			{
				sizeCheck.close();
				ScaleN = Point3f(1.0, 1.0, 1.0);
				meshDoc()->scaleMesh(mm, ScaleN);

				if (isReset == false)
					currentViewContainer()->undoStack->push(new import_command(this, fileName, ScaleN, mm->id(), false));
				else
					resetScaleN.insert(mm->id(), ScaleN);

				return true;
			}
			else if (sizeCheck.clickedButton() == doNotImport)
			{
				sizeCheck.close();
				return false;
			}
			else
				return false;

		}

	}
	else
	{
		//*** 20170207 --- added redoOccured varialbe for import mesh undo/redo function.
		// added by KPPH R&D-I-SW, Mark
		if (redoOccured == true)
		{
			meshDoc()->scaleMesh(mm, ScaleN);
			meshDoc()->groove.setGrooveHit(SKT::detect_hit_groove<Scalarm>(*meshDoc()));
			redoOccured = false;
			return true;
		}
	}
#endif
	MeshModel *mmtest = meshDoc()->mm();

	return true;
}

bool MainWindow::importFromProject(QString fileName)
{
	const QString DEFAULT_DIR_KEY("default_p_dir");
	QSettings MySettings;

	if (fileName.isEmpty())
		fileName = QFileDialog::getOpenFileName(this, tr("Open Project File"), MySettings.value(DEFAULT_DIR_KEY).toString(), "3mf Project (*.3mf);");

	if (fileName.isEmpty())
		return false;
	else
	{
		QDir CurrentDir;
		MySettings.setValue(DEFAULT_DIR_KEY, CurrentDir.absoluteFilePath(fileName));
	}

	QFileInfo fi(fileName);

	if ((fi.suffix().toLower() != "3mf"))
	{
		QMessageBox::critical(this, tr("BCPware Opening Error"), "Unknown project file extension");
		return false;
	}

	meshDoc()->setBusy(true);
	qb->show();

	if (QString(fi.suffix()).toLower() == "3mf")
	{
		LPCWSTR pn = (const wchar_t*)fileName.utf16();
		ThreeMF_Manip *tmf = new ThreeMF_Manip();
		bool ret = tmf->open_3mf(this, meshDoc(), pn, QCallBack, true);


		if (ret == false)
		{
			QMessageBox::critical(this, tr("Palette Opening Error"), "Unable to open 3mf file");
			meshDoc()->setBusy(false);
			return false;
		}
		else
		{
			emit dispatchCustomSettings(currentGlobalParams);

			for (int i = 0; i < meshDoc()->meshList.size(); i++)
			{
				if (meshDoc()->meshList[i]->hasDataMask(MeshModel::MM_WEDGTEXCOORD))
				{
					GLA()->updateTexture();
				}

				//tri::UpdatePosition<CMeshO>::Matrix(this->meshDoc()->meshList[i]->cm, this->meshDoc()->meshList[i]->cm.Tr, false);
				tri::UpdateBounding<CMeshO>::Box(this->meshDoc()->meshList[i]->cm);
				tri::UpdateNormal<CMeshO>::PerFaceNormalized(this->meshDoc()->meshList[i]->cm);
				GLA()->updateRendermodemapSiganl = true;
			}

			meshDoc()->setBusy(false);
			if (this->GLA() == 0)  return false;
			this->currentViewContainer()->resetAllTrackBall();
			saveRecentProjectList(fileName);
			alreadyload = true;
			GLA()->reFreshGlListlist();
			GLA()->setHastoRefreshVBO();
			updateMenus();
		}

	}

	/*for (int i = 0; i < meshDoc()->meshList.size(); i++)
	{
	if (meshDoc()->meshList[i]->hasDataMask(MeshModel::MM_WEDGTEXCOORD))
	{
	GLA()->updateTexture();
	}

	tri::UpdateBounding<CMeshO>::Box(this->meshDoc()->meshList[i]->cm);
	tri::UpdateNormal<CMeshO>::PerFaceNormalized(this->meshDoc()->meshList[i]->cm);
	GLA()->updateRendermodemapSiganl = true;
	}*/

	meshDoc()->setBusy(false);
	//this->currentViewContainer()->resetAllTrackBall();
	qb->reset();
	return true;
}

bool MainWindow::importMeshWithLayerManagement(QString fileName)
{
	qDebug() << "qPrintable(fileName)" << fileName.toUtf8().data();
	meshCheckSwitch = true;

	bool layervisible = false;
	if (layerDialog != NULL)
	{
		layervisible = layerDialog->isVisible();
		showLayerDlg(false);
	}
	bool res = importMesh(fileName);
	if (layerDialog != NULL)
		//***20150818將layerDialog關掉
		//showLayerDlg(layervisible || meshDoc()->meshList.size() > 1);
		showLayerDlg(false);

	return res;
}

// Opening files in a transparent form (IO plugins contribution is hidden to user)
bool MainWindow::importMesh(QString fileName)
{
	if (!GLA())
	{
		this->newProject();
		if (!GLA())
			return false;
	}

	/*QString number = QString("%1").arg(13, 5, 10, QChar('0'));
	qDebug() << "qPrintable(fileNaddddddddddddme)1" << number;*/
	qDebug() << "qPrintable(fileName)1" << fileName.toUtf8().data();
	fileName = fileName.toUtf8().data();
	qDebug() << "qPrintable(fileName)5" << fileName;

	//QStringList suffixList;
	// HashTable storing all supported formats together with
	// the (1-based) index  of first plugin which is able to open it
	//QHash<QString, MeshIOInterface*> allKnownFormats;
	//PM.LoadFormats(suffixList, allKnownFormats,PluginManager::IMPORT);

	///***20151513example
	const QString DEFAULT_DIR_KEY("default_dir");

	QSettings MySettings; // Will be using application informations for correct location of your settings

	//QString SelectedFile = QFileDialog::getOpenFileName(this, "Select a file", MySettings.value(DEFAULT_DIR_KEY).toString());
	//if (!SelectedFile.isEmpty()) {
	//	QDir CurrentDir;
	//	MySettings.setValue(DEFAULT_DIR_KEY, CurrentDir.absoluteFilePath(SelectedFile));
	//	QMessageBox::information(this, "Info", "You selected the file '" + SelectedFile + "'");
	//}
	//*****

	meshDoc()->setBusy(true);

	QStringList fileNameList;
	if (fileName.isEmpty())
		//fileNameList = QFileDialog::getOpenFileNames(this, tr("Import Mesh"), lastUsedDirectory.path(), PM.inpFilters.join(";;"));
		fileNameList = QFileDialog::getOpenFileNames(this, tr("Import Model"), MySettings.value(DEFAULT_DIR_KEY).toString(), PM.inpFilters.join(";;"));

	else
		fileNameList.push_back(fileName);


	if (fileNameList.isEmpty()){
		meshDoc()->setBusy(false);
		return false;
	}
	else
	{
		//save path away so we can use it again
		QString path = fileNameList.first();
		path.truncate(path.lastIndexOf("/"));
		lastUsedDirectory.setPath(path);

		//***20151013***紀錄所選位置資料夾
		QDir CurrentDir;
		MySettings.setValue(DEFAULT_DIR_KEY, CurrentDir.absoluteFilePath(fileNameList.first()));
		//QMessageBox::information(this, "Info", "You selected the file '" + fileNameList.first() + "'");

	}

	QTime allFileTime;
	allFileTime.start();
	foreach(fileName, fileNameList)
	{
		QFileInfo fi(fileName);
		QString extension = fi.suffix();
		MeshIOInterface *pCurrentIOPlugin = PM.allKnowInputFormats[extension.toLower()];
		//pCurrentIOPlugin->setLog(gla->log);
		if (pCurrentIOPlugin == NULL)
		{
			QString errorMsgFormat("Unable to open file:\n\"%1\"\n\nError details: file format " + extension + " not supported.");
			QMessageBox::critical(this, tr("Meshlab Opening Error"), errorMsgFormat.arg(fileName));
			return false;
		}

		RichParameterSet prePar;
		pCurrentIOPlugin->initPreOpenParameter(extension, fileName, prePar);
		if (!prePar.isEmpty())
		{
			GenericParamDialog preOpenDialog(this, &prePar, tr("Pre-Open Options"));
			preOpenDialog.setFocus();
			preOpenDialog.exec();
		}
		int mask = 0;
		//MeshModel *mm= new MeshModel(gla->meshDoc);
		//***20150921
		QFileInfo info(fileName);
		//MeshModel *mm = meshDoc()->addNewMesh(qPrintable(fileName), info.fileName());
		qDebug() << "qPrintable(fileName)6" << fileName;
		MeshModel *mm = meshDoc()->addNewMesh(fileName, info.fileName());//*****************************************************
		qDebug() << "qPrintable(fileName)4" << meshDoc()->mm()->fullName();

		qb->show();
		QTime t; t.start();
		fileName = SKT::convertToVrml2FileName(fileName);
		bool open = loadMesh(fileName, pCurrentIOPlugin, mm, mask, &prePar);//*****************************************************************

		if (open)
		{
			//SYDNY 09/25/2017------------------------
			if (meshDoc()->multiSelectID.count() == 0)
			{
				meshDoc()->multiSelectID.insert(mm->id());
				updateMenus();
			}

			else if (meshDoc() != NULL && meshDoc()->multiSelectID.count() != 0)
			{
				if (!isReset)
				{
					meshDoc()->multiSelectID.clear();
					meshDoc()->multiSelectID.insert(mm->id());
					updateMenus();
				}
				else
				{
					meshDoc()->multiSelectID.insert(mm->id());
					updateMenus();
				}
			}

			//***20150918***不知為什麼會當掉
			//註解掉***GLA()->Logf(0, "Opened mesh %s in %i msec", qPrintable(fileName), t.elapsed());
			//RichParameterSet par;
			//pCurrentIOPlugin->initOpenParameter(extension, *mm, par);
			//if (!par.isEmpty())
			//{
			//	GenericParamDialog postOpenDialog(this, &par, tr("Post-Open Processing"));
			//	postOpenDialog.setFocus();
			//	postOpenDialog.exec();
			//	pCurrentIOPlugin->applyOpenParameter(extension, *mm, par);
			//}
			//***20150520 輸入完成時自動排列***	
			/*	QMap<QString, MeshFilterInterface *>::iterator msi;
			RichParameterSet dummyParSet;
			executeFilter(PM.actionFilterMap.value("AutoPacking"), dummyParSet, false);*/
			if (firstImport == true)
			{
				//toCenter();
				RichParameterSet dummyParSet;
				executeFilter(PM.actionFilterMap.value("FP_TEST_SEARCH_SPACE_ALL_IMAGE2"), dummyParSet, false);

				MeshModel *forResetMesh = new MeshModel(meshDoc(), mm->fullName(), "");
				forResetMesh->updateDataMask(mm->dataMask());
				vcg::tri::Append<CMeshO, CMeshO >::MeshCopy(forResetMesh->cm, mm->cm); //Copy the second mesh over the first one.
				forResetMesh->cm.Tr = mm->cm.Tr;
				forResetMesh->rmm = mm->rmm;
				forResetMesh->glw.curr_hints = mm->glw.curr_hints;
				forResetMesh->is3mf = false;

				meshDoc()->resetMeshList.insert(mm->id(), forResetMesh);
				////**********
				//***20160307

				if (meshDoc() != NULL && alreadyload == true)
				{
					alreadyload = false;
					//createSliceBar();//backup*20160608**********************************畫slicebar 
				}

			}
			else firstImport = true;

		}
		else
		{
			meshDoc()->delMesh(mm);
			objList->updateTable();
			updateMenus();
			GLA()->Logf(0, "Warning: Mesh %s has not been opened", qPrintable(fileName));
		}
	}
	meshDoc()->setBusy(false);// end foreach file of the input list
	//GLA()->Logf(0, "All files opened in %i msec", allFileTime.elapsed());

	//this->currentViewContainer()->resetAllTrackBall();

	if (getFCNumberLimit() < meshDoc()->fn() && fastRenderSwitch)
		setMdNumLimit(true);
	else
		setMdNumLimit(false);
	//***

	qb->reset();
	/*if (!getGLlistSwitch())
	setGLlistSwitch(true);*/
	GLA()->reFreshGlListlist();
	//emit updateMeshChanged();
	updateMenus();
	return true;
}

void MainWindow::openRecentMesh()
{
	if (!GLA()) return;
	if (meshDoc()->isBusy()) return;
	QAction *action = qobject_cast<QAction *>(sender());
	//***20150921
	meshCheckSwitch = true;
	if (action)
	{
		importMeshWithLayerManagement(action->data().toString());

	}


}

void MainWindow::openRecentProj()
{
	QAction *action = qobject_cast<QAction *>(sender());
	if (action)	openProject2(action->data().toString());
}

bool MainWindow::loadMeshWithStandardParams(QString& fullPath, MeshModel* mm)
{


	const char* myChar = fullPath.toStdString().c_str();
	/*fprintf(dbgff, "b_name %s\n", myChar);
	fflush(dbgff);*/

	if ((meshDoc() == NULL) || (mm == NULL))
		return false;
	bool ret = false;
	MeshModel *copy = new MeshModel(meshDoc(), mm->fullName(), "");
	vcg::tri::Append<CMeshO, CMeshO>::MeshCopy(copy->cm, mm->cm);
	copy->cm.Tr = mm->cm.Tr;
	copy->rmm = mm->rmm;
	copy->glw.curr_hints = mm->glw.curr_hints;
	mm->Clear();
	QFileInfo fi(fullPath);
	QString extension = fi.suffix();
	MeshIOInterface *pCurrentIOPlugin = PM.allKnowInputFormats[extension.toLower()];
	//fprintf(dbgff, "b-1\n"); //@@@
	//fflush(dbgff);
	if (pCurrentIOPlugin != NULL)
	{
		RichParameterSet prePar;
		pCurrentIOPlugin->initPreOpenParameter(extension, fullPath, prePar);
		int mask = 0;
		QTime t; t.start();
		bool open = loadMesh(fullPath, pCurrentIOPlugin, mm, mask, &prePar);//***********ffffffffuck******************************************************************
		if (open)
		{
			//會當掉20150921註解掉***GLA()->Logf(0, "Opened mesh %s in %i msec", qPrintable(fullPath), t.elapsed());
			RichParameterSet par;
			pCurrentIOPlugin->initOpenParameter(extension, *mm, par);
			pCurrentIOPlugin->applyOpenParameter(extension, *mm, par);
			ret = true;
			RichParameterSet dummyParSet;
			//***20160329
			meshDoc()->setCurrentMesh(mm->id());
			//executeFilter(PM.actionFilterMap.value("MoveToCenter"), dummyParSet, false);

		}
		else
		{
			//GLA()->Logf(0, "Warning: Mesh %s has not been opened", qPrintable(fullPath));
			mm->cm.Clear();
			vcg::tri::Append<CMeshO, CMeshO>::MeshCopy(mm->cm, copy->cm);
			mm->cm.Tr = copy->cm.Tr;
			mm->rmm = copy->rmm;
			mm->glw.curr_hints = copy->glw.curr_hints;
		}
	}
	else;
	//GLA()->Logf(0, "Warning: Mesh %s cannot be opened. Your  version has not plugin to read %s file format", qPrintable(fullPath), qPrintable(extension));
	//fprintf(dbgff, "b-ret_%i\n", ret); //@@@
	//fflush(dbgff);
	return ret;
}

void MainWindow::reloadAllMesh()
{
	//***20150921
	meshCheckSwitch = false;
	//***
	// Discards changes and reloads current file
	// save current file name
	qb->show();
	foreach(MeshModel *mmm, meshDoc()->meshList)
	{
		QString fileName = mmm->fullName();
		loadMeshWithStandardParams(fileName, mmm);
	}
	qb->reset();
	update();
}

void MainWindow::reload()
{
	if (meshDoc() == NULL) return;

	//***20150921
	meshCheckSwitch = true;
	isReset = true;
	//resetNameList.clear();
	resetScaleN.clear();
	resetListID.clear();

	delMeshes();

	foreach(int i, resetListID)
	{
		MeshModel *rm = new MeshModel(meshDoc(), meshDoc()->resetMeshList.value(i)->fullName(), meshDoc()->resetMeshList.value(i)->label());
		rm->updateDataMask(meshDoc()->resetMeshList.value(i)->dataMask());
		vcg::tri::Append<CMeshO, CMeshO >::MeshCopy(rm->cm, meshDoc()->resetMeshList.value(i)->cm);
		rm->cm.Tr = meshDoc()->resetMeshList.value(i)->cm.Tr;
		rm->rmm = meshDoc()->resetMeshList.value(i)->rmm;
		rm->is3mf = meshDoc()->resetMeshList.value(i)->is3mf;

		if (rm->is3mf == false)
		{
			rm = meshDoc()->resetMeshList.value(i);
			importMesh(rm->fullName());
			meshDoc()->multiSelectID.insert(meshDoc()->mm()->id());
			meshDoc()->resetMeshList.remove(i);
		}
		else
		{
			rm->setid(i);
			rm->is3mf = true;
			{
				rm->glw.SetHint(vcg::GLW::Hint::HNUseVBO);

			}
			meshDoc()->meshList.push_back(rm);
			meshDoc()->multiSelectID.insert(rm->id());
			meshDoc()->setCurrentMesh(rm->id());
			emit meshDoc()->meshSetChanged();
			emit meshDoc()->meshAdded(rm->id(), rm->rmm);
			tri::UpdateBounding<CMeshO>::Box(rm->cm);
			tri::UpdateNormal<CMeshO>::PerFaceNormalized(rm->cm);

			renderModeTextureWedgeAct->setChecked(false);

			if (!rm->cm.textures.empty())
			{
				renderModeTextureWedgeAct->setChecked(true);
				if (tri::HasPerVertexTexCoord(rm->cm))
				{
					GLA()->setTextureMode(rm->rmm, GLW::TMPerVert);
					rm->rmm.setTextureMode(GLW::TMPerVert);
					rm->rmm.setColorMode(GLW::CMNone);
				}
				if (tri::HasPerWedgeTexCoord(rm->cm))
				{
					GLA()->setTextureMode(rm->rmm, GLW::TMPerWedgeMulti);
					rm->rmm.setTextureMode(GLW::TMPerWedgeMulti);
				}

				GLArea* tempGLArea = NULL;
				for (int i = 0; i < 1; i++)
				{
					tempGLArea = currentViewContainer()->getViewer(i);
					tempGLArea->updateTexture();
					tempGLArea->setTextureMode(GLW::TextureMode::TMPerWedgeMulti);
				}
			}
		}
	}
	GLA()->setHastoRefreshVBO();

	currentViewContainer()->undoStack->push(new reset_command(this, meshDoc()->deletedMeshList, meshDoc()->deletedMeshId, resetScaleN, false));
	isReset = false;
	objList->updateTable();
	updateMenus();
}

bool MainWindow::exportMesh2(QString fileName, MeshModel* mod, const bool saveAllPossibleAttributes)
{
	QStringList& suffixList = PM.outFilters;

	//QHash<QString, MeshIOInterface*> allKnownFormats;
	QFileInfo fi(fileName);
	//PM.LoadFormats( suffixList, allKnownFormats,PluginManager::EXPORT);
	//QString defaultExt = "*." + mod->suffixName().toLower();
	QString defaultExt = "*." + fi.suffix().toLower();
	/*if (defaultExt == "*.")
		defaultExt = "*.ply";*/
	if (mod == NULL)
		return false;
	mod->meshModified() = false;
	QString laylabel = "Save \"" + mod->label() + "\" Layer";
	QString ss = fi.absoluteFilePath();
	QFileDialog* saveDialog = new QFileDialog(this, laylabel, fi.absolutePath());

#if defined(Q_OS_WIN)
	saveDialog->setOption(QFileDialog::DontUseNativeDialog);
#endif
	saveDialog->setNameFilters(suffixList);
	saveDialog->setAcceptMode(QFileDialog::AcceptSave);
	saveDialog->setFileMode(QFileDialog::AnyFile);
	saveDialog->selectFile(fileName);
	QStringList matchingExtensions = suffixList.filter(defaultExt);
	if (!matchingExtensions.isEmpty())
		saveDialog->selectNameFilter(matchingExtensions.last());
	connect(saveDialog, SIGNAL(filterSelected(const QString&)), this, SLOT(changeFileExtension(const QString&)));

	if (fileName.isEmpty()) {
		saveDialog->selectFile(meshDoc()->mm()->fullName());
		int dialogRet = saveDialog->exec();
		if (dialogRet == QDialog::Rejected)
			return false;
		fileName = saveDialog->selectedFiles().first();
		QFileInfo fni(fileName);
		if (fni.suffix().isEmpty())
		{
			QString ext = saveDialog->selectedNameFilter();
			ext.chop(1); ext = ext.right(4);
			fileName = fileName + ext;
			qDebug("File without extension adding it by hand '%s'", qPrintable(fileName));
		}
	}


	bool ret = false;

	QStringList fs = fileName.split(".");

	if (!fileName.isEmpty() && fs.size() < 2)
	{
		QMessageBox::warning(new QWidget(), "Save Error", "You must specify file extension!!");
		return ret;
	}

	if (!fileName.isEmpty())
	{
		//save path away so we can use it again
		QString path = fileName;
		path.truncate(path.lastIndexOf("/"));
		lastUsedDirectory.setPath(path);

		QString extension = fileName;
		extension.remove(0, fileName.lastIndexOf('.') + 1);

		QStringListIterator itFilter(suffixList);

		MeshIOInterface *pCurrentIOPlugin = PM.allKnowOutputFormats[extension.toLower()];
		if (pCurrentIOPlugin == 0)
		{
			QMessageBox::warning(this, "Unknown type", "File extension not supported!");
			return false;
		}
		//MeshIOInterface* pCurrentIOPlugin = meshIOPlugins[idx-1];
		pCurrentIOPlugin->setLog(&meshDoc()->Log);

		int capability = 0, defaultBits = 0;
		pCurrentIOPlugin->GetExportMaskCapability(extension, capability, defaultBits);

		// optional saving parameters (like ascii/binary encoding)
		RichParameterSet savePar;

		pCurrentIOPlugin->initSaveParameter(extension, *(mod), savePar);
		//***20160711***
		SaveMaskExporterDialog maskDialog(new QWidget(), mod, capability, defaultBits, &savePar, this->GLA());
		if (!saveAllPossibleAttributes)
			maskDialog.exec();//========================================start maskDialog
		else
		{
			maskDialog.SlotSelectionAllButton();
			maskDialog.updateMask();
		}
		int mask = maskDialog.GetNewMask();
		if (!saveAllPossibleAttributes)
		{
			maskDialog.close();
			if (maskDialog.result() == QDialog::Rejected)
				return false;
		}
		if (mask == -1)
			return false;

		qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
		qb->show();
		QTime tt; tt.start();
		ret = pCurrentIOPlugin->save(extension, fileName, *mod, mask, savePar, QCallBack, this);//****save function
		qb->reset();
		GLA()->Logf(GLLogStream::SYSTEM, "Saved Mesh %s in %i msec", qPrintable(fileName), tt.elapsed());

		qApp->restoreOverrideCursor();
		mod->setFileName(fileName);
		QSettings settings;
		int savedMeshCounter = settings.value("savedMeshCounter", 0).toInt();
		settings.setValue("savedMeshCounter", savedMeshCounter + 1);
		GLA()->setWindowModified(false);

	}
	return ret;
}
#pragma region exportMesh_backup
//bool MainWindow::exportMesh(QString fileName, MeshModel* mod, const bool saveAllPossibleAttributes)
//{
//	QStringList& suffixList = PM.outFilters;
//
//	//QHash<QString, MeshIOInterface*> allKnownFormats;
//	QFileInfo fi(fileName);
//	//PM.LoadFormats( suffixList, allKnownFormats,PluginManager::EXPORT);
//	//QString defaultExt = "*." + mod->suffixName().toLower();
//	QString defaultExt = "*." + fi.suffix().toLower();
//	if (defaultExt == "*.")
//		defaultExt = "*.ply";
//	if (mod == NULL)
//		return false;
//	mod->meshModified() = false;
//	QString laylabel = "Save \"" + mod->label() + "\" Layer";
//	QString ss = fi.absoluteFilePath();
//	QFileDialog* saveDialog = new QFileDialog(this, laylabel, fi.absolutePath());
//#if defined(Q_OS_WIN)
//	saveDialog->setOption(QFileDialog::DontUseNativeDialog);
//#endif
//	saveDialog->setNameFilters(suffixList);
//	saveDialog->setAcceptMode(QFileDialog::AcceptSave);
//	saveDialog->setFileMode(QFileDialog::AnyFile);
//	saveDialog->selectFile(fileName);
//	QStringList matchingExtensions = suffixList.filter(defaultExt);
//	if (!matchingExtensions.isEmpty())
//		saveDialog->selectNameFilter(matchingExtensions.last());
//	connect(saveDialog, SIGNAL(filterSelected(const QString&)), this, SLOT(changeFileExtension(const QString&)));
//
//	if (fileName.isEmpty()){
//		saveDialog->selectFile(meshDoc()->mm()->fullName());
//		int dialogRet = saveDialog->exec();
//		if (dialogRet == QDialog::Rejected)
//			return false;
//		fileName = saveDialog->selectedFiles().first();
//		QFileInfo fni(fileName);
//		if (fni.suffix().isEmpty())
//		{
//			QString ext = saveDialog->selectedNameFilter();
//			ext.chop(1); ext = ext.right(4);
//			fileName = fileName + ext;
//			qDebug("File without extension adding it by hand '%s'", qPrintable(fileName));
//		}
//	}
//
//
//	bool ret = false;
//
//	QStringList fs = fileName.split(".");
//
//	if (!fileName.isEmpty() && fs.size() < 2)
//	{
//		QMessageBox::warning(new QWidget(), "Save Error", "You must specify file extension!!");
//		return ret;
//	}
//
//	if (!fileName.isEmpty())
//	{
//		//save path away so we can use it again
//		QString path = fileName;
//		path.truncate(path.lastIndexOf("/"));
//		lastUsedDirectory.setPath(path);
//
//		QString extension = fileName;
//		extension.remove(0, fileName.lastIndexOf('.') + 1);
//
//		QStringListIterator itFilter(suffixList);
//
//		MeshIOInterface *pCurrentIOPlugin = PM.allKnowOutputFormats[extension.toLower()];
//		if (pCurrentIOPlugin == 0)
//		{
//			QMessageBox::warning(this, "Unknown type", "File extension not supported!");
//			return false;
//		}
//		//MeshIOInterface* pCurrentIOPlugin = meshIOPlugins[idx-1];
//		pCurrentIOPlugin->setLog(&meshDoc()->Log);
//
//		int capability = 0, defaultBits = 0;
//		pCurrentIOPlugin->GetExportMaskCapability(extension, capability, defaultBits);
//
//		// optional saving parameters (like ascii/binary encoding)
//		RichParameterSet savePar;
//
//		pCurrentIOPlugin->initSaveParameter(extension, *(mod), savePar);
//		//***20160711***
//		SaveMaskExporterDialog maskDialog(new QWidget(), mod, capability, defaultBits, &savePar, this->GLA());
//		if (!saveAllPossibleAttributes)
//			maskDialog.exec();//========================================start maskDialog
//		else
//		{
//			maskDialog.SlotSelectionAllButton();
//			maskDialog.updateMask();
//		}
//
//		int mask = maskDialog.GetNewMask();
//		int mask2 = mod->dataMask();
//		//qDebug()<<
//		if (!saveAllPossibleAttributes)
//		{
//			maskDialog.close();
//			if (maskDialog.result() == QDialog::Rejected)
//				return false;
//		}
//		if (mask == -1)
//			return false;
//
//		qApp->setOverrideCursor(QCursor(Qt::WaitCursor));//Wait Cursor
//		qb->show();
//		QTime tt; tt.start();
//		ret = pCurrentIOPlugin->save(extension, fileName, *mod, mask, savePar, QCallBack, this);//****save function
//		qb->reset();
//		GLA()->Logf(GLLogStream::SYSTEM, "Saved Mesh %s in %i msec", qPrintable(fileName), tt.elapsed());
//
//		qApp->restoreOverrideCursor();
//		mod->setFileName(fileName);
//		QSettings settings;
//		int savedMeshCounter = settings.value("savedMeshCounter", 0).toInt();
//		settings.setValue("savedMeshCounter", savedMeshCounter + 1);
//		GLA()->setWindowModified(false);
//
//	}
//	return ret;
//}
#pragma endregion exportMesh_backup
bool MainWindow::exportMesh(QString fileName, MeshModel* mod, const bool saveAllPossibleAttributes)
{
	QStringList& suffixList = PM.outFilters;//set in baseio.cpp QList<MeshIOInterface::Format> BaseMeshIOPlugin::exportFormats() const

	//QHash<QString, MeshIOInterface*> allKnownFormats;
	//QFileInfo fi(fileName);
	QFileInfo fi(mod->fullName());

	//PM.LoadFormats( suffixList, allKnownFormats,PluginManager::EXPORT);
	//QString defaultExt = "*." + mod->suffixName().toLower();
	QString defaultExt = "*." + fi.suffix().toLower();
	if (defaultExt == "*.")
		defaultExt = "*.ply";
	if (mod == NULL)
		return false;
	mod->meshModified() = false;
	//QString laylabel = "Save \"" + mod->label() + "\" Layer";
	QString laylabel = "Export Model";
	QString ss = fi.absoluteFilePath();
	QFileDialog* saveDialog = new QFileDialog(this, laylabel, fi.absolutePath());
#if defined(Q_OS_WIN)
	//saveDialog->setOption(QFileDialog::DontUseNativeDialog);
#endif
	saveDialog->setNameFilters(suffixList);
	saveDialog->setAcceptMode(QFileDialog::AcceptSave);
	saveDialog->setFileMode(QFileDialog::AnyFile);
	saveDialog->selectFile(fileName);
	QStringList matchingExtensions = suffixList.filter(defaultExt);
	if (!matchingExtensions.isEmpty())
		saveDialog->selectNameFilter(matchingExtensions.last());
	connect(saveDialog, SIGNAL(filterSelected(const QString&)), this, SLOT(changeFileExtension(const QString&)));

	if (fileName.isEmpty()) {
		saveDialog->selectFile(meshDoc()->mm()->fullName());
		int dialogRet = saveDialog->exec();
		if (dialogRet == QDialog::Rejected)
			return false;
		fileName = saveDialog->selectedFiles().first();
		QFileInfo fni(fileName);
		if (fni.suffix().isEmpty())
		{
			QString ext = saveDialog->selectedNameFilter();
			ext.chop(1); ext = ext.right(4);
			fileName = fileName + ext;
			qDebug("File without extension adding it by hand '%s'", qPrintable(fileName));
		}
	}


	bool ret = false;

	QStringList fs = fileName.split(".");

	if (!fileName.isEmpty() && fs.size() < 2)
	{
		QMessageBox::warning(new QWidget(), "Save Error", "You must specify file extension!!");
		return ret;
	}

	if (!fileName.isEmpty())
	{
		//save path away so we can use it again
		QString path = fileName;
		path.truncate(path.lastIndexOf("/"));
		lastUsedDirectory.setPath(path);

		QString extension = fileName;
		extension.remove(0, fileName.lastIndexOf('.') + 1);

		QStringListIterator itFilter(suffixList);

		MeshIOInterface *pCurrentIOPlugin = PM.allKnowOutputFormats[extension.toLower()];
		if (pCurrentIOPlugin == 0)
		{
			QMessageBox::warning(this, "Unknown type", "File extension not supported!");
			return false;
		}
		//MeshIOInterface* pCurrentIOPlugin = meshIOPlugins[idx-1];
		pCurrentIOPlugin->setLog(&meshDoc()->Log);

		int capability = 0, defaultBits = 0;
		pCurrentIOPlugin->GetExportMaskCapability(extension, capability, defaultBits);

		// optional saving parameters (like ascii/binary encoding)
		RichParameterSet savePar;

		pCurrentIOPlugin->initSaveParameter(extension, *(mod), savePar);
		//***20160711***
		SaveMaskExporterDialog maskDialog(new QWidget(), mod, capability, defaultBits, &savePar, this->GLA());
		if (!saveAllPossibleAttributes);
		//maskDialog.exec();//========================================start maskDialog
		else
		{
			maskDialog.SlotSelectionAllButton();
			maskDialog.updateMask();
		}

		int mask = maskDialog.GetNewMask();

		//qDebug()<<
		if (!saveAllPossibleAttributes)
		{
			/*maskDialog.close();
			if (maskDialog.result() == QDialog::Rejected)
			return false;*/
		}
		if (mask == -1)
			return false;

		qApp->setOverrideCursor(QCursor(Qt::WaitCursor));//Wait Cursor
		qb->show();
		QTime tt; tt.start();
		mod->updateDataMask(mod);
		ret = pCurrentIOPlugin->save(extension, fileName, *mod, mask, savePar, QCallBack, this);//****save function
		qb->reset();
		GLA()->Logf(GLLogStream::SYSTEM, "Saved Mesh %s in %i msec", qPrintable(fileName), tt.elapsed());

		qApp->restoreOverrideCursor();
		mod->setFileName(fileName);
		QSettings settings;
		int savedMeshCounter = settings.value("savedMeshCounter", 0).toInt();
		settings.setValue("savedMeshCounter", savedMeshCounter + 1);
		GLA()->setWindowModified(false);

	}
	return ret;
}
void MainWindow::changeFileExtension(const QString& st)
{
	QFileDialog* fd = qobject_cast<QFileDialog*>(sender());
	if (fd == NULL)
		return;
	QRegExp extlist("\\*.\\w+");
	int start = st.indexOf(extlist);
	(void)start;
	QString ext = extlist.cap().remove("*");
	QStringList stlst = fd->selectedFiles();
	if (!stlst.isEmpty())
	{
		QFileInfo fi(stlst[0]);
		fd->selectFile(fi.baseName() + ext);
	}
}

bool MainWindow::save(const bool saveAllPossibleAttributes)
{
	return exportMesh(meshDoc()->mm()->fullName(), meshDoc()->mm(), saveAllPossibleAttributes);
}

//*** Modified by KPPH R&D-I-SW, Mark to apply saving multiple mesh in a single  file
bool MainWindow::saveAs(QString fileName, const bool saveAllPossibleAttributes)
{
	bool exp;

	if (meshDoc()->multiSelectID.size() > 0)
	{
		if (meshDoc()->multiSelectID.size() > 1)
		{
			MeshModel *m = meshDoc()->mm();
			QList<MeshModel *> expMeshList;
			expMeshList.clear();

			foreach(int i, meshDoc()->multiSelectID)
			{
				MeshModel *newMesh = new MeshModel(meshDoc(), fileName, "b");
				newMesh->updateDataMask(meshDoc()->getMesh(i)->dataMask());

				if (newMesh->hasDataMask(MeshModel::MM_WEDGTEXCOORD))
				{
					QMessageBox msg;
					msg.setIcon(QMessageBox::Information);
					msg.setWindowTitle(tr("Export Mesh"));
					msg.setText(tr("STL file does not support texture files. texture will be removed from the mesh."));
					msg.setStandardButtons(QMessageBox::Ok);
					msg.exec();
				}

				vcg::tri::Append<CMeshO, CMeshO>::MeshCopy(newMesh->cm, meshDoc()->getMesh(i)->cm);
				newMesh->cm.Tr = meshDoc()->getMesh(i)->cm.Tr;
				newMesh->rmm = meshDoc()->getMesh(i)->rmm;

				//if (newMesh->hasDataMask(MeshModel::MM_COLOR))
				//{
				//	newMesh->Enable(tri::io::Mask::IOM_FACECOLOR);
				//	tri::UpdateColor<CMeshO>::PerFaceFromVertex(newMesh->cm);
				//}

				if (newMesh->rmm.colorMode == GLW::CMPerVert)
				{
					newMesh->Enable(tri::io::Mask::IOM_FACECOLOR);

					if ((newMesh->hasDataMask(MeshModel::MM_COLOR) || newMesh->hasDataMask(MeshModel::MM_VERTCOLOR)) && !newMesh->hasDataMask(MeshModel::MM_WEDGTEXCOORD))
					{
						newMesh->Enable(tri::io::Mask::IOM_FACECOLOR);
						tri::UpdateColor<CMeshO>::PerFaceFromVertex(newMesh->cm);
					}
					else
					{
						tri::UpdateColor<CMeshO>::PerVertexConstant(newMesh->cm, Color4b::White);
						tri::UpdateNormal<CMeshO>::PerVertex(newMesh->cm);
						newMesh->cm.C() = Color4b::White;
						tri::UpdateColor<CMeshO>::PerFaceFromVertex(newMesh->cm);
					}
				}
				else if (newMesh->rmm.colorMode == GLW::CMNone)
				{
					newMesh->Enable(tri::io::Mask::IOM_FACECOLOR);

					if (!newMesh->hasDataMask(MeshModel::MM_WEDGTEXCOORD))
					{
						tri::UpdateColor<CMeshO>::PerVertexConstant(newMesh->cm, Color4b::White);
						tri::UpdateNormal<CMeshO>::PerVertex(newMesh->cm);
						newMesh->cm.C() = Color4b::White;
						tri::UpdateColor<CMeshO>::PerFaceFromVertex(newMesh->cm);
					}
					else
					{
						tri::UpdateColor<CMeshO>::PerVertexConstant(newMesh->cm, Color4b::White);
						tri::UpdateNormal<CMeshO>::PerVertex(newMesh->cm);
						newMesh->cm.C() = Color4b::White;
						tri::UpdateColor<CMeshO>::PerFaceFromVertex(newMesh->cm);
					}
				}

				expMeshList.push_back(newMesh);
			}

			foreach(MeshModel *nm, expMeshList)
			{
				if (expMeshList[0]->id() != nm->id())
					vcg::tri::Append<CMeshO, CMeshO>::Mesh(expMeshList[0]->cm, nm->cm);
			}

			MeshModel *expMesh = expMeshList[0];

			exp = exportMesh(fileName, expMesh, saveAllPossibleAttributes);

			if (exp)
			{
				QMessageBox::information(this, tr("Export Mesh"), tr("Mesh Export Complete!"));
			}

			return exp;
		}
		else
		{
			MeshModel *expMesh = new MeshModel(meshDoc(), fileName, "b");
			expMesh->updateDataMask(meshDoc()->mm()->dataMask());

			if (expMesh->hasDataMask(MeshModel::MM_WEDGTEXCOORD))
			{
				QMessageBox msg;
				msg.setIcon(QMessageBox::Information);
				msg.setWindowTitle(tr("Export Mesh"));
				msg.setText(tr("STL file does not support texture files. texture will be removed from the mesh."));
				msg.setStandardButtons(QMessageBox::Ok);
				msg.exec();
			}

			vcg::tri::Append<CMeshO, CMeshO>::MeshCopy(expMesh->cm, meshDoc()->mm()->cm);
			expMesh->cm.Tr = meshDoc()->mm()->cm.Tr;
			expMesh->rmm = meshDoc()->mm()->rmm;

			//if (expMesh->hasDataMask(MeshModel::MM_COLOR))
			//{
			//	expMesh->Enable(tri::io::Mask::IOM_FACECOLOR);
			//	tri::UpdateColor<CMeshO>::PerFaceFromVertex(expMesh->cm);
			//}

			if (expMesh->rmm.colorMode == GLW::CMPerVert)
			{
				expMesh->Enable(tri::io::Mask::IOM_FACECOLOR);

				if ((expMesh->hasDataMask(MeshModel::MM_COLOR) || expMesh->hasDataMask(MeshModel::MM_VERTCOLOR)) && !expMesh->hasDataMask(MeshModel::MM_WEDGTEXCOORD))
				{
					expMesh->Enable(tri::io::Mask::IOM_FACECOLOR);
					tri::UpdateColor<CMeshO>::PerFaceFromVertex(expMesh->cm);
				}
				else
				{
					tri::UpdateColor<CMeshO>::PerVertexConstant(expMesh->cm, Color4b::White);
					tri::UpdateNormal<CMeshO>::PerVertex(expMesh->cm);
					expMesh->cm.C() = Color4b::White;
					tri::UpdateColor<CMeshO>::PerFaceFromVertex(expMesh->cm);
				}
			}
			else if (expMesh->rmm.colorMode == GLW::CMNone)
			{
				expMesh->Enable(tri::io::Mask::IOM_FACECOLOR);

				if (!expMesh->hasDataMask(MeshModel::MM_WEDGTEXCOORD))
				{
					tri::UpdateColor<CMeshO>::PerVertexConstant(expMesh->cm, Color4b::White);
					tri::UpdateNormal<CMeshO>::PerVertex(expMesh->cm);
					expMesh->cm.C() = Color4b::White;
					tri::UpdateColor<CMeshO>::PerFaceFromVertex(expMesh->cm);
				}
				else
				{
					tri::UpdateColor<CMeshO>::PerVertexConstant(expMesh->cm, Color4b::White);
					tri::UpdateNormal<CMeshO>::PerVertex(expMesh->cm);
					expMesh->cm.C() = Color4b::White;
					tri::UpdateColor<CMeshO>::PerFaceFromVertex(expMesh->cm);
				}
			}

			exp = exportMesh(fileName, expMesh, saveAllPossibleAttributes);

			if (exp)
			{
				QMessageBox::information(this, tr("Export Mesh"), tr("Mesh Export Complete!"));
			}

			return exp;
		}
	}
	else
	{
		QMessageBox::information(this, tr("Export Mesh"), tr("Please select a Mesh (or Meshes) to export"));
	}
}

bool MainWindow::saveSnapshot()
{
	SaveSnapshotDialog dialog(this);

	dialog.setValues(GLA()->ss);

	if (dialog.exec() == QDialog::Accepted)
	{
		GLA()->ss = dialog.getValues();
		GLA()->saveSnapshot();

		// if user ask to add the snapshot to raster layers
		/*
		if(dialog.addToRasters())
		{
		QString savedfile = QString("%1/%2%3.png")
		.arg(GLA()->ss.outdir).arg(GLA()->ss.basename)
		.arg(GLA()->ss.counter,2,10,QChar('0'));

		importRaster(savedfile);
		}
		*/

		return true;
	}

	return false;
}
void MainWindow::about()
{
	//QDialog *about_dialog = new QDialog();
	//Ui::aboutDialog temp;
	//temp.setupUi(about_dialog);
	//temp.labelMLName->setText(PicaApplication::completeName(PicaApplication::HW_ARCHITECTURE(QSysInfo::WordSize)) + "   (" + __DATE__ + ")");
	////about_dialog->setFixedSize(566,580);
	//about_dialog->show();

	/*QDialog *about_dialog = new QDialog(0, Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
	Ui::PaletteAbout temp;
	temp.setupUi(about_dialog);
	temp.programName->setText(PicaApplication::completeName(PicaApplication::HW_ARCHITECTURE(QSysInfo::WordSize)) + "   (" + __DATE__ + ")");
	///about_dialog->setFixedSize(566,580);
	about_dialog->show();*/

	PaletteAbout *pabout = new PaletteAbout(this);
	pabout->show();
	qb->reset();



}


void MainWindow::aboutPlugins()
{
	qDebug("aboutPlugins(): Current Plugins Dir: %s ", qPrintable(pluginManager().getDefaultPluginDirPath()));
	PluginDialog dialog(pluginManager().getDefaultPluginDirPath(), pluginManager().pluginsLoaded, this);
	dialog.exec();
}

void MainWindow::helpOnscreen()
{
	//if (GLA()) GLA()->toggleHelpVisible();
}

void MainWindow::helpOnline()
{
	//checkForUpdates(false);
	//QDesktopServices::openUrl(QUrl("http://sourceforge.net/apps/mediawiki/meshlab"));
}


void MainWindow::showToolbarFile() {
	mainToolBar->setVisible(!mainToolBar->isVisible());
}

void MainWindow::showToolbarRender() {
	renderToolBar->setVisible(!renderToolBar->isVisible());
}

void MainWindow::showInfoPane() { if (GLA() != 0)	GLA()->infoAreaVisible = !GLA()->infoAreaVisible; }
void MainWindow::showTrackBall() { if (GLA() != 0) 	GLA()->showTrackBall(!GLA()->isTrackBallVisible()); }
void MainWindow::resetTrackBall() { if (GLA() != 0)	GLA()->resetTrackBall(); }
void MainWindow::showRaster() { if (GLA() != 0)	GLA()->showRaster((QApplication::keyboardModifiers() & Qt::ShiftModifier)); }
void MainWindow::showLayerDlg(bool visible)
{
	if ((GLA() != 0) && (layerDialog != NULL))
	{
		layerDialog->setVisible(visible);
		showLayerDlgAct->setChecked(visible);

	}
}
void MainWindow::showXMLPluginEditorGui()
{
	if (GLA() != 0)
		plugingui->setVisible(!plugingui->isVisible());
}


void MainWindow::setCustomize()
{
	CustomDialog dialog(currentGlobalParams, defaultGlobalParams, this);
	connect(&dialog, SIGNAL(applyCustomSetting()), this, SLOT(updateCustomSettings()));
	dialog.exec();
}

//void MainWindow::renderBbox()        { GLA()->setDrawMode(GLW::DMBox     ); }
//void MainWindow::renderPoint()       { GLA()->setDrawMode(GLW::DMPoints  ); }
//void MainWindow::renderWire()        { GLA()->setDrawMode(GLW::DMWire    ); }
//void MainWindow::renderFlat()        { GLA()->setDrawMode(GLW::DMFlat    ); }
//void MainWindow::renderFlatLine()    { GLA()->setDrawMode(GLW::DMFlatWire); }
//void MainWindow::renderHiddenLines() { GLA()->setDrawMode(GLW::DMHidden  ); }
//void MainWindow::renderSmooth()      { GLA()->setDrawMode(GLW::DMSmooth  ); }
//void MainWindow::renderTexture()
//{
//    QAction *a = qobject_cast<QAction* >(sender());
//  if( tri::HasPerVertexTexCoord(meshDoc()->mm()->cm))
//    GLA()->setTextureMode(!a->isChecked() ? GLW::TMNone : GLW::TMPerVert);
//  if( tri::HasPerWedgeTexCoord(meshDoc()->mm()->cm))
//    GLA()->setTextureMode(!a->isChecked() ? GLW::TMNone : GLW::TMPerWedgeMulti);
//}


void MainWindow::fullScreen() {
	if (!isFullScreen())
	{
		toolbarState = saveState();//***
		/*menuBar()->hide();
		mainToolBar->hide();
		renderToolBar->hide();*/
		//globalStatusBar()->hide();
		setWindowState(windowState() ^ Qt::WindowFullScreen);
		bool found = true;
		//Caso di piu' finestre aperte in tile:
		if ((mdiarea->subWindowList()).size() > 1) {
			foreach(QWidget *w, mdiarea->subWindowList()) { if (w->isMaximized()) found = false; }
			if (found)mdiarea->tileSubWindows();
		}
	}
	else
	{
		menuBar()->show();
		restoreState(toolbarState);
		//globalStatusBar()->show();

		setWindowState(windowState() ^ Qt::WindowFullScreen);
		bool found = true;
		//Caso di piu' finestre aperte in tile:
		if ((mdiarea->subWindowList()).size() > 1) {
			foreach(QWidget *w, mdiarea->subWindowList()) { if (w->isMaximized()) found = false; }
			if (found) { mdiarea->tileSubWindows(); }
		}
		fullScreenAct->setChecked(false);
	}
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
	/*if (e->key() == Qt::Key_Return && e->modifiers() == Qt::AltModifier)
	{
	fullScreen();
	e->accept();
	}*/
	if (e->key() == Qt::Key_Slash && e->modifiers() == Qt::AltModifier)
		//if (e->key() == Qt::Key_0)
	{
		if (programmerTest->menuAction()->isVisible())
			programmerTest->menuAction()->setVisible(false);
		else
			programmerTest->menuAction()->setVisible(true);
		e->accept();
	}
	//else if (e->key() == Qt::Key_A)
	//{
	//	float qqtemp = currentGlobalParams.getFloat("Quarternion_test_param");
	//	//qqtemp += 0.1;
	//	currentGlobalParams.setValue("Quarternion_test_param", FloatValue(qqtemp));
	//	RichParameterSet printParamt;
	//	//printParamt
	//	executeFilter(PM.actionFilterMap.value("FP_Test_Quaternion"), printParamt, false);
	//	GLA()->setHastoRefreshVBO();
	//	//GLA()->setHastoUpdatePartVBO();
	//	meshDoc()->setBusy(false);


	//	e->accept();
	//}
	//else if (e->key() == Qt::Key_Z)
	//{
	//	float qqtemp = currentGlobalParams.getFloat("Quarternion_test_param");
	//	//qqtemp -= 0.1;
	//	currentGlobalParams.setValue("Quarternion_test_param", FloatValue(qqtemp));
	//	testFuncFunc();
	//	e->accept();
	//}
	//else if (e->key() == Qt::Key_Q)
	//{
	//	float qqtemp = currentGlobalParams.getFloat("Quarternion_test_param");
	//	//qqtemp -= 0.1;
	//	currentGlobalParams.setValue("Quarternion_test_param", FloatValue(qqtemp));
	//	testFuncFunc();
	//	e->accept();
	//}
	else e->ignore();
}
void MainWindow::set_cancel_filter()
{
	cancel_filter = false;
	std::string funGETouts;
	//ccbg.sendGET("job/cancel", ChipCommunication_t::GETParamPairSet(), funGETouts);
	//qDebug() << "QString::fromStdString(funGETouts)" << QString::fromStdString(funGETouts);
}
void MainWindow::set_pause_filter()
{
	pause_filter = true;

}

bool MainWindow::QCallBack2(const int pos, const char * str)
{

	int timeout = 0;

	QString ss = QString().fromStdString(str);

	//int completed_page = comm->jobCompletedPages();


	//int currentDynamicWipe = 0;
	//comm->getDynamicMidjob_PageInterval(currentDynamicWipe);//get dynamic page
	//QString currentdynamic = "currentdynamic: " + QString::number(currentDynamicWipe) + "\n";
	//fprintf(dbgff, currentdynamic.toStdString().c_str()); //@@@	
	//fflush(dbgff);

	//get Dynamic wipe value
	//define a normal wipe page 6
	//compare with 6
	//if(current value!=6)setdynamic wipe to 6

	//vector to store  wipe or not	
	//if yes, send dynamic wiper page.

	//******dynamic wipe pages	
	if ((dynamicWipe.size() > 2))
		//if ((dynamicWipe.size() > completed_page + 2 - DSP_Blank_pages) && completed_page>0)
	{
		if (dynamicWipe.size() < 4){
			dynamicWipe.insert(6, 4);
			dynamicWipe.insert(7, 4);

		}
		QMap<int, int>::const_iterator itr = dynamicWipe.find(dynamicWipe.lastKey());
		/*if (dynamicWipe.size() > 3)
		{*/
		/*int a = (itr).key();
		int b = (itr).value();
		int c = (--itr).value();*/
		itr -= 1;
		if (dynamicWipe.last() != (itr).value())
		{
			bool succ = comm->setDynamicMidjob_Page(dynamicWipe.lastKey(), dynamicWipe.last());
			finalDynamicPage.insert(dynamicWipe.lastKey(), dynamicWipe.last());
			QString dylastkey = QString::number(succ) + ", " + QString::number(dynamicWipe.lastKey()) + " : ";
			QString dylastvalue = QString::number(dynamicWipe.last()) + "\n";
			fprintf(dbgff, dylastkey.toStdString().c_str()); //@@@
			fprintf(dbgff, dylastvalue.toStdString().c_str()); //@@@
			fflush(dbgff);
		}
		//}

	}

	//fprintf(dbgff, "a-0\n"); //@@@
	//fflush(dbgff);
	if (ss == "start_printing_slice_process"){
		while (1)
		{
			std::string s, e;
			comm->printerStatus(s, e);
			//fprintf(dbgff, s.c_str()); //@@@
			//fflush(dbgff);
			QString temp = QString::fromStdString(s);
			bool breakflag = true;

			std::map<std::string, int>::iterator it;
			it = comm->printer3dpString.printerStatusmap.find(s);
			if (it != comm->printer3dpString.printerStatusmap.end())
			{


				switch (comm->printer3dpString.printerStatusmap.at(s))
				{
				case Comm3DP::printer3dpString::printStatus::Paused://pause 
				{
					statusLabel->setText(temp);
					statusLabel->repaint();
					return true;
				}
				break;
				case Comm3DP::printer3dpString::printStatus::WiperSetting:
				case Comm3DP::printer3dpString::printStatus::BinderRecovering:
				case Comm3DP::printer3dpString::printStatus::Poweroff:
				case Comm3DP::printer3dpString::printStatus::Pausing://pause
				case Comm3DP::printer3dpString::printStatus::Printing_INK_LOW://pause
				case Comm3DP::printer3dpString::printStatus::Printing_CARTRIDGE_MISSING://pause			
				case Comm3DP::printer3dpString::printStatus::Fill_INK_To_Resume://pause
				case Comm3DP::printer3dpString::printStatus::PanelControl://pause				
				{
					statusLabel->setText(temp);
					statusLabel->repaint();
					return true;
				}
				break;
				case Comm3DP::printer3dpString::printStatus::Printing://Printing
				{
					statusLabel->setText(temp);
					statusLabel->repaint();
					return true;
				}
				break;
				case Comm3DP::printer3dpString::printStatus::Online://????
				{
					statusLabel->setText(temp);
					statusLabel->repaint();
					//return true;
					return false;
				}
				break;
				case Comm3DP::printer3dpString::printStatus::PostPrinting:
				{
					statusLabel->setText(temp);
					statusLabel->repaint();
					return false;
				}break;
				case Comm3DP::printer3dpString::printStatus::DoorOpened:
				{
					statusLabel->setText(temp);
					statusLabel->repaint();
				}break;
				case Comm3DP::printer3dpString::printStatus::GettingError:
				{
					statusLabel->setText(temp);
					statusLabel->setStyleSheet(WidgetStyleSheet::statusLabel());
					statusLabel->repaint();
					return false;
				}
				break;
				default:
				{
					return true;
				}break;

				}
			}
			else return true;

			Sleep(500);
			timeout += 500;

			if (timeout > 1800000)return false;
		}
	}
	else if (ss == "slice_process")
	{
		int static lastPos = -1;

		//if (pos == lastPos) return true;
		lastPos = pos;

		static QTime currTime = QTime::currentTime();
		if (currTime.elapsed() < 100) return true;
		currTime.start();
		statusLabel->setText("Running");
		statusLabel->setStyleSheet(WidgetStyleSheet::statusLabel());
		statusLabel->repaint();
		//MainWindow::globalStatusBar()->showMessage(str, 5000);
		qb->show();
		qb->setEnabled(true);
		qb->setValue(pos);
		s_pushbutton->show();

		bool isOK;
		QMessageBox msgBox, msgBox2;
		qApp->processEvents();//***20160630***error at picasso runtime

		int result, result2;
		if (pause_filter)
		{
			msgBox.setWindowTitle("BCPware");
			msgBox.setText("Continue this Job");
			msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
			result = msgBox.exec();
			pause_filter = false;
		}

		if (!cancel_filter)
		{
			msgBox2.setWindowTitle("BCPware");
			msgBox2.setText("cancel");
			msgBox2.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
			result2 = msgBox2.exec();
			cancel_filter = true;
		}

		if (result2 == QMessageBox::Ok || result == QMessageBox::Cancel)
		{
			cancel_filter = true;
			statusLabel->setText("");
			statusLabel->setStyleSheet(WidgetStyleSheet::statusLabel());
			statusLabel->repaint();
			return false;
		}
		else
			return true;


	}
	return true;
}
bool MainWindow::QCallBackdata(const int layer, const int dw)
{
	//dynamicWipe.append(dw);
	QString temp = "qcallbackdata" + QString::number(dw) + "\n";
	fprintf(dbgff, temp.toStdString().c_str()); //@@@	
	fflush(dbgff);

	dynamicWipe.insert(layer, dw);

	return true;
}
bool MainWindow::QCallBack(const int pos, const char * str)
{
	int static lastPos = -1;

	if (pos == lastPos) return true;
	lastPos = pos;

	static QTime currTime = QTime::currentTime();
	if (currTime.elapsed() < 100) return true;

	currTime.start();
	//MainWindow::globalStatusBar()->showMessage(str, 5000);
	//s_pushbutton->show();

	qb->show();
	qb->setEnabled(true);
	qb->setValue(pos);

	//***20160623_Test

	//MainWindow::globalStatusBar()->update();

	qApp->processEvents();//***20160630***error at picasso runtime
	//MainWindow::s_pushbutton->update();	
	//testGetstring();//***20160701***test_get_string

	bool isOK;
	QMessageBox msgBox, msgBox2;
	int result, result2;

	if (pause_filter)
	{
		msgBox.setWindowTitle("BCPware");
		msgBox.setText("Continue ");
		msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
		result = msgBox.exec();
		pause_filter = false;
	}

	if (!cancel_filter)
	{
		msgBox2.setWindowTitle("BCPware");
		msgBox2.setText("Cancel");
		msgBox2.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
		result2 = msgBox2.exec();
		cancel_filter = true;
	}

	if (result2 == QMessageBox::Ok || result == QMessageBox::Cancel)
	{
		cancel_filter = true;
		qb->reset();
		s_pushbutton->setVisible(false);
		return false;
	}
	else
	{
		return true;
	}


}

void MainWindow::updateProgressBar(const int pos, const QString& text)
{
	this->QCallBack(pos, qPrintable(text));
}

//void MainWindow::evaluateExpression(const Expression& exp,Value** res )
//{
//	try
//	{
//		PM.env.pushContext();
//		*res = exp.eval(&PM.env);
//		PM.env.popContext();
//	}
//	catch (ParsingException& e)
//	{
//		GLA()->Logf(GLLogStream::WARNING,e.what());
//	}
//}
void MainWindow::updateDocumentScriptBindings()
{
	if (currentViewContainer())
	{
		//***20150527
		plugingui->setDocument(meshDoc());
		//PM.updateDocumentScriptBindings(*meshDoc());
	}
}

void MainWindow::loadAndInsertXMLPlugin(const QString& xmlpath, const QString& scriptname)
{
	if ((xmldialog != NULL) && (xmldialog->isVisible()))
		this->xmldialog->close();
	PM.deleteXMLPlugin(scriptname);
	try
	{
		PM.loadXMLPlugin(xmlpath);
	}
	catch (MeshLabXMLParsingException& e)
	{
		qDebug() << e.what();
	}
	fillFilterMenu();
	initSearchEngine();
}

void MainWindow::sendHistory()
{
	plugingui->getHistory(meshDoc()->xmlhistory);
}


void MainWindow::updateRenderToolBar(RenderModeAction* /*act*/)
{
	//if (GLA() == NULL)
	//	return;
	//QMap<int,RenderMode>& rmode = GLA()->rendermodemap;
	//const RenderMode& tmp = *(rmode.begin());
	//for(QMap<int,RenderMode>::const_iterator it = rmode.begin(); it != rmode.end();++it)
	//{
	//	if ()
	//}
}

void MainWindow::updateRenderMode()
{
	if ((GLA() == NULL) || (meshDoc() == NULL))
		return;
	QMap<int, RenderMode>& rmode = GLA()->rendermodemap;

	RenderModeAction* act = qobject_cast<RenderModeAction*>(sender());
	RenderModeTexturePerVertAction* textact = qobject_cast<RenderModeTexturePerVertAction*>(act);

	//act->data contains the meshid to which the action is referred.
	//if the meshid is -1 the action is intended to be per-document and not per mesh
	bool isvalidid = true;
	int meshid = act->data().toInt(&isvalidid);
	if (!isvalidid)
		throw MeshLabException("A RenderModeAction contains a non-integer data id.");

	if (meshid == -1)
	{
		for (QMap<int, RenderMode>::iterator it = rmode.begin(); it != rmode.end(); ++it)
		{

			act->updateRenderMode(it.value());
			//horrible trick caused by MeshLab GUI. In MeshLab exists just a button turning on/off the texture visualization.
			//Unfortunately the RenderMode::textureMode member field is not just a boolean value but and enum one.
			//The enum-value depends from the enabled attributes of input mesh.
			if (textact != NULL)
				setBestTextureModePerMesh(textact, it.key(), it.value());
		}
	}
	else
	{
		QMap<int, RenderMode>::iterator it = rmode.find(meshid);
		if (it == rmode.end())
			throw MeshLabException("A RenderModeAction contains a non-valid data meshid.");
		act->updateRenderMode(it.value());
		updateMenus();
		//horrible trick caused by MeshLab GUI. In MeshLab exists just a button turning on/off the texture visualization.
		//Unfortunately the RenderMode::textureMode member field is not just a boolean value but and enum one.
		//The enum-value depends from the enabled attributes of input mesh.
		if (textact != NULL)
			setBestTextureModePerMesh(textact, meshid, it.value());
	}
	GLA()->update();
}

void MainWindow::connectRenderModeActionList(QList<RenderModeAction*>& actlist)
{
	for (int ii = 0; ii < actlist.size(); ++ii)
		connect(actlist[ii], SIGNAL(triggered()), this, SLOT(updateRenderMode()));
}

vcg::GLW::TextureMode MainWindow::getBestTextureRenderModePerMesh(const int meshid)
{
	MeshModel* mesh = NULL;
	if ((meshDoc() == NULL) || ((mesh = meshDoc()->getMesh(meshid)) == NULL))
		return vcg::GLW::TMNone;

	if (mesh->hasDataMask(MeshModel::MM_WEDGTEXCOORD))
		return vcg::GLW::TMPerWedgeMulti;

	if (mesh->hasDataMask(MeshModel::MM_VERTTEXCOORD))
		return vcg::GLW::TMPerVert;

	return vcg::GLW::TMNone;
}

void MainWindow::setBestTextureModePerMesh(RenderModeAction* textact, const int meshid, RenderMode& rm)
{
	if ((textact == NULL) || !textact->isChecked())
		rm.setTextureMode(vcg::GLW::TMNone);
	else
	{
		vcg::GLW::TextureMode texmode = getBestTextureRenderModePerMesh(meshid);
		rm.setTextureMode(texmode);
	}
}
//***20150604***呼叫edit interface的函式
void MainWindow::executeEdit(QAction *editAction, RichParameterSet &passTrans, bool dummy)
{
	delOl(MeshModel::meshsort::slice_item);
	MeshEditInterFace_v2  *iEditI = qobject_cast<MeshEditInterFace_v2 *>(editAction->parent());
	//MeshEditInterFace_v2 *iEditI = PM.meshEditInterfacePlugV2.first();
	//***20150909***
	//delOl("_temp_outlines", "_temp_ol_mesh");
	delOl(meshDoc()->p_setting.getOlMeshName(), meshDoc()->p_setting.getCapMeshName());
	//*************
	//20160527_test_command
	QString b = editAction->text();
	Point3f old_dim = meshDoc()->mm()->cm.bbox.Dim();
	Matrix44m old_t_matrix = meshDoc()->mm()->cm.Tr;
	int x = meshDoc()->mm()->id();
	Matrix44m old_r_matrix = meshDoc()->mm()->cm.Tr;

	QList<Point3f> old_center;
	old_center.clear();

	foreach(int i, meshDoc()->multiSelectID)
	{
		old_center.push_back(meshDoc()->getMesh(i)->cm.bbox.Center());
	}

	//int x = meshDoc()->mm()->id();
	//***************
	iEditI->applyEdit_v2(editAction, *meshDoc(), passTrans, QCallBack);


	if (b.contains("edit_translate"))
	{
		//matrix_changed(meshDoc()->mm(), old_t_matrix);
		qDebug() << "executeEdit_edit_translate";
		matrix_changed_2(meshDoc()->multiSelectID, old_t_matrix);
		GLA()->VBOupdatePart[0] = true;

	}

	else if (b.contains("edit_scale"))
	{
		//scale_changed(meshDoc()->mm(), old_dim);

		scale_changed(meshDoc()->multiSelectID, old_dim);
	}

	else if (b.contains("edit_rotate"))
	{
		Point3d rotation = Point3d(rotate_widget_cus->get_x_rotation(), rotate_widget_cus->get_y_rotation(), rotate_widget_cus->get_z_rotation());
		//qDebug() << "x: " << rotate_widget_cus->get_x_rotation() << "   y: " << rotate_widget_cus->get_y_rotation() << "   z: " << rotate_widget_cus->get_z_rotation();
		rotate_changed(meshDoc()->multiSelectID, rotation, old_center);

		if (meshDoc()->mm() != NULL)
		{
			//transformwidgett1->loadParameter();
			scale_widget_cus->loadParameter();
			//rotate_widget_cus->loadParameter();
		}
		GLA()->VBOupdatePart[1] = true;
	}
	GLA()->reFreshGlListlist();
	//GLA()->setHastoUpdateVBO();
	GLA()->setHastoUpdatePartVBO();
	//***20151006***偵測邊界
	//if (!meshDoc()->meshList.empty())
	if (meshDoc() != NULL)
	{
		meshDoc()->groove.setGrooveHit(SKT::detect_hit_groove<Scalarm>(*meshDoc()));

	}
	//updateMenus();

}
//void MainWindow::startsetting()
//{
//	//GLint x[2];
//	//glGetIntegerv(GL_SMOOTH_LINE_WIDTH_RANGE, x);
//
//	//QMessageBox::information(this, "Info", "You selected the file '" + fileName + "'");
//
//	assert(meshDoc());
//	qDebug() << "startsetting";
//	RichParameterSet dummyParam;
//	float w = meshDoc()->groove.groovewidth;
//	float l = meshDoc()->groove.groovelength;
//	float h = meshDoc()->groove.grooveheight;//***建構槽大小
//
//	/*float t_g = ;
//	float b_g = ;
//	float r_g = ;
//	float l_g = ;
//	float bt_g = ;*/
//
//	const QString DEFAULT_VERTICES_LIMIT("vertices_limit");
//	QSettings settings;
//	///////////////load register value 
//	int Vertices = settings.value(DEFAULT_VERTICES_LIMIT).toInt();
//
//
//	if (Vertices == 0)Vertices = 30000000;
//
//	dummyParam.addParam(new RichFloat("groove_length", l, "x", ""));
//	dummyParam.addParam(new RichFloat("groove_height", h, "y", ""));
//	dummyParam.addParam(new RichFloat("groove_width", w, "z", ""));
//	dummyParam.addParam(new RichInt("Vertices_Number", Vertices, "Vertices Number", "bigger_use_Point_Draw"));
//	/*QStringList rotate_Axis;
//	rotate_Axis.push_back("3DP_1");
//	rotate_Axis.push_back("3DP_2");
//	dummyParam.addParam(new RichEnum("3DP", 0, rotate_Axis, tr("3DP"), tr("")));*/
//
//
//	dummyParam.addParam(new RichFloat("top_gap", meshDoc()->p_setting.getTopGap(), "top_gap", ""));
//	dummyParam.addParam(new RichFloat("bottom_gap", meshDoc()->p_setting.getBottom_Gap(), "bottom_gap", ""));
//	dummyParam.addParam(new RichFloat("right_gap", meshDoc()->p_setting.getRight_Gap(), "right_gap", ""));
//	dummyParam.addParam(new RichFloat("left_gap", meshDoc()->p_setting.getLeft_Gap(), "left_gapr", ""));
//	dummyParam.addParam(new RichFloat("between_gap", meshDoc()->p_setting.getBetweenGap(), "between_gap", ""));
//	dummyParam.addParam(new RichString("dir", meshDoc()->p_setting.getPicDir(), "dir", ""));
//	dummyParam.addParam(new RichEnum("grid_unit", 0, QStringList() << "centimeter" << "inch", tr("grid_unit"), QString("")));//*
//	dummyParam.addParam(new RichFloat("slice_Height", meshDoc()->p_setting.getbuildheight().value, "slice_height", ""));//*
//	dummyParam.addParam(new RichBool("start_print", meshDoc()->p_setting.get_print_or_not(), "start_print", ""));
//	dummyParam.addParam(new RichFloat("useless_print", meshDoc()->p_setting.get_useless_print(), "useless_print", ""));
//	dummyParam.addParam(new RichEnum("print_size", meshDoc()->p_setting.print_pagesize, QStringList() << "printBed_size" << "long_page_size", tr("print_size"), QString("")));
//
//	for (int ii = 0; ii < dummyParam.paramList.size(); ++ii)
//	{
//		RichParameterCopyConstructor v;
//		dummyParam.paramList.at(ii)->accept(v);
//		QDomDocument doc("someSetting");
//		RichParameterXMLVisitor vxml(doc);
//		v.lastCreated->accept(vxml);//create element attribute in here
//		doc.appendChild(vxml.parElem);
//		QString docstring = doc.toString();
//		QSettings setting;
//		setting.setValue(v.lastCreated->name, QVariant(docstring));
//	}
//
//
//	genStdialog = new GenericParamDialog(this, &dummyParam);
//
//	genStdialog->exec();
//	meshDoc()->groove.setLWH(dummyParam.getFloat("groove_length"), dummyParam.getFloat("groove_width"), dummyParam.getFloat("groove_height"));
//	//vtNumberLimit = ;//大於多少點用drawpoint顯示
//	setVtNumberLimit(dummyParam.getInt("Vertices_Number"));
//
//	//save value to register
//	settings.setValue(DEFAULT_VERTICES_LIMIT, dummyParam.getInt("Vertices_Number"));
//
//
//
//	meshDoc()->p_setting.setTopGap(dummyParam.getFloat("top_gap"));
//	meshDoc()->p_setting.setBottomGap(dummyParam.getFloat("bottom_gap"));
//	meshDoc()->p_setting.setRightGap(dummyParam.getFloat("right_gap"));
//	meshDoc()->p_setting.setLeftGap(dummyParam.getFloat("left_gap"));
//	meshDoc()->p_setting.setBetweenGap(dummyParam.getFloat("between_gap"));
//	meshDoc()->p_setting.gd_unit = (slice_program_setting::grid_unit)dummyParam.getEnum("grid_unit");
//	Label_Float temp = meshDoc()->p_setting.getbuildheight();
//	temp.value = dummyParam.getFloat("slice_Height");
//	meshDoc()->p_setting.setbuildheight(temp);
//	meshDoc()->p_setting.set_print_or_not(dummyParam.getBool("start_print"));
//	meshDoc()->p_setting.set_useless_print(dummyParam.getFloat("useless_print"));
//	meshDoc()->p_setting.print_pagesize = (slice_program_setting::page_size)dummyParam.getEnum("print_size");
//
//
//	//QSettings settings;
//
//	//settings.setValue("recentFileList", files);
//
//
//	/*qDebug() << dummyParam.getFloat("groove_length");
//	qDebug() << dummyParam.getFloat("groove_width");
//	qDebug() << dummyParam.getFloat("groove_height");*/
//
//
//
//	currentViewContainer()->updateAllViewer();
//
//}

void MainWindow::startsetting2()//Test
{
	//GLint x[2];
	//glGetIntegerv(GL_SMOOTH_LINE_WIDTH_RANGE, x);
	//QMessageBox::information(this, "Info", "You selected the file '" + fileName + "'");

	assert(meshDoc());
	qDebug() << "startsetting";
	RichParameterSet dummyParam;
	float w = meshDoc()->groove.groovewidth;
	float l = meshDoc()->groove.groovelength;
	float h = meshDoc()->groove.grooveheight;//***建構槽大小

	/*float t_g = ;
	float b_g = ;
	float r_g = ;
	float l_g = ;
	float bt_g = ;*/

	const QString DEFAULT_VERTICES_LIMIT("vertices_limit");


	QSettings settings;
	//init psetting value to tempSaveParSet2
	RichParameterSet tempSaveParSet2;
	meshDoc()->p_setting.initValue(tempSaveParSet2);

	for (int ii = 0; ii < tempSaveParSet2.paramList.size(); ++ii)//save parameter to register
	{
		RichParameterCopyConstructor v;
		tempSaveParSet2.paramList.at(ii)->accept(v);
		QDomDocument doc("someSetting");
		RichParameterXMLVisitor vxml(doc);
		v.lastCreated->accept(vxml);//create element attribute in here
		doc.appendChild(vxml.parElem);
		QString docstring = doc.toString();
		QSettings setting;
		setting.setValue(v.lastCreated->name, QVariant(docstring));
	}

	///////////////load register value
	RichParameterSet tempSaveParSet;
	QStringList klist = settings.allKeys();
	for (int ii = 0; ii < klist.size(); ++ii)//load from registor
	{
		QDomDocument doc;
		doc.setContent(settings.value(klist.at(ii)).toString());
		QString st = settings.value(klist.at(ii)).toString();
		qDebug() << "mainwindowInit_st" << klist.at(ii) << st;
		QDomElement docElem = doc.firstChild().toElement();
		if (!docElem.isNull()) {
			qDebug() << "docElem.tagName()" << docElem.tagName(); // the node really is an element.
		}

		RichParameter* rpar = NULL;
		if (!docElem.isNull())
		{
			bool ret = RichParameterFactory::create(docElem, &rpar);//***在這裡用RichParameterFactory::create 傳入QDomElement。
			if (!ret)
			{
				//  qDebug("Warning Ignored parameter '%s' = '%s'. Malformed.", qPrintable(docElem.attribute("name")),qPrintable(docElem.attribute("value")));
				continue;
			}
			if (!tempSaveParSet2.hasParameter(rpar->name))
			{
				//  qDebug("Warning Ignored parameter %s. In the saved parameters there are ones that are not in the HardWired ones. "
				//         "It happens if you are running MeshLab with only a subset of the plugins. ",qPrintable(rpar->name));
			}
			else tempSaveParSet.addParam(rpar);//***把參數加入currentGlobalParams
		}
	}
	//***********************

	int Vertices = settings.value(DEFAULT_VERTICES_LIMIT).toInt();

	if (Vertices == 0)Vertices = 30000000;

	//dummyParam.addParam(new RichFloat("groove_length", l, "x", ""));
	//dummyParam.addParam(new RichFloat("groove_height", h, "y", ""));
	//dummyParam.addParam(new RichFloat("groove_width", w, "z", ""));
	//dummyParam.addParam(new RichInt("Vertices_Number", Vertices, "Vertices Number", "bigger_use_Point_Draw"));	


	//dummyParam.addParam(new RichFloat("top_gap", meshDoc()->p_setting.getTopGap(), "top_gap", ""));
	//dummyParam.addParam(new RichFloat("bottom_gap", meshDoc()->p_setting.getBottom_Gap(), "bottom_gap", ""));
	//dummyParam.addParam(new RichFloat("right_gap", meshDoc()->p_setting.getRight_Gap(), "right_gap", ""));
	//dummyParam.addParam(new RichFloat("left_gap", meshDoc()->p_setting.getLeft_Gap(), "left_gapr", ""));
	//dummyParam.addParam(new RichFloat("between_gap", meshDoc()->p_setting.getBetweenGap(), "between_gap", ""));
	//dummyParam.addParam(new RichString("dir", meshDoc()->p_setting.getPicDir(), "dir", ""));
	//dummyParam.addParam(new RichEnum("grid_unit", 0, QStringList() << "centimeter" << "inch", tr("grid_unit"), QString("")));//*
	//dummyParam.addParam(new RichFloat("slice_Height", meshDoc()->p_setting.getbuildheight().value, "slice_height", ""));//*
	//dummyParam.addParam(new RichBool("start_print", meshDoc()->p_setting.get_print_or_not(), "start_print", ""));
	//dummyParam.addParam(new RichFloat("useless_print", meshDoc()->p_setting.get_useless_print(), "useless_print", ""));
	//dummyParam.addParam(new RichEnum("print_size", meshDoc()->p_setting.print_pagesize, QStringList() << "printBed_size" << "long_page_size", tr("print_size"), QString("")));




	genStdialog = new GenericParamDialog(this, &tempSaveParSet);

	genStdialog->exec();
	meshDoc()->groove.setLWH(dummyParam.getFloat("groove_length"), dummyParam.getFloat("groove_width"), dummyParam.getFloat("groove_height"));
	//vtNumberLimit = ;//大於多少點用drawpoint顯示
	//setVtNumberLimit(dummyParam.getInt("Vertices_Number"));

	//save value to register
	settings.setValue(DEFAULT_VERTICES_LIMIT, dummyParam.getInt("Vertices_Number"));



	meshDoc()->p_setting.setTopGap(dummyParam.getFloat("top_gap"));
	meshDoc()->p_setting.setBottomGap(dummyParam.getFloat("bottom_gap"));
	meshDoc()->p_setting.setRightGap(dummyParam.getFloat("right_gap"));
	meshDoc()->p_setting.setLeftGap(dummyParam.getFloat("left_gap"));
	meshDoc()->p_setting.setBetweenGap(dummyParam.getFloat("between_gap"));
	meshDoc()->p_setting.gd_unit = (slice_program_setting::grid_unit)dummyParam.getEnum("grid_unit");
	Label_Float temp = meshDoc()->p_setting.getbuildheight();
	temp.value = dummyParam.getFloat("slice_Height");
	meshDoc()->p_setting.setbuildheight(temp);
	meshDoc()->p_setting.set_print_or_not(dummyParam.getBool("start_print"));
	meshDoc()->p_setting.set_useless_print(dummyParam.getFloat("useless_print"));
	meshDoc()->p_setting.print_pagesize = (slice_program_setting::page_size)dummyParam.getEnum("print_size");




	//QSettings settings;

	//settings.setValue("recentFileList", files);


	/*qDebug() << dummyParam.getFloat("groove_length");
	qDebug() << dummyParam.getFloat("groove_width");
	qDebug() << dummyParam.getFloat("groove_height");*/




	currentViewContainer()->updateAllViewer();

}

void MainWindow::startsetting()
{
	//GLint x[2];
	//glGetIntegerv(GL_SMOOTH_LINE_WIDTH_RANGE, x);

	//QMessageBox::information(this, "Info", "You selected the file '" + fileName + "'");

	assert(meshDoc());
	qDebug() << "startsetting";

	QTranslator translator;
	translator.load(internationlization().toStdString().c_str());
	qApp->installTranslator(&translator);

	//Setting3DP *test = new Setting3DP(this, &currentGlobalParams);
	setting3DP2 = new Setting3DP(this, &currentGlobalParams);

	int settingResult = setting3DP2->exec();
	if (settingResult)
	{
		int x = currentGlobalParams.getEnum("TARGET_PRINTER");
		if (x == 0)
		{
			//350
			qWarning() << "threeDPrintAct visible ";
			threeDPrintAct->setVisible(true);
			genZXAct->setVisible(true);
			//352
			qWarning() << "printfunctionMono invisible ";
			printfunctionMono->setVisible(false);
			testDMSlicerAct->setVisible(false);

		}
		else if (x == 1)
		{
			//350
			threeDPrintAct->setVisible(false);
			genZXAct->setVisible(false);
			//mono 352
			printfunctionMono->setVisible(true);
			testDMSlicerAct->setVisible(true);
		}
	}


	/*if (undoLimit != currentGlobalParams.getInt("UNDO_LIMIT"))
	{
	QMessageBox msg;
	msg.setIcon(QMessageBox::Warning);
	msg.setWindowTitle(tr("Undo Limit Setting"));
	msg.setText(tr("Changing the undo limit will delete all previous actions."));
	msg.setInformativeText(tr("Do you want to proceed?"));
	msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msg.setDefaultButton(QMessageBox::No);
	int ret = msg.exec();

	if (ret == QMessageBox::Yes)
	{
	undoLimit = currentGlobalParams.getInt("UNDO_LIMIT");
	currentViewContainer()->undoStack->clear();
	currentViewContainer()->undoStack->setUndoLimit(undoLimit);
	}
	else
	{
	currentGlobalParams.setValue("UNDO_LIMIT", IntValue(undoLimit));
	}
	}*/

	updateCustomSettings();

	transformwidgett1->loadParameter();
	scale_widget_cus->loadParameter();
	rotate_widget_cus->loadParameter();
	updateMenus();

	QSettings settings;
	///////////////load register value
	RichParameterSet tempSaveParSet;
	//***********************
	const QString DEFAULT_VERTICES_LIMIT("vertices_limit");
	int Vertices = settings.value(DEFAULT_VERTICES_LIMIT).toInt();
	if (Vertices == 0)Vertices = 30000000;

	//RichParameterSet dummyParam;
	//float w = meshDoc()->groove.groovewidth;
	//float l = meshDoc()->groove.groovelength;
	//float h = meshDoc()->groove.grooveheight;//***建構槽大小

	//dummyParam.addParam(new RichFloat("groove_length", l, "x", ""));
	//dummyParam.addParam(new RichFloat("groove_height", h, "y", ""));
	//dummyParam.addParam(new RichFloat("groove_width", w, "z", ""));
	//dummyParam.addParam(new RichInt("Vertices_Number", Vertices, "Vertices Number", "bigger_use_Point_Draw"));


	//dummyParam.addParam(new RichFloat("top_gap", meshDoc()->p_setting.getTopGap(), "top_gap", ""));
	//dummyParam.addParam(new RichFloat("bottom_gap", meshDoc()->p_setting.getBottom_Gap(), "bottom_gap", ""));
	//dummyParam.addParam(new RichFloat("right_gap", meshDoc()->p_setting.getRight_Gap(), "right_gap", ""));
	//dummyParam.addParam(new RichFloat("left_gap", meshDoc()->p_setting.getLeft_Gap(), "left_gapr", ""));
	//dummyParam.addParam(new RichFloat("between_gap", meshDoc()->p_setting.getBetweenGap(), "between_gap", ""));
	//dummyParam.addParam(new RichString("dir", meshDoc()->p_setting.getPicDir(), "dir", ""));
	//dummyParam.addParam(new RichEnum("grid_unit", 0, QStringList() << "centimeter" << "inch", tr("grid_unit"), QString("")));//*
	//dummyParam.addParam(new RichFloat("slice_Height", meshDoc()->p_setting.getbuildheight().value, "slice_height", ""));//*
	//dummyParam.addParam(new RichBool("start_print", meshDoc()->p_setting.get_print_or_not(), "start_print", ""));
	//dummyParam.addParam(new RichFloat("useless_print", meshDoc()->p_setting.get_useless_print(), "useless_print", ""));
	//dummyParam.addParam(new RichEnum("print_size", meshDoc()->p_setting.print_pagesize, QStringList() << "printBed_size" << "long_page_size", tr("print_size"), QString("")));

	//


	//genStdialog = new GenericParamDialog(this, &dummyParam);

	//genStdialog->exec();
	//meshDoc()->groove.setLWH(dummyParam.getFloat("groove_length"), dummyParam.getFloat("groove_width"), dummyParam.getFloat("groove_height"));
	////vtNumberLimit = ;//大於多少點用drawpoint顯示
	//setVtNumberLimit(dummyParam.getInt("Vertices_Number"));

	////save value to register
	//settings.setValue(DEFAULT_VERTICES_LIMIT, dummyParam.getInt("Vertices_Number"));



	//meshDoc()->p_setting.setTopGap(dummyParam.getFloat("top_gap"));
	//meshDoc()->p_setting.setBottomGap(dummyParam.getFloat("bottom_gap"));
	//meshDoc()->p_setting.setRightGap(dummyParam.getFloat("right_gap"));
	//meshDoc()->p_setting.setLeftGap(dummyParam.getFloat("left_gap"));
	//meshDoc()->p_setting.setBetweenGap(dummyParam.getFloat("between_gap"));
	//meshDoc()->p_setting.gd_unit = (slice_program_setting::grid_unit)dummyParam.getEnum("grid_unit");
	//Label_Float temp = meshDoc()->p_setting.getbuildheight();
	//temp.value = dummyParam.getFloat("slice_Height");
	//meshDoc()->p_setting.setbuildheight(temp);
	//meshDoc()->p_setting.set_print_or_not(dummyParam.getBool("start_print"));
	//meshDoc()->p_setting.set_useless_print(dummyParam.getFloat("useless_print"));
	//meshDoc()->p_setting.print_pagesize = (slice_program_setting::page_size)dummyParam.getEnum("print_size");



	QString unitS;
	if (currentGlobalParams.getEnum("DISPLAY_UNIT") == 0)
		unitS = QString("Unit : mm");
	else if (currentGlobalParams.getEnum("DISPLAY_UNIT") == 1)
		unitS = QString("Unit :Inch ");
	unitStatus->setText(unitS);
	statusBar()->update();



	currentViewContainer()->updateAllViewer();

}





//***20150825//outline2D 預覽圖
void MainWindow::setTopTagView(bool toggle)
{
	MultiViewer_Container*  mvcont = currentViewContainer();
	//GLArea* tempGLArea = mvcont->getViewer(1);//***viewer0
	GLArea* tempGLArea = mvcont->getViewer(0);//***viewer0	
	if (toggle) {
		tempGLArea->templockTrackmouse = tempGLArea->lockTrackmouse;
		tempGLArea->tempVD = tempGLArea->vd;
		tempGLArea->setFov(6);
		tempGLArea->vd = GLArea::viewDirection::topTag;
		tempGLArea->lockTrackmouse = true;//backup_20160301
		//tempGLArea->lockTrackmouse = false;
		tempGLArea->resetTrackBall();
		tempGLArea->update();
		view_2dCB->setChecked(true);





		emit dockTabWidget->setCurrentIndex(1);
		//***20151005***執行slice_tool
		//showLayerToolButton->click();//***20160309註解掉，不執行按slice_tool按鈕功能
	}
	else
	{
		//tempGLArea = mvcont->getViewer(1);//***viewer1	
		tempGLArea = mvcont->getViewer(0);//***viewer1	
		tempGLArea->setFov(5);
		if (int(tempGLArea->tempVD) >= 0 && int(tempGLArea->tempVD) < 5)
		{
			tempGLArea->vd = tempGLArea->tempVD;// GLArea::viewDirection::original;
			tempGLArea->lockTrackmouse = tempGLArea->templockTrackmouse;
		}
		tempGLArea->resetTrackBall();//***不知道為什麼fov的設置會跑掉//20150804***不知道什哪裡會出bug
		view_2dCB->setChecked(false);
	}
}

//bool MainWindow::printFunction()
//{
//	
//	//***20150901****選擇檔案
//	////fileNameList.clear();
//	////fileNameList2.clear();
//	////fileNameList = QFileDialog::getOpenFileNames(this, tr("select_Print_Image"), lastUsedDirectory.path(), tr("ploy model(*.jpg *.bmp *.png)"));
//	//////fileNameList = QFileDialog::getOpenFileNames(this, tr("select_Print_Image"), lastUsedDirectory.path(), tr("All files(*.*)"));
//
//	////if (!fileNameList.empty())
//	////{
//	////	qDebug() << "selectfile" << fileNameList;
//	////	q2 = new QImage(fileNameList.first());
//	////}
//	////else return false;
//
//	//***自動找到圖片
//	//fill_fileNameList("D:/temptemp/xyzimage", "CapImageFileName", fileNameList, meshDoc()->bbox().DimZ() * 200);
//	//fill_fileNameList("D:/temptemp/xyzimage", "ImageFileName", fileNameList2, meshDoc()->bbox().DimZ() * 200);
//	fill_fileNameList(meshDoc()->p_setting.getPicDir(), meshDoc()->p_setting.getcapImageName(), fileNameList);
//	fill_fileNameList(meshDoc()->p_setting.getPicDir(), meshDoc()->p_setting.getoutlineName(), fileNameList2);
//
//
//	//*
//	QPrinter printer(QPrinter::HighResolution);
//	//printer.setPageSize(QPrinter::Custom);
//	printer.setPageSize(QPrinter::A4);
//	printer.setFullPage(true);
//
//	//QString outputpdf = "output.pdf";
//	//printer.setOutputFileName(outputpdf);
//	//printer.setOutputFormat(QPrinter::PdfFormat); //you can use native format of system usin QPrinter::NativeFormat
//	if (!fileNameList.empty())
//	{
//		QPrintPreviewDialog printPreviewdialog(&printer, this);
//		printPreviewdialog.setWindowFlags(Qt::Window);
//		connect(&printPreviewdialog, SIGNAL(paintRequested(QPrinter *)), this, SLOT(printPainterFunction(QPrinter*)));
//		printPreviewdialog.exec();
//
//	}
//	else return false;
//	//*/
//	/*if (dialog.exec() == QDialog::Accepted && !fileNameList.empty())
//	{
//
//	QPainter painter(&printer); // create a painter which will paint 'on printer'.
//	//QSize size = q1->size();//image的width, height;
//	QRect painterViewport = painter.viewport();
//	painter.setRenderHint(QPainter::Antialiasing);
//	//size.scale(painterViewport.size(), Qt::KeepAspectRatio);//***把image 的size放大到painter的大小
//	//painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
//	painter.setWindow(painterViewport);//***
//	//QRectF target(0, 0, 9917, 14033);	//target
//	QRectF target(0, 0, painterViewport.width(), painterViewport.height());	//target
//	QRectF source(0, 0, q2->width(), q2->height());	//image source畫image上的範圍
//
//	//開始列印
//	for (int i = 0; i < fileNameList.size(); i++)
//	{
//	*q2 = QImage(fileNameList[i]);
//	painter.drawPixmap(target, QPixmap::fromImage(*q2), source);
//	//if (i < fileNameList.size() - 1)
//	printer.newPage();
//	}
//
//	painter.end();
//	}*/
//
//	
//
//	return true;
//
//}

//***20160107***backup
//bool MainWindow::printFunction()
//{
//
//	//******backup*************************
//	//QString ppath = "cd /d " + getRoamingDir();
//
//	//QStringList arguments;
//	////arguments << "/k" << "cd /d d:/temptemp/zx_file" << "&" << "usbsend.exe" << "output.zx";						   
//	//arguments << "/k" << ppath << "&&" << "usbsend.exe" << "output.zx";// << "&" << "exit";
//	//QProcess::startDetached("cmd", arguments);
//
//
//	//return true;
//	//***********************************************************************************
//
//	//***test20160316**
//	PrinterJobInfo *pji = new PrinterJobInfo(this, meshDoc());
//	pji->exec();
//	//***********************
//
//
//	
//	
//
//
//	/*maskSave *ms = new maskSave();
//	ms->exec();*/
//
//
//	return true;
//}
void MainWindow::printJobEstimate()//bool all or selected
{
	bool doublePrint_V2 = currentGlobalParams.getBool("STIFF_PRIN_V2");
	int doublePrint_V2Percent = currentGlobalParams.getInt("STIFF_PRINT_VALUE");

	//bool allorselected = true;
	PrintjobParam pjpGenZX;
	pjpGenZX.setCommonPrintValue(meshDoc());
	pjpGenZX.setGenZxParam(false);
	pjpGenZX.setMono(false);
	pjpGenZX.setPrintStart(false);

	if (meshDoc()->multiSelectID.size() < 1)
	{
		foreach(MeshModel *mp, meshDoc()->meshList)
		{
			meshDoc()->multiSelectID.insert(mp->id());
		}
	}
	RichParameterSet tempTest;
	tempTest.copy(pjpGenZX.getprintJobParam());

	executeFilter(PM.actionFilterMap.value("FP_ESTIMATE_COLOR2"), tempTest, false);
	//executeFilter(PM.actionFilterMap.value("FP_ESTIMATE_SURFACE_COLOR"), tempTest, false);
	//
	//if ()
	/*cmyusage[0] = 0.;
	cmyusage[1] = 0.;
	cmyusage[2] = 0.;

	cmyusage[3] = 0.;
	cmyusage[4] = 0.;
	cmyusage[5] = 0.;
	cmyusage[6] = 0.;
	cmyusage[7] = 0.;
	cmyusage[8] = 0.;
	cmyusage[9] = 0.;
	cmyusage[10] = 0.;
	cmyusage[11] = 0.;*/
	cmyusage.clear();
	if (tempTest.hasParameter("CUsage") && tempTest.hasParameter("MaintYUsage"))
	{
		cmyusage.push_back(tempTest.getFloat("CUsage"));
		cmyusage.push_back(tempTest.getFloat("MUsage"));
		cmyusage.push_back(tempTest.getFloat("YUsage"));
	}
	if (tempTest.hasParameter("MaintYUsage") && true)
	{
		cmyusage.push_back(tempTest.getFloat("MaintCUsage"));
		cmyusage.push_back(tempTest.getFloat("MaintMUsage"));
		cmyusage.push_back(tempTest.getFloat("MaintYUsage"));

		cmyusage.push_back(tempTest.getFloat("MaintC_Spitton"));
		cmyusage.push_back(tempTest.getFloat("MaintM_Spitton"));
		cmyusage.push_back(tempTest.getFloat("MaintY_Spitton"));

		cmyusage.push_back(tempTest.getFloat("MaintC_Wiper"));
		cmyusage.push_back(tempTest.getFloat("MaintM_Wiper"));
		cmyusage.push_back(tempTest.getFloat("MaintY_Wiper"));

		cmyusage.push_back(tempTest.getFloat("MaintC_uselessPrint"));
		cmyusage.push_back(tempTest.getFloat("MaintM_uselessPrint"));
		cmyusage.push_back(tempTest.getFloat("MaintY_uselessPrint"));

		cmyusage.push_back(tempTest.getFloat("MaintC_PrintUsage"));
		cmyusage.push_back(tempTest.getFloat("MaintM_PrintUsage"));
		cmyusage.push_back(tempTest.getFloat("MaintY_PrintUsage"));
	}

	//***end color_estimate_test


	//PrinterJobInfo *pji = new PrinterJobInfo(this, meshDoc(), cmyusage, PrinterJobInfo::printInfoMode::PrintOptionMode);
	PrinterJobInfo *pji = new PrinterJobInfo(this, meshDoc(), &currentGlobalParams, &cmyusage, PrinterJobInfo::printInfoMode::EstimateMode, doublePrint_V2);
	pji->exec();


}
void MainWindow::getCmyusage()
{
	WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("getCmyusage"));

	pause_button->setVisible(false);
	s_pushbutton->setVisible(false);

	//cmyusage[0] = 0.;
	//cmyusage[1] = 0.;
	//cmyusage[2] = 0.;

	PrintjobParam pjpGenZX;
	pjpGenZX.setCommonPrintValue(meshDoc());
	pjpGenZX.setGenZxParam(false);
	pjpGenZX.setMono(false);
	pjpGenZX.setPrintStart(false);
	pjpGenZX.setStartPrintEstimate(true);

	RichParameterSet tempTest;
	tempTest.copy(pjpGenZX.getprintJobParam());

	WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("getCmyusage"));
	executeFilter(PM.actionFilterMap.value("FP_ESTIMATE_COLOR2"), tempTest, false);
	cmyusage.clear();
	//executeFilter(PM.actionFilterMap.value("FP_ESTIMATE_SURFACE_COLOR"), tempTest, false);
	if (tempTest.hasParameter("CUsage") && tempTest.hasParameter("MaintYUsage"))
	{
		/*cmyusage[0] = tempTest.getFloat("CUsage");
		cmyusage[1] = tempTest.getFloat("MUsage");
		cmyusage[2] = tempTest.getFloat("YUsage");*/
		cmyusage.push_back(tempTest.getFloat("CUsage"));
		cmyusage.push_back(tempTest.getFloat("MUsage"));
		cmyusage.push_back(tempTest.getFloat("YUsage"));
		WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("getCmyusage"));
	}
	//return &cmyusage;
}
void MainWindow::emitconnect()
{
	/*qRegisterMetaType<TextAndNumber>("TextAndNumber");
	qRegisterMetaType<TextAndNumber>("TextAndNumber&");*/
	//connect(&otherT1, SIGNAL(sendMessage(TextAndNumber&)), gl.w, SLOT(showString(TextAndNumber&)));

	/*otherT1.start();
	qDebug() << "otherT1" << otherT1.currentThreadId();*/

	//otherT1.exit();
}
bool MainWindow::GenZXFunc()
{
	/*Widget w;
	w.show();*/
	bool Channel_control_mode = currentGlobalParams.getBool("CHANNEL_CONTROL");
	QString zxaFilename = QFileDialog::getSaveFileName(this, tr("Export to Printable File"), QDir::currentPath(), tr("ZXA File (*.zxa)"));
	//string tempZipFilename = zxFilename.toStdString().append("a");
	//ZipIO zipfile(tempZipFilename.c_str());
	//QFileInfo tempFileInfo2(zxFilename);
	//zipfile.appendZipFile(tempFileInfo2.completeBaseName().toStdString().append(".txt").c_str());
	////zipfile.writeBuffertoEntry(ssd.rdbuf(), ssd.rdbuf()->in_avail());
	//zipfile.closeEntry();
	//zipfile.closeZip();

	if (zxaFilename.isEmpty())return false;

	PrintjobParam pjpGenZX;
	pjpGenZX.setCommonPrintValue(meshDoc());
	pjpGenZX.setGenZxParam(true);
	pjpGenZX.genDebugPic(false);//generate_final_picture
	pjpGenZX.setMono(false);
	pjpGenZX.setPrintStart(false);
	pjpGenZX.setZXFilePath(zxaFilename);
	pjpGenZX.setColorProfile(currentGlobalParams.getString("COLOR_PROFILE"));

	pjpGenZX.setStiffPrint(currentGlobalParams.getBool("STIFF_PRINT"));
	pjpGenZX.setDilateBinder(currentGlobalParams.getBool("DILATE_BINDER"));
	pjpGenZX.setDialteBinderValue(currentGlobalParams.getInt("DILATE_BINDER_VALUE"));
	pjpGenZX.setColorBinding(currentGlobalParams.getBool("COLOR_BINDING"));




	//	RichParameterSet printParam;
	//	printParam.addParam(new RichInt("print_dpi", 300, "dpi", ""));
	//	printParam.addParam(new RichFloat("slice_height", meshDoc()->p_setting.getbuildheight().value, "height_cm", ""));
	//	//printParam.addParam(new RichFloat("slice_height", 0.1, "height_cm", ""));
	//
	//	printParam.addParam(new RichBool("OL_Image", 0, "save_Outline_Image", ""));//輪廓
	//	printParam.addParam(new RichBool("Cap_Image", 0, "save_cap_Image", ""));	//膠水圖
	//
	//	printParam.addParam(new RichBool("BLK_OUT_LINE", 0, "BLK_OUT_LINE", ""));
	//
	//
	//	printParam.addParam(new RichBool("FaceColor", true, "FaceColor", ""));
	//
	//	printParam.addParam(new RichFloat("useless_print", 0, "useless_print_des", ""));//犧牲條寬度
	//	printParam.addParam(new RichBool("generate_zx", true, "generate_zx_or_not", ""));//
	//	printParam.addParam(new RichBool("generate_final_picture", 1, "generate_final_picture", ""));//
	//	printParam.addParam(new RichBool("mono_bool", false, "mono_bool", ""));//***20160322mono_bool
	//
	//
	//	printParam.addParam(new RichInt("start_page", 0, "start print pages"));
	//
	//	int boxy_dim = qCeil((meshDoc()->bbox().max.Z() - meshDoc()->bbox().min.Z()) / meshDoc()->p_setting.getbuildheight().value) + 1;
	//	printParam.addParam(new RichInt("end_page", boxy_dim, "end print pages"));
	//
	//
	//	printParam.addParam(new RichInt("per_zx_job", 10000, "number of zx pages", ""));//多少頁一個job
	//	printParam.addParam(new RichInt("send_page_num", 5, "send_page_num"));//幾頁send 一次
	//
	//	printParam.addParam(new RichBool("longpage_orshortpage", 0, "longpage_orshortpage", ""));
	//	qDebug() << "meshDoc()->p_setting.print_pagesize" << meshDoc()->p_setting.print_pagesize;
	//	if (meshDoc()->p_setting.print_pagesize == slice_program_setting::page_size::longPage_size) {		
	//#if 0
	//		//bb2
	//		printParam.addParam(new RichInt("plus_pixel", 2126, "plus_pixel"));//(40-22)*300/DSP_inchmm
	//		printParam.addParam(new RichFloat("PLUS_MM", 18, "PLUS_MM"));//40-22//steady function
	//#else
	//		
	//		int plus_pixel_val = meshDoc()->p_setting.get_plus_print_length()*300/DSP_inchmm;
	//		float plus_cm_val = meshDoc()->p_setting.get_plus_print_length();
	//		printParam.addParam(new RichInt("plus_pixel", plus_pixel_val, "plus_pixel"));//134*300/DSP_inchmm
	//		printParam.addParam(new RichFloat("PLUS_MM", plus_cm_val, "PLUS_MM"));//134
	//
	//#endif
	//
	//
	//		printParam.setValue("longpage_orshortpage", BoolValue(true));
	//	}
	//	else if (meshDoc()->p_setting.print_pagesize == slice_program_setting::page_size::printBed_size) {
	//
	//		printParam.addParam(new RichInt("plus_pixel", 0, "plus_pixel"));
	//		printParam.addParam(new RichFloat("PLUS_MM", 0, "PLUS_MM"));
	//		printParam.setValue("longpage_orshortpage", BoolValue(false));
	//	}
	//
	//	printParam.addParam(new RichBool("PRINT_START", false, "START PRINTING", ""));
	//	printParam.addParam(new RichFloat("USELESS_PRINT", meshDoc()->p_setting.get_useless_print(), "useless_print", ""));


	//openDashboard(true);
	qb->show();
	if (!Channel_control_mode)
		//executeFilter(PM.actionFilterMap.value("Print_Flow_2"), pjpGenZX.getprintJobParam(), false);
		executeFilter(PM.actionFilterMap.value("FP_PRINT_FLOW_2_V3"), pjpGenZX.getprintJobParam(), false);
	else
		executeFilter(PM.actionFilterMap.value("FP_PRINT_FOUR_IN_ONE"), pjpGenZX.getprintJobParam(), false);

	/*RichParameterSet tt;
	executeFilter(PM.actionFilterMap.value("count_hole"), tt, false);
	int x = tt.getInt("holeNum");

	if (tt.getInt("holeNum") > 0)
	{
	QMessageBox mb(QMessageBox::NoIcon, "Mesh contain Hole!", "Mesh contain Hole! \nPrint may not successful !\nStill want to continue? ", QMessageBox::NoButton, this);
	QPushButton *ok = mb.addButton("OK", QMessageBox::AcceptRole);
	mb.addButton(QMessageBox::Cancel);
	//mb.setInformativeText(tr( "Press button to retry connect printer\n") );
	mb.exec();

	if (mb.clickedButton() == ok)
	{
	pause_button->setVisible(true);
	s_pushbutton->setVisible(true);
	qb->show();
	//executeFilter(PM.actionFilterMap.value("Print_Flow_2"), printParam, false);
	executeFilter(PM.actionFilterMap.value("Print_Flow_2"), pjpGenZX.getprintJobParam(), false);
	}
	else return 0;
	}
	else
	{
	pause_button->setVisible(true);
	s_pushbutton->setVisible(true);
	qb->show();
	executeFilter(PM.actionFilterMap.value("Print_Flow_2"), pjpGenZX.getprintJobParam(), false);
	}*/
	pause_button->setVisible(false);
	s_pushbutton->setVisible(false);


	/*qb->show();
	executeFilter(PM.actionFilterMap.value("Print_Flow_2"), printParam, false);*/


	/*
	gen dynamic info
	*/
	//stringstream ssd;
	//std::stringbuf *pbuf = ssd.rdbuf();
	//string delimiter = "=";
	////ssd << "name=" << zxFilename.toStdString();
	//QMap<QString, QString>tempQMap;
	//GenPrintInfo::genPrinterInfo(currentGlobalParams,tempQMap);
	//QMapIterator<QString, QString> i(tempQMap);
	//while (i.hasNext()) {
	//	i.next();
	//	ssd << i.key().toStdString() << delimiter << i.value().toStdString() << "\n";
	//}
	//QFileInfo tempFileInfo(zxaFilename);
	//QString strNewName = tempFileInfo.path() + "/" + tempFileInfo.completeBaseName() + ".txt";
	////SKT::WriteToAFile<bool>(ssd, strNewName.toStdString());
	//const std::string tmp = ssd.str();
	//int ttt = tmp.size();
	////int xd = pbuf->in_avail();

	//string tempZipFilename = zxaFilename.toStdString();
	//ZipIO zipfile(tempZipFilename.c_str());
	//zipfile.appendZipFile(tempFileInfo.completeBaseName().toStdString().append(".txt").c_str());
	//zipfile.writeBuffertoEntry(tmp.c_str(), tmp.size());
	//zipfile.closeEntry();
	//zipfile.closeZip();
	/*
	********
	*/
	/*=7zip======*/

	QString zxtemp = PicaApplication::getRoamingDir();
	QDir zx_dir(zxtemp);
	zx_dir.setPath(zxtemp + "/zx_file");



	/*
	gen dynamic info
	*/
	PrinterJobInfo *pjii = new PrinterJobInfo(this, meshDoc(), &currentGlobalParams);

	stringstream ssd;
	std::stringbuf *pbuf = ssd.rdbuf();
	string delimiter = "=";
	//ssd << "name=" << zxFilename.toStdString();

	QMap<QString, QString>tempQMap;
	GenPrintInfo::genPrinterInfo(currentGlobalParams, tempQMap);
	QMapIterator<QString, QString> i(tempQMap);
	while (i.hasNext()) {
		i.next();
		ssd << i.key().toStdString() << delimiter << i.value().toStdString() << "\n";
	}


	QFileInfo tempFileInfo(zxaFilename);
	QString strNewName = zx_dir.absolutePath() + "/" + tempFileInfo.completeBaseName() + ".txt";
	SKT::WriteToAFile<bool>(ssd, strNewName.toStdString());
	const std::string tmp = ssd.str();
	int ttt = tmp.size();

	QFileInfoList filenin_zx_file = zx_dir.entryInfoList(QStringList() << "*.zx" << "*.txt");

	QProcess process;
	/*extract file*/
	QStringList arguments;
	arguments.append("a");
	arguments.append(zxaFilename);
	foreach(QFileInfo info, filenin_zx_file)
		arguments.append(info.absoluteFilePath());
	int iReturnCode = QProcess::execute("7z", arguments);


	/*=============================*/

	/*
	gen dynamic info
	*/
	//stringstream ssd;
	//std::stringbuf *pbuf = ssd.rdbuf();
	//string delimiter = "=";
	////ssd << "name=" << zxFilename.toStdString();
	//QMapIterator<int, int> i(finalDynamicPage);	
	//while (i.hasNext()) {
	//	i.next();
	//	ssd << i.key() << delimiter << i.value() << "\n";
	//}
	//QFileInfo tempFileInfo(zxaFilename);
	//QString strNewName = tempFileInfo.path() + "/" + tempFileInfo.completeBaseName() + ".txt";
	////SKT::WriteToAFile<bool>(ssd, strNewName.toStdString());
	//const std::string tmp = ssd.str();
	//int ttt = tmp.size();
	////int xd = pbuf->in_avail();

	//string tempZipFilename = zxaFilename.toStdString();
	//ZipIO zipfile(tempZipFilename.c_str());
	//zipfile.appendZipFile(tempFileInfo.completeBaseName().toStdString().append(".txt").c_str());
	//zipfile.writeBuffertoEntry(tmp.c_str(), tmp.size());
	//zipfile.closeEntry();
	//zipfile.closeZip();
	/*
		********
		*/

	return true;

}
bool MainWindow::printFunctionMono()//deprecated
{
	bool Channel_control_mode = currentGlobalParams.getBool("CHANNEL_CONTROL");
	detectOverlappingFunc();
	if (meshDoc()->getIntersectMeshesID2()->size() > 0)
	{
		QMessageBox mb(QMessageBox::NoIcon, "Meshes are overlapping!", "Meshes are overlapping! \nStill want to print? ", QMessageBox::NoButton, this);
		QPushButton *ok = mb.addButton("OK", QMessageBox::AcceptRole);
		mb.addButton(QMessageBox::Cancel);
		//mb.setInformativeText(tr( "Press button to retry connect printer\n") );
		mb.exec();

		if (mb.clickedButton() == ok);
		else
		{
			//openDashboard(true);
			return false;
		}
	}


	//==language=//
	QTranslator translator;
	translator.load(internationlization().toStdString().c_str());
	qApp->installTranslator(&translator);



	JobEstimateDialog *estimateBox = new JobEstimateDialog();
	int ret = estimateBox->exec();



	//QMessageBox estimateBox(QMessageBox::NoIcon, "Estimate Usage", "Do you want to run the pre-printing estimation now?? \n It is crucial to know that you have sufficient printing material before starting the actual print."/*, QMessageBox::Ok | QMessageBox::No*/);
	//QPushButton *ok = estimateBox.addButton("Estimate Now", QMessageBox::AcceptRole);
	//estimateBox.addButton("Skip",QMessageBox::NoRole);
	//int ret = estimateBox.exec();
	//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("printFunction"));
	if (ret)
	{
		//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("printFunction"));
		getCmyusage();
		//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("printFunction"));
	}
	else{
		return false;
		//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("printFunction"));
	}
	//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("printFunction"));

	PrintOption printOption_(PrintOption::optionMode::NORMAL_PRINT_MODE, this, meshDoc());

	//float *cmyusage = getCmyusage();
	//QVector<float> *cmyusage = getCmyusage();

	//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("printFunction"));
	bool twoSidePrint = currentGlobalParams.getBool("STIFF_PRIN_V2");
	PrinterJobInfo *pjii = new PrinterJobInfo(this, meshDoc(), &currentGlobalParams, &cmyusage, PrinterJobInfo::printInfoMode::PrintOptionMode, twoSidePrint);
	//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("printFunction"));
	int startPage = currentGlobalParams.getInt("START_PRINT_PAGE");
	if (startPage != 0)pjii->setSMLayers(startPage);
	//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("printFunction"));
	printOption_.updatePrintSummaryLabel(pjii->getPJI());
	printOption_.exec();
	//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("printFunction"));

	int wiperFrequency = currentGlobalParams.getInt("MIDJOB_FREQUENCY");//printOption_.getWiperFrequency();
	int wiperIndex = currentGlobalParams.getInt("WIPER_INDEX"); //printOption_.getWiperIndex();
	int wiperClick = currentGlobalParams.getInt("WIPER_CLICK"); //printOption_.getWiperClick();
	//int spittonMode = printOption_.getSpittonData();

	QVector<int> tempHeatParam;
	printOption_.getHeatStuffParam(tempHeatParam);
	tempHeatParam.replace(4, currentGlobalParams.getInt("PP_POST_HEATING_MINUTES"));
	tempHeatParam.replace(5, currentGlobalParams.getInt("PP_POST_HEATING_UPPER_LIMIT"));
	tempHeatParam.replace(6, currentGlobalParams.getInt("PP_POST_HEATING_LOWER_LIMIT"));
	PrintjobParam pjpGenZX;
	pjpGenZX.setCommonPrintValue(meshDoc());
	pjpGenZX.setGenZxParam(true);
	pjpGenZX.genDebugPic(false);//Test
	pjpGenZX.setMono(printOption_.getMonoParam());
	pjpGenZX.setPrintStart(true);
	pjpGenZX.setStartPrintEstimate(true);
	pjpGenZX.setColorProfile(currentGlobalParams.getString("COLOR_PROFILE"));
	pjpGenZX.setStiffPrint(currentGlobalParams.getBool("STIFF_PRINT"));
	pjpGenZX.setDilateBinder(currentGlobalParams.getBool("DILATE_BINDER"));
	pjpGenZX.setDialteBinderValue(currentGlobalParams.getInt("DILATE_BINDER_VALUE"));
	pjpGenZX.setDynamicWipe(currentGlobalParams.getBool("DYNAMIC_WIPE"));
	pjpGenZX.setColorBinding(currentGlobalParams.getBool("COLOR_BINDING"));


	int fanSpeed = currentGlobalParams.getInt("FAN_SPEED");
	int pumpValue = currentGlobalParams.getInt("PUMP_VALUE");
	bool dynamicSwitch = currentGlobalParams.getBool("DYNAMIC_WIPE");
	bool layerPrintMode = currentGlobalParams.getBool("STIFF_PRIN_V2");
	pause_button->setVisible(false);
	s_pushbutton->setVisible(false);

	std::string s_result;
	std::string cmdd = "GET /maint/pump_while_print?rpm=";// 65 HTTP / 1.1\r\n\r\n";
	cmdd.append(std::to_string(pumpValue));
	cmdd.append(" HTTP/1.1\r\n\r\n");
	comm->sendSCICommand(cmdd, s_result);

	/*
	======
	*/
	switch (1)
	{
	case 0:
	{
		/*WidgetMonitor widget_p_monitor(printParam);
		widget_p_monitor.exec();
		if (widget_p_monitor.result() == QDialog::Accepted)
		{
		qb->show();
		executeFilter(PM.actionFilterMap.value("Print_Flow_2"), printParam, false);
		}

		qb->reset();
		int nextfile = 0;*/


		//sendZXfile(meshDoc()->jobname.at(nextfile));
	}
	break;
	case 1:
	{
		if (printOption_.result() || DSP_TEST_MODE)
			//if (true)
		{//backup
			/*
			count mesh top pages
			*/
			recordRollerPage();
			QString rollerCMDToServer = SKT::createJsonStringCommand("Other", "Mesh_Top", rollerPageToServer);
			comm->SendJSONCmd(rollerCMDToServer.toStdString());


			CheckListWidget  checkListWidget(&pjii->getPJI());
			comm->PreAndPrintingHeated(tempHeatParam.at(0), tempHeatParam.at(1), tempHeatParam.at(2), tempHeatParam.at(3));
			comm->PostHeated(tempHeatParam.at(4), tempHeatParam.at(5), tempHeatParam.at(6));

			bool setsuc = comm->setDynamicMidjob_Mode(1);//mode 0 midjob every 4, 1 ,2 send by comm
			setsuc = comm->setDynamicMidjob_PageInterval(wiperFrequency);//mode 0 midjob every 4, 1 ,2 send by comm
			QString setsucs = "setDynamicMidjob_Mode : " + QString::number(setsuc) + "\n";
			fprintf(dbgff, setsucs.toStdString().c_str()); //@@@
			fflush(dbgff);

			comm->WiperIndex(wiperIndex);
			comm->WiperClick(wiperClick);

			/*
			send Roller_stop_page
			*/

			if (!layerPrintMode)
				comm->PrintMode(1);
			else
				comm->PrintMode(2);
			checkListWidget.exec();
			if (checkListWidget.result() == QDialog::Accepted || DSP_TEST_MODE)
				//if (true)
			{
				//assert(checkListWidget.sendHeader(pjii->getPJI()));
				pjii->genCSVFile();


				bool headt = checkListWidget.sendHeader(pjii->getPJI(), fanSpeed, pjpGenZX.getprintJobParam().getFloat("slice_height"));
				if (headt || DSP_TEST_MODE)
				{
					//QMessageBox::information(this, "Info", "header transfer successful");
					int waitTime = 0;
					while (true && (!DSP_TEST_MODE))
					{
						Sleep(500);
						std::string s, e;
						comm->printerStatus(s, e);

						QString temp = QString::fromStdString(s);
						fprintf(dbgff, temp.toStdString().c_str()); //@@@
						fflush(dbgff);
						if (temp == "Printing")
							//if (temp == "Online")
						{

							break;
						}
						waitTime += 500;
						if (waitTime > 600000)
						{
							QMessageBox::information(this, "Info", "Timeout");
							return false;
						}

					}
					openDashboard(true);
					qb->show();

					executeFilter(PM.actionFilterMap.value("FP_PRINT_DM_SLICER"), pjpGenZX.getprintJobParam(), false);



				}
				else
				{
					QMessageBox::information(this, "Info", "Printer header transfer false");
					/*qb->show();
					executeFilter(PM.actionFilterMap.value("Print_Flow_2"), printParam, false);*/
					return false;
				}
			}
			qb->reset();
			delOl(MeshModel::meshsort::slice_item);
		}
		//checkListWidget.~CheckListWidget();
	}
	break;
	}

	return true;

}
bool MainWindow::printFunction352()//
{


	bool Channel_control_mode = currentGlobalParams.getBool("CHANNEL_CONTROL");
	detectOverlappingFunc();
	if (meshDoc()->getIntersectMeshesID2()->size() > 0)
	{
		QMessageBox mb(QMessageBox::NoIcon, "Meshes are overlapping!", "Meshes are overlapping! \nStill want to print? ", QMessageBox::NoButton, this);
		QPushButton *ok = mb.addButton("OK", QMessageBox::AcceptRole);
		mb.addButton(QMessageBox::Cancel);
		//mb.setInformativeText(tr( "Press button to retry connect printer\n") );
		mb.exec();

		if (mb.clickedButton() == ok);
		else
		{
			//openDashboard(true);
			return false;
		}
	}


	//==language=//
	QTranslator translator;
	translator.load(internationlization().toStdString().c_str());
	qApp->installTranslator(&translator);


	JobEstimateDialog *estimateBox = new JobEstimateDialog();
	int ret = estimateBox->exec();



	if (ret)
	{
		//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("printFunction"));
		getCmyusage();
		//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("printFunction"));
	}
	else{
		//return false;

	}
	//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("printFunction"));

	PrintOption printOption_(PrintOption::optionMode::MONO_PRINT_MODE, this, meshDoc());


	bool layerPrintMode = currentGlobalParams.getBool("DM_TWICE_IN_ONE_LAYER");
	PrinterJobInfo *pjii = new PrinterJobInfo(this, meshDoc(), &currentGlobalParams, &cmyusage, PrinterJobInfo::printInfoMode::PrintOptionMode, layerPrintMode);

	int startPage = currentGlobalParams.getInt("START_PRINT_PAGE");
	if (startPage != 0)
		pjii->setSMLayers(startPage);
	//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("printFunction"));
	printOption_.updatePrintSummaryLabel(pjii->getPJI());
	printOption_.exec();


	int wiperFrequency = currentGlobalParams.getInt("MIDJOB_FREQUENCY");//printOption_.getWiperFrequency();
	int wiperIndex = currentGlobalParams.getInt("WIPER_INDEX"); //printOption_.getWiperIndex();
	int wiperClick = currentGlobalParams.getInt("WIPER_CLICK"); //printOption_.getWiperClick();
	//int spittonMode = printOption_.getSpittonData();

	bool postHeating = currentGlobalParams.getBool("PP_POST_HEATING_SWITCH");
	QVector<int> tempHeatParam;
	printOption_.getHeatStuffParam(tempHeatParam);
	if (postHeating)
		tempHeatParam.replace(4, currentGlobalParams.getInt("PP_POST_HEATING_MINUTES"));
	else
		tempHeatParam.replace(4, 0);
	tempHeatParam.replace(5, currentGlobalParams.getInt("PP_POST_HEATING_UPPER_LIMIT"));
	tempHeatParam.replace(6, currentGlobalParams.getInt("PP_POST_HEATING_LOWER_LIMIT"));
	PrintjobParam pjpGenZX;
	pjpGenZX.setCommonPrintValue(meshDoc());
	pjpGenZX.setGenZxParam(true);
	pjpGenZX.genDebugPic(false);//Test
	pjpGenZX.setMono(printOption_.getMonoParam());
	pjpGenZX.setPrintStart(true);
	pjpGenZX.setStartPrintEstimate(true);
	pjpGenZX.setColorProfile(currentGlobalParams.getString("COLOR_PROFILE"));
	pjpGenZX.setStiffPrint(currentGlobalParams.getBool("STIFF_PRINT"));
	pjpGenZX.setDilateBinder(currentGlobalParams.getBool("DILATE_BINDER"));
	pjpGenZX.setDialteBinderValue(currentGlobalParams.getInt("DILATE_BINDER_VALUE"));
	pjpGenZX.setDynamicWipe(currentGlobalParams.getBool("DYNAMIC_WIPE"));
	pjpGenZX.setColorBinding(currentGlobalParams.getBool("COLOR_BINDING"));


	int fanSpeed = currentGlobalParams.getInt("FAN_SPEED");
	int pumpValue = currentGlobalParams.getInt("PUMP_VALUE");
	bool dynamicSwitch = currentGlobalParams.getBool("DYNAMIC_WIPE");
	bool ir_On_Off_Setting = currentGlobalParams.getBool("IR_On_Off");
	pause_button->setVisible(false);
	s_pushbutton->setVisible(false);

	std::string s_result;
	std::string cmdd = "GET /maint/pump_while_print?rpm=";// 65 HTTP / 1.1\r\n\r\n";
	cmdd.append(std::to_string(pumpValue));
	cmdd.append(" HTTP/1.1\r\n\r\n");
	comm->sendSCICommand(cmdd, s_result);


	switch (1)
	{
	case 0:
	{

	}
	break;
	case 1:
	{
		if (printOption_.result() || DSP_TEST_MODE)
			//if (true)
		{//backup
			/*
			count mesh top pages
			*/
			recordRollerPage();
			QString rollerCMDToServer = SKT::createJsonStringCommand("Other", "Mesh_Top", rollerPageToServer);
			comm->SendJSONCmd(rollerCMDToServer.toStdString());

			QEventLoop loop;
			QTimer::singleShot(500, &loop, SLOT(quit()));
			loop.exec();

			QString irCommand;
			if (ir_On_Off_Setting)
				irCommand = SKT::createJsonStringCommand("FPGA", "@HTO", QStringList());
			else
				irCommand = SKT::createJsonStringCommand("FPGA", "@HTC", QStringList());
			comm->SendJSONCmd(irCommand.toStdString());


			CheckListWidget  checkListWidget(&pjii->getPJI());
			comm->PreAndPrintingHeated(tempHeatParam.at(0), tempHeatParam.at(1), tempHeatParam.at(2), tempHeatParam.at(3));
			comm->PostHeated(tempHeatParam.at(4), tempHeatParam.at(5), tempHeatParam.at(6));

			bool setsuc = comm->setDynamicMidjob_Mode(1);//mode 0 midjob every 4, 1 ,2 send by comm
			setsuc = comm->setDynamicMidjob_PageInterval(wiperFrequency);//mode 0 midjob every 4, 1 ,2 send by comm
			QString setsucs = "setDynamicMidjob_Mode : " + QString::number(setsuc) + "\n";
			fprintf(dbgff, setsucs.toStdString().c_str()); //@@@
			fflush(dbgff);

			comm->WiperIndex(wiperIndex);
			comm->WiperClick(wiperClick);

			/*
			send Roller_stop_page
			*/

			if (!layerPrintMode)
				comm->PrintMode(1);
			else
				comm->PrintMode(2);
			checkListWidget.exec();
			if (checkListWidget.result() == QDialog::Accepted || DSP_TEST_MODE)
				//if (true)
			{
				//assert(checkListWidget.sendHeader(pjii->getPJI()));
				pjii->genCSVFile();

				int xlayerHeight = (currentGlobalParams.getFloat("DM_MONO_LAYER_HEIGHT") * 10000 + 0.5);
				int layer = qCeil(meshDoc()->bbox().DimZ() / currentGlobalParams.getFloat("DM_MONO_LAYER_HEIGHT")) + 1;
				bool headt = comm->sendPrintjobHeader(layer, xlayerHeight, 'L', fanSpeed);

				//bool headt = checkListWidget.sendHeader(pjii->getPJI(), fanSpeed, xlayerHeight);

				if (headt || DSP_TEST_MODE)
				{
					//QMessageBox::information(this, "Info", "header transfer successful");
					int waitTime = 0;
					while (true && (!DSP_TEST_MODE))
					{
						Sleep(500);
						std::string s, e;
						comm->printerStatus(s, e);

						QString temp = QString::fromStdString(s);
						fprintf(dbgff, temp.toStdString().c_str()); //@@@
						fflush(dbgff);
						if (temp == "Printing")
							//if (temp == "Online")
						{

							break;
						}
						waitTime += 500;
						if (waitTime > 600000)
						{
							QMessageBox::information(this, "Info", "Timeout");
							return false;
						}

					}
					openDashboard(true);
					qb->show();
					if (!Channel_control_mode)
						executeFilter(PM.actionFilterMap.value("FP_PRINT_DM_SLICER_ALPHA"), pjpGenZX.getprintJobParam(), false);
					else
						//executeFilter(PM.actionFilterMap.value("FP_PRINT_FOUR_IN_ONE"), pjpGenZX.getprintJobParam(), false);
						executeFilter(PM.actionFilterMap.value("FP_PRINT_DM_SLICER_ALPHA"), pjpGenZX.getprintJobParam(), false);


				}
				else
				{
					QMessageBox::information(this, "Info", "Printer header transfer false");
					/*qb->show();
					executeFilter(PM.actionFilterMap.value("Print_Flow_2"), printParam, false);*/
					return false;
				}
			}
			qb->reset();
			delOl(MeshModel::meshsort::slice_item);
		}
		//checkListWidget.~CheckListWidget();
	}
	break;
	}

	return true;
}
bool MainWindow::printFunction()//
{
	/*thumbNail Test*/
	auto genThnumbnail = [&](){
		QString projectThumbnailName;
		if (!meshDoc()->getFileName().isEmpty())
		{
			QFileInfo prjectNameInfo(meshDoc()->getFileName());
			projectThumbnailName = BCPwareFileSystem::projectThumbnailFolder().filePath(QString("%1_%2.png").arg(prjectNameInfo.completeBaseName()).arg(QUuid::createUuid().toString().mid(1, 36)));

		}
		else
		{
			projectThumbnailName = BCPwareFileSystem::projectThumbnailFolder().filePath(QString("%1.png").arg(QUuid::createUuid().toString().mid(1, 36)));
		}

		PrintjobParam genPICParam;
		genPICParam.setCommonPrintValue(meshDoc());
		genPICParam.setGenZxParam(true);
		genPICParam.genDebugPic(false);//Test
		genPICParam.setThumbnailPath(projectThumbnailName);

		genPICParam.setPrintStart(false);
		genPICParam.setStartPrintEstimate(true);
		genPICParam.setColorProfile(currentGlobalParams.getString("COLOR_PROFILE"));
		genPICParam.setStiffPrint(currentGlobalParams.getBool("STIFF_PRINT"));
		genPICParam.setDilateBinder(currentGlobalParams.getBool("DILATE_BINDER"));
		genPICParam.setDialteBinderValue(currentGlobalParams.getInt("DILATE_BINDER_VALUE"));
		genPICParam.setDynamicWipe(currentGlobalParams.getBool("DYNAMIC_WIPE"));
		genPICParam.setColorBinding(currentGlobalParams.getBool("COLOR_BINDING"));
		executeFilter(PM.actionFilterMap.value("FP_SAVE_DEFAULT_SNAP_SHOT"), genPICParam.getprintJobParam(), false);
		//update Thumbnail file path
		ParamOp::updateToHistory(false, "JOB_THUMBNAIL_NAME", projectThumbnailName);
		//upadte project Name
		meshDoc()->getFileName().isEmpty() ? NULL : ParamOp::updateToHistory(false, "PROJECT_NAME", meshDoc()->getFileName());

		//GENERAL
		ParamOp::updateSliceSettingToHistory(false, 0, currentGlobalParams.getFloat("SLIGHT_HEIGHT"));
		ParamOp::updateSliceSettingToHistory(false, 1, currentGlobalParams.getBool("ADD_PATTERN"));
		ParamOp::updateSliceSettingToHistory(false, 2, currentGlobalParams.getFloat("PLUS_PRINT_LENGTH"));
		ParamOp::updateSliceSettingToHistory(false, 3, currentGlobalParams.getBool("LINE_OR_RECT"));
		////slice 
		ParamOp::updateSliceSettingToHistory(false, 4, currentGlobalParams.getBool("STIFF_PRIN_V2"));
		ParamOp::updateSliceSettingToHistory(false, 5, currentGlobalParams.getInt("STIFF_PRINT_VALUE"));
		ParamOp::updateSliceSettingToHistory(false, 6, currentGlobalParams.getInt("SHELL_PERCENT"));

		ParamOp::updateSliceSettingToHistory(false, 7, currentGlobalParams.getBool("DILATE_BINDER"));
		ParamOp::updateSliceSettingToHistory(false, 8, currentGlobalParams.getInt("DILATE_BINDER_VALUE"));
		ParamOp::updateSliceSettingToHistory(false, 9, currentGlobalParams.getFloat("PP_BOTTOM_LIGHTER_THICK"));
		ParamOp::updateSliceSettingToHistory(false, 10, currentGlobalParams.getInt("PP_BOTTOM_LIGHTER_PERCENTAGE"));
		ParamOp::updateSliceSettingToHistory(false, 11, currentGlobalParams.getFloat("HORIZONE_LINE_WIDTH"));
		ParamOp::updateSliceSettingToHistory(false, 12, currentGlobalParams.getFloat("VERTICAL_LINE_WIDTH"));

		ParamOp::updateSliceSettingToHistory(false, 13, currentGlobalParams.getBool("PROFILE_ON"));
		ParamOp::updateSliceSettingToHistory(false, 14, currentGlobalParams.getString("COLOR_PROFILE"));
		//spitton
		ParamOp::updateSliceSettingToHistory(false, 15, currentGlobalParams.getFloat("SPITTOON_M"));
		ParamOp::updateSliceSettingToHistory(false, 16, currentGlobalParams.getFloat("SPITTOON_C"));
		ParamOp::updateSliceSettingToHistory(false, 17, currentGlobalParams.getFloat("SPITTOON_Y"));
		ParamOp::updateSliceSettingToHistory(false, 18, currentGlobalParams.getFloat("SPITTOON_SECOND_CMY"));
		ParamOp::updateSliceSettingToHistory(false, 19, currentGlobalParams.getFloat("SPITTOON_SECOND_B"));
		//maintenance
		ParamOp::updateSliceSettingToHistory(false, 20, currentGlobalParams.getInt("FAN_SPEED"));
		ParamOp::updateSliceSettingToHistory(false, 21, currentGlobalParams.getBool("IR_On_Off"));
		ParamOp::updateSliceSettingToHistory(false, 22, currentGlobalParams.getInt("MIDJOB_FREQUENCY"));
		ParamOp::updateSliceSettingToHistory(false, 23, currentGlobalParams.getInt("WIPER_INDEX"));
		ParamOp::updateSliceSettingToHistory(false, 24, currentGlobalParams.getInt("WIPER_CLICK"));
		ParamOp::updateSliceSettingToHistory(false, 25, currentGlobalParams.getBool("PP_POST_HEATING_SWITCH"));
		ParamOp::updateSliceSettingToHistory(false, 26, currentGlobalParams.getInt("PP_POST_HEATING_MINUTES"));
		ParamOp::updateSliceSettingToHistory(false, 27, currentGlobalParams.getInt("PUMP_VALUE"));




	};
	//genThnumbnail();

	//return false;
	/**/
	bool Channel_control_mode = currentGlobalParams.getBool("CHANNEL_CONTROL");
	detectOverlappingFunc();
	if (meshDoc()->getIntersectMeshesID2()->size() > 0)
	{
		QMessageBox mb(QMessageBox::NoIcon, "Meshes are overlapping!", "Meshes are overlapping! \nStill want to print? ", QMessageBox::NoButton, this);
		QPushButton *ok = mb.addButton("OK", QMessageBox::AcceptRole);
		mb.addButton(QMessageBox::Cancel);
		//mb.setInformativeText(tr( "Press button to retry connect printer\n") );
		mb.exec();

		if (mb.clickedButton() == ok);
		else
		{
			//openDashboard(true);
			return false;
		}
	}


	//==language=//
	QTranslator translator;
	translator.load(internationlization().toStdString().c_str());
	qApp->installTranslator(&translator);


	//printOption_ = new PrintOption(PrintOption::optionMode::NORMAL_PRINT_MODE);	

	//QMessageBox estimateBox(QMessageBox::NoIcon, "Estimate Usage", "Do you want to estimate usage? \n ", QMessageBox::Ok | QMessageBox::No, this);
	//QMessageBox estimateBox(QMessageBox::NoIcon, "Estimate Usage", "Do you want to estimate usage? \n ", QMessageBox::Ok | QMessageBox::No);


	JobEstimateDialog *estimateBox = new JobEstimateDialog();
	int ret = estimateBox->exec();



	//QMessageBox estimateBox(QMessageBox::NoIcon, "Estimate Usage", "Do you want to run the pre-printing estimation now?? \n It is crucial to know that you have sufficient printing material before starting the actual print."/*, QMessageBox::Ok | QMessageBox::No*/);
	//QPushButton *ok = estimateBox.addButton("Estimate Now", QMessageBox::AcceptRole);
	//estimateBox.addButton("Skip",QMessageBox::NoRole);
	//int ret = estimateBox.exec();
	//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("printFunction"));
	if (ret)
	{
		//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("printFunction"));
		getCmyusage();
		//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("printFunction"));
	}
	else{
		//return false;
		//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("printFunction"));
	}
	//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("printFunction"));

	PrintOption printOption_(PrintOption::optionMode::NORMAL_PRINT_MODE, this, meshDoc());

	//float *cmyusage = getCmyusage();
	//QVector<float> *cmyusage = getCmyusage();

	//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("printFunction"));
	bool twoSidePrint = currentGlobalParams.getBool("STIFF_PRIN_V2");
	PrinterJobInfo *pjii = new PrinterJobInfo(this, meshDoc(), &currentGlobalParams, &cmyusage, PrinterJobInfo::printInfoMode::PrintOptionMode, twoSidePrint);
	//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("printFunction"));
	int startPage = currentGlobalParams.getInt("START_PRINT_PAGE");
	if (startPage != 0)pjii->setSMLayers(startPage);
	//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("printFunction"));
	printOption_.updatePrintSummaryLabel(pjii->getPJI());
	printOption_.exec();
	//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("printFunction"));

	int wiperFrequency = currentGlobalParams.getInt("MIDJOB_FREQUENCY");//printOption_.getWiperFrequency();
	int wiperIndex = currentGlobalParams.getInt("WIPER_INDEX"); //printOption_.getWiperIndex();
	int wiperClick = currentGlobalParams.getInt("WIPER_CLICK"); //printOption_.getWiperClick();
	//int spittonMode = printOption_.getSpittonData();

	bool postHeating = currentGlobalParams.getBool("PP_POST_HEATING_SWITCH");
	QVector<int> tempHeatParam;
	printOption_.getHeatStuffParam(tempHeatParam);
	if (postHeating)
		tempHeatParam.replace(4, currentGlobalParams.getInt("PP_POST_HEATING_MINUTES"));
	else
		tempHeatParam.replace(4, 0);
	tempHeatParam.replace(5, currentGlobalParams.getInt("PP_POST_HEATING_UPPER_LIMIT"));
	tempHeatParam.replace(6, currentGlobalParams.getInt("PP_POST_HEATING_LOWER_LIMIT"));
	PrintjobParam pjpGenZX;
	pjpGenZX.setCommonPrintValue(meshDoc());
	pjpGenZX.setGenZxParam(true);
	pjpGenZX.genDebugPic(false);//Test
	pjpGenZX.setMono(printOption_.getMonoParam());
	pjpGenZX.setPrintStart(true);
	pjpGenZX.setStartPrintEstimate(true);
	pjpGenZX.setColorProfile(currentGlobalParams.getString("COLOR_PROFILE"));
	pjpGenZX.setStiffPrint(currentGlobalParams.getBool("STIFF_PRINT"));
	pjpGenZX.setDilateBinder(currentGlobalParams.getBool("DILATE_BINDER"));
	pjpGenZX.setDialteBinderValue(currentGlobalParams.getInt("DILATE_BINDER_VALUE"));
	pjpGenZX.setDynamicWipe(currentGlobalParams.getBool("DYNAMIC_WIPE"));
	pjpGenZX.setColorBinding(currentGlobalParams.getBool("COLOR_BINDING"));


	int fanSpeed = currentGlobalParams.getInt("FAN_SPEED");
	int pumpValue = currentGlobalParams.getInt("PUMP_VALUE");
	bool dynamicSwitch = currentGlobalParams.getBool("DYNAMIC_WIPE");
	bool layerPrintMode = currentGlobalParams.getBool("STIFF_PRIN_V2");
	bool ir_On_Off_Setting = currentGlobalParams.getBool("IR_On_Off");
	pause_button->setVisible(false);
	s_pushbutton->setVisible(false);

	std::string s_result;
	std::string cmdd = "GET /maint/pump_while_print?rpm=";// 65 HTTP / 1.1\r\n\r\n";
	cmdd.append(std::to_string(pumpValue));
	cmdd.append(" HTTP/1.1\r\n\r\n");
	comm->sendSCICommand(cmdd, s_result);

	/*
	======
	*/
	//
	//QProgressDialog dialog;
	//dialog.setLabelText(QString("Progressing using %1 thread(s)...").arg(QThread::idealThreadCount()));
	//// Create a QFutureWatcher and connect signals and slots.
	//QFutureWatcher<void> futureWatcher;
	//QObject::connect(&futureWatcher, SIGNAL(finished()), &dialog, SLOT(reset()));
	//QObject::connect(&dialog, SIGNAL(canceled()), &futureWatcher, SLOT(cancel()));
	//QObject::connect(&futureWatcher, SIGNAL(progressRangeChanged(int, int)), &dialog, SLOT(setRange(int, int)));
	//QObject::connect(&futureWatcher, SIGNAL(progressValueChanged(int)), &dialog, SLOT(setValue(int)));
	//QFuture<void> future = QtConcurrent::run(this,&MainWindow::executeSirius);
	//QAction *temp = PM.actionFilterMap.value("Print_FLow");
	//QFuture<void> future = QtConcurrent::run(this,&MainWindow::wrapperexecutefilter,printParam);
	//QFuture<void> future = QtConcurrent::run(this,static_cast<void(MainWindow::*)(QAction *,RichParameterSet,bool)>(&executeFilter),temp,printParam,false);
	//QFuture<void> future = QtConcurrent::run(this,[&](QAction *action, RichParameterSet &srcpar, bool isPreview){ return executeFilter(action,srcpar,isPreview); },temp,printParam,false);
	//void (*fpc)(QAction *, RichParameterSet &, bool  ) = &executeFilter;
	//void (*fpc)() = &MainWindow::executeFilter;
	//futureWatcher.setFuture(future);
	//static_cast<void (A::*)(char)>(&f)
	//[&](QAction *action, RichParameterSet &srcpar, bool isPreview){ return executeFilter(action,srcpar,isPreview); };
	//qb->show();


	//connect(&widget_p_monitor, SIGNAL(finished(int)), this, SLOT(monitor_isfinished(int)));
	//QThread t;
	//widget_p_monitor.moveToThread(&t);
	/*
	======
	*/
	switch (1)
	{
	case 0:
	{
		/*WidgetMonitor widget_p_monitor(printParam);
		widget_p_monitor.exec();
		if (widget_p_monitor.result() == QDialog::Accepted)
		{
		qb->show();
		executeFilter(PM.actionFilterMap.value("Print_Flow_2"), printParam, false);
		}

		qb->reset();
		int nextfile = 0;*/


		//sendZXfile(meshDoc()->jobname.at(nextfile));
	}
	break;
	case 1:
	{
		if (printOption_.result() || DSP_TEST_MODE)
			//if (true)
		{//backup
			/*
			  count mesh top pages
			  */
			recordRollerPage();
			QString rollerCMDToServer = SKT::createJsonStringCommand("Other", "Mesh_Top", rollerPageToServer);
			//comm->SendJSONCmd(rollerCMDToServer.toStdString());

			QEventLoop loop;
			QTimer::singleShot(500, &loop, SLOT(quit()));
			loop.exec();
			QString irCommand;
			if (ir_On_Off_Setting)
				irCommand = SKT::createJsonStringCommand("FPGA", "@HTO", QStringList());
			else
				irCommand = SKT::createJsonStringCommand("FPGA", "@HTC", QStringList());
			comm->SendJSONCmd(irCommand.toStdString());

			QTimer::singleShot(500, &loop, SLOT(quit()));
			loop.exec();

			CheckListWidget  checkListWidget(&pjii->getPJI());
			comm->PreAndPrintingHeated(tempHeatParam.at(0), tempHeatParam.at(1), tempHeatParam.at(2), tempHeatParam.at(3));
			comm->PostHeated(tempHeatParam.at(4), tempHeatParam.at(5), tempHeatParam.at(6));

			bool setsuc = comm->setDynamicMidjob_Mode(1);//mode 0 midjob every 4, 1 ,2 send by comm
			setsuc = comm->setDynamicMidjob_PageInterval(wiperFrequency);//mode 0 midjob every 4, 1 ,2 send by comm
			QString setsucs = "setDynamicMidjob_Mode : " + QString::number(setsuc) + "\n";
			fprintf(dbgff, setsucs.toStdString().c_str()); //@@@
			fflush(dbgff);

			comm->WiperIndex(wiperIndex);
			comm->WiperClick(wiperClick);

			/*
			  send Roller_stop_page
			  */

			if (!layerPrintMode)
				comm->PrintMode(1);
			else
				comm->PrintMode(2);
			checkListWidget.exec();
			if (checkListWidget.result() == QDialog::Accepted || DSP_TEST_MODE)
				//if (true)
			{
				//assert(checkListWidget.sendHeader(pjii->getPJI()));
				pjii->genCSVFile();


				bool headt = checkListWidget.sendHeader(pjii->getPJI(), fanSpeed, pjpGenZX.getprintJobParam().getFloat("slice_height"));
				if (headt || DSP_TEST_MODE)
				{
					//QMessageBox::information(this, "Info", "header transfer successful");
					int waitTime = 0;
					while (true && (!DSP_TEST_MODE))
					{
						Sleep(500);
						std::string s, e;
						comm->printerStatus(s, e);

						QString temp = QString::fromStdString(s);
						fprintf(dbgff, temp.toStdString().c_str()); //@@@
						fflush(dbgff);
						if (temp == "Printing")
							//if (temp == "Online")
						{
							genThnumbnail();
							break;
						}
						waitTime += 500;
						if (waitTime > 600000)
						{
							QMessageBox::information(this, "Info", "Timeout");
							return false;
						}

					}
					/*
					Create Snap Shot
					*/
					//executeFilter(PM.actionFilterMap.value("FP_SAVE_DEFAULT_SNAP_SHOT"), pjpGenZX.getprintJobParam(), false);
					/**/
					openDashboard(true);
					qb->show();
					if (!Channel_control_mode)
						executeFilter(PM.actionFilterMap.value("FP_PRINT_FLOW_2_V3"), pjpGenZX.getprintJobParam(), false);
					else
						executeFilter(PM.actionFilterMap.value("FP_PRINT_FOUR_IN_ONE"), pjpGenZX.getprintJobParam(), false);

					/*RichParameterSet tt;
					executeFilter(PM.actionFilterMap.value("count_hole"), tt, false);
					int x = tt.getInt("holeNum");

					if (tt.getInt("holeNum") > 0)
					{
					QMessageBox mb(QMessageBox::NoIcon, "Mesh contain Hole!", "Mesh contain Hole! \nPrint may not successful !\nStill want to print? ", QMessageBox::NoButton, this);
					QPushButton *ok = mb.addButton("OK", QMessageBox::AcceptRole);
					mb.addButton(QMessageBox::Cancel);
					//mb.setInformativeText(tr( "Press button to retry connect printer\n") );
					mb.exec();

					if (mb.clickedButton() == ok)
					{
					openDashboard(true);
					qb->show();
					executeFilter(PM.actionFilterMap.value("Print_Flow_2"), pjpGenZX.getprintJobParam(), false);
					}
					else
					{
					openDashboard(true);
					return false;
					}
					}
					else
					{
					openDashboard(true);
					qb->show();
					executeFilter(PM.actionFilterMap.value("Print_Flow_2"), pjpGenZX.getprintJobParam(), false);
					}*/
				}
				else
				{
					QMessageBox::information(this, "Info", "Printer header transfer false");
					/*qb->show();
					executeFilter(PM.actionFilterMap.value("Print_Flow_2"), printParam, false);*/
					return false;
				}
			}
			qb->reset();
			delOl(MeshModel::meshsort::slice_item);
		}
		//checkListWidget.~CheckListWidget();
	}
	break;
	}

	return true;
}
bool MainWindow::print_test_page_function()
{
	QAction *action = qobject_cast<QAction *>(sender());
	QString filen;
	if (action->text() == "Test Bor File")
	{
		filen = QFileDialog::getOpenFileName(this, tr("Open BOR File"), QDir::currentPath(), "BOR File (*.bor *.);");
		if (filen.isEmpty())return false;
	}

	//***********************
	PrintOption *printOption_;
	if (action->text() == "Test Print(Color Test Page)" || action->text() == "Test Bor File"){
		printOption_ = new PrintOption(PrintOption::optionMode::TEST_PRINT_PAGE_MODE, this, meshDoc());
	}
	else if (action->text() == "Test Print(Start Point Measurement)"){
		printOption_ = new PrintOption(PrintOption::optionMode::START_POINT_MEASUREMENT, this, meshDoc());
	}
	//PrintOption printOption_(PrintOption::optionMode::TEST_PRINT_PAGE_MODE, this, meshDoc());

	WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T(""));

	//float cmyusage[3] = { 0, 0, 0 };
	PrinterJobInfo *pjii = new PrinterJobInfo(this, meshDoc(), &currentGlobalParams, &cmyusage);

	printOption_->updatePrintSummaryLabel(pjii->getPJI());
	printOption_->exec();


	PrintjobParam pjpGenZX;
	pjpGenZX.setCommonPrintValue(meshDoc());
	pjpGenZX.setGenZxParam(false);
	pjpGenZX.setPrintStart(true);



	int pumpValue = currentGlobalParams.getInt("PUMP_VALUE");

	std::string s_result;
	std::string cmdd = "GET /maint/pump_while_print?rpm=";// 65 HTTP / 1.1\r\n\r\n";
	cmdd.append(std::to_string(pumpValue));
	cmdd.append(" HTTP/1.1\r\n\r\n");
	comm->sendSCICommand(cmdd, s_result);


	if (action->text() == "Test Print(Color Test Page)" || action->text() == "Test Bor File")
	{
		pjpGenZX.setPlusSpitton(false);
		//sendBORfile(filen);

	}
	else if (action->text() == "Test Print(Start Point Measurement)")
		pjpGenZX.setPlusSpitton(true);

	//pjpGenZX.setPlusSpitton(true);
	//executeFilter(PM.actionFilterMap.value("FP_PRINT_TEST_PAGE"), pjpGenZX.getprintJobParam(), false);

	/*if (DSP_TEST_MODE)
		pjpGenZX.setPrintStart(false);
		else*/

	/*if (action->text() == "Test Print")
		executeFilter(PM.actionFilterMap.value("FP_PRINT_TEST_PAGE"), pjpGenZX.getprintJobParam(), false);*/

	if (printOption_->result() || DSP_TEST_MODE) //backup	
		//if (1)
	{
		/*bool headt = comm->sendPrintjobHeader(1, 10, 'S');
		if (true)
		{
		QMessageBox::information(this, "Info", "header transfer successful");
		int waitTime = 0;
		while (true)
		{
		Sleep(500);
		std::string s, e;
		comm->printerStatus(s, e);
		QString temp = QString::fromStdString(s);
		if (temp == "Printing")
		{
		break;
		}
		waitTime += 500;
		if (waitTime > 600000)
		{
		QMessageBox::information(this, "Info", "Timeout");
		return false;
		}

		}*/

		std::string s, e;
		int waitTime = 0;
		int timeout = 0;
		bool headerSendOnce = true;

		bool re;
		if (action->text() == "Test Print(Color Test Page)" || action->text() == "Test Bor File")
			re = comm->printTestPage();
		else if (action->text() == "Test Print(Start Point Measurement)")
			re = comm->printIPQCPage();

		while (1)
		{
			comm->printerStatus(s, e);
			//fprintf(dbgff, s.c_str()); //@@@
			//fflush(dbgff);
			QString temp = QString::fromStdString(s);
			bool breakflag = true;

			if (temp == "Printing")break;

			std::map<std::string, int>::iterator it;
			it = comm->printer3dpString.printerStatusmap.find(s);
			if (it != comm->printer3dpString.printerStatusmap.end())
			{

				switch (comm->printer3dpString.printerStatusmap.at(s))
				{
				case Comm3DP::printer3dpString::printStatus::WiperSetting:
				case Comm3DP::printer3dpString::printStatus::BinderRecovering:
				case Comm3DP::printer3dpString::printStatus::Poweroff:
				case Comm3DP::printer3dpString::printStatus::Paused://pause 
				{
					statusLabel->setText(temp);
					statusLabel->repaint();
				}
				break;
				case Comm3DP::printer3dpString::printStatus::Pausing://pause
				{
					statusLabel->setText(temp);
					statusLabel->repaint();
				}break;
				case Comm3DP::printer3dpString::printStatus::Printing://Printing
				{
					statusLabel->setText(temp);
					statusLabel->repaint();


				}break;
				case Comm3DP::printer3dpString::printStatus::Online://????
				{
					statusLabel->setText(temp);
					statusLabel->repaint();
					//bool re = comm->printTestPage();
					if (headerSendOnce)
					{
						/*bool headt = comm->sendPrintjobHeader(1, 10, 'S', 0);
						QMessageBox::information(this, "Info", "header transfer successful");
						executeFilter(PM.actionFilterMap.value("FP_PRINT_TEST_PAGE"), pjpGenZX.getprintJobParam(), false);*/
						headerSendOnce = false;
					}


				}break;
				case Comm3DP::printer3dpString::printStatus::PostPrinting:
				{
					statusLabel->setText(temp);
					statusLabel->repaint();
				}break;
				case Comm3DP::printer3dpString::printStatus::DoorOpened:
				{
					statusLabel->setText(temp);
					statusLabel->repaint();
				}break;
				case Comm3DP::printer3dpString::printStatus::GettingError:
				{
					statusLabel->setText(temp);
					statusLabel->setStyleSheet(WidgetStyleSheet::statusLabel());
					statusLabel->repaint();
					return false;
				}
				break;
				default:
				{

				}

				}
			}

			Sleep(500);
			timeout += 500;

			if (timeout > 1800000)return false;
		}

		QString fileName = PicaApplication::getRoamingDir() + "/TestPage/csa.zx";
		if (action->text() == "Test Print(Color Test Page)")
			executeFilter(PM.actionFilterMap.value("FP_PRINT_TEST_PAGE"), pjpGenZX.getprintJobParam(), false);
		else if (action->text() == "Test Print(Start Point Measurement)")
			executeFilter(PM.actionFilterMap.value("FP_PRINT_TEST_PAGE"), pjpGenZX.getprintJobParam(), false);
		else if (action->text() == "Test Bor File")
		{
			sendBORfile(filen);
		}

		//bool re = comm->printTestPage();
		//}
		qb->reset();
		delOl(MeshModel::meshsort::slice_item);
	}



	return true;

}

bool MainWindow::printFunction2()//picasso JR
{

	//***********************
	PrintOption printOption_(PrintOption::optionMode::NORMAL_PRINT_MODE);
	PrinterJobInfo *pjii = new PrinterJobInfo(this, meshDoc(), &currentGlobalParams);

	printOption_.updatePrintSummaryLabel(pjii->getPJI());
	printOption_.exec();


	//*********
	RichParameterSet printParam;
	printParam.addParam(new RichInt("print_dpi", 300, "dpi", ""));
	printParam.addParam(new RichFloat("slice_height", meshDoc()->p_setting.getbuildheight().value, "height_cm", ""));
	//printParam.addParam(new RichFloat("slice_height", 0.1, "height_cm", ""));

	printParam.addParam(new RichBool("OL_Image", 0, "save_Outline_Image", ""));//輪廓
	printParam.addParam(new RichBool("Cap_Image", 0, "save_cap_Image", ""));	//膠水圖

	printParam.addParam(new RichBool("BLK_OUT_LINE", 0, "BLK_OUT_LINE", ""));
	printParam.addParam(new RichBool("FaceColor", true, "FaceColor", ""));

	printParam.addParam(new RichFloat("useless_print", 0, "useless_print_des", ""));//犧牲條寬度
	printParam.addParam(new RichBool("generate_zx", true, "generate_zx_or_not", ""));//
	printParam.addParam(new RichBool("generate_final_picture", 1, "generate_final_picture", ""));//
	printParam.addParam(new RichBool("mono_bool", printOption_.getMonoParam(), "mono_bool", ""));//***20160322mono_bool


	printParam.addParam(new RichInt("start_page", 0, "start print pages"));

	int boxy_dim = qCeil((meshDoc()->bbox().max.Z() - meshDoc()->bbox().min.Z()) / meshDoc()->p_setting.getbuildheight().value) + 1;
	printParam.addParam(new RichInt("end_page", boxy_dim, "end print pages"));


	printParam.addParam(new RichInt("per_zx_job", 10000, "number of zx pages", ""));//多少頁一個job
	printParam.addParam(new RichInt("send_page_num", 5, "send_page_num"));//幾頁send 一次

	/*printParam.addParam(new RichInt("plus_pixel", 2308, "plus_pixel"));
	printParam.addParam(new RichInt("PLUS_MM", 19, "PLUS_MM"));*/

	printParam.addParam(new RichBool("longpage_orshortpage", 0, "longpage_orshortpage", ""));
	qDebug() << "meshDoc()->p_setting.print_pagesize" << meshDoc()->p_setting.print_pagesize;
	if (meshDoc()->p_setting.print_pagesize == slice_program_setting::page_size::longPage_size) {
		//printParam.addParam(new RichInt("plus_pixel", 2203, "plus_pixel"));//(40-22*0.97)*300/DSP_inchmm
		//printParam.addParam(new RichInt("PLUS_MM", 18, "PLUS_MM"));//40-22//steady function
#if 0
		//bb2
		printParam.addParam(new RichInt("plus_pixel", 2126, "plus_pixel"));//(40-22)*300/DSP_inchmm
		printParam.addParam(new RichFloat("PLUS_MM", 18, "PLUS_MM"));//40-22//steady function
#else
		//bb3
		//printParam.addParam(new RichInt("plus_pixel", 1571, "plus_pixel"));//(35.3-22)*300/DSP_inchmm
		//printParam.addParam(new RichFloat("PLUS_MM", 13.3, "PLUS_MM"));//35.3-22//steady function
		//bb3 22.2
		printParam.addParam(new RichInt("plus_pixel", 1547, "plus_pixel"));//(35.3-22.2)*300/DSP_inchmm
		printParam.addParam(new RichFloat("PLUS_MM", 13.1, "PLUS_MM"));//35.3-22//steady function
#endif

		printParam.setValue("longpage_orshortpage", BoolValue(true));
	}
	else if (meshDoc()->p_setting.print_pagesize == slice_program_setting::page_size::printBed_size) {

		printParam.addParam(new RichInt("plus_pixel", 0, "plus_pixel"));
		printParam.addParam(new RichFloat("PLUS_MM", 0, "PLUS_MM"));
		printParam.setValue("longpage_orshortpage", BoolValue(false));
	}

	printParam.addParam(new RichBool("PRINT_START", meshDoc()->p_setting.get_print_or_not(), "START PRINTING", ""));
	printParam.addParam(new RichFloat("USELESS_PRINT", meshDoc()->p_setting.get_useless_print(), "useless_print", ""));


	switch (1)
	{
	case 0:
	{
		/*WidgetMonitor widget_p_monitor(printParam);
		widget_p_monitor.exec();
		if (widget_p_monitor.result() == QDialog::Accepted)
		{
		qb->show();
		executeFilter(PM.actionFilterMap.value("Print_Flow_2"), printParam, false);
		}

		qb->reset();
		int nextfile = 0;*/


		//sendZXfile(meshDoc()->jobname.at(nextfile));
	}
	break;
	case 1:
	{
		if (printOption_.result()) {

			CheckListWidget  checkListWidget;
			checkListWidget.exec();
			if (checkListWidget.result() == QDialog::Accepted)
			{
				//assert(checkListWidget.sendHeader(pjii->getPJI()));
				if (checkListWidget.sendHeader(pjii->getPJI()))
				{
					QMessageBox::information(this, "Info", "header transfer unsuccessful");

				}
				RichParameterSet tt;
				executeFilter(PM.actionFilterMap.value("count_hole"), tt, false);
				int x = tt.getInt("holeNum");

				if (tt.getInt("holeNum") > 0)
				{
					QMessageBox mb(QMessageBox::NoIcon, "Mesh contain Hole!", "Mesh contain Hole! \nPrint may not successful !\nStill want to print? ", QMessageBox::NoButton, this);
					QPushButton *ok = mb.addButton("OK", QMessageBox::AcceptRole);
					mb.addButton(QMessageBox::Cancel);
					//mb.setInformativeText(tr( "Press button to retry connect printer\n") );
					mb.exec();

					if (mb.clickedButton() == ok)
					{
						qb->show();
						executeFilter(PM.actionFilterMap.value("FP_PRINT_JR"), printParam, false);
					}
					else
					{
						return false;
					}
				}
				else
				{
					qb->show();
					executeFilter(PM.actionFilterMap.value("FP_PRINT_JR"), printParam, false);
				}
			}
			qb->reset();
			delOl(MeshModel::meshsort::slice_item);
		}
		//checkListWidget.~CheckListWidget();
	}
	break;
	}



	return true;
}

void MainWindow::sendZXfile(QString printzxfilename)
{
	QStringList arguments;
	QString tempFileanme = "\"" + QDir::fromNativeSeparators(printzxfilename) + "\"";
	qDebug() << "filepagename" << printzxfilename;

	QString ppath = "cd /d " + PicaApplication::getRoamingDir();
	//arguments << "/k" << "cd /d d:/temptemp/zx_file" << "&" << "usbsend.exe" << "output.zx";						   
	//arguments << "/k" << ppath << "&&" << "usbsend.exe " << tempFileanme ;// << "&" << "exit";	
	//arguments << "/k" << QDir::fromNativeSeparators(ppath) << "&&" << "usbsend.exe"<< tempFileanme;// << "&" << "exit";	

	QString cmd = "cmd /k " + ppath + " && usbsend.exe " + tempFileanme;
	//QProcess::startDetached("cmd /k cd /d C:/Users/tb495076/AppData/Roaming/BCPware && usbsend.exe \"D:\temptemp\Print_Mission\zx\New folder\0912_cube_viewCheck.zx\"");
	//QProcess::startDetached("cmd", arguments);
	QProcess::startDetached(cmd);

}
void MainWindow::sendBORfile(QString printzxfilename)
{
	QStringList arguments;
	QString tempFileanme = "\"" + QDir::fromNativeSeparators(printzxfilename) + "\"";
	qDebug() << "filepagename" << printzxfilename;

	QString ppath = "cd /d " + PicaApplication::getRoamingDir();
	//arguments << "/k" << "cd /d d:/temptemp/zx_file" << "&" << "usbsend.exe" << "output.zx";						   
	//arguments << "/k" << ppath << "&&" << "usbsend.exe " << tempFileanme ;// << "&" << "exit";	
	//arguments << "/k" << QDir::fromNativeSeparators(ppath) << "&&" << "usbsend.exe"<< tempFileanme;// << "&" << "exit";	

	QString cmd = "cmd /k " + ppath + " && usbsend.exe -bor " + printzxfilename;
	//QString cmd = "cmd /k " + ppath + " && usbsend.exe -bor " + "C:\Program Files\18-5-3-57.1_FastNTP_best_v0.7_OmniPEC.bor";

	//QProcess::startDetached("cmd /k cd /d C:/Users/tb495076/AppData/Roaming/BCPware && usbsend.exe \"D:\temptemp\Print_Mission\zx\New folder\0912_cube_viewCheck.zx\"");
	//QProcess::startDetached("cmd", arguments);
	QProcess::startDetached(cmd);
}
void MainWindow::wrapperexecutefilter(RichParameterSet printParam)
{
	executeFilter(PM.actionFilterMap.value("Print_FLow"), printParam, false);
}


void MainWindow::printPainterFunction(QPrinter *printer)
{
	QPainter painter(printer);
	//QSize size = q1->size();//image的width, height;
	QRect painterViewport = painter.viewport();
	painter.setRenderHint(QPainter::Antialiasing);

	painter.setWindow(painterViewport);//***
	//QRectF target(0, 0, 9917, 14033);	//target
	//QRectF target(0, 0, painterViewport.width(), painterViewport.height());	//target	
	QRectF target(0, 0, painterViewport.width(), painterViewport.width());	//target	
	qDebug() << "mainwindow_painter_viewport" << painterViewport.width();
	qDebug() << "mainwindow_painter_viewport" << painterViewport.height();

	//開始列印
	for (int i = 0; i < fileNameList.size(); i++)
	{
		*q2 = QImage(fileNameList[i]);
		QRectF source(0, 0, q2->width(), q2->height());	//image source畫image上的範圍

		painter.drawPixmap(target, QPixmap::fromImage(*q2), source);

		//***20151013
		if (!fileNameList2.empty())
		{
			printer->newPage();
			*q2 = QImage(fileNameList2[i]);
			QRectF source(0, 0, q2->width(), q2->height());	//image source畫image上的範圍
			painter.drawPixmap(target, QPixmap::fromImage(*q2), source);
		}
		if (i < fileNameList.size() - 1)
			printer->newPage();

	}

	painter.end();
}

void MainWindow::executeSirius()
{
	//QString exeFileName = QFileDialog::getOpenFileName(this, tr("select_Print_Image"), lastUsedDirectory.path(), tr("All files(*.*)"));
	//qDebug() << "exeFileName" << exeFileName;coveted

	//***20150903***test execute exe*****************
	//QDesktopServices::openUrl(QUrl("file:///" + exeFileName, QUrl::TolerantMode));

	//QString exeFileName = "d:/VC_TempWorkSpace/Mesh_Test/SKT_version1/meshlab/src/distrib/SiriusUI_.exe";
	//QString exeFileName = "d:/VC_TempWorkSpace/Mesh_Test/SKT_version1/meshlab/src/distrib/";


	/*QDir::setCurrent(exeFileName);
	QString eeFileName = "SiriusUI_.exe";*/

	//****20151221***back up
	QString exeFileName = QApplication::applicationDirPath() + "/SiriusUI_.exe";
	QDesktopServices::openUrl(QUrl("file:///" + exeFileName, QUrl::TolerantMode));

	//***********************************************	
	//QString usbsendpath = "C:/Windows/System32/cmd.exe";	
	//QStringList arguments;
	////arguments << "exit \n";// << " - ddd";		
	//const QString batchPath = QStringLiteral("D:/VC_TempWorkSpace/Mesh_Test/SKT_version1/meshlab/src/distrib");	
	//
	//QString ppath = "cd /d " + getRoamingDir();	
	//arguments << "/k" << ppath << "&&" << "usbsend.exe" << "output.zx";// << "&" << "exit";	
	////p.startDetached("C:/Windows/System32/cmd.exe", arguments);
	//QProcess::startDetached("cmd", arguments);	
}
QString MainWindow::getRoamingDir()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Temp", "application");
	QString location = QFileInfo(settings.fileName()).absolutePath() + "/";
	return location;
}
void MainWindow::delOl(QString s1, QString s2)
{
	//**20160307
	foreach(MeshModel *bm, meshDoc()->meshList)
	{
		if ((bm->label().contains(s1, Qt::CaseSensitive) || bm->label().contains(s2, Qt::CaseSensitive)))
		{
			meshDoc()->delMesh(bm);
		}
	}
}
void MainWindow::delOl(MeshModel::meshsort a)
{
	//**20160307
	foreach(MeshModel *bm, meshDoc()->meshList)
	{
		if ((bm->getMeshSort() == MeshModel::meshsort::cap_item || bm->getMeshSort() == a))
		{
			meshDoc()->delMesh(bm);
		}
	}
}

void MainWindow::delSelectedOl(MeshModel::meshsort a)
{
	foreach(int i, meshDoc()->multiSelectID)
	{
		if (!sliceId.isEmpty())
		{
			foreach(MeshModel *bm, meshDoc()->meshList)
			{
				if ((bm->getMeshSort() == MeshModel::meshsort::cap_item || bm->getMeshSort() == a))
				{
					if (meshDoc()->getMesh(i)->id() == sliceId.value(bm->id()))
					{
						sliceId.remove(bm->id());
						meshDoc()->delMesh(bm);
					}
				}
			}
		}
	}
}

//*** 20170207 --- modified delMeshes() function to apply undo/redo of delete mesh.
// modified by KPPH R&D-I-SW, Mark
void MainWindow::delMeshes()
{
	currentViewContainer()->meshDoc.deletedMeshList.clear();
	currentViewContainer()->meshDoc.deletedMeshId.clear();

	QList<int> list = currentViewContainer()->meshDoc.multiSelectID.toList();
	qSort(list.begin(), list.end(), qLess<int>());

	foreach(int i, list)
	{
		if (isReset == true)
		{
			qDebug() << i;
			resetListID.push_back(i);
		}

		MeshModel *deletedMesh = new MeshModel(meshDoc(), currentViewContainer()->meshDoc.getMesh(i)->fullName(), currentViewContainer()->meshDoc.getMesh(i)->label());
		deletedMesh->updateDataMask(meshDoc()->getMesh(i)->dataMask());
		vcg::tri::Append<CMeshO, CMeshO >::MeshCopy(deletedMesh->cm, currentViewContainer()->meshDoc.getMesh(i)->cm);
		deletedMesh->cm.Tr = meshDoc()->getMesh(i)->cm.Tr;
		deletedMesh->rmm = meshDoc()->getMesh(i)->rmm;
		deletedMesh->is3mf = meshDoc()->getMesh(i)->is3mf;
		meshDoc()->deletedMeshList.push_back(deletedMesh);
		//qDebug() << "deleted id: " << i;
		meshDoc()->deletedMeshId.push_back(i);
	}

	delSelectedOl(MeshModel::meshsort::slice_item);
	meshDoc()->delMeshes();

	if (isReset == false)
		currentViewContainer()->undoStack->push(new delete_command(this, currentViewContainer()->meshDoc.deletedMeshList, currentViewContainer()->meshDoc.deletedMeshId, false));



	updateMenus();
}
int MainWindow::getFCNumberLimit()
{
	return fcNumberLimit;
}
bool MainWindow::fill_fileNameList(QString dirpath, QString filenametoken, QStringList &fileNameList, int sliceNum)
{
	//QDir d_tmep("D:/temptemp/xyzimage");
	//d_tmep.file
	//fileNameList

	QFileInfo tempFIfo;
	QFile file;
	int i = 0;
	QDir recordcurrent = QDir::current();
	//qDebug() <<"fill_fileNameLis_sliceNumt"<< sliceNum;

	if (sliceNum <= 0 && QDir::setCurrent(dirpath))sliceNum = QDir::current().count();
	qDebug() << "sliceNum::qdir" << sliceNum;
	while (i < sliceNum)
	{

		QString tempfilename = filenametoken + "_" + QString("%1.png").arg(i, 4, 10, QChar('0'));

		file.setFileName(tempfilename);
		if (QDir::setCurrent(dirpath));
		else return false;

		if (file.exists())
		{
			qDebug() << "mainwindow_tempfilename_trueuuu" << tempfilename;
			tempFIfo.setFile(QDir::current(), tempfilename);
			fileNameList.push_back(tempFIfo.absoluteFilePath());
		}
		//else return false;
		i++;
	}
	qDebug() << "mainwindow_tempfilename..." << fileNameList;

	QDir::setCurrent(recordcurrent.absolutePath());

	return true;

	//qDebug() << "mainwindow_tempfilename" << tempfilename;

}

void MainWindow::createSliceBar()
{
	MeshModel *slicebar = meshDoc()->addNewMesh("a", "slice_bar", false, RenderMode(GLW::DMFlat));//backup

	slicebar->setMeshSort(MeshModel::meshsort::decorate_item);
	slicebar->Enable(vcg::tri::io::Mask::IOM_VERTCOLOR);
	slicebar->updateDataMask(MeshModel::MM_VERTCOLOR);

	CMeshO::VertexIterator vii;
	CMeshO::FaceIterator efii;

	vii = vcg::tri::Allocator<CMeshO>::AddVertices(slicebar->cm, 3);
	efii = vcg::tri::Allocator<CMeshO>::AddFaces(slicebar->cm, 1);
	CMeshO::VertexPointer  ptVec[3];
	CMeshO::VertexPointer  ntVec[3];
	float z = -10;
	float x = 10.5;
	float y = -12;
	(*vii).C() = Color4b(255, 255, 0, 0);
	(*vii).N() = vcg::Point3f(0, -1, 0);
	ptVec[0] = &*vii; vii->P() = CMeshO::CoordType(x + 1, y, z + 1); ++vii;
	(*vii).C() = Color4b(255, 255, 0, 0);
	(*vii).N() = vcg::Point3f(0, -1, 0);
	ptVec[1] = &*vii; vii->P() = CMeshO::CoordType(x, y, z); ++vii;
	(*vii).C() = Color4b(255, 255, 0, 0);
	(*vii).N() = vcg::Point3f(0, -1, 0);
	ptVec[2] = &*vii; vii->P() = CMeshO::CoordType(x + 1, y, z - 1); ++vii;

	efii->V(0) = ptVec[0];
	efii->V(1) = ptVec[1];
	efii->V(2) = ptVec[2];
	//****************************
	vii = vcg::tri::Allocator<CMeshO>::AddVertices(slicebar->cm, 3);
	efii = vcg::tri::Allocator<CMeshO>::AddFaces(slicebar->cm, 1);
	z = -10;
	x = 10.5;
	y = -12;
	(*vii).C() = Color4b(255, 255, 0, 0);
	(*vii).N() = vcg::Point3f(0, -1, 0);
	ptVec[0] = &*vii; vii->P() = CMeshO::CoordType(x, y, z - 0.1); ++vii;
	(*vii).C() = Color4b(255, 255, 0, 0);
	(*vii).N() = vcg::Point3f(0, -1, 0);
	ptVec[1] = &*vii; vii->P() = CMeshO::CoordType(x, y, z + 0.1); ++vii;
	(*vii).C() = Color4b(255, 255, 0, 0);
	(*vii).N() = vcg::Point3f(0, -1, 0);
	ptVec[2] = &*vii; vii->P() = CMeshO::CoordType(-x, y, z + 0.1); ++vii;

	efii->V(0) = ptVec[0];
	efii->V(1) = ptVec[1];
	efii->V(2) = ptVec[2];

	//****************
	vii = vcg::tri::Allocator<CMeshO>::AddVertices(slicebar->cm, 3);
	efii = vcg::tri::Allocator<CMeshO>::AddFaces(slicebar->cm, 1);
	z = -10;
	x = 10.5;
	y = -12;
	(*vii).C() = Color4b(255, 255, 0, 0);
	(*vii).N() = vcg::Point3f(0, -1, 0);
	ptVec[0] = &*vii; vii->P() = CMeshO::CoordType(-x, y, z - 0.1); ++vii;
	(*vii).C() = Color4b(255, 255, 0, 0);
	(*vii).N() = vcg::Point3f(0, -1, 0);
	ptVec[1] = &*vii; vii->P() = CMeshO::CoordType(x, y, z - 0.1); ++vii;
	(*vii).C() = Color4b(255, 255, 0, 0);
	(*vii).N() = vcg::Point3f(0, -1, 0);
	ptVec[2] = &*vii; vii->P() = CMeshO::CoordType(-x, y, z + 0.1); ++vii;

	efii->V(0) = ptVec[0];
	efii->V(1) = ptVec[1];
	efii->V(2) = ptVec[2];

	tri::UpdateBounding<CMeshO>::Box(slicebar->cm);

	tri::UpdateColor<CMeshO>::PerVertexConstant(slicebar->cm, Color4b(255, 255, 0, 255));
	tri::UpdateTopology<CMeshO>::EdgeEdge(slicebar->cm);
	slicebar->UpdateBoxAndNormals();

	/*QMap<int, RenderMode>::iterator it = GLA()->rendermodemap.find(meshDoc()->mm()->id());
	if (it == GLA()->rendermodemap.end())
	it.value().colorMode = vcg::GLW::CMPerVert;
	GLA()->updateRendermodemapSiganl = true;*/

	//slicebar->Enable(vcg::tri::io::Mask::IOM_VERTTEXCOORD);
	//	tri::UpdateTopology<CMeshO>::FaceFace(slicebar->cm);
	/*ptVec[0]->P().V() = MeshO::CoordType(0, 0, 0);
	ptVec[1]->P() = CMeshO::CoordType(1, 0, 0);
	ptVec[2]->P() = CMeshO::CoordType(0, 1, 0);*/
	/*std::vector<Point3d> ptVec;
	ptVec.push_back(0., 0., 0.);
	ptVec.push_back(1, 0, 0);
	ptVec.push_back (0., 0, 1);*/


	//(*vii).P() = ptVec[0];//第一點	
	///*if (tri::HasPerWedgeTexCoord(m))(*vii).T() = tex_coord[0];
	//if (vcg::tri::HasPerVertexColor(m))(*vii).C() = ptColor[0]*/;
	//(*efii).V(0) = (&*vii);//***			
	//vii++;

	//(*vii).P() = ptVec[1];//第二點
	///*if (tri::HasPerWedgeTexCoord(m))(*vii).T() = tex_coord[1];
	//if (vcg::tri::HasPerVertexColor(m))(*vii).C() = ptColor[1];*/
	//(*efii).V(1) = (&*vii);//***			
	//vii++;

	//(*vii).P() = ptVec[2];//第3點
	///*if (tri::HasPerWedgeTexCoord(m))(*vii).T() = tex_coord[2];
	//if (vcg::tri::HasPerVertexColor(m))(*vii).C() = ptColor[2];*/
	//(*efii).V(2) = (&*vii);
}
void MainWindow::testslicefunc(MeshDocument &md, float sliceheight)
{
	sliceH = sliceheight;
	Point3m planeAxis(0, 0, 1);
	planeAxis.Normalize();

	//***20150610改成Z軸
	float maxVal = md.bbox().max[2];
	float minVal = md.bbox().min[2];

	Point3m planeCenter;
	Plane3m slicingPlane;

	//***20150610改成Z軸
	planeCenter = Point3m(0, 0, sliceheight);//從最底部開始
	slicingPlane.Init(planeCenter, planeAxis);//init切平面

	QString olMName = md.p_setting.getOlMeshName();
	QString capMName = md.p_setting.getCapMeshName();
	sliceId.clear();
	if (view_outline->isChecked())
		foreach(MeshModel *tmm, md.meshList)
	{
		if (tmm->getMeshSort() == MeshModel::meshsort::print_item)
			if (vcg::tri::HasPerWedgeTexCoord(tmm->cm) || vcg::tri::HasPerVertexColor(tmm->cm))
			{
				MeshModel *cap = md.addNewMesh("", olMName, false, RenderMode(GLW::DMTexWire));//backup	
				cap->rmm.drawMode = GLW::DMTexWire;
				cap->setMeshSort(MeshModel::meshsort::slice_item);
				sliceId.insert(cap->id(), tmm->id());

				if (vcg::tri::HasPerWedgeTexCoord(tmm->cm))
				{
					//qDebug("has_Tex_in meshFilter");
					cap->Enable(vcg::tri::io::Mask::IOM_VERTTEXCOORD);
					//cap->rmm.textureMode = GLW::TMPerVert;
					//qDebug("2_place %i, %i, ", tmm->glw.TMId[0], tmm->glw.TMId[1]);
				}
				//判斷有無材質，且存在材質
				if (vcg::tri::HasPerWedgeTexCoord(tmm->cm) && !tmm->cm.textures.empty())cap->rmm.textureMode = GLW::TMPerVert;

				if ((vcg::tri::HasPerVertexColor(tmm->cm) || vcg::tri::HasPerFaceColor(tmm->cm)) && (tmm->rmm.colorMode == GLW::ColorMode::CMPerVert || tmm->rmm.colorMode == GLW::ColorMode::CMPerFace))
				{
					cap->Enable(vcg::tri::io::Mask::IOM_VERTCOLOR);
					//===20151111
					cap->Enable(vcg::tri::io::Mask::IOM_FACECOLOR);
					//cap->rmm.colorMode = tmm->rmm.colorMode;//***20160301
					cap->rmm.colorMode = GLW::CMPerVert;//backup
				}
				cap->glw.TMIdd = tmm->glw.TMIdd;
				int tempmode = (int)tmm->rmm.colorMode;
				vcg::IntersectionPlane_TexMesh<CMeshO, CMeshO, CMeshO::ScalarType>(tmm->cm, slicingPlane, cap->cm, true, tempmode);
				//vcg::IntersectionPlane_TexMesh_test_20151113<CMeshO, CMeshO, CMeshO::ScalarType>(tmm->cm, slicingPlane, cap->cm, faceColor);
				//tri::Clean<CMeshO>::RemoveDuplicateVertex(cap->cm);

			}
	}

}
void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{

	/*QMenu menu(this);
	menu.addAction(copyAct);
	menu.addAction(pasteAct);
	menu.addSeparator();
	menu.addAction(moveToCenterAct);
	QMenu *submenu = menu.addMenu("submenu");
	menu.exec(event->globalPos());*/

	//QMenu menu(this);
	//menu.addAction(copyAct);
	//menu.addMenu(contextMenu);

	//rightmouse_contextMenu->exec(event->globalPos());

	//menu.exec(event->globalPos());
}
void MainWindow::move_to_center_ground()
{
	foreach(int i, currentViewContainer()->meshDoc.multiSelectID)
	{
		toCenter();
	}
}

void MainWindow::executeSliceFunc(double position)
{
	qDebug() << "slice position: " << position;
	delOl(MeshModel::meshsort::slice_item);
	testslicefunc(*meshDoc(), position);
}
bool MainWindow::testGetstring()
{
#define BUF_SIZE 256
	TCHAR szName[] = TEXT("Global\MyFileMappingObject");
	HANDLE hMapFile;
	LPCTSTR pBuf;

	hMapFile = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,   // read/write access
		FALSE,                 // do not inherit the name
		szName);               // name of mapping object

	if (hMapFile == NULL)
	{
		qDebug() << (TEXT("Could not open file mapping object (%d).\n"),
			GetLastError());
		return 1;
	}

	pBuf = (LPTSTR)MapViewOfFile(hMapFile, // handle to map object
		FILE_MAP_ALL_ACCESS,  // read/write permission
		0,
		0,
		BUF_SIZE);

	if (pBuf == NULL)
	{
		qDebug() << (TEXT("Could not map view of file (%d).\n"),
			GetLastError());

		CloseHandle(hMapFile);

		return 1;
	}

	MessageBox(NULL, pBuf, TEXT("Process2"), MB_OK);
	//QMessageBox::warning(NULL, TEXT("Process2"),QString( pBuf));

	UnmapViewOfFile(pBuf);

	CloseHandle(hMapFile);
}
//void MainWindow::mousePressEvent(QMouseEvent *event)
//{
//	if (!this->hasFocus())
//	{
//		this->setFocus();
//		//bt1->setChecked(false);
//		//fw1->close();
//	}
//}
void MainWindow::zoomInSlot()
{
	GLA()->trackball.MouseWheel(-1);
}
void MainWindow::zoomOutSlot()
{
	GLA()->trackball.MouseWheel(1);
}
void MainWindow::closeEvent(QCloseEvent *event)
{
	if (filter_process_running)
	{
		QMessageBox::StandardButton ret = QMessageBox::question(
			this, tr("Palette"), tr("Function Processing .\n\nAre you sure to close?"),
			QMessageBox::Yes | QMessageBox::No,
			QMessageBox::No);
		if (ret == QMessageBox::No)	// don't close please!
		{
			event->ignore();
			return;
		}

	}
	else
	{
	}

	//SYDNY 05/08/2017
	if (mdiarea->subWindowList().size() == 0)
	{
		event->accept();
		return;
	}



	int winCount = mdiarea->subWindowList().size();

	for (int i = 0; i < winCount; ++i)
	{
		int currentWinCount = mdiarea->subWindowList().size();

		mdiarea->closeActiveSubWindow();

		if (currentWinCount > mdiarea->subWindowList().size())
		{
			mdiarea->activatePreviousSubWindow();
		}

		else
		{
			event->ignore();
			break;
		}
	}

	QSettings xyzReg("HKEY_CURRENT_USER\\SOFTWARE\\XYZ", QSettings::NativeFormat);
	if (!xyzReg.childGroups().contains("rememberuser", Qt::CaseInsensitive))
	{
		xyzReg.remove("xyzsettings");
		xyzReg.remove("offline");
	}
}

//void MainWindow::setlayerLabel(double layer)
//{
//	float x = meshDoc()->bbox().min.Z();
//
//	//layerLabel->setNum(int((layer - x) / meshDoc()->p_setting.getbuildheight().value));
//	qDebug() << "setlayerLabel" << meshDoc()->bbox().min.Z();
//	if (layer >= x)
//		//layerLabel->setNum(int((layer - x) *100) );
//		layerLabel->setText(QString::number(int((layer - x) * 10)) + " slices");
//}

void MainWindow::setSliceSpinBox(double layer)
{
	/*
		SliceSpinBox
		0 ~ 1968
		*/
	//meshDocZ = meshDoc()->bbox().min.Z();
	//sliceSpinBox->setMaximum((100 - meshDocZ) * 10);
	//sliceSpinBox->setMinimum(0);

	sliceSpinBox->blockSignals(true);
	//double realLayer = layer * 1.016;
	//if (realLayer >= meshDocZ)
	//sliceSpinBox->setValue((realLayer - meshDocZ) * 10);
	sliceSpinBox->setValue(layer * 10 + 984);
	//else
	//sliceSpinBox->setValue(0);

	sliceSpinBox->blockSignals(false);
	double pos = -100 + (layer * 10 + 984) * 0.1016;
	emit slicePositionChanged(pos);
}

void MainWindow::setSliderPosition(double value)//slider represent layer
{
	/*
		crossSlider
		-984 ~ 984
		*/
	meshDocZ = meshDoc()->bbox().min.Z();

	//int intValue = int(((value / 10) + meshDocZ) * 10);
	double temp = -984 + value;
	crossSlider->setSliderPosition(-984 + value);

	//emit slicePositionChanged((value / 10) + meshDocZ);
	double pos = -100 + value*0.1016;
	emit slicePositionChanged(pos);
}

void MainWindow::showObjectList(bool visible)
{
	if ((GLA() != 0) && (layerDialog != NULL))
	{
		objList->setVisible(visible);
		showObjectListAct->setChecked(visible);
	}
}

void MainWindow::changeMeshColor()
{
	currentViewContainer()->meshDoc.oldColorList.clear();

	QList<int> list = meshDoc()->multiSelectID.toList();
	qSort(list.begin(), list.end(), qLess<int>());


	Color4b defColor = vcg::Color4b::Black;




	foreach(int i, list)
	{
		if (meshDoc()->getMesh(i)->hasDataMask(MeshModel::MM_COLOR))
		{
			defColor = meshDoc()->getMesh(i)->cm.C();
			break;
		}
	}

	delOl(MeshModel::meshsort::slice_item);
	QColorDialog dlg(this);
	dlg.setWindowTitle("Change Color");
	QColor newDefColor = QColor(defColor.X(), defColor.Y(), defColor.Z(), 255);
	dlg.setCurrentColor(newDefColor);
	dlg.exec();

	if (dlg.result())
	{
		color = dlg.selectedColor(); //QColorDialog::getColor(Qt::green, 0, "Select Color");

		//*** 20170126 --- added foreach below to apply multiple change of meshes color.
		// added by KPPH (R&D-I-SW, Mark)
		QMap<int, int> meshDataMask;
		QMap<int, RenderMode> meshRenderMode;
		QMap<int, Color4b> recordColor;
		QMap<int, QList<Color4b>> recordColorVector;
		QMap<int, QList<Color4b>> recordColorFace;

		foreach(int i, currentViewContainer()->meshDoc.multiSelectID)
		{
			MeshModel *chm = meshDoc()->getMesh(i);
			meshDataMask.insert(i, chm->dataMask());
			meshRenderMode.insert(i, chm->rmm);

			if (!chm->hasDataMask(MeshModel::MM_COLOR) && (chm->hasDataMask(MeshModel::MM_VERTCOLOR)) && !chm->hasDataMask(MeshModel::MM_FACECOLOR))
			{
				QList<Color4b> meshVertexcolor;
				for (CMeshO::VertexIterator vi = chm->cm.vert.begin(); vi != chm->cm.vert.end(); ++vi)
				{
					if (!(*vi).IsD())
					{
						meshVertexcolor.push_back(vi->C());
					}
				}
				recordColorVector.insert(i, meshVertexcolor);
			}
			else if (!chm->hasDataMask(MeshModel::MM_COLOR) && chm->hasDataMask(MeshModel::MM_FACECOLOR))
			{
				QList<Color4b> meshFaceColor;
				for (CMeshO::FaceIterator fi = chm->cm.face.begin(); fi != chm->cm.face.end(); ++fi)
				{
					if (!(*fi).IsD())
					{
						meshFaceColor.push_back(fi->C());
					}
				}
				recordColorFace.insert(i, meshFaceColor);
			}
			else if (chm->hasDataMask(MeshModel::MM_COLOR))
			{
				recordColor.insert(i, chm->cm.C());
			}

			Color4b newColor = Color4b(color.red(), color.green(), color.blue(), 255);
			chm->updateDataMask(MeshModel::MM_VERTFACETOPO | MeshModel::MM_FACEMARK | MeshModel::MM_VERTMARK);
			chm->Enable(tri::io::Mask::IOM_VERTCOLOR);
			chm->updateDataMask(MeshModel::MM_VERTCOLOR);
			chm->updateDataMask(MeshModel::MM_COLOR);

			if (chm->hasDataMask(MeshModel::MM_FACECOLOR) && !(chm->hasDataMask(MeshModel::MM_WEDGTEXCOORD)))
				chm->clearDataMask(MeshModel::MM_FACECOLOR);

			tri::InitVertexIMark(chm->cm);
			tri::UpdateColor<CMeshO>::PerVertexConstant(chm->cm, newColor);
			tri::UpdateNormal<CMeshO>::PerVertex(chm->cm);
			chm->cm.C() = newColor;
			chm->rmm.colorMode = vcg::GLW::CMPerVert;
			chm->rmm.textureMode = vcg::GLW::TMNone;
			GLA()->updateRendermodemapSiganl = true;
		}

		currentViewContainer()->undoStack->push(new colorChange_command(this, meshDoc(), currentViewContainer()->meshDoc.multiSelectID, recordColor, meshDataMask, meshRenderMode, recordColorVector, recordColorFace, false));
	}
	//GLA()->reFreshGlListlist();
	/*GLA()->setHastoUpdatePartVBO();
	GLA()->VBOupdatePart[0] = false;
	GLA()->VBOupdatePart[1] = false;
	GLA()->VBOupdatePart[2] = true;*/
	//GLA()->updatePartVBO(false, true);
	GLA()->setHastoRefreshVBO();

}

//extern void picassojrprintfunc();
void MainWindow::tempPicassoJRPrinting()
{

	//ShellExecuteA(0, ("open"), "cmd.exe", " /C timm_test2_re.exe t", 0, SW_HIDE);
	ShellExecuteA(0, ("open"), "cmd.exe", " /C timm_test2_re.exe t", 0, SW_SHOWNORMAL);
	//picassojrprintfunc();

}
void MainWindow::openDashboard(bool toggle)
{
	// ShellExecuteA(0, ("open"), "cmd.exe", " /C timm_test2_re.exe t", 0, SW_SHOWNORMAL);
	//ShellExecuteEx(NULL, L"runas", L"3DPDashboard.exe", NULL, NULL, SW_SHOWDEFAULT);
	//ShellExecute(NULL, L"runas", L"3DPDashboard.exe", NULL, NULL, SW_SHOWNORMAL);
	//QProcess::startDetached("3DPDashboard.exe");

	if (toggle)
	{
		//WCHAR *tempcc = _T("DashBoardWin");
		HWND hwnd = ::FindWindowEx(NULL, NULL, NULL, DSP_DASHBOARD_WINDOW_NAME);
		if (IsWindow(hwnd))
		{
			bool bo = SKT::show3DPDashboard<bool>(DSP_DASHBOARD_WINDOW_NAME, SW_SHOWNORMAL);
		}
		else {
			QString temppath = QCoreApplication::applicationDirPath() + "/";
			//QString temppath = PicaApplication::getRoamingDir() + QString::fromWCharArray(DSP_DASHBOARD_NAME) ;
			temppath.append(QString::fromWCharArray(DSP_DASHBOARD_NAME));
			temppath.replace("/", "\\");
			QByteArray byteArray = temppath.toLocal8Bit();
			char *tempchar = byteArray.data();
			std::wstring wc(temppath.size(), L'#');
			mbstowcs(&wc[0], tempchar, temppath.size());
			SKT::execute3DPDashboard<bool>(wc, SW_SHOWNORMAL);
		}
	}
	else
	{
		//WCHAR *tempcc = _T("DashBoardWin");
		bool bo = SKT::show3DPDashboard<bool>(DSP_DASHBOARD_WINDOW_NAME, SW_HIDE);
	}


}
void MainWindow::updateMenuForCommand() {

	if (currentViewContainer() != NULL  && meshDoc()->mm() != NULL && meshDoc()->multiSelectID.size() != 0)
	{
		/*transformwidgett1->curmwi = this;
		transformwidgett1->curmwi2 = this;
		transformwidgett1->meshDocument = meshDoc();
		rotate_widget_cus->curmwi = this;
		rotate_widget_cus->meshDocument = meshDoc();
		scale_widget_cus->curmwi = this;
		scale_widget_cus->meshDocument = meshDoc();*/

		transformwidgett1->loadParameter();
		rotate_widget_cus->loadParameter();
		scale_widget_cus->loadParameter();
	}
	if (currentViewContainer() != NULL && meshDoc() != NULL)
	{
		meshDoc()->groove.setGrooveHit(SKT::detect_hit_groove<Scalarm>(*meshDoc()));

	}


}
void MainWindow::changeCurrentMeshRenderMode()
{
	if (!GLA()) return;
	if (meshDoc()->isBusy()) return;
	QAction *action = qobject_cast<QAction *>(sender());


	QString ts = action->text();
	if (ts == "Vertex")
	{
		foreach(int i, meshDoc()->multiSelectID)
		{
			MeshModel *m = meshDoc()->getMesh(i);
			m->Enable(tri::io::Mask::IOM_VERTCOLOR);

			m->rmm.setColorMode(GLW::CMPerVert);
			m->glw.SetHint(vcg::GLW::Hint::HNUseVBO);
			GLA()->setHastoRefreshVBO();
			/*m->rmm.setColorMode(GLW::CMPerFace);
			m->glw.ClearHint(vcg::GLW::Hint::HNUseVBO);*/
		}
	}
	else if (ts == "Facet")
	{
		foreach(int i, meshDoc()->multiSelectID)
		{
			MeshModel *m = meshDoc()->getMesh(i);



			m->Enable(tri::io::Mask::IOM_FACECOLOR);

			m->rmm.setColorMode(GLW::CMPerFace);
			m->glw.SetHint(vcg::GLW::Hint::HNUseVBO);
			GLA()->setHastoRefreshVBO();

			/*m->rmm.setColorMode(GLW::CMPerFace);
			m->glw.ClearHint(vcg::GLW::Hint::HNUseVBO);*/

		}
	}
	else if (ts == "Texture")
	{
		foreach(int i, meshDoc()->multiSelectID)
		{
			MeshModel *m = meshDoc()->getMesh(i);
			m->rmm.setColorMode(GLW::CMNone);
			m->Enable(tri::io::Mask::IOM_WEDGTEXMULTI);
			m->rmm.setTextureMode(GLW::TMPerWedgeMulti);
			//m->Enable(tri::io::Mask::IOM_WEDGTEXMULTI);
			/*if (m->cm.textures.size() < 2)
			{*/
			m->glw.SetHint(vcg::GLW::Hint::HNUseVBO);
			GLA()->setHastoRefreshVBO();
			//}

		}
	}
	//GLA()->reFreshGlListlist();
}

void MainWindow::copyNumber()
{

	CopyDialog testcopy;
	int ret = testcopy.exec();
	if (ret == QDialog::Accepted)
	{
		int test = testcopy.getcopyNum();
		for (int i = 0; i < test; i++)
		{
			pasteSelectMeshes();
			bool x = QCallBack(((i + 1) * 100) / test, "");
			if (!x)break;
		}
	}
	pause_button->setVisible(false);
	s_pushbutton->setVisible(false);
	qb->reset();
}



void MainWindow::genByDMSlicer()
{
	/*=======================================================
	DM_FUNC_TEST
	==========================================================*/
	QString zxaFilename = QFileDialog::getSaveFileName(this, tr("Save ZX File"), QDir::currentPath(), tr("ZXA File (*.zxa)"));
	if (zxaFilename.isEmpty())return;
	if (QFileInfo(zxaFilename).exists())
	{
		QProcess process;
		/*extract file*/
		QStringList arguments;
		arguments.append("d");
		arguments.append("-r");
		arguments.append(zxaFilename);
		arguments.append("*.*");
		int iReturnCode = QProcess::execute("7z", arguments);
	}
	PrintjobParam pjpGenZX;
	pjpGenZX.setCommonPrintValue(meshDoc());
	pjpGenZX.setGenZxParam(true);
	pjpGenZX.genDebugPic(false);//Test
	pjpGenZX.setZXFilePath(zxaFilename);

	pjpGenZX.setPrintStart(false);
	pjpGenZX.setStartPrintEstimate(true);
	pjpGenZX.setColorProfile(currentGlobalParams.getString("COLOR_PROFILE"));
	pjpGenZX.setStiffPrint(currentGlobalParams.getBool("STIFF_PRINT"));
	pjpGenZX.setDilateBinder(currentGlobalParams.getBool("DILATE_BINDER"));
	pjpGenZX.setDialteBinderValue(currentGlobalParams.getInt("DILATE_BINDER_VALUE"));
	pjpGenZX.setDynamicWipe(currentGlobalParams.getBool("DYNAMIC_WIPE"));
	pjpGenZX.setColorBinding(currentGlobalParams.getBool("COLOR_BINDING"));

	//executeFilter(PM.actionFilterMap.value("FP_PRINT_DM_SLICER"), pjpGenZX.getprintJobParam(), false);//	
	executeFilter(PM.actionFilterMap.value("FP_PRINT_DM_SLICER_ALPHA"), pjpGenZX.getprintJobParam(), false);//	

	/*add 7zip*/

	QString zxtemp = PicaApplication::getRoamingDir();
	QDir zx_dir(zxtemp);
	zx_dir.setPath(zxtemp + "/zx_file");
	/*
	gen dynamic info
	*/
	PrinterJobInfo *pjii = new PrinterJobInfo(this, meshDoc(), &currentGlobalParams);

	stringstream ssd;
	std::stringbuf *pbuf = ssd.rdbuf();
	string delimiter = "=";
	//ssd << "name=" << zxFilename.toStdString();
	//QMap<QString, QVariant>tempQMap;
	QList<QPair<QString, QVariant>> tempQList;
	GenPrintInfo::genPrinterInfoDM(currentGlobalParams, tempQList, *pjii);
	for (int i = 0; i < tempQList.size(); i++)
	{
		ssd << tempQList[i].first.toStdString() << delimiter << tempQList[i].second.toString().toStdString() << "\n";
	}


	QFileInfo tempFileInfo(zxaFilename);
	QString strNewName = zx_dir.absolutePath() + "/" + tempFileInfo.completeBaseName() + ".txt";
	SKT::WriteToAFile<bool>(ssd, strNewName.toStdString());
	const std::string tmp = ssd.str();
	int ttt = tmp.size();

	QFileInfoList filenin_zx_file = zx_dir.entryInfoList(QStringList() << "*.zx" << "*.txt");

	QProcess process;
	bool ret = false;
	bool ret2 = false;
	/*extract file*/
	QStringList arguments;
	arguments.append("a");
	arguments.append(zxaFilename);
	foreach(QFileInfo info, filenin_zx_file)
		arguments.append(info.absoluteFilePath());
	int iReturnCode = QProcess::execute("7z", arguments);



	/**/

	//int xd = pbuf->in_avail();

	/*string tempZipFilename = zxaFilename.toStdString();
	ZipIO zipfile(tempZipFilename.c_str());
	zipfile.appendZipFile(tempFileInfo.completeBaseName().toStdString().append(".txt").c_str());
	zipfile.writeBuffertoEntry(tmp.c_str(), tmp.size());
	zipfile.closeEntry();
	zipfile.closeZip();*/
	/*
	********
	*/

	/*===========================================================================*/
	GLA()->setHastoRefreshVBO();
	//GLA()->setHastoUpdatePartVBO();
	meshDoc()->setBusy(false);
	/*end*/
}

void MainWindow::genByDMSlicer_BJM()
{
	/*=======================================================
	DM_FUNC_TEST
	==========================================================*/
	QString zxaFilename = QFileDialog::getSaveFileName(this, tr("Save ZX File"), QDir::currentPath(), tr("ZXA File (*.zxa)"));
	if (zxaFilename.isEmpty())return;
	if (QFileInfo(zxaFilename).exists())
	{
		QProcess process;
		/*extract file*/
		QStringList arguments;
		arguments.append("d");
		arguments.append("-r");
		arguments.append(zxaFilename);
		arguments.append("*.*");
		int iReturnCode = QProcess::execute("7z", arguments);
	}

	PrintjobParam pjpGenZX;
	pjpGenZX.setCommonPrintValue(meshDoc());
	pjpGenZX.setGenZxParam(true);
	pjpGenZX.genDebugPic(false);//Test
	pjpGenZX.setZXFilePath(zxaFilename);

	pjpGenZX.setPrintStart(false);
	pjpGenZX.setStartPrintEstimate(true);
	pjpGenZX.setColorProfile(currentGlobalParams.getString("COLOR_PROFILE"));
	pjpGenZX.setStiffPrint(currentGlobalParams.getBool("STIFF_PRINT"));
	pjpGenZX.setDilateBinder(currentGlobalParams.getBool("DILATE_BINDER"));
	pjpGenZX.setDialteBinderValue(currentGlobalParams.getInt("DILATE_BINDER_VALUE"));
	pjpGenZX.setDynamicWipe(currentGlobalParams.getBool("DYNAMIC_WIPE"));
	pjpGenZX.setColorBinding(currentGlobalParams.getBool("COLOR_BINDING"));

	//executeFilter(PM.actionFilterMap.value("FP_PRINT_DM_SLICER"), pjpGenZX.getprintJobParam(), false);//	
	executeFilter(PM.actionFilterMap.value("FP_PRINT_DM_SLICER_ALPHA"), pjpGenZX.getprintJobParam(), false);//	

	/*add 7zip*/

	QString zxtemp = PicaApplication::getRoamingDir();
	QDir zx_dir(zxtemp);
	zx_dir.setPath(zxtemp + "/zx_file");




	/*
	gen dynamic info
	*/
	PrinterJobInfo *pjii = new PrinterJobInfo(this, meshDoc(), &currentGlobalParams, NULL, PrinterJobInfo::PrintOptionMode);



	QFileInfo tempFileInfo(zxaFilename);
	QString strNewName = zx_dir.absolutePath() + "/" + "metadata.json";
	QString zxFileName = /*zx_dir.absolutePath() + "/" + */tempFileInfo.completeBaseName() + ".zx";
	QString thumbNailFileName = /*zx_dir.absolutePath() + "/" +*/ tempFileInfo.completeBaseName() + ".png";
	QString stlStr = zx_dir.absolutePath() + "/" + tempFileInfo.completeBaseName() + ".stl";

	/*
	Create Snap Shot
	*/
	executeFilter(PM.actionFilterMap.value("FP_SAVE_DEFAULT_SNAP_SHOT"), pjpGenZX.getprintJobParam(), false);

	/*
	create Meta Data
	*/
	GenPrintInfo::genPrinterInfoDMJson(currentGlobalParams, *pjii, strNewName, thumbNailFileName, tempFileInfo.completeBaseName() + ".stl", zxFileName);//Create Meta Data


	/*
	save STL
	*/

	MeshModel *newMesh = new MeshModel(meshDoc(), "", "b");
	//newMesh->updateDataMask(meshDoc()->mm()->dataMask());
	foreach(MeshModel *mm, meshDoc()->meshList)
	{
		newMesh->updateDataMask(mm->dataMask());
		vcg::tri::Append<CMeshO, CMeshO>::Mesh(newMesh->cm, mm->cm);
		tri::UpdateNormal<CMeshO>::PerVertex(newMesh->cm);
	}
	bool re4t;

	if (!stlStr.isEmpty())
	{
		//save path away so we can use it again
		QString path = stlStr;
		path.truncate(path.lastIndexOf("/"));
		lastUsedDirectory.setPath(path);

		QString extension = "stl";// stlStr;
		//extension.remove(0, stlStr.lastIndexOf('.') + 1);


		MeshIOInterface *pCurrentIOPlugin = PM.allKnowOutputFormats[extension.toLower()];
		if (pCurrentIOPlugin == 0)
		{
			QMessageBox::warning(this, "Unknown type", "File extension not supported!");
		}
		//MeshIOInterface* pCurrentIOPlugin = meshIOPlugins[idx-1];
		pCurrentIOPlugin->setLog(&meshDoc()->Log);

		int capability = 0, defaultBits = 0;
		pCurrentIOPlugin->GetExportMaskCapability(extension, capability, defaultBits);

		// optional saving parameters (like ascii/binary encoding)
		RichParameterSet savePar;

		pCurrentIOPlugin->initSaveParameter(extension, *(newMesh), savePar);
		//***20160711***
		int mask = 256;// maskDialog.GetNewMask();

		re4t = pCurrentIOPlugin->save(extension, stlStr, *newMesh, mask, savePar, QCallBack, this);//****save function	


		meshDoc()->delMesh(newMesh);

	}



	/*=========================================*/






	QFileInfoList filenin_zx_file = zx_dir.entryInfoList(QStringList() << "*.zx" << "*.json" << "*.png" << "*.stl");





	QProcess process;
	bool ret = false;
	bool ret2 = false;
	/*extract file*/
	QStringList arguments;
	arguments.append("a");
	arguments.append(zxaFilename);
	foreach(QFileInfo info, filenin_zx_file)
		arguments.append(info.absoluteFilePath());
	int iReturnCode = QProcess::execute("7z", arguments);



	/**/





	//int xd = pbuf->in_avail();

	/*string tempZipFilename = zxaFilename.toStdString();
	ZipIO zipfile(tempZipFilename.c_str());
	zipfile.appendZipFile(tempFileInfo.completeBaseName().toStdString().append(".txt").c_str());
	zipfile.writeBuffertoEntry(tmp.c_str(), tmp.size());
	zipfile.closeEntry();
	zipfile.closeZip();*/
	/*
	********
	*/




	/*===========================================================================*/
	GLA()->setHastoRefreshVBO();
	//GLA()->setHastoUpdatePartVBO();
	meshDoc()->setBusy(false);
	/*end*/
}

void MainWindow::testFuncFunc()
{


	/*========================================================================================
	test quternion
	========================================================================================*/
	//RichParameterSet printParamt;
	////printParamt
	//executeFilter(PM.actionFilterMap.value("FP_Test_Quaternion"), printParamt, false);
	/*
	========================================================================================
	========================================================================================
	*/


	/*
	test witness bar
	*/
	/*RichParameterSet printParam;
	executeFilter(PM.actionFilterMap.value("FP_TEST_WITNESS_BAR"), printParam, false);*/




	/*Test 7zip*/
	//QProcess process;
	//bool ret = false;
	//bool ret2 = false;
	///*extract file*/
	//QStringList arguments;
	//arguments.append("a");
	//arguments.append("C:/Users/TB495076/Desktop/TempTemp/zx/test.7z");
	//arguments.append("C:/Users/TB495076/Desktop/TempTemp/zx/output20191006_18h15m54s.zx");	

	//int iReturnCode = QProcess::execute("7z", arguments);


	/*get file name*/
	/*arguments.clear();
	arguments.append("l");
	arguments.append(zxaAFilename);
	process.start("7z", arguments);
	process.waitForFinished();*/





	//EntryNames savename;
	//string zxaFilenames = "C:/Users/TB495076/Desktop/TempTemp/zxa/testttttttt.zxa";
	//ZipIO zip_zx_txt(zxaFilenames.c_str());//zxa file path
	//string extract_zxa_dirName = "C:/Users/TB495076/Desktop/TempTemp/zxa/testttttttt/";
	//if (zip_zx_txt.extract2File(extract_zxa_dirName.c_str(), &savename) == 0)
	//{
	//	return;//extract position
	//}
	//zip_zx_txt.closeZip();


	/*========================================================================================
	test save snap shot
	========================================================================================*/


	//PrintjobParam pjpGenZX;
	//pjpGenZX.setCommonPrintValue(meshDoc());
	//pjpGenZX.setGenZxParam(true);
	//pjpGenZX.genDebugPic(false);//Test
	//pjpGenZX.setMono(false);
	//pjpGenZX.setPrintStart(true);
	//pjpGenZX.setStartPrintEstimate(true);
	//pjpGenZX.setColorProfile(currentGlobalParams.getString("COLOR_PROFILE"));
	//pjpGenZX.setStiffPrint(currentGlobalParams.getBool("STIFF_PRINT"));
	//pjpGenZX.setDilateBinder(currentGlobalParams.getBool("DILATE_BINDER"));
	//pjpGenZX.setDialteBinderValue(currentGlobalParams.getInt("DILATE_BINDER_VALUE"));
	//pjpGenZX.setDynamicWipe(currentGlobalParams.getBool("DYNAMIC_WIPE"));
	//pjpGenZX.setColorBinding(currentGlobalParams.getBool("COLOR_BINDING"));
	//executeFilter(PM.actionFilterMap.value("FP_SAVE_DEFAULT_SNAP_SHOT"), pjpGenZX.getprintJobParam(), false);
	/*
	========================================================================================
	========================================================================================
	*/

	/*========================================================================================
	test seperate
	========================================================================================*/
	RichParameterSet printParam;
	executeFilter(PM.actionFilterMap.value("FP_SEPERATE_TEST"), printParam, false);
	/*
	========================================================================================
	========================================================================================
	*/

	/*================================
	test read Param get Param
	=================================*/
	/*GenPrintInfo::genAllParamInfo(currentGlobalParams);
	RichParameterSet tempParam;
	GenPrintInfo::getParamfromJsonFile(tempParam);*/

	/*
	========================================
	*/
	Point3f x;
	x.Zero();
	x.X() = 1;




	/*
	test bch tree
	*/

	/*int detail = 8;
	QVector*/






	//meshDoc()->logger.ClearOutputStreams();//***delete log and cannot write, need to AddOutputStream again
	//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("Program starting"));
	//***logTest
	/*QString path = QStandardPaths::locate(QStandardPaths::DocumentsLocation, "", QStandardPaths::LocateDirectory) + "picasso" + "/log.log";
	QByteArray byteArray = path.toLocal8Bit();
	char *logchar = byteArray.data();
	meshDoc()->Log.Save(0, logchar);*/
	//***logTest

	//***start***color_estimate_test
	//	RichParameterSet printParam;
	//	printParam.addParam(new RichInt("print_dpi", 300, "dpi", ""));
	//	printParam.addParam(new RichFloat("slice_height", meshDoc()->p_setting.getbuildheight().value, "height_cm", ""));
	//	//printParam.addParam(new RichFloat("slice_height", 0.1, "height_cm", ""));
	//
	//	printParam.addParam(new RichBool("OL_Image", 0, "save_Outline_Image", ""));//輪廓
	//	printParam.addParam(new RichBool("Cap_Image", 0, "save_cap_Image", ""));	//膠水圖
	//
	//	printParam.addParam(new RichBool("BLK_OUT_LINE", 0, "BLK_OUT_LINE", ""));
	//	printParam.addParam(new RichBool("FaceColor", true, "FaceColor", ""));
	//
	//	printParam.addParam(new RichFloat("useless_print", 0, "useless_print_des", ""));//犧牲條寬度
	//	printParam.addParam(new RichBool("generate_zx", true, "generate_zx_or_not", ""));//
	//	printParam.addParam(new RichBool("generate_final_picture", 1, "generate_final_picture", ""));//
	//	printParam.addParam(new RichBool("mono_bool", false, "mono_bool", ""));//***20160322mono_bool
	//
	//
	//	printParam.addParam(new RichInt("start_page", 0, "start print pages"));
	//
	//	int boxy_dim = qCeil((meshDoc()->bbox().max.Z() - meshDoc()->bbox().min.Z()) / meshDoc()->p_setting.getbuildheight().value) + 1;
	//	printParam.addParam(new RichInt("end_page", boxy_dim, "end print pages"));
	//
	//
	//	printParam.addParam(new RichInt("per_zx_job", 10000, "number of zx pages", ""));//多少頁一個job
	//	printParam.addParam(new RichInt("send_page_num", 5, "send_page_num"));//幾頁send 一次
	//
	//	/*printParam.addParam(new RichInt("plus_pixel", 2308, "plus_pixel"));
	//	printParam.addParam(new RichInt("PLUS_MM", 19, "PLUS_MM"));*/
	//
	//	printParam.addParam(new RichBool("longpage_orshortpage", 0, "longpage_orshortpage", ""));
	//	qDebug() << "meshDoc()->p_setting.print_pagesize" << meshDoc()->p_setting.print_pagesize;
	//	if (meshDoc()->p_setting.print_pagesize == slice_program_setting::page_size::longPage_size) {
	//		//printParam.addParam(new RichInt("plus_pixel", 2203, "plus_pixel"));//(40-22*0.97)*300/DSP_inchmm
	//		//printParam.addParam(new RichInt("PLUS_MM", 18, "PLUS_MM"));//40-22//steady function
	//#if 0
	//		//bb2
	//		printParam.addParam(new RichInt("plus_pixel", 2126, "plus_pixel"));//(40-22)*300/DSP_inchmm
	//		printParam.addParam(new RichFloat("PLUS_MM", 18, "PLUS_MM"));//40-22//steady function
	//#else
	//		//bb3
	//		//printParam.addParam(new RichInt("plus_pixel", 1571, "plus_pixel"));//(35.3-22)*300/DSP_inchmm
	//		//printParam.addParam(new RichFloat("PLUS_MM", 13.3, "PLUS_MM"));//35.3-22//steady function
	//		//bb3 22.2
	//		printParam.addParam(new RichInt("plus_pixel", 1547, "plus_pixel"));//(35.3-22.2)*300/DSP_inchmm
	//		printParam.addParam(new RichFloat("PLUS_MM", 13.1, "PLUS_MM"));//35.3-22//steady function
	//#endif
	//
	//		printParam.setValue("longpage_orshortpage", BoolValue(true));
	//	}
	//	else if (meshDoc()->p_setting.print_pagesize == slice_program_setting::page_size::printBed_size) {
	//
	//		printParam.addParam(new RichInt("plus_pixel", 0, "plus_pixel"));
	//		printParam.addParam(new RichFloat("PLUS_MM", 0, "PLUS_MM"));
	//		printParam.setValue("longpage_orshortpage", BoolValue(false));
	//	}
	//
	//	printParam.addParam(new RichBool("PRINT_START", meshDoc()->p_setting.get_print_or_not(), "START PRINTING", ""));
	//	printParam.addParam(new RichFloat("USELESS_PRINT", meshDoc()->p_setting.get_useless_print(), "useless_print", ""));
	//
	//
	//
	//
	//	executeFilter(PM.actionFilterMap.value("FP_ESTIMATE_COLOR"), printParam, false);

	//float  C = printParam.getInt("holeNum");
	//float  M = printParam.getInt("holeNum");
	//float  Y



	//***test translator
	/*QTranslator translator;
	qApp->removeTranslator(&translator);
	translator.load("picasso_ja");
	qApp->instance()->installTranslator(&translator);*/
	//this->retrans
	//updateMenus();

	//***end color_estimate_test

	//RichParameterSet tt;
	////executeFilter(PM.actionFilterMap.value("FP_LANDING_BY_IMAGE"), tt, false);
	////executeFilter(PM.actionFilterMap.value("FP_LANDING_BY_IMAGE_Z_AXIS"), tt, false);
	//executeFilter(PM.actionFilterMap.value("FP_LANDING_BY_IMAGE_X_AXIS"), tt, false);
	//executeFilter(PM.actionFilterMap.value("FP_LANDING_BY_IMAGE_Y_AXIS"), tt, false);


	/*RichParameterSet dummyParSet;
	dummyParSet.addParam(new RichBool("collision_detected", false, "", ""));
	dummyParSet.addParam(new RichBool("all_or_not", false, "", ""));
	dummyParSet.setValue("collision_detected", BoolValue(true));
	dummyParSet.setValue("all_or_not", BoolValue(false));
	executeFilter(PM.actionFilterMap.value("Landing_Tool"), dummyParSet, false);*/

	//***test depth image
	RichParameterSet tt;
	//executeFilter(PM.actionFilterMap.value("FP_GET_DEPTH_IMAGE"), tt, false);

	//executeFilter(PM.actionFilterMap.value("FP_GET_DEPTH_LANDING_TEST2"), tt, false);//
	//executeFilter(PM.actionFilterMap.value("FP_TEST_SEARCH_SPACE_ALL_IMAGE"), tt, false);//
	//meshDoc()->groove.setGrooveHit(SKT::detect_hit_groove<Scalarm>(*meshDoc()));
	//GLA()->reFreshGlListlist();
	//===Test generate mesh from gray image
	//executeFilter(PM.actionFilterMap.value("FP_TEST_GENERATE_POINTS_FROM_GRAY_IMAGE_FLOAT"), currentGlobalParams, false);//

	/****************
	Test generate volumn mesh
	*/
	//executeFilter(PM.actionFilterMap.value("FP_MESH_DELAUNAY_TRIANGULATION"), currentGlobalParams, false);//
	//executeFilter(PM.actionFilterMap.value("FP_MESH_INTERSECT_TEST2"), currentGlobalParams, false);//
	/**********/



	/*
	test packing
	*/

	/*test BVH all rotation*/
	//int detail = pow(2, 8.);
	//Point3f xsef = SKT::Axis::AxisVector(SKT::Axis::AxisXYZ::AxisX);
	//meshDoc()->setBusy(true);
	//SKT::Rotations.clear();
	//SKT::initRotations_zAxis();

	//QVector<BVHTrees> testTrees;
	////BVHTrees testTrees2;
	//foreach(MeshModel *mx, meshDoc()->meshList)
	//{
	//	testTrees.push_back( SKT::GroupMesh::Addd(mx, 8, 0));


	//	for (int i = 0; i < 5; i++)
	//	{
	//		MeshModel* createm = meshDoc()->addNewMesh("", "temp", false);

	//		for (int j = testTrees.last()[i].size() - 256; j < 511; j++)
	//		{
	//			BVHTree tree(testTrees.last().at(i));
	//			if (tree.at(j).IsNull())
	//				continue;
	//			//qDebug()<<"bvh"<<count++;
	//			MeshModel* mmm = meshDoc()->addNewMesh("", "tempp", false);
	//			tri::Box<CMeshO>(mmm->cm, tree[j]);
	//			//tri::Box<CMeshO>(mmm->cm, box);
	//			//mmm->UpdateBoxAndNormals();
	//			vcg::tri::Append<CMeshO, CMeshO>::Mesh(createm->cm, mmm->cm);
	//			meshDoc()->delMesh(mmm);
	//		}
	//		createm->setMeshSort(MeshModel::meshsort::print_item);


	//		createm->Enable(vcg::tri::io::Mask::IOM_VERTCOLOR);
	//		createm->updateDataMask(MeshModel::MM_VERTCOLOR);
	//		createm->UpdateBoxAndNormals();
	//		createm->Enable(tri::io::Mask::IOM_VERTCOLOR);
	//		createm->updateDataMask(MeshModel::MM_VERTCOLOR);
	//	}
	//}

	//bool intersection = SKT::treeIntersects(testTrees[0].at(0), testTrees[1].at(0), Point3f(0, 0, 0), Point3f(40, 0, 0));



	/*test Rotations*/
	//int detail = pow(2, 8.);
	//Point3f xsef = SKT::Axis::AxisVector(SKT::Axis::AxisXYZ::AxisX);
	//meshDoc()->setBusy(true);
	//SKT::Rotations.clear();
	//SKT::initRotations();



	//foreach(Matrix44m transformmm, SKT::Rotations)
	//{
	//	MeshModel *mp = meshDoc()->addNewMesh("", "tempp", false);

	//	
	//	//MeshModel *mp = md;
	//	vcg::tri::Append<CMeshO, CMeshO>::MeshCopy(mp->cm, meshDoc()->mm()->cm);
	//	//vcg::tri::Append<CMeshO, CMeshO>::Mesh(mp->cm, meshDoc()->mm()->cm);
	//	Matrix44m toCenter;
	//	Point3m translatePath;
	//	translatePath.SetZero();
	//	translatePath = Point3m(0, 0, 0) - mp->cm.bbox.Center();
	//	toCenter.SetTranslate(translatePath);
	//	tri::UpdatePosition<CMeshO>::Matrix(mp->cm, toCenter, true);
	//	mp->setMeshSort(MeshModel::meshsort::print_item);
	//	mp->UpdateBoxAndNormals();

	//	tri::UpdatePosition<CMeshO>::Matrix(mp->cm, transformmm, true);
	//	
	//}



	//GLA()->setHastoRefreshVBO();
	//meshDoc()->setBusy(false);



	/*
	test BVHMesh
	*/
	//QVector<Box3m> tree;

	//tree = SKT::newTreeForMesh(meshDoc(),meshDoc()->mm(), 8);
	//int detail = pow(2, 8.);

	//MeshModel* createm = meshDoc()->addNewMesh("", "temp");
	//
	//int count = 0;
	////foreach(Box3m box, tree)
	//for (int i = tree.size()-detail; i < 511; i++)
	//{	
	//	/*if (box.IsNull())
	//		continue;*/
	//	if (tree[i].IsNull())
	//		continue;
	//	//qDebug()<<"bvh"<<count++;
	//	MeshModel* mmm = meshDoc()->addNewMesh("", "tempp");
	//	tri::Box<CMeshO>(mmm->cm, tree[i]);
	//	//tri::Box<CMeshO>(mmm->cm, box);
	//	//mmm->UpdateBoxAndNormals();
	//	vcg::tri::Append<CMeshO, CMeshO>::Mesh(createm->cm,mmm->cm );
	//	meshDoc()->delMesh(mmm);
	//}
	///*Box3m box;
	//tri::Box<CMeshO>(createm->cm, box);*/
	//createm->setMeshSort(MeshModel::meshsort::print_item);
	//

	//createm->Enable(vcg::tri::io::Mask::IOM_VERTCOLOR);
	//createm->updateDataMask(MeshModel::MM_VERTCOLOR);
	//createm->UpdateBoxAndNormals();
	//createm->Enable(tri::io::Mask::IOM_VERTCOLOR);
	//createm->updateDataMask(MeshModel::MM_VERTCOLOR);


	//GLA()->setHastoRefreshVBO();
	//meshDoc()->setBusy(false);



	/*Box3m major(Point3m(-5, -5, -5), Point3m(5, 5, 5));
	Box3m minor(Point3m(-1, -1, -1), Point3m(6, 6, 6));
	Box3m intersectionn = major.Intersection(minor);
	int x = 23;*/
	/*******************************************/



	/****************Test auto packing */
	//RichParameterSet dummy;
	//dummy.addParam(new RichBool("auto_rotate_or_not", false, "auto_rotate", ""));
	//executeFilter(PM.actionFilterMap.value("Auto Packing"), dummy, false);//
	/**********/

	//===***test start page
	//executeFilter(PM.actionFilterMap.value("FP_MESH_INTERSECT_TEST2"), currentGlobalParams, false);//
	//===***endtest start page

	//===***test webService
	/*webService testweb;
	testweb.downloadAPI();*/

	//===***end test webService
	//==***test copy***

	/*CopyDialog testcopy;
	testcopy.exec();
	int test = testcopy.getcopyNum();
	for (int i = 0; i < test; i++)
	pasteSelectMeshes();*/

	/*GDPRDialog gdpr;
	int dialogCode = gdpr.exec();*/
	//==***end test copy***


	//OpWidget pppp(this);
	////pppp.setModal(true);
	//pppp.show();


	//foreach(MeshModel *tmm, meshDoc()->meshList)
	//{
	//	CMeshO::FaceIterator fid;
	//	//size_t size = 0;
	//	for (fid = tmm->cm.face.begin(); fid != tmm->cm.face.end(); ++fid)
	//	{
	//		for (int n = 0; n < 3; n++)
	//		{
	//			vertices_test.push_back((*fid).P(n).X());
	//			vertices_test.push_back((*fid).P(n).Y());
	//			vertices_test.push_back((*fid).P(n).Z());
	//			normal_test.push_back((*fid).N().X());
	//			normal_test.push_back((*fid).N().Y());
	//			normal_test.push_back((*fid).N().Z());
	//		}
	//	}
	//}

	//======================start testvbo
	//vboTest ^= 1;

	//foreach(MeshModel *tmm, meshDoc()->meshList)
	//{


	//	if (tmm->getMeshSort() == MeshModel::meshsort::print_item)
	//	{
	//		//tmm->rmm.colorMode = GLW::CMNone;
	//		if (tmm->glw.TMIdd[0].size() < 2)
	//		{
	//			tmm->updateDataMask(MeshModel::MM_FACEFACETOPO);
	//			tmm->updateDataMask(MeshModel::MM_VERTFACETOPO);
	//			tri::UpdateNormal<CMeshO>::PerVertexFromCurrentFaceNormal(tmm->cm);
	//			//tri::UpdateNormal<CMeshO>::PerVertex(tmm->cm);
	//			vcg::tri::Allocator<CMeshO>::CompactFaceVector(tmm->cm);
	//			vcg::tri::Allocator<CMeshO>::CompactVertexVector(tmm->cm);
	//			vcg::tri::UpdateTopology<CMeshO>::FaceFace(tmm->cm);
	//			vcg::tri::UpdateTopology<CMeshO>::VertexFace(tmm->cm);

	//			if (vboTest)
	//			{
	//				tmm->glw.SetHint(GLW::Hint::HNUseVBO);
	//				tmm->glw.initVBOUpdate(GLW::DMSmooth, tmm->rmm.colorMode, tmm->rmm.textureMode);
	//			}
	//			else
	//			{

	//				tmm->glw.ClearHint(GLW::Hint::HNUseVBO);
	//			}

	//			tmm->glw.initVBOUpdate(GLW::DMSmooth, tmm->rmm.colorMode, tmm->rmm.textureMode);
	//		}
	//		else
	//		{
	//			return;
	//		}
	//	}
	//	//HasTexCoord
	//}
	//======================end testvbo
	//std::vector<TexCoord2f> vertTex;
	//int count = 0;
	//foreach(MeshModel *tmm, meshDoc()->meshList)
	//{
	//	CMeshO::VertexIterator vi;
	//	for (vi = tmm->cm.vert.begin(); vi != tmm->cm.vert.end(); vi++)
	//	{
	//		//vcg::tri::Index(mm->cm, (*fi).V(k))l
	//		//vi->T
	//		//vid->IsNull
	//		//he.VInd
	//		//Color4b a = vid->C();
	//		//vid->cVFp()->cWT(0)
	//		//bool y = vid->is();
	//		
	//		CFaceO * one_face = vi->VFp();
	//		//one_face->V0()
	//		if (one_face != NULL)
	//		{
	//			//one_face->V(i)->C().W()

	//			face::Pos<CFaceO> he((*vi).VFp(), &*vi);
	//			TexCoord2f test = vi->VFp()->cWT(he.VInd());
	//			
	//			if ((test.U() !=0 )&& (test.V()!=0))
	//				vertTex.push_back(vi->VFp()->cWT(he.VInd()));
	//			else
	//			{
	//				vertTex.push_back(vcg::TexCoord2f(-1., -1.));
	//			}
	//			
	//		}
	//		//else
	//		//qDebug()<<count++;
	//		//vid = tmm->cm.vert.begin();
	//		bool x = vi->HasTexCoord();
	//	}
	//}
	//===========================================================================

	//bool allorselected = true;

	//======start print job history test==================================================================
	//PrintjobParam pjpGenZX;
	//pjpGenZX.setCommonPrintValue(meshDoc());
	//pjpGenZX.setGenZxParam(false);
	//pjpGenZX.setMono(false);
	//pjpGenZX.setPrintStart(false);
	//if (meshDoc()->multiSelectID.size() < 1)
	//{
	//	foreach(MeshModel *mp, meshDoc()->meshList)
	//	{
	//		meshDoc()->multiSelectID.insert(mp->id());
	//	}
	//}
	//RichParameterSet tempTest;
	//tempTest.copy(pjpGenZX.getprintJobParam());
	//executeFilter(PM.actionFilterMap.value("FP_ESTIMATE_COLOR2"), tempTest, false);
	////executeFilter(PM.actionFilterMap.value("FP_ESTIMATE_SURFACE_COLOR"), tempTest, false);
	////
	////if ()
	//cmyusage[0] = 0.;
	//cmyusage[1] = 0.;
	//cmyusage[2] = 0.;
	//cmyusage[4] = 0.;
	//cmyusage[5] = 0.;
	//cmyusage[6] = 0.;
	//if (tempTest.hasParameter("CUsage") && tempTest.hasParameter("MaintYUsage"))
	//{
	//	cmyusage[0] = tempTest.getFloat("CUsage");
	//	cmyusage[1] = tempTest.getFloat("MUsage");
	//	cmyusage[2] = tempTest.getFloat("YUsage");
	//	cmyusage[3] = tempTest.getFloat("MaintCUsage");
	//	cmyusage[4] = tempTest.getFloat("MaintMUsage");
	//	cmyusage[5] = tempTest.getFloat("MaintYUsage");
	//	//***end color_estimate_test
	//	//PrinterJobInfo *pji = new PrinterJobInfo(this, meshDoc(), cmyusage, PrinterJobInfo::printInfoMode::PrintOptionMode);
	//	PrinterJobInfo *pji = new PrinterJobInfo(this, meshDoc(), cmyusage, PrinterJobInfo::printInfoMode::EstimateMode);
	//	pji->genCSVFile();
	//	pji->writeAtLastLine();
	//	//pji->exec();
	//}
	//======end print job history test==================================================================
	//QStringList arguments;

	//
	//QString ppath = "PrintJogHistory.exe";// "cd /d " + PicaApplication::getRoamingDir();
	////arguments << "/k" << "cd /d d:/temptemp/zx_file" << "&" << "usbsend.exe" << "output.zx";						   
	//arguments << "/k" << ppath;// << "&&" << "PrintJogHistory.exe";
	//QProcess::startDetached("cmd", arguments);
	//
	/*QProcess process(this);
	QStringList arguments;
	arguments << "C:/Users/tb495076/Documents/BCPware/printInfoCSV.csv";
	QString csvFile = "PrintJogHistory.exe C:/Users/tb495076/Documents/BCPware/printInfoCSV.csv";
	process.startDetached("PrintJogHistory.exe", arguments);*/


	/*
	Count binder area
	*/
	//PrintjobParam pjpGenZX;
	//pjpGenZX.setCommonPrintValue(meshDoc());
	//pjpGenZX.setGenZxParam(true);
	//pjpGenZX.genDebugPic(false);//Test
	////pjpGenZX.setMono(printOption_.getMonoParam());
	//pjpGenZX.setPrintStart(true);
	//pjpGenZX.setStartPrintEstimate(true);
	//pjpGenZX.setColorProfile(currentGlobalParams.getString("COLOR_PROFILE"));
	//pjpGenZX.setStiffPrint(currentGlobalParams.getBool("STIFF_PRINT"));
	//pjpGenZX.setDilateBinder(currentGlobalParams.getBool("DILATE_BINDER"));
	//pjpGenZX.setDialteBinderValue(currentGlobalParams.getInt("DILATE_BINDER_VALUE"));
	//pjpGenZX.setDynamicWipe(currentGlobalParams.getBool("DYNAMIC_WIPE"));
	//pjpGenZX.setColorBinding(currentGlobalParams.getBool("COLOR_BINDING"));
	//executeFilter(PM.actionFilterMap.value("FP_COUNT_BINDER_AREA"), pjpGenZX.getprintJobParam(), false);
	/*end*/


	/*=======================================
	Simulated Annealing_Volume
	==========================================*/
	//#if 0
	//	meshDoc()->setBusy(true);
	//	SKT::Rotations.clear();
	//	SKT::initRotations();
	//	//SKT::initRotations_zAxis();
	//	int bvhDetail = 8;
	//	int annealingIterations = 5000;
	//	int count = 1;
	//	double totalVolume = 0, side;
	//	SKT::GroupMesh *GroupMeshTest1 = SKT::newGroupMesh();
	//	//for (int i = 1; i < 3; i++)
	//	srand(time(NULL));
	//	foreach(MeshModel *mmm, meshDoc()->meshList)
	//	{
	//
	//		//MeshModel *mmm = meshDoc()->getMesh(i);
	//		GroupMeshTest1->Add(mmm, bvhDetail, count, SKT::GroupMesh::packing_Parameter::Volume_pa);
	//		totalVolume += mmm->cm.bbox.Volume();
	//		//totalVolume += mmm->cm.bbox.DimZ();
	//	}
	//
	//
	//	side = std::pow(totalVolume, 1.0 / 3);
	//	GroupMeshTest1->deviation = side / 32;
	//
	//
	//	SKT::GroupMesh goodone = GroupMeshTest1->pack(annealingIterations,QCallBack/*, NULL*/,SKT::GroupMesh::packing_Parameter::Volume_pa).Copy();
	//	qDebug() << "Volume" << goodone.Volume();
	//	qDebug()<<"outenergy"<<goodone.Energy();
	//
	//	foreach(SKT::Item *ii, goodone.items)
	//	{
	//
	//		MeshModel *mx = ii->meshItem;
	//
	//		Matrix44m toCenter;
	//		Point3m translatePath;
	//		translatePath.SetZero();
	//		translatePath = Point3m(0, 0, 0) - mx->cm.bbox.Center();
	//		toCenter.SetTranslate(translatePath);
	//		tri::UpdatePosition<CMeshO>::Matrix(mx->cm, toCenter, true);
	//		tri::UpdateBounding<CMeshO>::Box(mx->cm);
	//
	//
	//		tri::UpdatePosition<CMeshO>::Matrix(mx->cm, SKT::Rotations[ii->rotationNum], true);
	//
	//		xyz::mesh_translate(mx->cm, ii->translation_mesh);
	//
	//		//SKT::Rotations[ii->rotationNum];
	//	}
	//
	//	Point3f movetocenter = Point3m(0, 0, 0) - (goodone.BoundingBox().min);
	//
	//	foreach(SKT::Item *ii, goodone.items)
	//	{
	//
	//		MeshModel *mx = ii->meshItem;		
	//		xyz::mesh_translate(mx->cm, movetocenter);	
	//	}
	//	//rotate to lowest height
	//
	//	qb->reset();
	//#else
	//	/*
	//	Simulated Annealing_Volume
	//	*/
	//	meshDoc()->setBusy(true);
	//	SKT::Rotations.clear();
	//	SKT::initRotations_zAxis();
	//	//SKT::initRotations_zAxis();
	//	int bvhDetail = 8;
	//	int annealingIterations = 5000;
	//	int count = 1;
	//	double totalVolume = 0, side;
	//	SKT::GroupMesh *GroupMeshTest1 = SKT::newGroupMesh();
	//	//for (int i = 1; i < 3; i++)
	//	srand(time(NULL));
	//	foreach(MeshModel *mmm, meshDoc()->meshList)
	//	{
	//
	//		//MeshModel *mmm = meshDoc()->getMesh(i);
	//		GroupMeshTest1->Add(mmm, bvhDetail, count, SKT::GroupMesh::packing_Parameter::Z_Axis_volume);
	//
	//		totalVolume += 0.7*mmm->cm.bbox.DimZ()*mmm->cm.bbox.DimZ()*mmm->cm.bbox.DimZ() + 0.3*mmm->cm.bbox.Volume();
	//		//totalVolume += mmm->cm.bbox.DimZ()*mmm->cm.bbox.DimZ() + mmm->cm.bbox.DimX()*mmm->cm.bbox.DimY();
	//	}
	//
	//	side = std::pow(totalVolume, 1.0 / 3);
	//	GroupMeshTest1->deviation = side / 32;
	//
	//
	//	SKT::GroupMesh goodone = GroupMeshTest1->pack(annealingIterations, QCallBack/*, NULL*/, SKT::GroupMesh::packing_Parameter::Z_Axis_volume).Copy();
	//
	//
	//	/*foreach(SKT::Item *ii, GroupMeshTest1->items)
	//	{
	//	MeshModel *mx = ii->meshItem;
	//
	//	Matrix44m toCenter;
	//	Point3m translatePath;
	//	translatePath.SetZero();
	//	translatePath = Point3m(0, 0, 0) - mx->cm.bbox.Center();
	//	toCenter.SetTranslate(translatePath);
	//	tri::UpdatePosition<CMeshO>::Matrix(mx->cm, toCenter, true);
	//	tri::UpdateBounding<CMeshO>::Box(mx->cm);
	//
	//
	//	tri::UpdatePosition<CMeshO>::Matrix(mx->cm, -SKT::Rotations[ii->rotationNum], true);
	//	tri::UpdateBounding<CMeshO>::Box(mx->cm);
	//	xyz::mesh_translate(mx->cm, ii->translation_mesh);
	//	}*/
	//
	//	/*Point3f movetocenter = Point3m(0, 0, 0) - (GroupMeshTest1->BoundingBox().min);
	//
	//	foreach(SKT::Item *ii, GroupMeshTest1->items)
	//	{
	//
	//	MeshModel *mx = ii->meshItem;
	//	xyz::mesh_translate(mx->cm, movetocenter);
	//	}*/
	//
	//	foreach(SKT::Item *ii, goodone.items)
	//	{
	//
	//		MeshModel *mx = ii->meshItem;
	//
	//		Matrix44m toCenter;
	//		Point3m translatePath;
	//		translatePath.SetZero();
	//		translatePath = Point3m(0, 0, 0) - mx->cm.bbox.Center();
	//		toCenter.SetTranslate(translatePath);
	//		tri::UpdatePosition<CMeshO>::Matrix(mx->cm, toCenter, true);
	//		tri::UpdateBounding<CMeshO>::Box(mx->cm);
	//
	//
	//		tri::UpdatePosition<CMeshO>::Matrix(mx->cm, SKT::Rotations[ii->rotationNum], true);
	//		tri::UpdateBounding<CMeshO>::Box(mx->cm);
	//		xyz::mesh_translate(mx->cm, ii->translation_mesh);
	//	}
	//
	//	Point3f movetocenter = Point3m(0, 0, 0) - (goodone.BoundingBox().min);
	//
	//	foreach(SKT::Item *ii, goodone.items)
	//	{
	//
	//		MeshModel *mx = ii->meshItem;
	//		xyz::mesh_translate(mx->cm, movetocenter);
	//	}
	//	//rotate to lowest height
	//
	//	qb->reset();
	//#endif
	//================================================

	GLA()->setHastoRefreshVBO();
	//GLA()->setHastoUpdatePartVBO();
	meshDoc()->setBusy(false);
	/*end*/

}

void MainWindow::testFuncFunc2()
{
	//***logTest
	//	QString path = QStandardPaths::locate(QStandardPaths::DocumentsLocation, "", QStandardPaths::LocateDirectory) + "picasso" + "/log.log";
	//	QByteArray byteArray = path.toLocal8Bit();
	//	char *logchar = byteArray.data();
	//	meshDoc()->Log.Save(0, logchar);
	//	//***logTest
	//
	//
	//	//***start***color_estimate_test   
	//	RichParameterSet printParam;
	//	printParam.addParam(new RichInt("print_dpi", 300, "dpi", ""));
	//	printParam.addParam(new RichFloat("slice_height", meshDoc()->p_setting.getbuildheight().value, "height_cm", ""));
	//	//printParam.addParam(new RichFloat("slice_height", 0.1, "height_cm", ""));
	//
	//	printParam.addParam(new RichBool("OL_Image", 0, "save_Outline_Image", ""));//輪廓
	//	printParam.addParam(new RichBool("Cap_Image", 0, "save_cap_Image", ""));	//膠水圖
	//
	//	printParam.addParam(new RichBool("BLK_OUT_LINE", 0, "BLK_OUT_LINE", ""));
	//	printParam.addParam(new RichBool("FaceColor", true, "FaceColor", ""));
	//
	//	printParam.addParam(new RichFloat("useless_print", 0, "useless_print_des", ""));//犧牲條寬度
	//	printParam.addParam(new RichBool("generate_zx", true, "generate_zx_or_not", ""));//
	//	printParam.addParam(new RichBool("generate_final_picture", 1, "generate_final_picture", ""));//
	//	printParam.addParam(new RichBool("mono_bool", false, "mono_bool", ""));//***20160322mono_bool
	//
	//
	//	printParam.addParam(new RichInt("start_page", 0, "start print pages"));
	//
	//	int boxy_dim = qCeil((meshDoc()->bbox().max.Z() - meshDoc()->bbox().min.Z()) / meshDoc()->p_setting.getbuildheight().value) + 1;
	//	printParam.addParam(new RichInt("end_page", boxy_dim, "end print pages"));
	//
	//
	//	printParam.addParam(new RichInt("per_zx_job", 10000, "number of zx pages", ""));//多少頁一個job
	//	printParam.addParam(new RichInt("send_page_num", 5, "send_page_num"));//幾頁send 一次
	//
	//
	//
	//	/*printParam.addParam(new RichInt("plus_pixel", 2308, "plus_pixel"));
	//	printParam.addParam(new RichInt("PLUS_MM", 19, "PLUS_MM"));*/
	//
	//	printParam.addParam(new RichBool("longpage_orshortpage", 0, "longpage_orshortpage", ""));
	//	qDebug() << "meshDoc()->p_setting.print_pagesize" << meshDoc()->p_setting.print_pagesize;
	//	if (meshDoc()->p_setting.print_pagesize == slice_program_setting::page_size::longPage_size) {
	//		//printParam.addParam(new RichInt("plus_pixel", 2203, "plus_pixel"));//(40-22*0.97)*300/DSP_inchmm
	//		//printParam.addParam(new RichInt("PLUS_MM", 18, "PLUS_MM"));//40-22//steady function
	//#if 0
	//		//bb2
	//		printParam.addParam(new RichInt("plus_pixel", 2126, "plus_pixel"));//(40-22)*300/DSP_inchmm
	//		printParam.addParam(new RichFloat("PLUS_MM", 18, "PLUS_MM"));//40-22//steady function
	//#else
	//		//bb3
	//		//printParam.addParam(new RichInt("plus_pixel", 1571, "plus_pixel"));//(35.3-22)*300/DSP_inchmm
	//		//printParam.addParam(new RichFloat("PLUS_MM", 13.3, "PLUS_MM"));//35.3-22//steady function
	//		//bb3 22.2
	//		printParam.addParam(new RichInt("plus_pixel", 0, "plus_pixel"));//(35.3-22.2)*300/DSP_inchmm
	//		printParam.addParam(new RichFloat("PLUS_MM", 0, "PLUS_MM"));//35.3-22//steady function
	//#endif
	//
	//		printParam.setValue("longpage_orshortpage", BoolValue(true));
	//	}
	//	else if (meshDoc()->p_setting.print_pagesize == slice_program_setting::page_size::printBed_size) {
	//
	//		printParam.addParam(new RichInt("plus_pixel", 0, "plus_pixel"));
	//		printParam.addParam(new RichFloat("PLUS_MM", 0, "PLUS_MM"));
	//		printParam.setValue("longpage_orshortpage", BoolValue(false));
	//	}
	//
	//	printParam.addParam(new RichBool("PRINT_START", meshDoc()->p_setting.get_print_or_not(), "START PRINTING", ""));
	//	printParam.addParam(new RichFloat("USELESS_PRINT", meshDoc()->p_setting.get_useless_print(), "useless_print", ""));
	//
	//
	//
	//
	//	executeFilter(PM.actionFilterMap.value("FP_ESTIMATE_COLOR2"), printParam, false);
	//***end color_estimate_test
	/*
	BIN PCKING TEST
	*/
	using namespace BINPACK;
	const int sizeX = 80;
	const int sizeY = 50;
	const int sizeZ = 45;

	const double  S = 100;
	const double P = 2.5;

	int score = 1;

	BPRotations.clear();
	initBPRotations();

	meshDoc()->setBusy(true);

	//foreach(Matrix44m transformmm, BPRotations)
	//{
	//	MeshModel *mp = meshDoc()->addNewMesh("", "tempp", false);
	//	
	//	//MeshModel *mp = md;
	//	vcg::tri::Append<CMeshO, CMeshO>::MeshCopy(mp->cm, meshDoc()->mm()->cm);
	//	//vcg::tri::Append<CMeshO, CMeshO>::Mesh(mp->cm, meshDoc()->mm()->cm);
	//	Matrix44m toCenter;
	//	Point3m translatePath;
	//	translatePath.SetZero();
	//	translatePath = Point3m(0, 0, 0) - mp->cm.bbox.Center();
	//	toCenter.SetTranslate(translatePath);
	//	tri::UpdatePosition<CMeshO>::Matrix(mp->cm, toCenter, true);
	//	mp->setMeshSort(MeshModel::meshsort::print_item);
	//	mp->UpdateBoxAndNormals();
	//	tri::UpdatePosition<CMeshO>::Matrix(mp->cm, transformmm, true);
	//	
	//}
	//


	std::vector<BinItem> items;

	foreach(MeshModel *mx, meshDoc()->meshList)
	{
		int i = mx->id();
		Box3m box33 = mx->cm.bbox;
		BinBox boxbin(box33.Center(), box33.Size());

		for (int j = 0; j < BINPACK::BPRotations.size(); j++)
		{
			Matrix44m  m = BINPACK::BPRotations[j];
			int id = i*BINPACK::BPRotations.size() + j;
			Point3m s = box33.Transformbox_test(m).Size();
			int sx = int(std::ceil(s.X() + P * 2)*S);
			int sy = int(std::ceil(s.Y() + P * 2)*S);
			int sz = int(std::ceil(s.Z() + P * 2)*S);

			items.push_back(*new BINPACK::BinItem(id, score, Point3f(sx, sy, sz)));
		}
	}

	//box: = binpack.Box{ binpack.Vector{}, binpack.Vector{ SizeX * S, SizeY * S, SizeZ * S } }
	BinBox box2 = *new BinBox(Point3f(0, 0, 0), Point3f(sizeX*S, sizeY*S, sizeZ *S));

	Result<Placement> result;
	result = bin_pack<Placement>(items, box2);
	//result = ;

	for (int x = 0; x < result.Placements.size(); x++)
	{
		Placement placement = result.Placements[x];
		Point3f p = placement.position;


		Point3f path(p.X() / S + P, p.Y() / S + P, p.Z() / S + P);


		double mId = placement.item->ID / BPRotations.size();
		double j = placement.item->ID % BPRotations.size();

		MeshModel *cmmm = meshDoc()->getMesh(mId);

		MeshModel *mp = meshDoc()->addNewMesh("", "tempp", false);
		vcg::tri::Append<CMeshO, CMeshO>::MeshCopy(mp->cm, cmmm->cm);

		mp->setMeshSort(MeshModel::meshsort::print_item);
		mp->UpdateBoxAndNormals();

		//tri::UpdatePosition<CMeshO>::Matrix(mp->cm,BPRotations[j], true);
		//tri::UpdateBounding<CMeshO>::Box(mp->cm);


		/*Rotate*/
		Matrix44m translate_to_origin, translate_to_box_center;//***先回原點,再轉回原來位置
		Matrix44m rotate_m;//***20150420旋轉矩陣	
		//***20150827
		Matrix44m wholeTransformMatrix;

		translate_to_origin.SetTranslate(-mp->cm.bbox.Center());//***先回原點		
		translate_to_box_center.SetTranslate(mp->cm.bbox.Center());//***再回原位置
		wholeTransformMatrix = (translate_to_box_center * BPRotations[j] * translate_to_origin);
		mp->cm.Tr = wholeTransformMatrix * mp->cm.Tr;
		//***********************
		tri::UpdatePosition<CMeshO>::Matrix(mp->cm, wholeTransformMatrix, true);
		tri::UpdateBounding<CMeshO>::Box(mp->cm);




		/*
			Translate
			*/
		Box3m *m_box(&mp->cm.bbox);

		Point3f way = path - m_box->min;

		Matrix44m tranlate_to;
		tranlate_to.SetTranslate(way);
		mp->cm.Tr = tranlate_to *mp->cm.Tr;
		tri::UpdatePosition<CMeshO>::Matrix(mp->cm, tranlate_to, true);
		tri::UpdateBounding<CMeshO>::Box(mp->cm);
		//*********end*************




		//xyz::mesh_translate(mp->cm, ii->translation_mesh);
		//mp->
	}


	GLA()->setHastoRefreshVBO();
	meshDoc()->setBusy(false);
	/******BINPACKING*************/

	/**/
	//QLabel *label1 = new QLabel();
	//QPixmap image(":/images/SKT_images/help.png");	
	//label1->setPixmap(QPixmap::fromImage(QImage(":/images/icons/btn_menu_front.png")));
	////label1->setText("label1");
	//label1->show();
	/**/





}

//Sydny 01-19-2017 added show function menu
void MainWindow::showFunctionMenu()
{
	dock->close();
	//dock = createQDockWidget4();
	//dock->setStyleSheet(WidgetStyleSheet::dockWidget());
	//addDockWidget(Qt::LeftDockWidgetArea, dock);
	dock->show();
}
void MainWindow::supportdia()
{
	/*QDialog *support_dialog = new QDialog(0,  Qt::WindowCloseButtonHint | Qt::WindowTitleHint);
	Ui::SupportDia temp;
	temp.setupUi(support_dialog);
	support_dialog->show();
	*/
	//QDesktopServices::openUrl(QUrl("http://support.xyzprinting.com/tw_zh_tw/Help/ticket/home"));
	QDesktopServices::openUrl(QUrl("https://pro.xyzprinting.com/support/en-US/EndUser/Help"));
}
void MainWindow::contactSupport()
{
	QDesktopServices::openUrl(QUrl("http://support.xyzprinting.com/tw_zh_tw/Help/ticket/home"));
}

void MainWindow::selectAllSlot()
{
	meshDoc()->multiSelectID.clear();

	foreach(MeshModel *sm, meshDoc()->meshList)
	{
		//if (sm->getMeshSort() == MeshModel::meshsort::print_item)
		meshDoc()->multiSelectID.insert(sm->id());
	}

	updateMenus();
}
void MainWindow::selectNoneSlot()
{
	meshDoc()->multiSelectID.clear();



	updateMenus();
}


void MainWindow::transferFilterNameSlot()
{
	QList<Point3f> savePos;
	savePos.clear();

	foreach(int i, meshDoc()->multiSelectID)
	{
		savePos.push_back(meshDoc()->getMesh(i)->cm.bbox.Center());
	}

	QMap<int, Point3f> saveAllPos;
	saveAllPos.clear();

	foreach(MeshModel *mm, meshDoc()->meshList)
	{
		saveAllPos.insert(mm->id(), mm->cm.bbox.Center());
	}

	QAction *action = qobject_cast<QAction *>(sender());
	if (action->text() == "Front")
	{
		PM.actionFilterMap.value("JUSTIFY_FRONT")->trigger();
		currentViewContainer()->undoStack->push(new justify_command(this, meshDoc(), meshDoc()->multiSelectID, savePos, false));
	}
	else if (action->text() == "Back")
	{
		PM.actionFilterMap.value("JUSTIFY_BACK")->trigger();
		currentViewContainer()->undoStack->push(new justify_command(this, meshDoc(), meshDoc()->multiSelectID, savePos, false));
	}
	else if (action->text() == "Right")
	{
		PM.actionFilterMap.value("JUSTIFY_RIGHT")->trigger();
		currentViewContainer()->undoStack->push(new justify_command(this, meshDoc(), meshDoc()->multiSelectID, savePos, false));
	}
	else if (action->text() == "Left")
	{
		PM.actionFilterMap.value("JUSTIFY_LEFT")->trigger();
		currentViewContainer()->undoStack->push(new justify_command(this, meshDoc(), meshDoc()->multiSelectID, savePos, false));
	}
	else if (action->text() == "Top")
	{
		PM.actionFilterMap.value("JUSTIFY_TOP")->trigger();
		currentViewContainer()->undoStack->push(new justify_command(this, meshDoc(), meshDoc()->multiSelectID, savePos, false));
	}
	else if (action->text() == "Bottom")
	{
		PM.actionFilterMap.value("JUSTIFY_BOTTOM")->trigger();
		currentViewContainer()->undoStack->push(new justify_command(this, meshDoc(), meshDoc()->multiSelectID, savePos, false));
	}

	else if (action->text() == "By X-Y plane")
	{
		PM.actionFilterMap.value("FLIP_BY_X_Y_PLANE")->trigger();
		currentViewContainer()->undoStack->push(new flip_command(this, meshDoc(), meshDoc()->multiSelectID, action->text(), false));
	}
	else if (action->text() == "By Y-Z plane")
	{
		PM.actionFilterMap.value("FLIP_BY_Y_Z_PLANE")->trigger();
		currentViewContainer()->undoStack->push(new flip_command(this, meshDoc(), meshDoc()->multiSelectID, action->text(), false));
	}
	else if (action->text() == "By X-Z plane")
	{
		PM.actionFilterMap.value("FLIP_BY_X_Z_PLANE")->trigger();
		currentViewContainer()->undoStack->push(new flip_command(this, meshDoc(), meshDoc()->multiSelectID, action->text(), false));
	}
	else if (action->text() == "Land (Selected Models)" || action->text() == "Land")
	{
		RichParameterSet dummyParSet;
		dummyParSet.addParam(new RichBool("collision_detected", false, "", ""));
		dummyParSet.addParam(new RichBool("all_or_not", false, "", ""));
		dummyParSet.setValue("collision_detected", BoolValue(!transformwidgett1->getAllowOverLapping()));
		dummyParSet.setValue("all_or_not", BoolValue(false));
		//executeFilter(PM.actionFilterMap.value("Landing_Tool"), dummyParSet, false);
		executeFilter(PM.actionFilterMap.value("FP_GET_DEPTH_LANDING_TEST2"), dummyParSet, false);

		int count = 0;
		foreach(int i, currentViewContainer()->meshDoc.multiSelectID)
		{
			if (count < currentViewContainer()->meshDoc.multiSelectID.size())
			{
				float z = meshDoc()->getMesh(i)->cm.bbox.Center().Z() - savePos[count].Z();
				if (z != 0)
				{
					currentViewContainer()->undoStack->push(new land_command(this, meshDoc(), meshDoc()->multiSelectID, savePos, false));
					break;
				}
				count++;
			}
		}
	}
	else if (action->text() == "Land All")
	{
		RichParameterSet dummyParSet;
		dummyParSet.addParam(new RichBool("collision_detected", false, "", ""));
		dummyParSet.addParam(new RichBool("all_or_not", false, "", ""));
		dummyParSet.setValue("collision_detected", BoolValue(!transformwidgett1->getAllowOverLapping()));
		dummyParSet.setValue("all_or_not", BoolValue(true));
		//executeFilter(PM.actionFilterMap.value("Landing_Tool"), dummyParSet, false);
		executeFilter(PM.actionFilterMap.value("FP_GET_DEPTH_LANDING_TEST2"), dummyParSet, false);

		int count = 0;
		foreach(MeshModel *tm, meshDoc()->meshList)
		{
			if (count < meshDoc()->meshList.size())
			{
				float z = tm->cm.bbox.Center().Z() - saveAllPos.value(tm->id()).Z();
				if (z != 0)
				{
					currentViewContainer()->undoStack->push(new landAll_command(this, meshDoc(), saveAllPos, false));
					break;
				}
				count++;
			}
		}
	}

	//SYDNY 05/11/2017
	else if (action->text() == "To Center")
	{
		RichParameterSet dummyParSet;
		QList<int> list = meshDoc()->multiSelectID.toList();
		qSort(list.begin(), list.end(), qLess<int>());

		foreach(int i, list)
		{
			savePos.push_back(meshDoc()->getMesh(i)->cm.bbox.Center());
		}

		foreach(int i, list)
		{
			currentViewContainer()->meshDoc.setCurrentMesh(i);
			executeFilter(PM.actionFilterMap.value("MoveToCenter"), dummyParSet, false);
			currentViewContainer()->undoStack->push(new landAll_command(this, meshDoc(), saveAllPos, false));
		}
	}

	else if (action->text() == "Reset View")
		viewOriginalAct->trigger();

	else if (action->text() == "Auto Placement")
		PM.actionFilterMap.value("FP_TEST_SEARCH_SPACE_ALL_IMAGE")->trigger();
	else if (action->text() == "Auto Placement(Bounding Box)")
	{
		selectAllSlot();
		PM.actionFilterMap.value("Auto Packing")->trigger();
	}
	else if (action->text() == "Allow Overlapping")
	{
		if (!overLapAct->isChecked())
		{
			overLapAct->setChecked(false);
			transformwidgett1->setAllowOverLapping(false);
		}
		else
		{
			overLapAct->setChecked(true);
			transformwidgett1->setAllowOverLapping(true);
		}
	}
}
bool MainWindow::testload3mf(QString fileName)
{
	//if (!GLA())
	//{
	//	this->newProject();
	//	if (!GLA())
	//		return false;
	//}

	///*QString number = QString("%1").arg(13, 5, 10, QChar('0'));
	//qDebug() << "qPrintable(fileNaddddddddddddme)1" << number;*/
	//qDebug() << "qPrintable(fileName)1" << fileName.toUtf8().data();
	//fileName = fileName.toUtf8().data();
	//qDebug() << "qPrintable(fileName)5" << fileName;

	/////***20151513example
	//const QString DEFAULT_DIR_KEY("default_dir");

	//QSettings MySettings; // Will be using application informations for correct location of your settings

	//QStringList fileNameList;
	//if (fileName.isEmpty())
	//	//fileNameList = QFileDialog::getOpenFileNames(this, tr("Import Mesh"), lastUsedDirectory.path(), PM.inpFilters.join(";;"));
	//	fileNameList = QFileDialog::getOpenFileNames(this, tr("Import Mesh"), MySettings.value(DEFAULT_DIR_KEY).toString(), "*.3mf");

	//else
	//	fileNameList.push_back(fileName);

	//if (fileNameList.isEmpty())	return false;
	//else
	//{
	//	//save path away so we can use it again
	//	QString path = fileNameList.first();
	//	path.truncate(path.lastIndexOf("/"));
	//	lastUsedDirectory.setPath(path);

	//	//***20151013***紀錄所選位置資料夾
	//	QDir CurrentDir;
	//	MySettings.setValue(DEFAULT_DIR_KEY, CurrentDir.absoluteFilePath(fileNameList.first()));
	//	//QMessageBox::information(this, "Info", "You selected the file '" + fileNameList.first() + "'");

	//}

	//QTime allFileTime;
	//allFileTime.start();
	//foreach(fileName, fileNameList)
	//{
	//	QFileInfo fi(fileName);
	//	QString extension = fi.suffix();
	//	//MeshIOInterface *pCurrentIOPlugin = PM.allKnowInputFormats[extension.toLower()];
	//	////pCurrentIOPlugin->setLog(gla->log);
	//	//if (pCurrentIOPlugin == NULL)
	//	//{
	//	//	QString errorMsgFormat("Unable to open file:\n\"%1\"\n\nError details: file format " + extension + " not supported.");
	//	//	QMessageBox::critical(this, tr("Meshlab Opening Error"), errorMsgFormat.arg(fileName));
	//	//	return false;
	//	//}

	//	//RichParameterSet prePar;
	//	//pCurrentIOPlugin->initPreOpenParameter(extension, fileName, prePar);
	//	//if (!prePar.isEmpty())
	//	//{
	//	//	GenericParamDialog preOpenDialog(this, &prePar, tr("Pre-Open Options"));
	//	//	preOpenDialog.setFocus();
	//	//	preOpenDialog.exec();
	//	//}
	//	int mask = 0;
	//	//MeshModel *mm= new MeshModel(gla->meshDoc);
	//	//***20150921
	//	QFileInfo info(fileName);
	//	//MeshModel *mm = meshDoc()->addNewMesh(qPrintable(fileName), info.fileName());
	//	qDebug() << "qPrintable(fileName)6" << fileName;
	//	MeshModel *mm = meshDoc()->addNewMesh(fileName, info.fileName());//*****************************************************

	//	ThreeMF_Manip *tmf = new ThreeMF_Manip();
	//	tmf->Open3mf(mm->cm, fileName.toStdString().c_str());


	//	tri::UpdateColor<CMeshO>::PerVertexConstant(mm->cm, Color4b(255, 255, 0, 255));
	//	mm->UpdateBoxAndNormals();
	//	//tri::UpdateTopology<CMeshO>::FaceFace(mm->cm);
	//	tri::UpdateTopology<CMeshO>::EdgeEdge(mm->cm);


	//	CMeshO::FaceIterator fid;
	//	//size_t size = 0;
	//	for (fid = mm->cm.face.begin(); fid != mm->cm.face.end(); ++fid)
	//	{
	//		for (int n = 0; n < 3; n++)
	//		{
	//			vertices_test.push_back((*fid).P(n).X());
	//			vertices_test.push_back((*fid).P(n).Y());
	//			vertices_test.push_back((*fid).P(n).Z());

	//			normal_test.push_back((*fid).N().X());
	//			normal_test.push_back((*fid).N().Y());
	//			normal_test.push_back((*fid).N().Z());

	//		}

	//	}

	//	std::copy(vertices_test.begin(), vertices_test.end(), vbodataptr);
	//	std::copy(normal_test.begin(), normal_test.end(), vbonormalptr);

	//	GLA()->createVBO(vbodataptr, sizeof(vbodataptr)+sizeof(vbonormalptr), GL_ARRAY_BUFFER_ARB, GL_STATIC_DRAW_ARB);


	//}
	return true;
}
void MainWindow::testClearSiuiusError()
{
	comm->clearSiriusError();
	//ShellExecuteA(0, ("open"), "cmd.exe", " usbsend.exe", 0, SW_SHOWNORMAL);
}

//SYDNY
void MainWindow::logout()
{
	status_Icon->setIcon(QIcon(":/images/icons/status_off.png"));
	status->setText(tr("Not Login"));
	status_Arrow->setIcon(QIcon(":/images/icons/logout_off.png"));

	QSettings xyzReg("HKEY_CURRENT_USER\\SOFTWARE\\XYZ", QSettings::NativeFormat);
	if (xyzReg.childGroups().contains("xyzsettings", Qt::CaseInsensitive))
	{
		QSettings xyzReg("HKEY_CURRENT_USER\\SOFTWARE\\XYZ", QSettings::NativeFormat);
		xyzReg.remove("xyzsettings");
	}

	QSettings xyzReg2("HKEY_CURRENT_USER\\SOFTWARE\\XYZ", QSettings::NativeFormat);
	if (xyzReg.childGroups().contains("rememberuser", Qt::CaseInsensitive))
	{

		QSettings xyzReg2("HKEY_CURRENT_USER\\SOFTWARE\\XYZ", QSettings::NativeFormat);
		xyzReg.remove("rememberuser");
	}

	//if gdpr_setting
	if (logoutAct->text() == "Login")
	{
		GDPRDialog *test = new GDPRDialog();
		gdprResult = test->exec();
		QSettings settings;
		if (gdprResult == QDialog::Accepted)
		{
			const QString gdpr_Setting("GDPR_SETTING");
			settings.setValue(gdpr_Setting, 1);

			/*login logindialog(this);
			int r = logindialog.exec();


			QSettings xyzReg3("HKEY_CURRENT_USER\\SOFTWARE\\XYZ", QSettings::NativeFormat);
			if (!xyzReg3.childGroups().contains("xyzsettings", Qt::CaseInsensitive) && !xyzReg3.childGroups().contains("rememberuser", Qt::CaseInsensitive))
			{
			if (r == 0)
			{
			close();
			}
			}
			else
			{

			updateLoginData();
			}*/

			QNetworkAccessManager nam;
			QNetworkRequest req(QUrl("http://www.google.com"));
			QNetworkReply *reply = nam.get(req);
			QEventLoop loop;
			connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
			loop.exec();
			QString rep = reply->readAll();


			if (rep == NULL)
			{
				if (!xyzReg.childGroups().contains("xyzsettings", Qt::CaseInsensitive) && !xyzReg.childGroups().contains("rememberuser", Qt::CaseInsensitive))
				{
					while (rep == NULL)
					{//while loop
						QMessageBox::StandardButton ret = QMessageBox::question(
							this, tr("No Internet Connection"), tr("Skip Login?\nYes, Skip Login.\nNo, Need Internet."),
							QMessageBox::Yes | QMessageBox::No,
							QMessageBox::No);

						if (ret == QMessageBox::Yes)
						{
							updateLoginData();

							break;
						}
						if (ret == QMessageBox::No)
						{
							QString sURL = "http://service.xyzprinting.com/XYZPrinting/Members/REQUESTLOGIN/JSON";

							// create custom temporary event loop on stack
							QEventLoop eventLoop;

							// "quit()" the event-loop, when the network request "finished()"
							QNetworkAccessManager mgr;
							QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

							// the HTTP request
							QNetworkRequest geturl;
							geturl.setUrl(QUrl(sURL));
							//geturl.setRawHeader("EMAIL", username.toAscii());
							geturl.setRawHeader("key", "033bd94b1168d7e4f0d644c3c95e35bf");
							QNetworkReply *reply = mgr.get(geturl);
							eventLoop.exec();
							rep = reply->readAll();

							QNetworkAccessManager nam;
							QNetworkRequest req(QUrl("http://www.google.com"));
							QNetworkReply *replyg = nam.get(req);
							QEventLoop loop;
							connect(replyg, SIGNAL(finished()), &loop, SLOT(quit()));
							loop.exec();
							QString repgoogle = replyg->readAll();


							if (repgoogle != NULL)
							{
								//SYDNY 
								if (!xyzReg.childGroups().contains("xyzsettings", Qt::CaseInsensitive) && !xyzReg.childGroups().contains("rememberuser", Qt::CaseInsensitive))
								{
									login logindialog(this);
									logindialog.setWindowFlags(Qt::Tool);
									int r = logindialog.exec();

									if (!xyzReg.childGroups().contains("xyzsettings", Qt::CaseInsensitive) && !xyzReg.childGroups().contains("rememberuser", Qt::CaseInsensitive))
									{
										if (r == 0)
										{
											close();
										}
									}
									else if (xyzReg.childGroups().contains("xyzsettings", Qt::CaseInsensitive))
									{
										if (r == 0)
										{
											updateLoginData();
										}
									}
								}
								else if (xyzReg.childGroups().contains("xyzsettings", Qt::CaseInsensitive) && xyzReg.childGroups().contains("rememberuser", Qt::CaseInsensitive))
								{
									updateLoginData();
								}
							}
							else
							{
								repgoogle = reply->readAll();
							}
						}
					}//while loop

				}
				else
				{
					updateLoginData();
				}
			}

			else
			{
				//SYDNY 
				if (!xyzReg.childGroups().contains("xyzsettings", Qt::CaseInsensitive) && !xyzReg.childGroups().contains("rememberuser", Qt::CaseInsensitive))
				{
					login logindialog(this);
					logindialog.setWindowFlags(Qt::Tool);
					int r = logindialog.exec();

					if (!xyzReg.childGroups().contains("xyzsettings", Qt::CaseInsensitive) && !xyzReg.childGroups().contains("rememberuser", Qt::CaseInsensitive))
					{
						if (r == 0)
						{
							close();
						}
					}
					else if (xyzReg.childGroups().contains("xyzsettings", Qt::CaseInsensitive))
					{
						if (r == 0)
						{
							updateLoginData();
						}
					}
				}
				else if (xyzReg.childGroups().contains("xyzsettings", Qt::CaseInsensitive) && xyzReg.childGroups().contains("rememberuser", Qt::CaseInsensitive))
				{
					updateLoginData();
				}
			}

		}
		else
		{
			const QString gdpr_Setting("GDPR_SETTING");
			settings.setValue(gdpr_Setting, 0);
			updateLoginData();
		}
	}
	else
	{
		updateLoginData();
	}
}

bool MainWindow::testReadZX(QString zxFilename)
{
	//QString zxFilename = QFileDialog::getOpenFileName(this, tr("Open ZX File"), QDir::currentPath(), "ZX File (*.zx);");

	/*QFileInfo tempFileInfo(zxFilename);
	QString strDynamicWipewName = tempFileInfo.path() + "/" + tempFileInfo.completeBaseName() + ".txt";
	QFileInfo dynamicWipewfile(strDynamicWipewName);
	if (dynamicWipewfile.exists())
	{
	QMap<int, int> tempmap;
	SKT::readfromfile<bool>(strDynamicWipewName.toStdString(), tempmap);
	dynamicWipe.clear();
	dynamicWipe = tempmap;

	}*/
	//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("testReadZX"));
	QFileInfo fi(zxFilename);
	if (fi.suffix().toLower() != "zx")
	{
		QMessageBox::critical(this, tr("Loading Failed"), tr("This file is not a ZX file!"));
		return 0;
	}
	ZxParser *zx = new ZxParser();
	int pageCount = zx->getZxNPage(zxFilename.toStdString().c_str());
	QMessageBox::information(this, tr("ZX File Reader"), tr("There are %1 pages in this file.").arg(pageCount));
	//***********************
	PrintOption printOption_(PrintOption::optionMode::PRINT_ZXA_FILE, this, meshDoc());

	//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("testReadZX"));

	//float *cmyusage = getCmyusage();
	cmyusage.clear();
	PrinterJobInfo *pjii = new PrinterJobInfo(this, meshDoc(), &currentGlobalParams, &cmyusage);
	pjii->setLayers(pageCount);
	//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("testReadZX"));
	printOption_.updatePrintSummaryLabel(pjii->getPJI());
	printOption_.exec();

	int wiperFrequency = currentGlobalParams.getInt("MIDJOB_FREQUENCY");// printOption_.getWiperFrequency();
	int wiperIndex = currentGlobalParams.getInt("WIPER_INDEX");// printOption_.getWiperIndex();
	int wiperClick = currentGlobalParams.getInt("WIPER_CLICK"); //printOption_.getWiperClick();

	//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("testReadZX"));
	QVector<int> tempHeatParam;
	printOption_.getHeatStuffParam(tempHeatParam);
	tempHeatParam.replace(4, currentGlobalParams.getInt("PP_POST_HEATING_MINUTES"));
	tempHeatParam.replace(5, currentGlobalParams.getInt("PP_POST_HEATING_UPPER_LIMIT"));
	tempHeatParam.replace(6, currentGlobalParams.getInt("PP_POST_HEATING_LOWER_LIMIT"));
	PrintjobParam pjpGenZX;
	pjpGenZX.setCommonPrintValue(meshDoc());
	pjpGenZX.setGenZxParam(true);
	pjpGenZX.setMono(printOption_.getMonoParam());
	pjpGenZX.setPrintStart(true);

	int fanSpeed = currentGlobalParams.getInt("FAN_SPEED");
	int pumpValue = currentGlobalParams.getInt("PUMP_VALUE");
	bool dynamicSwitch = currentGlobalParams.getBool("DYNAMIC_WIPE");
	pause_button->setVisible(false);
	s_pushbutton->setVisible(false);

	std::string s_result;
	std::string cmdd = "GET /maint/pump_while_print?rpm=";// 65 HTTP / 1.1\r\n\r\n";
	cmdd.append(std::to_string(pumpValue));
	cmdd.append(" HTTP/1.1\r\n\r\n");
	comm->sendSCICommand(cmdd, s_result);
	//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("testReadZX"));
	switch (1)
	{
	case 0:
	{
		/*WidgetMonitor widget_p_monitor(printParam);
		widget_p_monitor.exec();
		if (widget_p_monitor.result() == QDialog::Accepted)
		{
		qb->show();
		executeFilter(PM.actionFilterMap.value("Print_Flow_2"), printParam, false);
		}

		qb->reset();
		int nextfile = 0;*/


		//sendZXfile(meshDoc()->jobname.at(nextfile));
	}
	break;
	case 1:
	{
		if (printOption_.result()) {//backup
			//if (1){

			//if (!printOption_.result()){
			comm->PreAndPrintingHeated(tempHeatParam.at(0), tempHeatParam.at(1), tempHeatParam.at(2), tempHeatParam.at(3));
			comm->PostHeated(tempHeatParam.at(4), tempHeatParam.at(5), tempHeatParam.at(6));
			comm->WiperIndex(wiperIndex);
			comm->WiperClick(wiperClick);
			bool setsuc = comm->setDynamicMidjob_Mode(1);//mode 0 midjob every 4, 1 ,2 send by comm
			setsuc = comm->setDynamicMidjob_PageInterval(wiperFrequency);//mode 0 midjob every 4, 1 ,2 send by comm
			CheckListWidget  checkListWidget(&pjii->getPJI());

			checkListWidget.exec();
			if (checkListWidget.result() == QDialog::Accepted)
			{
				//assert(checkListWidget.sendHeader(pjii->getPJI()));
				//bool headt = checkListWidget.sendHeader(pjii->getPJI());
				bool headt = checkListWidget.sendHeader(pjii->getPJI(), fanSpeed, pjpGenZX.getprintJobParam().getFloat("slice_height"));
				if (headt)
				{
					//QMessageBox::information(this, "Info", "header transfer successful");
					int waitTime = 0;
					while (true)
					{
						Sleep(500);
						std::string s, e;
						comm->printerStatus(s, e);
						QString temp = QString::fromStdString(s);
						if (temp == "Printing")
						{
							//if (dynamicWipewfile.exists())
							//{
							// bool setsuc = comm->setDynamicMidjob_Mode(2);//mode 0 midjob every 4, 1 ,2 send by comm
							// QMapIterator<int, int> imap(dynamicWipe);
							// while (imap.hasNext()) {
							//  imap.next();
							//  comm->setDynamicMidjob_Page(imap.key(), imap.value());										  
							// }
							//}
							//else
							//{
							//bool setsuc = comm->setDynamicMidjob_Mode(0);//mode 0 midjob every 4, 1 ,2 send by comm

							//}

							break;
						}
						waitTime += 500;
						if (waitTime > 600000)
						{
							QMessageBox::information(this, "Info", "Timeout");
							return false;
						}

					}
					RichParameterSet tt;
					openDashboard(true);



					sendZXfile(zxFilename);
					/*int i = (int)ShellExecuteA(0, ("open"), "USBSend.exe", zxFilename.toStdString().c_str(), 0, SW_SHOWNORMAL);
					qDebug() << "shell value: " << i;
					if (i > 32)
					QMessageBox::information(this, tr("USBSend.EXE"), tr("file %1 successfuly opened by USBSend.exe").arg(fi.fileName()));
					else
					QMessageBox::information(this, tr("USBSend.EXE"), tr("file %1 failed to open by USBSend.exe").arg(fi.fileName()));*/

				}
				else
				{
					QMessageBox::information(this, "Info", "Printer header transfer false");
					/*qb->show();
					executeFilter(PM.actionFilterMap.value("Print_Flow_2"), printParam, false);*/
					return false;
				}
			}
			qb->reset();
			delOl(MeshModel::meshsort::slice_item);
		}
		//checkListWidget.~CheckListWidget();
	}
	break;
	}

	return true;








}
bool MainWindow::testReadZip(QString zxaAFilename)
{
	//get zxa file
	//determine zxa is valid
	//extract to temp folder
	//load txt and zxsend zx file
	//delete tempzxa file

	//QString zxaAFilename = QFileDialog::getOpenFileName(this, tr("Open ZXA File"), QDir::currentPath(), "ZXA File (*.zxa);");
	QFileInfo tempZXAFileInfo(zxaAFilename);
	if (tempZXAFileInfo.suffix().toLower() != "zxa")
	{
		QMessageBox::critical(this, tr("Loading Failed"), tr("This file is not a ZXA file!"));
		return 0;
	}
	//***check tempzxa directory
	QString temRoamppath = PicaApplication::getRoamingDir();
	//QString temRoamppath = QStandardPaths::locate(QStandardPaths::DocumentsLocation, "", QStandardPaths::LocateDirectory) + PicaApplication::appName();
	EntryNames savename;
	QDir extract_zxa_dir(temRoamppath);
	if (extract_zxa_dir.exists("tempzxa"))
	{
		extract_zxa_dir.setPath(temRoamppath + "/tempzxa");
		extract_zxa_dir.removeRecursively();
		extract_zxa_dir.setPath(temRoamppath);
		extract_zxa_dir.mkpath("tempzxa");
	}
	else
	{
		extract_zxa_dir.setPath(temRoamppath);
		extract_zxa_dir.mkpath("tempzxa");
	}
	extract_zxa_dir.setPath(temRoamppath + "/tempzxa");
	std::string extract_zxa_dirName = extract_zxa_dir.absolutePath().toStdString();
	//***extract to temp folder	
	//string zxaFilenames = zxaAFilename.toStdString();
	//ZipIO zip_zx_txt(zxaFilenames.c_str());//zxa file path
	//string extract_zxa_dirName = extract_zxa_dir.absolutePath().toStdString();
	//if (zip_zx_txt.extract2File(extract_zxa_dirName.c_str(), &savename) == 0)
	//	//if (zip_zx_txt.extract2File("C:/Users/tb495076/Documents/BCPware", &savename) == 0)
	//{
	//	QMessageBox::critical(this, tr("zxa Failed Extract"), tr("ZXA file is not valid!"));
	//	return 0;//extract position	
	//}
	//zip_zx_txt.closeZip();

	//if (savename.entryName.size() > 2)
	//{
	//	QMessageBox::critical(this, tr("Loading Failed"), tr("ZXA file is not valid!"));
	//	return 0;
	//}
	////***get zx file and txt
	//QString zxfile, dynamicfile;
	//for (auto const &name : savename.entryName)
	//{
	//	QFileInfo temp(QString::fromStdString(name));
	//	if (temp.suffix() == "zx")
	//		zxfile = QString::fromStdString(extract_zxa_dirName) + "/" + temp.fileName();
	//	else if (temp.suffix() == "txt")
	//		dynamicfile = QString::fromStdString(extract_zxa_dirName) + "/" + temp.fileName();
	//}

	/////////////////////////////////////////////////////
	/*============7zip==========================*/
	QStringList arguments;
	int iReturnCode = -1;
	QString printJob, headerFile;
	QProcess process;

	bool ret = false;
	bool ret2 = false;

	/*extract file*/
	arguments.append("x");
	arguments.append(zxaAFilename);
	arguments.append("-o" + extract_zxa_dir.path());       // Output directory
	arguments.append("-y"); // Blindly overwrite files (the temporary directory should be empty anyway)

	iReturnCode = QProcess::execute("7z", arguments);

	/*get file name*/
	arguments.clear();
	arguments.append("l");
	arguments.append(zxaAFilename);
	process.start("7z", arguments);
	process.waitForFinished();
	QString output(process.readAllStandardOutput());

	QRegularExpression regPrintFile("\\S+.zx\\s");
	QRegularExpression reggMetaFile("\\S+.txt\\s");
	QRegularExpressionMatch match, matchHeader;

	//list = output.split(QRegularExpression("\\s+"));
	ret = output.contains(regPrintFile, &match);
	ret2 = output.contains(reggMetaFile, &matchHeader);
	if (ret && ret2) {
		printJob = match.captured();
		headerFile = matchHeader.captured();
	}
	else {
		qDebug() << "can't get printJob";

	}
	(void)iReturnCode;

	//***get zx file and txt

	QString zxfile, dynamicfile;

	zxfile = QString::fromStdString(extract_zxa_dirName) + "/" + printJob;
	dynamicfile = QString::fromStdString(extract_zxa_dirName) + "/" + headerFile;
	if (zxfile.endsWith("\r"))
		zxfile.replace("\r", "");
	if (dynamicfile.endsWith("\r"))
		dynamicfile.replace("\r", "");


	//========================================//

	//QString strDynamicWipewName = tempFileInfo.path() + "/" + tempFileInfo.completeBaseName() + ".txt";
	QFileInfo dynamicWipewfile(dynamicfile);
	if (dynamicWipewfile.exists())
	{
		QMap<int, int> tempmap;
		SKT::readfromfile<bool>(dynamicfile.toStdString(), tempmap);
		dynamicWipe.clear();
		dynamicWipe = tempmap;

	}
	int monomode = 0;
	double layer_Height = 0;
	QString stiff_mode;
	int layer_Mode = 1;
	if (dynamicWipewfile.exists())
	{
		QSettings settings(dynamicfile, QSettings::IniFormat);
		monomode = settings.value("MONO_MODE").toInt();
		layer_Height = settings.value("Layer_Height").toInt() / 10000.;
		stiff_mode = settings.value("Print_Mode").toString();
	}
	if (monomode == 1 && layer_Height == 0)
	{
		QMessageBox::critical(this, tr("Can't get layer_Height "), tr("layer_Height = 0"));
		return 0;
	}
	if (stiff_mode == "1Layer1Page")
		layer_Mode = 1;
	else if (stiff_mode == "1Layer2Page")
		layer_Mode = 2;
	qWarning() << "Print_Mode" << stiff_mode;
	qWarning() << "layer_Mode" << layer_Mode;
	/*QFileInfo fi(zxaAFilename);
	if (fi.suffix().toLower() != "zx")
	{
	QMessageBox::critical(this, tr("Loading Failed"), tr("This file is not a ZX file!"));
	return 0;
	}*/
	ZxParser *zx = new ZxParser();
	int pageCount = zx->getZxNPage(zxfile.toStdString().c_str());
	QMessageBox::information(this, tr("ZX File Reader"), tr("There are %1 pages in this file.").arg(pageCount));
	//***********************
	//PrintOption printOption_(PrintOption::optionMode::PRINT_ZXA_FILE, this, meshDoc());
	PrintOption *printOption_;// (PrintOption::optionMode::PRINT_ZXA_FILE, this, meshDoc());

	if (!monomode)
	{
		printOption_ = new PrintOption(PrintOption::optionMode::PRINT_ZXA_FILE, this, meshDoc());
	}
	else
		printOption_ = new PrintOption(PrintOption::optionMode::PRINT_ZXA_MONO_FILE, this, meshDoc());

	WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T(""));

	//float *cmyusage = getCmyusage();
	cmyusage.clear();
	PrinterJobInfo *pjii = new PrinterJobInfo(this, meshDoc(), &currentGlobalParams, &cmyusage);
	pjii->setLayers(pageCount);

	printOption_->updatePrintSummaryLabel(pjii->getPJI());
	printOption_->exec();
	int wiperFrequency = currentGlobalParams.getInt("MIDJOB_FREQUENCY"); //printOption_.getWiperFrequency();
	int wiperIndex = currentGlobalParams.getInt("WIPER_INDEX");// printOption_.getWiperIndex();
	int wiperClick = currentGlobalParams.getInt("WIPER_CLICK"); //printOption_.getWiperClick();
	bool ir_On_Off_Setting = currentGlobalParams.getBool("IR_On_Off");
	QVector<int> tempHeatParam;
	printOption_->getHeatStuffParam(tempHeatParam);
	PrintjobParam pjpGenZX;
	pjpGenZX.setCommonPrintValue(meshDoc());
	pjpGenZX.setGenZxParam(true);
	pjpGenZX.setMono(printOption_->getMonoParam());
	pjpGenZX.setPrintStart(true);


	float x = pjpGenZX.getprintJobParam().getFloat("slice_height");

	int fanSpeed = currentGlobalParams.getInt("FAN_SPEED");
	int pumpValue = currentGlobalParams.getInt("PUMP_VALUE");
	pause_button->setVisible(false);
	s_pushbutton->setVisible(false);

	std::string s_result;
	std::string cmdd = "GET /maint/pump_while_print?rpm=";// 65 HTTP / 1.1\r\n\r\n";
	cmdd.append(std::to_string(pumpValue));
	cmdd.append(" HTTP/1.1\r\n\r\n");
	comm->sendSCICommand(cmdd, s_result);



	switch (monomode)
	{
	case 0:
	{
		if (printOption_->result()) {//backup
			//if (1){
			QString irCommand;
			if (ir_On_Off_Setting)
				irCommand = SKT::createJsonStringCommand("FPGA", "@HTO", QStringList());
			else
				irCommand = SKT::createJsonStringCommand("FPGA", "@HTC", QStringList());
			comm->SendJSONCmd(irCommand.toStdString());


			comm->PreAndPrintingHeated(tempHeatParam.at(0), tempHeatParam.at(1), tempHeatParam.at(2), tempHeatParam.at(3));
			comm->PostHeated(tempHeatParam.at(4), tempHeatParam.at(5), tempHeatParam.at(6));
			comm->WiperIndex(wiperIndex);
			comm->WiperClick(wiperClick);
			bool setsuc = comm->setDynamicMidjob_Mode(1);//mode 0 midjob every 4, 1 ,2 send by comm
			setsuc = comm->setDynamicMidjob_PageInterval(wiperFrequency);//mode 0 midjob every 4, 1 ,2 send by comm

			comm->PrintMode(layer_Mode);

			//if (!printOption_.result()){
			CheckListWidget  checkListWidget(&pjii->getPJI());

			checkListWidget.exec();
			if (checkListWidget.result() == QDialog::Accepted)
				//if (1)
			{
				//assert(checkListWidget.sendHeader(pjii->getPJI()));
				bool headt = checkListWidget.sendHeader(pjii->getPJI(), fanSpeed, pjpGenZX.getprintJobParam().getFloat("slice_height"));
				if (headt)
				{
					//QMessageBox::information(this, "Info", "header transfer successful");
					int waitTime = 0;
					while (true)
					{
						Sleep(500);
						std::string s, e;
						comm->printerStatus(s, e);
						QString temp = QString::fromStdString(s);
						if (temp == "Printing")
						{
							if (dynamicWipewfile.exists())
							{
								//bool setsuc = comm->setDynamicMidjob_Mode(2);//mode 0 midjob every 4, 1 ,2 send by comm

								QMapIterator<int, int> imap(dynamicWipe);
								while (imap.hasNext()) {
									imap.next();
									comm->setDynamicMidjob_Page(imap.key(), imap.value());
								}
							}
							else
							{
								bool setsuc = comm->setDynamicMidjob_Mode(0);//mode 0 midjob every 4, 1 ,2 send by comm
							}

							break;
						}
						waitTime += 500;
						if (waitTime > 600000)
						{
							QMessageBox::information(this, "Info", "Timeout");
							return false;
						}

					}
					RichParameterSet tt;
					openDashboard(true);



					sendZXfile(zxfile);
					/*int i = (int)ShellExecuteA(0, ("open"), "USBSend.exe", zxFilename.toStdString().c_str(), 0, SW_SHOWNORMAL);
					qDebug() << "shell value: " << i;
					if (i > 32)
					QMessageBox::information(this, tr("USBSend.EXE"), tr("file %1 successfuly opened by USBSend.exe").arg(fi.fileName()));
					else
					QMessageBox::information(this, tr("USBSend.EXE"), tr("file %1 failed to open by USBSend.exe").arg(fi.fileName()));*/

				}
				else
				{
					QMessageBox::information(this, "Info", "Printer header transfer false");
					/*qb->show();
					executeFilter(PM.actionFilterMap.value("Print_Flow_2"), printParam, false);*/
					return false;
				}
			}
			qb->reset();
			delOl(MeshModel::meshsort::slice_item);
		}
		//checkListWidget.~CheckListWidget();
	}
	break;
	case 1:
	{
		if (printOption_->result()) {//backup
			//if (1){
			QString irCommand;
			if (ir_On_Off_Setting)
				irCommand = SKT::createJsonStringCommand("FPGA", "@HTO", QStringList());
			else
				irCommand = SKT::createJsonStringCommand("FPGA", "@HTC", QStringList());
			comm->SendJSONCmd(irCommand.toStdString());

			comm->PreAndPrintingHeated(tempHeatParam.at(0), tempHeatParam.at(1), tempHeatParam.at(2), tempHeatParam.at(3));
			comm->PostHeated(tempHeatParam.at(4), tempHeatParam.at(5), tempHeatParam.at(6));
			comm->WiperIndex(wiperIndex);
			comm->WiperClick(wiperClick);
			bool setsuc = comm->setDynamicMidjob_Mode(1);//mode 0 midjob every 4, 1 ,2 send by comm
			setsuc = comm->setDynamicMidjob_PageInterval(wiperFrequency);//mode 0 midjob every 4, 1 ,2 send by comm

			comm->PrintMode(layer_Mode);
			//if (!printOption_.result()){
			CheckListWidget  checkListWidget(&pjii->getPJI());

			checkListWidget.exec();
			if (checkListWidget.result() == QDialog::Accepted)
				//if (1)
			{
				//assert(checkListWidget.sendHeader(pjii->getPJI()));
				bool headt = checkListWidget.sendHeader(pjii->getPJI(), fanSpeed, layer_Height);
				if (headt)
				{
					//QMessageBox::information(this, "Info", "header transfer successful");
					int waitTime = 0;
					while (true)
					{
						Sleep(500);
						std::string s, e;
						comm->printerStatus(s, e);
						QString temp = QString::fromStdString(s);
						if (temp == "Printing")
						{
							if (dynamicWipewfile.exists())
							{
								//bool setsuc = comm->setDynamicMidjob_Mode(2);//mode 0 midjob every 4, 1 ,2 send by comm

								QMapIterator<int, int> imap(dynamicWipe);
								while (imap.hasNext()) {
									imap.next();
									comm->setDynamicMidjob_Page(imap.key(), imap.value());
								}
							}
							else
							{
								bool setsuc = comm->setDynamicMidjob_Mode(0);//mode 0 midjob every 4, 1 ,2 send by comm
							}

							break;
						}
						waitTime += 500;
						if (waitTime > 600000)
						{
							QMessageBox::information(this, "Info", "Timeout");
							return false;
						}

					}
					RichParameterSet tt;
					openDashboard(true);



					sendZXfile(zxfile);
					/*int i = (int)ShellExecuteA(0, ("open"), "USBSend.exe", zxFilename.toStdString().c_str(), 0, SW_SHOWNORMAL);
					qDebug() << "shell value: " << i;
					if (i > 32)
					QMessageBox::information(this, tr("USBSend.EXE"), tr("file %1 successfuly opened by USBSend.exe").arg(fi.fileName()));
					else
					QMessageBox::information(this, tr("USBSend.EXE"), tr("file %1 failed to open by USBSend.exe").arg(fi.fileName()));*/

				}
				else
				{
					QMessageBox::information(this, "Info", "Printer header transfer false");
					/*qb->show();
					executeFilter(PM.actionFilterMap.value("Print_Flow_2"), printParam, false);*/
					return false;
				}
			}
			qb->reset();
			delOl(MeshModel::meshsort::slice_item);
		}
		//checkListWidget.~CheckListWidget();
	}
	break;
	}

	return true;
}
bool MainWindow::testReadPrintingFile()
{
	//QProcess::startDetached("cmd");
	QString filen = QFileDialog::getOpenFileName(this, tr("Open Printable File for Printing"), QDir::currentPath(), "ZX File (*.zx *.zxa);");

	QFileInfo tempinfo(filen);
	if (tempinfo.suffix() == "zx")
		return testReadZX(tempinfo.absoluteFilePath());
	else if (tempinfo.suffix() == "zxa")
		return testReadZip(tempinfo.absoluteFilePath());

}
int MainWindow::countJobWipe(int wipeStep)
{
	QMap<int, int>::const_iterator i = finalDynamicPage.constBegin();
	QMap<int, int>::const_iterator head = finalDynamicPage.constBegin();
	int total_wipe_page = 0;
	while (i != finalDynamicPage.constEnd())
	{
		if (i == finalDynamicPage.constBegin())
		{
			total_wipe_page += (i.key() - 0) / dynamicWipe.first();
		}

		if (i.value() != head.value())
		{
			int page_gap = i.key() - head.key();
			total_wipe_page += page_gap / head.value() + 1;
			head = i;

		}
		if (i.key() == finalDynamicPage.lastKey())
		{
			total_wipe_page += (dynamicWipe.lastKey() - i.key()) / i.value();
			total_wipe_page++;
		}

		i++;
	}
	if (finalDynamicPage.size() == 0 && dynamicWipe.size() > 0)
	{
		total_wipe_page += ((dynamicWipe.lastKey()) / dynamicWipe.last());
	}

	return (total_wipe_page + 0.5) / wipeStep;
	//dynamicwipe

}
void MainWindow::detectOverlappingFunc()
{
	RichParameterSet tt;
	executeFilter(PM.actionFilterMap.value("FP_MESH_INTERSECT_TEST2"), tt, false);
}
void MainWindow::genPrePrintingSlot()
{
	RichParameterSet tt;

	//===Test generate mesh from gray image
	executeFilter(PM.actionFilterMap.value("FP_TEST_GENERATE_POINTS_FROM_GRAY_IMAGE_FLOAT"), tt, false);//


}

void MainWindow::view_outlineSlot()
{
	int x = sliceSpinBox->value();
	setSliderPosition(x);
}
void MainWindow::updateLoginData()
{
	//SYDNY
	QSettings xyzReg("HKEY_CURRENT_USER\\SOFTWARE\\XYZ", QSettings::NativeFormat);
	if (xyzReg.childGroups().contains("xyzsettings", Qt::CaseInsensitive))
	{
		QSettings xyzReg2("HKEY_CURRENT_USER\\SOFTWARE\\XYZ\\xyzsettings", QSettings::NativeFormat);
		QString nickname2 = xyzReg2.value("nickname").toString();
		QString username2 = xyzReg2.value("account").toString();
		status_Icon->setIcon(QIcon(":/images/icons/status_on.png"));
		status->setText(nickname2);
		status_Arrow->setIcon(QIcon(":/images/icons/logout_on.png"));
		nickname->setText("        " + nickname2);
		username->setText("        " + username2);
		status_Arrow->addAction(nickname);
		status_Arrow->addAction(username);
		nickname->setEnabled(false);
		username->setEnabled(false);
		status_Arrow->setStyleSheet("image: url(:/images/icons/account_background.png); color: #000000; ");
		status_Arrow->addSeparator();
		logoutAct->setText("Logout");
		status_Arrow->addAction(logoutAct);
	}
	else
	{
		QString nickname2("");
		QString username2("");
		status_Icon->setIcon(QIcon(":/images/icons/status_on.png"));
		status->setText(nickname2);
		status_Arrow->setIcon(QIcon(":/images/icons/logout_on.png"));
		nickname->setText("        " + nickname2);
		username->setText("        " + username2);
		status_Arrow->addAction(nickname);
		status_Arrow->addAction(username);
		nickname->setEnabled(false);
		username->setEnabled(false);
		status_Arrow->setStyleSheet("image: url(:/images/icons/account_background.png); color: #000000; ");
		status_Arrow->addSeparator();
		logoutAct->setText("Login");
		status_Arrow->addAction(logoutAct);

	}
}
void MainWindow::recordRollerPage()
{
	float unit = currentGlobalParams.getFloat("SLIGHT_HEIGHT");
	bool doublePrint_V2 = currentGlobalParams.getBool("STIFF_PRIN_V2");

	/*
		foreach every object
		get mesh top mm,
		if(two pages in one layer)
		(height/layer_height)*2
		*/
	rollerPage.clear();
	rollerPageToServer.clear();
	foreach(MeshModel *mm, meshDoc()->meshList)
	{
		double top = mm->cm.bbox.max.Z() + meshDoc()->groove.max.Z() / 2.;
		int record = ((top + DSP_grooveZ / 2.) / unit + 0.5);
		assert(record > 5);
		if (doublePrint_V2)
		{
			for (int i = 5; i >= 0; i--)
				rollerPage.insert(record - i);
		}
		else
		{
			for (int i = 2; i >= 0; i--)
				rollerPage.insert(record - i);
		}


	}

	//QStringList rollerMessageToServer;
	foreach(int x, rollerPage)
	{
		rollerPageToServer.append(QString::number(x));
	}

}