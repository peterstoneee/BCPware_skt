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
#include "webService.h"
#include <common/picaApplication.h>
#include <QDateTime>
#include <QDate>
#include <QSettings>

webService::webService(QWidget *parent) 
{
	testServerURL.push_back("http://test-vm-02.eastasia.cloudapp.azure.com:9000/api/uploadFilamentUsage");
	testServerURL.push_back("http://test-vm-01.eastasia.cloudapp.azure.com:9000/api/uploadFilamentUsage");
	QSettings settings;
	settings.setValue("uploadTime", QVariant(QString("042018")));
}

bool webService::upload_PrintingLog(QByteArray pData)
{
	try
	{
		QTimer timer;
		timer.setSingleShot(true);

		QByteArray postDataSize = QByteArray::number(pData.size());
		QNetworkAccessManager mgr;

		//QUrl serviceURL("http://test-vm-02.eastasia.cloudapp.azure.com:9000/api/uploadFilamentUsage"); //Test Server
		QUrl serviceURL("http://xyzlog.xyzprinting.com/api/uploadFilamentUsage");
		QNetworkRequest request(serviceURL);

		request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
		request.setRawHeader("Content-Length", postDataSize);

		QNetworkReply * reply = mgr.post(request, pData);
		QEventLoop eventLoop;
		QObject::connect(&timer, SIGNAL(timeout()), &eventLoop, SLOT(quit()));
		QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
		timer.start(3000);   // 30 secs. timeout
		eventLoop.exec();

		if (timer.isActive()) 
		{ //uploading
			timer.stop();
			if (reply->error() == QNetworkReply::NoError) 
			{
				//success
				//                int v = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
				//                if (v >= 200 && v < 300) {  // Success
				//                  ...
				//                }
				qDebug() << "Success" << reply->readAll();
				//delete reply;
				QObject::disconnect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
				reply->abort();
				reply->deleteLater();
				return  0;
			}
			else {
				//failure
				qDebug() << "Failure" << reply->errorString();
				int tReturn = 0;
				if (reply->errorString().indexOf("not found") == -1)
				{
					tReturn = -1;
				}
				else
				{
					tReturn = 0;
				}
				QObject::disconnect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
				reply->abort();
				reply->deleteLater();
				//delete reply;
				return tReturn;
			}
		}
		else {
			// timeout
			//disconnect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
			QObject::disconnect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
			reply->abort();
			reply->deleteLater();
			qDebug() << "Timeout";
			return 0;
		}
	}
	catch (...)
	{
		return 0;
	}
}

void webService::downloadAPI()
{
	QVariant sn("3FM1WPGBXTH59N0BB1"), model("MfgJobPageLog");
	QJsonObject json;

	json.insert("printer_sn", sn.toString());
	json.insert("model", model.toString());
	QJsonDocument jsonDoc(json);
	QString inputStr(jsonDoc.toJson(QJsonDocument::Compact));
	QByteArray encryptdata = encryptString(inputStr);

	QString urll = "http://test-vm-01.eastasia.cloudapp.azure.com:9000/api/getFeedback?log=" + encryptdata;

	QNetworkAccessManager nam;
	QNetworkRequest request;
	request.setUrl(QUrl(urll));
	QNetworkReply *reply = nam.get(request);

	while (!reply->isFinished())
	{
		qApp->processEvents();
	}
	QByteArray response_data = reply->readAll();

}
QByteArray webService::encryptString(QString inputstr)
{
	setKey("@xyzprinting.com");
	QByteArray keyy = key.toUtf8();
	QByteArray iv;
	iv.resize(16);
	for (int i = 0; i < 16; i++)
		iv[i] = 0x00;

	QAESEncryption encryption(QAESEncryption::AES_128, QAESEncryption::CBC, QAESEncryption::PKCS7);
	QByteArray encodeText = encryption.encode(inputstr.toUtf8(), keyy, iv);
	QByteArray ba_as_hex_string = encodeText.toHex();

	return ba_as_hex_string;
}

