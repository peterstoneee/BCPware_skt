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


class CustomArc :public QWidget
{
	Q_OBJECT
public:
	explicit CustomArc(QString inktype, QVector<float> value, QVector<QColor> arcColor, float startAngle = 90, QString strValue = QString(), QColor bg_Color = QColor(233, 233, 233), int radius = 150, float valueSum = 100, bool clockWise = false, int penWidth = 3, QWidget *parent = nullptr);
	bool clockWise;
	float valueSum;
	QVector<float> inputValue;

	int radius;
	float startAngle;
	QVector<QColor> arc_Color;
	QColor bg_Color;
	QString strValue;
	int penWidth;
	int upperY;
	QString inkType;

private:
	void drawArc(QPainter *);
	void drawBG(QPainter *);
	void drawInfo(QPainter *);
	void drawCircle(QPainter *);
protected:
	void paintEvent(QPaintEvent *event);
};


class CustomBar : public QWidget
{
	Q_OBJECT
public:
	explicit  CustomBar(QWidget *parent = nullptr);
private:
	void drawBG(QPainter *);
	void drawRuler(QPainter *);
	void drawBarBG(QPainter *);
	void drawBar(QPainter *);

	QRectF barRect;
protected:
	void paintEvent(QPaintEvent *event);
};

class CustomBarSecond : public QWidget
{
	Q_OBJECT
public:
	explicit  CustomBarSecond(QWidget *parent = nullptr);
	CustomBarSecond(bool _wOrP, int _minBarValue, int _maxBarValue, QVariant _startValue, QVariant _endValue, QVariant _usedValue, QVariant _printHeadID = QVariant(), QWidget *parent = nullptr);
private:

	void drawBarBG(QPainter *);
	void drawBar(QPainter *);
	void drawInfo(QPainter *);

	QRectF barRect;
	bool wiperOrPrintHead;
	int minBarValue;
	int maxBarValue;
	QVariant startValue;
	QVariant endValue;
	QVariant usedValue;
	QString printHeadID;//
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

	void initSummary();

	QWidget *factoryLabel(QString decription, QString value);
	
	Ui::HistoryDetailDia *ui;
	PrintHistoryItem *inPHI;
	QVariantMap itemMap;
	QVariantMap sliceSettingMap;
	QVariantList sliceSettingList;


	

	
};

