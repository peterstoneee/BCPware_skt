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
#include "printOption.h"
#include <QDir>
#include <QMessageBox>
#include <tchar.h>
#include "skt_function.h"




PrintOption::PrintOption(optionMode mode, QWidget *parent, MeshDocument *_meshdoc) :QDialog(parent), om(mode), meshDoc(_meshdoc), ui(new Ui::PrintOptionDiaUi)
{
	//dbgf = fopen("D:\\PrintOption.txt", "w");//@@@
	
	ui->setupUi(this);
	initComm();
	/*ui->wiperFrequencyLE->setValidator(new QIntValidator(0, 1000, this));
	ui->wiperFrequencyLE->setText("8");

	ui->wiperIndexLE->setValidator(new QIntValidator(0, 1000, this));
	ui->wiperIndexLE->setText("2");

	ui->wiperClickLE->setValidator(new QIntValidator(0, 1000, this));
	ui->wiperClickLE->setText("1");*/
	ui->refreshTB->setVisible(false);
	ui->heatCHB->setVisible(false);
	ui->line_2->setVisible(false);
	ui->heatLE5->setVisible(false);
	ui->ColorModeL_3->setVisible(false);
	ui->heatLE5->setEnabled(false);
	ui->ColorModeL_3->setEnabled(false);


	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(getPrintStatus()));
	timer->start(1000);
	//connect(ui->refreshTB, SIGNAL(clicked()), this, SLOT(getPrintStatus()));

	connect(ui->heatCHB, SIGNAL(toggled(bool)), this, SLOT(showHeatSetting(bool)));

	ui->spmLB->setVisible(false);
	ui->color_TestPageLB->setVisible(false);
	switch (mode)
	{
	case MONO_PRINT_MODE:
	{
		setWindowTitle("Prepare for Printing");
		ui->printListCB->setEditable(true);
		connect(ui->prepareBtn, SIGNAL(clicked()), this, SLOT(gotoCheckListDialog()));
		
		ui->webView->hide();
		ui->printScrollArea->setWidget(ui->printSummaryFrame);
		ui->printSummaryFrame->setMinimumHeight(760);

		initPOL();
		initItem();

		showHeatSetting(false);
		

		ui->heatLE5->setText(QString::number(120));
		ui->heatLE6->setText(QString::number(45));
		ui->heatLE7->setText(QString::number(40));

		bool uselsessForMono = false;
		ui->mono_or_color->setVisible(uselsessForMono);
		ui->ColorModeL->setVisible(uselsessForMono);
		ui->line->setVisible(uselsessForMono);
		ui->line_3->setVisible(uselsessForMono);
		ui->frame_2->setVisible(uselsessForMono);

		//fprintf(dbgf, "0\n");//@@@
		//fflush(dbgf);
	}break;
	case NORMAL_PRINT_MODE:
	{
		setWindowTitle("Prepare for Printing");
		ui->printListCB->setEditable(true);
		connect(ui->prepareBtn, SIGNAL(clicked()), this, SLOT(gotoCheckListDialog()));
		//ui->webView->load(QUrl(QStringLiteral("qrc:/images/htmlpage/htmlTestFile.html")));
		//ui->webView->show();
		ui->webView->hide();
		ui->printScrollArea->setWidget(ui->printSummaryFrame);
		ui->printSummaryFrame->setMinimumHeight(760);

		initPOL();//set style sheet
		//ui->comboBox->currentText
		initItem();

		showHeatSetting(false);
		/*
		  Heat stuff
		  */
		/* ui->heatLE1->setText(QString::number(0));
		 ui->heatLE2->setText(QString::number(40));
		 ui->heatLE3->setText(QString::number(45));
		 ui->heatLE4->setText(QString::number(45));*/

		ui->heatLE5->setText(QString::number(120));
		ui->heatLE6->setText(QString::number(45));
		ui->heatLE7->setText(QString::number(40));
		/* receiveStatus.insert("Online", 0);
		 receiveStatus.insert("Offline", 1);
		 receiveStatus.insert("GettingError", 2);
		 receiveStatus.insert("", 3);*/

		//fprintf(dbgf, "0\n");//@@@
		//fflush(dbgf);


	}
	break;
	case PRINT_ZXA_MONO_FILE:
	{
		setWindowTitle("Prepare for Printing");
		ui->printListCB->setEditable(true);
		connect(ui->prepareBtn, SIGNAL(clicked()), this, SLOT(gotoCheckListDialog()));
	
		ui->webView->hide();
		ui->printScrollArea->setWidget(ui->printSummaryFrame);
		ui->printSummaryFrame->setMinimumHeight(760);

		initPOL();//set style sheet
		//ui->comboBox->currentText
		initItem();
					  
		showHeatSetting(false);
		
		ui->heatLE5->setText(QString::number(120));
		ui->heatLE6->setText(QString::number(45));
		ui->heatLE7->setText(QString::number(40));


		bool uselsessForMono = false;
		ui->mono_or_color->setVisible(uselsessForMono);
		ui->ColorModeL->setVisible(uselsessForMono);
		ui->line->setVisible(uselsessForMono);
		ui->line_3->setVisible(uselsessForMono);
		

	}break;
	case PRINT_ZXA_FILE:
	{
		setWindowTitle("Prepare for Printing");
		ui->printListCB->setEditable(true);
		connect(ui->prepareBtn, SIGNAL(clicked()), this, SLOT(gotoCheckListDialog()));
		//ui->webView->load(QUrl(QStringLiteral("qrc:/images/htmlpage/htmlTestFile.html")));
		//ui->webView->show();
		ui->webView->hide();
		ui->printScrollArea->setWidget(ui->printSummaryFrame);
		ui->printSummaryFrame->setMinimumHeight(760);

		initPOL();//set style sheet
		//ui->comboBox->currentText
		initItem();
		//timer = new QTimer(this);
		//connect(timer, SIGNAL(timeout()), this, SLOT(getPrintStatus()));
		//timer->start(1000);
		////connect(ui->refreshTB, SIGNAL(clicked()), this, SLOT(getPrintStatus()));						  
		showHeatSetting(false);
		/*
		  heat stuff
		  */
		/*ui->heatLE1->setText(QString::number(0));
		ui->heatLE2->setText(QString::number(40));
		ui->heatLE3->setText(QString::number(45));
		ui->heatLE4->setText(QString::number(45));*/

		ui->heatLE5->setText(QString::number(120));
		ui->heatLE6->setText(QString::number(45));
		ui->heatLE7->setText(QString::number(40));

	}break;
	case TEST_PRINT_PAGE_MODE:
	{

		ui->printListCB->setEditable(true);
		connect(ui->prepareBtn, SIGNAL(clicked()), this, SLOT(gotoCheckListDialog()));
		//ui->heatCHB->setVisible(false);
		showHeatSetting(false);
		setWindowTitle("Test Print");
		//ui->webView->load(QUrl(QStringLiteral("qrc:/images/htmlpage/htmlTestFile.html")));
		//ui->webView->show();

		//ui->printScrollArea->setWidget(ui->webView);
		//initTestPrintItem();
		//ui->webView->load(QUrl(QStringLiteral("qrc:/images/htmlpage/htmlTestFile.html")));
		//ui->webView->show();
		ui->webView->hide();
		
		ui->ColorModeL->setVisible(false);
		ui->mono_or_color->setVisible(false);
		//ui->wiperFrequencyCB->setVisible(false);
		/* ui->wiperFrequencyLB->setVisible(false);
		 ui->wiperFrequencyLE->setVisible(false);

		 ui->wipeIndexLB->setVisible(false);
		 ui->wiperIndexLE->setVisible(false);

		 ui->wipeClickLB->setVisible(false);
		 ui->wiperClickLE->setVisible(false);*/
		ui->color_TestPageLB->setVisible(true);

		ui->printSummaryFrame->setMinimumHeight(760);

		initPOL();//set style sheet
		//ui->comboBox->currentText
		initItem();
		/* timer = new QTimer(this);
		 connect(timer, SIGNAL(timeout()), this, SLOT(getPrintStatus()));
		 timer->start(1000);*/
		//connect(ui->refreshTB, SIGNAL(clicked()), this, SLOT(getPrintStatus()));

		/* receiveStatus.insert("Online", 0);
		 receiveStatus.insert("Offline", 1);
		 receiveStatus.insert("GettingError", 2);
		 receiveStatus.insert("", 3);*/

	}
	break;
	case START_POINT_MEASUREMENT:
	{

		ui->printListCB->setEditable(true);
		connect(ui->prepareBtn, SIGNAL(clicked()), this, SLOT(gotoCheckListDialog()));
		//ui->heatCHB->setVisible(false);
		showHeatSetting(false);
		setWindowTitle("Test Print Option");

		//ui->printScrollArea->setWidget(ui->webView);
		ui->webView->hide();

		ui->spmLB->setVisible(true);

		ui->printSummaryFrame->setVisible(false);

		ui->ColorModeL->setVisible(false);
		ui->mono_or_color->setVisible(false);


		ui->printSummaryFrame->setMinimumHeight(760);

		initPOL();//set style sheet

		initItem();


	}	break;
	}

	setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
}

