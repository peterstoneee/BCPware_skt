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
#ifndef _BOXPACKING_H_
#define _BOXPACKING_H_

#include <QObject>
#include <QColorDialog>
#include <common/interfaces.h>
#include "three_d_packing.h"

class GLArea;
class BoxPacking : public QObject,public MeshFilterInterface
{
    Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(MESH_FILTER_INTERFACE_IID)
	Q_INTERFACES(MeshFilterInterface)
public:
    BoxPacking();
	~BoxPacking(){}

signals:

public slots:

public:
	enum
	{
		FP_LOEST_HEIGHT,
		FP_AUTO_PACKING,
		FP_GROUND,
		FP_CURRENT_SELECT_MESH_LANDING,
		FP_AUTO_LANDING,
		FP_AUTO_ALL_LANDING,
		FP_AUTO_CENTER,
		FP_COUNT_VOLUMN,
		FP_TEST_AREA,
		FP_TEST_ALIGN_OTHER_AXIS,
		FP_TEST_COPY_PACKING,
		FP_CHANGE_COLOR,
		FP_MIRROR,
		FP_MIRROR_X,
		FP_MIRROR_Y,
		FP_MIRROR_Z,
		FP_JUSTIFY_FRONT,
		FP_JUSTIFY_BACK,
		FP_JUSTIFY_LEFT,
		FP_JUSTIFY_RIGHT,
		FP_JUSTIFY_BOTTOM,
		FP_JUSTIFY_TOP,
		FP_Test_Quaternion,
		FP_COUNT_HOLES,
		FP_SEPERATE_TEST
	};
	void initParameterSet(QAction *action, MeshDocument &md, RichParameterSet & parlst);
	QString filterName(FilterIDType filterID) const;
	QString filterInfo(FilterIDType filter) const;
	bool applyFilter(QAction *   filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb,GLArea *a=0,vcg::SendBackData *sb= 0);
	FilterClass getClass(QAction *a);
};

#endif // BOXPACKING_H
