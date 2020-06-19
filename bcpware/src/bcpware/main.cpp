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
#include <io.h>
//#include <QDateTime>


//#include "chipcommunication.h"

//ChipCommunication_t ccbg;
Comm3DP *comm = NULL;
/*===related log=====*/
static bool g_bWriteToLog = true;
int logSeverity = 3;// ParameterModel::getValueFromCategoryAndName(2, "RECORD_LOG_SEVERITY").toInt();/*2 = Common_Setting*/;
static FILE * g_hOut = stderr;

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
static void logMessageOutputHandler(QtMsgType Type, const QMessageLogContext & Context, const QString & Message);
static void rotateLogs();
int main(int argc, char *argv[])
{
	//logger = 
	/*QString logFileName = "/" + QDateTime::currentDateTime().toString("yyyy_M") + "BCPware_log.txt";
	QFileInfo outfile(QStandardPaths::locate(QStandardPaths::DocumentsLocation, "", QStandardPaths::LocateDirectory) + PicaApplication::appName() + logFileName);
	std::wofstream *tempWof;	
	tempWof = new std::wofstream(outfile.absoluteFilePath().toStdString(), std::ofstream::out | std::ofstream::app);
	logger.AddOutputStream(tempWof, true, framework::Diagnostics::LogLevel::Info);
	WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("hahahahah"));
	WRITELOG(logger, framework::Diagnostics::LogLevel::Error, _T("hahahahahb"));
	WRITELOG(logger, framework::Diagnostics::LogLevel::Debug, _T("hahahahahc"));*/
	WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("hahahahah"));
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

#if(!defined QT_DEBUG) || (!defined _DEBUG)
		qInstallMessageHandler(logMessageOutputHandler);
		rotateLogs();
#endif


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



static void logMessageOutputHandler(QtMsgType Type, const QMessageLogContext & Context, const QString & Message)
{
	QString Timestamp = QDateTime::currentDateTimeUtc().toString("HH:mm:ss.zzz");
	QByteArray LocalMsg = Message.toUtf8();
	const char * pcType = "";
	const char * pcFile = Context.file ? Context.file : "";
	const char * pcFunction = Context.function ? Context.function : "";
	char memoryUsage[10];
	int8_t cSeverity = 0;

#ifdef _WIN32
	MEMORYSTATUSEX statex;

	statex.dwLength = sizeof(statex);

	GlobalMemoryStatusEx(&statex);

	sprintf(memoryUsage, " %*ld%%", 7, statex.dwMemoryLoad);


#endif 

	switch (Type)
	{
	case QtDebugMsg:
		pcType = "Debug:   ";
		cSeverity = 1;
		break;
	/*case QtInfoMsg:
		pcType = "Info:    ";
		cSeverity = 2;
		break;*/
	case QtWarningMsg:
		pcType = "Warning: ";
		cSeverity = 3;
		break;
	case QtCriticalMsg:
		pcType = "Critical:";
		cSeverity = 4;
		break;
	case QtFatalMsg:
		pcType = "Fatal:   ";
		cSeverity = 5;
		break;
	}

	if (cSeverity >= logSeverity)
	{
		fprintf(g_hOut,
			//"%s %s\t%s \t(%s:%u, %s) %s\n",
			"%s %s\t%s \t(%s:%u) \n",
			Timestamp.toLatin1().constData(),
			pcType,
			LocalMsg.constData(),
			pcFile,
			Context.line
			/*pcFunction, memoryUsage*/);
		fflush(g_hOut);
	}




	//    if ( nullptr != g_hOut )
	//    {
	//        if ( g_bVerboseLogging )
	//        {
	//            fprintf( g_hOut,
	//                     "%s %s\t%s (%s:%u, %s) %s\n",
	//                     Timestamp.toLatin1().constData(),
	//                     pcType,
	//                     LocalMsg.constData(),
	//                     pcFile,
	//                     Context.line,
	//                     pcFunction, memoryUsage);
	//        }
	//        else
	//#ifndef QT_DEBUG
	//            if ( Type != QtDebugMsg )
	//#endif  // QT_DEBUG
	//            {
	//                fprintf( g_hOut, "%s %s %s\n", Timestamp.toLatin1().constData(), pcType, LocalMsg.constData() );
	//            }
	//        fflush( g_hOut );
	//    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

static void rotateLogs()
{
	static QTimer * pLogRotateTimer = nullptr;

	// Check whether logs are supposed to be used.
	if (!g_bWriteToLog)
	{
		return;
	}

	if (nullptr == pLogRotateTimer)
	{
		pLogRotateTimer = new QTimer(nullptr);
		QObject::connect(pLogRotateTimer, &QTimer::timeout, rotateLogs);

		// Configure the timer.
		pLogRotateTimer->setSingleShot(false);
		QDateTime Now = QDateTime::currentDateTimeUtc();
		QDateTime Midnight = Now.addDays(1);
		Midnight.setTime(QTime(0, 0, 0));
		qint64 llMillisecondsToMidnight = Now.msecsTo(Midnight);
		if (llMillisecondsToMidnight < 1000)
		{
			llMillisecondsToMidnight = (24 * 60 * 60 * 1000);
		}
		pLogRotateTimer->start(static_cast<int>(llMillisecondsToMidnight));
	}
	else
	{
		// Update interval to 24 hours.
		pLogRotateTimer->start(24 * 60 * 60 * 1000);
	}

	static QFile * pErrorLog = nullptr;

	// Rotate time, close the existing log
	if (nullptr != pErrorLog)
	{
		fclose(g_hOut);
		g_hOut = stderr;
		pErrorLog->close();
		delete pErrorLog;
		pErrorLog = nullptr;
	}

	// Try to create the log directory
	QDir LogDirectory(getDocumentFolder());// = getDocumentFolder();// DMShopFilesystem::logDir();
	if (QDir().mkpath(getDocumentFolder()) && (nullptr == pErrorLog))
	{
		// Open a new log
		pErrorLog = new QFile(
			LogDirectory.filePath(QDateTime::currentDateTimeUtc().toString("'BCPwareQlog_'yyyyMMdd'.log'")));
		if (pErrorLog->open(QIODevice::WriteOnly | QIODevice::Append))
		{
			// Re-open the file as a FILE*
			int iFD = pErrorLog->handle();
			g_hOut = fdopen(dup(iFD), "a");

			// Close the file.
			pErrorLog->close();
		}
	}

	// Failed to open a file, write to stderr instead
	if (nullptr == g_hOut)
	{
		g_hOut = stderr;
	}

	// Create the archive directory
	QDir ArchiveDirectory = QDir(LogDirectory.path() + "/archive");
	if (ArchiveDirectory.mkpath(ArchiveDirectory.path()))
	{
		// Create a list of existing log files (typically one).
		QStringList OldLogs = LogDirectory.entryList(QStringList("*.log"), QDir::Files | QDir::NoDotAndDotDot);
		for (const auto & Log : OldLogs)
		{
			if ((nullptr == pErrorLog) || pErrorLog->fileName().endsWith(Log))
			{
				// Skip the current log
				continue;
			}

			// Move the log into the archive then call gzip (which deletes the original file).
			QStringList Arguments;
			Arguments << LogDirectory.filePath(Log);
			Arguments << LogDirectory.filePath("archive/" + Log);
			QProcess::execute("mv", Arguments);
			QProcess::startDetached("gzip", QStringList(Log), LogDirectory.path() + "/archive");
		}
	}

	qWarning() << "Date UTC: " << QDateTime::currentDateTimeUtc().toString("yyyy/MM/dd");
}
/*--------------------------------------------------------------------------------------------------------------------*/



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
