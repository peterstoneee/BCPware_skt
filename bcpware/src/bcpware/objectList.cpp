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

#include "objectList.h"
#include "mainwindow.h"


ModelTreeWidgetItem::ModelTreeWidgetItem(MeshModel *meshModel, QTreeWidget* tree)
{
	setTextAlignment(0, Qt::AlignLeft);

	//if (meshModel->getMeshSort() != MeshModel::meshsort::slice_item)
	//{
	setText(0, QString::number(meshModel->id()));
	QString meshName = meshModel->label();
	if (meshModel->meshModified())
		meshName += " ";
	setText(1, meshName);
	mItem = meshModel;
	//}

}
ModelTreeWidgetItem::~ModelTreeWidgetItem()
{

}

ObjectList::ObjectList(QWidget *parent) :QDockWidget(parent), ui(new Ui::Object_List), myTreeWidget(new MyTreeWidget(parent))
{
	ui->setupUi(this);
	//setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::SubWindow);
	init();

	mw_table = qobject_cast<MainWindow *>(parent);
	//this->setFixedWidth(350);
	//this->setMinimumSize(QSize(321, 418));
	//this->setMaximumSize(QSize(321, 418));

	/*ui->toggleTreeWidget->setCheckable(true);
	connect(ui->toggleTreeWidget, SIGNAL(clicked(bool)), this, SLOT(setTreeWidgetVisible(bool)));*/

	connect(myTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(modelTreeItemClicked(QTreeWidgetItem *, int)));
	connect(myTreeWidget, SIGNAL(itemSelectionChanged()), SLOT(itemSelectChangedSlot()));
	connect(myTreeWidget, SIGNAL(pressed(const QModelIndex &)), this, SLOT(meshTreePressSlot(const QModelIndex)));
	connect(myTreeWidget, SIGNAL(clearall()), this, SLOT(deselectSlot()));

	connect(this, SIGNAL(topLevelChanged(bool)), this, SLOT(changeListSize(bool)));


	connect(ui->deselect_Btn, SIGNAL(clicked()), this, SLOT(deselectSlot()));
	connect(ui->selectAllBtn, SIGNAL(clicked()), this, SLOT(selectAllSlot()));


	myTreeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);


	qDebug() << "columnCount" << myTreeWidget->columnCount();
}
void ObjectList::init()
{
	QGridLayout *temp = new QGridLayout;
	temp->addWidget(myTreeWidget);
	ui->testFrame->setLayout(temp);

	QDesktopWidget widget;
	QRect mainScreenSize = widget.availableGeometry(widget.primaryScreen());

	//SYDNY 08/07/2017
	int reso_W = mainScreenSize.width();
	int reso_H = mainScreenSize.height();

	int obj_Height = reso_H - 133;
	int testframe_Height = reso_H - 207;

	this->setFixedHeight(obj_Height);
	ui->testFrame->setFixedHeight(testframe_Height);
}
void ObjectList::modelTreeItemClicked(QTreeWidgetItem *item, int col)
{
	md_table = mw_table->meshDoc();
	//md_table->multiSelectID.clear();
	ModelTreeWidgetItem *modelItem = dynamic_cast<ModelTreeWidgetItem *>(item);
	//qDebug() << "modelItem->checkState(col)" << modelItem->checkState(col);
	qDebug() << "modelItem->checkState(col)" << modelItem->isSelected();
	if (modelItem && QApplication::keyboardModifiers() == Qt::ControlModifier)
	{
		if (modelItem->isSelected())
		{
			//md_table->setCurrent(modelItem->mItem);//emit other signal ,cause deselect
			md_table->multiSelectID.insert(modelItem->mItem->id());
		}
		else md_table->multiSelectID.remove(modelItem->mItem->id());

	}
	else if (modelItem && QApplication::keyboardModifiers() == Qt::ShiftModifier)
	{
		QList<QTreeWidgetItem *> modelItemList = myTreeWidget->selectedItems();
		//qDebug() << "itemSelectChangedSlot====" << modelItemList.size();
		if (modelItemList.size() != 0)
		{
			foreach(QTreeWidgetItem *item, modelItemList)
			{
				ModelTreeWidgetItem *modelItem = dynamic_cast<ModelTreeWidgetItem *>(item);
				md_table->multiSelectID.insert(modelItem->mItem->id());
			}
		}

	}
	else if (modelItem)
	{
		//deselectSlot();		
		md_table->multiSelectID.clear();
		//int x = modelItem->mItem->id();
		md_table->setCurrentMesh(modelItem->mItem->id());//cause scroll problem
		//md_table->setCurrent(modelItem->mItem);//emit other signal ,cause deselect
		md_table->multiSelectID.insert(modelItem->mItem->id());

	}
	//else
	//{
	//	qDebug() << "no itemitem";
	//}

	updateLabel();
}
void ObjectList::selectAllSlot()
{
	/*foreach(MeshModel *sm, md()->meshList){
	if (sm->getMeshSort() == MeshModel::meshsort::print_item)
	md()->multiSelectID.insert(sm->id());
	}*/
	myTreeWidget->selectAll();
	updateLabel();
	mw_table->updateMenus();
}
void ObjectList::updateLabel()
{
	qDebug()<<"updateLabel:" << (myTreeWidget->selectedItems()).size();
	qDebug()<<"updateLabel:" << myTreeWidget->topLevelItemCount();
	ui->selectLabel->setText(QString("%1 / %2").arg((myTreeWidget->selectedItems()).size()).arg(myTreeWidget->topLevelItemCount()));
}
void ObjectList::updateTable()
{

	myTreeWidget->clear();
	myTreeWidget->setColumnCount(2);
	myTreeWidget->setColumnWidth(0, 50);
	myTreeWidget->setColumnWidth(1, 40);
	myTreeWidget->setHeaderHidden(true);

	md_table = mw_table->meshDoc();
	int count = 0;
	foreach(MeshModel *mmT, md_table->meshList)
	{
		ModelTreeWidgetItem *item = new ModelTreeWidgetItem(mmT, myTreeWidget);

		if (mmT->getMeshSort() != MeshModel::meshsort::slice_item)
		{
			count++;
			item->setText(0, QString::number(count));
			myTreeWidget->addTopLevelItem(item);

			//qDebug() << "md_table->multiSelectID" << md_table->multiSelectID.size();
			if (md_table->multiSelectID.contains(mmT->id()))
			{
				item->setSelected(true);

			}
			else
			{
				item->setSelected(false);

			}

			//qDebug() << "updateTable->checkState(col)" << item->isSelected();
		}
	}

	mw_table->updateMenuForCommand();


	//ui->tableView->setColumnWidth(0, 40);
	//ui->tableView->setColumnWidth(1, 40);
	////ui->meshTreeWidget->setColumnWidth(2,40);
	//ui->tableView->header()->hide();
}
void ObjectList::setTreeWidgetVisible(bool bs)
{
	if (bs)
	{
		myTreeWidget->hide();
		//this->setFixedHeight(50);
	}
	else {
		myTreeWidget->show();
		//this->setFixedHeight(600);
	}
}
void ObjectList::mousePressEvent(QMouseEvent *mouseEvent)
{


}
void ObjectList::meshTreePressSlot(const QModelIndex &index)
{
	qDebug() << "meshTreePressSlot====";//

}
void ObjectList::itemSelectChangedSlot()
{

	QList<QTreeWidgetItem *> modelItemList = myTreeWidget->selectedItems();
	//qDebug() << "itemSelectChangedSlot====" << modelItemList.size();
	if (modelItemList.size() != 0)
	{
		foreach(QTreeWidgetItem *item, modelItemList)
		{
			ModelTreeWidgetItem *modelItem = dynamic_cast<ModelTreeWidgetItem *>(item);
			md_table->multiSelectID.insert(modelItem->mItem->id());
		}
	}
	qDebug() << "itemSelectChangedSlot:" << (myTreeWidget->selectedItems()).size();
	qDebug() << "itemSelectChangedSlot:" << myTreeWidget->topLevelItemCount();
	//updateLabel();
	if (mw_table)mw_table->updateMenuForCommand();
}
//
void ObjectList::deselectSlot()
{
	if (mw_table) {
		myTreeWidget->clearSelection();
		md_table = mw_table->meshDoc();
		md_table->multiSelectID.clear();
		updateLabel();
		mw_table->updateMenus();
	}
}
void ObjectList::keyPressEvent(QKeyEvent *event)
{
	md_table = mw_table->meshDoc();
	//if (event->key() == Qt::Key_Space)
	//mw_table->meshDoc()->advanceCurrentRaster(1);
	if (event->key() == Qt::Key_Delete)
	{
		mw_table->delMeshes();
		updateTable();
		foreach(QTreeWidgetItem *item, myTreeWidget->selectedItems())
		{
			//myTreeWidget->removeItemWidget(item,);
		}
	}

	if (event->matches(QKeySequence::Copy))
	{
		if (md_table->multiSelectID.size() > 0)
			mw_table->setCopyMeshes();
	}

	if (event->matches(QKeySequence::Paste))
	{

		if (!md_table->copiedMeshList.isEmpty())
		{
			mw_table->pasteSelectMeshes();
		}
	}

}


