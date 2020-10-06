#include "printingHistory.h"
#include <QHeaderView>
#include <qDebug>
#include <QLabel>
#include "PrintingHistoryDetail.h"
#include <QCollator>

#include "widgetStyleSheet.h"

//TBD different sort function
//

PrintingHistory::PrintingHistory(QWidget *parent) : QDialog(parent), ui(new Ui::PrintingHistoryDlg), testX(1), phIDSwitch(1)
{
	ui->setupUi(this);
	this->setFixedSize(1050, 720);
	historyColumnNum = 8;
	ui->scrollArea->setVisible(false);
	ui->frame->setFixedWidth(1020);
	/*create history list*/
	QVariant _dmHistory;
	if (ParamOp::extractVariantTest(_dmHistory, QVariant(), QString(), -1, BCPwareFileSystem::printingHistoryFilePath()))
	{
		QVariant currentID, idMap;
		if (ParamOp::extractVariantTest(currentID, _dmHistory, "CURRENT_RECORD_ID"))
		{
			for (int i = 0; i <= currentID.toInt(); i++)
			{
				if (ParamOp::extractVariantTest(idMap, _dmHistory, QString::number(i)))
					printHistoryItemList.push_front(new PrintHistoryItem(*new QFileInfo(BCPwareFileSystem::printingHistoryFilePath()), QString::number(i)));

			}
		}


		/* QVariantMap _dmhistoryMap = _dmHistory.toMap();
		QMapIterator<QString, QVariant> dmHistoryMapIter(_dmhistoryMap);
		while (dmHistoryMapIter.hasNext()) {
		dmHistoryMapIter.next();
		if(QMetaType::QVariantMap == static_cast<QMetaType::Type>(dmHistoryMapIter.value().type()))
		{
		printHistoryItemList.push_back(new PrintHistoryItem(*new QFileInfo(DMShopFilesystem::printerHistoryPath()),dmHistoryMapIter.key()));
		}
		}*/
	}

	/*init tableWidget*/
	initTableInformation();

	/*phistoryTb1 = new QTableWidget(10, );
	QCheckBox *chb = new QCheckBox();
	QPushButton *pb1 = new QPushButton();
	connect(pb1, &QPushButton::clicked, [&](){

	});
	QStringList header;
	header << "Month" << "Description";
	phistoryTb1->setHorizontalHeaderLabels(header);
	phistoryTb1->setItem(0, 0, new QTableWidgetItem("Jan"));
	phistoryTb1->setItem(1, 0, new QTableWidgetItem("Feb"));
	phistoryTb1->setItem(2, 0, new QTableWidgetItem("Mar"));
	phistoryTb1->setCellWidget(3, 0, chb);
	phistoryTb1->setCellWidget(4, 0, pb1);
	QGridLayout *la1 = new QGridLayout();

	la1->addWidget(phistoryTb1);

	setLayout(la1);*/

	count_Total_Printhead();
}
void PrintingHistory::updateTable()
{
	phistoryTb1->clearContents();

	/*for (int i = 0; i < printHistoryItemList.size(); i++)
	{
	phistoryTb1->removeRow(i);
	}*/
	for (int i = 0; i < printHistoryItemList.size(); i++)
	{

		//phistoryTb1->setItem(i, 0, printHistoryItemList.at(i)->gethistoryID_TWI());
		QHBoxLayout *chbHB = new QHBoxLayout();
		QFrame *chbF = new QFrame();
		QCheckBox *chb = new QCheckBox();	


		chbHB->setAlignment(Qt::AlignCenter);
		chbHB->setContentsMargins(0, 0, 0, 0);
		chbHB->addWidget(chb);
		chbF->setLayout(chbHB);
		/*QPushButton*pb = new QPushButton(QIcon(":/images/icons/btn_detail_normal.png"), "", this);
		pb->setIconSize(QSize(32, 32));*/
		QLabel *lb1 = new QLabel(this);
		const int w = lb1->width();
		const int h = lb1->height();

		qDebug() << "setPixmap : " << printHistoryItemList.at(i)->getJobThumbnailName().toString();
		
		//;
		if (printHistoryItemList.at(i)->getJobThumbnailName().toString().isEmpty())
		{
			QPixmap *thumbnailp = new QPixmap();
			bool abcd = thumbnailp->load(":/images/icons/history_nan_pic.png");
			lb1->setPixmap(thumbnailp->scaled(w, h, Qt::KeepAspectRatio));
		}
		else
			lb1->setPixmap(QPixmap(printHistoryItemList.at(i)->getJobThumbnailName().toString()).scaled(w, h, Qt::KeepAspectRatio));

		if (printHistoryItemList.at(i)->getCheckState().toBool())
		{
			chb->setChecked(true);
		}

		connect(chb, static_cast<void (QCheckBox::*)(int)>(&QCheckBox::stateChanged), [&, i](int ix)
		{
			if (ix)
			{
				ui->deleteJobButton->setDisabled(false);
				printHistoryItemList.at(i)->setCheckState(true);
			}
			else
			{
				printHistoryItemList.at(i)->setCheckState(false);
			}
		});
		/*connect(pb, &QPushButton::clicked, [=](){

			PrintingHistoryDetail *phD = new PrintingHistoryDetail(printHistoryItemList.at(i)->getHistoryID().toInt(), this);
			phD->setWindowFlags(Qt::Popup);
			phD->setAttribute(Qt::WA_DeleteOnClose);
			phD->move(200, 200);
			phD->exec();
			//===================================================================

			//QDialog *dia = new QDialog(this);
			//dia->setWindowTitle("Detail");
			//dia->setFixedSize(1000, 800);
			//QGridLayout *gdl = new QGridLayout();
			//QTableWidget *subTable = new QTableWidget(10,6);
			//subTable->setColumnWidth(0, 200);
			//subTable->setColumnWidth(2, 200);
			//subTable->setColumnWidth(4, 200);
			////init subItem struct
			//subItemList.clear();
			//subItemList.push_back(new SubItemStruct("Binder start level", printHistoryItemList.at(i)->getBinderStartLevel().toString()));
			//subItemList.push_back(new SubItemStruct("Binder end level", printHistoryItemList.at(i)->getBinderEndLevel().toString()));
			//subItemList.push_back(new SubItemStruct("Binder used", QString::number(printHistoryItemList.at(i)->getBinderStartLevel().toFloat() - printHistoryItemList.at(i)->getBinderEndLevel().toFloat(), 'f', 2)));

			//subItemList.push_back(new SubItemStruct("Cyan start level", printHistoryItemList.at(i)->getCyanStartLevel().toString()));
			//subItemList.push_back(new SubItemStruct("Cyan end level", printHistoryItemList.at(i)->getCyanEndLevel().toString()));
			//subItemList.push_back(new SubItemStruct("Cyan used", QString::number(printHistoryItemList.at(i)->getCyanStartLevel().toFloat() - printHistoryItemList.at(i)->getCyanEndLevel().toFloat(), 'f', 2)));

			//subItemList.push_back(new SubItemStruct("Magenta start level", printHistoryItemList.at(i)->getMagentaStartLevel().toString()));
			//subItemList.push_back(new SubItemStruct("Magenta end level", printHistoryItemList.at(i)->getMagentaEndLevel().toString()));
			//subItemList.push_back(new SubItemStruct("Magenta used", QString::number(printHistoryItemList.at(i)->getMagentaStartLevel().toFloat() - printHistoryItemList.at(i)->getMagentaEndLevel().toFloat(), 'f', 2)));

			//subItemList.push_back(new SubItemStruct("Yellow start level", printHistoryItemList.at(i)->getYellowStartLevel().toString()));
			//subItemList.push_back(new SubItemStruct("Yellow end level", printHistoryItemList.at(i)->getYellowEndLevel().toString()));
			//subItemList.push_back(new SubItemStruct("Yellow used", QString::number(printHistoryItemList.at(i)->getYellowStartLevel().toFloat() - printHistoryItemList.at(i)->getYellowEndLevel().toFloat(), 'f', 2)));

			//subItemList.push_back(new SubItemStruct("Printhead page at start", printHistoryItemList.at(i)->getPrintHeadPageStart().toString()));
			//subItemList.push_back(new SubItemStruct("Printhead page at end", printHistoryItemList.at(i)->getPrintHeadPageEnd().toString()));
			//subItemList.push_back(new SubItemStruct("Printhead printed", QString::number(printHistoryItemList.at(i)->getPrintHeadPageEnd().toInt() - printHistoryItemList.at(i)->getPrintHeadPageStart().toInt())));

			//subItemList.push_back(new SubItemStruct("Wiper at start", printHistoryItemList.at(i)->getWipesStart().toString()));
			//subItemList.push_back(new SubItemStruct("Wiper at end", printHistoryItemList.at(i)->getWipesEnd().toString()));

			//subItemList.push_back(new SubItemStruct("Printhead ID", printHistoryItemList.at(i)->getPrintHeadID().toString()));

			//subItemList.push_back(new SubItemStruct("job Start time", printHistoryItemList.at(i)->getJobStartTime().toString()));
			//subItemList.push_back(new SubItemStruct("job Start time", printHistoryItemList.at(i)->getJobCompletedTime().toString()));
			//for (int i = 0, x = 0; i < 10; i++)
			//{
			//	for (int j = 0; j < 6; j++)
			//	{
			//		if (subItemList.size() > x)
			//		{
			//			if (j % 2 == 0)
			//			{
			//				subTable->setItem(i, j, new QTableWidgetItem(subItemList.at(x)->name));
			//				
			//			}
			//			else
			//			{
			//				subTable->setItem(i, j, new QTableWidgetItem(subItemList.at(x)->value.toString()));
			//				x++;
			//			}
			//		}
			//		
			//	}

			//}
			//
			//subTable->setMinimumSize(subTable->maximumSize());
			//
			//
			//gdl->addWidget(subTable);
			//dia->setLayout(gdl);
			//dia->exec();
		});*/


		//ui->listWidget, static_cast<void (QListWidget::*)(int)>(&QListWidget::currentRowChanged), [=](int ix)

		//header << "" << "Preview" << "ProjectName" << "Outcome" << "Start at" << "Finished at" << "Elapsed time" << "Printhead ID" << "Detail";
		//==========0=========1=============2==============3=============4==============5=================6=================7==============8

		for (int l = 0; l < 10; l++)
		{
			QTableWidgetItem *widgetItem = new QTableWidgetItem();
			QString outcomeResult = printHistoryItemList.at(i)->getOutcomeHistory().toString();
			widgetItem->setBackground(QBrush(QColor(255, 255, 255)));
			if (outcomeResult.isEmpty() || outcomeResult == "Cancel" || outcomeResult == "Abort")
			{
				widgetItem->setForeground(QBrush(QColor(237, 28, 36)));
			}
			else
			{
				widgetItem->setForeground(QBrush(QColor(0, 0, 0)));
			}
			QString temp;
			switch (l)
			{
			case 0:
				phistoryTb1->setCellWidget(i, l, chbF);
				break;
			case 1:
				phistoryTb1->setCellWidget(i, l, lb1);
				break;
			case 2:
				temp = printHistoryItemList.at(i)->getProjectName().toString();
				widgetItem->setText(temp.isEmpty() ? "NAN" : temp);
				phistoryTb1->setItem(i, l,widgetItem);				
				break;
			case 3:
				if (outcomeResult.isEmpty())
					outcomeResult = "Fail";
				widgetItem->setText(outcomeResult);
				phistoryTb1->setItem(i, l, widgetItem);				
				break;
			case 4:
				temp = printHistoryItemList.at(i)->getJobStartTime().toString();
				widgetItem->setText(temp.isEmpty() ? "NAN" : temp);				
				phistoryTb1->setItem(i, l, widgetItem);				
				break;
			case 5:
				temp = printHistoryItemList.at(i)->getJobCompletedTime().toString();
				widgetItem->setText(temp.isEmpty() ? "NAN" : temp);				
				phistoryTb1->setItem(i, l, widgetItem);				
				break;
			case 6:
				temp = printHistoryItemList.at(i)->getJobElapsedTime().toString();
				widgetItem->setText(temp.isEmpty() ? "NAN" : temp);				
				phistoryTb1->setItem(i, l, widgetItem);
				
				break;
			case 7:
				temp = printHistoryItemList.at(i)->getPrintHeadID().toString();
				widgetItem->setText(temp.isEmpty() ? "NAN" : temp);				
				phistoryTb1->setItem(i, l, widgetItem);				
				break;
			/*case 8:
				phistoryTb1->setCellWidget(i, l, pb);
				break;*/
			default:
				;
			}
			

		}

		phistoryTb1->setRowHeight(i, 47);

	}
	connect(phistoryTb1, static_cast<void(QTableWidget::*)(int, int)>(&QTableWidget::cellClicked), [&](int x, int y){
		qDebug() << "qtablewidget x,y : " << x << y;
		//QString

		QTableWidgetItem  *temp = phistoryTb1->item(x, 4);
		QString temps = temp->text();
		foreach(PrintHistoryItem* temp, printHistoryItemList)
		{
			if (temp->getJobStartTime().toString() == temps)
			{
				PrintingHistoryDetail *phD = new PrintingHistoryDetail(temp->getHistoryID().toInt(), this);
				phD->setWindowFlags(Qt::Popup);
				phD->setAttribute(Qt::WA_DeleteOnClose);
				phD->move(200, 200);
				phD->exec();
			}
		}
		
		

		
	});
}
void PrintingHistory::initTableInformation()
{
	/************************init Action **********************/
	selectAllAct = new QAction(QIcon(":/images/icons/new_project.png"),"select All", this);
	cancelAct = new QAction("deselect ALl", this);
	ui->selectAllTB->addAction(selectAllAct);
	ui->selectAllTB->addAction(cancelAct);
	connect(selectAllAct, &QAction::triggered, [&](){
		qDebug() << "select all";
		for (int i = 0; i < printHistoryItemList.size(); i++)
		{
			printHistoryItemList.at(i)->setCheckState(true);
		}
		ui->selectAllTB->setChecked(true);
		ui->selectAllTB->setIcon(QIcon(":/images/icons/btn_checkbox_on_large.png"));
		updateTable();
	});
	connect(cancelAct, &QAction::triggered, [&](){
		qDebug() << "deselect all";
		for (int i = 0; i < printHistoryItemList.size(); i++)
		{
			printHistoryItemList.at(i)->setCheckState(false);
		}
		ui->selectAllTB->setChecked(false);
		ui->selectAllTB->setIcon(QIcon());
		updateTable();
	});
	connect(ui->selectAllTB, static_cast<void(QToolButton::*)(bool)>(&QToolButton::clicked), [&](bool bo){
		qDebug() << "bobo" << bo;
		for (int i = 0; i < printHistoryItemList.size(); i++)
		{
			if (bo)
				printHistoryItemList.at(i)->setCheckState(true);
			else
				printHistoryItemList.at(i)->setCheckState(false);
		}
		if (bo)
		{
			ui->selectAllTB->setIcon(QIcon(":/images/icons/btn_checkbox_on_large.png"));
		}else
			ui->selectAllTB->setIcon(QIcon());
		updateTable();
	});


	const QString chbStylesheet
		(
		"QToolButton[popupMode = \"1\"] {"
		"padding-right:20px;"		
		"}"

		"QToolButton:checked{"
		//"background-color: rgb(0, 255, 0);"
		//"image: url(:/images/icons/btn_add_label.png);"
		"}"
		
		);
	ui->selectAllTB->setStyleSheet(chbStylesheet);
	

	/*********************************************/

	QStringList header;
	//header << "" << "id" << "Preview" << "ProjectName" << "Outcome" << "Start at" << "Print Time" << "Printhead ID" << "Detail";
	header << "" << "Preview" << "ProjectName" << "Outcome" << "Start at" << "Finished at" << "Elapsed time" << "Printhead ID";// << "Detail";
	//========0=========1=============2==============3=============4==============5=================6=============7==============8
	phistoryTb1 = new QTableWidget(printHistoryItemList.size(), historyColumnNum);	
	QHeaderView *hHeader = phistoryTb1->horizontalHeader();
	hHeader->setFrameStyle(QFrame::Sunken);
	//hHeader->setLineWidth(4);
	phistoryTb1->horizontalHeader()->setFixedHeight(47);
	phistoryTb1->setHorizontalHeader(hHeader);
	phistoryTb1->setHorizontalHeaderLabels(header);
	//phistoryTb1->
	for (int l = 0; l < header.size(); l++)
	{
		
		switch (l)
		{
		case 0:
			phistoryTb1->setColumnWidth(l, 40);
			break;
		case 1:			
			break;
		case 2:		
			phistoryTb1->setColumnWidth(l, 150);
			break;
		case 3:			
			break;
		case 4:			
		case 5:			
		case 6:
			phistoryTb1->setColumnWidth(l, 150);
			break;
		case 7:			
			break;
		case 8:
			//phistoryTb1->setColumnWidth(l, 48);
			break;
		default:
			;
		}
	}
	
	phistoryTb1->verticalHeader()->setVisible(false);

	const QString thisItemStyle
		(
		//"QTableView"
		//"{"
		//"color: {color};"
		//"border: 1px;"
		//"background: {color};"
		//"gridline-color: {color};"
		//"}"

		"QTableView::section{"
		"font: 14px \"Arial\";"
		"color: rgb(99, 99, 99);"
		"border:  1px solid  #ff0000;"
		"background-color: rgb(255, 255, 255);"
		"}"

		/*"QTableView{"
		"font: 14px \"Arial\";"
		"background-color: rgb(255, 255, 255);"
		"gridline-color:#c4c4c4;"
		"}"*/		

		);
	phistoryTb1->setStyleSheet(thisItemStyle);
	
		
	/**/
	updateTable();


	phistoryTb1->setSortingEnabled(true);
	phistoryTb1->sortByColumn(4, Qt::DescendingOrder);

	const QString removeBtnStyleSheet
		(

		"QToolButton:enabled {\n"

		"   border: none;\n"
		"   image: url(:/images/icons/history_btn_remove_all.png) top center no-repeat;\n"
		//"   padding-top: 200px;\n"
		"   width: 30px;\n"
		"   height :30px;\n"
		"   font: 9pt \"Arial\";\n"

		"   color: white;\n"
		"}\n"

		"QToolButton:disabled {\n"

		"   border: none;\n"
		"   image: url(:/images/icons/history_btn_remove_all_disabled.png) top center no-repeat;\n"
		"   color: white;\n"
		"}\n"

		"QToolButton:hover {\n"

		"   border: none;\n"
		"   image: url(:/images/icons/history_btn_remove_all_hover.png) top center no-repeat;\n"
		"   color: white;\n"
		"}\n"
		);

	ui->deleteJobButton->setStyleSheet(removeBtnStyleSheet);
	ui->deleteJobButton->setDisabled(true);
	//ui->deleteJobButton->setIcon(QIcon(":/images/icons/history_btn_remove_all.png"));
	//ui->deleteJobButton->setIconSize(QSize(20,20));

	connect(ui->deleteJobButton, &QPushButton::clicked, [&](){
		QMessageBox::StandardButton ret = QMessageBox::question(this, tr("Delete history item"), tr("Are you sure ?"),QMessageBox::Yes | QMessageBox::No,QMessageBox::No);

		if (ret == QMessageBox::Yes)
		{
			QMutableListIterator<PrintHistoryItem *> historyItr(printHistoryItemList);
			while (historyItr.hasNext()) {
				historyItr.next();
				if (historyItr.value()->getCheckState().toBool())
				{

					QVariant _dmHistory;
					if (ParamOp::extractVariantTest(_dmHistory, QVariant(), QString(), -1, BCPwareFileSystem::printingHistoryFilePath()))
					{
						QVariant idMap;
						QVariantMap tempMap;
						if (ParamOp::extractVariantTest(idMap, _dmHistory, historyItr.value()->getHistoryID().toString()))
						{
							qDebug() << historyItr.value()->getHistoryID().toString();

							QVariantMap tempMap = _dmHistory.toMap();
							tempMap.remove(historyItr.value()->getHistoryID().toString());
							ParamOp::mergeValue(QVariant(tempMap), QVariant(), QString(), -1, BCPwareFileSystem::printingHistoryFilePath());
						}
					}

					historyItr.remove();
				}
			}
			ui->deleteJobButton->setDisabled(true);
		}
		else
		{
			ui->deleteJobButton->setDisabled(false);
		}
		updateTable();
		


	});

	//connect(phistoryTb1, static_cast<void (QTableWidget::*)(int, int)>(&QTableWidget::cellClicked), [=](int row, int col)
	//{
	//	if (row == 1 && col == 1)
	//	{
	//		qSort(printHistoryItemList.begin(), printHistoryItemList.end(), [&](const PrintHistoryItem *infoA, const PrintHistoryItem *infoB){
	//			return infoA->getHistoryID().toInt() > infoB->getHistoryID().toInt(); });
	//		QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg("aaaaaaa"));
	//		//phistoryTb1->takeItem(1, 1);
	//		phistoryTb1->setItem(1, 1, newItem);
	//		//updateTable();
	//		phistoryTb1->setSortingEnabled(true);
	//		phistoryTb1->sortByColumn(2);
	//	}
	//});



	//QHeaderView *headerV = qobject_cast<QTableWidget *>(phistoryTb1)->horizontalHeader();
	//connect(headerV, &QHeaderView::sectionClicked, [this](int logicalIndex){
	//
	//	switch (logicalIndex)
	//	{
	//	case 1:
	//	{
	//		if (testX)
	//		{
	//			qSort(printHistoryItemList.begin(), printHistoryItemList.end(), [&](const PrintHistoryItem *infoA, const PrintHistoryItem *infoB){
	//				return infoA->getHistoryID().toInt() > infoB->getHistoryID().toInt(); });
	//			updateTable();
	//		}
	//		else
	//		{
	//			qSort(printHistoryItemList.begin(), printHistoryItemList.end(), [&](const PrintHistoryItem *infoA, const PrintHistoryItem *infoB){
	//				return infoA->getHistoryID().toInt() < infoB->getHistoryID().toInt(); });
	//			updateTable();
	//		}
	//		testX ^= 1;
	//	}
	//		break;
	//	case 5:
	//	{
	//		if (testX)
	//		{
	//			qSort(printHistoryItemList.begin(), printHistoryItemList.end(), [&](const PrintHistoryItem *infoA, const PrintHistoryItem *infoB){
	//				qDebug() << infoA->getJobElapsedTime().toString();
	//				QDateTime completeTimeA = QDateTime::fromString(infoA->getJobCompletedTime().toString(), Qt::ISODate); //QDateTime::fromString(infoA->getJobElapsedTime().toString(), "hh:mm:ss");
	//				QDateTime completeTimeB = QDateTime::fromString(infoB->getJobCompletedTime().toString(), Qt::ISODate);
	//				qDebug() << "completeTimeA : " << completeTimeA.toMSecsSinceEpoch() << "completeTimeB : " << completeTimeB.toMSecsSinceEpoch();
	//				return completeTimeA.toMSecsSinceEpoch() > completeTimeB.toMSecsSinceEpoch();
	//			});
	//			updateTable();
	//		}
	//		else
	//		{
	//			qSort(printHistoryItemList.begin(), printHistoryItemList.end(), [&](const PrintHistoryItem *infoA, const PrintHistoryItem *infoB){
	//				qDebug() << infoA->getJobElapsedTime().toString();
	//				QDateTime completeTimeA = QDateTime::fromString(infoA->getJobCompletedTime().toString(), Qt::ISODate); //QDateTime::fromString(infoA->getJobElapsedTime().toString(), "hh:mm:ss");
	//				QDateTime completeTimeB = QDateTime::fromString(infoB->getJobCompletedTime().toString(), Qt::ISODate);
	//				qDebug() << "completeTimeA : " << completeTimeA.toMSecsSinceEpoch() << "completeTimeB : " << completeTimeB.toMSecsSinceEpoch();
	//				return completeTimeA.toMSecsSinceEpoch() < completeTimeB.toMSecsSinceEpoch();
	//			});
	//			updateTable();
	//		}
	//		testX ^= 1;
	//	}
	//	break;
	//	case 6:
	//	{
	//		if (testX)
	//		{
	//			qSort(printHistoryItemList.begin(), printHistoryItemList.end(), [&](const PrintHistoryItem *infoA, const PrintHistoryItem *infoB){
	//				qDebug() << infoA->getJobElapsedTime().toString();
	//				QDateTime elapsedTimeA = QDateTime::fromString(infoA->getJobElapsedTime().toString(), "hh:mm:ss");
	//				QDateTime elapsedTimeB = QDateTime::fromString(infoB->getJobElapsedTime().toString(), "hh:mm:ss");
	//				qDebug() << "elapsedTimeA : " << elapsedTimeA.toMSecsSinceEpoch() << "elapsedTimeB : " << elapsedTimeB.toMSecsSinceEpoch();
	//				return elapsedTimeA.toMSecsSinceEpoch() > elapsedTimeB.toMSecsSinceEpoch();
	//			});
	//			updateTable();
	//		}
	//		else
	//		{
	//			qSort(printHistoryItemList.begin(), printHistoryItemList.end(), [&](const PrintHistoryItem *infoA, const PrintHistoryItem *infoB){
	//				QDateTime elapsedTimeA = QDateTime::fromString(infoA->getJobElapsedTime().toString(), Qt::ISODate);
	//				QDateTime elapsedTimeB = QDateTime::fromString(infoB->getJobElapsedTime().toString(), Qt::ISODate);
	//				qDebug() << "elapsedTimeA : " << elapsedTimeA.toMSecsSinceEpoch() << "elapsedTimeB : " << elapsedTimeB.toMSecsSinceEpoch();
	//				return elapsedTimeA.toMSecsSinceEpoch()< elapsedTimeB.toMSecsSinceEpoch();
	//			});
	//			updateTable();
	//		}
	//		testX ^= 1;
	//		
	//	}
	//		break;
	//	case 4:
	//	{
	//		QCollator coll;
	//		coll.setNumericMode(true);
	//		if (phIDSwitch)
	//		{
	//			qSort(printHistoryItemList.begin(), printHistoryItemList.end(), [&](const PrintHistoryItem *infoA, const PrintHistoryItem *infoB){
	//				QString outcomeA = infoA->getOutcomeHistory().toString();
	//				QString outcomeB = infoB->getOutcomeHistory().toString();
	//				qDebug() << "printheadIDA : " << outcomeA.toLower() << "outcomeB : " << outcomeB.toLower();
	//				qDebug() << (coll.compare(outcomeA, outcomeB) < 0);
	//				return  outcomeA > outcomeB;// coll.compare(printheadIDA, printheadIDB) < 0;
	//			});
	//			updateTable();
	//		}
	//		else
	//		{
	//			qStableSort(printHistoryItemList.begin(), printHistoryItemList.end(), [&](const PrintHistoryItem *infoA, const PrintHistoryItem *infoB){
	//				QString outcomeA = infoA->getOutcomeHistory().toString();
	//				QString outcomeB = infoB->getOutcomeHistory().toString();					
	//				return  outcomeA < outcomeB;// coll.compare(printheadIDA, printheadIDB) < 0;
	//			});
	//			updateTable();
	//		}
	//		phIDSwitch ^= 1;
	//	}break;
	//	case 7:
	//	{
	//		QCollator coll;
	//		coll.setNumericMode(true);
	//		if (phIDSwitch)
	//		{
	//			qSort(printHistoryItemList.begin(), printHistoryItemList.end(), [&](const PrintHistoryItem *infoA, const PrintHistoryItem *infoB){
	//				QString printheadIDA = infoA->getPrintHeadID().toString();
	//				QString printheadIDB = infoB->getPrintHeadID().toString();
	//				qDebug() << "printheadIDA : " << printheadIDA.toLower() << "printheadIDB : " << printheadIDB.toLower();
	//				qDebug() << (coll.compare(printheadIDA, printheadIDB) < 0);
	//				return  printheadIDA > printheadIDB;// coll.compare(printheadIDA, printheadIDB) < 0;
	//			});
	//			updateTable();
	//		}
	//		else
	//		{
	//			qStableSort(printHistoryItemList.begin(), printHistoryItemList.end(), [&](const PrintHistoryItem *infoA, const PrintHistoryItem *infoB){
	//				QString printheadIDA = infoA->getPrintHeadID().toString();
	//				QString printheadIDB = infoB->getPrintHeadID().toString();
	//				qDebug() << "printheadIDA : " << printheadIDA.toLower() << "printheadIDB : " << printheadIDB.toLower();
	//				return  printheadIDA < printheadIDB; //coll.compare(printheadIDA, printheadIDB)>0;
	//			});
	//			updateTable();
	//		}
	//		phIDSwitch ^= 1;
	//	}break;
	//	case 2:
	//		break;
	//	}
	//	printHistoryItemList.size();
	//
	//	QString text = phistoryTb1->horizontalHeaderItem(logicalIndex)->text();
	//	qDebug() << logicalIndex << text << testX;
	//});


	QGridLayout *la1 = new QGridLayout();
	la1->setMargin(24);
	la1->addWidget(phistoryTb1);
	qDebug() << "phistoryTb1->width() : " << phistoryTb1->width();
	ui->frame->setLayout(la1);

	//setLayout(la1);
}

