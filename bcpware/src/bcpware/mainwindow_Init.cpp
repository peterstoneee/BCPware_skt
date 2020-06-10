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
#include "../common/xmlfilterinfo.h"
#include "../common/searcher.h"
//#include "../common/mlapplication.h"

#include <QToolBar>
#include <QProgressBar>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFileOpenEvent>
#include <QFile>
#include <QtXml>
#include <QSysInfo>
#include <QDesktopServices>
#include <QStatusBar>
#include <QMenuBar>
#include <QDoubleSpinBox>

#include "mainwindow.h"
#include "plugindialog.h"
#include "customDialog.h"
#include "saveSnapshotDialog.h"
//#include "ui_congratsDialog.h"
#include "rendermodeactions.h"
#include "meshcheck.h"
#include "widgetStyleSheet.h"

#include "login.h"




QProgressBar *MainWindow::qb;
QPushButton *MainWindow::s_pushbutton;
QPushButton *MainWindow::pause_button;
//QMessageBox *MainWindow::printerStatusMsg;
QLabel *MainWindow::statusLabel;
FILE *MainWindow::dbgff;
//CusDialog *MainWindow::statusWidget;

bool MainWindow::cancel_filter = true;
bool MainWindow::pause_filter = false;
bool MainWindow::filter_process_running = false;
static bool tab_menu_bool = 0;

MainWindow::MainWindow()
	:mwsettings(), xmlfiltertimer(), wama(), logInFUNC(false)
{
	//=============
	/*time_t seconds_past_epoch = time(0);
	std::stringstream ss;
	ss << seconds_past_epoch;
	std::string ts = ss.str();*/
	//=================
	

	QFileInfo outfile(QStandardPaths::locate(QStandardPaths::DocumentsLocation, "", QStandardPaths::LocateDirectory) + PicaApplication::appName() + "/printerStatus_log.txt");
	dbgff = fopen(outfile.absoluteFilePath().toStdString().data(), "w");//@@@
	//meshCheckSwitch = true;
	//***20150603
	//genTranGroupbox = 0;
	//***
	/*qApp->setStyleSheet(
	"QProgressBar {	border: 2px solid grey;	border-radius:5px;	text-align:center;}"
	"QProgressBar::chunk {background-color:#CD96CD; width:10px; margin:0.5px;}"
	);
	qApp->setStyleSheet("QGroupBox{	background-color: qlineargradient(x1 : 0, y1 : 0, x2 : 0, y2 : 1,	stop : 0 #E0E0E0, stop: 1 #FFFFFF);}"
	"QGroupBox{ border: 1px solid gray;}"
	"QGroupBox{border-radius: 5px;}"
	"QGroupBox{margin-top: 1ex; }"
	"QGroupBox::title{subcontrol-origin: margin; subcontrol-position: bottom center;padding: 0 3px;	background-color: qlineargradient(x1 : 0, y1 : 0, x2 : 0, y2 : 1,stop : 0 #FFOECE, stop: 1 #FFFFFF);}"
	);*/
	//***20160517
	//view = new  QGraphicsView(&scene);
	//view = new  MyView();

	//xmlfiltertimer will be called repeatedly, so like Qt documentation suggests, the first time start function should be called.
	//Subsequently restart function will be invoked.
	xmlfiltertimer.start();
	//xmlfiltertimer.elapsed();


	//workspace = new QWorkspace(this);
	mdiarea = new QMdiArea(this);//產生視窗內的視窗
	layerDialog = new LayerDialog(this);
	layerDialog->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	addDockWidget(Qt::RightDockWidgetArea, layerDialog);

	//
	objList = new ObjectList(this);
	objList->setAllowedAreas(Qt::RightDockWidgetArea);
	addDockWidget(Qt::RightDockWidgetArea, objList);
	objList->hide();


	//setCentralWidget(workspace);
	setCentralWidget(mdiarea);
	windowMapper = new QSignalMapper(this);
	// Permette di passare da una finestra all'altra e tenere aggiornato il workspace
	connect(windowMapper, SIGNAL(mapped(QWidget*)), this, SLOT(wrapSetActiveSubWindow(QWidget *)));
	// Quando si passa da una finestra all'altra aggiorna lo stato delle toolbar e dei menu
	connect(mdiarea, SIGNAL(subWindowActivated(QMdiSubWindow *)), this, SLOT(updateMenus()));
	//connect(mdiarea, SIGNAL(subWindowActivated(QMdiSubWindow *)), this, SLOT(testslot()));
	connect(mdiarea, SIGNAL(subWindowActivated(QMdiSubWindow *)), this, SLOT(updateWindowMenu()));
	connect(mdiarea, SIGNAL(subWindowActivated(QMdiSubWindow *)), this, SLOT(updateStdDialog()));
	connect(mdiarea, SIGNAL(subWindowActivated(QMdiSubWindow *)), this, SLOT(updateXMLStdDialog()));
	//connect(mdiarea, SIGNAL(subWindowActivated(QMdiSubWindow *)),this, SLOT(updateDocumentScriptBindings()));
	httpReq = new QNetworkAccessManager(this);
	//***20150522_ connect(httpReq, SIGNAL(finished(QNetworkReply*)), this, SLOT(connectionDone(QNetworkReply*)));
	connect(httpReq, SIGNAL(finished(QNetworkReply*)), this, SLOT(connectionDone2(QNetworkReply*)));


	//***2015
	QIcon icon;
	icon.addPixmap(QPixmap(":images/SKT_images/bcp_icon.png"));
	setWindowIcon(icon);
	PM.loadPlugins(defaultGlobalParams);//pluging從這裡匯入，可是不知道參數設在哪裡
	QSettings settings;
	QVariant vers = settings.value(PicaApplication::versionRegisterKeyName());//
																			  //should update those values only after I run MeshLab for the very first time or after I installed a new version
	if (!vers.isValid() || vers.toString() < PicaApplication::appVer())//***在這邊設置globalparameter ,appver
	{
		settings.setValue(PicaApplication::pluginsPathRegisterKeyName(), PluginManager::getDefaultPluginDirPath());
		settings.setValue(PicaApplication::versionRegisterKeyName(), PicaApplication::appVer());
		settings.setValue(PicaApplication::wordSizeKeyName(), QSysInfo::WordSize);
		foreach(QString plfile, PM.pluginsLoaded)
			settings.setValue(PluginManager::osIndependentPluginName(plfile), PicaApplication::appVer());
	}
	// Now load from the registry the settings and  merge the hardwired values got from the PM.loadPlugins with the ones found in the registry.
	//loadMeshLabSettings();

	//***20160525
	//undoStack = new QUndoStack(this);
	//undoStack->setUndoLimit(15);
	//createUndoView();


	loadPicassoSettings();
	int x= currentGlobalParams.getEnum("Palette_Language");
	/*===============================*/
	//Load From txt to currentParam

	//
	/*================================*/
	QString lan;
	if (x==0)
	{
		lan = "";
	}
	else if(x==1)
	{		
		lan = "picasso_ja";
	}
	

	QTranslator translator;
	translator.load(lan);
	qApp->installTranslator(&translator);
	
	


	//used QUndoGroup to seperate stack per project (added by KPPH R&D-I-SW, Mark)
	undoGroup = new QUndoGroup(this);
	createUndoView();



	createActions();
	createToolBars();



	//****20150526***testtab function***


	//connect(GLA(), SIGNAL(matrix_changed(MeshModel *, Matrix44m)), this, SLOT(matrix_changed(MeshModel*, Matrix44m)));
	//connect(GLA(), SIGNAL(matrix_changed_2(MeshModel *, Matrix44m)), this, SLOT(matrix_changed_2(QSet<int>, Matrix44m)));

	//QMenuBar *menuBar = new QMenuBar(0);
	//createTransformGroupBox();
	createQToolButton();
	createToolBars_v2();
	buildwholeTabwidget();

	if (tab_menu_bool) {
		setMenuWidget(pTabWidget);
		createMenus_v2();
	}
	else
	{
		createMenus_v3();
	}
	//**********
	//menubar
	//createMenus();

	firstImport = true;
	stddialog = 0;
	xmldialog = 0;
	setAcceptDrops(true);
	mdiarea->setAcceptDrops(true);
	setWindowTitle(PicaApplication::completeName(PicaApplication::HW_ARCHITECTURE(QSysInfo::WordSize)));
	setStatusBar(new QStatusBar(this));
	globalStatusBar() = statusBar();
	qb = new QProgressBar(this);
	qb->setMaximum(100);
	qb->setMinimum(0);
	qb->setFixedWidth(100);
	qb->setStyleSheet(WidgetStyleSheet::progressBarWidget());
	qb->reset();
	//***20160623_
	s_pushbutton = new QPushButton(tr("Cancel"), this);//
	connect(s_pushbutton, SIGNAL(clicked()), this, SLOT(set_cancel_filter()), Qt::DirectConnection);

	pause_button = new QPushButton(tr("pause"), this);
	//pause_button->setCheckable(true);
	connect(pause_button, SIGNAL(clicked()), this, SLOT(set_pause_filter()), Qt::DirectConnection);

	pause_button->setVisible(false);
	s_pushbutton->setVisible(false);

	statusLabel = new QLabel;
	statusLabel->setMinimumWidth(200);
	//statusWidget = new CusDialog(this);
	

	//printerStatusMsg = new QMessageBox(this);
	//printerStatusMsg->information(this, "check update", "please check Firewall");
	
	statusBar()->addPermanentWidget(qb, 0);
	statusBar()->addPermanentWidget(s_pushbutton, 0);
	statusBar()->addPermanentWidget(pause_button, 0);
	statusBar()->addPermanentWidget(statusLabel);	
	//***stylesheet
	setGenericStyleSheet();

	newProject();
	//PM should be initialized before passing it to PluginGeneratorGUIfil
	plugingui = new PluginGeneratorGUI(PM, this);
	plugingui->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::BottomDockWidgetArea | Qt::RightDockWidgetArea);
	addDockWidget(Qt::LeftDockWidgetArea, plugingui);
	updateCustomSettings();
	connect(plugingui, SIGNAL(scriptCodeExecuted(const QScriptValue&, const int, const QString&)), this, SLOT(scriptCodeExecuted(const QScriptValue&, const int, const QString&)));
	connect(plugingui, SIGNAL(insertXMLPluginRequested(const QString&, const QString&)), this, SLOT(loadAndInsertXMLPlugin(const QString&, const QString&)));
	connect(plugingui, SIGNAL(historyRequest()), this, SLOT(sendHistory()));

	connect(this, SIGNAL(updateLayerTable()), layerDialog, SLOT(updateTable()));
	connect(layerDialog, SIGNAL(removeDecoratorRequested(QAction*)), this, SLOT(switchOffDecorator(QAction*)));

	////**********2015_2_5增加init_ortho front top視窗 

	//***20150515設置建構槽大小
	//MeshDocument *tempmdc = meshDoc();
	//tempmdc->groove.setLWH(15, 17, 19); //(x, z, y)
	//tempmdc->groove.setLWH(20.92, 20, 20.92); //(x, z, y)
	//************
	/*mainToolBar->setVisible(false);
	renderToolBar->setVisible(false);
	editToolBar->setVisible(false);
	filterToolBar->setVisible(false);
	searchToolBar->setVisible(false);*/

	//**20150618***
	viewToolBar->addWidget(testRenderButton);
	//vtNumberLimit = 5000;	
	//***20161112
	//const QString DEFAULT_VERTICES_LIMIT("vertices_limit");
	//settings.setValue(DEFAULT_VERTICES_LIMIT, 30000000);
	//setVtNumberLimit(settings.value("vertices_limit").toInt());

	alreadyload = true;

	/*testdialog = new QWidget(this);
	testdialog->setWindowFlags(Qt::Sheet | Qt::WindowStaysOnTopHint |Qt::X11BypassWindowManagerHint );
	testdialog->show();*/

	/*Qt::WindowFlags flags = this->windowFlags();
	this->setWindowFlags(flags | Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint);
*/
	//***20160310***//
	TPreview = new TransformPreview(this);
	//TPreview->setWindowFlags(Qt::Dialog);//加了會home widget不能按
	//TPreview->setWindowFlags(Qt::SplashScreen);
	//TPreview->setAttribute(Qt::WA_DeleteOnClose);

	//***20160517
	/*QDockWidget *dock = new QDockWidget(tr("DockWindow1"), this);
	dock->setFeatures(QDockWidget::DockWidgetMovable);
	dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	QTextEdit *te1 = new QTextEdit();
	te1->setText(tr("Window1,The dock widget can be moved between docks by the user"));
	dock->setWidget(te1);
	addDockWidget(Qt::LeftDockWidgetArea, dock);*/





	//createQDockWidget3();
	dock = createQDockWidget4();
	dock->setWindowFlags(  Qt::FramelessWindowHint);
	dock->setStyleSheet(WidgetStyleSheet::dockWidget());
	dock->setTitleBarWidget(0);
	dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
	//dock->setWindowFlags(Qt::CustomizeWindowHint);
	//dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
	addDockWidget(Qt::LeftDockWidgetArea, dock);

	//undoLimit = currentGlobalParams.getInt("UNDO_LIMIT");
	currentViewContainer()->undoStack->setUndoLimit(100);
	qDebug() << "first undo limit: " << undoLimit;

	

	


	unitStatus = new QPushButton("Unit");
	unitStatus->setStyleSheet(" QPushButton{	border:0pxsolid #8f8f91;		border-radius: 0px;		background-color: rgb(237, 237, 237);"
	"font: 75 7pt \"Arial\"; }"
	);
	QString unitS;
	if (currentGlobalParams.getEnum("DISPLAY_UNIT")==0)
		unitS = QString("Unit : mm");
	else if(currentGlobalParams.getEnum("DISPLAY_UNIT") == 1)
		unitS = QString("Unit : Inch");
	unitStatus->setText(unitS);
	connect(unitStatus, SIGNAL(clicked()), this, SLOT(startsetting()));
	statusBar()->addWidget(unitStatus);
	QApplication::instance()->setAttribute(Qt::AA_DontShowIconsInMenus, true);//menu no icon

	isReset = false;

	QDir tempTexPath = PicaApplication::getRoamingDir() + "temptexture";
	if (tempTexPath.exists())
		tempTexPath.removeRecursively();
	setting3DP2 = 0;
	//printOption_ = 0;

	vboTest = 1;
	filterResult = true;
	//get GDPR_setting
	//if get GDPR

	//else
	//if(GDPR_setting)
		//run dialog
		//save setting
	//else
	/*
		GPRD Setting
	*/
	/*QSettings xyzReg;
	//if (xyzReg.childGroups().contains("s", Qt::CaseInsensitive))
	if (xyzReg.contains("GDPR_SETTING"))
	{
		if (xyzReg.value("GDPR_SETTING").toInt() == 1)
		{
			gdprResult = QDialog::Accepted;
		}
		else if (xyzReg.value("GDPR_SETTING").toInt() == 0)
		{
			gdprResult = QDialog::Rejected;
		}
	}
	else
	{		
		GDPRDialog *test = new GDPRDialog();
		gdprResult = test->exec();
		
		if (gdprResult == QDialog::Accepted)
		{
			const QString gdpr_Setting("GDPR_SETTING");
			settings.setValue(gdpr_Setting, 1);
		}
		else if (gdprResult == QDialog::Rejected)
		{
			const QString gdpr_Setting("GDPR_SETTING");
			settings.setValue(gdpr_Setting, 0);
		}
	}*/

	

	

}


void MainWindow::matrix_changed_2(QSet<int> multiSelectID, const Matrix44m &execute_matrix)
{
	currentViewContainer()->undoStack->push(new Translate_Command(this, meshDoc(), multiSelectID, execute_matrix, false));
}
void MainWindow::matrix_changed(MeshModel *sm, const Matrix44m &tt_matrix)
{
	currentViewContainer()->undoStack->push(new Matrix_command(this, sm, tt_matrix, false));

}
void MainWindow::scale_changed(QSet<int> multiSelectID, const Point3f &scale_rate)
{
	currentViewContainer()->undoStack->push(new Scale_command(this, meshDoc(), multiSelectID, scale_rate, false));

}
void MainWindow::rotate_changed(QSet<int> multiSelectID, Point3d _rotation, QList<Point3f> _old_center)
{
	currentViewContainer()->undoStack->push(new Rotate_command(this, meshDoc(), multiSelectID, _rotation, _old_center, false));
}

void MainWindow::packing_occur(QSet<int> multiSelectID,QMap<int, Point3f> savePos)
{
	currentViewContainer()->undoStack->push(new packing_command(this, meshDoc(), multiSelectID,savePos, false));
}

void MainWindow::packing_rotate_occur(QSet<int> multiSelectID,QMap<int, Point3f> savePos, QMap<int, Point3d> firstRotation, QMap<int, Point3d> secondRotation)
{
	currentViewContainer()->undoStack->push(new packing_rotate_command(this, meshDoc(), multiSelectID,savePos, firstRotation, secondRotation, false));
}

void MainWindow::createUndoView()
{
	unDoAct = undoGroup->createUndoAction(this, tr("&Undo"));
	unDoAct->setShortcuts(QKeySequence::Undo);
	unDoAct->setIcon(QIcon(":/images/icons/btn_undo_2x.png"));
	reDoAct = undoGroup->createRedoAction(this, tr("&Redo"));
	reDoAct->setShortcutContext(Qt::ApplicationShortcut);
	reDoAct->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_Z);
	//reDoAct->setShortcuts(QKeySequence::Redo);
	
	connect(unDoAct, SIGNAL(triggered()), this, SLOT(updateMenus()));
	connect(reDoAct,SIGNAL(triggered()),this,SLOT(updateMenus()));

	undoView = new QUndoView();
	undoView->setGroup(undoGroup);
	undoView->setWindowTitle(tr("Command List"));
	//undoView->show();
	undoView->setAttribute(Qt::WA_QuitOnClose, false);
}

