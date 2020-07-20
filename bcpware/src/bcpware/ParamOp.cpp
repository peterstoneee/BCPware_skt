#include "ParamOp.h"
#include <QDateTime>
#include <QFile>
#include <QTranslator>


ParamOp::ParamOp()
{

}


bool ParamOp::saveJsonToFile(QString jsonString)
{
	QFile outfile(ParamOp::docPath() + "/BCPwareDM/" + DM_PARAM_SETTING);

	if (outfile.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
	{
		QTextStream stream(&outfile);
		stream << jsonString << endl;
		outfile.close();
	}
	return true;
}
bool ParamOp::saveJsonToFile(QString jsonString, QString fileName)
{
	QFile outfile(ParamOp::docPath() + "/BCPwareDM/" + fileName);

	if (outfile.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
	{
		QTextStream stream(&outfile);
		stream << jsonString << endl;
		outfile.close();
	}
	return true;
}
bool ParamOp::saveStringToFileWithPath(QString jsonString, QString fileName)
{
	QFile outfile(fileName);

	if (outfile.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
	{
		QTextStream stream(&outfile);
		stream << jsonString << endl;
		outfile.close();
	}
	return true;
}
bool ParamOp::saveStringToFileWithPath(QByteArray qqString, QString fileName)
{
	QFile outfile(fileName);

	if (outfile.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
	{
		QTextStream stream(&outfile);
		stream << qqString << endl;
		outfile.close();
	}
	return true;
}


//void ParamOp::genAllParamInfo(RichParameterSet &param)
//{
//	QFile outfile(ParamOp::docPath() + "/BCPwareDM" + "/PRINTER_PARAM.txt");
//
//	QVariantMap firstFloor;
//	QVariantMap secondFloor, secondFloor2;
//	QVariantMap thirdFloor;
//
//	QString jsonString;
//	QJsonDocument jsonDoc;
//
//
//	if (outfile.exists())
//	{
//		if (!outfile.open(QIODevice::ReadOnly))
//		{
//			qDebug() << "file open Error";
//		}
//		QTextStream jsonStream(&outfile);
//		jsonString = jsonStream.readAll();
//		QJsonParseError error;
//		jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8(), &error);
//		if (error.error == QJsonParseError::NoError)
//			firstFloor = jsonDoc.toVariant().toMap();
//
//		outfile.close();
//	}
//
//	foreach(RichParameter *curpar, param.paramList)
//	{
//		if (curpar->name == QString("FAN_SPEED")){ secondFloor.insert(curpar->name, QString::number(curpar->val->getInt())); }
//		else if (curpar->name == QString("PUMP_VALUE")){ secondFloor.insert(curpar->name, QString::number(curpar->val->getInt())); }
//		else if (curpar->name == QString("STIFF_PRINT_VALUE")){ secondFloor.insert(curpar->name, QString::number(curpar->val->getInt())); }
//		else if (curpar->name == QString("COLOR_PROFILE")){ secondFloor2.insert(curpar->name, curpar->val->getString()); }
//
//		//else if (curpar->name == QString("PAGE_SIZE")){ secondFloor.insert(curpar->name, QString::number(curpar->val->getEnum())); }
//
//	}
//
//	/*int DP3 = param.getEnum("TARGET_PRINTER");
//	if (DP3 == 1)
//	secondFloor.insert("PartPro350 XBC", thirdFloor);
//	else if (DP3 == 0)
//	secondFloor.insert("Palette", thirdFloor);*/
//
//	//firstFloor.insert(QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss"), secondFloor);
//	firstFloor.insert("SIRIUS_PARAM", secondFloor);
//	firstFloor.insert("FPGA_PARAM", secondFloor2);
//	//firstFloor.insert(QString("total_pages"), QString::number(test_count));
//
//	jsonDoc = QJsonDocument::fromVariant(firstFloor);
//	outfile.open(QIODevice::WriteOnly);
//	QTextStream outtext(&outfile);
//	outtext << QString(jsonDoc.toJson());
//
//	outfile.close();
//
//}


QVariant ParamOp::getValueFromJsonSet(QString &jsonString, QString firstValue, QString secondValue)
{
	//	int test_count = 0;
	QVariantMap firstFloor;
	//QString jsonString;
	QJsonDocument jsonDoc;

	QJsonParseError error;
	jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8(), &error);
	if (error.error == QJsonParseError::NoError)
		firstFloor = jsonDoc.toVariant().toMap();

	//********test count execute layers*********************//
	QMapIterator<QString, QVariant> ii(jsonDoc.toVariant().toMap());
	while (ii.hasNext())
	{
		ii.next();
		if (ii.key() == firstValue) {
			QMapIterator<QString, QVariant> ii2(ii.value().toMap());
			while (ii2.hasNext())
			{
				ii2.next();
				if (!ii2.key().compare(secondValue))return ii2.value();
			}
		}

	}
	//outfile.close();

	/*firstFloor.insert(QString("total_pages"), QString::number(test_count));
	jsonDoc = QJsonDocument::fromVariant(firstFloor);
	outfile.open(QIODevice::WriteOnly);
	QTextStream outtext(&outfile);
	outtext << QString(jsonDoc.toJson());
	outfile.close();*/
	//************************************
	//}

	return QVariant();
}

bool ParamOp::setValueToJsonSet(QString &jsonString, QString secondFloor, QVariant value)
{
	//	int test_count = 0;

	QVariantMap NewfirstFloor, NewSecondFloor;

	QJsonDocument jsonDoc;

	QJsonParseError error;
	jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8(), &error);
	bool getKey = false;
	if (error.error == QJsonParseError::NoError)
	{
		QMapIterator <QString, QVariant> ii(jsonDoc.toVariant().toMap());
		while (ii.hasNext())
		{
			ii.next();
			QMap <QString, QVariant>ii2(ii.value().toMap());
			if (ii2.contains(secondFloor))
			{
				ii2.insert(secondFloor, QVariant(value).toString());
				getKey = true;
			}
			NewfirstFloor.insert(ii.key(), ii2);
		}

		/*
		generate new Json String
		*/
		if (getKey)
		{

			QJsonDocument json = QJsonDocument::fromVariant(NewfirstFloor);
			QString updateJson(json.toJson(QJsonDocument::Compact));
			jsonString = updateJson;
			return true;
		}
		else return false;
		/*QMap<QString, int> myMap;
		QJsonObject json;
		QMapIterator<QString, int> iii(myMap);
		while (iii.hasNext())
		{
		iii.next();
		QMapIterator <QString, QVariant> iii2(ii.value().toMap());
		while (iii2.hasNext())
		{
		iii2.next();
		json.insert(iii2.key(), iii2.value());
		}
		}*/


		//		return true;


	}

	return false;
	//************************************
	//}
}


//void ParamOp::getParamfromJsonFile(RichParameterSet &paramGet)
//{
//	int test_count = 0;
//	QVariantMap firstFloor;
//	QString jsonString;
//	QJsonDocument jsonDoc;
//
//	QFile outfile(ParamOp::docPath() + "/BCPwareDM" + "/printAllInfo.txt");
//	if (outfile.exists())
//	{
//		if (!outfile.open(QIODevice::ReadOnly))
//		{
//			qDebug() << "file open Error";
//		}
//		QTextStream jsonStream(&outfile);
//		jsonString = jsonStream.readAll();
//		QJsonParseError error;
//		jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8(), &error);
//		if (error.error == QJsonParseError::NoError)
//			firstFloor = jsonDoc.toVariant().toMap();
//
//		//********test count execute layers*********************//
//		QMapIterator<QString, QVariant> ii(jsonDoc.toVariant().toMap());
//		while (ii.hasNext())
//		{
//			ii.next();
//			QMapIterator<QString, QVariant> ii2(ii.value().toMap());
//			while (ii2.hasNext())
//			{
//				ii2.next();
//				if (ii2.key().contains("FAN_SPEED")){ paramGet.setValue("FAN_SPEED", IntValue(ii2.value().toInt())); }
//				else if (ii2.key().contains("PUMP_VALUE")){ paramGet.setValue("PUMP_VALUE", IntValue(ii2.value().toInt())); }
//				else if (ii2.key().contains("STIFF_PRINT_VALUE")){ paramGet.setValue("STIFF_PRINT_VALUE", IntValue(ii2.value().toInt())); }
//				else if (ii2.key().contains("COLOR_PROFILE")){ paramGet.setValue("COLOR_PROFILE", StringValue(ii2.value().toString())); }
//
//			}
//		}
//		outfile.close();
//		firstFloor.insert(QString("total_pages"), QString::number(test_count));
//		jsonDoc = QJsonDocument::fromVariant(firstFloor);
//		outfile.open(QIODevice::WriteOnly);
//		QTextStream outtext(&outfile);
//		outtext << QString(jsonDoc.toJson());
//		outfile.close();
//		//************************************
//	}
//}


bool ParamOp::getJsonFiletoString(QString &getS)
{
	//QString jsonString;
	QFile outfile(ParamOp::docPath() + "/BCPwareDM" + "/PRINTER_PARAM.txt");
	if (outfile.exists())
	{
		//if (!outfile.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
		if (!outfile.open(QIODevice::ReadWrite))
		{
			qDebug() << "file open Error";
		}
		QTextStream jsonStream(&outfile);
		getS = jsonStream.readAll();
		return true;
	}return false;


}


bool ParamOp::getJsonFiletoString(QString &jsonString, QString jsonFileName)
{

	QFile outfile(jsonFileName);
	if (outfile.exists())
	{
		//if (!outfile.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
		if (!outfile.open(QIODevice::ReadWrite))
		{
			qDebug() << "file open Error";
		}
		QTextStream jsonStream(&outfile);
		jsonString = jsonStream.readAll();
		outfile.close();
		return true;
	}return false;

}
QString ParamOp::createJsonStringCommand(QString board, QString command, QStringList value)
{


	QVariantMap firstFloor;
	QVariantMap secondFloor;
	QVariantMap thirdFloor;

	QString jsonString;
	QJsonDocument jsonDoc;

	secondFloor.insert(command, value);

	firstFloor.insert(board, secondFloor);


	//firstFloor.insert(QString("total_pages"), QString::number(test_count));

	jsonDoc = QJsonDocument::fromVariant(firstFloor);

	QString test = jsonDoc.toJson();

	return QString(jsonDoc.toJson());


}

bool ParamOp::exTractJsonStringCommand(QString inputJsonString, QString &board, QString &command, QStringList &value)
{
	QVariantMap firstFloor;



	QJsonDocument jsonDoc;

	QJsonParseError error;
	jsonDoc = QJsonDocument::fromJson(inputJsonString.toUtf8(), &error);
	if (error.error == QJsonParseError::NoError)
		firstFloor = jsonDoc.toVariant().toMap();

	//********test count execute layers*********************//
	QMapIterator<QString, QVariant> ii(jsonDoc.toVariant().toMap());

	while (ii.hasNext())
	{
		ii.next();
		board = ii.key();

		QMapIterator<QString, QVariant> ii2(ii.value().toMap());
		while (ii2.hasNext())
		{
			ii2.next();
			//if (ii2.key().contains("VIBRATION"));
			command = ii2.key();
			value = ii2.value().toStringList();
			return true;
		}


	}

	return false;

}


//bool ParamOp::createWiperMap(QVariantMap &newWiper, int countINit)
//{
//	newWiper.insert(WIPER_RECORD_LAST_INSTALL_TIME, QDateTime::currentDateTime().toString("yyyy_MM_dd_HH:mm:ss"));
//	newWiper.insert(WIPER_RECORD_KEY_LEFT_COUNT, countINit);
//	newWiper.insert(WIPER_RECORD_KEY_USED_TIMES, 1);
//	return 0;
//}

bool ParamOp::extractVariantTest(QVariant &output, QVariant input, QString mapKey, int listID, QString docfileName, QString jsonString)
{
	if (!docfileName.isNull())
	{
		QString jsonString;
		QJsonDocument jsonDoc;
		QFile outfile(docfileName);
		if (outfile.exists())
		{
			if (!outfile.open(QIODevice::ReadOnly))
			{

			}
			QTextStream jsonStream(&outfile);
			jsonString = jsonStream.readAll();
			QJsonParseError error;
			jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8(), &error);
			if (error.error == QJsonParseError::NoError)
				output = jsonDoc.toVariant();
			outfile.close();
			return true;
		}
	}
	else if (!jsonString.isNull())
	{
		QJsonDocument jsonDoc;
		QJsonParseError error;
		jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8(), &error);
		if (error.error == QJsonParseError::NoError)
			output = jsonDoc.toVariant();
		return true;
	}
    if ( QMetaType::QVariantMap == static_cast<QMetaType::Type>( input.type() ) )
	{
		QVariantMap temp = input.toMap();
		output = temp.value(mapKey);
		if (output.isValid())return true;
		else return false;
	}
    if ( QMetaType::QVariantList == static_cast<QMetaType::Type>( input.type() ) )
    {
		output = input.toList()[listID];
		return true;

	}
	return false;

}
bool ParamOp::mergeValue(QVariant &highLevelValue, QVariant insertValue, QString key, int replaceNum, QString docFileName)
{
    if ( ( QMetaType::QVariantMap == static_cast<QMetaType::Type>( highLevelValue.type() ) )
         && docFileName.isNull())
	{
		QVariantMap temp = highLevelValue.toMap();
		temp.insert(key, insertValue);
		highLevelValue = temp;
		return true;
	}
    else if ( ( QMetaType::QVariantList == static_cast<QMetaType::Type>( highLevelValue.type() ) )
		&& (-1 == replaceNum) && docFileName.isNull())
	{
		QVariantList temp = highLevelValue.toList();
		temp.append(insertValue);
		highLevelValue = temp;
		return true;
	}
    else if ( ( QMetaType::QVariantList == static_cast<QMetaType::Type>( highLevelValue.type() ) )
		&& (-1 != replaceNum) && docFileName.isNull())
    {
		QVariantList temp = highLevelValue.toList();
		if (replaceNum < temp.size() )
		{
			temp.replace(replaceNum, insertValue);
			highLevelValue = temp;
			return true;
		}
		else if (replaceNum >= temp.size())
		{
			int emptyNum = replaceNum + 1 - temp.size();
			for (int i = 0; i < emptyNum; i++)
			{
				temp.append(QVariant());
			}
			temp.replace(replaceNum, insertValue);
			highLevelValue = temp;
			return true;
		}
			return false;
	}
	else if ((QMetaType::QString == static_cast<QMetaType::Type>(highLevelValue.type()))
		 && docFileName.isNull())
	{
		QJsonDocument jsonDoc = QJsonDocument::fromVariant(insertValue);
		QString updateJson(jsonDoc.toJson(QJsonDocument::Compact));
		highLevelValue = updateJson;
		if (!updateJson.isEmpty())
			return true;
	}
    else if ( !docFileName.isNull() )
	{
		QJsonDocument jsonDoc = QJsonDocument::fromVariant(highLevelValue);
		QString updateJson(jsonDoc.toJson(QJsonDocument::Compact));
		if (ParamOp::saveStringToFileWithPath(updateJson, docFileName))
			return true;

	}
	return false;
}