QString webService::getOSName()
{
	QString osName;
	OSVERSIONINFO os;
	ZeroMemory(&os, sizeof(OSVERSIONINFO));
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&os);
	QString ret = "Windows ";
	if (os.dwMajorVersion == 10)
		ret += "10";
	else if (os.dwMajorVersion == 6){
		if (os.dwMinorVersion == 3)
			ret += "8.1";
		else if (os.dwMinorVersion == 2)
			ret += "8";
		else if (os.dwMinorVersion == 1)
			ret += "7";
		else
			ret += "Vista";
	}
	else if (os.dwMajorVersion == 5){
		if (os.dwMinorVersion == 2)
			ret += "XP SP2";
		else if (os.dwMinorVersion == 1)
			ret += "XP";
	}
	return ret;
}
QString webService::getSWVersion()
{
	return PicaApplication::appVer(); 
}
void webService::createJson()
{

	QVariantMap firstFloor;
	QVariantMap secondFloor;
	QVariantMap thirdFloor;

	QString jsonString;
	QJsonDocument jsonDoc;


	//QFile outfile(GenPrintInfo::docPath() + PicaApplication::appName() + "/printInfo.txt");

	//if (outfile.exists())
	//{
	//	if (!outfile.open(QIODevice::ReadOnly))
	//	{
	//		qDebug() << "file open Error";
	//	}
	//	QTextStream jsonStream(&outfile);
	//	jsonString = jsonStream.readAll();
	//	QJsonParseError error;
	//	jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8(), &error);
	//	if (error.error == QJsonParseError::NoError)
	//		firstFloor = jsonDoc.toVariant().toMap();
	//	outfile.close();
	//}

	//foreach(RichParameter *curpar, printParam.paramList)
	//{
	//	if (curpar->name == QString("slice_height")){ thirdFloor.insert(curpar->name, QString::number(curpar->val->getFloat())); }
	//	else if (curpar->name == QString("USELESS_PRINT")){ thirdFloor.insert(curpar->name, QString::number(curpar->val->getFloat())); }
	//	else if (curpar->name == QString("Job_Wipe")){ thirdFloor.insert(curpar->name, QString::number(curpar->val->getInt())); }
	//	else if (curpar->name == QString("COLOR_PROFILE")){ thirdFloor.insert(curpar->name, curpar->val->getString()); }
	//	//else if (curpar->name == QString("PAGE_SIZE")){ secondFloor.insert(curpar->name, QString::number(curpar->val->getEnum())); }
	//}

	/*thirdFloor.insert(pjii.numoflayer.label, pjii.numoflayer.value);
	thirdFloor.insert(pjii.modelNumber.label, pjii.modelNumber.value);
	thirdFloor.insert(pjii.estimatedBinderUsage.label, QString::number(pjii.estimatedBinderUsage.value, 'g', 3));
	thirdFloor.insert(pjii.estimatedBuildTime.label, pjii.estimatedBuildTime.time);*/

	//int DP3 = printParam.getEnum("TARGET_PRINTER");
	//if (DP3 == 1)
	//	secondFloor.insert("Palette_1_5", thirdFloor);
	//else if (DP3 == 0)
	//	secondFloor.insert("Palette", thirdFloor);

	//firstFloor.insert(QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss"), secondFloor);
	////firstFloor.insert(QString("total_pages"), QString::number(test_count));

	//jsonDoc = QJsonDocument::fromVariant(firstFloor);
	//outfile.open(QIODevice::WriteOnly);
	//QTextStream outtext(&outfile);
	//outtext << QString(jsonDoc.toJson());

	//outfile.close();



}
bool webService::checkUploadMonth()
{
	//get current time
	//get last update month
	QDate cdate = QDate::currentDate();		
	QSettings setting;
	QVariant uptime = setting.value("uploadTime");
	QDate readMY = QDate::fromString(uptime.toString(), "MMyyyy");
	int temp = readMY.daysTo(cdate);
	
	if (temp>62)
	{	
		return true;
	}
		else return false;
	
	
	
	//if ()
}
webService::~webService()
{
}