bool PrintOption::initComm()
{
	//***	
	if (comm->initFail())
	{
		/*SKT::terminateCMDProcess<bool>(DSP_CHIPCOMMNAME);
		comm->reset();
		Comm3DP::openProc();*/
		//return false;
	}
	else {

		return true;
	}

}
void PrintOption::updatePrintSummaryLabel(Print_Job_Information &pJI)
{
	//fprintf(dbgf, "1\n");//@@@
	//fflush(dbgf);
	switch (om)
	{
	case NORMAL_PRINT_MODE:
	case TEST_PRINT_PAGE_MODE:
	{
		ui->estimatedPrintTimeVaLB->setText(pJI.currenttime.currenttime.toString(QString("yyyy/MM/dd hh:mm:ss")) + "\n" +
			pJI.estimatedBuildTime.time.toString(QString("hh:mm:ss")));
		ui->numberOfLayerVaLB->setText(QString::number(pJI.numoflayer.value));
		ui->sliceHeightVaLB->setText(QString::number(pJI.layerThickness.value));
		ui->estimatedBinderUsageVaLB->setText(QString::number(pJI.estimatedBinderUsage.value, 'f', 3) + pJI.estimatedBinderUsage.unit);
		ui->estimatedCyanUsageVaLB->setText(QString::number(pJI.estimated_cyan.value, 'f', 3) + pJI.estimated_cyan.unit);
		ui->estimatedMagentaUsageVaLB->setText(QString::number(pJI.estimated_magenta.value, 'f', 3) + pJI.estimated_magenta.unit);
		ui->estimatedYellowUsageVaLB->setText(QString::number(pJI.estimated_yellow.value, 'f', 3) + pJI.estimated_yellow.unit);
		ui->totalModelNumberVaLB->setText(QString::number(pJI.modelNumber.value) + pJI.modelNumber.unit);

		//fprintf(dbgf, "2\n");//@@@
		//fflush(dbgf);
		//ui->volumnVaLB->setText(QString::number(pJI.volumn.value, 'f', 3) + pJI.volumn.unit);
		ui->volumnVaLB->setText(QString::number(pJI.volumn.value, 'f', 1) + QString(" <font face=\"Arial\"size=\"3\"><b>cm<sup>3</sup></b></font>"));

		//ui->areaVaLB->setText(QString::number(pJI.area.value, 'f', 1) + pJI.area.unit);
		ui->areaVaLB->setText(QString::number(pJI.area.value, 'f', 1) + QString("<font face=\"Arial\"size=\"3\"><b>cm<sup>2</sup></b></font>"));
		//cursor.insertHtml(" " + QString("<font face=\"Arial\"size=\"3\"><b>cm<sup>2</sup></b></font>"));


		ui->totalNumberOfFacetLBVaLB->setText(QString::number(pJI.totalNumOfFacet.value) + pJI.totalNumOfFacet.unit);
		ui->totalNumberOfVerticesVaLB->setText(QString::number(pJI.totalNumOfVertices.value) + pJI.totalNumOfVertices.unit);
	}
	break;
	case PRINT_ZXA_MONO_FILE:
	case PRINT_ZXA_FILE:
	{
		QString unknowST = "Unknow";
		ui->estimatedPrintTimeVaLB->setText(pJI.currenttime.currenttime.toString(QString("yyyy/MM/dd hh:mm:ss")) + "\n" +
			pJI.estimatedBuildTime.time.toString(QString("hh:mm:ss")));
		ui->numberOfLayerVaLB->setText(QString::number(pJI.numoflayer.value));
		ui->sliceHeightVaLB->setText(unknowST);
		ui->estimatedBinderUsageVaLB->setText(unknowST);
		ui->estimatedCyanUsageVaLB->setText(unknowST);
		ui->estimatedMagentaUsageVaLB->setText(unknowST);
		ui->estimatedYellowUsageVaLB->setText(unknowST);
		ui->totalModelNumberVaLB->setText(unknowST);


		ui->volumnVaLB->setText(unknowST);
		ui->areaVaLB->setText(unknowST);
		ui->totalNumberOfFacetLBVaLB->setText(unknowST);
		ui->totalNumberOfVerticesVaLB->setText(unknowST);
	}
	break;
	}


}
void PrintOption::initItem()
{


	std::string s, e;
	bool result = comm->printerStatus(s, e);
	QString tempStatus = QString::fromStdString(s);
	QString errorStatus = QString::fromStdString(e);
	ui->printListCB->addItem("");

	//switch (comm->printer3dpString.errorStatusMap.at(e))
	//{
	//case Comm3DP::printer3dpString::errorStatus::Error_no_printer:
	//{
	//																 ui->printListCB->addItem("no_printer");
	//																 //ui->printListCB->removeItem(1);																 
	//																 ui->printListCB->setDisabled(true);
	//																 ui->prepareBtn->setDisabled(true);
	//																 QString qss = QString("  border: 0px solid #8f8f91;	border-radius: 2px; background-color: rgb(255, 0, 0);");
	//																 ui->prepareBtn->setStyleSheet(qss);
	//																 ui->prepareBtn->setText("Not Ready");
	//}break;
	//default:
	//	break;
	//	
	//}
	//ui->wiperFrequencyCB->setCurrentIndex(1);

	if (result)
	{
		std::map<std::string, int>::iterator it;
		it = comm->printer3dpString.printerStatusmap.find(s);
		if (it != comm->printer3dpString.printerStatusmap.end())
		{
			switch (comm->printer3dpString.printerStatusmap.at(s))
			{
			case Comm3DP::printer3dpString::printStatus::Online://pause 
			{
				//ui->printListCB->addItem("BCP230");
				ui->printListCB->setCurrentText(tr("PartPro350 xBC"));
				//ui->label_2->setText(tr("PartPro350 xBC ") + tempStatus);
				//ui->printListCB->removeItem(1);
				QString qss = QString("  border: 0px solid #8f8f91;	border-radius: 2px; background-color: rgb(54, 187, 155);");
				if (om == optionMode::NORMAL_PRINT_MODE || optionMode::PRINT_ZXA_FILE || PRINT_ZXA_MONO_FILE || optionMode::MONO_PRINT_MODE)
				{
					ui->prepareBtn->setStyleSheet(qss);
					ui->prepareBtn->setText(tr("Prepare"));
					ui->prepareBtn->setEnabled(true);
				}
				else if (om == optionMode::TEST_PRINT_PAGE_MODE)
				{
					ui->prepareBtn->setStyleSheet(qss);
					ui->prepareBtn->setText(tr("StartPrinting"));
					ui->prepareBtn->setEnabled(true);
				}

			}
			break;
			case Comm3DP::printer3dpString::printStatus::GettingError://pause 
			{
				ui->printListCB->setCurrentText(tr("Printer Not Detected"));
				//ui->label_2->setText(tr("No Printer") + tempStatus);
				//ui->printListCB->removeItem(1);																 
				ui->printListCB->setDisabled(true);
				ui->prepareBtn->setDisabled(true);
				QString qss = QString("  border: 0px solid #8f8f91;	border-radius: 2px; background-color: rgb(183, 231, 219);");
				ui->prepareBtn->setStyleSheet(qss);
				ui->prepareBtn->setText(tr("Not Ready"));


			}break;
			default:
			{
				//ui->printListCB->setCurrentText("PartPro350_xBC " + tempStatus);
				//ui->label_2->setText(tr("PartPro350_xBC_") + tempStatus);
				ui->printListCB->setCurrentText("PartPro350 xBC ");

				//ui->printListCB->removeItem(1);								

				ui->printListCB->setDisabled(true);
				ui->prepareBtn->setDisabled(true);
				QString qss = QString("  border: 0px solid #8f8f91;	border-radius: 2px; background-color: rgb(183, 231, 219);");
				ui->prepareBtn->setStyleSheet(qss);
				ui->prepareBtn->setText(tr("Not Ready"));
			}

			break;
			}
		}
	}

	//std::string s, e;
	//comm->printerStatus(s, e);
	//QString tempStatus = QString::fromStdString(s);
	//QString errorStatus = QString::fromStdString(e);



	//if (tempStatus.contains("Online", Qt::CaseSensitive))
	//{
	//	ui->printListCB->addItem("BCP230");

	//	ui->prepareBtn->setEnabled(true);
	//	QString qss = QString("  border: 0px solid #8f8f91;	border-radius: 2px; background-color: rgb(54, 187, 155); ");
	//	ui->prepareBtn->setStyleSheet(qss);
	//	ui->prepareBtn->setText("Prepare");
	//}	
	//else //if (temp.contains("", Qt::CaseSensitive))
	//{
	//	ui->printListCB->addItem("no_printer");
	//	ui->printListCB->setDisabled(true);
	//	ui->prepareBtn->setDisabled(true);

	//	QString qss = QString("  border: 0px solid #8f8f91;	border-radius: 2px; background-color: rgb(255, 0, 0); ");
	//	ui->prepareBtn->setStyleSheet(qss);
	//	ui->prepareBtn->setText("no_Printer");

	//}
}

