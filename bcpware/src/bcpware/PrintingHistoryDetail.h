#pragma once
#include <QDialog>
#include <QObject>
#include "printingHistory.h"
#include "ui_historyDetail.h"
#include <QGroupBox>
#include <QPaintEvent>
#include <QPainter>
#include <QColor>




namespace
{
	class HistoryDetailDia;
}


class PrintHistoryItem;


class InformationWidget : public QWidget
{
	Q_OBJECT
public:
	explicit InformationWidget(QWidget *parent = nullptr);
protected : 
	void paintEvent(QPaintEvent *);
private:
	//Q

};


class CustomPie :public QWidget
{
	Q_OBJECT
public:
	explicit CustomPie(float value, QColor pie_Color, float startAngle = 90, QString strValue = QString(), QColor bg_Color = QColor(233, 233, 233), int radius = 99, float valueSum = 100, bool clockWise = false, QWidget *parent = nullptr);
	bool clockWise;
	float valueSum;
	float value;
	int radius;
	float startAngle;
	QColor pie_Color;
	QColor bg_Color;
	QString strValue;

private:
	void drawPie(QPainter *);
	void drawBG(QPainter *);
protected:
	void paintEvent(QPaintEvent *event);
};









class PrintingHistoryDetail : public QDialog
{
	Q_OBJECT
public:
	explicit PrintingHistoryDetail(int i, QWidget *parent = nullptr);
	~PrintingHistoryDetail();
private:
	void initConsumeTab();
	void initSliceTab();
	QWidget *factoryLabel(QString decription, QString value);
	
	Ui::HistoryDetailDia *ui;
	PrintHistoryItem *inPHI;
	QVariantMap itemMap;
	QVariantMap sliceSettingMap;
	QVariantList sliceSettingList;


	

	
};

