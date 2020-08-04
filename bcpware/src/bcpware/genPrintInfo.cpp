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
#include "genPrintInfo.h"
#include <QDateTime>
#include "zxparser.h"





GenPrintInfo::GenPrintInfo(QString *_proejectName) :proejectName(_proejectName)
{

}
void GenPrintInfo::genAllParamInfo(RichParameterSet &param)
{
	QFile outfile(GenPrintInfo::docPath() + PicaApplication::appNameDOC() + "/printAllInfo.txt");

	QVariantMap firstFloor;
	QVariantMap secondFloor;
	QVariantMap thirdFloor;
	
	QString jsonString;
	QJsonDocument jsonDoc;

	if (outfile.exists())
	{
		if (!outfile.open(QIODevice::ReadOnly))
		{
			qDebug() << "file open Error";
		}
		QTextStream jsonStream(&outfile);
		jsonString = jsonStream.readAll();
		QJsonParseError error;
		jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8(), &error);
		if (error.error == QJsonParseError::NoError)
			firstFloor = jsonDoc.toVariant().toMap();

		outfile.close();
	}

	foreach(RichParameter *curpar, param.paramList)
	{
		if (curpar->name == QString("FAN_SPEED")){ secondFloor.insert(curpar->name, QString::number(curpar->val->getInt())); }
		else if (curpar->name == QString("PUMP_VALUE")){ secondFloor.insert(curpar->name, QString::number(curpar->val->getInt())); }
		else if (curpar->name == QString("STIFF_PRINT_VALUE")){ secondFloor.insert(curpar->name, QString::number(curpar->val->getInt())); }
		else if (curpar->name == QString("COLOR_PROFILE")){ secondFloor.insert(curpar->name, curpar->val->getString()); }
		//else if (curpar->name == QString("PAGE_SIZE")){ secondFloor.insert(curpar->name, QString::number(curpar->val->getEnum())); }

	}	

	/*int DP3 = param.getEnum("TARGET_PRINTER");
	if (DP3 == 1)
		secondFloor.insert("PartPro350 XBC", thirdFloor);
	else if (DP3 == 0)
		secondFloor.insert("Palette", thirdFloor);*/

	firstFloor.insert(QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss"), secondFloor);
	//firstFloor.insert(QString("total_pages"), QString::number(test_count));

	jsonDoc = QJsonDocument::fromVariant(firstFloor);
	outfile.open(QIODevice::WriteOnly);
	QTextStream outtext(&outfile);
	outtext << QString(jsonDoc.toJson());

	outfile.close();	

}





void GenPrintInfo::getParamfromJsonFile(RichParameterSet &paramGet)
{
	int test_count = 0;
	QVariantMap firstFloor;
	QString jsonString;
	QJsonDocument jsonDoc;

	QFile outfile(GenPrintInfo::docPath() + PicaApplication::appNameDOC() + "/printAllInfo.txt");
	if (outfile.exists())
	{
		if (!outfile.open(QIODevice::ReadOnly))
		{
			qDebug() << "file open Error";
		}
		QTextStream jsonStream(&outfile);
		jsonString = jsonStream.readAll();
		QJsonParseError error;
		jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8(), &error);
		if (error.error == QJsonParseError::NoError)
			firstFloor = jsonDoc.toVariant().toMap();

		//********test count execute layers*********************//
		QMapIterator<QString, QVariant> ii(jsonDoc.toVariant().toMap());
		while (ii.hasNext())
		{
			ii.next();
			QMapIterator<QString, QVariant> ii2(ii.value().toMap());
			while (ii2.hasNext())
			{
				ii2.next();
				if (ii2.key().contains("FAN_SPEED")){paramGet.setValue("FAN_SPEED", IntValue(ii2.value().toInt()));	}
				else if(ii2.key().contains("PUMP_VALUE")){ paramGet.setValue("FAN_SPEED", IntValue(ii2.value().toInt())); }
				else if(ii2.key().contains("STIFF_PRINT_VALUE")){ paramGet.setValue("FAN_SPEED", IntValue(ii2.value().toInt())); }
				else if (ii2.key().contains("COLOR_PROFILE")){ paramGet.setValue("FAN_SPEED", StringValue(ii2.value().toString())); }
				
			}
		}
		outfile.close();
		firstFloor.insert(QString("total_pages"), QString::number(test_count));
		jsonDoc = QJsonDocument::fromVariant(firstFloor);
		outfile.open(QIODevice::WriteOnly);
		QTextStream outtext(&outfile);
		outtext << QString(jsonDoc.toJson());
		outfile.close();
		//************************************
	}
}



