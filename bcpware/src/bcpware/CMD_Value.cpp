#include "CMD_Value.h"

static std::size_t Checksum_Cal(unsigned short *addr, std::size_t count);

CMD_Value::CMD_Value(Comm3DP *_comm, QObject *parent) : QObject(parent)/*, comm(_comm)*/, comm(_comm)
{

	ParamOp::getJsonFiletoString(jsonInputString, PicaApplication::getRoamingDir() + "ParameterUI_STX.txt");
	gatherAdvanced_SettingName();
	init();

	/*
	*/
	//getNVMValue();

	/*===========================================*/

}
/*--------------------------------------------------------------------------------------------------------------------*/




CMD_Value::CMD_Value() : QObject()
{

	ParamOp::getJsonFiletoString(jsonInputString, PicaApplication::getRoamingDir() + "ParameterUI_STX.txt");
	init();
}
/*--------------------------------------------------------------------------------------------------------------------*/

void CMD_Value::initDesMap()
{
	//TBD
	/*CMD_des_string.clear();
	CMD_des_string.insert(CMD_Description::CMD_Printhead_Fan, "");
	CMD_des_string.insert(CMD_Description::CMD_Vibration, "");
	CMD_des_string.insert(CMD_Description::CMD_Builder_Heat, "");
	CMD_des_string.insert(CMD_Description::CMD_Preheat_Params, "");
	CMD_des_string.insert(CMD_Description::CMD_Postheat_Params, "");
	CMD_des_string.insert(CMD_Description::SIRIUS_WiperFrequency, "");
	CMD_des_string.insert(CMD_Description::CMD_WiperIndex, "");
	CMD_des_string.insert(CMD_Description::CMD_WiperClicks, "");
	CMD_des_string.insert(CMD_Description::CMD_PrintMode, "");
	CMD_des_string.insert(CMD_Description::CMD_Printhead_Fan, "");
	CMD_des_string.insert(CMD_Description::CMD_Printhead_Fan, "");
	CMD_des_string.insert(CMD_Description::CMD_Printhead_Fan, "");
	CMD_des_string.insert(CMD_Description::CMD_Printhead_Fan, "");*/

}
/*--------------------------------------------------------------------------------------------------------------------*/

bool CMD_Value::getValueFromUI(QVariantList inputList)
{
	qDebug() << inputList;
	QVector<QVariantList> nvmValueList(5);
	//QVariantList nvmValueList[5];
	for (int i = 0; i < inputList.size(); i++)
	{
		if (i < NUMOFNVM1) {
			nvmValueList[0].append(inputList.at(i));
		}
		else if (i < NUMOFNVM1 + NUMOFNVM2) {
			nvmValueList[1].append(inputList.at(i));
		}
		else if (i < (NUMOFNVM1 + NUMOFNVM2 + NUMOFNVM3)) {
			nvmValueList[2].append(inputList.at(i));
		}
		else if (i < (NUMOFNVM1 + NUMOFNVM2 + NUMOFNVM3 + NUMOFNVM4)) {
			nvmValueList[3].append(inputList.at(i));

		}
		else if (i < (NUMOFNVM1 + NUMOFNVM2 + NUMOFNVM3 + NUMOFNVM4 + NUMOFNVM5)) {
			nvmValueList[4].append(inputList.at(i));
		}
	}

	/*
	count check sum
	add check sum to end of numValueList
	*/
	for (int listNum = 0; listNum < 5; listNum++)
	{
		unsigned char *iobuf = nullptr;
		iobuf = new unsigned char[4000000];
		memset(iobuf, 0x00, 4000000);
		unsigned char *ptr = iobuf;
		std::size_t count = 0;
		std::size_t varNumCount2 = 0;

		if (listNum == 0) {
			foreach(QVariant var, nvmValueList[0])
			{
				if (varNumCount2 == 5 || varNumCount2 == 6)
				{
					int tempint = var.toInt();
					unsigned char * pucTemporary = reinterpret_cast<unsigned char *>(&tempint);

					*ptr = pucTemporary[0]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;
					*ptr = pucTemporary[1]; /*qDebug() << "Value : " << hex << *ptr;*/  ptr++;
					*ptr = pucTemporary[2]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;
					*ptr = pucTemporary[3]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;

					count += 4;
					varNumCount2++;
				}
				else
				{
					std::string tempString = var.toString().toStdString();
					std::copy(tempString.begin(), tempString.end(), ptr);
					//qDebug() << "tempString.size(): " << tempString.size();
					if (tempString.size() % 2 > 0)
					{
						ptr++; count++;
						ptr += tempString.size();
						count += tempString.size();
					}
					else
					{
						ptr += tempString.size();
						count += tempString.size();
					}
					varNumCount2++;
				}
			}
			std::size_t xsx = Checksum_Cal(reinterpret_cast<unsigned short*>(iobuf), count);
			QString hexvalue = QString("%1").arg(xsx, 4, 16, QLatin1Char('0')).toUpper().right(8);
			nvmValueList[0].append(hexvalue);
			qDebug() << hexvalue;
		}
		else if (listNum == 1)
		{
			foreach(QVariant var, nvmValueList[1])
			{
				int tempint = var.toInt();
				unsigned char * pucTemporary = reinterpret_cast<unsigned char *>(&tempint);

				*ptr = pucTemporary[0]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;
				*ptr = pucTemporary[1]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;
				*ptr = pucTemporary[2]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;
				*ptr = pucTemporary[3]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;

				count += 4;
				varNumCount2++;
			}
			size_t xsx = Checksum_Cal(reinterpret_cast<unsigned short*>(iobuf), count);
			QString hexvalue = QString("%1").arg(xsx, 4, 16, QLatin1Char('0')).toUpper().right(8);
			nvmValueList[1].append(hexvalue);
			qDebug() << hexvalue;
		}
		else if (listNum == 2)
		{
			foreach(QVariant var, nvmValueList[2])
			{
				int tempint = var.toInt();
				unsigned char * pucTemporary = reinterpret_cast<unsigned char *>(&tempint);
				*ptr = pucTemporary[0]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;
				*ptr = pucTemporary[1]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;
				*ptr = pucTemporary[2]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;
				*ptr = pucTemporary[3]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;
				count += 4;
				varNumCount2++;
			}
			size_t xsx = Checksum_Cal(reinterpret_cast<unsigned short*>(iobuf), count);
			QString hexvalue = QString("%1").arg(xsx, 4, 16, QLatin1Char('0')).toUpper().right(8);
			nvmValueList[2].append(hexvalue);
			qDebug() << hexvalue;
		}
		else if (listNum == 3)
		{
			foreach(QVariant var, nvmValueList[3])
			{
				if (varNumCount2 == 13 || varNumCount2 == 14 || varNumCount2 == 15)
				{
					std::string tempString = var.toString().toStdString();
					std::copy(tempString.begin(), tempString.end(), ptr);

					//qDebug() << "tempString.size(): " << tempString.size();
					if (tempString.size() % 2 > 0) {
						ptr++; count++;
						ptr += tempString.size();
						count += tempString.size();
					}
					else {
						ptr += tempString.size();
						count += tempString.size();
					}
					varNumCount2++;

				}
				else {
					int tempint = var.toInt();
					unsigned char * pucTemporary = reinterpret_cast<unsigned char *>(&tempint);

					*ptr = pucTemporary[0]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;
					*ptr = pucTemporary[1]; /*qDebug() << "Value : " << hex << *ptr;*/  ptr++;
					*ptr = pucTemporary[2]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;
					*ptr = pucTemporary[3]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;

					count += 4;
					varNumCount2++;

				}
			}
			size_t xsx = Checksum_Cal(reinterpret_cast<unsigned short*>(iobuf), count);
			QString hexvalue = QString("%1").arg(xsx, 4, 16, QLatin1Char('0')).toUpper().right(8);
			nvmValueList[3].append(hexvalue);
			qDebug() << hexvalue;
		}
		else if (listNum == 4) {
			foreach(QVariant var, nvmValueList[4])
			{
				int tempint = var.toInt();
				unsigned char * pucTemporary = reinterpret_cast<unsigned char *>(&tempint);

				*ptr = pucTemporary[0]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;
				*ptr = pucTemporary[1]; /*qDebug() << "Value : " << hex << *ptr;*/  ptr++;
				*ptr = pucTemporary[2]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;
				*ptr = pucTemporary[3]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;

				count += 4;
				varNumCount2++;
			}
			size_t xsx = Checksum_Cal(reinterpret_cast<unsigned short*>(iobuf), count);
			QString hexvalue = QString("%1").arg(xsx, 4, 16, QLatin1Char('0')).toUpper().right(8);
			nvmValueList[4].append(hexvalue);
			qDebug() << hexvalue;
		}
	}



	CMD_valueMap.insert(CMD_Description::CMD_NVM_R1, *new CMD_value("$NVMR1", QVariantList(), CMD_value::CMD_TYPE::NVM_Reading));
	CMD_valueMap.insert(CMD_Description::CMD_NVM_W1, *new CMD_value("$NVMW1", nvmValueList[0], CMD_value::CMD_TYPE::NVM_Setting));
	CMD_valueMap.insert(CMD_Description::CMD_NVM_R2, *new CMD_value("$NVMR2", QVariantList(), CMD_value::CMD_TYPE::NVM_Reading));
	CMD_valueMap.insert(CMD_Description::CMD_NVM_W2, *new CMD_value("$NVMW2", nvmValueList[1], CMD_value::CMD_TYPE::NVM_Setting));
	CMD_valueMap.insert(CMD_Description::CMD_NVM_R3, *new CMD_value("$NVMR3", QVariantList(), CMD_value::CMD_TYPE::NVM_Reading));
	CMD_valueMap.insert(CMD_Description::CMD_NVM_W3, *new CMD_value("$NVMW3", nvmValueList[2], CMD_value::CMD_TYPE::NVM_Setting));
	CMD_valueMap.insert(CMD_Description::CMD_NVM_R4, *new CMD_value("$NVMR4", QVariantList(), CMD_value::CMD_TYPE::NVM_Reading));
	CMD_valueMap.insert(CMD_Description::CMD_NVM_W4, *new CMD_value("$NVMW4", nvmValueList[3], CMD_value::CMD_TYPE::NVM_Setting));
	CMD_valueMap.insert(CMD_Description::CMD_NVM_R5, *new CMD_value("$NVMR5", QVariantList(), CMD_value::CMD_TYPE::NVM_Reading));
	CMD_valueMap.insert(CMD_Description::CMD_NVM_W5, *new CMD_value("$NVMW5", nvmValueList[4], CMD_value::CMD_TYPE::NVM_Setting));






	return true;
}




