/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2008                                          \/)\/    *
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

#include "stdpardialog.h"

#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QFileDialog>
#include <QDialogButtonBox>
#include <QClipboard>
#include <QColorDialog>
#include <QToolBar>
#include <QButtonGroup>
#include "mainwindow.h"


using namespace vcg;
MeshlabStdDialog::MeshlabStdDialog(QWidget *p)
:QDockWidget(QString("Plugin"), p), curmask(MeshModel::MM_UNKNOWN)
{
	qf = NULL;
	stdParFrame = NULL;
	clearValues();
}

StdParFrame::StdParFrame(QWidget *p, QWidget *curr_gla)
:QFrame(p)
{
	gla = curr_gla;
}


/* manages the setup of the standard parameter window, when the execution of a plugin filter is requested */
bool MeshlabStdDialog::showAutoDialog(MeshFilterInterface *mfi, MeshModel *mm, MeshDocument * mdp, QAction *action, MainWindowInterface *mwi, QWidget *gla)
{
	validcache = false;
	curAction = action;
	curmfi = mfi;
	curmwi = mwi;
	curParSet.clear();
	prevParSet.clear();
	curModel = mm;
	curMeshDoc = mdp;
	curgla = gla;

	mfi->initParameterSet(action, *mdp, curParSet);//interface連結到filter實作的initParameterSet
	curmask = mfi->postCondition(action);
	if (curParSet.isEmpty() && !isPreviewable()) return false;

	createFrame();
	loadFrameContent(mdp);
	//if(1)
	if (isPreviewable())//預覽applyfilter之後的結果
	{
		meshState.create(curmask, curModel);
		connect(stdParFrame, SIGNAL(parameterChanged()), this, SLOT(applyDynamic()));
	}
	qDebug("isPreviewable");
	connect(curMeshDoc, SIGNAL(currentMeshChanged(int)), this, SLOT(changeCurrentMesh(int)));
	//***20150730***按下applybutton時會多執行一次//把產生圖片屏蔽調
	//if (curmfi->filterName(curAction) != "Generate_Slice_Pic" && curmfi->filterName(curAction) != "All_AutoLanding")
	//{
	//	connect(stdParFrame, SIGNAL(parameterChanged()), this, SLOT(applyClick()));//***新增，當參數變化時就apply
	//}

	if (curmfi->filterName(curAction) == "Slice_Tool")
	{
		connect(stdParFrame, SIGNAL(parameterChanged()), this, SLOT(applyClick()));//***新增，當參數變化時就apply
		//connect(stdParFrame, SIGNAL(parameterChanged()), applyButton, SIGNAL(clicked()));//***新增，當參數變化時就apply
	}
	/*QWidget *temp = static_cast<QWidget *>(gla->parent());
	this->move(temp->width()*0.8, gla->height()*0.4);*/
	this->move(this->pos().x() + 600, this->pos().y());

	raise();
	activateWindow();



	//***20150428***重開一次時就更新參數
	resetValues();
	//*****
	return true;
}

void MeshlabStdDialog::changeCurrentMesh(int meshInd)
{
	if (isPreviewable())
	{
		meshState.apply(curModel);
		curModel = curMeshDoc->getMesh(meshInd);
		meshState.create(curmask, curModel);
		applyDynamic();
	}
}

bool MeshlabStdDialog::isPreviewable()
{
	if ((curmask == MeshModel::MM_UNKNOWN) || (curmask == MeshModel::MM_NONE))
		return false;

	if ((curmask & MeshModel::MM_VERTNUMBER) ||
		(curmask & MeshModel::MM_FACENUMBER))
		return false;

	return true;
}


void MeshlabStdDialog::clearValues()
{
	curAction = NULL;
	curModel = NULL;
	curmfi = NULL;
	curmwi = NULL;
}

void MeshlabStdDialog::createFrame()
{
	if (qf) delete qf;

	QFrame *newqf = new QFrame(this);
	setWidget(newqf);
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	qf = newqf;
}

// update the values of the widgets with the values in the paramlist;
void MeshlabStdDialog::resetValues()
{
	curParSet.clear();
	curmfi->initParameterSet(curAction, *curMeshDoc, curParSet);

	assert(qf);
	assert(qf->isVisible());
	// assert(curParSet.paramList.count() == stdfieldwidgets.count());
	stdParFrame->resetValues(curParSet);
}

void StdParFrame::resetValues(RichParameterSet &curParSet)
{
	QList<RichParameter*> &parList = curParSet.paramList;
	assert(stdfieldwidgets.size() == parList.size());
	for (int i = 0; i < parList.count(); i++)
	{
		RichParameter* fpi = parList.at(i);
		if (fpi != NULL)
			stdfieldwidgets[i]->resetValue();
	}
}

/* creates widgets for the standard parameters */

void MeshlabStdDialog::loadFrameContent(MeshDocument *mdPt)
{
	assert(qf);
	qf->hide();

	QGridLayout *gridLayout = new QGridLayout(qf);

	setWindowTitle(curmfi->filterName(curAction));
	QLabel *ql = new QLabel("<i>" + curmfi->filterInfo(curAction) + "</i>", qf);
	ql->setTextFormat(Qt::RichText);
	ql->setWordWrap(true);
	gridLayout->addWidget(ql, 0, 0, 1, 2, Qt::AlignTop); // this widgets spans over two columns.

	stdParFrame = new StdParFrame(this, curgla);
	stdParFrame->loadFrameContent(curParSet, mdPt);
	gridLayout->addWidget(stdParFrame, 1, 0, 1, 2);

	int buttonRow = 2;  // the row where the line of buttons start

	QPushButton *helpButton = new QPushButton("Help", qf);
	QPushButton *closeButton = new QPushButton("Close", qf);
	applyButton = new QPushButton("Apply", qf);
	QPushButton *defaultButton = new QPushButton("Default", qf);
	QLabel *show_height = new QLabel("test", qf);
	applyButton->setFocus();
#ifdef Q_OS_MAC
	// Hack needed on mac for correct sizes of button in the bottom of the dialog.
	helpButton->setMinimumSize(100, 25);
	closeButton->setMinimumSize(100,25);
	applyButton->setMinimumSize(100,25);
	defaultButton->setMinimumSize(100, 25);
#endif

	if (isPreviewable())
	{
		previewCB = new QCheckBox("Preview", qf);
		previewCB->setCheckState(Qt::Unchecked);
		gridLayout->addWidget(previewCB, buttonRow + 0, 0, Qt::AlignBottom);
		connect(previewCB, SIGNAL(toggled(bool)), this, SLOT(togglePreview()));
		buttonRow++;
	}

	gridLayout->addWidget(helpButton, buttonRow + 0, 1, Qt::AlignBottom);
	gridLayout->addWidget(defaultButton, buttonRow + 0, 0, Qt::AlignBottom);
	gridLayout->addWidget(closeButton, buttonRow + 1, 0, Qt::AlignBottom);
	gridLayout->addWidget(applyButton, buttonRow + 1, 1, Qt::AlignBottom);
	gridLayout->addWidget(show_height, buttonRow + 2, 1, Qt::AlignBottom);

	connect(helpButton, SIGNAL(clicked()), this, SLOT(toggleHelp()));
	connect(applyButton, SIGNAL(clicked()), this, SLOT(applyClick()));
	connect(closeButton, SIGNAL(clicked()), this, SLOT(closeClick()));
	connect(defaultButton, SIGNAL(clicked()), this, SLOT(resetValues()));

	qf->showNormal();
	qf->adjustSize();

	//set the minimum size so it will shrink down to the right size	after the help is toggled
	this->setMinimumSize(qf->sizeHint());
	this->showNormal();
	this->adjustSize();
}

void StdParFrame::loadFrameContent(RichParameterSet &curParSet, MeshDocument * /*_mdPt*/)
{
	if (layout()) delete layout();
	QGridLayout* glay = new QGridLayout();
	// QGridLayout *vlayout = new QGridLayout(this);
	//    vLayout->setAlignment(Qt::AlignTop);
	//glay->setAlignment(Qt::AlignTop);
	RichWidgetInterfaceConstructor rwc(this);//***繼承visitor
	for (int i = 0; i < curParSet.paramList.count(); i++)
	{
		RichParameter* fpi = curParSet.paramList.at(i);//***
		fpi->accept(rwc);//***丟給RichWidgetInterfaceConstructor 去實作widget，用動態連結
		//vLayout->addWidget(rwc.lastCreated,i,0,1,1,Qt::AlignTop);
		stdfieldwidgets.push_back(rwc.lastCreated);
		helpList.push_back(rwc.lastCreated->helpLab);
		//glay->addItem(rwc.lastCreated->leftItem(),i,0);
		//glay->addItem(rwc.lastCreated->centralItem(),i,1);
		//glay->addItem(rwc.lastCreated->rightItem(),i,2);
		//rwc.lastCreated->addWidgetToGridLayout(glay,i);//***20150907
		if (i < 3)rwc.lastCreated->addWidgetToGridLayout_v2(glay, i, 0);//***20150907
		else rwc.lastCreated->addWidgetToGridLayout_v2(glay, i - 3, 3);

	} // end for each parameter
	setLayout(glay);
	this->setMinimumSize(glay->sizeHint());

	this->showNormal();
	this->adjustSize();
}
//***20150603
void StdParFrame::loadFrameContent_v2(RichParameterSet &curParSet, MeshDocument * /*_mdPt*/)
{
	if (layout()) delete layout();
	QGridLayout* glay = new QGridLayout();


	// QGridLayout *vlayout = new QGridLayout(this);
	//    vLayout->setAlignment(Qt::AlignTop);
	//glay->setAlignment(Qt::AlignTop);
	RichWidgetInterfaceConstructor rwc(this);//***繼承visitor
	for (int i = 0; i < curParSet.paramList.count(); i++)
	{
		RichParameter* fpi = curParSet.paramList.at(i);//***把parmeter的set拿出來
		fpi->accept(rwc);//***在這裡創造widget，用動態連結
		//vLayout->addWidget(rwc.lastCreated,i,0,1,1,Qt::AlignTop);
		stdfieldwidgets.push_back(rwc.lastCreated);
		helpList.push_back(rwc.lastCreated->helpLab);
		//glay->addItem(rwc.lastCreated->leftItem(),i,0);
		//glay->addItem(rwc.lastCreated->centralItem(),i,1);
		//glay->addItem(rwc.lastCreated->rightItem(),i,2);
		//rwc.lastCreated->addWidgetToGridLayout(glay, i);

		//***20150904***
		if (i < 3)rwc.lastCreated->addWidgetToGridLayout_v2(glay, i, 0);
		else rwc.lastCreated->addWidgetToGridLayout_v2(glay, i - 3, 3);


	} // end for each parameter
	setLayout(glay);
	//this->setMinimumSize(20, 20);
	this->showNormal();
	this->adjustSize();
}

void StdParFrame::toggleHelp()
{
	for (int i = 0; i < helpList.count(); i++)
		helpList.at(i)->setVisible(!helpList.at(i)->isVisible());
	updateGeometry();
	adjustSize();
}

void MeshlabStdDialog::toggleHelp()
{
	stdParFrame->toggleHelp();
	qf->updateGeometry();
	qf->adjustSize();
	this->updateGeometry();
	this->adjustSize();
}

//void StdParFrame::readValues(ParameterDeclarationSet &curParSet)
void StdParFrame::readValues(RichParameterSet &curParSet)
{
	QList<RichParameter*> &parList = curParSet.paramList;
	assert(parList.size() == stdfieldwidgets.size());
	QVector<MeshLabWidget*>::iterator it = stdfieldwidgets.begin();
	for (int i = 0; i < parList.count(); i++)
	{
		QString sname = parList.at(i)->name;
		curParSet.setValue(sname, (*it)->getWidgetValue());
		++it;
	}
}

StdParFrame::~StdParFrame()
{

}

/* click event for the apply button of the standard plugin window */
// If the filter has some dynamic parameters
// - before applying the filter restore the original state of the mesh.
// - after applying the filter reget the state of the mesh.

void MeshlabStdDialog::applyClick()
{
	//================
	/*QProgressDialog progress("Task in progress...", "Cancel", 0, 10000, this);
	progress.setWindowModality(Qt::WindowModal);
	progress.setValue(1);*/
	//=================
	qDebug() << "applyClick ";
	QAction *q = curAction;
	stdParFrame->readValues(curParSet);

	// Note that curModel CAN BE NULL (for creation filters on empty docs...)
	if (curmask && curModel)
		meshState.apply(curModel);

	//PreView Caching: if the apply parameters are the same to those used in the preview mode
	//we don't need to reapply the filter to the mesh
	bool isEqual = (curParSet == prevParSet);
	if (curModel && (isEqual) && (validcache))
		meshCacheState.apply(curModel);//
	else
		curmwi->executeFilter(q, curParSet, false);//執行filter mainwindow實作的execute filter dialog

	if (curmask && curModel)
		meshState.create(curmask, curModel);
	if (this->curgla)
		this->curgla->update();

}


void MeshlabStdDialog::applyDynamic()
{
	if (!previewCB->isChecked())
		return;
	QAction *q = curAction;
	stdParFrame->readValues(curParSet);
	//for cache mechanism
	//needed to allocate the required memory space in prevParSet
	//it called the operator=(RichParameterSet) function defined in RichParameterSet
	prevParSet = curParSet;
	stdParFrame->readValues(prevParSet);
	// Restore the
	meshState.apply(curModel);
	curmwi->executeFilter(q, curParSet, true);
	meshCacheState.create(curmask, curModel);
	validcache = true;


	if (this->curgla)
		this->curgla->update();
}

void MeshlabStdDialog::togglePreview()
{
	if (previewCB->isChecked())
	{
		stdParFrame->readValues(curParSet);
		if (!prevParSet.isEmpty() && (validcache) && (curParSet == prevParSet))
			meshCacheState.apply(curModel);
		else
			applyDynamic();
	}
	else
		meshState.apply(curModel);

	curgla->update();
}

/* click event for the close button of the standard plugin window */

void MeshlabStdDialog::closeClick()
{
	//int mask = 0;//curParSet.get();
	if (curmask != MeshModel::MM_UNKNOWN)
		meshState.apply(curModel);
	curmask = MeshModel::MM_UNKNOWN;
	// Perform the update only if there is Valid GLarea.
	if (this->curgla)
		this->curgla->update();
	close();
}


// click event for the standard red crossed close button in the upper right widget's corner
void MeshlabStdDialog::closeEvent(QCloseEvent * /*event*/)
{
	closeClick();
}

MeshlabStdDialog::~MeshlabStdDialog()
{
	delete stdParFrame;
	if (isPreviewable())
		delete previewCB;
}


/******************************************/
// AbsPercWidget Implementation
/******************************************/


//QGridLayout(NULL)
AbsPercWidget::AbsPercWidget(QWidget *p, RichAbsPerc* rabs) :MeshLabWidget(p, rabs)

{
	AbsPercDecoration* absd = reinterpret_cast<AbsPercDecoration*>(rp->pd);
	m_min = absd->min;
	m_max = absd->max;

	fieldDesc = new QLabel(rp->pd->fieldDesc + " (abs and %)", this);
	fieldDesc->setToolTip(rp->pd->tooltip);
	absSB = new QDoubleSpinBox(this);
	percSB = new QDoubleSpinBox(this);

	absSB->setMinimum(m_min - (m_max - m_min));
	absSB->setMaximum(m_max * 2);
	absSB->setAlignment(Qt::AlignRight);

	int decimals = 7 - ceil(log10(fabs(m_max - m_min)));
	//qDebug("range is (%f %f) %f ",m_max,m_min,fabs(m_max-m_min));
	//qDebug("log range is %f ",log10(fabs(m_max-m_min)));
	absSB->setDecimals(decimals);
	absSB->setSingleStep((m_max - m_min) / 100.0);
	float initVal = rp->val->getAbsPerc();
	absSB->setValue(initVal);

	percSB->setMinimum(-200);
	percSB->setMaximum(200);
	percSB->setAlignment(Qt::AlignRight);
	percSB->setSingleStep(0.5);
	percSB->setValue((100 * (initVal - m_min)) / (m_max - m_min));
	percSB->setDecimals(3);
	QLabel *absLab = new QLabel("<i> <small> world unit</small></i>");
	QLabel *percLab = new QLabel("<i> <small> perc on" + QString("(%1 .. %2)").arg(m_min).arg(m_max) + "</small></i>");

	//  gridLay->addWidget(fieldDesc,row,0,Qt::AlignHCenter);

	vlay = new QGridLayout();
	vlay->addWidget(absLab, 0, 0, Qt::AlignHCenter);
	vlay->addWidget(percLab, 0, 1, Qt::AlignHCenter);

	vlay->addWidget(absSB, 1, 0, Qt::AlignTop);
	vlay->addWidget(percSB, 1, 1, Qt::AlignTop);

	//gridLay->addLayout(lay,row,1,Qt::AlignTop);

	connect(absSB, SIGNAL(valueChanged(double)), this, SLOT(on_absSB_valueChanged(double)));
	connect(percSB, SIGNAL(valueChanged(double)), this, SLOT(on_percSB_valueChanged(double)));
	connect(this, SIGNAL(dialogParamChanged()), p, SIGNAL(parameterChanged()));
}

AbsPercWidget::~AbsPercWidget()
{
	delete absSB;
	delete percSB;
	delete fieldDesc;
}


void AbsPercWidget::on_absSB_valueChanged(double newv)
{
	disconnect(percSB, SIGNAL(valueChanged(double)), this, SLOT(on_percSB_valueChanged(double)));
	percSB->setValue((100 * (newv - m_min)) / (m_max - m_min));
	connect(percSB, SIGNAL(valueChanged(double)), this, SLOT(on_percSB_valueChanged(double)));
	emit dialogParamChanged();
}

void AbsPercWidget::on_percSB_valueChanged(double newv)
{
	disconnect(absSB, SIGNAL(valueChanged(double)), this, SLOT(on_absSB_valueChanged(double)));
	absSB->setValue((m_max - m_min)*0.01*newv + m_min);
	connect(absSB, SIGNAL(valueChanged(double)), this, SLOT(on_absSB_valueChanged(double)));
	emit dialogParamChanged();
}

//float AbsPercWidget::getValue()
//{
//	return float(absSB->value());
//}

void AbsPercWidget::setValue(float val, float minV, float maxV)
{
	assert(absSB);
	absSB->setValue(val);
	m_min = minV;
	m_max = maxV;
}

void AbsPercWidget::collectWidgetValue()
{
	rp->val->set(AbsPercValue(float(absSB->value())));
}

void AbsPercWidget::resetWidgetValue()
{
	const AbsPercDecoration* absd = reinterpret_cast<const AbsPercDecoration*>(&(rp->pd));
	setValue(rp->pd->defVal->getAbsPerc(), absd->min, absd->max);
}

void AbsPercWidget::setWidgetValue(const Value& nv)
{
	const AbsPercDecoration* absd = reinterpret_cast<const AbsPercDecoration*>(&(rp->pd));
	setValue(nv.getAbsPerc(), absd->min, absd->max);
}

void AbsPercWidget::addWidgetToGridLayout(QGridLayout* lay, const int r)
{
	if (lay != NULL)
	{
		lay->addWidget(fieldDesc, r, 0, Qt::AlignLeft);
		lay->addLayout(vlay, r, 1, Qt::AlignTop);
	}
	MeshLabWidget::addWidgetToGridLayout(lay, r);
}
/******************************************/
// Point3fWidget Implementation
/******************************************/