void PrintOption::initTestPrintItem()
{
	QLabel *l1 = new QLabel("before executing the test print, you need to");
	ui->printScrollArea->setWidget(l1);

}



void PrintOption::getPrintStatus()
{
	//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("PrintOption"));
	std::string s, e;
	comm->printerStatus(s, e);
	//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("PrintOption"));
	QString tempStatus = QString::fromStdString(s);
	QString errorStatus = QString::fromStdString(e);

	ui->consoleLB->setText("Printer Status :" + tempStatus);
	//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("PrintOption"));

	const char *c_str2 = s.data();

	wchar_t *wmsg = new wchar_t[strlen(c_str2) + 1]; //memory allocation
	mbstowcs(wmsg, c_str2, strlen(c_str2) + 1);
	WRITELOG(logger, framework::Diagnostics::LogLevel::Info, wmsg);


	std::map<std::string, int>::iterator it;
	it = comm->printer3dpString.printerStatusmap.find(s);
	if (it != comm->printer3dpString.printerStatusmap.end())
	{
		switch (it->second)
		{
		case Comm3DP::printer3dpString::printStatus::Online:
		{
			//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("PrintOption"));
			ui->printListCB->setCurrentText("PartPro350 xBC");
			//ui->label_2->setText("PartPro350 xBC " + tempStatus);
			//ui->printListCB->removeItem(1);
			QString qss = QString("  border: 0px solid #8f8f91;	border-radius: 2px; background-color: rgb(54, 187, 155);");
			if (om == (optionMode::NORMAL_PRINT_MODE || optionMode::MONO_PRINT_MODE || PRINT_ZXA_MONO_FILE || PRINT_ZXA_FILE))
			{
				ui->prepareBtn->setStyleSheet(qss);
				ui->prepareBtn->setText(tr("Prepare"));
				ui->prepareBtn->setEnabled(true);
			}
			else if (om == optionMode::TEST_PRINT_PAGE_MODE || om == optionMode::START_POINT_MEASUREMENT )
			{
				ui->prepareBtn->setStyleSheet(qss);
				ui->prepareBtn->setText(tr("StartPrinting"));
				ui->prepareBtn->setEnabled(true);
			}
			//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("PrintOption"));
			//timer->stop();
		}
		break;
		case Comm3DP::printer3dpString::printStatus::GettingError://pause 
		{
			//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("PrintOption"));
			ui->printListCB->setCurrentText(tr("Printer Not Detected"));
			//ui->label_2->setText("No Printer " + tempStatus);
			//ui->printListCB->removeItem(1);																 
			ui->printListCB->setDisabled(true);
			ui->prepareBtn->setDisabled(true);
			QString qss = QString("  border: 0px solid #8f8f91;	border-radius: 2px; background-color: rgb(183, 231, 219);");
			ui->prepareBtn->setStyleSheet(qss);
			ui->prepareBtn->setText(tr("Not Ready"));
			//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("PrintOption"));

		}break;
		default:
		{
			//WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("PrintOption"));
			ui->printListCB->setCurrentText("PartPro350 xBC");
			ui->printListCB->setDisabled(true);
			ui->prepareBtn->setDisabled(true);
			QString qss = QString("  border: 0px solid #8f8f91;	border-radius: 2px; background-color: rgb(183, 231, 219);");
			ui->prepareBtn->setStyleSheet(qss);
			ui->prepareBtn->setText(tr("Not Ready"));
		}

		break;
		}
	}
	else
	{
		WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("PrintOption"));
		ui->printListCB->setCurrentText("Printer Not Detected");
		//ui->label_2->setText("PartPro350 xBC ");
		//ui->printListCB->removeItem(1);																 
		ui->printListCB->setDisabled(true);
		ui->prepareBtn->setDisabled(true);
		QString qss = QString("  border: 0px solid #8f8f91;	border-radius: 2px; background-color: rgb(183, 231, 219);");
		ui->prepareBtn->setStyleSheet(qss);
		ui->prepareBtn->setText(tr("Not Ready"));
		WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("PrintOption"));
	}

	WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("PrintOption"));

	//switch (comm->printer3dpString.errorStatusMap.at(e))
	//{
	//case Comm3DP::printer3dpString::errorStatus::Error_no_printer:
	//{
	//																 ui->printListCB->addItem("no_printer");
	//																 ui->label_2->setText("no_printer_" + errorStatus);
	//																 //ui->printListCB->removeItem(1);																 
	//																 ui->printListCB->setDisabled(true);
	//																 ui->prepareBtn->setDisabled(true);
	//																 QString qss = QString("  border: 0px solid #8f8f91;	border-radius: 2px; background-color: rgb(255, 0, 0);");
	//																 ui->prepareBtn->setStyleSheet(qss);
	//																 ui->prepareBtn->setText("Not Ready");
	//}break;
	//default:
	//	break;

	//}




}
void PrintOption::initPOL()
{
	ui->estimatedPrintTimeLB->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	ui->estimatedPrintTimeVaLB->setStyleSheet(WidgetStyleSheet::textStyleSheet2());
	ui->numberOfLayerLB->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	ui->numberOfLayerVaLB->setStyleSheet(WidgetStyleSheet::textStyleSheet2());
	ui->sliceHeightLB->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	ui->sliceHeightVaLB->setStyleSheet(WidgetStyleSheet::textStyleSheet2());
	ui->estimatedBinderUsageLB->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	ui->estimatedBinderUsageVaLB->setStyleSheet(WidgetStyleSheet::textStyleSheet2());

	ui->estimatedCyanUsageLB->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	ui->estimatedCyanUsageVaLB->setStyleSheet(WidgetStyleSheet::textStyleSheet2());

	ui->estimatedMagentaUsageLB->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	ui->estimatedMagentaUsageVaLB->setStyleSheet(WidgetStyleSheet::textStyleSheet2());

	ui->estimatedYellowUsageLB->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	ui->estimatedYellowUsageVaLB->setStyleSheet(WidgetStyleSheet::textStyleSheet2());

	ui->totalModelNumberLB->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	ui->totalModelNumberVaLB->setStyleSheet(WidgetStyleSheet::textStyleSheet2());

	ui->volumnLB->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	ui->volumnVaLB->setStyleSheet(WidgetStyleSheet::textStyleSheet2());

	ui->areaLB->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	ui->areaVaLB->setStyleSheet(WidgetStyleSheet::textStyleSheet2());

	ui->totalNumberOfFacetLB->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	ui->totalNumberOfFacetLBVaLB->setStyleSheet(WidgetStyleSheet::textStyleSheet2());

	ui->totalNumberOfVerticesLB->setStyleSheet(WidgetStyleSheet::textStyleSheet());
	ui->totalNumberOfVerticesVaLB->setStyleSheet(WidgetStyleSheet::textStyleSheet2());
}
void PrintOption::gotoCheckListDialog()
{

	accept();
}