void CMD_Value::init()
{
	CMD_valueMap.clear();
	/*@FanSet*/
	QVariant temp1, temp2, temp3, temp4, temp5;
	getValuebyName("Basic_Setting", "Printhead_Fan_1", temp1);
	getValuebyName("Basic_Setting", "Printhead_Fan_2", temp2);
	getValuebyName("Basic_Setting", "Printhead_Fan_3", temp3);
	getValuebyName("Basic_Setting", "Printhead_Fan_4", temp4);
	getValuebyName("Basic_Setting", "Printhead_Fan_5", temp5);
	CMD_valueMap.insert(CMD_Description::CMD_Printhead_Fan, *new CMD_value("@FANSET", QVariantList() << temp1 << temp2 << temp3 << temp4 << temp5, CMD_value::CMD_TYPE::Board_Param));

	/*Vibration */
	getValuebyName("Basic_Setting", "Roller_Vibration", temp1);
	if (temp1.toBool())
		CMD_valueMap.insert(CMD_Description::CMD_Vibration, *new CMD_value("VIB_ON", QVariantList(), CMD_value::CMD_TYPE::Board_Param));
	else
		CMD_valueMap.insert(CMD_Description::CMD_Vibration, *new CMD_value("VIB_OFF", QVariantList(), CMD_value::CMD_TYPE::Board_Param));
	/*Vibration Value*/
	getValuebyName("Basic_Setting", "Roller_Vibration_Percent", temp1);
	CMD_valueMap.insert(CMD_Description::CMD_Vibration_Value, *new CMD_value("VIB_SET", QVariantList() << temp1, CMD_value::CMD_TYPE::Board_Param));


	//CMD_valueMap.insert(CMD_Description::CMD_Vibration, *new CMD_value("VIB_ON", QVariantList(), CMD_value::CMD_TYPE::Board_Param));
	//CMD_valueMap.insert(CMD_Description::CMD_Vibration_Value, *new CMD_value("VIB_SET", QVariantList() << 500, CMD_value::CMD_TYPE::Board_Param));

	/*TRA IR*/
	getValuebyName("Basic_Setting", "Builder_Heating", temp1);
	if (temp1.toBool())
		CMD_valueMap.insert(CMD_Description::CMD_Builder_Heat, *new CMD_value("TRA_ON", QVariantList(), CMD_value::CMD_TYPE::Board_Param));
	else
		CMD_valueMap.insert(CMD_Description::CMD_Builder_Heat, *new CMD_value("TRA_OFF", QVariantList(), CMD_value::CMD_TYPE::Board_Param));
	/*Pre Heating*/
	getValuebyName("Basic_Setting", "Pre_Heating_Minutes", temp1);
	getValuebyName("Basic_Setting", "Pre_Heating_Temperture", temp2);
	getValuebyName("Basic_Setting", "Pre_Heating_High_Temperture", temp3);
	getValuebyName("Basic_Setting", "Pre_Heating_Low_Temperture", temp4);
	CMD_valueMap.insert(CMD_Description::CMD_Preheat_Params, *new CMD_value("@PRH", QVariantList() << temp1 << temp2 << temp3 << temp4, CMD_value::CMD_TYPE::Board_Param));
	/*Post Heating*/
	getValuebyName("Basic_Setting", "Post_Heating_Minutes", temp1);
	getValuebyName("Basic_Setting", "Post_Heating_High_Temperture", temp2);
	getValuebyName("Basic_Setting", "Post_Heating_Low_Temperture", temp3);
	CMD_valueMap.insert(CMD_Description::CMD_Postheat_Params, *new CMD_value("@AFP", QVariantList() << temp1 << temp2 << temp3, CMD_value::CMD_TYPE::Board_Param));

	/*Wiper Frequency Index, Clikck*/
	getValuebyName("Basic_Setting", "MidJob_Frequency", temp1);
	CMD_valueMap.insert(CMD_Description::SIRIUS_WiperFrequency, *new CMD_value("Sirius", QVariantList() << temp1, CMD_value::CMD_TYPE::Board_Param));

	getValuebyName("Basic_Setting", "Wipe_Index", temp1);
	CMD_valueMap.insert(CMD_Description::CMD_WiperIndex, *new CMD_value("@IDX", QVariantList() << temp1, CMD_value::CMD_TYPE::Board_Param));

	getValuebyName("Basic_Setting", "Wipe_Click", temp1);
	CMD_valueMap.insert(CMD_Description::CMD_WiperClicks, *new CMD_value("@CLKS", QVariantList() << temp1, CMD_value::CMD_TYPE::Board_Param));

	CMD_valueMap.insert(CMD_Description::CMD_NORMAL_PRINT, *new CMD_value("@FPR", QVariantList(), CMD_value::CMD_TYPE::PrintModeParam));
	CMD_valueMap.insert(CMD_Description::CMD_REVERSE_PRINT, *new CMD_value("@RPR", QVariantList(), CMD_value::CMD_TYPE::PrintModeParam));
	CMD_valueMap.insert(CMD_Description::CMD_NORMAL_AND_REVERSE_PRINT, *new CMD_value("@BPR", QVariantList(), CMD_value::CMD_TYPE::PrintModeParam));


	/*create long Header year, month, day, hour, minute, second*/
	/*create long Header page, layer, form, DQA_Flag */
	CMD_valueMap.insert(CMD_Description::CMD_LONG_PAGE_HEADER_FOR_ONE_PAGE, *new CMD_value("@HEAD", QVariantList() << "2019" << "12" << "12" << "13" << "59" << "59" <<
		"1" << "500" << "L" << "0" << "0", CMD_value::CMD_TYPE::Header_Param));
	/*create short Header year month day hour minute second*/
	CMD_valueMap.insert(CMD_Description::CMD_SHORT_PAGE_HEADER_FOR_ONE_PAGE, *new CMD_value("@HEAD", QVariantList() << "2019" << "12" << "12" << "13" << "59" << "59" <<
		"1" << "10" << "S" << "0" << "0", CMD_value::CMD_TYPE::Header_Param));

	CMD_valueMap.insert(CMD_Description::CMD_QUEEN_START, *new CMD_value("M_DMG", QVariantList(), CMD_value::CMD_TYPE::Motion));

	CMD_valueMap.insert(CMD_Description::CMD_QUEEN_STOP, *new CMD_value("M_DME", QVariantList(), CMD_value::CMD_TYPE::Motion));




	//CMD_valueMap.insert(CMD_Description::CMD_Vibration_Value, *new CMD_value("VIB_SET", QVariantList() << temp1, CMD_value::CMD_TYPE::Board_Param));
	/*PRINT MODE*/
	getValuebyName("Basic_Setting", "Print_Mode", temp1);
	if (temp1.toString() == "1Layer2Page")
		CMD_valueMap.insert(CMD_Description::CMD_PrintMode, *new CMD_value("@NPM", QVariantList(), CMD_value::CMD_TYPE::Board_Param));
	else
		CMD_valueMap.insert(CMD_Description::CMD_PrintMode, *new CMD_value("@FPM", QVariantList(), CMD_value::CMD_TYPE::Board_Param));

	getValuebyName("Basic_Setting", "Fill_Powder_Ratio", temp1);
	CMD_valueMap.insert(CMD_Description::CMD_FILL_POWDER_RATIO_DURING_PRINTING, *new CMD_value("S_RATIO", QVariantList() << temp1, CMD_value::CMD_TYPE::Board_Param));//FIll Bed Ratio During Printing

	CMD_valueMap.insert(CMD_Description::CMD_FP_BUILDER_UP, *new CMD_value("Z_BUM", QVariantList(), CMD_value::CMD_TYPE::Motion));
	CMD_valueMap.insert(CMD_Description::CMD_FP_BUILDER_DOWN, *new CMD_value("Z_BDM", QVariantList(), CMD_value::CMD_TYPE::Motion));
	CMD_valueMap.insert(CMD_Description::CMD_FP_FEEDER_UP, *new CMD_value("Z_PUM", QVariantList(), CMD_value::CMD_TYPE::Motion));
	CMD_valueMap.insert(CMD_Description::CMD_FP_FEEDER_DOWN, *new CMD_value("Z_PDM", QVariantList(), CMD_value::CMD_TYPE::Motion));

	CMD_valueMap.insert(CMD_Description::CMD_FP_BUILDER_UP_ONE_MM, *new CMD_value("Z_BUN", QVariantList() << "1", CMD_value::CMD_TYPE::Motion));
	CMD_valueMap.insert(CMD_Description::CMD_FP_BUILDER_DOWN_ONE_MM, *new CMD_value("Z_BDN", QVariantList() << "1", CMD_value::CMD_TYPE::Motion));
	CMD_valueMap.insert(CMD_Description::CMD_FP_FEEDER_UP_ONE_MM, *new CMD_value("Z_PUN", QVariantList() << "1", CMD_value::CMD_TYPE::Motion));
	CMD_valueMap.insert(CMD_Description::CMD_FP_FEEDER_DOWN_ONE_MM, *new CMD_value("Z_PDN", QVariantList() << "1", CMD_value::CMD_TYPE::Motion));

	/*CMD_valueMap.insert(CMD_Description::CMD_FP_BUILDER_UP_VALUE, *new CMD_value("Z_PUN", QVariantList(), CMD_value::CMD_TYPE::Motion));
	CMD_valueMap.insert(CMD_Description::CMD_FP_BUILDER_DOWN_VALUE, *new CMD_value("Z_PDN", QVariantList(), CMD_value::CMD_TYPE::Motion));
	CMD_valueMap.insert(CMD_Description::CMD_FP_FEEDER_UP_VALUE, *new CMD_value("Z_BUN", QVariantList(), CMD_value::CMD_TYPE::Motion));
	CMD_valueMap.insert(CMD_Description::CMD_FP_FEEDER_DOWN_VALUE, *new CMD_value("Z_BDN", QVariantList(), CMD_value::CMD_TYPE::Motion));*/

	CMD_valueMap.insert(CMD_Description::CMD_FP_Z_AXIS_STOP, *new CMD_value("Z_MSP", QVariantList(), CMD_value::CMD_TYPE::Motion));
	CMD_valueMap.insert(CMD_Description::CMD_FP_FILL_POWDER_START, *new CMD_value("M_AFB", QVariantList(), CMD_value::CMD_TYPE::Motion));
	CMD_valueMap.insert(CMD_Description::CMD_FP_FILL_POWDER_END, *new CMD_value("M_AFE", QVariantList()));

	CMD_valueMap.insert(CMD_Description::CMD_FP_CLEAN_PH_MODULE_START, *new CMD_value("L_CMA", QVariantList(), CMD_value::CMD_TYPE::Motion));
	CMD_valueMap.insert(CMD_Description::CMD_FP_CLEAN_PH_MODULE_END, *new CMD_value("L_CMB", QVariantList(), CMD_value::CMD_TYPE::Motion));
	CMD_valueMap.insert(CMD_Description::CMD_FP_CLEAN_ROLLER_START, *new CMD_value("L_CRA", QVariantList(), CMD_value::CMD_TYPE::Motion));
	CMD_valueMap.insert(CMD_Description::CMD_FP_CLEAN_PH_ROLLER_BACK, *new CMD_value("L_CRB", QVariantList(), CMD_value::CMD_TYPE::Motion));

	CMD_valueMap.insert(CMD_Description::CMD_FP_ROLLER_MOVE_MIDDLE, *new CMD_value("L_APA", QVariantList(), CMD_value::CMD_TYPE::Motion));
	CMD_valueMap.insert(CMD_Description::CMD_FP_ROLLER_MOVE_MIDDLE_END, *new CMD_value("L_APB", QVariantList(), CMD_value::CMD_TYPE::Motion));

	CMD_valueMap.insert(CMD_Description::CMD_FP_FEEDER_DOWN_TO_PRINT_PAGE_POSITION, *new CMD_value("Z_BDN", QVariantList(), CMD_value::CMD_TYPE::Motion));
	CMD_valueMap.insert(CMD_Description::CMD_FP_CHANGE_WIPER, *new CMD_value("L_CWP", QVariantList(), CMD_value::CMD_TYPE::Motion));
	CMD_valueMap.insert(CMD_Description::CMD_FP_MOVE_ITEM_TRAY_STEP_ONE, *new CMD_value("M_RMM", QVariantList(), CMD_value::CMD_TYPE::Motion));
	CMD_valueMap.insert(CMD_Description::CMD_FP_MOVE_ITEM_TRAY_STEP_TWO, *new CMD_value("M_RMB", QVariantList(), CMD_value::CMD_TYPE::Motion));

	CMD_valueMap.insert(CMD_Description::CMD_CANCEL_CURING_END, *new CMD_value("@DRYEND", QVariantList(), CMD_value::CMD_TYPE::Motion));
	CMD_valueMap.insert(CMD_Description::CMD_DOOR_LATCH, *new CMD_value("D_LATCH", QVariantList(), CMD_value::CMD_TYPE::Motion));

	CMD_valueMap.insert(CMD_Description::CMD_ENTER, *new CMD_value("\r", QVariantList(), CMD_value::CMD_TYPE::Board_Param));



	/*CMD_valueMap.insert(CMD_Description::CMD_DOOR_SPACE, *new CMD_value("@HEAD", QVariantList(), CMD_value::CMD_TYPE::Board_Param));
	CMD_valueMap.insert(CMD_Description::CMD_DOOR_SPACE1, *new CMD_value("1", QVariantList(), CMD_value::CMD_TYPE::Board_Param));
	CMD_valueMap.insert(CMD_Description::CMD_DOOR_SPACE2, *new CMD_value("10", QVariantList(), CMD_value::CMD_TYPE::Board_Param));
	CMD_valueMap.insert(CMD_Description::CMD_DOOR_SPACE3, *new CMD_value("S", QVariantList(), CMD_value::CMD_TYPE::Board_Param));
	CMD_valueMap.insert(CMD_Description::CMD_DOOR_SPACE4, *new CMD_value("0", QVariantList(), CMD_value::CMD_TYPE::Board_Param));
	CMD_valueMap.insert(CMD_Description::CMD_DOOR_SPACE5, *new CMD_value("0", QVariantList(), CMD_value::CMD_TYPE::Board_Param));*/




	/*
	NVM Setting
	need to transform back to integer TBD
	*/

	//QVariantList nvm1List, nvm2List, nvm3List, nvm4List, nvm5List;
	QVariantList nvmValueList[5];
	for (int i = 0; i < 5; i++)
	{
		foreach(QVariant temp, NVM_NameList[i])
		{
			nvmValueList[i].append(getValuebyName2("Advanced_Setting", temp.toString()));
		}
	}

	/*
	count check sum
	add check sum to end of numValueList
	*/
	for (int listNum = 0; listNum < 5; listNum++)
	{
		unsigned char *iobuf = nullptr;
		iobuf = new unsigned char[4000000];
		memset(iobuf, 0x00, 4000000);
		unsigned char *ptr = iobuf;
		std::size_t count = 0;
		std::size_t varNumCount2 = 0;

		if (listNum == 0) {
			foreach(QVariant var, nvmValueList[0])
			{
				if (varNumCount2 == 5 || varNumCount2 == 6)
				{
					int tempint = var.toInt();
					unsigned char * pucTemporary = reinterpret_cast<unsigned char *>(&tempint);

					*ptr = pucTemporary[0]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;
					*ptr = pucTemporary[1]; /*qDebug() << "Value : " << hex << *ptr;*/  ptr++;
					*ptr = pucTemporary[2]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;
					*ptr = pucTemporary[3]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;

					count += 4;
					varNumCount2++;
				}
				else
				{
					std::string tempString = var.toString().toStdString();
					std::copy(tempString.begin(), tempString.end(), ptr);
					//qDebug() << "tempString.size(): " << tempString.size();
					if (tempString.size() % 2 > 0)
					{
						ptr++; count++;
						ptr += tempString.size();
						count += tempString.size();
					}
					else
					{
						ptr += tempString.size();
						count += tempString.size();
					}
					varNumCount2++;
				}
			}
			std::size_t xsx = Checksum_Cal(reinterpret_cast<unsigned short*>(iobuf), count);
			QString hexvalue = QString("%1").arg(xsx, 4, 16, QLatin1Char('0')).toUpper().right(8);
			nvmValueList[0].append(hexvalue);
			qDebug() << hexvalue;
		}
		else if (listNum == 1)
		{
			foreach(QVariant var, nvmValueList[1])
			{
				int tempint = var.toInt();
				unsigned char * pucTemporary = reinterpret_cast<unsigned char *>(&tempint);

				*ptr = pucTemporary[0]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;
				*ptr = pucTemporary[1]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;
				*ptr = pucTemporary[2]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;
				*ptr = pucTemporary[3]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;

				count += 4;
				varNumCount2++;
			}
			size_t xsx = Checksum_Cal(reinterpret_cast<unsigned short*>(iobuf), count);
			QString hexvalue = QString("%1").arg(xsx, 4, 16, QLatin1Char('0')).toUpper().right(8);
			nvmValueList[1].append(hexvalue);
			qDebug() << hexvalue;
		}
		else if (listNum == 2)
		{
			foreach(QVariant var, nvmValueList[2])
			{
				int tempint = var.toInt();
				unsigned char * pucTemporary = reinterpret_cast<unsigned char *>(&tempint);
				*ptr = pucTemporary[0]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;
				*ptr = pucTemporary[1]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;
				*ptr = pucTemporary[2]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;
				*ptr = pucTemporary[3]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;
				count += 4;
				varNumCount2++;
			}
			size_t xsx = Checksum_Cal(reinterpret_cast<unsigned short*>(iobuf), count);
			QString hexvalue = QString("%1").arg(xsx, 4, 16, QLatin1Char('0')).toUpper().right(8);
			nvmValueList[2].append(hexvalue);
			qDebug() << hexvalue;
		}
		else if (listNum == 3)
		{
			foreach(QVariant var, nvmValueList[3])
			{
				if (varNumCount2 == 13 || varNumCount2 == 14 || varNumCount2 == 15)
				{
					std::string tempString = var.toString().toStdString();
					std::copy(tempString.begin(), tempString.end(), ptr);

					//qDebug() << "tempString.size(): " << tempString.size();
					if (tempString.size() % 2 > 0) {
						ptr++; count++;
						ptr += tempString.size();
						count += tempString.size();
					}
					else {
						ptr += tempString.size();
						count += tempString.size();
					}
					varNumCount2++;

				}
				else {
					int tempint = var.toInt();
					unsigned char * pucTemporary = reinterpret_cast<unsigned char *>(&tempint);

					*ptr = pucTemporary[0]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;
					*ptr = pucTemporary[1]; /*qDebug() << "Value : " << hex << *ptr;*/  ptr++;
					*ptr = pucTemporary[2]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;
					*ptr = pucTemporary[3]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;

					count += 4;
					varNumCount2++;

				}
			}
			size_t xsx = Checksum_Cal(reinterpret_cast<unsigned short*>(iobuf), count);
			QString hexvalue = QString("%1").arg(xsx, 4, 16, QLatin1Char('0')).toUpper().right(8);
			nvmValueList[3].append(hexvalue);
			qDebug() << hexvalue;
		}
		else if (listNum == 4) {
			foreach(QVariant var, nvmValueList[4])
			{
				int tempint = var.toInt();
				unsigned char * pucTemporary = reinterpret_cast<unsigned char *>(&tempint);

				*ptr = pucTemporary[0]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;
				*ptr = pucTemporary[1]; /*qDebug() << "Value : " << hex << *ptr;*/  ptr++;
				*ptr = pucTemporary[2]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;
				*ptr = pucTemporary[3]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;

				count += 4;
				varNumCount2++;
			}
			size_t xsx = Checksum_Cal(reinterpret_cast<unsigned short*>(iobuf), count);
			QString hexvalue = QString("%1").arg(xsx, 4, 16, QLatin1Char('0')).toUpper().right(8);
			nvmValueList[4].append(hexvalue);
			qDebug() << hexvalue;
		}
		free(iobuf);
		iobuf = nullptr;
	}

	qDebug() << "test_exception 1";

	//foreach(QVariant var, nvmValueList[0])
	//{
	//	if (varNumCount2 == 5 || varNumCount2 == 6)
	//	{
	//		int tempint = var.toInt();
	//		unsigned short *tempshort = (unsigned short *)&tempint;
	//		//memcpy(ptr, (unsigned char*)&tempint, sizeof(int));
	//		//std::copy((unsigned char*)&tempint, *((unsigned char*)(&tempint) + 1), ptr);
	//		*ptr = *((unsigned char*)&tempint); qDebug() << "Value : " << hex << *ptr; ptr++;
	//		*ptr = *((unsigned char*)(&tempint) + 1); qDebug() << "Value : " << hex << *ptr;  ptr++;
	//		*ptr = *((unsigned char*)(&tempint) + 2); qDebug() << "Value : " << hex << *ptr; ptr++;
	//		*ptr = *((unsigned char*)(&tempint) + 3); qDebug() << "Value : " << hex << *ptr; ptr++;
	//		count += 4;
	//		varNumCount2++;
	//	}
	//	else {
	//		std::string tempString = var.toString().toStdString();
	//		std::copy(tempString.begin(), tempString.end(), ptr);
	//		//qDebug() << "tempString.size(): " << tempString.size();
	//		if (tempString.size() % 2 > 0) {
	//			ptr++; count++;
	//			ptr += tempString.size();
	//			count += tempString.size();
	//		}
	//		else {
	//			ptr += tempString.size();
	//			count += tempString.size();
	//		}
	//		varNumCount2++;
	//	}
	//}
	//int xsx = Checksum_Cal((unsigned short*)iobuf, count);

	//qDebug() << QString::number(xsx, 16);
	//qDebug() << "Value : " << hex << xsx;





	/*nvm1List.append(getValuebyName2("Advanced_Setting", "MODEL_NAME"));
	nvm1List.append(getValuebyName2("Advanced_Setting", "MODEL_VERSION"));
	nvm1List.append(getValuebyName2("Advanced_Setting", "MACHINE_SERIAL_NUMBER"));
	nvm1List.append(getValuebyName2("Advanced_Setting", "FPGA_FW_VERSION"));
	nvm1List.append(getValuebyName2("Advanced_Setting", "SIRIUS_FW_VERSION"));
	nvm1List.append(getValuebyName2("Advanced_Setting", "FPGA_POF_CHECKSUM"));
	nvm1List.append(getValuebyName2("Advanced_Setting", "FPGA_POF_USERCODE"));

	nvm2List.append(getValuebyName2("Advanced_Setting", "PRINT_POSITION_SHORT_X1"));
	nvm2List.append(getValuebyName2("Advanced_Setting", "PRINT_POSITION_LONG_X1"));
	nvm2List.append(getValuebyName2("Advanced_Setting", "HOME_POSITION_X1"));
	nvm2List.append(getValuebyName2("Advanced_Setting", "WIPE_POSITION_X1"));
	nvm2List.append(getValuebyName2("Advanced_Setting", "SPIT_POSITION_X1"));
	nvm2List.append(getValuebyName2("Advanced_Setting", "END_POSITION_OFFSET_X1"));
	nvm2List.append(getValuebyName2("Advanced_Setting", "END_POSITION_OFFSET_X2"));
	nvm2List.append(getValuebyName2("Advanced_Setting", "HOME_POSITION_OFFSET_X1"));
	nvm2List.append(getValuebyName2("Advanced_Setting", "HOME_POSITION_OFFSET_X2"));
	nvm2List.append(getValuebyName2("Advanced_Setting", "Z1_MAX_STEP"));
	nvm2List.append(getValuebyName2("Advanced_Setting", "Z2_MAX_STEP"));
	nvm2List.append(getValuebyName2("Advanced_Setting", "Z1_MAX_POS"));
	nvm2List.append(getValuebyName2("Advanced_Setting", "Z2_MAX_POS"));
	nvm2List.append(getValuebyName2("Advanced_Setting", "Z1_MIN_POS"));
	nvm2List.append(getValuebyName2("Advanced_Setting", "Z2_MIN_POS"));

	nvm3List.append(getValuebyName2("Advanced_Setting", "INITIAL_FREQUENCY_Z1"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "FINAL_FREQUENCY_Z1"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "STEP_TO_ACCELERATE_Z1"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "STEP_TO_DECELERATE_Z1"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "INITIAL_FREQUENCY_Z2"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "FINAL_FREQUENCY_Z2"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "STEP_TO_ACCELERATE_Z2"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "STEP_TO_DECELERATE_Z2"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "INITIAL_FREQUENCY_ROLLER"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "FINAL_FREQUENCY_ROLLER"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "STEP_TO_ACCELERATE_ROLLER"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "STEP_TO_DECELERATE_ROLLER"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "INITIAL_FREQUENCY_HOME_X1"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "FINAL_FREQUENCY_HOME_X1"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "STEP_TO_ACCELERATE_HOME_X1"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "STEP_TO_DECELERATE_HOME_X1"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "INITIAL_FREQUENCY_OUT_X1"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "FINAL_FREQUENCY_OUT_X1"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "STEP_TO_ACCELERATE_OUT_X1"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "STEP_TO_DECELERATE_OUT_X1"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "INITIAL_FREQUENCY_HOME_X2"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "FINAL_FREQUENCY_HOME_X2"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "STEP_TO_ACCELERATE_HOME_X2"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "STEP_TO_DECELERATE_HOME_X2"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "INITIAL_FREQUENCY_OUT_X2"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "FINAL_FREQUENCY_OUT_X2"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "STEP_TO_ACCELERATE_OUT_X2"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "STEP_TO_DECELERATE_OUT_X2"));

	nvm3List.append(getValuebyName2("Advanced_Setting", "TARGET_FREQUENCY_Z1_UP"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "TARGET_FREQUENCY_Z1_DOWN"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "TARGET_FREQUENCY_Z2_UP"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "TARGET_FREQUENCY_Z2_DOWN"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "TARGET_FREQUENCY_ROLLER_POWDER"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "TARGET_FREQUENCY_ROLLER_REV"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "TARGET_FREQUENCY_ROLLER_STOP"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "TARGET_FREQUENCY_X1_HOME"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "TARGET_FREQUENCY_X1_OUT"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "TARGET_FREQUENCY_X1_WIPE_HOME"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "TARGET_FREQUENCY_X1_WIPE_OUT"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "TARGET_FREQUENCY_X1_SLOW_HOME"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "TARGET_FREQUENCY_X1_SLOW_OUT"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "TARGET_FREQUENCY_X2_HOME"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "TARGET_FREQUENCY_X2_OUT"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "TARGET_FREQUENCY_X2_SLOW_HOME"));
	nvm3List.append(getValuebyName2("Advanced_Setting", "TARGET_FREQUENCY_X2_SLOW_OUT"));

	nvm4List.append(getValuebyName2("Advanced_Setting", "USER_INSTALL_MACHINE_DATA_PH"));
	nvm4List.append(getValuebyName2("Advanced_Setting", "TOTAL_POWER_ON_TIME"));
	nvm4List.append(getValuebyName2("Advanced_Setting", "TOTAL_PRINT_TIME"));
	nvm4List.append(getValuebyName2("Advanced_Setting", "SIRIUS_TOTAL_PAGE"));
	nvm4List.append(getValuebyName2("Advanced_Setting", "LCM_LANGUAGE"));
	nvm4List.append(getValuebyName2("Advanced_Setting", "DIRTY_FLAG"));
	nvm4List.append(getValuebyName2("Advanced_Setting", "PH_FAN_PWM"));
	nvm4List.append(getValuebyName2("Advanced_Setting", "LAST_ERROR_CODE"));
	nvm4List.append(getValuebyName2("Advanced_Setting", "LAST_ERROR_DATE"));
	nvm4List.append(getValuebyName2("Advanced_Setting", "LAST_ERROR_TIME"));
	nvm4List.append(getValuebyName2("Advanced_Setting", "COVER_SENSOR_IGONRE"));
	nvm4List.append(getValuebyName2("Advanced_Setting", "LAST_COVER_SENSOR_OFF_DATE"));
	nvm4List.append(getValuebyName2("Advanced_Setting", "LAST_COVER_SNESOR_OFF_TIME"));
	nvm4List.append(getValuebyName2("Advanced_Setting", "DURALINK_WIPER_1_SN"));
	nvm4List.append(getValuebyName2("Advanced_Setting", "DURALINK_WIPER_2_SN"));
	nvm4List.append(getValuebyName2("Advanced_Setting", "DURALINK_WIPER_3_SN"));
	nvm4List.append(getValuebyName2("Advanced_Setting", "DURALINK_WIPER_1_COUNT"));
	nvm4List.append(getValuebyName2("Advanced_Setting", "DURALINK_WIPER_1_DATE"));
	nvm4List.append(getValuebyName2("Advanced_Setting", "DURALINK_WIPER_1_TIME"));
	nvm4List.append(getValuebyName2("Advanced_Setting", "DURALINK_WIPER_2_COUNT"));
	nvm4List.append(getValuebyName2("Advanced_Setting", "DURALINK_WIPER_2_DATE"));
	nvm4List.append(getValuebyName2("Advanced_Setting", "DURALINK_WIPER_2_TIME"));
	nvm4List.append(getValuebyName2("Advanced_Setting", "DURALINK_WIPER_3_COUNT"));
	nvm4List.append(getValuebyName2("Advanced_Setting", "DURALINK_WIPER_3_DATE"));
	nvm4List.append(getValuebyName2("Advanced_Setting", "DURALINK_WIPER_3_TIME"));
	nvm4List.append(getValuebyName2("Advanced_Setting", "Valid"));

	nvm5List.append(getValuebyName2("Advanced_Setting", "NVM_NUMBER_OF_WIPES_TO_INDEX"));
	nvm5List.append(getValuebyName2("Advanced_Setting", "NVM_CLICKS_OF_ONE_INDEX"));
	nvm5List.append(getValuebyName2("Advanced_Setting", "NVM_PP_FEED_UP_A_LAYERS"));
	nvm5List.append(getValuebyName2("Advanced_Setting", "NVM_PP_BUILD_FEED_B_LAYERS"));
	nvm5List.append(getValuebyName2("Advanced_Setting", "NVM_POWDER_SUPPLY_RATIO_FIRST_10_PAGE"));
	nvm5List.append(getValuebyName2("Advanced_Setting", "NVM_POWDER_SUPPLY_RATIO"));
	nvm5List.append(getValuebyName2("Advanced_Setting", "NVM_SPREAD_1F_BUILD_C_LAYERS"));
	nvm5List.append(getValuebyName2("Advanced_Setting", "NVM_SPREAD_1F_FEED_D_LAYERS"));
	nvm5List.append(getValuebyName2("Advanced_Setting", "NVM_SPREAD_1R_BUILD_E_LAYERS"));
	nvm5List.append(getValuebyName2("Advanced_Setting", "NVM_SPREAD_1R_FEED_F_LAYERS"));
	nvm5List.append(getValuebyName2("Advanced_Setting", "NVM_SPREAD_2F_BUILD_G_LAYERS"));
	nvm5List.append(getValuebyName2("Advanced_Setting", "NVM_SPREAD_2F_FEED_H_LAYERS"));
	nvm5List.append(getValuebyName2("Advanced_Setting", "NVM_SPREAD_2R_BUILD_I_LAYERS"));
	nvm5List.append(getValuebyName2("Advanced_Setting", "NVM_SPREAD_2R_FEED_J_LAYERS"));
	nvm5List.append(getValuebyName2("Advanced_Setting", "NVM_ROLLER_CLEAN_POSITION"));
	nvm5List.append(getValuebyName2("Advanced_Setting", "NVM_ROLLER_CLEAN_SPEED"));
	nvm5List.append(getValuebyName2("Advanced_Setting", "NVM_FEEDER_DOWN_LAYERS_DURING_PRINTING"));
	nvm5List.append(getValuebyName2("Advanced_Setting", "NVM_WIPE_SPEED"));*/




	CMD_valueMap.insert(CMD_Description::CMD_NVM_R1, *new CMD_value("$NVMR1", QVariantList(), CMD_value::CMD_TYPE::NVM_Reading));
	CMD_valueMap.insert(CMD_Description::CMD_NVM_W1, *new CMD_value("$NVMW1", nvmValueList[0], CMD_value::CMD_TYPE::NVM_Setting));
	CMD_valueMap.insert(CMD_Description::CMD_NVM_R2, *new CMD_value("$NVMR2", QVariantList(), CMD_value::CMD_TYPE::NVM_Reading));
	CMD_valueMap.insert(CMD_Description::CMD_NVM_W2, *new CMD_value("$NVMW2", nvmValueList[1], CMD_value::CMD_TYPE::NVM_Setting));
	CMD_valueMap.insert(CMD_Description::CMD_NVM_R3, *new CMD_value("$NVMR3", QVariantList(), CMD_value::CMD_TYPE::NVM_Reading));
	CMD_valueMap.insert(CMD_Description::CMD_NVM_W3, *new CMD_value("$NVMW3", nvmValueList[2], CMD_value::CMD_TYPE::NVM_Setting));
	CMD_valueMap.insert(CMD_Description::CMD_NVM_R4, *new CMD_value("$NVMR4", QVariantList(), CMD_value::CMD_TYPE::NVM_Reading));
	CMD_valueMap.insert(CMD_Description::CMD_NVM_W4, *new CMD_value("$NVMW4", nvmValueList[3], CMD_value::CMD_TYPE::NVM_Setting));
	CMD_valueMap.insert(CMD_Description::CMD_NVM_R5, *new CMD_value("$NVMR5", QVariantList(), CMD_value::CMD_TYPE::NVM_Reading));
	CMD_valueMap.insert(CMD_Description::CMD_NVM_W5, *new CMD_value("$NVMW5", nvmValueList[4], CMD_value::CMD_TYPE::NVM_Setting));


	CMD_valueMap.insert(CMD_Description::CMD_FP_PC_ON, *new CMD_value("@PCOn", QVariantList(), CMD_value::CMD_TYPE::Common));

	CMD_valueMap.insert(CMD_Description::CMD_START_WIPER_CALIBRATION, *new CMD_value("@PRHS", QVariantList(), CMD_value::CMD_TYPE::Motion));
	CMD_valueMap.insert(CMD_Description::CMD_FP_WIPER_NEXT_STEP, *new CMD_value("@RWE", QVariantList(), CMD_value::CMD_TYPE::Motion));

	qDebug() << "test_exception 2";

}
/*--------------------------------------------------------------------------------------------------------------------*/

