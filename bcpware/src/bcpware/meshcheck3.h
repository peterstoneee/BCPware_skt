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
#ifndef MESHCHECK3_H
#define MESHCHECK3_H

#include <QDialog>
#include <QWidget>
#include <QPushButton>
#include <QButtonGroup>
#include <QLineEdit>

#include "GLArea.h"
#include "ui_meshcheck3.h"

class meshCheck3 : public QDialog
{
	Q_OBJECT

public:
	meshCheck3(MeshModel *m, Construction_Groove<Scalarm> cg, GLArea *gla, QWidget *parent = 0);
	~meshCheck3() {}

	void init();
	Point3m scaleN;

public slots:
	void getAccept();

private:
	Ui::meshCheck3 ui;

	MeshModel *checkModel;
	MeshDocument *mdd;
	QButtonGroup *dimensionGroup;
	Matrix44m transMatrix;
	Construction_Groove<Scalarm> cg;
	GLArea *gla;
};



#endif // MESHCHECK3_H
