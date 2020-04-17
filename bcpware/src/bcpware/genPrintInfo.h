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
#pragma once
#include <QFile>
#include <QDebug>
#include <QTextStream>
#include <QJsonDocument>
#include <QVariantMap>
#include <QJsonParseError>
#include <QJsonValue>
#include <QJsonObject>
#include <QDataStream>
#include <QTime>
#include <QDateTime>
#include <QStandardPaths>
#include <QMap>

#include "../common/meshmodel.h"
#include "../common/filterparameter.h"
#include "../common/picaApplication.h"
#include "../common/slice_program_setting.h"
#include "printerJobInfo.h"
#include <QMdiArea>


class GenPrintInfo
{
	//Q_OBJECT
	
public:
	GenPrintInfo(QString *);
	~GenPrintInfo();

	static void genZxInfo(MeshDocument &md, RichParameterSet &printParam,PrinterJobInfo &pji);
	static void genPrinterInfo(RichParameterSet &printParam,QMap<QString,QString> &infoMap);
	static void genPrinterInfoDM(RichParameterSet &printParam, QList<QPair<QString, QVariant>> &infoMap, PrinterJobInfo &pji);
	static void genPrinterInfoDMJson(RichParameterSet &printParam, PrinterJobInfo &pji, QString jsonFilename , QString thumbnailName, QString jobModelFileName, QString zxFileName);
	static double estimateInkUsageAllFiveChannel(RichParameterSet &printParam, PrinterJobInfo &pji);

	static void genAllParamInfo(RichParameterSet &param);
	static void getParamfromJsonFile(RichParameterSet &paramGet);
	static void count_total_Page();

	
	
private:
	static QString docPath(){ return QStandardPaths::locate(QStandardPaths::DocumentsLocation, "", QStandardPaths::LocateDirectory); }
	QString *proejectName;
};