void CMD_Value::updatCMD_valueMap()
{
	CMD_valueMap.clear();
	ParamOp::getJsonFiletoString(jsonInputString, PicaApplication::getRoamingDir() + "ParameterUI_STX.txt");
	init();
}
/*--------------------------------------------------------------------------------------------------------------------*/

void CMD_Value::sendAllCommand()
{
	updatCMD_valueMap();
	QString CMD_Json;
	allCommand.clear();
	for (int i = CMD_Description::Begin; i != CMD_Description::END; ++i)
	{
		QStringList CMDValueList;
		QString command;
		command = CMD_valueMap.value(CMD_Description(i)).CMD;
		CMD_value::CMD_TYPE cmdType = CMD_valueMap.value(CMD_Description(i)).type;
		//if (command != "Sirius" && cmdType != CMD_value::CMD_TYPE::Motion && cmdType != CMD_value::CMD_TYPE::NVM_Setting &&cmdType != CMD_value::CMD_TYPE::NVM_Reading)
		if (command != "Sirius" && cmdType == CMD_value::CMD_TYPE::Board_Param)
		{
			foreach(QVariant temp, CMD_valueMap.value(CMD_Description(i)).valueList)
			{
				CMDValueList << temp.toString();

			}
			CMD_Json = ParamOp::createJsonStringCommand("FPGA", command, CMDValueList);
			allCommand << CMD_Json;
		}
		else if (i == CMD_Description::SIRIUS_WiperFrequency)
		{
			//mode 0 midjob every 4, 1 ,2 send by comm
			//bool ret = comm->setDynamicMidjob_PageInterval(CMD_valueMap.value(CMD_Description::SIRIUS_WiperFrequency).valueList[0].toInt());
			if (nullptr != comm)
			{
				//comm->sendCommandToServer(Comm3DP::CMD_To_Server_Description::CMD_To_Server_setDynamicMidjobPageInterval, QVariantList() << CMD_valueMap.value(CMD_Description::SIRIUS_WiperFrequency).valueList[0].toInt());
			}
		}
	}

	if (nullptr != comm)
	{
		foreach(QString ss, allCommand)
		{

			//comm->sendCommandToServer(Comm3DP::CMD_To_Server_Description::CMD_To_Server_JSON_COMMAND, QVariantList() << ss);

			QEventLoop loop;
			QTimer::singleShot(600, &loop, SLOT(quit()));
			loop.exec();
			//QThread::msleep(500);
			//Sleep(500);
		}
	}

}
/*--------------------------------------------------------------------------------------------------------------------*/

