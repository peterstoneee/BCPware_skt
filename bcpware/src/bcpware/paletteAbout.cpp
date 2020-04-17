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
#include "paletteAbout.h"
#include "mainwindow.h"

#include "credits.h"
#include <regex>


PaletteAbout::PaletteAbout(QWidget *parent) :QDialog(parent), ui(new Ui::PaletteAbout)
{
	ui->setupUi(this);
	/*QDialog *about_dialog = new QDialog(0, Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
	Ui::PaletteAbout temp;
	temp.setupUi(about_dialog);
	temp.programName->setText(PicaApplication::completeName(PicaApplication::HW_ARCHITECTURE(QSysInfo::WordSize)) + "   (" + __DATE__ + ")");
	///about_dialog->setFixedSize(566,580);
	about_dialog->show();*/
	//ui->programName->setText(PicaApplication::completeName(PicaApplication::HW_ARCHITECTURE(QSysInfo::WordSize)) + "   (" + __DATE__ + ")");
	ui->versionLB->setText(PicaApplication::appVer());
	ui->palette_versionLB->setOpenExternalLinks(true);
	
	connect(ui->ppPB, &QPushButton::clicked, [this]() {
		QDesktopServices::openUrl(QUrl("https://pro.xyzprinting.com/en-US/policy"));
	});


	//connect(ui->testBtn, SIGNAL(clicked()), this, SLOT(checkForUpdates_2()));

	//SYDNY 07/22/2017
	//connect(ui->credit, SIGNAL(clicked()), this, SLOT(credit()));
	//ui->credit->setIcon(QIcon(":/images/icons/btn_credit.png"));
	//ui->bcp_icon->setPixmap(QPixmap(":/images/icons/BCPware_txt.png"));
	initLicenseDia();
	
	httpReq = new QNetworkAccessManager(this);
	connect(httpReq, SIGNAL(finished(QNetworkReply*)), this, SLOT(connectionDone2(QNetworkReply*)));
	connect(ui->licensePB, &QPushButton::pressed, [this]()
	{
		//licenseDia->show();
		QMessageBox msgBox;
		msgBox.setWindowTitle("License");
		msgBox.setFixedSize(459, 700);
		msgBox.setText(
			"This program is free software; you can redistribute it and/or \n" 
			"modify it under the terms of the GNU General Public License \n"
			"as published by the Free Software Foundation; either version \n"
			"2 of the License, or (at your option) any later version.\n\n"

			"This program is distributed in the hope that it will be useful,\n"
			"but WITHOUT ANY WARRANTY; without even the implied\n"
			"warranty of MERCHANTABILITY or FITNESS FOR A\n"
			"PARTICULAR PURPOSE.See the GNU General Public License\n"
			"for more details.\n\n"

			"You should have received a copy of the GNU General Public\n"
			"License along with this program; if not, write to the Free\n"
			"Software Foundation, Inc., 675 Mass Ave, Cambridge, MA\n"
			"02139, USA.");		

		msgBox.setStyleSheet("QMessageBox{"
			"font: 9pt \"Arial\";"
			"border: 0px solid black"
			"}"
			"QTabWidget::pane{ border: 1px solid #646464;}");
		msgBox.setStandardButtons(QMessageBox::Close);		
		int ret = msgBox.exec();


	});
	/*
		disable check for update
	*/
	//checkForUpdates_2();
	/*======================*/
	setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
	setModal(false);
	//dbgff = fopen("C:\\debug_open_1.txt", "w");//@@@
	
}

void PaletteAbout::checkForUpdates_2()
{

	//QNetworkRequest request(QUrl("http://www.xyzprinting.com/api2/CheckForStageUpdates?app=com.xyzprinting.FoodPrinter&os=w,6.1&model=XYZFoodPrinter10.engine,XYZFoodPrinter10.panel"));
	/*QNetworkReply *reply =*/

	//startSendURL(QUrl("http://www.xyzprinting.com/api2/CheckForStageUpdates?app=com.xyzprinting.picasso&os=w,6.1&model="));
	//startSendURL(QUrl("http://www.xyzprinting.com/api2/CheckForStageUpdates?app=com.xyzprinting.BCPware&os=w,6.1"));
	startSendURL(QUrl("http://www.xyzprinting.com/api2/CheckForUpdates?app=com.xyzprinting.BCPware&os=w,6.1"));



}
void PaletteAbout::startSendURL(QUrl url)
{
	QNetworkRequest request(url);
	qDebug() << url;
	QNetworkReply *reply = httpReq->get(request);
	currentDownloads.append(reply);
	connect(currentDownloads.back(), SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(downloadStatus(qint64, qint64)));

}

void PaletteAbout::downloadStatus(qint64 bytesReceived, qint64 bytesTotal)
{
	//qDebug() << "download_process " << (bytesReceived * 100 / bytesTotal) << bytesReceived << bytesTotal;

	//MainWindow::QCallBack((bytesReceived * 100. / bytesTotal), "download_process");
	
		

	

	ui->palette_versionLB->setText(QString::number(bytesReceived * 100. / bytesTotal, 'f', 1) + "%");



}

