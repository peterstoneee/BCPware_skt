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

Setting3DP::Setting3DP(MainWindow *_mw, RichParameterSet *currParm, QWidget *parent) :QDialog(parent), ui(new Ui::setting3DP_UI), loadtoWidgetParam(currParm)
{
	ui->setupUi(this);
	ui->retranslateUi(this);
	ui->listWidget->setCurrentRow(0);
	ui->SliceSettingPage->setEnabled(false);
	ui->NVMPage->setEnabled(false);
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

		item = ui->listWidget->item(4);
		item->setHidden(true);

		item = ui->listWidget->item(5);
		item->setHidden(true);

		ui->stackedWidget->widget(3)->hide();
		ui->stackedWidget->widget(3)->setHidden(true);

		ui->stackedWidget->widget(4)->hide();
		ui->stackedWidget->widget(4)->setHidden(true);

		ui->stackedWidget->widget(5)->hide();
		ui->stackedWidget->widget(5)->setHidden(true);


		//ui->stackedWidget->setCurrentWidget(ui->SliceSettingPage);
		ui->stackedWidget->setCurrentWidget(ui->general_page);
	}


	switchSetting = false;
	switchDM = false;

	mw = _mw;
	connect(ui->importSamplePushButton, SIGNAL(clicked()), this, SLOT(importSampleFile()));
	connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(getaccept()));
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
	connect(ui->startPageCB, &QCheckBox::clicked, [this]() {
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
	});

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
	paramType << NVM_SETTING_NAME << PRINTER_SETTING_NAME << SLICING_SETTING_NAME;
	paramWidgetVector.insert(NVM_SETTING_NAME, new QVector<SKTWidget*>());
	paramWidgetVector.insert(PRINTER_SETTING_NAME, new QVector<SKTWidget*>());
	paramWidgetVector.insert(SLICING_SETTING_NAME, new QVector<SKTWidget*>());


	createParamSettingUI(NVM_SETTING_NAME);
	createNVMPage();

	//connect(setDefaultValueButton, SIGNAL(clicked()), this, SLOT(loadDefaultValue()));
	connect(setDefaultValueButton, &QPushButton::clicked, [=](){

		QVector<SKTWidget *> *tempVector = paramWidgetVector.value(NVM_SETTING_NAME);
		for (int i = 0; i < tempVector->size(); i++)
		{
			SKTWidget *tempWidget = tempVector->at(i);
			if (tempWidget->getVisible())
			{
				qDebug() << tempWidget->getIdentifyName() << " " << tempWidget->getDefaultValue();
				tempWidget->updateUIValue(tempWidget->getDefaultValue());
			}


		}
	});

	connect(updateToFPGAButton, &QPushButton::clicked, [=](){

		sendNVMPreProcess();
	});

	connect(getFromFPGA, &QPushButton::clicked, [=](){

		getNVMFromFPGA();
	});



	//ParamOp::transformJsonToRichParameter(tempSet, QString());

}

