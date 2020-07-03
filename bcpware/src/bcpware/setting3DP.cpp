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

#include "setting3DP.h"
#include "mainwindow.h"

#include <QInputDialog>
#include <QtGlobal>

#define TRANSFER_SWITCH 0
QString Setting3DP::paramFileLocation = getDocumentFolder() + "ParameterUI_STX.txt";
QString Setting3DP::decodeParamString;
Setting3DP::Setting3DP(MainWindow *_mw, RichParameterSet *currParm, QWidget *parent) :QDialog(parent), ui(new Ui::setting3DP_UI), loadtoWidgetParam(currParm)
, expertsMode(false)
{
	this->setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);
	ui->retranslateUi(this);
	ui->listWidget->setCurrentRow(0);
	ui->SliceSettingPage->setEnabled(false);
	//ui->NVMPage->setEnabled(false);
	initWidgetParam();
	initDefaultSetting();
	ui->label_3->setVisible(false);
	ui->offLB->setVisible(false);
	ui->sendUsageCB->setVisible(false);
	if (DSP_TEST_MODE)
	{
	}
	else
	{
		QModelIndex index = ui->printerCB->model()->index(0, 0);
		QModelIndex index2 = ui->printerCB->model()->index(1, 0);
		QVariant v(0);
		//ui->printerCB->model()->setData(index, v, Qt::UserRole - 1);
		ui->printerCB->model()->setData(index2, v, Qt::UserRole - 1);


		QListWidgetItem *item = ui->listWidget->item(3);
		item->setHidden(true);

		item = ui->listWidget->item(2);
		item->setHidden(true);

		item = ui->listWidget->item(4);
		item->setHidden(true);

		item = ui->listWidget->item(8);
		item->setHidden(true);

		item = ui->listWidget->item(9);
		item->setHidden(true);



		/*item = ui->listWidget->item(5);
		item->setHidden(true);*/

		ui->stackedWidget->widget(3)->hide();
		ui->stackedWidget->widget(3)->setHidden(true);

		ui->stackedWidget->widget(4)->hide();
		ui->stackedWidget->widget(4)->setHidden(true);

		/*ui->stackedWidget->widget(5)->hide();
		ui->stackedWidget->widget(5)->setHidden(true);*/

		ui->stackedWidget->widget(4)->hide();
		ui->stackedWidget->widget(4)->setHidden(true);


		//ui->stackedWidget->setCurrentWidget(ui->SliceSettingPage);
		ui->stackedWidget->setCurrentWidget(ui->general_page);
	}


	switchSetting = false;
	switchDM = false;

	mw = _mw;

	connect(ui->importSamplePushButton, SIGNAL(clicked()), this, SLOT(importSampleFile()));
	//connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(getaccept()));
	connect(ui->okButton, SIGNAL(clicked()), this, SLOT(getaccept()));
	connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	//connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(updateTOFile()));
	connect(ui->sendUsageCB, SIGNAL(clicked()), this, SLOT(sendUsage()));
	connect(ui->colorProfilePB, SIGNAL(clicked()), this, SLOT(getColorProfile()));
	connect(ui->resetBtn, SIGNAL(clicked()), this, SLOT(resetSetting()));
	//connect(ui->dilateBinderCB,SIGNAL(),this,);
	connect(ui->dilateBinderCB, &QCheckBox::clicked, [this]() {
		if (ui->dilateBinderCB->checkState())
			ui->dilateBinderLE_2->setEnabled(true);
		else
		{
			ui->dilateBinderLE_2->setEnabled(false);
		}
	});


	connect(ui->colorBind_CB, &QCheckBox::clicked, [this]() {
		if (ui->colorBind_CB->checkState())
			ui->colorBindLE->setEnabled(true);
		else
		{
			ui->colorBindLE->setEnabled(false);
		}
	});

	//connect(ui->spitton_modeCB, QOverload<int>(&QComboBox::currentIndexChanged), [=](int index)
	/*connect(ui->spitton_modeCB, static_cast<void (QComboBox:: *)(int)>(&QComboBox::currentIndexChanged), [=](int index)
	{
	switch (index)
	{
	case 0:
	{
	ui->spittonM_le->setReadOnly(true);
	ui->spittonY_le->setReadOnly(true);
	ui->spittonC_le->setReadOnly(true);
	ui->spittonB_le->setReadOnly(true);

	ui->spittonM_le->setText(QString::number(0.5));
	ui->spittonY_le->setText(QString::number(0.5));
	ui->spittonC_le->setText(QString::number(0.5));
	ui->spittonB_le->setText(QString::number(5));
	}
	break;
	case 1:
	{
	ui->spittonM_le->setReadOnly(true);
	ui->spittonY_le->setReadOnly(true);
	ui->spittonC_le->setReadOnly(true);
	ui->spittonB_le->setReadOnly(true);

	ui->spittonM_le->setText(QString::number(3));
	ui->spittonY_le->setText(QString::number(3));
	ui->spittonC_le->setText(QString::number(3));
	ui->spittonB_le->setText(QString::number(5));
	}
	break;
	case 2:
	{
	ui->spittonM_le->setReadOnly(false);
	ui->spittonY_le->setReadOnly(false);
	ui->spittonC_le->setReadOnly(false);
	ui->spittonB_le->setReadOnly(false);
	}
	break;
	}
	}
	);*/


	////////////////////////////////////////////////////////////////////////////////////////

	connect(ui->stiffPrintV2CB, &QCheckBox::clicked, [this]() {
		if (ui->stiffPrintV2CB->checkState())
		{
			ui->stiffPrintV2LE->setEnabled(true);
			ui->stiffPrintV2CB->setEnabled(true);
			ui->begin_twenty_Layer_Thick_SPB->setEnabled(true);
			ui->begin_twenty_Layer_Percent_SPB->setEnabled(true);
			ui->shellPercentLE->setEnabled(true);

			ui->spittonCMY_SecondLE->setEnabled(true);
			ui->spittonB_SecondLE->setEnabled(true);

		}
		else
		{
			ui->stiffPrintV2LE->setEnabled(false);
			ui->begin_twenty_Layer_Thick_SPB->setEnabled(false);
			ui->begin_twenty_Layer_Percent_SPB->setEnabled(false);
			ui->shellPercentLE->setEnabled(false);

			ui->spittonCMY_SecondLE->setEnabled(false);
			ui->spittonB_SecondLE->setEnabled(false);
		}
	});
	connect(ui->witnessbarCB, &QCheckBox::clicked, [this]() {
		if (ui->witnessbarCB->checkState())
		{
			ui->witnessbar_SPB->setEnabled(true);

		}
		else
		{
			ui->witnessbar_SPB->setEnabled(false);
		}
	});


	connect(ui->CMYB_PercentageCB, &QCheckBox::clicked, [this]() {
		if (ui->CMYB_PercentageCB->checkState())
		{
			ui->CMYB_PercentageLE->setEnabled(true);

		}
		else
		{
			ui->CMYB_PercentageLE->setEnabled(false);
		}
	});

	connect(ui->channelControl_CB, &QCheckBox::clicked, [this]() {
		if (ui->channelControl_CB->checkState())
		{
			ui->C_onOff_CB->setEnabled(true);
			ui->M_onOff_CB->setEnabled(true);
			ui->Y_onOff_CB->setEnabled(true);
			ui->B_onOff_CB->setEnabled(true);

		}
		else
		{
			ui->C_onOff_CB->setEnabled(false);
			ui->M_onOff_CB->setEnabled(false);
			ui->Y_onOff_CB->setEnabled(false);
			ui->B_onOff_CB->setEnabled(false);
		}
	});

	connect(ui->listWidget, static_cast<void (QListWidget::*)(int)>(&QListWidget::currentRowChanged), [=](int ix)
	{
		int listIndex = ui->listWidget->currentRow();
		switch (listIndex)
		{
		case 0:
		case 1:
		case 2:
			ui->stackedWidget->setCurrentIndex(listIndex);
			break;
		case 3:
		{

		}
		break;
		case 4:
		{
			bool ok;
			QString text = QInputDialog::getText(this, tr("Password"),
				tr("Password:"), QLineEdit::Normal,
				"", &ok);
			if (ok && text == ("dm"))
			{
				ui->stackedWidget->setCurrentWidget(ui->DMSettingPage);
				ui->DMSettingPage->setEnabled(true);
			}
			else
			{
				ui->DMSettingPage->setEnabled(false);
				ui->stackedWidget->setCurrentWidget(ui->DMSettingPage);
			}
		}
		break;
		}

	});

	/*connect(ui->line_or_rectangleCB, &QCheckBox::clicked, [this]() {
		if (ui->line_or_rectangleCB->checkState())
		{
		ui->horizonLineWidthDB->setEnabled(false);


		}
		else
		{
		ui->horizonLineWidthDB->setEnabled(true);

		}
		});*/


	/*connect(ui->listWidget, static_cast<void (QListWidget::*)(int)>(&QListWidget::currentRowChanged), [=](int ix)
	{
	if (ui->listWidget->currentRow() == 4)
	{
	bool ok;
	QString text = QInputDialog::getText(this, tr("Password"),
	tr("Password:"), QLineEdit::Normal,
	"", &ok);
	if (ok && text.compare("dm"))
	{
	ui->stackedWidget->setCurrentWidget(ui->page->setd);
	}
	else
	{

	}

	}
	});*/

	connect(ui->ColorProfileCMB, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int ix)
	{
		switch (ui->ColorProfileCMB->currentIndex())
		{
		case 0:
		{
			colorProfilePath = PicaApplication::getRoamingDir() + "/ColorProfile/CMY_54_64_93c05to0_m05to0_y1to0_c295to245_m36to29_y505to415_c6to4_m8-7.icm";
			QFileInfo tempInfo(colorProfilePath);
			if (tempInfo.exists())
			{
				ui->colorProileLB->setText(tempInfo.fileName());
				ui->colorProileLB->setFixedWidth(tempInfo.fileName().size() * 6);
			}
		}
		break;
		case 1:
		{
			colorProfilePath = PicaApplication::getRoamingDir() + "/ColorProfile/ECI2002CMYKEyeOneL_siriusCMY_73_76_91_large_paperGray_1ti_coveron_lightPlus20.icm";
			QFileInfo tempInfo(colorProfilePath);
			if (tempInfo.exists())
			{
				ui->colorProileLB->setText(tempInfo.fileName());
				ui->colorProileLB->setFixedWidth(tempInfo.fileName().size() * 6);
			}
		}
		break;

		}
	});

	connect(ui->dmICMPB, SIGNAL(clicked()), this, SLOT(getDMICMColorProfile()));




	if (ui->stiffPrintV2CB->checkState())
	{
		ui->stiffPrintV2LE->setEnabled(true);
		ui->begin_twenty_Layer_Thick_SPB->setEnabled(true);
		ui->begin_twenty_Layer_Percent_SPB->setEnabled(true);
		ui->shellPercentLE->setEnabled(true);

		ui->spittonCMY_SecondLE->setEnabled(true);
		ui->spittonB_SecondLE->setEnabled(true);
	}
	else
	{
		ui->stiffPrintV2LE->setEnabled(false);
		ui->begin_twenty_Layer_Thick_SPB->setEnabled(false);
		ui->begin_twenty_Layer_Percent_SPB->setEnabled(false);
		ui->shellPercentLE->setEnabled(false);

		ui->spittonCMY_SecondLE->setEnabled(false);
		ui->spittonB_SecondLE->setEnabled(false);
	}
	/*
	witness bar
	*/
	if (ui->witnessbarCB->checkState())
	{
		ui->witnessbar_SPB->setEnabled(true);
	}
	else
	{
		ui->witnessbar_SPB->setEnabled(false);

	}


	/*if (ui->CMYB_PercentageCB->checkState())
		ui->CMYB_PercentageLE->setEnabled(true);
		else*/
	ui->CMYB_PercentageCB->setEnabled(false);
	ui->CMYB_PercentageLE->setEnabled(false);


	if (ui->channelControl_CB->checkState())
	{
		ui->C_onOff_CB->setEnabled(true);
		ui->M_onOff_CB->setEnabled(true);
		ui->Y_onOff_CB->setEnabled(true);
		ui->B_onOff_CB->setEnabled(true);
	}
	else
	{
		ui->C_onOff_CB->setEnabled(false);
		ui->M_onOff_CB->setEnabled(false);
		ui->Y_onOff_CB->setEnabled(false);
		ui->B_onOff_CB->setEnabled(false);
	}


	if (ui->startPageCB->checkState())
	{
		ui->startPageLE->setEnabled(true);
		ui->endPageLE->setEnabled(true);

	}
	else
	{
		ui->startPageLE->setEnabled(false);
		ui->endPageLE->setEnabled(false);
	}

	/*if (ui->line_or_rectangleCB->checkState())
	{
	ui->horizonLineWidthDB->setEnabled(false);


	}
	else
	{
	ui->horizonLineWidthDB->setEnabled(true);

	}*/


	ui->sendUsageCB->setStyleSheet(WidgetStyleSheet::check_onoff_styleSheet());

	ks = new QKeySequence(Qt::CTRL + Qt::Key_X, Qt::CTRL + Qt::Key_C);

	this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
	/*---------------create New UI-----------------------------------------------------*/

	paramType << "Basic_Setting" << "Advanced_Setting" << "PP350_Settings" << "PP352_Settings" << "Common_Setting";
	foreach(QString ca, paramType)
	{
		paramWidgetVector.insert(ca, new QVector<QVector<SKTWidget*>*>());
		paramGroupName.insert(ca, new QMap<int, QString>);
	}

	/*QString paramJsonString;
	ParamOp::getJsonFiletoString(paramJsonString, BCPwareFileSystem::parameterFilePath());
	QString encryptString = BCPwareFileSystem::encryptParam(paramJsonString);
	ParamOp::saveStringToFileWithPath(encryptString, BCPwareFileSystem::documentDir().filePath("parameter_setting.zxb"));

	QString outputString;
	BCPwareFileSystem::decodeParam(outputString, QString(), BCPwareFileSystem::documentDir().filePath("parameter_setting.zxb"));*/

	connect(ui->open_encryptFile, &QPushButton::clicked, [=](){
		QString encryptfilePath = QFileDialog::getOpenFileName(this, tr("Get encrypt parameter file"), BCPwareFileSystem::documentDir().absolutePath(), "encrypt file (*.zxb;)");
		QString outputstring;
		BCPwareFileSystem::decodeParam(outputstring, QString(), encryptfilePath);

		ui->decodeText->setText(outputstring);

	});

	connect(ui->save_zxb, &QPushButton::clicked, [=](){
		QString encryptfilePath = QFileDialog::getSaveFileName(this, tr("save to encrypt zxb file"), BCPwareFileSystem::documentDir().absolutePath(), "encrypt file (*.zxb;)");
		QString outputstring;
		BCPwareFileSystem::encryptParam(ui->decodeText->toPlainText(), QFileInfo(encryptfilePath));



	});


	BCPwareFileSystem::decodeParam(decodeParamString, QString(), BCPwareFileSystem::parameterFilePath());
	connect(this, &Setting3DP::jsonFileChanged, [this]{
		BCPwareFileSystem::decodeParam(decodeParamString, QString(), BCPwareFileSystem::parameterFilePath());
	});

	createParamSettingUI(Advanced_Setting);
	createParamSettingUI(PP350_SETTING_ca);
	createParamSettingUI(PP352_SETTING_ca);
	createParamSettingUI(Common_Setting_ca);
	//TBD : create General Setting UI to replace old one
	//TBD : create Editor UI to replace old one


	if (getWidgetValue(Common_Setting_ca, "TARGET_PRINTER") == 0)
	{
		qDebug() << "pp350";
	}
	else
	{
		qDebug() << "pp352";
	}

	setCurrentToDefault = new QPushButton("ui_set_default_from_current_value");
	setDefaultToCurrent = new QPushButton("");
	exportSettingPB = new QPushButton("");
	importSettingPB = new QPushButton("");
	importSampleFileBtn = new QPushButton("Import sample 3D model");
	switchExpertSetting = new QCheckBox("Switch Button");
	pp350g1Layout = new QGridLayout;
	pp350glay = new QVBoxLayout();

	setDefaultToCurrent->setStyleSheet(WidgetStyleSheet::settingPrinterButtonStyleSheet(":/images/icons/btn_reset_normal.png", ":/images/icons/btn_reset_click.png", ":/images/icons/btn_reset_hover.png"));
	setDefaultToCurrent->setToolTip("Load default setting");
	exportSettingPB->setStyleSheet(WidgetStyleSheet::settingPrinterButtonStyleSheet(":/images/icons/btn_export_normal.png", ":/images/icons/btn_export_click.png", ":/images/icons/btn_export_hover.png"));
	exportSettingPB->setToolTip("Export printer setting");
	importSettingPB->setStyleSheet(WidgetStyleSheet::settingPrinterButtonStyleSheet(":/images/icons/btn_import_normal.png", ":/images/icons/btn_import_click.png", ":/images/icons/btn_import_hover.png"));
	importSettingPB->setToolTip("Load printer setting");


	//pp350g1Layout->addWidget(setCurrentToDefault, 0, 0);
	//pp350g1Layout->addWidget(setDefaultToCurrent, 0, 1);
	//pp350g1Layout->addWidget(setDefaultToCurrent, 0, 0, 1, 2 );
	pp350g1Layout->addWidget(setDefaultToCurrent, 0, 0);	
	pp350g1Layout->addWidget(exportSettingPB, 0, 1);
	pp350g1Layout->addWidget(importSettingPB, 0, 2);
	

	QWidget *spacerWidget = new QWidget(this);//***ªÅ¥Õspacer
	spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	spacerWidget->setVisible(true);
	pp350g1Layout->addWidget(spacerWidget, 0, 3);

	//pp350g1Layout->addWidget(switchExpertSetting, 2, 0, 2, 1);
	/*topLayout->addLayout(g1Layout);
	topLayout->addStretch();*/
	ui->pp350buttonFrame->setLayout(pp350g1Layout);



	//switchExpertSetting->setChecked(false);
	connect(setCurrentToDefault, &QPushButton::clicked, [=](){
		ui_set_default_from_current_value(JsonfileCategory::PP350_SETTING_ca);
	});
	connect(setDefaultToCurrent, &QPushButton::clicked, [=](){
		ui_set_value_from_default(JsonfileCategory::PP350_SETTING_ca);
	});
	connect(exportSettingPB, &QPushButton::clicked, [&](){
		exportSetting(JsonfileCategory::PP350_SETTING_ca);
	});
	connect(importSettingPB, &QPushButton::clicked, [=](){
		importSetting(JsonfileCategory::PP350_SETTING_ca);
	});

	connect(importSampleFileBtn, &QPushButton::clicked, this, &Setting3DP::importSampleFile);

	/*connect(switchExpertSetting, static_cast<void (QCheckBox::*)(int)>(&QCheckBox::stateChanged), [=](int ix)
	{
	createParamSettingUI(PP350_SETTING_ca);
	create_PP350_Page();
	});*/

	connect(this, &Setting3DP::switchsettingChanged, [&]()
	{

		createParamSettingUI(PP350_SETTING_ca);
		create_PP350_Page();
		connect(getWidget(PP350_SETTING_ca, "COLOR_MODE"), &SKTWidget::parameterChanged, [this]() {
			QVariant colormodeValue = getWidgetValue(PP350_SETTING_ca, "COLOR_MODE");
			switch (colormodeValue.toInt())
			{
			case 0:
				qDebug() << "pp350";
				getWidget(PP350_SETTING_ca, "COLOR_PROFILE")->updateUIValue(PicaApplication::getRoamingDir() + "/ColorProfile/CMY_54_64_93c05to0_m05to0_y1to0_c295to245_m36to29_y505to415_c6to4_m8-7.icm");
				break;
			case 1:
				getWidget(PP350_SETTING_ca, "COLOR_PROFILE")->updateUIValue(PicaApplication::getRoamingDir() + "/ColorProfile/ECI2002CMYKEyeOneL_siriusCMY_73_76_91_large_paperGray_1ti_coveron_lightPlus20.icm");
				//ui->stackedWidget->setCurrentWidget(ui->general_page);
				break;
			}

		});

		connect(getWidget(PP350_SETTING_ca, "CONTINUE_PRINTING"), &SKTWidget::parameterChanged, [this]() {
			QVariant customValue = getWidgetValue(PP350_SETTING_ca, "CONTINUE_PRINTING");
			if (!customValue.isNull())
			{
				getWidget(PP350_SETTING_ca, "CONTINUE_PRINTING_PAGE")->setEnabled(customValue.toBool());
				getWidget(PP350_SETTING_ca, "CONTINUE_PRINTING_END_PAGE")->setEnabled(customValue.toBool());
			}
		});
		//DILATE_BINDER : DILATE_BINDER_VALUE
		connect(getWidget(PP350_SETTING_ca, "DILATE_BINDER"), &SKTWidget::parameterChanged, [this]() {
			QVariant customValue = getWidgetValue(PP350_SETTING_ca, "DILATE_BINDER");
			if (!customValue.isNull())
			{
				getWidget(PP350_SETTING_ca, "DILATE_BINDER_VALUE")->setEnabled(customValue.toBool());				
			}
		});		
		//STIFF_PRIN_V2 : STIFF_PRINT_VALUE
		connect(getWidget(PP350_SETTING_ca, "STIFF_PRIN_V2"), &SKTWidget::parameterChanged, [this]() {
			QVariant customValue = getWidgetValue(PP350_SETTING_ca, "STIFF_PRIN_V2");
			if (!customValue.isNull())
			{
				getWidget(PP350_SETTING_ca, "STIFF_PRINT_VALUE")->setEnabled(customValue.toBool());
				getWidget(PP350_SETTING_ca, "SHELL_PERCENT")->setEnabled(customValue.toBool());
				
			}
		});
		//WITNESS_BAR : WITNESS_BAR_PERCENT,WITNESS_WIDTH
		connect(getWidget(PP350_SETTING_ca, "WITNESS_BAR"), &SKTWidget::parameterChanged, [this]() {
			QVariant customValue = getWidgetValue(PP350_SETTING_ca, "WITNESS_BAR");
			if (!customValue.isNull())
			{
				getWidget(PP350_SETTING_ca, "WITNESS_BAR_PERCENT")->setEnabled(customValue.toBool());
				getWidget(PP350_SETTING_ca, "WITNESS_WIDTH")->setEnabled(customValue.toBool());
			}
		});

		//PP_POST_HEATING_SWITCH : PP_POST_HEATING_UPPER_LIMIT, PP_POST_HEATING_LOWER_LIMIT, PP_POST_HEATING_MINUTES
		connect(getWidget(PP350_SETTING_ca, "PP_POST_HEATING_SWITCH"), &SKTWidget::parameterChanged, [this]() {
			QVariant customValue = getWidgetValue(PP350_SETTING_ca, "PP_POST_HEATING_SWITCH");
			if (!customValue.isNull())
			{
				getWidget(PP350_SETTING_ca, "PP_POST_HEATING_UPPER_LIMIT")->setEnabled(customValue.toBool());
				getWidget(PP350_SETTING_ca, "PP_POST_HEATING_LOWER_LIMIT")->setEnabled(customValue.toBool());
				getWidget(PP350_SETTING_ca, "PP_POST_HEATING_MINUTES")->setEnabled(customValue.toBool());
			}
		});
		//PROFILE_ON : COLOR_MODE,COLOR_PROFILE
		connect(getWidget(PP350_SETTING_ca, "PROFILE_ON"), &SKTWidget::parameterChanged, [this]() {
			QVariant customValue = getWidgetValue(PP350_SETTING_ca, "PROFILE_ON");
			if (!customValue.isNull())
			{
				getWidget(PP350_SETTING_ca, "COLOR_MODE")->setEnabled(customValue.toBool());
				getWidget(PP350_SETTING_ca, "COLOR_PROFILE")->setEnabled(customValue.toBool());
				
			}
		});

		//widget initial state========================		
		emit getWidget(PP350_SETTING_ca, "CONTINUE_PRINTING")->parameterChanged();
		emit getWidget(PP350_SETTING_ca, "DILATE_BINDER")->parameterChanged();
		emit getWidget(PP350_SETTING_ca, "STIFF_PRIN_V2")->parameterChanged();
		emit getWidget(PP350_SETTING_ca, "WITNESS_BAR")->parameterChanged();
		emit getWidget(PP350_SETTING_ca, "PP_POST_HEATING_SWITCH")->parameterChanged();
		emit getWidget(PP350_SETTING_ca, "PROFILE_ON")->parameterChanged();
		/*if (!customValue.isNull())
		{
			getWidget(PP350_SETTING_ca, "CONTINUE_PRINTING_PAGE")->setEnabled(customValue.toBool());
			getWidget(PP350_SETTING_ca, "CONTINUE_PRINTING_END_PAGE")->setEnabled(customValue.toBool());

			qDebug() << "abcdefg : " << getWidget(PP350_SETTING_ca, "CONTINUE_PRINTING_PAGE")->isEnabled();
		}
		customValue = getWidgetValue(PP350_SETTING_ca, "DILATE_BINDER");
		if (!customValue.isNull())
		{
			getWidget(PP350_SETTING_ca, "DILATE_BINDER_VALUE")->setEnabled(customValue.toBool());
		}
		customValue = getWidgetValue(PP350_SETTING_ca, "STIFF_PRIN_V2");
		if (!customValue.isNull())
		{
			getWidget(PP350_SETTING_ca, "STIFF_PRINT_VALUE")->setEnabled(customValue.toBool());
			getWidget(PP350_SETTING_ca, "SHELL_PERCENT")->setEnabled(customValue.toBool());
		}
		customValue = getWidgetValue(PP350_SETTING_ca, "WITNESS_BAR");
		if (!customValue.isNull())
		{
			getWidget(PP350_SETTING_ca, "WITNESS_BAR_PERCENT")->setEnabled(customValue.toBool());
			getWidget(PP350_SETTING_ca, "WITNESS_WIDTH")->setEnabled(customValue.toBool());
		}
		customValue = getWidgetValue(PP350_SETTING_ca, "PP_POST_HEATING_SWITCH");
		if (!customValue.isNull())
		{
			getWidget(PP350_SETTING_ca, "PP_POST_HEATING_UPPER_LIMIT")->setEnabled(customValue.toBool());
			getWidget(PP350_SETTING_ca, "PP_POST_HEATING_LOWER_LIMIT")->setEnabled(customValue.toBool());
			getWidget(PP350_SETTING_ca, "PP_POST_HEATING_MINUTES")->setEnabled(customValue.toBool());
		}
		customValue = getWidgetValue(PP350_SETTING_ca, "PROFILE_ON");
		if (!customValue.isNull())
		{
			getWidget(PP350_SETTING_ca, "COLOR_MODE")->setEnabled(customValue.toBool());
			getWidget(PP350_SETTING_ca, "COLOR_PROFILE")->setEnabled(customValue.toBool());

		}*/


	});

	

	//==============================================


	createNVMPage();
	create_PP350_Page();
	create_PP352_Page();
	//create_Common_Page();
	create_GeneralAndEditPage();

	connect(getWidget(Common_Setting_ca, "TARGET_PRINTER"), &SKTWidget::parameterChanged, [this]() {
		QVariant widgetValue = getWidgetValue(Common_Setting_ca, "TARGET_PRINTER");
		if (widgetValue == 0)
		{
			qDebug() << "pp350";
			ui->stackedWidget->widget(7)->setHidden(true);
			ui->stackedWidget->widget(5)->setHidden(true);

			QListWidgetItem *item = ui->listWidget->item(3);
			item->setHidden(true);

			item = ui->listWidget->item(6);
			item->setHidden(false);

			item = ui->listWidget->item(7);
			item->setHidden(true);

			item = ui->listWidget->item(5);
			item->setHidden(true);

			//ui->stackedWidget->setCurrentWidget(ui->general_page);
		}
		else if (widgetValue == 1)
		{
			ui->stackedWidget->widget(6)->setHidden(true);
			//ui->stackedWidget->widget(5)->setHidden(false);
			qDebug() << "pp352";

			QListWidgetItem *item = ui->listWidget->item(3);
			item->setHidden(true);

			item = ui->listWidget->item(6);
			item->setHidden(true);

			item = ui->listWidget->item(7);
			item->setHidden(false);

			item = ui->listWidget->item(5);
			item->setHidden(false);

			//ui->stackedWidget->setCurrentWidget(ui->general_page);
		}
	});

	connect(getWidget(PP350_SETTING_ca, "COLOR_MODE"), &SKTWidget::parameterChanged, [this]() {
		QVariant colormodeValue = getWidgetValue(PP350_SETTING_ca, "COLOR_MODE");
		switch (colormodeValue.toInt())
		{
		case 0:
			qDebug() << "pp350";
			getWidget(PP350_SETTING_ca, "COLOR_PROFILE")->updateUIValue(PicaApplication::getRoamingDir() + "/ColorProfile/CMY_54_64_93c05to0_m05to0_y1to0_c295to245_m36to29_y505to415_c6to4_m8-7.icm");
			break;
		case 1:
			getWidget(PP350_SETTING_ca, "COLOR_PROFILE")->updateUIValue(PicaApplication::getRoamingDir() + "/ColorProfile/ECI2002CMYKEyeOneL_siriusCMY_73_76_91_large_paperGray_1ti_coveron_lightPlus20.icm");
			//ui->stackedWidget->setCurrentWidget(ui->general_page);
			break;
		}

	});


	if (getWidgetValue(Common_Setting_ca, "TARGET_PRINTER") == 0)
	{
		qDebug() << "pp350";
		ui->stackedWidget->widget(7)->setHidden(true);
		ui->stackedWidget->widget(5)->setHidden(true);

		QListWidgetItem *item = ui->listWidget->item(3);
		item->setHidden(true);

		item = ui->listWidget->item(7);
		item->setHidden(true);

		item = ui->listWidget->item(5);
		item->setHidden(true);


	}
	else
	{
		ui->stackedWidget->widget(6)->setHidden(true);
		//ui->stackedWidget->widget(5)->setHidden(false);
		qDebug() << "pp352";

		QListWidgetItem *item = ui->listWidget->item(3);
		item->setHidden(true);

		item = ui->listWidget->item(6);
		item->setHidden(true);


		item = ui->listWidget->item(5);
		item->setHidden(false);


	}
	ui->stackedWidget->setCurrentWidget(ui->general_page);

	
	connect(updateToFPGAButton, &QPushButton::clicked, [=](){

		sendNVMPreProcess();
	});

	connect(getFromFPGA, &QPushButton::clicked, [=](){

		getNVMFromFPGA();
	});



	// Initial encrypt
	key = "@xyzprinting.com";
	hashKey = QCryptographicHash::hash(key.toLocal8Bit(), QCryptographicHash::Sha256);
	iv.resize(16);
	for (int i = 0; i < 16; i++)
		iv[i] = 0x00;
	encryption = new QAESEncryption(QAESEncryption::AES_128, QAESEncryption::CBC, QAESEncryption::PKCS7);

	connect(outputSettingToFile, &QPushButton::clicked, [&](){

		/*QString key("@xyzprinting.com");
		QByteArray hashKey = QCryptographicHash::hash(key.toLocal8Bit(), QCryptographicHash::Sha256);
		QByteArray iv;
		iv.resize(16);
		for (int i = 0; i < 16; i++)
		iv[i] = 0x00;*/

		//QAESEncryption encryption(QAESEncryption::AES_128, QAESEncryption::CBC, QAESEncryption::PKCS7);		
		//output to a file, output value format

		QVariant identifyNameValueMap = QVariantMap();
		QVariant categoryMap = QVariantMap();
		for (int i = 0; i < paramWidgetVector.value(paramType[2])->size(); i++)
		{
			for (int j = 0; j < paramWidgetVector.value(paramType[2])->at(i)->size(); j++)
			{
				SKTWidget *sktwidget = paramWidgetVector.value(paramType[2])->at(i)->at(j);

				ParamOp::mergeValue(identifyNameValueMap, "", sktwidget->getIdentifyName().toString());
				ParamOp::mergeValue(categoryMap, identifyNameValueMap, sktwidget->getCategoryName().toString());
			}
		}
		QJsonDocument jsonDoc = QJsonDocument::fromVariant(categoryMap);
		QString updateJson(jsonDoc.toJson(QJsonDocument::Compact));



		encodeText = encryption->encode(updateJson.toUtf8(), hashKey, iv);
		QByteArray ba_as_hex_string = encodeText.toHex();

		decodedText = encryption->removePadding(encryption->decode(encodeText, hashKey, iv));
		ParamOp::saveStringToFileWithPath(updateJson.toUtf8(), getDocumentDir() + "testEncode.txt");




	});

	connect(inputSettingFromFile, &QPushButton::clicked, [=](){
		QString fileString;

		ParamOp::getJsonFiletoString(fileString, getDocumentDir() + "testEncode.txt");
		QByteArray testbya = QByteArray::fromHex(fileString.toLatin1());
		/*decode*/

		//QString key("@xyzprinting.com");
		//QByteArray hashKey = QCryptographicHash::hash(key.toLocal8Bit(), QCryptographicHash::Sha256);

		//QAESEncryption encryption(QAESEncryption::AES_128, QAESEncryption::CBC, QAESEncryption::PKCS7);		
		/*QByteArray iv;
		iv.resize(16);
		for (int i = 0; i < 16; i++)
		iv[i] = 0x00;*/

		decodedText = encryption->removePadding(encryption->decode(testbya, hashKey, iv));

		QVariant categoryMap;
		ParamOp::extractVariantTest(categoryMap, QVariant(), QString(), -1, QString(), QString(decodedText));

	});

	//ParamOp::transformJsonToRichParameter(tempSet, QString());

	


}

