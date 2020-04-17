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
#define _UNICODE
#include "comm3DP.h"
#include <QVariant>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include <QTimer>
#include <QPixmap>
#include <QPainter>
#include <QDialog>
#include <QWidget>

#include "../common/slice_program_setting.h"
#include "ui_PrintOptionDia_ui.h"
#include "widgetStyleSheet.h"
#include "../common/meshmodel.h"

namespace{
	class PrintOptionDiaUi;
}

extern Comm3DP *comm;

class PrintOption: public QDialog
{
	Q_OBJECT
public:
	enum optionMode{MONO_PRINT_MODE,NORMAL_PRINT_MODE, TEST_PRINT_PAGE_MODE,PRINT_ZXA_FLE, START_POINT_MEASUREMENT};
	PrintOption(optionMode mode,QWidget *parent = 0,MeshDocument *meshdoc = 0);
	~PrintOption();
public:
	Ui::PrintOptionDiaUi *ui;
public:
	void initPOL();
	bool initComm();
	void initTestPrintItem();
public:
	void updatePrintSummaryLabel(Print_Job_Information &pJI);
	bool getMonoParam();
	//int getWiperFrequency();
	int getSpittonData();
	void initItem();
	void getHeatStuffParam(QVector<int> &);
	//int getWiperClick();
	//int getWiperIndex();
public slots:
	void gotoCheckListDialog();
	void getPrintStatus();
	void showHeatSetting(bool);
private:
	FILE *dbgf;
	QTimer *timer;
	optionMode om;
	QMap<QString, int> receiveStatus;
	MeshDocument *meshDoc;
	

};

