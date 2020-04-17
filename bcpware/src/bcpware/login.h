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
#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include "ui_login.h"

class login : public QDialog
{
	Q_OBJECT

public:
	login(QWidget *parent = 0);
	~login();

	void init();
	void closeEvent(QCloseEvent *event);


public slots:
	void log();
	void reg();
	void echo();
	void forgot();

private:
	Ui::login ui;
};

#endif // LOGIN_H
