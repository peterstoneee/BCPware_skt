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
#include <QCheckBox>
#include "qaesencryption.h"

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
		:QWidget(p), visible(_visible), categoryName(_categoryName), defaultValue(_defaultValue), uiUnit(_uiUnit), transformType(_transformType), changed(false)
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

	void setChanged(bool _changed){ changed = _changed; }
	bool getChanged(){ return changed; }

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
	bool changed;

	
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
		hlayout->addStretch();
		hlayout->addWidget(spinBox);
		this->setLayout(hlayout);

		//connect(spinBox, SIGNAL(valueChanged(int)), this, SIGNAL(parameterChanged()));
		connect(spinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [&](int _widget_value) {
			setValue(_widget_value);
			setChanged(true);
			label->setStyleSheet("QLabel {  color : red; }");
			qDebug() << "spinBox"<<getValue();
			emit parameterChanged();
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
		spinBox->setDecimals(4);
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
		hlayout->addStretch();
		hlayout->addWidget(spinBox);
		this->setLayout(hlayout);

		//connect(spinBox, SIGNAL(valueChanged(double)), this, SIGNAL(parameterChanged()));
		connect(spinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [&](double _widget_value) {
			setValue(transformDataFromUIToFPGA(_widget_value, getTransformType()));
			setChanged(true);
			label->setStyleSheet("QLabel {  color : red; }");
			qDebug() << "doublespinbox" << getValue();
			emit parameterChanged();
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
	TextLabelUI_SKX(QWidget *p, QString labelName, QString _identifyName, QVariant _categoryName, QVariant _value, QVariant _defaultValue,QVariant _uiUnit = NULL, int _transformType = 0, bool _visible = true) :
		SKTWidget(p, true, _categoryName, _defaultValue)
	{
		setValue(_value);
		setIdentifyName(_identifyName);
		hlayout = new QHBoxLayout();

		label = new QLabel(labelName);
		labelValue = new QLineEdit(value.toString());		
		labelValue->setReadOnly(true);
		labelValue->setFixedWidth(250);
		labelValue->setTextMargins(4, 0, 4, 0);

		hlayout->addWidget(label);
		hlayout->addStretch();
		hlayout->addWidget(labelValue);
		this->setLayout(hlayout);		


		//connect(labelValue, SIGNAL(textChanged(QString)), this, SIGNAL(parameterChanged()));

		connect(labelValue, &QLineEdit::textChanged, this, [&](const QString _widget_value) {
			setValue(_widget_value);
			setChanged(true);
			qDebug() << label->text() << "TextLabelUI_SKX" << getValue();
			label->setStyleSheet("QLabel {  color : red; }");
			emit parameterChanged();
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
	EnumUI_SKX(QWidget *p, QString labelName, QString _identifyName, QVariant _categoryName, QVariant _value, QVariant _defaultValue, QVariant enumList, QVariant _uiUnit = NULL, bool _visible = true)
		:SKTWidget(p, true, _categoryName, _defaultValue)
	{
		setValue(_value);
		setIdentifyName( _identifyName);
		hlayout = new QHBoxLayout();
		label = new QLabel(labelName);
		comboBox = new QComboBox();
		comboBox->addItems(enumList.toStringList());
		comboBox->setCurrentIndex(_value.toInt());
		hlayout->addWidget(label);
		hlayout->addStretch();
		hlayout->addWidget(comboBox);
		this->setLayout(hlayout);

		//connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SIGNAL(parameterChanged()));
		connect(comboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [&](int _widget_value) {
			setValue(_widget_value);
			setChanged(true);
			label->setStyleSheet("QLabel {  color : red; }");
			qDebug() << "QComboBox" << getValue();
			emit parameterChanged();
		});
	}
	void addWidgetToGridLayout(QGridLayout* lay, const int r,const int c)
	{
		lay->addWidget(this, r, c);
		//lay->addLayout(hlayout, r, 0);
	}
	void updateUIValue(QVariant _index)
	{
		comboBox->setCurrentIndex(_index.toInt());
	}
private:
	QLabel *label;
	QComboBox *comboBox;
	QHBoxLayout *hlayout;
	
};

class CheckUI_SKX : public SKTWidget
{
	Q_OBJECT
public:
	CheckUI_SKX(QWidget *p, QString labelName, QString _identifyName, QVariant _categoryName, QVariant _value, QVariant _defaultValue, QVariant _uiUnit = NULL, bool _visible = true)
		:SKTWidget(p,true, _categoryName, _defaultValue)
	{
		setValue(_value);
		setIdentifyName(_identifyName);
		hlayout = new QHBoxLayout();
		label = new QLabel(labelName);
		checkBox = new QCheckBox;
		checkBox->setChecked(_value.toBool());
		hlayout->addWidget(label);
		hlayout->addStretch();
		hlayout->addWidget(checkBox);
		this->setLayout(hlayout);

		connect(checkBox, SIGNAL(stateChanged(int)), this, SIGNAL(parameterChanged()));
		connect(checkBox, static_cast<void (QCheckBox::*)(int)>(&QCheckBox::stateChanged), this, [&](int _widget_value)
		{
			setValue(_widget_value);
			setChanged(true);
			label->setStyleSheet("QLabel {  color : red; }");
		});
		
	}
	void addWidgetToGridLayout(QGridLayout* lay, const int r, const int c)
	{
		lay->addWidget(this, r, c);
		
	}
	void updateUIValue(QVariant _bool)
	{
		checkBox->setChecked(_bool.toBool());
	}
private:
	QLabel *label;
	QCheckBox *checkBox;
	QHBoxLayout *hlayout;


};

//TO DO
/*
A button for open file
A label for show the filename
*/
class FileDialog_SKX : public SKTWidget
{
	Q_OBJECT
public:
	FileDialog_SKX(QWidget *p, QString labelName, QString _identifyName, QVariant _categoryName, QVariant _value, QVariant _defaultValue )
		:SKTWidget(p, true, _categoryName, _defaultValue)
	{
		setValue(_value);
		identifyName = _identifyName;

		hlayout = new QHBoxLayout();
		label = new QLabel(labelName);
		valueLabel = new QLabel(_value.toString());
		valueLabel->setWordWrap(true);
		valueLabel->setFixedWidth(200);

		
		openFileDialogButton = new QPushButton("Open File Dialog");
		
		//connect(valueLabel, SIGNAL(stateChanged(int)), this, SIGNAL(parameterChanged()));
		
		connect(openFileDialogButton, &QPushButton::clicked, [=]() {
			QDir defaultPath(getValue().toString());
			QString filePath = QFileDialog::getOpenFileName(this, tr("Get Color Profile"), defaultPath.absolutePath(), "Color Profile (*.icc *.icm;)");
			valueLabel->setText(filePath);
			setValue(filePath);
			setChanged(true);
			label->setStyleSheet("QLabel {  color : red; }");
			emit parameterChanged();

		});


		hlayout->addWidget(label);
		hlayout->addStretch();
		hlayout->addWidget(openFileDialogButton);
		hlayout->addWidget(valueLabel);
		this->setLayout(hlayout);
	}

	void addWidgetToGridLayout(QGridLayout* lay, const int r, const int c)
	{
		lay->addWidget(this, r, c);
	}
	void updateUIValue(QVariant _value)
	{
		valueLabel->setText(_value.toString());
		setValue(_value);
		setChanged(true);
		label->setStyleSheet("QLabel {  color : red; }");
		emit parameterChanged();
	}
private:
	QLabel *label;
	QPushButton *openFileDialogButton;
	QLabel *valueLabel;
	QFileDialog *selectFileDialog;
	
	//QSpinBox *spinBox;
	QHBoxLayout *hlayout;

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


class Setting3DP:public QDialog
{
	Q_OBJECT
public:	

	Setting3DP() {}
	Setting3DP(MainWindow *_mw, RichParameterSet *,QWidget *parent = 0);
	~Setting3DP();

	enum JsonfileCategory
	{
		Basic_Setting,
		Advanced_Setting,
		PP350_SETTING_ca,
		PP352_SETTING_ca,
		Common_Setting_ca


	};

	static void initSetting(RichParameterSet *);
	static void createRichParamfromJdoc(JsonfileCategory, RichParameterSet *in);
	static void createPrinterSetting_FromRichParameter(int , QString, RichParameterSet *in);
	//static bool 
	void initDefaultSetting();
	//setting loadfromregister();	
	void initWidgetParam();

	
	static QString paramFileLocation;
	

public slots:
	//save setting to register and pack as RichParameter
	void getaccept();
	void importSampleFile();
	void sendUsage();
	void getColorProfile();
	void getDMICMColorProfile();
	void resetSetting();
	/*---------NVM another accept--------------*/
	void updateUIToJsonFile(JsonfileCategory);
	void updateJsonFileToRichParameter(JsonfileCategory);
	
	
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
	void updateRichParameterFromJsonFile(QString);
	void createParamSettingUI(JsonfileCategory);
	// Create Printer Setting Page
	void create_PP350_Page();
	void create_PP352_Page();
	void create_Common_Page();
	
	void ui_set_default_from_current_value(JsonfileCategory);
	void ui_set_value_from_default(JsonfileCategory);
	bool exportSetting(JsonfileCategory);
	bool importSetting(JsonfileCategory);
	bool updateValueToUI(QString,int,QString,QVariant);
	QVariant getWidgetValue(JsonfileCategory,QString);
	SKTWidget * getWidget(JsonfileCategory, QString);
	

	QMap<QString, QVector<QVector<SKTWidget *> *> *> paramWidgetVector;
	QMap<QString, QMap<int , QString> *> paramGroupName;
	

	QStringList paramType;
	/*---------------------------------------------------------*/
	QPushButton *updateToFPGAButton;
	QPushButton *getFromFPGA;
	QPushButton *setDefaultValueButton;
	QPushButton *outputSettingToFile;
	QPushButton *inputSettingFromFile;

	void sendNVMPreProcess();
	void getNVMFromFPGA();
	//void updateTOFile();
	bool updateUIFromJsonFile(JsonfileCategory);
	//void update

	QByteArray encodeText, decodedText;

	QString key;// ("@xyzprinting.com");
	QByteArray hashKey;// = QCryptographicHash::hash(key.toLocal8Bit(), QCryptographicHash::Sha256);
	QByteArray iv;
	QAESEncryption *encryption;
	
	static QString decodeParamString;

protected:
	void keyPressEvent(QKeyEvent *e);
signals:
	void jsonFileChanged();



};


