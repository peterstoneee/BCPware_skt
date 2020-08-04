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
	historyColumnNum = 9;
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
		QPushButton*pb = new QPushButton("detail",this);

		QLabel *lb1 = new QLabel(this);
		const int w = lb1->width();
		const int h = lb1->height();
		
		qDebug() << "setPixmap : " << printHistoryItemList.at(i)->getJobThumbnailName().toString();
		lb1->setPixmap(QPixmap(printHistoryItemList.at(i)->getJobThumbnailName().toString()).scaled(w,h,Qt::KeepAspectRatio));

		connect(chb, static_cast<void (QCheckBox::*)(int)>(&QCheckBox::stateChanged), [&, i](int ix)
		{
			if (ix)
			{
				printHistoryItemList.at(i)->setCheckState(true);
			}
			else
			{
				printHistoryItemList.at(i)->setCheckState(false);
			}
		});
		connect(pb, &QPushButton::clicked, [=](){

			PrintingHistoryDetail *phD = new PrintingHistoryDetail(printHistoryItemList.at(i)->getHistoryID().toInt(), this);
			phD->setWindowFlags(Qt::Popup);
			phD->setAttribute(Qt::WA_DeleteOnClose);
			phD->move(200, 200);
			phD->exec();


			//QDialog *dia = new QDialog(this);
			//dia->setWindowTitle("Detail");
			//dia->setFixedSize(1000, 800);
			//QGridLayout *gdl = new QGridLayout();
			//QTableWidget *subTable = new QTableWidget(10,6);
			//subTable->setColumnWidth(0, 200);
			//subTable->setColumnWidth(2, 200);
			//subTable->setColumnWidth(4, 200);
			///*init subItem struct*/
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
		});
		//ui->listWidget, static_cast<void (QListWidget::*)(int)>(&QListWidget::currentRowChanged), [=](int ix)
		


		phistoryTb1->setCellWidget(i, 0, chbF);
		phistoryTb1->setItem(i, 1, new QTableWidgetItem(printHistoryItemList.at(i)->getHistoryID().toString()));	
		phistoryTb1->setCellWidget(i, 2, lb1);
		//project name
		phistoryTb1->setItem(i, 3, new QTableWidgetItem(printHistoryItemList.at(i)->getProjectName().toString()));
		phistoryTb1->setItem(i, 4, new QTableWidgetItem(printHistoryItemList.at(i)->getOutcomeHistory().toString()));
		phistoryTb1->setItem(i, 5, new QTableWidgetItem(printHistoryItemList.at(i)->getJobCompletedTime().toString()));

		phistoryTb1->setItem(i, 6, new QTableWidgetItem(printHistoryItemList.at(i)->getJobElapsedTime().toString()));
		
		phistoryTb1->setItem(i, 7, new QTableWidgetItem(printHistoryItemList.at(i)->getPrintHeadID().toString()));

		phistoryTb1->setCellWidget(i, 8, pb);

	}
	connect(phistoryTb1, static_cast<void(QTableWidget::*)(int, int)>(&QTableWidget::cellClicked), [&](int x, int y){
		qDebug() << "qtablewidget x,y : " << x << y;

	});
}
void PrintingHistory::initTableInformation()
{
	QStringList header;
	header << ""<< "id"<< "Preview"<<"ProjectName"<<"Outcome"<<"Finished at"<<"Print Time"<<"Printhead ID"<<"Detail";
	phistoryTb1 = new QTableWidget(printHistoryItemList.size(), historyColumnNum);
	phistoryTb1->setHorizontalHeaderLabels(header);
	phistoryTb1->setColumnWidth(5, 150);
	phistoryTb1->verticalHeader()->setVisible(false);
	phistoryTb1->setStyleSheet(WidgetStyleSheet::historyTableWidget());
	
	/**/
	updateTable();
	

	
	
	
	QHeaderView *headerV = qobject_cast<QTableWidget *>(phistoryTb1)->horizontalHeader();
	connect(headerV, &QHeaderView::sectionClicked, [this](int logicalIndex){
	
		switch (logicalIndex)
		{
		case 1:
		{
			if (testX)
			{
				qSort(printHistoryItemList.begin(), printHistoryItemList.end(), [&](const PrintHistoryItem *infoA, const PrintHistoryItem *infoB){
					return infoA->getHistoryID().toInt() > infoB->getHistoryID().toInt(); });

				updateTable();
			}
			else
			{
				qSort(printHistoryItemList.begin(), printHistoryItemList.end(), [&](const PrintHistoryItem *infoA, const PrintHistoryItem *infoB){
					return infoA->getHistoryID().toInt() < infoB->getHistoryID().toInt(); });

				updateTable();
			}

			testX ^= 1;
		}
			break;
		case 5:
		{
			if (testX)
			{
				qSort(printHistoryItemList.begin(), printHistoryItemList.end(), [&](const PrintHistoryItem *infoA, const PrintHistoryItem *infoB){
					qDebug() << infoA->getJobElapsedTime().toString();
					QDateTime completeTimeA = QDateTime::fromString(infoA->getJobCompletedTime().toString(), Qt::ISODate); //QDateTime::fromString(infoA->getJobElapsedTime().toString(), "hh:mm:ss");
					QDateTime completeTimeB = QDateTime::fromString(infoB->getJobCompletedTime().toString(), Qt::ISODate);
					qDebug() << "completeTimeA : " << completeTimeA.toMSecsSinceEpoch() << "completeTimeB : " << completeTimeB.toMSecsSinceEpoch();

					return completeTimeA.toMSecsSinceEpoch() > completeTimeB.toMSecsSinceEpoch();
				});

				updateTable();
			}
			else
			{
				qSort(printHistoryItemList.begin(), printHistoryItemList.end(), [&](const PrintHistoryItem *infoA, const PrintHistoryItem *infoB){
					qDebug() << infoA->getJobElapsedTime().toString();
					QDateTime completeTimeA = QDateTime::fromString(infoA->getJobCompletedTime().toString(), Qt::ISODate); //QDateTime::fromString(infoA->getJobElapsedTime().toString(), "hh:mm:ss");
					QDateTime completeTimeB = QDateTime::fromString(infoB->getJobCompletedTime().toString(), Qt::ISODate);
					qDebug() << "completeTimeA : " << completeTimeA.toMSecsSinceEpoch() << "completeTimeB : " << completeTimeB.toMSecsSinceEpoch();

					return completeTimeA.toMSecsSinceEpoch() < completeTimeB.toMSecsSinceEpoch();
				});

				updateTable();
			}
			testX ^= 1;

		}
		break;
		case 6:
		{
			if (testX)
			{
				qSort(printHistoryItemList.begin(), printHistoryItemList.end(), [&](const PrintHistoryItem *infoA, const PrintHistoryItem *infoB){
					qDebug() << infoA->getJobElapsedTime().toString();
					QDateTime elapsedTimeA = QDateTime::fromString(infoA->getJobElapsedTime().toString(), "hh:mm:ss");
					QDateTime elapsedTimeB = QDateTime::fromString(infoB->getJobElapsedTime().toString(), "hh:mm:ss");
					qDebug() << "elapsedTimeA : " << elapsedTimeA.toMSecsSinceEpoch() << "elapsedTimeB : " << elapsedTimeB.toMSecsSinceEpoch();

					return elapsedTimeA.toMSecsSinceEpoch() > elapsedTimeB.toMSecsSinceEpoch();
				});

				updateTable();
			}
			else
			{
				qSort(printHistoryItemList.begin(), printHistoryItemList.end(), [&](const PrintHistoryItem *infoA, const PrintHistoryItem *infoB){
					QDateTime elapsedTimeA = QDateTime::fromString(infoA->getJobElapsedTime().toString(), Qt::ISODate);
					QDateTime elapsedTimeB = QDateTime::fromString(infoB->getJobElapsedTime().toString(), Qt::ISODate);
					qDebug() << "elapsedTimeA : " << elapsedTimeA.toMSecsSinceEpoch() << "elapsedTimeB : " << elapsedTimeB.toMSecsSinceEpoch();

					return elapsedTimeA.toMSecsSinceEpoch()< elapsedTimeB.toMSecsSinceEpoch();
				});

				updateTable();
			}
			testX ^= 1;
			
		}
			break;
		case 4:
		{
			QCollator coll;
			coll.setNumericMode(true);
			if (phIDSwitch)
			{

				qSort(printHistoryItemList.begin(), printHistoryItemList.end(), [&](const PrintHistoryItem *infoA, const PrintHistoryItem *infoB){
					QString outcomeA = infoA->getOutcomeHistory().toString();
					QString outcomeB = infoB->getOutcomeHistory().toString();
					qDebug() << "printheadIDA : " << outcomeA.toLower() << "outcomeB : " << outcomeB.toLower();
					qDebug() << (coll.compare(outcomeA, outcomeB) < 0);
					return  outcomeA > outcomeB;// coll.compare(printheadIDA, printheadIDB) < 0;
				});
				updateTable();
			}
			else
			{
				qStableSort(printHistoryItemList.begin(), printHistoryItemList.end(), [&](const PrintHistoryItem *infoA, const PrintHistoryItem *infoB){
					QString outcomeA = infoA->getOutcomeHistory().toString();
					QString outcomeB = infoB->getOutcomeHistory().toString();					
					return  outcomeA < outcomeB;// coll.compare(printheadIDA, printheadIDB) < 0;
				});
				updateTable();
			}
			phIDSwitch ^= 1;
		}break;
		case 7:
		{
			QCollator coll;
			coll.setNumericMode(true);
			if (phIDSwitch)
			{

				qSort(printHistoryItemList.begin(), printHistoryItemList.end(), [&](const PrintHistoryItem *infoA, const PrintHistoryItem *infoB){
					QString printheadIDA = infoA->getPrintHeadID().toString();
					QString printheadIDB = infoB->getPrintHeadID().toString();
					qDebug() << "printheadIDA : " << printheadIDA.toLower() << "printheadIDB : " << printheadIDB.toLower();
					qDebug() << (coll.compare(printheadIDA, printheadIDB) < 0);
					return  printheadIDA > printheadIDB;// coll.compare(printheadIDA, printheadIDB) < 0;
				});
				updateTable();
			}
			else
			{
				qStableSort(printHistoryItemList.begin(), printHistoryItemList.end(), [&](const PrintHistoryItem *infoA, const PrintHistoryItem *infoB){
					QString printheadIDA = infoA->getPrintHeadID().toString();
					QString printheadIDB = infoB->getPrintHeadID().toString();
					qDebug() << "printheadIDA : " << printheadIDA.toLower() << "printheadIDB : " << printheadIDB.toLower();

					return  printheadIDA < printheadIDB; //coll.compare(printheadIDA, printheadIDB)>0;
				});
				updateTable();
			}
			phIDSwitch ^= 1;
		}break;
		case 2:
			break;
		}

		printHistoryItemList.size();
	
		QString text = phistoryTb1->horizontalHeaderItem(logicalIndex)->text();
		qDebug() << logicalIndex << text << testX;
	});
	
	
	QGridLayout *la1 = new QGridLayout();
	la1->setMargin(24);
	la1->addWidget(phistoryTb1);
	qDebug() <<"phistoryTb1->width() : "<< phistoryTb1->width();
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
		qDebug() << phidS <<":"<< pages.last();
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
			setJobElapsedTime(QDateTime::fromMSecsSinceEpoch(elapsedSeconds*1000).toUTC().toString("hh:mm:ss"));

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