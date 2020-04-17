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
#include "CheckListWidget.h"


CheckListWidget::CheckListWidget(Print_Job_Information *_pji, QWidget *parent) :pJI(_pji), QDialog(parent), ui(new Ui::Form)
{
	ui->setupUi(this);

	//ui->printConnectedChb->setDisabled(true);
	//ui->checkBox_3->setDisabled(true);
	ui->start_printingBtn->setDisabled(true);

	initButton();
	intiInkStackWidget();

	ui->stackedWidget->setCurrentIndex(5);

	//connect(ui->printConnectTB, SIGNAL(clicked()), this, SLOT(stackWidgetSetToPrintConnect()));
	connect(ui->inkReadyTB, SIGNAL(clicked()), this, SLOT(stackWidgetSetToInkReady()));
	connect(ui->inkReadyRefreshTB, SIGNAL(clicked()), this, SLOT(stackWidgetSetToInkReady()));
	connect(ui->powderReadyTB, SIGNAL(clicked()), this, SLOT(stackWidgetSetToPowerReady()));
	connect(ui->wasteTankReadyTB, SIGNAL(clicked()), this, SLOT(stackWidgetSetToWasteTankReady()));
	connect(ui->printerZ1Z2TB, SIGNAL(clicked()), this, SLOT(stackWidgetSetToZ1Z2Ready()));

	
	connect(ui->inkChB, SIGNAL(clicked()), this, SLOT(stackWidgetSetToInkReady()));
	connect(ui->powerReadyCHB, SIGNAL(clicked()), this, SLOT(stackWidgetSetToPowerReady()));
	connect(ui->wasteTankChb, SIGNAL(clicked()), this, SLOT(stackWidgetSetToWasteTankReady()));
	connect(ui->printZ1Z1Chb, SIGNAL(clicked()), this, SLOT(stackWidgetSetToZ1Z2Ready()));
	connect(ui->wiper_Check_CHB, SIGNAL(clicked()), this, SLOT(stackWidgetSetToWiperReady()));




	connect(ui->skip_checkingBtn, SIGNAL(clicked()), this, SLOT(skip_checking()));
	connect(ui->start_printingBtn, SIGNAL(clicked()), this, SLOT(startPrint_accepted()));

	connect(ui->printZ1Z1Chb, SIGNAL(stateChanged(int)), this, SLOT(checkListIsReady()));

	connect(ui->inkChB, SIGNAL(clicked()), this, SLOT(checkListIsReady()));

	connect(ui->powerReadyCHB, SIGNAL(clicked()), this, SLOT(checkListIsReady()));
	connect(ui->wasteTankChb, SIGNAL(clicked()), this, SLOT(checkListIsReady()));

	connect(ui->wiper_Check_CHB, SIGNAL(clicked()), this, SLOT(checkListIsReady()));
	//connect(ui->show_connectTB, SIGNAL(clicked()), this, SLOT(getPrintStatus()));

	//connect()
	//Comm3DP::openProc();
	//for (int i = 0; i < 10000; i++);

	//ui->printConnectTB->setVisible(false);
	//ui->printConnectedChb->setVisible(false);
	//ui->printerReadyFreshTB->setVisible(false);

	//***init ink  Table Widget
	for (int i = 0; i < 8; i++)
	{
		item[i] = new QTableWidgetItem();
		item[i]->setFlags(item[i]->flags() ^ (Qt::ItemIsEditable | Qt::ItemIsSelectable));
	}
	if (pJI)updateCMYBUsage();
	//ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	//***init Z1 Z2 Table Widget
	for (int i = 0; i < 4; i++)
	{
		spaceItem[i] = new QTableWidgetItem();
		spaceItem[i]->setFlags(spaceItem[i]->flags() ^ (Qt::ItemIsEditable | Qt::ItemIsSelectable));
	}
	ui->z1Z2Table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->z1Z2Table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	/*
	wiper table
	*/
	for (int i = 0; i < 2; i++)
	{
		wiperItem[i] = new QTableWidgetItem();
		wiperItem[i]->setFlags(spaceItem[i]->flags() ^ (Qt::ItemIsEditable | Qt::ItemIsSelectable));
	}
	ui->wiper_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->wiper_table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);



	unsigned int c, m, y, b;
	if (comm->inkStatus(c, m, y, b))
	{
		updateInkStatus2(c, m, y, b);
	}
	else
	{
		ui->ink_label0->setText("cannot get ink status");
	}

	ui->cyan_lb->setVisible(false);
	ui->magenta_lb->setVisible(false);
	ui->yellow_lb->setVisible(false);
	ui->binder_lb->setVisible(false);
	ui->ink_TB->setVisible(false);

	checkListTimer = new QTimer(this);

	connect(checkListTimer, SIGNAL(timeout()), this, SLOT(updatePrintStatus()));

	checkListTimer->start(500);
	setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);

}
void CheckListWidget::checkListIsReady()
{
	if (ui->inkChB->isChecked() && ui->powerReadyCHB->isChecked() && ui->wasteTankChb->isChecked() && getbuilderFeederReady() && ui->wiper_Check_CHB->isChecked())
	{
		ui->start_printingBtn->setDisabled(false);
	}
	else
	{
		ui->start_printingBtn->setDisabled(true);
	}
}
void CheckListWidget::stackWidgetSetToPrintConnect()
{
	ui->stackedWidget->setCurrentIndex(1);

}
void CheckListWidget::stackWidgetSetToInkReady()
{
	ui->stackedWidget->setCurrentIndex(2);
	unsigned int c, m, y, b;
	if (comm->inkStatus(c, m, y, b))
	{
		//updateInkStatus(c, m, y, b);
		updateInkStatus2(c, m, y, b);

	}
	else
	{
		//updateInkStatus2(0, 0, 0, 0);
		ui->ink_label0->setText("cannot get ink status");
	}
}
void CheckListWidget::stackWidgetSetToWiperReady()
{
	ui->stackedWidget->setCurrentIndex(6);
	ui->wiperWebView->load(QUrl(QStringLiteral("qrc:/images/htmlpage/Wiper.html")));
	ui->wiperWebView->show();
	int estimateWiper = pJI->wiperEstimateUsage.value;
	int wiperRemain;
	bool result = comm->getWiperCount(wiperRemain);
	int wiperReadyCount = wiperRemain - estimateWiper;

	/*QString wiperReady("Wiper is ready. Wiper remains (%1) steps\n");
	QString wiperNotReady("Wiper isn't enough to print.\nWiper remains %1 clicks\n.It needs %2 clicks");


	if (result && wiperReadyCount > 0)
	{
		ui->wiperLB->setText(wiperReady.arg(QString::number(wiperRemain)));
		ui->wiperLB->setStyleSheet
			(
			"QLabel{"
			"font: 14pt \"Arial\";"
			"color:rgb(255, 64, 0);"
			"}"
			);

	}
	else
	{
		ui->wiperLB->setText(wiperNotReady.arg(QString::number(wiperRemain)).arg(QString::number(estimateWiper)));
		ui->wiperLB->setStyleSheet
			(
			"QLabel{"
			"font: 14pt \"Arial\";"
			"color: rgb(255, 0, 0);"
			"}"
			);



	}*/

	QString wiperReady("%1");
	QString wiperNotReady(">%1");


	if (result && wiperReadyCount > 0)
	{		
		wiperItem[0]->setTextColor(QColor(0, 0, 0));
		wiperItem[0]->setText(QString::number(wiperRemain));
		wiperItem[1]->setText(QString::number(estimateWiper));

	}
	else
	{		
		wiperItem[0]->setTextColor(QColor(255, 0, 0));
		wiperItem[0]->setText(QString::number(wiperRemain));
		wiperItem[1]->setText(QString::number(estimateWiper));



	}


	


	ui->wiper_table->setItem(0, 0, wiperItem[0]);
	ui->wiper_table->setItem(1, 0, wiperItem[1]);




}