void CMD_Value::sendTestPageSetting() {
	QString CMD_Json;
	allCommand.clear();
	for (int i = CMD_Description::Begin; i != CMD_Description::END; ++i)
	{
		QStringList CMDValueList;
		QString command;
		command = CMD_valueMap.value(CMD_Description(i)).CMD;
		CMD_value::CMD_TYPE cmdType = CMD_valueMap.value(CMD_Description(i)).type;
		if (command != "Sirius" && cmdType == CMD_value::CMD_TYPE::Board_Param)
		{
			if (CMD_Description(i) == CMD_Printhead_Fan)
			{
				CMDValueList << QString::number(0) << QString::number(0) << QString::number(0) << QString::number(0) << QString::number(0);
			}
			else
			{
				foreach(QVariant temp, CMD_valueMap.value(CMD_Description(i)).valueList)
				{
					CMDValueList << temp.toString();
				}
			}
			CMD_Json = ParamOp::createJsonStringCommand("FPGA", command, CMDValueList);
			allCommand << CMD_Json;
		}

	}

	if (nullptr != comm)
	{
		int count = 0;
		foreach(QString ss, allCommand)
		{
			//comm->sendCommandToServer(Comm3DP::CMD_To_Server_Description::CMD_To_Server_JSON_COMMAND, QVariantList() << ss);

			QEventLoop loop;
			QTimer::singleShot(500, &loop, SLOT(quit()));
			loop.exec();
			//}

			count++;
		}
	}
}
/*--------------------------------------------------------------------------------------------------------------------*/