void Setting3DP::keyPressEvent(QKeyEvent *e)
{
	if (e->matches(QKeySequence::Underline))
	{
		switchSetting ^= 1;
		emit switchsettingChanged();
		if (switchSetting)
		{
			/*QListWidgetItem *item = ui->listWidget->item(3);
			item->setHidden(false);

			item = ui->listWidget->item(5);
			item->setHidden(false);



			//ui->SliceSettingPage->setDisabled(false);
			//ui->page->setDisabled(false);
			//ui->stackedWidget->addWidget(ui->SliceSettingPage);
			ui->SliceSettingPage->setEnabled(true);
			ui->SliceSettingPage->setHidden(false);
			ui->listWidget->setCurrentRow(3);

			ui->NVMPage->setEnabled(true);
			ui->NVMPage->setHidden(false);
			ui->listWidget->setCurrentRow(3);*/
		}
		else
		{
			/*QListWidgetItem *item = ui->listWidget->item(3);
			item->setHidden(true);

			item = ui->listWidget->item(5);
			item->setHidden(true);



			//ui->SliceSettingPage->setDisabled(true);
			//ui->page->setDisabled(true);
			//ui->stackedWidget->removeWidget(ui->SliceSettingPage);

			ui->SliceSettingPage->setEnabled(false);
			ui->SliceSettingPage->setHidden(true);
			ui->listWidget->setCurrentRow(0);

			ui->NVMPage->setEnabled(false);
			ui->NVMPage->setHidden(true);
			ui->listWidget->setCurrentRow(0);*/
		}
	}

	else if (e->matches(QKeySequence::Bold))
	{
		/* deprecated
		switchDM ^= 1;
		if (switchDM)
		{
		QListWidgetItem *item = ui->listWidget->item(4);
		item = ui->listWidget->item(4);
		item->setHidden(false);
		ui->listWidget->setCurrentRow(4);
		}
		else
		{
		QListWidgetItem *item = ui->listWidget->item(4);
		item = ui->listWidget->item(4);
		item->setHidden(true);
		ui->listWidget->setCurrentRow(0);
		}
		*/
		switchDM ^= 1;
		if (switchDM)
		{
			QListWidgetItem *item = ui->listWidget->item(9);
			item = ui->listWidget->item(9);
			item->setHidden(false);
			ui->listWidget->setCurrentRow(9);

			/*item = ui->listWidget->item(4);
			item->setHidden(false);
			ui->listWidget->setCurrentRow(4);*/

			//setCurrentToDefault->setHidden(false);
		}
		else
		{
			QListWidgetItem *item = ui->listWidget->item(9);
			item = ui->listWidget->item(9);
			item->setHidden(true);
			ui->listWidget->setCurrentRow(0);
			//setCurrentToDefault->setHidden(true);
		}


	}



}