void MainWindow::changeTabIcon(int temp)
{

	switch (temp)
	{
	case 0:
		dockTabWidget->setTabIcon(0, QIcon(":/images/icons/btn_view_on_2x.png"));
		dockTabWidget->setTabIcon(1, QIcon(":/images/icons/btn_cross_2x.png"));
		dockTabWidget->setTabIcon(2, QIcon(":/images/icons/btn_move_2x.png"));
		dockTabWidget->setTabIcon(3, QIcon(":/images/icons/btn_rotate_2x.png"));
		dockTabWidget->setTabIcon(4, QIcon(":/images/icons/btn_scale_2x.png"));
		dockTabWidget->setTabIcon(5, QIcon(":/images/icons/btn_info_2x.png"));
		//dockTabWidget->setTabIcon(6, QIcon(":/images/icons/btn_remove_all_2x.png"));
		break;
	case 1:
		dockTabWidget->setTabIcon(1, QIcon(":/images/icons/btn_cross_on_2x.png"));
		dockTabWidget->setTabIcon(0, QIcon(":/images/icons/btn_view_2x.png"));
		dockTabWidget->setTabIcon(2, QIcon(":/images/icons/btn_move_2x.png"));
		dockTabWidget->setTabIcon(3, QIcon(":/images/icons/btn_rotate_2x.png"));
		dockTabWidget->setTabIcon(4, QIcon(":/images/icons/btn_scale_2x.png"));
		dockTabWidget->setTabIcon(5, QIcon(":/images/icons/btn_info_2x.png"));
		//dockTabWidget->setTabIcon(6, QIcon(":/images/icons/btn_remove_all_2x.png"));
		break;
	case 2:
		dockTabWidget->setTabIcon(2, QIcon(":/images/icons/btn_move_on_2x.png"));
		dockTabWidget->setTabIcon(0, QIcon(":/images/icons/btn_view_2x.png"));
		dockTabWidget->setTabIcon(1, QIcon(":/images/icons/btn_cross_2x.png"));
		dockTabWidget->setTabIcon(3, QIcon(":/images/icons/btn_rotate_2x.png"));
		dockTabWidget->setTabIcon(4, QIcon(":/images/icons/btn_scale_2x.png"));
		dockTabWidget->setTabIcon(5, QIcon(":/images/icons/btn_info_2x.png"));
		//dockTabWidget->setTabIcon(6, QIcon(":/images/icons/btn_remove_all_2x.png"));
		break;
	case 3:
		dockTabWidget->setTabIcon(3, QIcon(":/images/icons/btn_rotate_on_2x.png"));
		dockTabWidget->setTabIcon(0, QIcon(":/images/icons/btn_view_2x.png"));
		dockTabWidget->setTabIcon(1, QIcon(":/images/icons/btn_cross_2x.png"));
		dockTabWidget->setTabIcon(2, QIcon(":/images/icons/btn_move_2x.png"));
		dockTabWidget->setTabIcon(4, QIcon(":/images/icons/btn_scale_2x.png"));
		dockTabWidget->setTabIcon(5, QIcon(":/images/icons/btn_info_2x.png"));
		//dockTabWidget->setTabIcon(6, QIcon(":/images/icons/btn_remove_all_2x.png"));
		break;
	case 4:
		dockTabWidget->setTabIcon(4, QIcon(":/images/icons/btn_scale_on_2x.png"));
		dockTabWidget->setTabIcon(0, QIcon(":/images/icons/btn_view_2x.png"));
		dockTabWidget->setTabIcon(1, QIcon(":/images/icons/btn_cross_2x.png"));
		dockTabWidget->setTabIcon(2, QIcon(":/images/icons/btn_move_2x.png"));
		dockTabWidget->setTabIcon(3, QIcon(":/images/icons/btn_rotate_2x.png"));
		dockTabWidget->setTabIcon(5, QIcon(":/images/icons/btn_info_2x.png"));
		//dockTabWidget->setTabIcon(6, QIcon(":/images/icons/btn_remove_all_2x.png"));
		break;
	case 5:
		dockTabWidget->setTabIcon(5, QIcon(":/images/icons/btn_info_on_2x.png"));
		dockTabWidget->setTabIcon(0, QIcon(":/images/icons/btn_view_2x.png"));
		dockTabWidget->setTabIcon(1, QIcon(":/images/icons/btn_cross_2x.png"));
		dockTabWidget->setTabIcon(2, QIcon(":/images/icons/btn_move_2x.png"));
		dockTabWidget->setTabIcon(3, QIcon(":/images/icons/btn_rotate_2x.png"));
		dockTabWidget->setTabIcon(4, QIcon(":/images/icons/btn_scale_2x.png"));
		//dockTabWidget->setTabIcon(6, QIcon(":/images/icons/btn_remove_all_2x.png"));
		break;
		//case 6:
		//{		  //dockTabWidget->setTabIcon(6, QIcon(":/images/icons/btn_remove_all_on_2x.png"));
		//		  dockTabWidget->setTabIcon(5, QIcon(":/images/icons/btn_info_2x.png"));
		//		  dockTabWidget->setTabIcon(0, QIcon(":/images/icons/btn_view_2x.png"));
		//		  dockTabWidget->setTabIcon(1, QIcon(":/images/icons/btn_move_2x.png"));
		//		  dockTabWidget->setTabIcon(2, QIcon(":/images/icons/btn_rotate_2x.png"));
		//		  dockTabWidget->setTabIcon(3, QIcon(":/images/icons/btn_scale_2x.png"));
		//		  dockTabWidget->setTabIcon(4, QIcon(":/images/icons/btn_cross_2x.png"));
		//		  //deleteAct->trigger();
		//		  
		//}break;
	default:
		break;
	}
	updateMenus();
}
void MainWindow::dockTabClicked(int temp)
{
	switch (temp)
	{
	case 6:
		deleteAct->trigger();
		break;
	}
}
void MainWindow::createQDockWidget2()
{
	dock = new QDockWidget(tr("DockWindow1"), this);
	//dock->setFixedSize(300, 1200);
	fw1 = new DockLeftWidget(this);
	dock->setWidget(fw1);
	//dock->setWidget(pTabWidget);
}
void MainWindow::createQDockWidget3()
{
	dock = new QDockWidget(tr("DockWindow1"), this);
	listWidget1 = new QListWidget();
	listWidget1->setFixedWidth(100);

	QListWidgetItem *listItem1 = new QListWidgetItem(tr("View"), listWidget1);
	QListWidgetItem *listItem2 = new QListWidgetItem(tr("Transform"), listWidget1);
	QListWidgetItem *listItem3 = new QListWidgetItem(tr("Rotate"), listWidget1);
	QListWidgetItem *listItem4 = new QListWidgetItem(tr("Scale"), listWidget1);


	stackWidget1 = new QStackedWidget();
	/*stackWidget1->addWidget(new QPushButton("pt1"));
	stackWidget1->addWidget(new QPushButton("pt2"));
	stackWidget1->addWidget(new QPushButton("pt3"));*/

	/*QTabWidget *tab = new QTabWidget;
	tab->tabPosition*/
	QGroupBox *viewDirGB = new QGroupBox();
	QVBoxLayout *viewDirLa = new QVBoxLayout;
	QToolButton *viewTopTB = new QToolButton;
	QToolButton *viewBottomTB = new QToolButton;
	QToolButton *viewRightTB = new QToolButton;
	QToolButton *viewLeftTB = new QToolButton;
	QToolButton *viewFrontTB = new QToolButton;
	QToolButton *viewBackTB = new QToolButton;
	//QWidget *spacerWidget = new QWidget();//***空白spacer
	//spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	//spacerWidget->setVisible(true);


	viewTopTB->setDefaultAction(viewTopAct);
	viewTopTB->setText("Back");
	viewBottomTB->setDefaultAction(viewBottomAct);
	viewBottomTB->setText("Front");

	viewRightTB->setDefaultAction(viewRightAct);
	viewRightTB->setText("Right");
	viewLeftTB->setDefaultAction(viewLeftAct);
	viewLeftTB->setText("Left");

	viewFrontTB->setDefaultAction(viewFrontAct);
	viewFrontTB->setText("Top");
	viewBackTB->setDefaultAction(viewBackAct);
	viewBackTB->setText("Bottom");

	viewDirLa->addWidget(viewTopTB);
	viewDirLa->addWidget(viewBottomTB);
	viewDirLa->addWidget(viewRightTB);
	viewDirLa->addWidget(viewLeftTB);
	viewDirLa->addWidget(viewFrontTB);
	viewDirLa->addWidget(viewBackTB);
	viewDirLa->addStretch();
	//viewDirLa->addWidget(spacerWidget);



	viewDirGB->setLayout(viewDirLa);




	MeshEditInterFace_v2 *iEdit = PM.stringEditMap_v2.last();
	QAction *editAction = iEdit->AC(PM.stringEditMap_v2.lastKey());

	transformwidgett1 = new TransformWidget(editAction, iEdit, this);
	stackWidget1->addWidget(transformwidgett1);

	stackWidget1->addWidget(viewDirGB);
	//stackWidget1->addWidget(genGroupbox.at(0));
	//stackWidget1->addWidget(genGroupbox.at(1));
	//stackWidget1->addWidget(genGroupbox.at(2));
	connect(listWidget1, SIGNAL(currentRowChanged(int)), stackWidget1, SLOT(setCurrentIndex(int)));

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(listWidget1);
	layout->addWidget(stackWidget1);

	QFrame *qframe1 = new QFrame();

	qframe1->setLayout(layout);

	dock->setWidget(qframe1);
	//genGroupbox

}
QDockWidget* MainWindow::createQDockWidget4()
{
	dockTabWidget = createDockTabWidget();
	//dockTabWidget->setFixedWidth(198);
	dockTabWidget->setIconSize(QSize(48, 48));

	dockTabWidget->setStyleSheet(WidgetStyleSheet::viewTabWidgetStyleSheet());


	dockTabWidget->setTabIcon(0, QIcon(":/images/icons/btn_view_2x.png"));
	dockTabWidget->setTabIcon(1, QIcon(":/images/icons/btn_cross_2x.png"));
	dockTabWidget->setTabIcon(2, QIcon(":/images/icons/btn_move_2x.png"));
	dockTabWidget->setTabIcon(3, QIcon(":/images/icons/btn_rotate_2x.png"));
	dockTabWidget->setTabIcon(4, QIcon(":/images/icons/btn_scale_2x.png"));
	dockTabWidget->setTabIcon(5, QIcon(":/images/icons/btn_info_2x.png"));
	//dockTabWidget->setTabIcon(6, QIcon(":/images/icons/btn_remove_all_2x.png"));
	//dockTabWidget->settab

	switch (dockTabWidget->currentIndex())
	{
	case 0:
		dockTabWidget->setTabIcon(0, QIcon(":/images/icons/btn_view_on_2x.png"));
		break;
	case 1:
		dockTabWidget->setTabIcon(1, QIcon(":/images/icons/btn_cross_on_2x.png"));
		break;	
	case 2:
		dockTabWidget->setTabIcon(2, QIcon(":/images/icons/btn_move_on_2x.png"));
		break;		
	case 3:
		dockTabWidget->setTabIcon(3, QIcon(":/images/icons/btn_rotate_on_2x.png"));
		break;		
	case 4:
		dockTabWidget->setTabIcon(4, QIcon(":/images/icons/btn_scale_on_2x.png"));
		break;
	case 5:
		dockTabWidget->setTabIcon(5, QIcon(":/images/icons/btn_info_on_2x.png"));
		break;
		/*case 6:
		dockTabWidget->setTabIcon(6, QIcon(":/images/icons/btn_remove_all_on_2x.png"));
		break;*/
	default:
		break;
	}

	connect(dockTabWidget, SIGNAL(currentChanged(int)), this, SLOT(changeTabIcon(int)));
	//connect(dockTabWidget, SIGNAL(tabBarClicked(int)),this, SLOT(dockTabClicked(int)));

	QDockWidget *tempdock = new QDockWidget;// new QDockWidget(tr("Function"));	
	tempdock->setWidget(dockTabWidget);


	return tempdock;

}
QTabWidget* MainWindow::createDockTabWidget()
{
	dockTabViewWidget = createDockViewTabWidget();
	//dockTabTranslateWidget = createDockTranslateTabWidget();
	//dockTabRotateWidget = createDockRotateTabWidget();
	//dockTabScaleWidget = createDockScaleTabWidget();
	dockCrossSectionWidget = createDockCrossSection2();
	dockInformationWidget = createDockInformationWidget();
	dockInformationWidget2 = createDockInformationWidget2();
	//deleteTabWidget = new QWidget();
	connect(dockInformationWidget2, SIGNAL(sOAswitch()), this, SLOT(updateMenus()));
	connect(dockInformationWidget2, SIGNAL(estimateSIG()), this, SLOT(printJobEstimate()));

	QTabWidget *temptab = new QTabWidget();
	temptab->setTabPosition(QTabWidget::West);
	temptab->addTab(dockTabViewWidget, "");

	/*temptab->addTab(dockTabTranslateWidget, "Translate");
	temptab->addTab(dockTabRotateWidget, "Rotate");
	temptab->addTab(dockTabScaleWidget, "Scale");*/

	temptab->addTab(dockCrossSectionWidget, "");

	MeshEditInterFace_v2 *iEdit = *PM.stringEditMap_v2.find("edit_translate");
	QAction *editAction = iEdit->AC(PM.stringEditMap_v2.lastKey());
	transformwidgett1 = new TransformWidget(editAction, iEdit, this, "", meshDoc());
	transformwidgett1->curmwi = this;
	temptab->addTab(transformwidgett1, "");

	MeshEditInterFace_v2 *iEdit_rotate = *PM.stringEditMap_v2.find("edit_rotate");
	QAction *editRotateAction = iEdit_rotate->AC("edit_rotate");
	rotate_widget_cus = new RotateWidget_Customized(editRotateAction, iEdit_rotate, this, "", meshDoc());
	rotate_widget_cus->curmwi = this;
	temptab->addTab(rotate_widget_cus, "");

	MeshEditInterFace_v2 *iEdit_scale = *PM.stringEditMap_v2.find("edit_scale");
	QAction *editScaleAction = iEdit_scale->AC("edit_scale");
	scale_widget_cus = new ScaleWidget_Customized(editScaleAction, iEdit_scale, this, "", meshDoc());
	scale_widget_cus->curmwi = this;
	temptab->addTab(scale_widget_cus, "");


	
	temptab->addTab(dockInformationWidget2, "");

	temptab->setTabToolTip(0, tr("View"));
	temptab->setTabToolTip(1, tr("Cross Section"));
	temptab->setTabToolTip(2, tr("Move"));
	temptab->setTabToolTip(3, tr("Rotate"));
	temptab->setTabToolTip(4, tr("Resize"));
	temptab->setTabToolTip(5, tr("Info"));


	//temptab->addTab(dockInformationWidget, "");
	//temptab->setStyleSheet(WidgetStyleSheet::tabWidgetStyleSheet());



	//temptab->addTab(deleteTabWidget, "");



	return temptab;
}
QWidget *MainWindow::createDockViewTabWidget()
{
	QFrame *viewDirGB = new QFrame();
	viewDirGB->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	QVBoxLayout *viewDirLa = new QVBoxLayout;

	QVBoxLayout *allLayout = new QVBoxLayout;
	QHBoxLayout *zoomLa = new QHBoxLayout;

	QToolButton *viewTab = new QToolButton;

	QToolButton *viewZoomInTB = new QToolButton;
	QToolButton *viewZoomOutTB = new QToolButton;

	QToolButton *viewTopTB = new QToolButton;
	QToolButton *viewBottomTB = new QToolButton;
	QToolButton *viewRightTB = new QToolButton;
	QToolButton *viewLeftTB = new QToolButton;
	QToolButton *viewFrontTB = new QToolButton;
	QToolButton *viewBackTB = new QToolButton;
	QToolButton *viewOriginalTB = new QToolButton;
	QToolButton *viewLockTB = new QToolButton;
	QToolButton *view2DViewTB = new QToolButton;

	QToolButton *viewResetTB = new QToolButton;



	//QWidget *spacerWidget = new QWidget();//***空白spacer
	//spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	//spacerWidget->setVisible(true);


	viewTab->setText(tr("VIEW"));
	viewTab->setStyleSheet(WidgetStyleSheet::viewTextStyleSheet(":/images/icons/tab_name_bg_on.png", ":/images/icons/tab_name_bg_off.png"));
	viewTab->setToolTipDuration(1);

	viewZoomInTB->setDefaultAction(zoomInAct);
	viewZoomInTB->setFixedSize(48, 48);
	viewZoomInTB->setIconSize(QSize(48, 48));
	viewZoomInTB->setToolTipDuration(1);
	//viewZoomInTB->setText("ZoomIn");	
	viewZoomOutTB->setDefaultAction(zoomOutAct);
	viewZoomOutTB->setFixedSize(48, 48);
	viewZoomOutTB->setIconSize(QSize(48, 48));
	viewZoomOutTB->setToolTipDuration(1);
	//viewZoomOutTB->setText("ZoomOut");	
	zoomLa->addWidget(viewZoomInTB);
	zoomLa->addWidget(viewZoomOutTB);

	viewTopTB->setDefaultAction(viewTopAct);
	viewTopTB->setStyleSheet(WidgetStyleSheet::viewToolButtonStyleSheet(":/images/icons/btn_view_back.png", ":/images/icons/btn_view_back_disable.png"));
	viewTopTB->setText(tr("   Back"));

	viewBottomTB->setDefaultAction(viewBottomAct);
	viewBottomTB->setIcon(QIcon(""));
	//viewBottomTB->setToolButtonStyle(Qt::ToolButtonTextUnderIcon); 	
	viewBottomTB->setStyleSheet(WidgetStyleSheet::viewToolButtonStyleSheet(":/images/icons/btn_view_front.png", ":/images/icons/btn_view_front_disable.png"));
	//viewBottomTB->setStyleSheet("QToolButton {\n"
	//	"   border: none;\n"
	//	"   background: url(:/images/icons/btn_view_front.png) top center no-repeat;\n"
	//	//"   padding-top: 200px;\n"
	//	"   width: 100px;\n"
	//	"   height :30px;\n"
	//	"   font: bold 9pt \"Arial\";\n"
	//	"   color: white;\n"
	//	"}\n");
	//
	viewBottomTB->setText(tr("    FRONT"));
	viewBottomTB->setToolTipDuration(1);

	viewRightTB->setDefaultAction(viewRightAct);
	viewRightTB->setIcon(QIcon(""));
	viewRightTB->setStyleSheet(WidgetStyleSheet::viewToolButtonStyleSheet(":/images/icons/btn_view_side.png", ":/images/icons/btn_view_right_disable.png"));
	viewRightTB->setText(tr("    RIGHT"));
	viewRightTB->setToolTipDuration(1);

	viewLeftTB->setDefaultAction(viewLeftAct);
	viewLeftTB->setIcon(QIcon(""));
	viewLeftTB->setStyleSheet(WidgetStyleSheet::viewToolButtonStyleSheet(":/images/icons/btn_view_left.png", ":/images/icons/btn_view_left_disable.png"));
	viewLeftTB->setText(tr("    Left"));
	viewLeftTB->setToolTipDuration(1);

	viewFrontTB->setDefaultAction(viewFrontAct);
	viewFrontTB->setIcon(QIcon(""));
	viewFrontTB->setStyleSheet(WidgetStyleSheet::viewToolButtonStyleSheet(":/images/icons/btn_view_top.png", ":/images/icons/btn_view_top_disable.png"));
	viewFrontTB->setText(tr("    TOP"));
	viewFrontTB->setToolTipDuration(1);

	viewBackTB->setDefaultAction(viewBackAct);
	viewBackTB->setIcon(QIcon(""));
	viewBackTB->setStyleSheet(WidgetStyleSheet::viewToolButtonStyleSheet(":/images/icons/btn_view_bottom.png", ":/images/icons/btn_view_bottom_disable.png"));
	viewBackTB->setText(tr("   Bottom"));
	viewBackTB->setToolTipDuration(1);

	viewOriginalTB->setDefaultAction(viewOriginalAct);
	viewOriginalTB->setIcon(QIcon(""));
	viewOriginalTB->setStyleSheet(WidgetStyleSheet::viewToolButtonStyleSheet(":/images/icons/btn_view_original.png", ":/images/icons/btn_view_original_disable.png"));
	viewOriginalTB->setText(tr("     ORIGINAL"));
	viewOriginalTB->setToolTipDuration(1);

	viewLockTB = new QToolButton();
	viewLockTB->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	viewLockTB->setStyleSheet(WidgetStyleSheet::viewToolButtonStyleSheet(":/images/icons/btn_lock_view.png", ":/images/icons/btn_lock_view_disable.png"));
	viewLockTB->setCheckable(true);
	viewLockTB->setDefaultAction(this->lockViewAct);
	viewLockTB->setText(tr("     LOCK"));
	viewLockTB->setToolTipDuration(1);

	view2DViewTB = new QToolButton();
	view2DViewTB->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	view2DViewTB->setCheckable(true);
	view2DViewTB->setIcon(QIcon(""));
	view2DViewTB->setDefaultAction(this->topTagAction);
	view2DViewTB->setText(tr("2D_View"));

	viewResetTB = new QToolButton();
	viewResetTB->setStyleSheet(WidgetStyleSheet::viewDockWidgetBottomTB(":/images/icons/btn_reset.png"));
	viewResetTB->setText(tr("Reset"));


	//***********************
	QLabel *lockLB = new QLabel("Lock Viewport");
	lockLB->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	QHBoxLayout *hlayout = new QHBoxLayout;
	QLabel *onLB = new QLabel(tr("on"));
	onLB->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	onLB->setFixedHeight(36);

	QLabel *offLB = new QLabel(tr("off"));
	offLB->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	offLB->setFixedHeight(36);


	QCheckBox *view_lockCB = new QCheckBox(tr(""));
	view_lockCB->setStyleSheet(WidgetStyleSheet::check_onoff_styleSheet());
	//bt1->setStyleSheet(WidgetStyleSheet::checkBoxStyleSheet());
	connect(view_lockCB, SIGNAL(toggled(bool)), lockViewAct, SLOT(setChecked(bool)));

	hlayout->addWidget(offLB, 0, Qt::AlignRight);
	hlayout->addWidget(view_lockCB, 0, Qt::AlignLeft);
	hlayout->addWidget(onLB);


	//****************************************




	//viewDirLa->addWidget(viewZoomInTB);
	//viewDirLa->addWidget(viewZoomOutTB);
	viewDirLa->addWidget(viewTab, 0, Qt::AlignHCenter);
	viewDirLa->addLayout(zoomLa);
	viewDirLa->addWidget(viewBottomTB, 0, Qt::AlignHCenter);
	viewDirLa->addWidget(viewRightTB, 0, Qt::AlignHCenter);
	viewDirLa->addWidget(viewFrontTB, 0, Qt::AlignHCenter);
	viewDirLa->addWidget(viewTopTB, 0, Qt::AlignHCenter);
	viewDirLa->addWidget(viewLeftTB, 0, Qt::AlignHCenter);
	viewDirLa->addWidget(viewBackTB, 0, Qt::AlignHCenter);
	viewDirLa->addWidget(viewOriginalTB, 0, Qt::AlignHCenter);
	//viewDirLa->addWidget(viewLockTB, 0,Qt::AlignHCenter);
	viewDirLa->addWidget(lockLB, 0, Qt::AlignHCenter);
	viewDirLa->addLayout(hlayout, 0);
	viewDirLa->insertSpacing(0, 0);
	viewDirLa->setSpacing(20);



	allLayout->addLayout(viewDirLa);
	allLayout->setAlignment(viewDirLa, Qt::AlignTop);
	/*allLayout->addWidget(viewResetTB);
	allLayout->setAlignment(viewResetTB, Qt::AlignBottom);*/
	allLayout->setMargin(0);



	//viewDirLa->setMargin(0);


	//viewDirLa->addWidget(view2DViewTB);
	//viewDirLa->addWidget(spacerWidget);

	//viewDirLa->addStretch();
	viewDirGB->setLayout(allLayout);

	//QWidget *temp = new QWidget();
	return viewDirGB;
}
QWidget *MainWindow::createDockTranslateTabWidget()
{
	QVBoxLayout *dockTranslateLay = new QVBoxLayout;

	QWidget *spacerWidget = new QWidget();//***空白spacer
	spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	spacerWidget->setVisible(true);

	dockTranslateLay->addWidget(genGroupbox[0]);
	dockTranslateLay->addWidget(spacerWidget);

	QWidget *temp = new QWidget();
	temp->setLayout(dockTranslateLay);
	return temp;
}
QWidget *MainWindow::createDockRotateTabWidget()
{
	QVBoxLayout *dockTranslateLay = new QVBoxLayout;

	QWidget *spacerWidget = new QWidget();//***空白spacer
	spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	spacerWidget->setVisible(true);

	dockTranslateLay->addWidget(genGroupbox[1]);
	dockTranslateLay->addWidget(spacerWidget);

	QWidget *temp = new QWidget();
	temp->setLayout(dockTranslateLay);
	return temp;
}
QWidget *MainWindow::createDockScaleTabWidget()
{
	QVBoxLayout *dockTranslateLay = new QVBoxLayout;

	QWidget *spacerWidget = new QWidget();//***空白spacer
	spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	spacerWidget->setVisible(true);

	dockTranslateLay->addWidget(genGroupbox[2]);
	dockTranslateLay->addWidget(spacerWidget);

	QWidget *temp = new QWidget();
	temp->setLayout(dockTranslateLay);
	return temp;
}
QWidget *MainWindow::createDockInformationWidget()
{
	//if (meshDoc()->mm() != NULL){
	originalNameLabel = new QLabel(tr("Original Name"));
	buildHeightLabel = new QLabel(tr("Build Height"));;
	dimensionLabel = new QLabel(tr("Dimension"));
	numOfverticesLabel = new QLabel(tr("Vertices"));
	numOfFaceLabel = new QLabel(tr("Faces"));

	originalName = new QLabel();
	buildHeight = new QLabel;
	dimension = new QLabel;
	numOfvertices = new QLabel;
	numOfFace = new QLabel;

	QToolButton *infoTab = new QToolButton;

	infoTab->setText(tr("INFO"));
	infoTab->setStyleSheet(WidgetStyleSheet::viewTextStyleSheet(":/images/icons/tab_name_bg_on.png", ":/images/icons/tab_name_bg_off.png"));
	infoTab->setToolTipDuration(1);

	QToolButton *singleObjectInfo = new QToolButton;
	QToolButton *printEstimateTB = new QToolButton;
	connect(printEstimateTB, SIGNAL(clicked()), this, SLOT(printJobEstimate()));
	printEstimateTB->setText(tr("        ESTIMATE"));
	printEstimateTB->setStyleSheet(WidgetStyleSheet::viewDockWidgetBottomTB(":/images/icons/btn_print_estimate.png", ":/images/icons/btn_print_estimate_disable.png"));

	originalNameLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	buildHeightLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	dimensionLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	numOfverticesLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	numOfFaceLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

	originalName->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
	buildHeight->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	dimension->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	numOfvertices->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	numOfFace->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	originalName->setFixedWidth(100);
	originalName->setWordWrap(true);

	originalNameLabel->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	buildHeightLabel->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	dimensionLabel->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	numOfverticesLabel->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	numOfFaceLabel->setStyleSheet(WidgetStyleSheet::textStyleSheet());

	originalName->setStyleSheet(WidgetStyleSheet::textStyleSheet2());
	buildHeight->setStyleSheet(WidgetStyleSheet::textStyleSheet2());
	dimension->setStyleSheet(WidgetStyleSheet::textStyleSheet2());
	numOfvertices->setStyleSheet(WidgetStyleSheet::textStyleSheet2());
	numOfFace->setStyleSheet(WidgetStyleSheet::textStyleSheet2());

	//}
	QWidget *spacerWidget = new QWidget();//***空白spacer
	spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	QVBoxLayout *vLayout = new QVBoxLayout;
	//QGridLayout *vLayout = new QGridLayout;
	/*vLayout->addWidget(originalNameLabel, 0, 0);
	vLayout->addWidget(originalName, 1, 0);

	vLayout->addWidget(buildHeightLabel, 2, 0);
	vLayout->addWidget(buildHeight, 3, 0);

	vLayout->addWidget(dimensionLabel, 4, 0);
	vLayout->addWidget(dimension, 5, 0);

	vLayout->addWidget(numOfverticesLabel, 6, 0);
	vLayout->addWidget(numOfvertices, 7, 0);

	vLayout->addWidget(numOfFaceLabel, 8, 0);
	vLayout->addWidget(numOfFace, 9, 0);

	vLayout->addWidget(spacerWidget);
	vLayout->setMargin(10);*/


	vLayout->addWidget(originalNameLabel, 0, Qt::AlignLeft);
	vLayout->addWidget(originalName, 0, Qt::AlignLeft | Qt::AlignTop);

	vLayout->addWidget(buildHeightLabel, 0, Qt::AlignLeft);
	vLayout->addWidget(buildHeight, 0, Qt::AlignLeft | Qt::AlignTop);

	vLayout->addWidget(dimensionLabel, 0, Qt::AlignLeft);
	vLayout->addWidget(dimension, 0, Qt::AlignLeft | Qt::AlignTop);

	vLayout->addWidget(numOfverticesLabel, 0, Qt::AlignLeft);
	vLayout->addWidget(numOfvertices, 0, Qt::AlignLeft | Qt::AlignTop);

	vLayout->addWidget(numOfFaceLabel, 0, Qt::AlignLeft);
	vLayout->addWidget(numOfFace, 0, Qt::AlignLeft | Qt::AlignTop);




	vLayout->addWidget(spacerWidget);
	vLayout->insertSpacing(0, 20);
	vLayout->setSpacing(20);

	vLayout->setMargin(20);



	/*vLayout->insertSpacing(1, 50);
	vLayout->insertSpacing(2, 10);
	vLayout->insertSpacing(3, 50);
	vLayout->insertSpacing(4, 10);
	vLayout->insertSpacing(5, 50);
	vLayout->insertSpacing(6, 10);
	vLayout->insertSpacing(7, 50);
	vLayout->insertSpacing(8, 10);*/



	QVBoxLayout *vLayout2 = new QVBoxLayout;
	
	vLayout2->addLayout(vLayout);
	vLayout2->addWidget(infoTab, 0, Qt::AlignHCenter);
	vLayout2->addWidget(printEstimateTB, 0, Qt::AlignBottom);
	vLayout2->setMargin(0);

	//vLayout->setSpacing(0);





	//QVBoxLayout *vLayout = new QVBoxLayout;		
	//vLayout->addWidget(originalNameLabel);
	//vLayout->addWidget(originalName);
	//vLayout->addWidget(buildHeightLabel);
	//vLayout->addWidget(buildHeight);
	//vLayout->addWidget(dimensionLabel);
	//vLayout->addWidget(dimension);
	//vLayout->addWidget(numOfverticesLabel);
	//vLayout->addWidget(numOfvertices);
	//vLayout->addWidget(numOfFaceLabel);
	//vLayout->addWidget(numOfFace);
	////vLayout->addWidget(spacerWidget);
	//vLayout->addStretch();


	QWidget *temp = new QWidget;
	temp->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	temp->setLayout(vLayout2);
	return temp;
}
DockInfoWIdget *MainWindow::createDockInformationWidget2()
{
	DockInfoWIdget *temp = new DockInfoWIdget();
	temp->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	return temp;

}
//QWidget *MainWindow::createDockCrossSection()
//{
//	delOl(MeshModel::meshsort::slice_item);
//
//	QVBoxLayout *vLayout = new QVBoxLayout;
//	QGridLayout *docklayout = new QGridLayout;
//
//	QDoubleSpinBox *slice_positionSpinBox = new QDoubleSpinBox;
//	slice_positionSpinBox->setMinimum(-100);
//	slice_positionSpinBox->setSingleStep(0.1);
//	slice_positionSpinBox->setStyleSheet(WidgetStyleSheet::spinBoxStyleSheet());
//	slice_positionSpinBox->setFixedSize(100, 24);
//
//	QDoubleSpinBox *slice_position_10_SpinBox = new QDoubleSpinBox;
//	slice_position_10_SpinBox->setMinimum(-100);
//	slice_position_10_SpinBox->setSingleStep(1.0);
//	//slice_position_10_SpinBox->setValue(-10);
//	slice_position_10_SpinBox->setStyleSheet(WidgetStyleSheet::spinBoxStyleSheet());
//	slice_position_10_SpinBox->setFixedSize(100, 24);
//
//	//***********************
//	QHBoxLayout *hlayout = new QHBoxLayout;
//	QLabel *onLB = new QLabel(tr("on"));
//	onLB->setStyleSheet(WidgetStyleSheet::textStyleSheet());
//	onLB->setFixedHeight(36);
//
//	QLabel *offLB = new QLabel(tr("off"));
//	offLB->setStyleSheet(WidgetStyleSheet::textStyleSheet());
//	offLB->setFixedHeight(36);
//
//
//	QCheckBox *view_2dCB = new QCheckBox(tr(""));
//	view_2dCB->setStyleSheet(WidgetStyleSheet::check_onoff_styleSheet());
//	//bt1->setStyleSheet(WidgetStyleSheet::checkBoxStyleSheet());
//	connect(view_2dCB, SIGNAL(toggled(bool)), topTagAction, SLOT(setChecked(bool)));
//
//
//
//	hlayout->addWidget(offLB, 0, Qt::AlignRight);
//	hlayout->addWidget(view_2dCB, 0, Qt::AlignLeft);
//	hlayout->addWidget(onLB);
//
//	QHBoxLayout *hlayoutSB1 = new QHBoxLayout;
//	QLabel *sb1Label = new QLabel(tr("+0.1 mm"));
//	sb1Label->setStyleSheet(WidgetStyleSheet::textStyleSheet());
//	sb1Label->setFixedHeight(36);
//
//	hlayoutSB1->addWidget(slice_positionSpinBox, 0, Qt::AlignVCenter);
//	hlayoutSB1->addWidget(sb1Label, 0, Qt::AlignVCenter);
//
//	QHBoxLayout *hlayoutSB10 = new QHBoxLayout;
//	QLabel *sb10Label = new QLabel(tr("+1.0 mm"));
//	sb10Label->setStyleSheet(WidgetStyleSheet::textStyleSheet());
//	sb10Label->setFixedHeight(36);
//
//	hlayoutSB10->addWidget(slice_position_10_SpinBox, 0, Qt::AlignVCenter);
//	hlayoutSB10->addWidget(sb10Label, 0, Qt::AlignVCenter);
//
//
//	//*****************************
//
//	DoubleSlider *sl1 = new DoubleSlider;
//	//sl1->setFocusPolicy(Qt::ClickFocus);
//	sl1->setRange(-10000, 10000);
//	sl1->setSingleStep(10);
//	
//
//	//sl1->setTickPosition(QSlider::TicksBothSides);
//	//sl1->setTickInterval(10);
//
//
//
//	/*QToolButton *bt1 = new QToolButton;
//	bt1->setDefaultAction(topTagAction);*/
//
//
//	layerLabel = new QLabel;
//	layerLabel->setNum(0);
//	layerLabel->setStyleSheet(WidgetStyleSheet::textStyleSheet());
//	layerLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
//	connect(sl1, SIGNAL(doubleValueChanged(double)), this, SLOT(setlayerLabel(double)));
//
//
//	
//	//****************************************
//	QWidget *spacerWidget = new QWidget();//***空白spacer
//	spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
//
//	//vLayout->addWidget(bt1, 0, Qt::AlignHCenter);
//	vLayout->addLayout(hlayout);
//	vLayout->addLayout(hlayoutSB1);
//	vLayout->addLayout(hlayoutSB10);
//	vLayout->addWidget(layerLabel, 0, Qt::AlignHCenter);
//	vLayout->addWidget(sl1, 0, Qt::AlignHCenter);
//	vLayout->addWidget(spacerWidget, 5, 0);
//	vLayout->insertSpacing(0, 30);
//	vLayout->setSpacing(20);
//
//
//	/*docklayout->setAlignment(sl1, Qt::AlignTop);
//	docklayout->setAlignment(layerLabel, Qt::AlignTop);
//	docklayout->setAlignment(slice_positionSpinBox, Qt::AlignTop);
//	docklayout->setAlignment(slice_position_10_SpinBox, Qt::AlignTop);*/
//	//docklayout->setSpacing(0);
//
//	//docklayout->addWidget(genGroupbox.last());
//
//	//connect(slice_position_10_SpinBox, SIGNAL(valueChanged(double)), sl1, SLOT(doubleSetValue(double)));
//
//	connect(sl1, SIGNAL(doubleValueChanged(double)), slice_positionSpinBox, SLOT(setValue(double)));
//	connect(slice_positionSpinBox, SIGNAL(valueChanged(double)), slice_position_10_SpinBox, SLOT(setValue(double)));
//	connect(slice_position_10_SpinBox, SIGNAL(valueChanged(double)), sl1, SLOT(doubleSetValue(double)));
//	connect(slice_positionSpinBox, SIGNAL(valueChanged(double)), GLA(), SLOT(set_equ3(double)));
//	connect(slice_positionSpinBox, SIGNAL(valueChanged(double)), this, SLOT(executeSliceFunc(double)));	
//
//
//	if (meshDoc()->count_print_item() > 0)testslicefunc(*meshDoc(), GLA()->equ[3]);
//	//sl1->setValue(-100);
//
//
//	QFrame *frame1 = new QFrame();
//	frame1->setLayout(vLayout);
//	//frame1->setFixedWidth(300);
//
//	/*QWidget *temp = new QWidget();
//	temp->setLayout(docklayout);*/
//
//
//	//dock = new QDockWidget(tr("DockWindow1"), this);
//	//dock->setWidget(frame1);
//
//
//	/*buildFileLayout->addWidget(genGroupbox.first());
//	buildFileLayout->addWidget(genGroupbox[1]);
//	buildFileLayout->addWidget(genGroupbox[2]);*/
//	sl1->setValue(-10000);
//	return frame1;
//}