//SYDNY 05/15/2017
void ObjectList::closeEvent(QCloseEvent *event)
{
	event->accept();
	mw_table->showObjectList(false);
}

//SYDNY 08/07/2017
void ObjectList::changeListSize(bool visible)
{
	QDesktopWidget widget;
	QRect mainScreenSize = widget.availableGeometry(widget.primaryScreen());

	int reso_W = mainScreenSize.width();
	int reso_H = mainScreenSize.height();

	if (visible)
	{
		this->setFixedHeight(425);
		ui->testFrame->setFixedHeight(360);
	}
	else
	{
		int obj_Height = reso_H - 133;
		int testframe_Height = reso_H - 207;

		this->setFixedHeight(obj_Height);
		ui->testFrame->setFixedHeight(testframe_Height);
	}
}

void ObjectList::updateUI(bool activeDoc)
{
	//ui->toggleTreeWidget->setEnabled(activeDoc);
	ui->selectAllBtn->setEnabled(activeDoc);
	ui->deselect_Btn->setEnabled(activeDoc);
	ui->testFrame->setEnabled(activeDoc);
	ui->selectLabel->setVisible(activeDoc);
	qDebug() << "updateUI:" << (myTreeWidget->selectedItems()).size();
	qDebug() << "updateUI:" << myTreeWidget->topLevelItemCount();
	updateLabel();

	foreach(MeshModel *mmT, md_table->meshList)
	{
		this->setMaximumHeight((md_table->meshList.count()) * 19);
	}
}



ObjectList::~ObjectList()
{
}


MyTreeWidget::MyTreeWidget(QWidget *parent) :QTreeWidget(parent)
{

}
void MyTreeWidget::mousePressEvent(QMouseEvent *event)
{
	QModelIndex item = indexAt(event->pos());
	this->setAutoScroll(false);
	QTreeView::mousePressEvent(event);
	if (!item.isValid())
	{
		clearSelection();
		emit clearall();
	}
}
MyTreeWidget::~MyTreeWidget() {}