void CMD_Value::sendCommandWithValue(CMD_Description key, QStringList cmdValueList)
{
	QString CMD_Json;
	QString command;
	command = CMD_valueMap.value(CMD_Description(key)).CMD;
	commandRecord = command;
	if (command != "Sirius")
	{
		/*foreach(QVariant temp, CMD_valueMap.value(CMD_Description(key)).valueList)
		{
		CMDValueList << temp.toString();

		}*/
		CMD_Json = ParamOp::createJsonStringCommand("FPGA", command, cmdValueList);
	}
	else if (key == CMD_Description::SIRIUS_WiperFrequency)
	{
		//mode 0 midjob every 4, 1 ,2 send by comm
		//bool ret = comm->setDynamicMidjob_PageInterval(CMD_valueMap.value(CMD_Description::SIRIUS_WiperFrequency).valueList[0].toInt());
		if (nullptr != comm)
		{
			//comm->sendCommandToServer(Comm3DP::CMD_To_Server_Description::CMD_To_Server_setDynamicMidjobPageInterval, QVariantList() << CMD_valueMap.value(CMD_Description::SIRIUS_WiperFrequency).valueList[0].toInt());
		}
	}

	if (nullptr != comm)
	{
		QThread::msleep(500);
		//comm_qt->sendCommandToServer(Comm3DP::CMD_To_Server_Description::CMD_To_Server_JSON_COMMAND, QVariantList() << CMD_Json);
	}
}
/*--------------------------------------------------------------------------------------------------------------------*/



