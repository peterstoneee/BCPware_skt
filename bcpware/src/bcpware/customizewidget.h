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
#ifndef CUSTOMIZEWIDGET_H
#define CUSTOMIZEWIDGET_H

#include <QWidget>
#include <QGroupBox>
#include <QLabel>
#include "stdpardialog.h"

class CustomizeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CustomizeWidget(QWidget *parent = 0);

signals:

public slots:

};

class CusDialog :public QDialog
{
	Q_OBJECT
public:
	QLabel *lb;
	CusDialog(QWidget *parent) :QDialog(parent)
	{
		lb = new QLabel("testdddddd");
		QLayout *l = new QVBoxLayout(this);
		this->setWindowTitle("Status");
		l->addWidget(lb);
		this->setLayout(l);
	}
	~CusDialog(){};
};


class CustomizeGroupBox : public QGroupBox
{
    Q_OBJECT
public:
    CustomizeGroupBox(const QString &title, QWidget* parent=0);

};
//class CustomizeGroupBox_v2 : public QGroupBox
//{
//	Q_OBJECT
//public:
//	CustomizeGroupBox_v2(const QString &title, QWidget *parent = 0);
//
//private:
//	RichParameterSet& curpar;
//protected:
//	DynamicFloatWidget dy_x_axis;
//	DynamicFloatWidget dy_y_axis;
//	DynamicFloatWidget dy_z_axis;
//	FloatWidget x_ed;
//	FloatWidget y_ed;
//	FloatWidget z_ed;	
//	
//	QGridLayout gdly;
//	
//	
//};

#endif // CUSTOMIZEWIDGET_H
