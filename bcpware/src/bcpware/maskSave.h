/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/


/****************************************************************************
* My Extension to Meshlab
*
*  Copyright 2018 by
*
* This file is part of Meshlab.
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
#include <wrap/io_trimesh/io_mask.h>

#include "stdpardialog.h"
#include "../common/filterparameter.h"
#include "glarea.h"
#include <QDialog>


class maskSave :public QDialog
{
public:
	maskSave();
	maskSave(QWidget *parent, MeshModel *m, int capability, int defaultBits, RichParameterSet *par, GLArea* glar = NULL);
	void initForm();
	~maskSave();
private:
	QCheckBox *cb1;
	QCheckBox *cb2;

	MeshModel *m;
	int mask;
	int type;
	int capability;
	int defaultBits;
	RichParameterSet *parSet;
	StdParFrame *stdParFrame;
	GLArea* glar;

signals:



	public slots :
		void okbutton();
		void canclebutton();
		void setColor();
		void setTexture();

};

