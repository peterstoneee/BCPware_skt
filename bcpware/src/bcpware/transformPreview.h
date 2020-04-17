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
#include <GL/glew.h>
#include <QtWidgets>

#include "../common/pluginmanager.h"
#include "../common/scriptinterface.h"
#include "glarea.h"
#include "layerDialog.h"
#include "stdpardialog.h"
#include "xmlstdpardialog.h"
#include "xmlgeneratorgui.h"
#include "customizewidget.h"
#include "skt_function.h"
#include "SaveDialogTest.h"
#include "maskSave.h"
#include "transformPreview.h"


#include <QtScript>
#include <QDir>
#include <QMainWindow>
#include <QMdiArea>
#include <QStringList>
#include <QColorDialog>
#include <QMdiSubWindow>
#include <QGroupBox>
#include <QtPrintSupport>
#include <qstylefactory.h>

class TransformPreview : public QDialog
{
	Q_OBJECT
public:
	TransformPreview(QWidget *parent = 0);
	void setWindowFlags(Qt::WindowFlags flags);
	void createContent();

private:
	QTextEdit *tpTE;
	QPushButton *tpPB;

	//~TransformPreview();
};

