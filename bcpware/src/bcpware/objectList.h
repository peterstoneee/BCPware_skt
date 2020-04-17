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
#pragma once
#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QDockWidget>
#include <QTableWidgetItem>
#include <QTableWidget>
#include <QAbstractItemView>
#include "../common/filterparameter.h"

#include "ui_objectList_ui.h"

class MainWindow;
class GLLogStream;
class MeshModel;
class RasterModel;
class MeshDocument;
class StdParFrame;
class QGridLayout;
class QToolBar;

namespace
{
	class Object_List;
}

class ModelTreeWidgetItem : public QTreeWidgetItem
{
public:
	ModelTreeWidgetItem(MeshModel *meshModel, QTreeWidget* tree);
	~ModelTreeWidgetItem();
	MeshModel* mItem;

};

class MyTreeWidget :public QTreeWidget
{
	Q_OBJECT
public:
	MyTreeWidget(QWidget *parent = 0);
	~MyTreeWidget();
protected:
	void mousePressEvent(QMouseEvent *event);
signals:
	void clearall();
	
};


class ObjectList:public QDockWidget
{
	Q_OBJECT
public:
	ObjectList(QWidget *parent = 0);
	~ObjectList();
	void updateTable();
	void updateLabel();
	void updateUI(bool activeDoc);
	MeshDocument *md_table;
	MyTreeWidget *myTreeWidget;
	void init();
	void closeEvent(QCloseEvent *event);
public slots:
	void setTreeWidgetVisible(bool);
	void modelTreeItemClicked(QTreeWidgetItem *,int);
	void deselectSlot();
	void selectAllSlot();
	void itemSelectChangedSlot();
	void meshTreePressSlot(const QModelIndex &);

	void changeListSize(bool);
	
protected:
	void keyPressEvent(QKeyEvent *);
	void mousePressEvent(QMouseEvent *);

private:
	MainWindow *mw_table;
	Ui::Object_List *ui;
};