void CheckListWidget::stackWidgetSetToPowerReady()
{
	ui->stackedWidget->setCurrentIndex(3);

	ui->powerReadyWV->load(QUrl(QStringLiteral("qrc:/images/htmlpage/printBedReady.html")));
	ui->powerReadyWV->show();


	//unsigned int cmy[4];
	//comm->inkStatus(cmy[0],cmy[1], cmy[2], cmy[3]);
	//ui->ink_label0->setText("");
}
void CheckListWidget::stackWidgetSetToWasteTankReady()
{
	ui->stackedWidget->setCurrentIndex(4);
	ui->tankReadyWV->load(QUrl(QStringLiteral("qrc:/images/htmlpage/wasteTankReady.html")));
	ui->tankReadyWV->show();

}
void CheckListWidget::stackWidgetSetToZ1Z2Ready()
{

	ui->stackedWidget->setCurrentIndex(5);

}

void CheckListWidget::initButton()
{
	QIcon *ico = new QIcon();
	ico->addPixmap(QPixmap(":/images/SKT_images/connect.png"), QIcon::Normal, QIcon::On);
	ico->addPixmap(QPixmap(":/images/SKT_images/disconnect.png"), QIcon::Normal, QIcon::Off);
	ui->show_connectTB->setIcon(*ico);
	ui->show_connectTB->setFixedSize(100, 100);
	ui->show_connectTB->setIconSize(QSize(100, 100));
	ui->show_connectTB->setCheckable(true);

	//*************************************
	//ui->printerReadyFreshTB->setIcon(QIcon(":/images/SKT_images/refresh.jpg"));

	ui->inkReadyRefreshTB->setIcon(QIcon(":/images/SKT_images/refresh.jpg"));
	ui->printerZ1Z2RefreshTB->setIcon(QIcon(":/images/SKT_images/refresh.jpg"));

	ui->powderReadyTB->setVisible(false);;
	ui->printerZ1Z2TB->setVisible(false);;
	ui->printerZ1Z2RefreshTB->setVisible(false);;
	ui->inkReadyTB->setVisible(false);;
	ui->inkReadyRefreshTB->setVisible(false);;
	ui->powderReadyTB->setVisible(false);;
	ui->wasteTankReadyTB->setVisible(false);;





	//ui->printZ1Z1Chb->setAttribute(Qt::WA_TransparentForMouseEvents);
	//ui->printZ1Z1Chb->setFocusPolicy(Qt::NoFocus);
	

	/*ui->inkChB->setAttribute(Qt::WA_TransparentForMouseEvents);
	ui->inkChB->setFocusPolicy(Qt::NoFocus);*/
}
bool CheckListWidget::initComm()
{
	//***
	/*if (comm->initFail())
	{
	initcccc = false;
	return 0;
	}
	else {
	initcccc = true;
	return 1;
	}*/
	return 0;
}
void CheckListWidget::updatePrintStatus()
{
	//initComm();
	//std::string s;
	//comm->printerStatus(s);
	//QString temp = QString::fromStdString(s);	
	//if (temp.contains("Online", Qt::CaseSensitive))
	//{
	//	ui->show_connectTB->setChecked(true);
	//	ui->printConnectedChb->setChecked(true);
	//}
	//else //if (temp.contains("", Qt::CaseSensitive))
	//{
	//	ui->show_connectTB->setChecked(false);//what if not connected

	//	QMessageBox mb(QMessageBox::NoIcon, "Please Try again", "Please Try again", QMessageBox::NoButton, this);
	//	//mb.setWindowTitle(tr("MeshLab"));
	//	QPushButton *submitBug = mb.addButton("try connect",QMessageBox::AcceptRole);
	//	mb.addButton(QMessageBox::Cancel);
	//	mb.setText(tr(""
	//	    "Please Try again.\n"
	//	    ) );
	//	mb.setInformativeText(	tr(
	//	    "Press button to retry connect printer\n"
	//	    ) );

	//	mb.exec();

	//	if (mb.clickedButton() == submitBug)
	//	{
	//		ui->show_connectTB->click();
	//	}				
	//}


	std::string s, e;
	comm->printerStatus(s, e);
	QString tempStatus = QString::fromStdString(s);
	QString errorStatus = QString::fromStdString(e);
	bool getStatus = true;

	std::map<std::string, int>::iterator it;
	it = comm->printer3dpString.printerStatusmap.find(s);
	if (it != comm->printer3dpString.printerStatusmap.end())
	{

		switch (comm->printer3dpString.printerStatusmap.at(s))
		{
		case Comm3DP::printer3dpString::printStatus::Online://pause 
		{

		}break;
		case Comm3DP::printer3dpString::printStatus::PanelControl://pause 
		{

																	  getStatus = false;
		}break;
		default:
			getStatus = true;

		}
	}
	if (getStatus){
		unsigned int c, m, y, b;
		if (comm->inkStatus(c, m, y, b))
		{
			//updateInkStatus(c, m, y, b);
			updateInkStatus2(c, m, y, b);

		}
		else
		{
			//updateInkStatus2(0, 0, 0, 0);
			ui->ink_label0->setText("cannot get ink status");
			ui->skip_checkingBtn->setEnabled(false);
			ui->inkChB->setChecked(false);

		}

		float builder, feeder;
		bool ret = false;
		if (comm->zAxis(builder, feeder))
		{
			ret = updateZAxis(builder, feeder);

		}
		else
		{
			ui->Z1_LB->setText("cannot get z1 z2 status");
			ui->printZ1Z1Chb->setChecked(false);
		}
		setbuilderFeederReady(ret);

		//updateZAxis(builder, feeder);
	}



}
bool CheckListWidget::updateZAxis(float builder, float feeder)
{
	/*builder = 0;
	feeder = 0;*/
	/////////////////////////////////////////
	//***200*****350
	//***0*******0
	////////////////////////////////////////
	int safeBuilderHeight = 0;
	int safeFeederHeight = 0;
	//safeBuilderHeight = pJI->layerThickness.value * pJI->numoflayer.value;
	safeBuilderHeight = 0.1016 * pJI->numoflayer.value;
	safeFeederHeight = DSP_Feeder_Height - (safeBuilderHeight * DSP_Feeder_Up_Ratio + 15);
	QString label("Builder(%1) need to be higher\n");
	QString label2("Feeder(%1) need to be lower");
	QString label3("%1 height : Ready\n");
	if ((safeBuilderHeight > builder))
	{
		//not enough builder space,spaceItem0, spaceItem1 become red
		spaceItem[0]->setTextColor(QColor(128, 0, 0));
		ui->Z1_LB->setText(label.arg(QString::number(builder)));
		ui->Z1_LB->setStyleSheet("QLabel{ color: rgb(255, 0, 0);}");

	}
	else
	{
		//spaceItem0, spaceItem1 become green
		spaceItem[0]->setTextColor(QColor(0, 64, 0));
		ui->Z1_LB->setText(label3.arg(QString("Builder")));
		ui->Z1_LB->setStyleSheet("QLabel{ color: rgb(0, 64, 0);}");
	}


	if (feeder > 0 && feeder > safeFeederHeight)
	{
		//not enough feeder space
		spaceItem[1]->setTextColor(QColor(128, 0, 0));
		ui->Z2_LB->setText(label2.arg(QString::number(feeder)));
		ui->Z2_LB->setStyleSheet("QLabel{ color: rgb(128, 0, 0);}");
	}
	else
	{
		spaceItem[1]->setTextColor(QColor(0, 64, 0));
		ui->Z2_LB->setText(label3.arg(QString("Feeder")));
		ui->Z2_LB->setStyleSheet("QLabel{ color: rgb(0,64, 0);}");
	}

	/*if((builder < 0 || (safeBuilderHeight > builder)) || (feeder<0 || feeder > safeFeederHeight))
	{
	ui->skip_checkingBtn->setEnabled(false);
	ui->printZ1Z1Chb->setChecked(false);
	}else
	{
	ui->skip_checkingBtn->setEnabled(true);
	ui->printZ1Z1Chb->setChecked(true);
	}*/




	//*****table
	//*****01
	//*****23
	spaceItem[0]->setText(QString("%1mm").arg(builder));
	spaceItem[1]->setText(QString("%1mm").arg(feeder));

	spaceItem[2]->setText(QString("> %1mm").arg(safeBuilderHeight));
	spaceItem[3]->setText(QString("< %1mm").arg(safeFeederHeight));


	ui->z1Z2Table->setItem(0, 0, spaceItem[0]);
	ui->z1Z2Table->setItem(0, 1, spaceItem[1]);
	ui->z1Z2Table->setItem(1, 0, spaceItem[2]);
	ui->z1Z2Table->setItem(1, 1, spaceItem[3]);

	if (builder > safeBuilderHeight && (feeder >= 0 && feeder < safeFeederHeight))
	{
		ui->skip_checkingBtn->setEnabled(true);
		ui->printZ1Z1Chb->setChecked(true);
		return true;
	}
	else
	{
		ui->skip_checkingBtn->setEnabled(false);
		ui->printZ1Z1Chb->setChecked(false);

		/*
		herbii Test
		*/
		/*ui->skip_checkingBtn->setEnabled(true);
		ui->printZ1Z1Chb->setChecked(true);*/
		return false;
	}
	return true;
}
bool CheckListWidget::sendHeader(Print_Job_Information &pji, int fanspeed, double layerHeight)
{
	int xlayerHeight = (layerHeight * 10000 + 0.5);
	if (comm->sendPrintjobHeader(pji.numoflayer.value, xlayerHeight, 'L', fanspeed))
		//if (comm->sendPrintjobHeader(pji.numoflayer.value, 10, 'L', fanspeed))
		return true;
	else return false;
}
void CheckListWidget::startPrint_accepted()
{

	accept();

}
void CheckListWidget::skip_checking()
{
	ui->start_printingBtn->setEnabled(true);

}

