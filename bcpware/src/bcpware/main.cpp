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

#include <common/mlapplication.h>
#include <common/picaApplication.h>
#include <QMessageBox>
#include "mainwindow.h"
#include <QString>
#include <gl/freeglut.h>
#include "comm3DP.h"
#include <shellapi.h>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include "skt_function.h"
#include "gui_launcher.h"
#include "login.h"
//#include <QDateTime>


//#include "chipcommunication.h"

//ChipCommunication_t ccbg;
Comm3DP *comm = NULL;



struct InfoINeed {
	unsigned long info_processID;
	HWND best_handle;

};

BOOL is_main_window(HWND handle)
{
	return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
}

BOOL CALLBACK EnumWindowsProcMy(HWND hwnd, LPARAM lParam)
{
	InfoINeed* data = (InfoINeed*)lParam;
	unsigned long process_id = 0;
	GetWindowThreadProcessId(hwnd, &process_id);
	if (data->info_processID != process_id || !is_main_window(hwnd)) {
		return TRUE;
	}
	data->best_handle = hwnd;
	return FALSE; 

}


HWND getWindowbyProcessID(unsigned long m_ProcessId)
{
	InfoINeed data;
	data.info_processID = m_ProcessId;
	data.best_handle = 0;
	EnumWindows(EnumWindowsProcMy, (LPARAM)&data);
	return data.best_handle;
}
QWidget *creatAWidget()
{


	QGridLayout *gridL1 = new QGridLayout;
	QString s(
		"background:url(:/images/icons/btn_printing_estimate.png);"
		"background-repeat: no-repeat;"
		//"background-color: rgba(0,0,0,0)"
		);
	QMovie *movie = new QMovie(":images/icons/giphy.gif");
	QWidget *xWg = new QWidget;
	xWg->resize(QSize(50, 50));
	QLabel *yLb = new QLabel(xWg);
	//yLb->setText("dfasdf");
	yLb->resize(QSize(50, 50));
	yLb->setMovie(movie);

	gridL1->addWidget(yLb);

	xWg->setLayout(gridL1);
	xWg->setWindowFlags(Qt::FramelessWindowHint);
	//x->setStyleSheet(s);	

	return xWg;


}

int main(int argc, char *argv[])
{
	qDebug() << SettingParams::settingWTF;
	HANDLE m_hMutex = CreateMutex(NULL, FALSE, DSP_PALETTE_GUID);
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		CloseHandle(m_hMutex);
		m_hMutex = NULL;


		DWORD processID = SKT::findCMDProcess<bool>(DSP_PALETTE_PROCESS_NAME);
		HWND m_hwnd = getWindowbyProcessID(processID);


		bool x = ShowWindow(m_hwnd, SW_SHOWMAXIMIZED);


		return FALSE;
	}

	SKT::terminateCMDProcess<bool>(DSP_MAINTOOL_NAME);
	SKT::terminateCMDProcess<bool>(DSP_USB2HTTP_NAME);
	SKT::terminateCMDProcess<bool>(DSP_DASHBOARD_NAME);	//
	SKT::terminateCMDProcess<bool>(DSP_CHIPCOMMNAME);
	//if (!DSP_TEST_MODE){
		Comm3DP::openProc();
		Sleep(3000);
		comm = new Comm3DP;
		bool x = comm->initFail();
		string s, e;
		bool y = comm->printerStatus(s, e);
		if (x)
		{
			return 0;
			SKT::terminateCMDProcess<bool>(DSP_CHIPCOMMNAME);
			comm->reset();
			Comm3DP::openProc();
		}
	//}
		

	//ShellExecute(NULL, L"runas", L"3DPDashboard.exe", NULL, NULL, SW_HIDE);
	//FILE *dbgff;
	//dbgff = fopen("D:\\debug_open_2.txt", "w");//@@@

	/*fprintf(dbgff, "1\n");
	fflush(dbgff);*/
	glutInit(&argc, argv);
	PicaApplication app(argc, argv);

	

	/*fprintf(dbgff, "2\n");
	fflush(dbgff);*/

	QLocale::setDefault(QLocale::C);
	QCoreApplication::setOrganizationName(PicaApplication::organization());
	

