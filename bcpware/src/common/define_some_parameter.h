/****************************************************************************
* BCPware
*
*  Copyright 2018 by 
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
#ifndef DEFINE_SOME_PARAMETER_H
#define DEFINE_SOME_PARAMETER_H

#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE
#define _UNICODE
#endif
#include <tchar.h>
#include <QString>
#include <QStandardPaths>
#include <QDateTime>
#include <QCryptographicHash>
#include <QDir>
#include "../bcpware/qaesencryption.h"
#include "define_version.h"

//#define DSP_SOFTWARE_VERSION "1.0.1.0"
#define DSP_cmmm 10
#define DSP_inchmm 25.4
#define DSP_plusPrintLength 134
#define DSP_grooveX 222
#define DSP_grooveY 222
#define DSP_grooveZ 200
#define DSP_grooveBigX 350
#define DSP_Color_thick 0.5
#define DSP_Binder_thick 6
#define DSP_Color_Replace_Binder_thick 3
#define DSP_Look_Distination 300
#define DSP_Blank_pages 0
#define DSP_Estimate_Usage_Slice_Height 0.1
#define DSP_Feeder_Up_Ratio 1.52
#define DSP_Feeder_Height 330
//#define

#define DSP_CHIPCOMMNAME _T("chip3dpcomm.exe")
#define DSP_DASHBOARD_NAME _T("3DPDashboard.exe")
#define DSP_DASHBOARD_WINDOW_NAME _T("Dashboard")

#define DSP_PALETTE_PROCESS_NAME _T("BCPware.exe")
#define DSP_PALETTE_GUID _T("79DD151F-0488-4E41-8A26-3AFE7B46A6F6")

#define DSP_MAINTOOL_NAME _T("maintTool.exe")


#define DSP_USB2HTTP_NAME _T("usb2http.exe")

#define DSP_APP_NAME "Partpro350 xBC"

#define DSP_REPORT_SAMPLE_FILE_NAME "Cost_Template.xlsx"
#define DSP_REPORT_SAMPLE_FILE_NAME_TWO "FUCK_COST.xlsx"

#define DSP_PARAMETER_FILE_NAME "ParameterUI_STX.txt"

//PrintHead
#define DSP_pagesPerWipe 128; //wiper :pages per step  128
#define DSP_suck_preJob 0.036 //
#define DSP_wipe_midJob 0.036 //
#define DSP_wipe_postJob 0.072
//#define DSP_Midjob_Frequence 8

#define DSP_WIPER_C 0.163
#define DSP_WIPER_M 0.176
#define DSP_WIPER_Y 0.196
#define DSP_WIPER_B 0.486

#define DSP_SLOW_WIPER_C 0.281
#define DSP_SLOW_WIPER_M 0.346
#define DSP_SLOW_WIPER_Y 0.392
#define DSP_SLOW_WIPER_B 0.99

//post Process Glu Usage
#define DSP_postProcessGlu 0.1 //0.1 g/cm^2
//Power proportion
#define DSP_Power_Proportion 1.098

#define DSP_FIRST_TWENTY 20


#define DSP_TEST_MODE false





#define DOCUMENT_NAME "BCPware"

#define LOG_FOLDER_NAME "log"

//#define DM_CMD_VALUE_JSON_FILE "NewUiParamTest.txt"
#define DM_UI_JSON_FILE "ParameterUITest.txt"
#define DM_TEST_PAGE_FILE "OmniPEC.bor"

#define BuilderPlateHeight 30
#define FeederZAxisHeight 350
#define SparePowderHeight 15
#define SAFE_BUILDER_HEIGHT_WITH_PLATE 120


//#define DM_CONSOLE_FILE ""




namespace SettingParams{
	static double settingWTF = 1.2345;

	static void setparamFunc()
	{
		settingWTF = 1;
	}
}



static inline QString getDocumentDir()
{
	return QStandardPaths::locate(QStandardPaths::DocumentsLocation, "", QStandardPaths::LocateDirectory);
}

static inline QString getDocumentFolder()
{
	return getDocumentDir() + "/" + DOCUMENT_NAME + "/";
}

static inline QString getLogDir()
{
	return getDocumentDir() + "/" + "log" + "/";
}
//static inline QString getLogFolder()
//{
//	return getDMDocumentFolder() + "/" + DM_LOG_FOLDER_NAME + "/";
//}

static inline QString getLogFileName()
{
	return getDocumentFolder() + "server_" + QDateTime::currentDateTime().toString("yyyy_MM_dd") + ".log";
}


class BCPwareFileSystem
{	

public:

	static const QString appName();
	static const QString dashboardName();
	static const QString encryptKey();
	static const QString parameterFileName();
	static const QString parameterFilePath();
	static QDir documentDir();
	static QDir documentDashboardDir();
	static QDir logDir();	
	static const QString printingHistoryFilePath();
	//static QString logFileName();
	static QDir projectThumbnailFolder();

	static bool encryptParam(QString inputString, QString &outputString);
	static bool encryptParam(QString inputString, QFileInfo);

	static bool decodeParam(QString &ouputString, QString inputString = QString(), QString filePath = QString());
	
	

};




#endif