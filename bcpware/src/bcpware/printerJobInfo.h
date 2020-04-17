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
#ifndef PRINTERJOBINGFO_H
#define PRINTERJOBINGFO_H

#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE
#define _UNICODE
#endif
#include <QDialog>
#include<QDateTime>
#include<QTime>
#include<QTextEdit>
#include "../common/meshmodel.h"
#include "../common/slice_program_setting.h"

QT_BEGIN_NAMESPACE
class QAction;
class QListWidget;
class QMenu;
class QTextEdit;
class QTime;
class QDataTime;
QT_END_NAMESPACE


class PrinterJobInfo:public QDialog
{
	Q_OBJECT
public:
	enum printInfoMode{ PrintOptionMode, EstimateMode,PrintPrintingFile }printInfoMode_Option;
	PrinterJobInfo(QWidget* p, MeshDocument *md, RichParameterSet * parlst, QVector<float> *cmyUsage = NULL, printInfoMode = EstimateMode, bool two_side_print = false);
	~PrinterJobInfo();

	void updateinfo();
	template<class T>
	QString combinestring(QString ,T);

	QTime estimatedbuildTime(float height);
	float estimateVolumn();
	float estimateArea();
	float estimateBinderInkUsage();
private:
	Print_Job_Information pJI;
	QTextEdit *te1;
	QFrame *frame1;
	MeshDocument *mdd;
	QVector<float> *CMYUsage;
	//float *CMYUsage;
	bool two_side_print;

	void initPJI();
	void createframe();
	float slice_height;
	int getSliceLayer(bool);
	int estimateWiper();
	float estimatePostProcessUsage();
	bool createReport();
	double maintBinder, maint_Wiper_B, maint_Spitton_B, maint_useless_B, binder_PrintUsage;;
	static QString docPath(){ return QStandardPaths::locate(QStandardPaths::DocumentsLocation, "", QStandardPaths::LocateDirectory); }
	int logsize;
	RichParameterSet *param;

public slots:
	void saveLogslot();
	void closeBtnSlot();
	void exportReportSlot();
public :
	static void updatePJI(Print_Job_Information *PJI, MeshDocument *md);
	Print_Job_Information getPJI();
	void setLayers(int);
	void setSMLayers(int);
	void genCSVFile();
	bool hasCSVHeader();
	//generate csv file
	//inesert data at specific position
	//find specific line
		//how do I know where to insert?
	//analysis first line
	void writeAtLastLine();

};

#endif