void CMD_Value::sendCommand(CMD_Value::CMD_Description key, QStringList CMDValueList, int delayTime)

{
	//qDebug() << "sendAllCommand()";
	//QString CMD_Json;
	////QStringList CMDValueList;
	//QString command;
	//command = CMD_valueMap.value(CMD_Description(key)).CMD;
	//commandRecord = command;
	//if (command != "Sirius")
	//{
	//	foreach(QVariant temp, CMD_valueMap.value(CMD_Description(key)).valueList)
	//	{
	//		CMDValueList << temp.toString();

	//	}
	//	CMD_Json = ParamOp::createJsonStringCommand("FPGA", command, CMDValueList);
	//}
	//else if (key == CMD_Description::SIRIUS_WiperFrequency)
	//{
	//	//mode 0 midjob every 4, 1 ,2 send by comm
	//	//bool ret = comm->setDynamicMidjob_PageInterval(CMD_valueMap.value(CMD_Description::SIRIUS_WiperFrequency).valueList[0].toInt());
	//	if (nullptr != comm)
	//	{
	//		comm->sendCommandToServer(Comm3DP::CMD_To_Server_Description::CMD_To_Server_setDynamicMidjobPageInterval, QVariantList() << CMD_valueMap.value(CMD_Description::SIRIUS_WiperFrequency).valueList[0].toInt());
	//	}
	//}
	//if (nullptr != comm)
	//{
	//	QThread::msleep(delayTime);
	//	comm->sendCommandToServer(Comm3DP::CMD_To_Server_Description::CMD_To_Server_JSON_COMMAND, QVariantList() << CMD_Json);
	//}
}
/*--------------------------------------------------------------------------------------------------------------------*/

bool CMD_Value::sendNVMCommand()
{

	QString CMD_Json;
	QStringList allCommands;// .clear();
	for (int i = CMD_Description::Begin; i != CMD_Description::END; ++i)
	{
		QStringList CMDValueList;
		QString command;
		command = CMD_valueMap.value(CMD_Description(i)).CMD;
		CMD_value::CMD_TYPE cmdType = CMD_valueMap.value(CMD_Description(i)).type;
		if (cmdType == CMD_value::CMD_TYPE::NVM_Setting)
		{
			foreach(QVariant temp, CMD_valueMap.value(CMD_Description(i)).valueList)
			{
				CMDValueList << temp.toString();
			}
			CMD_Json = ParamOp::createJsonStringCommand("FPGA", command, CMDValueList);
			allCommands << CMD_Json;
		}
	}

	int result = -1;
	if (nullptr != comm)
	{
		int counter = 0;
		foreach(QString ss, allCommands)
		{
			qDebug() << "test_exception : " << counter;
			comm->SendJSONCmd(ss.toStdString());
			QThread::msleep(2000);
			counter++;
		}
	}
	if (nullptr != comm)
	{
		comm->NVMsendcheck(result);

		QEventLoop loop;
		QTimer::singleShot(600, &loop, SLOT(quit()));
		loop.exec();
		QVariantList getReturnValue;

		if (result > 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return false;
}
/*--------------------------------------------------------------------------------------------------------------------*/

void CMD_Value::getValuebyName(QString categoryName, QString paramName, QVariant &paramValue)
{
	QVariantMap firstFloor;
	QVariantList secondFloorList;
	QVariantMap secondFloorMap;
	QVariantList thirdFloorList;
	QVariantMap thirdFloorMap;
	QVariantMap fourthFloorMap;

	bool valueGet = false;

	QJsonDocument jsonDoc;

	QJsonParseError error;
	jsonDoc = QJsonDocument::fromJson(jsonInputString.toUtf8(), &error);
	if (error.error == QJsonParseError::NoError)
		firstFloor = jsonDoc.toVariant().toMap();

	//********test count execute layers*********************//
	QMapIterator<QString, QVariant> firstFloor_ii(jsonDoc.toVariant().toMap());
	while (firstFloor_ii.hasNext())
	{
		firstFloor_ii.next();
		if (firstFloor_ii.key() == "categories")
		{
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
							* get data in JsonDocument
							*/
							fourthFloorMap = thirdFloorMap.value("control").toMap();
							paramValue = fourthFloorMap.value("value");
							valueGet = true;
							break;
						}
					}
				}
				if (valueGet)
				{
					break;
				}
			}
		}
	}
}
/*--------------------------------------------------------------------------------------------------------------------*/

QVariant CMD_Value::getValuebyName2(QString categoryName, QString paramName)
{
	QVariantMap firstFloor;
	QVariantList secondFloorList;
	QVariantMap secondFloorMap;
	QVariantList thirdFloorList;
	QVariantMap thirdFloorMap;
	QVariantMap fourthFloorMap;
	QVariant paramValue;
	QVariant transFormENUM;

	bool valueGet = false;

	QJsonDocument jsonDoc;

	QJsonParseError error;
	jsonDoc = QJsonDocument::fromJson(jsonInputString.toUtf8(), &error);
	if (error.error == QJsonParseError::NoError)
	{
		firstFloor = jsonDoc.toVariant().toMap();
	}

	//********test count execute layers*********************//
	QMapIterator<QString, QVariant> firstFloor_ii(jsonDoc.toVariant().toMap());
	while (firstFloor_ii.hasNext())
	{
		firstFloor_ii.next();
		if (firstFloor_ii.key() == "categories")
		{
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
							* get data in JsonDocument
							*/
							fourthFloorMap = thirdFloorMap.value("control").toMap();
							//paramValue = fourthFloorMap.value("value");
							transFormENUM = fourthFloorMap.value("transform_enum");

							//transform 
							//if (transFormENUM.toBool())
							if (false)
							{
								paramValue = transformDataFromUIToFPGA(fourthFloorMap.value("value"), transFormENUM.toInt());
							}
							else
							{
								paramValue = fourthFloorMap.value("value");
							}
							valueGet = true;
							break;
						}
					}
				}
				if (valueGet)
				{
					break;
				}
			}
		}
	}
	return paramValue;
}
/*--------------------------------------------------------------------------------------------------------------------*/