void CheckListWidget::updateInkStatus(unsigned int cPercent, unsigned int mPercent, unsigned int yPercent, unsigned int bPercent)
{
	drawPainter(cyanPainter, cyanPixmap, ui->cyan_lb, cPercent, QColor(Qt::cyan), QColor(Qt::lightGray), QColor(223, 223, 223), 8, QSize(50, 50));
	drawPainter(magentaPainter, magentaPixmap, ui->magenta_lb, mPercent, QColor(Qt::magenta), QColor(Qt::lightGray), QColor(223, 223, 223), 8, QSize(50, 50));
	drawPainter(yellowPainter, yellowPixmap, ui->yellow_lb, yPercent, QColor(Qt::yellow), QColor(Qt::lightGray), QColor(223, 223, 223), 8, QSize(50, 50));
	drawPainter(binderPainter, binderPixmap, ui->binder_lb, bPercent, QColor(Qt::black), QColor(Qt::lightGray), QColor(223, 223, 223), 8, QSize(50, 50));

}

void CheckListWidget::updateInkStatus2(unsigned int cPercent, unsigned int mPercent, unsigned int yPercent, unsigned int bPercent)
{


	item[0]->setText(QString("%1%").arg(cPercent));
	item[1]->setText(QString("%1%").arg(mPercent));
	item[2]->setText(QString("%1%").arg(yPercent));
	item[3]->setText(QString("%1%").arg(bPercent));

	item[4]->setText(QString("%1%").arg(pJI->estimated_cyan.value / 20., 0, 'f', 2));
	item[5]->setText(QString("%1%").arg(pJI->estimated_magenta.value / 20., 0, 'f', 2));
	item[6]->setText(QString("%1%").arg(pJI->estimated_yellow.value / 20., 0, 'f', 2));
	item[7]->setText(QString("%1%").arg(pJI->estimatedBinderUsage.value / 20., 0, 'f', 2));

	ui->tableWidget->setItem(0, 0, item[0]);
	ui->tableWidget->setItem(0, 1, item[1]);
	ui->tableWidget->setItem(0, 2, item[2]);
	ui->tableWidget->setItem(0, 3, item[3]);

	ui->tableWidget->setItem(1, 0, item[4]);
	ui->tableWidget->setItem(1, 1, item[5]);
	ui->tableWidget->setItem(1, 2, item[6]);
	ui->tableWidget->setItem(1, 3, item[7]);



	QString message;

	bool flag = false;

	if (cPercent < pJI->estimated_cyan.value / 20.)//***pJI->estimated_cyan.value/2000*100
	{
		item[0]->setBackgroundColor(QColor(255, 0, 0));
		item[4]->setBackgroundColor(QColor(255, 0, 0));
		message.append("cyan");
		flag = true;
	}
	if (mPercent < pJI->estimated_magenta.value / 20.)
	{
		item[1]->setBackgroundColor(QColor(255, 0, 0));
		item[5]->setBackgroundColor(QColor(255, 0, 0));
		message.append(", magenta");
		flag = true;
	}
	if (yPercent < pJI->estimated_yellow.value / 20.)
	{
		item[2]->setBackgroundColor(QColor(255, 0, 0));
		item[6]->setBackgroundColor(QColor(255, 0, 0));
		message.append(", yellow");
		flag = true;
	}
	if (bPercent < pJI->estimatedBinderUsage.value / 20.)
	{
		item[3]->setBackgroundColor(QColor(255, 0, 0));
		item[7]->setBackgroundColor(QColor(255, 0, 0));
		message.append(", binder\n");
		flag = true;
	}

	if (flag)ui->ink_label0->setText(message.append(" is not enough"));




}