//QHBoxLayout(NULL)
Point3fWidget::Point3fWidget(QWidget *p, RichPoint3f* rpf, QWidget *gla_curr) : MeshLabWidget(p, rpf)
{
	//qDebug("Creating a Point3fWidget");
	paramName = rpf->name;
	//int row = gridLay->rowCount() - 1;
	descLab = new QLabel(rpf->pd->fieldDesc, this);
	descLab->setToolTip(rpf->pd->fieldDesc);
	//gridLay->addWidget(descLab,row,0);

	vlay = new QHBoxLayout();
	vlay->setSpacing(0);
	for (int i = 0; i < 3; ++i)
	{
		coordSB[i] = new QLineEdit(this);
		QFont baseFont = coordSB[i]->font();
		if (baseFont.pixelSize() != -1) baseFont.setPixelSize(baseFont.pixelSize() * 3 / 4);
		else baseFont.setPointSize(baseFont.pointSize() * 3 / 4);
		coordSB[i]->setFont(baseFont);
		coordSB[i]->setMaximumWidth(coordSB[i]->sizeHint().width() / 2);
		coordSB[i]->setValidator(new QDoubleValidator());
		coordSB[i]->setAlignment(Qt::AlignRight);
		coordSB[i]->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
		vlay->addWidget(coordSB[i]);
		connect(coordSB[i], SIGNAL(textChanged(QString)), p, SIGNAL(parameterChanged()));
	}
	this->setValue(paramName, rp->val->getPoint3f());
	if (gla_curr) // if we have a connection to the current glarea we can setup the additional button for getting the current view direction.
	{
		getPoint3Button = new QPushButton("Get", this);
		getPoint3Button->setMaximumWidth(getPoint3Button->sizeHint().width() / 2);

		getPoint3Button->setFlat(true);
		getPoint3Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
		//getPoint3Button->setMinimumWidth(getPoint3Button->sizeHint().width());
		//this->addWidget(getPoint3Button,0,Qt::AlignHCenter);
		vlay->addWidget(getPoint3Button);
		QStringList names;
		names << "View Dir";
		names << "View Pos";
		names << "Surf. Pos";
		names << "Camera Pos";

		getPoint3Combo = new QComboBox(this);
		getPoint3Combo->addItems(names);
		//getPoint3Combo->setMinimumWidth(getPoint3Combo->sizeHint().width());
		//this->addWidget(getPoint3Combo,0,Qt::AlignHCenter);
		vlay->addWidget(getPoint3Combo);

		connect(getPoint3Button, SIGNAL(clicked()), this, SLOT(getPoint()));
		connect(getPoint3Combo, SIGNAL(currentIndexChanged(int)), this, SLOT(getPoint()));
		connect(gla_curr, SIGNAL(transmitViewDir(QString, vcg::Point3f)), this, SLOT(setValue(QString, vcg::Point3f)));
		connect(gla_curr, SIGNAL(transmitShot(QString, vcg::Shotf)), this, SLOT(setShotValue(QString, vcg::Shotf)));
		connect(gla_curr, SIGNAL(transmitSurfacePos(QString, vcg::Point3f)), this, SLOT(setValue(QString, vcg::Point3f)));
		connect(this, SIGNAL(askViewDir(QString)), gla_curr, SLOT(sendViewDir(QString)));
		connect(this, SIGNAL(askViewPos(QString)), gla_curr, SLOT(sendMeshShot(QString)));
		connect(this, SIGNAL(askSurfacePos(QString)), gla_curr, SLOT(sendSurfacePos(QString)));
		connect(this, SIGNAL(askCameraPos(QString)), gla_curr, SLOT(sendCameraPos(QString)));
	}
	//gridLay->addLayout(lay,row,1,Qt::AlignTop);
}

void Point3fWidget::getPoint()
{
	int index = getPoint3Combo->currentIndex();
	//qDebug("Got signal %i",index);
	switch (index)
	{
	case 0: emit askViewDir(paramName);		 break;
	case 1: emit askViewPos(paramName);		 break;
	case 2: emit askSurfacePos(paramName); break;
	case 3: emit askCameraPos(paramName); break;
	default: assert(0);
	}
}

Point3fWidget::~Point3fWidget() {
	//qDebug("Deallocating a point3fwidget");
	this->disconnect();
}

void Point3fWidget::setValue(QString name, Point3f newVal)
{
	//qDebug("setValue parametername: %s ",qPrintable(name));
	if (name == paramName)
	{
		for (int i = 0; i < 3; ++i)
			coordSB[i]->setText(QString::number(newVal[i], 'g', 4));
	}
}

void Point3fWidget::setShotValue(QString name, Shotf newValShot)
{
	Point3f p = newValShot.GetViewPoint();
	setValue(name, p);
}

vcg::Point3f Point3fWidget::getValue()
{
	return Point3f(coordSB[0]->text().toFloat(), coordSB[1]->text().toFloat(), coordSB[2]->text().toFloat());
}

void Point3fWidget::collectWidgetValue()
{
	rp->val->set(Point3fValue(vcg::Point3f(coordSB[0]->text().toFloat(), coordSB[1]->text().toFloat(), coordSB[2]->text().toFloat())));
}

void Point3fWidget::resetWidgetValue()
{
	for (unsigned int ii = 0; ii < 3; ++ii)
		coordSB[ii]->setText(QString::number(rp->pd->defVal->getPoint3f()[ii], 'g', 3));
}

void Point3fWidget::setWidgetValue(const Value& nv)
{
	for (unsigned int ii = 0; ii < 3; ++ii)
		coordSB[ii]->setText(QString::number(nv.getPoint3f()[ii], 'g', 3));
}

void Point3fWidget::addWidgetToGridLayout(QGridLayout* lay, const int r)
{
	if (lay != NULL)
	{
		lay->addWidget(descLab, r, 0);
		lay->addLayout(vlay, r, 1);
	}
	MeshLabWidget::addWidgetToGridLayout(lay, r);
}
/******************************************/
// Matrix44fWidget Implementation
/******************************************/


//QHBoxLayout(NULL)
Matrix44fWidget::Matrix44fWidget(QWidget *p, RichMatrix44f* rpf, QWidget *gla_curr) : MeshLabWidget(p, rpf)
{

	paramName = rpf->name;
	//int row = gridLay->rowCount() - 1;

	descLab = new QLabel(rpf->pd->fieldDesc, this);
	descLab->setToolTip(rpf->pd->fieldDesc);
	//gridLay->addWidget(descLab,row,0,Qt::AlignTop);
	vlay = new QVBoxLayout();
	lay44 = new QGridLayout();



	for (int i = 0; i < 16; ++i)
	{
		coordSB[i] = new QLineEdit(p);
		QFont baseFont = coordSB[i]->font();
		if (baseFont.pixelSize() != -1) baseFont.setPixelSize(baseFont.pixelSize() * 3 / 4);
		else baseFont.setPointSize(baseFont.pointSize() * 3 / 4);
		coordSB[i]->setFont(baseFont);
		//coordSB[i]->setMinimumWidth(coordSB[i]->sizeHint().width()/4);
		coordSB[i]->setMinimumWidth(0);
		coordSB[i]->setMaximumWidth(coordSB[i]->sizeHint().width() / 2);
		//coordSB[i]->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
		coordSB[i]->setValidator(new QDoubleValidator(p));
		coordSB[i]->setAlignment(Qt::AlignRight);
		//this->addWidget(coordSB[i],1,Qt::AlignHCenter);
		lay44->addWidget(coordSB[i], i / 4, i % 4);

	}
	this->setValue(paramName, rp->val->getMatrix44f());

	vlay->addLayout(lay44);

	QPushButton     * getMatrixButton = new QPushButton("Read from current layer");
	vlay->addWidget(getMatrixButton);

	QPushButton     * pasteMatrixButton = new QPushButton("Paste from clipboard");
	vlay->addWidget(pasteMatrixButton);

	//gridLay->addLayout(vlay,row,1,Qt::AlignTop);

	connect(gla_curr, SIGNAL(transmitMatrix(QString, vcg::Matrix44f)), this, SLOT(setValue(QString, vcg::Matrix44f)));
	connect(getMatrixButton, SIGNAL(clicked()), this, SLOT(getMatrix()));
	connect(pasteMatrixButton, SIGNAL(clicked()), this, SLOT(pasteMatrix()));
	connect(this, SIGNAL(askMeshMatrix(QString)), gla_curr, SLOT(sendMeshMatrix(QString)));

}


Matrix44fWidget::~Matrix44fWidget() {}

void Matrix44fWidget::setValue(QString name, Matrix44f newVal)
{
	if (name == paramName)
	{
		for (int i = 0; i < 16; ++i)
			coordSB[i]->setText(QString::number(newVal[i / 4][i % 4], 'g', 4));
	}
}


vcg::Matrix44f Matrix44fWidget::getValue()
{
	float val[16];
	for (unsigned int i = 0; i < 16; ++i)
		val[i] = coordSB[i]->text().toFloat();
	return Matrix44f(val);
}

void Matrix44fWidget::getMatrix(){

	emit askMeshMatrix(QString("TransformMatrix"));
}

void Matrix44fWidget::pasteMatrix(){
	QClipboard *clipboard = QApplication::clipboard();
	QString shotString = clipboard->text();
	QStringList list1 = shotString.split(" ");
	if (list1.size() != 16) return;
	int id = 0;
	for (QStringList::iterator i = list1.begin(); i != list1.end(); ++i, ++id){
		bool ok = true;
		(*i).toFloat(&ok);
		if (!ok) return;
	}
	id = 0;
	for (QStringList::iterator i = list1.begin(); i != list1.end(); ++i, ++id)
		coordSB[id]->setText(*i);
}

void Matrix44fWidget::collectWidgetValue()
{
	vcg::Matrix44f  m;
	for (unsigned int i = 0; i < 16; ++i) m[i / 4][i % 4] = coordSB[i]->text().toFloat();
	rp->val->set(Matrix44fValue(m));
}

void Matrix44fWidget::resetWidgetValue()
{
	vcg::Matrix44f  m; m.SetIdentity();
	for (unsigned int ii = 0; ii < 16; ++ii)
		coordSB[ii]->setText(QString::number(rp->pd->defVal->getMatrix44f()[ii / 4][ii % 4], 'g', 3));
}

void Matrix44fWidget::setWidgetValue(const Value& nv)
{
	for (unsigned int ii = 0; ii < 16; ++ii)
		coordSB[ii]->setText(QString::number(nv.getMatrix44f()[ii / 4][ii % 4], 'g', 3));
}

void Matrix44fWidget::addWidgetToGridLayout(QGridLayout* lay, const int r)
{
	if (lay != NULL)
	{
		lay->addWidget(descLab, r, 0, Qt::AlignTop);
		lay->addLayout(vlay, r, 1, Qt::AlignTop);
	}
	MeshLabWidget::addWidgetToGridLayout(lay, r);
}
/********************/
// ShotfWidget Implementation

ShotfWidget::ShotfWidget(QWidget *p, RichShotf* rpf, QWidget *gla_curr) : MeshLabWidget(p, rpf)
{

	paramName = rpf->name;
	//int row = gridLay->rowCount() - 1;

	descLab = new QLabel(rpf->pd->fieldDesc, p);
	descLab->setToolTip(rpf->pd->fieldDesc);
	//gridLay->addWidget(descLab,row,0,Qt::AlignTop);

	hlay = new QHBoxLayout();


	this->setShotValue(paramName, rp->val->getShotf());
	if (gla_curr) // if we have a connection to the current glarea we can setup the additional button for getting the current view direction.
	{
		getShotButton = new QPushButton("Get shot", this);
		getShotButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
		hlay->addWidget(getShotButton);

		QStringList names;
		names << "Current Trackball";
		names << "Current Mesh";
		names << "Current Raster";
		names << "From File";

		getShotCombo = new QComboBox(this);
		getShotCombo->addItems(names);
		hlay->addWidget(getShotCombo);
		connect(getShotCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(getShot()));
		connect(getShotButton, SIGNAL(clicked()), this, SLOT(getShot()));
		connect(gla_curr, SIGNAL(transmitShot(QString, vcg::Shotf)), this, SLOT(setShotValue(QString, vcg::Shotf)));
		connect(this, SIGNAL(askViewerShot(QString)), gla_curr, SLOT(sendViewerShot(QString)));
		connect(this, SIGNAL(askMeshShot(QString)), gla_curr, SLOT(sendMeshShot(QString)));
		connect(this, SIGNAL(askRasterShot(QString)), gla_curr, SLOT(sendRasterShot(QString)));
	}
	//gridLay->addLayout(hlay,row,1,Qt::AlignTop);
}

void ShotfWidget::getShot()
{
	int index = getShotCombo->currentIndex();
	switch (index)  {
	case 0: emit askViewerShot(paramName); break;
	case 1: emit askMeshShot(paramName); break;
	case 2: emit askRasterShot(paramName); break;
	case 3:
	{
			  QString filename = QFileDialog::getOpenFileName(this, tr("Load xml camera"), "./", tr("Xml Files (*.xml)"));
			  QFile qf(filename);
			  QFileInfo qfInfo(filename);

			  if (!qf.open(QIODevice::ReadOnly))
				  return;

			  QDomDocument doc("XmlDocument");    //It represents the XML document
			  if (!doc.setContent(&qf))     return;
			  qf.close();

			  QString type = doc.doctype().name();

			  //TextAlign file project
			  //if(type == "RegProjectML")   loadShotFromTextAlignFile(doc);
			  //View State file
			  //else if(type == "ViewState") loadViewFromViewStateFile(doc);

			  //qDebug("End file reading");


			  // return true;
	}
	default: assert(0);
	}
}

ShotfWidget::~ShotfWidget() {}

void ShotfWidget::setShotValue(QString name, Shotf newVal)
{
	if (name == paramName)
	{
		curShot = newVal;
	}
}

vcg::Shotf ShotfWidget::getValue()
{
	return curShot;
}

void ShotfWidget::collectWidgetValue()
{
	rp->val->set(ShotfValue(curShot));
}

void ShotfWidget::resetWidgetValue()
{
	curShot = rp->pd->defVal->getShotf();
}

void ShotfWidget::setWidgetValue(const Value& nv)
{
	curShot = nv.getShotf();
}

void ShotfWidget::addWidgetToGridLayout(QGridLayout* lay, const int r)
{
	if (lay != NULL)
	{
		lay->addLayout(hlay, r, 1);
		lay->addWidget(descLab, r, 0);
	}
	MeshLabWidget::addWidgetToGridLayout(lay, r);
}
/********************/
// ComboWidget End Implementation

ComboWidget::ComboWidget(QWidget *p, RichParameter* rpar) :MeshLabWidget(p, rpar) {
}

void ComboWidget::Init(QWidget *p, int defaultEnum, QStringList values)
{
	enumLabel = new QLabel(this);
	enumLabel->setText(rp->pd->fieldDesc);
	enumCombo = new QComboBox(this);
	enumCombo->addItems(values);
	setIndex(defaultEnum);
	//int row = gridLay->rowCount() - 1;
	//gridLay->addWidget(enumLabel,row,0,Qt::AlignTop);
	//gridLay->addWidget(enumCombo,row,1,Qt::AlignTop);
	connect(enumCombo, SIGNAL(activated(int)), this, SIGNAL(dialogParamChanged()));
	connect(this, SIGNAL(dialogParamChanged()), p, SIGNAL(parameterChanged()));
}

void ComboWidget::setIndex(int newEnum)
{
	enumCombo->setCurrentIndex(newEnum);
}

int ComboWidget::getIndex()
{
	return enumCombo->currentIndex();
}

ComboWidget::~ComboWidget()
{
	delete enumCombo;
	delete enumLabel;
}

void ComboWidget::addWidgetToGridLayout(QGridLayout* lay, const int r)
{
	if (lay != NULL)
	{
		lay->addWidget(enumLabel, r, 0);
		lay->addWidget(enumCombo, r, 1);
	}
	MeshLabWidget::addWidgetToGridLayout(lay, r);
}
void ComboWidget::addWidgetToGridLayout_v2(QGridLayout* lay, const int x, const int y)
{
	if (lay != NULL)
	{
		lay->addWidget(enumLabel, x, y);
		lay->addWidget(enumCombo, x, y + 1);
	}
	MeshLabWidget::addWidgetToGridLayout_v2(lay, x, y);
}
/******************************************/
//EnumWidget Implementation
/******************************************/

EnumWidget::EnumWidget(QWidget *p, RichEnum* rpar)
:ComboWidget(p, rpar)
{
	//you MUST call it!!!!
	Init(p, rpar->val->getEnum(), reinterpret_cast<EnumDecoration*>(rpar->pd)->enumvalues);
	//assert(enumCombo != NULL);
}

int EnumWidget::getSize()
{
	return enumCombo->count();
}


void EnumWidget::collectWidgetValue()
{
	rp->val->set(EnumValue(enumCombo->currentIndex()));
}

void EnumWidget::resetWidgetValue()
{
	//lned->setText(QString::number(rp->val->getFloat(),'g',3));
	enumCombo->setCurrentIndex(rp->pd->defVal->getEnum());
}

void EnumWidget::setWidgetValue(const Value& nv)
{
	enumCombo->setCurrentIndex(nv.getEnum());
}

/******************************************/
//MeshEnumWidget Implementation
/******************************************/


MeshWidget::MeshWidget(QWidget *p, RichMesh* rpar)
:ComboWidget(p, rpar)
{
	md = reinterpret_cast<MeshDecoration*>(rp->pd)->meshdoc;

	QStringList meshNames;

	//make the default mesh Index be 0
	//defaultMeshIndex = -1;

	int currentmeshindex = -1;
	for (int i = 0; i < md->meshList.size(); ++i)
	{
		QString shortName = md->meshList.at(i)->label();
		meshNames.push_back(shortName);
		/*  if(md->meshList.at(i) == rp->pd->defVal->getMesh())
			  defaultMeshIndex = i;*/
		if (md->meshList.at(i) == rp->val->getMesh())
		{
			currentmeshindex = i;
			rpar->meshindex = currentmeshindex;
		}
	}

	//add a blank choice because there is no default available
	//if(defaultMeshIndex == -1)
	//{
	//    meshNames.push_back("");
	//    defaultMeshIndex = meshNames.size()-1;  //have the blank choice be shown
	//}
	Init(p, currentmeshindex, meshNames);
}

MeshModel * MeshWidget::getMesh()
{
	//test to make sure index is in bounds
	int index = enumCombo->currentIndex();
	if (index < md->meshList.size() && index > -1)
	{
		//RichMesh* rm = reinterpret_cast<RichMesh*>(rp);
		//rm->meshindex = index;
		return md->meshList.at(enumCombo->currentIndex());
	}
	else return NULL;
}

void MeshWidget::setMesh(MeshModel * newMesh)
{
	for (int i = 0; i < md->meshList.size(); ++i)
	{
		if (md->meshList.at(i) == newMesh)
			setIndex(i);
	}
}

void MeshWidget::collectWidgetValue()
{
	//MeshDecoration* dec = reinterpret_cast<MeshDecoration*>(rp->pd);
	RichMesh* rm = reinterpret_cast<RichMesh*>(rp);
	rm->meshindex = enumCombo->currentIndex();
	rp->val->set(MeshValue(md->meshList.at(rm->meshindex)));
}

void MeshWidget::resetWidgetValue()
{
	int meshindex = -1;
	for (int i = 0; i < md->meshList.size(); ++i)
	{
		if (md->meshList.at(i) == rp->pd->defVal->getMesh())
		{
			meshindex = i;
			//RichMesh* rm = reinterpret_cast<RichMesh*>(rp);
			//rm->meshindex = enumCombo->currentIndex();
		}

	}
	enumCombo->setCurrentIndex(meshindex);
}

void MeshWidget::setWidgetValue(const Value& nv)
{
	//WARNING!!!!! I HAVE TO THINK CAREFULLY ABOUT THIS FUNCTION!!!
	//assert(0);
	int meshindex = -1;
	for (int i = 0; i < md->meshList.size(); ++i)
	{
		if (md->meshList.at(i) == nv.getMesh())
		{
			meshindex = i;
			//RichMesh* rm = reinterpret_cast<RichMesh*>(rp);
			//rm->meshindex = meshindex;
		}
	}
	enumCombo->setCurrentIndex(meshindex);
}