void GenPrintInfo::genZxInfo(MeshDocument &md, RichParameterSet &printParam, PrinterJobInfo &pji)
{
	Print_Job_Information pjii = pji.getPJI();

	QVariantMap firstFloor;
	QVariantMap secondFloor;
	QVariantMap thirdFloor;

	QString jsonString;
	QJsonDocument jsonDoc;

	QFile outfile(GenPrintInfo::docPath() + PicaApplication::appNameDOC() + "/printInfo.txt");

	if (outfile.exists())
	{
		if (!outfile.open(QIODevice::ReadOnly))
		{
			qDebug() << "file open Error";
		}
		QTextStream jsonStream(&outfile);
		jsonString = jsonStream.readAll();
		QJsonParseError error;
		jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8(), &error);
		if (error.error == QJsonParseError::NoError)
			firstFloor = jsonDoc.toVariant().toMap();

		outfile.close();
	}

	foreach(RichParameter *curpar, printParam.paramList)
	{
		if (curpar->name == QString("slice_height")){ thirdFloor.insert(curpar->name, QString::number(curpar->val->getFloat())); }
		else if (curpar->name == QString("USELESS_PRINT")){ thirdFloor.insert(curpar->name, QString::number(curpar->val->getFloat())); }
		else if (curpar->name == QString("Job_Wipe")){ thirdFloor.insert(curpar->name, QString::number(curpar->val->getInt())); }
		else if (curpar->name == QString("COLOR_PROFILE")){ thirdFloor.insert(curpar->name, curpar->val->getString()); }
		//else if (curpar->name == QString("PAGE_SIZE")){ secondFloor.insert(curpar->name, QString::number(curpar->val->getEnum())); }
		
	}

	thirdFloor.insert(pjii.numoflayer.label, pjii.numoflayer.value);
	thirdFloor.insert(pjii.modelNumber.label, pjii.modelNumber.value);
	thirdFloor.insert(pjii.estimatedBinderUsage.label, QString::number(pjii.estimatedBinderUsage.value, 'g', 3));
	thirdFloor.insert(pjii.estimatedBuildTime.label, pjii.estimatedBuildTime.time);

	int DP3 = printParam.getEnum("TARGET_PRINTER");
	if (DP3 == 1)
		secondFloor.insert("PartPro350 XBC", thirdFloor);
	else if (DP3 == 0)
		secondFloor.insert("Palette", thirdFloor);

	firstFloor.insert(QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss"), secondFloor);
	//firstFloor.insert(QString("total_pages"), QString::number(test_count));

	jsonDoc = QJsonDocument::fromVariant(firstFloor);
	outfile.open(QIODevice::WriteOnly);
	QTextStream outtext(&outfile);
	outtext << QString(jsonDoc.toJson());

	outfile.close();




}
void GenPrintInfo::genPrinterInfo(RichParameterSet &printParam, QMap<QString, QString> &printerInfo)
{

	bool layerPrintMode = printParam.getBool("STIFF_PRIN_V2");
	float unit = printParam.getFloat("SLIGHT_HEIGHT");

	if (!layerPrintMode)
		printerInfo.insert("Print_Mode","1Layer1Page");		
	else
		printerInfo.insert("Print_Mode", "1Layer2Page");
	printerInfo.insert("Layer_Height", QString::number(int(unit*10000)));

	//TBD Add DM All Slicer Parameter
	//printerInfo.insert("Page_Size", printParam.getBool("STIFF_PRIN_V2"));

}
void GenPrintInfo::genPrinterInfoDM(RichParameterSet &printParam, QList<QPair<QString, QVariant>> &printerInfo, PrinterJobInfo &pji)
{
	Print_Job_Information pjii = pji.getPJI();
	bool layerPrintMode = printParam.getBool("STIFF_PRIN_V2");
	float unit = printParam.getFloat("SLIGHT_HEIGHT");

	if (!layerPrintMode)
		printerInfo.append(QPair<QString, QVariant>("Print_Mode", "1Layer1Page"));
	else
		printerInfo.append(QPair<QString, QVariant>("Print_Mode", "1Layer2Page"));
	printerInfo.append(QPair<QString, QVariant>("Layer_Height", QString::number(int(unit * 10000))));

	//TBD Add DM All Slicer Parameter
	printerInfo.append(QPair<QString, QVariant>("Page_Size", printParam.getEnum("PAGE_SIZE")));

	printerInfo.append(QPair<QString, QVariant>("SHELL_THICK", printParam.getFloat("DM_SHELL_HORIZONTAL_THICKNESS")));
	printerInfo.append(QPair<QString, QVariant>("SHELL_C", printParam.getInt("DM_HORIZONTAL_C_Value")));
	printerInfo.append(QPair<QString, QVariant>("SHELL_M", printParam.getInt("DM_HORIZONTAL_M_Value")));
	printerInfo.append(QPair<QString, QVariant>("SHELL_Y", printParam.getInt("DM_HORIZONTAL_Y_Value")));
	printerInfo.append(QPair<QString, QVariant>("SHELL_K", printParam.getInt("DM_HORIZONTAL_K_Value")));

	printerInfo.append(QPair<QString, QVariant>("TOP_THICK", printParam.getFloat("DM_SHELL_VERTICAL_THICKNESS")));
	printerInfo.append(QPair<QString, QVariant>("TOP_C", printParam.getInt("DM_SHELL_TOP_C_Value")));
	printerInfo.append(QPair<QString, QVariant>("TOP_M_", printParam.getInt("DM_SHELL_TOP_M_Value")));
	printerInfo.append(QPair<QString, QVariant>("TOP_Y", printParam.getInt("DM_SHELL_TOP_Y_Value")));
	printerInfo.append(QPair<QString, QVariant>("TOP_K", printParam.getInt("DM_SHELL_TOP_K_Value")));

	printerInfo.append(QPair<QString, QVariant>("BOTTOM_THICK", printParam.getFloat("DM_SHELL_BOTTOM_LIGHTER")));
	printerInfo.append(QPair<QString, QVariant>("BOTTOM_C", printParam.getInt("DM_SHELL_BOTTOM_C_Value")));
	printerInfo.append(QPair<QString, QVariant>("BOTTOM_M", printParam.getInt("DM_SHELL_BOTTOM_M_Value")));
	printerInfo.append(QPair<QString, QVariant>("BOTTOM_Y", printParam.getInt("DM_SHELL_BOTTOM_Y_Value")));
	printerInfo.append(QPair<QString, QVariant>("BOTTOM_K", printParam.getInt("DM_SHELL_BOTTOM_K_Value")));
	
	printerInfo.append(QPair<QString, QVariant>("INNER_C", printParam.getInt("DM_INNER_C_Value")));
	printerInfo.append(QPair<QString, QVariant>("INNER_M", printParam.getInt("DM_INNER_M_Value")));
	printerInfo.append(QPair<QString, QVariant>("INNER_Y", printParam.getInt("DM_INNER_Y_Value")));
	printerInfo.append(QPair<QString, QVariant>("INNER_K", printParam.getInt("DM_INNER_K_Value")));

	printerInfo.append(QPair<QString, QVariant>("SOFTWARE_VERSION", DSP_SOFTWARE_VERSION));
	printerInfo.append(QPair<QString, QVariant>("ICM", printParam.getString("DM_ICM_FOR_DITHER")));
	printerInfo.append(QPair<QString, QVariant>("Number_Of_Layer", pjii.numoflayer.value));
	
	//printerInfo.insert("DSP_SOFTWARE_VERSION", DSP_SOFTWARE_VERSION);

	




}


