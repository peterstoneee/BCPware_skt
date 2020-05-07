#pragma once
#include <QObject>
#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QByteArray>
#include <QHash>
#include <QList>
#include <QByteArray>
#include <QDir>
#include <QAbstractListModel>
#include <QMap>
#include <QMapIterator>

#include <QApplication>

#include <QString>
#include <QStandardPaths>
#include <QTextStream>
#include <QJsonDocument>
#include <QThread>
#include <vector>
#include "./Comm3DP.h"
#include "./ParamOp.h"

#include "mainwindow.h"

#define NUMOFNVM1  7
#define NUMOFNVM2  15
#define NUMOFNVM3  45
#define NUMOFNVM4  25
#define NUMOFNVM5  26


class CMD_Value:public QObject
{
	Q_OBJECT
		Q_ENUMS(CMD_Description)
public:
	Q_PROPERTY(QString cmdRecord MEMBER commandRecord)
		QString commandRecord;
	CMD_Value(Comm3DP *comm /*Comm3DP_qt *commQt*/, QObject *parent = nullptr);
	//init without value
	//CMDValue();


	CMD_Value();


	~CMD_Value(){}

	Comm3DP *comm;
	
	QString jsonInputString;
	struct CMD_value
	{
		enum CMD_TYPE{ Motion, Board_Param, Sirius_Function, NVM_Setting, NVM_Reading, Common, Header_Param, PrintModeParam };

		QString CMD;
		QVariantList valueList;
		CMD_TYPE type;
		CMD_value(){}
		CMD_value(QString _cmd, QVariantList _value) { CMD = _cmd; valueList = _value; type = Board_Param; }
		CMD_value(QString _cmd, QVariantList _value, CMD_TYPE _type) { CMD = _cmd; valueList = _value; type = _type; }
	};
	enum CMD_Description
	{
		Begin,
		CMD_Printhead_Fan = Begin,
		CMD_Vibration,
		CMD_Vibration_Value,
		CMD_Builder_Heat,
		CMD_Preheat_Params,
		CMD_Postheat_Params,
		SIRIUS_WiperFrequency,
		CMD_WiperIndex,
		CMD_WiperClicks,
		CMD_PrintMode,
		CMD_FILL_POWDER_RATIO_DURING_PRINTING,

		CMD_FP_BUILDER_UP,
		CMD_FP_BUILDER_DOWN,
		CMD_FP_FEEDER_UP,
		CMD_FP_FEEDER_DOWN,
		CMD_FP_Z_AXIS_STOP,

		CMD_FP_BUILDER_UP_VALUE,
		CMD_FP_BUILDER_DOWN_VALUE,
		CMD_FP_FEEDER_UP_VALUE,
		CMD_FP_FEEDER_DOWN_VALUE,

		CMD_FP_BUILDER_UP_ONE_MM,
		CMD_FP_BUILDER_DOWN_ONE_MM,

		CMD_FP_FEEDER_UP_ONE_MM,
		CMD_FP_FEEDER_DOWN_ONE_MM,


		CMD_FP_FILL_POWDER_START,
		CMD_FP_FILL_POWDER_END,
		CMD_FP_CLEAN_PH_MODULE_START,
		CMD_FP_CLEAN_PH_MODULE_END,
		CMD_FP_CLEAN_ROLLER_START,
		CMD_FP_CLEAN_PH_ROLLER_BACK,
		CMD_FP_ROLLER_MOVE_MIDDLE,
		CMD_FP_ROLLER_MOVE_MIDDLE_END,
		CMD_FP_FEEDER_DOWN_TO_PRINT_PAGE_POSITION,
		CMD_FP_CHANGE_WIPER,
		CMD_FP_MOVE_ITEM_TRAY_STEP_ONE,
		CMD_FP_MOVE_ITEM_TRAY_STEP_TWO,

		CMD_NVM_R1,
		CMD_NVM_W1,

		CMD_NVM_R2,
		CMD_NVM_W2,

		CMD_NVM_R3,
		CMD_NVM_W3,

		CMD_NVM_R4,
		CMD_NVM_W4,

		CMD_NVM_R5,

		CMD_NVM_W5,



		CMD_FP_PC_ON,

		CMD_START_WIPER_CALIBRATION,
		CMD_FP_WIPER_NEXT_STEP,

		CMD_CANCEL_CURING_END,
		CMD_DOOR_LATCH,
		CMD_NORMAL_PRINT,
		CMD_REVERSE_PRINT,
		CMD_NORMAL_AND_REVERSE_PRINT,

		CMD_LONG_PAGE_HEADER_FOR_ONE_PAGE,
		CMD_SHORT_PAGE_HEADER_FOR_ONE_PAGE,

		CMD_QUEEN_START,
		CMD_QUEEN_STOP,
		CMD_ENTER,

		/*CMD_FP_LIST_A,
		CMD_FP_LIST_B,
		CMD_FP_LIST_A,
		CMD_FP_LIST_D,
		CMD_FP_LIST_E,
		CMD_FP_LIST_F,
		CMD_FP_LIST_G,
		CMD_FP_LIST_H,
		CMD_FP_LIST_I,
		CMD_FP_LIST_J,
		CMD_FP_HTBB,*/

		//CMD_SHOW_PRINTING_MOTION,
		END
	};


	QMap<CMD_Description, CMD_value> CMD_valueMap;
	QMap<CMD_Description, QString> CMD_des_string;

	void getValuebyName(QString categoryName, QString paramName, QVariant &value);
	//Q_INVOKABLE QVariant getValuebyName2(QString categoryName, QString paramName);
	Q_INVOKABLE QVariantList getValuebyName2(QString categoryName, int group);

	void gatherAdvanced_SettingName();
	void setValuebyName();
	void init();
	void initDesMap();
	Q_INVOKABLE void updatCMD_valueMap();
	//update void update

	Q_INVOKABLE void sendAllCommand();
	Q_INVOKABLE void sendTestPageSetting();

	Q_INVOKABLE void sendCommand(CMD_Description, QStringList = QStringList(), int = 50);

	Q_INVOKABLE void sendCommandWithValue(CMD_Description, QStringList);


	Q_INVOKABLE bool sendNVMCommand();
	Q_INVOKABLE bool getNVMValue();

	Q_INVOKABLE bool getValueFromUI(QVector<QVariantList>);

	void countCheckSum(QString);
	QString countCheckSumTest(QVariantList, int num);

	static QVariant transformDataFromUIToFPGA(QVariant input, int trasformEnum);
	static QVariant transformDataFromFPGAToUI(QVariant input, int trasformEnum);
private:
	bool initWithValue;
	bool sendall;
	QStringList allCommand;
	QList<QVariant> valueList;
	QVariantList NVM_NameList[5];
	QMap<QVariant, QVariant> NVM_Map;
};