/******************************************
QVariantListWidget Implementation
******************************************/

/*
QVariantListWidget::QVariantListWidget(QWidget *parent, QList<QVariant> &values)
{
tableWidget = new QTableWidget(parent);
tableWidget->setColumnCount(1);
tableWidget->setRowCount(values.size() );
tableWidget->setMaximumSize(135, 180);
tableWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
tableWidget->horizontalHeader()->setDisabled(true);

setList(values);

this->addWidget(tableWidget);
}

QList<QVariant> QVariantListWidget::getList()
{
QList<QVariant> values;

for(int i = 0; i < tableWidget->rowCount(); i++)
{
QString rowValue = tableWidget->item(i,0)->text();

//if the row was not blank
if("" != rowValue) values.push_back(QVariant(rowValue));
}

return values;
}


void QVariantListWidget::setList(QList<QVariant> &values)
{
tableWidget->clearContents();
tableWidget->setRowCount(values.size() );
for(int i = 0; i < values.size(); i++)
{
tableWidget->setItem(i, 0, new QTableWidgetItem(values.at(i).toString()));
}
}

void QVariantListWidget::addRow()
{
int count = tableWidget->rowCount();

//insert a new row at the end
tableWidget->insertRow(count);

//fill row with something so that it can be removed if left blank... qt forces us to do this
tableWidget->setItem(count, 0, new QTableWidgetItem(""));
}


void QVariantListWidget::removeRow()
{
QList<QTableWidgetItem *> items = tableWidget->selectedItems();

if(items.size() == 0){
//remove the last row if none are selected
tableWidget->removeRow(tableWidget->rowCount()-1);
} else
{
//remove each selected row
for(int i = 0; i < items.size(); i++){
tableWidget->removeRow(items.at(i)->row());
}
}
}
*/

/******************************************
GetFileNameButton Implementation
******************************************/

/*
GetFileNameWidget::GetFileNameWidget(QWidget *parent,
QString &defaultString, bool getOpenFileName, QString fileExtension) : QVBoxLayout(parent)
{
_fileName = defaultString;
_fileExtension = fileExtension;

_getOpenFileName = getOpenFileName;

launchFileNameDialogButton = new QPushButton(QString("Get FileName"), parent);
fileNameLabel = new QLabel(_fileName, parent);

addWidget(launchFileNameDialogButton);
addWidget(fileNameLabel);

connect(launchFileNameDialogButton, SIGNAL(clicked()), this, SLOT(launchGetFileNameDialog()));

}

GetFileNameWidget::~GetFileNameWidget()
{
delete launchFileNameDialogButton;
}

QString GetFileNameWidget::getFileName()
{
return _fileName;
}

void GetFileNameWidget::launchGetFileNameDialog()
{
QString temp;
if(_getOpenFileName){
temp = QFileDialog::getOpenFileName(parentWidget(), tr("Select File"),".", "*"+_fileExtension);
} else
temp = QFileDialog::getSaveFileName(parentWidget(), tr("Select File"),".", "*"+_fileExtension);

if(temp != ""){
_fileName = temp;
fileNameLabel->setText(QFileInfo(_fileName).fileName());
}
}

void GetFileNameWidget::setFileName(QString newName){
_fileName = newName;
}
*/

GenericParamDialog::GenericParamDialog(QWidget *p, RichParameterSet *_curParSet, QString title, MeshDocument *_meshDocument)
: QDialog(p) {
	stdParFrame = NULL;
	curParSet = _curParSet;
	meshDocument = _meshDocument;
	createFrame();
	if (!title.isEmpty())
		setWindowTitle(title);
}


// update the values of the widgets with the values in the paramlist;
void GenericParamDialog::resetValues()
{
	stdParFrame->resetValues(*curParSet);
}

void GenericParamDialog::toggleHelp()
{
	stdParFrame->toggleHelp();
	this->updateGeometry();
	this->adjustSize();
}


void GenericParamDialog::createFrame()
{
	QVBoxLayout *vboxLayout = new QVBoxLayout(this);
	setLayout(vboxLayout);

	stdParFrame = new StdParFrame(this);
	stdParFrame->loadFrameContent(*curParSet, meshDocument);
	layout()->addWidget(stdParFrame);

	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Help | QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	//add the reset button so we can get its signals
	QPushButton *resetButton = buttonBox->addButton(QDialogButtonBox::Reset);
	layout()->addWidget(buttonBox);

	connect(buttonBox, SIGNAL(accepted()), this, SLOT(getAccept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	connect(buttonBox, SIGNAL(helpRequested()), this, SLOT(toggleHelp()));
	connect(resetButton, SIGNAL(clicked()), this, SLOT(resetValues()));

	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

	//set the minimum size so it will shrink down to the right size	after the help is toggled
	this->setMinimumSize(stdParFrame->sizeHint());
	this->showNormal();
	this->adjustSize();
}


void GenericParamDialog::getAccept()
{
	stdParFrame->readValues(*curParSet);
	accept();
}

GenericParamDialog::~GenericParamDialog()
{
	delete stdParFrame;
}

/******************************************/
// GenericParamGroupbox BOXBOXBOXBOXBOXBOX
/******************************************/
GenericParamGroupbox::GenericParamGroupbox(QAction *editAction, MeshEditInterFace_v2 *mediv2, QWidget *p, QString title, MeshDocument *_meshDocument, QWidget *gla)
: QGroupBox(p) {
	stdParFrame = NULL;
	curParSet.clear();
	curgla = 0;
	curmwi = 0;
	enterButton = 0;
	currentAction = editAction;
	meshDocument = _meshDocument;
	editIF2 = mediv2;

	setStyleSheet("QGroupBox{	background-color: qlineargradient(x1 : 0, y1 : 0, x2 : 0, y2 : 1,	stop : 0 #E0E0E0, stop: 1 #FFFFFF);}"
		"QGroupBox{ border: 1px solid gray;}"
		"QGroupBox{border-radius: 5px;}"//***邊角的弧度
		"QGroupBox{margin-bottom: 2ex; /* leave space at the top for the title */}"
		"QGroupBox::title{subcontrol-origin: margin; subcontrol-position: bottom center;padding: 0 3px;	background-color: qlineargradient(x1 : 0, y1 : 0, x2 : 0, y2 : 1,stop : 0 #FFAECE, stop: 1 #FFFFFF);}"
		);
	if (!title.isEmpty())
		setTitle(title);
	//qDebug() << "genGroupBox";
	mediv2->initParameterSet(editAction, *meshDocument, curParSet);//***_curParSet目前無用
	createFrame();


	/*if (!title.isEmpty())
		setWindowTitle(title);*/
}


// update the values of the widgets with the values in the paramlist;
void GenericParamGroupbox::resetValues()
{
	stdParFrame->resetValues(curParSet);
}

void GenericParamGroupbox::toggleHelp()
{
	stdParFrame->toggleHelp();
	this->updateGeometry();
	this->adjustSize();
}


void GenericParamGroupbox::createFrame()
{


	QHBoxLayout *hboxLayout = new QHBoxLayout(this);


	//QVBoxLayout *vboxLayout = new QVBoxLayout();	
	setLayout(hboxLayout);

	stdParFrame = new StdParFrame(this, curgla);
	stdParFrame->loadFrameContent_v2(curParSet, meshDocument);

	layout()->addWidget(stdParFrame);

	//QButtonGroup
	//QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Help | QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	//add the reset button so we can get its signals
	//QPushButton *resetButton = buttonBox->addButton(QDialogButtonBox::Reset);
	//layout()->addWidget(buttonBox);

	//connect(buttonBox, SIGNAL(accepted()), this, SLOT(getAccept()));
	//connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	//connect(buttonBox, SIGNAL(helpRequested()), this, SLOT(toggleHelp()));
	//connect(resetButton, SIGNAL(clicked()), this, SLOT(resetValues()));

	//***20150907
	QVBoxLayout *vboxLayout = new QVBoxLayout();
	QButtonGroup *bg = new QButtonGroup(this);
	enterButton = new QPushButton("Enter");
	//setStyleSheet(" QPushButton{background - color: red;		border - style: outset;		border - width: 2px;		border - color: beige;	} ");
	//***20150904**增加resetbutton
	QPushButton *resetButton = new QPushButton("reset");//***20150907

	bg->addButton(enterButton);
	bg->addButton(resetButton);
	vboxLayout->addWidget(enterButton);
	vboxLayout->addWidget(resetButton);
	QFrame *vGroupBoxGroupBox = new QFrame();
	vGroupBoxGroupBox->setLayout(vboxLayout);

	connect(enterButton, SIGNAL(clicked()), this, SLOT(boxApplyEdit_v2()));
	connect(resetButton, SIGNAL(clicked()), this, SLOT(resetValues()));

	//connect(stdParFrame, SIGNAL(parameterChanged()), enterButton, SIGNAL(clicked()));

	//***20151013***
	if (editIF2->filterName(currentAction) != "edit_rotate")
	{
		connect(stdParFrame, SIGNAL(parameterChanged()), this, SLOT(boxApplyEdit_v2()));//***新增，當參數變化時就apply	
	}
	//connect();

	layout()->addWidget(vGroupBoxGroupBox);

	//QPushButton pb = new QPushButton("Translate");

	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

	//set the minimum size so it will shrink down to the right size	after the help is toggled
	this->setMinimumSize(stdParFrame->sizeHint());
	this->showNormal();
	this->adjustSize();
}


void GenericParamGroupbox::getAccept()
{
	stdParFrame->readValues(curParSet);
	//accept();
}
void GenericParamGroupbox::boxApplyEdit_v2()
{
	/* MainWindow *testwin = reinterpret_cast<MainWindow*>(this->parent()->parent());
	 testwin->GLA()->update();*/
	//GLArea *test = reinterpret_cast<GLArea *> (curgla);
	//enterButton->setAutoDefault(true);

	QAction *q = currentAction;
	stdParFrame->readValues(curParSet);
	curmwi->executeEdit(currentAction, curParSet);
	curgla->update();

}

GenericParamGroupbox::~GenericParamGroupbox()
{
	delete stdParFrame;

}

/******************************************/
// End GenericParamGroupbox Implementation
/******************************************/


/******************************************/
// DynamicFloatWidget Implementation
/******************************************/


//QGridLayout(NULL)
DynamicFloatWidget::DynamicFloatWidget(QWidget *p, RichDynamicFloat* rdf) :MeshLabWidget(p, rdf)
{
	int numbdecimaldigit = 4;
	minVal = reinterpret_cast<DynamicFloatDecoration*>(rdf->pd)->min;
	maxVal = reinterpret_cast<DynamicFloatDecoration*>(rdf->pd)->max;
	valueLE = new QLineEdit(this);
	valueLE->setAlignment(Qt::AlignRight);//20150313

	//int n_floor = (maxVal - minVal) * 200.;

	valueSlider = new QSlider(Qt::Vertical, this);//改為垂直
	valueSlider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	fieldDesc = new QLabel(rp->pd->fieldDesc, this);
	valueSlider->setMinimum(0);
	valueSlider->setMaximum(1000);
	valueSlider->setValue(floatToInt(rp->val->getFloat()));

	const DynamicFloatDecoration* dfd = reinterpret_cast<const DynamicFloatDecoration*>(&(rp->pd));
	QFontMetrics fm(valueLE->font());
	QSize sz = fm.size(Qt::TextSingleLine, QString::number(0));
	//valueLE->setValidator(new QDoubleValidator (dfd->min,dfd->max, numbdecimaldigit, valueLE));//有錯，dfd->min抓不到值
	//valueLE->setValidator(new QDoubleValidator(minVal, maxVal, numbdecimaldigit, valueLE));//***20150313
	valueLE->setValidator(new QIntValidator(-1000, 1000, valueLE));//20150313
	valueLE->setText(QString::number(rp->val->getFloat()));
	valueLE->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

	//int row = gridLay->rowCount() - 1;
	//lay->addWidget(fieldDesc,row,0);

	//***20150313*************
	valueSpinBox = new QSpinBox(this);
	valueSpinBox->setAlignment(Qt::AlignBottom);
	valueSpinBox->setRange(0, 1000);
	valueSpinBox->setSingleStep(1);
	//***************************

	hlay = new QHBoxLayout();
	hlay->addWidget(valueSpinBox);
	hlay->addWidget(valueLE);
	hlay->addWidget(valueSlider);

	int maxlenghtplusdot = 8;//numbmaxvaluedigit + numbdecimaldigit + 1;
	valueLE->setMaxLength(maxlenghtplusdot);
	valueLE->setMaximumWidth(sz.width() * maxlenghtplusdot);
	valueSlider->setMaximumHeight(200);//***沒有用


	//gridLay->addLayout(hlay,row,1);

	//connect(valueLE,SIGNAL(textChanged(const QString &)),this,SLOT(setValue()));	
	connect(valueSlider, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)));
	connect(this, SIGNAL(dialogParamChanged()), p, SIGNAL(parameterChanged()));//old

	//connect(valueSlider, SIGNAL(valueChanged(int)), this, SIGNAL(dialogParamChanged()));//20150313 即時產生顯示切層
	connect(valueLE, SIGNAL(editingFinished()), this, SLOT(setValue()));//20150313  修改成文字輸入完成觸發parameterChanged

	////***valueSpinBox
	connect(valueSpinBox, SIGNAL(editingFinished()), this, SLOT(setSliceValue()));//輸入完成後產生
	////connect(valueSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setSliceValue()));//spinbox一改變就產生一層
	connect(valueSlider, SIGNAL(valueChanged(int)), valueSpinBox, SLOT(setValue(int)));

	/*//connect(valueLE,SIGNAL(textChanged(const QString &)),this,SLOT(setValue()));
	connect(valueSlider,SIGNAL(valueChanged(int)),this,SLOT(setValue(int)));
	connect(this,SIGNAL(dialogParamChanged()),p,SIGNAL(parameterChanged()));//old
	//connect(valueSlider, SIGNAL(sliderReleased()), p, SIGNAL(parameterChanged()));//20150313  修改成放掉時觸發parameterChanged
	connect(valueSlider, SIGNAL(sliderReleased()), this, SIGNAL(dialogParamChanged()));//20150313  修改成放掉時觸發parameterChanged
	connect(valueSlider, SIGNAL(valueChanged(int)), p, SIGNAL(parameterChanged()));//20150313 即時產生一堆
	connect(valueLE, SIGNAL(editingFinished()), this, SLOT(setValue()));//20150313  修改成文字輸入完成觸發parameterChanged
	//***valueSpinBox
	connect(valueSpinBox, SIGNAL(editingFinished()), this, SLOT(setSliceValue()));//輸入完成後產生
	//connect(valueSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setSliceValue()));//spinbox一改變就產生一層
	connect(valueSlider, SIGNAL(valueChanged(int)),		valueSpinBox, SLOT(setValue(int)));
	*/
	//******************
}

DynamicFloatWidget::~DynamicFloatWidget()
{
	//***20150908
	delete valueLE;
	delete valueSlider;
	delete fieldDesc;
	delete valueSpinBox;
}

float DynamicFloatWidget::getValue()
{
	return float(valueLE->text().toDouble());
}

void DynamicFloatWidget::setValue(float  newVal)
{
	qDebug("setValue int float");
	if (QString::number(intToFloat(newVal)) != valueLE->text())
		valueLE->setText(QString::number(intToFloat(newVal), 'f', 2));
}

void DynamicFloatWidget::setValue(int  newVal)
{
	qDebug("setValue int");
	if (floatToInt(float(valueLE->text().toDouble())) != newVal)
	{
		valueLE->setText(QString::number(intToFloat(newVal), 'f', 2));
	}

	emit dialogParamChanged();
	//Q CoreApplication::processEvents();
}

void DynamicFloatWidget::setValue()
{
	float newValLE = float(valueLE->text().toDouble());
	//qDebug("newValle=%f",newValLE);
	qDebug("setValue");
	valueSlider->setValue(floatToInt(newValLE));//同步slider
	emit dialogParamChanged();
}
void DynamicFloatWidget::setSliceValue()
{
	qDebug("setValue2");
	valueSlider->setValue(valueSpinBox->value());//同步slider
	emit dialogParamChanged();
	//emit dialogParamChanged();
}
float DynamicFloatWidget::intToFloat(int val)
{
	return minVal + float(val) / 1000.0f*(maxVal - minVal);
}
int DynamicFloatWidget::floatToInt(float val)
{
	return int(1000.0f*(val - minVal) / (maxVal - minVal));
}

void DynamicFloatWidget::collectWidgetValue()
{
	rp->val->set(DynamicFloatValue(valueLE->text().toFloat()));
}

void DynamicFloatWidget::resetWidgetValue()
{
	valueLE->setText(QString::number(rp->pd->defVal->getFloat()));
}

void DynamicFloatWidget::setWidgetValue(const Value& nv)
{
	valueLE->setText(QString::number(nv.getFloat()));
}

void DynamicFloatWidget::addWidgetToGridLayout(QGridLayout* lay, const int r)
{
	if (lay != NULL)
	{
		lay->addWidget(fieldDesc, r, 0);
		lay->addLayout(hlay, r, 1);
	}
	MeshLabWidget::addWidgetToGridLayout(lay, r);
}
void DynamicFloatWidget::addWidgetToGridLayout_v2(QGridLayout* lay, const int x, const int y)
{
	if (lay != NULL)
	{
		lay->addWidget(fieldDesc, x, y);
		lay->addLayout(hlay, x, y + 1);
	}
	MeshLabWidget::addWidgetToGridLayout_v2(lay, x, y);
}

//**************************//
/******************************************/
// DynamicFloatWidget_S Implementation
/******************************************/