QWidget *MainWindow::createDockCrossSection2()
{
	delOl(MeshModel::meshsort::slice_item);

	QVBoxLayout *vLayout = new QVBoxLayout;

	//QGridLayout *gridL1 = new QGridLayout;

	QToolButton *crossTab = new QToolButton;

	crossTab->setText(tr("CROSS SECTION"));
	crossTab->setStyleSheet(WidgetStyleSheet::viewTextStyleSheet(":/images/icons/tab_name_bg_on.png", ":/images/icons/tab_name_bg_off.png"));
	crossTab->setToolTipDuration(1);

	/*sliceSpinBox = new QDoubleSpinBox;
	sliceSpinBox->setMinimum(0);
	sliceSpinBox->setMaximum(2000);
	sliceSpinBox->setSingleStep(5);
	sliceSpinBox->setDecimals(0);
	sliceSpinBox->setStyleSheet(WidgetStyleSheet::spinBoxStyleSheet());
	sliceSpinBox->setFixedSize(100, 24);
	sliceSpinBox->setSuffix(" slices");
	sliceSpinBox->setAlignment(Qt::AlignHCenter);*/

	sliceSpinBox = new QDoubleSpinBox;
	sliceSpinBox->setMinimum(0);
	sliceSpinBox->setMaximum(1968);
	sliceSpinBox->setSingleStep(5);
	sliceSpinBox->setDecimals(0);
	sliceSpinBox->setStyleSheet(WidgetStyleSheet::spinBoxStyleSheet());
	sliceSpinBox->setFixedSize(100, 24);
	sliceSpinBox->setSuffix(" slices");
	sliceSpinBox->setAlignment(Qt::AlignHCenter);

	crossUnitLabel = new QLabel(tr("Unit : mm"));
	crossUnitLabel->setFrameShape(QFrame::NoFrame);
	crossUnitLabel->setFixedHeight(28);
	crossUnitLabel->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	QLabel *twoDLabel = new QLabel(tr("2D View"));
	twoDLabel->setFrameShape(QFrame::NoFrame);
	twoDLabel->setFixedHeight(28);
	twoDLabel->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	QLabel *outlineLabel = new QLabel(tr("Cross Section Outline"));
	outlineLabel->setFrameShape(QFrame::NoFrame);
	outlineLabel->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	outlineLabel->setFixedHeight(28);
	QHBoxLayout *hlayout = new QHBoxLayout;
	QHBoxLayout *hlayout2 = new QHBoxLayout;
	QLabel *onLB = new QLabel(tr("on"));
	onLB->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	onLB->setFrameShape(QFrame::NoFrame);
	onLB->setFixedHeight(28);
	QLabel *onLB2 = new QLabel(tr("on"));
	onLB2->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	onLB2->setFrameShape(QFrame::NoFrame);
	onLB2->setFixedHeight(28);

	QLabel *offLB = new QLabel(tr("off"));
	offLB->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	offLB->setFrameShape(QFrame::NoFrame);
	offLB->setFixedHeight(28);
	QLabel *offLB2 = new QLabel(tr("off"));
	offLB2->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	offLB2->setFrameShape(QFrame::NoFrame);
	offLB2->setFixedHeight(28);

	view_2dCB = new QCheckBox(tr(""));
	view_2dCB->setStyleSheet(WidgetStyleSheet::check_onoff_styleSheet());
	view_outline = new QCheckBox(tr(""));
	view_outline->setChecked(false);
	view_outline->setStyleSheet(WidgetStyleSheet::check_onoff_styleSheet());

	hlayout->addWidget(offLB, 0, Qt::AlignRight);
	hlayout->addWidget(view_2dCB, 0, Qt::AlignLeft);
	hlayout->addWidget(onLB);

	hlayout2->addWidget(offLB2, 0, Qt::AlignRight);
	hlayout2->addWidget(view_outline, 0, Qt::AlignLeft);
	hlayout2->addWidget(onLB2);

	/*crossSlider = new DoubleSlider;
	crossSlider->setRange(-1000, 1000);
	crossSlider->setSingleStep(1);
	crossSlider->setValue(-1000);*/

	crossSlider = new DoubleSlider;
	crossSlider->setRange(-984, 984);
	crossSlider->setSingleStep(1);
	crossSlider->setValue(-984);

	
	up5 = new QToolButton();	
	up5->setFixedSize(44, 26);
	//up5->setIconSize(QSize(60, 60));
	//up5->setAutoRaise(true);
	up5->setStyleSheet(WidgetStyleSheet::viewTextStyleSheet(":/images/icons/btn-add-label-5.png", ":/images/icons/btn-add-label-5-disable.png"));
	connect(up5, SIGNAL(clicked()), crossSlider, SLOT(doubleAdd5Step()));

	
	down5 = new QToolButton();
	down5->setFixedSize(44, 26);
	down5->setStyleSheet(WidgetStyleSheet::viewTextStyleSheet(":/images/icons/btn-reduce-label-5.png", ":/images/icons/btn-reduce-label-5-disable.png"));	
	//down5->setAutoRaise(true);
	connect(down5, SIGNAL(clicked()), crossSlider, SLOT(doubleMinus5Step()));

	/*QWidget *spacerWidget = new QWidget();
	spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);*/

	vLayout->addWidget(crossTab, 0, Qt::AlignCenter);
	vLayout->addWidget(crossUnitLabel, 0, Qt::AlignCenter);
	vLayout->addWidget(twoDLabel, 0, Qt::AlignCenter);
	vLayout->addLayout(hlayout);
	vLayout->addWidget(outlineLabel, 0, Qt::AlignCenter);
	vLayout->addLayout(hlayout2);
	vLayout->addWidget(sliceSpinBox, 0, Qt::AlignCenter);

	vLayout->addWidget(up5, 0, Qt::AlignHCenter);
	vLayout->addWidget(crossSlider, 0, Qt::AlignHCenter);
	vLayout->addWidget(down5, 0, Qt::AlignHCenter);

	vLayout->addStretch();
	//vLayout->addWidget(spacerWidget, 5, 0);
	vLayout->insertSpacing(0, -9);
	vLayout->setSpacing(10);

	connect(view_2dCB, SIGNAL(toggled(bool)), topTagAction, SLOT(setChecked(bool)));
	connect(crossSlider, SIGNAL(doubleValueChanged(double)), this, SLOT(setSliceSpinBox(double)));
	connect(sliceSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setSliderPosition(double)));

	connect(view_outline, SIGNAL(clicked()), this, SLOT(view_outlineSlot()));



	connect(this, SIGNAL(slicePositionChanged(double)), GLA(), SLOT(set_equ3(double)));
	connect(this, SIGNAL(slicePositionChanged(double)), this, SLOT(executeSliceFunc(double)));
	//connect(this, SIGNAL(updateMeshChanged()), this, SLOT(testFuncFunc()));


	QFrame *frame1 = new QFrame();
	frame1->setLayout(vLayout);

	return frame1;
}

void MainWindow::init()
{
	/*PM.loadPlugins(defaultGlobalParams);
	QSettings settings;
	QVariant vers = settings.value(PicaApplication::versionRegisterKeyName());
	//should update those values only after I run MeshLab for the very first time or after I installed a new version
	if (!vers.isValid() || vers.toString() < PicaApplication::appVer())
	{
	settings.setValue(PicaApplication::pluginsPathRegisterKeyName(),PluginManager::getDefaultPluginDirPath());
	settings.setValue(PicaApplication::versionRegisterKeyName(),PicaApplication::appVer());
	settings.setValue(PicaApplication::wordSizeKeyName(),QSysInfo::WordSize);
	foreach(QString plfile,PM.pluginsLoaded)
	settings.setValue(PluginManager::osIndependentPluginName(plfile),PicaApplication::appVer());
	}
	// Now load from the registry the settings and  merge the hardwired values got from the PM.loadPlugins with the ones found in the registry.
	loadMeshLabSettings();
	createActions();
	createToolBars();
	createMenus();
	stddialog = 0;
	xmldialog = 0;
	setAcceptDrops(true);
	mdiarea->setAcceptDrops(true);
	setWindowTitle(PicaApplication::completeName(PicaApplication::HW_ARCHITECTURE(QSysInfo::WordSize)));
	setStatusBar(new QStatusBar(this));
	globalStatusBar()=statusBar();
	qb=new QProgressBar(this);//進度表
	qb->setMaximum(100);
	qb->setMinimum(0);
	qb->reset();
	statusBar()->addPermanentWidget(qb,0);
	//updateMenus();
	newProject();
	//PM should be initialized before passing it to PluginGeneratorGUI
	plugingui = new PluginGeneratorGUI(PM,this);
	plugingui->setAllowedAreas (    Qt::LeftDockWidgetArea | Qt::BottomDockWidgetArea | Qt::RightDockWidgetArea);
	addDockWidget(Qt::LeftDockWidgetArea,plugingui);
	connect(plugingui,SIGNAL(scriptCodeExecuted(const QScriptValue&,const int,const QString&)),this,SLOT(scriptCodeExecuted(const QScriptValue&,const int,const QString&)));
	connect(plugingui,SIGNAL(insertXMLPluginRequested(const QString&,const QString& )),this,SLOT(loadAndInsertXMLPlugin(const QString&,const QString&)));
	connect(plugingui,SIGNAL(historyRequest()),this,SLOT(sendHistory()));
	//QWidget* wid = reinterpret_cast<QWidget*>(ar->parent());
	//wid->showMaximized();
	//ar->update();

	//qb->setAutoClose(true);
	//qb->setMinimumDuration(0);
	//qb->reset();
	connect(this, SIGNAL(updateLayerTable()), layerDialog, SLOT(updateTable()));
	connect(layerDialog,SIGNAL(removeDecoratorRequested(QAction*)),this,SLOT(switchOffDecorator(QAction*)));*/
}
void MainWindow::testslot()
{
	qDebug() << "fuck_crash()";
	//GLA()->setHastoRefreshVBO();
	//GLA()->updateTexture();

}

void MainWindow::createActions()//實體化action，與函示連結上
{
	searchShortCut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_F), this);
	searchShortCut->setAutoRepeat(false);
	searchShortCut->setContext(Qt::ApplicationShortcut);
	//////////////Action Menu File ////////////////////////////////////////////////////////////////////////////
	//newProjectAct = new QAction(QIcon(":/images/new_project.png"), tr("New Empty Project..."), this);
	newProjectAct = new QAction(tr("New Project"), this);
	newProjectAct->setShortcutContext(Qt::ApplicationShortcut);
	newProjectAct->setShortcut(Qt::CTRL + Qt::Key_N);
	connect(newProjectAct, SIGNAL(triggered()), this, SLOT(newProject()));

	openProjectAct = new QAction(QIcon(":/images/icons/btn_open_project_2x.png"), tr("&Open Project"), this);
	openProjectAct->setShortcutContext(Qt::ApplicationShortcut);
	openProjectAct->setShortcut(Qt::CTRL + Qt::Key_O);
	connect(openProjectAct, SIGNAL(triggered()), this, SLOT(openProject2()));//1 is pip, 2 is 3mf

	importFromProjectAct = new QAction(tr("Import Project..."), this);
	connect(importFromProjectAct, SIGNAL(triggered()), this, SLOT(importFromProject()));

	appendProjectAct = new QAction(tr("Append project to current..."), this);
	connect(appendProjectAct, SIGNAL(triggered()), this, SLOT(appendProject()));

	saveProjectAsAct = new QAction(tr("S&ave Project as..."), this);
	saveProjectAsAct->setShortcutContext(Qt::ApplicationShortcut);
	saveProjectAsAct->setShortcut(Qt::Key_F12);
	connect(saveProjectAsAct, SIGNAL(triggered()), this, SLOT(saveProjectAs()));//****20160614***test//2 is pip, 3 is3mf

	saveProjectAct = new QAction(QIcon(":/images/icons/btn_save_2x.png"), tr("&Save Project"), this);
	saveProjectAct->setShortcutContext(Qt::ApplicationShortcut);
	saveProjectAct->setShortcut(Qt::CTRL + Qt::Key_S);
	connect(saveProjectAct, SIGNAL(triggered()), this, SLOT(saveProject()));



	//***20150903***執行Sirius,操控memjet head程式。
	sirius_settingAct = new QAction(tr("Sirius..."), this);//***20150831
	connect(sirius_settingAct, SIGNAL(triggered()), this, SLOT(executeSirius()));
	//connect(sirius_settingAct, SIGNAL(triggered()), this, SLOT(executeSirius()));
	//***20151013***tempfunction
	//connect(threeDPrintAct, SIGNAL(triggered()), this, SLOT(fill_fileNameList("abc")));

	printjobestimateAct = new QAction(QIcon(tr(":/images/icons/btn_printing_estimate.png")), tr("Estimate Usage"), this);
	printjobestimateAct->setToolTip("Estimate Usage");
	connect(printjobestimateAct, SIGNAL(triggered()), this, SLOT(printJobEstimate()));


	closeProjectAct = new QAction(tr("Close Project"), this);
	closeProjectAct->setShortcutContext(Qt::WidgetShortcut);
	closeProjectAct->setShortcut(Qt::CTRL+ Qt::Key_W);
	connect(closeProjectAct, SIGNAL(triggered()), mdiarea, SLOT(closeActiveSubWindow()));	
	connect(closeProjectAct, SIGNAL(triggered()), this, SLOT(testslot()));

	
	//connect(closeProjectAct, SIGNAL(triggered()),this, SLOT(closeProjectWindow()));
	importMeshAct = new QAction(QIcon(":/images/icons/btn_import_modal_2x.png"), tr("&Import Model"), this);
	importMeshAct->setShortcutContext(Qt::ApplicationShortcut);
	importMeshAct->setShortcut(Qt::CTRL + Qt::Key_I);
	connect(importMeshAct, SIGNAL(triggered()), this, SLOT(importMeshWithLayerManagement()));

	exportMeshAct = new QAction(QIcon(":/images/icons/btn_save_2x.png"), tr("&Export Mesh..."), this);
	exportMeshAct->setShortcutContext(Qt::ApplicationShortcut);
	//exportMeshAct->setShortcut(Qt::CTRL + Qt::Key_E);
	//connect(exportMeshAct, SIGNAL(triggered()), this, SLOT(save()));

	exportMeshAsAct = new QAction(QIcon(":/images/icons/btn_save_2x.png"), tr("&Save Selected Model as STL"), this);
	connect(exportMeshAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));
	exportMeshAsAct2 = new QAction(QIcon(":/images/icons/btn_save_2x.png"), tr("&Save Selected Model as STL"), this);
	connect(exportMeshAsAct2, SIGNAL(triggered()), this, SLOT(saveAs()));


	//reloadMeshAct = new QAction(QIcon(":/images/reload.png"), tr("&Reload"), this);
	reloadMeshAct = new QAction(tr("&Reset Model"), this);
	reloadMeshAct->setShortcutContext(Qt::ApplicationShortcut);
	//reloadMeshAct->setShortcut(Qt::ALT + Qt::Key_R);
	connect(reloadMeshAct, SIGNAL(triggered()), this, SLOT(reload()));

	reloadAllMeshAct = new QAction(tr("&Reload All"), this);
	reloadAllMeshAct->setShortcutContext(Qt::ApplicationShortcut);
	reloadAllMeshAct->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_R);
	connect(reloadAllMeshAct, SIGNAL(triggered()), this, SLOT(reloadAllMesh()));

	importRasterAct = new QAction(QIcon(":/images/open.png"), tr("Import Raster..."), this);
	connect(importRasterAct, SIGNAL(triggered()), this, SLOT(importRaster()));

	saveSnapshotAct = new QAction(QIcon(":/images/snapshot.png"), tr("Save snapsho&t"), this);
	connect(saveSnapshotAct, SIGNAL(triggered()), this, SLOT(saveSnapshot()));

	for (int i = 0; i < MAXRECENTFILES; ++i)
	{
		recentProjActs[i] = new QAction(this);
		recentProjActs[i]->setVisible(true);
		recentProjActs[i]->setEnabled(true);
		recentProjActs[i]->setShortcut(QKeySequence(Qt::ALT + Qt::SHIFT + Qt::Key_1 + i));

		recentFileActs[i] = new QAction(this);
		recentFileActs[i]->setVisible(true);
		recentFileActs[i]->setEnabled(true);
		recentFileActs[i]->setShortcutContext(Qt::ApplicationShortcut);
		recentFileActs[i]->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_1 + i));
		connect(recentProjActs[i], SIGNAL(triggered()), this, SLOT(openRecentProj()));
		connect(recentFileActs[i], SIGNAL(triggered()), this, SLOT(openRecentMesh()));
	}

	exitAct = new QAction(tr("E&xit"), this);
	exitAct->setShortcut(Qt::CTRL + Qt::Key_Q);
	connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

	//////////////Render Actions for Toolbar and Menu /////////////////////////////////////////////////////////
	QList<RenderModeAction*> rendlist;
	renderModeGroupAct = new QActionGroup(this);

	renderBboxAct = new RenderModeBBoxAction(renderModeGroupAct);
	renderBboxAct->setCheckable(true);
	rendlist.push_back(renderBboxAct);

	renderModePointsAct = new RenderModePointsAction(renderModeGroupAct);
	renderModePointsAct->setCheckable(true);
	rendlist.push_back(renderModePointsAct);

	renderModeWireAct = new RenderModeWireAction(renderModeGroupAct);
	renderModeWireAct->setCheckable(true);
	rendlist.push_back(renderModeWireAct);

	//******20150306新增*************************************
	renderModeTexWireAct = new RenderModeTexWireAction(renderModeGroupAct);
	renderModeTexWireAct->setCheckable(true);
	rendlist.push_back(renderModeTexWireAct);
	//***************************************************
	//******2150319新增*************************************
	renderModeOutlineTesselation = new RenderModeOutlineTesselation(renderModeGroupAct);
	renderModeOutlineTesselation->setCheckable(true);
	rendlist.push_back(renderModeOutlineTesselation);
	//***************************************************

	renderModeFlatLinesAct = new RenderModeFlatLinesAction(renderModeGroupAct);
	renderModeFlatLinesAct->setCheckable(true);
	rendlist.push_back(renderModeFlatLinesAct);

	renderModeFlatAct = new RenderModeFlatAction(renderModeGroupAct);
	renderModeFlatAct->setCheckable(true);
	rendlist.push_back(renderModeFlatAct);

	renderModeSmoothAct = new RenderModeSmoothAction(renderModeGroupAct);
	renderModeSmoothAct->setCheckable(true);
	rendlist.push_back(renderModeSmoothAct);

	renderModeTextureWedgeAct = new RenderModeTexturePerWedgeAction(this);
	renderModeTextureWedgeAct->setCheckable(true);
	rendlist.push_back(renderModeTextureWedgeAct);

	//renderModeTextureWedgeAct  = new RenderModeTexturePerWedgeAction(this);
	//renderModeTextureWedgeAct->setCheckable(true);
	//rendlist.push_back(renderModeTextureWedgeAct);

	setLightAct = new RenderModeLightOnOffAction(this);
	setLightAct->setCheckable(true);
	rendlist.push_back(setLightAct);

	setDoubleLightingAct = new RenderModeDoubleLightingAction(this);
	setDoubleLightingAct->setCheckable(true);
	setDoubleLightingAct->setShortcutContext(Qt::ApplicationShortcut);
	setDoubleLightingAct->setShortcut(Qt::CTRL + Qt::Key_D);
	rendlist.push_back(setDoubleLightingAct);

	setFancyLightingAct = new RenderModeFancyLightingAction(this);
	setFancyLightingAct->setCheckable(true);
	setFancyLightingAct->setShortcutContext(Qt::ApplicationShortcut);
	setFancyLightingAct->setShortcut(Qt::CTRL + Qt::Key_Y);
	rendlist.push_back(setFancyLightingAct);

	backFaceCullAct = new RenderModeFaceCullAction(this);
	backFaceCullAct->setCheckable(true);
	backFaceCullAct->setShortcutContext(Qt::ApplicationShortcut);
	backFaceCullAct->setShortcut(Qt::CTRL + Qt::Key_K);
	rendlist.push_back(backFaceCullAct);

	setSelectFaceRenderingAct = new RenderModeSelectedFaceRenderingAction(this);
	setSelectFaceRenderingAct->setCheckable(true);
	rendlist.push_back(setSelectFaceRenderingAct);

	setSelectVertRenderingAct = new RenderModeSelectedVertRenderingAction(this);
	setSelectVertRenderingAct->setCheckable(true);
	rendlist.push_back(setSelectVertRenderingAct);

	connectRenderModeActionList(rendlist);//********觸發函式為mainwindow_RunTime的updateRenderMode

										  //////////////Action Menu View ////////////////////////////////////////////////////////////////////////////
	fullScreenAct = new QAction(tr("&FullScreen"), this);
	fullScreenAct->setCheckable(true);
	fullScreenAct->setShortcutContext(Qt::ApplicationShortcut);
	fullScreenAct->setShortcut(Qt::ALT + Qt::Key_Return);
	connect(fullScreenAct, SIGNAL(triggered()), this, SLOT(fullScreen()));

	showToolbarStandardAct = new QAction(tr("&Standard"), this);
	showToolbarStandardAct->setCheckable(true);
	showToolbarStandardAct->setChecked(true);
	connect(showToolbarStandardAct, SIGNAL(triggered()), this, SLOT(showToolbarFile()));

	showToolbarRenderAct = new QAction(tr("&Render"), this);
	showToolbarRenderAct->setCheckable(true);
	showToolbarRenderAct->setChecked(true);
	connect(showToolbarRenderAct, SIGNAL(triggered()), this, SLOT(showToolbarRender()));

	showInfoPaneAct = new QAction(tr("Show Info &Panel"), this);
	showInfoPaneAct->setCheckable(true);
	connect(showInfoPaneAct, SIGNAL(triggered()), this, SLOT(showInfoPane()));


	showTrackBallAct = new QAction(tr("Show &Trackball"), this);
	showTrackBallAct->setCheckable(true);
	connect(showTrackBallAct, SIGNAL(triggered()), this, SLOT(showTrackBall()));

	resetTrackBallAct = new QAction(tr("Reset &Trackball"), this);
	resetTrackBallAct->setShortcutContext(Qt::ApplicationShortcut);