void PaletteAbout::connectionDone2(QNetworkReply *reply)
{
	ui->palette_versionLB->clear();
	//QString temp = reply->readAll();
	if (reply->peek(26) == "{\"com.xyzprinting.BCPware\"")
	{
		QString json = reply->readAll();
		updateOrNot(json);
	}
	else
	{
		QUrl url = reply->url();
		//if (url.isValid())
		string ssss = url.toString().toStdString();
		//if (std::regex_match(ssss, std::regex("(http|https)://([\\w\\./]*)")))
		if (std::regex_match(ssss, std::regex("https?:(.*)")))
		{
			QString filename = saveFileName(url);
			if (saveToDisk(filename, reply))
			{
				printf("Download of %s succeeded (saved to %s)\n", url.toEncoded().constData(), qPrintable(filename));
				QDesktopServices::openUrl(QUrl("file:///" + filename, QUrl::TolerantMode));
			}
		}
		else
		{
			QMessageBox::information(this, "check update", "please check intetnet");
		}

	}
	//MainWindow::qb->reset();
	//currentDownloads.removeAll(reply);
	reply->deleteLater();

}
QString PaletteAbout::saveFileName(const QUrl &url)
{
	QString path = url.path();
	QString basename = QFileInfo(path).fileName();

	if (basename.isEmpty())
		basename = "download";

	if (QFile::exists(basename)) {
		// already exists, don't overwrite
		int i = 0;
		basename += '.';
		while (QFile::exists(basename + QString::number(i)))
			++i;

		basename += QString::number(i);
	}
	QString fileTempPath = PicaApplication::getRoamingDir() + basename;
	return fileTempPath;
}

QString PaletteAbout::URLProcess(QString inStr)
{
	QString str2, str3;
	QString httpStr = "http";
	str2 = inStr.section('&', 0, 0);
	str3 = str2.section("url=", 1, 1);
	if (str3.size() < 1)
	{
		str2.replace("%3A", ":");
		str2.replace("%2F", "/");
		return str2;
	}
	else
	{
		str3.replace("%3A", ":");
		str3.replace("%2F", "/");
		return str3;

	}


}
bool PaletteAbout::updateOrNot(QString json)
{
	QJsonDocument jsonDoc = QJsonDocument::fromJson(json.toUtf8());
	QJsonObject jsonObj = jsonDoc.object();
	QJsonObject jsonObj2 = jsonObj.value("com.xyzprinting.BCPware").toObject();

	QString newVersion = jsonObj2["latest_version"].toString();
	QString downloadURL = URLProcess(jsonObj2["download_url"].toString());
	//fprintf(dbgff,downloadURL.toStdString().data());
	//fflush(dbgff); 

	qDebug() << newVersion;

	QString currentVersion = PicaApplication::appVer();
	if (VersionCheck(currentVersion.toStdString()) < VersionCheck(newVersion.toStdString()))
	//if (VersionCheck("0.0.0.0") < VersionCheck(newVersion.toStdString()))
	{
		qDebug() << "needUpdate";
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(this, "Test", "New Version detected,\n Start to download", QMessageBox::Yes | QMessageBox::No);
		if (reply == QMessageBox::Yes)
		{
			qDebug() << "Yes was clicked";	
			//QDesktopServices::openUrl(QUrl(downloadURL));
			startSendURL(QUrl(downloadURL));
		}
		else
		{
			qDebug() << "Yes was *not* clicked";
			QString text = "<p><a href=\"" + downloadURL + "\">New version available</a></p>";
			ui->palette_versionLB->setText(text);
		}
	}
	else
	{
		/*QMessageBox message(QMessageBox::NoIcon, "Title", "Content with icon.");
		message.setIconPixmap(QPixmap("icon.png"));
		message.exec();*/
		//QMessageBox::information(this, "check update", "version is the newest!");
		ui->palette_versionLB->setText(tr("BCPware is up to date."));
		//ui->testBtn->setText(currentVersion);
		//ui->testBtn->setDisabled
	}
	return true;
}
bool PaletteAbout::saveToDisk(const QString &filename, QIODevice *data)
{
	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly)) {
		fprintf(stderr, "Could not open %s for writing: %s\n",
			qPrintable(filename),
			qPrintable(file.errorString()));
		return false;
	}

	file.write(data->readAll());
	file.close();


	return true;
}
void PaletteAbout::initLicenseDia()
{
	licenseDia = new QDialog(this);
	licenseDia->setFixedSize(this->width(),this->height());
	

}

void PaletteAbout::credit()
{
	credits creditsdialog(this);
	creditsdialog.exec();
}

PaletteAbout::~PaletteAbout()
{
	foreach(QNetworkReply *re, currentDownloads)
		re->abort();
	//dbgff->close();
}
