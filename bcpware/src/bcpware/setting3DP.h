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
#pragma once

#include <QVariant>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include <QTimer>
#include <QPixmap>
#include <QPainter>
#include <QDialog>
#include <QWidget>
#include <QStackedWidget>
#include <QListWidget>
#include <QSettings>
#include <QScrollArea>


#include "mainwindow.h"

#include "../common/filterparameter.h"
#include "ui_setting_UI.h"
#include <QSpinBox>
#include "stdpardialog.h"
#include "./CMD_Value.h"

namespace
{
	class setting3DP_UI;
}
//TextLabelUI
//DSpinBox
//EnumUI
//TextfieldUI

extern Comm3DP *comm;

class SKTWidget :public QWidget
{
	Q_OBJECT

public:
	enum WidgetType
	{
		Spinbox = 0x00,
		DSpinBox = 0x01,
		EnumUI = 0x02,
		TextfieldUI = 0x03
	} uiType;
	SKTWidget(QWidget *p) :QWidget(p)
	{
	}
	SKTWidget(QWidget *p, bool _visible, QVariant _categoryName, QVariant _defaultValue, QVariant _uiUnit = NULL, int _transformType = 0)
		:QWidget(p), visible(_visible), categoryName(_categoryName), defaultValue(_defaultValue), uiUnit(_uiUnit), transformType(_transformType)
	{
		uiUnitlabel = new QLabel(uiUnit.toString());
	}
	virtual void addWidgetToGridLayout(QGridLayout* lay, const int r, const int c) = 0;
	virtual void updateUIValue(QVariant _value)  = 0;
	
	QVariant getValue(){ return value; }
	void setValue(QVariant _value){ value = _value; }
	
	QVariant getIdentifyName(){ return identifyName; }
	void setIdentifyName(QVariant _identifyName){ identifyName = _identifyName; }

	void setUiType(WidgetType _uitype)
	{
		uiType = _uitype;
	}
	WidgetType getUiType(){ return uiType; }

	void setTransformType(int _transformtype){ transformType = _transformtype; }
	int getTransformType(){ return transformType; }

	void setlVisible(bool _visible){ visible = _visible; }
	bool getVisible(){ return visible; }
	
	void setUiUnit(QVariant _uiUnit){ uiUnit = _uiUnit; }
	QVariant getUiUnit(){ return uiUnit; }

	void setCategoryName(QVariant _categoryName){ categoryName = _categoryName; }
	QVariant getCategoryName(){ return categoryName; }

	void setDefaultValue(QVariant _defaultValue){ defaultValue = _defaultValue; }
	QVariant getDefaultValue(){ return defaultValue; }

	//void 

private:
	QString spinBoxName;
	//create
signals:
	void parameterChanged();
	void stkValueChanged();

protected:
	QVariant value;
	QVariant categoryName;
	QVariant identifyName;
	bool visible;
	QVariant uiUnit;
	QLabel *uiUnitlabel;
	QVariant defaultValue;
	int transformType;

	
	//Value &getWidgetValue();
};

class SpinBoxWidget_SKX : public  SKTWidget
{
	Q_OBJECT
public:
	SpinBoxWidget_SKX(QWidget *p, QString labelName, QString _identifyName, QVariant categoryName, QVariant _value, QVariant _defaultValue,QVariant _uiUnit = NULL, int _transformType = 0, bool _visible = true, int highlimit = 0, int lowLimit = 0)
		:SKTWidget(p, _visible, categoryName,_defaultValue, _uiUnit, _transformType)
	{
		setValue(_value);
		identifyName = _identifyName;
		hlayout = new QHBoxLayout();
		label = new QLabel(labelName);

		spinBox = new QSpinBox();
		spinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
		spinBox->setMaximum(highlimit);
		spinBox->setMinimum(lowLimit);
		spinBox->setValue(value.toInt());
		spinBox->setFixedWidth(150);
		spinBox->setAlignment(Qt::AlignRight);
		if (!getUiUnit().isNull())
		{
			spinBox->setSuffix(" " + getUiUnit().toString());
		}
		
		
		hlayout->addWidget(label);
		hlayout->addWidget(spinBox);
		this->setLayout(hlayout);

		connect(spinBox, SIGNAL(valueChanged(int)), this, SIGNAL(parameterChanged()));
		connect(spinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [&](int _value) {
			setValue(_value);
			qDebug() << "spinBox"<<getValue();
		});
	}
	void addWidgetToGridLayout(QGridLayout* lay, const int r, const int c) 
	{		
		int col = c;
		lay->addWidget(this, r, col);
		/*if (!getUiUnit().isNull())
		{
			lay->addWidget(uiUnitlabel, r, ++col);
		}*/
	}
	void updateUIValue(QVariant _value)
	{
		spinBox->setValue(_value.toInt());		
	}
	
private:
	QLabel *label;
	QSpinBox *spinBox;
	QHBoxLayout *hlayout;	
	
};