#if defined(Q_OS_MAC)
	resetTrackBallAct->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_H);
#else
	resetTrackBallAct->setShortcut(Qt::CTRL + Qt::Key_H);
#endif
	connect(resetTrackBallAct, SIGNAL(triggered()), this, SLOT(resetTrackBall()));

	showLayerDlgAct = new QAction(QIcon(":/images/layers.png"), tr("Show Layer Dialog"), this);
	showLayerDlgAct->setCheckable(true);
	showLayerDlgAct->setChecked(true);
	showLayerDlgAct->setShortcut(Qt::CTRL + Qt::Key_L);
	connect(showLayerDlgAct, SIGNAL(triggered(bool)), this, SLOT(showLayerDlg(bool)));

	showObjectListAct = new QAction(QIcon(":/images/icons/btn_object_list_2x.png"), tr("Object List"), this);
	showObjectListAct->setCheckable(true);
	showObjectListAct->setChecked(false);
	showObjectListAct->setShortcut(Qt::CTRL + Qt::Key_L);
	connect(showObjectListAct, SIGNAL(triggered(bool)), this, SLOT(showObjectList(bool)));

	/*showRasterAct = new QAction(QIcon(":/images/view_raster.png"), tr("Show Current Raster Mode"), this);
	showRasterAct->setCheckable(true);
	showRasterAct->setChecked(true);
	showRasterAct->setShortcut(Qt::SHIFT + Qt::Key_R);
	connect(showRasterAct, SIGNAL(triggered()), this, SLOT(showRaster()));*/


	resetViewAct = new QAction(tr("Reset View"), this);
	connect(resetViewAct, SIGNAL(triggered()), this, SLOT(transferFilterNameSlot()));
	//zoomInAct = new QAction(tr("Zoom In"),this);
	//zoomOutAct = new QAction(tr("Zoom Out"),this);
	wireFrameModeAct = new QAction(tr("Wireframe Mode"), this);
	pointModeAct = new QAction(tr("Point Mode"), this);
	//Sydny 01-19-2017 added "showFunctionMenuAct"
	showFunctionMenuAct = new QAction(tr("Function Menu"), this);


	//////////////Action Menu EDIT /////////////////////////////////////////////////////////////////////////
	suspendEditModeAct = new QAction(QIcon(":/images/no_edit.png"), tr("Not editing"), this);
	suspendEditModeAct->setShortcut(Qt::Key_Escape);
	suspendEditModeAct->setCheckable(true);
	suspendEditModeAct->setChecked(true);
	connect(suspendEditModeAct, SIGNAL(triggered()), this, SLOT(suspendEditMode()));

	//***20160419
	copyAct = new QAction(QIcon(":/images/icons/btn_copy.png"), tr("Copy"), this);
	//***20170116 --- added shortcut for QAction copyAct and changed the slot connected to it: by KPPH R&D-I-SW, Mark
	copyAct->setShortcut(QKeySequence::Copy);
	connect(copyAct, SIGNAL(triggered()), this, SLOT(setCopyMeshes()));

	pasteAct = new QAction(QIcon(":/images/icons/btn_paste_2x.png"), tr("Paste"), this);
	//***20170116 --- added shortcut for QAction pasteAct and changed the slot connected to it: by KPPH R&D-I-SW, Mark
	pasteAct->setShortcut(QKeySequence::Paste);
	connect(pasteAct, SIGNAL(triggered()), this, SLOT(pasteSelectMeshes()), Qt::DirectConnection);
	//connect(pasteAct, SIGNAL(triggered()), this, SLOT(copyNumber()));
	
	deleteAct = new QAction(QIcon(":/images/icons/btn_remove_2x.png"), tr("Delete"), this);
	deleteAct->setShortcut(QKeySequence::Delete);
	connect(deleteAct, SIGNAL(triggered()), this, SLOT(delMeshes()));

	moveToCenterAct = new QAction(tr("MoveToCenter"), this);
	connect(moveToCenterAct, SIGNAL(triggered()), this, SLOT(move_to_center_ground()));



	//SYNDY 07/11/2017
	copyAct_right = new QAction(tr("Copy"), this);
	connect(copyAct_right, SIGNAL(triggered()), this, SLOT(setCopyMeshes()));

	pasteAct_right = new QAction(tr("Paste"), this);
	connect(pasteAct_right, SIGNAL(triggered()), this, SLOT(pasteSelectMeshes()));	

	pasteManyAct_right = new QAction(tr("Paste many"), this);	
	connect(pasteManyAct_right, SIGNAL(triggered()), this, SLOT(copyNumber()));

	deleteAct_right = new QAction(tr("Delete"), this);
	connect(deleteAct_right, SIGNAL(triggered()), this, SLOT(delMeshes()));

	newProjectAct_right = new QAction(tr("New Project"), this);
	connect(newProjectAct_right, SIGNAL(triggered()), this, SLOT(newProject()));

	landAct_right = new QAction(tr("Land"), this);
	connect(landAct_right, SIGNAL(triggered()), this, SLOT(transferFilterNameSlot()));

	//////////////Action Menu WINDOWS /////////windowsk的menu////////////////////////////////////////////////////////////////
	windowsTileAct = new QAction(tr("&Tile"), this);
	connect(windowsTileAct, SIGNAL(triggered()), mdiarea, SLOT(tileSubWindows()));

	windowsCascadeAct = new QAction(tr("&Cascade"), this);
	connect(windowsCascadeAct, SIGNAL(triggered()), mdiarea, SLOT(cascadeSubWindows()));

	windowsNextAct = new QAction(tr("&Next"), this);
	connect(windowsNextAct, SIGNAL(triggered()), mdiarea, SLOT(activateNextSubWindow()));

	closeAllAct = new QAction(tr("Close &All Windows"), this);
	connect(closeAllAct, SIGNAL(triggered()), mdiarea, SLOT(closeAllSubWindows()));

	setSplitGroupAct = new QActionGroup(this);	setSplitGroupAct->setExclusive(true);

	setSplitHAct = new QAction(/*QIcon(":/images/splitH.png"),*/ tr("&Horizontally"), setSplitGroupAct);
	setSplitVAct = new QAction(/*QIcon(":/images/splitV.png"), */tr("&Vertically"), setSplitGroupAct);

	connect(setSplitGroupAct, SIGNAL(triggered(QAction *)), this, SLOT(setSplit(QAction *)));

	setUnsplitAct = new QAction(tr("&Close current view"), this);
	connect(setUnsplitAct, SIGNAL(triggered()), this, SLOT(setUnsplit()));

	linkViewersAct = new QAction(tr("Link Viewers"), this);
	linkViewersAct->setCheckable(true);
	connect(linkViewersAct, SIGNAL(triggered()), this, SLOT(linkViewers()));

	viewFromGroupAct = new QActionGroup(this);	viewFromGroupAct->setExclusive(true);
	viewFromGroupActTB = new QActionGroup(this);	viewFromGroupActTB->setExclusive(true);

	/*viewTopAct	    = new QAction(tr("Top"),viewFromGroupAct);
	viewBottomAct	  = new QAction(tr("Bottom"),viewFromGroupAct);
	viewLeftAct	    = new QAction(tr("Left"),viewFromGroupAct);
	viewRightAct	  = new QAction(tr("Right"),viewFromGroupAct);
	viewFrontAct	  = new QAction(tr("Front"),viewFromGroupAct);
	viewBackAct	    = new QAction(tr("Back"),viewFromGroupAct);*/

	viewTopAct = new QAction(tr("Back View"), viewFromGroupAct);
	viewBottomAct = new QAction(QIcon(":/images/icons/btn_menu_front.png"), tr("Front View"), viewFromGroupAct);
	viewLeftAct = new QAction(tr("Left View"), viewFromGroupAct);
	viewRightAct = new QAction(QIcon(":/images/icons/btn_menu_side.png"), tr("Right View"), viewFromGroupAct);
	viewFrontAct = new QAction(QIcon(":/images/icons/btn_menu_top.png"), tr("Top View"), viewFromGroupAct);
	viewBackAct = new QAction(tr("Bottom View"), viewFromGroupAct);
	viewOriginalAct = new QAction(QIcon(":/images/icons/btn_menu_original.png"), tr("Original View"), viewFromGroupAct);

	viewBottomActTB = new QAction(QIcon(":/images/icons/btn_menu_front.png"), tr("Front View"), viewFromGroupActTB);
	viewSideActTB = new QAction(QIcon(":/images/icons/btn_menu_side.png"), tr("Side View"), viewFromGroupActTB);
	viewFrontActTB = new QAction(QIcon(":/images/icons/btn_menu_top.png"), tr("Top View"), viewFromGroupActTB);
	viewOriginalActTB = new QAction(QIcon(":/images/icons/btn_menu_original.png"), tr("Original View"), viewFromGroupActTB);

	lockViewAct = new QAction(QIcon(":/images/icons/btn_view_locker_2x.png"), tr("Lock View"), this);
	lockViewAct->setToolTip("Lock View");
	lockViewAct->setCheckable(true);

	selectionModeSwitchAct = new QAction(tr("Selection/View"), this);
	selectionModeSwitchAct->setToolTip("Selection/View");
	selectionModeSwitchAct->setCheckable(true);
	selectionModeSwitchAct->setChecked(true);

	// keyboard shortcuts for canonical viewdirections, blender style
	/*viewFrontAct->setShortcut(Qt::Key_End);
	viewBackAct->setShortcut(Qt::CTRL + Qt::Key_End);
	viewRightAct->setShortcut(Qt::Key_PageDown);
	viewLeftAct->setShortcut(Qt::CTRL + Qt::Key_PageDown);
	viewTopAct->setShortcut(Qt::Key_Home);
	viewBottomAct->setShortcut(Qt::CTRL + Qt::Key_Home);*/

	connect(viewFromGroupAct, SIGNAL(triggered(QAction *)), this, SLOT(viewFrom(QAction *)));
	connect(viewFromGroupActTB, SIGNAL(triggered(QAction *)), this, SLOT(viewFrom(QAction *)));	
	connect(lockViewAct, SIGNAL(toggled(bool)), this, SLOT(lock_View(bool)));
	connect(selectionModeSwitchAct, SIGNAL(toggled(bool)), this, SLOT(selection_View(bool)));

	viewFromMeshAct = new QAction(tr("View from Mesh Camera"), this);
	viewFromRasterAct = new QAction(tr("View from Raster Camera"), this);
	viewFromRasterAct->setShortcut(Qt::CTRL + Qt::Key_J);
	viewFromFileAct = new QAction(tr("View from file"), this);
	connect(viewFromFileAct, SIGNAL(triggered()), this, SLOT(readViewFromFile()));
	connect(viewFromMeshAct, SIGNAL(triggered()), this, SLOT(viewFromCurrentMeshShot()));
	connect(viewFromRasterAct, SIGNAL(triggered()), this, SLOT(viewFromCurrentRasterShot()));

	copyShotToClipboardAct = new QAction(tr("Copy shot"), this);
	copyShotToClipboardAct->setShortcut(QKeySequence::Copy);
	connect(copyShotToClipboardAct, SIGNAL(triggered()), this, SLOT(copyViewToClipBoard()));

	pasteShotFromClipboardAct = new QAction(tr("Paste shot"), this);
	pasteShotFromClipboardAct->setShortcut(QKeySequence::Paste);
	connect(pasteShotFromClipboardAct, SIGNAL(triggered()), this, SLOT(pasteViewFromClipboard()));

	//////////////Action Menu Filters /////////////filter的menu////////////////////////////////////////////////////////
	lastFilterAct = new QAction(tr("Apply filter"), this);
	lastFilterAct->setShortcutContext(Qt::ApplicationShortcut);
	lastFilterAct->setShortcut(Qt::CTRL + Qt::Key_P);
	lastFilterAct->setEnabled(false);
	connect(lastFilterAct, SIGNAL(triggered()), this, SLOT(applyLastFilter()));

	showFilterScriptAct = new QAction(tr("Show current filter script"), this);
	showFilterScriptAct->setEnabled(false);
	connect(showFilterScriptAct, SIGNAL(triggered()), this, SLOT(showFilterScript()));

	//////////////Action Menu Preferences //////////////tools menu中option的setting///////////////////////////////////////////////////////
	setCustomizeAct = new QAction(tr("&Options..."), this);
	connect(setCustomizeAct, SIGNAL(triggered()), this, SLOT(setCustomize()));

	//////////////Action Menu About ///////////////////////////////////////////////////////////////////////////
	aboutAct = new QAction(QIcon(":/images/SKT_images/help.png"), tr("&About"), this);
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

	aboutPluginsAct = new QAction(tr("About &Plugins"), this);
	connect(aboutPluginsAct, SIGNAL(triggered()), this, SLOT(aboutPlugins()));

	onlineHelpAct = new QAction(tr("Online &Documentation"), this);
	connect(onlineHelpAct, SIGNAL(triggered()), this, SLOT(helpOnline()));

	submitBugAct = new QAction(tr("Submit Bug"), this);
	//***20150521
	//connect(submitBugAct, SIGNAL(triggered()), this, SLOT(submitBug()));

	onscreenHelpAct = new QAction(tr("On screen quick help"), this);
	onscreenHelpAct->setShortcut(Qt::Key_F1);
	onscreenHelpAct->setShortcutContext(Qt::ApplicationShortcut);
	//***20150521
	//connect(onscreenHelpAct, SIGNAL(triggered()), this, SLOT(helpOnscreen()));

	checkUpdatesAct = new QAction(tr("Check for updates"), this);
	//***20150521
	//connect(checkUpdatesAct, SIGNAL(triggered()), this, SLOT(checkForUpdates()));
	connect(checkUpdatesAct, SIGNAL(triggered()), this, SLOT(checkForUpdates_2()));

	howToUseThisSWAct = new QAction(tr("User Manual"), this);
	connect(howToUseThisSWAct, &QAction::triggered, [this]() {
		qDebug() << "howToUseThisSWAct execute";		
		QString tmp = "c:\\windows\\explorer.exe\\ ./select,d:\\debug_open_1.txt";//
		QString appEPath = "file:///";
		appEPath.append(qApp->applicationDirPath()).append("./doc/");					
		
		QDesktopServices::openUrl(QUrl(appEPath, QUrl::TolerantMode));
				qDebug() << "App path : " << qApp->applicationDirPath();
	}
	);

	printHistoryAct = new QAction(tr("Print History(Testing)"), this);
	connect(printHistoryAct, &QAction::triggered, [this]() {
		qDebug() << "Print History";		
		
		QProcess process(this);
		QString fileName = QStandardPaths::locate(QStandardPaths::DocumentsLocation, "", QStandardPaths::LocateDirectory) + PicaApplication::appName() + "/printInfoCSV.csv";
		QString printHistoryName = qApp->applicationDirPath() + "./PrintJogHistory.exe";
		QStringList arguments;
		arguments << fileName;		
		process.startDetached("PrintJogHistory.exe", arguments);
	}
	);



	supportResourceAct = new QAction(tr("Support Resource"), this);
	picassoSupportSiteAct = new QAction(tr("Online Support"), this);
	connect(picassoSupportSiteAct, SIGNAL(triggered()), this, SLOT(supportdia()));
	contactSupportAct = new QAction(tr("Contact Support"), this);
	connect(contactSupportAct, SIGNAL(triggered()), this, SLOT(supportdia()));
	aboutPicassoAct = new QAction(tr("About BCPware"), this);
	connect(aboutPicassoAct, SIGNAL(triggered()), this, SLOT(about()));

	///////////////Action Menu Split/Unsplit from handle_螢幕分割////////////////////////////////////////////////////////
	splitGroupAct = new QActionGroup(this);
	unsplitGroupAct = new QActionGroup(this);

	splitUpAct = new QAction(tr("&Up"), splitGroupAct);
	splitDownAct = new QAction(tr("&Down"), splitGroupAct);
	unsplitUpAct = new QAction(tr("&Up"), unsplitGroupAct);
	unsplitDownAct = new QAction(tr("&Down"), unsplitGroupAct);
	splitRightAct = new QAction(tr("&Right"), splitGroupAct);
	splitLeftAct = new QAction(tr("&Left"), splitGroupAct);

	unsplitRightAct = new QAction(tr("&Right"), unsplitGroupAct);
	unsplitLeftAct = new QAction(tr("&Left"), unsplitGroupAct);

	connect(splitGroupAct, SIGNAL(triggered(QAction *)), this, SLOT(splitFromHandle(QAction *)));

	connect(unsplitGroupAct, SIGNAL(triggered(QAction *)), this, SLOT(unsplitFromHandle(QAction *)));

	//TOOL MENU
	showFilterEditAct = new QAction(tr("XML Plugin Editor GUI"), this);
	showFilterEditAct->setEnabled(true);
	connect(showFilterEditAct, SIGNAL(triggered()), this, SLOT(showXMLPluginEditorGui()));

	//***20150803 
	genStdialog = 0;
	groove_settingAct = new QAction(QIcon(":/images/icons/btn_setting_2x.png"), tr("Settings"), this);
	groove_settingAct->setToolTip("Settings");
	connect(groove_settingAct, SIGNAL(triggered()), this, SLOT(startsetting()));

	//***20150825
	topTagAction = new QAction(QIcon(":/images/icons/btn_menu_2dn3d.png"), tr("2D View"), this);
	//newProjectAct->setShortcutContext(Qt::ApplicationShortcut);
	topTagAction->setCheckable(true);
	connect(topTagAction, SIGNAL(toggled(bool)), this, SLOT(setTopTagView(bool)));

	//////////////Action Print Filters /////////////filter的menu////////////////////////////////////////////////////////
	threeDPrintAct = new QAction(QIcon(":/images/icons/btn_printing_2x.png"), tr("Prepare for Printing"), this);//***20150831
																												//connect(threeDPrintAct, SIGNAL(triggered()), this, SLOT(emitconnect()), Qt::DirectConnection);
	threeDPrintAct->setShortcut(Qt::CTRL + Qt::Key_P);
	connect(threeDPrintAct, SIGNAL(triggered()), this, SLOT(printFunction()));
	//connect(threeDPrintAct, SIGNAL(triggered()), this, SLOT(testGetstring()));
	threeDPrintAct2 = new QAction(tr("PicassoJR"), this);//***20150831	PicassoJR
	connect(threeDPrintAct2, SIGNAL(triggered()), this, SLOT(printFunction2()));
	genZXAct = new QAction(tr("Export to Printable File"), this);
	connect(genZXAct, SIGNAL(triggered()), this, SLOT(GenZXFunc()));

	preparePrintintAct = new QAction(tr("Prepare Printing"), this);
	//connect(preparePrintintAct, SIGNAL(triggered()), this, SLOT(showObjectList()));

	showPrintDashBoardAct = new QAction(QIcon(":/images/icons/btn_dashboard_2x.png"), tr("Printer Dashboard"), this);
	showPrintDashBoardAct->setCheckable(true);
	connect(showPrintDashBoardAct, SIGNAL(toggled(bool)), this, SLOT(openDashboard(bool)));

	testPrintAct = new QAction(tr("Test Print(Color Test Page)"), this);
	connect(testPrintAct, SIGNAL(triggered()), this, SLOT(print_test_page_function()));

	testPrintAct2 = new QAction(tr("Test Print(Start Point Measurement)"), this);
	connect(testPrintAct2, SIGNAL(triggered()), this, SLOT(print_test_page_function()));

	testPrintBORAct = new QAction(tr("Test Bor File"), this);
	connect(testPrintBORAct, SIGNAL(triggered()), this, SLOT(print_test_page_function()));
	
	zoomInAct = new QAction(QIcon(":/images/icons/btn_zoomin_2x.png"), tr("Zoom In"), this);
	connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomInSlot()));
	zoomOutAct = new QAction(QIcon(":/images/icons/btn_zoomout_2x.png"), tr("Zoom Out"), this);
	connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOutSlot()));

	changeColorAC = new QAction(QIcon(":/images/icons/btn_color_picker_2x.png"), tr("Change Color"), this);
	changeColorAC->setToolTip("Change Color");
	connect(changeColorAC, SIGNAL(triggered()), this, SLOT(changeMeshColor()));

	tempPicassoJRPrintingAct = new QAction("picassoJR", this);
	connect(tempPicassoJRPrintingAct, SIGNAL(triggered()), this, SLOT(tempPicassoJRPrinting()));

	piColorModeGroupAct = new QActionGroup(this);
	piColorModeGroupAct->setExclusive(true);
	for (int i = 0; i < 3; ++i)
	{
		switch (i)
		{
		case 0:
			changeRendertypeAct[i] = new QAction(tr("Vertex Color"), piColorModeGroupAct);
			changeRendertypeAct[i]->setText(tr("Vertex"));
			connect(changeRendertypeAct[i], SIGNAL(triggered()), this, SLOT(changeCurrentMeshRenderMode()));
			break;
		case 1:
			changeRendertypeAct[i] = new QAction(tr("Face Color"), piColorModeGroupAct);
			changeRendertypeAct[i]->setText(tr("Facet"));
			connect(changeRendertypeAct[i], SIGNAL(triggered()), this, SLOT(changeCurrentMeshRenderMode()));
			break;
		case 2:
			changeRendertypeAct[i] = new QAction(tr("No Color"), piColorModeGroupAct);
			changeRendertypeAct[i]->setText(tr("Texture"));
			connect(changeRendertypeAct[i], SIGNAL(triggered()), this, SLOT(changeCurrentMeshRenderMode()));
			break;

		}
	}
	testDMSlicerAct = new QAction(tr("TestDMSlicer"), this);
	connect(testDMSlicerAct, SIGNAL(triggered()), this, SLOT(genByDMSlicer()));


	testFuncAct = new QAction(tr("TestFuncFunc"), this);
	connect(testFuncAct, SIGNAL(triggered()), this, SLOT(testFuncFunc()));

	detectOverlapping = new QAction(tr("Detect Overlapping"), this);
	connect(detectOverlapping, SIGNAL(triggered()), this, SLOT(detectOverlappingFunc()));

	testFuncAct2 = new QAction(tr("TestFuncAct2"), this);
	connect(testFuncAct2, SIGNAL(triggered()), this, SLOT(testFuncFunc2()));
	testLoad3mfAct3 = new QAction(tr("testLoad3mfAct3"), this);
	connect(testLoad3mfAct3, SIGNAL(triggered()), this, SLOT(testload3mf()));
	testFunc_clearErrorAct = new QAction(tr("testFunc_clearErrorAct"), this);
	connect(testFunc_clearErrorAct, SIGNAL(triggered()), this, SLOT(testClearSiuiusError()));

	testFuncReadZX = new QAction(tr("Open Printable File for Printing"), this);
	//connect(testFuncReadZX, SIGNAL(triggered()), this, SLOT(testReadZX()));
	connect(testFuncReadZX, SIGNAL(triggered()), this, SLOT(testReadPrintingFile()));
	//connect(testFuncReadZX, SIGNAL(triggered()), this, SLOT(testReadZip()));


	selectAllAct = new QAction(tr("Select All"), this);
	selectAllAct->setShortcut(QKeySequence::SelectAll);
	connect(selectAllAct, SIGNAL(triggered()), this, SLOT(selectAllSlot()));

	selectNoneAct = new QAction(tr("Deselect All"), this);	
	connect(selectNoneAct, SIGNAL(triggered()), this, SLOT(selectNoneSlot()));

	justifyFront = new QAction(tr("Front"),this);
	connect(justifyFront, SIGNAL(triggered()), this, SLOT(transferFilterNameSlot()));
	justifyBack = new QAction(tr("Back"), this);
	connect(justifyBack, SIGNAL(triggered()), this, SLOT(transferFilterNameSlot()));
	justifyRight = new QAction(tr("Right"), this);
	connect(justifyRight, SIGNAL(triggered()), this, SLOT(transferFilterNameSlot()));
	justifyLeft = new QAction(tr("Left"), this);
	connect(justifyLeft, SIGNAL(triggered()), this, SLOT(transferFilterNameSlot()));
	justifyTop = new QAction(tr("Top"), this);
	connect(justifyTop, SIGNAL(triggered()), this, SLOT(transferFilterNameSlot()));
	justifyBottom = new QAction(tr("Bottom"), this);
	connect(justifyBottom, SIGNAL(triggered()), this, SLOT(transferFilterNameSlot()));

	flipByXYPlane = new QAction(tr("By X-Y plane"), this);
	connect(flipByXYPlane, SIGNAL(triggered()), this, SLOT(transferFilterNameSlot()));
	flipByYZPlane = new QAction(tr("By Y-Z plane"), this);
	connect(flipByYZPlane, SIGNAL(triggered()), this, SLOT(transferFilterNameSlot()));
	flipByXZPlane = new QAction(tr("By X-Z plane"), this);
	connect(flipByXZPlane, SIGNAL(triggered()), this, SLOT(transferFilterNameSlot()));

	pointCloudSwitchAct = new QAction(tr("Point Cloud View"), this);
	pointCloudSwitchAct->setCheckable(true);
	connect(pointCloudSwitchAct, SIGNAL(toggled(bool)), this, SLOT(updateMenus()));

	radarSwitchAct = new QAction(tr("X-Ray View"), this);
	radarSwitchAct->setCheckable(true);
	connect(radarSwitchAct, SIGNAL(toggled(bool)), this, SLOT(updateMenus()));

	chip_Gap_Mark_gird_Switch_Act = new QAction(tr("Grid/Chip_Gap_Mark"), this);
	chip_Gap_Mark_gird_Switch_Act->setCheckable(true);	
	chip_Gap_Mark_gird_Switch_Act->setChecked(true);
	connect(chip_Gap_Mark_gird_Switch_Act, &QAction::toggled, [this]() {
		if (chip_Gap_Mark_gird_Switch_Act->isChecked())
			GLA()->_grid_chip_gap_ = true;
		else
		{
			GLA()->_grid_chip_gap_ = false;
		}
	});


	landAct = new QAction(QIcon(":/myImage/images/btn_menu_land_2x.png"), tr("Land (Selected Models)"), this);
	landAct->setToolTip("Land");
	connect(landAct, SIGNAL(triggered()), this, SLOT(transferFilterNameSlot()));
	landAllAct = new QAction(tr("Land All"), this);
	connect(landAllAct, SIGNAL(triggered()), this, SLOT(transferFilterNameSlot()));

	genPrePrintingAct = new QAction(tr("Create Witness Part"),this);
	connect(genPrePrintingAct, SIGNAL(triggered()), this, SLOT(genPrePrintingSlot()));


	toCenterAct = new QAction(tr("To Center"), this);
	connect(toCenterAct, SIGNAL(triggered()), this, SLOT(transferFilterNameSlot()));

	autoPackingAct = new QAction(QIcon(":/myImage/images/btn_auto_packing@2x.png"), tr("Auto Placement"), this);
	autoPackingAct->setToolTip("Auto Placement");
	connect(autoPackingAct, SIGNAL(triggered()), this, SLOT(transferFilterNameSlot()));

	autoPackingAct2 = new QAction(tr("Auto Placement(Bounding Box)"), this);
	autoPackingAct2->setToolTip("Auto Placement");
	connect(autoPackingAct2, SIGNAL(triggered()), this, SLOT(transferFilterNameSlot()));

	overLapAct = new QAction(tr("Allow Overlapping"), this);
	overLapAct->setCheckable(true);
	overLapAct->setChecked(true);
	overLapAct->setText(tr("Allow Overlapping"));
	connect(overLapAct, SIGNAL(triggered()), this, SLOT(transferFilterNameSlot()));

	//logout//
	//SYDNY
	if (logInFUNC){
		nickname = new QAction(tr("username"), this);
		username = new QAction(tr("nickname"), this);
		logoutAct = new QAction(tr("Logout"), this);
		connect(logoutAct, SIGNAL(triggered()), this, SLOT(logout()));
		status = new QAction(tr("Not Login"), this);
	}
}