void PrintingHistory::count_Total_Printhead()
{
	QSet<QString> phidSet;
	QMultiMap<QString, int> ph_page;
	foreach(PrintHistoryItem *item, printHistoryItemList)
	{
		phidSet << item->getPrintHeadID().toString();
		ph_page.insert(item->getPrintHeadID().toString(), item->getPrintHeadPageEnd().toInt());
	}
	QString tempS;
	foreach(QString phidS, phidSet)
	{
		QList<int> pages = ph_page.values(phidS);
		std::sort(pages.begin(), pages.end());
		qDebug() << phidS << ":" << pages.last();
		tempS.append(QString("%1:%2\n").arg(phidS).arg(pages.last()));
		//phIDPagesLb->set
	}
	ui->phIDPagesLb->setWordWrap(true);
	ui->phIDPagesLb->setText(tempS);
	//qDebug() << ph_page;
}

PrintingHistory::~PrintingHistory()
{
	qDeleteAll(printHistoryItemList);
	printHistoryItemList.clear();
	phistoryTb1->setRowCount(0);
}

PrintHistoryItem::PrintHistoryItem(const QFileInfo &_hsitoryInfo, const QString number)
{

	QVariant _dmHistory;
	QVariant _currentID, _id_History;
	if (ParamOp::extractVariantTest(_dmHistory, QVariant(), QString(), -1, _hsitoryInfo.filePath()))
	{
		if (ParamOp::extractVariantTest(_id_History, _dmHistory, number))
		{


			QVariantMap idMap = _id_History.toMap();
			setJobLayers(idMap.value("LAYERS"));
			//setPrintHeadInkUsage(idMap.value("PRINTHEAD_PAGE_START").toInt() - idMap.value("PRINTHEAD_PAGE_END").toInt());
			setJobPrintedPage(idMap.value("PRINTHEAD_PAGE_END").toInt() - idMap.value("PRINTHEAD_PAGE_START").toInt());
			setPrintHeadID(idMap.value("PRINTHEAD_ID"));
			setPrintHeadPageStart(idMap.value("PRINTHEAD_PAGE_START").toInt());
			setPrintHeadPageEnd(idMap.value("PRINTHEAD_PAGE_END").toInt());


			setBuildPlateStart(idMap.value("BUILD_PLATE_START"));
			setFeederPalteStart(idMap.value("FEEDER_START"));

			setBinderStartLevel(idMap.value("BINDER_START_LEVEL"));
			setBinderEndLevel(idMap.value("BINDER_END_LEVEL"));

			setCyanStartLevel(idMap.value("CYAN_START_LEVEL"));
			setCyanEndLevel(idMap.value("CYAN_END_LEVEL"));

			setMagentaStartLevel(idMap.value("MAGENTA_START_LEVEL"));
			setMagentaEndLevel(idMap.value("MAGENTA_END_LEVEL"));

			setYellowStartLevel(idMap.value("YELLOW_START_LEVEL"));
			setYellowEndLevel(idMap.value("YELLOW_END_LEVEL"));

			setWipesStart(idMap.value("WIPES_AT_START"));
			setWipesEnd(idMap.value("WIPES_AT_END"));

			setProjectName(QFileInfo(idMap.value("PROJECT_NAME").toString()).fileName());
			setJobFileName(QFileInfo(idMap.value("JOB_NAME").toString()).fileName());
			setJobThumbnailName(idMap.value("JOB_THUMBNAIL_NAME"));

			setJobStartTime(idMap.value("START_TIME"));
			setJobCompletedTime(idMap.value("BUILD_COMPLETED_ON"));

			QDateTime endTime = QDateTime::fromString(idMap.value("BUILD_COMPLETED_ON").toString(), Qt::ISODate);
			QDateTime startTime = QDateTime::fromString(idMap.value("START_TIME").toString(), Qt::ISODate);
			int elapsedSeconds = startTime.secsTo(endTime);
			setJobElapsedTime(QDateTime::fromMSecsSinceEpoch(elapsedSeconds * 1000).toUTC().toString("hh%1:mm%2:ss%3").arg("hr.").arg("min.").arg("sec."));

			setOutcomeHistory(idMap.value("OUTCOME"));

			setJobPrintheadDropsStart(idMap.value("DOTS_PRINTED_TOTAL_START").toDouble() + idMap.value("DOTS_MAINT_TOTAL_START").toDouble());
			setJobPrintheadDropsEND(idMap.value("DOTS_PRINTED_TOTAL_END").toDouble() + idMap.value("DOTS_MAINT_TOTAL_END").toDouble());

			setHistoryID(idMap.value("id"));
			//historyID_TWI = new QTableWidgetItem(idMap.value("id").toString());
		}

	}
	setCheckState(false);
}

