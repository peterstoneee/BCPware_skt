#pragma once
#include <QDialog>
#include <QObject>
#include "ui_printingHistoryUI.h"
#include <QTableWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QGridLayout>
#include <QMessageBox>
#include <QFileInfo>
#include "ParamOp.h"
#include <QDateTime>
#include "../common/define_some_parameter.h"
namespace
{
	class PrintingHistoryDlg;
}


//class PrintHistoryItem
//{
//public:
//	PrintHistoryItem(const QFileInfo &historyFileInfo, const QString i);
//private:
//	QTableWidgetItem *jobLayers;
//
//};

class PrintHistoryItem
{
public:
	PrintHistoryItem(const QFileInfo &historyFileInfo, const QString i);

	QVariant getJobLayers() const;
	void setJobLayers(const QVariant &value);

	QVariant getPrintHeadInkUsage() const;
	void setPrintHeadInkUsage(const QVariant &value);

	QVariant getBuildPlateStart() const;
	void setBuildPlateStart(const QVariant &value);

	QVariant getFeederPalteStart() const;
	void setFeederPalteStart(const QVariant &value);
	/*=====================================================*/
	QVariant getBinderStartLevel() const;
	void setBinderStartLevel(const QVariant &value);

	QVariant getBinderEndLevel() const;
	void setBinderEndLevel(const QVariant &value);
	
	QVariant getCyanStartLevel() const;
	void setCyanStartLevel(const QVariant &value);

	QVariant getCyanEndLevel() const;
	void setCyanEndLevel(const QVariant &value);

	QVariant getMagentaStartLevel() const;
	void setMagentaStartLevel(const QVariant &value);

	QVariant getMagentaEndLevel() const;
	void setMagentaEndLevel(const QVariant &value);

	QVariant getYellowStartLevel() const;
	void setYellowStartLevel(const QVariant &value);

	QVariant getYellowEndLevel() const;
	void setYellowEndLevel(const QVariant &value);
	/*=====================================================*/

	QVariant getProjectName() const;
	void setProjectName(const QVariant &value);

	QVariant getJobFileName() const;
	void setJobFileName(const QVariant &value);

	QVariant getJobStartTime() const;
	void setJobStartTime(const QVariant &value);

	QVariant getOutcomeHistory() const;
	void setOutcomeHistory(const QVariant &value);

	QVariant getJobCompletedTime() const;
	void setJobCompletedTime(const QVariant &value);

	QVariant getHistoryID() const;
	void setHistoryID(const QVariant &value);

	QVariant getListSelected() const;
	void setListSelected(const QVariant &value);

	QVariant getVisible() const;
	void setVisible(const QVariant &value);

	QVariant getDetail_On_Off() const;
	void setDetail_On_Off(const QVariant &value);

	QVariant getJobThumbnailName() const;
	void setJobThumbnailName(const QVariant &value);

	QVariant getWipesStart() const;
	void setWipesStart(const QVariant &value);

	QVariant getWipesEnd() const;
	void setWipesEnd(const QVariant &value);

	QVariant getJobPrintheadDropsStart() const;
	void setJobPrintheadDropsStart(const QVariant &value);

	QVariant getJobPrintheadDropsEND() const;
	void setJobPrintheadDropsEND(const QVariant &value);

	QVariant getJobElapsedTime() const;
	void setJobElapsedTime(const QVariant &value);

	QVariant getJobPrintedPage() const;
	void setJobPrintedPage(const QVariant &value);

	QVariant getPrintHeadID() const;
	void setPrintHeadID(const QVariant &value);

	QVariant getPrintHeadPageStart() const;
	void setPrintHeadPageStart(const QVariant &value);

	QVariant getPrintHeadPageEnd() const;
	void setPrintHeadPageEnd(const QVariant &value);

	QVariant getCheckState() const;
	void setCheckState(const QVariant &value);

	

private:
	QVariant historyID;
	

	QVariant jobLayers;
	QVariant jobPrintedPage;
	QString jobLayersLabel;

	QVariant printHeadInkUsage;
	QVariant printHeadID;
	QVariant printHeadPageStart;
	QVariant printHeadPageEnd;

	QVariant buildPlateStart;
	QVariant feederPalteStart;

	QVariant binderStartLevel;
	QVariant binderEndLevel;

	QVariant cyanStartLevel;
	QVariant cyanEndLevel;

	QVariant magentaStartLevel;
	QVariant magentaEndLevel;

	QVariant yellowStartLevel;
	QVariant yellowEndLevel;

	QVariant projectName;
	QVariant jobFileName;
	QVariant jobThumbnailName;

	QVariant jobStartTime;
	QVariant jobCompletedTime;
	QVariant jobElapsedTime;

	QVariant outcomeHistory;

	//selected check
	//visible
	QVariant listSelected;
	QVariant detail_On_Off;
	QVariant visible;

	QVariant wipesStart;
	QVariant wipesEnd;

	QVariant jobPrintheadDropsStart;
	QVariant jobPrintheadDropsEND;

	QVariant checkState;

};










class PrintingHistory :	public QDialog
{
	Q_OBJECT
public:
	explicit PrintingHistory(QWidget *parent = nullptr);
	~PrintingHistory();
	struct SubItemStruct
	{
		QString name;
		QVariant value;
		SubItemStruct(QString ss, QVariant _value)
		{
			name = ss;
			value = _value;
		}
	};

private:
	short historyColumnNum;
	short testX;
	short phIDSwitch;
	void updateTable();
	void initTableInformation();
	QTableWidget *phistoryTb1;
	Ui::PrintingHistoryDlg *ui;
	QList<PrintHistoryItem *> printHistoryItemList;
	QList<SubItemStruct *> subItemList;
	QList<QPair<QString, QVariant>> mainItemList;
	struct{
		QPushButton *pb1;

	};
	void count_Total_Printhead();
	QAction *selectAllAct;
	QAction *cancelAct;

};

