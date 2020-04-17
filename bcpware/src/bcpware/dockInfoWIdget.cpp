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
#include "dockInfoWIdget.h"


DockInfoWIdget::DockInfoWIdget(QWidget *parent) :QWidget(parent), ui(new Ui::testInfoForm)
{
	ui->setupUi(this);
	//ui->retranslateUi(this);
	uiInit();
	ui->singleOrAllTB->setCheckable(true);
	ui->singleOrAllTB->setChecked(false);
	ui->buildHeightVaLa->setWordWrap(true);
	ui->dimensionVaLa->setWordWrap(true);
	ui->facesVaLa->setWordWrap(true);
	ui->nameVaLa->setWordWrap(true);
	ui->positionVaLa->setWordWrap(true);
	ui->verticesVaLa->setWordWrap(true);

	ui->printJobEstimateTB->setVisible(false);
	ui->singleOrAllTB->setVisible(false);
	//connect(ui->singleOrAllTB, SIGNAL(toggled(bool)), this, SLOT(sOAToggled(bool)));
	//connect(ui->printJobEstimateTB, SIGNAL(pressed()), this, SLOT(triggerPrintEstimate()));
	//sOAToggled(ui->singleOrAllTB->isChecked());
}
void DockInfoWIdget::updateUi(QStringList SL,bool oneOrMulti)
{
	if (oneOrMulti)
		ui->nameLa->setText(tr("Original Name"));
	else
		ui->nameLa->setText(tr("Number of objects"));
	ui->nameVaLa->setText(SL.at(0));
	ui->buildHeightVaLa->setText(SL.at(1));
	ui->dimensionVaLa->setText(SL.at(2));
	ui->verticesVaLa->setText(SL.at(3));
	ui->facesVaLa->setText(SL.at(4));
	ui->positionVaLa->setText(SL.at(5));

}
void DockInfoWIdget::sOAToggled(bool bo)
{
	if (!bo)
	{
		ui->singleOrAllTB->setStyleSheet(WidgetStyleSheet::viewDockWidgetBottomTB(":/images/icons/btn_all_object.png", ":/images/icons/btn_all_object_disable.png"));
		ui->singleOrAllTB->setText(tr("      ALL OBJECT"));
		ui->nameLa->setText(tr("Original Name"));
		emit sOAswitch();
	}
	else
	{
		ui->nameLa->setText(tr("Number of objects"));
		ui->singleOrAllTB->setStyleSheet(WidgetStyleSheet::viewDockWidgetBottomTB(":/images/icons/btn_single_object.png", ":/images/icons/btn_single_object_disable.png"));
		ui->singleOrAllTB->setText(tr("        SINGLE OBJECT"));
		emit sOAswitch();
	}
}
void DockInfoWIdget::uiInit()
{
	ui->infoTab->setText(tr("INFO"));
	ui->infoTab->setStyleSheet(WidgetStyleSheet::viewTextStyleSheet(":/images/icons/tab_name_bg_on.png", ":/images/icons/tab_name_bg_off.png"));
	ui->nameLa->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	ui->nameVaLa->setStyleSheet(WidgetStyleSheet::textStyleSheet2());
	ui->buildHeightLa->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	ui->buildHeightVaLa->setStyleSheet(WidgetStyleSheet::textStyleSheet2());
	ui->dimensionLa->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	ui->dimensionVaLa->setStyleSheet(WidgetStyleSheet::textStyleSheet2());
	ui->facesLa->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	ui->facesVaLa->setStyleSheet(WidgetStyleSheet::textStyleSheet2());
	ui->verticesLa->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	ui->verticesVaLa->setStyleSheet(WidgetStyleSheet::textStyleSheet2());
	ui->positionLa->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	ui->positionVaLa->setStyleSheet(WidgetStyleSheet::textStyleSheet2());
	ui->printJobEstimateTB->setText(tr("         ESTIMATE USAGE"));
	ui->printJobEstimateTB->setStyleSheet(WidgetStyleSheet::viewDockWidgetBottomTB(":/images/icons/btn_print_estimate.png", ":/images/icons/btn_print_estimate_disable.png"));

}
void DockInfoWIdget::triggerPrintEstimate()
{
	emit estimateSIG();
}




DockInfoWIdget::~DockInfoWIdget()
{
}