QVariant PrintHistoryItem::getJobLayers() const
{
	return jobLayers;
}

void PrintHistoryItem::setJobLayers(const QVariant &value)
{
	jobLayers = value;
}

QVariant PrintHistoryItem::getPrintHeadInkUsage() const
{
	return printHeadInkUsage;
}

void PrintHistoryItem::setPrintHeadInkUsage(const QVariant &value)
{
	printHeadInkUsage = value;
}

QVariant PrintHistoryItem::getBuildPlateStart() const
{
	return buildPlateStart;
}

void PrintHistoryItem::setBuildPlateStart(const QVariant &value)
{
	buildPlateStart = value;
}

QVariant PrintHistoryItem::getFeederPalteStart() const
{
	return feederPalteStart;
}

void PrintHistoryItem::setFeederPalteStart(const QVariant &value)
{
	feederPalteStart = value;
}

QVariant PrintHistoryItem::getBinderStartLevel() const
{
	return binderStartLevel;
}

void PrintHistoryItem::setBinderStartLevel(const QVariant &value)
{
	binderStartLevel = value;
}

QVariant PrintHistoryItem::getBinderEndLevel() const
{
	return binderEndLevel;
}

void PrintHistoryItem::setBinderEndLevel(const QVariant &value)
{
	binderEndLevel = value;
}