void MainWindow::createToolBars()//工具欄圖示
{
	QSize toolbarIconSize(48, 48);

#if defined(Q_OS_MAC)
	this->setStyleSheet("QToolBar {spacing: 0px; } QToolButton {border-radius: 0px;} QToolButton:checked {background: darkgray}");
#endif
	ImportRecentMeshToolButton = new QToolButton();
	ImportRecentMeshToolButton->setIcon(QIcon(":/images/icons/btn_recent_file_2x.png"));
	ImportRecentMeshToolButton->setText(tr("Recent Mesh"));
	ImportRecentMeshToolButton->setToolTip("Import Recent");
	ImportRecentMeshToolButton->setPopupMode(QToolButton::MenuButtonPopup);
	//ImportRecentMeshToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	for (int i = 0; i < MAXRECENTFILES; ++i)
	{
		ImportRecentMeshToolButton->addAction(recentFileActs[i]);
	}



	mainToolBar = addToolBar(tr("Standard"));//mainToolBar圖案的項目
											 //	mainToolBar->setIconSize(QSize(32,32));
	mainToolBar->setMovable(false);
	mainToolBar->setIconSize(toolbarIconSize);

	// Printing Toolbar
	mainToolBar->addAction(this->threeDPrintAct);
	mainToolBar->addAction(this->showPrintDashBoardAct);
	mainToolBar->addSeparator();

	// Project Toolbar
	mainToolBar->addAction(this->saveProjectAct);
	mainToolBar->addAction(this->openProjectAct);
	mainToolBar->addSeparator();

	// View Toolbar
	CustomToolButtonViewMenu = new QMenu;
	CustomToolButtonViewMenu->addAction(this->viewOriginalActTB);
	CustomToolButtonViewMenu->addAction(this->viewBottomActTB);
	CustomToolButtonViewMenu->addAction(this->viewSideActTB);
	CustomToolButtonViewMenu->addAction(this->viewFrontActTB);
	viewCustomToolButton = new CustomToolButton;
	viewCustomToolButton->setMenu(CustomToolButtonViewMenu);
	viewCustomToolButton->setDefaultAction(this->viewOriginalAct);
	viewDToolBar = addToolBar(tr("view_direction"));
	viewDToolBar->setIconSize(toolbarIconSize);
	viewDToolBar->setMovable(false);
	viewDToolBar->addWidget(viewCustomToolButton);
	viewDToolBar->addAction(lockViewAct);
	viewDToolBar->addAction(this->topTagAction);
	viewDToolBar->addSeparator();

	// Import/Export Toolbar
	ioToolBar = addToolBar("Import Toolbar");
	ioToolBar->setIconSize(toolbarIconSize);
	ioToolBar->setMovable(false);
	ioToolBar->addAction(importMeshAct);
	ioToolBar->addWidget(ImportRecentMeshToolButton);

	// Edit Toolbar
	editToolBar_pi = addToolBar("Edit");
	editToolBar_pi->setIconSize(toolbarIconSize);
	editToolBar_pi->setMovable(false);
	editToolBar_pi->addAction(copyAct);
	editToolBar_pi->addAction(pasteAct);
	editToolBar_pi->addAction(deleteAct);
	editToolBar_pi->addAction(this->unDoAct);
	editToolBar_pi->addAction(this->landAct);
	editToolBar_pi->addAction(this->autoPackingAct);
	editToolBar_pi->addAction(changeColorAC);
	editToolBar_pi->addSeparator();

	// Setting toolbar
	settingToolBar_pi = addToolBar("setting");
	settingToolBar_pi->setIconSize(toolbarIconSize);
	settingToolBar_pi->setMovable(false);
	settingToolBar_pi->addAction(showObjectListAct);
	settingToolBar_pi->addSeparator();
	settingToolBar_pi->addAction(printjobestimateAct);
	settingToolBar_pi->addAction(groove_settingAct);
	//settingToolBar_pi->addAction(selectionModeSwitchAct);
}

void MainWindow::createMenus_v2()
{
	//////////////////// Menu File ////////////////////////////////////////////////////////////////////////////
	fileMenu = new QMenu;// menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(newProjectAct);
	fileMenu->addAction(openProjectAct);
	fileMenu->addAction(appendProjectAct);
	fileMenu->addAction(saveProjectAct);
	fileMenu->addAction(closeProjectAct);
	fileMenu->addSeparator();

	fileMenu->addAction(importMeshAct);
	fileMenu->addAction(exportMeshAct);
	fileMenu->addAction(exportMeshAsAct);
	fileMenu->addAction(reloadMeshAct);
	fileMenu->addAction(reloadAllMeshAct);
	fileMenu->addSeparator();
	fileMenu->addAction(importRasterAct);
	fileMenu->addSeparator();

	fileMenu->addAction(saveSnapshotAct);
	separatorAct = fileMenu->addSeparator();
	recentProjMenu = fileMenu->addMenu(tr("Recent Projects"));
	recentFileMenu = fileMenu->addMenu(tr("Import Recent"));


	for (int i = 0; i < MAXRECENTFILES; ++i)
	{
		recentProjMenu->addAction(recentProjActs[i]);
		recentFileMenu->addAction(recentFileActs[i]);

	}
	//updateRecentFileActions();
	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);

	////////////////////// Menu Edit //////////////////////////////////////////////////////////////////////////
	editMenu = new QMenu;// menuBar()->addMenu(tr("&Edit"));
	editMenu->addAction(suspendEditModeAct);

	////////////////////// Menu Filter //////////////////////////////////////////////////////////////////////////
	filterMenu = new QMenu; //menuBar()->addMenu(tr("&Filters"));
	fillFilterMenu();


	////////////////////// Menu Render //////////////////////////////////////////////////////////////////////////
	renderMenu = new QMenu;//menuBar()->addMenu(tr("&Render"));

	renderModeMenu = renderMenu->addMenu(tr("Render &Mode"));
	renderModeMenu->addAction(backFaceCullAct);
	renderModeMenu->addActions(renderModeGroupAct->actions());
	renderModeMenu->addAction(renderModeTextureWedgeAct);
	renderModeMenu->addAction(setSelectFaceRenderingAct);
	renderModeMenu->addAction(setSelectVertRenderingAct);

	lightingModeMenu = renderMenu->addMenu(tr("&Lighting"));
	lightingModeMenu->addAction(setLightAct);
	lightingModeMenu->addAction(setDoubleLightingAct);
	lightingModeMenu->addAction(setFancyLightingAct);

	// Color SUBmenu
	colorModeMenu = renderMenu->addMenu(tr("&Color"));

	colorModeGroupAct = new QActionGroup(this);	colorModeGroupAct->setExclusive(true);

	QList<RenderModeAction*> rendlist;
	colorModeNoneAct = new RenderModeColorModeNoneAction(colorModeGroupAct);
	colorModeNoneAct->setCheckable(true);
	colorModeNoneAct->setChecked(true);
	rendlist.push_back(colorModeNoneAct);

	colorModePerMeshAct = new RenderModeColorModePerMeshAction(colorModeGroupAct);
	colorModePerMeshAct->setCheckable(true);
	rendlist.push_back(colorModePerMeshAct);

	colorModePerVertexAct = new RenderModeColorModePerVertexAction(colorModeGroupAct);
	colorModePerVertexAct->setCheckable(true);
	rendlist.push_back(colorModePerVertexAct);

	colorModePerFaceAct = new RenderModeColorModePerFaceAction(colorModeGroupAct);
	colorModePerFaceAct->setCheckable(true);
	rendlist.push_back(colorModePerFaceAct);

	connectRenderModeActionList(rendlist);//***把list中的action的signal與slot連結起來

	colorModeMenu->addAction(colorModeNoneAct);
	colorModeMenu->addAction(colorModePerMeshAct);
	colorModeMenu->addAction(colorModePerVertexAct);
	colorModeMenu->addAction(colorModePerFaceAct);

	// Shaders SUBmenu
	shadersMenu = renderMenu->addMenu(tr("&Shaders"));

	renderMenu->addSeparator();

	//////////////////// Menu View ////////////////////////////////////////////////////////////////////////////
	viewMenu = new QMenu; //menuBar()->addMenu(tr("&View"));
	viewMenu->addAction(fullScreenAct);
	viewMenu->addAction(showLayerDlgAct);
	viewMenu->addAction(showRasterAct);

	trackBallMenu = viewMenu->addMenu(tr("&Trackball"));
	trackBallMenu->addAction(showTrackBallAct);
	trackBallMenu->addAction(resetTrackBallAct);

	logMenu = viewMenu->addMenu(tr("&Info"));
	logMenu->addAction(showInfoPaneAct);

	toolBarMenu = viewMenu->addMenu(tr("&ToolBars"));
	toolBarMenu->addAction(showToolbarStandardAct);
	toolBarMenu->addAction(showToolbarRenderAct);
	connect(toolBarMenu, SIGNAL(aboutToShow()), this, SLOT(updateMenus()));

	//////////////////// Menu Windows /////////////////////////////////////////////////////////////////////////
	windowsMenu = new QMenu; //menuBar()->addMenu(tr("&Windows"));
	connect(windowsMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));
	//menuBar()->addSeparator();

	//////////////////// Menu Preferences /////////////////////////////////////////////////////////////////////
	preferencesMenu = new QMenu; //menuBar()->addMenu(tr("&Tools"));
								 //preferencesMenu->addAction(showFilterEditAct);
								 //preferencesMenu->addSeparator();
	preferencesMenu->addAction(setCustomizeAct);


	//////////////////// Menu Help ////////////////////////////////////////////////////////////////
	helpMenu = new QMenu; //menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(aboutAct);
	helpMenu->addAction(aboutPluginsAct);
	helpMenu->addAction(onlineHelpAct);
	helpMenu->addAction(onscreenHelpAct);
	helpMenu->addAction(submitBugAct);
	helpMenu->addAction(checkUpdatesAct);

	fillEditMenu();
	fillRenderMenu();
	fillDecorateMenu();

	////////////////////// Menu Split/Unsplit from handle
	handleMenu = new QMenu(this);
	splitMenu = handleMenu->addMenu(tr("&Split"));
	unSplitMenu = handleMenu->addMenu("&Close");

	////SearchMenu
	//if (searchButton != NULL)
	//{
	//	initSearchEngine();
	//	int longest = longestActionWidthInAllMenus();
	//	searchMenu = new SearchMenu(wama, 15, searchButton, longest);
	//	searchButton->setMenu(searchMenu);
	//	connect(searchShortCut, SIGNAL(activated()), searchButton, SLOT(openMenu()));
	//}


}
////////current use menu/////////////////////
void MainWindow::createMenus_v3()
{
#pragma region Menu_File
	//////////////////// Menu File ////////////////////////////////////////////////////////////////////////////
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(newProjectAct);
	fileMenu->addAction(openProjectAct);
	recentProjMenu = fileMenu->addMenu(tr("Open Recent"));
	fileMenu->addAction(importFromProjectAct);
	fileMenu->addAction(saveProjectAct);
	fileMenu->addAction(saveProjectAsAct);
	fileMenu->addAction(closeProjectAct);
	fileMenu->addSeparator();
	fileMenu->addAction(importMeshAct);
	recentFileMenu = fileMenu->addMenu(tr("Import Recent"));	
	fileMenu->addAction(exportMeshAsAct);
	fileMenu->addSeparator();
	fileMenu->addAction(testFuncReadZX);
	fileMenu->addAction(genZXAct);
	//fileMenu->addAction(testDMSlicerAct);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);	

	for (int i = 0; i < MAXRECENTFILES; ++i)
	{
		recentProjMenu->addAction(recentProjActs[i]);
		recentFileMenu->addAction(recentFileActs[i]);
	}
	fileMenu->addSeparator();
#pragma endregion Menu_File


	//////////////////// Menu Edit //////////////////////////////////////////////////////////////////////////
	//MeshFilterInterface * iFilter = PM.stringFilterMap.find("FP_CHANGE_COLOR").value();
	//QAction *changeColor = iFilter->AC("FP_CHANGE_COLOR");//


	editMenu = menuBar()->addMenu(tr("&Edit"));
	//editMenu->addAction(suspendEditModeAct);
	editMenu->addAction(this->unDoAct);
	editMenu->addAction(this->reDoAct);
	editMenu->addSeparator();
	//

	editMenu->addAction(this->copyAct);
	editMenu->addAction(this->pasteAct);
	editMenu->addAction(this->deleteAct);
	editMenu->addSeparator();

	tmt = new QMenu(tr("Object Color Mode"));
	tmt->addAction(changeRendertypeAct[0]);
	tmt->addAction(changeRendertypeAct[1]);
	tmt->addAction(changeRendertypeAct[2]);
	editMenu->addMenu(this->tmt);
	editMenu->addAction(changeColorAC);
	editMenu->addSeparator();

	editMenu->addAction(this->selectAllAct);
	editMenu->addAction(this->selectNoneAct);
	editMenu->addSeparator();

	flipMenu = new QMenu(tr("Flip"));
	flipMenu->addAction(flipByXYPlane);
	flipMenu->addAction(flipByYZPlane);
	flipMenu->addAction(flipByXZPlane);
	editMenu->addMenu(flipMenu);

	justify = new QMenu(tr("Justify"));
	justify->addAction(justifyFront);
	justify->addAction(justifyBack);
	justify->addAction(justifyLeft);
	justify->addAction(justifyRight);
	//justify->addAction(justifyTop);
	//justify->addAction(justifyBottom);
	editMenu->addMenu(justify);

	editMenu->addAction(this->autoPackingAct);
	editMenu->addAction(this->autoPackingAct2);
	

	editMenu->addAction(this->toCenterAct);

	landMenu = new QMenu(tr("Land Tool"));
	landMenu->addAction(this->overLapAct);
	landMenu->addAction(this->landAct);
	landMenu->addAction(this->landAllAct);	
	editMenu->addMenu(landMenu);

	editMenu->addAction(this->genPrePrintingAct);
	/*justify->addAction(PM.actionFilterMap.value("JUSTIFY_BACK"));
	justify->addAction(PM.actionFilterMap.value("JUSTIFY_LEFT"));
	justify->addAction(PM.actionFilterMap.value("JUSTIFY_RIGHT"));
	justify->addAction(PM.actionFilterMap.value("JUSTIFY_BOTTOM"));
	justify->addAction(PM.actionFilterMap.value("JUSTIFY_TOP"));*/
	//editMenu->addMenu(justify);
	//flipMenu = new QMenu("Flip");
	//flipMenu->addAction(flipByXYPlane);
	//flipMenu->addAction(flipByYZPlane);
	//flipMenu->addAction(flipByXZPlane);
	/*flipMenu->addAction(PM.actionFilterMap.value("FLIP_BY_X_Z_PLANE"));
	flipMenu->addAction(PM.actionFilterMap.value("FLIP_BY_Y_Z_PLANE"));
	flipMenu->addAction(PM.actionFilterMap.value("FLIP_BY_X_Y_PLANE"));*/
	//editMenu->addMenu(flipMenu);
	editMenu->addSeparator();
	editMenu->addAction(groove_settingAct);
	//editMenu->addAction();


#pragma region Menu_Filter
	//////////////////// Menu Filter //////////////////////////////////////////////////////////////////////////
	filterMenu = new QMenu;// menuBar()->addMenu(tr("&Filters"));
	fillFilterMenu();
	//filterMenu = menuBar()->addMenu(tr("Fi&lters"));
	//filterMenu->addAction(lastFilterAct);
	//filterMenu->addAction(showFilterScriptAct);
	//filterMenu->addAction(showScriptEditAct);
	//filterMenu->addAction(showFilterEditAct);
	//filterMenu->addSeparator();
#pragma endregion Menu_Filter
#pragma region Menu_Render
	//////////////////// Menu Render //////////////////////////////////////////////////////////////////////////
	renderMenu = new QMenu;// menuBar()->addMenu(tr("&Render"));

	renderModeMenu = renderMenu->addMenu(tr("Render &Mode"));
	renderModeMenu->addAction(backFaceCullAct);
	renderModeMenu->addActions(renderModeGroupAct->actions());
	renderModeMenu->addAction(renderModeTextureWedgeAct);
	renderModeMenu->addAction(setSelectFaceRenderingAct);
	renderModeMenu->addAction(setSelectVertRenderingAct);

	lightingModeMenu = renderMenu->addMenu(tr("&Lighting"));
	lightingModeMenu->addAction(setLightAct);
	lightingModeMenu->addAction(setDoubleLightingAct);
	lightingModeMenu->addAction(setFancyLightingAct);

	// Color SUBmenu
	colorModeMenu = renderMenu->addMenu(tr("&Color"));

	colorModeGroupAct = new QActionGroup(this);	colorModeGroupAct->setExclusive(true);

	QList<RenderModeAction*> rendlist;
	colorModeNoneAct = new RenderModeColorModeNoneAction(colorModeGroupAct);
	colorModeNoneAct->setCheckable(true);
	colorModeNoneAct->setChecked(true);
	rendlist.push_back(colorModeNoneAct);

	colorModePerMeshAct = new RenderModeColorModePerMeshAction(colorModeGroupAct);
	colorModePerMeshAct->setCheckable(true);
	rendlist.push_back(colorModePerMeshAct);

	colorModePerVertexAct = new RenderModeColorModePerVertexAction(colorModeGroupAct);
	colorModePerVertexAct->setCheckable(true);
	rendlist.push_back(colorModePerVertexAct);

	colorModePerFaceAct = new RenderModeColorModePerFaceAction(colorModeGroupAct);
	colorModePerFaceAct->setCheckable(true);
	rendlist.push_back(colorModePerFaceAct);

	connectRenderModeActionList(rendlist);//***把list中的action的signal與slot連結起來

	colorModeMenu->addAction(colorModeNoneAct);
	colorModeMenu->addAction(colorModePerMeshAct);
	colorModeMenu->addAction(colorModePerVertexAct);
	colorModeMenu->addAction(colorModePerFaceAct);

	// Shaders SUBmenu
	shadersMenu = renderMenu->addMenu(tr("&Shaders"));

	renderMenu->addSeparator();
#pragma endregion Menu_Render

#pragma region Menu_View
	//////////////////// Menu View ////////////////////////////////////////////////////////////////////////////
	viewMenu = menuBar()->addMenu(tr("&View"));
	/*viewMenu->addAction(fullScreenAct);
	viewMenu->addAction(showLayerDlgAct);
	viewMenu->addAction(showRasterAct);*/

	trackBallMenu = new QMenu;// viewMenu->addMenu(tr("&Trackball"));
	trackBallMenu->addAction(showTrackBallAct);
	trackBallMenu->addAction(resetTrackBallAct);

	logMenu = new QMenu;// viewMenu->addMenu(tr("&Info"));
	logMenu->addAction(showInfoPaneAct);

	toolBarMenu = new QMenu;// viewMenu->addMenu(tr("&ToolBars"));
	toolBarMenu->addAction(showToolbarStandardAct);
	toolBarMenu->addAction(showToolbarRenderAct);
	connect(toolBarMenu, SIGNAL(aboutToShow()), this, SLOT(updateMenus()));

	changeViewMenu = viewMenu->addMenu(tr("&Change View"));
	/*foreach(QAction *ac, viewFromGroupAct->actions())*/
	changeViewMenu->addAction(viewBottomAct);
	changeViewMenu->addAction(viewTopAct);
	changeViewMenu->addAction(viewLeftAct);
	changeViewMenu->addAction(viewRightAct);
	changeViewMenu->addAction(viewFrontAct);
	changeViewMenu->addAction(viewBackAct);
	changeViewMenu->addSeparator();
	changeViewMenu->addAction(viewOriginalAct);

	//viewMenu->addAction(resetViewAct);
	viewMenu->addAction(zoomInAct);
	viewMenu->addAction(zoomOutAct);
	viewMenu->addSeparator();
	viewMenu->addAction(topTagAction);
	viewMenu->addAction(lockViewAct);
	viewMenu->addAction(pointCloudSwitchAct);
	viewMenu->addAction(radarSwitchAct);
	viewMenu->addSeparator();
	viewMenu->addAction(showObjectListAct);
	viewMenu->addSeparator();
	viewMenu->addAction(chip_Gap_Mark_gird_Switch_Act);

	

	//Sydny 01-19-2017 added Function Menu Option on View Menu List
	//viewMenu->addSeparator();
	//viewMenu->addAction(showFunctionMenuAct);
	//connect(showFunctionMenuAct, SIGNAL(triggered()), this, SLOT(showFunctionMenu()));



#pragma endregion Menu_View

	printMenu = menuBar()->addMenu(tr("&Print"));
	printMenu->addAction(threeDPrintAct);
	//printMenu->addAction(preparePrintintAct);
	printMenu->addSeparator();
	printMenu->addAction(showPrintDashBoardAct);
	printMenu->addSeparator();
	printMenu->addAction(testPrintAct);
	printMenu->addAction(testPrintAct2);

	/*
	diable bor file
	*/
	//printMenu->addAction(testPrintBORAct);
	//printMenu->addAction(estimateUsageAct);
	printMenu->addAction(printjobestimateAct);
	//printMenu->addAction(checkUpdatesAct);


#pragma region Menu_Windows
	//////////////////// Menu Windows /////////////////////////////////////////////////////////////////////////
	windowsMenu = new QMenu;// menuBar()->addMenu(tr("&Windows"));
	connect(windowsMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));
	menuBar()->addSeparator();
#pragma endregion Menu_Windows
#pragma region Menu_Preferences
	//////////////////// Menu Preferences /////////////////////////////////////////////////////////////////////
	preferencesMenu = new QMenu;// menuBar()->addMenu(tr("&Tools"));
	preferencesMenu->addAction(showFilterEditAct);
	preferencesMenu->addSeparator();
	preferencesMenu->addAction(setCustomizeAct);
