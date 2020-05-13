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
	
	return "BCPware"; 
}
QDir BCPwareFileSystem::documentDir()
{
	QString path = QStandardPaths::locate(QStandardPaths::DocumentsLocation, appName(), QStandardPaths::LocateDirectory);
	if (path.isEmpty())
	{
		path = QStandardPaths::locate(QStandardPaths::DocumentsLocation, "", QStandardPaths::LocateDirectory);
		if (!path.isEmpty())
		{
			if (QDir(path).mkpath(appName()))
			{
				path.append("/").append(appName());
			}
		}
	}
	return QDir(path);
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

	QFile outfile(filepath.absoluteFilePath());

	if (outfile.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
	{
		QTextStream stream(&outfile);
		stream << outputString << endl;
		outfile.close();
	}
	return true;



	return true;

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
			outfile.close();
			QByteArray testbya = QByteArray::fromHex(fileString.toLatin1());
			outputString = QString(encryption->removePadding(encryption->decode(testbya, hashKey, iv)));
			return true;
		}return false;

	}else
	if (!inputString.isEmpty())
	{ 
		QByteArray testbya = QByteArray::fromHex(inputString.toLatin1());
		outputString = QString(encryption->removePadding(encryption->decode(testbya, hashKey, iv)));
		return true;
	}

}