QVariant PrintHistoryItem::getCyanStartLevel() const
{
	return cyanStartLevel;
}

void PrintHistoryItem::setCyanStartLevel(const QVariant &value)
{
	cyanStartLevel = value;
}

QVariant PrintHistoryItem::getCyanEndLevel() const
{
	return cyanEndLevel;
}

void PrintHistoryItem::setCyanEndLevel(const QVariant &value)
{
	cyanEndLevel = value;
}

QVariant PrintHistoryItem::getMagentaStartLevel() const
{
	return magentaStartLevel;
}

void PrintHistoryItem::setMagentaStartLevel(const QVariant &value)
{
	magentaStartLevel = value;
}

QVariant PrintHistoryItem::getMagentaEndLevel() const
{
	return magentaEndLevel;
}

void PrintHistoryItem::setMagentaEndLevel(const QVariant &value)
{
	magentaEndLevel = value;
}

QVariant PrintHistoryItem::getYellowStartLevel() const
{
	return yellowStartLevel;
}

void PrintHistoryItem::setYellowStartLevel(const QVariant &value)
{
	yellowStartLevel = value;
}

QVariant PrintHistoryItem::getYellowEndLevel() const
{
	return yellowEndLevel;
}

void PrintHistoryItem::setYellowEndLevel(const QVariant &value)
{
	yellowEndLevel = value;
}