void Setting3DP::keyPressEvent(QKeyEvent *e)
{
	if (e->matches(QKeySequence::Underline))
	{
		switchSetting ^= 1;

		if (switchSetting)
		{
			QListWidgetItem *item = ui->listWidget->item(3);
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
			ui->listWidget->setCurrentRow(3);
		}
		else
		{
			QListWidgetItem *item = ui->listWidget->item(3);
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
			ui->listWidget->setCurrentRow(0);
		}
	}

	else if (e->matches(QKeySequence::Bold))
	{
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

}

void Setting3DP::getaccept()
{
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
	QString fileName = QCoreApplication::applicationDirPath() + "/Cube.stl";
	mw->importMeshWithLayerManagement(fileName);
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
}

void Setting3DP::createNVMPage()
{
	updateToFPGAButton = new QPushButton("updateToFPGAButton");
	getFromFPGA = new QPushButton("getFromFPGA");
	setDefaultValueButton = new QPushButton("setDefaultValueButton");

	QHBoxLayout *h1Layout = new QHBoxLayout;
	QVBoxLayout *topLayout = new QVBoxLayout;

	updateToFPGAButton->setDisabled(true);
	h1Layout->addWidget(getFromFPGA);
	h1Layout->addWidget(updateToFPGAButton);
	topLayout->addLayout(h1Layout);
	topLayout->addWidget(setDefaultValueButton);
	topLayout->addStretch();
	ui->NVMPage->setLayout(topLayout);




	QGridLayout* glay = new QGridLayout();
	QVector<SKTWidget *> *tempVector = paramWidgetVector.value(NVM_SETTING_NAME);

	for (int i = 0, j = 0; i < tempVector->size(); i++)
	{
		SKTWidget *tempWidget = tempVector->at(i);
		QLabel *num = new QLabel(QString::number(i));

		if (tempWidget->getVisible())
		{
			glay->addWidget(num, j, 0);
			tempWidget->addWidgetToGridLayout(glay, j, 1);
			j++;
		}

		//glay->addWidget(temp,i,0);
	}
	glay->setColumnStretch(1, 0);
	ui->scrollAreaWidgetContents_2->setLayout(glay);
}
void Setting3DP::saveToRichParameterAndJson()
{


	//
	//if (reinterpret_cast<SpinboxDecoration *>(paramWidgetVector[0])->getName() == "abcdefg")
	//{
	//	RichParameter *param = loadtoWidgetParam->findParameter("abcdefg");
	//	//param->val->set(Value(paramWidgetVector[0]->getValue()));
	//}





}
void Setting3DP::createParamSettingUI(QString type)
{
	QString _jsonString;
	ParamOp::getJsonFiletoString(_jsonString, PicaApplication::getRoamingDir() + "ParameterUI_STX.txt");// "C:/Users/TB495076/Documents/BCPware/ParameterUI_STX.txt");

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
						QVariant parmaValue = fourthFloorMap.value("value");
						int transformType = fourthFloorMap.value("transform_enum").toInt();
						QString uiUnit = fourthFloorMap.value("ui_unit").toString();
						bool visible = fourthFloorMap.value("visible").toBool();
						int highLimit = fourthFloorMap.value("highLimit").toInt();
						int lowLimit = fourthFloorMap.value("lowLimit").toInt();
						QVariant _defaultValue = fourthFloorMap.value("default");

						if (controlName == "SpinBox")
						{
							//p--labelName--identifyName-- categoryName--, value, uiUnit, transformType, visible, highLimit, lowLimit
							SpinBoxWidget_SKX *tempSpinBox = new SpinBoxWidget_SKX(nullptr, tempLabelName, identifyerName, categoryName, parmaValue.toInt(), _defaultValue, uiUnit, transformType, visible, highLimit, lowLimit);
							paramWidgetVector.value(type)->push_back(tempSpinBox);
							connect(tempSpinBox, &DoubleSpinBox_SKX::parameterChanged, [this]() {
								updateToFPGAButton->setEnabled(true);
							});

						}
						if (controlName == "DSpinBox" && (fourthFloorMap.value("transform_enum") > 0))
						{
							DoubleSpinBox_SKX *tempSpinBox =
								new DoubleSpinBox_SKX(nullptr, tempLabelName, identifyerName, categoryName, parmaValue.toDouble(), _defaultValue, uiUnit, transformType, visible, highLimit, lowLimit);
							paramWidgetVector.value(type)->push_back(tempSpinBox);

							connect(tempSpinBox, &DoubleSpinBox_SKX::parameterChanged, [this]() {
								updateToFPGAButton->setEnabled(true);
							});

						}
						else if (controlName == "DSpinBox")
						{

						}
						else if (controlName == "CheckBox")
						{

						}
						else if (controlName == "EnumUI")
						{

						}
						else if (controlName == "TextfieldUI" || controlName == "TextLabelUI")
						{

							TextLabelUI_SKX *tempLabelUI =
								new TextLabelUI_SKX(nullptr, tempLabelName, identifyerName, categoryName, parmaValue.toString(), _defaultValue);
							paramWidgetVector.value(type)->push_back(tempLabelUI);
						}
						//paramAttrib->setTransformType(fourthFloorMap.value("transform_enum"));
						if (fourthFloorMap.value("transform_enum").toInt() > 0)
						{
							//qDebug() << "JSON Data" << paramAttrib->name() << paramAttrib->transformType();
						}


					}
				}
			}
		}
	}

	//connect(this,SIGNAL)
}