double GenPrintInfo::estimateInkUsageAllFiveChannel(RichParameterSet &printParam, PrinterJobInfo &pji)
{
	Print_Job_Information pjii = pji.getPJI();

	int SlicingMode = printParam.getEnum("DM_SLICING_MODE");
	switch (SlicingMode)
	{
	case 2:
	{
		bool layerPrintMode = printParam.getBool("DM_TWICE_IN_ONE_LAYER");
		float unit = printParam.getFloat("SLIGHT_HEIGHT");
		double totalMeshVolume = pjii.volumn.value;
		double inkDropVolumn = 1.2e-9;//ml	
		double oneMM_Cubic = pow((1600 / 2.54), 2) * (10. / unit)* inkDropVolumn;
		int inside_C = printParam.getInt("DM_INNER_C_Value");
		//need convert equation
		
		
		double oneChannel_InkUsage = oneMM_Cubic*totalMeshVolume*(inside_C / 255.);

		return 5* oneChannel_InkUsage;



	}
		break;
	}
}
//estiamte Usage
//case reorg five channels
//
//case 
//case 

void GenPrintInfo::genPrinterInfoDMJson(RichParameterSet &printParam, PrinterJobInfo &pji, QString jsonFileName, QString thumbnailFileName, QString jobModelFileName, QString zxFileName)
{

	Print_Job_Information pjii = pji.getPJI();

	bool layerPrintMode = printParam.getBool("DM_TWICE_IN_ONE_LAYER");
	float unit = printParam.getFloat("SLIGHT_HEIGHT");

	//estimate Ink Usage
	double totalMeshVolume = pjii.volumn.value;
	double inkDropVolumn = 1.2e-9;//ml	
	double oneMM_Cubic = pow((1600 / 2.54), 2) * (10. / unit)* inkDropVolumn;	
	int inside_C = printParam.getInt("DM_INNER_C_Value");
	double oneChannel_InkUsage = oneMM_Cubic*totalMeshVolume*(inside_C/255.);
	double fiveChannel_InkUsage = 5 * oneChannel_InkUsage;

	/*QFileInfo zxFileInfo(jsonFileName);
	QString zxFileaName =zxFileInfo.absolutePath() +"/" +zxFileInfo.completeBaseName() + ".zx";*/
	ZxParser *zx = new ZxParser();
	int pageCount = zx->getZxNPage(zxFileName.toStdString().c_str());

	QVariantMap firstFloor;
	QVariantMap secondFloorPrinter, secondFloorSlicerParam;
	QVariantMap thirdFloorShell, thirdFloorTop, thirdFloorBottom, thirdFloorInside;	
	QJsonDocument jsonDoc;

	thirdFloorShell.insert("thickness", printParam.getFloat("DM_SHELL_HORIZONTAL_THICKNESS"));
	thirdFloorShell.insert("C", printParam.getInt("DM_HORIZONTAL_C_Value"));
	thirdFloorShell.insert("M", printParam.getInt("DM_HORIZONTAL_M_Value"));
	thirdFloorShell.insert("Y", printParam.getInt("DM_HORIZONTAL_Y_Value"));
	thirdFloorShell.insert("K", printParam.getInt("DM_HORIZONTAL_K_Value"));

	thirdFloorTop.insert("thickness", printParam.getFloat("DM_SHELL_VERTICAL_THICKNESS"));
	thirdFloorTop.insert("C", printParam.getInt("DM_SHELL_TOP_C_Value"));
	thirdFloorTop.insert("M", printParam.getInt("DM_SHELL_TOP_M_Value"));
	thirdFloorTop.insert("Y", printParam.getInt("DM_SHELL_TOP_Y_Value"));
	thirdFloorTop.insert("K", printParam.getInt("DM_SHELL_TOP_K_Value"));

	thirdFloorBottom.insert("thickness", printParam.getFloat("DM_SHELL_BOTTOM_LIGHTER"));
	thirdFloorBottom.insert("C", printParam.getInt("DM_SHELL_BOTTOM_C_Value"));
	thirdFloorBottom.insert("M", printParam.getInt("DM_SHELL_BOTTOM_M_Value"));
	thirdFloorBottom.insert("Y", printParam.getInt("DM_SHELL_BOTTOM_Y_Value"));
	thirdFloorBottom.insert("K", printParam.getInt("DM_SHELL_BOTTOM_K_Value"));

	
	thirdFloorInside.insert("C", printParam.getInt("DM_INNER_C_Value"));
	thirdFloorInside.insert("M", printParam.getInt("DM_INNER_M_Value"));
	thirdFloorInside.insert("Y", printParam.getInt("DM_INNER_Y_Value"));
	thirdFloorInside.insert("K", printParam.getInt("DM_INNER_K_Value"));
	
	
	
	
	if (!layerPrintMode)
		secondFloorPrinter.insert("Print_Mode", "1Layer1Page");
	else
		secondFloorPrinter.insert("Print_Mode", "1Layer2Page");
	secondFloorPrinter.insert("Page_Mode", "");
	secondFloorPrinter.insert("Page_Count", pageCount);
	secondFloorPrinter.insert("Job_Height", "");
	secondFloorPrinter.insert("Layer_Height", QString::number(int(unit * 10000)));

	secondFloorSlicerParam.insert("iccfilename", printParam.getString("DM_ICM_FOR_DITHER"));
	

	secondFloorSlicerParam.insert("Shell", thirdFloorShell);
	secondFloorSlicerParam.insert("Top", thirdFloorTop);
	secondFloorSlicerParam.insert("Bottom", thirdFloorBottom);
	secondFloorSlicerParam.insert("Inside", thirdFloorInside);


	firstFloor.insert("version", DSP_SOFTWARE_VERSION);
	
	firstFloor.insert("thumbnail", thumbnailFileName);
	firstFloor.insert("model", jobModelFileName);
	firstFloor.insert("document", zxFileName);
	
	firstFloor.insert("type", "dm-shop zxa");
	firstFloor.insert("slicer", "dm-make 1.0.0");
	firstFloor.insert("printer", secondFloorPrinter);
	firstFloor.insert("Slicer_Parameters", secondFloorSlicerParam);
	firstFloor.insert("Ink_Usage", QString::number(fiveChannel_InkUsage, 'g', 2));

	jsonDoc = QJsonDocument::fromVariant(firstFloor);

	QString jsonString = jsonDoc.toJson();

	QFile outfile(jsonFileName);

	if (outfile.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
	{
		QTextStream stream(&outfile);
		stream << jsonString << endl;
		outfile.close();
	}


}



void GenPrintInfo::count_total_Page()
{
	int test_count = 0;
	QVariantMap firstFloor;
	QString jsonString;
	QJsonDocument jsonDoc;

	QFile outfile(GenPrintInfo::docPath() + PicaApplication::appNameDOC() + "/printInfo.txt");
	if (outfile.exists())
	{
		if (!outfile.open(QIODevice::ReadOnly))
		{
			qDebug() << "file open Error";
		}
		QTextStream jsonStream(&outfile);
		jsonString = jsonStream.readAll();
		QJsonParseError error;
		jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8(), &error);
		if (error.error == QJsonParseError::NoError)
			firstFloor = jsonDoc.toVariant().toMap();

		//********test count execute layers*********************//
		QMapIterator<QString, QVariant> ii(jsonDoc.toVariant().toMap());
		while (ii.hasNext())
		{
			ii.next();
			QMapIterator<QString, QVariant> ii2(ii.value().toMap());
			while (ii2.hasNext())
			{
				ii2.next();
				QMapIterator<QString, QVariant> ii3(ii2.value().toMap());
				while (ii3.hasNext())
				{
					ii3.next();
					if (ii3.key().contains("Number Of Layer"))
						test_count += ii3.value().toString().toFloat();
					qDebug() << ii3.key() << ": " << ii3.value() << endl;
				}
			}
		}
		outfile.close();
		firstFloor.insert(QString("total_pages"), QString::number(test_count));
		jsonDoc = QJsonDocument::fromVariant(firstFloor);
		outfile.open(QIODevice::WriteOnly);
		QTextStream outtext(&outfile);
		outtext << QString(jsonDoc.toJson());
		outfile.close();
		//************************************
	}

}

GenPrintInfo::~GenPrintInfo()
{
}