QVariant PrintHistoryItem::getProjectName() const
{
	return projectName;
}

void PrintHistoryItem::setProjectName(const QVariant &value)
{
	projectName = value;
}


QVariant PrintHistoryItem::getJobFileName() const
{
	return jobFileName;
}

void PrintHistoryItem::setJobFileName(const QVariant &value)
{
	jobFileName = value;
}

QVariant PrintHistoryItem::getJobStartTime() const
{
	return jobStartTime;
}

void PrintHistoryItem::setJobStartTime(const QVariant &value)
{
	jobStartTime = value;
}

QVariant PrintHistoryItem::getOutcomeHistory() const
{
	return outcomeHistory;
}

void PrintHistoryItem::setOutcomeHistory(const QVariant &value)
{
	outcomeHistory = value;
}

QVariant PrintHistoryItem::getJobCompletedTime() const
{
	return jobCompletedTime;
}

void PrintHistoryItem::setJobCompletedTime(const QVariant &value)
{
	jobCompletedTime = value;
}

QVariant PrintHistoryItem::getHistoryID() const
{
	return historyID;
}

void PrintHistoryItem::setHistoryID(const QVariant &value)
{
	historyID = value;
}



QVariant PrintHistoryItem::getListSelected() const
{
	return listSelected;
}