//QGridLayout(NULL)
DynamicFloatWidget_S::DynamicFloatWidget_S(QWidget *p, RichDynamicFloat_S* rdf) :MeshLabWidget(p, rdf)
{
	int numbdecimaldigit = 5;
	minVal = reinterpret_cast<DynamicFloatDecoration*>(rdf->pd)->min;
	maxVal = reinterpret_cast<DynamicFloatDecoration*>(rdf->pd)->max;
	n_floor = (maxVal - minVal) * 200.;

	valueLE = new QLineEdit(this);
	valueLE->setAlignment(Qt::AlignRight);//20150313

	valueSlider = new QSlider(Qt::Vertical, this);//改為垂直
	valueSlider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	fieldDesc = new QLabel(rp->pd->fieldDesc, this);
	valueSlider->setMinimum(0);
	valueSlider->setMaximum(n_floor);
	//valueSlider->setValue(floatToInt(rp->val->getFloat()));
	valueSlider->setValue(floatToInt(minVal));

	const DynamicFloatDecoration* dfd = reinterpret_cast<const DynamicFloatDecoration*>(&(rp->pd));
	QFontMetrics fm(valueLE->font());
	QSize sz = fm.size(Qt::TextSingleLine, QString::number(0));
	//valueLE->setValidator(new QDoubleValidator (dfd->min,dfd->max, numbdecimaldigit, valueLE));//有錯，dfd->min抓不到值
	//valueLE->setValidator(new QDoubleValidator(minVal, maxVal, numbdecimaldigit, valueLE));//***20150313
	//valueLE->setValidator(new QIntValidator(-10000, 10000, valueLE));//20150313
	valueLE->setValidator(new QDoubleValidator(-10000, 10000, numbdecimaldigit, valueLE));//20150313
	valueLE->setText(QString::number(rp->val->getFloat()));
	valueLE->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

	//int row = gridLay->rowCount() - 1;
	//lay->addWidget(fieldDesc,row,0);

	//***20150313*************
	valueSpinBox = new QSpinBox(this);
	valueSpinBox->setAlignment(Qt::AlignBottom);
	valueSpinBox->setRange(0, n_floor);
	valueSpinBox->setSingleStep(1);


	//***20151014
	spinTenBox = new QSpinBox(this);
	spinTenBox->setAlignment(Qt::AlignBottom);
	spinTenBox->setRange(0, n_floor);
	spinTenBox->setSingleStep(10);
	//***************************

	hlay = new QHBoxLayout();
	hlay->addWidget(valueSpinBox);
	hlay->addWidget(spinTenBox);
	hlay->addWidget(valueLE);
	hlay->addWidget(valueSlider);

	int maxlenghtplusdot = 8;//numbmaxvaluedigit + numbdecimaldigit + 1;
	valueLE->setMaxLength(maxlenghtplusdot);
	valueLE->setMaximumWidth(sz.width() * maxlenghtplusdot);
	valueSlider->setMaximumHeight(200);//***沒有用

	//gridLay->addLayout(hlay,row,1);
	connect(valueLE, SIGNAL(textChanged(const QString &)), this, SLOT(setValue()));
	connect(valueSlider, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)), Qt::DirectConnection);//set_value_LE	
	//connect(spinTenBox, SIGNAL(valueChanged(int)), this, SIGNAL(dialogParamChanged()), Qt::DirectConnection);//set_value_LE	
	connect(valueLE, SIGNAL(textChanged(const QString &)), this, SIGNAL(dialogParamChanged()), Qt::DirectConnection);

	//connect(valueLE, SIGNAL(editingFinished()), this, SLOT(setValue()), Qt::DirectConnection);//20150313  修改成文字輸入完成觸發parameterChanged	

	////***valueSpinBox
	//connect(valueSlider, SIGNAL(valueChanged(int)), spinTenBox, SLOT(setValue(int)), Qt::DirectConnection);
	//connect(spinTenBox, SIGNAL(valueChanged(int)), valueSpinBox, SLOT(setValue(int)), Qt::DirectConnection);//spinbox一改變就產生一層	
	//connect(valueSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setSliceValue()), Qt::DirectConnection);//spinbox一改變就產生一層

	connect(valueSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)), Qt::DirectConnection);//spinbox一改變就產生一層
	connect(spinTenBox, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)), Qt::DirectConnection);//spinbox一改變就產生一層


	connect(this, SIGNAL(dialogParamChanged()), p, SIGNAL(parameterChanged()), Qt::DirectConnection);//old



	//connect(valueSpinBox, SIGNAL(editingFinished()), this, SLOT(setSliceValue()));//輸入完成後產生
	//******************
}

DynamicFloatWidget_S::~DynamicFloatWidget_S()
{
	//***20150908
	delete valueLE;
	delete valueSlider;
	delete fieldDesc;
	delete valueSpinBox;
}

float DynamicFloatWidget_S::getValue()
{
	return float(valueLE->text().toDouble());
}

void DynamicFloatWidget_S::setValue(float  newVal)
{
	qDebug("setValue int float");
	if (QString::number(intToFloat(newVal)) != valueLE->text())
		valueLE->setText(QString::number(intToFloat(newVal), 'f', 3));
}

void DynamicFloatWidget_S::setValue(int  newVal)
{
	qDebug("setValue int");
	if (floatToInt(float(valueLE->text().toDouble())) != newVal)
	{
		valueLE->setText(QString::number(intToFloat(newVal), 'f', 3));
	}

	//emit dialogParamChanged();
	//Q CoreApplication::processEvents();
}

void DynamicFloatWidget_S::setValue()
{

	float newValLE = float(valueLE->text().toDouble());
	int temp = floatToInt(newValLE);//
	//qDebug("newValle=%f",newValLE);
	qDebug("setValue");

	if (valueSlider->value() != temp)
	{
		valueSlider->setValue(temp);//同步slider
	}
	if (valueSpinBox->value() != temp)
	{
		valueSpinBox->setValue(temp);//同步slider
	}
	if (spinTenBox->value() != temp)
	{
		spinTenBox->setValue(temp);//同步slider
	}
	//valueSlider->setValue(floatToInt(newValLE));//同步slider
	//valueSlider->setValue(temp);//同步slider
	//valueSpinBox->setValue(temp);
	//spinTenBox->setValue(temp);	

	//emit dialogParamChanged();
}
void DynamicFloatWidget_S::setSliceValue()
{
	qDebug("setValue2");
	valueSlider->setValue(valueSpinBox->value());//同步slider
	//emit dialogParamChanged();

}
float DynamicFloatWidget_S::intToFloat(int val)
{
	return minVal + float(val) / n_floor*(maxVal - minVal);
}
int DynamicFloatWidget_S::floatToInt(float val)
{
	float  dInput = n_floor*(val - minVal) / (maxVal - minVal);
	if (dInput >= 0.0f)
	{
		return ((int)(dInput + 0.5f));
	}

	return ((int)(dInput - 0.5f));

	//return int(n_floor*(val - minVal) / (maxVal - minVal));
}

void DynamicFloatWidget_S::collectWidgetValue()
{
	rp->val->set(DynamicFloatValue(valueLE->text().toFloat()));
	//rp->val->set(DynamicFloatValue(valueLE->text().toFloat()));
}

void DynamicFloatWidget_S::resetWidgetValue()
{
	valueLE->setText(QString::number(rp->pd->defVal->getFloat()));
}

void DynamicFloatWidget_S::setWidgetValue(const Value& nv)
{
	valueLE->setText(QString::number(nv.getFloat()));
}

void DynamicFloatWidget_S::addWidgetToGridLayout(QGridLayout* lay, const int r)
{
	if (lay != NULL)
	{
		lay->addWidget(fieldDesc, r, 0);
		lay->addLayout(hlay, r, 1);
	}
	MeshLabWidget::addWidgetToGridLayout(lay, r);
}
void DynamicFloatWidget_S::addWidgetToGridLayout_v2(QGridLayout* lay, const int x, const int y)
{
	if (lay != NULL)
	{
		lay->addWidget(fieldDesc, x, y);
		lay->addLayout(hlay, x, y + 1);
	}
	MeshLabWidget::addWidgetToGridLayout_v2(lay, x, y);
}


//***************************//


DynamicFloatWidget_SpinBox::DynamicFloatWidget_SpinBox(QWidget *p, RichDynamicFloat_spin* rdf) :MeshLabWidget(p, rdf)
{
	int numbdecimaldigit = 5;
	minVal = reinterpret_cast<DynamicFloatDecoration*>(rdf->pd)->min;
	maxVal = reinterpret_cast<DynamicFloatDecoration*>(rdf->pd)->max;
	n_floor = (maxVal - minVal) * 200.;

	valueLE = new QLineEdit(this);
	valueLE->setAlignment(Qt::AlignRight);//20150313

	valueSlider = new QSlider(Qt::Vertical, this);//改為垂直
	valueSlider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	fieldDesc = new QLabel(rp->pd->fieldDesc, this);
	valueSlider->setMinimum(0);
	valueSlider->setMaximum(n_floor);
	//valueSlider->setValue(floatToInt(rp->val->getFloat()));
	valueSlider->setValue(floatToInt(minVal));

	const DynamicFloatDecoration* dfd = reinterpret_cast<const DynamicFloatDecoration*>(&(rp->pd));
	QFontMetrics fm(valueLE->font());
	QSize sz = fm.size(Qt::TextSingleLine, QString::number(0));
	//valueLE->setValidator(new QDoubleValidator (dfd->min,dfd->max, numbdecimaldigit, valueLE));//有錯，dfd->min抓不到值
	//valueLE->setValidator(new QDoubleValidator(minVal, maxVal, numbdecimaldigit, valueLE));//***20150313
	//valueLE->setValidator(new QIntValidator(-10000, 10000, valueLE));//20150313
	valueLE->setValidator(new QDoubleValidator(-10000, 10000, numbdecimaldigit, valueLE));//20150313
	valueLE->setText(QString::number(rp->val->getFloat()));
	valueLE->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

	//int row = gridLay->rowCount() - 1;
	//lay->addWidget(fieldDesc,row,0);

	//***20150313*************
	valueSpinBox = new QSpinBox(this);
	valueSpinBox->setAlignment(Qt::AlignBottom);
	valueSpinBox->setRange(0, n_floor);
	valueSpinBox->setSingleStep(1);


	//***20151014
	spinTenBox = new QSpinBox(this);
	spinTenBox->setAlignment(Qt::AlignBottom);
	spinTenBox->setRange(0, n_floor);
	spinTenBox->setSingleStep(10);
	//***************************

	hlay = new QHBoxLayout();
	hlay->addWidget(valueSpinBox);
	hlay->addWidget(spinTenBox);
	hlay->addWidget(valueLE);
	hlay->addWidget(valueSlider);

	int maxlenghtplusdot = 8;//numbmaxvaluedigit + numbdecimaldigit + 1;
	valueLE->setMaxLength(maxlenghtplusdot);
	valueLE->setMaximumWidth(sz.width() * maxlenghtplusdot);
	valueSlider->setMaximumHeight(200);//***沒有用

	//gridLay->addLayout(hlay,row,1);
	connect(valueLE, SIGNAL(textChanged(const QString &)), this, SLOT(setValue()));
	connect(valueSlider, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)), Qt::DirectConnection);//set_value_LE	
	//connect(spinTenBox, SIGNAL(valueChanged(int)), this, SIGNAL(dialogParamChanged()), Qt::DirectConnection);//set_value_LE	
	connect(valueLE, SIGNAL(textChanged(const QString &)), this, SIGNAL(dialogParamChanged()), Qt::DirectConnection);

	//connect(valueLE, SIGNAL(editingFinished()), this, SLOT(setValue()), Qt::DirectConnection);//20150313  修改成文字輸入完成觸發parameterChanged	

	////***valueSpinBox
	//connect(valueSlider, SIGNAL(valueChanged(int)), spinTenBox, SLOT(setValue(int)), Qt::DirectConnection);
	//connect(spinTenBox, SIGNAL(valueChanged(int)), valueSpinBox, SLOT(setValue(int)), Qt::DirectConnection);//spinbox一改變就產生一層	
	//connect(valueSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setSliceValue()), Qt::DirectConnection);//spinbox一改變就產生一層

	connect(valueSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)), Qt::DirectConnection);//spinbox一改變就產生一層
	connect(spinTenBox, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)), Qt::DirectConnection);//spinbox一改變就產生一層


	connect(this, SIGNAL(dialogParamChanged()), p, SIGNAL(parameterChanged()), Qt::DirectConnection);//old



	//connect(valueSpinBox, SIGNAL(editingFinished()), this, SLOT(setSliceValue()));//輸入完成後產生
	//******************
}

DynamicFloatWidget_SpinBox::~DynamicFloatWidget_SpinBox()
{
	//***20150908
	delete valueLE;
	delete valueSlider;
	delete fieldDesc;
	delete valueSpinBox;
}

float DynamicFloatWidget_SpinBox::getValue()
{
	return float(valueLE->text().toDouble());
}

void DynamicFloatWidget_SpinBox::setValue(float  newVal)
{
	qDebug("setValue int float");
	if (QString::number(intToFloat(newVal)) != valueLE->text())
		valueLE->setText(QString::number(intToFloat(newVal), 'f', 3));
}

void DynamicFloatWidget_SpinBox::setValue(int  newVal)
{
	qDebug("setValue int");
	if (floatToInt(float(valueLE->text().toDouble())) != newVal)
	{
		valueLE->setText(QString::number(intToFloat(newVal), 'f', 3));
	}

	//emit dialogParamChanged();
	//Q CoreApplication::processEvents();
}

void DynamicFloatWidget_SpinBox::setValue()
{

	float newValLE = float(valueLE->text().toDouble());
	int temp = floatToInt(newValLE);//
	//qDebug("newValle=%f",newValLE);
	qDebug("setValue");

	if (valueSlider->value() != temp)
	{
		valueSlider->setValue(temp);//同步slider
	}
	if (valueSpinBox->value() != temp)
	{
		valueSpinBox->setValue(temp);//同步slider
	}
	if (spinTenBox->value() != temp)
	{
		spinTenBox->setValue(temp);//同步slider
	}
	//valueSlider->setValue(floatToInt(newValLE));//同步slider
	//valueSlider->setValue(temp);//同步slider
	//valueSpinBox->setValue(temp);
	//spinTenBox->setValue(temp);	

	//emit dialogParamChanged();
}
void DynamicFloatWidget_SpinBox::setSliceValue()
{
	qDebug("setValue2");
	valueSlider->setValue(valueSpinBox->value());//同步slider
	//emit dialogParamChanged();

}
float DynamicFloatWidget_SpinBox::intToFloat(int val)
{
	return minVal + float(val) / n_floor*(maxVal - minVal);
}
int DynamicFloatWidget_SpinBox::floatToInt(float val)
{
	float  dInput = n_floor*(val - minVal) / (maxVal - minVal);
	if (dInput >= 0.0f)
	{
		return ((int)(dInput + 0.5f));
	}

	return ((int)(dInput - 0.5f));

	//return int(n_floor*(val - minVal) / (maxVal - minVal));
}

void DynamicFloatWidget_SpinBox::collectWidgetValue()
{
	rp->val->set(DynamicFloatValue(valueLE->text().toFloat()));
	//rp->val->set(DynamicFloatValue(valueLE->text().toFloat()));
}

void DynamicFloatWidget_SpinBox::resetWidgetValue()
{
	valueLE->setText(QString::number(rp->pd->defVal->getFloat()));
}

void DynamicFloatWidget_SpinBox::setWidgetValue(const Value& nv)
{
	valueLE->setText(QString::number(nv.getFloat()));
}

void DynamicFloatWidget_SpinBox::addWidgetToGridLayout(QGridLayout* lay, const int r)
{
	if (lay != NULL)
	{
		lay->addWidget(fieldDesc, r, 0);
		lay->addLayout(hlay, r, 1);
	}
	MeshLabWidget::addWidgetToGridLayout(lay, r);
}
void DynamicFloatWidget_SpinBox::addWidgetToGridLayout_v2(QGridLayout* lay, const int x, const int y)
{
	if (lay != NULL)
	{
		lay->addWidget(fieldDesc, x, y);
		lay->addLayout(hlay, x, y + 1);
	}
	MeshLabWidget::addWidgetToGridLayout_v2(lay, x, y);
}


//***************************//






/****************************/
Value& MeshLabWidget::getWidgetValue()
{
	collectWidgetValue();
	return *(rp->val);
}


void MeshLabWidget::resetValue()
{
	rp->val->set(*rp->pd->defVal);
	resetWidgetValue();
}

MeshLabWidget::MeshLabWidget(QWidget* p, RichParameter* rpar)
:QWidget(p), rp(rpar) // this version of the line caused the very strange error of uncheckabe first bool widget
//:rp(rpar)
{
	//setParent(p);
	if (rp != NULL)
	{
		helpLab = new QLabel("<small>" + rpar->pd->tooltip + "</small>", this);
		helpLab->setTextFormat(Qt::RichText);
		helpLab->setWordWrap(true);
		helpLab->setVisible(false);
		helpLab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		helpLab->setMinimumWidth(250);
		helpLab->setMaximumWidth(QWIDGETSIZE_MAX);



		/*gridLay = qobject_cast<QGridLayout*>(p->layout());
		assert(gridLay != 0);
		row = gridLay->rowCount();*/
		//gridLay->addWidget(helpLab,row,3,1,1,Qt::AlignTop);
	}
}

void MeshLabWidget::InitRichParameter(RichParameter* rpar)
{
	rp = rpar;
}

MeshLabWidget::~MeshLabWidget()
{
	//delete rp;
	delete helpLab;
}

//void MeshLabWidget::setEqualSpaceForEachColumn()
//{
//	int singlewidth = gridLay->geometry().width() / gridLay->columnCount();
//	for (int ii = 0;ii < gridLay->columnCount();++ii)
//		gridLay->setColumnMinimumWidth(ii,singlewidth);
//}

void MeshLabWidget::addWidgetToGridLayout(QGridLayout* lay, const int r)
{
	if (lay != NULL)
		lay->addWidget(helpLab, r, 2, 1, 1);
}
void MeshLabWidget::addWidgetToGridLayout_v2(QGridLayout *lay, const int x, const int y)
{
	if (lay != NULL)
		lay->addWidget(helpLab, x, y, 1, 1);
}

//connect(qcb,SIGNAL(stateChanged(int)),this,SIGNAL(parameterChanged()));
BoolWidget::BoolWidget(QWidget* p, RichBool* rb)
:MeshLabWidget(p, rb)
{
	//cb = new QCheckBox(rp->pd->fieldDesc,this);
	cb = new QCheckBox(this);
	cb->setToolTip(rp->pd->tooltip);
	cb->setChecked(rp->val->getBool());
	//cb->setLayoutDirection(Qt::RightToLeft);

	//***20150904
	lab = new QLabel(rp->pd->fieldDesc, this);
	qDebug() <<"rp->pd->fieldDesc"<< rp->pd->fieldDesc;
	QFont font = lab->font();
	font.setPointSize(8);//set the font size here
	lab->setFont(font);
	lab->setBuddy(cb);
	//lab->setLayoutDirection(Qt::LeftToRight);
	//lab->setIndent(Qt::AlignLeft);
	//lab->setAlignment(Qt::AlignLeft);

	connect(cb, SIGNAL(stateChanged(int)), p, SIGNAL(parameterChanged()));
}

void BoolWidget::collectWidgetValue()
{
	rp->val->set(BoolValue(cb->isChecked()));
}

void BoolWidget::resetWidgetValue()
{
	cb->setChecked(rp->pd->defVal->getBool());
}

BoolWidget::~BoolWidget()
{
	delete lab;
	delete cb;
}

void BoolWidget::setWidgetValue(const Value& nv)
{
	cb->setChecked(nv.getBool());
}

void BoolWidget::addWidgetToGridLayout(QGridLayout* lay, const int r)
{
	if (lay != NULL){
		lay->addWidget(lab, r, 0);
		lay->addWidget(cb, r, 1);
		//lay->addWidget(cb, r, 0, 1, 2);
		//lay->addWidget(lab, r, 0, 1, 2);
	}
	MeshLabWidget::addWidgetToGridLayout(lay, r);
}
//***20150904
void BoolWidget::addWidgetToGridLayout_v2(QGridLayout* lay, const int x, const int y)
{
	if (lay != NULL)
	{
		lay->addWidget(cb, x, y, 1, 2);
		lay->addWidget(lab, x, y + 1, 1, 2);
	}
	MeshLabWidget::addWidgetToGridLayout_v2(lay, x, y);
}

//connect(qle,SIGNAL(editingFinished()),this,SIGNAL(parameterChanged()));
LineEditWidget::LineEditWidget(QWidget* p, RichParameter* rpar)
:MeshLabWidget(p, rpar)
{
	lab = new QLabel(rp->pd->fieldDesc, this);
	lned = new QLineEdit(this);
	lned->setValidator(new QDoubleValidator());
	//int row = gridLay->rowCount() -1;

	lab->setToolTip(rp->pd->tooltip);
	//gridLay->addWidget(lab,row,0);
	//gridLay->addWidget(lned,row,1);

	//connect(lned,SIGNAL(editingFinished()),this,SLOT(changeChecker()));
	//connect(this,SIGNAL(lineEditChanged()),p,SIGNAL(parameterChanged()));

	//***20150909***當在lineEdit按下enter時執行，先從label傳出來到lineWidget，在傳給stdparFrame
	connect(lned, SIGNAL(returnPressed()), this, SLOT(changeChecker()));
	connect(this, SIGNAL(lineEditChanged()), p, SIGNAL(parameterChanged()));


	lned->setAlignment(Qt::AlignLeft);
	lned->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	//lned->setFixedSize(20, 20);
	//***20150904
	QFont font = lab->font();
	font.setPointSize(8);//set the font size here
	lab->setFont(font);
}

