#pragma once
#include <QString>
#include <QStandardPaths>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QJsonDocument>
#include <QDebug>
#include <QMap>
#include <../common/filterparameter.h>
#include "../common/define_some_parameter.h"
//#include "ParamStuff.h"

#define JOB_QUEUE_FILE_NAME "job_queue.txt"
#define DM_PARAM_SETTING "PRINTER_PARAM.txt"
#define WIPER_INIT_COUNT 430
struct JobInfo
{
	//QDateTime Pr
	int serialNum;
	//QString jobFileName;
	QFileInfo *fI;
	bool printed;
	JobInfo()
	{
		printed = false;
		serialNum = -1;
		fI = new QFileInfo();
	}
	JobInfo(QString fileName, int sn)
	{
		fI = new QFileInfo(fileName);
		//jobFileName = fI->fileName();
		serialNum = sn;
		printed = false;
	}

	JobInfo& operator =(const JobInfo& a)
	{
		serialNum = a.serialNum;
		printed = a.printed;
		fI = a.fI;
		return *this;
	}

};
#define Wiper_Record_FileName "Wiper_Record.txt"

class ParamOp
{
public:
	ParamOp();
	~ParamOp();


	//static void genAllParamInfo(RichParameterSet &param);
	//static void getParamfromJsonFile(RichParameterSet &paramGet);
	static bool getJsonFiletoString(QString &jsonString, QString jsonFileName);
	static bool getJsonFiletoString(QString &jsonString);
	static QVariant getValueFromJsonSet(QString &jsonString, QString firstFloor, QString secondFloor);
	static bool setValueToJsonSet(QString &jsonString, QString secondFloor, QVariant value);
	static bool saveJsonToFile(QString jsonString);
	static bool saveJsonToFile(QString jsonString, QString saveFileName);
	static bool saveStringToFileWithPath(QString jsonString, QString saveFileName);
	static bool saveStringToFileWithPath(QByteArray jsonString, QString saveFileName);
	


	static QString docPath() { return QStandardPaths::locate(QStandardPaths::DocumentsLocation, "", QStandardPaths::LocateDirectory); }

	static QString createJsonStringCommand(QString floor1, QString floor2, QStringList floor3);
	static bool exTractJsonStringCommand(QString, QString &, QString &, QStringList &);

	//Wiper 
	//static void updateWiperCount(int minus);
	//static bool createWiperMap(QVariantMap &, bool used, QString WiperID, int countINit);
	//static bool createWiperMap(QVariantMap &, int countINit);
	


	//ZXA format

	static bool extractVariantTest(QVariant &output, QVariant input = QVariant(), QString mapKey = QString(), int listID = -1, QString docfileName = QString(), QString jsonString = QString());
	static bool mergeValue(QVariant &valueMerge, QVariant insertValue = QVariant(), QString key = QString(), int replacenum = -1, QString docFileName = QString());

	static bool transformJsonToRichParameter(RichParameterSet &input, QString josnString);
	
	//static void createDefaultParamJosonDoc();
	static void updateToHistory(bool newOrNot, QString _key, QVariant _value);
	//static void updateSliceSettingToHistory(bool newOrNot, QString _key, QVariant _value);
	static void updateSliceSettingToHistory(bool newOrNot, int, QVariant _value);
	

};