void CMD_Value::setValuebyName()//save value to Json File
{
	QVariantMap firstFloor;
	QVariantList secondFloorList;
	QVariantMap secondFloorMap;
	QVariantList thirdFloorList;
	QVariantMap thirdFloorMap;
	QVariantMap fourthFloorMap;

	QString categoryName;// = item->getCategoryParam().toString();
	QString paramName;// = item->getNameParam().toString();
	QString paramValue;// = item->getValueParam().toString();

	QJsonDocument jsonDoc;
	QJsonParseError error;
	ParamOp::getJsonFiletoString(jsonInputString, PicaApplication::getRoamingDir() + "ParameterUI_STX.txt");
	jsonDoc = QJsonDocument::fromJson(jsonInputString.toUtf8(), &error);
	if (error.error == QJsonParseError::NoError)
	{
		firstFloor = jsonDoc.toVariant().toMap();
	}


	QMapIterator<QVariant, QVariant> NVMiterator(NVM_Map);
	while (NVMiterator.hasNext())
	{
		NVMiterator.next();
		QMapIterator<QString, QVariant> firstFloor_ii(firstFloor);
		categoryName = "Advanced_Setting";
		paramName = NVMiterator.key().toString();
		paramValue = NVMiterator.value().toString();


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


								QVariant trEnum = fourthFloorMap["transform_enum"];

								
								fourthFloorMap["value"] = paramValue;

								valueSet = true;

								thirdFloorMap.insert("control", fourthFloorMap);
								int thirdFloorlistIndex = thirdFloorList.indexOf(thirdFloorListValue);
								thirdFloorList.replace(thirdFloorlistIndex, thirdFloorMap);

								secondFloorMap.insert("parameters", thirdFloorList);
								int secondFloorlistIndex = secondFloorList.indexOf(secondFloorListValue);
								secondFloorList.replace(secondFloorlistIndex, secondFloorMap);

								firstFloor.insert("categories", secondFloorList);
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

	QJsonDocument json = QJsonDocument::fromVariant(firstFloor);
	QString updateJson(json.toJson(QJsonDocument::Compact));
	ParamOp::saveJsonToFileWithPath(updateJson, PicaApplication::getRoamingDir() + "ParameterUI_STX.txt");
	jsonInputString = updateJson;
}
/*--------------------------------------------------------------------------------------------------------------------*/

void CMD_Value::gatherAdvanced_SettingName()
{

	NVM_NameList[0].clear(); NVM_NameList[1].clear(); NVM_NameList[2].clear(); NVM_NameList[3].clear(); NVM_NameList[4].clear();
	QVariantMap firstFloor;
	QVariantList secondFloorList;
	QVariantMap secondFloorMap;
	QVariantList thirdFloorList;
	QVariantMap thirdFloorMap;
	QVariantMap fourthFloorMap;
	QVariant paramValue;

	bool valueGet = false;

	QJsonDocument jsonDoc;

	QVariantList NVMList;
	QJsonParseError error;
	jsonDoc = QJsonDocument::fromJson(jsonInputString.toUtf8(), &error);
	if (error.error == QJsonParseError::NoError)
	{
		firstFloor = jsonDoc.toVariant().toMap();
	}

	//********test count execute layers*********************//
	QMapIterator<QString, QVariant> firstFloor_ii(jsonDoc.toVariant().toMap());
	while (firstFloor_ii.hasNext())
	{
		firstFloor_ii.next();
		if (firstFloor_ii.key() == "categories") {
			secondFloorList = firstFloor_ii.value().toList();
			foreach(QVariant secondFloorListValue, secondFloorList)
			{
				secondFloorMap = secondFloorListValue.toMap();
				if (secondFloorMap.value("name") == "Advanced_Setting")
				{
					thirdFloorList = secondFloorMap.value("parameters").toList();
					int i = 1;
					foreach(QVariant thirdFloorListValue, thirdFloorList)
					{
						thirdFloorMap = thirdFloorListValue.toMap();
						NVMList.append(thirdFloorMap.value("name"));
						if (i < (NUMOFNVM1 + 1)) {
							NVM_NameList[0].append(thirdFloorMap.value("name"));
						}
						else if (i < (NUMOFNVM1 + NUMOFNVM2 + 1)) {
							NVM_NameList[1].append(thirdFloorMap.value("name"));
						}
						else if (i > (NUMOFNVM1 + NUMOFNVM2 + 1) && i < (NUMOFNVM1 + NUMOFNVM2 + NUMOFNVM3 + 2)) {
							NVM_NameList[2].append(thirdFloorMap.value("name"));
						}
						else if (i > (NUMOFNVM1 + NUMOFNVM2 + NUMOFNVM3 + 2) && i < (NUMOFNVM1 + NUMOFNVM2 + NUMOFNVM3 + NUMOFNVM4 + 3)) {
							NVM_NameList[3].append(thirdFloorMap.value("name"));

						}
						else if (i > (NUMOFNVM1 + NUMOFNVM2 + NUMOFNVM3 + NUMOFNVM4 + 3) && i < (NUMOFNVM1 + NUMOFNVM2 + NUMOFNVM3 + NUMOFNVM4 + NUMOFNVM5 + 4)) {
							NVM_NameList[4].append(thirdFloorMap.value("name"));
						}
						i++;
					}

				}
				if (valueGet)
				{
					valueGet = false;
					break;
				}
			}
		}
	}
}
/*--------------------------------------------------------------------------------------------------------------------*/

bool CMD_Value::getNVMValue()
{
	NVM_Map.clear();
	allCommand.clear();
	QString NVMCommand;

	QString CMD_Json;
	for (int i = CMD_Description::Begin; i != CMD_Description::END; ++i)
	{
		QStringList CMDValueList;
		QString command;
		command = CMD_valueMap.value(CMD_Description(i)).CMD;
		CMD_value::CMD_TYPE cmdType = CMD_valueMap.value(CMD_Description(i)).type;
		if (cmdType == CMD_value::CMD_TYPE::NVM_Reading)
		{
			CMD_Json = ParamOp::createJsonStringCommand("FPGA", command, CMDValueList);
			allCommand << CMD_Json;
		}
	}

	if (nullptr == comm)
	{
		return false;
	}

	QString getValue;

	int j = 0;
	int NVMnumNumber[5] = { NUMOFNVM1 + 1, NUMOFNVM2 + 1, NUMOFNVM3 + 1, NUMOFNVM4 + 1, NUMOFNVM5 + 1 };
	bool getRightValue = true;
	//foreach(QString ss, allCommand)
	for (int cmd = 0; cmd < allCommand.size(); cmd++)
	{
		QString ss = allCommand.at(cmd);
		std::string AAA;

		comm->GetNVMValue(ss.toStdString(), AAA);

		//QThread::msleep(500);
		QEventLoop loop;
		QTimer::singleShot(500, &loop, SLOT(quit()));
		loop.exec();


		/*
		parse json and update to json txt
		*/

		if (AAA == "ERR") {
			qDebug() << "can't get return value";
			return false;
		}

		QString fromFPGA = QString::fromStdString(AAA);
		QStringList separateString = fromFPGA.split(QRegExp("[\r\n]+"), QString::SkipEmptyParts);
		QVariantList tempValueList;
		if (separateString.size() != NVMnumNumber[j])
		{
			cmd--;
			continue;
		}


		for (int i = 0; i < separateString.size() - 1; i++)
		{
			QString sp_string = separateString[i];
			QRegularExpression re("^(\\d_\\d+) (\\w+): (.+)$");
			QRegularExpressionMatch match = re.match(sp_string);
			NVM_Map.insert(NVM_NameList[j].at(i), match.captured(3));

			tempValueList.push_back(QVariant(match.captured(3)));
			/*qDebug() << match.captured(1);
			qDebug() << match.captured(2);
			qDebug() << match.captured(3);*/
		}

		/*check value is the same from FPGA*/
		QString chSp_string = separateString[separateString.size() - 1];
		QRegularExpression re("^(\\d_\\d+) (\\w+): (.+)$");
		QRegularExpressionMatch match = re.match(chSp_string);
		QString CHFromFPGA = match.captured(3);

		QString valuefromFPGA_CH = countCheckSumTest(tempValueList, j);
		qDebug() << "value_CH : " << valuefromFPGA_CH;
		qDebug() << "FPGA_CH : " << CHFromFPGA;
		if (CHFromFPGA == valuefromFPGA_CH) {
			qDebug() << "match";
		}
		else {
			getRightValue = false;
			qDebug() << "not Match";
		}
		/*=====================================================*/

		j++;
	}



	if (getRightValue)
	{
		setValuebyName();
		return true;
	}


	//update to json File
	//update cmdlist();

	// Failed to retrieve the value
	return false;
}
/*--------------------------------------------------------------------------------------------------------------------*/


//void CMDValue::getNVMValue()
//{
//	NVM_Map.clear();
//	allCommand.clear();
//	QString NVMCommand;;
//
//
//	QString CMD_Json;
//	for (int i = (int)CMD_Description::Begin; i != (int)CMD_Description::END; ++i)
//	{
//		QStringList CMDValueList;
//		QString command;
//		command = CMD_valueMap.value(CMD_Description(i)).CMD;
//		CMD_value::CMD_TYPE cmdType = CMD_valueMap.value(CMD_Description(i)).type;
//		if (cmdType == CMD_value::CMD_TYPE::NVM_Reading)
//		{
//			CMD_Json = ParamOp::createJsonStringCommand("FPGA", command, CMDValueList);
//			allCommand << CMD_Json;
//		}
//	}
//	std::string getValue;
//	int j = 0;
//	int NVMnumNumber[5] = { 8,16,46,26,20 };
//	bool getRightValue = true;
//	//foreach(QString ss, allCommand)
//	for (int cmd = 0; cmd < allCommand.size(); cmd++)
//	{
//		QString ss = allCommand.at(cmd);
//		comm->SendDMCmdGetValueTest(getValue, ss.toStdString());
//		Sleep(500);
//
//		/*
//			parse json and update to json txt
//		*/
//		QString fromFPGA(QString::fromStdString(getValue));
//		QStringList separateString = fromFPGA.split(QRegExp("[\r\n]+"), QString::SkipEmptyParts);
//		QVariantList tempValueList;
//		if (separateString.size() != NVMnumNumber[j])
//		{
//			cmd--;
//			continue;
//		}
//
//
//		for (int i = 0; i < separateString.size() - 1; i++)
//		{
//			QString sp_string = separateString[i];
//			QRegularExpression re("^(\\d_\\d+) (\\w+): (\.+)$");
//			QRegularExpressionMatch match = re.match(sp_string);
//			NVM_Map.insert(NVM_NameList[j].at(i), match.captured(3));
//
//			tempValueList.push_back(QVariant(match.captured(3)));
//			/*qDebug() << match.captured(1);
//			qDebug() << match.captured(2);
//			qDebug() << match.captured(3);*/
//		}
//
//		/*check value is the same from FPGA*/
//		QString chSp_string = separateString[separateString.size() - 1];
//		QRegularExpression re("^(\\d_\\d+) (\\w+): (\.+)$");
//		QRegularExpressionMatch match = re.match(chSp_string);
//		QString CHFromFPGA = match.captured(3);
//
//		QString valuefromFPGA_CH = countCheckSumTest(tempValueList, j);
//		qDebug() << "value_CH : " << valuefromFPGA_CH;
//		qDebug() << "FPGA_CH : " << CHFromFPGA;
//		if (CHFromFPGA == valuefromFPGA_CH) {
//			qDebug() << "match";
//		}
//		else {
//			getRightValue = false;
//			qDebug() << "not Match";
//		}
//		/*=====================================================*/
//
//		j++;
//	}
//
//
//
//	//if (getRightValue)
//		setValuebyName();
//
//
//	//update to json File
//	//update cmdlist();
//
//}
/*--------------------------------------------------------------------------------------------------------------------*/

void CMD_Value::countCheckSum(QString inputString)
{
	/*unsigned char *iobuf = NULL;
	iobuf = new unsigned char[4194304];
	memset(iobuf, 0xff, 4194304);

	int i, check_sum_result;

	std::string inS = inputString.toStdString();

	std::copy(inS.begin(), inS.end(), iobuf);

	for (int i = 0; i < 4194304; i++)
	{
	check_sum_result += iobuf[i];
	}
	*/
	(void)inputString;
}
/*--------------------------------------------------------------------------------------------------------------------*/

QVariant CMD_Value::transformDataFromUIToFPGA(QVariant value, int trasformEnum)
{
	double dvalue = value.toDouble();
	switch (trasformEnum)
	{
	case 1:     return int((1. / dvalue)*pow(10, 6));
	case 2:     return int(50 * dvalue);
	case 3:     return int((9.375 / dvalue)*pow(10, 5));
	case 4:     return int(3200 * dvalue);
	case 5:     return int(24804.68 / dvalue);
		//case 6:     return int(2015.74803*dvalue);
	case 6:     return int(5039.37*dvalue);
	default:
		return value;// Do nothing
		break;
	}
	return QVariant();
}
/*--------------------------------------------------------------------------------------------------------------------*/

QVariant CMD_Value::transformDataFromFPGAToUI(const QVariant value, int trasformEnum)
{
	int fvalue = value.toInt();
	switch (trasformEnum)
	{
	case 1:     return (1. / fvalue)*pow(10, 6);
	case 2:     return (fvalue / 50.);
	case 3:     return (9.375 / fvalue)* pow(10, 5);
	case 4:     return fvalue / 3200.;
	case 5:     return 24804.69 / fvalue;
		//case 6:     return fvalue / 2015.74803;
	case 6:     return fvalue / 5039.37;
	default:
		return value;// Do nothing
		break;
	}
	return QVariant();
}
/*--------------------------------------------------------------------------------------------------------------------*/

QString CMD_Value::countCheckSumTest(QVariantList valueList, int num)
{

	switch (num)
	{
	case 0:
	{
		unsigned char *iobuf = nullptr;
		iobuf = new unsigned char[4000000];
		memset(iobuf, 0x00, 4000000);
		unsigned char *ptr = iobuf;
		std::size_t count = 0;
		std::size_t varNumCount2 = 0;
		foreach(QVariant var, valueList)
		{
			if (varNumCount2 == 5 || varNumCount2 == 6)
			{
				int tempint = var.toInt();
				unsigned char * pucTemporary = reinterpret_cast<unsigned char *>(&tempint);
				*ptr = pucTemporary[0]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;
				*ptr = pucTemporary[1]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;
				*ptr = pucTemporary[2]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;
				*ptr = pucTemporary[3]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;
				count += 4;
				varNumCount2++;
			}
			else
			{
				std::string tempString = var.toString().toStdString();
				std::copy(tempString.begin(), tempString.end(), ptr);
				//qDebug() << "tempString.size(): " << tempString.size();
				if (tempString.size() % 2 > 0)
				{
					ptr++; count++;
					ptr += tempString.size();
					count += tempString.size();
				}
				else
				{
					ptr += tempString.size();
					count += tempString.size();
				}
				varNumCount2++;
			}
		}
		std::size_t xsx = Checksum_Cal(reinterpret_cast<unsigned short*>(iobuf), count);

		QString hexvalue = QString("%1").arg(xsx, 4, 16, QLatin1Char('0')).toUpper().right(8);
		qDebug() << hexvalue;
		return hexvalue;
	}

	case 1:
	{
		unsigned char *iobuf = nullptr;
		iobuf = new unsigned char[4000000];
		memset(iobuf, 0x00, 4000000);
		unsigned char *ptr = iobuf;
		int count = 0;
		int varNumCount2 = 0;

		foreach(QVariant var, valueList)
		{
			int tempint = var.toInt();
			unsigned char * pucTemporary = reinterpret_cast<unsigned char *>(&tempint);
			*ptr = pucTemporary[0]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;
			*ptr = pucTemporary[1]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;
			*ptr = pucTemporary[2]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;
			*ptr = pucTemporary[3]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;

			count += 4;
			varNumCount2++;
		}
		std::size_t xsx = Checksum_Cal(reinterpret_cast<unsigned short*>(iobuf), count);
		QString hexvalue = QString("%1").arg(xsx, 4, 16, QLatin1Char('0')).toUpper().right(8);
		qDebug() << hexvalue;
		return hexvalue;

	}

	case 2:
	{
		unsigned char *iobuf = nullptr;
		iobuf = new unsigned char[4000000];
		memset(iobuf, 0x00, 4000000);
		unsigned char *ptr = iobuf;
		std::size_t count = 0;
		std::size_t varNumCount2 = 0;
		foreach(QVariant var, valueList)
		{
			int tempint = var.toInt();
			unsigned char * pucTemporary = reinterpret_cast<unsigned char *>(&tempint);

			*ptr = pucTemporary[0]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;
			*ptr = pucTemporary[1]; /*qDebug() << "Value : " << hex << *ptr;*/  ptr++;
			*ptr = pucTemporary[2]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;
			*ptr = pucTemporary[3]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;

			count += 4;
			varNumCount2++;
		}
		std::size_t xsx = Checksum_Cal(reinterpret_cast<unsigned short*>(iobuf), count);
		QString hexvalue = QString("%1").arg(xsx, 4, 16, QLatin1Char('0')).toUpper().right(8);
		qDebug() << hexvalue;
		return hexvalue;
	}

	case 3:
	{
		unsigned char *iobuf = nullptr;
		iobuf = new unsigned char[4000000];
		memset(iobuf, 0x00, 4000000);
		unsigned char *ptr = iobuf;
		std::size_t count = 0;
		std::size_t varNumCount2 = 0;
		foreach(QVariant var, valueList)
		{
			if (varNumCount2 == 13 || varNumCount2 == 14 || varNumCount2 == 15)
			{
				std::string tempString = var.toString().toStdString();
				std::copy(tempString.begin(), tempString.end(), ptr);

				if (tempString.size() % 2 > 0)
				{
					ptr++; count++;
					ptr += tempString.size();
					count += tempString.size();
				}
				else
				{
					ptr += tempString.size();
					count += tempString.size();
				}
				varNumCount2++;

			}
			else
			{
				int tempint = var.toInt();
				unsigned char * pucTemporary = reinterpret_cast<unsigned char *>(&tempint);

				*ptr = pucTemporary[0]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;
				*ptr = pucTemporary[1]; /*qDebug() << "Value : " << hex << *ptr;*/  ptr++;
				*ptr = pucTemporary[2]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;
				*ptr = pucTemporary[3]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;

				count += 4;
				varNumCount2++;
			}
		}
		std::size_t xsx = Checksum_Cal(reinterpret_cast<unsigned short*>(iobuf), count);
		QString hexvalue = QString("%1").arg(xsx, 4, 16, QLatin1Char('0')).toUpper().right(8);
		qDebug() << hexvalue;
		return hexvalue;
	}

	case 4:
	{
		unsigned char *iobuf = nullptr;
		iobuf = new unsigned char[4000000];
		memset(iobuf, 0x00, 4000000);
		unsigned char *ptr = iobuf;
		std::size_t count = 0;
		std::size_t varNumCount2 = 0;
		foreach(QVariant var, valueList)
		{

			int tempint = var.toInt();
			unsigned char * pucTemporary = reinterpret_cast<unsigned char *>(&tempint);

			*ptr = pucTemporary[0]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;
			*ptr = pucTemporary[1]; /*qDebug() << "Value : " << hex << *ptr;*/  ptr++;
			*ptr = pucTemporary[2]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;
			*ptr = pucTemporary[3]; /*qDebug() << "Value : " << hex << *ptr;*/ ptr++;

			count += 4;
			varNumCount2++;
		}
		std::size_t xsx = Checksum_Cal(reinterpret_cast<unsigned short*>(iobuf), count);
		QString hexvalue = QString("%1").arg(xsx, 4, 16, QLatin1Char('0')).toUpper().right(8);
		qDebug() << hexvalue;
		return hexvalue;

	}

	default:
		return QString();
	}
}
/*--------------------------------------------------------------------------------------------------------------------*/

static size_t Checksum_Cal(unsigned short *addr, std::size_t count)
{
	/* Compute Internet Checksum for "count" bytes
	*         beginning at location "addr".
	*/
	volatile long sum = 0;

	while (count > 1)
	{
		// This is the inner loop /
		//printf("*addr = 0x%x; count = %d\n\r", *addr, count);
		//printf(" 0x%lx (Value 0x%hx ); count = %d\n\r", reinterpret_cast<uint64_t>(addr), *addr, count);	// XYZ - LP4 marked, 20181030

		//printf("sum: %lx", sum);
		sum += *(addr++);
		count -= 2;
	}

	// Add left - over byte, if any /
	if (count > 0)
		sum += static_cast<unsigned char>(*addr);

	// Fold 32 - bit sum to 16 bits /
	while (sum >> 16)
		sum = (sum & 0xffff) + (sum >> 16);

	return static_cast<int>(~sum);
}
/*--------------------------------------------------------------------------------------------------------------------*/