#pragma endregion Menu_Preferences
#pragma region Menu_Help
	//////////////////// Menu Help ////////////////////////////////////////////////////////////////
	helpMenu = menuBar()->addMenu(tr("&Help"));
	/*helpMenu->addAction(aboutAct);
	helpMenu->addAction(aboutPluginsAct);
	helpMenu->addAction(onlineHelpAct);
	helpMenu->addAction(onscreenHelpAct);
	helpMenu->addAction(submitBugAct);
	helpMenu->addAction(checkUpdatesAct);*/

	//helpMenu->addAction(groove_settingAct);
	helpMenu->addAction(howToUseThisSWAct);
	//helpMenu->addAction(supportResourceAct);
	helpMenu->addSeparator();
	helpMenu->addAction(picassoSupportSiteAct);
	//helpMenu->addAction(contactSupportAct);
	helpMenu->addAction(printHistoryAct);
	
	helpMenu->addSeparator();
	helpMenu->addAction(aboutPicassoAct);

	/*if (DSP_TEST_MODE)	
	{*/
	programmerTest = menuBar()->addMenu(tr("&testFunc"));
	programmerTest->addAction(testFuncAct);
	programmerTest->addAction(autoPackingAct2);
	programmerTest->addAction(testPrintBORAct);
	programmerTest->addAction(testDMSlicerAct);
	programmerTest->addAction(saveSnapshotAct);
	/*programmerTest->addAction(testFuncAct2);
	programmerTest->addAction(testLoad3mfAct3);
	programmerTest->addAction(testFunc_clearErrorAct);
	programmerTest->addAction(genZXAct);
	programmerTest->addAction(testFuncReadZX);*/
	programmerTest->menuAction()->setVisible(false);
	//}

	//sydny
	//////////////////// Logout ////////////////////////////////////////////////////////////////
	if (logInFUNC){
		QMenuBar *menuBar2 = new QMenuBar(0);
		menuBar2->setLayoutDirection(Qt::RightToLeft);
		status_Arrow = menuBar2->addMenu(tr(""));
		status_Arrow->setIcon(QIcon(":/images/icons/logout_off.png"));
		menuBar2->addAction(status);
		status_Icon = menuBar2->addAction(tr(""));
		status_Icon->setIcon(QIcon(":/images/icons/status_off.png"));
		menuBar()->setCornerWidget(menuBar2, Qt::TopRightCorner);
	}
	

	//status_Icon = menuBar()->addAction(tr(""));
	//status_Icon->setIcon(QIcon(":/images/icons/status_off.png"));
	//menuBar()->addAction(status);
	//status_Arrow = menuBar()->addMenu(tr(""));
	//status_Arrow->setIcon(QIcon(":/images/icons/logout_off.png"));


	fillEditMenu();
	fillRenderMenu();
	fillDecorateMenu();
#pragma endregion Menu_Help
	//////////////////// Menu Split/Unsplit from handle
	handleMenu = new QMenu(this);
	splitMenu = handleMenu->addMenu(tr("&Split"));
	unSplitMenu = handleMenu->addMenu("&Close");

	menuBar()->setStyleSheet(WidgetStyleSheet::menuStyleSheet());
}
void MainWindow::createMenus()
{
	//////////////////// Menu File ////////////////////////////////////////////////////////////////////////////
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(newProjectAct);
	fileMenu->addAction(openProjectAct);
	fileMenu->addAction(appendProjectAct);
	fileMenu->addAction(saveProjectAct);
	fileMenu->addAction(closeProjectAct);
	fileMenu->addSeparator();

	fileMenu->addAction(importMeshAct);
	//fileMenu->addAction(exportMeshAct);
	fileMenu->addAction(exportMeshAsAct);
	fileMenu->addAction(reloadMeshAct);
	fileMenu->addAction(reloadAllMeshAct);
	fileMenu->addSeparator();
	fileMenu->addAction(importRasterAct);
	fileMenu->addSeparator();

	fileMenu->addAction(saveSnapshotAct);
	separatorAct = fileMenu->addSeparator();
	recentProjMenu = fileMenu->addMenu(tr("Recent Projects"));
	recentFileMenu = fileMenu->addMenu(tr("Recent Files"));


	for (int i = 0; i < MAXRECENTFILES; ++i)
	{
		recentProjMenu->addAction(recentProjActs[i]);
		recentFileMenu->addAction(recentFileActs[i]);
	}
	//updateRecentFileActions();
	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);

	//////////////////// Menu Edit //////////////////////////////////////////////////////////////////////////
	editMenu = menuBar()->addMenu(tr("&Edit"));
	editMenu->addAction(suspendEditModeAct);

	//////////////////// Menu Filter //////////////////////////////////////////////////////////////////////////
	filterMenu = menuBar()->addMenu(tr("&Filters"));
	fillFilterMenu();
	//filterMenu = menuBar()->addMenu(tr("Fi&lters"));
	//filterMenu->addAction(lastFilterAct);
	//filterMenu->addAction(showFilterScriptAct);
	//filterMenu->addAction(showScriptEditAct);
	//filterMenu->addAction(showFilterEditAct);
	//filterMenu->addSeparator();


	//////////////////// Menu Render //////////////////////////////////////////////////////////////////////////
	renderMenu = menuBar()->addMenu(tr("&Render"));

	renderModeMenu = renderMenu->addMenu(tr("Render &Mode"));
	renderModeMenu->addAction(backFaceCullAct);
	renderModeMenu->addActions(renderModeGroupAct->actions());
	renderModeMenu->addAction(renderModeTextureWedgeAct);
	renderModeMenu->addAction(setSelectFaceRenderingAct);
	renderModeMenu->addAction(setSelectVertRenderingAct);

	lightingModeMenu = renderMenu->addMenu(tr("&Lighting"));
	lightingModeMenu->addAction(setLightAct);
	lightingModeMenu->addAction(setDoubleLightingAct);
	lightingModeMenu->addAction(setFancyLightingAct);

	// Color SUBmenu
	colorModeMenu = renderMenu->addMenu(tr("&Color"));

	colorModeGroupAct = new QActionGroup(this);	colorModeGroupAct->setExclusive(true);

	QList<RenderModeAction*> rendlist;
	colorModeNoneAct = new RenderModeColorModeNoneAction(colorModeGroupAct);
	colorModeNoneAct->setCheckable(true);
	colorModeNoneAct->setChecked(true);
	rendlist.push_back(colorModeNoneAct);

	colorModePerMeshAct = new RenderModeColorModePerMeshAction(colorModeGroupAct);
	colorModePerMeshAct->setCheckable(true);
	rendlist.push_back(colorModePerMeshAct);

	colorModePerVertexAct = new RenderModeColorModePerVertexAction(colorModeGroupAct);
	colorModePerVertexAct->setCheckable(true);
	rendlist.push_back(colorModePerVertexAct);

	colorModePerFaceAct = new RenderModeColorModePerFaceAction(colorModeGroupAct);
	colorModePerFaceAct->setCheckable(true);
	rendlist.push_back(colorModePerFaceAct);

	connectRenderModeActionList(rendlist);//***把list中的action的signal與slot連結起來

	colorModeMenu->addAction(colorModeNoneAct);
	colorModeMenu->addAction(colorModePerMeshAct);
	colorModeMenu->addAction(colorModePerVertexAct);
	colorModeMenu->addAction(colorModePerFaceAct);

	// Shaders SUBmenu
	shadersMenu = renderMenu->addMenu(tr("&Shaders"));

	renderMenu->addSeparator();

	//////////////////// Menu View ////////////////////////////////////////////////////////////////////////////
	viewMenu = menuBar()->addMenu(tr("&View"));
	viewMenu->addAction(fullScreenAct);
	viewMenu->addAction(showLayerDlgAct);
	viewMenu->addAction(showRasterAct);

	trackBallMenu = viewMenu->addMenu(tr("&Trackball"));
	trackBallMenu->addAction(showTrackBallAct);
	trackBallMenu->addAction(resetTrackBallAct);

	logMenu = viewMenu->addMenu(tr("&Info"));
	logMenu->addAction(showInfoPaneAct);

	toolBarMenu = viewMenu->addMenu(tr("&ToolBars"));
	toolBarMenu->addAction(showToolbarStandardAct);
	toolBarMenu->addAction(showToolbarRenderAct);
	connect(toolBarMenu, SIGNAL(aboutToShow()), this, SLOT(updateMenus()));

	//////////////////// Menu Windows /////////////////////////////////////////////////////////////////////////
	windowsMenu = menuBar()->addMenu(tr("&Windows"));
	connect(windowsMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));
	menuBar()->addSeparator();

	//////////////////// Menu Preferences /////////////////////////////////////////////////////////////////////
	preferencesMenu = menuBar()->addMenu(tr("&Tools"));
	//preferencesMenu->addAction(showFilterEditAct);
	//preferencesMenu->addSeparator();
	preferencesMenu->addAction(setCustomizeAct);


	//////////////////// Menu Help ////////////////////////////////////////////////////////////////
	helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(aboutAct);
	helpMenu->addAction(aboutPluginsAct);
	helpMenu->addAction(onlineHelpAct);
	helpMenu->addAction(onscreenHelpAct);
	helpMenu->addAction(submitBugAct);
	helpMenu->addAction(checkUpdatesAct);

	fillEditMenu();
	fillRenderMenu();
	fillDecorateMenu();

	//////////////////// Menu Split/Unsplit from handle
	handleMenu = new QMenu(this);
	splitMenu = handleMenu->addMenu(tr("&Split"));
	unSplitMenu = handleMenu->addMenu("&Close");

	//SearchMenu
	if (searchButton != NULL)
	{
		initSearchEngine();
		int longest = longestActionWidthInAllMenus();
		searchMenu = new SearchMenu(wama, 15, searchButton, longest);
		searchButton->setMenu(searchMenu);
		connect(searchShortCut, SIGNAL(activated()), searchButton, SLOT(openMenu()));
	}
}

void MainWindow::initSearchEngine()
{
	for (QMap<QString, QAction*>::iterator it = PM.actionFilterMap.begin(); it != PM.actionFilterMap.end(); ++it)
		initItemForSearching(it.value());

	for (QMap<QString, MeshLabXMLFilterContainer>::iterator it = PM.stringXMLFilterMap.begin(); it != PM.stringXMLFilterMap.end(); ++it)
		initItemForSearching(it.value().act);

	initMenuForSearching(editMenu);
	initMenuForSearching(renderMenu);
}

void MainWindow::initMenuForSearching(QMenu* menu)
{
	if (menu == NULL)
		return;
	const QList<QAction*>& acts = menu->actions();
	foreach(QAction* act, acts)
	{
		QMenu* submenu = act->menu();
		if (!act->isSeparator() && (submenu == NULL))
			initItemForSearching(act);
		else if (!act->isSeparator())
			initMenuForSearching(submenu);
	}
}

void MainWindow::initItemForSearching(QAction* act)
{
	QString tx = act->text() + " " + act->toolTip();
	wama.addWordsPerAction(*act, tx);
}

QString MainWindow::getDecoratedFileName(const QString& name)
{
	return  QString("<br><b><i>(") + name + ")</i></b>";
}

void MainWindow::fillFilterMenu()//***初始化filter qaction, 做connect
{
	filterMenu->clear();
	filterMenu->addAction(lastFilterAct);
	filterMenu->addAction(showFilterScriptAct);
	filterMenu->addSeparator();
	//filterMenu->addMenu(new SearcherMenu(this,filterMenu));
	//filterMenu->addSeparator();
	// Connects the events of the actions within colorize to the method which shows their tooltip

	filterMenuSelect = new MenuWithToolTip(tr("Selection"), this);
	filterMenu->addMenu(filterMenuSelect);
	filterMenuClean = new MenuWithToolTip(tr("Cleaning and Repairing"), this);
	filterMenu->addMenu(filterMenuClean);
	filterMenuCreate = new MenuWithToolTip(tr("Create New Mesh Layer"), this);
	filterMenu->addMenu(filterMenuCreate);
	filterMenuRemeshing = new MenuWithToolTip(tr("Remeshing, Simplification and Reconstruction"), this);
	filterMenu->addMenu(filterMenuRemeshing);
	filterMenuPolygonal = new MenuWithToolTip(tr("Polygonal and Quad Mesh"), this);
	filterMenu->addMenu(filterMenuPolygonal);
	filterMenuColorize = new MenuWithToolTip(tr("Color Creation and Processing"), this);
	filterMenu->addMenu(filterMenuColorize);
	filterMenuSmoothing = new MenuWithToolTip(tr("Smoothing, Fairing and Deformation"), this);
	filterMenu->addMenu(filterMenuSmoothing);
	filterMenuQuality = new MenuWithToolTip(tr("Quality Measure and Computations"), this);
	filterMenu->addMenu(filterMenuQuality);
	filterMenuNormal = new MenuWithToolTip(tr("Normals, Curvatures and Orientation"), this);
	filterMenu->addMenu(filterMenuNormal);
	filterMenuMeshLayer = new MenuWithToolTip(tr("Mesh Layer"), this);
	filterMenu->addMenu(filterMenuMeshLayer);
	filterMenuRasterLayer = new MenuWithToolTip(tr("Raster Layer"), this);
	filterMenu->addMenu(filterMenuRasterLayer);
	filterMenuRangeMap = new MenuWithToolTip(tr("Range Map"), this);
	filterMenu->addMenu(filterMenuRangeMap);
	filterMenuPointSet = new MenuWithToolTip(tr("Point Set"), this);
	filterMenu->addMenu(filterMenuPointSet);
	filterMenuSampling = new MenuWithToolTip(tr("Sampling"), this);
	filterMenu->addMenu(filterMenuSampling);
	filterMenuTexture = new MenuWithToolTip(tr("Texture"), this);
	filterMenu->addMenu(filterMenuTexture);
	filterMenuCamera = new MenuWithToolTip(tr("Camera"), this);
	filterMenu->addMenu(filterMenuCamera);

	//#if !defined(Q_OS_MAC)
	//	connect(filterMenuSelect, SIGNAL(hovered(QAction*)), this, SLOT(showTooltip(QAction*)) );
	//	connect(filterMenuClean, SIGNAL(hovered(QAction*)), this, SLOT(showTooltip(QAction*)) );
	//	connect(filterMenuCreate, SIGNAL(hovered(QAction*)), this, SLOT(showTooltip(QAction*)) );
	//	connect(filterMenuRemeshing, SIGNAL(hovered(QAction*)), this, SLOT(showTooltip(QAction*)) );
	//	connect(filterMenuPolygonal, SIGNAL(hovered(QAction*)), this, SLOT(showTooltip(QAction*)) );
	//	connect(filterMenuColorize, SIGNAL(hovered(QAction*)), this, SLOT(showTooltip(QAction*)) );
	//	connect(filterMenuQuality, SIGNAL(hovered(QAction*)), this, SLOT(showTooltip(QAction*)) );
	//	connect(filterMenuNormal, SIGNAL(hovered(QAction*)), this, SLOT(showTooltip(QAction*)) );
	//	connect(filterMenuMeshLayer,   SIGNAL(hovered(QAction*)), this, SLOT(showTooltip(QAction*)) );
	//	connect(filterMenuRasterLayer, SIGNAL(hovered(QAction*)), this, SLOT(showTooltip(QAction*)) );
	//	connect(filterMenuRangeMap, SIGNAL(hovered(QAction*)), this, SLOT(showTooltip(QAction*)) );
	//	connect(filterMenuPointSet, SIGNAL(hovered(QAction*)), this, SLOT(showTooltip(QAction*)) );
	//	connect(filterMenuSampling, SIGNAL(hovered(QAction*)), this, SLOT(showTooltip(QAction*)) );
	//	connect(filterMenuTexture, SIGNAL(hovered(QAction*)), this, SLOT(showTooltip(QAction*)) );
	//	connect(filterMenuCamera, SIGNAL(hovered(QAction*)), this, SLOT(showTooltip(QAction*)) );
	//#endif
	rightmouse_contextMenu = new QMenu("contextmenu", this);
	rightmouse_contextMenu->addAction(copyAct_right);
	rightmouse_contextMenu->addAction(pasteAct_right);
	rightmouse_contextMenu->addAction(pasteManyAct_right);
	rightmouse_contextMenu->addSeparator();
	rightmouse_contextMenu->addAction(landAct_right);
	rightmouse_contextMenu->addAction(changeColorAC);
	rightmouse_contextMenu->addAction(exportMeshAsAct2);
	rightmouse_contextMenu->addAction(reloadMeshAct);
	rightmouse_contextMenu->addAction(deleteAct_right);
	rightmouse_contextMenu->addAction(detectOverlapping);
	rightmouse_contextMenu->addAction(selectAllAct);
	rightmouse_contextMenu->addAction(selectNoneAct);

	rightmouse_contextMenu->addSeparator();
	//tmt = new QMenu(tr("Object Color Mode"));
	tmt->addAction(changeRendertypeAct[0]);
	tmt->addAction(changeRendertypeAct[1]);
	tmt->addAction(changeRendertypeAct[2]);
	rightmouse_contextMenu->addMenu(tmt);

	rightmouse_contextMenu->addSeparator();
	rightmouse_contextMenu->addAction(newProjectAct_right);

	rightmouse_contextMenu->addSeparator();
	//rightmouse_contextMenu->addAction(selectionModeSwitchAct);

	//Sydny 01-19-2017 added show function menu option when user press right click
	//rightmouse_contextMenu->addSeparator();
	//rightmouse_contextMenu->addAction(showFunctionMenuAct);



	QMap<QString, MeshFilterInterface *>::iterator msi;
	for (msi = PM.stringFilterMap.begin(); msi != PM.stringFilterMap.end(); ++msi)//QMap<QString, MeshFilterInterface*> stringFilterMap;
	{
		MeshFilterInterface * iFilter = msi.value();
		QAction *filterAction = iFilter->AC((msi.key()));//由filter名字取出action
		QString tooltip = iFilter->filterInfo(filterAction) + "<br>" + getDecoratedFileName(filterAction->data().toString());
		//filterAction->setToolTip(tooltip);
		//connect(filterAction, SIGNAL(hovered()), this, SLOT(showActionMenuTooltip()) );

		//***20150428在這邊做初始化
		connect(filterAction, SIGNAL(triggered()), this, SLOT(startFilter()));
		//***20160419
		/*if (!filterAction->icon().isNull())
		rightmouse_contextMenu->addAction(filterAction);*/

		int filterClass = iFilter->getClass(filterAction);//宣告在meshFilterInterface的filterClass enum，iFilter實作時用其資訊做分類，抓出來比對，加到對應的filter_menu
		if (filterClass & MeshFilterInterface::FaceColoring)
		{
			filterMenuColorize->addAction(filterAction);
		}
		if (filterClass & MeshFilterInterface::VertexColoring)
		{
			filterMenuColorize->addAction(filterAction);
		}
		if (filterClass & MeshFilterInterface::Selection)
		{
			filterMenuSelect->addAction(filterAction);
		}
		if (filterClass & MeshFilterInterface::Cleaning)
		{
			filterMenuClean->addAction(filterAction);
		}
		if (filterClass & MeshFilterInterface::Remeshing)
		{
			filterMenuRemeshing->addAction(filterAction);
		}
		if (filterClass & MeshFilterInterface::Smoothing)
		{
			filterMenuSmoothing->addAction(filterAction);
		}
		if (filterClass & MeshFilterInterface::Normal)
		{
			filterMenuNormal->addAction(filterAction);
		}
		if (filterClass & MeshFilterInterface::Quality)
		{
			filterMenuQuality->addAction(filterAction);
		}
		if (filterClass & MeshFilterInterface::Measure)
		{
			filterMenuQuality->addAction(filterAction);
		}
		if (filterClass & MeshFilterInterface::Layer)
		{
			filterMenuMeshLayer->addAction(filterAction);
		}
		if (filterClass & MeshFilterInterface::RasterLayer)
		{
			filterMenuRasterLayer->addAction(filterAction);
		}
		if (filterClass & MeshFilterInterface::MeshCreation)
		{
			filterMenuCreate->addAction(filterAction);
		}
		if (filterClass & MeshFilterInterface::RangeMap)
		{
			filterMenuRangeMap->addAction(filterAction);
		}
		if (filterClass & MeshFilterInterface::PointSet)
		{
			filterMenuPointSet->addAction(filterAction);
		}
		if (filterClass & MeshFilterInterface::Sampling)
		{
			filterMenuSampling->addAction(filterAction);
		}
		if (filterClass & MeshFilterInterface::Texture)
		{
			filterMenuTexture->addAction(filterAction);
		}
		if (filterClass & MeshFilterInterface::Polygonal)
		{
			filterMenuPolygonal->addAction(filterAction);
		}
		if (filterClass & MeshFilterInterface::Camera)
		{
			filterMenuCamera->addAction(filterAction);
		}
		//***20150323新增
		if (filterClass & MeshFilterInterface::Customized)
		{
			filterMenu->addAction(filterAction);
			filterAction->setEnabled(false);
		}
		//***20150429
		if (filterClass & MeshFilterInterface::Generic)
		{
			filterMenu->addAction(filterAction);
			filterAction->setEnabled(false);
		}
		//******
		//  MeshFilterInterface::Generic :
		if (filterClass == 0)
		{
			filterMenu->addAction(filterAction);
			filterAction->setEnabled(false);
		}
		//if(!filterAction->icon().isNull())
		//    filterToolBar->addAction(filterAction);


	}

	QMap<QString, MeshLabXMLFilterContainer>::iterator xmlit;
	for (xmlit = PM.stringXMLFilterMap.begin(); xmlit != PM.stringXMLFilterMap.end(); ++xmlit)
	{
		//MeshLabFilterInterface * iFilter= xmlit.value().filterInterface;
		QAction *filterAction = xmlit.value().act;
		MLXMLPluginInfo* info = xmlit.value().xmlInfo;
		QString filterName = xmlit.key();
		try
		{
			QString help = info->filterHelp(filterName);
			filterAction->setToolTip(help + getDecoratedFileName(filterAction->data().toString()));
			connect(filterAction, SIGNAL(triggered()), this, SLOT(startFilter()));
			QString filterClasses = info->filterAttribute(filterName, MLXMLElNames::filterClass);
			QStringList filterClassesList = filterClasses.split(QRegExp("\\W+"), QString::SkipEmptyParts);
			foreach(QString nameClass, filterClassesList)
			{
				if (nameClass == QString("FaceColoring"))
				{
					filterMenuColorize->addAction(filterAction);
				}
				if (nameClass == QString("VertexColoring"))
				{
					filterMenuColorize->addAction(filterAction);
				}
				if (nameClass == QString("Selection"))
				{
					filterMenuSelect->addAction(filterAction);
				}
				if (nameClass == QString("Cleaning"))
				{
					filterMenuClean->addAction(filterAction);
				}
				if (nameClass == QString("Remeshing"))
				{
					filterMenuRemeshing->addAction(filterAction);
				}
				if (nameClass == QString("Smoothing"))
				{
					filterMenuSmoothing->addAction(filterAction);
				}
				if (nameClass == QString("Normal"))
				{
					filterMenuNormal->addAction(filterAction);
				}
				if (nameClass == QString("Quality"))
				{
					filterMenuQuality->addAction(filterAction);
				}
				if (nameClass == QString("Measure"))
				{
					filterMenuQuality->addAction(filterAction);
				}
				if (nameClass == QString("Layer"))
				{
					filterMenuMeshLayer->addAction(filterAction);
				}
				if (nameClass == QString("RasterLayer"))
				{
					filterMenuRasterLayer->addAction(filterAction);
				}
				if (nameClass == QString("MeshCreation"))
				{
					filterMenuCreate->addAction(filterAction);
				}
				if (nameClass == QString("RangeMap"))
				{
					filterMenuRangeMap->addAction(filterAction);
				}
				if (nameClass == QString("PointSet"))
				{
					filterMenuPointSet->addAction(filterAction);
				}
				if (nameClass == QString("Sampling"))
				{
					filterMenuSampling->addAction(filterAction);
				}
				if (nameClass == QString("Texture"))
				{
					filterMenuTexture->addAction(filterAction);
				}
				if (nameClass == QString("Polygonal"))
				{
					filterMenuPolygonal->addAction(filterAction);
				}
				if (nameClass == QString("Camera"))
				{
					filterMenuCamera->addAction(filterAction);
				}
				//  //  MeshFilterInterface::Generic :
				if (nameClass == QString("Generic"))
				{
					filterMenu->addAction(filterAction);
				}
				//if(!filterAction->icon().isNull())
				//    filterToolBar->addAction(filterAction);
			}
		}
		catch (ParsingException e)
		{
			meshDoc()->Log.Logf(GLLogStream::SYSTEM, e.what());
		}
	}
}

void MainWindow::fillDecorateMenu()
{
	foreach(MeshDecorateInterface *iDecorate, PM.meshDecoratePlugins())
	{
		foreach(QAction *decorateAction, iDecorate->actions())
		{
			connect(decorateAction, SIGNAL(triggered()), this, SLOT(applyDecorateMode()));
			decorateAction->setToolTip(iDecorate->decorationInfo(decorateAction));
			renderMenu->addAction(decorateAction);
		}
	}
	//***20150618***
	testRenderButton = new QToolButton();
	testRenderButton->setPopupMode(QToolButton::MenuButtonPopup);
	testRenderButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	foreach(MeshDecorateInterface *iDecorate, PM.meshDecoratePlugins())
	{
		foreach(QAction *decorateAction, iDecorate->actions())
		{
			connect(decorateAction, SIGNAL(triggered()), this, SLOT(applyDecorateMode()));
			decorateAction->setToolTip(iDecorate->decorationInfo(decorateAction));
			testRenderButton->addAction(decorateAction);
		}
	}

	testRenderButton->setText("testRenderButton");
	//************
	connect(renderMenu, SIGNAL(hovered(QAction*)), this, SLOT(showTooltip(QAction*)));
}

void MainWindow::fillRenderMenu()
{
	QAction * qaNone = new QAction("None", this);
	qaNone->setCheckable(false);
	shadersMenu->addAction(qaNone);
	connect(qaNone, SIGNAL(triggered()), this, SLOT(applyRenderMode()));
	foreach(MeshRenderInterface *iRender, PM.meshRenderPlugins())
	{
		addToMenu(iRender->actions(), shadersMenu, SLOT(applyRenderMode()));
	}
}

void MainWindow::fillEditMenu()//edit menu 與edit專案 功能連結起來
{
	foreach(MeshEditInterfaceFactory *iEditFactory, PM.meshEditFactoryPlugins())
	{
		foreach(QAction* editAction, iEditFactory->actions())
		{
			editMenu->addAction(editAction);
			connect(editAction, SIGNAL(triggered()), this, SLOT(applyEditMode()));
		}
	}
}