bool ParamOp::transformJsonToRichParameter(RichParameterSet &input, QString jsonString)
{
	QString _jsonString;
	getJsonFiletoString(_jsonString,"C:/Users/TB495076/Documents/BCPware/ParameterUI_STX.txt");

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

	QStringList type;
	type << "Basic_Setting" << "Advanced_Setting" << "Printer_Setting";
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
				if (categoryName == type[2])
				{
					thirdFloorList = secondFloorMap.value("parameters").toList();
					foreach(QVariant thirdFloorListValue, thirdFloorList)
					{
						/*ParameterAttribute *paramAttrib = new ParameterAttribute();
						paramAttrib->setCategory(secondFloorMap.value("name"));
						thirdFloorMap = thirdFloorListValue.toMap();
						paramAttrib->setName(thirdFloorMap.value("name"));
						paramAttrib->setCommand(thirdFloorMap.value("CMD"));*/

						fourthFloorMap = thirdFloorMap.value("control").toMap();

						QVariant controlName = fourthFloorMap.value("name");
						//paramAttrib->setUIType(controlName);
						if (controlName == "SpinBox")
						{
							QStringList languageList = QStringList() << "English" << "Japanese";
							input.addParam(new RichEnum("Palette_Language", 0, languageList, QString("Language"), QString("")));
							/*paramAttrib->setValue(fourthFloorMap.value("value"));
							paramAttrib->setDefaultValue(fourthFloorMap.value("default"));
							paramAttrib->setSpinStep(fourthFloorMap.value("spinStep"));
							paramAttrib->setHighLimit(fourthFloorMap.value("highLimit"));
							paramAttrib->setLowLimit(fourthFloorMap.value("lowLimit"));
							paramAttrib->setUiUnit(fourthFloorMap.value("ui_unit"));*/

						}
						if (controlName == "DSpinBox" && (fourthFloorMap.value("transform_enum") > 0))
						{

							//qDebug() << "UI_data_ii:  " << item.name() << item.transformType() << item.value();

							//QVariant temp = CMDValue::transformDataFromFPGAToUI(fourthFloorMap.value("value"), (fourthFloorMap.value("transform_enum").toInt()));
							//qDebug() << "UI data1: " << controlName << temp.toDouble();
							//temp = qFloor(temp.toDouble() * 1000) / 1000.;
							//qDebug() << "UI data: " << controlName << temp;

							/*paramAttrib->setValue(temp);

							paramAttrib->setDefaultValue(fourthFloorMap.value("default"));
							paramAttrib->setSpinStep(fourthFloorMap.value("spinStep"));
							paramAttrib->setHighLimit(fourthFloorMap.value("highLimit"));
							paramAttrib->setLowLimit(fourthFloorMap.value("lowLimit"));
							paramAttrib->setUiUnit(fourthFloorMap.value("ui_unit"));*/

						}
						else if (controlName == "DSpinBox")
						{
							/*paramAttrib->setValue(fourthFloorMap.value("value"));
							paramAttrib->setDefaultValue(fourthFloorMap.value("default"));
							paramAttrib->setSpinStep(fourthFloorMap.value("spinStep"));
							paramAttrib->setHighLimit(fourthFloorMap.value("highLimit"));
							paramAttrib->setLowLimit(fourthFloorMap.value("lowLimit"));
							paramAttrib->setUiUnit(fourthFloorMap.value("ui_unit"));*/
						}
						else if (controlName == "CheckBox")
						{
							/*paramAttrib->setValue(fourthFloorMap.value("value"));
							paramAttrib->setDefaultValue(fourthFloorMap.value("default"));*/
						}
						else if (controlName == "EnumUI")
						{
							/*paramAttrib->setValue(fourthFloorMap.value("value"));
							paramAttrib->setDefaultValue(fourthFloorMap.value("default"));
							paramAttrib->setEnumeration(fourthFloorMap.value("enumeration"));*/
						}
						else if (controlName == "TextfieldUI" || controlName == "TextLabelUI")
						{
							/*paramAttrib->setValue(fourthFloorMap.value("value"));
							paramAttrib->setDefaultValue(fourthFloorMap.value("default"));*/
						}

						//paramAttrib->setTransformType(fourthFloorMap.value("transform_enum"));
						if (fourthFloorMap.value("transform_enum").toInt() > 0)
						{
							//qDebug() << "JSON Data" << paramAttrib->name() << paramAttrib->transformType();
						}

						//if (controlName != "TextLabelUI")
						//paramAttributeSet.push_back(*paramAttrib);



						
					}
				}
			}
		}
	}




	return true;



}