#if QT_VERSION >= 0x050100
	// Enable support for highres images (added in Qt 5.1, but off by default)
	QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
	/*fprintf(dbgff, "3\n");
	fflush(dbgff);*/

	//***test logo widget
	//QWidget *testwidget = new QWidget;
	////testwidget->setLayout();
	//testwidget->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
	////testwidget->setBackgroundRole();
	//testwidget->setWindowOpacity(1);
	//testwidget->show();
	////test->close();
	

	QString tmp = PicaApplication::appArchitecturalName(PicaApplication::HW_ARCHITECTURE(QSysInfo::WordSize));
	QCoreApplication::setApplicationName(PicaApplication::appArchitecturalName(PicaApplication::HW_ARCHITECTURE(QSysInfo::WordSize)));

	if (argc > 1)
	{
		QString helpOpt1 = "-h";
		QString helpOpt2 = "--help";
		if ((helpOpt1 == argv[1]) || (helpOpt2 == argv[1]))
		{

			exit(-1);
		}
	}
	//QMessageBox::warning(0, "Meshlab Initialization", "Serious error. Unable to find the plugins directory.");

	

	MainWindow window;
	window.setWindowState(window.windowState() ^ Qt::WindowMaximized);
	//window.setWindowState(Qt::WindowMaximized);
	//window.show();	
	//window.showMaximized();

	



	MainWindow2 ww(&window);
	ww.raise(); 
	ww.show();

	//ccbg.init();
	// This event filter is installed to intercept the open events sent directly by the Operative System.
	FileOpenEater *filterObj = new FileOpenEater(&window);
	app.installEventFilter(filterObj);
	app.processEvents();

	/*WCHAR *cc = _T(".\\3DPDashboard.exe");
	SKT::execute3DPDashboard<bool>(cc);*/

	////20180608work
	//window.testFuncFunc();

	//if (argc > 1)
	//{
	//	QString helpOpt1 = "-h";
	//	QString helpOpt2 = "--help";
	//	if ((helpOpt1 == argv[1]) || (helpOpt2 == argv[1]))
	//		;

	//	if (QString(argv[1]).endsWith("3mf", Qt::CaseInsensitive)/* || QString(argv[1]).endsWith("aln", Qt::CaseInsensitive) || QString(argv[1]).endsWith("out", Qt::CaseInsensitive) || QString(argv[1]).endsWith("nvm", Qt::CaseInsensitive)*/)
	//		window.openProject2(argv[1]);
	//	else
	//		window.importMeshWithLayerManagement(argv[1]);
	//}
	//else 	if(filterObj->noEvent) window.open();

	
	int r = app.exec();
	
	/*comm->closeProc();
	delete comm;
	comm = NULL;*/

	/*GDPRDialog gdpr;
	int dialogCode = gdpr.exec();
	if (dialogCode == QDialog::Accepted) {}
	if (dialogCode == QDialog::Rejected) {}*/



	QDir tempTexPath = PicaApplication::getRoamingDir() + "temptexture";
	if (tempTexPath.exists())
		tempTexPath.removeRecursively();
	SKT::terminateCMDProcess<bool>(DSP_DASHBOARD_NAME);
	SKT::terminateCMDProcess<bool>(DSP_CHIPCOMMNAME);
	


	return r;
}


//int main(int argc, char *argv[])
//{
//	FILE *dbgff;
//	dbgff = fopen("D:\\debug_open_2.txt", "w");//@@@
//
//	fprintf(dbgff, "1\n");
//	fflush(dbgff);
//	//ccbg.init();
//
//	return(0);
//}
