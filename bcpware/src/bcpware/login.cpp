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
#include "login.h"
#include "QDesktopServices"
#include "QUrl"
#include "QMessageBox"
#include <qnetworkrequest.h>
#include <qnetworkreply.h>
#include <qnetworkaccessmanager.h>
#include <QCryptographicHash>
#include <QSettings>
#include "mainwindow.h"

login::login(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	init();

	setWindowTitle("Login");
	//ui.bcp_icon->setPixmap(QPixmap(":/images/icons/bcp_icon.png"));
	
}

void login::init()
{
	connect(ui.loginPB, SIGNAL(clicked()), this, SLOT(log()));
	connect(ui.registerPB, SIGNAL(clicked()), this, SLOT(reg()));
	connect(ui.forgotPasswordPB, SIGNAL(clicked()), this, SLOT(forgot()));
	//connect(ui.forgotPasswordPB, SIGNAL(clicked()), this, SLOT(reg()));
	connect(ui.eyeCB, SIGNAL(clicked()), this, SLOT(echo()));
	ui.eyeCB->setStyleSheet(WidgetStyleSheet::check_onoff_styleSheet2());

}

login::~login()
{
	
}

void login::echo()
{
	if (ui.eyeCB->isChecked())
	{
		ui.passwordLE->setEchoMode(QLineEdit::Normal);
	}
	else
	{
		ui.passwordLE->setEchoMode(QLineEdit::Password);
	}
}

void login::log()
{
	QString username = ui.emailAdressLE->text();
	QString password = ui.passwordLE->text();

	if (username != NULL && password != NULL)
	{
		QString sURL = "http://service.xyzprinting.com/XYZPrinting/Members/REQUESTLOGIN/JSON";

		// create custom temporary event loop on stack
		QEventLoop eventLoop;

		// "quit()" the event-loop, when the network request "finished()"
		QNetworkAccessManager mgr;
		QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

		// the HTTP request
		QNetworkRequest geturl;
		geturl.setUrl(QUrl(sURL));
		geturl.setRawHeader("EMAIL", username.toAscii());
		geturl.setRawHeader("key", "033bd94b1168d7e4f0d644c3c95e35bf");
		QNetworkReply *reply = mgr.get(geturl);
		eventLoop.exec();
		QString rep = reply->readAll();
		QString rep0 = rep.mid(27, 36);
		QString pass = password + rep0;
		QString loginpass = QString("%1").arg(QString(QCryptographicHash::hash(pass.toUtf8(), QCryptographicHash::Sha1).toHex()));

		sURL = "http://service.xyzprinting.com/XYZPrinting/Members/LOGINCHECKWITHKEY/JSON";

		QNetworkRequest geturl1;
		geturl1.setUrl(QUrl(sURL));
		geturl1.setRawHeader("EMAIL", username.toAscii());
		geturl1.setRawHeader("key", "033bd94b1168d7e4f0d644c3c95e35bf");
		geturl1.setRawHeader("PASS", loginpass.toAscii());
		QNetworkReply *reply1 = mgr.get(geturl1);
		eventLoop.exec();

		QString rep1 = reply1->readAll();
		QString retCode = rep1.mid(11, 1);
		QString xyzToken_valid = rep1.mid(27, 46);
		QString xyzToken_invalid = rep1.mid(27, 5);

		if (retCode == "0")
		{
			//SYDNY getting member profile
			if (xyzToken_valid != NULL)
			{
				QString sURL = "http://service.xyzprinting.com/XYZPrinting/Members/WITHTOKEN/JSON";

				// create custom temporary event loop on stack
				QEventLoop eventLoop;

				// "quit()" the event-loop, when the network request "finished()"
				QNetworkAccessManager mgr;
				QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

				// the HTTP request
				QNetworkRequest geturl;
				geturl.setUrl(QUrl(sURL));
				geturl.setRawHeader("token", xyzToken_valid.toAscii());
				QNetworkReply *reply = mgr.get(geturl);
				eventLoop.exec();
				QString rep = reply->readAll();

				QStringList rep1 = rep.split("phone");
				QString rep2 = rep1.first();
				QStringList rep3 = rep2.split(':');
				QString rep4 = rep3.last();
				QStringList rep5 = rep4.split(',', QString::SkipEmptyParts);
				QString rep6 = rep5.takeFirst();
				QString nickname = rep6.remove('"');
				//qDebug() << ">>>>>>" << nickname;

				QSettings xyzReg("HKEY_CURRENT_USER\\SOFTWARE\\XYZ\\xyzsettings", QSettings::NativeFormat);
				xyzReg.setValue("token", xyzToken_valid);
				xyzReg.setValue("account", username);
				xyzReg.setValue("nickname", nickname);

				if (ui.rememberMeCB->isChecked())
				{
					QSettings xyzRegRemember("HKEY_CURRENT_USER\\SOFTWARE\\XYZ\\rememberuser", QSettings::NativeFormat);
					xyzRegRemember.setValue("account", username);
					xyzRegRemember.setValue("nickname", nickname);
				}
			}
			this->close();
		}


		else if (retCode == "1" && xyzToken_invalid == "passw")
		{
			QMessageBox msgBox;
			msgBox.setText("Incorrect Password.");
			msgBox.exec();
		}
		else if (retCode == "1" && xyzToken_invalid == "email")
		{
			QMessageBox msgBox;
			msgBox.setWindowTitle(PicaApplication::appName());
			msgBox.setText("Invalid Email Address.");
			msgBox.exec();
		}
		return;
	}


	else if (username != NULL && password == NULL)
	{
		QMessageBox msgBox;
		msgBox.setText("Please fill in Password box.");
		msgBox.exec();
	}
	else if (username == NULL && password != NULL)
	{
		QMessageBox msgBox;
		msgBox.setText("Please fill in Username box.");
		msgBox.exec();
	}
	else
	{
		QMessageBox msgBox;
		msgBox.setText("Please fill in Username and Password box.");
		msgBox.exec();
	}
}


void login::reg()
{
	//QString link = "http://www.xyzprinting.com/us_en/signin?returnUrl=http://us.store.xyzprinting.com/";
	QString link = "https://auth.xyzfamily.com/us_en/?returnUrl=http://support.xyzprinting.com/us_en/Help#signup";
	QDesktopServices::openUrl(QUrl(link));
	
}

void login::forgot()
{
	QString link = "https://auth.xyzfamily.com/us_en/?returnUrl=http://support.xyzprinting.com/us_en/Help";
	QDesktopServices::openUrl(QUrl(link));
}
void login::closeEvent(QCloseEvent *event)
{

}
