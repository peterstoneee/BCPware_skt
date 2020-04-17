
#include "comm3DP.h"

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
#include <QTableWidget>

#include <common/interfaces.h>
#include "../common/slice_program_setting.h"
#include "ui_checklist.h"


namespace{
	class Form;
}

extern Comm3DP *comm;

class CheckListWidget :public QDialog
{
	Q_OBJECT
public:
	CheckListWidget(Print_Job_Information *pji = 0 ,QWidget *parent = 0);
	~CheckListWidget();


public slots:
	void stackWidgetSetToPrintConnect();
	void stackWidgetSetToInkReady();
	void stackWidgetSetToPowerReady();
	void stackWidgetSetToWasteTankReady();
	void stackWidgetSetToZ1Z2Ready();
	void stackWidgetSetToWiperReady();
	void updatePrintStatus();
	void startPrint_accepted();
	void skip_checking();
	void checkListIsReady();
	
public:
	//Comm3DP *comm;
	bool initComm();
	void initButton();
	bool initcccc;
	bool sendHeader(Print_Job_Information &,int fanspeed = 5000, double layerHeight = 0.1016);
	void updateCMYBUsage();
private:	
	Ui::Form *ui;
	void updateInkStatus(unsigned int cPercent, unsigned int mPercent, unsigned int yPercent, unsigned int bPercent);
	void updateInkStatus2(unsigned int cPercent, unsigned int mPercent, unsigned int yPercent, unsigned int bPercent);	
	void intiInkStackWidget();
	bool updateZAxis(float, float);
	bool builderFeederReady;
	bool getbuilderFeederReady(){ return builderFeederReady; }
	void setbuilderFeederReady(bool ret){ builderFeederReady = ret; }

	QRect *rect1;
	QPixmap *cyanPixmap;
	QPixmap *magentaPixmap;
	QPixmap *yellowPixmap;
	QPixmap *binderPixmap;

	QPainter *cyanPainter;
	QPainter *magentaPainter;
	QPainter *yellowPainter;
	QPainter *binderPainter;
	QTableWidgetItem *item[8];
	QTableWidgetItem *spaceItem[4];
	QTableWidgetItem *wiperItem[2];



	Print_Job_Information *pJI;
	QTimer *checkListTimer;
	void drawPainter(QPainter *painter, QPixmap *pixmap, QLabel *label, unsigned int percentQ, QColor arcColor, QColor arcColor2, QColor recColor, int half_penWidth, QSize mapSize);

};