void Setting3DP::initWidgetParam()
{
	ui->languageCB->setCurrentIndex(loadtoWidgetParam->getEnum("Palette_Language"));
	//ui->undoLimitSB->setValue(loadtoWidgetParam->getInt("UNDO_LIMIT"));
	ui->displayUnitCB->setCurrentIndex(loadtoWidgetParam->getEnum("DISPLAY_UNIT"));
	ui->printerCB->setCurrentIndex(loadtoWidgetParam->getEnum("TARGET_PRINTER"));

	ui->sliceHeightLE->setValue(loadtoWidgetParam->getFloat("SLIGHT_HEIGHT"));
	//ui->sliceHeightLE->setValidator(new QDoubleValidator(0.1, 0.5, 2, this));


	ui->autoPackingGapLE->setText(QString::number(loadtoWidgetParam->getFloat("AUTO_PACKING_GAP"), 'g', 4));

	ui->grooveGapL->setText(QString::number(loadtoWidgetParam->getFloat("GROOVE_GAP_L"), 'g', 4));
	ui->grooveGapR->setText(QString::number(loadtoWidgetParam->getFloat("GROOVE_GAP_R"), 'g', 4));
	ui->grooveGapT->setText(QString::number(loadtoWidgetParam->getFloat("GROOVE_GAP_T"), 'g', 4));
	ui->grooveGapB->setText(QString::number(loadtoWidgetParam->getFloat("GROOVE_GAP_B"), 'g', 4));
	/*Spittoon*/
	ui->sacrificeStripLE->setText(QString::number(loadtoWidgetParam->getFloat("SACRIFICE_WIDTH"), 'g', 4));

	//ui->spitton_modeCB->setCurrentIndex(loadtoWidgetParam->getEnum("SPITTON_MODE"));

	ui->spittonM_le->setValue(loadtoWidgetParam->getFloat("SPITTOON_M"));
	ui->spittonC_le->setValue(loadtoWidgetParam->getFloat("SPITTOON_C"));
	ui->spittonY_le->setValue(loadtoWidgetParam->getFloat("SPITTOON_Y"));
	ui->spittonB_le->setValue(loadtoWidgetParam->getFloat("SPITTOON_B"));

	ui->spittonCMY_SecondLE->setValue(loadtoWidgetParam->getFloat("SPITTOON_SECOND_CMY"));
	ui->spittonB_SecondLE->setValue(loadtoWidgetParam->getFloat("SPITTOON_SECOND_B"));
	/*====================================================*/

	ui->startPrintCB->setChecked(loadtoWidgetParam->getBool("SYNCHRONIZE_PRINTING"));
	ui->pageSizeCB->setCurrentIndex(loadtoWidgetParam->getEnum("PAGE_SIZE"));
	ui->addpatternCB->setChecked(loadtoWidgetParam->getBool("ADD_PATTERN"));
	ui->plusPrintLengthLE->setValue(loadtoWidgetParam->getFloat("PLUS_PRINT_LENGTH"));
	ui->startPrintPageLE->setText(QString::number(loadtoWidgetParam->getInt("START_PRINT_PAGE")));

	ui->totalFacetCB->setChecked(loadtoWidgetParam->getBool("FAST_RENDERING"));
	ui->totalFacetsLE->setText(QString::number(loadtoWidgetParam->getInt("TOTAL_FACET")));

	ui->fanLE->setValue(loadtoWidgetParam->getInt("FAN_SPEED"));
	ui->pumpLE->setValue(loadtoWidgetParam->getInt("PUMP_VALUE"));
	//ui->wipe_frequencyLE->setText(QString::number(loadtoWidgetParam->getInt("WIPE_FREQUENCY")));
	/*
		mid Job Frequency and wiper param
		*/
	ui->midJobFrequencyLE->setValue(loadtoWidgetParam->getInt("MIDJOB_FREQUENCY"));
	ui->wiperIndexLE->setValue(loadtoWidgetParam->getInt("WIPER_INDEX"));
	ui->wiperClickLE->setValue(loadtoWidgetParam->getInt("WIPER_CLICK"));


	ui->stiffPrintCB->setChecked(loadtoWidgetParam->getBool("STIFF_PRINT"));
	ui->stiffPrintV2CB->setChecked(loadtoWidgetParam->getBool("STIFF_PRIN_V2"));
	ui->stiffPrintV2LE->setValue(loadtoWidgetParam->getInt("STIFF_PRINT_VALUE"));

	ui->witnessbarCB->setChecked(loadtoWidgetParam->getBool("WITNESS_BAR"));
	ui->witnessbar_SPB->setValue(loadtoWidgetParam->getInt("WITNESS_BAR_PERCENT"));

	ui->begin_twenty_Layer_Thick_SPB->setValue(loadtoWidgetParam->getFloat("PP_BOTTOM_LIGHTER_THICK"));
	ui->begin_twenty_Layer_Percent_SPB->setValue(loadtoWidgetParam->getInt("PP_BOTTOM_LIGHTER_PERCENTAGE"));

	ui->shellPercentLE->setValue(loadtoWidgetParam->getInt("SHELL_PERCENT"));

	ui->CMYB_PercentageCB->setChecked(loadtoWidgetParam->getBool("CMYB_BLACK"));
	ui->CMYB_PercentageLE->setText(QString::number(loadtoWidgetParam->getInt("CMYB_BLACK_VALUE")));

	ui->channelControl_CB->setChecked(loadtoWidgetParam->getBool("CHANNEL_CONTROL"));
	ui->C_onOff_CB->setChecked(loadtoWidgetParam->getBool("C_CHANNEL"));
	ui->M_onOff_CB->setChecked(loadtoWidgetParam->getBool("M_CHANNEL"));
	ui->Y_onOff_CB->setChecked(loadtoWidgetParam->getBool("Y_CHANNEL"));
	ui->B_onOff_CB->setChecked(loadtoWidgetParam->getBool("B_CHANNEL"));

	ui->postHeatingCB->setChecked(loadtoWidgetParam->getBool("PP_POST_HEATING_SWITCH"));
	ui->postHeatingTimeSB->setValue(loadtoWidgetParam->getInt("PP_POST_HEATING_MINUTES"));
	ui->postHeatingUpperLimitSP->setValue(loadtoWidgetParam->getInt("PP_POST_HEATING_UPPER_LIMIT"));
	ui->postHeatingLowerLimitSP->setValue(loadtoWidgetParam->getInt("PP_POST_HEATING_LOWER_LIMIT"));

	ui->profileOnCHB->setChecked(loadtoWidgetParam->getBool("PROFILE_ON"));
	ui->iR_On_OFF_CB->setChecked(loadtoWidgetParam->getBool("IR_On_Off"));

	//ui->CMYB_PercentageCB->setChecked(loadtoWidgetParam->getBool("CMYB_BLACK"));

	ui->dilateBinderCB->setChecked(loadtoWidgetParam->getBool("DILATE_BINDER"));
	if (ui->dilateBinderCB->isChecked())
		ui->dilateBinderLE_2->setText(QString::number(loadtoWidgetParam->getInt("DILATE_BINDER_VALUE")));
	else{
		ui->dilateBinderLE_2->setText(QString::number(loadtoWidgetParam->getInt("DILATE_BINDER_VALUE")));
		ui->dilateBinderLE_2->setEnabled(false);
	}
	ui->dynamicWipeCB->setChecked(loadtoWidgetParam->getBool("DYNAMIC_WIPE"));

	colorProfilePath = loadtoWidgetParam->getString("COLOR_PROFILE");
	QFileInfo tempInfo(colorProfilePath);
	if (tempInfo.exists())
	{
		ui->colorProileLB->setText(tempInfo.fileName());
		ui->colorProileLB->setFixedWidth(tempInfo.fileName().size() * 6);
	}

	ui->colorBind_CB->setChecked(loadtoWidgetParam->getBool("COLOR_BINDING"));
	if (ui->colorBind_CB->isChecked())
		ui->colorBindLE->setText(QString::number(loadtoWidgetParam->getFloat("COLOR_BINDING_VALUE"), 'g', 2));
	else
	{
		ui->colorBindLE->setText(QString::number(loadtoWidgetParam->getFloat("COLOR_BINDING_VALUE"), 'g', 2));
		ui->colorBindLE->setEnabled(false);
	}

	ui->witnessWidthLE->setText(QString::number(loadtoWidgetParam->getFloat("WITNESS_WIDTH"), 'g', 4));
	//ui->wiperLabel->setText(QString::number(getWiperUsage()));
	ui->startPageCB->setChecked(loadtoWidgetParam->getBool("CONTINUE_PRINTING"));
	if (ui->startPageCB->isChecked())
	{
		ui->startPageLE->setText(QString::number(loadtoWidgetParam->getInt("CONTINUE_PRINTING_PAGE")));
		ui->endPageLE->setText(QString::number(loadtoWidgetParam->getInt("CONTINUE_PRINTING_END_PAGE")));

	}
	else
	{
		ui->startPageLE->setText(QString::number(loadtoWidgetParam->getInt("CONTINUE_PRINTING_PAGE")));
		ui->endPageLE->setText(QString::number(loadtoWidgetParam->getInt("CONTINUE_PRINTING_END_PAGE")));
		ui->startPageLE->setEnabled(false);
		ui->endPageLE->setEnabled(false);
	}

	//ui->line_or_rectangleCB->setChecked(loadtoWidgetParam->getBool("LINE_OR_RECT"));
	ui->horizonLineWidthDB->setValue(loadtoWidgetParam->getFloat("HORIZONE_LINE_WIDTH"));
	ui->veticalShellThickDB->setValue(loadtoWidgetParam->getFloat("VERTICAL_LINE_WIDTH"));

	ui->ColorProfileCMB->setCurrentIndex(loadtoWidgetParam->getEnum("COLOR_MODE"));



	/*
	DM Setting,
	Horizontal thick mm,
	Horizontal channels,
	Horizontal cell size,
	Horizontal pattern percent

	Vertical mm,
	Vertical channels,
	Vertical cell size,
	Vertical pattern percent

	Bottom mm,
	Bottom fill mode,
	Bottom cell size,
	Bottom pattern percent

	Inner Cell Size,
	Inner Percent,
	Inner channels
	*/
	ui->horizontalThickDSB->setValue(loadtoWidgetParam->getFloat("DM_SHELL_HORIZONTAL_THICKNESS"));
	ui->C_HT_Che->setChecked(loadtoWidgetParam->getBool("DM_SHELL_HORIZONTAL_THICKNESS_C"));
	ui->M_HT_Che->setChecked(loadtoWidgetParam->getBool("DM_SHELL_HORIZONTAL_THICKNESS_M"));
	ui->Y_HT_Che->setChecked(loadtoWidgetParam->getBool("DM_SHELL_HORIZONTAL_THICKNESS_Y"));
	ui->B_HT_Che->setChecked(loadtoWidgetParam->getBool("DM_SHELL_HORIZONTAL_THICKNESS_B"));
	ui->shell_C_SB->setValue(loadtoWidgetParam->getInt("DM_HORIZONTAL_C_Value"));
	ui->shell_M_SB->setValue(loadtoWidgetParam->getInt("DM_HORIZONTAL_M_Value"));
	ui->shell_Y_SB->setValue(loadtoWidgetParam->getInt("DM_HORIZONTAL_Y_Value"));
	ui->shell_K_SB->setValue(loadtoWidgetParam->getInt("DM_HORIZONTAL_K_Value"));
	ui->horizonCellWidthSPB->setValue(loadtoWidgetParam->getInt("DM_HORIZONTAL_CELL_SIZE"));
	ui->horizonPatternPercentSPB->setValue(loadtoWidgetParam->getInt("DM_HORIZONTAL_PERCENTAGE"));
	ui->horizonePixelValueDS->setValue(loadtoWidgetParam->getInt("DM_HORIZONTAL_PIXEL_VALUE"));


	ui->vertcalThickDSB->setValue(loadtoWidgetParam->getFloat("DM_SHELL_VERTICAL_THICKNESS"));
	ui->C_VT_Che->setChecked(loadtoWidgetParam->getBool("DM_SHELL_VERTICAL_THICKNESS_C"));
	ui->M_VT_Che->setChecked(loadtoWidgetParam->getBool("DM_SHELL_VERTICAL_THICKNESS_M"));
	ui->Y_VT_Che->setChecked(loadtoWidgetParam->getBool("DM_SHELL_VERTICAL_THICKNESS_Y"));
	ui->B_VT_Che->setChecked(loadtoWidgetParam->getBool("DM_SHELL_VERTICAL_THICKNESS_B"));
	ui->top_C_SB->setValue(loadtoWidgetParam->getInt("DM_SHELL_TOP_C_Value"));
	ui->top_M_SB->setValue(loadtoWidgetParam->getInt("DM_SHELL_TOP_M_Value"));
	ui->top_Y_SB->setValue(loadtoWidgetParam->getInt("DM_SHELL_TOP_Y_Value"));
	ui->top_K_SB->setValue(loadtoWidgetParam->getInt("DM_SHELL_TOP_K_Value"));
	ui->verticalCellWidthSPB->setValue(loadtoWidgetParam->getInt("DM_VERTICAL_CELL_SIZE"));
	ui->verticalPatternPercentSPB->setValue(loadtoWidgetParam->getInt("DM_VERTICAL_PERCENTAGE"));
	ui->verticalPixelValueDS->setValue(loadtoWidgetParam->getInt("DM_VERTICAL_PIXEL_VALUE"));

	ui->bottomThickDSB->setValue(loadtoWidgetParam->getFloat("DM_SHELL_BOTTOM_LIGHTER"));
	ui->C_BM_Che->setChecked(loadtoWidgetParam->getBool("DM_SHELL_BOTTOM_LIGHTER_C"));
	ui->M_BM_Che->setChecked(loadtoWidgetParam->getBool("DM_SHELL_BOTTOM_LIGHTER_M"));
	ui->Y_BM_Che->setChecked(loadtoWidgetParam->getBool("DM_SHELL_BOTTOM_LIGHTER_Y"));
	ui->B_BM_Che->setChecked(loadtoWidgetParam->getBool("DM_SHELL_BOTTOM_LIGHTER_B"));
	ui->bottom_C_SB->setValue(loadtoWidgetParam->getInt("DM_SHELL_BOTTOM_C_Value"));
	ui->bottom_M_SB->setValue(loadtoWidgetParam->getInt("DM_SHELL_BOTTOM_M_Value"));
	ui->bottom_Y_SB->setValue(loadtoWidgetParam->getInt("DM_SHELL_BOTTOM_Y_Value"));
	ui->bottom_K_SB->setValue(loadtoWidgetParam->getInt("DM_SHELL_BOTTOM_K_Value"));
	ui->bottomCellWidthSPB->setValue(loadtoWidgetParam->getInt("DM_BOTTOM_CELL_SIZE"));
	ui->bottomPatternPercentSPB->setValue(loadtoWidgetParam->getInt("DM_BOTTOM_PERCENTAGE"));
	ui->bottomPixelValueDS->setValue(loadtoWidgetParam->getInt("DM_BOTTOM_PIXEL_VALUE"));

	ui->C_IN_Che->setChecked(loadtoWidgetParam->getBool("DM_INNER_C"));
	ui->M_IN_Che->setChecked(loadtoWidgetParam->getBool("DM_INNER_M"));
	ui->Y_IN_Che->setChecked(loadtoWidgetParam->getBool("DM_INNER_Y"));
	ui->B_IN_Che->setChecked(loadtoWidgetParam->getBool("DM_INNER_B"));
	ui->inside_C_SB->setValue(loadtoWidgetParam->getInt("DM_INNER_C_Value"));
	ui->inside_M_SB->setValue(loadtoWidgetParam->getInt("DM_INNER_M_Value"));
	ui->inside_Y_SB->setValue(loadtoWidgetParam->getInt("DM_INNER_Y_Value"));
	ui->inside_K_SB->setValue(loadtoWidgetParam->getInt("DM_INNER_K_Value"));
	ui->innerCellWidthSPB->setValue(loadtoWidgetParam->getInt("DM_INNER_CELL_SIZE"));
	ui->innerPatternPercentSPB->setValue(loadtoWidgetParam->getInt("DM_INNER_PERCENTAGE"));
	ui->innerPixelValueDS->setValue(loadtoWidgetParam->getInt("DM_INNER_PIXEL_VALUE"));

	ui->twiceInOneLayerCHB->setChecked(loadtoWidgetParam->getBool("DM_TWICE_IN_ONE_LAYER"));
	colorProfilePathForDitherMatrix = loadtoWidgetParam->getString("DM_ICM_FOR_DITHER");
	QFileInfo dmICMInfo(colorProfilePathForDitherMatrix);
	if (dmICMInfo.exists())
	{
		ui->iccNameLB->setText(dmICMInfo.fileName());
		ui->iccNameLB->setFixedWidth(dmICMInfo.fileName().size() * 6);
	}
	ui->pixelNumberSB->setValue(loadtoWidgetParam->getInt("DM_DITHER_PIXEL_NUM"));

	ui->genResultImg->setChecked(loadtoWidgetParam->getBool("DM_RESULT_IMG"));


	ui->generate_mode_settingCB->setCurrentIndex(loadtoWidgetParam->getEnum("DM_SLICING_MODE"));

	ui->spitSwitchCHB->setCurrentIndex(loadtoWidgetParam->getEnum("DM_SPIT_SWITCH"));


	//TODO change to spinBox
	//delete useless Params


}
int Setting3DP::getWiperUsage()
{
	int i_count = 0;
	QFileInfo outfile(QStandardPaths::locate(QStandardPaths::DocumentsLocation, "", QStandardPaths::LocateDirectory) + "BCP230Dashboard" + "/wiper.log");
	//QString dalogDirPath(QStandardPaths::locate(QStandardPaths::DocumentsLocation, "", QStandardPaths::LocateDirectory) );
	//QDir dir(dalogDirPath);//backup
	//if (dir.exists("BCP230Dashboard"))
	//{
	//	dir.setPath(dalogDirPath + "/BCP230Dashboard");
	//	dir.removeRecursively();
	//	dir.setPath(temppath);
	//	dir.mkpath("xyzimage");
	//}
	//else
	//{
	//	dir.setPath(temppath);
	//	dir.mkpath("xyzimage");
	//}


	FILE *pf;
	//read old value
	pf = fopen(outfile.absoluteFilePath().toStdString().data(), "r");
	if (!pf)
	{      //if file is no exit
		i_count = 0;
	}
	else
	{
		//get count
		fscanf(pf, "WipeCount %d", &i_count);
		i_count++;
		fclose(pf);//notice this
		//error case: TBD
	}

	return i_count;
}
void Setting3DP::initSetting(RichParameterSet *settingParam)
{

#if TRANSFER_SWITCH
	//language
	QStringList languageList = QStringList() << "English" << "Japanese";
	settingParam->addParam(new RichEnum("Palette_Language", 0, languageList, tr("Language"), tr("")));
	//undo limit	
	settingParam->addParam(new RichInt("UNDO_LIMIT", 20, "", ""));
	//Unit
	QStringList unitList = QStringList() << "mm" << "inch";
	settingParam->addParam(new RichEnum("DISPLAY_UNIT", 0, unitList));
	//TargetPrinter
	QStringList targetPrinter = QStringList() << "PartPro350 XBC" /*<<"Palette_Jr"*/;
	settingParam->addParam(new RichEnum("TARGET_PRINTER", 0, targetPrinter));

	settingParam->addParam(new RichFloat("SLIGHT_HEIGHT", 0.1016));

	settingParam->addParam(new RichFloat("AUTO_PACKING_GAP", 2));
	/*SPITTOON*/
	settingParam->addParam(new RichFloat("SACRIFICE_WIDTH", 0.01));//cm

	/*QStringList spitton_mode = QStringList() << " Normal" << "Quality" << "Customize";
	settingParam->addParam(new RichEnum("SPITTON_MODE", 0, spitton_mode));*/

	settingParam->addParam(new RichFloat("SPITTOON_M", 3));//mm
	settingParam->addParam(new RichFloat("SPITTOON_C", 3));//mm
	settingParam->addParam(new RichFloat("SPITTOON_Y", 3));//mm
	settingParam->addParam(new RichFloat("SPITTOON_B", 5));//mm
	//======================================//


	settingParam->addParam(new RichFloat("SPITTOON_SECOND_CMY", 0.5));//mm
	settingParam->addParam(new RichFloat("SPITTOON_SECOND_B", 5));//mm

	settingParam->addParam(new RichBool("SYNCHRONIZE_PRINTING", false, "SYNCHRONIZE_PRINTING", ""));

	settingParam->addParam(new RichBool("ADD_PATTERN", true, "ADD_PATTERN", ""));

	QStringList pageSize = QStringList() << "Short_Page" << "Long_Page";
	settingParam->addParam(new RichEnum("PAGE_SIZE", 1, pageSize));

	settingParam->addParam(new RichFloat("PLUS_PRINT_LENGTH", DSP_plusPrintLength));

	settingParam->addParam(new RichInt("START_PRINT_PAGE", 0));

	//Fast rendering
	settingParam->addParam(new RichBool("FAST_RENDERING", false, "", ""));

	settingParam->addParam(new RichInt("TOTAL_FACET", 30000000));

	//string desProfilePath = PicaApplication::getRoamingDir().toStdString() + "/ColorProfile/CMY_54_64_93c05to0_m05to0_y1to0_c295to245_m355to295_y55to45_c6-4m8-7.icm";
	string desProfilePath = PicaApplication::getRoamingDir().toStdString() + "/ColorProfile/CMY_54_64_93c05to0_m05to0_y1to0_c295to245_m36to29_y505to415_c6to4_m8-7.icm";
	settingParam->addParam(new RichString("COLOR_PROFILE", QString::fromStdString(desProfilePath)));

	settingParam->addParam(new RichInt("WIPER_CAPACITY", 500));
	settingParam->addParam(new RichInt("FAN_SPEED", 5400));
	settingParam->addParam(new RichInt("PUMP_VALUE", 0));
	//settingParam->addParam(new RichInt("WIPE_FREQUENCY", 8));

	/*
	mid Job Frequency and wiper param
	*/
	settingParam->addParam(new RichInt("MIDJOB_FREQUENCY", 8));
	settingParam->addParam(new RichInt("WIPER_INDEX", 2));
	settingParam->addParam(new RichInt("WIPER_CLICK", 1));


	settingParam->addParam(new RichBool("STIFF_PRINT", false, "", ""));

	settingParam->addParam(new RichBool("WITNESS_BAR", true, "", ""));
	settingParam->addParam(new RichInt("WITNESS_BAR_PERCENT", 50, "", ""));


	settingParam->addParam(new RichBool("STIFF_PRIN_V2", true, "", ""));
	settingParam->addParam(new RichInt("STIFF_PRINT_VALUE", 100, "", ""));

	settingParam->addParam(new RichInt("FIRST_TWENTY_LAYER_PERCENT", 20, "", ""));
	settingParam->addParam(new RichInt("SHELL_PERCENT", 100, "", ""));


	settingParam->addParam(new RichBool("CMYB_BLACK", false, "", ""));
	settingParam->addParam(new RichInt("CMYB_BLACK_VALUE", 100, "", ""));

	settingParam->addParam(new RichBool("CHANNEL_CONTROL", false, "", ""));
	settingParam->addParam(new RichBool("C_CHANNEL", false, "", ""));
	settingParam->addParam(new RichBool("M_CHANNEL", false, "", ""));
	settingParam->addParam(new RichBool("Y_CHANNEL", false, "", ""));
	settingParam->addParam(new RichBool("B_CHANNEL", true, "", ""));



	settingParam->addParam(new RichBool("DILATE_BINDER", true, "", ""));

	settingParam->addParam(new RichInt("DILATE_BINDER_VALUE", 2, "", ""));

	settingParam->addParam(new RichBool("DYNAMIC_WIPE", false, "", ""));

	//***Groove Gap
	settingParam->addParam(new RichFloat("GROOVE_GAP_L", 2));
	settingParam->addParam(new RichFloat("GROOVE_GAP_R", 2));
	settingParam->addParam(new RichFloat("GROOVE_GAP_T", 2));
	settingParam->addParam(new RichFloat("GROOVE_GAP_B", 2));
	//===color binding
	settingParam->addParam(new RichBool("COLOR_BINDING", false, "", ""));
	settingParam->addParam(new RichFloat("COLOR_BINDING_VALUE", 9, "", ""));

	settingParam->addParam(new RichFloat("WITNESS_WIDTH", 2, "", ""));

	settingParam->addParam(new RichBool("CONTINUE_PRINTING", false, "", ""));
	settingParam->addParam(new RichInt("CONTINUE_PRINTING_PAGE", 0, "", ""));
	settingParam->addParam(new RichInt("CONTINUE_PRINTING_END_PAGE", 0, "", ""));

	settingParam->addParam(new RichBool("LINE_OR_RECT", true, "", ""));
	settingParam->addParam(new RichFloat("HORIZONE_LINE_WIDTH", 0.8, "", ""));
	settingParam->addParam(new RichFloat("VERTICAL_LINE_WIDTH", 0.8, "", ""));

	QStringList colo_Mode = QStringList() << "WARM_COLOR" << "COOL_COLOR";
	settingParam->addParam(new RichEnum("COLOR_MODE", 0, colo_Mode, tr("COLOR_MODE"), tr("")));

	settingParam->addParam(new RichFloat("PP_BOTTOM_LIGHTER_THICK", 0));
	settingParam->addParam(new RichInt("PP_BOTTOM_LIGHTER_PERCENTAGE", 100));

	settingParam->addParam(new RichBool("PP_POST_HEATING_SWITCH", true, "", ""));
	settingParam->addParam(new RichInt("PP_POST_HEATING_MINUTES", 120));
	settingParam->addParam(new RichInt("PP_POST_HEATING_UPPER_LIMIT", 45));
	settingParam->addParam(new RichInt("PP_POST_HEATING_LOWER_LIMIT", 40));

	settingParam->addParam(new RichBool("PROFILE_ON", true, "", ""));

	settingParam->addParam(new RichBool("IR_On_Off", true, "", ""));



	/*string desProfilePath_test = PicaApplication::getRoamingDir().toStdString() + "/ColorProfile/CMY_54_64_93c05to0_m05to0_y1to0_c295to245_m36to29_y505to415_c6to4_m8-7.icm";
	settingParam->addParam(new RichString("COLOR_MODE_PROFILE", QString::fromStdString(desProfilePath_test)));*/
	/*
	DM Parameter
	*/
	/*settingParam->addParam(new RichFloat("DM_SHELL_THICKNESS", 5., "", ""));
	settingParam->addParam(new RichInt("DM_CELL_SIZE", 9, "", ""));
	settingParam->addParam(new RichInt("DM_INNER_PERCENT", 60, "", ""));
	QStringList DM_printMode = QStringList() << "DM_FORWARD" << "DM_BACKWARD" << "DM_FORWARD_BACKWARD";
	settingParam->addParam(new RichEnum("DM_PRINT_MODE", 0, DM_printMode, tr("DM_PRINT_MODE"), tr("")));*/

	/*
	DM Setting,
	Horizontal thick mm,
	Horizontal channels,
	Horizontal cell size,
	Horizontal pattern percent

	Vertical mm,
	Vertical channels,
	Vertical cell size,
	Vertical pattern percent

	Bottom mm,
	Bottom fill mode,
	Bottom cell size,
	Bottom pattern percent

	Inner Cell Size,
	Inner Percent,
	Inner channels
	*/
	settingParam->addParam(new RichFloat("DM_SHELL_HORIZONTAL_THICKNESS", 1., "", ""));
	settingParam->addParam(new RichBool("DM_SHELL_HORIZONTAL_THICKNESS_C", false, "", ""));
	settingParam->addParam(new RichBool("DM_SHELL_HORIZONTAL_THICKNESS_M", false, "", ""));
	settingParam->addParam(new RichBool("DM_SHELL_HORIZONTAL_THICKNESS_Y", false, "", ""));
	settingParam->addParam(new RichBool("DM_SHELL_HORIZONTAL_THICKNESS_B", false, "", ""));
	settingParam->addParam(new RichInt("DM_HORIZONTAL_C_Value", 0, "", ""));//R, G, B, K value
	settingParam->addParam(new RichInt("DM_HORIZONTAL_M_Value", 0, "", ""));
	settingParam->addParam(new RichInt("DM_HORIZONTAL_Y_Value", 0, "", ""));
	settingParam->addParam(new RichInt("DM_HORIZONTAL_K_Value", 0, "", ""));
	settingParam->addParam(new RichInt("DM_HORIZONTAL_CELL_SIZE", 9, "", ""));
	settingParam->addParam(new RichInt("DM_HORIZONTAL_PERCENTAGE", 60, "", ""));
	settingParam->addParam(new RichInt("DM_HORIZONTAL_PIXEL_VALUE", 0, "", ""));


	settingParam->addParam(new RichFloat("DM_SHELL_VERTICAL_THICKNESS", 1., "", ""));
	settingParam->addParam(new RichBool("DM_SHELL_VERTICAL_THICKNESS_C", false, "", ""));
	settingParam->addParam(new RichBool("DM_SHELL_VERTICAL_THICKNESS_M", false, "", ""));
	settingParam->addParam(new RichBool("DM_SHELL_VERTICAL_THICKNESS_Y", false, "", ""));
	settingParam->addParam(new RichBool("DM_SHELL_VERTICAL_THICKNESS_B", false, "", ""));
	settingParam->addParam(new RichInt("DM_SHELL_TOP_C_Value", 0, "", ""));// R, G, B, K value
	settingParam->addParam(new RichInt("DM_SHELL_TOP_M_Value", 0, "", ""));
	settingParam->addParam(new RichInt("DM_SHELL_TOP_Y_Value", 0, "", ""));
	settingParam->addParam(new RichInt("DM_SHELL_TOP_K_Value", 0, "", ""));
	settingParam->addParam(new RichInt("DM_VERTICAL_CELL_SIZE", 9, "", ""));
	settingParam->addParam(new RichInt("DM_VERTICAL_PERCENTAGE", 60, "", ""));
	settingParam->addParam(new RichInt("DM_VERTICAL_PIXEL_VALUE", 0, "", ""));


	settingParam->addParam(new RichFloat("DM_SHELL_BOTTOM_LIGHTER", 1., "", ""));
	settingParam->addParam(new RichBool("DM_SHELL_BOTTOM_LIGHTER_C", false, "", ""));
	settingParam->addParam(new RichBool("DM_SHELL_BOTTOM_LIGHTER_M", false, "", ""));
	settingParam->addParam(new RichBool("DM_SHELL_BOTTOM_LIGHTER_Y", false, "", ""));
	settingParam->addParam(new RichBool("DM_SHELL_BOTTOM_LIGHTER_B", false, "", ""));
	settingParam->addParam(new RichInt("DM_SHELL_BOTTOM_C_Value", 0, "", ""));// R, G, B, K value
	settingParam->addParam(new RichInt("DM_SHELL_BOTTOM_M_Value", 0, "", ""));
	settingParam->addParam(new RichInt("DM_SHELL_BOTTOM_Y_Value", 0, "", ""));
	settingParam->addParam(new RichInt("DM_SHELL_BOTTOM_K_Value", 0, "", ""));
	settingParam->addParam(new RichInt("DM_BOTTOM_CELL_SIZE", 9, "", ""));
	settingParam->addParam(new RichInt("DM_BOTTOM_PERCENTAGE", 60, "", ""));
	settingParam->addParam(new RichInt("DM_BOTTOM_PIXEL_VALUE", 0, "", ""));

	settingParam->addParam(new RichBool("DM_INNER_C", false, "", ""));
	settingParam->addParam(new RichBool("DM_INNER_M", false, "", ""));
	settingParam->addParam(new RichBool("DM_INNER_Y", false, "", ""));
	settingParam->addParam(new RichBool("DM_INNER_B", false, "", ""));
	settingParam->addParam(new RichInt("DM_INNER_C_Value", 0, "", ""));// R, G, B, K value
	settingParam->addParam(new RichInt("DM_INNER_M_Value", 0, "", ""));
	settingParam->addParam(new RichInt("DM_INNER_Y_Value", 0, "", ""));
	settingParam->addParam(new RichInt("DM_INNER_K_Value", 0, "", ""));
	settingParam->addParam(new RichInt("DM_INNER_CELL_SIZE", 9, "", ""));
	settingParam->addParam(new RichInt("DM_INNER_PERCENTAGE", 60, "", ""));
	settingParam->addParam(new RichInt("DM_INNER_PIXEL_VALUE", 0, "", ""));


	settingParam->addParam(new RichBool("DM_TWICE_IN_ONE_LAYER", false, "", ""));

	string dmICMFilePath = PicaApplication::getRoamingDir().toStdString() + "/ColorProfile/PlainPaper1600x1600color.icm";
	settingParam->addParam(new RichString("DM_ICM_FOR_DITHER", QString::fromStdString(dmICMFilePath)));
	settingParam->addParam(new RichInt("DM_DITHER_PIXEL_NUM", 127, "", ""));

	settingParam->addParam(new RichBool("DM_RESULT_IMG", false, "", ""));


	QStringList dmSlicingMode = QStringList() << "ONE_FACTOR" << "FOUR_FACTOR" << "FOUR DITHER MATRIX";
	settingParam->addParam(new RichEnum("DM_SLICING_MODE", 0, dmSlicingMode, tr("COLOR_MODE"), tr("")));

	QStringList dmSpitMode = QStringList() << "M_Spit" << "CMY_Spit";
	settingParam->addParam(new RichEnum("DM_SPIT_SWITCH", 0, dmSpitMode, tr("SPIT_MODE"), tr("")));
#else 
	if (decodeParamString.isNull())
		BCPwareFileSystem::decodeParam(decodeParamString, QString(), BCPwareFileSystem::parameterFilePath());
	//update folder position, vary in different computer	
	if (!QFileInfo(getJsonFileParamValue(PP350_SETTING_ca, "COLOR_PROFILE").toString()).exists())
	{
		QVariant pp350colorMode = getJsonFileParamValue(PP350_SETTING_ca, "COLOR_MODE");
		if (!pp350colorMode.isNull()){
			switch (pp350colorMode.toInt())
			{
			case 0:
				updateValueToJsonFile(PP350_SETTING_ca, "COLOR_PROFILE", PicaApplication::getRoamingDir() + "/ColorProfile/CMY_54_64_93c05to0_m05to0_y1to0_c295to245_m36to29_y505to415_c6to4_m8-7.icm");
				break;
			case 1:
				updateValueToJsonFile(PP350_SETTING_ca, "COLOR_PROFILE", PicaApplication::getRoamingDir() + "/ColorProfile/ECI2002CMYKEyeOneL_siriusCMY_73_76_91_large_paperGray_1ti_coveron_lightPlus20.icm");
				break;
			}
		}
		else
		{
			WRITELOG(logger, framework::Diagnostics::LogLevel::Info, _T("color mode is null"));
		}


		//PicaApplication::getRoamingDir() + "/ColorProfile/CMY_54_64_93c05to0_m05to0_y1to0_c295to245_m36to29_y505to415_c6to4_m8-7.icm"
	}

	if (!QFileInfo(getJsonFileParamValue(PP352_SETTING_ca, "DM_ICM_FOR_DITHER").toString()).exists())
	{
		QFileInfo dmICM(getJsonFileParamValue(PP352_SETTING_ca, "DM_ICM_FOR_DITHER").toString());
		QFileInfo currentPCLocation(PicaApplication::getRoamingDir() + dmICM.fileName());
		if (currentPCLocation.exists())
			updateValueToJsonFile(PP352_SETTING_ca, "DM_ICM_FOR_DITHER", currentPCLocation.absoluteFilePath());
		else qDebug() << "ICM not exist";

	}
	else qDebug() << "ICM  exist";
	//updateValueToJsonFile(PP350_SETTING_ca, "COLOR_PROFILE",);

	/*Init setting from Json document, those are current parameters*/
	createRichParamfromJdoc(PP350_SETTING_ca, settingParam);
	createRichParamfromJdoc(PP352_SETTING_ca, settingParam);
	createRichParamfromJdoc(Common_Setting_ca, settingParam);

	//createPrinterSetting_FromRichParameter(PP350_SETTING, settingParam);
	//createRichParamfromJdoc(SLICING_SETTING_NAME, settingParam);

#endif
	//createPrinterSetting_FromRichParameter(3, PP352_SETTING, settingParam);

}