void PrintHistoryItem::setListSelected(const QVariant &value)
{
	listSelected = value;
}

QVariant PrintHistoryItem::getVisible() const
{
	return visible;
}

void PrintHistoryItem::setVisible(const QVariant &value)
{
	visible = value;
}

QVariant PrintHistoryItem::getDetail_On_Off() const
{
	return detail_On_Off;
}

void PrintHistoryItem::setDetail_On_Off(const QVariant &value)
{
	detail_On_Off = value;
}

QVariant PrintHistoryItem::getJobThumbnailName() const
{
	return jobThumbnailName;
}

void PrintHistoryItem::setJobThumbnailName(const QVariant &value)
{
	jobThumbnailName = value;
}

QVariant PrintHistoryItem::getWipesStart() const
{
	return wipesStart;
}

void PrintHistoryItem::setWipesStart(const QVariant &value)
{
	wipesStart = value;
}

QVariant PrintHistoryItem::getWipesEnd() const
{
	return wipesEnd;
}

void PrintHistoryItem::setWipesEnd(const QVariant &value)
{
	wipesEnd = value;
}



QVariant PrintHistoryItem::getJobPrintheadDropsStart() const
{
	return jobPrintheadDropsStart;
}

void PrintHistoryItem::setJobPrintheadDropsStart(const QVariant &value)
{
	jobPrintheadDropsStart = value;
}