void Setting3DP::updateTOFile()
{
	QString categoryName;// = item->getCategoryParam().toString();
	QString paramName;// = item->getNameParam().toString();
	QString paramValue;// = item->getValueParam().toString();
	QString ui_typeValue;// = item->getUi_typeParam().toString();	

	QVariantMap firstFloor;
	QVariantList secondFloorList;
	QVariantMap secondFloorMap;
	QVariantList thirdFloorList;
	QVariantMap thirdFloorMap;
	QVariantMap fourthFloorMap;

	QJsonDocument jsonDoc;
	QJsonParseError error;
	QString _jsonString;
	//ParamOp::getJsonFiletoString(paramJsonString, DMShopFilesystem::workingDir().filePath(DM_UI_JSON_FILE));
	ParamOp::getJsonFiletoString(_jsonString, PicaApplication::getRoamingDir() + "ParameterUI_STX.txt");// "C:/Users/TB495076/Documents/BCPware/ParameterUI_STX.txt");
	jsonDoc = QJsonDocument::fromJson(_jsonString.toUtf8(), &error);
	if (error.error == QJsonParseError::NoError)
	{
		firstFloor = jsonDoc.toVariant().toMap();
	}

	//********test count execute layers*********************//
	//QMapIterator<QString, QVariant> firstFloor_ii(jsonDoc.toVariant().toMap());

	foreach(QString categoryType, paramType){
		foreach(SKTWidget *widget, *paramWidgetVector.value(categoryType))
		{
			QMapIterator<QString, QVariant> firstFloor_ii(firstFloor);
			categoryName = widget->getCategoryName().toString();
			paramName = widget->getIdentifyName().toString();

			paramValue = widget->getValue().toString();
			qDebug() << "ParamSet_Value" << widget->getIdentifyName().toString() << widget->getValue().toString();// << item.transformType();

			//ui_typeValue = item.uiType().toString();
			bool valueSet = false;
			firstFloor_ii.toFront();
			while (firstFloor_ii.hasNext())
			{
				firstFloor_ii.next();
				if (firstFloor_ii.key() == "categories") {
					secondFloorList = firstFloor_ii.value().toList();
					foreach(QVariant secondFloorListValue, secondFloorList)
					{
						secondFloorMap = secondFloorListValue.toMap();
						if (secondFloorMap.value("name") == categoryName)
						{
							thirdFloorList = secondFloorMap.value("parameters").toList();
							foreach(QVariant thirdFloorListValue, thirdFloorList)
							{
								thirdFloorMap = thirdFloorListValue.toMap();
								if (thirdFloorMap.value("name") == paramName)
								{
									/*
									* Replace data in JsonDocument
									*/
									fourthFloorMap = thirdFloorMap.value("control").toMap();

									if (fourthFloorMap["name"] == "DSpinBox")
									{

										fourthFloorMap["value"] = paramValue;

										qDebug() << "fourthFloorMap" << fourthFloorMap["value"];
									}
									else
									{
										fourthFloorMap["value"] = paramValue;
										qDebug() << "fourthFloorMap" << fourthFloorMap["label_name"] << fourthFloorMap["value"];
									}
									//fourthFloorMap["default"] = paramValue;

									valueSet = true;

									thirdFloorMap.insert("control", fourthFloorMap);
									int thirdFloorlistIndex = thirdFloorList.indexOf(thirdFloorListValue);
									thirdFloorList.replace(thirdFloorlistIndex, thirdFloorMap);

									secondFloorMap.insert("parameters", thirdFloorList);
									int secondFloorlistIndex = secondFloorList.indexOf(secondFloorListValue);
									secondFloorList.replace(secondFloorlistIndex, secondFloorMap);

									firstFloor.insert("categories", secondFloorList);
									qDebug() << "\n";
									break;
								}

							}

						}
						if (valueSet)
						{
							break;
						}
					}
				}
			}
		}
	}


	QJsonDocument json = QJsonDocument::fromVariant(firstFloor);
	QString updateJson(json.toJson(QJsonDocument::Compact));
	bool test = ParamOp::saveJsonToFileWithPath(updateJson, PicaApplication::getRoamingDir() + "ParameterUI_STX.txt");
}
void Setting3DP::sendNVMPreProcess()
{
	/*if (comm == nullptr)
		qDebug() << "no Server";return*/
	CMD_Value *cmdvalue = new CMD_Value(comm);
	/*get NVM value from UI*/
	QVariantList NVMValueList;
	QVector<SKTWidget *> *tempVector = paramWidgetVector.value(NVM_SETTING_NAME);
	for (int i = 0, j = 0; i < tempVector->size(); i++)
	{
		SKTWidget *tempWidget = tempVector->at(i);
		NVMValueList.push_back(tempWidget->getValue());
		//glay->addWidget(temp,i,0);
	}
	cmdvalue->getValueFromUI(NVMValueList);
	bool result = cmdvalue->sendNVMCommand();
	//qDebug() << result;
	/*result update to file*/
	//if (true)
	if (result)
	{
		updateTOFile();
		updateToFPGAButton->setDisabled(true);
	}
	else
		return;



}

void Setting3DP::getNVMFromFPGA()
{
	CMD_Value *cmdvalue = new CMD_Value(comm);
	cmdvalue->getNVMValue();

	/*update to UI from JsonFile*/
	updateUIFromJsonFile();

}

bool Setting3DP::updateUIFromJsonFile(){

	QVariantList NVMValueList;
	QVariant category;

	QVariant advancedMap, firstList, paramList;
	ParamOp::extractVariantTest(category, QVariant(), QString(), -1, PicaApplication::getRoamingDir() + "ParameterUI_STX.txt");
	ParamOp::extractVariantTest(firstList, category, "categories");
	ParamOp::extractVariantTest(advancedMap, firstList, QString(), 1);

	ParamOp::extractVariantTest(paramList, advancedMap, "parameters");

	for (int i = 0; i < paramList.toList().size(); i++)
	{
		QVariant paramAttributeMap;
		QVariant paramName, paramControl;
		QVariant paramValue;
		ParamOp::extractVariantTest(paramAttributeMap, paramList, QString(), i);

		ParamOp::extractVariantTest(paramName, paramAttributeMap, "name");
		ParamOp::extractVariantTest(paramControl, paramAttributeMap, "control");

		ParamOp::extractVariantTest(paramValue, paramControl, "value");
		if (!paramName.isNull())
			NVMValueList.push_back(paramValue);

	}

	QVector<SKTWidget *> *tempVector = paramWidgetVector.value(NVM_SETTING_NAME);
	if (NVMValueList.size() == tempVector->size())
	{
		for (int i = 0, j = 0; i < tempVector->size(); i++)
		{
			SKTWidget *tempWidget = tempVector->at(i);
			tempWidget->setValue(NVMValueList.at(i));
			tempWidget->updateUIValue(tempWidget->getValue());
		}
	}
	else
	{
		return false;
	}

	return true;
}