void Setting3DP::getaccept()
{
#if TRANSFER_SWITCH
	/*QStringList languageList = QStringList() << "English" << "Traditional Chinese";
	QStringList unitList = QStringList() << "mm" << "inch";
	QStringList targetPrinter = QStringList() << "Picasso_1_0" << "Picasso_Jr";

	tmpParSet.addParam(new RichEnum("Picasso_Language", ui->languageCB->currentIndex() , languageList, tr("Language"), tr("")));
	tmpParSet.addParam(new RichEnum("DisplayUnit", ui->displayUnitCB->currentIndex() , unitList, tr("unitList"), tr("")));
	tmpParSet.addParam(new RichEnum("TargetPrinter", ui->printerCB->currentIndex() , targetPrinter, tr("TargetPrinter"), tr("")));
	ui->languageCB->currentIndex();*/

	//getValue from widget and save to register
	foreach(RichParameter *curpar, loadtoWidgetParam->paramList)
	{
		if (curpar->name == QString("Palette_Language"))
		{
			curpar->val->set(EnumValue(ui->languageCB->currentIndex()));
			acceptOne(curpar);
		}
		//else if (curpar->name == QString("UNDO_LIMIT")) { curpar->val->set(IntValue(ui->undoLimitSB->value())); acceptOne(curpar); }
		else if (curpar->name == QString("DISPLAY_UNIT")) { curpar->val->set(EnumValue(ui->displayUnitCB->currentIndex())); acceptOne(curpar); }
		else if (curpar->name == QString("TARGET_PRINTER")) { curpar->val->set(EnumValue(ui->printerCB->currentIndex())); acceptOne(curpar); }
		else if (curpar->name == QString("SLIGHT_HEIGHT")) { curpar->val->set(FloatValue(ui->sliceHeightLE->value())); acceptOne(curpar); }
		else if (curpar->name == QString("AUTO_PACKING_GAP")) { curpar->val->set(FloatValue(ui->autoPackingGapLE->text().toFloat())); acceptOne(curpar); }

		else if (curpar->name == QString("GROOVE_GAP_L")) { curpar->val->set(FloatValue(ui->grooveGapL->text().toFloat())); acceptOne(curpar); }
		else if (curpar->name == QString("GROOVE_GAP_R")) { curpar->val->set(FloatValue(ui->grooveGapR->text().toFloat())); acceptOne(curpar); }
		else if (curpar->name == QString("GROOVE_GAP_T")) { curpar->val->set(FloatValue(ui->grooveGapT->text().toFloat())); acceptOne(curpar); }
		else if (curpar->name == QString("GROOVE_GAP_B")) { curpar->val->set(FloatValue(ui->grooveGapB->text().toFloat())); acceptOne(curpar); }

		else if (curpar->name == QString("SACRIFICE_WIDTH")) { curpar->val->set(FloatValue(ui->sacrificeStripLE->text().toFloat())); acceptOne(curpar); }
		//else if (curpar->name == QString("SPITTON_MODE")) { curpar->val->set(EnumValue(ui->spitton_modeCB->currentIndex())); acceptOne(curpar); }
		else if (curpar->name == QString("SPITTOON_M")) { curpar->val->set(FloatValue(ui->spittonM_le->value())); acceptOne(curpar); }
		else if (curpar->name == QString("SPITTOON_C")) { curpar->val->set(FloatValue(ui->spittonC_le->value())); acceptOne(curpar); }
		else if (curpar->name == QString("SPITTOON_Y")) { curpar->val->set(FloatValue(ui->spittonY_le->value())); acceptOne(curpar); }
		else if (curpar->name == QString("SPITTOON_B")) { curpar->val->set(FloatValue(ui->spittonB_le->value())); acceptOne(curpar); }

		else if (curpar->name == QString("SPITTOON_SECOND_CMY")) { curpar->val->set(FloatValue(ui->spittonCMY_SecondLE->value())); acceptOne(curpar); }
		else if (curpar->name == QString("SPITTOON_SECOND_B")) { curpar->val->set(FloatValue(ui->spittonB_SecondLE->value())); acceptOne(curpar); }

		else if (curpar->name == QString("SYNCHRONIZE_PRINTING")) { curpar->val->set(BoolValue(ui->startPrintCB->isChecked())); acceptOne(curpar); }
		else if (curpar->name == QString("PAGE_SIZE")) { curpar->val->set(EnumValue(ui->pageSizeCB->currentIndex())); acceptOne(curpar); }
		else if (curpar->name == QString("ADD_PATTERN")) { curpar->val->set(BoolValue(ui->addpatternCB->isChecked())); acceptOne(curpar); }
		else if (curpar->name == QString("PLUS_PRINT_LENGTH")) { curpar->val->set(FloatValue(ui->plusPrintLengthLE->value())); acceptOne(curpar); }
		else if (curpar->name == QString("START_PRINT_PAGE")) { curpar->val->set(IntValue(ui->startPrintPageLE->text().toInt())); acceptOne(curpar); }

		else if (curpar->name == QString("FAST_RENDERING")) { curpar->val->set(BoolValue(ui->totalFacetCB->isChecked())); acceptOne(curpar); }
		else if (curpar->name == QString("TOTAL_FACET")) { curpar->val->set(IntValue(ui->totalFacetsLE->text().toInt())); acceptOne(curpar); }
		else if (curpar->name == QString("FAN_SPEED")) { curpar->val->set(IntValue(ui->fanLE->value())); acceptOne(curpar); }
		//else if (curpar->name == QString("WIPE_FREQUENCY")) { curpar->val->set(IntValue(ui->wipe_frequencyLE->text().toInt())); acceptOne(curpar); }
		else if (curpar->name == QString("PUMP_VALUE")) { curpar->val->set(IntValue(ui->pumpLE->value())); acceptOne(curpar); }

		else if (curpar->name == QString("MIDJOB_FREQUENCY")) { curpar->val->set(IntValue(ui->midJobFrequencyLE->value())); acceptOne(curpar); }
		else if (curpar->name == QString("WIPER_INDEX")) { curpar->val->set(IntValue(ui->wiperIndexLE->value())); acceptOne(curpar); }
		else if (curpar->name == QString("WIPER_CLICK")) { curpar->val->set(IntValue(ui->wiperClickLE->value())); acceptOne(curpar); }

		else if (curpar->name == QString("STIFF_PRINT")) { curpar->val->set(BoolValue(ui->stiffPrintCB->isChecked())); acceptOne(curpar); }
		else if (curpar->name == QString("STIFF_PRIN_V2")) { curpar->val->set(BoolValue(ui->stiffPrintV2CB->isChecked())); acceptOne(curpar); }
		else if (curpar->name == QString("STIFF_PRINT_VALUE")) { curpar->val->set(IntValue(ui->stiffPrintV2LE->value())); acceptOne(curpar); }

		else if (curpar->name == QString("WITNESS_BAR")) { curpar->val->set(BoolValue(ui->witnessbarCB->isChecked())); acceptOne(curpar); }
		else if (curpar->name == QString("WITNESS_BAR_PERCENT")) { curpar->val->set(IntValue(ui->witnessbar_SPB->value())); acceptOne(curpar); }

		else if (curpar->name == QString("PP_BOTTOM_LIGHTER_THICK")) { curpar->val->set(FloatValue(ui->begin_twenty_Layer_Thick_SPB->value())); acceptOne(curpar); }
		else if (curpar->name == QString("PP_BOTTOM_LIGHTER_PERCENTAGE")) { curpar->val->set(IntValue(ui->begin_twenty_Layer_Percent_SPB->value())); acceptOne(curpar); }

		else if (curpar->name == QString("SHELL_PERCENT")) { curpar->val->set(IntValue(ui->shellPercentLE->value())); acceptOne(curpar); }

		else if (curpar->name == QString("CMYB_BLACK")) { curpar->val->set(BoolValue(ui->CMYB_PercentageCB->isChecked())); acceptOne(curpar); }
		else if (curpar->name == QString("CMYB_BLACK_VALUE")) { curpar->val->set(IntValue(ui->CMYB_PercentageLE->text().toInt())); acceptOne(curpar); }

		else if (curpar->name == QString("CHANNEL_CONTROL")) { curpar->val->set(BoolValue(ui->channelControl_CB->isChecked())); acceptOne(curpar); }
		else if (curpar->name == QString("C_CHANNEL")) { curpar->val->set(BoolValue(ui->C_onOff_CB->isChecked())); acceptOne(curpar); }
		else if (curpar->name == QString("M_CHANNEL")) { curpar->val->set(BoolValue(ui->M_onOff_CB->isChecked())); acceptOne(curpar); }
		else if (curpar->name == QString("Y_CHANNEL")) { curpar->val->set(BoolValue(ui->Y_onOff_CB->isChecked())); acceptOne(curpar); }
		else if (curpar->name == QString("B_CHANNEL")) { curpar->val->set(BoolValue(ui->B_onOff_CB->isChecked())); acceptOne(curpar); }

		else if (curpar->name == QString("PP_POST_HEATING_SWITCH")) { curpar->val->set(BoolValue(ui->postHeatingCB->isChecked())); acceptOne(curpar); }
		else if (curpar->name == QString("PP_POST_HEATING_MINUTES")) { curpar->val->set(IntValue(ui->postHeatingTimeSB->value())); acceptOne(curpar); }
		else if (curpar->name == QString("PP_POST_HEATING_UPPER_LIMIT")) { curpar->val->set(IntValue(ui->postHeatingUpperLimitSP->value())); acceptOne(curpar); }
		else if (curpar->name == QString("PP_POST_HEATING_LOWER_LIMIT")) { curpar->val->set(IntValue(ui->postHeatingLowerLimitSP->value())); acceptOne(curpar); }
		else if (curpar->name == QString("PROFILE_ON")) { curpar->val->set(BoolValue(ui->profileOnCHB->isChecked())); acceptOne(curpar); }
		else if (curpar->name == QString("IR_On_Off")) { curpar->val->set(BoolValue(ui->iR_On_OFF_CB->isChecked())); acceptOne(curpar); }



		else if (curpar->name == QString("DILATE_BINDER"))
		{
			curpar->val->set(BoolValue(ui->dilateBinderCB->isChecked()));
			acceptOne(curpar);
		}
		else if (curpar->name == QString("DILATE_BINDER_VALUE"))
		{
			curpar->val->set(IntValue(ui->dilateBinderLE_2->text().toInt()));
			acceptOne(curpar);
		}
		else if (curpar->name == QString("DYNAMIC_WIPE")) { curpar->val->set(BoolValue(ui->dynamicWipeCB->isChecked())); acceptOne(curpar); }


		else if (curpar->name == QString("COLOR_PROFILE"))
		{
			//curpar->val->set(StringValue(ui->colorProileLB->text()));
			curpar->val->set(StringValue(colorProfilePath));
			acceptOne(curpar);
		}
		else if (curpar->name == QString("COLOR_BINDING"))
		{
			//curpar->val->set(StringValue(ui->colorProileLB->text()));
			curpar->val->set(BoolValue(ui->colorBind_CB->isChecked()));
			acceptOne(curpar);
		}
		else if (curpar->name == QString("WITNESS_WIDTH")) { curpar->val->set(FloatValue(ui->witnessWidthLE->text().toFloat())); acceptOne(curpar); }
		else if (curpar->name == QString("CONTINUE_PRINTING"))
		{
			curpar->val->set(BoolValue(ui->startPageCB->isChecked()));
			acceptOne(curpar);
		}
		else if (curpar->name == QString("CONTINUE_PRINTING_PAGE"))
		{
			curpar->val->set(IntValue(ui->startPageLE->text().toInt()));
			acceptOne(curpar);
		}
		else if (curpar->name == QString("CONTINUE_PRINTING_END_PAGE"))
		{
			curpar->val->set(IntValue(ui->endPageLE->text().toInt()));
			acceptOne(curpar);
		}
		//else if (curpar->name == QString("LINE_OR_RECT")) { curpar->val->set(BoolValue(ui->line_or_rectangleCB->isChecked())); acceptOne(curpar); }
		else if (curpar->name == QString("HORIZONE_LINE_WIDTH")){ curpar->val->set(FloatValue(ui->horizonLineWidthDB->value())); acceptOne(curpar); }
		else if (curpar->name == QString("VERTICAL_LINE_WIDTH")){ curpar->val->set(FloatValue(ui->veticalShellThickDB->value())); acceptOne(curpar); }
		else if (curpar->name == QString("COLOR_MODE")){
			curpar->val->set(EnumValue(ui->ColorProfileCMB->currentIndex())); acceptOne(curpar);
		}
		/*
		DM
		*/
		/*else if (curpar->name == QString("DM_SHELL_THICKNESS")) { curpar->val->set(FloatValue(ui->DM_ShellThicknessLE->text().toFloat())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_CELL_SIZE")) { curpar->val->set(IntValue(ui->DM_Cell_SizeLE->text().toInt())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_INNER_PERCENT")) { curpar->val->set(IntValue(ui->DM_InnerPercentLE->text().toInt())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_PRINT_MODE")){	curpar->val->set(EnumValue(ui->DM_PrintModeCMB->currentIndex())); acceptOne(curpar);}*/




		else if (curpar->name == QString("DM_SHELL_HORIZONTAL_THICKNESS")){ curpar->val->set(FloatValue(ui->horizontalThickDSB->value())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_SHELL_HORIZONTAL_THICKNESS_C")) { curpar->val->set(BoolValue(ui->C_HT_Che->isChecked())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_SHELL_HORIZONTAL_THICKNESS_M")) { curpar->val->set(BoolValue(ui->M_HT_Che->isChecked())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_SHELL_HORIZONTAL_THICKNESS_Y")) { curpar->val->set(BoolValue(ui->Y_HT_Che->isChecked())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_SHELL_HORIZONTAL_THICKNESS_B")) { curpar->val->set(BoolValue(ui->B_HT_Che->isChecked())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_HORIZONTAL_C_Value")) { curpar->val->set(IntValue(ui->shell_C_SB->value())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_HORIZONTAL_M_Value")) { curpar->val->set(IntValue(ui->shell_M_SB->value())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_HORIZONTAL_Y_Value")) { curpar->val->set(IntValue(ui->shell_Y_SB->value())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_HORIZONTAL_K_Value")) { curpar->val->set(IntValue(ui->shell_K_SB->value())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_HORIZONTAL_CELL_SIZE")) { curpar->val->set(IntValue(ui->horizonCellWidthSPB->value())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_HORIZONTAL_PERCENTAGE")) { curpar->val->set(IntValue(ui->horizonPatternPercentSPB->value())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_HORIZONTAL_PIXEL_VALUE")) { curpar->val->set(IntValue(ui->horizonePixelValueDS->value())); acceptOne(curpar); }


		else if (curpar->name == QString("DM_SHELL_VERTICAL_THICKNESS")){ curpar->val->set(FloatValue(ui->vertcalThickDSB->value())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_SHELL_VERTICAL_THICKNESS_C")) { curpar->val->set(BoolValue(ui->C_VT_Che->isChecked())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_SHELL_VERTICAL_THICKNESS_M")) { curpar->val->set(BoolValue(ui->M_VT_Che->isChecked())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_SHELL_VERTICAL_THICKNESS_Y")) { curpar->val->set(BoolValue(ui->Y_VT_Che->isChecked())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_SHELL_VERTICAL_THICKNESS_B")) { curpar->val->set(BoolValue(ui->B_VT_Che->isChecked())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_SHELL_TOP_C_Value")) { curpar->val->set(IntValue(ui->top_C_SB->value())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_SHELL_TOP_M_Value")) { curpar->val->set(IntValue(ui->top_M_SB->value())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_SHELL_TOP_Y_Value")) { curpar->val->set(IntValue(ui->top_Y_SB->value())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_SHELL_TOP_K_Value")) { curpar->val->set(IntValue(ui->top_K_SB->value())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_VERTICAL_CELL_SIZE")) { curpar->val->set(IntValue(ui->verticalCellWidthSPB->value())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_VERTICAL_PERCENTAGE")) { curpar->val->set(IntValue(ui->verticalPatternPercentSPB->value())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_VERTICAL_PIXEL_VALUE")) { curpar->val->set(IntValue(ui->verticalPixelValueDS->value())); acceptOne(curpar); }


		else if (curpar->name == QString("DM_SHELL_BOTTOM_LIGHTER")){ curpar->val->set(FloatValue(ui->bottomThickDSB->value())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_SHELL_BOTTOM_LIGHTER_C")) { curpar->val->set(BoolValue(ui->C_BM_Che->isChecked())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_SHELL_BOTTOM_LIGHTER_M")) { curpar->val->set(BoolValue(ui->M_BM_Che->isChecked())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_SHELL_BOTTOM_LIGHTER_Y")) { curpar->val->set(BoolValue(ui->Y_BM_Che->isChecked())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_SHELL_BOTTOM_LIGHTER_B")) { curpar->val->set(BoolValue(ui->B_BM_Che->isChecked())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_SHELL_BOTTOM_C_Value")) { curpar->val->set(IntValue(ui->bottom_C_SB->value())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_SHELL_BOTTOM_M_Value")) { curpar->val->set(IntValue(ui->bottom_M_SB->value())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_SHELL_BOTTOM_Y_Value")) { curpar->val->set(IntValue(ui->bottom_Y_SB->value())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_SHELL_BOTTOM_K_Value")) { curpar->val->set(IntValue(ui->bottom_K_SB->value())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_BOTTOM_CELL_SIZE")) { curpar->val->set(IntValue(ui->bottomCellWidthSPB->value())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_BOTTOM_PERCENTAGE")) { curpar->val->set(IntValue(ui->bottomPatternPercentSPB->value())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_BOTTOM_PIXEL_VALUE")) { curpar->val->set(IntValue(ui->bottomPixelValueDS->value())); acceptOne(curpar); }

		else if (curpar->name == QString("DM_INNER_C")) { curpar->val->set(BoolValue(ui->C_IN_Che->isChecked())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_INNER_M")) { curpar->val->set(BoolValue(ui->M_IN_Che->isChecked())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_INNER_Y")) { curpar->val->set(BoolValue(ui->Y_IN_Che->isChecked())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_INNER_B")) { curpar->val->set(BoolValue(ui->B_IN_Che->isChecked())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_INNER_C_Value")) { curpar->val->set(IntValue(ui->inside_C_SB->value())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_INNER_M_Value")) { curpar->val->set(IntValue(ui->inside_M_SB->value())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_INNER_Y_Value")) { curpar->val->set(IntValue(ui->inside_Y_SB->value())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_INNER_K_Value")) { curpar->val->set(IntValue(ui->inside_K_SB->value())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_INNER_CELL_SIZE")) { curpar->val->set(IntValue(ui->innerCellWidthSPB->value())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_INNER_PERCENTAGE")) { curpar->val->set(IntValue(ui->innerPatternPercentSPB->value())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_INNER_PIXEL_VALUE")) { curpar->val->set(IntValue(ui->innerPixelValueDS->value())); acceptOne(curpar); }

		else if (curpar->name == QString("DM_TWICE_IN_ONE_LAYER")) { curpar->val->set(BoolValue(ui->twiceInOneLayerCHB->isChecked())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_ICM_FOR_DITHER"))
		{
			//curpar->val->set(StringValue(ui->colorProileLB->text()));
			curpar->val->set(StringValue(colorProfilePathForDitherMatrix));
			acceptOne(curpar);
		}
		else if (curpar->name == QString("DM_DITHER_PIXEL_NUM")) { curpar->val->set(IntValue(ui->pixelNumberSB->value())); acceptOne(curpar); }
		else if (curpar->name == QString("DM_RESULT_IMG")) { curpar->val->set(BoolValue(ui->genResultImg->isChecked())); acceptOne(curpar); }

		else if (curpar->name == QString("DM_SLICING_MODE")){
			curpar->val->set(EnumValue(ui->generate_mode_settingCB->currentIndex())); acceptOne(curpar);
		}
		else if (curpar->name == QString("DM_SPIT_SWITCH")){
			curpar->val->set(EnumValue(ui->spitSwitchCHB->currentIndex())); acceptOne(curpar);
		}
	}
	accept();


#else	
	updateUIToJsonFile(JsonfileCategory::PP350_SETTING_ca);
	updateUIToJsonFile(JsonfileCategory::PP352_SETTING_ca);
	updateUIToJsonFile(JsonfileCategory::Common_Setting_ca);
	emit jsonFileChanged();

	updateJsonFileToRichParameter(JsonfileCategory::PP350_SETTING_ca);
	updateJsonFileToRichParameter(JsonfileCategory::PP352_SETTING_ca);
	updateJsonFileToRichParameter(JsonfileCategory::Common_Setting_ca);


#endif
}
void Setting3DP::acceptOne(RichParameter *inpar)
{
	QDomDocument doc("PaletteSetting");
	RichParameterXMLVisitor v(doc);
	inpar->accept(v);
	doc.appendChild(v.parElem);
	QString docstring = doc.toString();
	qDebug("Writing into Settings param with name %s and content ****%s****", qPrintable(inpar->name), qPrintable(docstring));
	QSettings setting;
	setting.setValue(inpar->name, QVariant(docstring));

}

void Setting3DP::importSampleFile()
{
	QString fileName = QDir(QCoreApplication::applicationDirPath()).filePath("Sample.3mf");
	//mw->importMeshWithLayerManagement(fileName);
	if (QDir(QCoreApplication::applicationDirPath()).exists("Sample.3mf"))
		mw->openProject2(fileName);
	accept();
}

void Setting3DP::sendUsage()
{

}
void Setting3DP::getColorProfile()
{
	QString profileDirPath = PicaApplication::getRoamingDir() + "/ColorProfile";
	QString colorProfile = QFileDialog::getOpenFileName(this, tr("Get Color Profile"), profileDirPath, "Color Profile (*.icc *.icm;)");
	QFileInfo colorProfilef(colorProfile);
	if (colorProfilef.exists())
	{
		ui->colorProileLB->setText(colorProfilef.fileName());
		colorProfilePath = colorProfile;
		ui->colorProileLB->setFixedWidth(colorProfilef.fileName().size() * 6);
	}
	else
	{
		colorProfilePath = PicaApplication::getRoamingDir() + "/ColorProfile/CMY_54_64_93c05to0_m05to0_y1to0_c295to245_m36to29_y505to415_c6to4_m8-7.icm";
		QFileInfo tempPath(colorProfilePath);
		ui->colorProileLB->setText(tempPath.fileName());
		ui->colorProileLB->setFixedWidth(tempPath.fileName().size() * 6);
	}
}

void Setting3DP::getDMICMColorProfile()
{
	QString profileDirPath = PicaApplication::getRoamingDir() + "/ColorProfile";
	QString colorProfile = QFileDialog::getOpenFileName(this, tr("Get Color Profile"), profileDirPath, "Color Profile (*.icc *.icm;)");
	QFileInfo colorProfilef(colorProfile);
	if (colorProfilef.exists())
	{
		ui->iccNameLB->setText(colorProfilef.fileName());
		colorProfilePathForDitherMatrix = colorProfile;
		ui->iccNameLB->setFixedWidth(colorProfilef.fileName().size() * 6);
	}
	else
	{
		colorProfilePathForDitherMatrix = PicaApplication::getRoamingDir() + "/PlainPaper1600x1600color.icm";
		QFileInfo tempPath(colorProfilePathForDitherMatrix);
		ui->iccNameLB->setText(tempPath.fileName());
		ui->iccNameLB->setFixedWidth(tempPath.fileName().size() * 6);
	}
}


QString Setting3DP::rColorProfilePath()
{
	return QString("abc");
}


void Setting3DP::initDefaultSetting()
{
	//language
	QStringList languageList = QStringList() << "English" << "Japanese";
	defaultParam.addParam(new RichEnum("Palette_Language", 0, languageList, tr("Language"), tr("")));
	//undo limit	
	defaultParam.addParam(new RichInt("UNDO_LIMIT", 20, "", ""));
	//Unit
	QStringList unitList = QStringList() << "mm" << "inch";
	defaultParam.addParam(new RichEnum("DISPLAY_UNIT", 0, unitList));
	//TargetPrinter
	QStringList targetPrinter = QStringList() << "PartPro350 XBC" /*<<"Palette_Jr"*/;
	defaultParam.addParam(new RichEnum("TARGET_PRINTER", 0, targetPrinter));

	defaultParam.addParam(new RichFloat("SLIGHT_HEIGHT", 0.1016));

	defaultParam.addParam(new RichFloat("AUTO_PACKING_GAP", 2));
	/*spittoon*/
	//defaultParam.addParam(new RichEnum("SPITTON_MODE", 0, targetPrinter));

	defaultParam.addParam(new RichFloat("SPITTOON_M", 3));
	defaultParam.addParam(new RichFloat("SPITTOON_C", 3));
	defaultParam.addParam(new RichFloat("SPITTOON_Y", 3));
	defaultParam.addParam(new RichFloat("SPITTOON_B", 5));

	defaultParam.addParam(new RichFloat("SPITTOON_SECOND_CMY", 0.5));
	defaultParam.addParam(new RichFloat("SPITTOON_SECOND_B", 5));


	defaultParam.addParam(new RichFloat("SACRIFICE_WIDTH", 0.01));
	/*=============================*/
	defaultParam.addParam(new RichBool("SYNCHRONIZE_PRINTING", false, "", ""));

	defaultParam.addParam(new RichBool("ADD_PATTERN", true, "", ""));

	QStringList pageSize = QStringList() << "Short_Page" << "Long_Page";
	defaultParam.addParam(new RichEnum("PAGE_SIZE", 1, pageSize));

	defaultParam.addParam(new RichFloat("PLUS_PRINT_LENGTH", DSP_plusPrintLength));

	defaultParam.addParam(new RichInt("START_PRINT_PAGE", 0));

	//Fast rendering
	defaultParam.addParam(new RichBool("FAST_RENDERING", false, "", ""));

	defaultParam.addParam(new RichInt("TOTAL_FACET", 30000000));

	string desProfilePath = PicaApplication::getRoamingDir().toStdString() + "/ColorProfile/CMY_54_64_93c05to0_m05to0_y1to0_c295to245_m36to29_y505to415_c6to4_m8-7.icm";
	defaultParam.addParam(new RichString("COLOR_PROFILE", QString::fromStdString(desProfilePath)));

	defaultParam.addParam(new RichInt("WIPER_CAPACITY", 500));
	defaultParam.addParam(new RichInt("FAN_SPEED", 5400));
	defaultParam.addParam(new RichInt("PUMP_VALUE", 0));
	//defaultParam.addParam(new RichInt("WIPE_FREQUENCY", 8));

	/*
	mid Job Frequency and wiper param
	*/
	defaultParam.addParam(new RichInt("MIDJOB_FREQUENCY", 8));
	defaultParam.addParam(new RichInt("WIPER_INDEX", 2));
	defaultParam.addParam(new RichInt("WIPER_CLICK", 1));

	defaultParam.addParam(new RichBool("STIFF_PRINT", false, "", ""));
	defaultParam.addParam(new RichBool("STIFF_PRIN_V2", true, "", ""));
	defaultParam.addParam(new RichInt("STIFF_PRINT_VALUE", 100, "", ""));

	defaultParam.addParam(new RichBool("WITNESS_BAR", true, "", ""));
	defaultParam.addParam(new RichInt("WITNESS_BAR_PERCENT", 50, "", ""));

	//defaultParam.addParam(new RichInt("FIRST_TWENTY_LAYER_PERCENT", 20, "", ""));
	defaultParam.addParam(new RichInt("SHELL_PERCENT", 100, "", ""));

	defaultParam.addParam(new RichBool("CMYB_BLACK", false, "", ""));
	defaultParam.addParam(new RichInt("CMYB_BLACK_VALUE", 100, "", ""));

	defaultParam.addParam(new RichBool("CHANNEL_CONTROL", false, "", ""));
	defaultParam.addParam(new RichBool("C_CHANNEL", false, "", ""));
	defaultParam.addParam(new RichBool("M_CHANNEL", false, "", ""));
	defaultParam.addParam(new RichBool("Y_CHANNEL", false, "", ""));
	defaultParam.addParam(new RichBool("B_CHANNEL", true, "", ""));

	defaultParam.addParam(new RichBool("DILATE_BINDER", true, "", ""));

	defaultParam.addParam(new RichInt("DILATE_BINDER_VALUE", 2, "", ""));

	defaultParam.addParam(new RichBool("DYNAMIC_WIPE", false, "", ""));
	defaultParam.addParam(new RichBool("COLOR_BINDING", false, "", ""));
	defaultParam.addParam(new RichFloat("COLOR_BINDING_VALUE", 9, "", ""));

	defaultParam.addParam(new RichFloat("WITNESS_WIDTH", 2, "", ""));

	defaultParam.addParam(new RichBool("CONTINUE_PRINTING", false, "", ""));
	defaultParam.addParam(new RichInt("CONTINUE_PRINTING_PAGE", 0));
	defaultParam.addParam(new RichInt("CONTINUE_PRINTING_END_PAGE", 0));

	defaultParam.addParam(new RichBool("LINE_OR_RECT", true, "", ""));
	defaultParam.addParam(new RichFloat("HORIZONE_LINE_WIDTH", 0.8, "", ""));
	defaultParam.addParam(new RichFloat("VERTICAL_LINE_WIDTH", 0.8, "", ""));


	QStringList colo_Mode = QStringList() << "WARM_COLOR" << "COOL_COLOR";
	defaultParam.addParam(new RichEnum("COLOR_MODE", 0, colo_Mode, tr("COLOR_MODE"), tr("")));

	defaultParam.addParam(new RichFloat("PP_BOTTOM_LIGHTER_THICK", 0));
	defaultParam.addParam(new RichInt("PP_BOTTOM_LIGHTER_PERCENTAGE", 100));

	defaultParam.addParam(new RichBool("PP_POST_HEATING_SWITCH", true, "", ""));
	defaultParam.addParam(new RichInt("PP_POST_HEATING_MINUTES", 120));
	defaultParam.addParam(new RichInt("PP_POST_HEATING_UPPER_LIMIT", 45));
	defaultParam.addParam(new RichInt("PP_POST_HEATING_LOWER_LIMIT", 40));

	defaultParam.addParam(new RichBool("PROFILE_ON", true, "", ""));
	defaultParam.addParam(new RichBool("IR_On_Off", true, "", ""));

	/*
	DM
	*/
	/*defaultParam.addParam(new RichFloat("DM_SHELL_THICKNESS", 5., "", ""));
	defaultParam.addParam(new RichInt("DM_CELL_SIZE", 9, "", ""));
	defaultParam.addParam(new RichInt("DM_INNER_PERCENT", 60, "", ""));
	QStringList DM_printMode = QStringList() << "DM_FORWARD" << "DM_BACKWARD" << "DM_FORWARD_BACKWARD";
	defaultParam.addParam(new RichEnum("DM_PRINT_MODE", 0, DM_printMode, tr("DM_PRINT_MODE"), tr("")));*/

	/*
	DM Setting,
	Horizontal thick mm,
	Horizontal channels,
	Horizontal cell size,
	Horizontal pattern percent

	Vertical mm,
	Vertical channels,
	Vertical cell size,
	Vertical pattern percent

	Bottom mm,
	Bottom fill mode,
	Bottom cell size,
	Bottom pattern percent

	Inner Cell Size,
	Inner Percent,
	Inner channels
	*/
	defaultParam.addParam(new RichFloat("DM_SHELL_HORIZONTAL_THICKNESS", 1., "", ""));
	defaultParam.addParam(new RichBool("DM_SHELL_HORIZONTAL_THICKNESS_C", true, "", ""));
	defaultParam.addParam(new RichBool("DM_SHELL_HORIZONTAL_THICKNESS_M", true, "", ""));
	defaultParam.addParam(new RichBool("DM_SHELL_HORIZONTAL_THICKNESS_Y", true, "", ""));
	defaultParam.addParam(new RichBool("DM_SHELL_HORIZONTAL_THICKNESS_B", true, "", ""));
	defaultParam.addParam(new RichInt("DM_HORIZONTAL_C_Value", 0, "", ""));//R, G, B, K value
	defaultParam.addParam(new RichInt("DM_HORIZONTAL_M_Value", 0, "", ""));
	defaultParam.addParam(new RichInt("DM_HORIZONTAL_Y_Value", 0, "", ""));
	defaultParam.addParam(new RichInt("DM_HORIZONTAL_K_Value", 0, "", ""));
	defaultParam.addParam(new RichInt("DM_HORIZONTAL_CELL_SIZE", 9, "", ""));
	defaultParam.addParam(new RichInt("DM_HORIZONTAL_PERCENTAGE", 100, "", ""));
	defaultParam.addParam(new RichInt("DM_HORIZONTAL_PIXEL_VALUE", 0, "", ""));

	defaultParam.addParam(new RichFloat("DM_SHELL_VERTICAL_THICKNESS", 1., "", ""));
	defaultParam.addParam(new RichBool("DM_SHELL_VERTICAL_THICKNESS_C", true, "", ""));
	defaultParam.addParam(new RichBool("DM_SHELL_VERTICAL_THICKNESS_M", true, "", ""));
	defaultParam.addParam(new RichBool("DM_SHELL_VERTICAL_THICKNESS_Y", true, "", ""));
	defaultParam.addParam(new RichBool("DM_SHELL_VERTICAL_THICKNESS_B", true, "", ""));
	defaultParam.addParam(new RichInt("DM_SHELL_TOP_C_Value", 0, "", ""));// R, G, B, K value
	defaultParam.addParam(new RichInt("DM_SHELL_TOP_M_Value", 0, "", ""));
	defaultParam.addParam(new RichInt("DM_SHELL_TOP_Y_Value", 0, "", ""));
	defaultParam.addParam(new RichInt("DM_SHELL_TOP_K_Value", 0, "", ""));
	defaultParam.addParam(new RichInt("DM_VERTICAL_CELL_SIZE", 9, "", ""));
	defaultParam.addParam(new RichInt("DM_VERTICAL_PERCENTAGE", 100, "", ""));
	defaultParam.addParam(new RichInt("DM_VERTICAL_PIXEL_VALUE", 0, "", ""));

	defaultParam.addParam(new RichFloat("DM_SHELL_BOTTOM_LIGHTER", 1., "", ""));
	defaultParam.addParam(new RichBool("DM_SHELL_BOTTOM_LIGHTER_C", true, "", ""));
	defaultParam.addParam(new RichBool("DM_SHELL_BOTTOM_LIGHTER_M", true, "", ""));
	defaultParam.addParam(new RichBool("DM_SHELL_BOTTOM_LIGHTER_Y", true, "", ""));
	defaultParam.addParam(new RichBool("DM_SHELL_BOTTOM_LIGHTER_B", true, "", ""));
	defaultParam.addParam(new RichInt("DM_SHELL_BOTTOM_C_Value", 0, "", ""));// R, G, B, K value
	defaultParam.addParam(new RichInt("DM_SHELL_BOTTOM_M_Value", 0, "", ""));
	defaultParam.addParam(new RichInt("DM_SHELL_BOTTOM_Y_Value", 0, "", ""));
	defaultParam.addParam(new RichInt("DM_SHELL_BOTTOM_K_Value", 0, "", ""));
	defaultParam.addParam(new RichInt("DM_BOTTOM_CELL_SIZE", 9, "", ""));
	defaultParam.addParam(new RichInt("DM_BOTTOM_PERCENTAGE", 100, "", ""));
	defaultParam.addParam(new RichInt("DM_BOTTOM_PIXEL_VALUE", 0, "", ""));

	defaultParam.addParam(new RichBool("DM_INNER_C", true, "", ""));
	defaultParam.addParam(new RichBool("DM_INNER_M", true, "", ""));
	defaultParam.addParam(new RichBool("DM_INNER_Y", true, "", ""));
	defaultParam.addParam(new RichBool("DM_INNER_B", true, "", ""));
	defaultParam.addParam(new RichInt("DM_INNER_C_Value", 0, "", ""));// R, G, B, K value
	defaultParam.addParam(new RichInt("DM_INNER_M_Value", 0, "", ""));
	defaultParam.addParam(new RichInt("DM_INNER_Y_Value", 0, "", ""));
	defaultParam.addParam(new RichInt("DM_INNER_K_Value", 0, "", ""));
	defaultParam.addParam(new RichInt("DM_INNER_CELL_SIZE", 9, "", ""));
	defaultParam.addParam(new RichInt("DM_INNER_PERCENTAGE", 100, "", ""));
	defaultParam.addParam(new RichInt("DM_INNER_PIXEL_VALUE", 0, "", ""));


	defaultParam.addParam(new RichBool("DM_TWICE_IN_ONE_LAYER", false, "", ""));

	string dmICMFilePath = PicaApplication::getRoamingDir().toStdString() + "/ColorProfile/PlainPaper1600x1600color.icm";
	defaultParam.addParam(new RichString("DM_ICM_FOR_DITHER", QString::fromStdString(dmICMFilePath)));
	defaultParam.addParam(new RichInt("DM_DITHER_PIXEL_NUM", 127, "", ""));

	defaultParam.addParam(new RichBool("DM_RESULT_IMG", false, "", ""));


	QStringList dmSlicingMode = QStringList() << "ONE_FACTOR" << "FOUR_FACTOR" << "FOUR DITHER MATRIX";
	defaultParam.addParam(new RichEnum("DM_SLICING_MODE", 0, dmSlicingMode, tr("COLOR_MODE"), tr("")));

	QStringList dmSpitMode = QStringList() << "M_Spit" << "CMY_Spit";
	defaultParam.addParam(new RichEnum("DM_SPIT_SWITCH", 0, dmSpitMode, tr("SPIT_MODE"), tr("")));

}
void Setting3DP::resetSetting()
{

	ui->languageCB->setCurrentIndex(defaultParam.getEnum("Palette_Language"));
	//ui->undoLimitSB->setValue(loadtoWidgetParam->getInt("UNDO_LIMIT"));
	ui->displayUnitCB->setCurrentIndex(defaultParam.getEnum("DISPLAY_UNIT"));
	ui->printerCB->setCurrentIndex(defaultParam.getEnum("TARGET_PRINTER"));

	ui->sliceHeightLE->setValue(defaultParam.getFloat("SLIGHT_HEIGHT"));

	ui->autoPackingGapLE->setText(QString::number(defaultParam.getFloat("AUTO_PACKING_GAP"), 'g', 4));
	/*Spittoon*/
	ui->sacrificeStripLE->setText(QString::number(defaultParam.getFloat("SACRIFICE_WIDTH"), 'g', 4));
	//ui->spitton_modeCB->setCurrentIndex(defaultParam.getEnum("SPITTON_MODE"));
	ui->spittonM_le->setValue(defaultParam.getFloat("SPITTOON_M"));
	ui->spittonC_le->setValue(defaultParam.getFloat("SPITTOON_C"));
	ui->spittonY_le->setValue(defaultParam.getFloat("SPITTOON_Y"));
	ui->spittonB_le->setValue(defaultParam.getFloat("SPITTOON_B"));

	ui->spittonCMY_SecondLE->setValue(defaultParam.getFloat("SPITTOON_SECOND_CMY"));
	ui->spittonB_SecondLE->setValue(defaultParam.getFloat("SPITTOON_B"));

	ui->startPrintCB->setChecked(defaultParam.getBool("SYNCHRONIZE_PRINTING"));
	ui->pageSizeCB->setCurrentIndex(defaultParam.getEnum("PAGE_SIZE"));
	ui->addpatternCB->setChecked(defaultParam.getBool("ADD_PATTERN"));
	ui->plusPrintLengthLE->setValue(defaultParam.getFloat("PLUS_PRINT_LENGTH"));
	ui->startPrintPageLE->setText(QString::number(defaultParam.getInt("START_PRINT_PAGE")));

	ui->totalFacetCB->setChecked(defaultParam.getBool("FAST_RENDERING"));
	ui->totalFacetsLE->setText(QString::number(defaultParam.getInt("TOTAL_FACET")));

	ui->fanLE->setValue(defaultParam.getInt("FAN_SPEED"));
	ui->pumpLE->setValue(defaultParam.getInt("PUMP_VALUE"));
	//ui->wipe_frequencyLE->setText(QString::number(defaultParam.getInt("WIPE_FREQUENCY")));
	/*
	mid Job Frequency and wiper param
	*/
	ui->midJobFrequencyLE->setValue(defaultParam.getInt("MIDJOB_FREQUENCY"));
	ui->wiperIndexLE->setValue(defaultParam.getInt("WIPER_INDEX"));
	ui->wiperClickLE->setValue(defaultParam.getInt("WIPER_CLICK"));


	ui->stiffPrintCB->setChecked(defaultParam.getBool("STIFF_PRINT"));
	ui->stiffPrintV2CB->setChecked(defaultParam.getBool("STIFF_PRIN_V2"));
	ui->stiffPrintV2LE->setValue(defaultParam.getInt("STIFF_PRINT_VALUE"));

	ui->witnessbarCB->setChecked(defaultParam.getBool("WITNESS_BAR"));
	ui->witnessbar_SPB->setValue(defaultParam.getInt("WITNESS_BAR_PERCENT"));

	//ui->begin_twenty_Layer_PercentLE->setText(QString::number(defaultParam.getInt("FIRST_TWENTY_LAYER_PERCENT")));

	ui->begin_twenty_Layer_Thick_SPB->setValue(defaultParam.getFloat("PP_BOTTOM_LIGHTER_THICK"));
	ui->begin_twenty_Layer_Percent_SPB->setValue(defaultParam.getInt("PP_BOTTOM_LIGHTER_PERCENTAGE"));
	ui->shellPercentLE->setValue(defaultParam.getInt("SHELL_PERCENT"));

	ui->CMYB_PercentageCB->setChecked(defaultParam.getBool("CMYB_BLACK"));
	ui->CMYB_PercentageLE->setText(QString::number(defaultParam.getInt("CMYB_BLACK_VALUE")));

	ui->channelControl_CB->setChecked(defaultParam.getBool("CHANNEL_CONTROL"));
	ui->C_onOff_CB->setChecked(defaultParam.getBool("C_CHANNEL"));
	ui->M_onOff_CB->setChecked(defaultParam.getBool("M_CHANNEL"));
	ui->Y_onOff_CB->setChecked(defaultParam.getBool("Y_CHANNEL"));
	ui->B_onOff_CB->setChecked(defaultParam.getBool("B_CHANNEL"));

	ui->dilateBinderCB->setChecked(defaultParam.getBool("DILATE_BINDER"));
	ui->dilateBinderLE_2->setText(QString::number(defaultParam.getInt("DILATE_BINDER_VALUE")));

	ui->dynamicWipeCB->setChecked(defaultParam.getBool("DYNAMIC_WIPE"));

	colorProfilePath = defaultParam.getString("COLOR_PROFILE");
	QFileInfo tempInfo(colorProfilePath);
	if (tempInfo.exists())
	{
		ui->colorProileLB->setText(tempInfo.fileName());
		ui->colorProileLB->setFixedWidth(tempInfo.fileName().size() * 6);
	}

	ui->colorBind_CB->setChecked(defaultParam.getBool("COLOR_BINDING"));
	ui->colorBindLE->setText(QString::number(defaultParam.getFloat("COLOR_BINDING_VALUE"), 'g', 2));

	ui->witnessWidthLE->setText(QString::number(defaultParam.getFloat("WITNESS_WIDTH"), 'g', 4));

	ui->startPageCB->setChecked(defaultParam.getBool("CONTINUE_PRINTING"));
	ui->startPageLE->setText(QString::number(defaultParam.getInt("CONTINUE_PRINTING_PAGE")));
	ui->endPageLE->setText(QString::number(defaultParam.getInt("CONTINUE_PRINTING_END_PAGE")));

	//ui->line_or_rectangleCB->setChecked(defaultParam.getBool("LINE_OR_RECT"));
	ui->horizonLineWidthDB->setValue(defaultParam.getFloat("HORIZONE_LINE_WIDTH"));
	ui->veticalShellThickDB->setValue(defaultParam.getFloat("VERTICAL_LINE_WIDTH"));


	ui->ColorProfileCMB->setCurrentIndex(defaultParam.getEnum("COLOR_MODE"));

	ui->postHeatingCB->setChecked(defaultParam.getBool("PP_POST_HEATING_SWITCH"));
	ui->postHeatingTimeSB->setValue(defaultParam.getInt("PP_POST_HEATING_MINUTES"));
	ui->postHeatingUpperLimitSP->setValue(defaultParam.getInt("PP_POST_HEATING_UPPER_LIMIT"));
	ui->postHeatingLowerLimitSP->setValue(defaultParam.getInt("PP_POST_HEATING_LOWER_LIMIT"));

	ui->profileOnCHB->setChecked(defaultParam.getBool("PROFILE_ON"));
	ui->iR_On_OFF_CB->setChecked(defaultParam.getBool("IR_On_Off"));



	/*
	DM
	*/
	/*ui->DM_ShellThicknessLE->setText(QString::number(defaultParam.getFloat("DM_SHELL_THICKNESS"), 'g', 4));
	ui->DM_Cell_SizeLE->setText(QString::number(defaultParam.getInt("DM_CELL_SIZE")));
	ui->DM_InnerPercentLE->setText(QString::number(defaultParam.getInt("DM_INNER_PERCENT")));
	ui->DM_PrintModeCMB->setCurrentIndex(defaultParam.getEnum("DM_PRINT_MODE"));*/

	/*
	DM Setting,
	Horizontal thick mm,
	Horizontal channels,
	Horizontal cell size,
	Horizontal pattern percent

	Vertical mm,
	Vertical channels,
	Vertical cell size,
	Vertical pattern percent

	Bottom mm,
	Bottom fill mode,
	Bottom cell size,
	Bottom pattern percent

	Inner Cell Size,
	Inner Percent,
	Inner channels
	*/
	ui->horizontalThickDSB->setValue(defaultParam.getFloat("DM_SHELL_HORIZONTAL_THICKNESS"));
	ui->C_HT_Che->setChecked(defaultParam.getBool("DM_SHELL_HORIZONTAL_THICKNESS_C"));
	ui->M_HT_Che->setChecked(defaultParam.getBool("DM_SHELL_HORIZONTAL_THICKNESS_M"));
	ui->Y_HT_Che->setChecked(defaultParam.getBool("DM_SHELL_HORIZONTAL_THICKNESS_Y"));
	ui->B_HT_Che->setChecked(defaultParam.getBool("DM_SHELL_HORIZONTAL_THICKNESS_B"));
	ui->shell_C_SB->setValue(defaultParam.getInt("DM_HORIZONTAL_C_Value"));
	ui->shell_M_SB->setValue(defaultParam.getInt("DM_HORIZONTAL_M_Value"));
	ui->shell_Y_SB->setValue(defaultParam.getInt("DM_HORIZONTAL_Y_Value"));
	ui->shell_K_SB->setValue(defaultParam.getInt("DM_HORIZONTAL_K_Value"));

	ui->horizonCellWidthSPB->setValue(defaultParam.getInt("DM_HORIZONTAL_CELL_SIZE"));
	ui->horizonPatternPercentSPB->setValue(defaultParam.getInt("DM_HORIZONTAL_PERCENTAGE"));
	ui->horizonePixelValueDS->setValue(defaultParam.getInt("DM_HORIZONTAL_PIXEL_VALUE"));

	ui->vertcalThickDSB->setValue(defaultParam.getFloat("DM_SHELL_VERTICAL_THICKNESS"));
	ui->C_VT_Che->setChecked(defaultParam.getBool("DM_SHELL_VERTICAL_THICKNESS_C"));
	ui->M_VT_Che->setChecked(defaultParam.getBool("DM_SHELL_VERTICAL_THICKNESS_M"));
	ui->Y_VT_Che->setChecked(defaultParam.getBool("DM_SHELL_VERTICAL_THICKNESS_Y"));
	ui->B_VT_Che->setChecked(defaultParam.getBool("DM_SHELL_VERTICAL_THICKNESS_B"));
	ui->top_C_SB->setValue(defaultParam.getInt("DM_SHELL_TOP_C_Value"));
	ui->top_M_SB->setValue(defaultParam.getInt("DM_SHELL_TOP_M_Value"));
	ui->top_Y_SB->setValue(defaultParam.getInt("DM_SHELL_TOP_Y_Value"));
	ui->top_K_SB->setValue(defaultParam.getInt("DM_SHELL_TOP_K_Value"));
	ui->verticalCellWidthSPB->setValue(defaultParam.getInt("DM_VERTICAL_CELL_SIZE"));
	ui->verticalPatternPercentSPB->setValue(defaultParam.getInt("DM_VERTICAL_PERCENTAGE"));
	ui->verticalPixelValueDS->setValue(defaultParam.getInt("DM_VERTICAL_PIXEL_VALUE"));

	ui->bottomThickDSB->setValue(defaultParam.getFloat("DM_SHELL_BOTTOM_LIGHTER"));
	ui->C_BM_Che->setChecked(defaultParam.getBool("DM_SHELL_BOTTOM_LIGHTER_C"));
	ui->M_BM_Che->setChecked(defaultParam.getBool("DM_SHELL_BOTTOM_LIGHTER_M"));
	ui->Y_BM_Che->setChecked(defaultParam.getBool("DM_SHELL_BOTTOM_LIGHTER_Y"));
	ui->B_BM_Che->setChecked(defaultParam.getBool("DM_SHELL_BOTTOM_LIGHTER_B"));
	ui->bottom_C_SB->setValue(defaultParam.getInt("DM_SHELL_BOTTOM_C_Value"));
	ui->bottom_M_SB->setValue(defaultParam.getInt("DM_SHELL_BOTTOM_M_Value"));
	ui->bottom_Y_SB->setValue(defaultParam.getInt("DM_SHELL_BOTTOM_Y_Value"));
	ui->bottom_K_SB->setValue(defaultParam.getInt("DM_SHELL_BOTTOM_K_Value"));
	ui->bottomCellWidthSPB->setValue(defaultParam.getInt("DM_BOTTOM_CELL_SIZE"));
	ui->bottomPatternPercentSPB->setValue(defaultParam.getInt("DM_BOTTOM_PERCENTAGE"));
	ui->bottomPixelValueDS->setValue(defaultParam.getInt("DM_BOTTOM_PIXEL_VALUE"));

	ui->C_IN_Che->setChecked(defaultParam.getBool("DM_INNER_C"));
	ui->M_IN_Che->setChecked(defaultParam.getBool("DM_INNER_M"));
	ui->Y_IN_Che->setChecked(defaultParam.getBool("DM_INNER_Y"));
	ui->B_IN_Che->setChecked(defaultParam.getBool("DM_INNER_B"));
	ui->inside_C_SB->setValue(defaultParam.getInt("DM_INNER_C_Value"));
	ui->inside_M_SB->setValue(defaultParam.getInt("DM_INNER_M_Value"));
	ui->inside_Y_SB->setValue(defaultParam.getInt("DM_INNER_Y_Value"));
	ui->inside_K_SB->setValue(defaultParam.getInt("DM_INNER_K_Value"));
	ui->innerCellWidthSPB->setValue(defaultParam.getInt("DM_INNER_CELL_SIZE"));
	ui->innerPatternPercentSPB->setValue(defaultParam.getInt("DM_INNER_PERCENTAGE"));
	ui->innerPixelValueDS->setValue(defaultParam.getInt("DM_INNER_PIXEL_VALUE"));

	ui->twiceInOneLayerCHB->setChecked(defaultParam.getBool("DM_TWICE_IN_ONE_LAYER"));
	colorProfilePathForDitherMatrix = defaultParam.getString("DM_ICM_FOR_DITHER");
	QFileInfo dmICMInfo(colorProfilePathForDitherMatrix);
	if (dmICMInfo.exists())
	{
		ui->iccNameLB->setText(dmICMInfo.fileName());
		ui->iccNameLB->setFixedWidth(dmICMInfo.fileName().size() * 6);
	}
	ui->pixelNumberSB->setValue(defaultParam.getInt("DM_DITHER_PIXEL_NUM"));
	ui->genResultImg->setChecked(defaultParam.getBool("DM_RESULT_IMG"));


	ui->generate_mode_settingCB->setCurrentIndex(defaultParam.getEnum("DM_SLICING_MODE"));

	ui->spitSwitchCHB->setCurrentIndex(defaultParam.getEnum("DM_SPIT_SWITCH"));

	//ui->switchPixelModeCB->setValue(defaultParam.getInt("DM_BOTTOM_PIXEL_VALUE"));


}
Setting3DP::~Setting3DP()
{
	foreach(QString ca, paramType)
	{
		QVector<QVector<SKTWidget *> *>  *testtemp = paramWidgetVector.value(ca);
		for (int i = 0; i < testtemp->size(); i++)
		{
			QVector<SKTWidget *> * widgetTmep = testtemp->at(i);
			for (int j = 0; j < widgetTmep->size(); j++)
			{
				//delete widgetTmep->at(j);
				widgetTmep->at(j)->deleteLater();
			}widgetTmep->clear();
		}testtemp->clear();
	}paramWidgetVector.clear();


}

/**********************************************************************************************************************************************************************************************************/

void Setting3DP::create_PP350_Page()
{


	QLayoutItem *wItem;
	while ((wItem = pp350glay->takeAt(0)) != 0)
		delete wItem->widget();

	//exportSetting







	pp350glay->setMargin(10);
	pp350glay->setSpacing(15);

	QVector<QVector<SKTWidget *>*> *tempVector = paramWidgetVector.value(paramType.at(PP350_SETTING_ca));
	QVector<QVector<SKTWidget *> *>::ConstIterator groupIt;

	for (int i = 0; i < tempVector->size(); i++)
	{
		QVector<SKTWidget *> *groupSKTWidget = tempVector->at(i);
		QGridLayout* framGlay = new QGridLayout();
		QGroupBox *groupbox = new QGroupBox(paramGroupName.value(paramType[PP350_SETTING_ca])->value(i));
		groupbox->setStyleSheet(WidgetStyleSheet::groupBoxStyleSheet());
		groupbox->setFlat(true);
		/*QFrame *frame = new QFrame();
		frame->setFrameShadow(QFrame::Sunken);
		frame->setFrameShape(QFrame::Panel);*/
		for (int j = 0, k = 0; j < groupSKTWidget->size(); j++)
		{
			SKTWidget *tempWidget = groupSKTWidget->at(j);
			QLabel *num = new QLabel(QString::number(k + 1));
			qDebug() << tempWidget->getIdentifyName() << " " << tempWidget->getVisible();


			if (tempWidget->getVisible())
			{
				if (switchSetting)
				{
					/*framGlay->addWidget(num, k, 0);
					framGlay->setColumnStretch(1, 1);
					tempWidget->addWidgetToGridLayout(framGlay, k, 1, WidgetAlignment);*/
					tempWidget->addWidgetToGridLayout(framGlay, k, 0, WidgetAlignment);
					k++;

				}
				else
				{
					if (!tempWidget->getExpert())
					{
						/*//framGlay->addWidget(num, k, 0);
						framGlay->setColumnStretch(1, 1);
						tempWidget->addWidgetToGridLayout(framGlay, k, 1, WidgetAlignment);*/
						tempWidget->addWidgetToGridLayout(framGlay, k, 0, WidgetAlignment);
						k++;
					}

				}

			}

		}
		groupbox->setLayout(framGlay);
		qDebug("Gridlayout %s : %d", groupbox->title().toStdString().c_str(), groupbox->children().size());
		if (groupbox->title() != "deprecated" && groupbox->children().size() > 1)
			pp350glay->addWidget(groupbox);
	}
	pp350glay->setStretch(1, 0);
	ui->scrollAreaWidgetContents_4->setLayout(pp350glay);


}

void Setting3DP::create_PP352_Page()
{
	QPushButton *export352SettingPB = new QPushButton("Export 352 Setting");
	QPushButton *import352SettingPB = new QPushButton("Import 352 Setting");
	connect(export352SettingPB, &QPushButton::clicked, [&](){
		exportSetting(JsonfileCategory::PP352_SETTING_ca);
	});
	connect(import352SettingPB, &QPushButton::clicked, [=](){
		importSetting(JsonfileCategory::PP352_SETTING_ca);
	});
	QGridLayout *g1Layout = new QGridLayout;

	//QVBoxLayout *topLayout = new QVBoxLayout;



	g1Layout->addWidget(export352SettingPB, 1, 0);
	g1Layout->addWidget(import352SettingPB, 1, 1);
	//topLayout->addLayout(g1Layout);
	//topLayout->addStretch();
	ui->pp352buttonFrame->setLayout(g1Layout);




	QGridLayout* glay = new QGridLayout();
	glay->setMargin(10);
	glay->setSpacing(5);

	QVector<QVector<SKTWidget *>*> *tempVector = paramWidgetVector.value(paramType[3]);
	QVector<QVector<SKTWidget *> *>::ConstIterator groupIt;

	for (int i = 0; i < tempVector->size(); i++)
	{
		QVector<SKTWidget *> *groupSKTWidget = tempVector->at(i);
		QGridLayout* framGlay = new QGridLayout();
		QGroupBox *gb1 = new QGroupBox(paramGroupName.value(paramType[PP352_SETTING_ca])->value(i));
		gb1->setStyleSheet(WidgetStyleSheet::groupBoxStyleSheet());
		/*QFrame *frame = new QFrame();
		frame->setFrameShadow(QFrame::Sunken);
		frame->setFrameShape(QFrame::Panel);*/
		for (int j = 0, k = 0; j < groupSKTWidget->size(); j++)
		{
			SKTWidget *tempWidget = groupSKTWidget->at(j);
			QLabel *num = new QLabel(QString::number(k));

			if (tempWidget->getVisible())
			{
				framGlay->addWidget(num, k, 0);
				tempWidget->addWidgetToGridLayout(framGlay, k, 1, WidgetAlignment);
				k++;
			}

		}
		gb1->setLayout(framGlay);
		glay->addWidget(gb1);
	}
	glay->setColumnStretch(1, 0);
	ui->pp352ScorllArea->setLayout(glay);


}
void Setting3DP::create_Common_Page()
{
	QGridLayout* glay = new QGridLayout();
	glay->setMargin(10);
	glay->setSpacing(5);

	QVector<QVector<SKTWidget *>*> *tempVector = paramWidgetVector.value(paramType[4]);
	QVector<QVector<SKTWidget *> *>::ConstIterator groupIt;

	for (int i = 0; i < tempVector->size(); i++)
	{
		QVector<SKTWidget *> *groupSKTWidget = tempVector->at(i);
		QGridLayout* framGlay = new QGridLayout();
		QGroupBox *gb1 = new QGroupBox(paramGroupName.value(paramType[1])->value(i));
		gb1->setStyleSheet(WidgetStyleSheet::groupBoxStyleSheet());
		/*QFrame *frame = new QFrame();
		frame->setFrameShadow(QFrame::Sunken);
		frame->setFrameShape(QFrame::Panel);*/
		for (int j = 0, k = 0; j < groupSKTWidget->size(); j++)
		{
			SKTWidget *tempWidget = groupSKTWidget->at(j);
			QLabel *num = new QLabel(QString::number(k));

			if (tempWidget->getVisible())
			{
				framGlay->addWidget(num, k, 0);
				tempWidget->addWidgetToGridLayout(framGlay, k, 1, WidgetAlignment);
				k++;
			}

		}
		gb1->setLayout(framGlay);
		glay->addWidget(gb1);
	}
	glay->setColumnStretch(1, 0);
	ui->common_settingArea->setLayout(glay);
}
void Setting3DP::create_GeneralAndEditPage()
{
	/**/
	/**/



	QVector<QVector<SKTWidget *>*> *tempVector = paramWidgetVector.value(paramType[4]);
	QVector<QVector<SKTWidget *> *>::ConstIterator groupIt;
	QGridLayout* glayEditTest = new QGridLayout();
	glayEditTest->setMargin(10);
	glayEditTest->setSpacing(5);
	for (int i = 0; i < tempVector->size(); i++)
	{
		
		QGridLayout* glay = new QGridLayout();
		glay->setMargin(10);
		glay->setSpacing(5);

		QVector<SKTWidget *> *groupSKTWidget = tempVector->at(i);
		QGridLayout* framGlay = new QGridLayout();

		framGlay->setSpacing(5);
		QGroupBox *gb1 = new QGroupBox(paramGroupName.value(paramType[Common_Setting_ca])->value(i));
		gb1->setStyleSheet(WidgetStyleSheet::groupBoxStyleSheet());
		for (int j = 0, k = 0; j < groupSKTWidget->size(); j++)
		{
			SKTWidget *tempWidget = groupSKTWidget->at(j);
			QLabel *num = new QLabel(QString::number(k));

			if (tempWidget->getVisible())
			{
				//framGlay->addWidget(num, k, 0);				
				//tempWidget->addWidgetToGridLayout(framGlay, k, 1, WidgetAlignment);
				tempWidget->addWidgetToGridLayout(framGlay, k, 0, WidgetAlignment);
				k++;
			}

		}
		framGlay->setRowStretch(groupSKTWidget->size(), 1);
		gb1->setLayout(framGlay);		
		

		switch (i)
		{
		case 0:
			glay->addWidget(gb1);
			ui->general_page->setLayout(glay);
			break;
		case 1:
		case 2:
			glayEditTest->addWidget(gb1);
			//ui->editor_page->setLayout(glay);
			break;

		}

	}
	ui->scrollAreaWidgetContents_6->setLayout(glayEditTest);


}
void Setting3DP::createNVMPage()
{
	updateToFPGAButton = new QPushButton("updateToFPGAButton");
	getFromFPGA = new QPushButton("getFromFPGA");

	outputSettingToFile = new QPushButton("outputSettingToFile");
	inputSettingFromFile = new QPushButton("inputSettingFromFile");

	setDefaultValueButton = new QPushButton("setDefaultValueButton");

	QPushButton *exportSettingPB = new QPushButton("Export NVM Setting");
	QPushButton *importSettingPB = new QPushButton("Import NVM Setting");
	connect(exportSettingPB, &QPushButton::clicked, [&](){
		exportSetting(JsonfileCategory::Advanced_Setting);
	});
	connect(importSettingPB, &QPushButton::clicked, [=](){
		importSetting(JsonfileCategory::Advanced_Setting);
	});

	QGridLayout* gridLayout = new QGridLayout();
	/*QHBoxLayout *h1Layout = new QHBoxLayout;
	QHBoxLayout *h2Layout = new QHBoxLayout;
	QHBoxLayout *h3Layout = new QHBoxLayout;*/


	updateToFPGAButton->setDisabled(true);
	gridLayout->addWidget(getFromFPGA, 0, 0);
	gridLayout->addWidget(updateToFPGAButton, 0, 1);

	//gridLayout->addWidget(outputSettingToFile, 1, 0);
	//gridLayout->addWidget(inputSettingFromFile, 1, 1);

	gridLayout->addWidget(exportSettingPB, 1, 0);
	gridLayout->addWidget(importSettingPB, 1, 1);

	gridLayout->addWidget(setDefaultValueButton, 2, 0, 3, 2);
	ui->nvmButtonFrame->setLayout(gridLayout);


	/*topLayout->addWidget(setDefaultValueButton);
	topLayout->addStretch();*/
	//ui->NVMPage->setLayout(gridLayout);




	QGridLayout* glay = new QGridLayout();
	glay->setMargin(10);
	glay->setSpacing(5);

	QVector<QVector<SKTWidget *>*> *tempVector = paramWidgetVector.value(paramType[1]);
	QVector<QVector<SKTWidget *> *>::ConstIterator groupIt;

	for (int i = 0; i < tempVector->size(); i++)
	{
		QVector<SKTWidget *> *groupSKTWidget = tempVector->at(i);
		QGridLayout* framGlay = new QGridLayout();
		QGroupBox *gb1 = new QGroupBox(paramGroupName.value(paramType[1])->value(i));
		/*QFrame *frame = new QFrame();
		frame->setFrameShadow(QFrame::Sunken);
		frame->setFrameShape(QFrame::Panel);*/
		for (int j = 0, k = 0; j < groupSKTWidget->size(); j++)
		{
			SKTWidget *tempWidget = groupSKTWidget->at(j);
			QLabel *num = new QLabel(QString::number(k));

			if (tempWidget->getVisible())
			{
				framGlay->addWidget(num, k, 0);

				tempWidget->addWidgetToGridLayout(framGlay, k, 1, WidgetAlignment);
				k++;
			}

		}
		gb1->setLayout(framGlay);
		glay->addWidget(gb1);
	}
	glay->setColumnStretch(1, 0);
	ui->scrollAreaWidgetContents_2->setLayout(glay);



	//QGridLayout* glay = new QGridLayout();
	//QVector<SKTWidget *> *tempVector = paramWidgetVector.value(NVM_SETTING_NAME);

	//for (int i = 0, j = 0; i < tempVector->size(); i++)
	//{
	//	SKTWidget *tempWidget = tempVector->at(i);
	//	QLabel *num = new QLabel(QString::number(i));

	//	if (tempWidget->getVisible())
	//	{
	//		glay->addWidget(num, j, 0);
	//		tempWidget->addWidgetToGridLayout(glay, j, 1);
	//		j++;
	//	}

	//	//glay->addWidget(temp,i,0);
	//}
	//glay->setColumnStretch(1, 0);
	//ui->scrollAreaWidgetContents_2->setLayout(glay);
}

//deprecated function
void Setting3DP::updateRichParameterFromJsonFile(QString type)
{


	//
	//if (reinterpret_cast<SpinboxDecoration *>(paramWidgetVector[0])->getName() == "abcdefg")
	//{
	//	RichParameter *param = loadtoWidgetParam->findParameter("abcdefg");
	//	//param->val->set(Value(paramWidgetVector[0]->getValue()));
	//}

	/*createRichParamfromJdoc*/
	QString _jsonString;
	ParamOp::getJsonFiletoString(_jsonString, paramFileLocation);// "C:/Users/TB495076/Documents/BCPware/ParameterUI_STX.txt");

	//go for all element's attribute, transform to richparameterset
	QVariantMap firstFloor;
	QVariantList secondFloorList;
	QVariantMap secondFloorMap;
	QVariantList thirdFloorList;
	QVariantMap thirdFloorMap;
	QVariantMap fourthFloorMap;

	//QString jsonString;
	QJsonDocument jsonDoc;

	QJsonParseError error;
	jsonDoc = QJsonDocument::fromJson(_jsonString.toUtf8(), &error);
	if (error.error == QJsonParseError::NoError)
	{
		firstFloor = jsonDoc.toVariant().toMap();
	}


	//********test count execute layers*********************//
	QMapIterator<QString, QVariant> ii(jsonDoc.toVariant().toMap());
	while (ii.hasNext())
	{
		ii.next();
		if (ii.key() == "categories")
		{
			secondFloorList = ii.value().toList();
			foreach(QVariant secondFloorListValue, secondFloorList)
			{
				secondFloorMap = secondFloorListValue.toMap();
				QVariant categoryName = secondFloorMap.value("name");
				if (categoryName == type)
				{
					thirdFloorList = secondFloorMap.value("parameters").toList();
					foreach(QVariant thirdFloorListValue, thirdFloorList)
					{
						thirdFloorMap = thirdFloorListValue.toMap();
						fourthFloorMap = thirdFloorMap.value("control").toMap();

						QVariant controlName = fourthFloorMap.value("name");
						/*--------------Ui parameter Attribute ---------------------*/
						QString tempLabelName = fourthFloorMap.value("label_name").toString();
						QString identifyerName = thirdFloorMap.value("name").toString();
						QVariant paramValue = fourthFloorMap.value("value");
						int transformType = fourthFloorMap.value("transform_enum").toInt();
						QString uiUnit = fourthFloorMap.value("ui_unit").toString();
						bool visible = fourthFloorMap.value("visible").toBool();
						int highLimit = fourthFloorMap.value("highLimit").toInt();
						int lowLimit = fourthFloorMap.value("lowLimit").toInt();

						QStringList enumList = fourthFloorMap.value("enumeration").toStringList();

						QVariant _defaultValue = fourthFloorMap.value("default");

						foreach(RichParameter *curpar, loadtoWidgetParam->paramList)
						{
							if (identifyerName == curpar->name)
							{
								if (controlName == "SpinBox")
								{
									//p--labelName--identifyName-- categoryName--, value, uiUnit, transformType, visible, highLimit, lowLimit
									curpar->val->set(IntValue(paramValue.toInt()));
								}

								else if (controlName == "DSpinBox")
								{
									curpar->val->set(FloatValue(paramValue.toFloat()));
								}
								else if (controlName == "CheckBox")
								{
									curpar->val->set(FloatValue(paramValue.toFloat()));
								}
								else if (controlName == "EnumUI")
								{
									curpar->val->set(EnumValue(paramValue.toInt()));
								}
								else if (controlName == "FileDialogUI")
								{
									curpar->val->set(StringValue(paramValue.toString()));
								}
								else if (controlName == "TextfieldUI" || controlName == "TextLabelUI")
								{
									curpar->val->set(StringValue(paramValue.toString()));
								}
							}
						}
					}
				}
			}
		}
	}



}

//JSON file ===> SKT Widget
//void Setting3DP::createParamSettingUI(QString type)
//{
//	QString _jsonString;
//	ParamOp::getJsonFiletoString(_jsonString, PicaApplication::getRoamingDir() + "ParameterUI_STX.txt");// "C:/Users/TB495076/Documents/BCPware/ParameterUI_STX.txt");
//
//	//go for all element's attribute, transform to richparameterset
//	QVariantMap firstFloor;
//	QVariantList secondFloorList;
//	QVariantMap secondFloorMap;
//	QVariantList thirdFloorList;
//	QVariantMap thirdFloorMap;
//	QVariantMap fourthFloorMap;
//
//	//QString jsonString;
//	QJsonDocument jsonDoc;
//
//	QJsonParseError error;
//	jsonDoc = QJsonDocument::fromJson(_jsonString.toUtf8(), &error);
//	if (error.error == QJsonParseError::NoError)
//	{
//		firstFloor = jsonDoc.toVariant().toMap();
//	}
//
//
//	//********test count execute layers*********************//
//	QMapIterator<QString, QVariant> ii(jsonDoc.toVariant().toMap());
//	while (ii.hasNext())
//	{
//		ii.next();
//		if (ii.key() == "categories")
//		{
//			secondFloorList = ii.value().toList();
//			foreach(QVariant secondFloorListValue, secondFloorList)
//			{
//				secondFloorMap = secondFloorListValue.toMap();
//				QVariant categoryName = secondFloorMap.value("name");
//				if (categoryName == type)
//				{
//					thirdFloorList = secondFloorMap.value("parameters").toList();
//					foreach(QVariant thirdFloorListValue, thirdFloorList)
//					{
//						thirdFloorMap = thirdFloorListValue.toMap();
//						fourthFloorMap = thirdFloorMap.value("control").toMap();
//
//						QVariant controlName = fourthFloorMap.value("name");
//						/*--------------Ui parameter Attribute ---------------------*/
//						QString tempLabelName = fourthFloorMap.value("label_name").toString();
//						QString identifyerName = thirdFloorMap.value("name").toString();
//						QVariant parmaValue = fourthFloorMap.value("value");
//						int transformType = fourthFloorMap.value("transform_enum").toInt();
//						QString uiUnit = fourthFloorMap.value("ui_unit").toString();
//						bool visible = fourthFloorMap.value("visible").toBool();
//						int highLimit = fourthFloorMap.value("highLimit").toInt();
//						int lowLimit = fourthFloorMap.value("lowLimit").toInt();
//						QVariant _defaultValue = fourthFloorMap.value("default");
//
//						if (controlName == "SpinBox")
//						{
//							//p--labelName--identifyName-- categoryName--, value, uiUnit, transformType, visible, highLimit, lowLimit
//							SpinBoxWidget_SKX *tempSpinBox = new SpinBoxWidget_SKX(nullptr, tempLabelName, identifyerName, categoryName, parmaValue.toInt(), _defaultValue, uiUnit, transformType, visible, highLimit, lowLimit);
//							paramWidgetVector.value(type)->push_back(tempSpinBox);
//							connect(tempSpinBox, &DoubleSpinBox_SKX::parameterChanged, [this]() {
//								updateToFPGAButton->setEnabled(true);
//							});
//
//						}
//						if (controlName == "DSpinBox" && (fourthFloorMap.value("transform_enum") > 0))
//						{
//							DoubleSpinBox_SKX *tempSpinBox =
//								new DoubleSpinBox_SKX(nullptr, tempLabelName, identifyerName, categoryName, parmaValue.toDouble(), _defaultValue, uiUnit, transformType, visible, highLimit, lowLimit);
//							paramWidgetVector.value(type)->push_back(tempSpinBox);
//
//							connect(tempSpinBox, &DoubleSpinBox_SKX::parameterChanged, [this]() {
//								updateToFPGAButton->setEnabled(true);
//							});
//
//						}
//						else if (controlName == "DSpinBox")
//						{
//
//						}
//						else if (controlName == "CheckBox")
//						{
//
//						}
//						else if (controlName == "EnumUI")
//						{
//
//						}
//						else if (controlName == "FileDialogUI")
//						{
//							FileDialog_SKX *tempFileDialog = new FileDialog_SKX(nullptr, tempLabelName, identifyerName, categoryName, parmaValue.toString(), _defaultValue);
//							paramWidgetVector.value(type)->push_back(tempFileDialog);
//						}
//						else if (controlName == "TextfieldUI" || controlName == "TextLabelUI")
//						{
//
//							TextLabelUI_SKX *tempLabelUI =
//								new TextLabelUI_SKX(nullptr, tempLabelName, identifyerName, categoryName, parmaValue.toString(), _defaultValue);
//							paramWidgetVector.value(type)->push_back(tempLabelUI);
//						}
//						//paramAttrib->setTransformType(fourthFloorMap.value("transform_enum"));
//						if (fourthFloorMap.value("transform_enum").toInt() > 0)
//						{
//							//qDebug() << "JSON Data" << paramAttrib->name() << paramAttrib->transformType();
//						}
//
//
//					}
//				}
//			}
//		}
//	}
//
//	//connect(this,SIGNAL)
//}

//JSON file ===> SKT Widget
void Setting3DP::createParamSettingUI(JsonfileCategory type)
{
	QVector<QVector<SKTWidget *> *>  *testtemp = paramWidgetVector.value(paramType[type]);
	for (int i = 0; i < testtemp->size(); i++)
	{
		QVector<SKTWidget *> * widgetTmep = testtemp->at(i);
		for (int j = 0; j < widgetTmep->size(); j++)
		{
			//delete widgetTmep->at(j);
			widgetTmep->at(j)->deleteLater();
		}
	}

	paramWidgetVector.insert(paramType[type], new QVector<QVector<SKTWidget*>*>());


	QString outputstring;
	BCPwareFileSystem::decodeParam(outputstring, QString(), BCPwareFileSystem::parameterFilePath());

	QVariant category, categoryList, typeMap, paramList, categoryName;
	ParamOp::extractVariantTest(category, QVariant(), QString(), -1, QString(), decodeParamString);
	//ParamOp::extractVariantTest(category, QVariant(), QString(), -1, paramFileLocation);


	ParamOp::extractVariantTest(categoryList, category, "categories");


	ParamOp::extractVariantTest(typeMap, categoryList, QString(), type);
	ParamOp::extractVariantTest(categoryName, typeMap, "name");
	ParamOp::extractVariantTest(paramList, typeMap, "parameters");
	int i = 0;
	foreach(QVariant parameterItem, paramList.toList())
	{
		QVariant groupItemList, parameterName, groupName;

		ParamOp::extractVariantTest(groupItemList, parameterItem, "group");
		ParamOp::extractVariantTest(groupName, parameterItem, "groupName");

		paramWidgetVector.value(paramType[type])->push_back(new QVector<SKTWidget*>);
		paramGroupName.value(paramType[type])->insert(i, groupName.toString());


		foreach(QVariant groupItem, groupItemList.toList())
		{


			QVariant controlMap, defaultValue, enumItem, highValue, lowValue, uiName, label_name, transformValue, ui_unitValue, paramValue, visible, value_TypeValue, expertValue;
			ParamOp::extractVariantTest(parameterName, groupItem, "name");

			ParamOp::extractVariantTest(controlMap, groupItem, "control");
			ParamOp::extractVariantTest(uiName, controlMap, "name");
			ParamOp::extractVariantTest(defaultValue, controlMap, "default");
			ParamOp::extractVariantTest(enumItem, controlMap, "enumeration");
			ParamOp::extractVariantTest(highValue, controlMap, "highLimit");
			ParamOp::extractVariantTest(lowValue, controlMap, "lowLimit");
			ParamOp::extractVariantTest(label_name, controlMap, "label_name");
			ParamOp::extractVariantTest(transformValue, controlMap, "transform_enum");
			ParamOp::extractVariantTest(ui_unitValue, controlMap, "ui_unit");
			ParamOp::extractVariantTest(paramValue, controlMap, "value");
			ParamOp::extractVariantTest(visible, controlMap, "visible");
			ParamOp::extractVariantTest(value_TypeValue, controlMap, "value_Type");
			if (!ParamOp::extractVariantTest(expertValue, controlMap, "expert"))
			{
				expertValue = true;
			}
			else
			{
				qDebug() << "get expert value";
			}

			if (uiName == "SpinBox")
			{
				SpinBoxWidget_SKX *tempSpinBox =
					new SpinBoxWidget_SKX(nullptr, label_name.toString(), parameterName.toString(), categoryName.toString(), paramValue.toInt(), defaultValue, ui_unitValue.toString(), transformValue.toInt(), visible.toBool(), highValue.toInt(), lowValue.toInt());

				tempSpinBox->setExpert(expertValue.toBool());

				paramWidgetVector.value(paramType[type])->at(i)->push_back(tempSpinBox);



				connect(tempSpinBox, &DoubleSpinBox_SKX::parameterChanged, [this, tempSpinBox]() {
					updateToFPGAButton->setEnabled(true);


				});
			}
			else if (uiName == "DSpinBox" && transformValue.toInt() > 0)
			{
				DoubleSpinBox_SKX *tempSpinBox =
					new DoubleSpinBox_SKX(nullptr, label_name.toString(), parameterName.toString(), categoryName, paramValue, defaultValue, ui_unitValue.toString(), transformValue.toInt(), visible.toBool(), highValue.toInt(), lowValue.toInt());
				tempSpinBox->setExpert(expertValue.toBool());

				paramWidgetVector.value(paramType[type])->at(i)->push_back(tempSpinBox);


				connect(tempSpinBox, &DoubleSpinBox_SKX::parameterChanged, [this, tempSpinBox]() {
					updateToFPGAButton->setEnabled(true);

				});
			}
			else if (uiName == "DSpinBox")
			{
				DoubleSpinBox_SKX *tempSpinBox =
					new DoubleSpinBox_SKX(nullptr, label_name.toString(), parameterName.toString(), categoryName, paramValue.toDouble(), defaultValue, ui_unitValue.toString(), 0, visible.toBool(), highValue.toInt(), lowValue.toInt());
				tempSpinBox->setExpert(expertValue.toBool());

				paramWidgetVector.value(paramType[type])->at(i)->push_back(tempSpinBox);



				connect(tempSpinBox, &DoubleSpinBox_SKX::parameterChanged, [this, tempSpinBox]() {
					updateToFPGAButton->setEnabled(true);

				});
			}
			else if (uiName == "CheckBox")
			{
				CheckUI_SKX *tempCheckBox =
					new CheckUI_SKX(nullptr, label_name.toString(), parameterName.toString(), categoryName, paramValue, defaultValue, 0, visible.toBool());
				tempCheckBox->setExpert(expertValue.toBool());
				paramWidgetVector.value(paramType[type])->at(i)->push_back(tempCheckBox);

				/*connect(tempCheckBox, &CheckUI_SKX::parameterChanged, [this, tempCheckBox]() {
					tempCheckBox->setChanged(true);
					});*/
			}
			else if (uiName == "EnumUI")
			{

				EnumUI_SKX *tempEnum = new EnumUI_SKX(nullptr, label_name.toString(), parameterName.toString(), categoryName, paramValue, defaultValue, enumItem, 0, visible.toBool());

				tempEnum->setExpert(expertValue.toBool());

				paramWidgetVector.value(paramType[type])->at(i)->push_back(tempEnum);
				/*connect(tempEnum, &EnumUI_SKX::parameterChanged, [this, tempEnum]() {
					tempEnum->setChanged(true);
					});*/
			}
			else if (uiName == "FileDialogUI")
			{
				FileDialog_SKX *tempFileDialog = new FileDialog_SKX(nullptr, label_name.toString(), parameterName.toString(), categoryName, paramValue.toString(), defaultValue);
				tempFileDialog->setExpert(expertValue.toBool());
				paramWidgetVector.value(paramType[type])->at(i)->push_back(tempFileDialog);
				/*connect(tempFileDialog, &FileDialog_SKX::parameterChanged, [this, tempFileDialog]() {
					tempFileDialog->setChanged(true);
					});*/
			}
			else if (uiName == "TextfieldUI" || uiName.toString() == "TextLabelUI")
			{
				TextLabelUI_SKX *tempLabelUI =
					new TextLabelUI_SKX(nullptr, label_name.toString(), parameterName.toString(), categoryName, paramValue.toString(), defaultValue);
				tempLabelUI->setExpert(expertValue.toBool());
				paramWidgetVector.value(paramType[type])->at(i)->push_back(tempLabelUI);

				/*connect(tempLabelUI, &TextLabelUI_SKX::parameterChanged, [this, tempLabelUI]() {
					tempLabelUI->setChanged(true);

					});*/

			}



		}
		i++;
	}

}
//Json file === > RichParameterSet
void Setting3DP::updateJsonFileToRichParameter(JsonfileCategory type)
{
	QVariant category, categoryList, typeMap, paramList, categoryName;
	//ParamOp::extractVariantTest(category, QVariant(), QString(), -1, paramFileLocation);
	ParamOp::extractVariantTest(category, QVariant(), QString(), -1, QString(), decodeParamString);
	ParamOp::extractVariantTest(categoryList, category, "categories");


	ParamOp::extractVariantTest(typeMap, categoryList, QString(), type);
	ParamOp::extractVariantTest(categoryName, typeMap, "name");
	ParamOp::extractVariantTest(paramList, typeMap, "parameters");
	int i = 0;
	foreach(QVariant parameterItem, paramList.toList())
	{
		QVariant groupItemList, groupName;

		ParamOp::extractVariantTest(groupItemList, parameterItem, "group");


		foreach(QVariant groupItem, groupItemList.toList())
		{


			QVariant identifyerName, controlMap, defaultValue, enumItem, highValue, lowValue, uiName, label_name, transformValue, ui_unitValue, paramValue, visible, value_TypeValue;
			ParamOp::extractVariantTest(identifyerName, groupItem, "name");

			ParamOp::extractVariantTest(controlMap, groupItem, "control");
			ParamOp::extractVariantTest(uiName, controlMap, "name");
			ParamOp::extractVariantTest(defaultValue, controlMap, "default");
			ParamOp::extractVariantTest(enumItem, controlMap, "enumeration");
			ParamOp::extractVariantTest(highValue, controlMap, "highLimit");
			ParamOp::extractVariantTest(lowValue, controlMap, "lowLimit");
			ParamOp::extractVariantTest(label_name, controlMap, "label_name");
			ParamOp::extractVariantTest(transformValue, controlMap, "transform_enum");
			ParamOp::extractVariantTest(ui_unitValue, controlMap, "ui_unit");
			ParamOp::extractVariantTest(paramValue, controlMap, "value");
			ParamOp::extractVariantTest(visible, controlMap, "visible");
			ParamOp::extractVariantTest(value_TypeValue, controlMap, "value_Type");

			if (uiName == "SpinBox")
			{
				loadtoWidgetParam->setValue(identifyerName.toString(), IntValue(paramValue.toInt()));
			}
			else if (uiName == "DSpinBox" && transformValue.toInt() > 0)
			{
				loadtoWidgetParam->setValue(identifyerName.toString(), FloatValue(paramValue.toFloat()));
			}
			else if (uiName == "DSpinBox")
			{
				loadtoWidgetParam->setValue(identifyerName.toString(), FloatValue(paramValue.toFloat()));
			}
			else if (uiName == "CheckBox")
			{
				loadtoWidgetParam->setValue(identifyerName.toString(), BoolValue(paramValue.toBool()));
			}
			else if (uiName == "EnumUI")
			{
				loadtoWidgetParam->setValue(identifyerName.toString(), EnumValue(paramValue.toInt()));
			}
			else if (uiName == "FileDialogUI")
			{
				loadtoWidgetParam->setValue(identifyerName.toString(), StringValue(paramValue.toString()));
			}
			else if (uiName == "TextfieldUI" || uiName.toString() == "TextLabelUI")
			{
				loadtoWidgetParam->setValue(identifyerName.toString(), StringValue(paramValue.toString()));
			}



		}
		i++;
	}




}

// UI_Value==>Json File
void Setting3DP::updateUIToJsonFile(JsonfileCategory type)
{

	QString paramName;// = item->getNameParam().toString();
	QVariant paramValue;// = item->getValueParam().toString();
	QString ui_typeValue;// = item->getUi_typeParam().toString();	


	QJsonDocument jsonDoc;
	QJsonParseError error;
	QString _jsonString;

	QVariant category, categoryList, typeMap, paramList, categoryName;

	//if (!ParamOp::extractVariantTest(category, QVariant(), QString(), -1, paramFileLocation))
	if (!ParamOp::extractVariantTest(category, QVariant(), QString(), -1, QString(), decodeParamString))
		return;


	QVector<QVector<SKTWidget *>*> *groupWidget = paramWidgetVector.value(paramType.at(type));
	for (int x = 0; x < groupWidget->size(); x++)
	{
		QVector<SKTWidget *> *tempVector = groupWidget->at(x);
		for (int i = 0; i < tempVector->size(); i++)
		{
			SKTWidget *tempWidget = tempVector->at(i);
			//categoryName = tempWidget->getCategoryName().toString();
			paramName = tempWidget->getIdentifyName().toString();
			paramValue = tempWidget->getValue();
			bool changed = tempWidget->getChanged();

			ParamOp::extractVariantTest(categoryList, category, "categories");
			ParamOp::extractVariantTest(typeMap, categoryList, QString(), type);
			ParamOp::extractVariantTest(paramList, typeMap, "parameters");

			bool valueSet = false;
			if (changed)
				for (int j = 0; j < paramList.toList().size(); j++)
				{

					QVariant groupItemList, groupMap;
					ParamOp::extractVariantTest(groupMap, paramList.toList(), QString(), j);
					ParamOp::extractVariantTest(groupItemList, groupMap, "group");

					for (int z = 0; z < groupItemList.toList().size(); z++)
					{
						QVariant identifyerName, groupItem;
						ParamOp::extractVariantTest(groupItem, groupItemList, QString(), z);
						ParamOp::extractVariantTest(identifyerName, groupItem, "name");
						if (paramName == identifyerName)
						{
							QVariant controlMap;
							ParamOp::extractVariantTest(controlMap, groupItem, "control");

							ParamOp::mergeValue(controlMap, paramValue, "value");

							ParamOp::mergeValue(groupItem, controlMap, "control");
							ParamOp::mergeValue(groupItemList, groupItem, QString(), z);

							ParamOp::mergeValue(groupMap, groupItemList, "group");
							ParamOp::mergeValue(paramList, groupMap, QString(), j);

							ParamOp::mergeValue(typeMap, paramList, "parameters");
							ParamOp::mergeValue(categoryList, typeMap, QString(), type);

							ParamOp::mergeValue(category, categoryList, "categories");

							valueSet = true;
							tempWidget->setChanged(false);
							break;
						}
					}
					if (valueSet)
						break;
				}


		}
	}

	//ParamOp::mergeValue(category, QVariant(), QString(), -1, paramFileLocation);
	/*test encrypt*/
	//emit signal :  update decode string;
	QVariant testtemp = QString();
	ParamOp::mergeValue(testtemp, category);
	BCPwareFileSystem::encryptParam(testtemp.toString(), QFileInfo(BCPwareFileSystem::parameterFilePath()));
	emit jsonFileChanged();

}
void Setting3DP::sendNVMPreProcess()
{
#if 0
	/*if (comm == nullptr)
		qDebug() << "no Server";return*/
	CMD_Value *cmdvalue = new CMD_Value(comm);
	/*get NVM value from UI*/
	QVariantList NVMValueList;
	QVector<QVector<SKTWidget *>*> *groupWidget = paramWidgetVector.value(paramType.at(1));
	//QVector<SKTWidget *> *tempVector = paramWidgetVector.value(NVM_SETTING_NAME);

	for (int x = 0; x < groupWidget->size(); x++){
		QVector<SKTWidget *> *tempVector = groupWidget->at(x);
		for (int i = 0; i < tempVector->size(); i++)
		{
			SKTWidget *tempWidget = tempVector->at(i);
			NVMValueList.push_back(tempWidget->getValue());
			//glay->addWidget(temp,i,0);
		}
	}
	cmdvalue->getValueFromUI(NVMValueList);
	bool result = cmdvalue->sendNVMCommand();
	//qDebug() << result;
#endif

	CMD_Value *cmdvalue = new CMD_Value(comm);
	/*get NVM value from UI*/
	QVector<QVariantList> NVMValueList(5);
	QVector<QVector<SKTWidget *>*> *groupWidget = paramWidgetVector.value(paramType.at(1));
	//QVector<SKTWidget *> *tempVector = paramWidgetVector.value(NVM_SETTING_NAME);

	for (int x = 0; x < groupWidget->size(); x++){
		QVector<SKTWidget *> *tempVector = groupWidget->at(x);
		for (int i = 0; i < tempVector->size(); i++)
		{
			SKTWidget *tempWidget = tempVector->at(i);
			NVMValueList[x].push_back(tempWidget->getValue());
			//glay->addWidget(temp,i,0);
		}
	}
	cmdvalue->getValueFromUI(NVMValueList);
	bool result = cmdvalue->sendNVMCommand();
	//qDebug() << result;



	/*result update to file*/
	//if (true)
	if (result)
	{
		updateUIToJsonFile(Advanced_Setting);
		updateToFPGAButton->setDisabled(true);
	}
	else
		return;



}

void Setting3DP::getNVMFromFPGA()
{
	CMD_Value *cmdvalue = new CMD_Value(comm);
	if (cmdvalue->getNVMValue())
	{
		emit jsonFileChanged();
		/*update UI from JsonFile*/
		updateUIFromJsonFile(JsonfileCategory::Advanced_Setting);

	}





}
//function for NVM setting
bool Setting3DP::updateUIFromJsonFile(JsonfileCategory _category){

	QVariantMap NVMValueList;
	QVariant category;

	QVariant advancedMap, firstList, paramList;
	//ParamOp::extractVariantTest(category, QVariant(), QString(), -1, paramFileLocation);
	ParamOp::extractVariantTest(category, QVariant(), QString(), -1, QString(), decodeParamString);
	ParamOp::extractVariantTest(firstList, category, "categories");
	ParamOp::extractVariantTest(advancedMap, firstList, QString(), _category);
	ParamOp::extractVariantTest(paramList, advancedMap, "parameters");

	for (int i = 0; i < paramList.toList().size(); i++)
	{


		QVariant paramValue, identifyerName;


		//int group

		QVariant groupItemList, groupName;

		ParamOp::extractVariantTest(groupItemList, paramList.toList().at(i), "group");

		int countGroupItem = 0;
		foreach(QVariant groupItem, groupItemList.toList())
		{

			QVariant controlMap, value_TypeValue;
			ParamOp::extractVariantTest(identifyerName, groupItem, "name");

			ParamOp::extractVariantTest(controlMap, groupItem, "control");
			ParamOp::extractVariantTest(paramValue, controlMap, "value");

			if (!identifyerName.isNull())
			{
				QVector<QVector<SKTWidget *>*> *groupWidget = paramWidgetVector.value(paramType[_category]);
				QVector<SKTWidget *> *tempWidgets = groupWidget->at(i);
				tempWidgets->at(countGroupItem)->updateUIValue(paramValue);
			}
			else
			{
				qDebug() << "identifyerName.isNull()";
			}
			countGroupItem++;

		}






	}

	return true;
}

// Json file ===> RichParameterSet
void Setting3DP::createRichParamfromJdoc(JsonfileCategory type, RichParameterSet *currentParamSet)
{
	QVariant category, categoryList, typeMap, paramList, categoryName;
	//ParamOp::extractVariantTest(category, QVariant(), QString(), -1, paramFileLocation);
	QString decodeString;
	BCPwareFileSystem::decodeParam(decodeString, QString(), BCPwareFileSystem::parameterFilePath());

	ParamOp::extractVariantTest(category, QVariant(), QString(), -1, QString(), decodeString);
	ParamOp::extractVariantTest(categoryList, category, "categories");


	ParamOp::extractVariantTest(typeMap, categoryList, QString(), type);
	ParamOp::extractVariantTest(categoryName, typeMap, "name");
	ParamOp::extractVariantTest(paramList, typeMap, "parameters");
	int i = 0;
	foreach(QVariant parameterItem, paramList.toList())
	{
		QVariant groupItemList, groupName;

		ParamOp::extractVariantTest(groupItemList, parameterItem, "group");


		foreach(QVariant groupItem, groupItemList.toList())
		{


			QVariant identifyerName, controlMap, defaultValue, enumItem, highValue, lowValue, uiName, label_name, transformValue, ui_unitValue, paramValue, visible, value_TypeValue;
			ParamOp::extractVariantTest(identifyerName, groupItem, "name");

			ParamOp::extractVariantTest(controlMap, groupItem, "control");
			ParamOp::extractVariantTest(uiName, controlMap, "name");
			ParamOp::extractVariantTest(defaultValue, controlMap, "default");
			ParamOp::extractVariantTest(enumItem, controlMap, "enumeration");
			ParamOp::extractVariantTest(highValue, controlMap, "highLimit");
			ParamOp::extractVariantTest(lowValue, controlMap, "lowLimit");
			ParamOp::extractVariantTest(label_name, controlMap, "label_name");
			ParamOp::extractVariantTest(transformValue, controlMap, "transform_enum");
			ParamOp::extractVariantTest(ui_unitValue, controlMap, "ui_unit");
			ParamOp::extractVariantTest(paramValue, controlMap, "value");
			ParamOp::extractVariantTest(visible, controlMap, "visible");
			ParamOp::extractVariantTest(value_TypeValue, controlMap, "value_Type");

			if (uiName == "SpinBox")
			{
				currentParamSet->addParam(new RichInt(identifyerName.toString(), paramValue.toInt(), "", ""));
			}
			else if (uiName == "DSpinBox" && transformValue.toInt() > 0)
			{
				currentParamSet->addParam(new RichFloat(identifyerName.toString(), paramValue.toFloat()));
			}
			else if (uiName == "DSpinBox")
			{
				currentParamSet->addParam(new RichFloat(identifyerName.toString(), paramValue.toFloat()));
			}
			else if (uiName == "CheckBox")
			{
				currentParamSet->addParam(new RichBool(identifyerName.toString(), paramValue.toBool(), "", ""));
			}
			else if (uiName == "EnumUI")
			{
				currentParamSet->addParam(new RichEnum(identifyerName.toString(), paramValue.toInt(), enumItem.toStringList()));
			}
			else if (uiName == "FileDialogUI")
			{
				currentParamSet->addParam(new RichString(identifyerName.toString(), paramValue.toString()));
			}
			else if (uiName == "TextfieldUI" || uiName.toString() == "TextLabelUI")
			{
				currentParamSet->addParam(new RichString(identifyerName.toString(), paramValue.toString()));
			}



		}
		i++;
	}

}

//Deprecated: RichParameterSet ===> JsonFile, Create Printer_Setting( Slice Setting )Json file from RichParameter   
void Setting3DP::createPrinterSetting_FromRichParameter(int listNum, QString type, RichParameterSet *currentParamSet)
{
	/*for output*/
	QList<RichParameter*>::const_iterator fpli;
	QVariant outputMap = QVariantMap();


	QVariantList NVMValueList;
	QVariant category;
	QVariant printer_setting_map, firstList, paramListDoc;
	ParamOp::extractVariantTest(category, QVariant(), QString(), -1, paramFileLocation);
	ParamOp::extractVariantTest(firstList, category, "categories");
	ParamOp::extractVariantTest(printer_setting_map, firstList, QString(), listNum);
	ParamOp::extractVariantTest(paramListDoc, printer_setting_map, "parameters");

	for (fpli = currentParamSet->paramList.begin(); fpli != currentParamSet->paramList.end(); ++fpli)
	{
		//create paramList 
		ParamOp::mergeValue(outputMap, (*fpli)->val->typeName(), (*fpli)->name);

		//build param Map
		QVariant contorlMap = QVariantMap();
		QVariant parametersMap = QVariantMap();
		//QVariant parametersList = QVariantList();
		qDebug() << (*fpli)->name << " : " << (*fpli)->val->typeName() << " : " << (*fpli)->val->isEnum();
		if ((*fpli)->val->isBool())
		{

			ParamOp::mergeValue(contorlMap, (*fpli)->pd->defVal->getBool(), "default");
			ParamOp::mergeValue(contorlMap, QVariant(), "enumeration");
			ParamOp::mergeValue(contorlMap, QVariant(), "highLimit");
			ParamOp::mergeValue(contorlMap, (*fpli)->name, "label_name");
			ParamOp::mergeValue(contorlMap, QVariant(), "lowLimit");
			ParamOp::mergeValue(contorlMap, "CheckBox", "name");
			ParamOp::mergeValue(contorlMap, QVariant(), "spinStep");
			ParamOp::mergeValue(contorlMap, QVariant(), "transform_enum");
			ParamOp::mergeValue(contorlMap, QVariant(), "ui_unit");
			ParamOp::mergeValue(contorlMap, (*fpli)->val->getBool(), "value");
			ParamOp::mergeValue(contorlMap, QVariant(), "value_Type");
			ParamOp::mergeValue(contorlMap, "true", "visible");
			ParamOp::mergeValue(contorlMap, "0", "group");
			ParamOp::mergeValue(contorlMap, "", "groupName");



		}
		else if ((*fpli)->val->isEnum())
		{
			QStringList enumList = static_cast<EnumDecoration *>((*fpli)->pd)->enumvalues;

			ParamOp::mergeValue(contorlMap, (*fpli)->pd->defVal->getEnum(), "default");
			ParamOp::mergeValue(contorlMap, enumList, "enumeration");
			ParamOp::mergeValue(contorlMap, QVariant(), "highLimit");
			ParamOp::mergeValue(contorlMap, (*fpli)->name, "label_name");
			ParamOp::mergeValue(contorlMap, QVariant(), "lowLimit");
			ParamOp::mergeValue(contorlMap, "EnumUI", "name");
			ParamOp::mergeValue(contorlMap, QVariant(), "spinStep");
			ParamOp::mergeValue(contorlMap, QVariant(), "transform_enum");
			ParamOp::mergeValue(contorlMap, QVariant(), "ui_unit");
			ParamOp::mergeValue(contorlMap, (*fpli)->val->getEnum(), "value");
			ParamOp::mergeValue(contorlMap, QVariant(), "value_Type");
			ParamOp::mergeValue(contorlMap, "true", "visible");
			ParamOp::mergeValue(contorlMap, "0", "group");
			ParamOp::mergeValue(contorlMap, "", "groupName");
		}
		else if ((*fpli)->val->isInt())
		{
			ParamOp::mergeValue(contorlMap, (*fpli)->pd->defVal->getInt(), "default");
			ParamOp::mergeValue(contorlMap, QVariant(), "enumeration");
			ParamOp::mergeValue(contorlMap, QVariant(), "highLimit");
			ParamOp::mergeValue(contorlMap, (*fpli)->name, "label_name");
			ParamOp::mergeValue(contorlMap, QVariant(), "lowLimit");
			ParamOp::mergeValue(contorlMap, "SpinBox", "name");
			ParamOp::mergeValue(contorlMap, QVariant(), "spinStep");
			ParamOp::mergeValue(contorlMap, QVariant(), "transform_enum");
			ParamOp::mergeValue(contorlMap, QVariant(), "ui_unit");
			ParamOp::mergeValue(contorlMap, (*fpli)->val->getInt(), "value");
			ParamOp::mergeValue(contorlMap, QVariant(), "value_Type");
			ParamOp::mergeValue(contorlMap, "true", "visible");
			ParamOp::mergeValue(contorlMap, "0", "group");
			ParamOp::mergeValue(contorlMap, "", "groupName");
		}
		else if ((*fpli)->val->isFloat())
		{
			ParamOp::mergeValue(contorlMap, (*fpli)->pd->defVal->getFloat(), "default");
			ParamOp::mergeValue(contorlMap, QVariant(), "enumeration");
			ParamOp::mergeValue(contorlMap, QVariant(), "highLimit");
			ParamOp::mergeValue(contorlMap, (*fpli)->name, "label_name");
			ParamOp::mergeValue(contorlMap, QVariant(), "lowLimit");
			ParamOp::mergeValue(contorlMap, "DSpinBox", "name");
			ParamOp::mergeValue(contorlMap, QVariant(), "spinStep");
			ParamOp::mergeValue(contorlMap, QVariant(), "transform_enum");
			ParamOp::mergeValue(contorlMap, QVariant(), "ui_unit");
			ParamOp::mergeValue(contorlMap, (*fpli)->val->getFloat(), "value");
			ParamOp::mergeValue(contorlMap, QVariant(), "value_Type");
			ParamOp::mergeValue(contorlMap, "true", "visible");
			ParamOp::mergeValue(contorlMap, "0", "group");
			ParamOp::mergeValue(contorlMap, "", "groupName");
		}
		else if ((*fpli)->val->isString())
		{
			ParamOp::mergeValue(contorlMap, (*fpli)->pd->defVal->getString(), "default");
			ParamOp::mergeValue(contorlMap, QVariant(), "enumeration");
			ParamOp::mergeValue(contorlMap, QVariant(), "highLimit");
			ParamOp::mergeValue(contorlMap, (*fpli)->name, "label_name");
			ParamOp::mergeValue(contorlMap, QVariant(), "lowLimit");
			ParamOp::mergeValue(contorlMap, "TextfieldUI", "name");
			ParamOp::mergeValue(contorlMap, QVariant(), "spinStep");
			ParamOp::mergeValue(contorlMap, QVariant(), "transform_enum");
			ParamOp::mergeValue(contorlMap, QVariant(), "ui_unit");
			ParamOp::mergeValue(contorlMap, (*fpli)->val->getString(), "value");
			ParamOp::mergeValue(contorlMap, QVariant(), "value_Type");
			ParamOp::mergeValue(contorlMap, "true", "visible");
			ParamOp::mergeValue(contorlMap, "0", "group");
			ParamOp::mergeValue(contorlMap, "", "groupName");
		}

		else if ((*fpli)->name == "Palette_Language")
		{
			qDebug() << (*fpli)->val->typeName() << " : " << (*fpli)->val->isEnum();
		}

		ParamOp::mergeValue(parametersMap, contorlMap, "control");
		ParamOp::mergeValue(parametersMap, QVariant(), "CMD");
		ParamOp::mergeValue(parametersMap, (*fpli)->name, "name");
		ParamOp::mergeValue(paramListDoc, parametersMap, QString(), -1);

		ParamOp::mergeValue(printer_setting_map, paramListDoc, "parameters");
		ParamOp::mergeValue(firstList, printer_setting_map, QString(), listNum);
		ParamOp::mergeValue(category, firstList, "categories");



	}
	ParamOp::mergeValue(category, QVariant(), QString(), -1, paramFileLocation);

	//////////
}

bool Setting3DP::exportSetting(JsonfileCategory category)
{

	QVector<QVector<SKTWidget *>*> *groupWidget = paramWidgetVector.value(paramType[category]);
	QVariant categoryMap = QVariantMap();
	QVariant categoryList = QVariantList();
	for (int i = 0; i < groupWidget->size(); i++)
	{
		QVector<SKTWidget*> *tempWidgets = groupWidget->at(i);
		QVariant groupItemMap = QVariantMap();
		QString groupName = QString("group_%1").arg(i);
		QVariant valueMap = QVariantMap();
		foreach(SKTWidget* sktwidget, *tempWidgets)
		{
			ParamOp::mergeValue(valueMap, sktwidget->getValue(), sktwidget->getIdentifyName().toString());
			//ParamOp::mergeValue(valueMap, "", sktwidget->getIdentifyName().toString());
		}
		ParamOp::mergeValue(groupItemMap, valueMap, groupName);
		ParamOp::mergeValue(categoryList, groupItemMap, QString(), -1);
	}
	ParamOp::mergeValue(categoryMap, categoryList, paramType[category]);

	QString fileName = QFileDialog::getSaveFileName(this, tr("Save Setting File"), getDocumentFolder(), tr("Setting (*.zxb);"));


	if (!fileName.isNull())
	{
		/*if (ParamOp::mergeValue(categoryMap, QVariant(), QString(), -1, fileName))
			return true;
			else return false;*/
		QVariant jsonString = QString();
		ParamOp::mergeValue(jsonString, categoryMap, QString());
		BCPwareFileSystem::encryptParam(jsonString.toString(), QFileInfo(fileName));
		return true;
	}


}
bool Setting3DP::importSetting(JsonfileCategory type)
{
	//for every setting, update widget value
	QString filePath = QFileDialog::getOpenFileName(this, tr("Get Setting"), getDocumentFolder(), "Setting (*.zxb);");


	QVariant category, categoryList, typeMap, paramList, categoryName;
	if (!filePath.isNull()){

		QString outputstring;
		BCPwareFileSystem::decodeParam(outputstring, QString(), filePath);

		ParamOp::extractVariantTest(category, QVariant(), QString(), -1, QString(), outputstring);
		QMapIterator<QString, QVariant> ii(category.toMap());
		while (ii.hasNext())
		{
			ii.next();
			QVariant categoryTypeList, groupList;
			ParamOp::extractVariantTest(categoryTypeList, category, ii.key());
			for (int i = 0; i < categoryTypeList.toList().size(); i++)
			{
				QVariant groupItem;
				ParamOp::extractVariantTest(groupItem, categoryTypeList.toList(), QString(), i);
				QMapIterator<QString, QVariant> ii2(groupItem.toMap());
				while (ii2.hasNext()){
					ii2.next();
					QVariant groupItemMap;
					ParamOp::extractVariantTest(groupItemMap, groupItem, ii2.key());
					QMapIterator<QString, QVariant> ii3(groupItemMap.toMap());
					while (ii3.hasNext()){
						ii3.next();
						updateValueToUI(ii.key(), i, ii3.key(), ii3.value());
					}

				}

			}


		}



	}



	return true;
}

bool Setting3DP::updateValueToUI(QString category, int group, QString valueName, QVariant value)
{
	QVector<QVector<SKTWidget *>*> *uiCategory = paramWidgetVector.value(category);
	foreach(QVector<SKTWidget *> *uiGroup, *uiCategory)
	{
		foreach(SKTWidget *tempwidget, *uiGroup)
		{
			qDebug() << "tempwidget->getIdentifyName()" << tempwidget->getIdentifyName();
			if (valueName == tempwidget->getIdentifyName())
			{
				tempwidget->updateUIValue(value);
				return true;
			}
		}
	}

	qWarning() << valueName << ": not updated";
	return false; 

	/*QVector<SKTWidget *> *uiGroup = paramWidgetVector.value(category)->at(group);
	foreach(SKTWidget *tempwidget, *uiGroup)
	{
		qDebug() << "tempwidget->getIdentifyName()" << tempwidget->getIdentifyName();
		if (valueName == tempwidget->getIdentifyName())
		{
			tempwidget->updateUIValue(value);
			return true;
		}
	}
	qWarning() << valueName << ": not updated";
	return false;*/

}



void Setting3DP::ui_set_default_from_current_value(JsonfileCategory category)
{
	QVector<QVector<SKTWidget *>*> *groupWidget = paramWidgetVector.value(paramType[category]);
	for (int i = 0; i < groupWidget->size(); i++)
	{
		QVector<SKTWidget*> *tempWidgets = groupWidget->at(i);
		foreach(SKTWidget* temp, *tempWidgets)
		{
			temp->setDefaultValue(temp->getValue());
		}
	}
}
void Setting3DP::ui_set_value_from_default(JsonfileCategory category)
{
	QVector<QVector<SKTWidget *>*> *groupWidget = paramWidgetVector.value(paramType[category]);
	for (int i = 0; i < groupWidget->size(); i++)
	{
		QVector<SKTWidget*> *tempWidgets = groupWidget->at(i);
		foreach(SKTWidget* temp, *tempWidgets)
		{
			temp->updateUIValue(temp->getDefaultValue());
		}
	}
}
QVariant Setting3DP::getWidgetValue(JsonfileCategory category, QString paramName)
{
	QVector<QVector<SKTWidget *>*> *groupWidget = paramWidgetVector.value(paramType[category]);
	for (int i = 0; i < groupWidget->size(); i++)
	{
		QVector<SKTWidget*> *tempWidgets = groupWidget->at(i);
		foreach(SKTWidget* temp, *tempWidgets)
		{
			if (paramName == temp->getIdentifyName())
				return temp->getValue();
		}
	}
}
SKTWidget* Setting3DP::getWidget(JsonfileCategory category, QString paramName)
{
	QVector<QVector<SKTWidget *>*> *groupWidget = paramWidgetVector.value(paramType[category]);
	for (int i = 0; i < groupWidget->size(); i++)
	{
		QVector<SKTWidget*> *tempWidgets = groupWidget->at(i);
		foreach(SKTWidget* temp, *tempWidgets)
		{
			if (paramName == temp->getIdentifyName())
				return temp;
		}
	}
}
void Setting3DP::reConnectWidgetSignal()
{


}

void Setting3DP::updateValueToJsonFile(JsonfileCategory type, QString _identifyName, QVariant _value)
{
	QVariant category, categoryList, typeMap, paramList, categoryName;

	if (!ParamOp::extractVariantTest(category, QVariant(), QString(), -1, QString(), decodeParamString))
		return;

	ParamOp::extractVariantTest(categoryList, category, "categories");
	ParamOp::extractVariantTest(typeMap, categoryList, QString(), type);
	ParamOp::extractVariantTest(paramList, typeMap, "parameters");


	bool valueSet = false;
	for (int j = 0; j < paramList.toList().size(); j++)
	{

		QVariant groupItemList, groupMap;
		ParamOp::extractVariantTest(groupMap, paramList.toList(), QString(), j);
		ParamOp::extractVariantTest(groupItemList, groupMap, "group");

		for (int z = 0; z < groupItemList.toList().size(); z++)
		{
			QVariant identifyerName, groupItem;
			ParamOp::extractVariantTest(groupItem, groupItemList, QString(), z);
			ParamOp::extractVariantTest(identifyerName, groupItem, "name");
			if (_identifyName == identifyerName)
			{
				QVariant controlMap;
				ParamOp::extractVariantTest(controlMap, groupItem, "control");

				ParamOp::mergeValue(controlMap, _value, "value");

				ParamOp::mergeValue(groupItem, controlMap, "control");
				ParamOp::mergeValue(groupItemList, groupItem, QString(), z);

				ParamOp::mergeValue(groupMap, groupItemList, "group");
				ParamOp::mergeValue(paramList, groupMap, QString(), j);

				ParamOp::mergeValue(typeMap, paramList, "parameters");
				ParamOp::mergeValue(categoryList, typeMap, QString(), type);

				ParamOp::mergeValue(category, categoryList, "categories");
				valueSet = true;
				break;
			}
		}
		if (valueSet)
			break;
	}
	QVariant testtemp = QString();
	ParamOp::mergeValue(testtemp, category);
	BCPwareFileSystem::encryptParam(testtemp.toString(), QFileInfo(BCPwareFileSystem::parameterFilePath()));

}
QVariant Setting3DP::getJsonFileParamValue(JsonfileCategory type, QString _identifyName)
{
	QVariant category, categoryList, typeMap, paramList, categoryName;

	if (!ParamOp::extractVariantTest(category, QVariant(), QString(), -1, QString(), decodeParamString))
		return QVariant();

	ParamOp::extractVariantTest(categoryList, category, "categories");
	ParamOp::extractVariantTest(typeMap, categoryList, QString(), type);
	ParamOp::extractVariantTest(paramList, typeMap, "parameters");


	for (int j = 0; j < paramList.toList().size(); j++)
	{

		QVariant groupItemList, groupMap, value;
		ParamOp::extractVariantTest(groupMap, paramList.toList(), QString(), j);
		ParamOp::extractVariantTest(groupItemList, groupMap, "group");

		for (int z = 0; z < groupItemList.toList().size(); z++)
		{
			QVariant identifyerName, groupItem;
			ParamOp::extractVariantTest(groupItem, groupItemList, QString(), z);
			ParamOp::extractVariantTest(identifyerName, groupItem, "name");
			if (_identifyName == identifyerName)
			{
				QVariant controlMap;
				ParamOp::extractVariantTest(controlMap, groupItem, "control");
				ParamOp::extractVariantTest(value, controlMap, "value");

				return value;
			}
		}

	}
	return QVariant();
}