void CheckListWidget::intiInkStackWidget()
{
	int half_penWidth = 8;
	QSize pixmapSize(50, 50);

	rect1 = new QRect(half_penWidth, half_penWidth, pixmapSize.width() - 2 * half_penWidth, pixmapSize.height() - 2 * half_penWidth);

	cyanPixmap = new QPixmap(pixmapSize);
	cyanPainter = new QPainter(cyanPixmap);

	magentaPixmap = new QPixmap(pixmapSize);
	magentaPainter = new QPainter(magentaPixmap);

	yellowPixmap = new QPixmap(pixmapSize);
	yellowPainter = new QPainter(yellowPixmap);

	binderPixmap = new QPixmap(pixmapSize);
	binderPainter = new QPainter(binderPixmap);


	QFont serifFont("Times", 6, QFont::Bold);


	drawPainter(cyanPainter, cyanPixmap, ui->cyan_lb, 0, QColor(Qt::cyan), QColor(Qt::lightGray), QColor(223, 223, 223), 8, QSize(50, 50));
	drawPainter(magentaPainter, magentaPixmap, ui->magenta_lb, 0, QColor(Qt::magenta), QColor(Qt::lightGray), QColor(223, 223, 223), 8, QSize(50, 50));
	drawPainter(yellowPainter, yellowPixmap, ui->yellow_lb, 0, QColor(Qt::yellow), QColor(Qt::lightGray), QColor(223, 223, 223), 8, QSize(50, 50));
	drawPainter(binderPainter, binderPixmap, ui->binder_lb, 0, QColor(Qt::black), QColor(Qt::lightGray), QColor(223, 223, 223), 8, QSize(50, 50));


	//ui->cyan_lb->

}
void CheckListWidget::drawPainter(QPainter *painter, QPixmap *pixmap, QLabel *label, unsigned int percentQ, QColor arcColor, QColor arcColor2, QColor recColor, int half_penWidth, QSize mapSize)
{
	QRect rect(half_penWidth, half_penWidth, mapSize.width() - 2 * half_penWidth, mapSize.height() - 2 * half_penWidth);
	QPen *penArc = new QPen(arcColor, half_penWidth * 2);
	QPen *penRemainArc = new QPen(arcColor2, half_penWidth * 2);

	penArc->setCapStyle(Qt::FlatCap);
	//penArc->setStyle(Qt::DotLine);
	penRemainArc->setCapStyle(Qt::FlatCap);

	QFont serifFont("Times", 6, QFont::Bold);

	painter->fillRect(rect, recColor);

	int temp = (percentQ * 36 / 10) * 16;
	painter->setPen(*penArc);
	painter->drawArc(rect, 1440, -temp);

	int temp2 = -(360 * 16 - temp);
	painter->setPen(*penRemainArc);
	painter->drawArc(rect, 1440 - temp, temp2);


	painter->setFont(serifFont);
	painter->setPen(Qt::black);
	painter->drawText(20, 20, 15, 15, Qt::AlignHCenter | Qt::AlignVCenter, QString::number(percentQ).append("%"));
	label->setPixmap(*pixmap);


}
void CheckListWidget::updateCMYBUsage()
{
	item[4]->setText(QString("%1%").arg(QString::number(pJI->estimated_cyan.value / 20., 'f', 2)));
	item[5]->setText(QString("%1%").arg(QString::number(pJI->estimated_magenta.value / 20., 'f', 2)));
	item[6]->setText(QString("%1%").arg(QString::number(pJI->estimated_yellow.value / 20., 'f', 2)));
	item[7]->setText(QString("%1%").arg(QString::number(pJI->estimatedBinderUsage.value / 20., 'f', 2)));
	ui->tableWidget->setItem(1, 0, item[4]);
	ui->tableWidget->setItem(1, 1, item[5]);
	ui->tableWidget->setItem(1, 2, item[6]);
	ui->tableWidget->setItem(1, 3, item[7]);
	ui->tableWidget->repaint();


}
CheckListWidget::~CheckListWidget()
{
	//comm->closeProc();
	checkListTimer->destroyed();
}
