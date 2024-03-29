#include "define_some_parameter.h"

#include <QTextStream>
#include <QDebug>




QString const BCPwareFileSystem::encryptKey()
{  return "abcdefg"; 
}
QString const BCPwareFileSystem::parameterFilePath()
{
	return BCPwareFileSystem::documentDir().filePath(parameterFileName());
}
QString const BCPwareFileSystem::parameterFileName()
{
	return "parameter_setting.zxb";
}
QString const BCPwareFileSystem::appName(){
	
	return "XYZprint AM³ᴰᴾ"; 
}
QString const BCPwareFileSystem::appNameDoc(){

	return "XYZprint AM-3DP";
}
QString const BCPwareFileSystem::dashboardName(){

	return "DashboardLogs";
}
QDir BCPwareFileSystem::documentDir()
{
	QString path = QStandardPaths::locate(QStandardPaths::DocumentsLocation, appNameDoc(), QStandardPaths::LocateDirectory);
	if (path.isEmpty())
	{
		path = QStandardPaths::locate(QStandardPaths::DocumentsLocation, "", QStandardPaths::LocateDirectory);
		if (!path.isEmpty())
		{
			if (QDir(path).mkpath(appNameDoc()))
			{
				path.append("/").append(appNameDoc());
			}
		}
	}
	return QDir(path);
}
QDir BCPwareFileSystem::documentDashboardDir()
{
	QString path = QStandardPaths::locate(QStandardPaths::DocumentsLocation, dashboardName(), QStandardPaths::LocateDirectory);
	if (path.isEmpty())
	{
		path = QStandardPaths::locate(QStandardPaths::DocumentsLocation, "", QStandardPaths::LocateDirectory);
		if (!path.isEmpty())
		{
			if (QDir(path).mkpath(dashboardName()))
			{
				path.append("/").append(dashboardName());
			}
		}
	}
	return QDir(path);
}


QDir BCPwareFileSystem::logDir()
{
	QDir logDir(documentDir());
	if (logDir.exists("log") || logDir.mkpath("log"))
	{
		logDir.cd("log");
	}
	return logDir;
	
}
QString const BCPwareFileSystem::printingHistoryFilePath()
{
	return documentDashboardDir().filePath("Print_History_ALL.log");
}
//QString BCPwareFileSystem::logFileName()
//{
//	return BCPwareFileSystem::logDir().filePath("BCPware" + QDateTime::currentDateTime().toString("yyyy_MM_dd") + ".log");
//}

QDir BCPwareFileSystem::projectThumbnailFolder()
{
	QDir thumbnailFolder(documentDir());
	if (thumbnailFolder.exists("PjTm") || thumbnailFolder.mkpath("PjTm"))
	{
		thumbnailFolder.cd("PjTm");
	}
	return thumbnailFolder;
}



bool BCPwareFileSystem::encryptParam(QString inputString, QString &outputString)
{
	QAESEncryption *encryption = new QAESEncryption(QAESEncryption::AES_128, QAESEncryption::CBC, QAESEncryption::PKCS7);
	QByteArray hashKey = QCryptographicHash::hash(encryptKey().toLocal8Bit(), QCryptographicHash::Sha256);

	QByteArray iv;
	iv.resize(16);
	for (int i = 0; i < 16; i++)
		iv[i] = 0x00;


	QByteArray encodeText = encryption->encode(inputString.toUtf8(), hashKey, iv);
	outputString = QString(encodeText.toHex());
	
	return true;
	
}
bool BCPwareFileSystem::encryptParam(QString inputString, QFileInfo filepath)
{
	QAESEncryption *encryption = new QAESEncryption(QAESEncryption::AES_128, QAESEncryption::CBC, QAESEncryption::PKCS7);
	QByteArray hashKey = QCryptographicHash::hash(encryptKey().toLocal8Bit(), QCryptographicHash::Sha256);

	QByteArray iv;
	iv.resize(16);
	for (int i = 0; i < 16; i++)
		iv[i] = 0x00;




	QByteArray encodeText = encryption->encode(inputString.toUtf8(), hashKey, iv);
	QString outputString = QString(encodeText.toHex());


	//=====Test ouput hex to file
	QByteArray test = encodeText.toHex();
	qDebug() << "list file Size : " << encodeText.size();
	/*QStringList temptest;
	for (char c : encodeText) {
		temptest << QString("0x%1").arg((ushort)c, 2, 16, QChar{ '0' });
	}

	QFile outfile22("C:/Users/TB495076/Documents/XYZprint AM-3DP/outputfile.txt");
	
	if (outfile22.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
	{
		QTextStream stream(&outfile22);
		int i = 1; 
		foreach(QString listString, temptest)
		{
			stream << listString << ",";
			if (i % 10 == 0)
				stream << endl;
			i++;
		}
		outfile22.close();
		
	}*/
	
	//=============================================================================================



	if (outputString.isEmpty())
	{
		qWarning() << "ouputString is empty";
		return false;
	}


	QFile outfile(filepath.absoluteFilePath());

	if (outfile.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
	{
		QTextStream stream(&outfile);
		stream << outputString << endl;
		outfile.close();
		return true;
	}
	else return false;



	

}
bool BCPwareFileSystem::decodeParam(QString &outputString, QString inputString,QString filePath )
{
	QAESEncryption *encryption = new QAESEncryption(QAESEncryption::AES_128, QAESEncryption::CBC, QAESEncryption::PKCS7);
	QByteArray hashKey = QCryptographicHash::hash(encryptKey().toLocal8Bit(), QCryptographicHash::Sha256);

	QByteArray iv;
	iv.resize(16);
	for (int i = 0; i < 16; i++)
		iv[i] = 0x00;

	if (!filePath.isEmpty())
	{
		QFile outfile(filePath);
		if (outfile.exists())
		{
			//if (!outfile.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
			if (!outfile.open(QIODevice::ReadWrite))
			{
				qDebug() << "file open Error";
			}
			QTextStream stringStream(&outfile);
			
			QString fileString = stringStream.readAll();
			if (fileString.isEmpty())
				return false;
			outfile.close();
			QByteArray testbya = QByteArray::fromHex(fileString.toLatin1());
			//outputString = QString(encryption->removePadding(encryption->decode(testbya, hashKey, iv)));
			outputString = QString((encryption->decode(testbya, hashKey, iv)));
			if (outputString.isEmpty())return false;
			return true;
		}return false;

	}else if (!inputString.isEmpty())
	{ 
		QByteArray testbya = QByteArray::fromHex(inputString.toLatin1());
		//outputString = QString(encryption->removePadding(encryption->decode(testbya, hashKey, iv)));
		outputString = QString((encryption->decode(testbya, hashKey, iv)));
		return true;
	}

}