LineEditWidget::~LineEditWidget()
{
	delete lned;
	delete lab;
}

void LineEditWidget::changeChecker()
{
	//***20150909
	/*if(lned->text() != this->lastVal)
	{*/
	this->lastVal = lned->text();
	if (!this->lastVal.isEmpty())
		emit lineEditChanged();
	//}
}

void LineEditWidget::addWidgetToGridLayout(QGridLayout* lay, const int r)
{
	if (lay != NULL)
	{
		lay->addWidget(lab, r, 0);
		lay->addWidget(lned, r, 1);
	}
	MeshLabWidget::addWidgetToGridLayout(lay, r);
}

void LineEditWidget::addWidgetToGridLayout_v2(QGridLayout* lay, const int x, const int y)
{
	if (lay != NULL)
	{
		lay->addWidget(lab, x, y);
		lay->addWidget(lned, x, y + 1);
	}
	MeshLabWidget::addWidgetToGridLayout_v2(lay, x, y);
}

IntWidget::IntWidget(QWidget* p, RichInt* rpar)
:LineEditWidget(p, rpar)
{
	lned->setText(QString::number(rp->val->getInt()));
}

void IntWidget::collectWidgetValue()
{
	rp->val->set(IntValue(lned->text().toInt()));
}

void IntWidget::resetWidgetValue()
{
	lned->setText(QString::number(rp->pd->defVal->getInt()));
}

void IntWidget::setWidgetValue(const Value& nv)
{
	lned->setText(QString::number(nv.getInt()));
}
//
FloatWidget::FloatWidget(QWidget* p, RichFloat* rpar)
:LineEditWidget(p, rpar)
{
	lned->setText(QString::number(rp->val->getFloat(), 'g', 4));
	//***20150609
	lned->setFixedSize(30, 18);
	//this->adjustSize();
	//connect(lned, SIGNAL(editingFinished()), p, SIGNAL((parameterChanged)));
	//***20150909
	//connect(lned, SIGNAL(returnPressed()), p, SIGNAL((parameterChanged)));

}

void FloatWidget::collectWidgetValue()
{
	rp->val->set(FloatValue(lned->text().toFloat()));
}

void FloatWidget::resetWidgetValue()
{
	lned->setText(QString::number(rp->pd->defVal->getFloat(), 'g', 3));
}

void FloatWidget::setWidgetValue(const Value& nv)
{
	lned->setText(QString::number(nv.getFloat(), 'g', 3));
}

StringWidget::StringWidget(QWidget* p, RichString* rpar)
:LineEditWidget(p, rpar)
{
	lned->setText(rp->val->getString());
}

void StringWidget::collectWidgetValue()
{
	rp->val->set(StringValue(lned->text()));
}

void StringWidget::resetWidgetValue()
{
	lned->setText(rp->pd->defVal->getString());
}

void StringWidget::setWidgetValue(const Value& nv)
{
	lned->setText(nv.getString());
}


/******************************************/
// ColorWidget Implementation
/******************************************/


ColorWidget::ColorWidget(QWidget *p, RichColor* newColor)
:MeshLabWidget(p, newColor), pickcol()
{
	colorLabel = new QLabel(this);
	descLabel = new QLabel(rp->pd->fieldDesc, this);
	colorButton = new QPushButton(this);
	colorButton->setAutoFillBackground(true);
	colorButton->setFlat(true);
	colorButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	//const QColor cl = rp->pd->defVal->getColor();
	//resetWidgetValue();
	initWidgetValue();
	//int row = gridLay->rowCount() - 1;
	//gridLay->addWidget(descLabel,row,0,Qt::AlignTop);

	vlay = new QHBoxLayout();
	QFontMetrics met(colorLabel->font());
	QColor black(Qt::black);
	QString blackname = "(" + black.name() + ")";
	QSize sz = met.size(Qt::TextSingleLine, blackname);
	colorLabel->setMaximumWidth(sz.width());
	colorLabel->setMinimumWidth(sz.width());
	vlay->addWidget(colorLabel, 0, Qt::AlignRight);
	vlay->addWidget(colorButton);


	//gridLay->addLayout(lay,row,1,Qt::AlignTop);
	pickcol = rp->val->getColor();
	connect(colorButton, SIGNAL(clicked()), this, SLOT(pickColor()));
	connect(this, SIGNAL(dialogParamChanged()), p, SIGNAL(parameterChanged()));
}

void ColorWidget::updateColorInfo(const ColorValue& newColor)
{
	QColor col = newColor.getColor();
	colorLabel->setText("(" + col.name() + ")");
	QPalette palette(col);
	colorButton->setPalette(palette);
}

void ColorWidget::pickColor()
{
	pickcol = QColorDialog::getColor(pickcol, this->parentWidget(), "Pick a Color", QColorDialog::DontUseNativeDialog | QColorDialog::ShowAlphaChannel);
	if (pickcol.isValid())
	{
		collectWidgetValue();
		updateColorInfo(ColorValue(pickcol));
	}
	emit dialogParamChanged();
}

void ColorWidget::initWidgetValue()
{
	QColor cl = rp->val->getColor();
	pickcol = cl;
	updateColorInfo(cl);
}

void ColorWidget::resetWidgetValue()
{
	QColor cl = rp->pd->defVal->getColor();
	pickcol = cl;
	updateColorInfo(cl);
}

void ColorWidget::collectWidgetValue()
{
	rp->val->set(ColorValue(pickcol));
}

ColorWidget::~ColorWidget()
{
	delete colorButton;
	delete colorLabel;
	delete descLabel;
}

void ColorWidget::setWidgetValue(const Value& nv)
{
	QColor cl = nv.getColor();
	pickcol = cl;
	updateColorInfo(cl);
}

void ColorWidget::addWidgetToGridLayout(QGridLayout* lay, const int r)
{
	if (lay != NULL)
	{
		lay->addWidget(descLabel, r, 0);
		lay->addLayout(vlay, r, 1);
	}
	MeshLabWidget::addWidgetToGridLayout(lay, r);
}
/*
void GetFileNameWidget::launchGetFileNameDialog()
{

}*/

void RichParameterToQTableWidgetItemConstructor::visit(RichBool& pd)
{
	if (pd.val->getBool())
		lastCreated = new QTableWidgetItem("true"/*,lst*/);
	else
		lastCreated = new QTableWidgetItem("false"/*,lst*/);

}

void RichParameterToQTableWidgetItemConstructor::visit(RichInt& pd)
{
	lastCreated = new QTableWidgetItem(QString::number(pd.val->getInt())/*,lst*/);
}

void RichParameterToQTableWidgetItemConstructor::visit(RichFloat& pd)
{
	lastCreated = new QTableWidgetItem(QString::number(pd.val->getFloat())/*,lst*/);
}

void RichParameterToQTableWidgetItemConstructor::visit(RichString& pd)
{
	lastCreated = new QTableWidgetItem(pd.val->getString()/*,lst*/);
}

void RichParameterToQTableWidgetItemConstructor::visit(RichPoint3f& pd)
{
	vcg::Point3f pp = pd.val->getPoint3f();
	QString pst = "P3(" + QString::number(pp.X()) + "," + QString::number(pp.Y()) + "," + QString::number(pp.Z()) + ")";
	lastCreated = new QTableWidgetItem(pst/*,lst*/);
}

void RichParameterToQTableWidgetItemConstructor::visit(RichShotf& /*pd*/)
{
	assert(0); ///
	lastCreated = new QTableWidgetItem(QString("TODO")/*,lst*/);
}

void RichParameterToQTableWidgetItemConstructor::visit(RichOpenFile& pd)
{
	lastCreated = new QTableWidgetItem(pd.val->getFileName()/*,lst*/);
}

void RichParameterToQTableWidgetItemConstructor::visit(RichColor& pd)
{
	QPixmap pix(10, 10);
	pix.fill(pd.val->getColor());
	QIcon ic(pix);
	lastCreated = new QTableWidgetItem(ic, ""/*,lst*/);
}

void RichParameterToQTableWidgetItemConstructor::visit(RichAbsPerc& pd)
{
	lastCreated = new QTableWidgetItem(QString::number(pd.val->getAbsPerc())/*,lst*/);
}

void RichParameterToQTableWidgetItemConstructor::visit(RichEnum& pd)
{
	lastCreated = new QTableWidgetItem(QString::number(pd.val->getEnum())/*,lst*/);
}

void RichParameterToQTableWidgetItemConstructor::visit(RichDynamicFloat& pd)
{
	lastCreated = new QTableWidgetItem(QString::number(pd.val->getDynamicFloat())/*,lst*/);
}

void RichParameterToQTableWidgetItemConstructor::visit(RichDynamicFloat_S& pd)
{
	lastCreated = new QTableWidgetItem(QString::number(pd.val->getDynamicFloat())/*,lst*/);
}
void RichParameterToQTableWidgetItemConstructor::visit(RichDynamicFloat_spin& pd)
{
	lastCreated = new QTableWidgetItem(QString::number(pd.val->getDynamicFloat())/*,lst*/);
}

IOFileWidget::IOFileWidget(QWidget* p, RichParameter* rpar)
:MeshLabWidget(p, rpar), fl()
{
	filename = new QLineEdit(this);
	filename->setText(tr(""));
	browse = new QPushButton(this);
	descLab = new QLabel(rp->pd->fieldDesc, this);
	browse->setText("...");
	//const QColor cl = rp->pd->defVal->getColor();
	//resetWidgetValue();
	//int row = gridLay->rowCount() - 1;
	//gridLay->addWidget(descLab,row,0,Qt::AlignTop);
	hlay = new QHBoxLayout();
	hlay->addWidget(filename, 2);
	hlay->addWidget(browse);

	//gridLay->addLayout(lay,row,1,Qt::AlignTop);

	connect(browse, SIGNAL(clicked()), this, SLOT(selectFile()));
	connect(this, SIGNAL(dialogParamChanged()), p, SIGNAL(parameterChanged()));
}

IOFileWidget::~IOFileWidget()
{
	delete filename;
	delete browse;
	delete descLab;
}

void IOFileWidget::collectWidgetValue()
{
	rp->val->set(FileValue(fl));
}

void IOFileWidget::resetWidgetValue()
{
	QString fle = rp->pd->defVal->getFileName();
	fl = fle;
	updateFileName(fle);
}


void IOFileWidget::setWidgetValue(const Value& nv)
{
	QString fle = nv.getFileName();
	fl = fle;
	updateFileName(QString());
}

void IOFileWidget::updateFileName(const FileValue& file)
{
	filename->setText(file.getFileName());
}

void IOFileWidget::addWidgetToGridLayout(QGridLayout* lay, const int r)
{
	if (lay != NULL)
	{
		lay->addWidget(descLab, r, 0, Qt::AlignTop);
		lay->addLayout(hlay, r, 1, Qt::AlignTop);
	}
	MeshLabWidget::addWidgetToGridLayout(lay, r);
}

OpenFileWidget::OpenFileWidget(QWidget *p, RichOpenFile* rdf)
:IOFileWidget(p, rdf)
{
}

void OpenFileWidget::selectFile()
{
	OpenFileDecoration* dec = reinterpret_cast<OpenFileDecoration*>(rp->pd);
	QString ext;
	fl = QFileDialog::getOpenFileName(this, tr("Open"), dec->defVal->getFileName(), dec->exts.join(" "));
	collectWidgetValue();
	updateFileName(fl);
	emit dialogParamChanged();
}


OpenFileWidget::~OpenFileWidget()
{
}


SaveFileWidget::SaveFileWidget(QWidget* p, RichSaveFile* rpar)
:IOFileWidget(p, rpar)
{
	filename->setText(rpar->val->getFileName());
}

SaveFileWidget::~SaveFileWidget()
{
}

void SaveFileWidget::selectFile()
{
	SaveFileDecoration* dec = reinterpret_cast<SaveFileDecoration*>(rp->pd);
	QString ext;
	fl = QFileDialog::getSaveFileName(this, tr("Save"), dec->defVal->getFileName(), dec->ext);
	collectWidgetValue();
	updateFileName(fl);
	emit dialogParamChanged();
}


TransformWidget::TransformWidget(QAction *editAction, MeshEditInterFace_v2 *mediv2, QWidget *_p,  QString title, MeshDocument *_meshDocument, QWidget *gla) :QFrame(_p)
{
	stdParFrame = NULL;
	curParSet.clear();
	curgla = 0;
	curmwi = 0;
	enterButton = 0;
	currentAction = editAction;
	meshDocument = _meshDocument;
	editIF2 = mediv2;

	grooveXYZ = meshDocument->groove.Dim();
	grooveXYZ.X() -= 0.01;
	grooveXYZ.Y() -= 0.01;
	grooveXYZ.Z() -= 0.01;
	
	init();
	
	mediv2->initParameterSet(editAction, *meshDocument, curParSet);//***
	loadParameter();

	tempParamSet.addParam(new RichBool("collision_detected", allowOverLappingCB->isChecked(), "", ""));
	tempParamSet.addParam(new RichBool("all_or_not", false, "", ""));
	if (meshDocument != NULL ){

		int unit = meshDocument->p_setting.gd_unit;
		switch (unit)
		{
		case 0:
			unitLB->setText(tr("Unit : cm"));
			unitRatio = 1;
			break;
		case 1:
			unitLB->setText(tr("Unit : cm"));
			//unitRatio = DSP_inchmm;
			unitRatio = DSP_inchmm;
			break;
		}
	}
	unitRatio = 1;
}
void TransformWidget::init()
{
	QVBoxLayout *layout = new QVBoxLayout;
	QGridLayout *gridL1 = new QGridLayout;
	//QGridLayout *gridL2 = new QGridLayout;
	//QGridLayout *gridL3 = new QGridLayout;

	QToolButton *moveTab = new QToolButton;

	moveTab->setText(tr("MOVE"));
	moveTab->setStyleSheet(WidgetStyleSheet::viewTextStyleSheet(":/images/icons/tab_name_bg_on.png", ":/images/icons/tab_name_bg_off.png"));
	moveTab->setToolTipDuration(1);


	labelX = new QLabel("X");
	labelY = new QLabel("Y");
	labelZ = new QLabel("Z");
	unitLB = new QLabel;
	unitLB->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	labelX->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	labelY->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	labelZ->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	unitLB->setStyleSheet(WidgetStyleSheet::textStyleSheet());

	//X Translate DB
	xTranslateDBSB = new QDoubleSpinBox;
	xTranslateDBSB->setRange(-grooveXYZ.X() / 2, grooveXYZ.X() / 2);
	xTranslateDBSB->setSingleStep(1.0);
	xTranslateDBSB->setFixedSize(100, 100);
	xTranslateDBSB->setFixedSize(100, 24);
	xTranslateDBSB->setKeyboardTracking(false);
	xTranslateDBSB->setStyleSheet(WidgetStyleSheet::spinBoxStyleSheet());
		
	
	connect(xTranslateDBSB, SIGNAL(valueChanged(double)), this, SLOT(applyTransformchange()));

	yTranslateDBSB = new QDoubleSpinBox;
	yTranslateDBSB->setRange(-grooveXYZ.Y() / 2, grooveXYZ.Y() / 2);
	yTranslateDBSB->setSingleStep(1.0);
	yTranslateDBSB->setFixedSize(100, 24);
	yTranslateDBSB->setStyleSheet(WidgetStyleSheet::spinBoxStyleSheet());
	yTranslateDBSB->setKeyboardTracking(false);
	connect(yTranslateDBSB, SIGNAL(valueChanged(double)), this, SLOT(applyTransformchange()));

	zTranslateDBSB = new QDoubleSpinBox;
	//zTranslateDBSB->setRange(-grooveXYZ.Z() / 2, grooveXYZ.Z() / 2);
	zTranslateDBSB->setRange(0, 200);
	zTranslateDBSB->setSingleStep(1.0);
	zTranslateDBSB->setFixedSize(100, 24);
	zTranslateDBSB->setStyleSheet(WidgetStyleSheet::spinBoxStyleSheet());
	zTranslateDBSB->setKeyboardTracking(false);
	connect(zTranslateDBSB, SIGNAL(valueChanged(double)), this, SLOT(applyTransformchange()));

	resetTB = new QToolButton;
	resetTB->setStyleSheet(WidgetStyleSheet::viewDockWidgetBottomTB(":/images/icons/btn_reset.png"));
	resetTB->setText(tr("Reset"));
	connect(resetTB, SIGNAL(clicked()), this, SLOT(resetTBPressed()));


	toCenterTB = new QToolButton;
	toCenterTB->setStyleSheet(WidgetStyleSheet::viewDockWidgetBottomTB(":/images/icons/btn_to_center.png", ":/images/icons/btn_to_center_disable.png"));
	toCenterTB->setText(tr("To Center"));
	connect(toCenterTB, SIGNAL(clicked()), this, SLOT(toCenterTBClicked()));

	landToolGB = createlandGB();

	

	


	//apply
	applyTransformTB = new QToolButton();
	applyTransformTB->setText(tr("applyTransform"));
	connect(applyTransformTB, SIGNAL(clicked()), this, SLOT(applyTransformchange()));



	gridL1->addWidget(unitLB, 0, 1);

	gridL1->addWidget(labelX, 1, 0);
	//gridL1->addWidget(slider1, 0, 1);
	//gridL1->addWidget(lineEdit1,0, 2);
	gridL1->addWidget(xTranslateDBSB, 1, 1);

	gridL1->addWidget(labelY, 2, 0);
	gridL1->addWidget(yTranslateDBSB, 2, 1);

	gridL1->addWidget(labelZ, 3, 0);
	gridL1->addWidget(zTranslateDBSB, 3, 1);

	

	gridL1->setAlignment(labelX, Qt::AlignHCenter);
	gridL1->setAlignment(labelY, Qt::AlignHCenter);
	gridL1->setAlignment(labelZ, Qt::AlignHCenter);
	
	gridL1->setMargin(10);
	gridL1->setVerticalSpacing(20);
	


	//QWidget *spacerWidget = new QWidget();//***空白spacer
	//spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	//spacerWidget->setVisible(true);
	layout->addWidget(moveTab,0, Qt::AlignHCenter);
	layout->addLayout(gridL1, Qt::AlignTop);
	layout->addStretch();
	//layout->addWidget(landTB, 0, Qt::AlignHCenter);
	layout->addWidget(toCenterTB, Qt::AlignBottom);
	layout->setAlignment(toCenterTB, Qt::AlignHCenter);
	layout->addWidget(landToolGB, Qt::AlignBottom);
	//layout->addLayout(gridL2);
	//layout->addLayout(gridL3);
	//layout->addWidget(applyTransformTB);
	//layout->addWidget(resetTB);
	//layout->insertSpacing(1, 50);
	layout->setMargin(0);
	setLayout(layout);
}