void MainWindow::loadMeshLabSettings()
{
	// I have already loaded the plugins so the default parameters for the settings
	// of the plugins are already in the <defaultGlobalParams> .
	// we just miss the globals default of meshlab itself
	MainWindowSetting::initGlobalParameterSet(&defaultGlobalParams);
	GLArea::initGlobalParameterSet(&defaultGlobalParams);//static function, 初始化參數

	QSettings settings;
	QStringList klist = settings.allKeys();
	foreach(QString qs, klist)
	{
		qDebug() << "mainwindowInit_qs" << qs;
	}

	// 1) load saved values into the <currentGlobalParams>
	for (int ii = 0; ii < klist.size(); ++ii)
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
			if (!defaultGlobalParams.hasParameter(rpar->name))
			{
				//  qDebug("Warning Ignored parameter %s. In the saved parameters there are ones that are not in the HardWired ones. "
				//         "It happens if you are running MeshLab with only a subset of the plugins. ",qPrintable(rpar->name));
			}
			else currentGlobalParams.addParam(rpar);//***把參數加入currentGlobalParams
		}
		/*QString xml = doc.toString();
		qDebug() << "xmlxml" << xml;*/
	}

	// 2) eventually fill missing values with the hardwired defaults
	for (int ii = 0; ii < defaultGlobalParams.paramList.size(); ++ii)
	{
		//		qDebug("Searching param[%i] %s of the default into the loaded settings. ",ii,qPrintable(defaultGlobalParams.paramList.at(ii)->name));
		qDebug() << "paramList.size()" << endl;
		if (!currentGlobalParams.hasParameter(defaultGlobalParams.paramList.at(ii)->name))
		{
			qDebug("Warning! a default param was not found in the saved settings. This should happen only on the first run...");
			RichParameterCopyConstructor v;
			defaultGlobalParams.paramList.at(ii)->accept(v);
			currentGlobalParams.paramList.push_back(v.lastCreated);

			QDomDocument doc("MeshLabSettings");
			RichParameterXMLVisitor vxml(doc);
			v.lastCreated->accept(vxml);//create element attribute in here
			doc.appendChild(vxml.parElem);
			QString docstring = doc.toString();
			QSettings setting;
			setting.setValue(v.lastCreated->name, QVariant(docstring));
		}
	}

	//emit dispatchCustomSettings(currentGlobalParams);
}
void MainWindow::loadFromFileToPicassoSettings()
{
	RichParameterSet parameter;
	

}
void MainWindow::loadPicassoSettings()
{
	MainWindowSetting::initGlobalParameterSet(&defaultGlobalParams);
	GLArea::initGlobalParameterSet(&defaultGlobalParams);//static function, 初始化參數
	Setting3DP::initSetting(&defaultGlobalParams);

	QSettings settings;
	QStringList klist = settings.allKeys();
	foreach(QString qs, klist)
	{
		qDebug() << "mainwindowInit_qs" << qs;
	}

	// 1) load saved values into the <currentGlobalParams>
	for (int ii = 0; ii < klist.size(); ++ii)
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
			if (!defaultGlobalParams.hasParameter(rpar->name))
			{
				//  qDebug("Warning Ignored parameter %s. In the saved parameters there are ones that are not in the HardWired ones. "
				//         "It happens if you are running MeshLab with only a subset of the plugins. ",qPrintable(rpar->name));
			}
			else currentGlobalParams.addParam(rpar);//***把參數加入currentGlobalParams
		}
		/*QString xml = doc.toString();
		qDebug() << "xmlxml" << xml;*/
	}

	// 2) eventually fill missing values with the hardwired defaults
	for (int ii = 0; ii < defaultGlobalParams.paramList.size(); ++ii)
	{
		//		qDebug("Searching param[%i] %s of the default into the loaded settings. ",ii,qPrintable(defaultGlobalParams.paramList.at(ii)->name));
		qDebug() << "paramList.size()" << endl;
		if (!currentGlobalParams.hasParameter(defaultGlobalParams.paramList.at(ii)->name))
		{
			qDebug("Warning! a default param was not found in the saved settings. This should happen only on the first run...");
			RichParameterCopyConstructor v;
			defaultGlobalParams.paramList.at(ii)->accept(v);
			currentGlobalParams.paramList.push_back(v.lastCreated);

			QDomDocument doc("PaletteSetting");
			RichParameterXMLVisitor vxml(doc);
			v.lastCreated->accept(vxml);//create element attribute in here
			doc.appendChild(vxml.parElem);
			QString docstring = doc.toString();
			QSettings setting;
			setting.setValue(v.lastCreated->name, QVariant(docstring));
		}
	}
	//emit dispatchCustomSettings(currentGlobalParams);
}
void MainWindow::addToMenu(QList<QAction *> actionList, QMenu *menu, const char *slot)
{
	foreach(QAction *a, actionList)
	{
		connect(a, SIGNAL(triggered()), this, slot);
		menu->addAction(a);
	}
}

// this function update the app settings with the current recent file list
// and update the loaded mesh counter
void MainWindow::saveRecentFileList(const QString &fileName)
{
	QSettings settings;
	QStringList files = settings.value("recentFileList").toStringList();//***如果指定key的value不存在，傳回default value
	files.removeAll(fileName);
	files.prepend(fileName);
	while (files.size() > MAXRECENTFILES)//***recent file設定為4個，增加為10個
		files.removeLast();

	//avoid the slash/back-slash path ambiguity
	for (int ii = 0; ii < files.size(); ++ii)
		files[ii] = QDir::fromNativeSeparators(files[ii]);
	settings.setValue("recentFileList", files);

	foreach(QWidget *widget, QApplication::topLevelWidgets()) {
		MainWindow *mainWin = qobject_cast<MainWindow *>(widget);
		if (mainWin) mainWin->updateRecentFileActions();
	}

	settings.setValue("totalKV", settings.value("totalKV", 0).toInt() + (GLA()->mm()->cm.vn) / 1000);
	settings.setValue("loadedMeshCounter", settings.value("loadedMeshCounter", 0).toInt() + 1);

	int loadedMeshCounter = settings.value("loadedMeshCounter", 20).toInt();
	int connectionInterval = settings.value("connectionInterval", 20).toInt();
	int lastComunicatedValue = settings.value("lastComunicatedValue", 0).toInt();

	//***恭喜畫面註解掉
	/* if(loadedMeshCounter-lastComunicatedValue>connectionInterval && !myLocalBuf.isOpen())
	{
	#if !defined(__DISABLE_AUTO_STATS__)
	checkForUpdates(false);
	#endif
	int congratsMeshCounter = settings.value("congratsMeshCounter",50).toInt();
	//***恭喜畫面
	if(loadedMeshCounter > congratsMeshCounter * 2 )
	{
	// This preference values store when you did the last request for a mail
	settings.setValue("congratsMeshCounter",loadedMeshCounter);

	QDialog *congratsDialog = new QDialog();
	Ui::CongratsDialog temp;
	temp.setupUi(congratsDialog);
	temp.buttonBox->addButton("Send Mail", QDialogButtonBox::AcceptRole);
	congratsDialog->exec();
	if(congratsDialog->result()==QDialog::Accepted)
	QDesktopServices::openUrl(QUrl("mailto:p.cignoni@isti.cnr.it;g.ranzuglia@isti.cnr.it?subject=[MeshLab] Reporting Info on MeshLab Usage"));
	}
	}*/
}

void MainWindow::saveRecentProjectList(const QString &projName)
{
	QSettings settings;
	QStringList files = settings.value("recentProjList").toStringList();
	files.removeAll(projName);
	files.prepend(projName);
	while (files.size() > MAXRECENTFILES)
		files.removeLast();

	for (int ii = 0; ii < files.size(); ++ii)
		files[ii] = QDir::fromNativeSeparators(files[ii]);

	settings.setValue("recentProjList", files);

	foreach(QWidget *widget, QApplication::topLevelWidgets())
	{
		MainWindow *mainWin = qobject_cast<MainWindow *>(widget);
		if (mainWin)
			mainWin->updateRecentProjActions();
	}
}

void MainWindow::checkForUpdates_2()
{

	//QNetworkRequest request(QUrl("http://www.xyzprinting.com/api2/CheckForStageUpdates?app=com.xyzprinting.FoodPrinter&os=w,6.1&model=XYZFoodPrinter10.engine,XYZFoodPrinter10.panel"));
	/*QNetworkReply *reply =*/

	startSendURL(QUrl("http://www.xyzprinting.com/api2/CheckForStageUpdates?app=com.xyzprinting.picasso&os=w,6.1&model="));


}
void MainWindow::startSendURL(QUrl url)
{
	QNetworkRequest request(url);
	QNetworkReply *reply = httpReq->get(request);
	currentDownloads.append(reply);
	connect(currentDownloads.back(), SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(downloadStatus(qint64, qint64)));
}

void MainWindow::downloadStatus(qint64 bytesReceived, qint64 bytesTotal)
{
	qDebug() << "download_process " << (bytesReceived * 100 / bytesTotal) << bytesReceived << bytesTotal;
	QCallBack((bytesReceived * 100 / bytesTotal), "download_process");
}

void MainWindow::connectionDone2(QNetworkReply *reply)
{
	//QString temp = reply->readAll();
	if (reply->peek(26) == "{\"com.xyzprinting.Picasso\"")
	{
		QString json = reply->readAll();
		updateOrNot(json);
	}
	else
	{
		QUrl url = reply->url();
		if (url.isValid())
		{
			QString filename = saveFileName(url);
			if (saveToDisk(filename, reply))
			{
				printf("Download of %s succeeded (saved to %s)\n", url.toEncoded().constData(), qPrintable(filename));
				QDesktopServices::openUrl(QUrl("file:///" + filename, QUrl::TolerantMode));
			}
		}
		else
		{
			QMessageBox::information(this, "check update", "please check Firewall");
		}

	}
	qb->reset();
	currentDownloads.removeAll(reply);
	reply->deleteLater();

}
bool MainWindow::updateOrNot(QString json)
{
	QJsonDocument jsonDoc = QJsonDocument::fromJson(json.toUtf8());
	QJsonObject jsonObj = jsonDoc.object();
	QJsonObject jsonObj2 = jsonObj.value("com.xyzprinting.Picasso").toObject();

	QString newVersion = jsonObj2["latest_version"].toString();
	QString downloadURL = jsonObj2["download_url"].toString();
	qDebug() << newVersion;

	QString currentVersion = PicaApplication::appVer();
	//if (VersionCheck(currentVersion.toStdString()) < VersionCheck(newVersion.toStdString()))
	if (VersionCheck("0.0.0.0") < VersionCheck(newVersion.toStdString()))
	{
		qDebug() << "needUpdate";
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(this, "Test", "New Version detected,\n Start to download", QMessageBox::Yes | QMessageBox::No);
		if (reply == QMessageBox::Yes)
		{
			qDebug() << "Yes was clicked";
			//QNetworkReply *reply = httpReq->get(QNetworkRequest(QUrl("https://xyzprod.blob.core.windows.net/firmware-test/Setup_Foodware_0.1.1.2.exe")));
			//currentDownloads.append(reply);
			//connect(currentDownloads.back(), SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(downloadStatus(qint64, qint64)));
			//startSendURL(QUrl("https://xyzprod.blob.core.windows.net/firmware-test/Setup_Foodware_0.1.1.2.exe"));
			startSendURL(QUrl(downloadURL));
		}
		else
		{
			qDebug() << "Yes was *not* clicked";
		}
	}
	else
	{
		/*QMessageBox message(QMessageBox::NoIcon, "Title", "Content with icon.");
		message.setIconPixmap(QPixmap("icon.png"));
		message.exec();*/
		QMessageBox::information(this, "check update", "version is the newest!");
	}
	return true;
}
QString MainWindow::saveFileName(const QUrl &url)
{
	QString path = url.path();
	QString basename = QFileInfo(path).fileName();

	if (basename.isEmpty())
		basename = "download";

	if (QFile::exists(basename)) {
		// already exists, don't overwrite
		int i = 0;
		basename += '.';
		while (QFile::exists(basename + QString::number(i)))
			++i;

		basename += QString::number(i);
	}
	QString fileTempPath = PicaApplication::getRoamingDir() + basename;
	return fileTempPath;
}
bool MainWindow::saveToDisk(const QString &filename, QIODevice *data)
{
	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly)) {
		fprintf(stderr, "Could not open %s for writing: %s\n",
			qPrintable(filename),
			qPrintable(file.errorString()));
		return false;
	}

	file.write(data->readAll());
	file.close();

	return true;
}
void MainWindow::checkForUpdates(bool verboseFlag)
{
	VerboseCheckingFlag = verboseFlag;
	QSettings settings;
	int totalKV = settings.value("totalKV", 0).toInt();
	int connectionInterval = settings.value("connectionInterval", 20).toInt();
	settings.setValue("connectionInterval", connectionInterval);
	int loadedMeshCounter = settings.value("loadedMeshCounter", 0).toInt();
	int savedMeshCounter = settings.value("savedMeshCounter", 0).toInt();
	QString UID = settings.value("UID", QString("")).toString();
	if (UID.isEmpty())
	{
		UID = QUuid::createUuid().toString();
		settings.setValue("UID", UID);
	}

#ifdef _DEBUG_PHP
	QString BaseCommand("/~cignoni/meshlab_d.php");
#else
	QString BaseCommand("/~cignoni/meshlab.php");
#endif

#ifdef Q_WS_WIN
	QString OS = "Win";
#elif defined( Q_WS_MAC)
	QString OS = "Mac";
#else
	QString OS = "Lin";
#endif
	//***更新check
	QString message = BaseCommand + QString("?code=%1&count=%2&scount=%3&totkv=%4&ver=%5&os=%6").arg(UID).arg(loadedMeshCounter).arg(savedMeshCounter).arg(totalKV).arg(PicaApplication::appVer()).arg(OS);
	//idHost=httpReq->setHost(PicaApplication::organizationHost()); // id == 1
	httpReq->get(QNetworkRequest(PicaApplication::organizationHost() + message));
	qDebug() << "message=" << message;
	//idGet=httpReq->get(message,&myLocalBuf);     // id == 2
}

void MainWindow::connectionDone(QNetworkReply *reply)
{
	QString answer = reply->readAll();
	if (answer.left(3) == QString("NEW"))
		QMessageBox::information(this, " Version Checking", answer.remove(0, 3));
	else if (VerboseCheckingFlag)
		QMessageBox::information(this, " Version Checking", "Your version is the most recent one.");

	reply->deleteLater();

	QSettings settings;
	int loadedMeshCounter = settings.value("loadedMeshCounter", 0).toInt();
	settings.setValue("lastComunicatedValue", loadedMeshCounter);
}


void MainWindow::submitBug()
{
	//***bug reporter
	//QMessageBox mb(QMessageBox::NoIcon,PicaApplication::appName(),PicaApplication::appName(),QMessageBox::NoButton, this);
	////mb.setWindowTitle(tr("MeshLab"));
	//QPushButton *submitBug = mb.addButton("Submit Bug",QMessageBox::AcceptRole);
	//mb.addButton(QMessageBox::Cancel);
	//mb.setText(tr("If Meshlab closed in unexpected way (e.g. it crashed badly) and"
	//    "if you are able to repeat the bug, please consider to submit a report using the SourceForge tracking system.\n"
	//    ) );
	//mb.setInformativeText(	tr(
	//    "Hints for a good, useful bug report:\n"
	//    "- Be verbose and descriptive\n"
	//    "- Report meshlab version and OS\n"
	//    "- Describe the sequence of actions that bring you to the crash.\n"
	//    "- Consider submitting the mesh file causing a particular crash.\n"
	//    ) );

	//mb.exec();

	//if (mb.clickedButton() == submitBug)
	//    QDesktopServices::openUrl(QUrl("http://sourceforge.net/tracker/?func=add&group_id=149444&atid=774731"));

}

void MainWindow::wrapSetActiveSubWindow(QWidget* window) {
	QMdiSubWindow* subwindow;
	subwindow = dynamic_cast<QMdiSubWindow*>(window);
	if (subwindow != NULL) {
		mdiarea->setActiveSubWindow(subwindow);
	}
	else {
		qDebug("Type of window is not a QMdiSubWindow*");
	}
}

int MainWindow::longestActionWidthInMenu(QMenu* m, const int longestwidth)
{
	int longest = longestwidth;

	const QList<QAction*>& acts = m->actions();
	foreach(QAction* act, acts)
	{
		QMenu* submenu = act->menu();
		if (!act->isSeparator() && (submenu == NULL))
			longest = std::max(longest, m->actionGeometry(act).width());

		else if (!act->isSeparator())
			longest = std::max(longest, longestActionWidthInMenu(submenu, longest));
	}
	return longest;
}

int MainWindow::longestActionWidthInMenu(QMenu* m)
{
	return longestActionWidthInMenu(m, 0);
}

int MainWindow::longestActionWidthInAllMenus()
{
	int longest = 0;
	QList<QMenu*> list = menuBar()->findChildren<QMenu*>();
	foreach(QMenu* m, list)
		longest = std::max(longest, longestActionWidthInMenu(m));
	return longest;
}

void MainWindowSetting::initGlobalParameterSet(RichParameterSet* glbset)
{
	//glbset->addParam(new RichBool(perMeshRenderingToolBar(), true, "Show Per-Mesh Rendering Side ToolBar", "If true the per-mesh rendering side toolbar will be redendered inside the layerdialog."));
	glbset->addParam(new RichBool(perMeshRenderingToolBar(), true, "", ""));
}

void MainWindowSetting::updateGlobalParameterSet(RichParameterSet& rps)
{
	permeshtoolbar = rps.getBool(perMeshRenderingToolBar());
}
//***20150526***
void MainWindow::buildwholeTabwidget()
{

	pTabWidget = new QTabWidget();
	homeTabWidget = createHomeTabWidget();
	viewTabWidget = createViewTabWidget();
	transformTabWidget = createTransformTabWidget();
	settingTabWidget = createSettingTabWidget();
	//toolsTabWidget = createToolsTabWidget();



	pTabWidget->addTab(homeTabWidget, tr("Home"));
	pTabWidget->addTab(viewTabWidget, tr("View"));
	pTabWidget->addTab(transformTabWidget, tr("Tools"));
	//pTabWidget->addTab(toolsTabWidget, tr("Tools"));
	pTabWidget->addTab(settingTabWidget, tr("Setting"));


	//pTabWidget->resize(800, 150);
	//pTabWidget->setSizePolicy();

}
QWidget *MainWindow::createHomeTabWidget()
{
	/*buildFileToolBar = new QToolBar;
	buildFileToolBar->addAction(newProjectAct);
	buildFileToolBar->addAction(openProjectAct);
	buildFileToolBar->addAction(saveProjectAct);*/


	QHBoxLayout *buildFileLayout = new QHBoxLayout;
	buildFileLayout->addWidget(buildFileToolBar);
	//buildFileLayout->setSizeConstraint(QLayout::SetMinimumSize);
	CustomizeGroupBox *buildFileGroupBox = new CustomizeGroupBox(tr("BuildFile"));
	buildFileGroupBox->setLayout(buildFileLayout);

	QHBoxLayout *partFileLayout = new QHBoxLayout;
	partFileLayout->addWidget(partFileToolBar);
	//partFileLayout->setSizeConstraint(QLayout::SetMinimumSize);
	CustomizeGroupBox *partFileGroupBox = new CustomizeGroupBox(tr("PartFile"));
	partFileGroupBox->setLayout(partFileLayout);

	QHBoxLayout *buildLayout = new QHBoxLayout;
	buildLayout->addWidget(buildToolBar);
	//buildLayout->setSizeConstraint(QLayout::SetMinimumSize);
	CustomizeGroupBox *buildGroupBox = new CustomizeGroupBox(tr("Build"));

	buildGroupBox->setLayout(buildLayout);

	QHBoxLayout *helpLayout = new QHBoxLayout;
	//helpLayout->setSizeConstraint(QLayout::SetMinimumSize);
	helpLayout->addWidget(helpToolBar);
	CustomizeGroupBox *helpGroupBox = new CustomizeGroupBox(tr("Help"));
	helpGroupBox->setLayout(helpLayout);

	QWidget *spacerWidget = new QWidget();//***空白spacer
	spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	spacerWidget->setVisible(true);

	QBoxLayout *HomeTabLayout = new QHBoxLayout;
	HomeTabLayout->setSizeConstraint(QLayout::SetMinimumSize);
	HomeTabLayout->addWidget(buildFileGroupBox);
	HomeTabLayout->addWidget(partFileGroupBox);
	HomeTabLayout->addWidget(buildGroupBox);
	HomeTabLayout->addWidget(helpGroupBox);
	HomeTabLayout->addWidget(spacerWidget);



	QWidget *homeTabWidget = new QWidget();
	homeTabWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
	homeTabWidget->setLayout(HomeTabLayout);


	//buildFileGroupBox->setStyleSheet("QGroupBox{	background-color: qlineargradient(x1 : 0, y1 : 0, x2 : 0, y2 : 1,	stop : 0 #E0E0E0, stop: 1 #FFFFFF);}"
	//	"QGroupBox{ border: 1px solid gray;}"
	//	"QGroupBox{border-radius: 5px;}"
	//	"QGroupBox{margin-top: 1ex; /* leave space at the top for the title */}"
	//	"QGroupBox::title{subcontrol-origin: margin; subcontrol-position: bottom center;padding: 0 3px;	background-color: qlineargradient(x1 : 0, y1 : 0, x2 : 0, y2 : 1,stop : 0 #FFOECE, stop: 1 #FFFFFF);}"
	//
	//
	//	);

	return homeTabWidget;
}
QWidget *MainWindow::createViewTabWidget()
{
	QHBoxLayout *viewLayout = new QHBoxLayout;
	viewLayout->addWidget(viewToolBar);
	CustomizeGroupBox *viewGroupBox = new CustomizeGroupBox(tr("View"));
	viewGroupBox->setLayout(viewLayout);

	QHBoxLayout *displayLayout = new QHBoxLayout;
	displayLayout->addWidget(displayToolBar);
	CustomizeGroupBox *displayGroupBox = new CustomizeGroupBox(tr("Display"));
	displayGroupBox->setLayout(displayLayout);

	QHBoxLayout *layerLayout = new QHBoxLayout;
	CustomizeGroupBox *layerGroupBox = new CustomizeGroupBox(tr("Layer"));
	layerGroupBox->setLayout(layerLayout);

	QHBoxLayout *zoomLayout = new QHBoxLayout;
	CustomizeGroupBox *zoomGroupBox = new CustomizeGroupBox(tr("Zoom"));
	zoomGroupBox->setLayout(zoomLayout);

	QWidget *spacerWidget = new QWidget();//***空白spcer
	spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	spacerWidget->setVisible(true);

	QHBoxLayout *ViewTabLayout = new QHBoxLayout;
	ViewTabLayout->addWidget(viewGroupBox);
	//ViewTabLayout->addWidget(displayGroupBox);
	//ViewTabLayout->addWidget(layerGroupBox);
	//ViewTabLayout->addWidget(zoomGroupBox);
	ViewTabLayout->addWidget(spacerWidget);

	QWidget *viewTabWidget = new QWidget();
	viewTabWidget->setLayout(ViewTabLayout);

	return viewTabWidget;
}
QWidget *MainWindow::createTransformTabWidget()
{
	QHBoxLayout *clipboardLayout = new QHBoxLayout;
	CustomizeGroupBox *clipboardGroupBox = new CustomizeGroupBox(tr("Clipboard"));
	clipboardGroupBox->setLayout(clipboardLayout);

	QHBoxLayout *editLayout = new QHBoxLayout;
	editLayout->addWidget(editToolBar_v2);
	CustomizeGroupBox *editGroupBox = new CustomizeGroupBox(tr("Edit"));
	editGroupBox->setLayout(editLayout);

	/*QHBoxLayout *translateLayout = new QHBoxLayout;
	CustomizeGroupBox *translateGroupBox = new CustomizeGroupBox(tr("Translate"));
	translateGroupBox->setLayout(translateLayout);

	QHBoxLayout *rotateLayout = new QHBoxLayout;
	CustomizeGroupBox *rotateGroupBox = new CustomizeGroupBox(tr("Rotate"));
	rotateGroupBox->setLayout(rotateLayout);

	QHBoxLayout *scaleLayout = new QHBoxLayout;
	CustomizeGroupBox *scaleGroupBox = new CustomizeGroupBox(tr("Scale"));
	scaleGroupBox->setLayout(scaleLayout);*/

	QWidget *spacerWidget = new QWidget();//***空白spcer
	spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	spacerWidget->setVisible(true);

	QHBoxLayout *transformTabLayout = new QHBoxLayout();
	transformTabLayout->addWidget(clipboardGroupBox);
	transformTabLayout->addWidget(editGroupBox);

	QWidget *transformTabWidget = new QWidget();

	//transformTabLayout->addWidget(translateGroupBox);
	//transformTabLayout->addWidget(rotateGroupBox);
	/*transformTabLayout->addWidget(ptranslate );
	transformTabLayout->addWidget(protate);
	transformTabLayout->addWidget(pscale);*/


	qDebug() << "plugv2" << PM.meshEditInterfacePlugV2.size();
	qDebug() << "plugv2" << PM.stringEditMap_v2.size();
	//************************************************************	
	QMap<QString, MeshEditInterFace_v2 *>::iterator mdi;
	//QVector<GenericParamGroupbox *>::iterator vgo;
	////for (int i = 0; i < 1; i++)
	MeshEditInterFace_v2 *iEdit = PM.stringEditMap_v2.last();
	QAction *editAction = iEdit->AC(PM.stringEditMap_v2.lastKey());
	genGroupbox.push_back(new GenericParamGroupbox(editAction, iEdit, transformTabWidget, PM.stringEditMap_v2.lastKey(), 0));
	transformTabLayout->addWidget(genGroupbox.last());


	for (mdi = PM.stringEditMap_v2.begin(); mdi != PM.stringEditMap_v2.end() - 1; ++mdi)
	{
		//mdi = PM.stringEditMap_v2.begin();
		MeshEditInterFace_v2 *iEdit = mdi.value();
		QAction *editAction = iEdit->AC(mdi.key());
		qDebug() << "editAction" << mdi.key();
		genGroupbox.push_back(new GenericParamGroupbox(editAction, iEdit, transformTabWidget, mdi.key(), 0));
		//genGroupbox.last()->setFixedSize(150, 110);
		//setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

		/*genGroupbox[0]->curgla = GLA();
		genGroupbox[0]->curmwi = this;*/

		transformTabLayout->addWidget(genGroupbox.last());

	}
	QHBoxLayout *toolsLayout = new QHBoxLayout;
	toolsLayout->addWidget(ToolToolBar);
	CustomizeGroupBox *toolsGroupBox = new CustomizeGroupBox(tr("Tools"));
	toolsGroupBox->setLayout(toolsLayout);
	transformTabLayout->addWidget(toolsGroupBox);

	//genTranGroupbox = new GenericParamGroupbox(PM.meshEditInterfacePlugV2[0], transformTabWidget, "Translate", 0);
	//genTranGroupbox->setFixedSize(150, 110);
	/*genRotateGroupbox = new GenericParamGroupbox(PM.meshEditInterfacePlugV2.first,transformTabWidget, &testParSet, "Rotate");
	genRotateGroupbox->setFixedSize(150, 110);
	genScaleGroupbox = new GenericParamGroupbox(PM.meshEditInterfacePlugV2[0],transformTabWidget, &testParSet, "Scale");
	genScaleGroupbox->setFixedSize(150, 110);*/
	//transformTabLayout->addWidget(genTranGroupbox);
	//transformTabLayout->addWidget(genRotateGroupbox);
	//transformTabLayout->addWidget(genScaleGroupbox);



	//*************************************************************************************************************************
	transformTabLayout->addWidget(spacerWidget);

	transformTabWidget->setLayout(transformTabLayout);

	return transformTabWidget;
}
QWidget *MainWindow::createSettingTabWidget()
{
	QHBoxLayout *settingLayout = new QHBoxLayout;
	settingLayout->addWidget(settingToolBar);
	CustomizeGroupBox *settingGroupBox = new CustomizeGroupBox(tr("Setting"));
	settingGroupBox->setLayout(settingLayout);


	QWidget *spacerWidget = new QWidget();//***空白spacer
	spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	spacerWidget->setVisible(true);

	QHBoxLayout *SettingTabLayout = new QHBoxLayout;
	SettingTabLayout->addWidget(settingGroupBox);
	SettingTabLayout->addWidget(spacerWidget);
	QWidget *SettingTabWidget = new QWidget();
	SettingTabWidget->setLayout(SettingTabLayout);

	return SettingTabWidget;
}
QWidget *MainWindow::createToolsTabWidget()
{
	QHBoxLayout *toolsLayout = new QHBoxLayout;
	toolsLayout->addWidget(ToolToolBar);
	CustomizeGroupBox *toolsGroupBox = new CustomizeGroupBox(tr("Tools"));
	toolsGroupBox->setLayout(toolsLayout);

	QWidget *spacerWidget = new QWidget();//***空白spcer
	spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	spacerWidget->setVisible(true);

	QHBoxLayout *ToolsTabLayout = new QHBoxLayout;
	ToolsTabLayout->addWidget(toolsGroupBox);
	ToolsTabLayout->addWidget(spacerWidget);
	QWidget *ToolsTabWidget = new QWidget();
	ToolsTabWidget->setLayout(ToolsTabLayout);

	return ToolsTabWidget;
}
void MainWindow::createToolBars_v2()
{
	buildFileToolBar = new QToolBar(tr("buildFile"));
	buildFileToolBar->addWidget(newProjectToolButton);
	buildFileToolBar->addWidget(openProjectToolButton);
	buildFileToolBar->addWidget(saveProjectToolButton);



	/*buildFileToolBar->addAction(this->newProjectAct);
	buildFileToolBar->addAction(this->openProjectAct);
	buildFileToolBar->addAction(reloadMeshAct);
	buildFileToolBar->addAction(saveSnapshotAct);
	buildFileToolBar->addAction(showLayerDlgAct);
	buildFileToolBar->addAction(showRasterAct);*/

	partFileToolBar = new QToolBar(tr("part"));
	partFileToolBar->addWidget(importMeshToolButton);
	//partFileToolBar->addWidget(exportMeshToolButton);
	partFileToolBar->addWidget(exportMeshAsToolButton);
	partFileToolBar->addWidget(ImportRecentMeshToolButton);

	/*partFileToolBar->addAction(importMeshAct);
	partFileToolBar->addAction(exportMeshAct);*/

	buildToolBar = new QToolBar(tr("build"));
	buildToolBar->addWidget(printImageFileButton);
	//buildToolBar->addWidget(dynamic_cast<QWidget   *>(&widget_p_monitor));

	helpToolBar = new QToolBar(tr("Help"));
	helpToolBar->addWidget(helpToolButton);


	//***view Tab
	viewToolBar = new QToolBar(tr("viewF"));
	viewToolBar->addWidget(frontToolButton);
	viewToolBar->addWidget(backToolButton);
	viewToolBar->addWidget(topToolButton);
	viewToolBar->addWidget(bottomToolButton);
	viewToolBar->addWidget(leftToolButton);
	viewToolBar->addWidget(rightToolButton);
	viewToolBar->addWidget(lockViewButton);
	viewToolBar->addWidget(topTagViewButton);
	//viewToolBar->addWidget(isometricToolButton);
	//viewToolBar->addWidget(projectToolButton);

	displayToolBar = new QToolBar(tr("display"));
	displayToolBar->addActions(renderModeGroupAct->actions());
	displayToolBar->addAction(renderModeTextureWedgeAct);
	displayToolBar->addAction(setLightAct);
	displayToolBar->addAction(setSelectFaceRenderingAct);
	displayToolBar->addAction(setSelectVertRenderingAct);
	//connect(renderToolBar, SIGNAL(actionTriggered(QAction*)), this, SLOT(updateMenus()));

	layerToolBar = new QToolBar(tr("layer"));
	zoomToolBar = new QToolBar(tr("zoom"));
	//***Transform Tab
	clipToolBar = new QToolBar(tr("clip"));

	editToolBar_v2 = new QToolBar(tr("edit"));
	editToolBar_v2->addWidget(undoToolButton);
	editToolBar_v2->addWidget(redoToolButton);

	TranslateToolBar = new QToolBar(tr("Translate"));
	RotateToolBar = new QToolBar(tr("Rotate"));
	ScaleToolBar = new QToolBar(tr("Scale"));
	//***Tools Tab
	ToolToolBar = new QToolBar(tr("Tool"));
	ToolToolBar->addWidget(autoPackingToolButton);
	ToolToolBar->addWidget(autoLandingToolButton);
	ToolToolBar->addWidget(showLayerToolButton);
	ToolToolBar->addWidget(startbuildToolButton);
	//ToolToolBar->addAction(PM.actionFilterMap.value("AutoPacking"));

	//***Setting Tab
	settingToolBar = new QToolBar(tr("setting"));
	settingToolBar->addAction(printjobestimateAct);
	settingToolBar->addWidget(groove_settingBtn);
	settingToolBar->addWidget(sirius_settingBtn);
	settingToolBar->addAction(saveSnapshotAct);

}