void ParamOp::updateToHistory(bool newOrNot, QString _key, QVariant _value)
{
	QVariant _dmHistory;
	QVariant _currentID, _id_History;
	if (ParamOp::extractVariantTest(_dmHistory, QVariant(), QString(), -1, BCPwareFileSystem::printingHistoryFilePath()) || _dmHistory.isNull())
	{
		if (ParamOp::extractVariantTest(_currentID, _dmHistory, "CURRENT_RECORD_ID"))
		{
			if (ParamOp::extractVariantTest(_id_History, _dmHistory, _currentID.toString()))
			{
				ParamOp::mergeValue(_id_History, _value, _key);
				ParamOp::mergeValue(_dmHistory, _id_History, _currentID.toString());
				ParamOp::mergeValue(_dmHistory, QVariant(), QString(), -1, BCPwareFileSystem::printingHistoryFilePath());
			}
		}
	}
	

	
}

//void ParamOp::updateSliceSettingToHistory(bool newOrNot, QString _key, QVariant _value)
//{
//	QVariant _dmHistory;
//	QVariant _currentID, _id_History;
//	QVariant _sliceSettingMap;
//	QVariant _sliceSettingMapNew = QVariantMap();
//	if (ParamOp::extractVariantTest(_dmHistory, QVariant(), QString(), -1, BCPwareFileSystem::printingHistoryFilePath()) || _dmHistory.isNull())
//	{
//		if (ParamOp::extractVariantTest(_currentID, _dmHistory, "CURRENT_RECORD_ID"))
//		{
//			if (ParamOp::extractVariantTest(_id_History, _dmHistory, _currentID.toString()))
//			{
//				if (ParamOp::extractVariantTest(_sliceSettingMap, _id_History, "SLICE_SETTING"))
//				{
//					ParamOp::mergeValue(_sliceSettingMap, _value, _key);
//					ParamOp::mergeValue(_id_History, _sliceSettingMap, "SLICE_SETTING");
//					ParamOp::mergeValue(_dmHistory, _id_History, _currentID.toString());
//					ParamOp::mergeValue(_dmHistory, QVariant(), QString(), -1, BCPwareFileSystem::printingHistoryFilePath());
//				}
//				else
//				{
//					ParamOp::mergeValue(_sliceSettingMapNew, _value, _key);
//					ParamOp::mergeValue(_id_History, _sliceSettingMapNew, "SLICE_SETTING");
//					ParamOp::mergeValue(_dmHistory, _id_History, _currentID.toString());
//					ParamOp::mergeValue(_dmHistory, QVariant(), QString(), -1, BCPwareFileSystem::printingHistoryFilePath());
//				}
//			}
//		}
//	}
//
//
//
//}