QGroupBox* TransformWidget::createlandGB()
{
	QGroupBox*tempGB = new QGroupBox(" ");
	tempGB->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	tempGB->setStyleSheet(WidgetStyleSheet::grupBox());
	tempGB->setFixedHeight(250);


	QHBoxLayout *hlayout = new QHBoxLayout;
	QVBoxLayout *vlayout = new QVBoxLayout;

	onLB = new QLabel(tr("On"));
	onLB->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	onLB->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	onLB->setFixedHeight(36);

	offLB = new QLabel(tr("Off"));
	offLB->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	offLB->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	offLB->setFixedHeight(36);

	overlappingLB = new QLabel(tr("Allow Overlapping"));
	overlappingLB->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	overlappingLB->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	overlappingLB->setFixedHeight(36);
	//overlappingLB
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);


	//allowOverLappingCB
	allowOverLappingCB = new QCheckBox(" ");
	allowOverLappingCB->setStyleSheet(WidgetStyleSheet::check_onoff_styleSheet());
	allowOverLappingCB->setChecked(true);
	connect(allowOverLappingCB, SIGNAL(stateChanged(int)), this, SLOT(lappingCBstateChanged(int)));

	hlayout->addWidget(offLB, 0, Qt::AlignCenter);
	hlayout->addWidget(allowOverLappingCB, 0, Qt::AlignCenter);
	hlayout->addWidget(onLB, 0, Qt::AlignCenter);

	landTB = new QToolButton;
	landTB->setStyleSheet(WidgetStyleSheet::viewDockWidgetBottomTB(":/images/icons/btn_land.png", ":/images/icons/btn_land_disable.png"));
	landTB->setText(tr("    Land"));
	connect(landTB, SIGNAL(clicked()), this, SLOT(landTBClicked()));

	landAllTB = new QToolButton;
	landAllTB->setStyleSheet(WidgetStyleSheet::viewDockWidgetBottomTB(":/images/icons/btn_land_all.png", ":/images/icons/btn_land_all_disable.png"));
	landAllTB->setText(tr("    Land All"));
	connect(landAllTB, SIGNAL(clicked()), this, SLOT(landAllTBClicked()));

	//QWidget *spacerWidget = new QWidget();//***空白spacer
	//spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	//spacerWidget->setVisible(true);

	vlayout->setMargin(0);
	vlayout->setSpacing(0);
	vlayout->addWidget(landTB, 0, Qt::AlignCenter);
	vlayout->addWidget(landAllTB, 0, Qt::AlignCenter);
	vlayout->addWidget(overlappingLB, 0, Qt::AlignCenter);
	vlayout->addLayout(hlayout);
	//vlayout->addWidget(toCenterTB);
	//vlayout->addWidget(spacerWidget);


	tempGB->setLayout(vlayout);


	return tempGB;


}


void TransformWidget::lappingCBstateChanged(int value)
{
	if (value == 0)
		curmwi2->setOverlapping(false);
	else
		curmwi2->setOverlapping(true);
}

void TransformWidget::applyTransformchange()
{
	QAction *q = currentAction;
	//stdParFrame->readValues(curParSet);
	//loadParameter();
	if (meshDocument->multiSelectID.size() > 0){
		getWidgetValue();
		curmwi->executeEdit(currentAction, curParSet);
	}
	//curgla->update();

	/*
	QList<RichParameter*> &parList = curParSet.paramList;
	assert(parList.size() == stdfieldwidgets.size());
	QVector<MeshLabWidget*>::iterator it = stdfieldwidgets.begin();
	for (int i = 0; i < parList.count(); i++)
	{
	QString sname = parList.at(i)->name;
	curParSet.setValue(sname, (*it)->getWidgetValue());
	++it;
	}

	*/

}
void TransformWidget::loadParameter()
{
	grooveXYZ = meshDocument->groove.Dim();

	Box3m selectedBBox;
	Matrix44m iden;
	iden.SetIdentity();
	qDebug() << "selectedBBox.min" << meshDocument->multiSelectID.size();
	foreach(int i, meshDocument->multiSelectID)
	{
		if (meshDocument->getMesh(i)->getMeshSort() == MeshModel::meshsort::print_item)
			selectedBBox.Add(iden, meshDocument->getMesh(i)->cm.bbox);
	}
	qDebug() << "selectedBBox.min"<<selectedBBox.min.X() << selectedBBox.min.Y() << selectedBBox.min.Z();
	//if (meshDocument->multiSelectID.size() > 0){
	if (meshDocument->mm() != NULL && curmwi!=NULL){		

		int unit = meshDocument->p_setting.gd_unit;
		switch (unit)
		{
		case 0:
			unitLB->setText(tr("Unit : mm"));
			unitRatio = 1;
			break;
		case 1:
			unitLB->setText(tr("Unit : inch"));
			unitRatio = DSP_inchmm;
			break;
		}

		QList<RichParameter*> tempParList = curParSet.paramList;
		RichFloat *floatTemp = static_cast<RichFloat *>(tempParList.at(0));
		//lineEdit1->setText(QString::number(floatTemp->val->getFloat()));
		//MeshModel *tm = meshDocument->getMesh(*meshDocument->multiSelectID.begin());
		MeshModel *tm = meshDocument->mm();
		//Point3f tp = tm->cm.bbox.Center();
		//Point3f tp = selectedBBox.Center();
		Point3f tp = selectedBBox.min;
		//Point3f tmp = tm->cm.bbox.Dim();
		//Point3f tmp = (tm->cm.bbox.Dim())/2.;
		Point3f tmp = selectedBBox.Dim() / 2.;
		Point3f halfDim = grooveXYZ / 2.;
		
		

		//set maxima first
		xTranslateDBSB->blockSignals(true);
		yTranslateDBSB->blockSignals(true);
		zTranslateDBSB->blockSignals(true);


		/*xTranslateDBSB->setMaximum((grooveXYZ.X() / 2 - tmp.X()) / unitRatio);
		yTranslateDBSB->setMaximum((grooveXYZ.Y() / 2 - tmp.Y()) / unitRatio);
		//zTranslateDBSB->setMaximum(((grooveXYZ.Z() / 2 - tmp.Z()) + 100) / unitRatio);
		zTranslateDBSB->setMaximum(((grooveXYZ.Z() / 2 - (tmp.Z() * 2)) + halfDim.Z()) / unitRatio);

		xTranslateDBSB->setMinimum(-(grooveXYZ.X() / 2 - tmp.X()) / unitRatio);
		yTranslateDBSB->setMinimum(-(grooveXYZ.Y() / 2 - tmp.Y()) / unitRatio);
		//zTranslateDBSB->setMinimum(((-(grooveXYZ.Z() / 2) + 100) + tmp.Z()) / unitRatio);
		zTranslateDBSB->setMinimum((-(grooveXYZ.Z() / 2) + halfDim.Z()) / unitRatio);

		xTranslateDBSB->setValue(tp.X() / unitRatio);
		qDebug() << "zCenter: " << selectedBBox.Center().Z();
		yTranslateDBSB->setValue(tp.Y() / unitRatio);
		if (meshDocument->multiSelectID.size() == 0)
			zTranslateDBSB->setValue(tp.Z() / unitRatio);
		else
			//zTranslateDBSB->setValue((tp.Z() + 100) / unitRatio);
			zTranslateDBSB->setValue(((tp.Z() - (selectedBBox.DimZ() / 2)) + halfDim.Z()) / unitRatio);

		*/


		xTranslateDBSB->setMaximum(((grooveXYZ.X() / 2 - (tmp.X() * 2)) + halfDim.X()) / unitRatio);
		yTranslateDBSB->setMaximum(((grooveXYZ.Y() / 2 - (tmp.Y() * 2)) + halfDim.Y()) / unitRatio);		
		zTranslateDBSB->setMaximum(((grooveXYZ.Z() / 2 - (tmp.Z() * 2)) + halfDim.Z()) / unitRatio);

		xTranslateDBSB->setMinimum(0.);
		yTranslateDBSB->setMinimum(0.);		
		zTranslateDBSB->setMinimum(0.);

		xTranslateDBSB->setValue((tp.X() + halfDim.X()) / unitRatio);		
		yTranslateDBSB->setValue((tp.Y() + halfDim.Y()) / unitRatio);
		if (meshDocument->multiSelectID.size() == 0)
			zTranslateDBSB->setValue((tp.Z()) / unitRatio);
		else
			//zTranslateDBSB->setValue((tp.Z() + halfDim.Z()) / unitRatio);
			zTranslateDBSB->setValue(((tp.Z()) + halfDim.Z()) / unitRatio);

		

		
			

		xTranslateDBSB->blockSignals(false);
		yTranslateDBSB->blockSignals(false);
		zTranslateDBSB->blockSignals(false);

		//applyTransformchange();
		

		/*qDebug() << "xTranslateDBSB->value() - tm->cm.bbox.min.X()" << xTranslateDBSB->value() << yTranslateDBSB->value() << zTranslateDBSB->value();
		qDebug() << "max.x.x" << tm->cm.bbox.max.X() << 11 - tmp.X() << tmp.X();*/
	}
}

void TransformWidget::getWidgetValue()
{
	Box3m selectedBBox;
	Matrix44m iden;
	iden.SetIdentity();

	Point3f halfDim = meshDocument->groove.Dim() / 2.;

	foreach(int i, meshDocument->multiSelectID)
	{
		if (meshDocument->getMesh(i)->getMeshSort() == MeshModel::meshsort::print_item)
			selectedBBox.Add(iden, meshDocument->getMesh(i)->cm.bbox);
	}

	//float x, y, z;
	//x = xTranslateDBSB->value()*unitRatio - selectedBBox.Center().X();
	//y = yTranslateDBSB->value()*unitRatio - selectedBBox.Center().Y();
	////z = (zTranslateDBSB->value() - 100) * unitRatio - selectedBBox.Center().Z();
	//z = ((zTranslateDBSB->value() + (selectedBBox.DimZ() / 2)) - halfDim.Z()) * unitRatio - selectedBBox.Center().Z();

	float x, y, z;
	//x = xTranslateDBSB->value()*unitRatio - selectedBBox.Center().X();
	//y = yTranslateDBSB->value()*unitRatio - selectedBBox.Center().Y();
	//z = (zTranslateDBSB->value() - 100) * unitRatio - selectedBBox.Center().Z();
	x = ((xTranslateDBSB->value() + (selectedBBox.DimX() / 2)) - halfDim.X()) * unitRatio - selectedBBox.Center().X();
	y = ((yTranslateDBSB->value() + (selectedBBox.DimY() / 2)) - halfDim.Y()) * unitRatio - selectedBBox.Center().Y();
	z = ((zTranslateDBSB->value() + (selectedBBox.DimZ() / 2)) - halfDim.Z()) * unitRatio - selectedBBox.Center().Z();
	qDebug() << "x is: " << x;
	qDebug() << "y is: " << y;
	qDebug() << "z is: " << z;

	/*if (!xTranslateDBSB->hasFocus())
		x = 0;
	if (!yTranslateDBSB->hasFocus())
		y = 0;
	if (!zTranslateDBSB->hasFocus())
		z = 0;*/

	MeshModel *tm = meshDocument->mm();
	float xMin = tm->cm.bbox.min.X();
	float yMin = tm->cm.bbox.min.Y();
	float zMin = tm->cm.bbox.min.Z();
	float xMax = tm->cm.bbox.max.X();
	float yMax = tm->cm.bbox.max.Y();
	float zMax = tm->cm.bbox.max.Z();

	foreach(int i, meshDocument->multiSelectID)
	{
		MeshModel *tmm = meshDocument->getMesh(i);

		if (xMin > tmm->cm.bbox.min.X())
			xMin = tmm->cm.bbox.min.X();
		if (xMax < tmm->cm.bbox.max.X())
			xMax = tmm->cm.bbox.max.X();
		if (yMin > tmm->cm.bbox.min.Y())
			yMin = tmm->cm.bbox.min.Y();
		if (yMax < tmm->cm.bbox.max.Y())
			yMax = tmm->cm.bbox.max.Y();
		if (zMin > tmm->cm.bbox.min.Z())
			zMin = tmm->cm.bbox.min.Z();
		if (zMax < tmm->cm.bbox.max.Z())
			zMax = tmm->cm.bbox.max.Z();
	}

	if ((-grooveXYZ.X() / 2) > (xMin + x))
		x = x - ((xMin + x) - (-grooveXYZ.X() / 2));
	if ((grooveXYZ.X() / 2) < (xMax + x))
		x = x - ((xMax + x) - (grooveXYZ.X() / 2));
	if ((-grooveXYZ.Y() / 2) > (yMin + y))
		y = y - ((yMin + y) - (-grooveXYZ.Y() / 2));
	if ((grooveXYZ.Y() / 2) < (yMax + y))
		y = y - ((yMax + y) - (grooveXYZ.Y() / 2));
	if ((grooveXYZ.Z() / 2) < (zMax + z))
		z = z - ((zMax + z) - (grooveXYZ.Z() / 2));
	if ((-grooveXYZ.Z() / 2) > (zMin + z))
		z = z - ((zMin + z) - (-grooveXYZ.Z() / 2));

	curParSet.setValue("X_translate", FloatValue(x));
	curParSet.setValue("Y_translate", FloatValue(y));
	curParSet.setValue("Z_translate", FloatValue(z));

	selectedBBox.Translate(Point3f(x, y, z));
}

void TransformWidget::resetTBPressed()
{
	//MeshModel *tm = meshDocument->mm();
	//curParSet.setValue("X_translate", FloatValue(0 - tm->cm.bbox.min.Y()));
	////qDebug() << "getWidgetValue" << xTranslateDBSB->value() << xTranslateDBSB->value() - tm->cm.bbox.min.X() + tm->cm.bbox.Dim().X();
	//curParSet.setValue("Y_translate", FloatValue(0 - tm->cm.bbox.min.Y()));
	//curParSet.setValue("Z_translate", FloatValue(-9.99- tm->cm.bbox.min.Z()));
	//curmwi->executeEdit(currentAction, curParSet);

	curmwi2->reload();

}
void TransformWidget::landTBClicked()
{
	MeshDocument *md = curmwi2->meshDoc();
	QList<Point3f> savePos;
	savePos.clear();

	foreach(int i, md->multiSelectID)
	{
		savePos.push_back(md->getMesh(i)->cm.bbox.Center());
	}

	//RichParameterSet dummyParSet;
	tempParamSet.setValue("collision_detected", BoolValue(!allowOverLappingCB->isChecked()));
	tempParamSet.setValue("all_or_not", BoolValue(false));
	//curmwi2->executeFilter(curmwi2->PM.actionFilterMap.value("Landing_Tool"), tempParamSet, false);
	curmwi2->executeFilter(curmwi2->PM.actionFilterMap.value("FP_GET_DEPTH_LANDING_TEST2"), tempParamSet, false);
	
	int count = 0;
	foreach(int i, md->multiSelectID)
	{
		if (count < md->multiSelectID.size())
		{
			float z = md->getMesh(i)->cm.bbox.Center().Z() - savePos[count].Z();
			if (z != 0)
			{
				curmwi2->setUndoStack()->push(new land_command(curmwi2,md, md->multiSelectID, savePos, false));
				break;
			}
			count++;
		}
	}
}
void TransformWidget::landAllTBClicked()
{
	MeshDocument *md = curmwi2->meshDoc();
	QMap<int, Point3f> savePos;
	savePos.clear();

	foreach(MeshModel *tmm, md->meshList)
	{
		savePos.insert(tmm->id(), tmm->cm.bbox.Center());
	}

	tempParamSet.setValue("collision_detected", BoolValue(!allowOverLappingCB->isChecked()));
	tempParamSet.setValue("all_or_not", BoolValue(true));
	//curmwi2->executeFilter(curmwi2->PM.actionFilterMap.value("Landing_Tool"), tempParamSet, false);
	curmwi2->executeFilter(curmwi2->PM.actionFilterMap.value("FP_GET_DEPTH_LANDING_TEST2"), tempParamSet, false);

	int count = 0;
	foreach(MeshModel *mm, md->meshList)
	{
		if (count < md->meshList.size())
		{
			float z = mm->cm.bbox.Center().Z() - savePos.value(mm->id()).Z();
			if (z != 0)
			{
				curmwi2->setUndoStack()->push(new landAll_command(curmwi2,md, savePos, false));
				break;
			}
			count++;
		}
	}
}
void TransformWidget::toCenterTBClicked()
{

	MeshDocument *md = curmwi2->meshDoc();
	QList<Point3f> savePos;
	savePos.clear();

	QList<int> list = md->multiSelectID.toList();
	qSort(list.begin(), list.end(), qLess<int>());

	foreach(int i, list)
	{
		savePos.push_back(md->getMesh(i)->cm.bbox.Center());
	}

	//*** 20170126 --- modified toCenterTBClicked() function to be applicable to multiple meshes
	// Modified by KPPH (R&D-I-SW, Mark)
	foreach(int i, list)
	{	
		curmwi2->currentViewContainer()->meshDoc.setCurrentMesh(i);
		curmwi2->executeFilter(curmwi2->PM.actionFilterMap.value("MoveToCenter"), tempParamSet, false);
	}

	curmwi2->setUndoStack()->push(new ToCenter_command(curmwi2,md, list, savePos, false));
}

TransformWidget::~TransformWidget()
{

}