void MainWindow::createTransformGroupBox()
{
	//   ptranslate = new CustomizeGroupBox("tanslate") ;
	//QGridLayout  *translate_grid = new QGridLayout();
	//   translate_grid->addWidget(new QLabel(tr("X")),0, 0);
	//   translate_grid->addWidget(new QLabel(tr("Y")),1, 0);
	//   translate_grid->addWidget(new QLabel(tr("Z")),2, 0);
	//   translate_grid->addWidget(new QLineEdit( ),2, 1);
	//ptranslate->setLayout(translate_grid);


	//   protate = new CustomizeGroupBox("rotate");
	//   pscale = new CustomizeGroupBox("scale");
	//   //connect(ptranslate, );
	//ptranslate->setEnabled(false);
	//protate->setEnabled(false);
	//pscale->setEnabled(false);
}


void MainWindow::createQToolButton()
{
	//testbutton = new QToolButton();
	////testbutton->setPopupMode(QToolButton::MenuButtonPopup);
	//testbutton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	//testbutton->setDefaultAction(this->newProjectAct);
	//testbutton->setText("New Project");

	newProjectToolButton = new QToolButton(this);
	newProjectToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	newProjectToolButton->setDefaultAction(this->newProjectAct);
	newProjectToolButton->setText(tr("New Project"));

	openProjectToolButton = new QToolButton();
	openProjectToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	//20150827_comment_ 
	openProjectToolButton->setDefaultAction(this->openProjectAct);
	//openProjectToolButton->setDefaultAction(this->appendProjectAct);

	openProjectToolButton->setText(tr("Open Project"));

	saveProjectToolButton = new QToolButton();
	saveProjectToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	saveProjectToolButton->setDefaultAction(this->saveProjectAct);
	saveProjectToolButton->setText(tr("Save Project"));

	importMeshToolButton = new QToolButton();
	importMeshToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	importMeshToolButton->setDefaultAction(this->importMeshAct);
	importMeshToolButton->setText(tr("Import Model"));

	ImportRecentMeshToolButton = new QToolButton();
	ImportRecentMeshToolButton->setPopupMode(QToolButton::MenuButtonPopup);
	ImportRecentMeshToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	for (int i = 0; i < MAXRECENTFILES; ++i)
	{
		ImportRecentMeshToolButton->addAction(recentFileActs[i]);
	}
	ImportRecentMeshToolButton->setText(tr("Recent Mesh"));

	printImageFileButton = new QToolButton();
	printImageFileButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	printImageFileButton->setDefaultAction(this->threeDPrintAct);//appendProjectAct
																 //printImageFileButton->setDefaultAction(this->appendProjectAct);//--20151221_test

	printImageFileButton->setText(tr("Print Image"));



	exportMeshToolButton = new QToolButton();
	exportMeshToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	/*exportMeshToolButton->setDefaultAction(this->exportMeshAct);
	exportMeshToolButton->setText("Export Mesh");*/

	exportMeshAsToolButton = new QToolButton();
	exportMeshAsToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	exportMeshAsToolButton->setDefaultAction(this->exportMeshAsAct);
	exportMeshAsToolButton->setText(tr("Export As Mesh"));

	helpToolButton = new QToolButton();
	helpToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	helpToolButton->setDefaultAction(this->aboutAct);
	//helpToolButton->setDefaultAction(this->unDoAct);
	helpToolButton->setText("Help");

	cutToolButton = new QToolButton();
	cutToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	//cutToolButton->setDefaultAction(this->aboutAct);
	cutToolButton->setText("Cut");

	copyToolButton = new QToolButton();
	copyToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	//cutToolButton->setDefaultAction(this->aboutAct);
	copyToolButton->setText("Copy");

	pasteToolButton = new QToolButton();
	pasteToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	//cutToolButton->setDefaultAction(this->aboutAct);
	pasteToolButton->setText("Paste");

	//***view  
	topToolButton = new QToolButton();
	topToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	topToolButton->setDefaultAction(this->viewTopAct);
	topToolButton->setText("Back");

	bottomToolButton = new QToolButton();
	bottomToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	bottomToolButton->setDefaultAction(this->viewBottomAct);
	bottomToolButton->setText("Front");

	frontToolButton = new QToolButton();
	frontToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	frontToolButton->setDefaultAction(this->viewFrontAct);
	frontToolButton->setText("Top");

	backToolButton = new QToolButton();
	backToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	backToolButton->setDefaultAction(this->viewBackAct);
	backToolButton->setText("Bottom");

	leftToolButton = new QToolButton();
	leftToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	leftToolButton->setDefaultAction(this->viewLeftAct);
	leftToolButton->setText("Left");

	rightToolButton = new QToolButton();
	rightToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	rightToolButton->setDefaultAction(this->viewRightAct);
	rightToolButton->setText("Right");

	//***20160518 lock viewer
	lockViewButton = new QToolButton();
	lockViewButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	lockViewButton->setCheckable(true);
	lockViewButton->setDefaultAction(this->lockViewAct);
	lockViewButton->setText("Lock_View");
	//*****

	//***20150825***topTagView
	topTagViewButton = new QToolButton();
	topTagViewButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	topTagViewButton->setCheckable(true);
	topTagViewButton->setDefaultAction(this->topTagAction);
	topTagViewButton->setText("2DView");
	//***


	isometricToolButton = new QToolButton();
	isometricToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	//isometricToolButton->setDefaultAction(this->aboutAct);
	isometricToolButton->setText("Isometric");

	projectToolButton = new QToolButton();
	projectToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	//projectToolButton->setDefaultAction(this->aboutAct);
	projectToolButton->setText("Projection");

	//***edit
	undoToolButton = new QToolButton();
	undoToolButton->setIcon(QIcon(":undo.ico"));
	undoToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	undoToolButton->setDefaultAction(this->unDoAct);
	undoToolButton->setText("Undo");

	redoToolButton = new QToolButton();
	redoToolButton->setIcon(QIcon(":redo.ico"));
	redoToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	//redoToolButton->setDefaultAction(this->aboutAct);
	redoToolButton->setDefaultAction(this->reDoAct);
	redoToolButton->setText("Redo");

	////***view
	//QToolButton *topToolButton;
	//QToolButton *frontToolButton;
	//QToolButton *sideToolButton;
	//QToolButton *isometricToolButton;
	//QToolButton *projectbutton;
	//QToolButton *fastrenderToolbutton;
	//QToolButton *twoDToolbutton;
	////***Tool

	autoPackingToolButton = new QToolButton();
	autoPackingToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	autoPackingToolButton->setDefaultAction(this->autoPackingAct);
	autoPackingToolButton->setText("Auto Packing");

	autoLandingToolButton = new QToolButton();
	autoLandingToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	autoLandingToolButton->setDefaultAction(this->landAct);
	autoLandingToolButton->setText("Landing_Tool");

	showLayerToolButton = new QToolButton();
	showLayerToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	showLayerToolButton->setDefaultAction(PM.actionFilterMap.value("Slice_Tool"));
	showLayerToolButton->setText("ShowLayer");

	startbuildToolButton = new QToolButton();
	startbuildToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	startbuildToolButton->setDefaultAction(PM.actionFilterMap.value("Generate_Slice_Pic"));
	startbuildToolButton->setText("StartBuild");

	printjobestimateButton = new QToolButton();
	printjobestimateButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	printjobestimateButton->setDefaultAction(this->printjobestimateAct);
	printjobestimateButton->setText("print_job_estimate");

	groove_settingBtn = new QToolButton();
	groove_settingBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	groove_settingBtn->setDefaultAction(groove_settingAct);
	groove_settingBtn->setText("program_setting");

	sirius_settingBtn = new QToolButton();
	sirius_settingBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	sirius_settingBtn->setDefaultAction(this->sirius_settingAct);
	sirius_settingBtn->setText("sirius_setting");

	//QToolButton *autoPackingToolbutton;
	//QToolButton *showLayerToolbutton;
	//QToolButton *butterflyToolbutton;
	//QToolButton *startbuildToolbutton;
	
}
//***20150904
void MainWindow::connectEditAction()
{
	QMap<QString, MeshEditInterFace_v2 *>::iterator mdi;
	for (mdi = PM.stringEditMap_v2.begin(); mdi != PM.stringEditMap_v2.end(); ++mdi)
	{
		MeshEditInterFace_v2 * iEdit = mdi.value();
		QAction *editAction = iEdit->AC(mdi.key());
		connect(editAction, SIGNAL(triggered), this, SLOT(executeEdit()));
	}

}
void MainWindow::setGenericStyleSheet()
{
	//***20150526*****
	//***progressbar
	MainWindow::setStyleSheet(
		"QProgressBar {	border: 2px solid grey;	border-radius:5px;	text-align:center;}"
		"QProgressBar::chunk {background-color:#CD96CD; width:10px; margin:0.5px;}"
	);

	//***Tabbar
	//pTabWidget->setStyleSheet(
	pTabWidget->setStyleSheet(
		"QTabWidget::pane{ 	border-top: 2px solid #C2C7CB;	position: absolute;	top: -0.0em;	}"
		//"QTabWidget::tab-bar {	alignment: center;}"

		"QTabWidget::pane {  border-top: 2px solid #C2C7CB;  }"

		"QTabWidget::tab-bar { left: 2px; }"//***與mainwin邊界的距離

		"QTabBar::tab { height: 25px; width:100px;color: black; padding: 0px;}"
		"QTabBar::tab:selected { background: lightgray; } "

		"QTabBar::tab { background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #E1E1E1, stop: 0.4 #DDDDDD,  stop: 0.5 #D8D8D8, stop: 1.0 #D3D3D3);}"
		"QTabBar::tab {border: 2px solid #C4C4C3;}"//***設置邊緣屬性，可以設置邊角弧度，與顏色
		"QTabBar::tab { border-bottom-color: #C2C7CB;}"
		"QTabBar::tab { border-top-left-radius: 10px;}"
		"QTabBar::tab { border-top-right-radius: 10px;}"
		"QTabBar::tab { min-width: 8ex;}"
		"QTabBar::tab { padding: 2px;}"
		"QTabBar::tab:selected, QTabBar::tab:hover { background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,top: 0 #fafafa, stop: 0.4 #f4f4f4, stop: 0.5 #e7e7e7, stop: 1.0 #fafafa); }"
		"QTabBar::tab:selected {border-color: #9B9B9B;   border-bottom-color: #C2C7CB; }"
		"QTabBar::tab:!selected { margin-top: 0px; }"
		"QTabBar::tab:selected {margin-left: -0px;margin-right: -0px;}"
		"QTabBar::tab:first : selected{	margin-left: 0;	}"
		"QTabBar::tab : last : selected{margin-right: 0; }"
		"QTabBar::tab : only-one{margin: 0; }"

	);


	MainWindow::setStyleSheet("QGroupBox{	background-color: qlineargradient(x1 : 0, y1 : 0, x2 : 0, y2 : 1,	stop : 0 #E0E0E0, stop: 1 #FFFFFF);}"
		"QGroupBox{ border: 1px solid gray;}"
		"QGroupBox{border-radius: 5px;}"
		"QGroupBox{margin-top: 1ex; /* leave space at the top for the title */}"
		"QGroupBox::title{subcontrol-origin: margin; subcontrol-position: bottom center;padding: 0 3px;	background-color: qlineargradient(x1 : 0, y1 : 0, x2 : 0, y2 : 1,stop : 0 #FFAECE, stop: 1 #FFFFFF);}"
	);


	QApplication::setStyle(QStyleFactory::create("Fusion"));
	//menuBar()->setStyle(QStyleFactory::create("Fusion"));
	//***********
}
//**********
//***20160308
QHBoxLayout *MainWindow::test_transform_layout()
{
	QHBoxLayout *clipboardLayout = new QHBoxLayout;
	CustomizeGroupBox *clipboardGroupBox = new CustomizeGroupBox(tr("Clipboard"));
	clipboardGroupBox->setLayout(clipboardLayout);

	QHBoxLayout *editLayout = new QHBoxLayout;
	editLayout->addWidget(editToolBar_v2);
	CustomizeGroupBox *editGroupBox = new CustomizeGroupBox(tr("Edit"));
	editGroupBox->setLayout(editLayout);



	QWidget *spacerWidget = new QWidget();//***空白spcer
	spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	spacerWidget->setVisible(true);

	QHBoxLayout *transformTabLayout = new QHBoxLayout;


	QWidget *transformTabWidget = new QWidget();




	qDebug() << "plugv2" << PM.meshEditInterfacePlugV2.size();
	qDebug() << "plugv2" << PM.stringEditMap_v2.size();
	//************************************************************	
	QMap<QString, MeshEditInterFace_v2 *>::iterator mdi;
	//QVector<GenericParamGroupbox *>::iterator vgo;
	////for (int i = 0; i < 1; i++)
	MeshEditInterFace_v2 *iEdit = PM.stringEditMap_v2.last();
	QAction *editAction = iEdit->AC(PM.stringEditMap_v2.lastKey());
	genGroupbox.push_back(new GenericParamGroupbox(editAction, iEdit, transformTabWidget, PM.stringEditMap_v2.lastKey(), 0));
	transformTabLayout->addWidget(genGroupbox.last());

	for (mdi = PM.stringEditMap_v2.begin(); mdi != PM.stringEditMap_v2.end() - 1; ++mdi)
	{
		//mdi = PM.stringEditMap_v2.begin();
		MeshEditInterFace_v2 *iEdit = mdi.value();
		QAction *editAction = iEdit->AC(mdi.key());
		qDebug() << "editAction" << mdi.key();
		genGroupbox.push_back(new GenericParamGroupbox(editAction, iEdit, transformTabWidget, mdi.key(), 0));
		//genGroupbox.last()->setFixedSize(150, 110);
		//setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

		/*genGroupbox[0]->curgla = GLA();
		genGroupbox[0]->curmwi = this;*/

		transformTabLayout->addWidget(genGroupbox.last());

	}
	QHBoxLayout *toolsLayout = new QHBoxLayout;
	toolsLayout->addWidget(ToolToolBar);






	//*************************************************************************************************************************
	transformTabLayout->addWidget(spacerWidget);

	//transformTabWidget->setLayout(transformTabLayout);

	return transformTabLayout;
}
void MainWindow::createWhereIsSlicePosition()
{

}
QString MainWindow::internationlization()
{
	int x = currentGlobalParams.getEnum("Palette_Language");
	QString lan;
	if (x == 0)
	{
		lan = "";
	}
	else if (x == 1)
	{
		lan = "picasso_ja";
	}
	return lan;
}


///floatwidget
DockLeftWidget::DockLeftWidget(QWidget *parent) :QWidget(parent)
{
	createAction();
	bt1 = new QToolButton(this);
	bt1->setDefaultAction(btac1);
	bt2 = new QToolButton(this);
	bt2->setDefaultAction(btac2);
	bt3 = new QToolButton(this);
	bt3->setDefaultAction(btac3);

	bt1->setCheckable(true);
	bt2->setCheckable(true);
	bt3->setCheckable(true);
	connect(bt1, SIGNAL(toggled(bool)), this, SLOT(bt1show(bool)));


	//genGroupbox;
	QMap<QString, MeshEditInterFace_v2 *>::iterator mdi;

	for (mdi = mw()->PM.stringEditMap_v2.begin(); mdi != mw()->PM.stringEditMap_v2.end(); ++mdi)
	{
		//mdi = PM.stringEditMap_v2.begin();
		MeshEditInterFace_v2 *iEdit = mdi.value();
		QAction *editAction = iEdit->AC(mdi.key());
		qDebug() << "editAction" << mdi.key();
		genGroupbox.push_back(new GenericParamGroupbox(editAction, iEdit, this, mdi.key(), 0));
		genGroupbox.last()->setEnabled(true);
		genGroupbox.last()->curgla = mw()->GLA();
		genGroupbox.last()->curmwi = mw();
		//genGroupbox.last()->setWindowFlags(Qt::CustomizeWindowHint);		
		genGroupbox.last()->hide();
	}


	//QFrame *qf1 = new QFrame();
	QGridLayout *qGL1 = new QGridLayout();

	qGL1->addWidget(bt1);
	qGL1->addWidget(bt2);
	qGL1->addWidget(bt3);
	setLayout(qGL1);

	//setWindowFlags(Qt::CustomizeWindowHint);

}
MainWindow* DockLeftWidget::mw()
{
	QObject * curParent = this->parent();
	while (qobject_cast<MainWindow *>(curParent) == 0)
	{
		curParent = curParent->parent();
	}
	return qobject_cast<MainWindow *>(curParent);

}
void DockLeftWidget::bt1show(bool bt1ch)
{
	if (bt1ch)
	{
		genGroupbox.last()->show();
		qDebug() << "fw1";
	}
	else
	{
		genGroupbox.last()->close();
	}


	//else genGroupbox.last()->close();
}
void DockLeftWidget::showGroupbox(QAction *acin)
{
	QString acSt = acin->text();
	if (acSt == "bt1Ac")
	{
		genGroupbox[2]->show();
		genGroupbox[0]->close();
		genGroupbox[1]->close();
		qDebug() << "bt1Ac1";
	}
	else if (acSt == "bt2Ac")
	{
		genGroupbox[0]->show();
		genGroupbox[1]->close();
		genGroupbox[2]->close();
		qDebug() << "bt2Ac2";
	}
	else if (acSt == "bt3Ac")
	{
		genGroupbox[1]->show();
		genGroupbox[0]->close();
		genGroupbox[2]->close();
		qDebug() << "bt3Ac2";
	}
}
void DockLeftWidget::mousePressEvent(QMouseEvent *event)
{
	/*if (!this->hasFocus())
	{
	this->setFocus();
	bt1->setChecked(false);
	genGroupbox.last()->close();
	}*/
	if (!bt1->hasFocus())
	{
		bt1->setChecked(false);
		genGroupbox.last()->close();
	}
}
void DockLeftWidget::createAction()
{
	acGroup = new QActionGroup(this);
	acGroup->setExclusive(true);
	btac1 = new QAction("bt1Ac", acGroup);
	btac1->setCheckable(true);
	btac2 = new QAction("bt2Ac", acGroup);
	btac2->setCheckable(true);
	btac3 = new QAction("bt3Ac", acGroup);
	btac3->setCheckable(true);
	connect(acGroup, SIGNAL(triggered(QAction *)), this, SLOT(showGroupbox(QAction *)));

}
DockLeftWidget::~DockLeftWidget()
{

}