QVariant PrintHistoryItem::getJobPrintheadDropsEND() const
{
	return jobPrintheadDropsEND;
}

void PrintHistoryItem::setJobPrintheadDropsEND(const QVariant &value)
{
	jobPrintheadDropsEND = value;
}

QVariant PrintHistoryItem::getJobElapsedTime() const
{
	return jobElapsedTime;
}

void PrintHistoryItem::setJobElapsedTime(const QVariant &value)
{
	jobElapsedTime = value;
}

QVariant PrintHistoryItem::getJobPrintedPage() const
{
	return jobPrintedPage;
}

void PrintHistoryItem::setJobPrintedPage(const QVariant &value)
{
	jobPrintedPage = value;
}

QVariant PrintHistoryItem::getPrintHeadID() const
{
	return printHeadID;
}

void PrintHistoryItem::setPrintHeadID(const QVariant &value)
{
	printHeadID = value;
}

QVariant PrintHistoryItem::getPrintHeadPageStart() const
{
	return printHeadPageStart;
}

void PrintHistoryItem::setPrintHeadPageStart(const QVariant &value)
{
	printHeadPageStart = value;
}

QVariant PrintHistoryItem::getPrintHeadPageEnd() const
{
	return printHeadPageEnd;
}

void PrintHistoryItem::setPrintHeadPageEnd(const QVariant &value)
{
	printHeadPageEnd = value;
}

QVariant PrintHistoryItem::getCheckState() const
{
	return checkState;
}

void PrintHistoryItem::setCheckState(const QVariant &value)
{
	checkState = value;
}