RotateWidget_Customized::RotateWidget_Customized(QAction *editAction, MeshEditInterFace_v2 *mediv2, QWidget *_p, QString title, MeshDocument *_meshDocument, QWidget *gla) :QFrame(_p)
{
	stdParFrame = NULL;
	curParSet.clear();
	curgla = 0;
	curmwi = 0;
	enterButton = 0;
	currentAction = editAction;
	meshDocument = _meshDocument;
	editIF2 = mediv2;

	grooveXYZ = meshDocument->groove.Dim();
	grooveXYZ.X() -= 0.01;
	grooveXYZ.Y() -= 0.01;
	grooveXYZ.Z() -= 0.01;

	init();

	mediv2->initParameterSet(editAction, *meshDocument, curParSet);//***
	loadParameter();
	
}
void RotateWidget_Customized::init()
{
	QVBoxLayout *layout = new QVBoxLayout;
	QGridLayout *gridL1 = new QGridLayout;
	//QGridLayout *gridL2 = new QGridLayout;
	//QGridLayout *gridL3 = new QGridLayout;

	QToolButton *rotateTab = new QToolButton;

	rotateTab->setText(tr("ROTATE"));
	rotateTab->setStyleSheet(WidgetStyleSheet::viewTextStyleSheet(":/images/icons/tab_name_bg_on.png", ":/images/icons/tab_name_bg_off.png"));
	rotateTab->setToolTipDuration(1);

	x_accumulation_degree = 0;
	y_accumulation_degree = 0;
	z_accumulation_degree = 0;

	xRotValue = 0;
	yRotValue = 0;
	zRotValue = 0;


	labelX = new QLabel("X");
	labelY = new QLabel("Y");
	labelZ = new QLabel("Z");
	unitLB = new QLabel(tr("Unit : degree"));
	unitLB->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	unitLB->setStyleSheet(WidgetStyleSheet::textStyleSheet());

	labelX->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	labelY->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	labelZ->setStyleSheet(WidgetStyleSheet::textStyleSheet());

	//X Translate DB
	xRotateDBSB = new My_DoubleSpinBox(My_DoubleSpinBox::db_name::x_rotate_db);
	xRotateDBSB->setRange(-360, 360);
	xRotateDBSB->setValue(0);
	xRotateDBSB->setSingleStep(1);
	xRotateDBSB->setSuffix("");
	xRotateDBSB->setKeyboardTracking(false);
	connect(xRotateDBSB, SIGNAL(valueChanged(double)), this, SLOT(applyTransformchange()));

	yRotateDBSB = new My_DoubleSpinBox(My_DoubleSpinBox::db_name::y_rotate_db);
	yRotateDBSB->setRange(-360, 360);
	yRotateDBSB->setValue(0);
	yRotateDBSB->setSingleStep(1);
	yRotateDBSB->setSuffix("");
	yRotateDBSB->setKeyboardTracking(false);
	connect(yRotateDBSB, SIGNAL(valueChanged(double)), this, SLOT(applyTransformchange()));

	zRotateDBSB = new My_DoubleSpinBox(My_DoubleSpinBox::db_name::z_rotate_db);
	zRotateDBSB->setRange(-360, 360);
	zRotateDBSB->setValue(0);
	zRotateDBSB->setSingleStep(1);
	zRotateDBSB->setSuffix("");
	zRotateDBSB->setKeyboardTracking(false);
	connect(zRotateDBSB, SIGNAL(valueChanged(double)), this, SLOT(applyTransformchange()));

	xDial = new QDial;


	//applyf
	applyTransformTB = new QToolButton();
	applyTransformTB->setText(tr("applyRotate"));
	connect(applyTransformTB, SIGNAL(pressed()), this, SLOT(applyTransformchange()));

	resetTB = new QToolButton;
	resetTB->setStyleSheet(WidgetStyleSheet::viewDockWidgetBottomTB(":/images/icons/btn_reset.png"));
	resetTB->setText(tr("Reset"));
	connect(resetTB, SIGNAL(clicked()), this, SLOT(resetTBPressed()));
	

	gridL1->addWidget(unitLB, 0, 1);

	gridL1->addWidget(labelX, 1, 0);
	gridL1->addWidget(xRotateDBSB, 1, 1);

	gridL1->addWidget(labelY, 2, 0);
	gridL1->addWidget(yRotateDBSB, 2, 1);

	gridL1->addWidget(labelZ, 3, 0);
	gridL1->addWidget(zRotateDBSB, 3, 1);

	gridL1->setAlignment(labelX, Qt::AlignHCenter);
	gridL1->setAlignment(labelY, Qt::AlignHCenter);
	gridL1->setAlignment(labelZ, Qt::AlignHCenter);

	gridL1->setMargin(10);
	gridL1->setVerticalSpacing(20);	
	//qDebug() << "xRotateDBSB size" << xRotateDBSB->size().width() << xRotateDBSB->size().height();

	//QWidget *spacerWidget = new QWidget();//***空白spacer
	//spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	//spacerWidget->setVisible(true);

	layout->addWidget(rotateTab, 0, Qt::AlignHCenter);
	layout->addLayout(gridL1);	
	//layout->addWidget(applyTransformTB);
	//layout->addWidget(spacerWidget);
	layout->addStretch();
	//layout->addWidget(resetTB);
	layout->insertSpacing(0, 0);
	layout->setMargin(0);
	

	
	setLayout(layout);

}
void RotateWidget_Customized::applyTransformchange()
{
	QAction *q = currentAction;
	//stdParFrame->readValues(curParSet);
	//loadParameter();
	if (meshDocument->multiSelectID.size() > 0){
		getWidgetValue();
		curmwi->executeEdit(currentAction, curParSet);
	}
	//curgla->update();

	/*
	QList<RichParameter*> &parList = curParSet.paramList;
	assert(parList.size() == stdfieldwidgets.size());
	QVector<MeshLabWidget*>::iterator it = stdfieldwidgets.begin();
	for (int i = 0; i < parList.count(); i++)
	{
	QString sname = parList.at(i)->name;
	curParSet.setValue(sname, (*it)->getWidgetValue());
	++it;
	}

	*/
}
void RotateWidget_Customized::loadParameter()
{	
	if (xRotateDBSB->hasFocus() == false)
		x_accumulation_degree = 0; 
	if (yRotateDBSB->hasFocus() == false)
		y_accumulation_degree = 0;
	if (zRotateDBSB->hasFocus() == false)
		z_accumulation_degree = 0;

	xRotateDBSB->blockSignals(true);
	yRotateDBSB->blockSignals(true);
	zRotateDBSB->blockSignals(true);

	xRotateDBSB->setValue(0);
	yRotateDBSB->setValue(0);
	zRotateDBSB->setValue(0);

	xRotateDBSB->blockSignals(false);
	yRotateDBSB->blockSignals(false);
	zRotateDBSB->blockSignals(false);	
}
void RotateWidget_Customized::getWidgetValue()
{
	int xt = *meshDocument->multiSelectID.begin();
	MeshModel *tm = meshDocument->getMesh(*meshDocument->multiSelectID.begin());

	My_DoubleSpinBox *dbsb = (My_DoubleSpinBox *)sender();
	if (dbsb != NULL)
		switch (dbsb->dbsb_name)
	{
		case My_DoubleSpinBox::db_name::x_rotate_db:
		{
			xRotValue = dbsb->value() - x_accumulation_degree;
			x_accumulation_degree = xRotateDBSB->value();
			yRotValue = 0;
			zRotValue = 0;
			curParSet.setValue("Axis", EnumValue(0));
			curParSet.setValue("rotate_degree", FloatValue(xRotValue));
		}
			break;
		case My_DoubleSpinBox::db_name::y_rotate_db:
		{
			yRotValue = dbsb->value() - y_accumulation_degree;
			y_accumulation_degree = yRotateDBSB->value();
			xRotValue = 0;
			zRotValue = 0;
			curParSet.setValue("Axis", EnumValue(1));
			curParSet.setValue("rotate_degree", FloatValue(yRotValue));
		}
			break;
		case My_DoubleSpinBox::db_name::z_rotate_db:
		{
			zRotValue = dbsb->value() - z_accumulation_degree;
			z_accumulation_degree = zRotateDBSB->value();
			xRotValue = 0;
			yRotValue = 0;
			curParSet.setValue("Axis", EnumValue(2));
			curParSet.setValue("rotate_degree", FloatValue(zRotValue));
		}
			break;

	}

	

}
void RotateWidget_Customized::resetTBPressed()
{
	//MeshModel *tm = meshDocument->mm();
	//*tm = originalM;
	
	/*Matrix44m tempMa4 = tm->cm.Tr;
	tempMa4.
	Point3f c0= tm->cm.Tr.GetColumn3(0);
	Point3f c1 = tm->cm.Tr.GetColumn3(1);
	Point3f c2 = tm->cm.Tr.GetColumn3(2);
	c0[1] = 0; c0[1] = 0;
	c1[1] = 0; c1[1] = 0;
	c2[1] = 0; c2[1] = 0;
	tm->cm.Tr.SetColumn(0,c0);
	tm->cm.Tr.SetColumn(0, c1);
	tm->cm.Tr.SetColumn(0, c2);		
*/
	//meshDocument->delMesh(tm);
	//meshDocument->add
	//Matrix44m temp
	//temp
	//tri::UpdateBounding<CMeshO>::Box(tm->cm);
}
RotateWidget_Customized::~RotateWidget_Customized()
{

}



My_DoubleSpinBox::My_DoubleSpinBox(db_name _digit, QWidget *parent) :QDoubleSpinBox(parent)
{
	//digit = _digit;
	dbsb_name = _digit;
	setFixedSize(100,24);
	setStyleSheet(WidgetStyleSheet::spinBoxStyleSheet());
}

ScaleWidget_Customized::ScaleWidget_Customized(QAction *editAction, MeshEditInterFace_v2 *mediv2, QWidget *_p, QString title, MeshDocument *_meshDocument, QWidget *gla) :QFrame(_p)
{
	stdParFrame = NULL;
	curParSet.clear();
	curgla = 0;
	curmwi = 0;
	enterButton = 0;
	currentAction = editAction;
	meshDocument = _meshDocument;
	editIF2 = mediv2;

	grooveXYZ = meshDocument->groove.Dim();
	//grooveXYZ.X() -= 0.01;
	//grooveXYZ.Y() -= 0.01;
	//grooveXYZ.Z() -= 0.01;

	//initStackedWidget();
	init();

	mediv2->initParameterSet(editAction, *meshDocument, curParSet);//***
	loadParameter();

	if (meshDocument != NULL){

		int unit = meshDocument->p_setting.gd_unit;
		switch (unit)
		{
		case 0:
			unitLB->setText(tr("Unit : mm"));
			unitRatio = 1;
			break;
		case 1:
			unitLB->setText(tr("Unit : inch"));
			unitRatio = DSP_inchmm;
			break;
		}
	}

	setUnitRatio(1);


}


QGridLayout* ScaleWidget_Customized::createUniformWidget()
{
	//QFrame *gb2 = new QFrame;
	
	

	QVBoxLayout *layoutst = new QVBoxLayout;
	QGridLayout *gridL1 = new QGridLayout;
	//QGridLayout *gridL2 = new QGridLayout;
	//QGridLayout *gridL3 = new QGridLayout;


	labelX = new QLabel("X");
	labelY = new QLabel("Y");
	labelZ = new QLabel("Z");

	unitLB = new QLabel("test");
	unitLB->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	unitLB->setStyleSheet(WidgetStyleSheet::textStyleSheet());

	labelX->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	labelY->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	labelZ->setStyleSheet(WidgetStyleSheet::textStyleSheet());

	labelUX = new QLabel("X");
	labelUY = new QLabel("Y");
	labelUZ = new QLabel("Z");

	labelUX->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	labelUY->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	labelUZ->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	


	//X Translate DB
	xUPercentDBSB = new My_DoubleSpinBox(My_DoubleSpinBox::db_name::xPercent);
	xUPercentDBSB->setRange(0, 10000);	
	xUPercentDBSB->setSingleStep(1);
	xUPercentDBSB->setSuffix("%");
	xUPercentDBSB->setDecimals(0);
	xUPercentDBSB->setValue(100);
	xUPercentDBSB->setKeyboardTracking(false);
	connect(xUPercentDBSB, SIGNAL(valueChanged(double)), this, SLOT(applyTransformchange2()));

	xUSizeDBSB = new My_DoubleSpinBox(My_DoubleSpinBox::db_name::xSize);
	xUSizeDBSB->setRange(0.01, 10000);	
	xUSizeDBSB->setSingleStep(0.1);
	xUSizeDBSB->setValue(1);
	xUSizeDBSB->setKeyboardTracking(false);
	connect(xUSizeDBSB, SIGNAL(valueChanged(double)), this, SLOT(applyTransformchange2()));

	yUPercentDBSB = new My_DoubleSpinBox(My_DoubleSpinBox::db_name::yPercent);
	yUPercentDBSB->setRange(0, 10000);	
	yUPercentDBSB->setSingleStep(1);
	yUPercentDBSB->setSuffix("%");
	yUPercentDBSB->setDecimals(0);
	yUPercentDBSB->setValue(100);
	yUPercentDBSB->setKeyboardTracking(false);
	connect(yUPercentDBSB, SIGNAL(valueChanged(double)), this, SLOT(applyTransformchange2()));

	yUSizeDBSB = new My_DoubleSpinBox(My_DoubleSpinBox::db_name::ySize);
	yUSizeDBSB->setRange(0.01, 10000);	
	yUSizeDBSB->setSingleStep(0.1);
	yUSizeDBSB->setValue(1);
	yUSizeDBSB->setKeyboardTracking(false);
	connect(yUSizeDBSB, SIGNAL(valueChanged(double)), this, SLOT(applyTransformchange2()));

	zUPercentDBSB = new My_DoubleSpinBox(My_DoubleSpinBox::db_name::zPercent);
	zUPercentDBSB->setRange(0, 10000);	
	zUPercentDBSB->setSingleStep(1);
	zUPercentDBSB->setSuffix("%");
	zUPercentDBSB->setDecimals(0);
	zUPercentDBSB->setValue(100);
	zUPercentDBSB->setKeyboardTracking(false);
	connect(zUPercentDBSB, SIGNAL(valueChanged(double)), this, SLOT(applyTransformchange2()));

	zUSizeDBSB = new My_DoubleSpinBox(My_DoubleSpinBox::db_name::zSize);
	zUSizeDBSB->setRange(0.01, 10000);	
	zUSizeDBSB->setSingleStep(0.1);
	zUSizeDBSB->setValue(1);
	zUSizeDBSB->setKeyboardTracking(false);
	connect(zUSizeDBSB, SIGNAL(valueChanged(double)), this, SLOT(applyTransformchange2()));

	gridL1->addWidget(unitLB, 0, 1);
	gridL1->addWidget(labelUX, 1, 0);
	gridL1->addWidget(xUSizeDBSB, 1, 1);
	gridL1->addWidget(xUPercentDBSB, 2, 1);
	gridL1->addWidget(labelUY, 3, 0);
	gridL1->addWidget(yUSizeDBSB, 3, 1);
	gridL1->addWidget(yUPercentDBSB, 4, 1);
	gridL1->addWidget(labelUZ, 5, 0);
	gridL1->addWidget(zUSizeDBSB, 5, 1);
	gridL1->addWidget(zUPercentDBSB, 6, 1);	

	gridL1->setAlignment(labelUX, Qt::AlignHCenter);
	gridL1->setAlignment(labelUY, Qt::AlignHCenter);
	gridL1->setAlignment(labelUZ, Qt::AlignHCenter);
	

	gridL1->setMargin(10);
	gridL1->setVerticalSpacing(20);

	
	return gridL1;
}

void ScaleWidget_Customized::init()
{
	

	//QGridLayout *grid = new QGridLayout;
	//apply
	applyTransformTB = new QToolButton();
	applyTransformTB->setText(tr("applyScale"));
	connect(applyTransformTB, SIGNAL(pressed()), this, SLOT(applyTransformchange2()));

	resetTB = new QToolButton;
	resetTB->setStyleSheet(WidgetStyleSheet::viewDockWidgetBottomTB(":/images/icons/btn_reset.png"));
	resetTB->setText(tr("Reset"));

	QToolButton *scaleTab = new QToolButton;
	scaleTab->setText(tr("RESIZE"));
	scaleTab->setStyleSheet(WidgetStyleSheet::viewTextStyleSheet(":/images/icons/tab_name_bg_on.png", ":/images/icons/tab_name_bg_off.png"));
	scaleTab->setToolTipDuration(1);

	QHBoxLayout *hlayout = new QHBoxLayout;
	QVBoxLayout *vlayout = new QVBoxLayout;



	onLB = new QLabel(tr("On"));
	onLB->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	onLB->setStyleSheet(WidgetStyleSheet::textStyleSheet());

	offLB = new QLabel(tr("Off"));
	offLB->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	offLB->setStyleSheet(WidgetStyleSheet::textStyleSheet());

	uniformLB = new QLabel(tr("Uniform"));
	uniformLB->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	uniformLB->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	vlayout->addWidget(uniformLB);
	vlayout->setMargin(10);
	vlayout->setSpacing(20);

	//uniform_CheckBox
	uniformCHB = new QCheckBox("");
	uniformCHB->setStyleSheet(WidgetStyleSheet::check_onoff_styleSheet());
	uniformCHB->setChecked(true);
	hlayout->addWidget(offLB,0,Qt::AlignRight);
	hlayout->addWidget(uniformCHB, 0, Qt::AlignLeft);
	hlayout->addWidget(onLB);

	//QWidget *spacerWidget = new QWidget();//***空白spacer
	//spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	//spacerWidget->setVisible(true);

	//grid->addWidget(uniformCHB, 0, 1);
	//grid->setRowMinimumHeight(0, 20);

	QVBoxLayout *layoutSW = new QVBoxLayout;	
	layoutSW->addWidget(scaleTab, 0, Qt::AlignHCenter);
	layoutSW->addLayout(createUniformWidget());		
	layoutSW->addLayout(vlayout);
	layoutSW->addLayout(hlayout);
	//layoutSW->addWidget(spacerWidget);
	layoutSW->addStretch();
	//layoutSW->addWidget(resetTB);
	layoutSW->setMargin(0);

	layoutSW->insertSpacing(0, 0);
	layoutSW->setMargin(0);

	setLayout(layoutSW);
	

	//layoutSW->addWidget(applyTransformTB);
	//setEnabledWidget(1);
	currentPercent = Point3f(100,100,100);

}