void ParamOp::updateSliceSettingToHistory(bool newOrNot, int listNum, QVariant _value)
{
	QVariant _dmHistory;
	QVariant _currentID, _id_History;
	QVariant _sliceSettingList;
	QVariant _sliceSettingListNew = QVariantList();
	if (ParamOp::extractVariantTest(_dmHistory, QVariant(), QString(), -1, BCPwareFileSystem::printingHistoryFilePath()) || _dmHistory.isNull())
	{
		if (ParamOp::extractVariantTest(_currentID, _dmHistory, "CURRENT_RECORD_ID"))
		{
			if (ParamOp::extractVariantTest(_id_History, _dmHistory, _currentID.toString()))
			{
				if (ParamOp::extractVariantTest(_sliceSettingList, _id_History, "SLICE_SETTING"))
				{
					ParamOp::mergeValue(_sliceSettingList, _value, QString(),listNum);
					ParamOp::mergeValue(_id_History, _sliceSettingList, "SLICE_SETTING");
					ParamOp::mergeValue(_dmHistory, _id_History, _currentID.toString());
					ParamOp::mergeValue(_dmHistory, QVariant(), QString(), -1, BCPwareFileSystem::printingHistoryFilePath());
				}
				else
				{
					ParamOp::mergeValue(_sliceSettingListNew, _value, QString(), listNum);
					ParamOp::mergeValue(_id_History, _sliceSettingListNew, "SLICE_SETTING");
					ParamOp::mergeValue(_dmHistory, _id_History, _currentID.toString());
					ParamOp::mergeValue(_dmHistory, QVariant(), QString(), -1, BCPwareFileSystem::printingHistoryFilePath());
				}
			}
		}
	}



}
ParamOp::~ParamOp()
{
}