class DoubleSpinBox_SKX :public SKTWidget
{
	Q_OBJECT
public:
	DoubleSpinBox_SKX(QWidget *p, QString labelName, QString _identifyName, QVariant _categoryName, QVariant _value, QVariant _defaultValue, QVariant _uiUnit = NULL, int _transformType = 0, bool _visible = true, int highlimit = 0, int lowLimit = 0)
		:SKTWidget(p, _visible, _categoryName, _defaultValue, _uiUnit, _transformType)
	{		
		setValue(_value);
		identifyName = _identifyName;
		hlayout = new QHBoxLayout();
		label = new QLabel(labelName);

		spinBox = new QDoubleSpinBox();
		spinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
		spinBox->setMaximum(highlimit);
		spinBox->setMinimum(lowLimit);
		spinBox->setFixedWidth(150);
		spinBox->setAlignment(Qt::AlignRight);
		if (!getUiUnit().isNull())
		{
			spinBox->setSuffix(" "+getUiUnit().toString());
		}
		if (getTransformType() > 0)
		{
			spinBox->setValue(transformDataFromFPGAToUI(getValue(), getTransformType()).toDouble());
		}
		else
		{
			spinBox->setValue(value.toDouble());
		}
		hlayout->addWidget(label);
		hlayout->addWidget(spinBox);
		this->setLayout(hlayout);

		connect(spinBox, SIGNAL(valueChanged(double)), this, SIGNAL(parameterChanged()));
		connect(spinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [&](double _value_) {
			setValue(transformDataFromUIToFPGA(_value_, getTransformType()));

			qDebug() << "doublespinbox" << getValue();
		});
	}
	void addWidgetToGridLayout(QGridLayout* lay, const int r, const int c)
	{
		int col = c;
		lay->addWidget(this, r, col);
		/*if (!getUiUnit().isNull())
		{
			lay->addWidget(uiUnitlabel, r, ++col);
		}*/
	}
	void updateUIValue(QVariant _value)
	{
		if (getTransformType() > 0)
		{
			spinBox->setValue(transformDataFromFPGAToUI(_value, getTransformType()).toDouble());
		}
		else
		{
			spinBox->setValue(_value.toDouble());
		}
	}
private:
	QLabel *label;
	QDoubleSpinBox *spinBox;
	QHBoxLayout *hlayout;
	int transformType;

	QVariant transformDataFromUIToFPGA(QVariant value, int trasformEnum)
	{
		double dvalue = value.toDouble();
		switch (trasformEnum)
		{
		case 1:     return int((1. / dvalue)*pow(10, 6));
		case 2:     return int(50 * dvalue);
		case 3:     return int((9.375 / dvalue)*pow(10, 5));
		case 4:     return int(3200 * dvalue);
		case 5:     return int(24804.68 / dvalue);
		case 6:     return int(2015.74803*dvalue);
		//case 6:     return int(5039.37*dvalue);
		default:
			return value;// Do nothing
			break;
		}
		return QVariant();
	}
	/*--------------------------------------------------------------------------------------------------------------------*/

	QVariant transformDataFromFPGAToUI(const QVariant value, int trasformEnum)
	{
		int fvalue = value.toInt();
		switch (trasformEnum)
		{
		case 1:     return (1. / fvalue)*pow(10, 6);
		case 2:     return (fvalue / 50.);
		case 3:     return (9.375 / fvalue)* pow(10, 5);
		case 4:     return fvalue / 3200.;
		case 5:     return 24804.69 / fvalue;
		case 6:     return fvalue / 2015.74803;
		//case 6:     return fvalue / 5039.37;
		default:
			return value;// Do nothing
			break;
		}
		return QVariant();
	}
	/*--------------------------------------------------------------------------------------------------------------------*/


};

class TextLabelUI_SKX : public  SKTWidget
{
	Q_OBJECT
public:
	//QWidget *p, QString labelName, QString _identifyName, QVariant _categoryName, QVariant _value, QVariant _defaultValue, QVariant _uiUnit = NULL, int _transformType = 0, bool _visible = true, int highlimit = 0, int lowLimit = 0
	TextLabelUI_SKX(QWidget *p, QString labelName, QString _identifyName, QVariant _categoryName, QVariant _value, QVariant _defaultValue) :
		SKTWidget(p, true, _categoryName, _defaultValue)
	{
		setValue(_value);
		identifyName = _identifyName;
		hlayout = new QHBoxLayout();
		label = new QLabel(labelName);
		labelValue = new QLineEdit(value.toString());
		labelValue->setReadOnly(true);
		labelValue->setFixedWidth(250);
		labelValue->setTextMargins(4, 0, 4, 0);
		hlayout->addWidget(label);
		hlayout->addWidget(labelValue);
		this->setLayout(hlayout);		

		connect(labelValue, &QLineEdit::textChanged, this, [&](const QString _value_) {
			setValue(_value_);

			qDebug() << label->text() << "TextLabelUI_SKX" << getValue();
		});


	}
	void addWidgetToGridLayout(QGridLayout* lay, const int r, const int c)
	{
		lay->addWidget(this, r, c);		
	}
	void updateUIValue(QVariant _value)
	{
		labelValue->setText(_value.toString());
	}
	
private:
	QLabel *label;
	QLineEdit *labelValue;
	//QSpinBox *spinBox;
	QHBoxLayout *hlayout;
	
};