void ScaleWidget_Customized::applyTransformchange2()
{
	if (meshDocument->multiSelectID.size() > 0){
		getWidgetValue2();
		curmwi->executeEdit(currentAction, curParSet);
	}

	/*
	QList<RichParameter*> &parList = curParSet.paramList;
	assert(parList.size() == stdfieldwidgets.size());
	QVector<MeshLabWidget*>::iterator it = stdfieldwidgets.begin();
	for (int i = 0; i < parList.count(); i++)
	{
	QString sname = parList.at(i)->name;
	curParSet.setValue(sname, (*it)->getWidgetValue());
	++it;
	}

	*/
}
void ScaleWidget_Customized::loadParameter()
{
	if (meshDocument->multiSelectID.size() > 0){

		int unit = meshDocument->p_setting.gd_unit;
		switch (unit)
		{
		case 0:
			unitLB->setText(tr("Unit : mm"));
			setUnitRatio(1);
			break;
		case 1:
			unitLB->setText(tr("Unit : inch"));
			setUnitRatio(DSP_inchmm);
			break;
		}

		QList<RichParameter*> tempParList = curParSet.paramList;
		RichFloat *floatTemp = static_cast<RichFloat *>(tempParList.at(0));
		//lineEdit1->setText(QString::number(floatTemp->val->getFloat()));
		//MeshModel *tm = meshDocument->getMesh(*meshDocument->multiSelectID.begin());
		
		if (meshDocument->getMultiSelectID()->size() > 0)
		{
			currentSize = meshDocument->selBBox().Dim();
		}
		else
		{
			MeshModel *tm = meshDocument->mm();			
			currentSize = tm->cm.bbox.Dim();
		}

		//MeshModel *tm = meshDocument->mm();
		////Point3f tp = tm->cm.bbox.min;
		//currentSize = tm->cm.bbox.Dim();

		xUPercentDBSB->blockSignals(true);
		yUPercentDBSB->blockSignals(true);
		zUPercentDBSB->blockSignals(true);

		xUSizeDBSB->blockSignals(true);
		yUSizeDBSB->blockSignals(true);
		zUSizeDBSB->blockSignals(true);

		xUPercentDBSB->setValue(100);
		yUPercentDBSB->setValue(100);
		zUPercentDBSB->setValue(100);

		/*if (currentSize.X() > DSP_grooveBigX) xUSizeDBSB->setMaximum(currentSize.X());
		else xUSizeDBSB->setMaximum(DSP_grooveBigX);

		if (currentSize.Y() > DSP_grooveY) yUSizeDBSB->setMaximum(currentSize.Y());
		else yUSizeDBSB->setMaximum(DSP_grooveY);

		if (currentSize.Z() > DSP_grooveZ) zUSizeDBSB->setMaximum(currentSize.Z());
		else zUSizeDBSB->setMaximum(DSP_grooveZ);*/

		//xUSizeDBSB->setMaximum(grooveXYZ.X());
		//yUSizeDBSB->setMaximum(grooveXYZ.Y());
		//zUSizeDBSB->setMaximum(grooveXYZ.Z());


		xUSizeDBSB->setValue(currentSize.X() / getUnitRatio());
		yUSizeDBSB->setValue(currentSize.Y() / getUnitRatio());
		zUSizeDBSB->setValue(currentSize.Z() / getUnitRatio());


		xUPercentDBSB->blockSignals(false);
		yUPercentDBSB->blockSignals(false);
		zUPercentDBSB->blockSignals(false);

		xUSizeDBSB->blockSignals(false);
		yUSizeDBSB->blockSignals(false);
		zUSizeDBSB->blockSignals(false);
		currentPercent = Point3f(100, 100, 100);
		//applyTransformchange2();

		/*qDebug() << "xTranslateDBSB->value() - tm->cm.bbox.min.X()" << xTranslateDBSB->value() << yTranslateDBSB->value() << zTranslateDBSB->value();
		qDebug() << "max.x.x" << tm->cm.bbox.max.X() << 11 - tmp.X() << tmp.X();*/
	}
}
void ScaleWidget_Customized::getWidgetValue2()
{
	int xt = *meshDocument->multiSelectID.begin();
	//MeshModel *tm = meshDocument->getMesh(*meshDocument->multiSelectID.begin());
	Point3f tmp;
	if (meshDocument->getMultiSelectID()->size() > 0)
	{
		tmp = meshDocument->selBBox().Dim();
	}
	else
	{
		MeshModel *tm = meshDocument->mm();
		//Point3f tmp = tm->cm.bbox.Dim();
	}


	MeshModel *tm = meshDocument->mm();
	//Point3f tmp = tm->cm.bbox.Dim();

	My_DoubleSpinBox *dbsb = (My_DoubleSpinBox *)sender();
	if (dbsb != NULL)
		switch (dbsb->dbsb_name)
	{
		case My_DoubleSpinBox::db_name::xSize:
		{
												 if (uniformCHB->isChecked())
												 {

													 yUSizeDBSB->blockSignals(true);
													 zUSizeDBSB->blockSignals(true);
													 xUPercentDBSB->blockSignals(true);
													 yUPercentDBSB->blockSignals(true);
													 zUPercentDBSB->blockSignals(true);

													 yUSizeDBSB->setValue((tmp.Y()*(xUSizeDBSB->value() * getUnitRatio() / tmp.X())) / getUnitRatio());
													 zUSizeDBSB->setValue((tmp.Z()*(xUSizeDBSB->value() * getUnitRatio() / tmp.X())) / getUnitRatio());

													 xUPercentDBSB->setValue((xUSizeDBSB->value()* getUnitRatio() / currentSize.X()) * 100);
													 yUPercentDBSB->setValue((yUSizeDBSB->value()* getUnitRatio() / currentSize.Y()) * 100);
													 zUPercentDBSB->setValue((zUSizeDBSB->value()* getUnitRatio() / currentSize.Z()) * 100);

													 curParSet.setValue("scale_all_axis", FloatValue(xUSizeDBSB->value()*getUnitRatio() / tmp.X()));
													 curParSet.setValue("Lock_axis", BoolValue(true));

													 yUSizeDBSB->blockSignals(false);
													 zUSizeDBSB->blockSignals(false);
													 xUPercentDBSB->blockSignals(false);
													 yUPercentDBSB->blockSignals(false);
													 zUPercentDBSB->blockSignals(false);
												 }
												 else
												 {
													 yUSizeDBSB->blockSignals(true);
													 zUSizeDBSB->blockSignals(true);
													 xUPercentDBSB->blockSignals(true);
													 yUPercentDBSB->blockSignals(true);
													 zUPercentDBSB->blockSignals(true);

													 xUPercentDBSB->setValue((xUSizeDBSB->value() * getUnitRatio() / currentSize.X()) * 100);
													 curParSet.setValue("X_scale", FloatValue(xUSizeDBSB->value() * getUnitRatio() / tmp.X()));
													 curParSet.setValue("Y_scale", FloatValue(yUSizeDBSB->value() * getUnitRatio() / tmp.Y()));
													 curParSet.setValue("Z_scale", FloatValue(zUSizeDBSB->value() * getUnitRatio() / tmp.Z()));
													 curParSet.setValue("Lock_axis", BoolValue(false));

													 yUSizeDBSB->blockSignals(false);
													 zUSizeDBSB->blockSignals(false);
													 xUPercentDBSB->blockSignals(false);
													 yUPercentDBSB->blockSignals(false);
													 zUPercentDBSB->blockSignals(false);

												 }


		}
			break;
		case My_DoubleSpinBox::db_name::ySize:
		{
												 if (uniformCHB->isChecked())
												 {
													 xUSizeDBSB->blockSignals(true);
													 zUSizeDBSB->blockSignals(true);
													 xUPercentDBSB->blockSignals(true);
													 yUPercentDBSB->blockSignals(true);
													 zUPercentDBSB->blockSignals(true);


													 //QDoubleSpinBox *dbsb = (QDoubleSpinBox*)sender();
													 xUSizeDBSB->setValue((tmp.X()*(yUSizeDBSB->value() * getUnitRatio() / tmp.Y())) / getUnitRatio());
													 zUSizeDBSB->setValue((tmp.Z()*(xUSizeDBSB->value() * getUnitRatio() / tmp.X())) / getUnitRatio());

													 xUPercentDBSB->setValue((xUSizeDBSB->value() * getUnitRatio() / currentSize.X()) * 100);
													 yUPercentDBSB->setValue((yUSizeDBSB->value() * getUnitRatio() / currentSize.Y()) * 100);
													 zUPercentDBSB->setValue((zUSizeDBSB->value() * getUnitRatio() / currentSize.Z()) * 100);

													 curParSet.setValue("scale_all_axis", FloatValue(yUSizeDBSB->value() * getUnitRatio() / tmp.Y()));
													 curParSet.setValue("Lock_axis", BoolValue(true));

													 xUSizeDBSB->blockSignals(false);
													 zUSizeDBSB->blockSignals(false);
													 xUPercentDBSB->blockSignals(false);
													 yUPercentDBSB->blockSignals(false);
													 zUPercentDBSB->blockSignals(false);
												 }
												 else
												 {
													 xUSizeDBSB->blockSignals(true);
													 zUSizeDBSB->blockSignals(true);
													 xUPercentDBSB->blockSignals(true);
													 yUPercentDBSB->blockSignals(true);
													 zUPercentDBSB->blockSignals(true);

													 yUPercentDBSB->setValue((yUSizeDBSB->value() * getUnitRatio() / currentSize.Y()) * 100);
													 curParSet.setValue("X_scale", FloatValue(xUSizeDBSB->value() * getUnitRatio() / tmp.X()));
													 curParSet.setValue("Y_scale", FloatValue(yUSizeDBSB->value() * getUnitRatio() / tmp.Y()));
													 curParSet.setValue("Z_scale", FloatValue(zUSizeDBSB->value() * getUnitRatio() / tmp.Z()));
													 curParSet.setValue("Lock_axis", BoolValue(false));

													 xUSizeDBSB->blockSignals(false);
													 zUSizeDBSB->blockSignals(false);
													 xUPercentDBSB->blockSignals(false);
													 yUPercentDBSB->blockSignals(false);
													 zUPercentDBSB->blockSignals(false);
												 }
		}
			break;
		case My_DoubleSpinBox::db_name::zSize:
		{
												 if (uniformCHB->isChecked())
												 {
													 xUSizeDBSB->blockSignals(true);
													 yUSizeDBSB->blockSignals(true);
													 xUPercentDBSB->blockSignals(true);
													 yUPercentDBSB->blockSignals(true);
													 zUPercentDBSB->blockSignals(true);

													 xUSizeDBSB->setValue(tmp.X()*(zUSizeDBSB->value() / tmp.Z()) );
													 yUSizeDBSB->setValue(tmp.Y()*(zUSizeDBSB->value() / tmp.Z()) );

													 xUPercentDBSB->setValue((xUSizeDBSB->value() * getUnitRatio() / currentSize.X()) * 100);
													 yUPercentDBSB->setValue((yUSizeDBSB->value() * getUnitRatio() / currentSize.Y()) * 100);
													 zUPercentDBSB->setValue((zUSizeDBSB->value() * getUnitRatio() / currentSize.Z()) * 100);

													 curParSet.setValue("scale_all_axis", FloatValue(zUSizeDBSB->value() * getUnitRatio() / tmp.Z()));
													 curParSet.setValue("Lock_axis", BoolValue(true));

													 xUSizeDBSB->blockSignals(false);
													 yUSizeDBSB->blockSignals(false);
													 xUPercentDBSB->blockSignals(false);
													 yUPercentDBSB->blockSignals(false);
													 zUPercentDBSB->blockSignals(false);
												 }
												 else
												 {
													 xUSizeDBSB->blockSignals(true);
													 yUSizeDBSB->blockSignals(true);
													 xUPercentDBSB->blockSignals(true);
													 yUPercentDBSB->blockSignals(true);
													 zUPercentDBSB->blockSignals(true);
													 qDebug() << zUSizeDBSB->value() << getUnitRatio();

													 zUPercentDBSB->setValue((zUSizeDBSB->value() * getUnitRatio() / currentSize.Z()) * 100);
													 curParSet.setValue("X_scale", FloatValue(xUSizeDBSB->value() * getUnitRatio() / tmp.X()));
													 curParSet.setValue("Y_scale", FloatValue(yUSizeDBSB->value() * getUnitRatio() / tmp.Y()));
													 curParSet.setValue("Z_scale", FloatValue(zUSizeDBSB->value() * getUnitRatio() / tmp.Z()));
													 curParSet.setValue("Lock_axis", BoolValue(false));


													 xUSizeDBSB->blockSignals(false);
													 yUSizeDBSB->blockSignals(false);
													 xUPercentDBSB->blockSignals(false);
													 yUPercentDBSB->blockSignals(false);
													 zUPercentDBSB->blockSignals(false);
												 }

		}break;
		case My_DoubleSpinBox::db_name::xPercent:
		{
													if (uniformCHB->isChecked())
													{
														xUSizeDBSB->blockSignals(true);
														yUSizeDBSB->blockSignals(true);
														zUSizeDBSB->blockSignals(true);
														yUPercentDBSB->blockSignals(true);
														zUPercentDBSB->blockSignals(true);

														qDebug() << "((xUPercentDBSB->value() / 100.)*currentSize.X()) / getUnitRatio())" << ((xUPercentDBSB->value() / 100.)*currentSize.X()) / getUnitRatio();

														xUSizeDBSB->setValue(((xUPercentDBSB->value() / 100.)*currentSize.X()) / getUnitRatio());
														yUSizeDBSB->setValue(tmp.Y()*(xUSizeDBSB->value() / tmp.X()));
														zUSizeDBSB->setValue(tmp.Z()*(xUSizeDBSB->value()  / tmp.X()));

														yUPercentDBSB->setValue((yUSizeDBSB->value() * getUnitRatio() / currentSize.Y()) * 100);
														zUPercentDBSB->setValue((zUSizeDBSB->value() * getUnitRatio() / currentSize.Z()) * 100);

														//curParSet.setValue("scale_all_axis", FloatValue(xUSizeDBSB->value() * getUnitRatio() / tmp.X()));
														qDebug() << "xUSizeDBSB->value() * getUnitRatio() / tmp.X())" << xUSizeDBSB->value() * getUnitRatio() / tmp.X();
														qDebug() << "xUSizeDBSB->value() * getUnitRatio() / tmp.X())" << xUSizeDBSB->value() << "  " << tmp.X();
														
														curParSet.setValue("scale_all_axis", FloatValue((xUPercentDBSB->value() / currentPercent.X())));
														curParSet.setValue("Lock_axis", BoolValue(true));

														xUSizeDBSB->blockSignals(false);
														yUSizeDBSB->blockSignals(false);
														zUSizeDBSB->blockSignals(false);
														yUPercentDBSB->blockSignals(false);
														zUPercentDBSB->blockSignals(false);
														currentPercent[0] = xUPercentDBSB->value();
													}
													else
													{
														xUSizeDBSB->blockSignals(true);
														yUSizeDBSB->blockSignals(true);
														zUSizeDBSB->blockSignals(true);
														yUPercentDBSB->blockSignals(true);
														zUPercentDBSB->blockSignals(true);

														double xpercenttemp = xUPercentDBSB->value();
														xUSizeDBSB->setValue(((xpercenttemp / 100)*currentSize.X()) / getUnitRatio());

														/*curParSet.setValue("X_scale", FloatValue(xUSizeDBSB->value() * getUnitRatio() / tmp.X()));
														curParSet.setValue("Y_scale", FloatValue(yUSizeDBSB->value() * getUnitRatio() / tmp.Y()));
														curParSet.setValue("Z_scale", FloatValue(zUSizeDBSB->value() * getUnitRatio() / tmp.Z()));*/
														
														curParSet.setValue("X_scale", FloatValue(xpercenttemp / currentPercent.X()));
														curParSet.setValue("Y_scale", FloatValue(1.));
														curParSet.setValue("Z_scale", FloatValue(1.));
														curParSet.setValue("Lock_axis", BoolValue(false));

														xUSizeDBSB->blockSignals(false);
														yUSizeDBSB->blockSignals(false);
														zUSizeDBSB->blockSignals(false);
														yUPercentDBSB->blockSignals(false);
														zUPercentDBSB->blockSignals(false);
														currentPercent[0] = xUPercentDBSB->value();


													}

		}break;
		case My_DoubleSpinBox::db_name::yPercent:
		{
													if (uniformCHB->isChecked())
													{

														xUSizeDBSB->blockSignals(true);
														yUSizeDBSB->blockSignals(true);
														zUSizeDBSB->blockSignals(true);
														xUPercentDBSB->blockSignals(true);
														zUPercentDBSB->blockSignals(true);

														yUSizeDBSB->setValue(((yUPercentDBSB->value() / 100)*currentSize.Y()) / getUnitRatio());
														xUSizeDBSB->setValue(tmp.X()*(yUSizeDBSB->value()  / tmp.Y()));
														zUSizeDBSB->setValue(tmp.Z()*(yUSizeDBSB->value()  / tmp.Y()));

														xUPercentDBSB->setValue((xUSizeDBSB->value() * getUnitRatio() / currentSize.X()) * 100);
														zUPercentDBSB->setValue((zUSizeDBSB->value() * getUnitRatio() / currentSize.Z()) * 100);

														//curParSet.setValue("scale_all_axis", FloatValue(yUSizeDBSB->value() * getUnitRatio() / tmp.Y()));
														curParSet.setValue("scale_all_axis", FloatValue((yUPercentDBSB->value() / currentPercent.Y())));
														curParSet.setValue("Lock_axis", BoolValue(true));
														currentPercent[1] = yUPercentDBSB->value();

														xUSizeDBSB->blockSignals(false);
														yUSizeDBSB->blockSignals(false);
														zUSizeDBSB->blockSignals(false);
														xUPercentDBSB->blockSignals(false);
														zUPercentDBSB->blockSignals(false);
													}
													else
													{
														xUSizeDBSB->blockSignals(true);
														yUSizeDBSB->blockSignals(true);
														zUSizeDBSB->blockSignals(true);
														xUPercentDBSB->blockSignals(true);
														zUPercentDBSB->blockSignals(true);

														yUSizeDBSB->setValue(((yUPercentDBSB->value() / 100)*currentSize.Y()) / getUnitRatio());

														/*curParSet.setValue("X_scale", FloatValue(xUSizeDBSB->value()  * getUnitRatio() / tmp.X()));
														curParSet.setValue("Y_scale", FloatValue(yUSizeDBSB->value()  * getUnitRatio() / tmp.Y()));
														curParSet.setValue("Z_scale", FloatValue(zUSizeDBSB->value()  * getUnitRatio() / tmp.Z()));*/

														curParSet.setValue("X_scale", FloatValue(1.));
														curParSet.setValue("Y_scale", FloatValue(yUPercentDBSB->value() / currentPercent.Y()));
														curParSet.setValue("Z_scale", FloatValue(1.)); 
														curParSet.setValue("Lock_axis", BoolValue(false));
														currentPercent[1] = yUPercentDBSB->value();

														xUSizeDBSB->blockSignals(false);
														yUSizeDBSB->blockSignals(false);
														zUSizeDBSB->blockSignals(false);
														xUPercentDBSB->blockSignals(false);
														zUPercentDBSB->blockSignals(false);

													}
		}break;
		case My_DoubleSpinBox::db_name::zPercent:
		{
													if (uniformCHB->isChecked())
													{
														xUSizeDBSB->blockSignals(true);
														yUSizeDBSB->blockSignals(true);
														zUSizeDBSB->blockSignals(true);
														xUPercentDBSB->blockSignals(true);
														yUPercentDBSB->blockSignals(true);

														zUSizeDBSB->setValue(((zUPercentDBSB->value() / 100)*currentSize.Z()) / getUnitRatio());
														xUSizeDBSB->setValue(tmp.X()*(zUSizeDBSB->value() / tmp.Z()));
														yUSizeDBSB->setValue(tmp.Y()*(zUSizeDBSB->value() / tmp.Z()));

														xUPercentDBSB->setValue((xUSizeDBSB->value() * getUnitRatio() / currentSize.X()) * 100);
														yUPercentDBSB->setValue((yUSizeDBSB->value() * getUnitRatio() / currentSize.Y()) * 100);

														//curParSet.setValue("scale_all_axis", FloatValue(zUSizeDBSB->value() * getUnitRatio() / tmp.Z()));
														curParSet.setValue("scale_all_axis", FloatValue(zUPercentDBSB->value() / currentPercent.Z()));
														curParSet.setValue("Lock_axis", BoolValue(true));
														currentPercent[2] = zUPercentDBSB->value();

														xUSizeDBSB->blockSignals(false);
														yUSizeDBSB->blockSignals(false);
														zUSizeDBSB->blockSignals(false);
														xUPercentDBSB->blockSignals(false);
														yUPercentDBSB->blockSignals(false);
													}
													else
													{
														xUSizeDBSB->blockSignals(true);
														yUSizeDBSB->blockSignals(true);
														zUSizeDBSB->blockSignals(true);
														xUPercentDBSB->blockSignals(true);
														yUPercentDBSB->blockSignals(true);

														zUSizeDBSB->setValue(((zUPercentDBSB->value() / 100)*currentSize.Z()) / getUnitRatio());

														/*curParSet.setValue("X_scale", FloatValue(xUSizeDBSB->value() * getUnitRatio() / tmp.X()));
														curParSet.setValue("Y_scale", FloatValue(yUSizeDBSB->value() * getUnitRatio() / tmp.Y()));
														curParSet.setValue("Z_scale", FloatValue(zUSizeDBSB->value() * getUnitRatio() / tmp.Z()));*/

														curParSet.setValue("X_scale", FloatValue(1.));
														curParSet.setValue("Y_scale", FloatValue(1.));
														curParSet.setValue("Z_scale", FloatValue(zUPercentDBSB->value() / currentPercent.Z()));
														curParSet.setValue("Lock_axis", BoolValue(false));
														currentPercent[2] = zUPercentDBSB->value();

														xUSizeDBSB->blockSignals(false);
														yUSizeDBSB->blockSignals(false);
														zUSizeDBSB->blockSignals(false);
														xUPercentDBSB->blockSignals(false);
														yUPercentDBSB->blockSignals(false);

													}
		}break;
	}

}

  

void ScaleWidget_Customized::updateXSizeDBSB()
{
	/*xSizeDBSB->setDisabled(true);
	xSizeDBSB->setValue(xPercentDBSB->value()*currentSize.X());*/

}
//void ScaleWidget_Customized::setEnabledWidget(int index)
//{
//	switch (index)
//	{
//	case 0:
//		xPercentDBSB->blockSignals(false);
//		yPercentDBSB->blockSignals(false);
//		zPercentDBSB->blockSignals(false);
//		xPercentDBSB->setDisabled(false);
//		yPercentDBSB->setDisabled(false);
//		zPercentDBSB->setDisabled(false);
//
//		xSizeDBSB->blockSignals(true);
//		ySizeDBSB->blockSignals(true);
//		zSizeDBSB->blockSignals(true);
//		xSizeDBSB->setDisabled(true);
//		ySizeDBSB->setDisabled(true);
//		zSizeDBSB->setDisabled(true);
//		break;
//	case 1:
//		xPercentDBSB->blockSignals(true);
//		yPercentDBSB->blockSignals(true);
//		zPercentDBSB->blockSignals(true);
//		xPercentDBSB->setDisabled(true);
//		yPercentDBSB->setDisabled(true);
//		zPercentDBSB->setDisabled(true);
//
//		xSizeDBSB->blockSignals(false);
//		ySizeDBSB->blockSignals(false);
//		zSizeDBSB->blockSignals(false);
//		xSizeDBSB->setDisabled(false);
//		ySizeDBSB->setDisabled(false);
//		zSizeDBSB->setDisabled(false);
//		break;
//	}
//}

ScaleWidget_Customized::~ScaleWidget_Customized()
{

}

SettingWidget_Customized::SettingWidget_Customized(QAction *editAction, MeshEditInterFace_v2 *mediv2, QWidget *p, QString title , MeshDocument *_meshDocument , QWidget *gla )
{}
SettingWidget_Customized::~SettingWidget_Customized()
{}
void SettingWidget_Customized::init()
{}
void SettingWidget_Customized::applyTransformchange()
{}
void SettingWidget_Customized::resetTBPressed()
{}
void SettingWidget_Customized::landTBClicked()
{}
void SettingWidget_Customized::loadParameter()
{}
void SettingWidget_Customized::getWidgetValue()
{}