bool PrintOption::getMonoParam()
{
	return ui->mono_or_color->currentIndex();

}
//int PrintOption::getWiperFrequency()
//{
//	int x = 0;
//	return ui->wiperFrequencyLE->text().toInt();
//	/*switch (ui->wiperFrequencyCB->currentIndex())
//	{
//	case 0:
//		return 2;
//		break;
//	case 1:
//		return 4;
//		break;
//	case 2:
//		return 6;
//		break;
//	case 3:
//		return 8;
//		break;
//	case 4:
//		return 12;
//		break;
//	case 5:
//		return 16;
//		break;
//	case 6:
//		return 32;
//		break;
//	default:
//		return 4;
//	}*/
//
//}
//int PrintOption::getWiperClick()
//{
//	int x = 0;
//	return ui->wiperClickLE->text().toInt();
//
//}
//
//int PrintOption::getWiperIndex()
//{
//	return ui->wiperIndexLE->text().toInt();
//
//}
int PrintOption::getSpittonData()
{
	int x = 0;
	return 0;// ui->spitton_cb->currentIndex();


}
void PrintOption::getHeatStuffParam(QVector<int> &heatParam)
{
	heatParam.clear();

	/*heatParam.push_back(ui->heatLE1->text().toInt());
	heatParam.push_back(ui->heatLE2->text().toInt());
	heatParam.push_back(ui->heatLE3->text().toInt());
	heatParam.push_back(ui->heatLE4->text().toInt());*/
	//heatParam.push_back(0);
	//heatParam.push_back(0);
	heatParam.push_back(ui->heatLE1->text().toInt());
	heatParam.push_back(ui->heatLE2->text().toInt());
	heatParam.push_back(45);
	heatParam.push_back(40);
	heatParam.push_back(ui->heatLE5->text().toInt());
	heatParam.push_back(ui->heatLE6->text().toInt());
	heatParam.push_back(ui->heatLE7->text().toInt());

}
void PrintOption::showHeatSetting(bool showheat)
{
	if (showheat)
	{

		ui->heatLE1->setVisible(false);
		ui->heatLE2->setVisible(false);
		//ui->heatLE5->setVisible(true);
		ui->heatLE6->setVisible(true);
		ui->heatLE7->setVisible(true);

		ui->heatLB1->setVisible(false);
		ui->heatLB2->setVisible(false);
		ui->heatLB5->setVisible(true);
		ui->heatLB6->setVisible(true);
		ui->heatLB7->setVisible(true);
	}
	else
	{

		ui->heatLE1->setVisible(false);
		ui->heatLE2->setVisible(false);
		//ui->heatLE5->setVisible(false);
		ui->heatLE6->setVisible(false);
		ui->heatLE7->setVisible(false);

		ui->heatLB1->setVisible(false);
		ui->heatLB2->setVisible(false);
		ui->heatLB5->setVisible(false);
		ui->heatLB6->setVisible(false);
		ui->heatLB7->setVisible(false);
	}

}
PrintOption::~PrintOption()
{
	timer->destroyed();
}
