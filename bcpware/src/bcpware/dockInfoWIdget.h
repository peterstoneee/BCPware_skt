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


#include "ui_dockInfoWidgetUI.h"
#include "widgetStyleSheet.h"

namespace { class testInfoForm; }
class DockInfoWIdget : public QWidget
{
	Q_OBJECT
public:
	DockInfoWIdget(QWidget *parent = 0);
	~DockInfoWIdget();
public:
		Ui::testInfoForm *ui;
		void updateUi(QStringList, bool);
		void uiInit();
public slots:
		void sOAToggled(bool);
		void triggerPrintEstimate();
	signals:
		void sOAswitch();
		void estimateSIG();

};