class EnumUI_SKX : public SKTWidget
{
	Q_OBJECT
public:
	EnumUI_SKX(QWidget *p, QString labelName, QString _identifyName, QVariant value,QVariant enumList, int highlimit = 0, int lowLimit = 0) :SKTWidget(p)
	{
		identifyName = _identifyName;
		hlayout = new QHBoxLayout();
		label = new QLabel(labelName);
		comboBox = new QComboBox();
		comboBox->addItems(enumList.toStringList());
		hlayout->addWidget(label);
		hlayout->addWidget(comboBox);
		this->setLayout(hlayout);
		connect(comboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [&](int _value) {
			setValue(_value);
			qDebug() << "QComboBox" << getValue();
		});
	}
	void addWidgetToGridLayout(QGridLayout* lay, const int r)
	{
		lay->addWidget(this, r, 0);
		//lay->addLayout(hlayout, r, 0);
	}
	void setTransformType(int _transformtype){ transformType = _transformtype; }
	int getTransformType(){ return transformType; }
private:
	QLabel *label;
	QComboBox *comboBox;
	QHBoxLayout *hlayout;
	int transformType;
};




//class LineEditWidget : SKTWidget
//{
//	Q_OBJECT
//};
//class ComeboWidget : SKTWidget
//{
//	Q_OBJECT
//};
//class DoubleSpinWidget : SKTWidget
//{
//	Q_OBJECT
//};

class SpinboxDecoration:public QWidget
{
	Q_OBJECT
public:
	SpinboxDecoration(){}
	SpinboxDecoration(QString _spinBoxName, int _value){ initSpinboxDecoration(_spinBoxName, _value); }
	~SpinboxDecoration(){}
	void initSpinboxDecoration(QString spinBoxName, int value, int defaultValue = 123)
	{
		QHBoxLayout *hlayout = new QHBoxLayout();
		label = new QLabel(spinBoxName);
		spinBox = new QSpinBox();
		spinBox->setValue(value);
		hlayout->addWidget(label);
		hlayout->addWidget(spinBox);
		this->setLayout(hlayout);
		connect(spinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [&](int _value) {
			setValue(_value);
		});
	}

	int getValue(){ return value; }
	void setValue(int _value){
		value = _value; 
	}
	QString getName(){ return name; }
	void setName(QString _name){ name = _name; }

protected:

private:
	QSpinBox *spinBox;
	QLabel *label;
	QString name;
	int value;
};

#define NVM_SETTING_NAME "Advanced_Setting"
#define PRINTER_SETTING_NAME "Basic_Setting"
#define SLICING_SETTING_NAME "Printer_Setting"

class Setting3DP:public QDialog
{
	Q_OBJECT
public:	

	Setting3DP() {}
	Setting3DP(MainWindow *_mw, RichParameterSet *,QWidget *parent = 0);
	~Setting3DP();

	static void initSetting(RichParameterSet *);
	void initDefaultSetting();
	//setting loadfromregister();	
	void initWidgetParam();


public slots:
	//save setting to register and pack as RichParameter
	void getaccept();
	void importSampleFile();
	void sendUsage();
	void getColorProfile();
	void getDMICMColorProfile();
	void resetSetting();
	/*---------NVM another accept--------------*/
	void updateTOFile();
	
	
private:
	void acceptOne(RichParameter *inpar);	
	Ui::setting3DP_UI *ui;
	RichParameterSet *loadtoWidgetParam;
	RichParameterSet defaultParam;
	int undoLimit;
	MainWindow *mw;
	bool switchSetting;
	bool switchDM;
	QKeySequence *ks;
	QString colorProfilePath;
	QString colorProfilePathForDitherMatrix;
	int getWiperUsage();
	QString rColorProfilePath();
	QString password;

	/*---------------------------------------------------------*/
	void createNVMPage();
	void saveToRichParameterAndJson();
	void createParamSettingUI(QString);
	
	QMap<QString,QVector<SKTWidget *> *> paramWidgetVector;
	QStringList paramType;
	/*---------------------------------------------------------*/
	QPushButton *updateToFPGAButton;
	QPushButton *getFromFPGA;
	QPushButton *setDefaultValueButton;
	void sendNVMPreProcess();
	void getNVMFromFPGA();
	//void updateTOFile();
	bool updateUIFromJsonFile();
	//void update

	
	
	
protected:
	void keyPressEvent(QKeyEvent *e);
	


};


