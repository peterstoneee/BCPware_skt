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
#pragma once
#include <QWidget>
#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSslError>
#include <QDesktopServices>
#include <common/interfaces.h>
#include <common/picaApplication.h>

#include "ui_palette.h"
#include "versionCheck.h"

namespace{
	class meshCheckDlg;
}
class MainWindow;

class PaletteAbout:public QDialog
{
	Q_OBJECT
public:
	PaletteAbout(QWidget *parent = 0);
	~PaletteAbout();
	MainWindow *mw;
private slots:
	void checkForUpdates_2();
	void connectionDone2(QNetworkReply *reply);
	void downloadStatus(qint64 bytesReceived, qint64 bytesTotal);

	//SYDNY
	void credit();
	

private:
	FILE *dbgff;
	
	void initLicenseDia();
	bool updateOrNot(QString json);
	void startSendURL(QUrl url);
	bool saveToDisk(const QString &filename, QIODevice *data);
	QString URLProcess(QString inStr);
	QString saveFileName(const QUrl &url);
	
	QDialog *licenseDia;

	QNetworkAccessManager *httpReq;
	QList<QNetworkReply *> currentDownloads;
	Ui::PaletteAbout *ui;
};

